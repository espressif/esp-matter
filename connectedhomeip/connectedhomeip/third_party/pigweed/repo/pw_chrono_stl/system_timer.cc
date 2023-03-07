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

#include <thread>

namespace pw::chrono {
namespace {

using CallbackContext = backend::NativeSystemTimer::CallbackContext;

void SystemTimerThreadFunction(
    std::shared_ptr<bool> timer_enabled,
    std::shared_ptr<CallbackContext> callback_context,
    SystemClock::time_point expiry_deadline) {
  // Sleep until the requested deadline.
  std::this_thread::sleep_until(expiry_deadline);

  {
    std::lock_guard lock(callback_context->mutex);
    // Only invoke the user's callback if this invocation has not been
    // cancelled.
    if (*timer_enabled) {
      (callback_context->callback)(expiry_deadline);
    }
  }
}

}  // namespace

void SystemTimer::InvokeAt(SystemClock::time_point timestamp) {
  std::lock_guard lock(native_type_.callback_context->mutex);

  // First, cancel any outstanding requests.
  if (native_type_.active_timer_enabled) {
    *native_type_.active_timer_enabled = false;
  }

  // Second, active another detached timer thread with a new shared atomic bool.
  native_type_.active_timer_enabled = std::make_shared<bool>(true);
  std::thread(SystemTimerThreadFunction,
              native_type_.active_timer_enabled,
              native_type_.callback_context,
              timestamp)
      .detach();
}

void SystemTimer::Cancel() {
  std::lock_guard lock(native_type_.callback_context->mutex);

  if (native_type_.active_timer_enabled) {
    *native_type_.active_timer_enabled = false;
  }
}

}  // namespace pw::chrono
