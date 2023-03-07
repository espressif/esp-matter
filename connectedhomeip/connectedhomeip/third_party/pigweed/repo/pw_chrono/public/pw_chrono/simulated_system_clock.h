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

#include <mutex>

#include "pw_chrono/system_clock.h"
#include "pw_sync/interrupt_spin_lock.h"

namespace pw::chrono {

// A simulated system clock is a concrete virtual SystemClock implementation
// that does not "tick" on its own. Time is advanced by explicit calls to
// AdvanceTime() or SetTime() functions. This can be used as stub for testing
// which can be dependency injected. Be careful when using SetTime() to not
// violate the is_monotonic requirement, in other words avoid going backwards
// unless initializing the clock before consumers have a reference to the clock.
//
// Example:
//   SimulatedSystemClock sim_system_clock;
//
//   SystemClock::time_point now = sim_system_clock.now();
//   // now.time_since_epoch.duration() == std::chrono::seconds(0)
//
//   sim_system_clock.AdvanceTime(std::chrono::seconds(42));
//   // now.time_since_epoch.duration() == std::chrono::seconds(42)
//
// This code is thread & IRQ safe.
class SimulatedSystemClock : public VirtualSystemClock {
 public:
  SimulatedSystemClock(SystemClock::time_point timestamp =
                           SystemClock::time_point(SystemClock::duration(0)))
      : current_timestamp_(timestamp) {}

  void AdvanceTime(SystemClock::duration duration) {
    std::lock_guard lock(interrupt_spin_lock_);
    current_timestamp_ += duration;
  }

  // WARNING: Use of this function may violate the is_monotonic clock attribute.
  void SetTime(SystemClock::time_point timestamp) {
    std::lock_guard lock(interrupt_spin_lock_);
    current_timestamp_ = timestamp;
  }

  SystemClock::time_point now() override {
    std::lock_guard lock(interrupt_spin_lock_);
    return current_timestamp_;
  }

 private:
  // In theory atomics could be used if 64bit atomics are supported, however
  // performance of this test object shouldn't matter.
  sync::InterruptSpinLock interrupt_spin_lock_;
  SystemClock::time_point current_timestamp_;
};

}  // namespace pw::chrono
