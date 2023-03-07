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
#pragma once

#include <stdbool.h>

#include "pw_preprocessor/util.h"
#include "pw_sync/lock_annotations.h"

#ifdef __cplusplus

#include "pw_sync/virtual_basic_lockable.h"
#include "pw_sync_backend/interrupt_spin_lock_native.h"

namespace pw::sync {

// The InterruptSpinLock is a synchronization primitive that can be used to
// protect shared data from being simultaneously accessed by multiple threads
// and/or interrupts as a targeted global lock, with the exception of
// Non-Maskable Interrupts (NMIs).
// It offers exclusive, non-recursive ownership semantics where IRQs up to a
// backend defined level of "NMIs" will be masked to solve priority-inversion.
//
// NOTE: This InterruptSpinLock relies on built-in local interrupt masking to
// make it interrupt safe without requiring the caller to separately mask and
// unmask interrupts when using this primitive.
//
// Unlike global interrupt locks, this also works safely and efficiently on SMP
// systems. On systems which are not SMP, spinning is not required and it's
// possible that only interrupt masking occurs but some state may still be used
// to detect recursion.
//
// This entire API is IRQ safe, but NOT NMI safe.
//
// Precondition: Code that holds a specific InterruptSpinLock must not try to
// re-acquire it. However, it is okay to nest distinct spinlocks.
class PW_LOCKABLE("pw::sync::InterruptSpinLock") InterruptSpinLock {
 public:
  using native_handle_type = backend::NativeInterruptSpinLockHandle;

  constexpr InterruptSpinLock();
  ~InterruptSpinLock() = default;
  InterruptSpinLock(const InterruptSpinLock&) = delete;
  InterruptSpinLock(InterruptSpinLock&&) = delete;
  InterruptSpinLock& operator=(const InterruptSpinLock&) = delete;
  InterruptSpinLock& operator=(InterruptSpinLock&&) = delete;

  // Locks the spinlock, blocking indefinitely. Failures are fatal.
  //
  // Precondition: Recursive locking is undefined behavior.
  void lock() PW_EXCLUSIVE_LOCK_FUNCTION();

  // Tries to lock the spinlock in a non-blocking manner.
  // Returns true if the spinlock was successfully acquired.
  //
  // Precondition: Recursive locking is undefined behavior.
  bool try_lock() PW_EXCLUSIVE_TRYLOCK_FUNCTION(true);

  // Unlocks the spinlock. Failures are fatal.
  //
  // PRECONDITION:
  //   The spinlock is held by the caller.
  void unlock() PW_UNLOCK_FUNCTION();

  native_handle_type native_handle();

 private:
  // This may be a wrapper around a native type with additional members.
  backend::NativeInterruptSpinLock native_type_;
};

class PW_LOCKABLE("pw::sync::VirtualInterruptSpinLock")
    VirtualInterruptSpinLock final : public VirtualBasicLockable {
 public:
  VirtualInterruptSpinLock() = default;

  VirtualInterruptSpinLock(const VirtualInterruptSpinLock&) = delete;
  VirtualInterruptSpinLock(VirtualInterruptSpinLock&&) = delete;
  VirtualInterruptSpinLock& operator=(const VirtualInterruptSpinLock&) = delete;
  VirtualInterruptSpinLock& operator=(VirtualInterruptSpinLock&&) = delete;

  InterruptSpinLock& interrupt_spin_lock() { return interrupt_spin_lock_; }

 private:
  void DoLockOperation(Operation operation) override
      PW_NO_LOCK_SAFETY_ANALYSIS {
    switch (operation) {
      case Operation::kLock:
        return interrupt_spin_lock_.lock();

      case Operation::kUnlock:
      default:
        return interrupt_spin_lock_.unlock();
    }
  }

  InterruptSpinLock interrupt_spin_lock_;
};

}  // namespace pw::sync

#include "pw_sync_backend/interrupt_spin_lock_inline.h"

using pw_sync_InterruptSpinLock = pw::sync::InterruptSpinLock;

#else  // !defined(__cplusplus)

typedef struct pw_sync_InterruptSpinLock pw_sync_InterruptSpinLock;

#endif  // __cplusplus

PW_EXTERN_C_START

void pw_sync_InterruptSpinLock_Lock(pw_sync_InterruptSpinLock* spin_lock)
    PW_NO_LOCK_SAFETY_ANALYSIS;
bool pw_sync_InterruptSpinLock_TryLock(pw_sync_InterruptSpinLock* spin_lock)
    PW_NO_LOCK_SAFETY_ANALYSIS;
void pw_sync_InterruptSpinLock_Unlock(pw_sync_InterruptSpinLock* spin_lock)
    PW_NO_LOCK_SAFETY_ANALYSIS;

PW_EXTERN_C_END
