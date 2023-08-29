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

#include <app-common/zap-generated/cluster-objects.h>
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

esp_err_t send_trigger_effect(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t effect_identifier,
                              uint8_t effect_variant);
} // namespace command
} // namespace identify

namespace group_key_management {
namespace command {
using group_keyset_struct = chip::app::Clusters::GroupKeyManagement::Structs::GroupKeySetStruct::Type;
using keyset_read_callback =
    void (*)(void *, const chip::app::Clusters::GroupKeyManagement::Commands::KeySetRead::Type::ResponseType &);

esp_err_t send_keyset_write(peer_device_t *remote_device, uint16_t remote_endpoint_id,
                            group_keyset_struct &group_keyset);

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

namespace scenes {
namespace command {

using extension_field_sets = chip::app::DataModel::List<chip::app::Clusters::Scenes::Structs::ExtensionFieldSet::Type>;
using add_scene_callback = void (*)(void *,
                                    const chip::app::Clusters::Scenes::Commands::AddScene::Type::ResponseType &);
using view_scene_callback = void (*)(void *,
                                     const chip::app::Clusters::Scenes::Commands::ViewScene::Type::ResponseType &);
using remove_scene_callback = void (*)(void *,
                                       const chip::app::Clusters::Scenes::Commands::RemoveScene::Type::ResponseType &);
using remove_all_scenes_callback =
    void (*)(void *, const chip::app::Clusters::Scenes::Commands::RemoveAllScenes::Type::ResponseType &);
using store_scene_callback = void (*)(void *,
                                      const chip::app::Clusters::Scenes::Commands::StoreScene::Type::ResponseType &);
using get_scene_membership_callback =
    void (*)(void *, const chip::app::Clusters::Scenes::Commands::GetSceneMembership::Type::ResponseType &);

esp_err_t send_add_scene(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint16_t group_id, uint8_t scene_id,
                         uint16_t transition_time, char *scene_name, extension_field_sets &efs,
                         add_scene_callback add_scene_cb);

esp_err_t send_view_scene(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint16_t group_id,
                          uint8_t scene_id, view_scene_callback view_scene_cb);

esp_err_t send_remove_scene(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint16_t group_id,
                            uint8_t scene_id, remove_scene_callback remove_scene_cb);

esp_err_t send_remove_all_scenes(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint16_t group_id,
                                 remove_all_scenes_callback remove_all_scenes_cb);

esp_err_t send_store_scene(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint16_t group_id,
                           uint8_t scene_id, store_scene_callback store_scene_cb);

esp_err_t send_recall_scene(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint16_t group_id,
                            uint8_t scene_id);

esp_err_t send_get_scene_membership(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint16_t group_id,
                                    get_scene_membership_callback get_scene_membership_cb);

} // namespace command
} // namespace scenes

namespace thermostat {
namespace command {

using transitions =
    chip::app::DataModel::List<chip::app::Clusters::Thermostat::Structs::ThermostatScheduleTransition::Type>;

using get_weekly_schedule_callback =
    void (*)(void *, const chip::app::Clusters::Thermostat::Commands::GetWeeklySchedule::Type::ResponseType &);

esp_err_t send_setpoint_raise_lower(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t mode,
                                    uint8_t amount);

esp_err_t send_set_weekly_schedule(peer_device_t *remote_device, uint16_t remote_endpoint_id,
                                   uint8_t num_of_tras_for_seq, uint8_t day_of_week_for_seq, uint8_t mode_for_seq,
                                   transitions &trans);

esp_err_t send_get_weekly_schedule(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t day_to_return,
                                   uint8_t mode_to_return, get_weekly_schedule_callback get_weekly_schedule_cb);

esp_err_t send_clear_weekly_schedule(peer_device_t *remote_device, uint16_t remote_endpoint_id);

} // namespace command
} // namespace thermostat

namespace door_lock {
namespace command {

using get_week_day_schedule_callback =
    void (*)(void *, const chip::app::Clusters::DoorLock::Commands::GetWeekDaySchedule::Type::ResponseType &);

using get_year_day_schedule_callback =
    void (*)(void *, const chip::app::Clusters::DoorLock::Commands::GetYearDaySchedule::Type::ResponseType &);

using get_holiday_schedule_callback =
    void (*)(void *, const chip::app::Clusters::DoorLock::Commands::GetHolidaySchedule::Type::ResponseType &);

using get_user_callback = void (*)(void *,
                                   const chip::app::Clusters::DoorLock::Commands::GetUser::Type::ResponseType &);

using set_credential_callback =
    void (*)(void *, const chip::app::Clusters::DoorLock::Commands::SetCredential::Type::ResponseType &);

using get_credential_status_callback =
    void (*)(void *, const chip::app::Clusters::DoorLock::Commands::GetCredentialStatus::Type::ResponseType &);

using credential_struct = chip::app::Clusters::DoorLock::Structs::CredentialStruct::Type;

esp_err_t send_lock_door(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint16_t timed_invoke_timeout_ms);

esp_err_t send_unlock_door(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint16_t timed_invoke_timeout_ms);

esp_err_t send_unlock_with_timeout(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint16_t timeout,
                                   uint16_t timed_invoke_timeout_ms);

esp_err_t send_set_week_day_schedule(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t week_day_index,
                                     uint16_t user_index, uint8_t days_mask, uint8_t start_hour, uint8_t start_minute,
                                     uint8_t end_hour, uint8_t end_minute);

esp_err_t send_get_week_day_schedule(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t week_day_index,
                                     uint16_t user_index, get_week_day_schedule_callback success_cb);

esp_err_t send_clear_week_day_schedule(peer_device_t *remote_device, uint16_t remote_endpoint_id,
                                       uint8_t week_day_index, uint16_t user_index);

esp_err_t send_set_year_day_schedule(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t year_day_index,
                                     uint16_t user_index, uint32_t local_start_time, uint32_t local_end_time);

esp_err_t send_get_year_day_schedule(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t year_day_index,
                                     uint16_t user_index, get_year_day_schedule_callback success_cb);

esp_err_t send_clear_year_day_schedule(peer_device_t *remote_device, uint16_t remote_endpoint_id,
                                       uint8_t year_day_index, uint16_t user_index);

esp_err_t send_set_holiday_schedule(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t holiday_index,
                                    uint32_t local_start_time, uint32_t local_end_time, uint8_t operating_mode);

esp_err_t send_get_holiday_schedule(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t holiday_index,
                                    get_holiday_schedule_callback success_cb);

esp_err_t send_clear_holiday_schedule(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t holiday_index);

esp_err_t send_set_user(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t operation_type,
                        uint16_t user_index, char *user_name, uint32_t user_unique_id, uint8_t user_status,
                        uint8_t user_type, uint8_t credential_rule, uint16_t timed_invoke_timeout_ms);

esp_err_t send_get_user(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint16_t user_index,
                        get_user_callback success_cb);

esp_err_t send_clear_user(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint16_t user_index,
                          uint16_t timed_invoke_timeout_ms);

esp_err_t send_set_credential(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t operation_type,
                              credential_struct credential, uint8_t *credential_data, size_t credential_len,
                              uint16_t user_index, uint8_t user_status, uint8_t user_type,
                              set_credential_callback success_cb, uint16_t timed_invoke_timeout_ms);

esp_err_t send_get_credential_status(peer_device_t *remote_device, uint16_t remote_endpoint_id,
                                     credential_struct &credential, get_credential_status_callback success_cb);

esp_err_t send_clear_credential(peer_device_t *remote_device, uint16_t remote_endpoint_id,
                                credential_struct &credential, uint16_t timed_invoke_timeout_ms);

esp_err_t send_unbolt_door(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t *pin_code,
                           size_t pin_code_len, uint16_t timed_invoke_timeout_ms);

} // namespace command
} // namespace door_lock

namespace window_covering {
namespace command {

esp_err_t send_up_or_open(peer_device_t *remote_device, uint16_t remote_endpoint_id);

esp_err_t send_down_or_close(peer_device_t *remote_device, uint16_t remote_endpoint_id);

esp_err_t send_stop_motion(peer_device_t *remote_device, uint16_t remote_endpoint_id);

esp_err_t send_go_to_lift_value(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint16_t lift_value);

esp_err_t send_go_to_lift_percentage(peer_device_t *remote_device, uint16_t remote_endpoint_id,
                                     uint16_t lift_percent100ths_value);

esp_err_t send_go_to_tilt_value(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint16_t tilt_value);

esp_err_t send_go_to_tilt_percentage(peer_device_t *remote_device, uint16_t remote_endpoint_id,
                                     uint16_t tilt_percent100ths_value);

} // namespace command
} // namespace window_covering

} // namespace cluster
} // namespace esp_matter
