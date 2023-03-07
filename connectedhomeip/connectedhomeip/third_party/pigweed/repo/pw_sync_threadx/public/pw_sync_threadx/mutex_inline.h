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

#include "pw_assert/assert.h"
#include "pw_interrupt/context.h"
#include "pw_sync/mutex.h"
#include "tx_api.h"

namespace pw::sync {
namespace backend {

inline constexpr char kMutexName[] = "pw::Mutex";

// Precondition: The mutex must be held when calling this!
inline bool NotRecursivelyHeld(TX_MUTEX& mutex) {
  // Don't use tx_mutex_info_get which goes into a critical section to read
  // this count. Given that this is only called while the mutex is held, nothing
  // can mutate this count and ergo we access the control block directly.
  return mutex.tx_mutex_ownership_count == 1;
}

}  // namespace backend

inline Mutex::Mutex() : native_type_() {
  PW_ASSERT(tx_mutex_create(&native_type_,
                            const_cast<char*>(backend::kMutexName),
                            TX_INHERIT) == TX_SUCCESS);
}

inline Mutex::~Mutex() {
  PW_ASSERT(tx_mutex_delete(&native_type_) == TX_SUCCESS);
}

inline void Mutex::lock() {
  // Enforce the pw::sync::Mutex IRQ contract.
  PW_DASSERT(!interrupt::InInterruptContext());
  PW_ASSERT(tx_mutex_get(&native_type_, TX_WAIT_FOREVER) == TX_SUCCESS);

  // Recursive locking is not supported.
  PW_DASSERT(backend::NotRecursivelyHeld(native_type_));
}

inline bool Mutex::try_lock() {
  // Enforce the pw::sync::Mutex IRQ contract.
  PW_DASSERT(!interrupt::InInterruptContext());
  const UINT result = tx_mutex_get(&native_type_, TX_NO_WAIT);
  if (result == TX_NOT_AVAILABLE) {
    return false;
  }
  PW_ASSERT(result == TX_SUCCESS);
  // Recursive locking is not supported.
  PW_DASSERT(backend::NotRecursivelyHeld(native_type_));
  return true;
}

inline void Mutex::unlock() {
  // Enforce the pw::sync::Mutex IRQ contract.
  PW_DASSERT(!interrupt::InInterruptContext());
  PW_ASSERT(tx_mutex_put(&native_type_) == TX_SUCCESS);
}

inline Mutex::native_handle_type Mutex::native_handle() { return native_type_; }

}  // namespace pw::sync
