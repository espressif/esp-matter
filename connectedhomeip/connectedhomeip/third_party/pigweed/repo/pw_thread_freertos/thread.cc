// Copyright 2020 The Pigweed Authors
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

#include "FreeRTOS.h"
#include "pw_assert/check.h"
#include "pw_preprocessor/compiler.h"
#include "pw_thread/id.h"
#include "pw_thread_freertos/config.h"
#include "pw_thread_freertos/context.h"
#include "pw_thread_freertos/options.h"
#include "task.h"

using pw::thread::freertos::Context;

namespace pw::thread {
namespace {
#if PW_THREAD_JOINING_ENABLED
constexpr EventBits_t kThreadDoneBit = 1 << 0;
#endif  // PW_THREAD_JOINING_ENABLED
}  // namespace

void Context::ThreadEntryPoint(void* void_context_ptr) {
  Context& context = *static_cast<Context*>(void_context_ptr);

  // Invoke the user's thread function. This may never return.
  context.user_thread_entry_function_(context.user_thread_entry_arg_);

  // Use a task only critical section to guard against join() and detach().
  vTaskSuspendAll();
  if (context.detached()) {
    // There is no threadsafe way to re-use detached threads, as there's no way
    // to signal the vTaskDelete success. Joining MUST be used for this.
    // However to enable unit test coverage we go ahead and clear this.
    context.set_task_handle(nullptr);

#if PW_THREAD_JOINING_ENABLED
    // If the thread handle was detached before the thread finished execution,
    // i.e. got here, then we are responsible for cleaning up the join event
    // group.
    vEventGroupDelete(
        reinterpret_cast<EventGroupHandle_t>(&context.join_event_group()));
#endif  // PW_THREAD_JOINING_ENABLED

#if PW_THREAD_FREERTOS_CONFIG_DYNAMIC_ALLOCATION_ENABLED
    // The thread was detached before the task finished, free any allocations
    // it ran on.
    if (context.dynamically_allocated()) {
      delete &context;
    }
#endif  // PW_THREAD_FREERTOS_CONFIG_DYNAMIC_ALLOCATION_ENABLED

    // Re-enable the scheduler before we delete this execution.
    xTaskResumeAll();
    vTaskDelete(nullptr);
    PW_UNREACHABLE;
  }

  // Otherwise the task finished before the thread was detached or joined, defer
  // cleanup to Thread's join() or detach().
  context.set_thread_done();
  xTaskResumeAll();

#if PW_THREAD_JOINING_ENABLED
  xEventGroupSetBits(
      reinterpret_cast<EventGroupHandle_t>(&context.join_event_group()),
      kThreadDoneBit);
#endif  // PW_THREAD_JOINING_ENABLED

  while (true) {
#if INCLUDE_vTaskSuspend == 1
    // Use indefinite suspension when available.
    vTaskSuspend(nullptr);
#else
    vTaskDelay(portMAX_DELAY);
#endif  // INCLUDE_vTaskSuspend == 1
  }
  PW_UNREACHABLE;
}

void Context::TerminateThread(Context& context) {
  // Stop the other task first.
  PW_DCHECK_NOTNULL(context.task_handle(), "We shall not delete ourselves!");
  vTaskDelete(context.task_handle());

  // Mark the context as unused for potential later re-use.
  context.set_task_handle(nullptr);

#if PW_THREAD_JOINING_ENABLED
  // Just in case someone abused our API, ensure their use of the event group is
  // properly handled by the kernel regardless.
  vEventGroupDelete(
      reinterpret_cast<EventGroupHandle_t>(&context.join_event_group()));
#endif  // PW_THREAD_JOINING_ENABLED

#if PW_THREAD_FREERTOS_CONFIG_DYNAMIC_ALLOCATION_ENABLED
  // Then free any allocations it ran on.
  if (context.dynamically_allocated()) {
    delete &context;
  }
#endif  // PW_THREAD_FREERTOS_CONFIG_DYNAMIC_ALLOCATION_ENABLED
}

Thread::Thread(const thread::Options& facade_options,
               ThreadRoutine entry,
               void* arg)
    : native_type_(nullptr) {
  // Cast the generic facade options to the backend specific option of which
  // only one type can exist at compile time.
  auto options = static_cast<const freertos::Options&>(facade_options);
  if (options.static_context() != nullptr) {
    // Use the statically allocated context.
    native_type_ = options.static_context();
    // Can't use a context more than once.
    PW_DCHECK_PTR_EQ(native_type_->task_handle(), nullptr);
    // Reset the state of the static context in case it was re-used.
    native_type_->set_detached(false);
    native_type_->set_thread_done(false);
#if PW_THREAD_JOINING_ENABLED
    const EventGroupHandle_t event_group_handle =
        xEventGroupCreateStatic(&native_type_->join_event_group());
    PW_DCHECK_PTR_EQ(event_group_handle,
                     &native_type_->join_event_group(),
                     "Failed to create the joining event group");
#endif  // PW_THREAD_JOINING_ENABLED

    // In order to support functions which return and joining, a delegate is
    // deep copied into the context with a small wrapping function to actually
    // invoke the task with its arg.
    native_type_->set_thread_routine(entry, arg);
    const TaskHandle_t task_handle =
        xTaskCreateStatic(Context::ThreadEntryPoint,
                          options.name(),
                          options.static_context()->stack().size(),
                          native_type_,
                          options.priority(),
                          options.static_context()->stack().data(),
                          &options.static_context()->tcb());
    PW_CHECK_NOTNULL(task_handle);  // Ensure it succeeded.
    native_type_->set_task_handle(task_handle);
  } else {
#if !PW_THREAD_FREERTOS_CONFIG_DYNAMIC_ALLOCATION_ENABLED
    PW_CRASH(
        "dynamic thread allocations are not enabled and no static_context "
        "was provided");
#else  // PW_THREAD_FREERTOS_CONFIG_DYNAMIC_ALLOCATION_ENABLED
    // Dynamically allocate the context and the task.
    native_type_ = new pw::thread::freertos::Context();
    native_type_->set_dynamically_allocated();
#if PW_THREAD_JOINING_ENABLED
    const EventGroupHandle_t event_group_handle =
        xEventGroupCreateStatic(&native_type_->join_event_group());
    PW_DCHECK_PTR_EQ(event_group_handle,
                     &native_type_->join_event_group(),
                     "Failed to create the joining event group");
#endif  // PW_THREAD_JOINING_ENABLED

    // In order to support functions which return and joining, a delegate is
    // deep copied into the context with a small wrapping function to actually
    // invoke the task with its arg.
    native_type_->set_thread_routine(entry, arg);
    TaskHandle_t task_handle;
    const BaseType_t result = xTaskCreate(Context::ThreadEntryPoint,
                                          options.name(),
                                          options.stack_size_words(),
                                          native_type_,
                                          options.priority(),
                                          &task_handle);

    // Ensure it succeeded.
    PW_CHECK_UINT_EQ(result, pdPASS);
    native_type_->set_task_handle(task_handle);
#endif  // !PW_THREAD_FREERTOS_CONFIG_DYNAMIC_ALLOCATION_ENABLED
  }
}

void Thread::detach() {
  PW_CHECK(joinable());

#if (INCLUDE_vTaskSuspend == 1) && (INCLUDE_xTaskGetSchedulerState == 1)
  // No need to suspend extra tasks.
  if (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING) {
    vTaskSuspend(native_type_->task_handle());
  }
#else
  // Safe to suspend all tasks while scheduler is not running.
  vTaskSuspendAll();
#endif  // INCLUDE_vTaskSuspend == 1
  native_type_->set_detached();
  const bool thread_done = native_type_->thread_done();
#if (INCLUDE_vTaskSuspend == 1) && (INCLUDE_xTaskGetSchedulerState == 1)
  // No need to suspend extra tasks, but only safe to call once scheduler is
  // running.
  if (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING) {
    vTaskResume(native_type_->task_handle());
  }
#else
  // Safe to resume all tasks while scheduler is not running.
  xTaskResumeAll();
#endif  // INCLUDE_vTaskSuspend == 1

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

  // Wait indefinitely until kThreadDoneBit is set.
  while (xEventGroupWaitBits(reinterpret_cast<EventGroupHandle_t>(
                                 &native_type_->join_event_group()),
                             kThreadDoneBit,
                             pdTRUE,   // Clear the bits.
                             pdFALSE,  // Any bits is fine, N/A.
                             portMAX_DELAY) != kThreadDoneBit) {
  }

  // No need for a critical section here as the thread at this point is
  // waiting to be terminated.
  Context::TerminateThread(*native_type_);

  // Update to no longer represent a thread of execution.
  native_type_ = nullptr;
}
#endif  // PW_THREAD_JOINING_ENABLED

}  // namespace pw::thread
