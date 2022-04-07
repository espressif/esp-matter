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

#include <esp_matter_attribute.h>

static const char *TAG = "esp_matter_attribute";

esp_matter_attribute_t *esp_matter_attribute_create_cluster_revision(esp_matter_cluster_t *cluster, uint16_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_CLUSTER_REVISION_SERVER_ATTRIBUTE_ID,
                                       ESP_MATTER_ATTRIBUTE_FLAG_NONE, esp_matter_uint16(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_feature_map(esp_matter_cluster_t *cluster, uint32_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_FEATURE_MAP_SERVER_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_bitmap32(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_device_list(esp_matter_cluster_t *cluster, uint8_t *value,
                                                                uint16_t length, uint16_t count)
{
    return esp_matter_attribute_create(cluster, ZCL_DEVICE_LIST_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_array(value, length, count));
}

esp_matter_attribute_t *esp_matter_attribute_create_server_list(esp_matter_cluster_t *cluster, uint8_t *value,
                                                                uint16_t length, uint16_t count)
{
    return esp_matter_attribute_create(cluster, ZCL_SERVER_LIST_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_array(value, length, count));
}

esp_matter_attribute_t *esp_matter_attribute_create_client_list(esp_matter_cluster_t *cluster, uint8_t *value,
                                                                uint16_t length, uint16_t count)
{
    return esp_matter_attribute_create(cluster, ZCL_CLIENT_LIST_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_array(value, length, count));
}

esp_matter_attribute_t *esp_matter_attribute_create_parts_list(esp_matter_cluster_t *cluster, uint8_t *value,
                                                               uint16_t length, uint16_t count)
{
    return esp_matter_attribute_create(cluster, ZCL_PARTS_LIST_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_array(value, length, count));
}

esp_matter_attribute_t *esp_matter_attribute_create_acl(esp_matter_cluster_t *cluster, uint8_t *value, uint16_t length,
                                                        uint16_t count)
{
    return esp_matter_attribute_create(cluster, ZCL_ACL_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_WRITABLE,
                                       esp_matter_array(value, length, count));
}

esp_matter_attribute_t *esp_matter_attribute_create_extension(esp_matter_cluster_t *cluster, uint8_t *value,
                                                              uint16_t length, uint16_t count)
{
    return esp_matter_attribute_create(cluster, ZCL_EXTENSION_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_WRITABLE,
                                       esp_matter_array(value, length, count));
}

esp_matter_attribute_t *esp_matter_attribute_create_data_model_revision(esp_matter_cluster_t *cluster, uint16_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_DATA_MODEL_REVISION_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_uint16(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_vendor_name(esp_matter_cluster_t *cluster, char *value,
                                                                uint16_t length)
{
    return esp_matter_attribute_create(cluster, ZCL_VENDOR_NAME_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_char_str(value, length));
}

esp_matter_attribute_t *esp_matter_attribute_create_vendor_id(esp_matter_cluster_t *cluster, uint16_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_VENDOR_ID_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_uint16(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_product_name(esp_matter_cluster_t *cluster, char *value,
                                                                 uint16_t length)
{
    return esp_matter_attribute_create(cluster, ZCL_PRODUCT_NAME_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_char_str(value, length));
}

esp_matter_attribute_t *esp_matter_attribute_create_product_id(esp_matter_cluster_t *cluster, uint16_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_PRODUCT_ID_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_uint16(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_node_label(esp_matter_cluster_t *cluster, char *value,
                                                               uint16_t length)
{
    return esp_matter_attribute_create(cluster, ZCL_NODE_LABEL_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_WRITABLE,
                                       esp_matter_char_str(value, length));
}

esp_matter_attribute_t *esp_matter_attribute_create_location(esp_matter_cluster_t *cluster, char *value,
                                                             uint16_t length)
{
    return esp_matter_attribute_create(cluster, ZCL_LOCATION_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_WRITABLE,
                                       esp_matter_char_str(value, length));
}

esp_matter_attribute_t *esp_matter_attribute_create_hardware_version(esp_matter_cluster_t *cluster, uint16_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_HARDWARE_VERSION_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_uint16(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_hardware_version_string(esp_matter_cluster_t *cluster, char *value,
                                                                            uint16_t length)
{
    return esp_matter_attribute_create(cluster, ZCL_HARDWARE_VERSION_STRING_ATTRIBUTE_ID,
                                       ESP_MATTER_ATTRIBUTE_FLAG_NONE, esp_matter_char_str(value, length));
}

esp_matter_attribute_t *esp_matter_attribute_create_software_version(esp_matter_cluster_t *cluster, uint32_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_SOFTWARE_VERSION_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_uint32(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_software_version_string(esp_matter_cluster_t *cluster, char *value,
                                                                            uint16_t length)
{
    return esp_matter_attribute_create(cluster, ZCL_SOFTWARE_VERSION_STRING_ATTRIBUTE_ID,
                                       ESP_MATTER_ATTRIBUTE_FLAG_NONE, esp_matter_char_str(value, length));
}

esp_matter_attribute_t *esp_matter_attribute_create_binding(esp_matter_cluster_t *cluster, uint8_t *value,
                                                            uint16_t length, uint16_t count)
{
    return esp_matter_attribute_create(cluster, ZCL_BINDING_LIST_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_WRITABLE,
                                       esp_matter_array(value, length, count));
}

esp_matter_attribute_t *esp_matter_attribute_create_default_ota_providers(esp_matter_cluster_t *cluster, uint8_t *value,
                                                                          uint16_t length)
{
    return esp_matter_attribute_create(cluster, ZCL_DEFAULT_OTA_PROVIDERS_ATTRIBUTE_ID,
                                       ESP_MATTER_ATTRIBUTE_FLAG_WRITABLE, esp_matter_octet_str(value, length));
}

esp_matter_attribute_t *esp_matter_attribute_create_update_possible(esp_matter_cluster_t *cluster, bool value)
{
    return esp_matter_attribute_create(cluster, ZCL_UPDATE_POSSIBLE_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_bool(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_update_state(esp_matter_cluster_t *cluster, uint8_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_UPDATE_STATE_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_enum8(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_update_state_progress(esp_matter_cluster_t *cluster, uint8_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_UPDATE_STATE_PROGRESS_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_uint8(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_breadcrumb(esp_matter_cluster_t *cluster, uint64_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_BREADCRUMB_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_WRITABLE,
                                       esp_matter_uint64(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_basic_commissioning_info(esp_matter_cluster_t *cluster,
                                                                             uint8_t *value, uint16_t length,
                                                                             uint16_t count)
{
    return esp_matter_attribute_create(cluster, ZCL_BASICCOMMISSIONINGINFO_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_array(value, length, count));
}

esp_matter_attribute_t *esp_matter_attribute_create_regulatory_config(esp_matter_cluster_t *cluster, uint8_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_REGULATORYCONFIG_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_enum8(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_location_capability(esp_matter_cluster_t *cluster, uint8_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_LOCATIONCAPABILITY_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_enum8(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_max_networks(esp_matter_cluster_t *cluster, uint8_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_MAX_NETWORKS_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_uint8(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_networks(esp_matter_cluster_t *cluster, uint8_t *value,
                                                             uint16_t length, uint16_t count)
{
    return esp_matter_attribute_create(cluster, ZCL_NETWORKS_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_array(value, length, count));
}

esp_matter_attribute_t *esp_matter_attribute_create_scan_max_time_seconds(esp_matter_cluster_t *cluster, uint8_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_SCAN_MAX_TIME_SECONDS_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_uint8(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_connect_max_time_seconds(esp_matter_cluster_t *cluster,
                                                                             uint8_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_CONNECT_MAX_TIME_SECONDS_ATTRIBUTE_ID,
                                       ESP_MATTER_ATTRIBUTE_FLAG_NONE, esp_matter_uint8(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_interface_enabled(esp_matter_cluster_t *cluster, bool value)
{
    return esp_matter_attribute_create(cluster, ZCL_INTERFACE_ENABLED_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_WRITABLE,
                                       esp_matter_bool(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_last_networking_status(esp_matter_cluster_t *cluster, uint8_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_LAST_NETWORKING_STATUS_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_enum8(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_last_network_id(esp_matter_cluster_t *cluster, uint8_t *value,
                                                                    uint16_t length)
{
    return esp_matter_attribute_create(cluster, ZCL_LAST_NETWORK_ID_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_octet_str(value, length));
}

esp_matter_attribute_t *esp_matter_attribute_create_last_connect_error_value(esp_matter_cluster_t *cluster,
                                                                             uint32_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_LAST_CONNECT_ERROR_VALUE_ATTRIBUTE_ID,
                                       ESP_MATTER_ATTRIBUTE_FLAG_NONE, esp_matter_uint32(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_network_interfaces(esp_matter_cluster_t *cluster, uint8_t *value,
                                                                       uint16_t length, uint16_t count)
{
    return esp_matter_attribute_create(cluster, ZCL_NETWORK_INTERFACES_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_array(value, length, count));
}

esp_matter_attribute_t *esp_matter_attribute_create_reboot_count(esp_matter_cluster_t *cluster, uint16_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_REBOOT_COUNT_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_uint16(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_window_status(esp_matter_cluster_t *cluster, uint8_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_WINDOW_STATUS_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_uint8(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_admin_fabric_index(esp_matter_cluster_t *cluster, uint16_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_ADMIN_FABRIC_INDEX_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_uint16(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_admin_vendor_id(esp_matter_cluster_t *cluster, uint16_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_ADMIN_VENDOR_ID_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_uint16(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_nocs(esp_matter_cluster_t *cluster, uint8_t *value,
                                                         uint16_t length, uint16_t count)
{
    return esp_matter_attribute_create(cluster, ZCL_NOCS_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_array(value, length, count));
}

esp_matter_attribute_t *esp_matter_attribute_create_fabrics(esp_matter_cluster_t *cluster, uint8_t *value,
                                                            uint16_t length, uint16_t count)
{
    return esp_matter_attribute_create(cluster, ZCL_FABRICS_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_array(value, length, count));
}

esp_matter_attribute_t *esp_matter_attribute_create_supported_fabrics(esp_matter_cluster_t *cluster, uint8_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_SUPPORTED_FABRICS_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_uint8(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_commissioned_fabrics(esp_matter_cluster_t *cluster, uint8_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_COMMISSIONED_FABRICS_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_uint8(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_trusted_root_certificates(esp_matter_cluster_t *cluster,
                                                                              uint8_t *value, uint16_t length,
                                                                              uint16_t count)
{
    return esp_matter_attribute_create(cluster, ZCL_TRUSTED_ROOTS_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_array(value, length, count));
}

esp_matter_attribute_t *esp_matter_attribute_create_current_fabric_index(esp_matter_cluster_t *cluster, uint8_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_CURRENT_FABRIC_INDEX_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_uint8(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_group_key_map(esp_matter_cluster_t *cluster, uint8_t *value,
                                                                  uint16_t length, uint16_t count)
{
    return esp_matter_attribute_create(cluster, ZCL_GROUP_KEY_MAP_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_WRITABLE,
                                       esp_matter_array(value, length, count));
}

esp_matter_attribute_t *esp_matter_attribute_create_group_table(esp_matter_cluster_t *cluster, uint8_t *value,
                                                                uint16_t length, uint16_t count)
{
    return esp_matter_attribute_create(cluster, ZCL_GROUP_TABLE_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_array(value, length, count));
}

esp_matter_attribute_t *esp_matter_attribute_create_max_groups_per_fabric(esp_matter_cluster_t *cluster, uint16_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_MAX_GROUPS_PER_FABRIC_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_uint16(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_max_group_keys_per_fabric(esp_matter_cluster_t *cluster,
                                                                              uint16_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_MAX_GROUP_KEYS_PER_FABRIC_ATTRIBUTE_ID,
                                       ESP_MATTER_ATTRIBUTE_FLAG_NONE, esp_matter_uint16(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_identify_time(esp_matter_cluster_t *cluster, uint16_t value,
                                                                  uint16_t min, uint16_t max)
{
    esp_matter_attribute_t *attribute = esp_matter_attribute_create(cluster, ZCL_IDENTIFY_TIME_ATTRIBUTE_ID,
                                                                    ESP_MATTER_ATTRIBUTE_FLAG_WRITABLE,
                                                                    esp_matter_uint16(value));
    if (!attribute) {
        ESP_LOGE(TAG, "Could not create attribute");
        return NULL;
    }
    esp_matter_attribute_add_bounds(attribute, esp_matter_uint16(min), esp_matter_uint16(max));
    return attribute;
}

esp_matter_attribute_t *esp_matter_attribute_create_identify_type(esp_matter_cluster_t *cluster, uint8_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_IDENTIFY_TYPE_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_uint8(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_group_name_support(esp_matter_cluster_t *cluster, uint8_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_GROUP_NAME_SUPPORT_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_bitmap8(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_scene_count(esp_matter_cluster_t *cluster, uint8_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_SCENE_COUNT_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_uint8(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_current_scene(esp_matter_cluster_t *cluster, uint8_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_CURRENT_SCENE_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_uint8(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_current_group(esp_matter_cluster_t *cluster, uint16_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_CURRENT_GROUP_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_uint16(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_scene_valid(esp_matter_cluster_t *cluster, bool value)
{
    return esp_matter_attribute_create(cluster, ZCL_SCENE_VALID_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_bool(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_scene_name_support(esp_matter_cluster_t *cluster, uint8_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_SCENE_NAME_SUPPORT_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_bitmap8(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_on_off(esp_matter_cluster_t *cluster, bool value)
{
    return esp_matter_attribute_create(cluster, ZCL_ON_OFF_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_bool(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_global_scene_control(esp_matter_cluster_t *cluster, bool value)
{
    return esp_matter_attribute_create(cluster, ZCL_GLOBAL_SCENE_CONTROL_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_bool(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_on_time(esp_matter_cluster_t *cluster, uint16_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_ON_TIME_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_WRITABLE,
                                       esp_matter_uint16(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_off_wait_time(esp_matter_cluster_t *cluster, uint16_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_OFF_WAIT_TIME_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_WRITABLE,
                                       esp_matter_uint16(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_start_up_on_off(esp_matter_cluster_t *cluster, uint8_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_START_UP_ON_OFF_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_WRITABLE,
                                       esp_matter_enum8(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_current_level(esp_matter_cluster_t *cluster, uint8_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_CURRENT_LEVEL_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_uint8(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_on_level(esp_matter_cluster_t *cluster, uint8_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_ON_LEVEL_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_WRITABLE,
                                       esp_matter_uint8(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_options(esp_matter_cluster_t *cluster, uint8_t value, uint8_t min,
                                                            uint8_t max)
{
    esp_matter_attribute_t *attribute = esp_matter_attribute_create(cluster, ZCL_OPTIONS_ATTRIBUTE_ID,
                                                                    ESP_MATTER_ATTRIBUTE_FLAG_WRITABLE,
                                                                    esp_matter_bitmap8(value));
    if (!attribute) {
        ESP_LOGE(TAG, "Could not create attribute");
        return NULL;
    }
    esp_matter_attribute_add_bounds(attribute, esp_matter_bitmap8(min), esp_matter_bitmap8(max));
    return attribute;
}

esp_matter_attribute_t *esp_matter_attribute_create_remaining_time(esp_matter_cluster_t *cluster, uint16_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_REMAINING_TIME_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_uint16(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_min_level(esp_matter_cluster_t *cluster, uint8_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_MIN_LEVEL_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_uint8(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_max_level(esp_matter_cluster_t *cluster, uint8_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_MAX_LEVEL_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_uint8(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_start_up_current_level(esp_matter_cluster_t *cluster,
                                                                           uint8_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_START_UP_CURRENT_LEVEL_ATTRIBUTE_ID,
                                       ESP_MATTER_ATTRIBUTE_FLAG_WRITABLE, esp_matter_uint8(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_current_hue(esp_matter_cluster_t *cluster, uint8_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_COLOR_CONTROL_CURRENT_HUE_ATTRIBUTE_ID,
                                       ESP_MATTER_ATTRIBUTE_FLAG_NONE, esp_matter_uint8(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_current_saturation(esp_matter_cluster_t *cluster, uint8_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_COLOR_CONTROL_CURRENT_SATURATION_ATTRIBUTE_ID,
                                       ESP_MATTER_ATTRIBUTE_FLAG_NONE, esp_matter_uint8(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_color_mode(esp_matter_cluster_t *cluster, uint8_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_COLOR_CONTROL_COLOR_MODE_ATTRIBUTE_ID,
                                       ESP_MATTER_ATTRIBUTE_FLAG_NONE, esp_matter_enum8(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_color_control_options(esp_matter_cluster_t *cluster, uint8_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_COLOR_CONTROL_OPTIONS_ATTRIBUTE_ID,
                                       ESP_MATTER_ATTRIBUTE_FLAG_WRITABLE, esp_matter_bitmap8(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_enhanced_color_mode(esp_matter_cluster_t *cluster, uint8_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_COLOR_CONTROL_ENHANCED_COLOR_MODE_ATTRIBUTE_ID,
                                       ESP_MATTER_ATTRIBUTE_FLAG_NONE, esp_matter_enum8(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_color_capabilities(esp_matter_cluster_t *cluster, uint16_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_COLOR_CONTROL_COLOR_CAPABILITIES_ATTRIBUTE_ID,
                                       ESP_MATTER_ATTRIBUTE_FLAG_NONE, esp_matter_bitmap16(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_fan_mode(esp_matter_cluster_t *cluster, uint8_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_FAN_CONTROL_FAN_MODE_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_enum8(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_fan_mode_sequence(esp_matter_cluster_t *cluster, uint8_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_FAN_CONTROL_FAN_MODE_SEQUENCE_ATTRIBUTE_ID,
                                       ESP_MATTER_ATTRIBUTE_FLAG_NONE, esp_matter_enum8(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_local_temperature(esp_matter_cluster_t *cluster, uint16_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_LOCAL_TEMPERATURE_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_uint16(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_occupied_cooling_setpoint(esp_matter_cluster_t *cluster,
                                                                              uint16_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_OCCUPIED_COOLING_SETPOINT_ATTRIBUTE_ID,
                                       ESP_MATTER_ATTRIBUTE_FLAG_WRITABLE, esp_matter_uint16(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_occupied_heating_setpoint(esp_matter_cluster_t *cluster,
                                                                              uint16_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_OCCUPIED_HEATING_SETPOINT_ATTRIBUTE_ID,
                                       ESP_MATTER_ATTRIBUTE_FLAG_WRITABLE, esp_matter_uint16(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_control_sequence_of_operation(esp_matter_cluster_t *cluster,
                                                                                  uint8_t value, uint8_t min,
                                                                                  uint8_t max)
{
    esp_matter_attribute_t *attribute = esp_matter_attribute_create(cluster,
                                                                    ZCL_CONTROL_SEQUENCE_OF_OPERATION_ATTRIBUTE_ID,
                                                                    ESP_MATTER_ATTRIBUTE_FLAG_WRITABLE,
                                                                    esp_matter_enum8(value));
    if (!attribute) {
        ESP_LOGE(TAG, "Could not create attribute");
        return NULL;
    }
    esp_matter_attribute_add_bounds(attribute, esp_matter_enum8(min), esp_matter_enum8(max));
    return attribute;
}

esp_matter_attribute_t *esp_matter_attribute_create_system_mode(esp_matter_cluster_t *cluster, uint8_t value,
                                                                uint8_t min, uint8_t max)
{
    esp_matter_attribute_t *attribute = esp_matter_attribute_create(cluster, ZCL_SYSTEM_MODE_ATTRIBUTE_ID,
                                                                    ESP_MATTER_ATTRIBUTE_FLAG_WRITABLE,
                                                                    esp_matter_enum8(value));
    if (!attribute) {
        ESP_LOGE(TAG, "Could not create attribute");
        return NULL;
    }
    esp_matter_attribute_add_bounds(attribute, esp_matter_enum8(min), esp_matter_enum8(max));
    return attribute;
}

esp_matter_attribute_t *esp_matter_attribute_create_lock_state(esp_matter_cluster_t *cluster, uint8_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_LOCK_STATE_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_enum8(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_lock_type(esp_matter_cluster_t *cluster, uint8_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_LOCK_TYPE_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_enum8(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_actuator_enabled(esp_matter_cluster_t *cluster, bool value)
{
    return esp_matter_attribute_create(cluster, ZCL_ACTUATOR_ENABLED_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_bool(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_auto_relock_time(esp_matter_cluster_t *cluster, uint32_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_AUTO_RELOCK_TIME_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_WRITABLE,
                                       esp_matter_bitmap32(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_operating_mode(esp_matter_cluster_t *cluster, uint8_t value,
                                                                   uint8_t min, uint8_t max)
{
    esp_matter_attribute_t *attribute = esp_matter_attribute_create(cluster, ZCL_OPERATING_MODE_ATTRIBUTE_ID,
                                                                    ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                                                    esp_matter_enum8(value));
    if (!attribute) {
        ESP_LOGE(TAG, "Could not create attribute");
        return NULL;
    }
    esp_matter_attribute_add_bounds(attribute, esp_matter_enum8(min), esp_matter_enum8(max));
    return attribute;
}

esp_matter_attribute_t *esp_matter_attribute_create_supported_operating_modes(esp_matter_cluster_t *cluster,
                                                                              uint16_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_SUPPORTED_OPERATING_MODES_ATTRIBUTE_ID,
                                       ESP_MATTER_ATTRIBUTE_FLAG_NONE, esp_matter_bitmap16(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_bridged_device_basic_node_label(esp_matter_cluster_t *cluster,
                                                                                    char *value, uint16_t length)
{
    return esp_matter_attribute_create(cluster, ZCL_NODE_LABEL_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_char_str(value, length));
}

esp_matter_attribute_t *esp_matter_attribute_create_reachable(esp_matter_cluster_t *cluster, bool value)
{
    return esp_matter_attribute_create(cluster, ZCL_REACHABLE_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_bool(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_label_list(esp_matter_cluster_t *cluster, uint8_t *value,
                                                               uint16_t length, uint16_t count)
{
    return esp_matter_attribute_create(cluster, ZCL_LABEL_LIST_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_WRITABLE,
                                       esp_matter_array(value, length, count));
}

esp_matter_attribute_t *esp_matter_attribute_create_number_of_positions(esp_matter_cluster_t *cluster, uint8_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_NUMBER_OF_POSITIONS_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_uint8(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_current_position(esp_matter_cluster_t *cluster, uint8_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_CURRENT_POSITION_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_uint8(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_multi_press_max(esp_matter_cluster_t *cluster, uint8_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_MULTI_PRESS_MAX_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_uint8(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_temperature_measured_value(esp_matter_cluster_t *cluster,
                                                                               int16_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_TEMP_MEASURED_VALUE_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                       esp_matter_int16(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_temperature_min_measured_value(esp_matter_cluster_t *cluster,
                                                                                   int16_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_TEMP_MIN_MEASURED_VALUE_ATTRIBUTE_ID,
                                       ESP_MATTER_ATTRIBUTE_FLAG_NONE, esp_matter_int16(value));
}

esp_matter_attribute_t *esp_matter_attribute_create_temperature_max_measured_value(esp_matter_cluster_t *cluster,
                                                                                   int16_t value)
{
    return esp_matter_attribute_create(cluster, ZCL_TEMP_MAX_MEASURED_VALUE_ATTRIBUTE_ID,
                                       ESP_MATTER_ATTRIBUTE_FLAG_NONE, esp_matter_int16(value));
}
