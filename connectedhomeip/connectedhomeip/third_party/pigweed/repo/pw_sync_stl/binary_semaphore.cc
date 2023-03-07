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

#include "pw_sync/binary_semaphore.h"

#include "pw_assert/check.h"

using pw::chrono::SystemClock;

namespace pw::sync {

void BinarySemaphore::release() {
  std::lock_guard lock(native_type_.mutex);
  PW_DCHECK_UINT_LT(native_type_.count, BinarySemaphore::max());
  ++native_type_.count;
  native_type_.condition.notify_one();
}

void BinarySemaphore::acquire() {
  std::unique_lock lock(native_type_.mutex);
  native_type_.condition.wait(lock, [&] { return native_type_.count != 0; });
  native_type_.count = 0;
}

bool BinarySemaphore::try_acquire() noexcept {
  std::lock_guard lock(native_type_.mutex);
  if (native_type_.count != 0) {
    native_type_.count = 0;
    return true;
  }
  return false;
}

bool BinarySemaphore::try_acquire_until(SystemClock::time_point deadline) {
  std::unique_lock lock(native_type_.mutex);
  if (native_type_.condition.wait_until(
          lock, deadline, [&] { return native_type_.count != 0; })) {
    native_type_.count = 0;
    return true;
  }
  return false;
}

}  // namespace pw::sync
