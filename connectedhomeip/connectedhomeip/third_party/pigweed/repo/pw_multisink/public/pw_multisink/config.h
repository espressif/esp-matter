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
#pragma once

// PW_MULTISINK_LOCK_INTERRUPT_SAFE controls whether an interrupt-safe lock is
// used when reading and writing from the underlying ring-buffer. This is
// enabled by default, using an interrupt spin-lock instead of a mutex.
// Disabling this alters the entry precondition of the multisink, requiring that
// it not be invoked from an interrupt context.
#if !defined(PW_MULTISINK_CONFIG_LOCK_INTERRUPT_SAFE)
#define PW_MULTISINK_CONFIG_LOCK_INTERRUPT_SAFE 1
#endif  // !defined(PW_MULTISINK_CONFIG_LOCK_INTERRUPT_SAFE)

#if PW_MULTISINK_CONFIG_LOCK_INTERRUPT_SAFE
#include "pw_sync/interrupt_spin_lock.h"
#else  // !PW_MULTISINK_CONFIG_LOCK_INTERRUPT_SAFE
#include "pw_sync/mutex.h"
#endif  // PW_MULTISINK_CONFIG_LOCK_INTERRUPT_SAFE

namespace pw {
namespace multisink {

#if PW_MULTISINK_CONFIG_LOCK_INTERRUPT_SAFE
using LockType = pw::sync::InterruptSpinLock;
#else   // !PW_MULTISINK_CONFIG_LOCK_INTERRUPT_SAFE
using LockType = pw::sync::Mutex;
#endif  // PW_MULTISINK_CONFIG_LOCK_INTERRUPT_SAFE

}  // namespace multisink
}  // namespace pw
