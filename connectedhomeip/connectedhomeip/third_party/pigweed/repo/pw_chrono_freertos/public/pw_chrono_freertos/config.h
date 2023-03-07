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

#include "FreeRTOS.h"

// Because the SystemClock::now() implementation requires the user to invoke it
// more than once per overflow period, the max timeout is set to ensure that
// blocking indefinitely on a single primitive will meet this constraint with
// margin (i.e. more than twice per overflow).
#ifndef PW_CHRONO_FREERTOS_CFG_MAX_TIMEOUT
#define PW_CHRONO_FREERTOS_CFG_MAX_TIMEOUT (portMAX_DELAY / 3)
#endif  // PW_CHRONO_FREERTOS_CFG_MAX_TIMEOUT

static_assert((PW_CHRONO_FREERTOS_CFG_MAX_TIMEOUT > 0) &&
                  (PW_CHRONO_FREERTOS_CFG_MAX_TIMEOUT <= portMAX_DELAY),
              "Invalid MAX timeout configuration");
