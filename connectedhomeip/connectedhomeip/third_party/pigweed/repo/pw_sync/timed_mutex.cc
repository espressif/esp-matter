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

#include "pw_sync/timed_mutex.h"

using pw::chrono::SystemClock;

extern "C" void pw_sync_TimedMutex_Lock(pw_sync_TimedMutex* mutex) {
  mutex->lock();
}

extern "C" bool pw_sync_TimedMutex_TryLock(pw_sync_TimedMutex* mutex) {
  return mutex->try_lock();
}

extern "C" bool pw_sync_TimedMutex_TryLockFor(
    pw_sync_TimedMutex* mutex, pw_chrono_SystemClock_Duration timeout) {
  return mutex->try_lock_for(SystemClock::duration(timeout.ticks));
}

extern "C" bool pw_sync_TimedMutex_TryLockUntil(
    pw_sync_TimedMutex* mutex, pw_chrono_SystemClock_TimePoint deadline) {
  return mutex->try_lock_until(SystemClock::time_point(
      SystemClock::duration(deadline.duration_since_epoch.ticks)));
}

extern "C" void pw_sync_TimedMutex_Unlock(pw_sync_TimedMutex* mutex) {
  mutex->unlock();
}
