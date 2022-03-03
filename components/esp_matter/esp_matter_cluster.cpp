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

#include <esp_log.h>
#include <esp_matter.h>
#include <esp_matter_cluster.h>
#include <esp_matter_core.h>

#include <app-common/zap-generated/att-storage.h>
#include <app-common/zap-generated/attribute-id.h>
#include <app-common/zap-generated/cluster-id.h>
#include <app-common/zap-generated/command-id.h>
#include <app-common/zap-generated/callbacks/PluginCallbacks.h>
#include <app-common/zap-generated/callback.h>

static const char *TAG = "esp_matter_cluster";

const esp_matter_cluster_function_generic_t *esp_matter_cluster_descriptor_function_list = NULL;
const int esp_matter_cluster_descriptor_function_flags = ESP_MATTER_CLUSTER_FLAG_NONE;

const esp_matter_cluster_function_generic_t *esp_matter_cluster_ota_provider_function_list = NULL;
const int esp_matter_cluster_ota_provider_function_flags = ESP_MATTER_CLUSTER_FLAG_NONE;

const esp_matter_cluster_function_generic_t *esp_matter_cluster_ota_requestor_function_list = NULL;
const int esp_matter_cluster_ota_requestor_function_flags = ESP_MATTER_CLUSTER_FLAG_NONE;

const esp_matter_cluster_function_generic_t *esp_matter_cluster_general_commissioning_function_list = NULL;
const int esp_matter_cluster_general_commissioning_function_flags = ESP_MATTER_CLUSTER_FLAG_NONE;

const esp_matter_cluster_function_generic_t *esp_matter_cluster_network_commissioning_function_list = NULL;
const int esp_matter_cluster_network_commissioning_function_flags = ESP_MATTER_CLUSTER_FLAG_NONE;

const esp_matter_cluster_function_generic_t *esp_matter_cluster_general_diagnostics_function_list = NULL;
const int esp_matter_cluster_general_diagnostics_function_flags = ESP_MATTER_CLUSTER_FLAG_NONE;

const esp_matter_cluster_function_generic_t *esp_matter_cluster_administrator_commissioning_function_list = NULL;
const int esp_matter_cluster_administrator_commissioning_function_flags = ESP_MATTER_CLUSTER_FLAG_NONE;

const esp_matter_cluster_function_generic_t *esp_matter_cluster_operational_credentials_function_list = NULL;
const int esp_matter_cluster_operational_credentials_function_flags = ESP_MATTER_CLUSTER_FLAG_NONE;

const esp_matter_cluster_function_generic_t *esp_matter_cluster_group_key_management_function_list = NULL;
const int esp_matter_cluster_group_key_management_function_flags = ESP_MATTER_CLUSTER_FLAG_NONE;

const esp_matter_cluster_function_generic_t *esp_matter_cluster_fan_control_function_list = NULL;
const int esp_matter_cluster_fan_control_function_flags = ESP_MATTER_CLUSTER_FLAG_NONE;

const esp_matter_cluster_function_generic_t *esp_matter_cluster_binding_function_list = NULL;
const int esp_matter_cluster_binding_function_flags = ESP_MATTER_CLUSTER_FLAG_NONE;

const esp_matter_cluster_function_generic_t *esp_matter_cluster_bridged_device_basic_function_list = NULL;
const int esp_matter_cluster_bridged_device_basic_function_flags = ESP_MATTER_CLUSTER_FLAG_NONE;

const esp_matter_cluster_function_generic_t *esp_matter_cluster_fixed_label_function_list = NULL;
const int esp_matter_cluster_fixed_label_function_flags = ESP_MATTER_CLUSTER_FLAG_NONE;

const esp_matter_cluster_function_generic_t *esp_matter_cluster_switch_function_list = NULL;
const int esp_matter_cluster_switch_function_flags = ESP_MATTER_CLUSTER_FLAG_NONE;

const esp_matter_cluster_function_generic_t *esp_matter_cluster_time_synchronization_function_list = NULL;
const int esp_matter_cluster_time_synchronization_function_flags = ESP_MATTER_CLUSTER_FLAG_NONE;

const esp_matter_cluster_function_generic_t *esp_matter_cluster_temperature_measurement_function_list = NULL;
const int esp_matter_cluster_temperature_measurement_function_flags = ESP_MATTER_CLUSTER_FLAG_NONE;

const esp_matter_cluster_function_generic_t esp_matter_cluster_access_control_function_list[] = {
    (esp_matter_cluster_function_generic_t)emberAfAccessControlClusterServerInitCallback,
};
const int esp_matter_cluster_access_control_function_flags = ESP_MATTER_CLUSTER_FLAG_INIT_FUNCTION;

const esp_matter_cluster_function_generic_t esp_matter_cluster_basic_function_list[] = {
    (esp_matter_cluster_function_generic_t)emberAfBasicClusterServerInitCallback,
};
const int esp_matter_cluster_basic_function_flags = ESP_MATTER_CLUSTER_FLAG_INIT_FUNCTION;

const esp_matter_cluster_function_generic_t esp_matter_cluster_identify_function_list[] = {
    (esp_matter_cluster_function_generic_t)emberAfIdentifyClusterServerInitCallback,
    (esp_matter_cluster_function_generic_t)MatterIdentifyClusterServerAttributeChangedCallback,
};
const int esp_matter_cluster_identify_function_flags = ESP_MATTER_CLUSTER_FLAG_INIT_FUNCTION |
                                                       ESP_MATTER_CLUSTER_FLAG_ATTRIBUTE_CHANGED_FUNCTION;

const esp_matter_cluster_function_generic_t esp_matter_cluster_groups_function_list[] = {
    (esp_matter_cluster_function_generic_t)emberAfGroupsClusterServerInitCallback,
};
const int esp_matter_cluster_groups_function_flags = ESP_MATTER_CLUSTER_FLAG_INIT_FUNCTION;

const esp_matter_cluster_function_generic_t esp_matter_cluster_scenes_function_list[] = {
    (esp_matter_cluster_function_generic_t)emberAfScenesClusterServerInitCallback,
};
const int esp_matter_cluster_scenes_function_flags = ESP_MATTER_CLUSTER_FLAG_INIT_FUNCTION;

const esp_matter_cluster_function_generic_t esp_matter_cluster_on_off_function_list[] = {
    (esp_matter_cluster_function_generic_t)emberAfOnOffClusterServerInitCallback,
};
const int esp_matter_cluster_on_off_function_flags = ESP_MATTER_CLUSTER_FLAG_INIT_FUNCTION;

const esp_matter_cluster_function_generic_t esp_matter_cluster_level_control_function_list[] = {
    (esp_matter_cluster_function_generic_t)emberAfLevelControlClusterServerInitCallback,
};
const int esp_matter_cluster_level_control_function_flags = ESP_MATTER_CLUSTER_FLAG_INIT_FUNCTION;

const esp_matter_cluster_function_generic_t esp_matter_cluster_color_control_function_list[] = {
    (esp_matter_cluster_function_generic_t)emberAfColorControlClusterServerInitCallback,
};
const int esp_matter_cluster_color_control_function_flags = ESP_MATTER_CLUSTER_FLAG_INIT_FUNCTION;

const esp_matter_cluster_function_generic_t esp_matter_cluster_thermostat_function_list[] = {
    (esp_matter_cluster_function_generic_t)emberAfThermostatClusterServerInitCallback,
};
const int esp_matter_cluster_thermostat_function_flags = ESP_MATTER_CLUSTER_FLAG_INIT_FUNCTION;

const esp_matter_cluster_function_generic_t esp_matter_cluster_door_lock_function_list[] = {
        (esp_matter_cluster_function_generic_t) MatterDoorLockClusterServerAttributeChangedCallback,
        (esp_matter_cluster_function_generic_t) MatterDoorLockClusterServerPreAttributeChangedCallback,
};
const int esp_matter_cluster_door_lock_function_flags = ESP_MATTER_CLUSTER_FLAG_ATTRIBUTE_CHANGED_FUNCTION |
                                                        ESP_MATTER_CLUSTER_FLAG_PRE_ATTRIBUTE_CHANGED_FUNCTION;

void esp_matter_cluster_plugin_init_callback_common()
{
    ESP_LOGI(TAG, "Cluster plugin init common callback");
    esp_matter_node_t *node = esp_matter_node_get();
    esp_matter_endpoint_t *endpoint = esp_matter_endpoint_get_first(node);
    while (endpoint) {
        esp_matter_cluster_t *cluster = esp_matter_cluster_get_first(endpoint);
        while (cluster) {
            /* Plugin server init callback */
            esp_matter_cluster_plugin_server_init_callback_t plugin_server_init_callback =
                                                        esp_matter_cluster_get_plugin_server_init_callback(cluster);
            if (plugin_server_init_callback) {
                plugin_server_init_callback();
            }
            /* Plugin client init callback */
            esp_matter_cluster_plugin_client_init_callback_t plugin_client_init_callback =
                                                        esp_matter_cluster_get_plugin_client_init_callback(cluster);
            if (plugin_client_init_callback) {
                plugin_client_init_callback();
            }
            cluster = esp_matter_cluster_get_next(cluster);
        }
        endpoint = esp_matter_endpoint_get_next(endpoint);
    }
}

esp_matter_cluster_t *esp_matter_cluster_create_descriptor(esp_matter_endpoint_t *endpoint,
                                                           esp_matter_cluster_descriptor_config_t *config,
                                                           uint8_t flags)
{
    esp_matter_cluster_t *cluster = esp_matter_cluster_create(endpoint, ZCL_DESCRIPTOR_CLUSTER_ID, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & ESP_MATTER_CLUSTER_FLAG_SERVER) {
        esp_matter_cluster_set_plugin_server_init_callback(cluster, MatterDescriptorPluginServerInitCallback);
        esp_matter_cluster_add_function_list(cluster, esp_matter_cluster_descriptor_function_list,
                                             esp_matter_cluster_descriptor_function_flags);
    }
    if (flags & ESP_MATTER_CLUSTER_FLAG_CLIENT) {
        esp_matter_cluster_set_plugin_client_init_callback(cluster, MatterDescriptorPluginClientInitCallback);
    }

    esp_matter_attribute_create(cluster, ZCL_CLUSTER_REVISION_SERVER_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint16(config->cluster_revision));
    esp_matter_attribute_create(cluster, ZCL_DEVICE_LIST_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_array(config->device_type_list, sizeof(config->device_type_list), 0));
    esp_matter_attribute_create(cluster, ZCL_SERVER_LIST_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_array(config->server_list, sizeof(config->server_list), 0));
    esp_matter_attribute_create(cluster, ZCL_CLIENT_LIST_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_array(config->client_list, sizeof(config->client_list), 0));
    esp_matter_attribute_create(cluster, ZCL_PARTS_LIST_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_array(config->parts_list, sizeof(config->parts_list), 0));

    return cluster;
}

esp_matter_cluster_t *esp_matter_cluster_create_access_control(esp_matter_endpoint_t *endpoint,
                                                               esp_matter_cluster_access_control_config_t *config,
                                                               uint8_t flags)
{
    /* Not implemented
    esp_matter_cluster_t *cluster = esp_matter_cluster_create(endpoint, ZCL_ACCESS_CONTROL_CLUSTER_ID,
                                                              ESP_MATTER_CLUSTER_FLAG_SERVER);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & ESP_MATTER_CLUSTER_FLAG_SERVER) {
        esp_matter_cluster_set_plugin_server_init_callback(cluster, MatterAcccessControlPluginServerInitCallback);
        esp_matter_cluster_add_function_list(cluster, esp_matter_cluster_access_control_function_list,
                                             esp_matter_cluster_access_control_function_flags);
    }
    if (flags & ESP_MATTER_CLUSTER_FLAG_CLIENT) {
        esp_matter_cluster_set_plugin_client_init_callback(cluster, MatterAcccessControlPluginClientInitCallback);
    }

    esp_matter_attribute_create(cluster, ZCL_CLUSTER_REVISION_SERVER_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint16(config->cluster_revision));
    esp_matter_attribute_create(cluster, ZCL_ACL_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_array(config->acl, sizeof(config->acl), 0));
    esp_matter_attribute_create(cluster, ZCL_EXTENSION_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_array(config->extension, sizeof(config->extension), 0));

    return cluster;
    */
    return NULL;
}

esp_matter_cluster_t *esp_matter_cluster_create_basic(esp_matter_endpoint_t *endpoint,
                                                      esp_matter_cluster_basic_config_t *config, uint8_t flags)
{
    esp_matter_cluster_t *cluster = esp_matter_cluster_create(endpoint, ZCL_BASIC_CLUSTER_ID, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & ESP_MATTER_CLUSTER_FLAG_SERVER) {
        esp_matter_cluster_set_plugin_server_init_callback(cluster, MatterBasicPluginServerInitCallback);
        esp_matter_cluster_add_function_list(cluster, esp_matter_cluster_basic_function_list,
                                             esp_matter_cluster_basic_function_flags);
    }
    if (flags & ESP_MATTER_CLUSTER_FLAG_CLIENT) {
        esp_matter_cluster_set_plugin_client_init_callback(cluster, MatterBasicPluginClientInitCallback);
    }

    esp_matter_attribute_create(cluster, ZCL_CLUSTER_REVISION_SERVER_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint16(config->cluster_revision));
    esp_matter_attribute_create(cluster, ZCL_INTERACTION_MODEL_VERSION_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint16(config->interaction_model_version));
    esp_matter_attribute_create(cluster, ZCL_VENDOR_NAME_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_char_str(config->vendor_name, sizeof(config->vendor_name)));
    esp_matter_attribute_create(cluster, ZCL_VENDOR_ID_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint16(config->vendor_id));
    esp_matter_attribute_create(cluster, ZCL_PRODUCT_NAME_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_char_str(config->product_name, sizeof(config->product_name)));
    esp_matter_attribute_create(cluster, ZCL_PRODUCT_ID_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint16(config->product_id));
    esp_matter_attribute_create(cluster, ZCL_NODE_LABEL_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_char_str(config->node_label, sizeof(config->node_label)));
    esp_matter_attribute_create(cluster, ZCL_LOCATION_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_char_str(config->location, sizeof(config->location)));
    esp_matter_attribute_create(cluster, ZCL_HARDWARE_VERSION_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint16(config->hardware_version));
    esp_matter_attribute_create(cluster, ZCL_HARDWARE_VERSION_STRING_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_char_str(config->hardware_version_string,
                                sizeof(config->hardware_version_string)));
    esp_matter_attribute_create(cluster, ZCL_SOFTWARE_VERSION_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint32(config->software_version));
    esp_matter_attribute_create(cluster, ZCL_SOFTWARE_VERSION_STRING_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_char_str(config->software_version_string,
                                sizeof(config->software_version_string)));
    esp_matter_attribute_create(cluster, ZCL_SERIAL_NUMBER_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_char_str(config->serial_number, sizeof(config->serial_number)));

    return cluster;
}

esp_matter_cluster_t *esp_matter_cluster_create_binding(esp_matter_endpoint_t *endpoint,
                                                        esp_matter_cluster_binding_config_t *config, 
                                                        uint8_t flags)
{
    esp_matter_cluster_t *cluster = esp_matter_cluster_create(endpoint, ZCL_BINDING_CLUSTER_ID, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & ESP_MATTER_CLUSTER_FLAG_SERVER) {
        esp_matter_cluster_set_plugin_server_init_callback(cluster, MatterBasicPluginServerInitCallback);
        esp_matter_cluster_add_function_list(cluster, esp_matter_cluster_binding_function_list,
                                             esp_matter_cluster_binding_function_flags);
    }
    if (flags & ESP_MATTER_CLUSTER_FLAG_CLIENT) {
        esp_matter_cluster_set_plugin_client_init_callback(cluster, MatterBasicPluginClientInitCallback);
    }
    /* Extra initialization */
    esp_matter_binding_manager_init();

    esp_matter_attribute_create(cluster, ZCL_CLUSTER_REVISION_SERVER_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint16(config->cluster_revision));

    esp_matter_command_create_bind(cluster);
    esp_matter_command_create_unbind(cluster);

    return cluster;
}

esp_matter_cluster_t *esp_matter_cluster_create_ota_provider(esp_matter_endpoint_t *endpoint,
                                                             esp_matter_cluster_ota_provider_config_t *config,
                                                             uint8_t flags)
{
    esp_matter_cluster_t *cluster = esp_matter_cluster_create(endpoint, ZCL_OTA_PROVIDER_CLUSTER_ID, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & ESP_MATTER_CLUSTER_FLAG_SERVER) {
        esp_matter_cluster_set_plugin_server_init_callback(cluster,
                                                           MatterOtaSoftwareUpdateProviderPluginServerInitCallback);
        esp_matter_cluster_add_function_list(cluster, esp_matter_cluster_ota_provider_function_list,
                                             esp_matter_cluster_ota_provider_function_flags);
    }
    if (flags & ESP_MATTER_CLUSTER_FLAG_CLIENT) {
        esp_matter_cluster_set_plugin_client_init_callback(cluster,
                                                           MatterOtaSoftwareUpdateProviderPluginClientInitCallback);
    }

    esp_matter_attribute_create(cluster, ZCL_CLUSTER_REVISION_SERVER_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint16(config->cluster_revision));

    esp_matter_command_create_query_image(cluster);                              
    esp_matter_command_create_query_image_response(cluster);                              
    esp_matter_command_create_apply_update_request(cluster);                              
    esp_matter_command_create_apply_update_response(cluster);                              
    esp_matter_command_create_notify_update_applied(cluster);                              

    return cluster;
}

esp_matter_cluster_t *esp_matter_cluster_create_ota_requestor(esp_matter_endpoint_t *endpoint,
                                                              esp_matter_cluster_ota_requestor_config_t *config,
                                                              uint8_t flags)
{
    esp_matter_cluster_t *cluster = esp_matter_cluster_create(endpoint, ZCL_OTA_REQUESTOR_CLUSTER_ID, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & ESP_MATTER_CLUSTER_FLAG_SERVER) {
        esp_matter_cluster_set_plugin_server_init_callback(cluster,
                                                           MatterOtaSoftwareUpdateRequestorPluginServerInitCallback);
        esp_matter_cluster_add_function_list(cluster, esp_matter_cluster_ota_requestor_function_list,
                                             esp_matter_cluster_ota_requestor_function_flags);
    }
    if (flags & ESP_MATTER_CLUSTER_FLAG_CLIENT) {
        esp_matter_cluster_set_plugin_client_init_callback(cluster,
                                                           MatterOtaSoftwareUpdateRequestorPluginClientInitCallback);
    }

    esp_matter_attribute_create(cluster, ZCL_CLUSTER_REVISION_SERVER_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint16(config->cluster_revision));
    esp_matter_attribute_create(cluster, ZCL_DEFAULT_OTA_PROVIDERS_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_octet_str(config->default_ota_providers,
                                sizeof(config->default_ota_providers), 0));
    esp_matter_attribute_create(cluster, ZCL_UPDATE_POSSIBLE_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_bool(config->update_possible));
    esp_matter_attribute_create(cluster, ZCL_UPDATE_STATE_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_enum8(config->update_state));
    esp_matter_attribute_create(cluster, ZCL_UPDATE_STATE_PROGRESS_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint8(config->update_state_progress));

    esp_matter_command_create_announce_ota_provider(cluster);                              

    return cluster;
}

esp_matter_cluster_t *esp_matter_cluster_create_general_commissioning(esp_matter_endpoint_t *endpoint,
                                                            esp_matter_cluster_general_commissioning_config_t *config,
                                                            uint8_t flags)
{
    esp_matter_cluster_t *cluster = esp_matter_cluster_create(endpoint, ZCL_GENERAL_COMMISSIONING_CLUSTER_ID, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & ESP_MATTER_CLUSTER_FLAG_SERVER) {
        esp_matter_cluster_set_plugin_server_init_callback(cluster, MatterGeneralCommissioningPluginServerInitCallback);
        esp_matter_cluster_add_function_list(cluster, esp_matter_cluster_general_commissioning_function_list,
                                             esp_matter_cluster_general_commissioning_function_flags);
    }
    if (flags & ESP_MATTER_CLUSTER_FLAG_CLIENT) {
        esp_matter_cluster_set_plugin_client_init_callback(cluster, MatterGeneralCommissioningPluginClientInitCallback);
    }

    esp_matter_attribute_create(cluster, ZCL_CLUSTER_REVISION_SERVER_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint16(config->cluster_revision));
    esp_matter_attribute_create(cluster, ZCL_BREADCRUMB_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint64(config->breadcrumb));
    esp_matter_attribute_create(cluster, ZCL_BASICCOMMISSIONINGINFO_LIST_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_array(config->basic_commissioning_info,
                                sizeof(config->basic_commissioning_info), 0));
    esp_matter_attribute_create(cluster, ZCL_REGULATORYCONFIG_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_enum8(config->regulatory_config));
    esp_matter_attribute_create(cluster, ZCL_LOCATIONCAPABILITY_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_enum8(config->location_capability));

    esp_matter_command_create_arm_fail_safe(cluster);                              
    esp_matter_command_create_arm_fail_safe_response(cluster);                              
    esp_matter_command_create_set_regulatory_config(cluster);                              
    esp_matter_command_create_set_regulatory_config_response(cluster);                              
    esp_matter_command_create_commissioning_complete(cluster);                              
    esp_matter_command_create_commissioning_complete_response(cluster);                              

    return cluster;
}

esp_matter_cluster_t *esp_matter_cluster_create_network_commissioning(esp_matter_endpoint_t *endpoint,
                                                            esp_matter_cluster_network_commissioning_config_t *config,
                                                            uint8_t flags)
{
    esp_matter_cluster_t *cluster = esp_matter_cluster_create(endpoint, ZCL_NETWORK_COMMISSIONING_CLUSTER_ID, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & ESP_MATTER_CLUSTER_FLAG_SERVER) {
        esp_matter_cluster_set_plugin_server_init_callback(cluster, MatterNetworkCommissioningPluginServerInitCallback);
        esp_matter_cluster_add_function_list(cluster, esp_matter_cluster_network_commissioning_function_list,
                                             esp_matter_cluster_network_commissioning_function_flags);
    }
    if (flags & ESP_MATTER_CLUSTER_FLAG_CLIENT) {
        esp_matter_cluster_set_plugin_client_init_callback(cluster, MatterNetworkCommissioningPluginClientInitCallback);
    }

    esp_matter_attribute_create(cluster, ZCL_CLUSTER_REVISION_SERVER_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint16(config->cluster_revision));
    esp_matter_attribute_create(cluster, ZCL_FEATURE_MAP_SERVER_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_bitmap32(config->feature_map));
    esp_matter_attribute_create(cluster, ZCL_MAX_NETWORKS_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint8(config->max_networks));
    esp_matter_attribute_create(cluster, ZCL_NETWORKS_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_array(config->networks, sizeof(config->networks), 0));
    esp_matter_attribute_create(cluster, ZCL_SCAN_MAX_TIME_SECONDS_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint8(config->scan_max_time_seconds));
    esp_matter_attribute_create(cluster, ZCL_CONNECT_MAX_TIME_SECONDS_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint8(config->connect_max_time_seconds));
    esp_matter_attribute_create(cluster, ZCL_INTERFACE_ENABLED_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_bool(config->interface_enabled));
    esp_matter_attribute_create(cluster, ZCL_LAST_NETWORKING_STATUS_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_enum8(config->last_networking_status));
    esp_matter_attribute_create(cluster, ZCL_LAST_NETWORK_ID_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_octet_str(config->last_network_id, sizeof(config->last_network_id), 0));
    esp_matter_attribute_create(cluster, ZCL_LAST_CONNECT_ERROR_VALUE_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint32(config->last_connect_error_value));

    esp_matter_command_create_scan_networks(cluster);                              
    esp_matter_command_create_scan_networks_response(cluster);                              
    esp_matter_command_create_add_or_update_wifi_network(cluster);                              
    esp_matter_command_create_add_or_update_thread_network(cluster);                              
    esp_matter_command_create_remove_network(cluster);                              
    esp_matter_command_create_network_config_response(cluster);                              
    esp_matter_command_create_connect_network(cluster);                              
    esp_matter_command_create_connect_network_response(cluster);                              
    esp_matter_command_create_reorder_network(cluster);                              

    return cluster;
}

esp_matter_cluster_t *esp_matter_cluster_create_general_diagnostics(esp_matter_endpoint_t *endpoint,
                                                            esp_matter_cluster_general_diagnostics_config_t *config,
                                                            uint8_t flags)
{
    esp_matter_cluster_t *cluster = esp_matter_cluster_create(endpoint, ZCL_GENERAL_DIAGNOSTICS_CLUSTER_ID, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & ESP_MATTER_CLUSTER_FLAG_SERVER) {
        esp_matter_cluster_set_plugin_server_init_callback(cluster, MatterGeneralDiagnosticsPluginServerInitCallback);
        esp_matter_cluster_add_function_list(cluster, esp_matter_cluster_general_diagnostics_function_list,
                                             esp_matter_cluster_general_diagnostics_function_flags);
    }
    if (flags & ESP_MATTER_CLUSTER_FLAG_CLIENT) {
        esp_matter_cluster_set_plugin_client_init_callback(cluster, MatterGeneralDiagnosticsPluginClientInitCallback);
    }

    esp_matter_attribute_create(cluster, ZCL_CLUSTER_REVISION_SERVER_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint16(config->cluster_revision));
    esp_matter_attribute_create(cluster, ZCL_NETWORK_INTERFACES_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_array(config->network_interfaces, sizeof(config->network_interfaces), 0));
    esp_matter_attribute_create(cluster, ZCL_REBOOT_COUNT_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint16(config->reboot_count));

    return cluster;
}

esp_matter_cluster_t *esp_matter_cluster_create_administrator_commissioning(esp_matter_endpoint_t *endpoint,
                                                    esp_matter_cluster_administrator_commissioning_config_t *config,
                                                    uint8_t flags)
{
    esp_matter_cluster_t *cluster = esp_matter_cluster_create(endpoint, ZCL_ADMINISTRATOR_COMMISSIONING_CLUSTER_ID,
                                                              flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & ESP_MATTER_CLUSTER_FLAG_SERVER) {
        esp_matter_cluster_set_plugin_server_init_callback(cluster,
                                                           MatterAdministratorCommissioningPluginServerInitCallback);
        esp_matter_cluster_add_function_list(cluster, esp_matter_cluster_administrator_commissioning_function_list,
                                             esp_matter_cluster_administrator_commissioning_function_flags);
    }
    if (flags & ESP_MATTER_CLUSTER_FLAG_CLIENT) {
        esp_matter_cluster_set_plugin_client_init_callback(cluster,
                                                           MatterAdministratorCommissioningPluginClientInitCallback);
    }

    esp_matter_attribute_create(cluster, ZCL_CLUSTER_REVISION_SERVER_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint16(config->cluster_revision));
    esp_matter_attribute_create(cluster, ZCL_WINDOW_STATUS_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint8(config->window_status));
    esp_matter_attribute_create(cluster, ZCL_ADMIN_FABRIC_INDEX_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint16(config->admin_fabric_index));
    esp_matter_attribute_create(cluster, ZCL_ADMIN_VENDOR_ID_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint16(config->admin_vendor_id));

    esp_matter_command_create_open_commissioning_window(cluster);                              
    esp_matter_command_create_open_basic_commissioning_window(cluster);                              
    esp_matter_command_create_revoke_commissioning(cluster);                              

    return cluster;
}

esp_matter_cluster_t *esp_matter_cluster_create_operational_credentials(esp_matter_endpoint_t *endpoint,
                                                        esp_matter_cluster_operational_credentials_config_t *config,
                                                        uint8_t flags)
{
    esp_matter_cluster_t *cluster = esp_matter_cluster_create(endpoint, ZCL_OPERATIONAL_CREDENTIALS_CLUSTER_ID, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & ESP_MATTER_CLUSTER_FLAG_SERVER) {
        esp_matter_cluster_set_plugin_server_init_callback(cluster,
                                                           MatterOperationalCredentialsPluginServerInitCallback);
        esp_matter_cluster_add_function_list(cluster, esp_matter_cluster_operational_credentials_function_list,
                                             esp_matter_cluster_operational_credentials_function_flags);
    }
    if (flags & ESP_MATTER_CLUSTER_FLAG_CLIENT) {
        esp_matter_cluster_set_plugin_client_init_callback(cluster,
                                                           MatterOperationalCredentialsPluginClientInitCallback);
    }

    esp_matter_attribute_create(cluster, ZCL_CLUSTER_REVISION_SERVER_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint16(config->cluster_revision));
    /* Not implemented
    esp_matter_attribute_create(cluster, ZCL_NOCS_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_array(config->nocs, sizeof(config->nocs), 0));
    */
    esp_matter_attribute_create(cluster, ZCL_FABRICS_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_array(config->fabrics, sizeof(config->fabrics), 0));
    esp_matter_attribute_create(cluster, ZCL_SUPPORTED_FABRICS_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint8(config->supported_fabrics));
    esp_matter_attribute_create(cluster, ZCL_COMMISSIONED_FABRICS_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint8(config->commissioned_fabrics));
    esp_matter_attribute_create(cluster, ZCL_TRUSTED_ROOTS_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_array(config->trusted_root_certificates,
                                sizeof(config->trusted_root_certificates), 0));
    esp_matter_attribute_create(cluster, ZCL_CURRENT_FABRIC_INDEX_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint8(config->current_fabric_index));

    esp_matter_command_create_attestation_request(cluster);                              
    esp_matter_command_create_attestation_response(cluster);                              
    esp_matter_command_create_certificate_chain_request(cluster);                              
    esp_matter_command_create_certificate_chain_response(cluster);                              
    esp_matter_command_create_csr_request(cluster);                              
    esp_matter_command_create_csr_response(cluster);                              
    esp_matter_command_create_add_noc(cluster);    
    esp_matter_command_create_update_noc(cluster);                              
    esp_matter_command_create_noc_response(cluster);                              
    esp_matter_command_create_update_fabric_label(cluster);                              
    esp_matter_command_create_remove_fabric(cluster);                              
    esp_matter_command_create_add_trusted_root_certificate(cluster);                              
    esp_matter_command_create_remove_trusted_root_certificate(cluster);                              

    return cluster;
}

esp_matter_cluster_t *esp_matter_cluster_create_group_key_management(esp_matter_endpoint_t *endpoint,
                                                            esp_matter_cluster_group_key_management_config_t *config,
                                                            uint8_t flags)
{
    esp_matter_cluster_t *cluster = esp_matter_cluster_create(endpoint, ZCL_GROUP_KEY_MANAGEMENT_CLUSTER_ID, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & ESP_MATTER_CLUSTER_FLAG_SERVER) {
        esp_matter_cluster_set_plugin_server_init_callback(cluster, MatterGroupKeyManagementPluginServerInitCallback);
        esp_matter_cluster_add_function_list(cluster, esp_matter_cluster_group_key_management_function_list,
                                             esp_matter_cluster_group_key_management_function_flags);
    }
    if (flags & ESP_MATTER_CLUSTER_FLAG_CLIENT) {
        esp_matter_cluster_set_plugin_client_init_callback(cluster, MatterGroupKeyManagementPluginClientInitCallback);
    }

    esp_matter_attribute_create(cluster, ZCL_CLUSTER_REVISION_SERVER_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint16(config->cluster_revision));
    esp_matter_attribute_create(cluster, ZCL_GROUP_KEY_MAP_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_array(config->group_key_map, sizeof(config->group_key_map), 0));
    esp_matter_attribute_create(cluster, ZCL_GROUP_TABLE_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_array(config->group_table, sizeof(config->group_table), 0));
    /* Not implemented
    esp_matter_attribute_create(cluster, ZCL_MAX_GROUPS_PER_FABRIC_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint16(config->max_groups_per_fabric));
    esp_matter_attribute_create(cluster, ZCL_MAX_GROUP_KEYS_PER_FABRIC_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint16(config->max_group_keys_per_fabric));

    esp_matter_command_create_key_set_write(cluster);                              
    esp_matter_command_create_key_set_read(cluster);                              
    esp_matter_command_create_key_set_read_response(cluster);                              
    esp_matter_command_create_key_set_remove(cluster);                              
    esp_matter_command_create_key_set_read_all_indices(cluster);                              
    esp_matter_command_create_key_set_read_all_indices_response(cluster);                              
    */

    return cluster;
}

esp_matter_cluster_t *esp_matter_cluster_create_identify(esp_matter_endpoint_t *endpoint,
                                                         esp_matter_cluster_identify_config_t *config, uint8_t flags)
{
    esp_matter_cluster_t *cluster = esp_matter_cluster_create(endpoint, ZCL_IDENTIFY_CLUSTER_ID, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & ESP_MATTER_CLUSTER_FLAG_SERVER) {
        esp_matter_cluster_set_plugin_server_init_callback(cluster, MatterIdentifyPluginServerInitCallback);
        esp_matter_cluster_add_function_list(cluster, esp_matter_cluster_identify_function_list,
                                             esp_matter_cluster_identify_function_flags);
    }
    if (flags & ESP_MATTER_CLUSTER_FLAG_CLIENT) {
        esp_matter_cluster_set_plugin_client_init_callback(cluster, MatterIdentifyPluginClientInitCallback);
    }

    esp_matter_attribute_create(cluster, ZCL_CLUSTER_REVISION_SERVER_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint16(config->cluster_revision));
    esp_matter_attribute_create(cluster, ZCL_IDENTIFY_TIME_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint16(config->identify_time));
    esp_matter_attribute_create(cluster, ZCL_IDENTIFY_TYPE_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint8(config->identify_type));

    esp_matter_command_create_identify(cluster);                              
    esp_matter_command_create_identify_query(cluster);                              
    esp_matter_command_create_identify_query_response(cluster);                              

    return cluster;
}

esp_matter_cluster_t *esp_matter_cluster_create_groups(esp_matter_endpoint_t *endpoint,
                                                       esp_matter_cluster_groups_config_t *config, uint8_t flags)
{
    esp_matter_cluster_t *cluster = esp_matter_cluster_create(endpoint, ZCL_GROUPS_CLUSTER_ID, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & ESP_MATTER_CLUSTER_FLAG_SERVER) {
        esp_matter_cluster_set_plugin_server_init_callback(cluster, MatterGroupsPluginServerInitCallback);
        esp_matter_cluster_add_function_list(cluster, esp_matter_cluster_groups_function_list,
                                             esp_matter_cluster_groups_function_flags);
    }
    if (flags & ESP_MATTER_CLUSTER_FLAG_CLIENT) {
        esp_matter_cluster_set_plugin_client_init_callback(cluster, MatterGroupsPluginClientInitCallback);
    }

    esp_matter_attribute_create(cluster, ZCL_CLUSTER_REVISION_SERVER_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint16(config->cluster_revision));
    esp_matter_attribute_create(cluster, ZCL_GROUP_NAME_SUPPORT_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_bitmap8(config->name_support));

    esp_matter_command_create_add_group(cluster);                              
    esp_matter_command_create_view_group(cluster);                              
    esp_matter_command_create_get_group_membership(cluster);                              
    esp_matter_command_create_remove_group(cluster);                              
    esp_matter_command_create_remove_all_groups(cluster);                              
    esp_matter_command_create_add_group_if_identifying(cluster);                              
    esp_matter_command_create_add_group_response(cluster);                              
    esp_matter_command_create_view_group_response(cluster);                              
    esp_matter_command_create_get_group_membership_response(cluster);                              
    esp_matter_command_create_remove_group_response(cluster);                              

    return cluster;
}

esp_matter_cluster_t *esp_matter_cluster_create_scenes(esp_matter_endpoint_t *endpoint,
                                                       esp_matter_cluster_scenes_config_t *config, uint8_t flags)
{
    esp_matter_cluster_t *cluster = esp_matter_cluster_create(endpoint, ZCL_SCENES_CLUSTER_ID, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & ESP_MATTER_CLUSTER_FLAG_SERVER) {
        esp_matter_cluster_set_plugin_server_init_callback(cluster, MatterScenesPluginServerInitCallback);
        esp_matter_cluster_add_function_list(cluster, esp_matter_cluster_scenes_function_list,
                                             esp_matter_cluster_scenes_function_flags);
    }
    if (flags & ESP_MATTER_CLUSTER_FLAG_CLIENT) {
        esp_matter_cluster_set_plugin_client_init_callback(cluster, MatterScenesPluginClientInitCallback);
    }

    esp_matter_attribute_create(cluster, ZCL_CLUSTER_REVISION_SERVER_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint16(config->cluster_revision));
    esp_matter_attribute_create(cluster, ZCL_SCENE_COUNT_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint8(config->scene_count));
    esp_matter_attribute_create(cluster, ZCL_CURRENT_SCENE_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint8(config->current_scene));
    esp_matter_attribute_create(cluster, ZCL_CURRENT_GROUP_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint16(config->current_group));
    esp_matter_attribute_create(cluster, ZCL_SCENE_VALID_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_bool(config->scene_valid));
    esp_matter_attribute_create(cluster, ZCL_SCENE_NAME_SUPPORT_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_bitmap8(config->name_support));

    esp_matter_command_create_add_scene(cluster);                              
    esp_matter_command_create_view_scene(cluster);                              
    esp_matter_command_create_remove_scene(cluster);                              
    esp_matter_command_create_remove_all_scenes(cluster);                              
    esp_matter_command_create_store_scene(cluster);                              
    esp_matter_command_create_recall_scene(cluster);                              
    esp_matter_command_create_get_scene_membership(cluster);                              
    esp_matter_command_create_add_scene_response(cluster);                              
    esp_matter_command_create_view_scene_response(cluster);                              
    esp_matter_command_create_remove_scene_response(cluster);                              
    esp_matter_command_create_remove_all_scenes_response(cluster);                              
    esp_matter_command_create_store_scene_response(cluster);                              
    esp_matter_command_create_get_scene_membership_response(cluster);                              

    return cluster;
}

esp_matter_cluster_t *esp_matter_cluster_create_on_off(esp_matter_endpoint_t *endpoint,
                                                       esp_matter_cluster_on_off_config_t *config, uint8_t flags)
{
    esp_matter_cluster_t *cluster = esp_matter_cluster_create(endpoint, ZCL_ON_OFF_CLUSTER_ID, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & ESP_MATTER_CLUSTER_FLAG_SERVER) {
        esp_matter_cluster_set_plugin_server_init_callback(cluster, MatterOnOffPluginServerInitCallback);
        esp_matter_cluster_add_function_list(cluster, esp_matter_cluster_on_off_function_list,
                                             esp_matter_cluster_on_off_function_flags);
    }
    if (flags & ESP_MATTER_CLUSTER_FLAG_CLIENT) {
        esp_matter_cluster_set_plugin_client_init_callback(cluster, MatterOnOffPluginClientInitCallback);
    }

    esp_matter_attribute_create(cluster, ZCL_CLUSTER_REVISION_SERVER_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint16(config->cluster_revision));
    esp_matter_attribute_create(cluster, ZCL_ON_OFF_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_bool(config->on_off));

    esp_matter_command_create_off(cluster);    
    esp_matter_command_create_on(cluster);    
    esp_matter_command_create_toggle(cluster);    

    return cluster;
}

esp_matter_cluster_t *esp_matter_cluster_create_level_control(esp_matter_endpoint_t *endpoint,
                                                              esp_matter_cluster_level_control_config_t *config,
                                                              uint8_t flags)
{
    esp_matter_cluster_t *cluster = esp_matter_cluster_create(endpoint, ZCL_LEVEL_CONTROL_CLUSTER_ID, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & ESP_MATTER_CLUSTER_FLAG_SERVER) {
        esp_matter_cluster_set_plugin_server_init_callback(cluster, MatterLevelControlPluginServerInitCallback);
        esp_matter_cluster_add_function_list(cluster, esp_matter_cluster_level_control_function_list,
                                             esp_matter_cluster_level_control_function_flags);
    }
    if (flags & ESP_MATTER_CLUSTER_FLAG_CLIENT) {
        esp_matter_cluster_set_plugin_client_init_callback(cluster, MatterLevelControlPluginClientInitCallback);
    }

    esp_matter_attribute_create(cluster, ZCL_CLUSTER_REVISION_SERVER_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint16(config->cluster_revision));
    esp_matter_attribute_create(cluster, ZCL_CURRENT_LEVEL_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint8(config->current_level));
    esp_matter_attribute_create(cluster, ZCL_ON_LEVEL_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint8(config->on_level));
    esp_matter_attribute_create(cluster, ZCL_OPTIONS_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_bitmap8(config->options));

    esp_matter_command_create_move_to_level(cluster);                              
    esp_matter_command_create_move(cluster);    
    esp_matter_command_create_step(cluster);    
    esp_matter_command_create_stop(cluster);    
    esp_matter_command_create_move_to_level_with_on_off(cluster);                              
    esp_matter_command_create_move_with_on_off(cluster);                              
    esp_matter_command_create_step_with_on_off(cluster);                              
    esp_matter_command_create_stop_with_on_off(cluster);                              

    return cluster;
}

esp_matter_cluster_t *esp_matter_cluster_create_color_control(esp_matter_endpoint_t *endpoint,
                                                              esp_matter_cluster_color_control_config_t *config,
                                                              uint8_t flags)
{
    esp_matter_cluster_t *cluster = esp_matter_cluster_create(endpoint, ZCL_COLOR_CONTROL_CLUSTER_ID, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & ESP_MATTER_CLUSTER_FLAG_SERVER) {
        esp_matter_cluster_set_plugin_server_init_callback(cluster, MatterColorControlPluginServerInitCallback);
        esp_matter_cluster_add_function_list(cluster, esp_matter_cluster_color_control_function_list,
                                             esp_matter_cluster_color_control_function_flags);
    }
    if (flags & ESP_MATTER_CLUSTER_FLAG_CLIENT) {
        esp_matter_cluster_set_plugin_client_init_callback(cluster, MatterColorControlPluginClientInitCallback);
    }

    esp_matter_attribute_create(cluster, ZCL_CLUSTER_REVISION_SERVER_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint16(config->cluster_revision));
    esp_matter_attribute_create(cluster, ZCL_FEATURE_MAP_SERVER_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_bitmap32(config->feature_map));
    esp_matter_attribute_create(cluster, ZCL_COLOR_CONTROL_CURRENT_HUE_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint8(config->current_hue));
    esp_matter_attribute_create(cluster, ZCL_COLOR_CONTROL_CURRENT_SATURATION_ATTRIBUTE_ID,
                                ESP_MATTER_ATTRIBUTE_FLAG_NONE, esp_matter_uint8(config->current_saturation));
    esp_matter_attribute_create(cluster, ZCL_COLOR_CONTROL_COLOR_MODE_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_enum8(config->color_mode));
    esp_matter_attribute_create(cluster, ZCL_OPTIONS_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_bitmap8(config->options));
    esp_matter_attribute_create(cluster, ZCL_COLOR_CONTROL_ENHANCED_COLOR_MODE_ATTRIBUTE_ID,
                                ESP_MATTER_ATTRIBUTE_FLAG_NONE, esp_matter_enum8(config->enhanced_color_mode));
    esp_matter_attribute_create(cluster, ZCL_COLOR_CONTROL_COLOR_CAPABILITIES_ATTRIBUTE_ID,
                                ESP_MATTER_ATTRIBUTE_FLAG_NONE, esp_matter_bitmap16(config->color_capabilities));

    esp_matter_command_create_move_to_hue(cluster);                              
    esp_matter_command_create_move_hue(cluster);                              
    esp_matter_command_create_step_hue(cluster);                              
    esp_matter_command_create_move_to_saturation(cluster);                              
    esp_matter_command_create_move_saturation(cluster);                              
    esp_matter_command_create_step_saturation(cluster);                              
    esp_matter_command_create_move_to_hue_and_saturation(cluster);                              

    return cluster;
}

esp_matter_cluster_t *esp_matter_cluster_create_fan_control(esp_matter_endpoint_t *endpoint,
                                                            esp_matter_cluster_fan_control_config_t *config,
                                                            uint8_t flags)
{
    esp_matter_cluster_t *cluster = esp_matter_cluster_create(endpoint, ZCL_FAN_CONTROL_CLUSTER_ID, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & ESP_MATTER_CLUSTER_FLAG_SERVER) {
        esp_matter_cluster_set_plugin_server_init_callback(cluster, MatterFanControlPluginServerInitCallback);
        esp_matter_cluster_add_function_list(cluster, esp_matter_cluster_fan_control_function_list,
                                             esp_matter_cluster_fan_control_function_flags);
    }
    if (flags & ESP_MATTER_CLUSTER_FLAG_CLIENT) {
        esp_matter_cluster_set_plugin_client_init_callback(cluster, MatterFanControlPluginClientInitCallback);
    }

    esp_matter_attribute_create(cluster, ZCL_CLUSTER_REVISION_SERVER_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint16(config->cluster_revision));
    esp_matter_attribute_create(cluster, ZCL_FAN_CONTROL_FAN_MODE_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_enum8(config->fan_mode));
    esp_matter_attribute_create(cluster, ZCL_FAN_CONTROL_FAN_MODE_SEQUENCE_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_enum8(config->fan_mode_sequence));
    /* Not implemented
    esp_matter_attribute_create(cluster, ZCL_FAN_CONTROL_PERCENT_SETTING_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint8(config->percent_setting));
    esp_matter_attribute_create(cluster, ZCL_FAN_CONTROL_PERCENT_CURRENT_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint8(config->percent_current));
    esp_matter_attribute_create(cluster, ZCL_FAN_CONTROL_SPEED_MAX_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint8(config->speed_max));
    esp_matter_attribute_create(cluster, ZCL_FAN_CONTROL_SPEED_SETTING_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint8(config->speed_max));
    esp_matter_attribute_create(cluster, ZCL_FAN_CONTROL_SPEED_CURRENT_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_enum8(config->speed_setting));
    esp_matter_attribute_create(cluster, ZCL_FAN_CONTROL_ROCK_SUPPORT_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_bitmap8(config->rock_support));
    esp_matter_attribute_create(cluster, ZCL_FAN_CONTROL_ROCK_SETTING_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_bitmap8(config->rock_setting));
    esp_matter_attribute_create(cluster, ZCL_FAN_CONTROL_WIND_SUPPORT_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_bitmap8(config->wind_support));
    esp_matter_attribute_create(cluster, ZCL_FAN_CONTROL_WIND_SETTING_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_bitmap8(config->wind_setting));
    */

    return cluster;
}

esp_matter_cluster_t *esp_matter_cluster_create_thermostat(esp_matter_endpoint_t *endpoint,
                                                           esp_matter_cluster_thermostat_config_t *config,
                                                           uint8_t flags)
{
    esp_matter_cluster_t *cluster = esp_matter_cluster_create(endpoint, ZCL_THERMOSTAT_CLUSTER_ID, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & ESP_MATTER_CLUSTER_FLAG_SERVER) {
        esp_matter_cluster_set_plugin_server_init_callback(cluster, MatterThermostatPluginServerInitCallback);
        esp_matter_cluster_add_function_list(cluster, esp_matter_cluster_thermostat_function_list,
                                             esp_matter_cluster_thermostat_function_flags);
    }
    if (flags & ESP_MATTER_CLUSTER_FLAG_CLIENT) {
        esp_matter_cluster_set_plugin_client_init_callback(cluster, MatterThermostatPluginClientInitCallback);
    }

    esp_matter_attribute_create(cluster, ZCL_CLUSTER_REVISION_SERVER_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint16(config->cluster_revision));
    esp_matter_attribute_create(cluster, ZCL_LOCAL_TEMPERATURE_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_int16(config->local_temperature));
    esp_matter_attribute_create(cluster, ZCL_OCCUPIED_COOLING_SETPOINT_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_int16(config->occupied_cooling_setpoint));
    esp_matter_attribute_create(cluster, ZCL_OCCUPIED_HEATING_SETPOINT_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_int16(config->occupied_heating_setpoint));
    esp_matter_attribute_create(cluster, ZCL_CONTROL_SEQUENCE_OF_OPERATION_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_enum8(config->control_sequence_of_operation));
    esp_matter_attribute_create(cluster, ZCL_SYSTEM_MODE_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_enum8(config->system_mode));

    esp_matter_command_create_setpoint_raise_lower(cluster);

    return cluster;
}

esp_matter_cluster_t *esp_matter_cluster_create_door_lock(esp_matter_endpoint_t *endpoint,
                                                          esp_matter_cluster_door_lock_config_t *config,
                                                          uint8_t flags)
{
    esp_matter_cluster_t *cluster = esp_matter_cluster_create(endpoint, ZCL_DOOR_LOCK_CLUSTER_ID, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & ESP_MATTER_CLUSTER_FLAG_SERVER) {
        esp_matter_cluster_set_plugin_server_init_callback(cluster, MatterDoorLockPluginServerInitCallback);
        esp_matter_cluster_add_function_list(cluster, esp_matter_cluster_door_lock_function_list,
                                             esp_matter_cluster_door_lock_function_flags);
    }
    if (flags & ESP_MATTER_CLUSTER_FLAG_CLIENT) {
        esp_matter_cluster_set_plugin_client_init_callback(cluster, MatterDoorLockPluginClientInitCallback);
    }

    esp_matter_attribute_create(cluster, ZCL_CLUSTER_REVISION_SERVER_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint8(config->cluster_revision));
    esp_matter_attribute_create(cluster, ZCL_LOCK_STATE_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_enum8(config->lock_state));
    esp_matter_attribute_create(cluster, ZCL_LOCK_TYPE_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_enum8(config->lock_type));
    esp_matter_attribute_create(cluster, ZCL_ACTUATOR_ENABLED_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_bool(config->actuator_enabled));
    esp_matter_attribute_create(cluster, ZCL_AUTO_RELOCK_TIME_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint32(config->auto_relock_time));
    esp_matter_attribute_create(cluster, ZCL_OPERATING_MODE_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_enum8(config->operating_mode));
    esp_matter_attribute_create(cluster, ZCL_SUPPORTED_OPERATING_MODES_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_bitmap16(config->supported_operating_modes));

    esp_matter_command_create_lock_door(cluster);
    esp_matter_command_create_unlock_door(cluster);

    return cluster;
}

esp_matter_cluster_t *esp_matter_cluster_create_time_synchronization(esp_matter_endpoint_t *endpoint,
                                                          esp_matter_cluster_time_synchronization_config_t *config,
                                                          uint8_t flags)
{
    esp_matter_cluster_t *cluster = esp_matter_cluster_create(endpoint, ZCL_TIME_SYNCHRONIZATION_CLUSTER_ID, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return  NULL;
    }

    if (flags & ESP_MATTER_CLUSTER_FLAG_SERVER) {
        esp_matter_cluster_set_plugin_server_init_callback(cluster, MatterTimeSynchronizationPluginServerInitCallback);
        esp_matter_cluster_add_function_list(cluster, esp_matter_cluster_time_synchronization_function_list,
                                             esp_matter_cluster_time_synchronization_function_flags);
    }
    if (flags & ESP_MATTER_CLUSTER_FLAG_CLIENT) {
        esp_matter_cluster_set_plugin_client_init_callback(cluster, MatterTimeSynchronizationPluginClientInitCallback);
    }

    esp_matter_attribute_create(cluster, ZCL_CLUSTER_REVISION_SERVER_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint16(config->cluster_revision));
    return cluster;
}

esp_matter_cluster_t *esp_matter_cluster_create_bridged_device_basic(esp_matter_endpoint_t *endpoint,
                                                                     esp_matter_cluster_bridged_device_basic_config_t *config,
                                                                     uint8_t flags)
{
    esp_matter_cluster_t *cluster = esp_matter_cluster_create(endpoint, ZCL_BRIDGED_DEVICE_BASIC_CLUSTER_ID, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return  NULL;
    }

    if (flags & ESP_MATTER_CLUSTER_FLAG_SERVER) {
        // There is not PluginServer(Client)InitCallback for this cluster
        esp_matter_cluster_add_function_list(cluster, esp_matter_cluster_bridged_device_basic_function_list,
                                             esp_matter_cluster_bridged_device_basic_function_flags);
    }

    esp_matter_attribute_create(cluster, ZCL_CLUSTER_REVISION_SERVER_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint16(config->cluster_revision));
    esp_matter_attribute_create(cluster, ZCL_NODE_LABEL_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_char_str(config->node_label, sizeof(config->node_label)));
    esp_matter_attribute_create(cluster, ZCL_REACHABLE_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_bool(config->reachable));
    return cluster;
}

esp_matter_cluster_t *esp_matter_cluster_create_fixed_label(esp_matter_endpoint_t *endpoint,
                                                            esp_matter_cluster_fixed_label_config_t *config, uint8_t flags)
{
    esp_matter_cluster_t *cluster = esp_matter_cluster_create(endpoint, ZCL_FIXED_LABEL_CLUSTER_ID, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return  NULL;
    }

    if (flags & ESP_MATTER_CLUSTER_FLAG_SERVER) {
        esp_matter_cluster_set_plugin_server_init_callback(cluster, MatterFixedLabelPluginServerInitCallback);
        esp_matter_cluster_add_function_list(cluster, esp_matter_cluster_fixed_label_function_list,
                                             esp_matter_cluster_fixed_label_function_flags);
    }
    if (flags & ESP_MATTER_CLUSTER_FLAG_CLIENT) {
        esp_matter_cluster_set_plugin_client_init_callback(cluster, MatterFixedLabelPluginClientInitCallback);
    }

    esp_matter_attribute_create(cluster, ZCL_CLUSTER_REVISION_SERVER_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint16(config->cluster_revision));
    esp_matter_attribute_create(cluster, ZCL_LABEL_LIST_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_array(config->label_list, sizeof(config->label_list), 0));
    return cluster;
}

esp_matter_cluster_t *esp_matter_cluster_create_switch(esp_matter_endpoint_t *endpoint,
                                                       esp_matter_cluster_switch_config_t *config, uint8_t flags)
{
    esp_matter_cluster_t *cluster = esp_matter_cluster_create(endpoint, ZCL_SWITCH_CLUSTER_ID, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return  NULL;
    }

    if (flags & ESP_MATTER_CLUSTER_FLAG_SERVER) {
        esp_matter_cluster_set_plugin_server_init_callback(cluster, MatterSwitchPluginServerInitCallback);
        esp_matter_cluster_add_function_list(cluster, esp_matter_cluster_switch_function_list,
                                             esp_matter_cluster_switch_function_flags);
    }
    if (flags & ESP_MATTER_CLUSTER_FLAG_CLIENT) {
        esp_matter_cluster_set_plugin_client_init_callback(cluster, MatterSwitchPluginClientInitCallback);
    }

    esp_matter_attribute_create(cluster, ZCL_CLUSTER_REVISION_SERVER_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint16(config->cluster_revision));
    esp_matter_attribute_create(cluster, ZCL_NUMBER_OF_POSITIONS_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint8(config->number_of_positions));
    esp_matter_attribute_create(cluster, ZCL_CURRENT_POSITION_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint8(config->current_position));
    esp_matter_attribute_create(cluster, ZCL_MULTI_PRESS_MAX_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint8(config->multi_press_max));

    return cluster;
}

esp_matter_cluster_t *esp_matter_cluster_create_temperature_measurement(esp_matter_endpoint_t *endpoint,
                                                              esp_matter_cluster_temperature_measurement_config_t *config,
                                                              uint8_t flags)
{
    esp_matter_cluster_t *cluster = esp_matter_cluster_create(endpoint, ZCL_TEMP_MEASUREMENT_CLUSTER_ID, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & ESP_MATTER_CLUSTER_FLAG_SERVER) {
        esp_matter_cluster_set_plugin_server_init_callback(cluster, MatterTemperatureMeasurementPluginServerInitCallback);
        esp_matter_cluster_add_function_list(cluster, esp_matter_cluster_temperature_measurement_function_list,
                                             esp_matter_cluster_temperature_measurement_function_flags);
    }
    if (flags & ESP_MATTER_CLUSTER_FLAG_CLIENT) {
        esp_matter_cluster_set_plugin_client_init_callback(cluster, MatterTemperatureMeasurementPluginClientInitCallback);
    }

    esp_matter_attribute_create(cluster, ZCL_CLUSTER_REVISION_SERVER_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint16(config->cluster_revision));
    esp_matter_attribute_create(cluster, ZCL_TEMP_MEASURED_VALUE_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_int16(config->measured_value));
    esp_matter_attribute_create(cluster, ZCL_TEMP_MIN_MEASURED_VALUE_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_int16(config->min_measured_value));
    esp_matter_attribute_create(cluster, ZCL_TEMP_MAX_MEASURED_VALUE_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_int16(config->max_measured_value));

    return cluster;
}
