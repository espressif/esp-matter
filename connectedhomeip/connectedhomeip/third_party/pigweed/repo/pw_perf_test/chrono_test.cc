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

#include <chrono>

#include "gtest/gtest.h"
#include "pw_chrono/system_clock.h"
#include "pw_perf_test/internal/chrono_timer_interface.h"
#include "pw_thread/sleep.h"

namespace pw::perf_test::internal::backend {
namespace {

constexpr chrono::SystemClock::duration kArbitraryDuration =
    chrono::SystemClock::for_at_least(std::chrono::milliseconds(1));

TEST(ChronoTest, DurationIsReasonable) {
  Timestamp start = GetCurrentTimestamp();
  this_thread::sleep_for(kArbitraryDuration);
  Timestamp end = GetCurrentTimestamp();
  int64_t duration = GetDuration(start, end);
  EXPECT_GE(duration, 1000000);
}

}  // namespace
}  // namespace pw::perf_test::internal::backend
