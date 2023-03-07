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

#include "pw_chrono/system_clock.h"

#include <atomic>
#include <chrono>
#include <limits>
#include <mutex>

#include "RTOS.h"
#include "pw_sync/interrupt_spin_lock.h"

namespace pw::chrono::backend {
namespace {

sync::InterruptSpinLock system_clock_interrupt_spin_lock;
int64_t overflow_tick_count = 0;
uint32_t native_tick_count = 0;
static_assert(!SystemClock::is_nmi_safe,
              "global state is not atomic nor double buferred");

static_assert(sizeof(void*) == 4, "this backend only supports 32 bit targets!");

inline uint32_t GetUint32TickCount() {
  // embOS returns a signed 32 bit value, however according to their developers
  // the binary value continues to increment like an unsigned value, ergo we
  // instead reinterpret the tick count as the raw underlying 32 bit unsigned
  // tick count.
  return static_cast<uint32_t>(OS_GetTime32());
}

// The tick count resets to 0, ergo the overflow count is the max count + 1.
constexpr int64_t kNativeOverflowTickCount =
    static_cast<int64_t>(std::numeric_limits<uint32_t>::max()) + 1;

}  // namespace

int64_t GetSystemClockTickCount() {
  std::lock_guard lock(system_clock_interrupt_spin_lock);
  const uint32_t new_native_tick_count = GetUint32TickCount();
  // WARNING: This must be called more than once per overflow period!
  if (new_native_tick_count < native_tick_count) {
    // Native tick count overflow detected!
    overflow_tick_count += kNativeOverflowTickCount;
  }
  native_tick_count = new_native_tick_count;
  return overflow_tick_count + native_tick_count;
}

}  // namespace pw::chrono::backend
