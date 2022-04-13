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

namespace esp_matter {
namespace cluster {

namespace global {
namespace attribute {

attribute_t *create_cluster_revision(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_CLUSTER_REVISION_SERVER_ATTRIBUTE_ID,
                                         ESP_MATTER_ATTRIBUTE_FLAG_NONE, esp_matter_uint16(value));
}

attribute_t *create_feature_map(cluster_t *cluster, uint32_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_FEATURE_MAP_SERVER_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                         esp_matter_bitmap32(value));
}

} /* attribute */
} /* global */

namespace descriptor {
namespace attribute {

attribute_t *create_device_list(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, ZCL_DEVICE_LIST_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                         esp_matter_array(value, length, count));
}

attribute_t *create_server_list(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, ZCL_SERVER_LIST_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                         esp_matter_array(value, length, count));
}

attribute_t *create_client_list(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, ZCL_CLIENT_LIST_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                         esp_matter_array(value, length, count));
}

attribute_t *create_parts_list(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, ZCL_PARTS_LIST_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                         esp_matter_array(value, length, count));
}

} /* attribute */
} /* descriptor */

namespace access_control {
namespace attribute {

attribute_t *create_acl(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, ZCL_ACL_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_WRITABLE,
                                         esp_matter_array(value, length, count));
}

attribute_t *create_extension(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, ZCL_EXTENSION_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_WRITABLE,
                                         esp_matter_array(value, length, count));
}

} /* attribute */
} /* access_control */

namespace basic {
namespace attribute {

attribute_t *create_data_model_revision(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_DATA_MODEL_REVISION_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                         esp_matter_uint16(value));
}

attribute_t *create_vendor_name(cluster_t *cluster, char *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, ZCL_VENDOR_NAME_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                         esp_matter_char_str(value, length));
}

attribute_t *create_vendor_id(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_VENDOR_ID_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                         esp_matter_uint16(value));
}

attribute_t *create_product_name(cluster_t *cluster, char *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, ZCL_PRODUCT_NAME_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                         esp_matter_char_str(value, length));
}

attribute_t *create_product_id(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_PRODUCT_ID_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                         esp_matter_uint16(value));
}

attribute_t *create_node_label(cluster_t *cluster, char *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, ZCL_NODE_LABEL_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_WRITABLE,
                                         esp_matter_char_str(value, length));
}

attribute_t *create_location(cluster_t *cluster, char *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, ZCL_LOCATION_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_WRITABLE,
                                         esp_matter_char_str(value, length));
}

attribute_t *create_hardware_version(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_HARDWARE_VERSION_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                         esp_matter_uint16(value));
}

attribute_t *create_hardware_version_string(cluster_t *cluster, char *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, ZCL_HARDWARE_VERSION_STRING_ATTRIBUTE_ID,
                                         ESP_MATTER_ATTRIBUTE_FLAG_NONE, esp_matter_char_str(value, length));
}

attribute_t *create_software_version(cluster_t *cluster, uint32_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_SOFTWARE_VERSION_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                         esp_matter_uint32(value));
}

attribute_t *create_software_version_string(cluster_t *cluster, char *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, ZCL_SOFTWARE_VERSION_STRING_ATTRIBUTE_ID,
                                         ESP_MATTER_ATTRIBUTE_FLAG_NONE, esp_matter_char_str(value, length));
}

} /* attribute */
} /* basic */

namespace binding {
namespace attribute {

attribute_t *create_binding(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, ZCL_BINDING_LIST_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_WRITABLE,
                                         esp_matter_array(value, length, count));
}

} /* attribute */
} /* binding */

namespace ota_requestor {
namespace attribute {

attribute_t *create_default_ota_providers(cluster_t *cluster, uint8_t *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, ZCL_DEFAULT_OTA_PROVIDERS_ATTRIBUTE_ID,
                                         ESP_MATTER_ATTRIBUTE_FLAG_WRITABLE, esp_matter_octet_str(value, length));
}

attribute_t *create_update_possible(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, ZCL_UPDATE_POSSIBLE_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                         esp_matter_bool(value));
}

attribute_t *create_update_state(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_UPDATE_STATE_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                         esp_matter_enum8(value));
}

attribute_t *create_update_state_progress(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_UPDATE_STATE_PROGRESS_ATTRIBUTE_ID,
                                         ESP_MATTER_ATTRIBUTE_FLAG_NONE, esp_matter_uint8(value));
}

} /* attribute */
} /* ota_requestor */

namespace general_commissioning {
namespace attribute {

attribute_t *create_breadcrumb(cluster_t *cluster, uint64_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_BREADCRUMB_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_WRITABLE,
                                         esp_matter_uint64(value));
}

attribute_t *create_basic_commissioning_info(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, ZCL_BASICCOMMISSIONINGINFO_ATTRIBUTE_ID,
                                         ESP_MATTER_ATTRIBUTE_FLAG_NONE, esp_matter_array(value, length, count));
}

attribute_t *create_regulatory_config(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_REGULATORYCONFIG_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                         esp_matter_enum8(value));
}

attribute_t *create_location_capability(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_LOCATIONCAPABILITY_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                         esp_matter_enum8(value));
}

} /* attribute */
} /* general_commissioning */

namespace network_commissioning {
namespace attribute {

attribute_t *create_max_networks(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_MAX_NETWORKS_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                         esp_matter_uint8(value));
}

attribute_t *create_networks(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, ZCL_NETWORKS_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                         esp_matter_array(value, length, count));
}

attribute_t *create_scan_max_time_seconds(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_SCAN_MAX_TIME_SECONDS_ATTRIBUTE_ID,
                                         ESP_MATTER_ATTRIBUTE_FLAG_NONE, esp_matter_uint8(value));
}

attribute_t *create_connect_max_time_seconds(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_CONNECT_MAX_TIME_SECONDS_ATTRIBUTE_ID,
                                         ESP_MATTER_ATTRIBUTE_FLAG_NONE, esp_matter_uint8(value));
}

attribute_t *create_interface_enabled(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, ZCL_INTERFACE_ENABLED_ATTRIBUTE_ID,
                                         ESP_MATTER_ATTRIBUTE_FLAG_WRITABLE, esp_matter_bool(value));
}

attribute_t *create_last_networking_status(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_LAST_NETWORKING_STATUS_ATTRIBUTE_ID,
                                         ESP_MATTER_ATTRIBUTE_FLAG_NONE, esp_matter_enum8(value));
}

attribute_t *create_last_network_id(cluster_t *cluster, uint8_t *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, ZCL_LAST_NETWORK_ID_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                         esp_matter_octet_str(value, length));
}

attribute_t *create_last_connect_error_value(cluster_t *cluster, uint32_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_LAST_CONNECT_ERROR_VALUE_ATTRIBUTE_ID,
                                         ESP_MATTER_ATTRIBUTE_FLAG_NONE, esp_matter_uint32(value));
}

} /* attribute */
} /* network_commissioning */

namespace general_diagnostics {
namespace attribute {

attribute_t *create_network_interfaces(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, ZCL_NETWORK_INTERFACES_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                         esp_matter_array(value, length, count));
}

attribute_t *create_reboot_count(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_REBOOT_COUNT_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                         esp_matter_uint16(value));
}

} /* attribute */
} /* general_diagnostics */

namespace administrator_commissioning {
namespace attribute {

attribute_t *create_window_status(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_WINDOW_STATUS_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                         esp_matter_uint8(value));
}

attribute_t *create_admin_fabric_index(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_ADMIN_FABRIC_INDEX_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                         esp_matter_uint16(value));
}

attribute_t *create_admin_vendor_id(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_ADMIN_VENDOR_ID_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                         esp_matter_uint16(value));
}

} /* attribute */
} /* administrator_commissioning */

namespace operational_credentials {
namespace attribute {

attribute_t *create_nocs(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, ZCL_NOCS_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                         esp_matter_array(value, length, count));
}

attribute_t *create_fabrics(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, ZCL_FABRICS_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                         esp_matter_array(value, length, count));
}

attribute_t *create_supported_fabrics(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_SUPPORTED_FABRICS_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                         esp_matter_uint8(value));
}

attribute_t *create_commissioned_fabrics(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_COMMISSIONED_FABRICS_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                         esp_matter_uint8(value));
}

attribute_t *create_trusted_root_certificates(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, ZCL_TRUSTED_ROOTS_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                         esp_matter_array(value, length, count));
}

attribute_t *create_current_fabric_index(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_CURRENT_FABRIC_INDEX_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                         esp_matter_uint8(value));
}

} /* attribute */
} /* operational_credentials */

namespace group_key_management {
namespace attribute {

attribute_t *create_group_key_map(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, ZCL_GROUP_KEY_MAP_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_WRITABLE,
                                         esp_matter_array(value, length, count));
}

attribute_t *create_group_table(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, ZCL_GROUP_TABLE_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                         esp_matter_array(value, length, count));
}

attribute_t *create_max_groups_per_fabric(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_MAX_GROUPS_PER_FABRIC_ATTRIBUTE_ID,
                                         ESP_MATTER_ATTRIBUTE_FLAG_NONE, esp_matter_uint16(value));
}

attribute_t *create_max_group_keys_per_fabric(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_MAX_GROUP_KEYS_PER_FABRIC_ATTRIBUTE_ID,
                                         ESP_MATTER_ATTRIBUTE_FLAG_NONE, esp_matter_uint16(value));
}

} /* attribute */
} /* group_key_management */

namespace identify {
namespace attribute {

attribute_t *create_identify_time(cluster_t *cluster, uint16_t value, uint16_t min, uint16_t max)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, ZCL_IDENTIFY_TIME_ATTRIBUTE_ID,
                                                           ESP_MATTER_ATTRIBUTE_FLAG_WRITABLE,
                                                           esp_matter_uint16(value));
    if (!attribute) {
        ESP_LOGE(TAG, "Could not create attribute");
        return NULL;
    }
    esp_matter::attribute::add_bounds(attribute, esp_matter_uint16(min), esp_matter_uint16(max));
    return attribute;
}

attribute_t *create_identify_type(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_IDENTIFY_TYPE_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                         esp_matter_uint8(value));
}

} /* attribute */
} /* identify */

namespace groups {
namespace attribute {

attribute_t *create_group_name_support(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_GROUP_NAME_SUPPORT_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                         esp_matter_bitmap8(value));
}

} /* attribute */
} /* groups */

namespace scenes {
namespace attribute {

attribute_t *create_scene_count(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_SCENE_COUNT_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                         esp_matter_uint8(value));
}

attribute_t *create_current_scene(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_CURRENT_SCENE_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                         esp_matter_uint8(value));
}

attribute_t *create_current_group(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_CURRENT_GROUP_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                         esp_matter_uint16(value));
}

attribute_t *create_scene_valid(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, ZCL_SCENE_VALID_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                         esp_matter_bool(value));
}

attribute_t *create_scene_name_support(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_SCENE_NAME_SUPPORT_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                         esp_matter_bitmap8(value));
}

} /* attribute */
} /* scenes */

namespace on_off {
namespace attribute {

attribute_t *create_on_off(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, ZCL_ON_OFF_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                         esp_matter_bool(value));
}

attribute_t *create_global_scene_control(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, ZCL_GLOBAL_SCENE_CONTROL_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                         esp_matter_bool(value));
}

attribute_t *create_on_time(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_ON_TIME_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_WRITABLE,
                                         esp_matter_uint16(value));
}

attribute_t *create_off_wait_time(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_OFF_WAIT_TIME_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_WRITABLE,
                                         esp_matter_uint16(value));
}

attribute_t *create_start_up_on_off(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_START_UP_ON_OFF_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_WRITABLE,
                                         esp_matter_enum8(value));
}

} /* attribute */
} /* on_off */

namespace level_control {
namespace attribute {

attribute_t *create_current_level(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_CURRENT_LEVEL_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                         esp_matter_uint8(value));
}

attribute_t *create_on_level(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_ON_LEVEL_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_WRITABLE,
                                         esp_matter_uint8(value));
}

attribute_t *create_options(cluster_t *cluster, uint8_t value, uint8_t min, uint8_t max)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, ZCL_OPTIONS_ATTRIBUTE_ID,
                                                           ESP_MATTER_ATTRIBUTE_FLAG_WRITABLE,
                                                           esp_matter_bitmap8(value));
    if (!attribute) {
        ESP_LOGE(TAG, "Could not create attribute");
        return NULL;
    }
    esp_matter::attribute::add_bounds(attribute, esp_matter_bitmap8(min), esp_matter_bitmap8(max));
    return attribute;
}

attribute_t *create_remaining_time(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_REMAINING_TIME_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                         esp_matter_uint16(value));
}

attribute_t *create_min_level(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_MINIMUM_LEVEL_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                         esp_matter_uint8(value));
}

attribute_t *create_max_level(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_MAXIMUM_LEVEL_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                         esp_matter_uint8(value));
}

attribute_t *create_start_up_current_level(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_START_UP_CURRENT_LEVEL_ATTRIBUTE_ID,
                                         ESP_MATTER_ATTRIBUTE_FLAG_WRITABLE, esp_matter_uint8(value));
}

} /* attribute */
} /* level_control */

namespace color_control {
namespace attribute {

attribute_t *create_current_hue(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_COLOR_CONTROL_CURRENT_HUE_ATTRIBUTE_ID,
                                         ESP_MATTER_ATTRIBUTE_FLAG_NONE, esp_matter_uint8(value));
}

attribute_t *create_current_saturation(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_COLOR_CONTROL_CURRENT_SATURATION_ATTRIBUTE_ID,
                                         ESP_MATTER_ATTRIBUTE_FLAG_NONE, esp_matter_uint8(value));
}

attribute_t *create_color_mode(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_COLOR_CONTROL_COLOR_MODE_ATTRIBUTE_ID,
                                         ESP_MATTER_ATTRIBUTE_FLAG_NONE, esp_matter_enum8(value));
}

attribute_t *create_color_control_options(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_COLOR_CONTROL_OPTIONS_ATTRIBUTE_ID,
                                         ESP_MATTER_ATTRIBUTE_FLAG_WRITABLE, esp_matter_bitmap8(value));
}

attribute_t *create_enhanced_color_mode(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_COLOR_CONTROL_ENHANCED_COLOR_MODE_ATTRIBUTE_ID,
                                         ESP_MATTER_ATTRIBUTE_FLAG_NONE, esp_matter_enum8(value));
}

attribute_t *create_color_capabilities(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_COLOR_CONTROL_COLOR_CAPABILITIES_ATTRIBUTE_ID,
                                         ESP_MATTER_ATTRIBUTE_FLAG_NONE, esp_matter_bitmap16(value));
}

} /* attribute */
} /* color_control */

namespace fan_control {
namespace attribute {

attribute_t *create_fan_mode(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_FAN_CONTROL_FAN_MODE_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                         esp_matter_enum8(value));
}

attribute_t *create_fan_mode_sequence(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_FAN_CONTROL_FAN_MODE_SEQUENCE_ATTRIBUTE_ID,
                                         ESP_MATTER_ATTRIBUTE_FLAG_NONE, esp_matter_enum8(value));
}

} /* attribute */
} /* fan_control */

namespace thermostat {
namespace attribute {

attribute_t *create_local_temperature(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_LOCAL_TEMPERATURE_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                         esp_matter_uint16(value));
}

attribute_t *create_occupied_cooling_setpoint(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_OCCUPIED_COOLING_SETPOINT_ATTRIBUTE_ID,
                                         ESP_MATTER_ATTRIBUTE_FLAG_WRITABLE, esp_matter_uint16(value));
}

attribute_t *create_occupied_heating_setpoint(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_OCCUPIED_HEATING_SETPOINT_ATTRIBUTE_ID,
                                         ESP_MATTER_ATTRIBUTE_FLAG_WRITABLE, esp_matter_uint16(value));
}

attribute_t *create_control_sequence_of_operation(cluster_t *cluster, uint8_t value, uint8_t min, uint8_t max)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, ZCL_CONTROL_SEQUENCE_OF_OPERATION_ATTRIBUTE_ID,
                                                           ESP_MATTER_ATTRIBUTE_FLAG_WRITABLE,
                                                           esp_matter_enum8(value));
    if (!attribute) {
        ESP_LOGE(TAG, "Could not create attribute");
        return NULL;
    }
    esp_matter::attribute::add_bounds(attribute, esp_matter_enum8(min), esp_matter_enum8(max));
    return attribute;
}

attribute_t *create_system_mode(cluster_t *cluster, uint8_t value, uint8_t min, uint8_t max)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, ZCL_SYSTEM_MODE_ATTRIBUTE_ID,
                                                           ESP_MATTER_ATTRIBUTE_FLAG_WRITABLE,
                                                           esp_matter_enum8(value));
    if (!attribute) {
        ESP_LOGE(TAG, "Could not create attribute");
        return NULL;
    }
    esp_matter::attribute::add_bounds(attribute, esp_matter_enum8(min), esp_matter_enum8(max));
    return attribute;
}

} /* attribute */
} /* thermostat */

namespace door_lock {
namespace attribute {

attribute_t *create_lock_state(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_LOCK_STATE_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                         esp_matter_enum8(value));
}

attribute_t *create_lock_type(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_LOCK_TYPE_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                         esp_matter_enum8(value));
}

attribute_t *create_actuator_enabled(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, ZCL_ACTUATOR_ENABLED_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                         esp_matter_bool(value));
}

attribute_t *create_auto_relock_time(cluster_t *cluster, uint32_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_AUTO_RELOCK_TIME_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_WRITABLE,
                                         esp_matter_bitmap32(value));
}

attribute_t *create_operating_mode(cluster_t *cluster, uint8_t value, uint8_t min, uint8_t max)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, ZCL_OPERATING_MODE_ATTRIBUTE_ID,
                                                           ESP_MATTER_ATTRIBUTE_FLAG_NONE, esp_matter_enum8(value));
    if (!attribute) {
        ESP_LOGE(TAG, "Could not create attribute");
        return NULL;
    }
    esp_matter::attribute::add_bounds(attribute, esp_matter_enum8(min), esp_matter_enum8(max));
    return attribute;
}

attribute_t *create_supported_operating_modes(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_SUPPORTED_OPERATING_MODES_ATTRIBUTE_ID,
                                         ESP_MATTER_ATTRIBUTE_FLAG_NONE, esp_matter_bitmap16(value));
}

} /* attribute */
} /* door_lock */

namespace bridged_device_basic {
namespace attribute {

attribute_t *create_bridged_device_basic_node_label(cluster_t *cluster, char *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, ZCL_NODE_LABEL_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                         esp_matter_char_str(value, length));
}

attribute_t *create_reachable(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, ZCL_REACHABLE_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                         esp_matter_bool(value));
}

} /* attribute */
} /* bridged_device_basic */

namespace fixed_label {
namespace attribute {

attribute_t *create_label_list(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, ZCL_LABEL_LIST_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_WRITABLE,
                                         esp_matter_array(value, length, count));
}

} /* attribute */
} /* fixed_label */

namespace switch_cluster {
namespace attribute {

attribute_t *create_number_of_positions(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_NUMBER_OF_POSITIONS_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                         esp_matter_uint8(value));
}

attribute_t *create_current_position(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_CURRENT_POSITION_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                         esp_matter_uint8(value));
}

attribute_t *create_multi_press_max(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_MULTI_PRESS_MAX_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                         esp_matter_uint8(value));
}

} /* attribute */
} /* switch_cluster */

namespace temperature_measurement {
namespace attribute {

attribute_t *create_temperature_measured_value(cluster_t *cluster, int16_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_TEMP_MEASURED_VALUE_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                         esp_matter_int16(value));
}

attribute_t *create_temperature_min_measured_value(cluster_t *cluster, int16_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_TEMP_MIN_MEASURED_VALUE_ATTRIBUTE_ID,
                                         ESP_MATTER_ATTRIBUTE_FLAG_NONE, esp_matter_int16(value));
}

attribute_t *create_temperature_max_measured_value(cluster_t *cluster, int16_t value)
{
    return esp_matter::attribute::create(cluster, ZCL_TEMP_MAX_MEASURED_VALUE_ATTRIBUTE_ID,
                                         ESP_MATTER_ATTRIBUTE_FLAG_NONE, esp_matter_int16(value));
}

} /* attribute */
} /* temperature_measurement */

} /* cluster */
} /* esp_matter */
