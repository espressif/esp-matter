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
#pragma once

#include <stdbool.h>

#include "pw_preprocessor/util.h"
#include "pw_sync/lock_annotations.h"

#ifdef __cplusplus

#include "pw_sync_backend/recursive_mutex_native.h"

namespace pw::sync {

// WARNING: Recursive mutexes are problematic and their use is discouraged. DO
// NOT use this class without consulting with the Pigweed team first.
//
// RecursiveMutex is the same as Mutex, except that the thread holding the mutex
// may safely attempt to lock the mutex again.
// This is thread safe, but NOT IRQ safe.
//
// Note that RecursiveMutex's lock safety annotations the same as Mutex's, so
// lock safety analysis will not allow for recursive locking. Many cases where
// recursive locking is needed cannot be checked with lock safety analysis
// anyway (e.g. callbacks). For cases where lock safety analysis complains
// about safe usage, disable it locally.
//
// WARNING: In order to support global statically constructed RecusiveMutexes,
// the user and/or backend MUST ensure that any initialization required in your
// environment is done prior to the creation and/or initialization of the native
// synchronization primitives (e.g. kernel initialization).
class PW_LOCKABLE("pw::sync::RecursiveMutex") RecursiveMutex {
 public:
  using native_handle_type = backend::NativeRecursiveMutexHandle;

  RecursiveMutex();
  ~RecursiveMutex();

  RecursiveMutex(const RecursiveMutex&) = delete;
  RecursiveMutex(RecursiveMutex&&) = delete;

  RecursiveMutex& operator=(const RecursiveMutex&) = delete;
  RecursiveMutex& operator=(RecursiveMutex&&) = delete;

  // Locks the mutex, blocking indefinitely. Failures are fatal.
  void lock() PW_EXCLUSIVE_LOCK_FUNCTION();

  // Attempts to lock the mutex in a non-blocking manner.
  // Returns true if the mutex was successfully acquired.
  bool try_lock() PW_EXCLUSIVE_TRYLOCK_FUNCTION(true);

  // Unlocks the mutex. Failures are fatal.
  void unlock() PW_UNLOCK_FUNCTION();

  native_handle_type native_handle();

 private:
  // This may be a wrapper around a native type with additional members.
  backend::NativeRecursiveMutex native_type_;
};

}  // namespace pw::sync

#include "pw_sync_backend/recursive_mutex_inline.h"

using pw_sync_RecursiveMutex = pw::sync::RecursiveMutex;

#else  // !defined(__cplusplus)

typedef struct pw_sync_RecursiveMutex pw_sync_RecursiveMutex;

#endif  // __cplusplus

PW_EXTERN_C_START

void pw_sync_RecursiveMutex_Lock(pw_sync_RecursiveMutex* mutex)
    PW_NO_LOCK_SAFETY_ANALYSIS;
bool pw_sync_RecursiveMutex_TryLock(pw_sync_RecursiveMutex* mutex)
    PW_NO_LOCK_SAFETY_ANALYSIS;
void pw_sync_RecursiveMutex_Unlock(pw_sync_RecursiveMutex* mutex)
    PW_NO_LOCK_SAFETY_ANALYSIS;

PW_EXTERN_C_END
