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
} // namespace command
} // namespace on_off

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
esp_err_t group_send_move(uint8_t fabric_index, uint16_t group_id, uint8_t move_mode, uint8_t rate, uint8_t option_mask,
                          uint8_t option_override);
esp_err_t group_send_move_to_level(uint8_t fabric_index, uint16_t group_id, uint8_t level, uint16_t transition_time,
                                   uint8_t option_mask, uint8_t option_override);
esp_err_t group_send_move_to_level_with_on_off(uint8_t fabric_index, uint16_t group_id, uint8_t level,
                                               uint16_t transition_time);
esp_err_t group_send_move_with_on_off(uint8_t fabric_index, uint16_t group_id, uint8_t move_mode, uint8_t rate);
esp_err_t group_send_step(uint8_t fabric_index, uint16_t group_id, uint8_t step_mode, uint8_t step_size,
                          uint16_t transition_time, uint8_t option_mask, uint8_t option_override);
esp_err_t group_send_step_with_on_off(uint8_t fabric_index, uint16_t group_id, uint8_t step_mode, uint8_t step_size,
                                      uint16_t transition_time);
esp_err_t group_send_stop(uint8_t fabric_index, uint16_t group_id, uint8_t option_mask, uint8_t option_override);
esp_err_t group_send_stop_with_on_off(uint8_t fabric_index, uint16_t group_id);
} // namespace command
} // namespace level_control

namespace color_control {
namespace command {
esp_err_t send_move_hue(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t move_mode, uint8_t rate,
                        uint8_t option_mask, uint8_t option_override);
esp_err_t send_move_saturation(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t move_mode,
                               uint8_t rate, uint8_t option_mask, uint8_t option_override);
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
esp_err_t send_move_to_color(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint16_t color_x,
                             uint16_t color_y, uint16_t transition_time, uint8_t option_mask, uint8_t option_override);
esp_err_t send_move_color(peer_device_t *remote_device, uint16_t remote_endpoint_id, int16_t rate_x, int16_t rate_y,
                          uint8_t option_mask, uint8_t option_override);
esp_err_t send_step_color(peer_device_t *remote_device, uint16_t remote_endpoint_id, int16_t step_x, int16_t step_y,
                          uint16_t transition_time, uint8_t option_mask, uint8_t option_override);
esp_err_t send_move_to_color_temperature(peer_device_t *remote_device, uint16_t remote_endpoint_id,
                                         uint16_t color_temperature_mireds, uint16_t transition_time,
                                         uint8_t option_mask, uint8_t option_override);
esp_err_t send_stop_move_step(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t option_mask,
                              uint8_t option_override);
esp_err_t send_move_color_temperature(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t move_mode,
                                      uint16_t rate, uint16_t color_temperature_minimum_mireds,
                                      uint16_t color_temperature_maximum_mireds, uint8_t option_mask,
                                      uint8_t option_override);
esp_err_t send_step_color_temperature(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t step_mode,
                                      uint16_t step_size, uint16_t transition_time,
                                      uint16_t color_temperature_minimum_mireds,
                                      uint16_t color_temperature_maximum_mireds, uint8_t option_mask,
                                      uint8_t option_override);

esp_err_t group_send_move_hue(uint8_t fabric_index, uint16_t group_id, uint8_t move_mode, uint8_t rate,
                              uint8_t option_mask, uint8_t option_override);
esp_err_t group_send_move_saturation(uint8_t fabric_index, uint16_t group_id, uint8_t move_mode, uint8_t rate,
                                     uint8_t option_mask, uint8_t option_override);
esp_err_t group_send_move_to_hue(uint8_t fabric_index, uint16_t group_id, uint8_t hue, uint8_t direction,
                                 uint16_t transition_time, uint8_t option_mask, uint8_t option_override);
esp_err_t group_send_move_to_hue_and_saturation(uint8_t fabric_index, uint16_t group_id, uint8_t hue,
                                                uint8_t saturation, uint16_t transition_time, uint8_t option_mask,
                                                uint8_t option_override);
esp_err_t group_send_move_to_saturation(uint8_t fabric_index, uint16_t group_id, uint8_t saturation,
                                        uint16_t transition_time, uint8_t option_mask, uint8_t option_override);
esp_err_t group_send_step_hue(uint8_t fabric_index, uint16_t group_id, uint8_t step_mode, uint8_t step_size,
                              uint16_t transition_time, uint8_t option_mask, uint8_t option_override);
esp_err_t group_send_step_saturation(uint8_t fabric_index, uint16_t group_id, uint8_t step_mode, uint8_t step_size,
                                     uint16_t transition_time, uint8_t option_mask, uint8_t option_override);
esp_err_t group_send_move_to_color(uint8_t fabric_index, uint16_t group_id, uint16_t color_x, uint16_t color_y,
                                   uint16_t transition_time, uint8_t option_mask, uint8_t option_override);
esp_err_t group_send_move_color(uint8_t fabric_index, uint16_t group_id, int16_t rate_x, int16_t rate_y,
                                uint8_t option_mask, uint8_t option_override);
esp_err_t group_send_step_color(uint8_t fabric_index, uint16_t group_id, int16_t step_x, int16_t step_y,
                                uint16_t transition_time, uint8_t option_mask, uint8_t option_override);
esp_err_t group_send_move_to_color_temperature(uint8_t fabric_index, uint16_t group_id,
                                               uint16_t color_temperature_mireds, uint16_t transition_time,
                                               uint8_t option_mask, uint8_t option_override);
esp_err_t group_send_stop_move_step(uint8_t fabric_index, uint16_t group_id, uint8_t option_mask,
                                    uint8_t option_override);
esp_err_t group_send_move_color_temperature(uint8_t fabric_index, uint16_t group_id, uint8_t move_mode, uint16_t rate,
                                            uint16_t color_temperature_minimum_mireds,
                                            uint16_t color_temperature_maximum_mireds, uint8_t option_mask,
                                            uint8_t option_override);
esp_err_t group_send_step_color_temperature(uint8_t fabric_index, uint16_t group_id, uint8_t step_mode,
                                            uint16_t step_size, uint16_t transition_time,
                                            uint16_t color_temperature_minimum_mireds,
                                            uint16_t color_temperature_maximum_mireds, uint8_t option_mask,
                                            uint8_t option_override);

} // namespace command
} // namespace color_control

namespace identify {
namespace command {
esp_err_t send_identify(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint16_t identify_time);

esp_err_t group_send_identify(uint8_t fabric_index, uint16_t group_id, uint16_t identify_time);
} // namespace command
} // namespace identify

namespace group_key_management {
namespace command {
using group_keyset_struct = chip::app::Clusters::GroupKeyManagement::Structs::GroupKeySetStruct::Type;
using keyset_read_callback =
    void (*)(void *, const chip::app::Clusters::GroupKeyManagement::Commands::KeySetRead::Type::ResponseType &);

esp_err_t send_keyset_write(peer_device_t *remote_device, uint16_t remote_endpoint_id,
                            group_keyset_struct group_keyset);

esp_err_t send_keyset_read(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint16_t keyset_id,
                           keyset_read_callback read_callback);

} // namespace command
} // namespace group_key_management

namespace groups {
namespace command {
using add_group_callback = void (*)(void *,
                                    const chip::app::Clusters::Groups::Commands::AddGroup::Type::ResponseType &);
using view_group_callback = void (*)(void *,
                                     const chip::app::Clusters::Groups::Commands::ViewGroup::Type::ResponseType &);
using remove_group_callback = void (*)(void *,
                                       const chip::app::Clusters::Groups::Commands::RemoveGroup::Type::ResponseType &);

esp_err_t send_add_group(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint16_t group_id, char *group_name,
                         add_group_callback add_group_cb);

esp_err_t send_view_group(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint16_t group_id,
                          view_group_callback view_group_cb);

esp_err_t send_remove_group(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint16_t group_id,
                            remove_group_callback remove_group_cb);

} // namespace command
} // namespace groups

} // namespace cluster
} // namespace esp_matter
