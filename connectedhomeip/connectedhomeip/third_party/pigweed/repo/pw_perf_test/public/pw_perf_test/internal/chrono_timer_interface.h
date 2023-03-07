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

#include <chrono>
#include <cstdbool>

#include "pw_chrono/system_clock.h"
#include "pw_perf_test/internal/duration_unit.h"

namespace pw::perf_test::internal::backend {

using Timestamp = chrono::SystemClock::time_point;

inline constexpr DurationUnit kDurationUnit = DurationUnit::kNanoseconds;

[[nodiscard]] inline bool TimerPrepare() { return true; }

inline void TimerCleanup() {}

inline Timestamp GetCurrentTimestamp() { return chrono::SystemClock::now(); }

inline int64_t GetDuration(Timestamp begin, Timestamp end) {
  return std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin)
      .count();
}

}  // namespace pw::perf_test::internal::backend
