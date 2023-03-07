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

#include "pw_sync/counting_semaphore.h"

using pw::chrono::SystemClock;

extern "C" void pw_sync_CountingSemaphore_Release(
    pw_sync_CountingSemaphore* semaphore) {
  semaphore->release();
}

extern "C" void pw_sync_CountingSemaphore_ReleaseNum(
    pw_sync_CountingSemaphore* semaphore, ptrdiff_t update) {
  semaphore->release(update);
}

extern "C" void pw_sync_CountingSemaphore_Acquire(
    pw_sync_CountingSemaphore* semaphore) {
  semaphore->acquire();
}

extern "C" bool pw_sync_CountingSemaphore_TryAcquire(
    pw_sync_CountingSemaphore* semaphore) {
  return semaphore->try_acquire();
}

extern "C" bool pw_sync_CountingSemaphore_TryAcquireFor(
    pw_sync_CountingSemaphore* semaphore,
    pw_chrono_SystemClock_Duration timeout) {
  return semaphore->try_acquire_for(SystemClock::duration(timeout.ticks));
}

extern "C" bool pw_sync_CountingSemaphore_TryAcquireUntil(
    pw_sync_CountingSemaphore* semaphore,
    pw_chrono_SystemClock_TimePoint deadline) {
  return semaphore->try_acquire_until(SystemClock::time_point(
      SystemClock::duration(deadline.duration_since_epoch.ticks)));
}

extern "C" ptrdiff_t pw_sync_CountingSemaphore_Max(void) {
  return pw::sync::CountingSemaphore::max();
}
