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
#pragma once

#include "RTOS.h"
#include "pw_assert/assert.h"
#include "pw_thread/thread.h"
#include "pw_thread_embos/config.h"
#include "pw_thread_embos/context.h"

namespace pw::thread::embos {

// pw::thread::Options for FreeRTOS.
//
// Example usage:
//
//   // Uses the default priority, but specifies a custom name and context.
//   pw::thread::Thread example_thread(
//     pw::thread::embos::Options()
//         .set_name("example_thread"),
//         .set_context(static_example_thread_context),
//     example_thread_function);
//
//   // Provides the name, priority, and pre-allocated context.
//   pw::thread::Thread static_example_thread(
//     pw::thread::embos::Options()
//         .set_name("static_example_thread")
//         .set_priority(kFooPriority)
//         .set_context(static_example_thread_context),
//     example_thread_function);
//
class Options : public thread::Options {
 public:
  constexpr Options() {}
  constexpr Options(const Options&) = default;
  constexpr Options(Options&&) = default;

  // Sets the name for the embOS task, this is optional.
  // Note that this will be deep copied into the context and may be truncated
  // based on PW_THREAD_EMBOS_CONFIG_MAX_THREAD_NAME_LEN.
  constexpr Options& set_name(const char* name) {
    name_ = name;
    return *this;
  }

  // Sets the priority for the embOS task. Higher values are higher priority,
  // see embOS OS_CreateTaskEx for more detail.
  //
  // Precondition: This must be >= PW_THREAD_EMBOS_CONFIG_MIN_PRIORITY.
  constexpr Options& set_priority(OS_PRIO priority) {
    PW_DASSERT(priority >= config::kMinimumPriority);
    priority_ = priority;
    return *this;
  }

  // Sets the number of ticks this thread is allowed to run before other ready
  // threads of the same priority are given a chance to run.
  //
  // A value of 0 disables time-slicing of this thread.
  //
  // Precondition: This must be <= 255 ticks.
  constexpr Options& set_time_slice_interval(OS_UINT time_slice_interval) {
    PW_DASSERT(time_slice_interval <= 255);
    time_slice_interval_ = time_slice_interval;
    return *this;
  }

  // Set the pre-allocated context (all memory needed to run a thread), see the
  // pw::thread::embos::Context for more detail.
  constexpr Options& set_context(Context& context) {
    context_ = &context;
    return *this;
  }

 private:
  friend thread::Thread;

  const char* name() const { return name_; }
  OS_PRIO priority() const { return priority_; }
  OS_PRIO time_slice_interval() const { return time_slice_interval_; }
  Context* context() const { return context_; }

  const char* name_ = nullptr;
  OS_PRIO priority_ = config::kDefaultPriority;
  OS_PRIO time_slice_interval_ = config::kDefaultTimeSliceInterval;
  Context* context_ = nullptr;
};

}  // namespace pw::thread::embos
