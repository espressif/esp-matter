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
#include "pw_sync_backend/mutex_native.h"

namespace pw::sync {

// The Mutex is a synchronization primitive that can be used to protect
// shared data from being simultaneously accessed by multiple threads.
// It offers exclusive, non-recursive ownership semantics where priority
// inheritance is used to solve the classic priority-inversion problem.
// This is thread safe, but NOT IRQ safe.
//
// WARNING: In order to support global statically constructed Mutexes, the user
// and/or backend MUST ensure that any initialization required in your
// environment is done prior to the creation and/or initialization of the native
// synchronization primitives (e.g. kernel initialization).
class PW_LOCKABLE("pw::sync::Mutex") Mutex {
 public:
  using native_handle_type = backend::NativeMutexHandle;

  Mutex();
  ~Mutex();
  Mutex(const Mutex&) = delete;
  Mutex(Mutex&&) = delete;
  Mutex& operator=(const Mutex&) = delete;
  Mutex& operator=(Mutex&&) = delete;

  // Locks the mutex, blocking indefinitely. Failures are fatal.
  //
  // PRECONDITION:
  //   The lock isn't already held by this thread. Recursive locking is
  //   undefined behavior.
  void lock() PW_EXCLUSIVE_LOCK_FUNCTION();

  // Attempts to lock the mutex in a non-blocking manner.
  // Returns true if the mutex was successfully acquired.
  //
  // PRECONDITION:
  //   The lock isn't already held by this thread. Recursive locking is
  //   undefined behavior.
  bool try_lock() PW_EXCLUSIVE_TRYLOCK_FUNCTION(true);

  // Unlocks the mutex. Failures are fatal.
  //
  // PRECONDITION:
  //   The mutex is held by this thread.
  void unlock() PW_UNLOCK_FUNCTION();

  native_handle_type native_handle();

 protected:
  // Expose the NativeMutex directly to derived classes (TimedMutex) in
  // case implementations use different types for backend::NativeMutex and
  // native_handle().
  backend::NativeMutex& native_type() { return native_type_; }
  const backend::NativeMutex& native_type() const { return native_type_; }

 private:
  // This may be a wrapper around a native type with additional members.
  backend::NativeMutex native_type_;
};

class PW_LOCKABLE("pw::sync::VirtualMutex") VirtualMutex final
    : public VirtualBasicLockable {
 public:
  VirtualMutex() = default;

  VirtualMutex(const VirtualMutex&) = delete;
  VirtualMutex(VirtualMutex&&) = delete;
  VirtualMutex& operator=(const VirtualMutex&) = delete;
  VirtualMutex& operator=(VirtualMutex&&) = delete;

  Mutex& mutex() { return mutex_; }

 private:
  void DoLockOperation(Operation operation) override
      PW_NO_LOCK_SAFETY_ANALYSIS {
    switch (operation) {
      case Operation::kLock:
        return mutex_.lock();

      case Operation::kUnlock:
      default:
        return mutex_.unlock();
    }
  }

  Mutex mutex_;
};

}  // namespace pw::sync

#include "pw_sync_backend/mutex_inline.h"

using pw_sync_Mutex = pw::sync::Mutex;

#else  // !defined(__cplusplus)

typedef struct pw_sync_Mutex pw_sync_Mutex;

#endif  // __cplusplus

PW_EXTERN_C_START

void pw_sync_Mutex_Lock(pw_sync_Mutex* mutex) PW_NO_LOCK_SAFETY_ANALYSIS;
bool pw_sync_Mutex_TryLock(pw_sync_Mutex* mutex) PW_NO_LOCK_SAFETY_ANALYSIS;
void pw_sync_Mutex_Unlock(pw_sync_Mutex* mutex) PW_NO_LOCK_SAFETY_ANALYSIS;

PW_EXTERN_C_END
