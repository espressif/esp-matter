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

#include "pw_sync/binary_semaphore.h"

#include <kernel.h>

namespace pw::sync {

bool BinarySemaphore::try_acquire_for(chrono::SystemClock::duration timeout) {
  // Use non-blocking try_acquire for negative and zero length durations.
  if (timeout <= chrono::SystemClock::duration::zero()) {
    return try_acquire();
  }

#ifndef CONFIG_TIMEOUT_64BIT
  constexpr chrono::SystemClock::duration kMaxTimeoutMinusOne =
      chrono::SystemClock::duration(K_FOREVER.ticks - 1);

  while (timeout > kMaxTimeoutMinusOne) {
    if (k_sem_take(&native_type_, K_TICKS(kMaxTimeoutMinusOne.count())) == 0) {
      return true;
    }
    timeout -= kMaxTimeoutMinusOne;
  }
#endif  // CONFIG_TIMEOUT_64BIT

  return k_sem_take(&native_type_, K_TICKS(timeout.count())) == 0;
}

}  // namespace pw::sync
