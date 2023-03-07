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
#pragma once

#include "pw_chrono/system_clock.h"
#include "pw_sync/counting_semaphore.h"

namespace pw::sync {

inline CountingSemaphore::CountingSemaphore()
    : native_type_{.mutex = {}, .condition = {}, .count = 0} {}

inline CountingSemaphore::~CountingSemaphore() {}

inline bool CountingSemaphore::try_acquire_for(
    pw::chrono::SystemClock::duration timeout) {
  // Due to spurious condition variable wakeups this cannot use wait_for().
  return try_acquire_until(
      pw::chrono::SystemClock::TimePointAfterAtLeast(timeout));
}

inline CountingSemaphore::native_handle_type
CountingSemaphore::native_handle() {
  return native_type_;
}

}  // namespace pw::sync
