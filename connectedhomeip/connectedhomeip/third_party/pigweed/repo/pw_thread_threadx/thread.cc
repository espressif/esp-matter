// Copyright 2021 The Pigweed Authors
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy of
// the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations under
// the License.
#include "pw_thread/thread.h"

#include "pw_assert/check.h"
#include "pw_preprocessor/compiler.h"
#include "pw_thread/id.h"
#include "pw_thread_threadx/config.h"
#include "pw_thread_threadx/context.h"
#include "pw_thread_threadx/options.h"
#include "tx_event_flags.h"

using pw::thread::threadx::Context;

namespace pw::thread {
namespace {
#if PW_THREAD_JOINING_ENABLED
constexpr ULONG kThreadDoneBit = 1;
#endif  // PW_THREAD_JOINING_ENABLED
}  // namespace

void Context::ThreadEntryPoint(ULONG void_context_ptr) {
  Context& context = *reinterpret_cast<Context*>(void_context_ptr);

  // Invoke the user's thread function. This may never return.
  context.user_thread_entry_function_(context.user_thread_entry_arg_);

  // Raise our preemption threshold as a thread only critical section to guard
  // against join() and detach().
  UINT original_preemption_threshold = TX_MAX_PRIORITIES;  // Invalid.
  UINT preemption_success = tx_thread_preemption_change(
      &context.tcb(), 0, &original_preemption_threshold);
  PW_DCHECK_UINT_EQ(TX_SUCCESS,
                    preemption_success,
                    "Failed to enter thread critical section");
  if (context.detached()) {
    // There is no threadsafe way to re-use detached threads, as there's no way
    // to invoke tx_thread_delete() from the running thread! Joining MUST be
    // used for this. However to enable unit test coverage we go ahead and clear
    // this.
    context.set_in_use(false);

#if PW_THREAD_JOINING_ENABLED
    // If the thread handle was detached before the thread finished execution,
    // i.e. got here, then we are responsible for cleaning up the join event
    // group.
    const UINT event_group_result =
        tx_event_flags_delete(&context.join_event_group());
    PW_DCHECK_UINT_EQ(TX_SUCCESS,
                      event_group_result,
                      "Failed to delete the join event group");
#endif  // PW_THREAD_JOINING_ENABLED

    // Note that we do not have to restore our preemption threshold as this
    // thread is completing execution.

    // WARNING: The thread at this point continues to be registered with the
    // kernel in TX_COMPLETED state, as tx_thread_delete cannot be invoked!
    return;
  }

  // Otherwise the task finished before the thread was detached or joined, defer
  // cleanup to Thread's join() or detach().
  context.set_thread_done();
  UINT unused = 0;
  preemption_success = tx_thread_preemption_change(
      &context.tcb(), original_preemption_threshold, &unused);
  PW_DCHECK_UINT_EQ(TX_SUCCESS,
                    preemption_success,
                    "Failed to leave thread critical section");

#if PW_THREAD_JOINING_ENABLED
  const UINT result =
      tx_event_flags_set(&context.join_event_group(), kThreadDoneBit, TX_OR);
  PW_DCHECK_UINT_EQ(TX_SUCCESS, result, "Failed to set the join event");
#endif  // PW_THREAD_JOINING_ENABLED
  return;
}

void Context::DeleteThread(Context& context) {
  // Stop the other task first.
  UINT thread_result = tx_thread_terminate(&context.tcb());
  PW_CHECK_UINT_EQ(TX_SUCCESS, thread_result, "Failed to terminate the thread");

  // Delete the thread, removing it out of the kernel.
  thread_result = tx_thread_delete(&context.tcb());
  PW_CHECK_UINT_EQ(TX_SUCCESS, thread_result, "Failed to delete the thread");

  // Mark the context as unused for potential later re-use.
  context.set_in_use(false);

#if PW_THREAD_JOINING_ENABLED
  // Just in case someone abused our API, ensure their use of the event group is
  // properly handled by the kernel regardless.
  const UINT event_group_result =
      tx_event_flags_delete(&context.join_event_group());
  PW_DCHECK_UINT_EQ(
      TX_SUCCESS, event_group_result, "Failed to delete the join event group");
#endif  // PW_THREAD_JOINING_ENABLED
}

Thread::Thread(const thread::Options& facade_options,
               ThreadRoutine entry,
               void* arg)
    : native_type_(nullptr) {
  // Cast the generic facade options to the backend specific option of which
  // only one type can exist at compile time.
  auto options = static_cast<const threadx::Options&>(facade_options);
  PW_DCHECK_NOTNULL(options.context(), "The Context is not optional");
  native_type_ = options.context();

  // Can't use a context more than once.
  PW_DCHECK(!native_type_->in_use());

  // Reset the state of the static context in case it was re-used.
  native_type_->set_in_use(true);
  native_type_->set_detached(false);
  native_type_->set_thread_done(false);
#if PW_THREAD_JOINING_ENABLED
  static const char* join_event_group_name = "pw::Thread";
  const UINT event_group_result =
      tx_event_flags_create(&options.context()->join_event_group(),
                            const_cast<char*>(join_event_group_name));
  PW_DCHECK_UINT_EQ(
      TX_SUCCESS, event_group_result, "Failed to create the join event group");
#endif  // PW_THREAD_JOINING_ENABLED

  // Copy over the thread name.
  native_type_->set_name(options.name());

  // In order to support functions which return and joining, a delegate is
  // deep copied into the context with a small wrapping function to actually
  // invoke the task with its arg.
  native_type_->set_thread_routine(entry, arg);

  const UINT thread_result =
      tx_thread_create(&options.context()->tcb(),
                       const_cast<char*>(native_type_->name()),
                       Context::ThreadEntryPoint,
                       reinterpret_cast<ULONG>(native_type_),
                       options.context()->stack().data(),
                       options.context()->stack().size_bytes(),
                       options.priority(),
                       options.preemption_threshold(),
                       options.time_slice_interval(),
                       TX_AUTO_START);
  PW_CHECK_UINT_EQ(TX_SUCCESS, thread_result, "Failed to create the thread");
}

void Thread::detach() {
  PW_CHECK(joinable());

  tx_thread_suspend(&native_type_->tcb());
  native_type_->set_detached();
  const bool thread_done = native_type_->thread_done();
  tx_thread_resume(&native_type_->tcb());

  if (thread_done) {
    // The task finished (hit end of Context::ThreadEntryPoint) before we
    // invoked detach, clean up the thread.
    Context::DeleteThread(*native_type_);
  } else {
    // We're detaching before the task finished, defer cleanup to the task at
    // the end of Context::ThreadEntryPoint.
  }

  // Update to no longer represent a thread of execution.
  native_type_ = nullptr;
}

#if PW_THREAD_JOINING_ENABLED
void Thread::join() {
  PW_CHECK(joinable());
  PW_CHECK(this_thread::get_id() != get_id());

  ULONG actual_flags = 0;
  const UINT result = tx_event_flags_get(&native_type_->join_event_group(),
                                         kThreadDoneBit,
                                         TX_OR_CLEAR,
                                         &actual_flags,
                                         TX_WAIT_FOREVER);
  PW_DCHECK_UINT_EQ(TX_SUCCESS, result, "Failed to get the join event");

  // No need for a critical section here as the thread at this point is
  // waiting to be deleted.
  Context::DeleteThread(*native_type_);

  // Update to no longer represent a thread of execution.
  native_type_ = nullptr;
}
#endif  // PW_THREAD_JOINING_ENABLED

}  // namespace pw::thread
