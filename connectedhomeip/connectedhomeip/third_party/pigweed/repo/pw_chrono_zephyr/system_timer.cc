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

#include "pw_chrono/system_timer.h"

#include <kernel.h>
#include <sys/mutex.h>

#include <algorithm>

#include "pw_chrono_zephyr/system_clock_constants.h"
#include "pw_chrono_zephyr/system_timer_native.h"

namespace pw::chrono {
namespace {

constexpr SystemClock::duration kMinTimerPeriod = SystemClock::duration(1);
// Work synchronization objects must be in cache-coherent memory, which excludes
// stacks on some architectures.
static k_work_sync work_sync;

}  // namespace

void HandleTimerWork(k_work* item) {
  k_work_delayable* delayable_item = k_work_delayable_from_work(item);
  backend::NativeSystemTimer* native_type =
      CONTAINER_OF(delayable_item, backend::ZephyrWorkWrapper, work)->owner;

  sys_mutex_lock(&native_type->mutex, K_FOREVER);
#ifdef CONFIG_TIMEOUT_64BIT
  native_type->user_callback(native_type->expiry_deadline);
#else
  const SystemClock::duration time_until_deadline =
      native_type->expiry_deadline - SystemClock::now();
  if (time_until_deadline <= SystemClock::duration::zero()) {
    native_type->user_callback(native_type->expiry_deadline);
  } else {
    // We haven't met the deadline yet, reschedule as far out as possible.
    const SystemClock::duration period =
        std::min(pw::chrono::zephyr::kMaxTimeout, time_until_deadline);
    k_work_schedule(&native_type->work_wrapper.work, K_TICKS(period.count()));
  }
#endif  // CONFIG_TIMEOUT_64BIT
  sys_mutex_unlock(&native_type->mutex);
}

SystemTimer::SystemTimer(ExpiryCallback callback)
    : native_type_{.work_wrapper =
                       {
                           .work = {},
                           .owner = nullptr,
                       },
                   .mutex = {},
                   .expiry_deadline = SystemClock::time_point(),
                   .user_callback = std::move(callback)} {
  k_work_init_delayable(&native_type_.work_wrapper.work, HandleTimerWork);
  sys_mutex_init(&native_type_.mutex);
  native_type_.work_wrapper.owner = &native_type_;
}

SystemTimer::~SystemTimer() {
  k_work_cancel_sync(&native_type_.work_wrapper.work, &work_sync);
}

void SystemTimer::InvokeAt(SystemClock::time_point timestamp) {
  sys_mutex_lock(&native_type_.mutex, K_FOREVER);
  native_type_.expiry_deadline = timestamp;

  const SystemClock::duration time_until_deadline =
      timestamp - SystemClock::now();
  const SystemClock::duration period =
      IS_ENABLED(CONFIG_TIMEOUT_64BIT)
          ? time_until_deadline
          : std::clamp(kMinTimerPeriod,
                       time_until_deadline,
                       pw::chrono::zephyr::kMaxTimeout);

  k_work_schedule(&native_type_.work_wrapper.work, K_TICKS(period.count()));
  sys_mutex_unlock(&native_type_.mutex);
}

}  // namespace pw::chrono
