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

#include <app/att-storage.h>
#include <app-common/zap-generated/callback.h>
#include <app/PluginApplicationCallbacks.h>

static const char *TAG = "esp_matter_cluster";

using namespace chip::app::Clusters;

namespace esp_matter {
namespace cluster {

void plugin_init_callback_common()
{
    ESP_LOGI(TAG, "Cluster plugin init common callback");
    node_t *node = node::get();
    if (!node) {
        /* Skip plugin_init_callback_common when ESP Matter data model is not used */
        return;
    }
    endpoint_t *endpoint = endpoint::get_first(node);
    while (endpoint) {
        cluster_t *cluster = get_first(endpoint);
        while (cluster) {
            /* Plugin server init callback */
            plugin_server_init_callback_t plugin_server_init_callback = get_plugin_server_init_callback(cluster);
            if (plugin_server_init_callback) {
                plugin_server_init_callback();
            }
            /* Plugin client init callback */
            plugin_client_init_callback_t plugin_client_init_callback = get_plugin_client_init_callback(cluster);
            if (plugin_client_init_callback) {
                plugin_client_init_callback();
            }
            cluster = get_next(cluster);
        }
        endpoint = endpoint::get_next(endpoint);
    }
}

cluster_t *create_default_binding_cluster(endpoint_t *endpoint)
{
    /* Don't create binding cluster if it already exists on the endpoint */
    cluster_t *cluster = get(endpoint, Binding::Id);
    if (cluster) {
        return cluster;
    }
    ESP_LOGI(TAG, "Creating default binding cluster");
    binding::config_t config;
    return binding::create(endpoint, &config, CLUSTER_FLAG_SERVER);
}

#if (FIXED_ENDPOINT_COUNT == 0)
namespace descriptor {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, Descriptor::Id, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        set_plugin_server_init_callback(cluster, MatterDescriptorPluginServerInitCallback);
        add_function_list(cluster, function_list, function_flags);
    }
    if (flags & CLUSTER_FLAG_CLIENT) {
        set_plugin_client_init_callback(cluster, MatterDescriptorPluginClientInitCallback);
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        /* Attributes managed internally */
        global::attribute::create_cluster_revision(cluster, 0);
        attribute::create_device_list(cluster, NULL, 0, 0);
        attribute::create_server_list(cluster, NULL, 0, 0);
        attribute::create_client_list(cluster, NULL, 0, 0);
        attribute::create_parts_list(cluster, NULL, 0, 0);

        /* Attributes updated later */
        global::attribute::create_feature_map(cluster, 0);
    }

    return cluster;
}
} /* descriptor */

namespace actions {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, Descriptor::Id, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        set_plugin_server_init_callback(cluster, NULL);
        add_function_list(cluster, function_list, function_flags);
    }
    if (flags & CLUSTER_FLAG_CLIENT) {
        set_plugin_client_init_callback(cluster, NULL);
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        /* Attributes managed internally */
        global::attribute::create_cluster_revision(cluster, 1);
        global::attribute::create_feature_map(cluster, 0);
        attribute::create_action_list(cluster, NULL, 0, 0);
        attribute::create_endpoint_lists(cluster, NULL, 0, 0);
    }

    return cluster;
}
} /* actions */

namespace access_control {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, AccessControl::Id, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }
    if (flags & CLUSTER_FLAG_SERVER) {
        set_plugin_server_init_callback(cluster, MatterAccessControlPluginServerInitCallback);
        add_function_list(cluster, function_list, function_flags);
    }
    if (flags & CLUSTER_FLAG_CLIENT) {
        set_plugin_client_init_callback(cluster, MatterAccessControlPluginClientInitCallback);
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        /* Attributes managed internally */
        global::attribute::create_cluster_revision(cluster, 0);
        attribute::create_acl(cluster, NULL, 0, 0);
        attribute::create_subjects_per_access_control_entry(cluster, 0);
        attribute::create_access_control_entries_per_fabric(cluster, 0);
        attribute::create_targets_per_access_control_entry(cluster, 0);

        /* Attributes updated later */
        global::attribute::create_feature_map(cluster, 0);
    }

    return cluster;
}
} /* access_control */

namespace basic_information {
const function_generic_t function_list[] = {
    (function_generic_t)emberAfBasicInformationClusterServerInitCallback,
};
const int function_flags = CLUSTER_FLAG_INIT_FUNCTION;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, BasicInformation::Id, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        set_plugin_server_init_callback(cluster, MatterBasicInformationPluginServerInitCallback);
        add_function_list(cluster, function_list, function_flags);
    }
    if (flags & CLUSTER_FLAG_CLIENT) {
        set_plugin_client_init_callback(cluster, MatterBasicInformationPluginClientInitCallback);
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);
        attribute::create_data_model_revision(cluster, 0);
        attribute::create_location(cluster, NULL, 0);
        attribute::create_vendor_name(cluster, NULL, 0);
        attribute::create_vendor_id(cluster, 0);
        attribute::create_product_name(cluster, NULL, 0);
        attribute::create_product_id(cluster, 0);
        attribute::create_hardware_version(cluster, 0);
        attribute::create_hardware_version_string(cluster, NULL, 0);
        attribute::create_software_version(cluster, 0);
        attribute::create_software_version_string(cluster, NULL, 0);
        attribute::create_capability_minima(cluster, NULL, 0, 0);

        /* Attributes not managed internally */
        if (config) {
            global::attribute::create_cluster_revision(cluster, config->cluster_revision);
            attribute::create_node_label(cluster, config->node_label, sizeof(config->node_label));
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    return cluster;
}
} /* basic_information */

namespace binding {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, Binding::Id, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        set_plugin_server_init_callback(cluster, MatterBindingPluginServerInitCallback);
        add_function_list(cluster, function_list, function_flags);
    }
    if (flags & CLUSTER_FLAG_CLIENT) {
        set_plugin_client_init_callback(cluster, MatterBindingPluginClientInitCallback);
    }

    /* Extra initialization */
    client::binding_init();

    if (flags & CLUSTER_FLAG_SERVER) {
        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);
        attribute::create_binding(cluster, NULL, 0, 0);

        /* Attributes not managed internally */
        if (config) {
            global::attribute::create_cluster_revision(cluster, config->cluster_revision);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    return cluster;
}
} /* binding */

namespace ota_provider {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, OtaSoftwareUpdateProvider::Id, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        set_plugin_server_init_callback(cluster, MatterOtaSoftwareUpdateProviderPluginServerInitCallback);
        add_function_list(cluster, function_list, function_flags);
    }
    if (flags & CLUSTER_FLAG_CLIENT) {
        set_plugin_client_init_callback(cluster, MatterOtaSoftwareUpdateProviderPluginClientInitCallback);
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        if (config) {
            global::attribute::create_cluster_revision(cluster, config->cluster_revision);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    /* Commands */
    command::create_query_image(cluster);
    command::create_query_image_response(cluster);
    command::create_apply_update_request(cluster);
    command::create_apply_update_response(cluster);
    command::create_notify_update_applied(cluster);

    return cluster;
}
} /* ota_provider */

namespace ota_requestor {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, OtaSoftwareUpdateRequestor::Id, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        set_plugin_server_init_callback(cluster, MatterOtaSoftwareUpdateRequestorPluginServerInitCallback);
        add_function_list(cluster, function_list, function_flags);
    }
    if (flags & CLUSTER_FLAG_CLIENT) {
        set_plugin_client_init_callback(cluster, MatterOtaSoftwareUpdateRequestorPluginClientInitCallback);
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);
        attribute::create_default_ota_providers(cluster, NULL, 0, 0);

        /* Attributes not managed internally */
        if (config) {
            global::attribute::create_cluster_revision(cluster, config->cluster_revision);
            attribute::create_update_possible(cluster, config->update_possible);
            attribute::create_update_state(cluster, config->update_state);
            attribute::create_update_state_progress(cluster, config->update_state_progress);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    /* Commands */
    command::create_announce_ota_provider(cluster);

    return cluster;
}
} /* ota_requestor */

namespace general_commissioning {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, GeneralCommissioning::Id, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        set_plugin_server_init_callback(cluster, MatterGeneralCommissioningPluginServerInitCallback);
        add_function_list(cluster, function_list, function_flags);
    }
    if (flags & CLUSTER_FLAG_CLIENT) {
        set_plugin_client_init_callback(cluster, MatterGeneralCommissioningPluginClientInitCallback);
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);
        attribute::create_regulatory_config(cluster, 0);
        attribute::create_location_capability(cluster, 0);
        attribute::create_basic_commissioning_info(cluster, NULL, 0, 0);
        attribute::create_supports_concurrent_connection(cluster, 0);

        /* Attributes not managed internally */
        if (config) {
            global::attribute::create_cluster_revision(cluster, config->cluster_revision);
            attribute::create_breadcrumb(cluster, config->breadcrumb);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    /* Commands */
    command::create_arm_fail_safe(cluster);
    command::create_arm_fail_safe_response(cluster);
    command::create_set_regulatory_config(cluster);
    command::create_set_regulatory_config_response(cluster);
    command::create_commissioning_complete(cluster);
    command::create_commissioning_complete_response(cluster);

    return cluster;
}
} /* general_commissioning */

namespace network_commissioning {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, NetworkCommissioning::Id, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        set_plugin_server_init_callback(cluster, MatterNetworkCommissioningPluginServerInitCallback);
        add_function_list(cluster, function_list, function_flags);
    }
    if (flags & CLUSTER_FLAG_CLIENT) {
        set_plugin_client_init_callback(cluster, MatterNetworkCommissioningPluginClientInitCallback);
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        /* Attributes managed internally */
        attribute::create_max_networks(cluster, 0);
        attribute::create_networks(cluster, NULL, 0, 0);
        attribute::create_scan_max_time_seconds(cluster, 0);
        attribute::create_connect_max_time_seconds(cluster, 0);
        attribute::create_interface_enabled(cluster, 0);
        attribute::create_last_networking_status(cluster, nullable<uint8_t>());
        attribute::create_last_network_id(cluster, NULL, 0);
        attribute::create_last_connect_error_value(cluster, nullable<int32_t>());
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        if (config) {
            global::attribute::create_cluster_revision(cluster, config->cluster_revision);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    /* Commands */
    command::create_scan_networks(cluster);
    command::create_scan_networks_response(cluster);
    command::create_add_or_update_wifi_network(cluster);
    command::create_add_or_update_thread_network(cluster);
    command::create_remove_network(cluster);
    command::create_network_config_response(cluster);
    command::create_connect_network(cluster);
    command::create_connect_network_response(cluster);
    command::create_reorder_network(cluster);

    return cluster;
}
} /* network_commissioning */

namespace general_diagnostics {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, GeneralDiagnostics::Id, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        set_plugin_server_init_callback(cluster, MatterGeneralDiagnosticsPluginServerInitCallback);
        add_function_list(cluster, function_list, function_flags);
    }
    if (flags & CLUSTER_FLAG_CLIENT) {
        set_plugin_client_init_callback(cluster, MatterGeneralDiagnosticsPluginClientInitCallback);
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        /* Attributes managed internally */
        attribute::create_network_interfaces(cluster, NULL, 0, 0);
        attribute::create_reboot_count(cluster, 0);
        attribute::create_test_event_triggers_enabled(cluster, 0);
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        if (config) {
            global::attribute::create_cluster_revision(cluster, config->cluster_revision);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    command::create_test_event_trigger(cluster);

    return cluster;
}
} /* general_diagnostics */

namespace administrator_commissioning {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, AdministratorCommissioning::Id, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        set_plugin_server_init_callback(cluster, MatterAdministratorCommissioningPluginServerInitCallback);
        add_function_list(cluster, function_list, function_flags);
    }
    if (flags & CLUSTER_FLAG_CLIENT) {
        set_plugin_client_init_callback(cluster, MatterAdministratorCommissioningPluginClientInitCallback);
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);
        attribute::create_window_status(cluster, 0);
        attribute::create_admin_fabric_index(cluster, 0);
        attribute::create_admin_vendor_id(cluster, 0);

        /* Attributes not managed internally */
        if (config) {
            global::attribute::create_cluster_revision(cluster, config->cluster_revision);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    /* Commands */
    command::create_open_commissioning_window(cluster);
    command::create_open_basic_commissioning_window(cluster);
    command::create_revoke_commissioning(cluster);

    return cluster;
}
} /* administrator_commissioning */

namespace operational_credentials {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, OperationalCredentials::Id, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        set_plugin_server_init_callback(cluster, MatterOperationalCredentialsPluginServerInitCallback);
        add_function_list(cluster, function_list, function_flags);
    }
    if (flags & CLUSTER_FLAG_CLIENT) {
        set_plugin_client_init_callback(cluster, MatterOperationalCredentialsPluginClientInitCallback);
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);
        attribute::create_nocs(cluster, NULL, 0, 0);
        attribute::create_supported_fabrics(cluster, 0);
        attribute::create_commissioned_fabrics(cluster, 0);
        attribute::create_fabrics(cluster, NULL, 0, 0);
        attribute::create_trusted_root_certificates(cluster, NULL, 0, 0);
        attribute::create_current_fabric_index(cluster, 0);

        /* Attributes not managed internally */
        if (config) {
            global::attribute::create_cluster_revision(cluster, config->cluster_revision);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    /* Commands */
    command::create_attestation_request(cluster);
    command::create_attestation_response(cluster);
    command::create_certificate_chain_request(cluster);
    command::create_certificate_chain_response(cluster);
    command::create_csr_request(cluster);
    command::create_csr_response(cluster);
    command::create_add_noc(cluster);
    command::create_update_noc(cluster);
    command::create_noc_response(cluster);
    command::create_update_fabric_label(cluster);
    command::create_remove_fabric(cluster);
    command::create_add_trusted_root_certificate(cluster);

    return cluster;
}
} /* operational_credentials */

namespace group_key_management {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, GroupKeyManagement::Id, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        set_plugin_server_init_callback(cluster, MatterGroupKeyManagementPluginServerInitCallback);
        add_function_list(cluster, function_list, function_flags);
    }
    if (flags & CLUSTER_FLAG_CLIENT) {
        set_plugin_client_init_callback(cluster, MatterGroupKeyManagementPluginClientInitCallback);
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);
        global::attribute::create_cluster_revision(cluster, 0);
        attribute::create_group_key_map(cluster, NULL, 0, 0);
        attribute::create_group_table(cluster, NULL, 0, 0);
        attribute::create_max_groups_per_fabric(cluster, 0);
        attribute::create_max_group_keys_per_fabric(cluster, 0);
    }

    /* Commands */
    command::create_key_set_write(cluster);
    command::create_key_set_read(cluster);
    command::create_key_set_read_response(cluster);
    command::create_key_set_remove(cluster);
    command::create_key_set_read_all_indices(cluster);
    command::create_key_set_read_all_indices_response(cluster);

    return cluster;
}
} /* group_key_management */

namespace diagnostics_network_wifi {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, WiFiNetworkDiagnostics::Id, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        set_plugin_server_init_callback(cluster, MatterWiFiNetworkDiagnosticsPluginServerInitCallback);
        add_function_list(cluster, function_list, function_flags);
    }
    if (flags & CLUSTER_FLAG_CLIENT) {
        set_plugin_client_init_callback(cluster, MatterWiFiNetworkDiagnosticsPluginClientInitCallback);
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);
        attribute::create_bssid(cluster, NULL, 0);
        attribute::create_security_type(cluster, nullable<uint8_t>());
        attribute::create_wifi_version(cluster, nullable<uint8_t>());
        attribute::create_channel_number(cluster, nullable<uint16_t>());
        attribute::create_rssi(cluster, nullable<int8_t>());

        /* Attributes not managed internally */
        if (config) {
            global::attribute::create_cluster_revision(cluster, config->cluster_revision);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    return cluster;
}
} /* diagnostics_network_wifi */

namespace diagnostics_network_thread {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, ThreadNetworkDiagnostics::Id, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        set_plugin_server_init_callback(cluster, MatterThreadNetworkDiagnosticsPluginServerInitCallback);
        add_function_list(cluster, function_list, function_flags);
    }
    if (flags & CLUSTER_FLAG_CLIENT) {
        set_plugin_client_init_callback(cluster, MatterThreadNetworkDiagnosticsPluginClientInitCallback);
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);
        attribute::create_channel(cluster, nullable<uint16_t>(0));
        attribute::create_routing_role(cluster, nullable<uint8_t>(0));
        attribute::create_network_name(cluster, NULL, 0);
        attribute::create_pan_id(cluster, nullable<uint16_t>(0));
        attribute::create_extended_pan_id(cluster, nullable<uint64_t>(0));
        attribute::create_mesh_local_prefix(cluster, NULL, 0);
        attribute::create_neighbor_table(cluster, NULL, 0, 0);
        attribute::create_route_table(cluster, NULL, 0, 0);
        attribute::create_partition_id(cluster, nullable<uint32_t>(0));
        attribute::create_weighting(cluster, nullable<uint8_t>(0));
        attribute::create_data_version(cluster, nullable<uint8_t>(0));
        attribute::create_stable_data_version(cluster, nullable<uint8_t>(0));
        attribute::create_leader_router_id(cluster, nullable<uint8_t>(0));
        attribute::create_security_policy(cluster, NULL, 0, 0);
        attribute::create_channel_page0_mask(cluster, NULL, 0);
        attribute::create_operational_dataset_components(cluster, NULL, 0, 0);
        attribute::create_active_network_faults(cluster, NULL, 0, 0);

        /* Attributes not managed internally */
        if (config) {
            global::attribute::create_cluster_revision(cluster, config->cluster_revision);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    /* commands */
    command::create_reset_counts(cluster);

    return cluster;
}
} /* diagnostics_network_thread */

namespace time_synchronization {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, TimeSynchronization::Id, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        set_plugin_server_init_callback(cluster, MatterTimeSynchronizationPluginServerInitCallback);
        add_function_list(cluster, function_list, function_flags);
    }
    if (flags & CLUSTER_FLAG_CLIENT) {
        set_plugin_client_init_callback(cluster, MatterTimeSynchronizationPluginClientInitCallback);
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        if (config) {
            global::attribute::create_cluster_revision(cluster, config->cluster_revision);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    return cluster;
}
} /* time_synchronization */

namespace bridged_device_basic_information {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, BridgedDeviceBasicInformation::Id, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        // There is not PluginServer(Client)InitCallback for this cluster
        add_function_list(cluster, function_list, function_flags);
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        if (config) {
            global::attribute::create_cluster_revision(cluster, config->cluster_revision);
            attribute::create_reachable(cluster, config->reachable);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    return cluster;
}
} /* bridged_device_basic_information */

namespace user_label {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, UserLabel::Id, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        set_plugin_server_init_callback(cluster, MatterUserLabelPluginServerInitCallback);
        add_function_list(cluster, function_list, function_flags);
    }
    if (flags & CLUSTER_FLAG_CLIENT) {
        set_plugin_client_init_callback(cluster, MatterUserLabelPluginClientInitCallback);
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);
        attribute::create_label_list(cluster, NULL, 0, 0);

        /* Attributes not managed internally */
        if (config) {
            global::attribute::create_cluster_revision(cluster, config->cluster_revision);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    return cluster;
}
} /* user_label */

namespace fixed_label {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, FixedLabel::Id, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        set_plugin_server_init_callback(cluster, MatterFixedLabelPluginServerInitCallback);
        add_function_list(cluster, function_list, function_flags);
    }
    if (flags & CLUSTER_FLAG_CLIENT) {
        set_plugin_client_init_callback(cluster, MatterFixedLabelPluginClientInitCallback);
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);
        attribute::create_label_list(cluster, NULL, 0, 0);

        /* Attributes not managed internally */
        if (config) {
            global::attribute::create_cluster_revision(cluster, config->cluster_revision);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    return cluster;
}
} /* fixed_label */

namespace identify {
const function_generic_t function_list[] = {
    (function_generic_t)emberAfIdentifyClusterServerInitCallback,
    (function_generic_t)MatterIdentifyClusterServerAttributeChangedCallback,
};
const int function_flags = CLUSTER_FLAG_INIT_FUNCTION | CLUSTER_FLAG_ATTRIBUTE_CHANGED_FUNCTION;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, Identify::Id, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        set_plugin_server_init_callback(cluster, MatterIdentifyPluginServerInitCallback);
        add_function_list(cluster, function_list, function_flags);
    }
    if (flags & CLUSTER_FLAG_CLIENT) {
        set_plugin_client_init_callback(cluster, MatterIdentifyPluginClientInitCallback);
    }

    /* Extra initialization */
    uint16_t endpoint_id = endpoint::get_id(endpoint);
    identification::init(endpoint_id, config->identify_type);

    if (flags & CLUSTER_FLAG_SERVER) {
        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        if (config) {
            global::attribute::create_cluster_revision(cluster, config->cluster_revision);
            attribute::create_identify_time(cluster, config->identify_time, 0x0, 0xFE);
            attribute::create_identify_type(cluster, config->identify_type);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    /* Commands */
    command::create_identify(cluster);

    return cluster;
}
} /* identify */

namespace groups {
const function_generic_t function_list[] = {
    (function_generic_t)emberAfGroupsClusterServerInitCallback,
};
const int function_flags = CLUSTER_FLAG_INIT_FUNCTION;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, Groups::Id, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        set_plugin_server_init_callback(cluster, MatterGroupsPluginServerInitCallback);
        add_function_list(cluster, function_list, function_flags);
    }
    if (flags & CLUSTER_FLAG_CLIENT) {
        set_plugin_client_init_callback(cluster, MatterGroupsPluginClientInitCallback);
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 1);

        /* Attributes not managed internally */
        if (config) {
            global::attribute::create_cluster_revision(cluster, config->cluster_revision);
            attribute::create_group_name_support(cluster, config->group_name_support);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    /* Commands */
    command::create_add_group(cluster);
    command::create_view_group(cluster);
    command::create_get_group_membership(cluster);
    command::create_remove_group(cluster);
    command::create_remove_all_groups(cluster);
    command::create_add_group_if_identifying(cluster);
    command::create_add_group_response(cluster);
    command::create_view_group_response(cluster);
    command::create_get_group_membership_response(cluster);
    command::create_remove_group_response(cluster);

    return cluster;
}
} /* groups */

namespace scenes {
const function_generic_t function_list[] = {
    (function_generic_t)emberAfScenesClusterServerInitCallback,
};
const int function_flags = CLUSTER_FLAG_INIT_FUNCTION;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, Scenes::Id, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        set_plugin_server_init_callback(cluster, MatterScenesPluginServerInitCallback);
        add_function_list(cluster, function_list, function_flags);
    }
    if (flags & CLUSTER_FLAG_CLIENT) {
        set_plugin_client_init_callback(cluster, MatterScenesPluginClientInitCallback);
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        if (config) {
            global::attribute::create_cluster_revision(cluster, config->cluster_revision);
            attribute::create_scene_count(cluster, config->scene_count);
            attribute::create_current_scene(cluster, config->current_scene);
            attribute::create_current_group(cluster, config->current_group);
            attribute::create_scene_valid(cluster, config->scene_valid);
            attribute::create_scene_name_support(cluster, config->scene_name_support);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    /* Commands */
    command::create_add_scene(cluster);
    command::create_view_scene(cluster);
    command::create_remove_scene(cluster);
    command::create_remove_all_scenes(cluster);
    command::create_store_scene(cluster);
    command::create_recall_scene(cluster);
    command::create_get_scene_membership(cluster);
    command::create_add_scene_response(cluster);
    command::create_view_scene_response(cluster);
    command::create_remove_scene_response(cluster);
    command::create_remove_all_scenes_response(cluster);
    command::create_store_scene_response(cluster);
    command::create_get_scene_membership_response(cluster);

    return cluster;
}
} /* scenes */

namespace on_off {
const function_generic_t function_list[] = {
    (function_generic_t)emberAfOnOffClusterServerInitCallback,
    (function_generic_t)MatterOnOffClusterServerShutdownCallback,
};
const int function_flags = CLUSTER_FLAG_INIT_FUNCTION | CLUSTER_FLAG_SHUTDOWN_FUNCTION;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags, uint32_t features)
{
    cluster_t *cluster = cluster::create(endpoint, OnOff::Id, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        set_plugin_server_init_callback(cluster, MatterOnOffPluginServerInitCallback);
        add_function_list(cluster, function_list, function_flags);
    }
    if (flags & CLUSTER_FLAG_CLIENT) {
        set_plugin_client_init_callback(cluster, MatterOnOffPluginClientInitCallback);
        create_default_binding_cluster(endpoint);
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        if (config) {
            global::attribute::create_cluster_revision(cluster, config->cluster_revision);
            attribute::create_on_off(cluster, config->on_off);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    /* Commands */
    command::create_off(cluster);
    command::create_on(cluster);
    command::create_toggle(cluster);

    /* Features */
    if (features & feature::lighting::get_id()) {
        feature::lighting::add(cluster, &(config->lighting));
    }

    return cluster;
}
} /* on_off */

namespace level_control {
const function_generic_t function_list[] = {
    (function_generic_t)emberAfLevelControlClusterServerInitCallback,
    (function_generic_t)MatterLevelControlClusterServerShutdownCallback,
};
const int function_flags = CLUSTER_FLAG_INIT_FUNCTION | CLUSTER_FLAG_SHUTDOWN_FUNCTION;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags, uint32_t features)
{
    cluster_t *cluster = cluster::create(endpoint, LevelControl::Id, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        set_plugin_server_init_callback(cluster, MatterLevelControlPluginServerInitCallback);
        add_function_list(cluster, function_list, function_flags);
    }
    if (flags & CLUSTER_FLAG_CLIENT) {
        set_plugin_client_init_callback(cluster, MatterLevelControlPluginClientInitCallback);
        create_default_binding_cluster(endpoint);
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        if (config) {
            global::attribute::create_cluster_revision(cluster, config->cluster_revision);
            attribute::create_current_level(cluster, config->current_level);
            attribute::create_on_level(cluster, config->on_level);
            attribute::create_options(cluster, config->options, 0x0, 0x3);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    /* Commands */
    command::create_move_to_level(cluster);
    command::create_move(cluster);
    command::create_step(cluster);
    command::create_stop(cluster);
    command::create_move_to_level_with_on_off(cluster);
    command::create_move_with_on_off(cluster);
    command::create_step_with_on_off(cluster);
    command::create_stop_with_on_off(cluster);

    /* Features */
    if (features & feature::on_off::get_id()) {
        feature::on_off::add(cluster);
    }
    if (features & feature::lighting::get_id()) {
        feature::lighting::add(cluster, &(config->lighting));
    }

    return cluster;
}
} /* level_control */

namespace color_control {
const function_generic_t function_list[] = {
    (function_generic_t)emberAfColorControlClusterServerInitCallback,
    (function_generic_t)MatterColorControlClusterServerShutdownCallback,
};
const int function_flags = CLUSTER_FLAG_INIT_FUNCTION | CLUSTER_FLAG_SHUTDOWN_FUNCTION;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags, uint32_t features)
{
    cluster_t *cluster = cluster::create(endpoint, ColorControl::Id, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        set_plugin_server_init_callback(cluster, MatterColorControlPluginServerInitCallback);
        add_function_list(cluster, function_list, function_flags);
    }
    if (flags & CLUSTER_FLAG_CLIENT) {
        set_plugin_client_init_callback(cluster, MatterColorControlPluginClientInitCallback);
        create_default_binding_cluster(endpoint);
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        if (config) {
            global::attribute::create_cluster_revision(cluster, config->cluster_revision);
            attribute::create_color_mode(cluster, config->color_mode);
            attribute::create_color_control_options(cluster, config->color_control_options);
            attribute::create_enhanced_color_mode(cluster, config->enhanced_color_mode);
            attribute::create_color_capabilities(cluster, config->color_capabilities);
            attribute::create_number_of_primaries(cluster, config->number_of_primaries);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }

        /* Attributes managed internally */
        attribute::create_remaining_time(cluster, 0);
        for (uint8_t idx = 1; idx <= config->number_of_primaries.value_or(0); ++idx) {
            attribute::create_primary_n_x(cluster, 0, idx);
            attribute::create_primary_n_y(cluster, 0, idx);
            attribute::create_primary_n_intensity(cluster, nullable<uint8_t>(), idx);
        }
    }

    /* Features */
    if (features & feature::hue_saturation::get_id()) {
        feature::hue_saturation::add(cluster, &(config->hue_saturation));
    }
    if (features & feature::color_temperature::get_id()) {
        feature::color_temperature::add(cluster, &(config->color_temperature));
    }
    if (features & feature::xy::get_id()) {
        feature::xy::add(cluster, &(config->xy));
    }
    if (features & feature::enhanced_hue::get_id()) {
        feature::enhanced_hue::add(cluster, &(config->enhanced_hue));
    }
    if (features & feature::color_loop::get_id()) {
        feature::color_loop::add(cluster, &(config->color_loop));
    }

    return cluster;
}
} /* color_control */

namespace fan_control {
const function_generic_t function_list[] = {
    (function_generic_t)MatterFanControlClusterServerAttributeChangedCallback,
    (function_generic_t)MatterFanControlClusterServerPreAttributeChangedCallback,
};
const int function_flags = CLUSTER_FLAG_ATTRIBUTE_CHANGED_FUNCTION | CLUSTER_FLAG_PRE_ATTRIBUTE_CHANGED_FUNCTION;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, FanControl::Id, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        set_plugin_server_init_callback(cluster, MatterFanControlPluginServerInitCallback);
        add_function_list(cluster, function_list, function_flags);
    }
    if (flags & CLUSTER_FLAG_CLIENT) {
        /* not implemented: Setting NULL since the MatterFanControlPluginClientInitCallback is not implemented */
        set_plugin_client_init_callback(cluster, NULL);
        create_default_binding_cluster(endpoint);
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        if (config) {
            global::attribute::create_cluster_revision(cluster, config->cluster_revision);
            attribute::create_fan_mode(cluster, config->fan_mode, 0, 6);
            attribute::create_fan_mode_sequence(cluster, config->fan_mode_sequence);
            attribute::create_percent_setting(cluster, config->percent_setting);
            attribute::create_percent_current(cluster, config->percent_current);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    return cluster;
}
} /* fan_control */

namespace thermostat {
const function_generic_t function_list[] = {
    (function_generic_t)emberAfThermostatClusterServerInitCallback,
    (function_generic_t)MatterThermostatClusterServerPreAttributeChangedCallback,
};
const int function_flags = CLUSTER_FLAG_INIT_FUNCTION | CLUSTER_FLAG_PRE_ATTRIBUTE_CHANGED_FUNCTION;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, Thermostat::Id, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        set_plugin_server_init_callback(cluster, MatterThermostatPluginServerInitCallback);
        add_function_list(cluster, function_list, function_flags);
    }
    if (flags & CLUSTER_FLAG_CLIENT) {
        set_plugin_client_init_callback(cluster, MatterThermostatPluginClientInitCallback);
        create_default_binding_cluster(endpoint);
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        if (config) {
            global::attribute::create_cluster_revision(cluster, config->cluster_revision);
            attribute::create_local_temperature(cluster, config->local_temperature);
            attribute::create_control_sequence_of_operation(cluster, config->control_sequence_of_operation, 0x0, 0x5);
            attribute::create_system_mode(cluster, config->system_mode, 0x0, 0x7);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    /* Commands */
    command::create_setpoint_raise_lower(cluster);

    return cluster;
}
} /* thermostat */

namespace door_lock {
const function_generic_t function_list[] = {
    (function_generic_t)MatterDoorLockClusterServerAttributeChangedCallback,
    (function_generic_t)MatterDoorLockClusterServerShutdownCallback,
    (function_generic_t)MatterDoorLockClusterServerPreAttributeChangedCallback,
};
const int function_flags = CLUSTER_FLAG_ATTRIBUTE_CHANGED_FUNCTION | CLUSTER_FLAG_SHUTDOWN_FUNCTION |
    CLUSTER_FLAG_PRE_ATTRIBUTE_CHANGED_FUNCTION;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, DoorLock::Id, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        set_plugin_server_init_callback(cluster, MatterDoorLockPluginServerInitCallback);
        add_function_list(cluster, function_list, function_flags);
    }
    if (flags & CLUSTER_FLAG_CLIENT) {
        set_plugin_client_init_callback(cluster, MatterDoorLockPluginClientInitCallback);
        create_default_binding_cluster(endpoint);
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        if (config) {
            global::attribute::create_cluster_revision(cluster, config->cluster_revision);
            attribute::create_lock_state(cluster, config->lock_state);
            attribute::create_lock_type(cluster, config->lock_type);
            attribute::create_actuator_enabled(cluster, config->actuator_enabled);
            attribute::create_operating_mode(cluster, config->operating_mode, 0x0, 0x4);
            attribute::create_supported_operating_modes(cluster, config->supported_operating_modes);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    /* Commands */
    command::create_lock_door(cluster);
    command::create_unlock_door(cluster);

    return cluster;
}
} /* door_lock */

namespace window_covering {
const function_generic_t function_list[] = {
    (function_generic_t)MatterWindowCoveringClusterServerAttributeChangedCallback,
};
const int function_flags = CLUSTER_FLAG_ATTRIBUTE_CHANGED_FUNCTION;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags, uint32_t features)
{
    cluster_t *cluster = cluster::create(endpoint, WindowCovering::Id, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        set_plugin_server_init_callback(cluster, MatterWindowCoveringPluginServerInitCallback);
        add_function_list(cluster, function_list, function_flags);
    }
    if (flags & CLUSTER_FLAG_CLIENT) {
        set_plugin_client_init_callback(cluster, MatterWindowCoveringPluginClientInitCallback);
        create_default_binding_cluster(endpoint);
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        if (config) {
            global::attribute::create_cluster_revision(cluster, config->cluster_revision);
            attribute::create_type(cluster, config->type);
            attribute::create_config_status(cluster, config->config_status);
            attribute::create_operational_status(cluster, config->operational_status);
            attribute::create_end_product_type(cluster, config->end_product_type);
            attribute::create_mode(cluster, config->mode);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    /* Commands */
    command::create_up_or_open(cluster);
    command::create_down_or_close(cluster);
    command::create_stop_motion(cluster);

    /* Features */
    if (features & feature::lift::get_id()) {
        feature::lift::add(cluster, &(config->lift));
    }

    return cluster;
}
} /* window_covering */

namespace switch_cluster {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, Switch::Id, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        set_plugin_server_init_callback(cluster, MatterSwitchPluginServerInitCallback);
        add_function_list(cluster, function_list, function_flags);
    }
    if (flags & CLUSTER_FLAG_CLIENT) {
        set_plugin_client_init_callback(cluster, MatterSwitchPluginClientInitCallback);
        create_default_binding_cluster(endpoint);
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        if (config) {
            global::attribute::create_cluster_revision(cluster, config->cluster_revision);
            attribute::create_number_of_positions(cluster, config->number_of_positions);
            attribute::create_current_position(cluster, config->current_position);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    return cluster;
}
} /* switch_cluster */

namespace temperature_measurement {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, TemperatureMeasurement::Id, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        set_plugin_server_init_callback(cluster, MatterTemperatureMeasurementPluginServerInitCallback);
        add_function_list(cluster, function_list, function_flags);
    }
    if (flags & CLUSTER_FLAG_CLIENT) {
        set_plugin_client_init_callback(cluster, MatterTemperatureMeasurementPluginClientInitCallback);
        create_default_binding_cluster(endpoint);
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        if (config) {
            global::attribute::create_cluster_revision(cluster, config->cluster_revision);
            attribute::create_temperature_measured_value(cluster, config->measured_value);
            attribute::create_temperature_min_measured_value(cluster, config->min_measured_value);
            attribute::create_temperature_max_measured_value(cluster, config->max_measured_value);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    return cluster;
}
} /* temperature_measurement */

namespace relative_humidity_measurement {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, RelativeHumidityMeasurement::Id, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        set_plugin_server_init_callback(cluster, MatterRelativeHumidityMeasurementPluginServerInitCallback);
        add_function_list(cluster, function_list, function_flags);
    }
    if (flags & CLUSTER_FLAG_CLIENT) {
        set_plugin_client_init_callback(cluster, MatterRelativeHumidityMeasurementPluginClientInitCallback);
        create_default_binding_cluster(endpoint);
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        if (config) {
            global::attribute::create_cluster_revision(cluster, config->cluster_revision);
            attribute::create_relative_humidity_measured_value(cluster, config->measured_value);
            attribute::create_relative_humidity_min_measured_value(cluster, config->min_measured_value);
            attribute::create_relative_humidity_max_measured_value(cluster, config->max_measured_value);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    return cluster;
}
} /* relative_humidity_measurement */

namespace occupancy_sensing {
const function_generic_t function_list[] = {
    (function_generic_t)emberAfOccupancySensingClusterServerInitCallback,
};
const int function_flags = CLUSTER_FLAG_INIT_FUNCTION;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, OccupancySensing::Id, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        set_plugin_server_init_callback(cluster, MatterOccupancySensingPluginServerInitCallback);
        add_function_list(cluster, function_list, function_flags);
    }
    if (flags & CLUSTER_FLAG_CLIENT) {
        set_plugin_client_init_callback(cluster, MatterOccupancySensingPluginClientInitCallback);
        create_default_binding_cluster(endpoint);
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        if (config) {
            global::attribute::create_cluster_revision(cluster, config->cluster_revision);
            attribute::create_occupancy(cluster, config->occupancy);
            attribute::create_occupancy_sensor_type(cluster, config->occupancy_sensor_type);
            attribute::create_occupancy_sensor_type_bitmap(cluster, config->occupancy_sensor_type_bitmap);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    return cluster;
}
} /* occupancy_sensing */

namespace boolean_state {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, BooleanState::Id, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        set_plugin_server_init_callback(cluster, MatterBooleanStatePluginServerInitCallback);
        add_function_list(cluster, function_list, function_flags);
    }
    if (flags & CLUSTER_FLAG_CLIENT) {
        set_plugin_client_init_callback(cluster, MatterBooleanStatePluginClientInitCallback);
        create_default_binding_cluster(endpoint);
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        if (config) {
            global::attribute::create_cluster_revision(cluster, config->cluster_revision);
            attribute::state_value(cluster, config->state_value);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    return cluster;
}
} /* boolean_state */

namespace localization_configuration {
const function_generic_t function_list[] = {
    (function_generic_t)emberAfLocalizationConfigurationClusterServerInitCallback,
    (function_generic_t)MatterLocalizationConfigurationClusterServerPreAttributeChangedCallback};
const int function_flags = CLUSTER_FLAG_INIT_FUNCTION | CLUSTER_FLAG_PRE_ATTRIBUTE_CHANGED_FUNCTION;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, LocalizationConfiguration::Id, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        set_plugin_server_init_callback(cluster, MatterLocalizationConfigurationPluginServerInitCallback);
        add_function_list(cluster, function_list, function_flags);
    }
    if (flags & CLUSTER_FLAG_CLIENT) {
        set_plugin_client_init_callback(cluster, MatterLocalizationConfigurationPluginClientInitCallback);
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        /* Attributes not managed internally */
        global::attribute::create_feature_map(cluster, 0);

        if (config) {
            /* Attributes not managed internally */
            global::attribute::create_cluster_revision(cluster, config->cluster_revision);
            attribute::create_active_locale(cluster, config->active_locale, sizeof(config->active_locale));

            /* Attributes managed internally */
            attribute::create_supported_locales(cluster, NULL, 0, 0);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    return cluster;
}

} /* localization_configuration */

namespace time_format_localization {
const function_generic_t function_list[] = {
    (function_generic_t)emberAfTimeFormatLocalizationClusterServerInitCallback,
    (function_generic_t)MatterLocalizationConfigurationClusterServerPreAttributeChangedCallback};
const int function_flags = CLUSTER_FLAG_INIT_FUNCTION | CLUSTER_FLAG_PRE_ATTRIBUTE_CHANGED_FUNCTION;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags, uint32_t features)
{
    cluster_t *cluster = cluster::create(endpoint, TimeFormatLocalization::Id, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        set_plugin_server_init_callback(cluster, MatterTimeFormatLocalizationPluginServerInitCallback);
        add_function_list(cluster, function_list, function_flags);
    }
    if (flags & CLUSTER_FLAG_CLIENT) {
        set_plugin_client_init_callback(cluster, MatterTimeFormatLocalizationPluginClientInitCallback);
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        /* Attributes not managed internally */
        global::attribute::create_feature_map(cluster, 0);

        if (config) {
            /* Attributes not managed internally */
            global::attribute::create_cluster_revision(cluster, config->cluster_revision);

            /* Attributes managed internally */
            attribute::create_hour_format(cluster, config->hour_format);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    /* Features */
    if (features & feature::calendar_format::get_id()) {
        feature::calendar_format::add(cluster, &(config->calendar_format));
    }

    return cluster;
}
} /* time_format_localization */

namespace illuminance_measurement {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, IlluminanceMeasurement::Id, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }
    if (flags & CLUSTER_FLAG_SERVER) {
        set_plugin_server_init_callback(cluster, MatterIlluminanceMeasurementPluginServerInitCallback);
        add_function_list(cluster, function_list, function_flags);
    }
    if (flags & CLUSTER_FLAG_CLIENT) {
        set_plugin_client_init_callback(cluster, MatterIlluminanceMeasurementPluginClientInitCallback);
        create_default_binding_cluster(endpoint);
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);
        /** Attributes not managed internally **/
        if (config) {
            global::attribute::create_cluster_revision(cluster, config->cluster_revision);
            attribute::create_illuminance_measured_value(cluster, config->illuminance_measured_value, 0x0000, 0xFFFF);
            attribute::create_illuminance_min_measured_value(cluster, config->illuminance_min_measured_value, 0x0001,
                                                             0xFFFD);
            attribute::create_illuminance_max_measured_value(cluster, config->illuminance_max_measured_value, 0x0002,
                                                             0xFFFE);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    return cluster;
}
} /* illuminance_measurement */

namespace pressure_measurement {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, PressureMeasurement::Id, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }
    if (flags & CLUSTER_FLAG_SERVER) {
        set_plugin_server_init_callback(cluster, MatterPressureMeasurementPluginServerInitCallback);
        add_function_list(cluster, function_list, function_flags);
    }
    if (flags & CLUSTER_FLAG_CLIENT) {
        set_plugin_client_init_callback(cluster, MatterPressureMeasurementPluginClientInitCallback);
        create_default_binding_cluster(endpoint);
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);
        /** Attributes not managed internally **/
        if (config) {
            global::attribute::create_cluster_revision(cluster, config->cluster_revision);
            attribute::create_pressure_measured_value(cluster, config->pressure_measured_value);
            attribute::create_pressure_min_measured_value(cluster, config->pressure_min_measured_value);
            attribute::create_pressure_max_measured_value(cluster, config->pressure_max_measured_value);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    return cluster;
}
} /* pressure_measurement */

namespace flow_measurement {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, FlowMeasurement::Id, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }
    if (flags & CLUSTER_FLAG_SERVER) {
        set_plugin_server_init_callback(cluster, MatterFlowMeasurementPluginServerInitCallback);
        add_function_list(cluster, function_list, function_flags);
    }
    if (flags & CLUSTER_FLAG_CLIENT) {
        set_plugin_client_init_callback(cluster, MatterFlowMeasurementPluginClientInitCallback);
        create_default_binding_cluster(endpoint);
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);
        /** Attributes not managed internally **/
        if (config) {
            global::attribute::create_cluster_revision(cluster, config->cluster_revision);
            attribute::create_flow_measured_value(cluster, config->flow_measured_value);
            attribute::create_flow_min_measured_value(cluster, config->flow_min_measured_value);
            attribute::create_flow_max_measured_value(cluster, config->flow_max_measured_value);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    return cluster;
}
} /* flow_measurement */

namespace pump_configuration_and_control {
const function_generic_t function_list[] = {
    (function_generic_t)emberAfPumpConfigurationAndControlClusterServerInitCallback,
    (function_generic_t)MatterPumpConfigurationAndControlClusterServerAttributeChangedCallback,
    (function_generic_t)MatterPumpConfigurationAndControlClusterServerPreAttributeChangedCallback,
};
const int function_flags =
    CLUSTER_FLAG_INIT_FUNCTION | CLUSTER_FLAG_ATTRIBUTE_CHANGED_FUNCTION | CLUSTER_FLAG_PRE_ATTRIBUTE_CHANGED_FUNCTION;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, PumpConfigurationAndControl::Id, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }
    if (flags & CLUSTER_FLAG_SERVER) {
        set_plugin_server_init_callback(cluster, MatterPumpConfigurationAndControlPluginServerInitCallback);
        add_function_list(cluster, function_list, function_flags);
    }
    if (flags & CLUSTER_FLAG_CLIENT) {
        set_plugin_client_init_callback(cluster, MatterPumpConfigurationAndControlPluginClientInitCallback);
        create_default_binding_cluster(endpoint);
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /** Attributes not managed internally **/
        if (config) {
            global::attribute::create_cluster_revision(cluster, config->cluster_revision);
            attribute::create_max_pressure(cluster, config->max_pressure);
            attribute::create_max_speed(cluster, config->max_speed);
            attribute::create_max_flow(cluster, config->max_flow);
            attribute::create_effective_operation_mode(cluster, config->effective_operation_mode);
            attribute::create_effective_control_mode(cluster, config->effective_control_mode);
            attribute::create_capacity(cluster, config->capacity);
            attribute::create_operation_mode(cluster, config->operation_mode);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    return cluster;
}
} /* pump_configuration_and_control */

namespace temperature_control {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags, uint32_t features)
{
    cluster_t *cluster = cluster::create(endpoint, TemperatureControl::Id, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        //set_plugin_server_init_callback(cluster, MatterTemperatureControlPluginServerInitCallback);
        add_function_list(cluster, function_list, function_flags);
    }
    if (flags & CLUSTER_FLAG_CLIENT) {
        //set_plugin_client_init_callback(cluster, MatterTemperatureControlPluginClientInitCallback);
        create_default_binding_cluster(endpoint);
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        if (config) {
            global::attribute::create_cluster_revision(cluster, config->cluster_revision);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    /* Commands */
    command::create_set_temperature(cluster);

    /* Features */
    if (features & feature::temperature_number::get_id()) {
        feature::temperature_number::add(cluster, &(config->temperature_number));
    } else{
        if (features & feature::temperature_level::get_id()) {
            feature::temperature_level::add(cluster, &(config->temperature_level));
        }
    }

    return cluster;
}
} /* temperature_control */

namespace refrigerator_alarm {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, RefrigeratorAlarm::Id, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        //set_plugin_server_init_callback(cluster, MatterRefrigeratorAlarmPluginServerInitCallback);
        add_function_list(cluster, function_list, function_flags);
    }
    if (flags & CLUSTER_FLAG_CLIENT) {
        //set_plugin_client_init_callback(cluster, MatterRefrigeratorAlarmPluginClientInitCallback);
        create_default_binding_cluster(endpoint);
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        if (config) {
            global::attribute::create_cluster_revision(cluster, config->cluster_revision);
            attribute::create_mask(cluster, config->mask);
            attribute::create_latch(cluster, config->latch);
            attribute::create_state(cluster, config->state);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    /* Commands */
    command::create_reset(cluster);


    return cluster;
}
} /* on_off */

#endif // FIXED_ENDPOINT_COUNT == 0
} /* cluster */
} /* esp_matter */
