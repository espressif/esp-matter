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
#include <platform/DeviceControlServer.h>

namespace esp_matter {
namespace cluster {

/** Specific event send APIs
 *
 * If some standard event is not present here, it can be added.
 */

namespace switch_cluster {
namespace event {
esp_err_t send_switch_latched(chip::EndpointId endpoint, uint8_t new_position);
esp_err_t send_initial_press(chip::EndpointId endpoint, uint8_t new_position);
esp_err_t send_long_press(chip::EndpointId endpoint, uint8_t new_position);
esp_err_t send_short_release(chip::EndpointId endpoint, uint8_t previous_position);
esp_err_t send_long_release(chip::EndpointId endpoint, uint8_t previous_position);
esp_err_t send_multi_press_ongoing(chip::EndpointId endpoint, uint8_t new_position, uint8_t count);
esp_err_t send_multi_press_complete(chip::EndpointId endpoint, uint8_t new_position, uint8_t count);
} /* event */
} /* switch_cluster */

} /* cluster */
} /* esp_matter */
