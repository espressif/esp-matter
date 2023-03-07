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

#include "pw_thread/sleep.h"

#include <algorithm>

#include "RTOS.h"
#include "pw_assert/check.h"
#include "pw_chrono/system_clock.h"
#include "pw_chrono_embos/system_clock_constants.h"
#include "pw_thread/id.h"

using pw::chrono::SystemClock;

namespace pw::this_thread {

void sleep_for(chrono::SystemClock::duration sleep_duration) {
  // Ensure we are invoking this from a thread.
  PW_DCHECK(get_id() != thread::Id());

  // Yield for negative and zero length durations.
  if (sleep_duration <= SystemClock::duration::zero()) {
    OS_Yield();
    return;
  }

  // In case the timeout is too long for us to express through the native
  // embOS API, we repeatedly wait with shorter durations. Note that on a tick
  // based kernel we cannot tell how far along we are on the current tick, ergo
  // we add one whole tick to the final duration. However, this also means that
  // the loop must ensure that timeout + 1 is less than the max timeout.
  constexpr SystemClock::duration kMaxTimeoutMinusOne =
      pw::chrono::embos::kMaxTimeout - SystemClock::duration(1);
  while (sleep_duration > kMaxTimeoutMinusOne) {
    OS_Delay(static_cast<OS_TIME>(kMaxTimeoutMinusOne.count()));
    sleep_duration -= kMaxTimeoutMinusOne;
  }
  // On a tick based kernel we cannot tell how far along we are on the current
  // tick, ergo we add one whole tick to the final duration.
  OS_Delay(static_cast<OS_TIME>(sleep_duration.count()) + 1);
}

}  // namespace pw::this_thread
