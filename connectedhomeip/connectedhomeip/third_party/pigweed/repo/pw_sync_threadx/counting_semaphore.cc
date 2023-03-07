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

#include "pw_sync/counting_semaphore.h"

#include <algorithm>

#include "pw_assert/check.h"
#include "pw_chrono/system_clock.h"
#include "pw_chrono_threadx/system_clock_constants.h"
#include "pw_interrupt/context.h"
#include "tx_api.h"

using pw::chrono::SystemClock;

namespace pw::sync {

bool CountingSemaphore::try_acquire_for(SystemClock::duration timeout) {
  // Enforce the pw::sync::CountingSemaphore IRQ contract.
  PW_DCHECK(!interrupt::InInterruptContext());

  // Use non-blocking try_acquire for negative and zero length durations.
  if (timeout <= SystemClock::duration::zero()) {
    return try_acquire();
  }

  // In case the timeout is too long for us to express through the native
  // ThreadX API, we repeatedly wait with shorter durations. Note that on a tick
  // based kernel we cannot tell how far along we are on the current tick, ergo
  // we add one whole tick to the final duration. However, this also means that
  // the loop must ensure that timeout + 1 is less than the max timeout.
  constexpr SystemClock::duration kMaxTimeoutMinusOne =
      pw::chrono::threadx::kMaxTimeout - SystemClock::duration(1);
  while (timeout > kMaxTimeoutMinusOne) {
    const UINT result = tx_semaphore_get(
        &native_type_, static_cast<ULONG>(kMaxTimeoutMinusOne.count()));
    if (result != TX_NO_INSTANCE) {
      // If we didn't time out (TX_NO_INSTANCE), then we should have succeeded.
      PW_CHECK_UINT_EQ(TX_SUCCESS, result);
      return true;
    }
    timeout -= kMaxTimeoutMinusOne;
  }
  // On a tick based kernel we cannot tell how far along we are on the current
  // tick, ergo we add one whole tick to the final duration.
  const UINT result =
      tx_semaphore_get(&native_type_, static_cast<ULONG>(timeout.count() + 1));
  if (result == TX_NO_INSTANCE) {
    return false;  // We timed out, there's still no token.
  }
  PW_CHECK_UINT_EQ(TX_SUCCESS, result);
  return true;
}

}  // namespace pw::sync
