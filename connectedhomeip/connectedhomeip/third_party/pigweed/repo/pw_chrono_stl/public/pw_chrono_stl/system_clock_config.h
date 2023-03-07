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

// Ideally we'd use std::chrono::steady_clock::period, however this is not
// something we can expose to the C API. Instead we assume it has nanosecond
// compatibility and we rely on implicit conversion to tell us at compile time
// whether this is incompatible.
#define PW_CHRONO_SYSTEM_CLOCK_PERIOD_SECONDS_NUMERATOR 1
#define PW_CHRONO_SYSTEM_CLOCK_PERIOD_SECONDS_DENOMINATOR 1000000000

#ifdef __cplusplus

#include "pw_chrono/epoch.h"

namespace pw::chrono::backend {

// The std::chrono::steady_clock does not have a defined epoch.
constexpr inline Epoch kSystemClockEpoch = pw::chrono::Epoch::kUnknown;

// The std::chrono::steady_clock can be used by signal handlers.
constexpr inline bool kSystemClockNmiSafe = true;

// The std::chrono::steady_clock ticks while in a signal handler.
constexpr inline bool kSystemClockFreeRunning = true;

}  // namespace pw::chrono::backend

#endif  // __cplusplus
