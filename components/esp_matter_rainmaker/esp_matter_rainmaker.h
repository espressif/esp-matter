// Copyright 2022 Espressif Systems (Shanghai) PTE LTD
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

#include <esp_err.h>
#include <esp_matter.h>

namespace esp_matter {
namespace rainmaker {

/** Initialize ESP Matter RainMaker
 *
 * This adds the custom RainMaker cluster which is used for RainMaker User Node Association.
 * The 'rainmaker' console command is also added.
 * This should be called before `esp_matter_start()`
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t init(void);

/** Start ESP Matter RainMaker
 *
 * This starts the post initialization process for RainMaker User Node Association.
 * This should be called after `esp_rmaker_node_init()` but before `esp_rmaker_start()`.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t start(void);

} /* rainmaker */
} /* esp_matter */
