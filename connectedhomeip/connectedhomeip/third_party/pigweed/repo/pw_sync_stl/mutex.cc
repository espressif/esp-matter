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

#include "pw_sync/mutex.h"

#include "pw_assert/check.h"
#include "pw_sync_stl/mutex_native.h"

namespace pw::sync {

Mutex::~Mutex() {
  PW_CHECK(!native_type_.locked, "Mutex was locked when it went out of scope");
}

namespace backend {

void NativeMutex::SetLockedState(bool new_state) {
  PW_CHECK_UINT_NE(locked,
                   new_state,
                   "Called %slock(), but the mutex is already in that state",
                   new_state ? "" : "un");
  locked = new_state;
}

}  // namespace backend
}  // namespace pw::sync
