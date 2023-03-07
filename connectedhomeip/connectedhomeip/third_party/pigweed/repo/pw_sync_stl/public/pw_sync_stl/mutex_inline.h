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

#include "pw_sync/mutex.h"

namespace pw::sync {

inline Mutex::Mutex() : native_type_() {}

inline void Mutex::lock() {
  native_handle().lock();
  native_type_.SetLockedState(true);
}

inline bool Mutex::try_lock() {
  if (native_handle().try_lock()) {
    native_type_.SetLockedState(true);
    return true;
  }
  return false;
}

inline void Mutex::unlock() {
  native_type_.SetLockedState(false);
  native_handle().unlock();
}

// Return a std::timed_mutex instead of the customized NativeMutex class.
inline Mutex::native_handle_type Mutex::native_handle() {
  return native_type_.mutex;
}

}  // namespace pw::sync
