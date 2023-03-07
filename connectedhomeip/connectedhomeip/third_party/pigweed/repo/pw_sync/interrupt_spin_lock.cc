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

#include "pw_sync/interrupt_spin_lock.h"

#include "pw_sync/lock_annotations.h"

extern "C" void pw_sync_InterruptSpinLock_Lock(
    pw_sync_InterruptSpinLock* interrupt_spin_lock) {
  interrupt_spin_lock->lock();
}

extern "C" bool pw_sync_InterruptSpinLock_TryLock(
    pw_sync_InterruptSpinLock* interrupt_spin_lock) {
  return interrupt_spin_lock->try_lock();
}

extern "C" void pw_sync_InterruptSpinLock_Unlock(
    pw_sync_InterruptSpinLock* interrupt_spin_lock) {
  interrupt_spin_lock->unlock();
}
