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
#include "pw_sync/counting_semaphore.h"
#include "semphr.h"

namespace pw::sync {

inline CountingSemaphore::CountingSemaphore() : native_type_() {
  const SemaphoreHandle_t handle =
      xSemaphoreCreateCountingStatic(max(), 0, &native_type_);
  // This should never fail since the pointer provided was not null and it
  // should return a pointer to the StaticSemaphore_t.
  PW_DASSERT(handle == reinterpret_cast<SemaphoreHandle_t>(&native_type_));
}

inline CountingSemaphore::~CountingSemaphore() {
  vSemaphoreDelete(reinterpret_cast<SemaphoreHandle_t>(&native_type_));
}

inline void CountingSemaphore::acquire() {
  // Enforce the pw::sync::CountingSemaphore IRQ contract.
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

inline bool CountingSemaphore::try_acquire() noexcept {
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
