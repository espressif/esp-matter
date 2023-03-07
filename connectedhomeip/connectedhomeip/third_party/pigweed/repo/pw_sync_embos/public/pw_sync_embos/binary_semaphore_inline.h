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
#include "pw_sync/binary_semaphore.h"

namespace pw::sync {

inline BinarySemaphore::BinarySemaphore() : native_type_() {
  OS_CreateCSema(&native_type_, 0);
}

inline BinarySemaphore::~BinarySemaphore() { OS_DeleteCSema(&native_type_); }

inline void BinarySemaphore::release() { OS_SignalCSemaMax(&native_type_, 1); }

inline void BinarySemaphore::acquire() {
  // Enforce the pw::sync::BinarySemaphore IRQ contract.
  PW_DASSERT(!interrupt::InInterruptContext());
  OS_WaitCSema(&native_type_);
}

inline bool BinarySemaphore::try_acquire() noexcept {
  return OS_CSemaRequest(&native_type_) != 0;
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
