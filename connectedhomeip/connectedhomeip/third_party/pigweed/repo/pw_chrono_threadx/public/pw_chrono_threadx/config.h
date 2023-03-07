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
// Configuration macros for the tokenizer module.
#pragma once

#include <assert.h>

#include "tx_api.h"

// For the ThreadX backend of the SystemClock, there is no way to determine the
// system timer's tick interval/frequency. By default most ports happen to be at
// 10ms intervals (i.e. 100Hz), however this can be tuned for different
// applications.
// The resulting clock period is defined in seconds = numerator / denominator.
// For example the default is configured to 1/100 seconds per clock tick.
#ifndef PW_CHRONO_THREADX_CFG_CLOCK_PERIOD_SECONDS_NUMERATOR
#define PW_CHRONO_THREADX_CFG_CLOCK_PERIOD_SECONDS_NUMERATOR 1
#endif  // PW_CHRONO_THREADX_CFG_CLOCK_PERIOD_SECONDS_NUMERATOR
#ifndef PW_CHRONO_THREADX_CFG_CLOCK_PERIOD_SECONDS_DENOMINATOR
#define PW_CHRONO_THREADX_CFG_CLOCK_PERIOD_SECONDS_DENOMINATOR 100
#endif  // PW_CHRONO_THREADX_CFG_CLOCK_PERIOD_SECONDS_DENOMINATOR

static_assert(PW_CHRONO_THREADX_CFG_CLOCK_PERIOD_SECONDS_NUMERATOR >= 1,
              "the numerator must be positive and cannot be fractional");
static_assert(PW_CHRONO_THREADX_CFG_CLOCK_PERIOD_SECONDS_DENOMINATOR >= 1,
              "the denominator must be positive and cannot be fractional");

// Because the SystemClock::now() implementation requires the user to invoke it
// more than once per overflow period, the max timeout is set to ensure that
// blocking indefinitely on a single primitive will meet this constraint with
// margin (i.e. more than twice per overflow).
#ifndef PW_CHRONO_THREADX_CFG_MAX_TIMEOUT
#define PW_CHRONO_THREADX_CFG_MAX_TIMEOUT ((TX_WAIT_FOREVER - 1) / 3)
#endif  // PW_CHRONO_THREADX_CFG_MAX_TIMEOUT

static_assert((PW_CHRONO_THREADX_CFG_MAX_TIMEOUT > 0) &&
                  (PW_CHRONO_THREADX_CFG_MAX_TIMEOUT <= (TX_WAIT_FOREVER - 1)),
              "the timeout must be greater than 0 and less than the sentinel");
