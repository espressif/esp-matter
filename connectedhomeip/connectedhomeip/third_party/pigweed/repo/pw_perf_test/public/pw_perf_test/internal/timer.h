// Copyright 2022 The Pigweed Authors
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

#include "pw_perf_test/internal/duration_unit.h"
#include "pw_perf_test_timer_backend/timer.h"

namespace pw::perf_test::internal {

using Timestamp = backend::Timestamp;  // implementation-defined type

// Returns true when the timer is ready to measure.
[[nodiscard]] inline bool TimerPrepare() { return backend::TimerPrepare(); }

// Performs any necessary cleanup for the timer.
inline void TimerCleanup() { return backend::TimerCleanup(); }

// Returns the current timestamp
inline Timestamp GetCurrentTimestamp() {
  return backend::GetCurrentTimestamp();
}

// Obtains the testing unit from the backend.
inline constexpr DurationUnit kDurationUnit =
    backend::kDurationUnit;  // <cycles, ns, etc.>

// Returns the duration in the specified unit.
inline int64_t GetDuration(Timestamp begin, Timestamp end) {
  return backend::GetDuration(begin, end);
}

constexpr const char* GetDurationUnitStr() {
  switch (kDurationUnit) {
    case DurationUnit::kNanoseconds:
      return "ns";
    case DurationUnit::kClockCycle:
      return "clock cycles";
    default:
      return "unknown";
  }
}
}  // namespace pw::perf_test::internal
