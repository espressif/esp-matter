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
#pragma once

#include "tx_api.h"

namespace pw::sync::backend {

// This currently does not support SMP as there's no actual lock to spin on
// internally, see the pw_sync_threadx docs for more details.
#ifdef TX_SMP_CORE_ID
#error "This backend does not support SMP versions of ThreadX yet!"
#endif  // defined(TX_SMP_CORE_ID)
struct NativeInterruptSpinLock {
  enum class State {
    kUnlocked = 0,  // This must be 0 to ensure it is bss eligible.
    kLockedFromInterrupt = 1,
    kLockedFromThread = 2,
  };
  State state;  // Used to detect recursion and interrupt context escapes.
  UINT saved_interrupt_mask;
  UINT saved_preemption_threshold;
};
using NativeInterruptSpinLockHandle = NativeInterruptSpinLock&;

}  // namespace pw::sync::backend
