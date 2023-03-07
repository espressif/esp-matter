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

// These tests call the pw_sync module interrupt_spin_lock API from C. The
// return values are checked in the main C++ tests.

#include <stdbool.h>

#include "pw_sync/interrupt_spin_lock.h"

void pw_sync_InterruptSpinLock_CallLock(
    pw_sync_InterruptSpinLock* interrupt_spin_lock) {
  pw_sync_InterruptSpinLock_Lock(interrupt_spin_lock);
}

bool pw_sync_InterruptSpinLock_CallTryLock(
    pw_sync_InterruptSpinLock* interrupt_spin_lock) {
  return pw_sync_InterruptSpinLock_TryLock(interrupt_spin_lock);
}

void pw_sync_InterruptSpinLock_CallUnlock(
    pw_sync_InterruptSpinLock* interrupt_spin_lock) {
  pw_sync_InterruptSpinLock_Unlock(interrupt_spin_lock);
}
