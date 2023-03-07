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

#include "pw_sync/timed_thread_notification.h"

#include <algorithm>

#include "FreeRTOS.h"
#include "pw_assert/check.h"
#include "pw_chrono/system_clock.h"
#include "pw_chrono_freertos/system_clock_constants.h"
#include "pw_interrupt/context.h"
#include "pw_sync_freertos/config.h"
#include "task.h"

using pw::chrono::SystemClock;

namespace pw::sync {
namespace {

BaseType_t WaitForNotification(TickType_t xTicksToWait) {
#ifdef configTASK_NOTIFICATION_ARRAY_ENTRIES
  return xTaskNotifyWaitIndexed(
      pw::sync::freertos::config::kThreadNotificationIndex,
      0,        // Clear no bits on entry.
      0,        // Clear no bits on exit.
      nullptr,  // Don't care about the notification value.
      xTicksToWait);
#else   // !configTASK_NOTIFICATION_ARRAY_ENTRIES
  return xTaskNotifyWait(0,        // Clear no bits on entry.
                         0,        // Clear no bits on exit.
                         nullptr,  // Don't care about the notification value.
                         xTicksToWait);
#endif  // configTASK_NOTIFICATION_ARRAY_ENTRIES
}

}  // namespace

bool TimedThreadNotification::try_acquire_until(
    const SystemClock::time_point deadline) {
  // Enforce the pw::sync::TImedThreadNotification IRQ contract.
  PW_DCHECK(!interrupt::InInterruptContext());

  // Enforce that only a single thread can block at a time.
  PW_DCHECK(native_handle().blocked_thread == nullptr);

  // Ensure that no one forgot to clean up nor corrupted the task notification
  // state in the TCB.
  PW_DCHECK(xTaskNotifyStateClear(nullptr) == pdFALSE);

  {
    std::lock_guard lock(native_handle().shared_spin_lock);
    const bool notified = native_handle().notified;
    // Don't block if we've already reached the specified deadline time.
    if (notified || (SystemClock::now() >= deadline)) {
      native_handle().notified = false;
      return notified;
    }
    // Not notified yet, set the task handle for a one-time notification.
    native_handle().blocked_thread = xTaskGetCurrentTaskHandle();
  }

  // xTaskNotifyWait may spuriously return pdFALSE due to vTaskSuspend &
  // vTaskResume. Ergo, loop until we have been notified or the specified
  // deadline time has been reached (whichever comes first).
  for (SystemClock::time_point now = SystemClock::now(); now < deadline;
       now = SystemClock::now()) {
    // Note that this must be greater than zero, due to the condition above.
    const SystemClock::duration timeout =
        std::min(deadline - now, pw::chrono::freertos::kMaxTimeout);
    if (WaitForNotification(static_cast<TickType_t>(timeout.count())) ==
        pdTRUE) {
      break;  // We were notified!
    }
  }

  std::lock_guard lock(native_handle().shared_spin_lock);
  // We need to clear the thread notification state in case we were
  // notified after timing out but before entering this critical section.
#ifdef configTASK_NOTIFICATION_ARRAY_ENTRIES
  xTaskNotifyStateClearIndexed(
      nullptr, pw::sync::freertos::config::kThreadNotificationIndex);
#else   // !configTASK_NOTIFICATION_ARRAY_ENTRIES
  xTaskNotifyStateClear(nullptr);
#endif  // configTASK_NOTIFICATION_ARRAY_ENTRIES
  // Instead of determining whether we were notified above while blocking in
  // the loop above, we instead read it in this subsequent critical section in
  // order to also include notifications which arrived after we timed out but
  // before we entered this critical section.
  const bool notified = native_handle().notified;
  native_handle().notified = false;
  native_handle().blocked_thread = nullptr;
  return notified;
}

}  // namespace pw::sync
