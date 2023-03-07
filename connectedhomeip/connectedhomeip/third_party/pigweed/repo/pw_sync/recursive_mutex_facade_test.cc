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
#include "pw_sync/recursive_mutex.h"

namespace pw::sync {
namespace {

extern "C" {

// Functions defined in recusive_mutex_facade_test_c.c that call the API from C.
void pw_sync_RecursiveMutex_CallLock(pw_sync_RecursiveMutex* mutex);
bool pw_sync_RecursiveMutex_CallTryLock(pw_sync_RecursiveMutex* mutex);
void pw_sync_RecursiveMutex_CallUnlock(pw_sync_RecursiveMutex* mutex);

}  // extern "C"

// TODO(b/235284163): Add real concurrency tests once we have pw::thread.

TEST(RecursiveMutex, LockUnlock) PW_NO_LOCK_SAFETY_ANALYSIS {
  pw::sync::RecursiveMutex mutex;
  for (int i = 0; i < 10; ++i) {
    mutex.lock();
  }

  for (int i = 0; i < 10; ++i) {
    mutex.unlock();
  }
}

RecursiveMutex static_mutex;
TEST(RecursiveMutex, LockUnlockStatic) PW_NO_LOCK_SAFETY_ANALYSIS {
  static_mutex.lock();
  for (int i = 0; i < 10; ++i) {
    EXPECT_TRUE(static_mutex.try_lock());
  }
  for (int i = 0; i < 10; ++i) {
    static_mutex.unlock();  // undo the try_lock() calls
  }
  static_mutex.unlock();  // undo the inital lock() call
}

TEST(RecursiveMutex, TryLockUnlock) PW_NO_LOCK_SAFETY_ANALYSIS {
  pw::sync::RecursiveMutex mutex;
  ASSERT_TRUE(mutex.try_lock());

  const bool locked_again = mutex.try_lock();
  EXPECT_TRUE(locked_again);
  if (locked_again) {
    mutex.unlock();
  }

  mutex.unlock();
}

TEST(RecursiveMutex, LockUnlockInC) {
  pw::sync::RecursiveMutex mutex;

  pw_sync_RecursiveMutex_CallLock(&mutex);
  pw_sync_RecursiveMutex_CallLock(&mutex);
  pw_sync_RecursiveMutex_CallLock(&mutex);

  pw_sync_RecursiveMutex_CallUnlock(&mutex);
  pw_sync_RecursiveMutex_CallUnlock(&mutex);
  pw_sync_RecursiveMutex_CallUnlock(&mutex);
}

TEST(RecursiveMutex, TryLockUnlockInC) {
  pw::sync::RecursiveMutex mutex;
  ASSERT_TRUE(pw_sync_RecursiveMutex_CallTryLock(&mutex));

  const bool locked_again = pw_sync_RecursiveMutex_CallTryLock(&mutex);
  EXPECT_TRUE(locked_again);
  if (locked_again) {
    pw_sync_RecursiveMutex_CallUnlock(&mutex);
  }

  pw_sync_RecursiveMutex_CallUnlock(&mutex);
}

}  // namespace
}  // namespace pw::sync
