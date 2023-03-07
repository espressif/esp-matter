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
#include "pw_sync/counting_semaphore.h"

using pw::chrono::SystemClock;
using namespace std::chrono_literals;

namespace pw::sync {
namespace {

extern "C" {

// Functions defined in counting_semaphore_facade_test_c.c which call the API
// from C.
void pw_sync_CountingSemaphore_CallRelease(
    pw_sync_CountingSemaphore* semaphore);
void pw_sync_CountingSemaphore_CallReleaseNum(
    pw_sync_CountingSemaphore* semaphore, ptrdiff_t update);
void pw_sync_CountingSemaphore_CallAcquire(
    pw_sync_CountingSemaphore* semaphore);
bool pw_sync_CountingSemaphore_CallTryAcquire(
    pw_sync_CountingSemaphore* semaphore);
bool pw_sync_CountingSemaphore_CallTryAcquireFor(
    pw_sync_CountingSemaphore* semaphore,
    pw_chrono_SystemClock_Duration timeout);
bool pw_sync_CountingSemaphore_CallTryAcquireUntil(
    pw_sync_CountingSemaphore* semaphore,
    pw_chrono_SystemClock_TimePoint deadline);
ptrdiff_t pw_sync_CountingSemaphore_CallMax(void);

}  // extern "C"

// We can't control the SystemClock's period configuration, so just in case
// duration cannot be accurately expressed in integer ticks, round the
// duration up.
constexpr SystemClock::duration kRoundedArbitraryDuration =
    SystemClock::for_at_least(42ms);
constexpr pw_chrono_SystemClock_Duration kRoundedArbitraryDurationInC =
    PW_SYSTEM_CLOCK_MS(42);

TEST(CountingSemaphore, EmptyInitialState) {
  CountingSemaphore semaphore;
  EXPECT_FALSE(semaphore.try_acquire());
}

// TODO(b/235284163): Add real concurrency tests once we have pw::thread.

TEST(CountingSemaphore, SingleRelease) {
  CountingSemaphore semaphore;
  semaphore.release();
  semaphore.release();
  semaphore.acquire();
  semaphore.acquire();
  // Ensure it fails when empty.
  EXPECT_FALSE(semaphore.try_acquire());
}

CountingSemaphore empty_initial_semaphore;
TEST(CountingSemaphore, EmptyInitialStateStatic) {
  EXPECT_FALSE(empty_initial_semaphore.try_acquire());
}

CountingSemaphore release_semaphore;
TEST(CountingSemaphore, ReleaseStatic) {
  release_semaphore.release();
  release_semaphore.release();
  release_semaphore.acquire();
  release_semaphore.acquire();
  // Ensure it fails when empty.
  EXPECT_FALSE(release_semaphore.try_acquire());
}

TEST(CountingSemaphore, MultiRelease) {
  CountingSemaphore semaphore;
  semaphore.release(2);
  semaphore.release(1);
  semaphore.acquire();
  semaphore.acquire();
  semaphore.acquire();
  // Ensure it fails when empty.
  EXPECT_FALSE(semaphore.try_acquire());
}

TEST(CountingSemaphore, TryAcquireForFull) {
  CountingSemaphore semaphore;
  semaphore.release();

  // Ensure it doesn't block and succeeds if not empty.
  SystemClock::time_point before = SystemClock::now();
  EXPECT_TRUE(semaphore.try_acquire_for(kRoundedArbitraryDuration));
  SystemClock::duration time_elapsed = SystemClock::now() - before;
  EXPECT_LT(time_elapsed, kRoundedArbitraryDuration);
}

TEST(CountingSemaphore, TryAcquireForEmptyPositiveTimeout) {
  CountingSemaphore semaphore;

  // Ensure it blocks and fails when empty.
  SystemClock::time_point before = SystemClock::now();
  EXPECT_FALSE(semaphore.try_acquire_for(kRoundedArbitraryDuration));
  SystemClock::duration time_elapsed = SystemClock::now() - before;
  EXPECT_GE(time_elapsed, kRoundedArbitraryDuration);
}

TEST(CountingSemaphore, TryAcquireForEmptyZeroLengthTimeout) {
  CountingSemaphore semaphore;

  // Ensure it doesn't block and fails when empty and a zero length duration is
  // used.
  SystemClock::time_point before = SystemClock::now();
  EXPECT_FALSE(semaphore.try_acquire_for(SystemClock::duration::zero()));
  SystemClock::duration time_elapsed = SystemClock::now() - before;
  EXPECT_LT(time_elapsed, kRoundedArbitraryDuration);
}

TEST(CountingSemaphore, TryAcquireForEmptyNegativeTimeout) {
  CountingSemaphore semaphore;

  // Ensure it doesn't block and fails when empty and a negative duration is
  // used.
  SystemClock::time_point before = SystemClock::now();
  EXPECT_FALSE(semaphore.try_acquire_for(-kRoundedArbitraryDuration));
  SystemClock::duration time_elapsed = SystemClock::now() - before;
  EXPECT_LT(time_elapsed, kRoundedArbitraryDuration);
}

TEST(CountingSemaphore, TryAcquireUntilFull) {
  CountingSemaphore semaphore;
  semaphore.release();

  // Ensure it doesn't block and succeeds if not empty.
  SystemClock::time_point deadline =
      SystemClock::now() + kRoundedArbitraryDuration;
  EXPECT_TRUE(semaphore.try_acquire_until(deadline));
  EXPECT_LT(SystemClock::now(), deadline);
}

TEST(CountingSemaphore, TryAcquireUntilEmptyFutureDeadline) {
  CountingSemaphore semaphore;

  // Ensure it blocks and fails when empty.
  SystemClock::time_point deadline =
      SystemClock::now() + kRoundedArbitraryDuration;
  EXPECT_FALSE(semaphore.try_acquire_until(deadline));
  EXPECT_GE(SystemClock::now(), deadline);
}

TEST(CountingSemaphore, TryAcquireUntilEmptyCurrentDeadline) {
  CountingSemaphore semaphore;

  // Ensure it doesn't block and fails when empty and now is used.
  SystemClock::time_point deadline =
      SystemClock::now() + kRoundedArbitraryDuration;
  EXPECT_FALSE(semaphore.try_acquire_until(SystemClock::now()));
  EXPECT_LT(SystemClock::now(), deadline);
}

TEST(CountingSemaphore, TryAcquireUntilEmptyPastDeadline) {
  CountingSemaphore semaphore;
  // Ensure it doesn't block and fails when empty and a timestamp in the past is
  // used.
  SystemClock::time_point deadline =
      SystemClock::now() + kRoundedArbitraryDuration;
  EXPECT_FALSE(semaphore.try_acquire_until(SystemClock::now() -
                                           kRoundedArbitraryDuration));
  EXPECT_LT(SystemClock::now(), deadline);
}

TEST(CountingSemaphore, EmptyInitialStateInC) {
  CountingSemaphore semaphore;
  EXPECT_FALSE(pw_sync_CountingSemaphore_CallTryAcquire(&semaphore));
}

TEST(CountingSemaphore, SingeReleaseInC) {
  CountingSemaphore semaphore;
  pw_sync_CountingSemaphore_CallRelease(&semaphore);
  pw_sync_CountingSemaphore_CallRelease(&semaphore);
  pw_sync_CountingSemaphore_CallAcquire(&semaphore);
  pw_sync_CountingSemaphore_CallAcquire(&semaphore);
  // Ensure it fails when empty.
  EXPECT_FALSE(pw_sync_CountingSemaphore_CallTryAcquire(&semaphore));
}

TEST(CountingSemaphore, MultiReleaseInC) {
  CountingSemaphore semaphore;
  pw_sync_CountingSemaphore_CallReleaseNum(&semaphore, 2);
  pw_sync_CountingSemaphore_CallReleaseNum(&semaphore, 1);
  pw_sync_CountingSemaphore_CallAcquire(&semaphore);
  pw_sync_CountingSemaphore_CallAcquire(&semaphore);
  pw_sync_CountingSemaphore_CallAcquire(&semaphore);
  // Ensure it fails when empty.
  EXPECT_FALSE(pw_sync_CountingSemaphore_CallTryAcquire(&semaphore));
}

TEST(CountingSemaphore, TryAcquireForFullInC) {
  CountingSemaphore semaphore;
  pw_sync_CountingSemaphore_CallRelease(&semaphore);

  // Ensure it doesn't block and succeeds if not empty.
  pw_chrono_SystemClock_TimePoint before = pw_chrono_SystemClock_Now();
  ASSERT_TRUE(pw_sync_CountingSemaphore_CallTryAcquireFor(
      &semaphore, kRoundedArbitraryDurationInC));
  pw_chrono_SystemClock_Duration time_elapsed =
      pw_chrono_SystemClock_TimeElapsed(before, pw_chrono_SystemClock_Now());
  EXPECT_LT(time_elapsed.ticks, kRoundedArbitraryDurationInC.ticks);
}

TEST(CountingSemaphore, TryAcquireForEmptyPositiveTimeoutInC) {
  CountingSemaphore semaphore;

  // Ensure it blocks and fails when empty.
  pw_chrono_SystemClock_TimePoint before = pw_chrono_SystemClock_Now();
  EXPECT_FALSE(pw_sync_CountingSemaphore_CallTryAcquireFor(
      &semaphore, kRoundedArbitraryDurationInC));
  pw_chrono_SystemClock_Duration time_elapsed =
      pw_chrono_SystemClock_TimeElapsed(before, pw_chrono_SystemClock_Now());
  EXPECT_GE(time_elapsed.ticks, kRoundedArbitraryDurationInC.ticks);
}

TEST(CountingSemaphore, TryAcquireForEmptyZeroLengthTimeoutInC) {
  CountingSemaphore semaphore;

  // Ensure it doesn't block and fails when empty and a zero length duration is
  // used.
  pw_chrono_SystemClock_TimePoint before = pw_chrono_SystemClock_Now();
  EXPECT_FALSE(pw_sync_CountingSemaphore_CallTryAcquireFor(
      &semaphore, PW_SYSTEM_CLOCK_MS(0)));
  pw_chrono_SystemClock_Duration time_elapsed =
      pw_chrono_SystemClock_TimeElapsed(before, pw_chrono_SystemClock_Now());
  EXPECT_LT(time_elapsed.ticks, kRoundedArbitraryDurationInC.ticks);
}

TEST(CountingSemaphore, TryAcquireForEmptyNegativeTimeoutInC) {
  CountingSemaphore semaphore;

  // Ensure it doesn't block and fails when empty and a negative duration is
  // used.
  pw_chrono_SystemClock_TimePoint before = pw_chrono_SystemClock_Now();
  EXPECT_FALSE(pw_sync_CountingSemaphore_CallTryAcquireFor(
      &semaphore, PW_SYSTEM_CLOCK_MS(-kRoundedArbitraryDurationInC.ticks)));
  pw_chrono_SystemClock_Duration time_elapsed =
      pw_chrono_SystemClock_TimeElapsed(before, pw_chrono_SystemClock_Now());
  EXPECT_LT(time_elapsed.ticks, kRoundedArbitraryDurationInC.ticks);
}

TEST(CountingSemaphore, TryAcquireUntilFullInC) {
  CountingSemaphore semaphore;
  pw_sync_CountingSemaphore_CallRelease(&semaphore);

  // Ensure it doesn't block and succeeds if not empty.
  pw_chrono_SystemClock_TimePoint deadline;
  deadline.duration_since_epoch.ticks =
      pw_chrono_SystemClock_Now().duration_since_epoch.ticks +
      kRoundedArbitraryDurationInC.ticks;
  ASSERT_TRUE(
      pw_sync_CountingSemaphore_CallTryAcquireUntil(&semaphore, deadline));
  EXPECT_LT(pw_chrono_SystemClock_Now().duration_since_epoch.ticks,
            deadline.duration_since_epoch.ticks);
}

TEST(CountingSemaphore, TryAcquireUntilEmptyFutureDeadlineInC) {
  CountingSemaphore semaphore;

  // Ensure it blocks and fails when empty.
  pw_chrono_SystemClock_TimePoint deadline;
  deadline.duration_since_epoch.ticks =
      pw_chrono_SystemClock_Now().duration_since_epoch.ticks +
      kRoundedArbitraryDurationInC.ticks;
  EXPECT_FALSE(
      pw_sync_CountingSemaphore_CallTryAcquireUntil(&semaphore, deadline));
  EXPECT_GE(pw_chrono_SystemClock_Now().duration_since_epoch.ticks,
            deadline.duration_since_epoch.ticks);
}

TEST(CountingSemaphore, TryAcquireUntilEmptyCurrentDeadlineInC) {
  CountingSemaphore semaphore;

  // Ensure it doesn't block and fails when empty and now is used.
  pw_chrono_SystemClock_TimePoint deadline;
  deadline.duration_since_epoch.ticks =
      pw_chrono_SystemClock_Now().duration_since_epoch.ticks +
      kRoundedArbitraryDurationInC.ticks;
  EXPECT_FALSE(pw_sync_CountingSemaphore_CallTryAcquireUntil(
      &semaphore, pw_chrono_SystemClock_Now()));
  EXPECT_LT(pw_chrono_SystemClock_Now().duration_since_epoch.ticks,
            deadline.duration_since_epoch.ticks);
}

TEST(CountingSemaphore, TryAcquireUntilEmptyPastDeadlineInC) {
  CountingSemaphore semaphore;

  // Ensure it doesn't block and fails when empty and a timestamp in the past is
  // used.
  pw_chrono_SystemClock_TimePoint deadline;
  deadline.duration_since_epoch.ticks =
      pw_chrono_SystemClock_Now().duration_since_epoch.ticks +
      kRoundedArbitraryDurationInC.ticks;
  pw_chrono_SystemClock_TimePoint old_timestamp;
  old_timestamp.duration_since_epoch.ticks =
      pw_chrono_SystemClock_Now().duration_since_epoch.ticks -
      kRoundedArbitraryDurationInC.ticks;
  EXPECT_FALSE(
      pw_sync_CountingSemaphore_CallTryAcquireUntil(&semaphore, old_timestamp));
  EXPECT_LT(pw_chrono_SystemClock_Now().duration_since_epoch.ticks,
            deadline.duration_since_epoch.ticks);
}

TEST(CountingSemaphore, MaxInC) {
  EXPECT_EQ(CountingSemaphore::max(), pw_sync_CountingSemaphore_Max());
}

}  // namespace
}  // namespace pw::sync
