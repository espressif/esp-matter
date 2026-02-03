// Copyright 2024 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <stdlib.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

/** Remap attribute values
 *
 * This can be used to remap attribute values to different ranges.
 * Example: To convert the brightness value (0-255) into brightness percentage (0-100) and vice-versa.
 */
#define REMAP_TO_RANGE(value, from, to) ((value * to) / from)

/** Remap attribute values with inverse dependency
 *
 * This can be used to remap attribute values with inverse dependency to different ranges.
 * Example: To convert the temperature mireds into temperature kelvin and vice-versa where the relation between them
 * is: Mireds = 1,000,000/Kelvin.
 */
#define REMAP_TO_RANGE_INVERSE(value, factor) (factor / (value ? value : 1))

#define ABORT_APP_ON_FAILURE(x, ...) do {           \
        if (!(unlikely(x))) {                       \
            __VA_ARGS__;                            \
            vTaskDelay(5000 / portTICK_PERIOD_MS);  \
            abort();                                \
        }                                           \
    } while (0)
