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

#include <optional>

#include "pw_assert/assert.h"
#include "pw_thread/thread.h"
#include "pw_thread_threadx/config.h"
#include "pw_thread_threadx/context.h"
#include "tx_api.h"

namespace pw::thread::threadx {

// pw::thread::Options for ThreadX.
//
// Example usage:
//
//   // Uses the default priority and time slice interval (which may be
//   // disabled), but specifies a custom name and pre-allocated context.
//   // Note that the preemption threshold is disabled by default.
//   pw::thread::Thread example_thread(
//     pw::thread::threadx::Options()
//         .set_name("example_thread"),
//         .set_context(example_thread_context),
//     example_thread_function);
//
//   // Specifies the name, priority, time slice interval, and pre-allocated
//   // context, but does not use a preemption threshold.
//   pw::thread::Thread static_example_thread(
//     pw::thread::threadx::Options()
//         .set_name("static_example_thread")
//         .set_priority(kFooPriority)
//         .set_time_slice_interval(1)
//         .set_context(example_thread_context),
//     example_thread_function);
//
class Options : public thread::Options {
 public:
  constexpr Options() {}
  constexpr Options(const Options&) = default;
  constexpr Options(Options&&) = default;

  // Sets the name for the ThreadX thread, note that this will be deep copied
  // into the context and may be truncated based on
  // PW_THREAD_THREADX_CONFIG_MAX_THREAD_NAME_LEN.
  constexpr Options& set_name(const char* name) {
    name_ = name;
    return *this;
  }

  // Sets the priority for the ThreadX thread from 0 through 31, where a value
  // of 0 represents the highest priority, see ThreadX tx_thread_create for
  // more detail.
  //
  // Precondition: priority <= PW_THREAD_THREADX_CONFIG_MIN_PRIORITY
  constexpr Options& set_priority(UINT priority) {
    PW_DASSERT(priority <= PW_THREAD_THREADX_CONFIG_MIN_PRIORITY);
    priority_ = priority;
    return *this;
  }

  // Optionally sets the preemption threshold for the ThreadX thread from 0
  // through 31.
  //
  // Only priorities higher than this level (i.e. lower number) are allowed to
  // preempt this thread. In other words this allows the thread to specify the
  // priority ceiling for disabling preemption. Threads that have a higher
  // priority than the ceiling are still allowed to preempt while those with
  // less than the ceiling are not allowed to preempt.
  //
  // Not setting the preemption threshold or explicitly specifying a value
  // equal to the priority disables preemption threshold.
  //
  // Time slicing is disabled while the preemption threshold is enabled, i.e.
  // not equal to the priority, even if a time slice interval was specified.
  //
  // The preemption threshold can be adjusted at run time, this only sets the
  // initial threshold.
  //
  // Precondition: preemption_threshold <= priority
  constexpr Options& set_preemption_threshold(UINT preemption_threshold) {
    PW_DASSERT(preemption_threshold < PW_THREAD_THREADX_CONFIG_MIN_PRIORITY);
    possible_preemption_threshold_ = preemption_threshold;
    return *this;
  }

  // Sets the number of ticks this thread is allowed to run before other ready
  // threads of the same priority are given a chance to run.
  //
  // Time slicing is disabled while the preemption threshold is enabled, i.e.
  // not equal to the priority, even if a time slice interval was specified.
  //
  // A value of TX_NO_TIME_SLICE (a value of 0) disables time-slicing of this
  // thread.
  //
  // Using time slicing results in a slight amount of system overhead, threads
  // with a unique priority should consider TX_NO_TIME_SLICE.
  constexpr Options& set_time_slice_interval(ULONG time_slice_interval) {
    time_slice_interval_ = time_slice_interval;
    return *this;
  }

  // Set the pre-allocated context (all memory needed to run a thread). Note
  // that this is required for this thread creation backend! The Context can
  // either be constructed with an externally provided span<ULONG> stack
  // or the templated form of ContextWihtStack<kStackSizeWords> can be used.
  constexpr Options& set_context(Context& context) {
    context_ = &context;
    return *this;
  }

 private:
  friend thread::Thread;
  // Note that the default name may end up truncated due to
  // PW_THREAD_THREADX_CONFIG_MAX_THREAD_NAME_LEN.
  static constexpr char kDefaultName[] = "pw::Thread";

  const char* name() const { return name_; }
  UINT priority() const { return priority_; }
  UINT preemption_threshold() const {
    return possible_preemption_threshold_.value_or(priority_);
  }
  ULONG time_slice_interval() const { return time_slice_interval_; }
  Context* context() const { return context_; }

  const char* name_ = kDefaultName;
  UINT priority_ = config::kDefaultPriority;
  // A default value cannot be used for the preemption threshold as it would
  // have to be based on the selected priority.
  std::optional<UINT> possible_preemption_threshold_ = std::nullopt;
  ULONG time_slice_interval_ = config::kDefaultTimeSliceInterval;
  Context* context_ = nullptr;
};

}  // namespace pw::thread::threadx
