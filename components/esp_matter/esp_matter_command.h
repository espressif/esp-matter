// Copyright 2021 Espressif Systems (Shanghai) PTE LTD
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

#include <esp_matter.h>

namespace esp_matter {
struct command_entry_t {
    uint32_t command_id;
    uint8_t flags;
    command::callback_t callback;
};

struct cluster_command_t {
    uint32_t cluster_id;
    size_t accepted_command_count;
    size_t generated_command_count;
    const command_entry_t *accepted_command_list;
    const command_entry_t *generated_command_list;
};

namespace cluster {

/** Specific command create APIs
 *
 * If some standard command is not present here, it can be added.
 * If a custom command needs to be created, the low level esp_matter::command::create() API can be used.
 */

namespace actions {
namespace command {
command_t *create_instant_action(cluster_t *cluster);
command_t *create_instant_action_with_transition(cluster_t *cluster);
command_t *create_start_action(cluster_t *cluster);
command_t *create_start_action_with_duration(cluster_t *cluster);
command_t *create_stop_action(cluster_t *cluster);
command_t *create_pause_action(cluster_t *cluster);
command_t *create_pause_action_with_duration(cluster_t *cluster);
command_t *create_resume_action(cluster_t *cluster);
command_t *create_enable_action(cluster_t *cluster);
command_t *create_enable_action_with_duration(cluster_t *cluster);
command_t *create_disable_action(cluster_t *cluster);
command_t *create_disable_action_with_duration(cluster_t *cluster);
} /* command */
} /* actions */

namespace access_control {
namespace command {
command_t *create_review_fabric_restrictions(cluster_t *cluster);
command_t *create_review_fabric_restrictions_response(cluster_t *cluster);
} /* command */
} /* access_control */

namespace bridged_device_basic_information {
namespace command {
command_t *create_keep_active(cluster_t *cluster);
} /* command */
} /* bridged_device_basic_information */

namespace thread_network_diagnostics {
namespace command {
command_t *create_reset_counts(cluster_t *cluster);
} /* command */
} /* thread_network_diagnostics */

namespace wifi_network_diagnotics {
namespace command {
command_t *create_reset_counts(cluster_t *cluster);
} /* command */
} /* wifi_network_diagnotics */

namespace ethernet_network_diagnostics {
namespace command {
command_t *create_reset_counts(cluster_t *cluster);
} /* command */
} /* ethernet_network_diagnostics */

namespace diagnostic_logs {
namespace command {
command_t *create_retrieve_logs_request(cluster_t *cluster);
command_t *create_retrieve_logs_response(cluster_t *cluster);
} /* command */
} /* diagnostic_logs */

namespace general_diagnostics {
namespace command {
command_t *create_test_event_trigger(cluster_t *cluster);
command_t *create_time_snap_shot(cluster_t *cluster);
command_t *create_time_snap_shot_response(cluster_t *cluster);
} /* command */
} /* general_diagnostics */

namespace software_diagnostics {
namespace command {
command_t *create_reset_watermarks(cluster_t *cluster);
} /* command */
} /* software_diagnostics */

namespace group_key_management {
namespace command {
command_t *create_key_set_write(cluster_t *cluster);
command_t *create_key_set_read(cluster_t *cluster);
command_t *create_key_set_remove(cluster_t *cluster);
command_t *create_key_set_read_all_indices(cluster_t *cluster);
command_t *create_key_set_read_response(cluster_t *cluster);
command_t *create_key_set_read_all_indices_response(cluster_t *cluster);
} /* command */
} /* group_key_management */

namespace general_commissioning {
namespace command {
command_t *create_arm_fail_safe(cluster_t *cluster);
command_t *create_set_regulatory_config(cluster_t *cluster);
command_t *create_commissioning_complete(cluster_t *cluster);
command_t *create_arm_fail_safe_response(cluster_t *cluster);
command_t *create_set_regulatory_config_response(cluster_t *cluster);
command_t *create_commissioning_complete_response(cluster_t *cluster);
command_t *create_set_tc_acknowledgements(cluster_t *cluster);
command_t *create_set_tc_acknowledgements_response(cluster_t *cluster);
} /* command */
} /* general_commissioning */

namespace network_commissioning {
namespace command {
command_t *create_scan_networks(cluster_t *cluster);
command_t *create_add_or_update_wifi_network(cluster_t *cluster);
command_t *create_add_or_update_thread_network(cluster_t *cluster);
command_t *create_remove_network(cluster_t *cluster);
command_t *create_connect_network(cluster_t *cluster);
command_t *create_reorder_network(cluster_t *cluster);
command_t *create_scan_networks_response(cluster_t *cluster);
command_t *create_network_config_response(cluster_t *cluster);
command_t *create_connect_network_response(cluster_t *cluster);
} /* command */
} /* network_commissioning */

namespace administrator_commissioning {
namespace command {
command_t *create_open_commissioning_window(cluster_t *cluster);
command_t *create_open_basic_commissioning_window(cluster_t *cluster);
command_t *create_revoke_commissioning(cluster_t *cluster);
} /* command */
} /* administrator_commissioning */

namespace operational_credentials {
namespace command {
command_t *create_attestation_request(cluster_t *cluster);
command_t *create_certificate_chain_request(cluster_t *cluster);
command_t *create_csr_request(cluster_t *cluster);
command_t *create_add_noc(cluster_t *cluster);
command_t *create_update_noc(cluster_t *cluster);
command_t *create_update_fabric_label(cluster_t *cluster);
command_t *create_remove_fabric(cluster_t *cluster);
command_t *create_add_trusted_root_certificate(cluster_t *cluster);
command_t *create_attestation_response(cluster_t *cluster);
command_t *create_certificate_chain_response(cluster_t *cluster);
command_t *create_csr_response(cluster_t *cluster);
command_t *create_noc_response(cluster_t *cluster);
} /* command */
} /* operational_credentials */

namespace ota_provider {
namespace command {
command_t *create_query_image(cluster_t *cluster);
command_t *create_apply_update_request(cluster_t *cluster);
command_t *create_notify_update_applied(cluster_t *cluster);
command_t *create_query_image_response(cluster_t *cluster);
command_t *create_apply_update_response(cluster_t *cluster);
} /* command */
} /* ota_provider */

namespace ota_requestor {
namespace command {
command_t *create_announce_ota_provider(cluster_t *cluster);
} /* command */
} /* ota_requestor */

namespace identify {
namespace command {
command_t *create_identify(cluster_t *cluster);
command_t *create_trigger_effect(cluster_t *cluster);
} /* command */
} /* identify */

namespace groups {
namespace command {
command_t *create_add_group(cluster_t *cluster);
command_t *create_view_group(cluster_t *cluster);
command_t *create_get_group_membership(cluster_t *cluster);
command_t *create_remove_group(cluster_t *cluster);
command_t *create_remove_all_groups(cluster_t *cluster);
command_t *create_add_group_if_identifying(cluster_t *cluster);
command_t *create_add_group_response(cluster_t *cluster);
command_t *create_view_group_response(cluster_t *cluster);
command_t *create_get_group_membership_response(cluster_t *cluster);
command_t *create_remove_group_response(cluster_t *cluster);
} /* command */
} /* groups */

namespace icd_management {
namespace command {
command_t *create_register_client(cluster_t *cluster);
command_t *create_register_client_response(cluster_t *cluster);
command_t *create_unregister_client(cluster_t *cluster);
command_t *create_stay_active_request(cluster_t *cluster);
} /* command */
} /* icd_management */

namespace scenes_management {
namespace command {
command_t *create_add_scene(cluster_t *cluster);
command_t *create_view_scene(cluster_t *cluster);
command_t *create_remove_scene(cluster_t *cluster);
command_t *create_remove_all_scenes(cluster_t *cluster);
command_t *create_store_scene(cluster_t *cluster);
command_t *create_recall_scene(cluster_t *cluster);
command_t *create_get_scene_membership(cluster_t *cluster);
command_t *create_copy_scene(cluster_t *cluster);
command_t *create_add_scene_response(cluster_t *cluster);
command_t *create_view_scene_response(cluster_t *cluster);
command_t *create_remove_scene_response(cluster_t *cluster);
command_t *create_remove_all_scenes_response(cluster_t *cluster);
command_t *create_store_scene_response(cluster_t *cluster);
command_t *create_get_scene_membership_response(cluster_t *cluster);
command_t *create_copy_scene_response(cluster_t *cluster);
} /* command */
} /* scenes_management */

namespace on_off {
namespace command {
command_t *create_off(cluster_t *cluster);
command_t *create_on(cluster_t *cluster);
command_t *create_toggle(cluster_t *cluster);
command_t *create_off_with_effect(cluster_t *cluster);
command_t *create_on_with_recall_global_scene(cluster_t *cluster);
command_t *create_on_with_timed_off(cluster_t *cluster);
} /* command */
} /* on_off */

namespace level_control {
namespace command {
command_t *create_move_to_level(cluster_t *cluster);
command_t *create_move(cluster_t *cluster);
command_t *create_step(cluster_t *cluster);
command_t *create_stop(cluster_t *cluster);
command_t *create_move_to_level_with_on_off(cluster_t *cluster);
command_t *create_move_with_on_off(cluster_t *cluster);
command_t *create_step_with_on_off(cluster_t *cluster);
command_t *create_stop_with_on_off(cluster_t *cluster);
command_t *create_move_to_closest_frequency(cluster_t *cluster);
} /* command */
} /* level_control */

namespace color_control {
namespace command {
command_t *create_move_to_hue(cluster_t *cluster);
command_t *create_move_hue(cluster_t *cluster);
command_t *create_step_hue(cluster_t *cluster);
command_t *create_move_to_saturation(cluster_t *cluster);
command_t *create_move_saturation(cluster_t *cluster);
command_t *create_step_saturation(cluster_t *cluster);
command_t *create_move_to_hue_and_saturation(cluster_t *cluster);
command_t *create_stop_move_step(cluster_t *cluster);
command_t *create_move_to_color_temperature(cluster_t *cluster);
command_t *create_move_color_temperature(cluster_t *cluster);
command_t *create_step_color_temperature(cluster_t *cluster);
command_t *create_move_to_color(cluster_t *cluster);
command_t *create_move_color(cluster_t *cluster);
command_t *create_step_color(cluster_t *cluster);
command_t *create_enhanced_move_to_hue(cluster_t *cluster);
command_t *create_enhanced_move_hue(cluster_t *cluster);
command_t *create_enhanced_step_hue(cluster_t *cluster);
command_t *create_enhanced_move_to_hue_and_saturation(cluster_t *cluster);
command_t *create_color_loop_set(cluster_t *cluster);
} /* command */
} /* color_control */

namespace thermostat {
namespace command {
command_t *create_setpoint_raise_lower(cluster_t *cluster);
command_t *create_set_weekly_schedule(cluster_t *cluster);
command_t *create_get_weekly_schedule(cluster_t *cluster);
command_t *create_clear_weekly_schedule(cluster_t *cluster);
command_t *create_get_weekly_schedule_response(cluster_t *cluster);
command_t *create_set_active_schedule_request(cluster_t *cluster);
command_t *create_set_active_preset_request(cluster_t *cluster);
} /* command */
} /* thermostat */

namespace operational_state {
namespace command {
command_t *create_pause(cluster_t *cluster);
command_t *create_stop(cluster_t *cluster);
command_t *create_start(cluster_t *cluster);
command_t *create_resume(cluster_t *cluster);
command_t *create_operational_command_response(cluster_t *cluster);
} /* command */
} /* operational_state */

namespace smoke_co_alarm {
namespace command {
command_t *create_self_test_request(cluster_t *cluster);
} /* command */
} /* smoke_co_alarm */

namespace door_lock {
namespace command {
command_t *create_lock_door(cluster_t *cluster);
command_t *create_unlock_door(cluster_t *cluster);
command_t *create_unlock_with_timeout(cluster_t *cluster);
command_t *create_set_weekday_schedule(cluster_t *cluster);
command_t *create_get_weekday_schedule(cluster_t *cluster);
command_t *create_get_weekday_schedule_response(cluster_t *cluster);
command_t *create_clear_weekday_schedule(cluster_t *cluster);
command_t *create_set_year_day_schedule(cluster_t *cluster);
command_t *create_get_year_day_schedule(cluster_t *cluster);
command_t *create_get_year_day_schedule_response(cluster_t *cluster);
command_t *create_clear_year_day_schedule(cluster_t *cluster);
command_t *create_set_holiday_schedule(cluster_t *cluster);
command_t *create_get_holiday_schedule(cluster_t *cluster);
command_t *create_get_holiday_schedule_response(cluster_t *cluster);
command_t *create_clear_holiday_schedule(cluster_t *cluster);
command_t *create_set_user_type(cluster_t *cluster);
command_t *create_get_user_type(cluster_t *cluster);
command_t *create_get_user_type_response(cluster_t *cluster);
command_t *create_set_user(cluster_t *cluster);
command_t *create_get_user(cluster_t *cluster);
command_t *create_get_user_response(cluster_t *cluster);
command_t *create_clear_user(cluster_t *cluster);
command_t *create_set_credential(cluster_t *cluster);
command_t *create_set_credential_response(cluster_t *cluster);
command_t *create_get_credential_status(cluster_t *cluster);
command_t *create_get_credential_status_response(cluster_t *cluster);
command_t *create_clear_credential(cluster_t *cluster);
command_t *create_unbolt_door(cluster_t *cluster);
command_t *create_set_aliro_reader_config(cluster_t *cluster);
command_t *create_clear_aliro_reader_config(cluster_t *cluster);
} /* command */
} /* door_lock */

namespace window_covering {
namespace command {
command_t *create_up_or_open(cluster_t *cluster);
command_t *create_down_or_close(cluster_t *cluster);
command_t *create_stop_motion(cluster_t *cluster);
command_t *create_go_to_lift_value(cluster_t *cluster);
command_t *create_go_to_lift_percentage(cluster_t *cluster);
command_t *create_go_to_tilt_value(cluster_t *cluster);
command_t *create_go_to_tilt_percentage(cluster_t *cluster);
} /* command */
} /* window_covering */

namespace mode_select {
namespace command {
command_t *create_change_to_mode(cluster_t *cluster);
} /* command */
} /* mode_select */

namespace temperature_control {
namespace command {
command_t *create_set_temperature(cluster_t *cluster);
} /* command */
} /* temperature_control */

namespace fan_control {
namespace command {
command_t *create_step(cluster_t *cluster);
} /* command */
} /* fan_control */

namespace hepa_filter_monitoring {
namespace command {
command_t *create_reset_condition(cluster_t *cluster);
} /* command */
} /* hepa_filter_monitoring */

namespace activated_carbon_filter_monitoring {
namespace command {
command_t *create_reset_condition(cluster_t *cluster);
} /* command */
} /* activated_carbon_filter_monitoring */

namespace mode_base {
namespace command {
command_t *create_change_to_mode(cluster_t *cluster);
command_t *create_change_to_mode_response(cluster_t *cluster);
} /* command */
} /* mode_base */

namespace keypad_input {
namespace command {
command_t *create_send_key(cluster_t *cluster);
command_t *create_send_key_response(cluster_t *cluster);
} /* command */
} /* keypad_input */

namespace boolean_state_configuration {
namespace command {
command_t *create_suppress_alarm(cluster_t *cluster);
command_t *create_enable_disable_alarm(cluster_t *cluster);
} /* command */
} /* boolean_state_configuration */

namespace energy_evse {
namespace command {
command_t *create_disable(cluster_t *cluster);
command_t *create_enable_charging(cluster_t *cluster);
command_t *create_enable_discharging(cluster_t *cluster);
command_t *create_start_diagnostics(cluster_t *cluster);
command_t *create_set_targets(cluster_t *cluster);
command_t *create_get_targets(cluster_t *cluster);
command_t *create_clear_targets(cluster_t *cluster);
command_t *create_get_targets_response(cluster_t *cluster);
} /* command */
} /* energy_evse */

namespace microwave_oven_control {
namespace command {
command_t *create_set_cooking_parameters(cluster_t *cluster);
command_t *create_add_more_time(cluster_t *cluster);
} /* command */
} /* microwave_oven_control */

namespace valve_configuration_and_control {
namespace command {
command_t *create_open(cluster_t *cluster);
command_t *create_close(cluster_t *cluster);
} /* command */
} /* valve_configuration_and_control */

namespace device_energy_management {
namespace command {
command_t *create_power_adjust_request(cluster_t *cluster);
command_t *create_cancel_power_adjust_request(cluster_t *cluster);
command_t *create_start_time_adjust_request(cluster_t *cluster);
command_t *create_pause_request(cluster_t *cluster);
command_t *create_resume_request(cluster_t *cluster);
command_t *create_modify_forecast_request(cluster_t *cluster);
command_t *create_request_constraint_based_forecast(cluster_t *cluster);
command_t *create_cancel_request(cluster_t *cluster);
} /* command */
} /* device_energy_management */

namespace thread_border_router_management {
namespace command {
command_t *create_get_active_dataset_request(cluster_t *cluster);
command_t *create_get_pending_dataset_request(cluster_t *cluster);
command_t *create_dataset_response(cluster_t *cluster);
command_t *create_set_active_dataset_request(cluster_t *cluster);
command_t *create_set_pending_dataset_request(cluster_t *cluster);
} /* command */
} /* thread_border_router_management */

namespace wifi_network_management {
namespace command {
command_t *create_network_passphrase_request(cluster_t *cluster);
command_t *create_network_passphrase_response(cluster_t *cluster);
} /* command */
} /* wifi_network_management */

namespace thread_network_directory {
namespace command {
command_t *create_add_network(cluster_t *cluster);
command_t *create_remove_network(cluster_t *cluster);
command_t *create_get_operational_dataset(cluster_t *cluster);
command_t *create_operational_dataset_response(cluster_t *cluster);
} /* command */
} /* thread_network_directory */

namespace service_area {
namespace command {
command_t *create_select_areas(cluster_t *cluster);
command_t *create_select_areas_response(cluster_t *cluster);
command_t *create_skip_area(cluster_t *cluster);
command_t *create_skip_area_response(cluster_t *cluster);
} /* command */
} /* service_area */

namespace water_heater_management {
namespace command {
command_t *create_boost(cluster_t *cluster);
command_t *create_cancel_boost(cluster_t *cluster);
} /* command */
} /* water_heater_management */

namespace commissioner_control {
namespace command {
command_t *create_request_commissioning_approval(cluster_t *cluster);
command_t *create_commission_node(cluster_t *cluster);
command_t *create_reverse_open_commissioning_window(cluster_t *cluster);
} /* command */
} /* commissioner_control */

namespace time_synchronization {
namespace command {
command_t *create_set_utc_time(cluster_t *cluster);
command_t *create_set_trusted_time_source(cluster_t *cluster);
command_t *create_set_time_zone(cluster_t *cluster);
command_t *create_set_time_zone_response(cluster_t *cluster);
command_t *create_set_dst_offset(cluster_t *cluster);
command_t *create_set_default_ntp(cluster_t *cluster);
} /* command */
} /* time_synchronization */

} /* cluster */
} /* esp_matter */
