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

#include "pw_chrono/system_clock.h"
#include "pw_sync/timed_thread_notification.h"

namespace pw::sync {

inline bool TimedThreadNotification::try_acquire_for(
    chrono::SystemClock::duration timeout) {
  // Because xTaskNotifyWait may spuriously return pdFALSE due to vTaskSuspend &
  // vTaskResume, a deadline is used instead of a timeout just like FreeRTOS
  // stream buffers.
  return try_acquire_until(chrono::SystemClock::TimePointAfterAtLeast(timeout));
}

}  // namespace pw::sync
