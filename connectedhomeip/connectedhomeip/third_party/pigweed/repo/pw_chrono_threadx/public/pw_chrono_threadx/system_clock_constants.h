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

#include "pw_chrono/system_clock.h"
#include "pw_chrono_threadx/config.h"

namespace pw::chrono::threadx {

// Max timeout to be used by users of the ThreadX's pw::chrono::SystemClock
// backend provided by this module.
inline constexpr SystemClock::duration kMaxTimeout =
    SystemClock::duration(PW_CHRONO_THREADX_CFG_MAX_TIMEOUT);

}  // namespace pw::chrono::threadx
