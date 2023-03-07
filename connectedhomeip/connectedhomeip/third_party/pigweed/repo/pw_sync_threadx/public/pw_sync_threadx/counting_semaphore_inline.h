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

#include <algorithm>

#include "pw_assert/assert.h"
#include "pw_chrono/system_clock.h"
#include "pw_interrupt/context.h"
#include "pw_sync/counting_semaphore.h"
#include "tx_api.h"

namespace pw::sync {
namespace backend {

inline constexpr char kCountingSemaphoreName[] = "pw::CountingSemaphore";

}  // namespace backend

inline CountingSemaphore::CountingSemaphore() : native_type_() {
  PW_ASSERT(
      tx_semaphore_create(&native_type_,
                          const_cast<char*>(backend::kCountingSemaphoreName),
                          0) == TX_SUCCESS);
}

inline CountingSemaphore::~CountingSemaphore() {
  PW_ASSERT(tx_semaphore_delete(&native_type_) == TX_SUCCESS);
}

inline void CountingSemaphore::release(ptrdiff_t update) {
  for (; update > 0; --update) {
    PW_ASSERT(tx_semaphore_put(&native_type_) == TX_SUCCESS);
  }
}

inline void CountingSemaphore::acquire() {
  // Enforce the pw::sync::CountingSemaphore IRQ contract.
  PW_DASSERT(!interrupt::InInterruptContext());
  PW_ASSERT(tx_semaphore_get(&native_type_, TX_WAIT_FOREVER) == TX_SUCCESS);
}

inline bool CountingSemaphore::try_acquire() noexcept {
  const UINT result = tx_semaphore_get(&native_type_, TX_NO_WAIT);
  if (result == TX_NO_INSTANCE) {
    return false;
  }
  PW_ASSERT(result == TX_SUCCESS);
  return true;
}

inline bool CountingSemaphore::try_acquire_until(
    chrono::SystemClock::time_point deadline) {
  // Note that if this deadline is in the future, it will get rounded up by
  // one whole tick due to how try_acquire_for is implemented.
  return try_acquire_for(deadline - chrono::SystemClock::now());
}

inline CountingSemaphore::native_handle_type
CountingSemaphore::native_handle() {
  return native_type_;
}

}  // namespace pw::sync
