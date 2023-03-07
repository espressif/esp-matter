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

// These tests call the pw_sync module mutex API from C. The return values are
// checked in the main C++ tests.

#include <stdbool.h>

#include "pw_sync/timed_mutex.h"

void pw_sync_TimedMutex_CallLock(pw_sync_TimedMutex* mutex) {
  pw_sync_TimedMutex_Lock(mutex);
}

bool pw_sync_TimedMutex_CallTryLock(pw_sync_TimedMutex* mutex) {
  return pw_sync_TimedMutex_TryLock(mutex);
}

bool pw_sync_TimedMutex_CallTryLockFor(pw_sync_TimedMutex* mutex,
                                       pw_chrono_SystemClock_Duration timeout) {
  return pw_sync_TimedMutex_TryLockFor(mutex, timeout);
}

bool pw_sync_TimedMutex_CallTryLockUntil(
    pw_sync_TimedMutex* mutex, pw_chrono_SystemClock_TimePoint deadline) {
  return pw_sync_TimedMutex_TryLockUntil(mutex, deadline);
}

void pw_sync_TimedMutex_CallUnlock(pw_sync_TimedMutex* mutex) {
  pw_sync_TimedMutex_Unlock(mutex);
}
