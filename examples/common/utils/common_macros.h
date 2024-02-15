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

#define ABORT_APP_ON_FAILURE(x, ...) do {           \
        if (!(unlikely(x))) {                       \
            __VA_ARGS__;                            \
            vTaskDelay(5000 / portTICK_PERIOD_MS);  \
            abort();                                \
        }                                           \
    } while (0)

