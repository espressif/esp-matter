// Copyright 2022 The Pigweed Authors
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

#include <thread>

#include "pw_assert/assert.h"

namespace pw::sync {

inline RecursiveMutex::RecursiveMutex() = default;

inline RecursiveMutex::~RecursiveMutex() {
  PW_ASSERT(native_type_.lock_count == 0u);
}

inline void RecursiveMutex::lock() {
  native_handle().lock();
  native_type_.lock_count += 1;
}

inline bool RecursiveMutex::try_lock() {
  if (native_handle().try_lock()) {
    native_type_.lock_count += 1;
    return true;
  }
  return false;
}

inline void RecursiveMutex::unlock() {
  PW_ASSERT(native_type_.lock_count > 0u);
  native_type_.lock_count -= 1;
  native_handle().unlock();
}

// Return a std::recursive_mutex instead of the customized class.
inline RecursiveMutex::native_handle_type RecursiveMutex::native_handle() {
  return native_type_.mutex;
}

}  // namespace pw::sync
