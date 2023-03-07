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
#include "pw_sync/mutex.h"

namespace pw::sync {

inline bool TimedMutex::try_lock_for(chrono::SystemClock::duration timeout) {
  if (native_handle().try_lock_for(timeout)) {
    native_type().SetLockedState(true);
    return true;
  }
  return false;
}

inline bool TimedMutex::try_lock_until(
    chrono::SystemClock::time_point deadline) {
  if (native_handle().try_lock_until(deadline)) {
    native_type().SetLockedState(true);
    return true;
  }
  return false;
}

}  // namespace pw::sync
