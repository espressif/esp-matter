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

#include "pw_chrono/system_clock.h"

#include <atomic>
#include <chrono>
#include <limits>
#include <mutex>

#include "FreeRTOS.h"
#include "pw_interrupt/context.h"
#include "pw_sync/interrupt_spin_lock.h"
#include "task.h"

namespace pw::chrono::backend {
namespace {

sync::InterruptSpinLock system_clock_interrupt_spin_lock;
int64_t overflow_tick_count = 0;
TickType_t native_tick_count = 0;
static_assert(!SystemClock::is_nmi_safe,
              "global state is not atomic nor double buferred");

// The tick count resets to 0, ergo the overflow count is the max count + 1.
constexpr int64_t kNativeOverflowTickCount =
    static_cast<int64_t>(std::numeric_limits<TickType_t>::max()) + 1;

}  // namespace

int64_t GetSystemClockTickCount() {
  std::lock_guard lock(system_clock_interrupt_spin_lock);
  const TickType_t new_native_tick_count = interrupt::InInterruptContext()
                                               ? xTaskGetTickCountFromISR()
                                               : xTaskGetTickCount();
  // WARNING: This must be called more than once per overflow period!
  if (new_native_tick_count < native_tick_count) {
    // Native tick count overflow detected!
    overflow_tick_count += kNativeOverflowTickCount;
  }
  native_tick_count = new_native_tick_count;
  return overflow_tick_count + native_tick_count;
}

}  // namespace pw::chrono::backend
