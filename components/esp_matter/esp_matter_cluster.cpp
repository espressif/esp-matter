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

#include <esp_matter.h>
#include <esp_matter_cluster.h>
#include <esp_matter_core.h>

#include <app-common/zap-generated/att-storage.h>
#include <app-common/zap-generated/attribute-id.h>
#include <app-common/zap-generated/cluster-id.h>
#include <app-common/zap-generated/command-id.h>

esp_matter_cluster_t *esp_matter_cluster_create_descriptor(esp_matter_endpoint_t *endpoint,
                                                           esp_matter_cluster_descriptor_config_t *config,
                                                           uint8_t flags)
{
    esp_matter_cluster_t *cluster = esp_matter_cluster_create(endpoint, ZCL_DESCRIPTOR_CLUSTER_ID, flags);

    esp_matter_attribute_create(cluster, ZCL_CLUSTER_REVISION_SERVER_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_uint16(config->cluster_revision));
    esp_matter_attribute_create(cluster, ZCL_DEVICE_LIST_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_array(config->device_type_list, sizeof(config->device_type_list), 0));
    esp_matter_attribute_create(cluster, ZCL_SERVER_LIST_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_array(config->server_list, sizeof(config->server_list), 0));
    esp_matter_attribute_create(cluster, ZCL_CLIENT_LIST_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_array(config->client_list, sizeof(config->client_list), 0));
    esp_matter_attribute_create(cluster, ZCL_PARTS_LIST_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_array(config->parts_list, sizeof(config->parts_list), 0));

    return cluster;
}

esp_matter_cluster_t *esp_matter_cluster_create_access_control(esp_matter_endpoint_t *endpoint,
                                                               esp_matter_cluster_access_control_config_t *config,
                                                               uint8_t flags)
{
    /* Not implemented
    esp_matter_cluster_t *cluster = esp_matter_cluster_create(endpoint, ZCL_ACCESS_CONTROL_CLUSTER_ID,
                                                              CLUSTER_MASK_SERVER);

    esp_matter_attribute_create(cluster, ZCL_CLUSTER_REVISION_SERVER_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_uint16(config->cluster_revision));
    esp_matter_attribute_create(cluster, ZCL_ACL_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_array(config->acl, sizeof(config->acl), 0));
    esp_matter_attribute_create(cluster, ZCL_EXTENSION_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_array(config->extension, sizeof(config->extension), 0));

    return cluster;
    */
    return NULL;
}

esp_matter_cluster_t *esp_matter_cluster_create_basic(esp_matter_endpoint_t *endpoint,
                                                      esp_matter_cluster_basic_config_t *config, uint8_t flags)
{
    esp_matter_cluster_t *cluster = esp_matter_cluster_create(endpoint, ZCL_BASIC_CLUSTER_ID, flags);

    esp_matter_attribute_create(cluster, ZCL_CLUSTER_REVISION_SERVER_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_uint16(config->cluster_revision));
    esp_matter_attribute_create(cluster, ZCL_INTERACTION_MODEL_VERSION_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_uint16(config->interaction_model_version));
    esp_matter_attribute_create(cluster, ZCL_VENDOR_NAME_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_char_str(config->vendor_name, sizeof(config->vendor_name)));
    esp_matter_attribute_create(cluster, ZCL_VENDOR_ID_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_uint16(config->vendor_id));
    esp_matter_attribute_create(cluster, ZCL_PRODUCT_NAME_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_char_str(config->product_name, sizeof(config->product_name)));
    esp_matter_attribute_create(cluster, ZCL_PRODUCT_ID_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_uint16(config->product_id));
    esp_matter_attribute_create(cluster, ZCL_NODE_LABEL_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_char_str(config->node_label, sizeof(config->node_label)));
    esp_matter_attribute_create(cluster, ZCL_LOCATION_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_char_str(config->location, sizeof(config->location)));
    esp_matter_attribute_create(cluster, ZCL_HARDWARE_VERSION_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_uint16(config->hardware_version));
    esp_matter_attribute_create(cluster, ZCL_HARDWARE_VERSION_STRING_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_char_str(config->hardware_version_string,
                                sizeof(config->hardware_version_string)));
    esp_matter_attribute_create(cluster, ZCL_SOFTWARE_VERSION_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_uint32(config->software_version));
    esp_matter_attribute_create(cluster, ZCL_SOFTWARE_VERSION_STRING_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_char_str(config->software_version_string,
                                sizeof(config->software_version_string)));
    esp_matter_attribute_create(cluster, ZCL_SERIAL_NUMBER_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_char_str(config->serial_number, sizeof(config->serial_number)));

    return cluster;
}

esp_matter_cluster_t *esp_matter_cluster_create_ota_provider(esp_matter_endpoint_t *endpoint,
                                                             esp_matter_cluster_ota_provider_config_t *config,
                                                             uint8_t flags)
{
    esp_matter_cluster_t *cluster = esp_matter_cluster_create(endpoint, ZCL_OTA_PROVIDER_CLUSTER_ID, flags);

    esp_matter_attribute_create(cluster, ZCL_CLUSTER_REVISION_SERVER_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_uint16(config->cluster_revision));

    esp_matter_command_create(cluster, ZCL_QUERY_IMAGE_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_query_image);
    esp_matter_command_create(cluster, ZCL_QUERY_IMAGE_RESPONSE_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_query_image_response);
    esp_matter_command_create(cluster, ZCL_APPLY_UPDATE_REQUEST_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_apply_update_request);
    esp_matter_command_create(cluster, ZCL_APPLY_UPDATE_RESPONSE_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_apply_update_response);
    esp_matter_command_create(cluster, ZCL_NOTIFY_UPDATE_APPLIED_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_notify_update_applied);

    return cluster;
}

esp_matter_cluster_t *esp_matter_cluster_create_ota_requestor(esp_matter_endpoint_t *endpoint,
                                                              esp_matter_cluster_ota_requestor_config_t *config,
                                                              uint8_t flags)
{
    esp_matter_cluster_t *cluster = esp_matter_cluster_create(endpoint, ZCL_OTA_REQUESTOR_CLUSTER_ID, flags);

    esp_matter_attribute_create(cluster, ZCL_CLUSTER_REVISION_SERVER_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_uint16(config->cluster_revision));
    esp_matter_attribute_create(cluster, ZCL_DEFAULT_OTA_PROVIDER_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_octet_str(config->default_ota_providers,
                                sizeof(config->default_ota_providers), 0));
    esp_matter_attribute_create(cluster, ZCL_UPDATE_POSSIBLE_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_bool(config->update_possible));
    /* Not impplemented
    esp_matter_attribute_create(cluster, ZCL_UPDATE_STATE_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_enum8(config->update_state));
    esp_matter_attribute_create(cluster, ZCL_UPDATE_STATE_PROGRESS_PROVIDERS_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_uint8(config->update_state_progress));
    */

    esp_matter_command_create(cluster, ZCL_ANNOUNCE_OTA_PROVIDER_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_announce_ota_provider);

    return cluster;
}

esp_matter_cluster_t *esp_matter_cluster_create_general_commissioning(esp_matter_endpoint_t *endpoint,
                                                            esp_matter_cluster_general_commissioning_config_t *config,
                                                            uint8_t flags)
{
    esp_matter_cluster_t *cluster = esp_matter_cluster_create(endpoint, ZCL_GENERAL_COMMISSIONING_CLUSTER_ID, flags);

    esp_matter_attribute_create(cluster, ZCL_CLUSTER_REVISION_SERVER_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_uint16(config->cluster_revision));
    esp_matter_attribute_create(cluster, ZCL_BREADCRUMB_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_uint64(config->breadcrumb));
    esp_matter_attribute_create(cluster, ZCL_BASICCOMMISSIONINGINFO_LIST_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_array(config->basic_commissioning_info,
                                sizeof(config->basic_commissioning_info), 0));
    esp_matter_attribute_create(cluster, ZCL_REGULATORYCONFIG_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_enum8(config->regulatory_config));
    esp_matter_attribute_create(cluster, ZCL_LOCATIONCAPABILITY_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_enum8(config->location_capability));

    esp_matter_command_create(cluster, ZCL_ARM_FAIL_SAFE_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_arm_fail_safe);
    esp_matter_command_create(cluster, ZCL_ARM_FAIL_SAFE_RESPONSE_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_arm_fail_safe_response);
    esp_matter_command_create(cluster, ZCL_SET_REGULATORY_CONFIG_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_set_regulatory_config);
    esp_matter_command_create(cluster, ZCL_SET_REGULATORY_CONFIG_RESPONSE_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_set_regulatory_config_response);
    esp_matter_command_create(cluster, ZCL_COMMISSIONING_COMPLETE_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_commissioning_complete);
    esp_matter_command_create(cluster, ZCL_COMMISSIONING_COMPLETE_RESPONSE_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_commissioning_complete_response);

    return cluster;
}

esp_matter_cluster_t *esp_matter_cluster_create_network_commissioning(esp_matter_endpoint_t *endpoint,
                                                            esp_matter_cluster_network_commissioning_config_t *config,
                                                            uint8_t flags)
{
    esp_matter_cluster_t *cluster = esp_matter_cluster_create(endpoint, ZCL_NETWORK_COMMISSIONING_CLUSTER_ID, flags);

    esp_matter_attribute_create(cluster, ZCL_CLUSTER_REVISION_SERVER_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_uint16(config->cluster_revision));
    esp_matter_attribute_create(cluster, ZCL_FEATURE_MAP_SERVER_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_bitmap32(config->feature_map));
    esp_matter_attribute_create(cluster, ZCL_MAX_NETWORKS_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_uint8(config->max_networks));
    esp_matter_attribute_create(cluster, ZCL_NETWORKS_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_array(config->networks, sizeof(config->networks), 0));
    esp_matter_attribute_create(cluster, ZCL_SCAN_MAX_TIME_SECONDS_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_uint8(config->scan_max_time_seconds));
    esp_matter_attribute_create(cluster, ZCL_CONNECT_MAX_TIME_SECONDS_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_uint8(config->connect_max_time_seconds));
    esp_matter_attribute_create(cluster, ZCL_INTERFACE_ENABLED_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_bool(config->interface_enabled));
    esp_matter_attribute_create(cluster, ZCL_LAST_NETWORKING_STATUS_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_enum8(config->last_networking_status));
    esp_matter_attribute_create(cluster, ZCL_LAST_NETWORK_ID_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_octet_str(config->last_network_id, sizeof(config->last_network_id), 0));
    esp_matter_attribute_create(cluster, ZCL_LAST_CONNECT_ERROR_VALUE_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_uint32(config->last_connect_error_value));

    esp_matter_command_create(cluster, ZCL_SCAN_NETWORKS_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_scan_networks);
    esp_matter_command_create(cluster, ZCL_SCAN_NETWORKS_RESPONSE_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_scan_networks_response);
    esp_matter_command_create(cluster, ZCL_ADD_OR_UPDATE_WI_FI_NETWORK_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_add_or_update_wifi_network);
    esp_matter_command_create(cluster, ZCL_ADD_OR_UPDATE_THREAD_NETWORK_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_add_or_update_thread_network);
    esp_matter_command_create(cluster, ZCL_REMOVE_NETWORK_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_remove_network);
    esp_matter_command_create(cluster, ZCL_NETWORK_CONFIG_RESPONSE_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_network_config_response);
    esp_matter_command_create(cluster, ZCL_CONNECT_NETWORK_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_connect_network);
    esp_matter_command_create(cluster, ZCL_CONNECT_NETWORK_RESPONSE_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_connect_network_response);
    esp_matter_command_create(cluster, ZCL_REORDER_NETWORK_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_reorder_network);

    return cluster;
}

esp_matter_cluster_t *esp_matter_cluster_create_general_diagnostics(esp_matter_endpoint_t *endpoint,
                                                            esp_matter_cluster_general_diagnostics_config_t *config,
                                                            uint8_t flags)
{
    esp_matter_cluster_t *cluster = esp_matter_cluster_create(endpoint, ZCL_GENERAL_DIAGNOSTICS_CLUSTER_ID, flags);

    esp_matter_attribute_create(cluster, ZCL_CLUSTER_REVISION_SERVER_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_uint16(config->cluster_revision));
    esp_matter_attribute_create(cluster, ZCL_NETWORK_INTERFACES_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_array(config->network_interfaces, sizeof(config->network_interfaces), 0));
    esp_matter_attribute_create(cluster, ZCL_REBOOT_COUNT_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_uint16(config->reboot_count));

    return cluster;
}

esp_matter_cluster_t *esp_matter_cluster_create_administrator_commissioning(esp_matter_endpoint_t *endpoint,
                                                    esp_matter_cluster_administrator_commissioning_config_t *config,
                                                    uint8_t flags)
{
    esp_matter_cluster_t *cluster = esp_matter_cluster_create(endpoint, ZCL_ADMINISTRATOR_COMMISSIONING_CLUSTER_ID,
                                                              flags);

    esp_matter_attribute_create(cluster, ZCL_CLUSTER_REVISION_SERVER_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_uint16(config->cluster_revision));
    esp_matter_attribute_create(cluster, ZCL_WINDOW_STATUS_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_uint8(config->window_status));
    esp_matter_attribute_create(cluster, ZCL_ADMIN_FABRIC_INDEX_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_uint16(config->admin_fabric_index));
    esp_matter_attribute_create(cluster, ZCL_ADMIN_VENDOR_ID_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_uint16(config->admin_vendor_id));

    esp_matter_command_create(cluster, ZCL_OPEN_COMMISSIONING_WINDOW_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_open_commissioning_window);
    esp_matter_command_create(cluster, ZCL_OPEN_BASIC_COMMISSIONING_WINDOW_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_open_basic_commissioning_window);
    esp_matter_command_create(cluster, ZCL_REVOKE_COMMISSIONING_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_revoke_commissioning);

    return cluster;
}

esp_matter_cluster_t *esp_matter_cluster_create_operational_credentials(esp_matter_endpoint_t *endpoint,
                                                        esp_matter_cluster_operational_credentials_config_t *config,
                                                        uint8_t flags)
{
    esp_matter_cluster_t *cluster = esp_matter_cluster_create(endpoint, ZCL_OPERATIONAL_CREDENTIALS_CLUSTER_ID, flags);

    esp_matter_attribute_create(cluster, ZCL_CLUSTER_REVISION_SERVER_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_uint16(config->cluster_revision));
    /* Not implemented
    esp_matter_attribute_create(cluster, ZCL_NOCS_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_array(config->nocs, sizeof(config->nocs), 0));
    */
    esp_matter_attribute_create(cluster, ZCL_FABRICS_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_array(config->fabrics, sizeof(config->fabrics), 0));
    esp_matter_attribute_create(cluster, ZCL_SUPPORTED_FABRICS_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_uint8(config->supported_fabrics));
    esp_matter_attribute_create(cluster, ZCL_COMMISSIONED_FABRICS_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_uint8(config->commissioned_fabrics));
    esp_matter_attribute_create(cluster, ZCL_TRUSTED_ROOTS_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_array(config->trusted_root_certificates,
                                sizeof(config->trusted_root_certificates), 0));
    esp_matter_attribute_create(cluster, ZCL_CURRENT_FABRIC_INDEX_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_uint8(config->current_fabric_index));

    esp_matter_command_create(cluster, ZCL_ATTESTATION_REQUEST_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_attestation_request);
    esp_matter_command_create(cluster, ZCL_ATTESTATION_RESPONSE_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_attestation_response);
    esp_matter_command_create(cluster, ZCL_CERTIFICATE_CHAIN_REQUEST_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_certificate_chain_request);
    esp_matter_command_create(cluster, ZCL_CERTIFICATE_CHAIN_RESPONSE_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_certificate_chain_response);
    esp_matter_command_create(cluster, ZCL_OP_CSR_REQUEST_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_csr_request);
    esp_matter_command_create(cluster, ZCL_OP_CSR_RESPONSE_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_csr_response);
    esp_matter_command_create(cluster, ZCL_ADD_NOC_COMMAND_ID, COMMAND_MASK_NONE, esp_matter_command_callback_add_noc);
    esp_matter_command_create(cluster, ZCL_UPDATE_NOC_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_update_noc);
    esp_matter_command_create(cluster, ZCL_NOC_RESPONSE_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_noc_response);
    esp_matter_command_create(cluster, ZCL_UPDATE_FABRIC_LABEL_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_update_fabric_label);
    esp_matter_command_create(cluster, ZCL_REMOVE_FABRIC_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_remove_fabric);
    esp_matter_command_create(cluster, ZCL_ADD_TRUSTED_ROOT_CERTIFICATE_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_add_trusted_root_certificate);
    esp_matter_command_create(cluster, ZCL_REMOVE_TRUSTED_ROOT_CERTIFICATE_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_remove_trusted_root_certificate);

    return cluster;
}

esp_matter_cluster_t *esp_matter_cluster_create_group_key_management(esp_matter_endpoint_t *endpoint,
                                                            esp_matter_cluster_group_key_management_config_t *config,
                                                            uint8_t flags)
{
    esp_matter_cluster_t *cluster = esp_matter_cluster_create(endpoint, ZCL_GROUP_KEY_MANAGEMENT_CLUSTER_ID, flags);

    esp_matter_attribute_create(cluster, ZCL_CLUSTER_REVISION_SERVER_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_uint16(config->cluster_revision));
    esp_matter_attribute_create(cluster, ZCL_GROUPS_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_array(config->group_table, sizeof(config->group_table), 0));
    esp_matter_attribute_create(cluster, ZCL_GROUPKEYS_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_array(config->group_key_map, sizeof(config->group_key_map), 0));
    /* Not implemented
    esp_matter_attribute_create(cluster, ZCL_MAX_GROUPS_PER_FABRIC_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_uint16(config->max_groups_per_fabric));
    esp_matter_attribute_create(cluster, ZCL_MAX_GROUP_KEYS_PER_FABRIC_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_uint16(config->max_group_keys_per_fabric));

    esp_matter_command_create(cluster, ZCL_KEY_SET_WRITE_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_key_set_write);
    esp_matter_command_create(cluster, ZCL_KEY_SET_READ_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_key_set_read);
    esp_matter_command_create(cluster, ZCL_KEY_SET_READ_RESPONSE_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_key_set_read_response);
    esp_matter_command_create(cluster, ZCL_KEY_SET_REMOVE_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_key_set_remove);
    esp_matter_command_create(cluster, ZCL_KEY_SET_READ_ALL_INDICES_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_key_set_read_all_indices);
    esp_matter_command_create(cluster, ZCL_KEY_SET_READ_ALL_INDICES_RESPONSE_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_key_set_read_all_indices_response);
    */

    return cluster;
}

esp_matter_cluster_t *esp_matter_cluster_create_identify(esp_matter_endpoint_t *endpoint,
                                                         esp_matter_cluster_identify_config_t *config, uint8_t flags)
{
    esp_matter_cluster_t *cluster = esp_matter_cluster_create(endpoint, ZCL_IDENTIFY_CLUSTER_ID, flags);

    esp_matter_attribute_create(cluster, ZCL_CLUSTER_REVISION_SERVER_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_uint16(config->cluster_revision));
    esp_matter_attribute_create(cluster, ZCL_IDENTIFY_TIME_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_uint16(config->identify_time));
    esp_matter_attribute_create(cluster, ZCL_IDENTIFY_TYPE_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_uint8(config->identify_type));

    esp_matter_command_create(cluster, ZCL_IDENTIFY_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_identify);
    esp_matter_command_create(cluster, ZCL_IDENTIFY_QUERY_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_identify_query);
    esp_matter_command_create(cluster, ZCL_IDENTIFY_QUERY_RESPONSE_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_identify_query_response);

    return cluster;
}

esp_matter_cluster_t *esp_matter_cluster_create_groups(esp_matter_endpoint_t *endpoint,
                                                       esp_matter_cluster_groups_config_t *config, uint8_t flags)
{
    esp_matter_cluster_t *cluster = esp_matter_cluster_create(endpoint, ZCL_GROUPS_CLUSTER_ID, flags);

    esp_matter_attribute_create(cluster, ZCL_CLUSTER_REVISION_SERVER_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_uint16(config->cluster_revision));
    esp_matter_attribute_create(cluster, ZCL_GROUP_NAME_SUPPORT_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_bitmap8(config->name_support));

    esp_matter_command_create(cluster, ZCL_ADD_GROUP_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_add_group);
    esp_matter_command_create(cluster, ZCL_VIEW_GROUP_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_view_group);
    esp_matter_command_create(cluster, ZCL_GET_GROUP_MEMBERSHIP_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_get_group_membership);
    esp_matter_command_create(cluster, ZCL_REMOVE_GROUP_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_remove_group);
    esp_matter_command_create(cluster, ZCL_REMOVE_ALL_GROUPS_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_remove_all_groups);
    esp_matter_command_create(cluster, ZCL_ADD_GROUP_IF_IDENTIFYING_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_add_group_if_identifying);
    esp_matter_command_create(cluster, ZCL_ADD_GROUP_RESPONSE_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_add_group_response);
    esp_matter_command_create(cluster, ZCL_VIEW_GROUP_RESPONSE_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_view_group_response);
    esp_matter_command_create(cluster, ZCL_GET_GROUP_MEMBERSHIP_RESPONSE_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_get_group_membership_response);
    esp_matter_command_create(cluster, ZCL_REMOVE_GROUP_RESPONSE_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_remove_group_response);

    return cluster;
}

esp_matter_cluster_t *esp_matter_cluster_create_scenes(esp_matter_endpoint_t *endpoint,
                                                       esp_matter_cluster_scenes_config_t *config, uint8_t flags)
{
    esp_matter_cluster_t *cluster = esp_matter_cluster_create(endpoint, ZCL_SCENES_CLUSTER_ID, flags);

    esp_matter_attribute_create(cluster, ZCL_CLUSTER_REVISION_SERVER_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_uint16(config->cluster_revision));
    esp_matter_attribute_create(cluster, ZCL_SCENE_COUNT_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_uint8(config->scene_count));
    esp_matter_attribute_create(cluster, ZCL_CURRENT_SCENE_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_uint8(config->current_scene));
    esp_matter_attribute_create(cluster, ZCL_CURRENT_GROUP_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_uint16(config->current_group));
    esp_matter_attribute_create(cluster, ZCL_SCENE_VALID_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_bool(config->scene_valid));
    esp_matter_attribute_create(cluster, ZCL_SCENE_NAME_SUPPORT_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_bitmap8(config->name_support));

    esp_matter_command_create(cluster, ZCL_ADD_SCENE_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_add_scene);
    esp_matter_command_create(cluster, ZCL_VIEW_SCENE_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_view_scene);
    esp_matter_command_create(cluster, ZCL_REMOVE_SCENE_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_remove_scene);
    esp_matter_command_create(cluster, ZCL_REMOVE_ALL_SCENES_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_remove_all_scenes);
    esp_matter_command_create(cluster, ZCL_STORE_SCENE_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_store_scene);
    esp_matter_command_create(cluster, ZCL_RECALL_SCENE_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_recall_scene);
    esp_matter_command_create(cluster, ZCL_GET_SCENE_MEMBERSHIP_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_get_scene_membership);
    esp_matter_command_create(cluster, ZCL_ADD_SCENE_RESPONSE_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_add_scene_response);
    esp_matter_command_create(cluster, ZCL_VIEW_SCENE_RESPONSE_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_view_scene_response);
    esp_matter_command_create(cluster, ZCL_REMOVE_SCENE_RESPONSE_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_remove_scene_response);
    esp_matter_command_create(cluster, ZCL_REMOVE_ALL_SCENES_RESPONSE_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_remove_all_scenes_response);
    esp_matter_command_create(cluster, ZCL_STORE_SCENE_RESPONSE_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_store_scene_response);
    esp_matter_command_create(cluster, ZCL_GET_SCENE_MEMBERSHIP_RESPONSE_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_get_scene_membership_response);

    return cluster;
}

esp_matter_cluster_t *esp_matter_cluster_create_on_off(esp_matter_endpoint_t *endpoint,
                                                       esp_matter_cluster_on_off_config_t *config, uint8_t flags)
{
    esp_matter_cluster_t *cluster = esp_matter_cluster_create(endpoint, ZCL_ON_OFF_CLUSTER_ID, flags);

    esp_matter_attribute_create(cluster, ZCL_CLUSTER_REVISION_SERVER_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_uint16(config->cluster_revision));
    esp_matter_attribute_create(cluster, ZCL_ON_OFF_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE, esp_matter_bool(config->on_off));

    esp_matter_command_create(cluster, ZCL_OFF_COMMAND_ID, COMMAND_MASK_NONE, esp_matter_command_callback_off);
    esp_matter_command_create(cluster, ZCL_ON_COMMAND_ID, COMMAND_MASK_NONE, esp_matter_command_callback_on);
    esp_matter_command_create(cluster, ZCL_TOGGLE_COMMAND_ID, COMMAND_MASK_NONE, esp_matter_command_callback_toggle);

    return cluster;
}

esp_matter_cluster_t *esp_matter_cluster_create_level_control(esp_matter_endpoint_t *endpoint,
                                                              esp_matter_cluster_level_control_config_t *config,
                                                              uint8_t flags)
{
    esp_matter_cluster_t *cluster = esp_matter_cluster_create(endpoint, ZCL_LEVEL_CONTROL_CLUSTER_ID, flags);

    esp_matter_attribute_create(cluster, ZCL_CLUSTER_REVISION_SERVER_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_uint16(config->cluster_revision));
    esp_matter_attribute_create(cluster, ZCL_CURRENT_LEVEL_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_uint8(config->current_level));
    esp_matter_attribute_create(cluster, ZCL_ON_LEVEL_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_uint8(config->on_level));
    esp_matter_attribute_create(cluster, ZCL_OPTIONS_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_bitmap8(config->options));

    esp_matter_command_create(cluster, ZCL_MOVE_TO_LEVEL_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_move_to_level);
    esp_matter_command_create(cluster, ZCL_MOVE_COMMAND_ID, COMMAND_MASK_NONE, esp_matter_command_callback_move);
    esp_matter_command_create(cluster, ZCL_STEP_COMMAND_ID, COMMAND_MASK_NONE, esp_matter_command_callback_step);
    esp_matter_command_create(cluster, ZCL_STOP_COMMAND_ID, COMMAND_MASK_NONE, esp_matter_command_callback_stop);
    esp_matter_command_create(cluster, ZCL_MOVE_TO_LEVEL_WITH_ON_OFF_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_move_to_level_with_on_off);
    esp_matter_command_create(cluster, ZCL_MOVE_WITH_ON_OFF_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_move_with_on_off);
    esp_matter_command_create(cluster, ZCL_STEP_WITH_ON_OFF_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_step_with_on_off);
    esp_matter_command_create(cluster, ZCL_STOP_WITH_ON_OFF_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_stop_with_on_off);

    return cluster;
}

esp_matter_cluster_t *esp_matter_cluster_create_color_control(esp_matter_endpoint_t *endpoint,
                                                              esp_matter_cluster_color_control_config_t *config,
                                                              uint8_t flags)
{
    esp_matter_cluster_t *cluster = esp_matter_cluster_create(endpoint, ZCL_COLOR_CONTROL_CLUSTER_ID, flags);

    esp_matter_attribute_create(cluster, ZCL_CLUSTER_REVISION_SERVER_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_uint16(config->cluster_revision));
    esp_matter_attribute_create(cluster, ZCL_FEATURE_MAP_SERVER_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_bitmap32(config->feature_map));
    esp_matter_attribute_create(cluster, ZCL_COLOR_CONTROL_CURRENT_HUE_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_uint8(config->current_hue));
    esp_matter_attribute_create(cluster, ZCL_COLOR_CONTROL_CURRENT_SATURATION_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_uint8(config->current_saturation));
    esp_matter_attribute_create(cluster, ZCL_COLOR_CONTROL_COLOR_MODE_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_enum8(config->color_mode));
    esp_matter_attribute_create(cluster, ZCL_OPTIONS_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_bitmap8(config->options));
    esp_matter_attribute_create(cluster, ZCL_COLOR_CONTROL_ENHANCED_COLOR_MODE_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_enum8(config->enhanced_color_mode));
    esp_matter_attribute_create(cluster, ZCL_COLOR_CONTROL_COLOR_CAPABILITIES_ATTRIBUTE_ID, ATTRIBUTE_MASK_NONE,
                                esp_matter_bitmap16(config->color_capabilities));

    esp_matter_command_create(cluster, ZCL_MOVE_TO_HUE_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_move_to_hue);
    esp_matter_command_create(cluster, ZCL_MOVE_HUE_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_move_hue);
    esp_matter_command_create(cluster, ZCL_STEP_HUE_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_step_hue);
    esp_matter_command_create(cluster, ZCL_MOVE_TO_SATURATION_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_move_to_saturation);
    esp_matter_command_create(cluster, ZCL_MOVE_SATURATION_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_move_saturation);
    esp_matter_command_create(cluster, ZCL_STEP_SATURATION_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_step_saturation);
    esp_matter_command_create(cluster, ZCL_MOVE_TO_HUE_AND_SATURATION_COMMAND_ID, COMMAND_MASK_NONE,
                              esp_matter_command_callback_move_to_hue_and_saturation);

    return cluster;
}
