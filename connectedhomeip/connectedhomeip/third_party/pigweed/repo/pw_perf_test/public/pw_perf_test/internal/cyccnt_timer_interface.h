// Copyright 2022 The Pigweed Authors
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

// This timing interface enables and targets ARM registers to enable clock cycle
// counting. The documentation can be found here:
// https://developer.arm.com/documentation/ddi0337/e/System-Debug/DWT?lang=en
// https://developer.arm.com/documentation/ddi0337/e/CEGHJDCF

#pragma once

#include <cstdint>

#include "pw_perf_test/internal/duration_unit.h"

namespace pw::perf_test::internal::backend {

// Creates a reference to the DWT Control unit
inline volatile uint32_t& kDwtCtrl =
    *reinterpret_cast<volatile uint32_t*>(0xE0001000);
// Creates a reference to the memory address in which the DWT register stores
// clock incrementations
inline volatile uint32_t& kDwtCcynt =
    *reinterpret_cast<volatile uint32_t*>(0xE0001004);
// Creates a reference to the location of the Debug Exception and Monitor
// Control register.
inline volatile uint32_t& kDemcr =
    *reinterpret_cast<volatile uint32_t*>(0xE000EDFC);

using Timestamp = uint32_t;

inline constexpr DurationUnit kDurationUnit = DurationUnit::kClockCycle;

[[nodiscard]] inline bool TimerPrepare() {
  kDemcr |= 0x01000000;    // Enables the DWT control register
  kDwtCtrl |= 0x00000001;  // Enables the DWT clock
  kDwtCcynt = 0x00000000;  // Intializes the clock to 0
  return (kDemcr & 0x01000000) && (kDwtCtrl & 0x00000001);
}

// Disables the DWT clock
inline void TimerCleanup() { kDwtCtrl &= ~0x00000001; }

inline Timestamp GetCurrentTimestamp() { return kDwtCcynt; }

// Warning: this duration will overflow if the duration is greater that 2^32
// clock cycles.
inline int64_t GetDuration(Timestamp begin, Timestamp end) {
  return static_cast<int64_t>(end - begin);
}

}  // namespace pw::perf_test::internal::backend
