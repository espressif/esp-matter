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

namespace access_control {
namespace attribute {
attribute_t *create_acl(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count);
attribute_t *create_extension(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count);
attribute_t *create_subjects_per_access_control_entry(cluster_t *cluster, uint16_t value);
attribute_t *create_targets_per_access_control_entry(cluster_t *cluster, uint16_t value);
attribute_t *create_access_control_entries_per_fabric(cluster_t *cluster, uint16_t value);
} /* attribute */
} /* access_control */

namespace basic {
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
attribute_t *create_manufacturing_date(cluster_t *cluster, char *value, uint16_t length);
attribute_t *create_part_number(cluster_t *cluster, char *value, uint16_t length);
attribute_t *create_product_url(cluster_t *cluster, char *value, uint16_t length);
attribute_t *create_product_label(cluster_t *cluster, char *value, uint16_t length);
attribute_t *create_serial_number(cluster_t *cluster, char *value, uint16_t length);
attribute_t *create_local_config_diabled(cluster_t *cluster, bool value);
attribute_t *create_reachable(cluster_t *cluster, bool value);
attribute_t *create_unique_id(cluster_t *cluster, char *value, uint16_t length);
} /* attribute */
} /* basic */

namespace binding {
namespace attribute {
attribute_t *create_binding(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count);
} /* attribute */
} /* binding */

namespace ota_requestor {
namespace attribute {
attribute_t *create_default_ota_providers(cluster_t *cluster, uint8_t *value, uint16_t length);
attribute_t *create_update_possible(cluster_t *cluster, bool value);
attribute_t *create_update_state(cluster_t *cluster, uint8_t value);
attribute_t *create_update_state_progress(cluster_t *cluster, uint8_t value);
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
attribute_t *create_last_networking_status(cluster_t *cluster, uint8_t value);
attribute_t *create_last_network_id(cluster_t *cluster, uint8_t *value, uint16_t length);
attribute_t *create_last_connect_error_value(cluster_t *cluster, uint32_t value);
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
attribute_t *create_security_type(cluster_t *cluster, uint8_t value);
attribute_t *create_wifi_version(cluster_t *cluster, uint8_t value);
attribute_t *create_channel_number(cluster_t *cluster, uint16_t value);
attribute_t *create_rssi(cluster_t *cluster, int8_t value);
attribute_t *create_beacon_lost_count(cluster_t *cluster, uint32_t value);
attribute_t *create_beacon_rx_count(cluster_t *cluster, uint32_t value);
attribute_t *create_packet_multicast_rx_count(cluster_t *cluster, uint32_t value);
attribute_t *create_packet_multicast_tx_count(cluster_t *cluster, uint32_t value);
attribute_t *create_packet_unicast_rx_count(cluster_t *cluster, uint32_t value);
attribute_t *create_packet_unicast_tx_count(cluster_t *cluster, uint32_t value);
attribute_t *create_current_max_rate(cluster_t *cluster, uint64_t value);
attribute_t *create_overrun_count(cluster_t *cluster, uint64_t value);
} /* attribute */
} /* diagnostics_network_wifi */

namespace diagnostics_network_thread {
namespace attribute {
attribute_t *create_channel(cluster_t *cluster, uint16_t value);
attribute_t *create_routing_role(cluster_t *cluster, uint8_t value);
attribute_t *create_network_name(cluster_t *cluster, char *value, uint16_t length);
attribute_t *create_pan_id(cluster_t *cluster, uint16_t value);
attribute_t *create_extended_pan_id(cluster_t *cluster, uint64_t value);
attribute_t *create_mesh_local_prefix(cluster_t *cluster, uint8_t *value, uint16_t length);
attribute_t *create_neighbor_table(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count);
attribute_t *create_route_table(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count);
attribute_t *create_extended_partition_id(cluster_t *cluster, uint32_t value);
attribute_t *create_weighting(cluster_t *cluster, uint8_t value);
attribute_t *create_data_version(cluster_t *cluster, uint8_t value);
attribute_t *create_stable_data_version(cluster_t *cluster, uint8_t value);
attribute_t *create_leader_router_id(cluster_t *cluster, uint8_t value);
attribute_t *create_security_policy(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count);
attribute_t *create_channel_mask(cluster_t *cluster, uint8_t *value, uint16_t length);
attribute_t *create_operational_dataset_components(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count);
attribute_t *create_active_network_faults(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count);
} /* attribute */
} /* diagnostics_network_thread */

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
attribute_t *create_on_time(cluster_t *cluster, uint16_t value);
attribute_t *create_off_wait_time(cluster_t *cluster, uint16_t value);
attribute_t *create_start_up_on_off(cluster_t *cluster, uint8_t value);
} /* attribute */
} /* on_off */

namespace level_control {
namespace attribute {
attribute_t *create_current_level(cluster_t *cluster, uint8_t value);
attribute_t *create_on_level(cluster_t *cluster, uint8_t value);
attribute_t *create_options(cluster_t *cluster, uint8_t value, uint8_t min, uint8_t max);
attribute_t *create_remaining_time(cluster_t *cluster, uint16_t value);
attribute_t *create_min_level(cluster_t *cluster, uint8_t value);
attribute_t *create_max_level(cluster_t *cluster, uint8_t value);
attribute_t *create_current_frequency(cluster_t *cluster, uint16_t value);
attribute_t *create_min_frequency(cluster_t *cluster, uint16_t value);
attribute_t *create_max_frequency(cluster_t *cluster, uint16_t value);
attribute_t *create_on_off_transition_time(cluster_t *cluster, uint16_t value);
attribute_t *create_on_transition_time(cluster_t* cluster, uint16_t value);
attribute_t *create_off_transition_time(cluster_t* cluster, uint16_t value);
attribute_t *create_default_move_rate(cluster_t* cluster, uint8_t value);
attribute_t *create_start_up_current_level(cluster_t *cluster, uint8_t value);
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
attribute_t *create_startup_color_temperature_mireds(cluster_t *cluster, uint16_t value);
attribute_t *create_current_x(cluster_t *cluster, uint16_t value);
attribute_t *create_current_y(cluster_t *cluster, uint16_t value);
attribute_t *create_enhanced_current_hue(cluster_t *cluster, uint16_t value);
attribute_t *create_color_loop_active(cluster_t *cluster, uint8_t value);
attribute_t *create_color_loop_direction(cluster_t *cluster, uint8_t value);
attribute_t *create_color_loop_time(cluster_t *cluster, uint16_t value);
attribute_t *create_color_loop_start_enhanced_hue(cluster_t *cluster, uint16_t value);
attribute_t *create_color_loop_stored_enhanced_hue(cluster_t *cluster, uint16_t value);
} /* attribute */
} /* color_control */

namespace fan_control {
namespace attribute {
attribute_t *create_fan_mode(cluster_t *cluster, uint8_t value);
attribute_t *create_fan_mode_sequence(cluster_t *cluster, uint8_t value);
} /* attribute */
} /* fan_control */

namespace thermostat {
namespace attribute {
attribute_t *create_local_temperature(cluster_t *cluster, uint16_t value);
attribute_t *create_occupied_cooling_setpoint(cluster_t *cluster, uint16_t value);
attribute_t *create_occupied_heating_setpoint(cluster_t *cluster, uint16_t value);
attribute_t *create_control_sequence_of_operation(cluster_t *cluster, uint8_t value, uint8_t min, uint8_t max);
attribute_t *create_system_mode(cluster_t *cluster, uint8_t value, uint8_t min, uint8_t max);
} /* attribute */
} /* thermostat */

namespace door_lock {
namespace attribute {
attribute_t *create_lock_state(cluster_t *cluster, uint8_t value);
attribute_t *create_lock_type(cluster_t *cluster, uint8_t value);
attribute_t *create_actuator_enabled(cluster_t *cluster, bool value);
attribute_t *create_auto_relock_time(cluster_t *cluster, uint32_t value);
attribute_t *create_operating_mode(cluster_t *cluster, uint8_t value, uint8_t min, uint8_t max);
attribute_t *create_supported_operating_modes(cluster_t *cluster, uint16_t value);
} /* attribute */
} /* door_lock */

namespace bridged_device_basic {
namespace attribute {
attribute_t *create_bridged_device_basic_node_label(cluster_t *cluster, char *value, uint16_t length);
attribute_t *create_reachable(cluster_t *cluster, bool value);
} /* attribute */
} /* bridged_device_basic */

namespace fixed_label {
namespace attribute {
attribute_t *create_label_list(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count);
} /* attribute */
} /* fixed_label */

namespace switch_cluster {
namespace attribute {
attribute_t *create_number_of_positions(cluster_t *cluster, uint8_t value);
attribute_t *create_current_position(cluster_t *cluster, uint8_t value);
attribute_t *create_multi_press_max(cluster_t *cluster, uint8_t value);
} /* attribute */
} /* switch_cluster */

namespace temperature_measurement {
namespace attribute {
attribute_t *create_temperature_measured_value(cluster_t *cluster, int16_t value);
attribute_t *create_temperature_min_measured_value(cluster_t *cluster, int16_t value);
attribute_t *create_temperature_max_measured_value(cluster_t *cluster, int16_t value);
} /* attribute */
} /* temperature_measurement */

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

} /* cluster */
} /* esp_matter */
