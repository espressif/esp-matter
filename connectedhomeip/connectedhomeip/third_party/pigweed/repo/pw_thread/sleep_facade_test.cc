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

#include <chrono>

#include "gtest/gtest.h"
#include "pw_chrono/system_clock.h"
#include "pw_thread/id.h"
#include "pw_thread/sleep.h"

using pw::chrono::SystemClock;
using namespace std::chrono_literals;

namespace pw::this_thread {
namespace {

extern "C" {

// Functions defined in sleep_facade_test_c.c which call the API from C.
void pw_this_thread_CallSleepFor(pw_chrono_SystemClock_Duration sleep_duration);
void pw_this_thread_CallSleepUntil(pw_chrono_SystemClock_TimePoint wakeup_time);

}  // extern "C"

// We can't control the SystemClock's period configuration, so just in case
// duration cannot be accurately expressed in integer ticks, round the
// duration up.
constexpr SystemClock::duration kRoundedArbitraryDuration =
    SystemClock::for_at_least(42ms);
constexpr pw_chrono_SystemClock_Duration kRoundedArbitraryDurationInC =
    PW_SYSTEM_CLOCK_MS(42);

TEST(Sleep, SleepForPositiveDuration) {
  // Ensure we are in a thread context, meaning we are permitted to sleep.
  ASSERT_NE(get_id(), thread::Id());

  SystemClock::time_point before = SystemClock::now();
  sleep_for(kRoundedArbitraryDuration);
  SystemClock::duration time_elapsed = SystemClock::now() - before;
  EXPECT_GE(time_elapsed, kRoundedArbitraryDuration);
}

TEST(Sleep, SleepForZeroLengthDuration) {
  // Ensure we are in a thread context, meaning we are permitted to sleep.
  ASSERT_NE(get_id(), thread::Id());

  // Ensure it doesn't sleep when a zero length duration is used.
  SystemClock::time_point before = SystemClock::now();
  sleep_for(SystemClock::duration::zero());
  SystemClock::duration time_elapsed = SystemClock::now() - before;
  EXPECT_LT(time_elapsed, kRoundedArbitraryDuration);
}

TEST(Sleep, SleepForNegativeDuration) {
  // Ensure we are in a thread context, meaning we are permitted to sleep.
  ASSERT_NE(get_id(), thread::Id());

  // Ensure it doesn't sleep when a negative duration is used.
  SystemClock::time_point before = SystemClock::now();
  sleep_for(-kRoundedArbitraryDuration);
  SystemClock::duration time_elapsed = SystemClock::now() - before;
  EXPECT_LT(time_elapsed, kRoundedArbitraryDuration);
}

TEST(Sleep, SleepUntilFutureWakeupTime) {
  // Ensure we are in a thread context, meaning we are permitted to sleep.
  ASSERT_NE(get_id(), thread::Id());

  SystemClock::time_point deadline =
      SystemClock::now() + kRoundedArbitraryDuration;
  sleep_until(deadline);
  EXPECT_GE(SystemClock::now(), deadline);
}

TEST(Sleep, SleepUntilCurrentWakeupTime) {
  // Ensure we are in a thread context, meaning we are permitted to sleep.
  ASSERT_NE(get_id(), thread::Id());

  // Ensure it doesn't sleep when now is used.
  SystemClock::time_point deadline =
      SystemClock::now() + kRoundedArbitraryDuration;
  sleep_until(SystemClock::now());
  EXPECT_LT(SystemClock::now(), deadline);
}

TEST(Sleep, SleepUntilPastWakeupTime) {
  // Ensure we are in a thread context, meaning we are permitted to sleep.
  ASSERT_NE(get_id(), thread::Id());

  // Ensure it doesn't sleep when a timestamp in the past is used.
  SystemClock::time_point deadline =
      SystemClock::now() + kRoundedArbitraryDuration;
  sleep_until(SystemClock::now() - kRoundedArbitraryDuration);
  EXPECT_LT(SystemClock::now(), deadline);
}

TEST(Sleep, SleepForPositiveDurationInC) {
  // Ensure we are in a thread context, meaning we are permitted to sleep.
  ASSERT_NE(get_id(), thread::Id());

  pw_chrono_SystemClock_TimePoint before = pw_chrono_SystemClock_Now();
  pw_this_thread_SleepFor(kRoundedArbitraryDurationInC);
  pw_chrono_SystemClock_Duration time_elapsed =
      pw_chrono_SystemClock_TimeElapsed(before, pw_chrono_SystemClock_Now());
  EXPECT_GE(time_elapsed.ticks, kRoundedArbitraryDurationInC.ticks);
}

TEST(Sleep, SleepForZeroLengthDurationInC) {
  // Ensure we are in a thread context, meaning we are permitted to sleep.
  ASSERT_NE(get_id(), thread::Id());

  // Ensure it doesn't sleep when a zero length duration is used.
  pw_chrono_SystemClock_TimePoint before = pw_chrono_SystemClock_Now();
  pw_this_thread_SleepFor(PW_SYSTEM_CLOCK_MS(0));
  pw_chrono_SystemClock_Duration time_elapsed =
      pw_chrono_SystemClock_TimeElapsed(before, pw_chrono_SystemClock_Now());
  EXPECT_LT(time_elapsed.ticks, kRoundedArbitraryDurationInC.ticks);
}

TEST(Sleep, SleepForNegativeDurationInC) {
  // Ensure we are in a thread context, meaning we are permitted to sleep.
  ASSERT_NE(get_id(), thread::Id());

  // Ensure it doesn't sleep when a negative duration is used.
  pw_chrono_SystemClock_TimePoint before = pw_chrono_SystemClock_Now();
  pw_this_thread_SleepFor(
      PW_SYSTEM_CLOCK_MS(-kRoundedArbitraryDurationInC.ticks));
  pw_chrono_SystemClock_Duration time_elapsed =
      pw_chrono_SystemClock_TimeElapsed(before, pw_chrono_SystemClock_Now());
  EXPECT_LT(time_elapsed.ticks, kRoundedArbitraryDurationInC.ticks);
}

TEST(Sleep, SleepUntilFutureWakeupTimeInC) {
  // Ensure we are in a thread context, meaning we are permitted to sleep.
  ASSERT_NE(get_id(), thread::Id());

  pw_chrono_SystemClock_TimePoint deadline;
  deadline.duration_since_epoch.ticks =
      pw_chrono_SystemClock_Now().duration_since_epoch.ticks +
      kRoundedArbitraryDurationInC.ticks;
  pw_this_thread_CallSleepUntil(deadline);
  EXPECT_GE(pw_chrono_SystemClock_Now().duration_since_epoch.ticks,
            deadline.duration_since_epoch.ticks);
}

TEST(Sleep, SleepUntilCurrentWakeupTimeInC) {
  // Ensure we are in a thread context, meaning we are permitted to sleep.
  ASSERT_NE(get_id(), thread::Id());

  // Ensure it doesn't sleep when now is used.
  pw_chrono_SystemClock_TimePoint deadline;
  deadline.duration_since_epoch.ticks =
      pw_chrono_SystemClock_Now().duration_since_epoch.ticks +
      kRoundedArbitraryDurationInC.ticks;
  pw_this_thread_CallSleepUntil(pw_chrono_SystemClock_Now());
  EXPECT_LT(pw_chrono_SystemClock_Now().duration_since_epoch.ticks,
            deadline.duration_since_epoch.ticks);
}

TEST(Sleep, SleepUntilPastWakeupTimeInC) {
  // Ensure we are in a thread context, meaning we are permitted to sleep.
  ASSERT_NE(get_id(), thread::Id());

  // Ensure it doesn't sleep when a timestamp in the past is used.
  pw_chrono_SystemClock_TimePoint deadline;
  deadline.duration_since_epoch.ticks =
      pw_chrono_SystemClock_Now().duration_since_epoch.ticks +
      kRoundedArbitraryDurationInC.ticks;
  pw_chrono_SystemClock_TimePoint old_timestamp;
  old_timestamp.duration_since_epoch.ticks =
      pw_chrono_SystemClock_Now().duration_since_epoch.ticks -
      kRoundedArbitraryDurationInC.ticks;
  pw_this_thread_CallSleepUntil(old_timestamp);
  EXPECT_LT(pw_chrono_SystemClock_Now().duration_since_epoch.ticks,
            deadline.duration_since_epoch.ticks);
}

}  // namespace
}  // namespace pw::this_thread
