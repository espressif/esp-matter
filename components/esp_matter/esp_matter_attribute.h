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
#include <esp_matter_core.h>

namespace esp_matter {
namespace cluster {

/** Specific attribute create APIs
 *
 * If some standard attribute is not present here, it can be added.
 * If a custom attribute needs to be created, the low level esp_matter::attribute::create() API can be used.
 */

namespace global {
namespace attribute {
attribute_t *create_cluster_revision(cluster_t *cluster, uint16_t value);
attribute_t *create_feature_map(cluster_t *cluster, uint32_t value);
attribute_t *create_event_list(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count);
} /* attribute */
} /* global */

namespace descriptor {
namespace attribute {
attribute_t *create_device_list(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count);
attribute_t *create_server_list(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count);
attribute_t *create_client_list(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count);
attribute_t *create_parts_list(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count);
} /* attribute */
} /* descriptor */

namespace actions {
namespace attribute {
attribute_t *create_action_list(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count);
attribute_t *create_endpoint_lists(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count);
attribute_t *create_setup_url(cluster_t *cluster, char *value, uint16_t length);
} /* attribute */
} /* actions */

namespace access_control {
namespace attribute {
attribute_t *create_acl(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count);
attribute_t *create_extension(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count);
attribute_t *create_subjects_per_access_control_entry(cluster_t *cluster, uint16_t value);
attribute_t *create_targets_per_access_control_entry(cluster_t *cluster, uint16_t value);
attribute_t *create_access_control_entries_per_fabric(cluster_t *cluster, uint16_t value);
} /* attribute */
} /* access_control */

namespace basic_information {
namespace attribute {
attribute_t *create_data_model_revision(cluster_t *cluster, uint16_t value);
attribute_t *create_vendor_name(cluster_t *cluster, char *value, uint16_t length);
attribute_t *create_vendor_id(cluster_t *cluster, uint16_t value);
attribute_t *create_product_name(cluster_t *cluster, char *value, uint16_t length);
attribute_t *create_product_id(cluster_t *cluster, uint16_t value);
attribute_t *create_node_label(cluster_t *cluster, char *value, uint16_t length);
attribute_t *create_location(cluster_t *cluster, char *value, uint16_t length);
attribute_t *create_hardware_version(cluster_t *cluster, uint16_t value);
attribute_t *create_hardware_version_string(cluster_t *cluster, char *value, uint16_t length);
attribute_t *create_software_version(cluster_t *cluster, uint32_t value);
attribute_t *create_software_version_string(cluster_t *cluster, char *value, uint16_t length);
attribute_t *create_capability_minima(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count);

/** These attributes are optional for the cluster, but when added to this cluster, the value is maintained internally.
  * If the attributes are added in some other cluster, then the value is not maintained internally.
  **/
attribute_t *create_manufacturing_date(cluster_t *cluster, char *value, uint16_t length);
attribute_t *create_part_number(cluster_t *cluster, char *value, uint16_t length);
attribute_t *create_product_url(cluster_t *cluster, char *value, uint16_t length);
attribute_t *create_product_label(cluster_t *cluster, char *value, uint16_t length);
attribute_t *create_serial_number(cluster_t *cluster, char *value, uint16_t length);
attribute_t *create_local_config_diabled(cluster_t *cluster, bool value);
attribute_t *create_reachable(cluster_t *cluster, bool value);
attribute_t *create_unique_id(cluster_t *cluster, char *value, uint16_t length);
} /* attribute */
} /* basic_information */

namespace binding {
namespace attribute {
attribute_t *create_binding(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count);
} /* attribute */
} /* binding */

namespace ota_requestor {
namespace attribute {
attribute_t *create_default_ota_providers(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count);
attribute_t *create_update_possible(cluster_t *cluster, bool value);
attribute_t *create_update_state(cluster_t *cluster, uint8_t value);
attribute_t *create_update_state_progress(cluster_t *cluster, nullable<uint8_t> value);
} /* attribute */
} /* ota_requestor */

namespace general_commissioning {
namespace attribute {
attribute_t *create_breadcrumb(cluster_t *cluster, uint64_t value);
attribute_t *create_basic_commissioning_info(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count);
attribute_t *create_regulatory_config(cluster_t *cluster, uint8_t value);
attribute_t *create_location_capability(cluster_t *cluster, uint8_t value);
attribute_t *create_supports_concurrent_connection(cluster_t *cluster, bool value);
} /* attribute */
} /* general_commissioning */

namespace network_commissioning {
namespace attribute {
attribute_t *create_max_networks(cluster_t *cluster, uint8_t value);
attribute_t *create_networks(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count);
attribute_t *create_scan_max_time_seconds(cluster_t *cluster, uint8_t value);
attribute_t *create_connect_max_time_seconds(cluster_t *cluster, uint8_t value);
attribute_t *create_interface_enabled(cluster_t *cluster, bool value);
attribute_t *create_last_networking_status(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_last_network_id(cluster_t *cluster, uint8_t *value, uint16_t length);
attribute_t *create_last_connect_error_value(cluster_t *cluster, nullable<int32_t> value);
} /* attribute */
} /* network_commissioning */

namespace general_diagnostics {
namespace attribute {
attribute_t *create_network_interfaces(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count);
attribute_t *create_reboot_count(cluster_t *cluster, uint16_t value);
attribute_t *create_test_event_triggers_enabled(cluster_t *cluster, bool value);
} /* attribute */
} /* general_diagnostics */

namespace administrator_commissioning {
namespace attribute {
attribute_t *create_window_status(cluster_t *cluster, uint8_t value);
attribute_t *create_admin_fabric_index(cluster_t *cluster, uint16_t value);
attribute_t *create_admin_vendor_id(cluster_t *cluster, uint16_t value);
} /* attribute */
} /* administrator_commissioning */

namespace operational_credentials {
namespace attribute {
attribute_t *create_nocs(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count);
attribute_t *create_fabrics(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count);
attribute_t *create_supported_fabrics(cluster_t *cluster, uint8_t value);
attribute_t *create_commissioned_fabrics(cluster_t *cluster, uint8_t value);
attribute_t *create_trusted_root_certificates(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count);
attribute_t *create_current_fabric_index(cluster_t *cluster, uint8_t value);
} /* attribute */
} /* operational_credentials */

namespace group_key_management {
namespace attribute {
attribute_t *create_group_key_map(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count);
attribute_t *create_group_table(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count);
attribute_t *create_max_groups_per_fabric(cluster_t *cluster, uint16_t value);
attribute_t *create_max_group_keys_per_fabric(cluster_t *cluster, uint16_t value);
} /* attribute */
} /* group_key_management */

namespace diagnostics_network_wifi {
namespace attribute {
attribute_t *create_bssid(cluster_t *cluster, uint8_t *value, uint16_t length);
attribute_t *create_security_type(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_wifi_version(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_channel_number(cluster_t *cluster, nullable<uint16_t> value);
attribute_t *create_rssi(cluster_t *cluster, nullable<int8_t> value);

/** These attributes are optional for the cluster, but when added to this cluster, the value is maintained internally.
  * If the attributes are added in some other cluster, then the value is not maintained internally.
  **/
attribute_t *create_beacon_lost_count(cluster_t *cluster, nullable<uint32_t> value);
attribute_t *create_beacon_rx_count(cluster_t *cluster, nullable<uint32_t> value);
attribute_t *create_packet_multicast_rx_count(cluster_t *cluster, nullable<uint32_t> value);
attribute_t *create_packet_multicast_tx_count(cluster_t *cluster, nullable<uint32_t> value);
attribute_t *create_packet_unicast_rx_count(cluster_t *cluster, nullable<uint32_t> value);
attribute_t *create_packet_unicast_tx_count(cluster_t *cluster, nullable<uint32_t> value);
attribute_t *create_current_max_rate(cluster_t *cluster, nullable<uint64_t> value);
attribute_t *create_overrun_count(cluster_t *cluster, nullable<uint64_t> value);
} /* attribute */
} /* diagnostics_network_wifi */

namespace diagnostics_network_thread {
namespace attribute {
attribute_t *create_channel(cluster_t *cluster, nullable<uint16_t> value);
attribute_t *create_routing_role(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_network_name(cluster_t *cluster, char *value, uint16_t length);
attribute_t *create_pan_id(cluster_t *cluster, nullable<uint16_t> value);
attribute_t *create_extended_pan_id(cluster_t *cluster, nullable<uint64_t> value);
attribute_t *create_mesh_local_prefix(cluster_t *cluster, uint8_t *value, uint16_t length);
attribute_t *create_neighbor_table(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count);
attribute_t *create_route_table(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count);
attribute_t *create_partition_id(cluster_t *cluster, nullable<uint32_t> value);
attribute_t *create_weighting(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_data_version(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_stable_data_version(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_leader_router_id(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_security_policy(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count);
attribute_t *create_channel_page0_mask(cluster_t *cluster, uint8_t *value, uint16_t length);
attribute_t *create_operational_dataset_components(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count);
attribute_t *create_active_network_faults(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count);
} /* attribute */
} /* diagnostics_network_thread */

namespace bridged_device_basic_information {
namespace attribute {
attribute_t *create_node_label(cluster_t *cluster, char *value, uint16_t length);
attribute_t *create_reachable(cluster_t *cluster, bool value);
} /* attribute */
} /* bridged_device_basic_information */

namespace user_label {
namespace attribute {
attribute_t *create_label_list(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count);
} /* attribute */
} /* user_label */

namespace fixed_label {
namespace attribute {
attribute_t *create_label_list(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count);
} /* attribute */
} /* fixed_label */

namespace identify {
namespace attribute {
attribute_t *create_identify_time(cluster_t *cluster, uint16_t value, uint16_t min, uint16_t max);
attribute_t *create_identify_type(cluster_t *cluster, uint8_t value);
} /* attribute */
} /* identify */

namespace groups {
namespace attribute {
attribute_t *create_group_name_support(cluster_t *cluster, uint8_t value);
} /* attribute */
} /* groups */

namespace scenes {
namespace attribute {
attribute_t *create_scene_count(cluster_t *cluster, uint8_t value);
attribute_t *create_current_scene(cluster_t *cluster, uint8_t value);
attribute_t *create_current_group(cluster_t *cluster, uint16_t value);
attribute_t *create_scene_valid(cluster_t *cluster, bool value);
attribute_t *create_scene_name_support(cluster_t *cluster, uint8_t value);
} /* attribute */
} /* scenes */

namespace on_off {
namespace attribute {
attribute_t *create_on_off(cluster_t *cluster, bool value);
attribute_t *create_global_scene_control(cluster_t *cluster, bool value);
attribute_t *create_on_time(cluster_t *cluster, nullable<uint16_t> value);
attribute_t *create_off_wait_time(cluster_t *cluster, nullable<uint16_t> value);
attribute_t *create_start_up_on_off(cluster_t *cluster, nullable<uint8_t> value);
} /* attribute */
} /* on_off */

namespace level_control {
namespace attribute {
attribute_t *create_current_level(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_on_level(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_options(cluster_t *cluster, uint8_t value, uint8_t min, uint8_t max);
attribute_t *create_remaining_time(cluster_t *cluster, uint16_t value);
attribute_t *create_min_level(cluster_t *cluster, uint8_t value);
attribute_t *create_max_level(cluster_t *cluster, uint8_t value);
attribute_t *create_current_frequency(cluster_t *cluster, uint16_t value);
attribute_t *create_min_frequency(cluster_t *cluster, uint16_t value);
attribute_t *create_max_frequency(cluster_t *cluster, uint16_t value);
attribute_t *create_on_off_transition_time(cluster_t *cluster, uint16_t value);
attribute_t *create_on_transition_time(cluster_t* cluster, nullable<uint16_t> value);
attribute_t *create_off_transition_time(cluster_t* cluster, nullable<uint16_t> value);
attribute_t *create_default_move_rate(cluster_t* cluster, nullable<uint8_t> value);
attribute_t *create_start_up_current_level(cluster_t *cluster, nullable<uint8_t> value);
} /* attribute */
} /* level_control */

namespace color_control {
namespace attribute {
attribute_t *create_current_hue(cluster_t *cluster, uint8_t value);
attribute_t *create_current_saturation(cluster_t *cluster, uint8_t value);
attribute_t *create_remaining_time(cluster_t *cluster, uint16_t value);
attribute_t *create_color_mode(cluster_t *cluster, uint8_t value);
attribute_t *create_color_control_options(cluster_t *cluster, uint8_t value);
attribute_t *create_enhanced_color_mode(cluster_t *cluster, uint8_t value);
attribute_t *create_color_capabilities(cluster_t *cluster, uint16_t value);
attribute_t *create_color_temperature_mireds(cluster_t *cluster, uint16_t value);
attribute_t *create_color_temp_physical_min_mireds(cluster_t *cluster, uint16_t value);
attribute_t *create_color_temp_physical_max_mireds(cluster_t *cluster, uint16_t value);
attribute_t *create_couple_color_temp_to_level_min_mireds(cluster_t *cluster, uint16_t value);
attribute_t *create_startup_color_temperature_mireds(cluster_t *cluster, nullable<uint16_t> value);
attribute_t *create_current_x(cluster_t *cluster, uint16_t value);
attribute_t *create_current_y(cluster_t *cluster, uint16_t value);
attribute_t *create_enhanced_current_hue(cluster_t *cluster, uint16_t value);
attribute_t *create_color_loop_active(cluster_t *cluster, uint8_t value);
attribute_t *create_color_loop_direction(cluster_t *cluster, uint8_t value);
attribute_t *create_color_loop_time(cluster_t *cluster, uint16_t value);
attribute_t *create_color_loop_start_enhanced_hue(cluster_t *cluster, uint16_t value);
attribute_t *create_color_loop_stored_enhanced_hue(cluster_t *cluster, uint16_t value);
attribute_t *create_number_of_primaries(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_primary_n_x(cluster_t * cluster, uint16_t value, uint8_t index);
attribute_t *create_primary_n_y(cluster_t * cluster, uint16_t value, uint8_t index);
attribute_t *create_primary_n_intensity(cluster_t * cluster, nullable<uint8_t> value, uint8_t index);
} /* attribute */
} /* color_control */

namespace fan_control {
namespace attribute {
attribute_t *create_fan_mode(cluster_t *cluster, uint8_t value, uint8_t min, uint8_t max);
attribute_t *create_fan_mode_sequence(cluster_t *cluster, uint8_t value);
attribute_t *create_percent_setting(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_percent_current(cluster_t *cluster, uint8_t value);
} /* attribute */
} /* fan_control */

namespace thermostat {
namespace attribute {
attribute_t *create_local_temperature(cluster_t *cluster, nullable<int16_t> value);
attribute_t *create_outdoor_temperature(cluster_t *cluster, nullable<int16_t> value);
attribute_t *create_occupancy(cluster_t *cluster, uint8_t value);
attribute_t *create_abs_min_heat_setpoint_limit(cluster_t *cluster, int16_t value);
attribute_t *create_abs_max_heat_setpoint_limit(cluster_t *cluster, int16_t value);
attribute_t *create_abs_min_cool_setpoint_limit(cluster_t *cluster, int16_t value);
attribute_t *create_abs_max_cool_setpoint_limit(cluster_t *cluster, int16_t value);
attribute_t *create_pi_cooling_demand(cluster_t *cluster, uint8_t value);
attribute_t *create_pi_heating_demand(cluster_t *cluster, uint8_t value);
attribute_t *create_hvac_system_type_config(cluster_t *cluster, uint8_t value);
attribute_t *create_local_temperature_calibration(cluster_t *cluster, int8_t value);
attribute_t *create_occupied_cooling_setpoint(cluster_t *cluster, int16_t value);
attribute_t *create_occupied_heating_setpoint(cluster_t *cluster, int16_t value);
attribute_t *create_unoccupied_cooling_setpoint(cluster_t *cluster, int16_t value);
attribute_t *create_unoccupied_heating_setpoint(cluster_t *cluster, int16_t value);
attribute_t *create_min_heat_setpoint_limit(cluster_t *cluster, int16_t value);
attribute_t *create_max_heat_setpoint_limit(cluster_t *cluster, int16_t value);
attribute_t *create_min_cool_setpoint_limit(cluster_t *cluster, int16_t value);
attribute_t *create_max_cool_setpoint_limit(cluster_t *cluster, int16_t value);
attribute_t *create_min_setpoint_dead_band(cluster_t *cluster, int8_t value);
attribute_t *create_remote_sensing(cluster_t *cluster, uint8_t value);
attribute_t *create_control_sequence_of_operation(cluster_t *cluster, uint8_t value, uint8_t min, uint8_t max);
attribute_t *create_system_mode(cluster_t *cluster, uint8_t value, uint8_t min, uint8_t max);
attribute_t *create_thermostat_running_mode(cluster_t *cluster, uint8_t value);
attribute_t *create_start_of_week(cluster_t *cluster, uint8_t value);
attribute_t *create_number_of_weekly_transitions(cluster_t *cluster, uint8_t value);
attribute_t *create_number_of_daily_transitions(cluster_t *cluster, uint8_t value);
attribute_t *create_temperature_setpoint_hold(cluster_t *cluster, uint8_t value);
attribute_t *create_temperature_setpoint_hold_duration(cluster_t *cluster, nullable<uint16_t> value);
attribute_t *create_thermostat_programming_operation_mode(cluster_t *cluster, uint8_t value);
attribute_t *create_thermostat_running_state(cluster_t *cluster, uint16_t value);
attribute_t *create_setpoint_change_source(cluster_t *cluster, uint8_t value);
attribute_t *create_setpoint_change_amount(cluster_t *cluster, nullable<int16_t> value);
attribute_t *create_setpoint_change_source_timestamp(cluster_t *cluster, uint16_t value);
attribute_t *create_occupied_setback(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_occupied_setback_min(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_occupied_setback_max(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_unoccupied_setback(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_unoccupied_setback_min(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_unoccupied_setback_max(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_emergency_heat_delta(cluster_t *cluster, uint8_t value);
attribute_t *create_ac_type(cluster_t *cluster, uint8_t value);
attribute_t *create_ac_capacity(cluster_t *cluster, uint16_t value);
attribute_t *create_ac_refrigerant_type(cluster_t *cluster, uint8_t value);
attribute_t *create_ac_compressor_type(cluster_t *cluster, uint8_t value);
attribute_t *create_ac_error_code(cluster_t *cluster, uint32_t value);
attribute_t *create_ac_louver_position(cluster_t *cluster, uint8_t value);
attribute_t *create_ac_coil_temperature(cluster_t *cluster, nullable<int16_t> value);
attribute_t *create_ac_capacity_format(cluster_t *cluster, uint8_t value);
} /* attribute */
} /* thermostat */

namespace door_lock {
namespace attribute {
attribute_t *create_lock_state(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_lock_type(cluster_t *cluster, uint8_t value);
attribute_t *create_actuator_enabled(cluster_t *cluster, bool value);
attribute_t *create_auto_relock_time(cluster_t *cluster, uint32_t value);
attribute_t *create_operating_mode(cluster_t *cluster, uint8_t value, uint8_t min, uint8_t max);
attribute_t *create_supported_operating_modes(cluster_t *cluster, uint16_t value);
} /* attribute */
} /* door_lock */

namespace window_covering {
namespace attribute {
attribute_t *create_type(cluster_t *cluster, uint8_t value);
attribute_t *create_physical_closed_limit_lift(cluster_t *cluster, uint16_t value);
attribute_t *create_physical_closed_limit_tilt(cluster_t *cluster, uint16_t value);
attribute_t *create_current_position_lift(cluster_t *cluster, nullable<uint16_t> value);
attribute_t *create_current_position_tilt(cluster_t *cluster, nullable<uint16_t> value);
attribute_t *create_number_of_actuations_lift(cluster_t *cluster, uint16_t value);
attribute_t *create_number_of_actuations_tilt(cluster_t *cluster, uint16_t value);
attribute_t *create_config_status(cluster_t *cluster, uint8_t value);
attribute_t *create_current_position_lift_percentage(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_current_position_tilt_percentage(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_operational_status(cluster_t *cluster, uint8_t value);
attribute_t *create_target_position_lift_percent_100ths(cluster_t *cluster, nullable<uint16_t> value);
attribute_t *create_target_position_tilt_percent_100ths(cluster_t *cluster, nullable<uint16_t> value);
attribute_t *create_end_product_type(cluster_t *cluster, const uint8_t value);
attribute_t *create_current_position_lift_percent_100ths(cluster_t *cluster, nullable<uint16_t> value);
attribute_t *create_current_position_tilt_percent_100ths(cluster_t *cluster, nullable<uint16_t> value);
attribute_t *create_installed_open_limit_lift(cluster_t *cluster, uint16_t value);
attribute_t *create_installed_closed_limit_lift(cluster_t *cluster, uint16_t value);
attribute_t *create_installed_open_limit_tilt(cluster_t *cluster, uint16_t value);
attribute_t *create_installed_closed_limit_tilt(cluster_t *cluster, uint16_t value);
attribute_t *create_mode(cluster_t *cluster, uint8_t value);
attribute_t *create_safety_status(cluster_t *cluster, uint16_t value);
} /* attribute */
} /* window_covering */

namespace switch_cluster {
namespace attribute {
attribute_t *create_number_of_positions(cluster_t *cluster, uint8_t value);
attribute_t *create_current_position(cluster_t *cluster, uint8_t value);
attribute_t *create_multi_press_max(cluster_t *cluster, uint8_t value);
} /* attribute */
} /* switch_cluster */

namespace temperature_measurement {
namespace attribute {
attribute_t *create_temperature_measured_value(cluster_t *cluster, nullable<int16_t> value);
attribute_t *create_temperature_min_measured_value(cluster_t *cluster, nullable<int16_t> value);
attribute_t *create_temperature_max_measured_value(cluster_t *cluster, nullable<int16_t> value);
} /* attribute */
} /* temperature_measurement */

namespace relative_humidity_measurement {
namespace attribute {
attribute_t *create_relative_humidity_measured_value(cluster_t *cluster, nullable<uint16_t> value);
attribute_t *create_relative_humidity_min_measured_value(cluster_t *cluster, nullable<uint16_t> value);
attribute_t *create_relative_humidity_max_measured_value(cluster_t *cluster, nullable<uint16_t> value);
attribute_t *create_relative_humidity_tolerance(cluster_t *cluster, nullable<uint16_t> value);
} /* attribute */
} /* relative_humidity_measurement */

namespace occupancy_sensing {
namespace attribute {
attribute_t *create_occupancy(cluster_t *cluster, uint8_t value);
attribute_t *create_occupancy_sensor_type(cluster_t *cluster, uint8_t value);
attribute_t *create_occupancy_sensor_type_bitmap(cluster_t *cluster, uint8_t value);
} /* attribute */
} /* occupancy_sensing */

namespace boolean_state {
namespace attribute {
attribute_t *state_value(cluster_t *cluster, bool value);
} /* attribute */
} /* boolean_state */

namespace localization_configuration {
namespace attribute {
attribute_t *create_active_locale(cluster_t *cluster, char *value, uint16_t length);
attribute_t *create_supported_locales(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count);
} /* attribute */
} /* localization_configuration */

namespace time_format_localization {
namespace attribute {
attribute_t *create_hour_format(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_active_calendar_type(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_supported_calendar_types(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count);
} /* attribute */
} /* time_format_localization */

namespace illuminance_measurement {
namespace attribute {
attribute_t *create_illuminance_measured_value(cluster_t *cluster, nullable<uint16_t> value, nullable<uint16_t> min, nullable<uint16_t> max);
attribute_t *create_illuminance_min_measured_value(cluster_t *cluster, nullable<uint16_t> value, nullable<uint16_t> min, nullable<uint16_t> max);
attribute_t *create_illuminance_max_measured_value(cluster_t *cluster, nullable<uint16_t> value, nullable<uint16_t> min, nullable<uint16_t> max);
attribute_t *create_illuminance_tolerance(cluster_t *cluster, uint16_t value, uint16_t min, uint16_t max);
attribute_t *create_illuminance_light_sensor_type(cluster_t *cluster, nullable<uint8_t> value, nullable<uint8_t> min, nullable<uint8_t> max);
} /* attribute */
} /* illuminance_measurement */

namespace pressure_measurement {
namespace attribute {
attribute_t *create_pressure_measured_value(cluster_t *cluster, nullable<int16_t> value);
attribute_t *create_pressure_min_measured_value(cluster_t *cluster, nullable<int16_t> value);
attribute_t *create_pressure_max_measured_value(cluster_t *cluster, nullable<int16_t> value);
attribute_t *create_pressure_tolerance(cluster_t *cluster, uint16_t value, uint16_t min, uint16_t max);
attribute_t *create_pressure_scaled_value(cluster_t *cluster, nullable<int16_t> value);
attribute_t *create_pressure_min_scaled_value(cluster_t *cluster, nullable<int16_t> value);
attribute_t *create_pressure_max_scaled_value(cluster_t *cluster, nullable<int16_t> value);
attribute_t *create_pressure_scaled_tolerance(cluster_t *cluster, uint16_t value, uint16_t min, uint16_t max);
attribute_t *create_pressure_scale(cluster_t *cluster, int8_t value);
} /* attribute */
} /* pressure_measurement */

namespace flow_measurement {
namespace attribute {
attribute_t *create_flow_measured_value(cluster_t *cluster, nullable<uint16_t> value);
attribute_t *create_flow_min_measured_value(cluster_t *cluster, nullable<uint16_t> value);
attribute_t *create_flow_max_measured_value(cluster_t *cluster, nullable<uint16_t> value);
attribute_t *create_flow_tolerance(cluster_t *cluster, uint16_t value, uint16_t min, uint16_t max);
} /* attribute */
} /* flow_measurement */

namespace pump_configuration_and_control {
namespace attribute {
attribute_t *create_max_pressure(cluster_t *cluster, nullable<int16_t> value);
attribute_t *create_max_speed(cluster_t *cluster, nullable<uint16_t> value);
attribute_t *create_max_flow(cluster_t *cluster, nullable<uint16_t> value);
attribute_t *create_min_const_pressure(cluster_t *cluster, nullable<int16_t> value);
attribute_t *create_max_const_pressure(cluster_t *cluster, nullable<int16_t> value);
attribute_t *create_min_comp_pressure(cluster_t *cluster, nullable<int16_t> value);
attribute_t *create_max_comp_pressure(cluster_t *cluster, nullable<int16_t> value);
attribute_t *create_min_const_speed(cluster_t *cluster, nullable<uint16_t> value);
attribute_t *create_max_const_speed(cluster_t *cluster, nullable<uint16_t> value);
attribute_t *create_min_const_flow(cluster_t *cluster, nullable<uint16_t> value);
attribute_t *create_max_const_flow(cluster_t *cluster, nullable<uint16_t> value);
attribute_t *create_min_const_temp(cluster_t *cluster, nullable<int16_t> value);
attribute_t *create_max_const_temp(cluster_t *cluster, nullable<int16_t> value);
attribute_t *create_pump_status(cluster_t *cluster, uint16_t value);
attribute_t *create_effective_operation_mode(cluster_t *cluster, uint8_t value);
attribute_t *create_effective_control_mode(cluster_t *cluster, uint8_t value);
attribute_t *create_capacity(cluster_t *cluster, nullable<int16_t> value);
attribute_t *create_speed(cluster_t *cluster, nullable<uint16_t> value);
attribute_t *create_lifetime_running_hours(cluster_t *cluster, nullable<uint32_t> value);
attribute_t *create_pump_power(cluster_t *cluster, nullable<uint32_t> value);
attribute_t *create_lifetime_energy_consumed(cluster_t *cluster, nullable<uint32_t> value);
attribute_t *create_operation_mode(cluster_t *cluster, uint8_t value);
attribute_t *create_control_mode(cluster_t *cluster, uint8_t value);
} /* attribute */
} /* pump_configuration_and_control */

} /* cluster */
} /* esp_matter */
