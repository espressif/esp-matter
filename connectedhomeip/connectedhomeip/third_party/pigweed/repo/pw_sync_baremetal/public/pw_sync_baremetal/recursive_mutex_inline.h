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

#include <limits>

#include "pw_assert/assert.h"
#include "pw_sync/recursive_mutex.h"

namespace pw::sync {

inline RecursiveMutex::RecursiveMutex() : native_type_(0) {}

inline RecursiveMutex::~RecursiveMutex() = default;

inline void RecursiveMutex::lock() {
  try_lock();  // Locking always succeeds
}

inline bool RecursiveMutex::try_lock() {
  int lock_count = native_type_.fetch_add(1, std::memory_order_acquire);
  PW_DASSERT(lock_count != std::numeric_limits<int>::max());  // Detect overflow
  return true;  // No threads, so you can always acquire a recursive mutex.
}

inline void RecursiveMutex::unlock() {
  int lock_count = native_type_.fetch_sub(1, std::memory_order_release);
  PW_ASSERT(lock_count > 0);  // Unlocked mutex that wasn't held
}

inline RecursiveMutex::native_handle_type RecursiveMutex::native_handle() {
  return native_type_;
}

}  // namespace pw::sync
