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

// These tests call the pw_sync module counting_semaphore API from C. The return
// values are checked in the main C++ tests.

#include <stdbool.h>

#include "pw_sync/binary_semaphore.h"

void pw_sync_BinarySemaphore_CallRelease(pw_sync_BinarySemaphore* semaphore) {
  pw_sync_BinarySemaphore_Release(semaphore);
}

void pw_sync_BinarySemaphore_CallAcquire(pw_sync_BinarySemaphore* semaphore) {
  pw_sync_BinarySemaphore_Acquire(semaphore);
}

bool pw_sync_BinarySemaphore_CallTryAcquire(
    pw_sync_BinarySemaphore* semaphore) {
  return pw_sync_BinarySemaphore_TryAcquire(semaphore);
}

bool pw_sync_BinarySemaphore_CallTryAcquireFor(
    pw_sync_BinarySemaphore* semaphore,
    pw_chrono_SystemClock_Duration timeout) {
  return pw_sync_BinarySemaphore_TryAcquireFor(semaphore, timeout);
}

bool pw_sync_BinarySemaphore_CallTryAcquireUntil(
    pw_sync_BinarySemaphore* semaphore,
    pw_chrono_SystemClock_TimePoint deadline) {
  return pw_sync_BinarySemaphore_TryAcquireUntil(semaphore, deadline);
}

ptrdiff_t pw_sync_BinarySemaphore_CallMax(void) {
  return pw_sync_BinarySemaphore_Max();
}
