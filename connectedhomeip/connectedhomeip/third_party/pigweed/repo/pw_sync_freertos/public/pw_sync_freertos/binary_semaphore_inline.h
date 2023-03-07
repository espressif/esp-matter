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

#include "FreeRTOS.h"
#include "pw_assert/assert.h"
#include "pw_chrono/system_clock.h"
#include "pw_chrono_freertos/system_clock_constants.h"
#include "pw_interrupt/context.h"
#include "pw_sync/binary_semaphore.h"
#include "semphr.h"

namespace pw::sync {

inline BinarySemaphore::BinarySemaphore() : native_type_() {
  const SemaphoreHandle_t handle = xSemaphoreCreateBinaryStatic(&native_type_);
  // This should never fail since the pointer provided was not null and it
  // should return a pointer to the StaticSemaphore_t.
  PW_DASSERT(handle == reinterpret_cast<SemaphoreHandle_t>(&native_type_));
}

inline BinarySemaphore::~BinarySemaphore() { vSemaphoreDelete(&native_type_); }

inline void BinarySemaphore::release() {
  if (interrupt::InInterruptContext()) {
    BaseType_t woke_higher_task = pdFALSE;
    // It's perfectly fine if the semaphore already has a count of 1.
    [[maybe_unused]] BaseType_t already_full = xSemaphoreGiveFromISR(
        reinterpret_cast<SemaphoreHandle_t>(&native_type_), &woke_higher_task);
    portYIELD_FROM_ISR(woke_higher_task);
  } else {  // Task context
    // It's perfectly fine if the semaphore already has a count of 1.
    [[maybe_unused]] BaseType_t already_full =
        xSemaphoreGive(reinterpret_cast<SemaphoreHandle_t>(&native_type_));
  }
}

inline void BinarySemaphore::acquire() {
  // Enforce the pw::sync::BinarySemaphore IRQ contract.
  PW_DASSERT(!interrupt::InInterruptContext());
#if INCLUDE_vTaskSuspend == 1  // This means portMAX_DELAY is indefinite.
  const BaseType_t result = xSemaphoreTake(
      reinterpret_cast<SemaphoreHandle_t>(&native_type_), portMAX_DELAY);
  PW_DASSERT(result == pdTRUE);
#else
  // In case we need to block for longer than the FreeRTOS delay can represent
  // repeatedly hit take until success.
  while (xSemaphoreTake(reinterpret_cast<SemaphoreHandle_t>(&native_type_),
                        chrono::freertos::kMaxTimeout.count()) == pdFALSE) {
  }
#endif  // INCLUDE_vTaskSuspend
}

inline bool BinarySemaphore::try_acquire() noexcept {
  if (interrupt::InInterruptContext()) {
    BaseType_t woke_higher_task = pdFALSE;
    const bool success = xSemaphoreTakeFromISR(
                             reinterpret_cast<SemaphoreHandle_t>(&native_type_),
                             &woke_higher_task) == pdTRUE;
    portYIELD_FROM_ISR(woke_higher_task);
    return success;
  }

  // Task Context
  return xSemaphoreTake(reinterpret_cast<SemaphoreHandle_t>(&native_type_),
                        0) == pdTRUE;
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
