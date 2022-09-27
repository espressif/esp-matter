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
#include <esp_matter_core.h>

namespace esp_matter {
namespace cluster {
using client::peer_device_t;

/** Specific command send APIs
 *
 * If some standard command is not present here, it can be added.
 */

namespace on_off {
namespace command {
esp_err_t send_off(peer_device_t *remote_device, uint16_t remote_endpoint_id);
esp_err_t send_on(peer_device_t *remote_device, uint16_t remote_endpoint_id);
esp_err_t send_toggle(peer_device_t *remote_device, uint16_t remote_endpoint_id);
esp_err_t group_send_off(uint8_t fabric_index, uint16_t group_id);
esp_err_t group_send_on(uint8_t fabric_index, uint16_t group_id);
esp_err_t group_send_toggle(uint8_t fabric_index, uint16_t group_id);
} /* command */
} /* on_off */

namespace level_control {
namespace command {
esp_err_t send_move(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t move_mode, uint8_t rate,
                    uint8_t option_mask, uint8_t option_override);
esp_err_t send_move_to_level(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t level,
                             uint16_t transition_time, uint8_t option_mask, uint8_t option_override);
esp_err_t send_move_to_level_with_on_off(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t level,
                                         uint16_t transition_time);
esp_err_t send_move_with_on_off(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t move_mode,
                                uint8_t rate);
esp_err_t send_step(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t step_mode, uint8_t step_size,
                    uint16_t transition_time, uint8_t option_mask, uint8_t option_override);
esp_err_t send_step_with_on_off(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t step_mode,
                                uint8_t step_size, uint16_t transition_time);
esp_err_t send_stop(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t option_mask,
                    uint8_t option_override);
esp_err_t send_stop_with_on_off(peer_device_t *remote_device, uint16_t remote_endpoint_id);
} /* command */
} /* level_control */

namespace color_control {
namespace command {
esp_err_t send_move_hue(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t move_mode, uint8_t rate,
                        uint8_t option_mask, uint8_t option_override);
esp_err_t send_move_saturation(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t move_mode,
                               uint8_t rate,
                               uint8_t option_mask, uint8_t option_override);
esp_err_t send_move_to_hue(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t hue, uint8_t direction,
                           uint16_t transition_time, uint8_t option_mask, uint8_t option_override);
esp_err_t send_move_to_hue_and_saturation(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t hue,
                                          uint8_t saturation, uint16_t transition_time, uint8_t option_mask,
                                          uint8_t option_override);
esp_err_t send_move_to_saturation(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t saturation,
                                  uint16_t transition_time, uint8_t option_mask, uint8_t option_override);
esp_err_t send_step_hue(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t step_mode, uint8_t step_size,
                        uint16_t transition_time, uint8_t option_mask, uint8_t option_override);
esp_err_t send_step_saturation(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t step_mode,
                               uint8_t step_size, uint16_t transition_time, uint8_t option_mask,
                               uint8_t option_override);
} /* command */
} /* color_control */

} /* cluster */
} /* esp_matter */
