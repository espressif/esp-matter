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

#include "pw_assert/check.h"
#include "pw_interrupt/context.h"
#include "tx_api.h"

namespace pw::sync {
namespace {

using State = backend::NativeInterruptSpinLock::State;

}  // namespace

void InterruptSpinLock::lock() {
  // In order to be pw::sync::InterruptSpinLock compliant, mask the interrupts
  // before attempting to grab the internal spin lock.
  native_type_.saved_interrupt_mask = tx_interrupt_control(TX_INT_DISABLE);

  const bool in_interrupt = interrupt::InInterruptContext();

  // Disable thread switching to ensure kernel APIs cannot switch to other
  // threads which could then end up deadlocking recursively on this same lock.
  if (!in_interrupt) {
    TX_THREAD* current_thread = tx_thread_identify();
    // During init, i.e. tx_application_define, there may not be a thread yet.
    if (current_thread != nullptr) {
      // Disable thread switching by raising the preemption threshold to the
      // highest priority value of 0.
      UINT preemption_success = tx_thread_preemption_change(
          tx_thread_identify(), 0, &native_type_.saved_preemption_threshold);
      PW_DCHECK_UINT_EQ(
          TX_SUCCESS, preemption_success, "Failed to disable thread switching");
    }
  }

  // This implementation is not set up to support SMP, meaning we cannot
  // deadlock here due to the global interrupt lock, so we crash on recursion
  // on a specific spinlock instead.
  PW_DCHECK_UINT_EQ(native_type_.state,
                    State::kUnlocked,
                    "Recursive InterruptSpinLock::lock() detected");

  native_type_.state =
      in_interrupt ? State::kLockedFromInterrupt : State::kLockedFromThread;
}

void InterruptSpinLock::unlock() {
  const bool in_interrupt = interrupt::InInterruptContext();

  const State expected_state =
      in_interrupt ? State::kLockedFromInterrupt : State::kLockedFromThread;
  PW_CHECK_UINT_EQ(
      native_type_.state,
      expected_state,
      "InterruptSpinLock::unlock() was called from a different context "
      "compared to the lock()");

  native_type_.state = State::kUnlocked;

  if (!in_interrupt) {
    TX_THREAD* current_thread = tx_thread_identify();
    // During init, i.e. tx_application_define, there may not be a thread yet.
    if (current_thread != nullptr) {
      // Restore thread switching.
      UINT unused = 0;
      UINT preemption_success =
          tx_thread_preemption_change(tx_thread_identify(),
                                      native_type_.saved_preemption_threshold,
                                      &unused);
      PW_DCHECK_UINT_EQ(
          TX_SUCCESS, preemption_success, "Failed to restore thread switching");
    }
  }
  tx_interrupt_control(native_type_.saved_interrupt_mask);
}

}  // namespace pw::sync
