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
#include "pw_sync/binary_semaphore.h"

using pw::chrono::SystemClock;
using namespace std::chrono_literals;

namespace pw::sync {
namespace {

extern "C" {

// Functions defined in binary_semaphore_facade_test_c.c which call the API
// from C.
void pw_sync_BinarySemaphore_CallRelease(pw_sync_BinarySemaphore* semaphore);
void pw_sync_BinarySemaphore_CallAcquire(pw_sync_BinarySemaphore* semaphore);
bool pw_sync_BinarySemaphore_CallTryAcquire(pw_sync_BinarySemaphore* semaphore);
bool pw_sync_BinarySemaphore_CallTryAcquireFor(
    pw_sync_BinarySemaphore* semaphore, pw_chrono_SystemClock_Duration timeout);
bool pw_sync_BinarySemaphore_CallTryAcquireUntil(
    pw_sync_BinarySemaphore* semaphore,
    pw_chrono_SystemClock_TimePoint deadline);
ptrdiff_t pw_sync_BinarySemaphore_CallMax(void);

}  // extern "C"

// We can't control the SystemClock's period configuration, so just in case
// duration cannot be accurately expressed in integer ticks, round the
// duration up.
constexpr SystemClock::duration kRoundedArbitraryDuration =
    SystemClock::for_at_least(42ms);
constexpr pw_chrono_SystemClock_Duration kRoundedArbitraryDurationInC =
    PW_SYSTEM_CLOCK_MS(42);

TEST(BinarySemaphore, EmptyInitialState) {
  BinarySemaphore semaphore;
  EXPECT_FALSE(semaphore.try_acquire());
}

// TODO(b/235284163): Add real concurrency tests once we have pw::thread.

TEST(BinarySemaphore, Release) {
  BinarySemaphore semaphore;
  semaphore.release();
  semaphore.release();
  semaphore.acquire();
  // Ensure it fails when empty.
  EXPECT_FALSE(semaphore.try_acquire());
}

BinarySemaphore empty_initial_semaphore;
TEST(BinarySemaphore, EmptyInitialStateStatic) {
  EXPECT_FALSE(empty_initial_semaphore.try_acquire());
}

BinarySemaphore release_semaphore;
TEST(BinarySemaphore, ReleaseStatic) {
  release_semaphore.release();
  release_semaphore.release();
  release_semaphore.acquire();
  // Ensure it fails when empty.
  EXPECT_FALSE(release_semaphore.try_acquire());
}

TEST(BinarySemaphore, TryAcquireForFull) {
  BinarySemaphore semaphore;
  semaphore.release();

  // Ensure it doesn't block and succeeds if not empty.
  SystemClock::time_point before = SystemClock::now();
  EXPECT_TRUE(semaphore.try_acquire_for(kRoundedArbitraryDuration));
  SystemClock::duration time_elapsed = SystemClock::now() - before;
  EXPECT_LT(time_elapsed, kRoundedArbitraryDuration);
}

TEST(BinarySemaphore, TryAcquireForEmptyPositiveTimeout) {
  BinarySemaphore semaphore;

  // Ensure it blocks and fails when empty.
  SystemClock::time_point before = SystemClock::now();
  EXPECT_FALSE(semaphore.try_acquire_for(kRoundedArbitraryDuration));
  SystemClock::duration time_elapsed = SystemClock::now() - before;
  EXPECT_GE(time_elapsed, kRoundedArbitraryDuration);
}

TEST(BinarySemaphore, TryAcquireForEmptyZeroLengthTimeout) {
  BinarySemaphore semaphore;

  // Ensure it doesn't block and fails when empty and a zero length duration is
  // used.
  SystemClock::time_point before = SystemClock::now();
  EXPECT_FALSE(semaphore.try_acquire_for(SystemClock::duration::zero()));
  SystemClock::duration time_elapsed = SystemClock::now() - before;
  EXPECT_LT(time_elapsed, kRoundedArbitraryDuration);
}

TEST(BinarySemaphore, TryAcquireForEmptyNegativeTimeout) {
  BinarySemaphore semaphore;

  // Ensure it doesn't block and fails when empty and a negative duration is
  // used.
  SystemClock::time_point before = SystemClock::now();
  EXPECT_FALSE(semaphore.try_acquire_for(-kRoundedArbitraryDuration));
  SystemClock::duration time_elapsed = SystemClock::now() - before;
  EXPECT_LT(time_elapsed, kRoundedArbitraryDuration);
}

TEST(BinarySemaphore, TryAcquireUntilFull) {
  BinarySemaphore semaphore;
  semaphore.release();

  // Ensure it doesn't block and succeeds if not empty.
  SystemClock::time_point deadline =
      SystemClock::now() + kRoundedArbitraryDuration;
  EXPECT_TRUE(semaphore.try_acquire_until(deadline));
  EXPECT_LT(SystemClock::now(), deadline);
}

TEST(BinarySemaphore, TryAcquireUntilEmptyFutureDeadline) {
  BinarySemaphore semaphore;

  // Ensure it blocks and fails when empty.
  SystemClock::time_point deadline =
      SystemClock::now() + kRoundedArbitraryDuration;
  EXPECT_FALSE(semaphore.try_acquire_until(deadline));
  EXPECT_GE(SystemClock::now(), deadline);
}

TEST(BinarySemaphore, TryAcquireUntilEmptyCurrentDeadline) {
  BinarySemaphore semaphore;

  // Ensure it doesn't block and fails when empty and now is used.
  SystemClock::time_point deadline =
      SystemClock::now() + kRoundedArbitraryDuration;
  EXPECT_FALSE(semaphore.try_acquire_until(SystemClock::now()));
  EXPECT_LT(SystemClock::now(), deadline);
}

TEST(BinarySemaphore, TryAcquireUntilEmptyPastDeadline) {
  BinarySemaphore semaphore;

  // Ensure it doesn't block and fails when empty and a timestamp in the past is
  // used.
  SystemClock::time_point deadline =
      SystemClock::now() + kRoundedArbitraryDuration;
  EXPECT_FALSE(semaphore.try_acquire_until(SystemClock::now() -
                                           kRoundedArbitraryDuration));
  EXPECT_LT(SystemClock::now(), deadline);
}

TEST(BinarySemaphore, EmptyInitialStateInC) {
  BinarySemaphore semaphore;
  EXPECT_FALSE(pw_sync_BinarySemaphore_CallTryAcquire(&semaphore));
}

TEST(BinarySemaphore, ReleaseInC) {
  BinarySemaphore semaphore;
  pw_sync_BinarySemaphore_CallRelease(&semaphore);
  pw_sync_BinarySemaphore_CallRelease(&semaphore);
  pw_sync_BinarySemaphore_CallAcquire(&semaphore);
  // Ensure it fails when empty.
  EXPECT_FALSE(pw_sync_BinarySemaphore_CallTryAcquire(&semaphore));
}

TEST(BinarySemaphore, TryAcquireForFullInC) {
  BinarySemaphore semaphore;
  pw_sync_BinarySemaphore_CallRelease(&semaphore);

  // Ensure it doesn't block and succeeds if not empty.
  pw_chrono_SystemClock_TimePoint before = pw_chrono_SystemClock_Now();
  ASSERT_TRUE(pw_sync_BinarySemaphore_CallTryAcquireFor(
      &semaphore, kRoundedArbitraryDurationInC));
  pw_chrono_SystemClock_Duration time_elapsed =
      pw_chrono_SystemClock_TimeElapsed(before, pw_chrono_SystemClock_Now());
  EXPECT_LT(time_elapsed.ticks, kRoundedArbitraryDurationInC.ticks);
}

TEST(BinarySemaphore, TryAcquireForEmptyPositiveTimeoutInC) {
  BinarySemaphore semaphore;

  // Ensure it blocks and fails when empty.
  pw_chrono_SystemClock_TimePoint before = pw_chrono_SystemClock_Now();
  EXPECT_FALSE(pw_sync_BinarySemaphore_CallTryAcquireFor(
      &semaphore, kRoundedArbitraryDurationInC));
  pw_chrono_SystemClock_Duration time_elapsed =
      pw_chrono_SystemClock_TimeElapsed(before, pw_chrono_SystemClock_Now());
  EXPECT_GE(time_elapsed.ticks, kRoundedArbitraryDurationInC.ticks);
}

TEST(BinarySemaphore, TryAcquireForEmptyZeroLengthTimeoutInC) {
  BinarySemaphore semaphore;

  // Ensure it doesn't block and fails when empty and a zero length duration is
  // used.
  pw_chrono_SystemClock_TimePoint before = pw_chrono_SystemClock_Now();
  EXPECT_FALSE(pw_sync_BinarySemaphore_CallTryAcquireFor(
      &semaphore, PW_SYSTEM_CLOCK_MS(0)));
  pw_chrono_SystemClock_Duration time_elapsed =
      pw_chrono_SystemClock_TimeElapsed(before, pw_chrono_SystemClock_Now());
  EXPECT_LT(time_elapsed.ticks, kRoundedArbitraryDurationInC.ticks);
}

TEST(BinarySemaphore, TryAcquireForEmptyNegativeTimeoutInC) {
  BinarySemaphore semaphore;

  // Ensure it doesn't block and fails when empty and a negative duration is
  // used.
  pw_chrono_SystemClock_TimePoint before = pw_chrono_SystemClock_Now();
  EXPECT_FALSE(pw_sync_BinarySemaphore_CallTryAcquireFor(
      &semaphore, PW_SYSTEM_CLOCK_MS(-kRoundedArbitraryDurationInC.ticks)));
  pw_chrono_SystemClock_Duration time_elapsed =
      pw_chrono_SystemClock_TimeElapsed(before, pw_chrono_SystemClock_Now());
  EXPECT_LT(time_elapsed.ticks, kRoundedArbitraryDurationInC.ticks);
}

TEST(BinarySemaphore, TryAcquireUntilFullInC) {
  BinarySemaphore semaphore;
  pw_sync_BinarySemaphore_CallRelease(&semaphore);

  pw_chrono_SystemClock_TimePoint deadline;
  deadline.duration_since_epoch.ticks =
      pw_chrono_SystemClock_Now().duration_since_epoch.ticks +
      kRoundedArbitraryDurationInC.ticks;
  ASSERT_TRUE(
      pw_sync_BinarySemaphore_CallTryAcquireUntil(&semaphore, deadline));
  EXPECT_LT(pw_chrono_SystemClock_Now().duration_since_epoch.ticks,
            deadline.duration_since_epoch.ticks);
}

TEST(BinarySemaphore, TryAcquireUntilEmptyFutureDeadlineInC) {
  BinarySemaphore semaphore;

  // Ensure it blocks and fails when empty.
  pw_chrono_SystemClock_TimePoint deadline;
  deadline.duration_since_epoch.ticks =
      pw_chrono_SystemClock_Now().duration_since_epoch.ticks +
      kRoundedArbitraryDurationInC.ticks;
  EXPECT_FALSE(
      pw_sync_BinarySemaphore_CallTryAcquireUntil(&semaphore, deadline));
  EXPECT_GE(pw_chrono_SystemClock_Now().duration_since_epoch.ticks,
            deadline.duration_since_epoch.ticks);
}

TEST(BinarySemaphore, TryAcquireUntilEmptyCurrentDeadlineInC) {
  BinarySemaphore semaphore;

  // Ensure it doesn't block and fails when empty and now is used.
  pw_chrono_SystemClock_TimePoint deadline;
  deadline.duration_since_epoch.ticks =
      pw_chrono_SystemClock_Now().duration_since_epoch.ticks +
      kRoundedArbitraryDurationInC.ticks;
  EXPECT_FALSE(pw_sync_BinarySemaphore_CallTryAcquireUntil(
      &semaphore, pw_chrono_SystemClock_Now()));
  EXPECT_LT(pw_chrono_SystemClock_Now().duration_since_epoch.ticks,
            deadline.duration_since_epoch.ticks);
}

TEST(BinarySemaphore, TryAcquireUntilEmptyPastDeadlineInC) {
  BinarySemaphore semaphore;

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
      pw_sync_BinarySemaphore_CallTryAcquireUntil(&semaphore, old_timestamp));
  EXPECT_LT(pw_chrono_SystemClock_Now().duration_since_epoch.ticks,
            deadline.duration_since_epoch.ticks);
}

TEST(BinarySemaphore, MaxInC) {
  EXPECT_EQ(BinarySemaphore::max(), pw_sync_BinarySemaphore_Max());
}

}  // namespace
}  // namespace pw::sync
