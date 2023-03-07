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

#include "pw_assert/assert.h"
#include "pw_sync/mutex.h"

namespace pw::sync {

inline Mutex::Mutex() : native_type_() {}

inline Mutex::~Mutex() = default;

inline void Mutex::lock() { PW_ASSERT(try_lock()); }

inline bool Mutex::try_lock() {
  return !native_type_.test_and_set(std::memory_order_acquire);
}

inline void Mutex::unlock() {
  PW_ASSERT(native_type_.test_and_set(std::memory_order_acquire));
  native_type_.clear(std::memory_order_release);
}

inline Mutex::native_handle_type Mutex::native_handle() { return native_type_; }

}  // namespace pw::sync
