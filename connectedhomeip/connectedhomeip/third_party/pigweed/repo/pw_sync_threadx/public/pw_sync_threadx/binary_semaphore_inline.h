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
#include "pw_chrono/system_clock.h"
#include "pw_interrupt/context.h"
#include "pw_sync/binary_semaphore.h"
#include "tx_api.h"

namespace pw::sync {
namespace backend {

inline constexpr char kBinarySemaphoreName[] = "pw::BinarySemaphore";

}  // namespace backend

inline BinarySemaphore::BinarySemaphore() : native_type_() {
  PW_ASSERT(
      tx_semaphore_create(&native_type_,
                          const_cast<char*>(backend::kBinarySemaphoreName),
                          0) == TX_SUCCESS);
}

inline BinarySemaphore::~BinarySemaphore() {
  PW_ASSERT(tx_semaphore_delete(&native_type_) == TX_SUCCESS);
}

inline void BinarySemaphore::release() {
  // Give at most 1 token.
  const UINT result = tx_semaphore_ceiling_put(&native_type_, 1);
  PW_ASSERT(result == TX_SUCCESS || result == TX_CEILING_EXCEEDED);
}

inline void BinarySemaphore::acquire() {
  // Enforce the pw::sync::BinarySemaphore IRQ contract.
  PW_DASSERT(!interrupt::InInterruptContext());
  const UINT result = tx_semaphore_get(&native_type_, TX_WAIT_FOREVER);
  PW_ASSERT(result == TX_SUCCESS);
}

inline bool BinarySemaphore::try_acquire() noexcept {
  const UINT result = tx_semaphore_get(&native_type_, TX_NO_WAIT);
  if (result == TX_NO_INSTANCE) {
    return false;
  }
  PW_ASSERT(result == TX_SUCCESS);
  return true;
}

inline bool BinarySemaphore::try_acquire_until(
    chrono::SystemClock::time_point deadline) {
  // Note that if this deadline is in the future, it will get rounded up by
  // one whole tick due to how try_acquire_for is implemented.
  return try_acquire_for(deadline - chrono::SystemClock::now());
}

inline BinarySemaphore::native_handle_type BinarySemaphore::native_handle() {
  return native_type_;
}

}  // namespace pw::sync
