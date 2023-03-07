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

#include <zephyr/kernel.h>

#include "pw_assert/assert.h"
#include "pw_interrupt/context.h"
#include "pw_sync/mutex.h"

namespace pw::sync {

inline Mutex::Mutex() : native_type_() { k_mutex_init(&native_type_); }

inline Mutex::~Mutex() = default;

inline void Mutex::lock() {
  PW_DASSERT(!interrupt::InInterruptContext());
  k_mutex_lock(&native_type_, K_FOREVER);
  PW_DASSERT(native_type_.lock_count == 1);
}

inline bool Mutex::try_lock() {
  PW_DASSERT(!interrupt::InInterruptContext());

  bool result = k_mutex_lock(&native_type_, K_NO_WAIT) == 0;
  PW_DASSERT(native_type_.lock_count <= 1);

  return result;
}

inline void Mutex::unlock() {
  PW_DASSERT(!interrupt::InInterruptContext());
  PW_ASSERT(k_mutex_unlock(&native_type_) == 0);
}

inline Mutex::native_handle_type Mutex::native_handle() { return native_type_; }

}  // namespace pw::sync
