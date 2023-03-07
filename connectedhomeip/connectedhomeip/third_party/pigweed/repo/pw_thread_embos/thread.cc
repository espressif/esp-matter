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
#include "pw_thread/id.h"
#include "pw_thread_embos/config.h"
#include "pw_thread_embos/context.h"
#include "pw_thread_embos/options.h"

using pw::thread::embos::Context;

namespace pw::thread {

void Context::ThreadEntryPoint(void* void_context_ptr) {
  Context& context = *reinterpret_cast<Context*>(void_context_ptr);

  // Invoke the user's thread function. This may never return.
  context.user_thread_entry_function_(context.user_thread_entry_arg_);

  // Use a task only critical section to guard against join() and detach().
  OS_SuspendAllTasks();
  if (context.detached()) {
    // There is no threadsafe way to re-use detached threads. Callbacks
    // registered through OS_AddOnTerminateHook CANNOT be used for this as they
    // are invoked before the kernel is done using the task's TCB!
    // However to enable unit test coverage we go ahead and clear this.
    context.set_in_use(false);

#if PW_THREAD_JOINING_ENABLED
    // If the thread handle was detached before the thread finished execution,
    // i.e. got here, then we are responsible for cleaning up the join event
    // object.
    OS_EVENT_Delete(&context.join_event_object());
#endif  // PW_THREAD_JOINING_ENABLED

    // Re-enable the scheduler before we delete this execution. Note this name
    // is a bit misleading as reference counting is used.
    OS_ResumeAllSuspendedTasks();
    OS_TerminateTask(nullptr);
    PW_UNREACHABLE;
  }

  // Otherwise the task finished before the thread was detached or joined, defer
  // cleanup to Thread's join() or detach().
  context.set_thread_done();
  OS_ResumeAllSuspendedTasks();

#if PW_THREAD_JOINING_ENABLED
  OS_EVENT_Set(&context.join_event_object());
#endif  // PW_THREAD_JOINING_ENABLED

  // Let the thread handle owner terminate this task when they detach or join.
  OS_Suspend(nullptr);
  PW_UNREACHABLE;
}

void Context::TerminateThread(Context& context) {
  // Stop the other task first.
  OS_TerminateTask(&context.tcb());

  // Mark the context as unused for potential later re-use.
  context.set_in_use(false);

#if PW_THREAD_JOINING_ENABLED
  // Just in case someone abused our API, ensure their use of the event group is
  // properly handled by the kernel regardless.
  OS_EVENT_Delete(&context.join_event_object());
#endif  // PW_THREAD_JOINING_ENABLED
}

Thread::Thread(const thread::Options& facade_options,
               ThreadRoutine entry,
               void* arg)
    : native_type_(nullptr) {
  // Cast the generic facade options to the backend specific option of which
  // only one type can exist at compile time.
  auto options = static_cast<const embos::Options&>(facade_options);
  PW_DCHECK_NOTNULL(options.context(), "The Context is not optional");
  native_type_ = options.context();

  // Can't use a context more than once.
  PW_DCHECK(!native_type_->in_use());

  // Reset the state of the static context in case it was re-used.
  native_type_->set_in_use(true);
  native_type_->set_detached(false);
  native_type_->set_thread_done(false);
#if PW_THREAD_JOINING_ENABLED
  OS_EVENT_CreateEx(&options.context()->join_event_object(),
                    OS_EVENT_RESET_MODE_AUTO);
#endif  // PW_THREAD_JOINING_ENABLED

  // Copy over the thread name.
  native_type_->set_name(options.name());

  // In order to support functions which return and joining, a delegate is
  // deep copied into the context with a small wrapping function to actually
  // invoke the task with its arg.
  native_type_->set_thread_routine(entry, arg);

  OS_CreateTaskEx(&options.context()->tcb(),
                  native_type_->name(),
                  options.priority(),
                  Context::ThreadEntryPoint,
                  options.context()->stack().data(),
                  static_cast<OS_UINT>(options.context()->stack().size_bytes()),
                  options.time_slice_interval(),
                  options.context());
}

void Thread::detach() {
  PW_CHECK(joinable());

  OS_Suspend(&native_type_->tcb());
  native_type_->set_detached();
  const bool thread_done = native_type_->thread_done();
  OS_Resume(&native_type_->tcb());

  if (thread_done) {
    // The task finished (hit end of Context::ThreadEntryPoint) before we
    // invoked detach, clean up the thread.
    Context::TerminateThread(*native_type_);
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

  OS_EVENT_Wait(&native_type_->join_event_object());

  // No need for a critical section here as the thread at this point is
  // waiting to be deleted.
  Context::TerminateThread(*native_type_);

  // Update to no longer represent a thread of execution.
  native_type_ = nullptr;
}
#endif  // PW_THREAD_JOINING_ENABLED

}  // namespace pw::thread
