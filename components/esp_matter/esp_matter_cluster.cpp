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
#include <esp_matter_attribute.h>
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

const esp_matter_cluster_function_generic_t *esp_matter_cluster_access_control_function_list = NULL;
const int esp_matter_cluster_access_control_function_flags = ESP_MATTER_CLUSTER_FLAG_NONE;

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

esp_matter_cluster_t *esp_matter_cluster_create_descriptor(esp_matter_endpoint_t *endpoint, uint8_t flags)
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

    /* Attributes managed internally */
    esp_matter_attribute_create_cluster_revision(cluster, 0);
    esp_matter_attribute_create_device_list(cluster, NULL, 0, 0);
    esp_matter_attribute_create_server_list(cluster, NULL, 0, 0);
    esp_matter_attribute_create_client_list(cluster, NULL, 0, 0);
    esp_matter_attribute_create_parts_list(cluster, NULL, 0, 0);

    return cluster;
}

esp_matter_cluster_t *esp_matter_cluster_create_access_control(esp_matter_endpoint_t *endpoint, uint8_t flags)
{
    esp_matter_cluster_t *cluster = esp_matter_cluster_create(endpoint, ZCL_ACCESS_CONTROL_CLUSTER_ID,
                                                              ESP_MATTER_CLUSTER_FLAG_SERVER);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }
    if (flags & ESP_MATTER_CLUSTER_FLAG_SERVER) {
        esp_matter_cluster_set_plugin_server_init_callback(cluster, MatterAccessControlPluginServerInitCallback);
        esp_matter_cluster_add_function_list(cluster, esp_matter_cluster_access_control_function_list,
                                             esp_matter_cluster_access_control_function_flags);
    }
    if (flags & ESP_MATTER_CLUSTER_FLAG_CLIENT) {
        esp_matter_cluster_set_plugin_client_init_callback(cluster, MatterAccessControlPluginClientInitCallback);
    }

    /* Attributes managed internally */
    esp_matter_attribute_create_cluster_revision(cluster, 0);
    esp_matter_attribute_create_acl(cluster, NULL, 0, 0);
    esp_matter_attribute_create_extension(cluster, NULL, 0, 0);
    return cluster;
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

    /* Attributes managed internally */
    esp_matter_attribute_create_data_model_revision(cluster, 0);
    esp_matter_attribute_create_location(cluster, NULL, 0);
    esp_matter_attribute_create_vendor_name(cluster, NULL, 0);
    esp_matter_attribute_create_vendor_id(cluster, 0);
    esp_matter_attribute_create_product_name(cluster, NULL, 0);
    esp_matter_attribute_create_product_id(cluster, 0);
    esp_matter_attribute_create_hardware_version(cluster, 0);
    esp_matter_attribute_create_hardware_version_string(cluster, NULL, 0);
    esp_matter_attribute_create_software_version(cluster, 0);
    esp_matter_attribute_create_software_version_string(cluster, NULL, 0);

    /* Attributes not managed internally */
    esp_matter_attribute_create_cluster_revision(cluster, config->cluster_revision);
    esp_matter_attribute_create_node_label(cluster, config->node_label, sizeof(config->node_label));

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
        esp_matter_cluster_set_plugin_server_init_callback(cluster, MatterBindingPluginServerInitCallback);
        esp_matter_cluster_add_function_list(cluster, esp_matter_cluster_binding_function_list,
                                             esp_matter_cluster_binding_function_flags);
    }
    if (flags & ESP_MATTER_CLUSTER_FLAG_CLIENT) {
        esp_matter_cluster_set_plugin_client_init_callback(cluster, MatterBindingPluginClientInitCallback);
    }
 
    /* Extra initialization */
    esp_matter_binding_init();

    /* Attributes managed internally */
    esp_matter_attribute_create_binding(cluster, NULL, 0, 0);

    /* Attributes not managed internally */
    esp_matter_attribute_create_cluster_revision(cluster, config->cluster_revision);

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

    /* Attributes not managed internally */
    esp_matter_attribute_create_cluster_revision(cluster, config->cluster_revision);

    /* Commands */
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

    /* Attributes managed internally */
    esp_matter_attribute_create_default_ota_providers(cluster, NULL, 0);

    /* Attributes not managed internally */
    esp_matter_attribute_create_cluster_revision(cluster, config->cluster_revision);
    esp_matter_attribute_create_update_possible(cluster, config->update_possible);
    esp_matter_attribute_create_update_state(cluster, config->update_state);
    esp_matter_attribute_create_update_state_progress(cluster, config->update_state_progress);

    /* Commands */
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

    /* Attributes managed internally */
    esp_matter_attribute_create_regulatory_config(cluster, 0);
    esp_matter_attribute_create_location_capability(cluster, 0);
    esp_matter_attribute_create_basic_commissioning_info(cluster, NULL, 0, 0);

    /* Attributes not managed internally */
    esp_matter_attribute_create_cluster_revision(cluster, config->cluster_revision);
    esp_matter_attribute_create_breadcrumb(cluster, config->breadcrumb);

    /* Commands */
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

    /* Attributes managed internally */
    esp_matter_attribute_create_max_networks(cluster, 0);
    esp_matter_attribute_create_networks(cluster, NULL, 0, 0);
    esp_matter_attribute_create_scan_max_time_seconds(cluster, 0);
    esp_matter_attribute_create_connect_max_time_seconds(cluster, 0);
    esp_matter_attribute_create_interface_enabled(cluster, 0);
    esp_matter_attribute_create_last_networking_status(cluster, 0);
    esp_matter_attribute_create_last_network_id(cluster, NULL, 0);
    esp_matter_attribute_create_last_connect_error_value(cluster, 0);
    esp_matter_attribute_create_feature_map(cluster, 0);

    /* Attributes not managed internally */
    esp_matter_attribute_create_cluster_revision(cluster, config->cluster_revision);

    /* Commands */
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

    /* Attributes managed internally */
    esp_matter_attribute_create_network_interfaces(cluster, NULL, 0, 0);
    esp_matter_attribute_create_reboot_count(cluster, 0);

    /* Attributes not managed internally */
    esp_matter_attribute_create_cluster_revision(cluster, config->cluster_revision);

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

    /* Attributes managed internally */
    esp_matter_attribute_create_window_status(cluster, 0);
    esp_matter_attribute_create_admin_fabric_index(cluster, 0);
    esp_matter_attribute_create_admin_vendor_id(cluster, 0);

    /* Attributes not managed internally */
    esp_matter_attribute_create_cluster_revision(cluster, config->cluster_revision);

    /* Commands */
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

    /* Attributes managed internally */
    esp_matter_attribute_create_nocs(cluster, NULL, 0, 0);
    esp_matter_attribute_create_supported_fabrics(cluster, 0);
    esp_matter_attribute_create_commissioned_fabrics(cluster, 0);
    esp_matter_attribute_create_fabrics(cluster, NULL, 0, 0);
    esp_matter_attribute_create_trusted_root_certificates(cluster, NULL, 0, 0);
    esp_matter_attribute_create_current_fabric_index(cluster, 0);

    /* Attributes not managed internally */
    esp_matter_attribute_create_cluster_revision(cluster, config->cluster_revision);

    /* Commands */
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

esp_matter_cluster_t *esp_matter_cluster_create_group_key_management(esp_matter_endpoint_t *endpoint, uint8_t flags)
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

    /* Attributes managed internally */
    esp_matter_attribute_create_cluster_revision(cluster, 0);
    esp_matter_attribute_create_group_key_map(cluster, NULL, 0, 0);
    esp_matter_attribute_create_group_table(cluster, NULL, 0, 0);
    esp_matter_attribute_create_max_groups_per_fabric(cluster, 0);
    esp_matter_attribute_create_max_group_keys_per_fabric(cluster, 0);

    /* Commands */
    esp_matter_command_create_key_set_write(cluster);                              
    esp_matter_command_create_key_set_read(cluster);                              
    esp_matter_command_create_key_set_read_response(cluster);                              
    esp_matter_command_create_key_set_remove(cluster);                              
    esp_matter_command_create_key_set_read_all_indices(cluster);                              
    esp_matter_command_create_key_set_read_all_indices_response(cluster);                              

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

    /* Attributes not managed internally */
    esp_matter_attribute_create_cluster_revision(cluster, config->cluster_revision);
    esp_matter_attribute_create_identify_time(cluster, config->identify_time, 0x0, 0xFE);
    esp_matter_attribute_create_identify_type(cluster, config->identify_type);

    /* Commands */
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

    /* Attributes not managed internally */
    esp_matter_attribute_create_cluster_revision(cluster, config->cluster_revision);
    esp_matter_attribute_create_group_name_support(cluster, config->group_name_support);

    /* Commands */
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

    /* Attributes not managed internally */
    esp_matter_attribute_create_cluster_revision(cluster, config->cluster_revision);
    esp_matter_attribute_create_scene_count(cluster, config->scene_count);
    esp_matter_attribute_create_current_scene(cluster, config->current_scene);
    esp_matter_attribute_create_current_group(cluster, config->current_group);
    esp_matter_attribute_create_scene_valid(cluster, config->scene_valid);
    esp_matter_attribute_create_scene_name_support(cluster, config->scene_name_support);

    /* Commands */
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

    /* Attributes not managed internally */
    esp_matter_attribute_create_cluster_revision(cluster, config->cluster_revision);
    esp_matter_attribute_create_on_off(cluster, config->on_off);

    /* Commands */
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

    /* Attributes not managed internally */
    esp_matter_attribute_create_cluster_revision(cluster, config->cluster_revision);
    esp_matter_attribute_create_current_level(cluster, config->current_level);
    esp_matter_attribute_create_on_level(cluster, config->on_level);
    esp_matter_attribute_create_options(cluster, config->options, 0x0, 0x3);

    /* Commands */
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

    /* Attributes not managed internally */
    esp_matter_attribute_create_cluster_revision(cluster, config->cluster_revision);
    esp_matter_attribute_create_feature_map(cluster, config->feature_map);
    esp_matter_attribute_create_current_hue(cluster, config->current_hue);
    esp_matter_attribute_create_current_saturation(cluster, config->current_saturation);
    esp_matter_attribute_create_color_mode(cluster, config->color_mode);
    esp_matter_attribute_create_color_control_options(cluster, config->color_control_options);
    esp_matter_attribute_create_enhanced_color_mode(cluster, config->enhanced_color_mode);
    esp_matter_attribute_create_color_capabilities(cluster, config->color_capabilities);

    /* Commands */
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
        /* not implemented: Setting NULL since the MatterFanControlPluginServerInitCallback is not implemented */
        esp_matter_cluster_set_plugin_server_init_callback(cluster, NULL);
        esp_matter_cluster_add_function_list(cluster, esp_matter_cluster_fan_control_function_list,
                                             esp_matter_cluster_fan_control_function_flags);
    }
    if (flags & ESP_MATTER_CLUSTER_FLAG_CLIENT) {
        /* not implemented: Setting NULL since the MatterFanControlPluginClientInitCallback is not implemented */
        esp_matter_cluster_set_plugin_client_init_callback(cluster, NULL);
    }

    /* Attributes not managed internally */
    esp_matter_attribute_create_cluster_revision(cluster, config->cluster_revision);
    esp_matter_attribute_create_fan_mode(cluster, config->fan_mode);
    esp_matter_attribute_create_fan_mode_sequence(cluster, config->fan_mode_sequence);
    /* Not implemented
    esp_matter_attribute_create_percent_setting(cluster, config->percent_setting);
    esp_matter_attribute_create_percent_current(cluster, config->percent_current);
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

    /* Attributes not managed internally */
    esp_matter_attribute_create_cluster_revision(cluster, config->cluster_revision);
    esp_matter_attribute_create_local_temperature(cluster, config->local_temperature);
    esp_matter_attribute_create_occupied_cooling_setpoint(cluster, config->occupied_cooling_setpoint);
    esp_matter_attribute_create_occupied_heating_setpoint(cluster, config->occupied_heating_setpoint);
    esp_matter_attribute_create_control_sequence_of_operation(cluster, config->control_sequence_of_operation, 0x0, 0x5);
    esp_matter_attribute_create_system_mode(cluster, config->system_mode, 0x0, 0x7);

    /* Commands */
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

    /* Attributes not managed internally */
    esp_matter_attribute_create_cluster_revision(cluster, config->cluster_revision);
    esp_matter_attribute_create_lock_state(cluster, config->lock_state);
    esp_matter_attribute_create_lock_type(cluster, config->lock_type);
    esp_matter_attribute_create_actuator_enabled(cluster, config->actuator_enabled);
    esp_matter_attribute_create_auto_relock_time(cluster, config->auto_relock_time);
    esp_matter_attribute_create_operating_mode(cluster, config->operating_mode, 0x0, 0x4);
    esp_matter_attribute_create_supported_operating_modes(cluster, config->supported_operating_modes);

    /* Commands */
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

    /* Attributes not managed internally */
    esp_matter_attribute_create_cluster_revision(cluster, config->cluster_revision);

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

    /* Attributes not managed internally */
    esp_matter_attribute_create_cluster_revision(cluster, config->cluster_revision);
    esp_matter_attribute_create_bridged_device_basic_node_label(cluster, config->node_label,
                                                                sizeof(config->node_label));
    esp_matter_attribute_create_reachable(cluster, config->reachable);

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

    /* Attributes managed internally */
    esp_matter_attribute_create_label_list(cluster, NULL, 0, 0);

    /* Attributes not managed internally */
    esp_matter_attribute_create_cluster_revision(cluster, config->cluster_revision);

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

    /* Attributes not managed internally */
    esp_matter_attribute_create_cluster_revision(cluster, config->cluster_revision);
    esp_matter_attribute_create_number_of_positions(cluster, config->number_of_positions);
    esp_matter_attribute_create_current_position(cluster, config->current_position);
    esp_matter_attribute_create_multi_press_max(cluster, config->multi_press_max);

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

    /* Attributes not managed internally */
    esp_matter_attribute_create_cluster_revision(cluster, config->cluster_revision);
    esp_matter_attribute_create_temperature_measured_value(cluster, config->measured_value);
    esp_matter_attribute_create_temperature_min_measured_value(cluster, config->min_measured_value);
    esp_matter_attribute_create_temperature_max_measured_value(cluster, config->max_measured_value);

    return cluster;
}
