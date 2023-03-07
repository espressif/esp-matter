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
#pragma once

#include "RTOS.h"
#include "pw_assert/assert.h"
#include "pw_chrono/system_clock.h"
#include "pw_chrono_embos/system_clock_constants.h"
#include "pw_interrupt/context.h"
#include "pw_sync/counting_semaphore.h"

namespace pw::sync {

inline CountingSemaphore::CountingSemaphore() : native_type_() {
  OS_CreateCSema(&native_type_, 0);
}

inline CountingSemaphore::~CountingSemaphore() {
  OS_DeleteCSema(&native_type_);
}

inline void CountingSemaphore::acquire() {
  // Enforce the pw::sync::CountingSemaphore IRQ contract.
  PW_DASSERT(!interrupt::InInterruptContext());
  OS_WaitCSema(&native_type_);
}

inline bool CountingSemaphore::try_acquire() noexcept {
  return OS_CSemaRequest(&native_type_) != 0;
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
