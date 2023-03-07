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
#include "pw_sync/mutex.h"

namespace pw::sync {
namespace {

extern "C" {

// Functions defined in mutex_facade_test_c.c which call the API from C.
void pw_sync_Mutex_CallLock(pw_sync_Mutex* mutex);
bool pw_sync_Mutex_CallTryLock(pw_sync_Mutex* mutex);
void pw_sync_Mutex_CallUnlock(pw_sync_Mutex* mutex);

}  // extern "C"

// TODO(b/235284163): Add real concurrency tests once we have pw::thread.

TEST(Mutex, LockUnlock) {
  pw::sync::Mutex mutex;
  mutex.lock();
  // TODO(b/235284163): Ensure it fails to lock when already held.
  // EXPECT_FALSE(mutex.try_lock());
  mutex.unlock();
}

Mutex static_mutex;
TEST(Mutex, LockUnlockStatic) {
  static_mutex.lock();
  // TODO(b/235284163): Ensure it fails to lock when already held.
  // EXPECT_FALSE(static_mutex.try_lock());
  static_mutex.unlock();
}

TEST(Mutex, TryLockUnlock) {
  pw::sync::Mutex mutex;
  const bool locked = mutex.try_lock();
  EXPECT_TRUE(locked);
  if (locked) {
    // TODO(b/235284163): Ensure it fails to lock when already held.
    // EXPECT_FALSE(mutex.try_lock());
    mutex.unlock();
  }
}

TEST(VirtualMutex, LockUnlock) {
  pw::sync::VirtualMutex mutex;
  mutex.lock();
  // TODO(b/235284163): Ensure it fails to lock when already held.
  // EXPECT_FALSE(mutex.try_lock());
  mutex.unlock();
}

VirtualMutex static_virtual_mutex;
TEST(VirtualMutex, LockUnlockStatic) {
  static_virtual_mutex.lock();
  // TODO(b/235284163): Ensure it fails to lock when already held.
  // EXPECT_FALSE(static_virtual_mutex.try_lock());
  static_virtual_mutex.unlock();
}

TEST(Mutex, LockUnlockInC) {
  pw::sync::Mutex mutex;
  pw_sync_Mutex_CallLock(&mutex);
  pw_sync_Mutex_CallUnlock(&mutex);
}

TEST(Mutex, TryLockUnlockInC) {
  pw::sync::Mutex mutex;
  ASSERT_TRUE(pw_sync_Mutex_CallTryLock(&mutex));
  // TODO(b/235284163): Ensure it fails to lock when already held.
  // EXPECT_FALSE(pw_sync_Mutex_CallTryLock(&mutex));
  pw_sync_Mutex_CallUnlock(&mutex);
}

}  // namespace
}  // namespace pw::sync
