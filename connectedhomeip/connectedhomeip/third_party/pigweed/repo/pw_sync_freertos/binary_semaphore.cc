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

#include "pw_sync/binary_semaphore.h"

#include <algorithm>

#include "FreeRTOS.h"
#include "pw_assert/check.h"
#include "pw_chrono/system_clock.h"
#include "pw_chrono_freertos/system_clock_constants.h"
#include "pw_interrupt/context.h"
#include "semphr.h"

using pw::chrono::SystemClock;

namespace pw::sync {
namespace {

static_assert(configSUPPORT_STATIC_ALLOCATION != 0,
              "FreeRTOS static allocations are required for this backend.");

}  // namespace

bool BinarySemaphore::try_acquire_for(SystemClock::duration timeout) {
  // Enforce the pw::sync::BinarySemaphore IRQ contract.
  PW_DCHECK(!interrupt::InInterruptContext());

  // Use non-blocking try_acquire for negative and zero length durations.
  if (timeout <= SystemClock::duration::zero()) {
    return try_acquire();
  }

  // In case the timeout is too long for us to express through the native
  // FreeRTOS API, we repeatedly wait with shorter durations. Note that on a
  // tick based kernel we cannot tell how far along we are on the current tick,
  // ergo we add one whole tick to the final duration. However, this also means
  // that the loop must ensure that timeout + 1 is less than the max timeout.
  constexpr SystemClock::duration kMaxTimeoutMinusOne =
      pw::chrono::freertos::kMaxTimeout - SystemClock::duration(1);
  while (timeout > kMaxTimeoutMinusOne) {
    if (xSemaphoreTake(reinterpret_cast<SemaphoreHandle_t>(&native_type_),
                       static_cast<TickType_t>(kMaxTimeoutMinusOne.count())) ==
        pdTRUE) {
      return true;
    }
    timeout -= kMaxTimeoutMinusOne;
  }
  // On a tick based kernel we cannot tell how far along we are on the current
  // tick, ergo we add one whole tick to the final duration.
  return xSemaphoreTake(reinterpret_cast<SemaphoreHandle_t>(&native_type_),
                        static_cast<TickType_t>(timeout.count() + 1)) == pdTRUE;
}

}  // namespace pw::sync
