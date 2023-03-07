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

#include <algorithm>
#include <mutex>

#include "FreeRTOS.h"
#include "pw_assert/check.h"
#include "pw_chrono_freertos/system_clock_constants.h"
#include "task.h"
#include "timers.h"

namespace pw::chrono {
namespace {

using State = backend::NativeSystemTimer::State;

// Instead of adding targeted locks to each instance, simply use the global
// scheduler critical section lock.
class SchedulerLock {
 public:
  static void lock() { vTaskSuspendAll(); }
  static void unlock() { xTaskResumeAll(); }
};
SchedulerLock global_timer_lock;

void HandleTimerCallback(TimerHandle_t timer_handle) {
  // The FreeRTOS timer service is always handled by a thread, ergo to ensure
  // this API is threadsafe we simply disable task switching.
  std::unique_lock lock(global_timer_lock);

  // Because the timer control block, AKA what the timer handle points at, is
  // the first member of the NativeSystemTimer struct we play a trick to cheaply
  // get the native handle reference.
  backend::NativeSystemTimer& native_type =
      *reinterpret_cast<backend::NativeSystemTimer*>(timer_handle);

  PW_CHECK_UINT_EQ(xTimerIsTimerActive(timer_handle),
                   pdFALSE,
                   "The timer is still active while being executed");

  if (native_type.state == State::kCancelled) {
    // Do nothing, we were invoked while the stop command was in the queue.
    return;
  }

  const SystemClock::duration time_until_deadline =
      native_type.expiry_deadline - SystemClock::now();
  if (time_until_deadline <= SystemClock::duration::zero()) {
    // We have met the deadline, cancel the current state and execute the user's
    // callback. Note we cannot update the state later as the user's callback
    // may alter the desired state through the Invoke*() API.
    native_type.state = State::kCancelled;

    // Release the scheduler lock once we won't modify native_state any further.
    lock.unlock();
    native_type.user_callback(native_type.expiry_deadline);
    return;
  }

  // We haven't met the deadline yet, reschedule as far out as possible.
  // Note that this must be > SystemClock::duration::zero() based on the
  // conditional above.
  const SystemClock::duration period =
      std::min(pw::chrono::freertos::kMaxTimeout, time_until_deadline);
  PW_CHECK_UINT_EQ(
      xTimerChangePeriod(reinterpret_cast<TimerHandle_t>(&native_type.tcb),
                         static_cast<TickType_t>(period.count()),
                         0),
      pdPASS,
      "Timer command queue overflowed");
  PW_CHECK_UINT_EQ(
      xTimerStart(reinterpret_cast<TimerHandle_t>(&native_type.tcb), 0),
      pdPASS,
      "Timer command queue overflowed");
}

// FreeRTOS requires a timer to have a non-zero period.
constexpr SystemClock::duration kMinTimerPeriod = SystemClock::duration(1);
constexpr TickType_t kInvalidPeriod = kMinTimerPeriod.count();
constexpr UBaseType_t kOneShotMode = pdFALSE;  // Do not use auto reload.

}  // namespace

#if configUSE_TIMERS != 1
#error \
    "Backend requires your FreeRTOS configuration to have configUSE_TIMERS == 1"
#endif

#if configSUPPORT_STATIC_ALLOCATION != 1
#error \
    "Backend requires your FreeRTOS configuration to have configSUPPORT_STATIC_ALLOCATION == 1"
#endif

SystemTimer::SystemTimer(ExpiryCallback&& callback)
    : native_type_{.tcb{},
                   .state = State::kCancelled,
                   .expiry_deadline = SystemClock::time_point(),
                   .user_callback = std::move(callback)} {
  // Note that timer "creation" is not enqueued through the command queue and
  // is ergo safe to do before the scheduler is running.
  const TimerHandle_t handle =
      xTimerCreateStatic("",  // "pw::chrono::SystemTimer",
                         kInvalidPeriod,
                         kOneShotMode,
                         this,
                         HandleTimerCallback,
                         &native_type_.tcb);

  // This should never fail since the pointer provided was not null and it
  // should return a pointer to the StaticTimer_t.
  PW_DCHECK_PTR_EQ(handle, reinterpret_cast<TimerHandle_t>(&native_type_.tcb));
}

SystemTimer::~SystemTimer() {
  Cancel();

  // WARNING: This enqueues the request to delete the timer through a queue, it
  // does not synchronously delete and disable the timer here! This means that
  // if the timer is about to expire and the timer service thread is a lower
  // priority that it may use the native_type_ after it is free'd.
  PW_CHECK_UINT_EQ(
      pdPASS,
      xTimerDelete(reinterpret_cast<TimerHandle_t>(&native_type_.tcb), 0),
      "Timer command queue overflowed");

  // In case the timer is still active as warned above, busy yield loop until it
  // has been removed. Note that this is safe before the scheduler has been
  // started because the timer cannot have been added to the queue yet and ergo
  // it shouldn't attempt to yield.
  while (
      xTimerIsTimerActive(reinterpret_cast<TimerHandle_t>(&native_type_.tcb))) {
    taskYIELD();
  }
}

void SystemTimer::InvokeAt(SystemClock::time_point timestamp) {
  // The FreeRTOS timer service is always handled by a thread, ergo to ensure
  // this API is threadsafe we simply disable task switching.
  std::lock_guard lock(global_timer_lock);

  // We don't want to call Cancel which would enqueue a stop command instead of
  // synchronously updating the state. Instead we update the expiry deadline
  // and update the state where the one shot only fires if the expiry deadline
  // is exceeded and the callback is executed once.
  native_type_.expiry_deadline = timestamp;

  // Schedule the timer as far out as possible. Note that the timeout might be
  // clamped and it may be rescheduled internally.
  const SystemClock::duration time_until_deadline =
      timestamp - SystemClock::now();
  const SystemClock::duration period = std::clamp(
      kMinTimerPeriod, time_until_deadline, pw::chrono::freertos::kMaxTimeout);

  PW_CHECK_UINT_EQ(
      xTimerChangePeriod(reinterpret_cast<TimerHandle_t>(&native_type_.tcb),
                         static_cast<TickType_t>(period.count()),
                         0),
      pdPASS,
      "Timer command queue overflowed");

  // Don't enqueue the start multiple times, schedule it once and let the
  // callback cancel.
  if (native_type_.state == State::kCancelled) {
    PW_CHECK_UINT_EQ(
        xTimerStart(reinterpret_cast<TimerHandle_t>(&native_type_.tcb), 0),
        pdPASS,
        "Timer command queue overflowed");
    native_type_.state = State::kScheduled;
  }
}

void SystemTimer::Cancel() {
  // The FreeRTOS timer service is always handled by a thread, ergo to ensure
  // this API is threadsafe we simply disable task switching.
  std::lock_guard lock(global_timer_lock);

  // The stop command may not be executed until later in case we're in a
  // critical section. For this reason update the internal state in case the
  // callback gets invoked.
  //
  // Note that xTimerIsTimerActive cannot be used here as the timer service
  // daemon may be a lower priority and ergo may still execute the callback
  // after Cancel() was invoked. This is because a single expired timer may be
  // processed before the entire command queue is emptied.
  native_type_.state = State::kCancelled;

  PW_CHECK_UINT_EQ(
      xTimerStop(reinterpret_cast<TimerHandle_t>(&native_type_.tcb), 0),
      pdPASS,
      "Timer command queue overflowed");
}

}  // namespace pw::chrono
