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
#include <app-common/zap-generated/callback.h>
#include <app-common/zap-generated/callbacks/PluginCallbacks.h>

static const char *TAG = "esp_matter_cluster";

using namespace chip::app::Clusters;

namespace esp_matter {
namespace cluster {

void plugin_init_callback_common()
{
    ESP_LOGI(TAG, "Cluster plugin init common callback");
    node_t *node = node::get();
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
        create_default_binding_cluster(endpoint);
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        /* Attributes managed internally */
        global::attribute::create_cluster_revision(cluster, 0);
        global::attribute::create_feature_map(cluster, 0);
        attribute::create_device_list(cluster, NULL, 0, 0);
        attribute::create_server_list(cluster, NULL, 0, 0);
        attribute::create_client_list(cluster, NULL, 0, 0);
        attribute::create_parts_list(cluster, NULL, 0, 0);
    }

    return cluster;
}
} /* descriptor */

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
        create_default_binding_cluster(endpoint);
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        /* Attributes managed internally */
        global::attribute::create_cluster_revision(cluster, 0);
        global::attribute::create_feature_map(cluster, 0);
        attribute::create_acl(cluster, NULL, 0, 0);
        attribute::create_extension(cluster, NULL, 0, 0);
        attribute::create_subjects_per_access_control_entry(cluster, 0);
        attribute::create_access_control_entries_per_fabric(cluster, 0);
        attribute::create_targets_per_access_control_entry(cluster, 0);
    }

    return cluster;
}
} /* access_control */

namespace basic {
const function_generic_t function_list[] = {
    (function_generic_t)emberAfBasicClusterServerInitCallback,
};
const int function_flags = CLUSTER_FLAG_INIT_FUNCTION;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, Basic::Id, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        set_plugin_server_init_callback(cluster, MatterBasicPluginServerInitCallback);
        add_function_list(cluster, function_list, function_flags);
    }
    if (flags & CLUSTER_FLAG_CLIENT) {
        set_plugin_client_init_callback(cluster, MatterBasicPluginClientInitCallback);
        create_default_binding_cluster(endpoint);
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
} /* basic */

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
        create_default_binding_cluster(endpoint);
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);
        attribute::create_default_ota_providers(cluster, NULL, 0);

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
        create_default_binding_cluster(endpoint);
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
        create_default_binding_cluster(endpoint);
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        /* Attributes managed internally */
        attribute::create_max_networks(cluster, 0);
        attribute::create_networks(cluster, NULL, 0, 0);
        attribute::create_scan_max_time_seconds(cluster, 0);
        attribute::create_connect_max_time_seconds(cluster, 0);
        attribute::create_interface_enabled(cluster, 0);
        attribute::create_last_networking_status(cluster, 0);
        attribute::create_last_network_id(cluster, NULL, 0);
        attribute::create_last_connect_error_value(cluster, 0);
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
        create_default_binding_cluster(endpoint);
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
        create_default_binding_cluster(endpoint);
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
        create_default_binding_cluster(endpoint);
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
        create_default_binding_cluster(endpoint);
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
        create_default_binding_cluster(endpoint);
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);
        attribute::create_bssid(cluster, NULL, 0);
        attribute::create_security_type(cluster, 0);
        attribute::create_wifi_version(cluster, 0);
        attribute::create_channel_number(cluster, 0);
        attribute::create_rssi(cluster, 0);

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
        create_default_binding_cluster(endpoint);
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);
        attribute::create_channel(cluster, 0);
        attribute::create_routing_role(cluster, 0);
        attribute::create_network_name(cluster, NULL, 0);
        attribute::create_pan_id(cluster, 0);
        attribute::create_extended_pan_id(cluster, 0);
        attribute::create_mesh_local_prefix(cluster, NULL, 0);
        attribute::create_neighbor_table(cluster, NULL, 0, 0);
        attribute::create_route_table(cluster, NULL, 0, 0);
        attribute::create_extended_partition_id(cluster, 0);
        attribute::create_weighting(cluster, 0);
        attribute::create_data_version(cluster, 0);
        attribute::create_stable_data_version(cluster, 0);
        attribute::create_leader_router_id(cluster, 0);
        attribute::create_security_policy(cluster, NULL, 0, 0);
        attribute::create_channel_mask(cluster, NULL, 0);
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
        create_default_binding_cluster(endpoint);
    }

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
    command::create_trigger_effect(cluster);

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
        create_default_binding_cluster(endpoint);
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
        create_default_binding_cluster(endpoint);
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
};
const int function_flags = CLUSTER_FLAG_INIT_FUNCTION;

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
};
const int function_flags = CLUSTER_FLAG_INIT_FUNCTION;

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
};
const int function_flags = CLUSTER_FLAG_INIT_FUNCTION;

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
        for (uint8_t idx = 1; idx <= config->number_of_primaries; ++idx) {
            attribute::create_primary_n_x(cluster, 0, idx);
            attribute::create_primary_n_y(cluster, 0, idx);
            attribute::create_primary_n_intensity(cluster, 0, idx);
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
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, FanControl::Id, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        /* not implemented: Setting NULL since the MatterFanControlPluginServerInitCallback is not implemented */
        set_plugin_server_init_callback(cluster, NULL);
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
            attribute::create_fan_mode(cluster, config->fan_mode);
            attribute::create_fan_mode_sequence(cluster, config->fan_mode_sequence);
            /* Not implemented
            attribute::create_percent_setting(cluster, config->percent_setting);
            attribute::create_percent_current(cluster, config->percent_current);
            */
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
};
const int function_flags = CLUSTER_FLAG_INIT_FUNCTION;

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
            attribute::create_occupied_cooling_setpoint(cluster, config->occupied_cooling_setpoint);
            attribute::create_occupied_heating_setpoint(cluster, config->occupied_heating_setpoint);
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
    (function_generic_t)MatterDoorLockClusterServerPreAttributeChangedCallback,
};
const int function_flags = CLUSTER_FLAG_ATTRIBUTE_CHANGED_FUNCTION | CLUSTER_FLAG_PRE_ATTRIBUTE_CHANGED_FUNCTION;

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
            attribute::create_auto_relock_time(cluster, config->auto_relock_time);
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

    return cluster;
}
} /* time_synchronization */

namespace bridged_device_basic {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, BridgedDeviceBasic::Id, flags);
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
            attribute::create_bridged_device_basic_node_label(cluster, config->node_label, sizeof(config->node_label));
            attribute::create_reachable(cluster, config->reachable);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    return cluster;
}
} /* bridged_device_basic */

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
        create_default_binding_cluster(endpoint);
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
            attribute::create_multi_press_max(cluster, config->multi_press_max);
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

namespace occupancy_sensing {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

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

} /* cluster */
} /* esp_matter */
