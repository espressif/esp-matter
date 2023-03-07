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

#include "gtest/gtest.h"
#include "pw_sync/interrupt_spin_lock.h"

namespace pw::sync {
namespace {

extern "C" {

// Functions defined in interrupt_spin_lock_facade_test_c.c which call the API
// from C.
void pw_sync_InterruptSpinLock_CallLock(
    pw_sync_InterruptSpinLock* interrupt_spin_lock);
bool pw_sync_InterruptSpinLock_CallTryLock(
    pw_sync_InterruptSpinLock* interrupt_spin_lock);
void pw_sync_InterruptSpinLock_CallUnlock(
    pw_sync_InterruptSpinLock* interrupt_spin_lock);

}  // extern "C"

TEST(InterruptSpinLock, LockUnlock) {
  pw::sync::InterruptSpinLock interrupt_spin_lock;
  interrupt_spin_lock.lock();
  interrupt_spin_lock.unlock();
}

// TODO(b/235284163): Add real concurrency tests once we have pw::thread on SMP
// systems given that uniprocessor systems cannot fail to acquire an ISL.

InterruptSpinLock static_interrupt_spin_lock;
TEST(InterruptSpinLock, LockUnlockStatic) {
  static_interrupt_spin_lock.lock();
  // TODO(b/235284163): Ensure other cores fail to lock when its locked.
  // EXPECT_FALSE(static_interrupt_spin_lock.try_lock());
  static_interrupt_spin_lock.unlock();
}

TEST(InterruptSpinLock, TryLockUnlock) {
  pw::sync::InterruptSpinLock interrupt_spin_lock;
  const bool locked = interrupt_spin_lock.try_lock();
  EXPECT_TRUE(locked);
  if (locked) {
    // TODO(b/235284163): Ensure other cores fail to lock when its locked.
    // EXPECT_FALSE(interrupt_spin_lock.try_lock());
    interrupt_spin_lock.unlock();
  }
}

TEST(VirtualInterruptSpinLock, LockUnlock) {
  pw::sync::VirtualInterruptSpinLock interrupt_spin_lock;
  interrupt_spin_lock.lock();
  // TODO(b/235284163): Ensure other cores fail to lock when its locked.
  // EXPECT_FALSE(interrupt_spin_lock.try_lock());
  interrupt_spin_lock.unlock();
}

VirtualInterruptSpinLock static_virtual_interrupt_spin_lock;
TEST(VirtualInterruptSpinLock, LockUnlockStatic) {
  static_virtual_interrupt_spin_lock.lock();
  // TODO(b/235284163): Ensure other cores fail to lock when its locked.
  // EXPECT_FALSE(static_virtual_interrupt_spin_lock.try_lock());
  static_virtual_interrupt_spin_lock.unlock();
}

TEST(InterruptSpinLock, LockUnlockInC) {
  pw::sync::InterruptSpinLock interrupt_spin_lock;
  pw_sync_InterruptSpinLock_CallLock(&interrupt_spin_lock);
  pw_sync_InterruptSpinLock_CallUnlock(&interrupt_spin_lock);
}

TEST(InterruptSpinLock, TryLockUnlockInC) {
  pw::sync::InterruptSpinLock interrupt_spin_lock;
  ASSERT_TRUE(pw_sync_InterruptSpinLock_CallTryLock(&interrupt_spin_lock));
  // TODO(b/235284163): Ensure other cores fail to lock when its locked.
  // EXPECT_FALSE(pw_sync_InterruptSpinLock_CallTryLock(&interrupt_spin_lock));
  pw_sync_InterruptSpinLock_CallUnlock(&interrupt_spin_lock);
}

}  // namespace
}  // namespace pw::sync
