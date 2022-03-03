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

/* cluster: on_off */
esp_err_t esp_matter_on_off_send_command_off(esp_matter_peer_device_t *remote_device, int remote_endpoint_id);
esp_err_t esp_matter_on_off_send_command_on(esp_matter_peer_device_t *remote_device, int remote_endpoint_id);
esp_err_t esp_matter_on_off_send_command_toggle(esp_matter_peer_device_t *remote_device, int remote_endpoint_id);

/* cluster: level_control */
esp_err_t esp_matter_level_control_send_command_move(esp_matter_peer_device_t *remote_device, int remote_endpoint_id,
                                                     uint8_t move_mode, uint8_t rate, uint8_t option_mask,
                                                     uint8_t option_override);
esp_err_t esp_matter_level_control_send_command_move_to_level(esp_matter_peer_device_t *remote_device,
                                                              int remote_endpoint_id, uint8_t level,
                                                              uint16_t transition_time, uint8_t option_mask,
                                                              uint8_t option_override);
esp_err_t esp_matter_level_control_send_command_move_to_level_with_on_off(esp_matter_peer_device_t *remote_device,
                                                                          int remote_endpoint_id, uint8_t level,
                                                                          uint16_t transition_time);
esp_err_t esp_matter_level_control_send_command_move_with_on_off(esp_matter_peer_device_t *remote_device,
                                                                 int remote_endpoint_id, uint8_t move_mode,
                                                                 uint8_t rate);
esp_err_t esp_matter_level_control_send_command_step(esp_matter_peer_device_t *remote_device, int remote_endpoint_id,
                                                     uint8_t step_mode, uint8_t step_size, uint16_t transition_time,
                                                     uint8_t option_mask, uint8_t option_override);
esp_err_t esp_matter_level_control_send_command_step_with_on_off(esp_matter_peer_device_t *remote_device,
                                                                 int remote_endpoint_id, uint8_t step_mode,
                                                                 uint8_t step_size, uint16_t transition_time);
esp_err_t esp_matter_level_control_send_command_stop(esp_matter_peer_device_t *remote_device, int remote_endpoint_id,
                                                     uint8_t option_mask, uint8_t option_override);
esp_err_t esp_matter_level_control_send_command_stop_with_on_off(esp_matter_peer_device_t *remote_device,
                                                                 int remote_endpoint_id);

/* cluster: color_control */
esp_err_t esp_matter_color_control_send_command_move_hue(esp_matter_peer_device_t *remote_device,
                                                         int remote_endpoint_id, uint8_t move_mode, uint8_t rate,
                                                         uint8_t option_mask, uint8_t option_override);
esp_err_t esp_matter_color_control_send_command_move_saturation(esp_matter_peer_device_t *remote_device,
                                                                int remote_endpoint_id, uint8_t move_mode, uint8_t rate,
                                                                uint8_t option_mask, uint8_t option_override);
esp_err_t esp_matter_color_control_send_command_move_to_hue(esp_matter_peer_device_t *remote_device,
                                                            int remote_endpoint_id, uint8_t hue, uint8_t direction,
                                                            uint16_t transition_time, uint8_t option_mask,
                                                            uint8_t option_override);
esp_err_t esp_matter_color_control_send_command_move_to_hue_and_saturation(esp_matter_peer_device_t *remote_device,
                                                                        int remote_endpoint_id, uint8_t hue,
                                                                        uint8_t saturation, uint16_t transition_time,
                                                                        uint8_t option_mask, uint8_t option_override);
esp_err_t esp_matter_color_control_send_command_move_to_saturation(esp_matter_peer_device_t *remote_device,
                                                                   int remote_endpoint_id, uint8_t saturation,
                                                                   uint16_t transition_time, uint8_t option_mask,
                                                                   uint8_t option_override);
esp_err_t esp_matter_color_control_send_command_step_hue(esp_matter_peer_device_t *remote_device,
                                                         int remote_endpoint_id, uint8_t step_mode, uint8_t step_size,
                                                         uint16_t transition_time, uint8_t option_mask,
                                                         uint8_t option_override);
esp_err_t esp_matter_color_control_send_command_step_saturation(esp_matter_peer_device_t *remote_device,
                                                                int remote_endpoint_id, uint8_t step_mode,
                                                                uint8_t step_size, uint16_t transition_time,
                                                                uint8_t option_mask, uint8_t option_override);
