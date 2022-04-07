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

/** cluster: global */
esp_matter_attribute_t *esp_matter_attribute_create_cluster_revision(esp_matter_cluster_t *cluster, uint16_t value);
esp_matter_attribute_t *esp_matter_attribute_create_feature_map(esp_matter_cluster_t *cluster, uint32_t value);

/** cluster: descriptor */
esp_matter_attribute_t *esp_matter_attribute_create_device_list(esp_matter_cluster_t *cluster, uint8_t *value,
                                                                uint16_t length, uint16_t count);
esp_matter_attribute_t *esp_matter_attribute_create_server_list(esp_matter_cluster_t *cluster, uint8_t *value,
                                                                uint16_t length, uint16_t count);
esp_matter_attribute_t *esp_matter_attribute_create_client_list(esp_matter_cluster_t *cluster, uint8_t *value,
                                                                uint16_t length, uint16_t count);
esp_matter_attribute_t *esp_matter_attribute_create_parts_list(esp_matter_cluster_t *cluster, uint8_t *value,
                                                               uint16_t length, uint16_t count);

/** cluster: access control */
esp_matter_attribute_t *esp_matter_attribute_create_acl(esp_matter_cluster_t *cluster, uint8_t *value, uint16_t length,
                                                        uint16_t count);
esp_matter_attribute_t *esp_matter_attribute_create_extension(esp_matter_cluster_t *cluster, uint8_t *value,
                                                              uint16_t length, uint16_t count);

/** cluster: basic */
esp_matter_attribute_t *esp_matter_attribute_create_data_model_revision(esp_matter_cluster_t *cluster, uint16_t value);
esp_matter_attribute_t *esp_matter_attribute_create_vendor_name(esp_matter_cluster_t *cluster, char *value,
                                                                uint16_t length);
esp_matter_attribute_t *esp_matter_attribute_create_vendor_id(esp_matter_cluster_t *cluster, uint16_t value);
esp_matter_attribute_t *esp_matter_attribute_create_product_name(esp_matter_cluster_t *cluster, char *value,
                                                                 uint16_t length);
esp_matter_attribute_t *esp_matter_attribute_create_product_id(esp_matter_cluster_t *cluster, uint16_t value);
esp_matter_attribute_t *esp_matter_attribute_create_node_label(esp_matter_cluster_t *cluster, char *value,
                                                               uint16_t length);
esp_matter_attribute_t *esp_matter_attribute_create_location(esp_matter_cluster_t *cluster, char *value,
                                                             uint16_t length);
esp_matter_attribute_t *esp_matter_attribute_create_hardware_version(esp_matter_cluster_t *cluster, uint16_t value);
esp_matter_attribute_t *esp_matter_attribute_create_hardware_version_string(esp_matter_cluster_t *cluster, char *value,
                                                                            uint16_t length);
esp_matter_attribute_t *esp_matter_attribute_create_software_version(esp_matter_cluster_t *cluster, uint32_t value);
esp_matter_attribute_t *esp_matter_attribute_create_software_version_string(esp_matter_cluster_t *cluster, char *value,
                                                                            uint16_t length);

/** cluster: binding */
esp_matter_attribute_t *esp_matter_attribute_create_binding(esp_matter_cluster_t *cluster, uint8_t *value,
                                                            uint16_t length, uint16_t count);

/** cluster: ota requestor */
esp_matter_attribute_t *esp_matter_attribute_create_default_ota_providers(esp_matter_cluster_t *cluster, uint8_t *value,
                                                                          uint16_t length);
esp_matter_attribute_t *esp_matter_attribute_create_update_possible(esp_matter_cluster_t *cluster, bool value);
esp_matter_attribute_t *esp_matter_attribute_create_update_state(esp_matter_cluster_t *cluster, uint8_t value);
esp_matter_attribute_t *esp_matter_attribute_create_update_state_progress(esp_matter_cluster_t *cluster, uint8_t value);

/** cluster: general commissioning */
esp_matter_attribute_t *esp_matter_attribute_create_breadcrumb(esp_matter_cluster_t *cluster, uint64_t value);
esp_matter_attribute_t *esp_matter_attribute_create_basic_commissioning_info(esp_matter_cluster_t *cluster,
                                                                             uint8_t *value, uint16_t length,
                                                                             uint16_t count);
esp_matter_attribute_t *esp_matter_attribute_create_regulatory_config(esp_matter_cluster_t *cluster, uint8_t value);
esp_matter_attribute_t *esp_matter_attribute_create_location_capability(esp_matter_cluster_t *cluster, uint8_t value);

/** cluster: network commissioning */
esp_matter_attribute_t *esp_matter_attribute_create_max_networks(esp_matter_cluster_t *cluster, uint8_t value);
esp_matter_attribute_t *esp_matter_attribute_create_networks(esp_matter_cluster_t *cluster, uint8_t *value,
                                                             uint16_t length, uint16_t count);
esp_matter_attribute_t *esp_matter_attribute_create_scan_max_time_seconds(esp_matter_cluster_t *cluster, uint8_t value);
esp_matter_attribute_t *esp_matter_attribute_create_connect_max_time_seconds(esp_matter_cluster_t *cluster,
                                                                             uint8_t value);
esp_matter_attribute_t *esp_matter_attribute_create_interface_enabled(esp_matter_cluster_t *cluster, bool value);
esp_matter_attribute_t *esp_matter_attribute_create_last_networking_status(esp_matter_cluster_t *cluster,
                                                                           uint8_t value);
esp_matter_attribute_t *esp_matter_attribute_create_last_network_id(esp_matter_cluster_t *cluster, uint8_t *value,
                                                                    uint16_t length);
esp_matter_attribute_t *esp_matter_attribute_create_last_connect_error_value(esp_matter_cluster_t *cluster,
                                                                             uint32_t value);

/** cluster: general diagnostics */
esp_matter_attribute_t *esp_matter_attribute_create_network_interfaces(esp_matter_cluster_t *cluster, uint8_t *value,
                                                                       uint16_t length, uint16_t count);
esp_matter_attribute_t *esp_matter_attribute_create_reboot_count(esp_matter_cluster_t *cluster, uint16_t value);

/** cluster: administrator commissioning */
esp_matter_attribute_t *esp_matter_attribute_create_window_status(esp_matter_cluster_t *cluster, uint8_t value);
esp_matter_attribute_t *esp_matter_attribute_create_admin_fabric_index(esp_matter_cluster_t *cluster, uint16_t value);
esp_matter_attribute_t *esp_matter_attribute_create_admin_vendor_id(esp_matter_cluster_t *cluster, uint16_t value);

/** cluster: operational credentials */
esp_matter_attribute_t *esp_matter_attribute_create_nocs(esp_matter_cluster_t *cluster, uint8_t *value,
                                                         uint16_t length, uint16_t count);
esp_matter_attribute_t *esp_matter_attribute_create_fabrics(esp_matter_cluster_t *cluster, uint8_t *value,
                                                            uint16_t length, uint16_t count);
esp_matter_attribute_t *esp_matter_attribute_create_supported_fabrics(esp_matter_cluster_t *cluster, uint8_t value);
esp_matter_attribute_t *esp_matter_attribute_create_commissioned_fabrics(esp_matter_cluster_t *cluster, uint8_t value);
esp_matter_attribute_t *esp_matter_attribute_create_trusted_root_certificates(esp_matter_cluster_t *cluster,
                                                                              uint8_t *value, uint16_t length,
                                                                              uint16_t count);
esp_matter_attribute_t *esp_matter_attribute_create_current_fabric_index(esp_matter_cluster_t *cluster, uint8_t value);

/** cluster: group key management */
esp_matter_attribute_t *esp_matter_attribute_create_group_key_map(esp_matter_cluster_t *cluster, uint8_t *value,
                                                                  uint16_t length, uint16_t count);
esp_matter_attribute_t *esp_matter_attribute_create_group_table(esp_matter_cluster_t *cluster, uint8_t *value,
                                                                uint16_t length, uint16_t count);
esp_matter_attribute_t *esp_matter_attribute_create_max_groups_per_fabric(esp_matter_cluster_t *cluster,
                                                                          uint16_t value);
esp_matter_attribute_t *esp_matter_attribute_create_max_group_keys_per_fabric(esp_matter_cluster_t *cluster,
                                                                              uint16_t value);

/** cluster: identify */
esp_matter_attribute_t *esp_matter_attribute_create_identify_time(esp_matter_cluster_t *cluster, uint16_t value,
                                                                  uint16_t min, uint16_t max);
esp_matter_attribute_t *esp_matter_attribute_create_identify_type(esp_matter_cluster_t *cluster, uint8_t value);

/** cluster: groups */
esp_matter_attribute_t *esp_matter_attribute_create_group_name_support(esp_matter_cluster_t *cluster, uint8_t value);

/** cluster: scenes */
esp_matter_attribute_t *esp_matter_attribute_create_scene_count(esp_matter_cluster_t *cluster, uint8_t value);
esp_matter_attribute_t *esp_matter_attribute_create_current_scene(esp_matter_cluster_t *cluster, uint8_t value);
esp_matter_attribute_t *esp_matter_attribute_create_current_group(esp_matter_cluster_t *cluster, uint16_t value);
esp_matter_attribute_t *esp_matter_attribute_create_scene_valid(esp_matter_cluster_t *cluster, bool value);
esp_matter_attribute_t *esp_matter_attribute_create_scene_name_support(esp_matter_cluster_t *cluster, uint8_t value);

/** cluster: on off */
esp_matter_attribute_t *esp_matter_attribute_create_on_off(esp_matter_cluster_t *cluster, bool value);
esp_matter_attribute_t *esp_matter_attribute_create_global_scene_control(esp_matter_cluster_t *cluster, bool value);
esp_matter_attribute_t *esp_matter_attribute_create_on_time(esp_matter_cluster_t *cluster, uint16_t value);
esp_matter_attribute_t *esp_matter_attribute_create_off_wait_time(esp_matter_cluster_t *cluster, uint16_t value);
esp_matter_attribute_t *esp_matter_attribute_create_start_up_on_off(esp_matter_cluster_t *cluster, uint8_t value);

/** cluster: level control */
esp_matter_attribute_t *esp_matter_attribute_create_current_level(esp_matter_cluster_t *cluster, uint8_t value);
esp_matter_attribute_t *esp_matter_attribute_create_on_level(esp_matter_cluster_t *cluster, uint8_t value);
esp_matter_attribute_t *esp_matter_attribute_create_options(esp_matter_cluster_t *cluster, uint8_t value, uint8_t min,
                                                            uint8_t max);
esp_matter_attribute_t *esp_matter_attribute_create_remaining_time(esp_matter_cluster_t *cluster, uint16_t value);
esp_matter_attribute_t *esp_matter_attribute_create_min_level(esp_matter_cluster_t *cluster, uint8_t value);
esp_matter_attribute_t *esp_matter_attribute_create_max_level(esp_matter_cluster_t *cluster, uint8_t value);
esp_matter_attribute_t *esp_matter_attribute_create_start_up_current_level(esp_matter_cluster_t *cluster,
                                                                           uint8_t value);

/** cluster: color control */
esp_matter_attribute_t *esp_matter_attribute_create_current_hue(esp_matter_cluster_t *cluster, uint8_t value);
esp_matter_attribute_t *esp_matter_attribute_create_current_saturation(esp_matter_cluster_t *cluster, uint8_t value);
esp_matter_attribute_t *esp_matter_attribute_create_color_mode(esp_matter_cluster_t *cluster, uint8_t value);
esp_matter_attribute_t *esp_matter_attribute_create_color_control_options(esp_matter_cluster_t *cluster, uint8_t value);
esp_matter_attribute_t *esp_matter_attribute_create_enhanced_color_mode(esp_matter_cluster_t *cluster, uint8_t value);
esp_matter_attribute_t *esp_matter_attribute_create_color_capabilities(esp_matter_cluster_t *cluster, uint16_t value);

/** cluster: fan control */
esp_matter_attribute_t *esp_matter_attribute_create_fan_mode(esp_matter_cluster_t *cluster, uint8_t value);
esp_matter_attribute_t *esp_matter_attribute_create_fan_mode_sequence(esp_matter_cluster_t *cluster, uint8_t value);

/** cluster: thermostat */
esp_matter_attribute_t *esp_matter_attribute_create_local_temperature(esp_matter_cluster_t *cluster, uint16_t value);
esp_matter_attribute_t *esp_matter_attribute_create_occupied_cooling_setpoint(esp_matter_cluster_t *cluster,
                                                                              uint16_t value);
esp_matter_attribute_t *esp_matter_attribute_create_occupied_heating_setpoint(esp_matter_cluster_t *cluster,
                                                                              uint16_t value);
esp_matter_attribute_t *esp_matter_attribute_create_control_sequence_of_operation(esp_matter_cluster_t *cluster,
                                                                                  uint8_t value, uint8_t min,
                                                                                  uint8_t max);
esp_matter_attribute_t *esp_matter_attribute_create_system_mode(esp_matter_cluster_t *cluster, uint8_t value,
                                                                uint8_t min, uint8_t max);

/** cluster: door lock */
esp_matter_attribute_t *esp_matter_attribute_create_lock_state(esp_matter_cluster_t *cluster, uint8_t value);
esp_matter_attribute_t *esp_matter_attribute_create_lock_type(esp_matter_cluster_t *cluster, uint8_t value);
esp_matter_attribute_t *esp_matter_attribute_create_actuator_enabled(esp_matter_cluster_t *cluster, bool value);
esp_matter_attribute_t *esp_matter_attribute_create_auto_relock_time(esp_matter_cluster_t *cluster, uint32_t value);
esp_matter_attribute_t *esp_matter_attribute_create_operating_mode(esp_matter_cluster_t *cluster, uint8_t value,
                                                                   uint8_t min, uint8_t max);
esp_matter_attribute_t *esp_matter_attribute_create_supported_operating_modes(esp_matter_cluster_t *cluster,
                                                                              uint16_t value);

/** cluster: bridged device basic */
esp_matter_attribute_t *esp_matter_attribute_create_bridged_device_basic_node_label(esp_matter_cluster_t *cluster,
                                                                                    char *value, uint16_t length);
esp_matter_attribute_t *esp_matter_attribute_create_reachable(esp_matter_cluster_t *cluster, bool value);

/** cluster: fixed label */
esp_matter_attribute_t *esp_matter_attribute_create_label_list(esp_matter_cluster_t *cluster, uint8_t *value,
                                                               uint16_t length, uint16_t count);

/** cluster: switch */
esp_matter_attribute_t *esp_matter_attribute_create_number_of_positions(esp_matter_cluster_t *cluster, uint8_t value);
esp_matter_attribute_t *esp_matter_attribute_create_current_position(esp_matter_cluster_t *cluster, uint8_t value);
esp_matter_attribute_t *esp_matter_attribute_create_multi_press_max(esp_matter_cluster_t *cluster, uint8_t value);

/** cluster: temperature measurement */
esp_matter_attribute_t *esp_matter_attribute_create_temperature_measured_value(esp_matter_cluster_t *cluster,
                                                                               int16_t value);
esp_matter_attribute_t *esp_matter_attribute_create_temperature_min_measured_value(esp_matter_cluster_t *cluster,
                                                                                   int16_t value);
esp_matter_attribute_t *esp_matter_attribute_create_temperature_max_measured_value(esp_matter_cluster_t *cluster,
                                                                                   int16_t value);
