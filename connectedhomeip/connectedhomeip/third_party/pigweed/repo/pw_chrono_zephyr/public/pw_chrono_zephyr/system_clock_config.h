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

#include <kernel.h>

// Use the Zephyr config's tick rate.
#define PW_CHRONO_SYSTEM_CLOCK_PERIOD_SECONDS_NUMERATOR 1
#define PW_CHRONO_SYSTEM_CLOCK_PERIOD_SECONDS_DENOMINATOR \
  CONFIG_SYS_CLOCK_TICKS_PER_SEC

#ifdef __cplusplus

#include "pw_chrono/epoch.h"

namespace pw::chrono::backend {

// The Zephyr clock starts at zero during initialization, approximately the
// time since boot.
inline constexpr Epoch kSystemClockEpoch = pw::chrono::Epoch::kTimeSinceBoot;

// The current backend implementation is not NMI safe.
inline constexpr bool kSystemClockNmiSafe = false;

// The Zephyr clock never halts.
inline constexpr bool kSystemClockFreeRunning = false;

}  // namespace pw::chrono::backend

#endif  // __cplusplus
