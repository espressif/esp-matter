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

#include "pw_sync/interrupt_spin_lock.h"

#include "pw_assert/check.h"
#include "pw_interrupt/context.h"
#include "task.h"

namespace pw::sync {

#if (INCLUDE_xTaskGetSchedulerState != 1) && (configUSE_TIMERS != 1)
#error "xTaskGetScheduler is required for pw_sync_freertos:interrupt_spin_lock"
#endif

void InterruptSpinLock::lock() {
  if (interrupt::InInterruptContext()) {
    native_type_.saved_interrupt_mask = taskENTER_CRITICAL_FROM_ISR();
  } else {  // Task context
    // Suspending the scheduler ensures that kernel API calls that occur
    // within the critical section will not preempt the current task
    // (if called from a thread context).  Otherwise, kernel APIs called
    // from within the critical section may preempt the running task if
    // the port implements portYIELD synchronously.
    // Note: calls to vTaskSuspendAll(), like taskENTER_CRITICAL() can
    // be nested.
    // Note: vTaskSuspendAll()/xTaskResumeAll() are not safe to call before the
    // scheduler has been started.
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
      vTaskSuspendAll();
    }
    taskENTER_CRITICAL();
  }
  // We can't deadlock here so crash instead.
  PW_DCHECK(!native_type_.locked,
            "Recursive InterruptSpinLock::lock() detected");
  native_type_.locked = true;
}

void InterruptSpinLock::unlock() {
  native_type_.locked = false;
  if (interrupt::InInterruptContext()) {
    taskEXIT_CRITICAL_FROM_ISR(native_type_.saved_interrupt_mask);
  } else {  // Task context
    taskEXIT_CRITICAL();
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
      xTaskResumeAll();
    }
  }
}

}  // namespace pw::sync
