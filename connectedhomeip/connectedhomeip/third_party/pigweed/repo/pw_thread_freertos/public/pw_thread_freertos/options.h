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
#pragma once

#include "FreeRTOS.h"
#include "pw_assert/assert.h"
#include "pw_thread/thread.h"
#include "pw_thread_freertos/config.h"
#include "pw_thread_freertos/context.h"
#include "task.h"

namespace pw::thread::freertos {

// pw::thread::Options for FreeRTOS.
//
// Example usage:
//
//   // Uses the default stack size and priority, but specifies a custom name.
//   pw::thread::Thread example_thread(
//     pw::thread::freertos::Options()
//         .set_name("example_thread"),
//     example_thread_function);
//
//   // Provides the name, priority, and pre-allocated context.
//   pw::thread::Thread static_example_thread(
//     pw::thread::freertos::Options()
//         .set_name("static_example_thread")
//         .set_priority(kFooPriority)
//         .set_static_context(static_example_thread_context),
//     example_thread_function);
//
class Options : public thread::Options {
 public:
  constexpr Options() {}
  constexpr Options(const Options&) = default;
  constexpr Options(Options&&) = default;

  // Sets the name for the FreeRTOS task, note that this will be truncated
  // based on configMAX_TASK_NAME_LEN.
  // This is deep copied by FreeRTOS into the task's task control block (TCB).
  constexpr Options& set_name(const char* name) {
    name_ = name;
    return *this;
  }

  // Sets the priority for the FreeRTOS task. This must be a value between
  // 0 to PW_THREAD_FREERTOS_CONFIG_MAXIMUM_PRIORITY. Higher priority
  // values have a higher priority.
  //
  // Note that the idle task priority, tskIDLE_PRIORITY, is fixed to 0.
  // See the FreeRTOS documentation on the idle task for more details.
  //
  // Precondition: This must be <= PW_THREAD_FREERTOS_CONFIG_MAXIMUM_PRIORITY.
  constexpr Options& set_priority(UBaseType_t priority) {
    PW_DASSERT(priority <= config::kMaximumPriority);
    priority_ = priority;
    return *this;
  }

#if PW_THREAD_FREERTOS_CONFIG_DYNAMIC_ALLOCATION_ENABLED
  // Set the stack size of dynamic thread allocations.
  //
  // Precondition: size_words must be >= configMINIMAL_STACK_SIZE
  constexpr Options& set_stack_size(size_t size_words) {
    PW_DASSERT(size_words >= config::kMinimumStackSizeWords);
    stack_size_words_ = size_words;
    return *this;
  }
#endif  // PW_THREAD_FREERTOS_CONFIG_DYNAMIC_ALLOCATION_ENABLED

  // Set the pre-allocated context (all memory needed to run a thread), see the
  // pw::thread::freertos::StaticContext for more detail.
  constexpr Options& set_static_context(StaticContext& context) {
    context_ = &context;
    return *this;
  }

  // Returns name of FreeRTOS task.
  //
  // Note that this thread name may not match the actual thread name. See the
  // FreeRTOS documentation on how names must be <= configMAX_TASK_NAME_LEN in
  // order to avoid truncation.
  const char* name() const { return name_; }

 private:
  friend thread::Thread;
  // FreeRTOS requires a valid name when asserts are enabled,
  // configMAX_TASK_NAME_LEN may be as small as one character.
  static constexpr char kDefaultName[] = "pw::Thread";

  UBaseType_t priority() const { return priority_; }
#if PW_THREAD_FREERTOS_CONFIG_DYNAMIC_ALLOCATION_ENABLED
  size_t stack_size_words() const { return stack_size_words_; }
#endif  // PW_THREAD_FREERTOS_CONFIG_DYNAMIC_ALLOCATION_ENABLED
  StaticContext* static_context() const { return context_; }

  const char* name_ = kDefaultName;
  UBaseType_t priority_ = config::kDefaultPriority;
#if PW_THREAD_FREERTOS_CONFIG_DYNAMIC_ALLOCATION_ENABLED
  size_t stack_size_words_ = config::kDefaultStackSizeWords;
#endif  // PW_THREAD_FREERTOS_CONFIG_DYNAMIC_ALLOCATION_ENABLED
  StaticContext* context_ = nullptr;
};

}  // namespace pw::thread::freertos
