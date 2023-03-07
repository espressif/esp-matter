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
#include "pw_sync/binary_semaphore.h"

namespace pw::sync {

inline BinarySemaphore::BinarySemaphore()
    : native_type_{.mutex = {}, .condition = {}, .count = 0} {}

inline BinarySemaphore::~BinarySemaphore() {}

inline bool BinarySemaphore::try_acquire_for(
    chrono::SystemClock::duration timeout) {
  // Due to spurious condition variable wakeups we prefer not to use wait_for()
  // as we may grossly extend the effective deadline after a spruious wakeup.
  // Ergo we instead use the derived deadline which can be re-used many times
  // without shifting the effective deadline. For more details on spurious
  // wakeups and CVs on Windows and POSIX see:
  //   https://en.wikipedia.org/wiki/Spurious_wakeup
  return try_acquire_until(chrono::SystemClock::TimePointAfterAtLeast(timeout));
}

inline BinarySemaphore::native_handle_type BinarySemaphore::native_handle() {
  return native_type_;
}

}  // namespace pw::sync
