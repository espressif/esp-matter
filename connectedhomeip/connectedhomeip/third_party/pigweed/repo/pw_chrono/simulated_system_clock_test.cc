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

#include "pw_chrono/simulated_system_clock.h"

#include "gtest/gtest.h"

using namespace std::chrono_literals;

namespace pw::chrono {
namespace {

// We can't control the SystemClock's period configuration, so just in case
// 42 hours cannot be accurately expressed in integer ticks, round the
// duration up.
constexpr SystemClock::duration kRoundedArbitraryDuration =
    SystemClock::for_at_least(42h);

TEST(SimulatedSystemClock, InitialTime) {
  SimulatedSystemClock clock;

  EXPECT_EQ(SystemClock::time_point(SystemClock::duration(0)), clock.now());
}

TEST(SimulatedSystemClock, SetTime) {
  SimulatedSystemClock clock;

  clock.SetTime(pw::chrono::SystemClock::time_point(kRoundedArbitraryDuration));
  EXPECT_EQ(kRoundedArbitraryDuration, clock.now().time_since_epoch());
}

TEST(SimulatedSystemClock, AdvanceTime) {
  SimulatedSystemClock clock;

  const SystemClock::time_point before_timestamp = clock.now();
  clock.AdvanceTime(kRoundedArbitraryDuration);
  const SystemClock::time_point after_timestamp = clock.now();

  EXPECT_EQ(kRoundedArbitraryDuration, clock.now().time_since_epoch());
  EXPECT_EQ(kRoundedArbitraryDuration, after_timestamp - before_timestamp);
}

}  // namespace
}  // namespace pw::chrono
