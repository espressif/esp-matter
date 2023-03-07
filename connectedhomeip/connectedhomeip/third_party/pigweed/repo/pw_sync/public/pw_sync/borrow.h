// Copyright 2021 The Pigweed Authors
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
#include <optional>
#include <type_traits>

#include "pw_assert/assert.h"
#include "pw_sync/lock_annotations.h"
#include "pw_sync/virtual_basic_lockable.h"

namespace pw::sync {

// The BorrowedPointer is an RAII handle which wraps a pointer to a borrowed
// object along with a held lock which is guarding the object. When destroyed,
// the lock is released.
template <typename GuardedType, typename Lock = pw::sync::VirtualBasicLockable>
class BorrowedPointer {
 public:
  // Release the lock on destruction.
  ~BorrowedPointer() {
    if (lock_ != nullptr) {
      lock_->unlock();
    }
  }

  // This object is moveable, but not copyable.
  //
  // Postcondition: The other BorrowedPointer is no longer valid and will assert
  //     if the GuardedType is accessed.
  BorrowedPointer(BorrowedPointer&& other)
      : lock_(other.lock_), object_(other.object_) {
    other.lock_ = nullptr;
    other.object_ = nullptr;
  }
  BorrowedPointer& operator=(BorrowedPointer&& other) {
    lock_ = other.lock_;
    object_ = other.object_;
    other.lock_ = nullptr;
    other.object_ = nullptr;
    return *this;
  }
  BorrowedPointer(const BorrowedPointer&) = delete;
  BorrowedPointer& operator=(const BorrowedPointer&) = delete;

  // Provides access to the borrowed object's members.
  GuardedType* operator->() {
    PW_ASSERT(object_ != nullptr);  // Ensure this isn't a stale moved instance.
    return object_;
  }

  // Provides access to the borrowed object directly.
  //
  // NOTE: The member of pointer member access operator, operator->(), is
  // recommended over this API as this is prone to leaking references. However,
  // this is sometimes necessary.
  //
  // WARNING: Be careful not to leak references to the borrowed object!
  GuardedType& operator*() {
    PW_ASSERT(object_ != nullptr);  // Ensure this isn't a stale moved instance.
    return *object_;
  }

 private:
  // Allow BorrowedPointer creation inside of Borrowable's acquire methods.
  template <typename G, typename L>
  friend class Borrowable;

  constexpr BorrowedPointer(Lock& lock, GuardedType& object)
      : lock_(&lock), object_(&object) {}

  Lock* lock_;
  GuardedType* object_;
};

// The Borrowable is a helper construct that enables callers to borrow an object
// which is guarded by a lock.
//
// Users who need access to the guarded object can ask to acquire a
// BorrowedPointer which permits access while the lock is held.
//
// This class is compatible with locks which comply with BasicLockable,
// Lockable, and TimedLockable C++ named requirements.
template <typename GuardedType, typename Lock = pw::sync::VirtualBasicLockable>
class Borrowable {
 public:
  constexpr Borrowable(GuardedType& object, Lock& lock) noexcept
      : lock_(&lock), object_(&object) {}

  Borrowable(const Borrowable&) = default;
  Borrowable& operator=(const Borrowable&) = default;
  Borrowable(Borrowable&& other) = default;
  Borrowable& operator=(Borrowable&& other) = default;

  // Blocks indefinitely until the object can be borrowed. Failures are fatal.
  BorrowedPointer<GuardedType, Lock> acquire() PW_NO_LOCK_SAFETY_ANALYSIS {
    lock_->lock();
    return BorrowedPointer<GuardedType, Lock>(*lock_, *object_);
  }

  // Tries to borrow the object in a non-blocking manner. Returns a
  // BorrowedPointer on success, otherwise std::nullopt (nothing).
  std::optional<BorrowedPointer<GuardedType, Lock>> try_acquire() {
    if (!lock_->try_lock()) {
      return std::nullopt;
    }
    return BorrowedPointer<GuardedType, Lock>(*lock_, *object_);
  }

  // Tries to borrow the object. Blocks until the specified timeout has elapsed
  // or the object has been borrowed, whichever comes first. Returns a
  // BorrowedPointer on success, otherwise std::nullopt (nothing).
  template <class Rep, class Period>
  std::optional<BorrowedPointer<GuardedType, Lock>> try_acquire_for(
      std::chrono::duration<Rep, Period> timeout) {
    if (!lock_->try_lock_for(timeout)) {
      return std::nullopt;
    }
    return BorrowedPointer<GuardedType, Lock>(*lock_, *object_);
  }

  // Tries to borrow the object. Blocks until the specified deadline has passed
  // or the object has been borrowed, whichever comes first. Returns a
  // BorrowedPointer on success, otherwise std::nullopt (nothing).
  template <class Clock, class Duration>
  std::optional<BorrowedPointer<GuardedType, Lock>> try_acquire_until(
      std::chrono::time_point<Clock, Duration> deadline) {
    if (!lock_->try_lock_until(deadline)) {
      return std::nullopt;
    }
    return BorrowedPointer<GuardedType, Lock>(*lock_, *object_);
  }

 private:
  Lock* lock_;
  GuardedType* object_;
};

}  // namespace pw::sync
