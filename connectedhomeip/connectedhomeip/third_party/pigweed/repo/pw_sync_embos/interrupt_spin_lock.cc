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

#include "pw_sync/interrupt_spin_lock.h"

#include "RTOS.h"
#include "pw_assert/check.h"

namespace pw::sync {

void InterruptSpinLock::lock() {
  // Mask interrupts.
  OS_IncDI();

  // Disable task switching to ensure kernel APIs cannot switch to other tasks
  // which could then end up deadlocking recursively on this same lock.
  OS_SuspendAllTasks();

  // We can't deadlock here so crash instead.
  PW_DCHECK(!native_type_.locked,
            "Recursive InterruptSpinLock::lock() detected");
  native_type_.locked = true;
}

void InterruptSpinLock::unlock() {
  native_type_.locked = false;
  OS_ResumeAllSuspendedTasks();  // Restore task switching.
  OS_DecRI();                    // Restore interrupts.
}

}  // namespace pw::sync
