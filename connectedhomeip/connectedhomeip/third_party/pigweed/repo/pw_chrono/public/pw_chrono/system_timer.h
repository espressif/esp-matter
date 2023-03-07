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
#include "pw_chrono_backend/system_timer_native.h"
#include "pw_function/function.h"

namespace pw::chrono {

// The SystemTimer allows an ExpiryCallback be executed at a set time in the
// future.
//
// The base SystemTimer only supports a one-shot style timer with a callback.
// A periodic timer can be implemented by rescheduling the timer in the callback
// through InvokeAt(kDesiredPeriod + expired_deadline).
//
// When implementing a periodic layer on top, the user should be mindful of
// handling missed periodic callbacks. They could opt to invoke the callback
// multiple times with the expected expired_deadline values or instead
// saturate and invoke the callback only once with the latest expired_deadline.
//
// The entire API is thread safe, however it is NOT always IRQ safe.
class SystemTimer {
 public:
  using native_handle_type = backend::NativeSystemTimerHandle;

  // The ExpiryCallback is either invoked from a high priority thread or an
  // interrupt.
  //
  // For a given timer instance, its ExpiryCallback will not preempt itself.
  // This makes it appear like there is a single executor of a timer instance's
  // ExpiryCallback.
  //
  // Ergo ExpiryCallbacks should be treated as if they are executed by an
  // interrupt, meaning:
  // - Processing inside of the callback should be kept to a minimum.
  // - Callbacks should never attempt to block.
  // - APIs which are not interrupt safe such as pw::sync::Mutex should not be
  //   used!
  using ExpiryCallback =
      Function<void(SystemClock::time_point expired_deadline)>;

  SystemTimer(ExpiryCallback&& callback);

  // Cancels the timer and blocks if necssary if the callback is already being
  // processed.
  //
  // Postcondition: The expiry callback is not in progress and will not be
  // called in the future.
  ~SystemTimer();

  SystemTimer(const SystemTimer&) = delete;
  SystemTimer(SystemTimer&&) = delete;
  SystemTimer& operator=(const SystemTimer&) = delete;
  SystemTimer& operator=(SystemTimer&&) = delete;

  // Invokes the expiry callback as soon as possible after at least the
  // specified duration.
  //
  // Scheduling a callback cancels the existing callback (if pending).
  // If the callback is already being executed while you reschedule it, it will
  // finish callback execution to completion. You are responsible for any
  // critical section locks which may be needed for timer coordination.
  //
  // This is thread safe, it may not be IRQ safe.
  void InvokeAfter(SystemClock::duration delay);

  // Invokes the expiry callback as soon as possible starting at the specified
  // time_point.
  //
  // Scheduling a callback cancels the existing callback (if pending).
  // If the callback is already being executed while you reschedule it, it will
  // finish callback execution to completion. You are responsible for any
  // critical section locks which may be needed for timer coordination.
  //
  // This is thread safe, it may not be IRQ safe.
  void InvokeAt(SystemClock::time_point timestamp);

  // Cancels the software timer expiry callback if pending.
  //
  // Canceling a timer which isn't scheduled does nothing.
  //
  // If the callback is already being executed while you cancel it, it will
  // finish callback execution to completion. You are responsible for any
  // synchronization which is needed for thread safety.
  //
  // This is thread safe, it may not be IRQ safe.
  void Cancel();

  native_handle_type native_handle();

 private:
  // This may be a wrapper around a native type with additional members.
  backend::NativeSystemTimer native_type_;
};

}  // namespace pw::chrono

#include "pw_chrono_backend/system_timer_inline.h"
