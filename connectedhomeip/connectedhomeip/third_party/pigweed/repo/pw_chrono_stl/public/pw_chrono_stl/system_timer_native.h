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

#include <memory>
#include <mutex>

#include "pw_chrono/system_clock.h"
#include "pw_function/function.h"

namespace pw::chrono::backend {

struct NativeSystemTimer {
  // Instead of using a more complex blocking timer cleanup, a shared_pointer is
  // used so that the heap allocation is still valid for the detached threads
  // even after the NativeSystemTimer has been destructed. Note this is shared
  // with all detached threads.
  struct CallbackContext {
    CallbackContext(
        Function<void(SystemClock::time_point expired_deadline)>&& cb)
        : callback(std::move(cb)) {}

    const Function<void(SystemClock::time_point expired_deadline)> callback;

    // The mutex is used both to ensure the public API is threadsafe and to
    // ensure that only one expiry callback is executed at time.
    // A recurisve mutex is used as the timer callback must be able to invoke
    // its own public API.
    std::recursive_mutex mutex;
  };
  std::shared_ptr<CallbackContext> callback_context;

  // This is only shared with the last active timer if there is one. Note that
  // this is guarded by the callback_context's mutex.
  std::shared_ptr<bool> active_timer_enabled;
};

using NativeSystemTimerHandle = NativeSystemTimer&;

}  // namespace pw::chrono::backend
