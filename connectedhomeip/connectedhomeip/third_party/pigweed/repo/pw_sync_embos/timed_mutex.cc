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

#include "pw_sync/timed_mutex.h"

#include <algorithm>

#include "RTOS.h"
#include "pw_assert/check.h"
#include "pw_chrono/system_clock.h"
#include "pw_chrono_embos/system_clock_constants.h"
#include "pw_interrupt/context.h"

using pw::chrono::SystemClock;

namespace pw::sync {

bool TimedMutex::try_lock_for(SystemClock::duration timeout) {
  // Enforce the pw::sync::TimedMutex IRQ contract.
  PW_DCHECK(!interrupt::InInterruptContext());

  // Use non-blocking try_lock for negative and zero length durations.
  if (timeout <= SystemClock::duration::zero()) {
    return try_lock();
  }

  // In case the timeout is too long for us to express through the native
  // embOS API, we repeatedly wait with shorter durations. Note that on a tick
  // based kernel we cannot tell how far along we are on the current tick, ergo
  // we add one whole tick to the final duration. However, this also means that
  // the loop must ensure that timeout + 1 is less than the max timeout.
  constexpr SystemClock::duration kMaxTimeoutMinusOne =
      pw::chrono::embos::kMaxTimeout - SystemClock::duration(1);
  while (timeout > kMaxTimeoutMinusOne) {
    const int lock_count = OS_UseTimed(
        &native_handle(), static_cast<OS_TIME>(kMaxTimeoutMinusOne.count()));
    if (lock_count != 0) {
      PW_DCHECK_UINT_EQ(1, lock_count, "Recursive locking is not permitted");
      return true;
    }
    timeout -= kMaxTimeoutMinusOne;
  }
  // On a tick based kernel we cannot tell how far along we are on the current
  // tick, ergo we add one whole tick to the final duration.
  const int lock_count =
      OS_UseTimed(&native_handle(), static_cast<OS_TIME>(timeout.count() + 1));
  PW_DCHECK_UINT_LE(1, lock_count, "Recursive locking is not permitted");
  return lock_count == 1;
}

}  // namespace pw::sync
