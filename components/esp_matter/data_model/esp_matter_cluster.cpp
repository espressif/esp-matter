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

#include <cassert>
#include <esp_log.h>
#include <esp_matter.h>
#include <esp_matter_attribute.h>
#include <esp_matter_cluster.h>
#include <esp_matter_command.h>
#include <esp_matter_core.h>
#include <esp_matter_data_model.h>
#include <esp_matter_delegate_callbacks.h>
#include <esp_matter_cluster_revisions.h>
#include <esp_matter_attribute_bounds.h>

#include <app-common/zap-generated/callback.h>
#include <app-common/zap-generated/cluster-enums.h>
#include <app/PluginApplicationCallbacks.h>
#include <app/ClusterCallbacks.h>
#include <lib/support/TypeTraits.h>
#include <lib/support/CodeUtils.h>

static const char *TAG = "esp_matter_cluster";

using namespace chip::app::Clusters;
using namespace esp_matter::cluster::delegate_cb;

#define CALL_ONCE(cb)                           \
    [](){                                       \
        static bool is_called = false;          \
        if (!is_called) {                       \
            cb();                               \
            is_called = true;                   \
        }                                       \
    }

// convenience macro for checking if features is enabled or not
// must be used only from the ::create() API so that it can find config
// eg: if (has(feature::constant_pressure)) { ... }
#define has(flags) ((config->feature_flags & flags) ? 1 : 0)

namespace {

// this space is for feature validation functions

enum class feature_policy
{
    k_exact_one = 0,        // O.a
    k_at_least_one = 1,     // 0.a+
};

const char feature_policy_strs[2][16] = {"Exactly one", "At least one"};

bool validate_features(uint32_t feature_flag, feature_policy policy,
                       const char *feature_name, std::initializer_list<uint32_t> features)
{
    uint8_t count = 0;
    for (uint32_t feature : features) {
        if (feature & feature_flag) {
            count++;
        }
    }

    bool result = false;

    switch (policy) {
    case feature_policy::k_exact_one:
        result = count == 1;
        break;
    case feature_policy::k_at_least_one:
        result = count >= 1;
        break;
    }

    if (!result) {
        ESP_LOGE(TAG, "%s of the feature(s) must be supported from (%s)", feature_policy_strs[static_cast<uint8_t>(policy)], feature_name);
    }

    return result;
}

// Macros to reduce repetitive validation code
#define VALIDATE_FEATURES_EXACT_ONE(name, ...) \
    do { if (!validate_features(config->feature_flags, feature_policy::k_exact_one, name, {__VA_ARGS__})) \
        return ABORT_CLUSTER_CREATE(cluster); } while(0)

#define VALIDATE_FEATURES_AT_LEAST_ONE(name, ...) \
    do { if (!validate_features(config->feature_flags, feature_policy::k_at_least_one, name, {__VA_ARGS__})) \
        return ABORT_CLUSTER_CREATE(cluster); } while(0)

} // anonymous namespace

namespace esp_matter {
namespace cluster {

namespace {
// simple API to abort cluster creation and return NULL
cluster_t * ABORT_CLUSTER_CREATE(cluster_t *cluster)
{
    esp_matter::cluster::destroy(cluster);
    assert(false);
    return NULL;
}
} // anonymous namespace

void plugin_init_callback_common()
{
    ESP_LOGI(TAG, "Cluster plugin init common callback");
    node_t *node = node::get();
    /* Skip plugin_init_callback_common when ESP Matter data model is not used */
    VerifyOrReturn(node);
    endpoint_t *endpoint = endpoint::get_first(node);
    while (endpoint) {
        cluster_t *cluster = get_first(endpoint);
        while (cluster) {
            /* Plugin server init callback */
            plugin_server_init_callback_t plugin_server_init_callback = get_plugin_server_init_callback(cluster);
            if (plugin_server_init_callback) {
                plugin_server_init_callback();
            }
            cluster = get_next(cluster);
        }
        endpoint = endpoint::get_next(endpoint);
    }
}

void delegate_init_callback_common()
{
    node_t *node = node::get();
    /* Skip delegate_init_callback_common when ESP Matter data model is not used */
    VerifyOrReturn(node);
    endpoint_t *endpoint = endpoint::get_first(node);
    while (endpoint) {
        uint16_t endpoint_id = endpoint::get_id(endpoint);
        cluster_t *cluster = get_first(endpoint);
        while (cluster) {
            /* Delegate server init callback */
            delegate_init_callback_t delegate_init_callback = get_delegate_init_callback(cluster);
            if (delegate_init_callback) {
                delegate_init_callback(get_delegate_impl(cluster), endpoint_id);
            }
            cluster = get_next(cluster);
        }
        endpoint = endpoint::get_next(endpoint);
    }
}

void add_bounds_callback_common()
{
    node_t *node = node::get();
    VerifyOrReturn(node);
    endpoint_t *endpoint = endpoint::get_first(node);
    while (endpoint) {
        cluster_t *cluster = get_first(endpoint);
        while (cluster) {
            add_bounds_callback_t add_bounds_callback = get_add_bounds_callback(cluster);
            if (add_bounds_callback) {
                add_bounds_callback(cluster);
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
    VerifyOrReturnValue(!cluster, cluster);
    ESP_LOGI(TAG, "Creating default binding cluster");
    binding::config_t config;
    return binding::create(endpoint, &config, CLUSTER_FLAG_SERVER);
}

namespace descriptor {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, Descriptor::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, Descriptor::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        static const auto plugin_server_init_cb = CALL_ONCE(MatterDescriptorPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        attribute::create_device_list(cluster, NULL, 0, 0);
        attribute::create_server_list(cluster, NULL, 0, 0);
        attribute::create_client_list(cluster, NULL, 0, 0);
        attribute::create_parts_list(cluster, NULL, 0, 0);

        /* Attributes updated later */
        global::attribute::create_feature_map(cluster, 0);
        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);
    }

    return cluster;
}
} /* descriptor */

namespace actions {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, Actions::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, Actions::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        if (config && config->delegate != nullptr) {
            static const auto delegate_init_cb = ActionsDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        set_plugin_server_init_callback(cluster, NULL);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        attribute::create_action_list(cluster, NULL, 0, 0);
        attribute::create_endpoint_lists(cluster, NULL, 0, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);
    }

    event::create_action_failed(cluster);
    event::create_state_changed(cluster);

    return cluster;
}
} /* actions */

namespace access_control {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, AccessControl::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, AccessControl::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        static const auto plugin_server_init_cb = CALL_ONCE(MatterAccessControlPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        attribute::create_acl(cluster, NULL, 0, 0);
        attribute::create_subjects_per_access_control_entry(cluster, 0);
        attribute::create_access_control_entries_per_fabric(cluster, 0);
        attribute::create_targets_per_access_control_entry(cluster, 0);

        /* Attributes updated later */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);
    }

    event::create_access_control_entry_changed(cluster);

    return cluster;
}
} /* access_control */

namespace basic_information {
const function_generic_t * function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, BasicInformation::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, BasicInformation::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        static const auto plugin_server_init_cb = CALL_ONCE(MatterBasicInformationPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

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
        attribute::create_unique_id(cluster, NULL, 0);
        attribute::create_capability_minima(cluster, NULL, 0, 0);
        attribute::create_specification_version(cluster, 0);
        attribute::create_max_paths_per_invoke(cluster, 0);
        attribute::create_configuration_version(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        if (config) {
            attribute::create_node_label(cluster, config->node_label, strlen(config->node_label));
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    event::create_start_up(cluster);

    return cluster;
}
} /* basic_information */

namespace binding {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, Binding::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, Binding::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        static const auto plugin_server_init_cb = CALL_ONCE(MatterBindingPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        attribute::create_binding(cluster, NULL, 0, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);
    }

    /* Extra initialization */
    client::binding_init();

    return cluster;
}
} /* binding */

namespace ota_software_update_provider {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, OtaSoftwareUpdateProvider::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, OtaSoftwareUpdateProvider::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        if (config && config->delegate != nullptr) {
            static const auto delegate_init_cb = OtaSoftwareUpdateProviderDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        static const auto plugin_server_init_cb = CALL_ONCE(MatterOtaSoftwareUpdateProviderPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);
        cluster::set_init_and_shutdown_callbacks(cluster, ESPMatterOtaSoftwareUpdateProviderClusterServerInitCallback,
                                                 ESPMatterOtaSoftwareUpdateProviderClusterServerShutdownCallback);
    }

    /* Commands */
    command::create_query_image(cluster);
    command::create_query_image_response(cluster);
    command::create_apply_update_request(cluster);
    command::create_apply_update_response(cluster);
    command::create_notify_update_applied(cluster);

    return cluster;
}
} /* ota_software_update_provider */

namespace ota_software_update_requestor {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, OtaSoftwareUpdateRequestor::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, OtaSoftwareUpdateRequestor::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        static const auto plugin_server_init_cb = CALL_ONCE(MatterOtaSoftwareUpdateRequestorPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        attribute::create_default_ota_providers(cluster, NULL, 0, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);
        if (config) {
            attribute::create_update_possible(cluster, config->update_possible);
            attribute::create_update_state(cluster, config->update_state);
            attribute::create_update_state_progress(cluster, config->update_state_progress);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    event::create_download_error(cluster);
    event::create_state_transition(cluster);
    event::create_version_applied(cluster);

    /* Commands */
    command::create_announce_ota_provider(cluster);

    return cluster;
}
} /* ota_software_update_requestor */

namespace general_commissioning {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, GeneralCommissioning::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, GeneralCommissioning::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        static const auto plugin_server_init_cb = CALL_ONCE(MatterGeneralCommissioningPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        attribute::create_regulatory_config(cluster, 0);
        attribute::create_location_capability(cluster, 0);
        attribute::create_basic_commissioning_info(cluster, NULL, 0, 0);
        attribute::create_supports_concurrent_connection(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);
        if (config) {
            attribute::create_breadcrumb(cluster, config->breadcrumb);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }

        /* Commands */
        command::create_arm_fail_safe(cluster);
        command::create_arm_fail_safe_response(cluster);
        command::create_set_regulatory_config(cluster);
        command::create_set_regulatory_config_response(cluster);
        command::create_commissioning_complete(cluster);
        command::create_commissioning_complete_response(cluster);
    }

    return cluster;
}
} /* general_commissioning */

namespace network_commissioning {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, NetworkCommissioning::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, NetworkCommissioning::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        static const auto plugin_server_init_cb = CALL_ONCE(MatterNetworkCommissioningPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        attribute::create_max_networks(cluster, 0);
        attribute::create_networks(cluster, NULL, 0, 0);
        attribute::create_interface_enabled(cluster, 0);
        attribute::create_last_networking_status(cluster, nullable<uint8_t>());
        attribute::create_last_network_id(cluster, NULL, 0);
        attribute::create_last_connect_error_value(cluster, nullable<int32_t>());

        if (config->feature_map & chip::to_underlying(NetworkCommissioning::Feature::kWiFiNetworkInterface) ||
            config->feature_map & chip::to_underlying(NetworkCommissioning::Feature::kThreadNetworkInterface)) {
            attribute::create_scan_max_time_seconds(cluster, 0);
            attribute::create_connect_max_time_seconds(cluster, 0);
        }
        if (config->feature_map & chip::to_underlying(NetworkCommissioning::Feature::kWiFiNetworkInterface)) {
            attribute::create_supported_wifi_bands(cluster, NULL, 0, 0);
        }
        if (config->feature_map & chip::to_underlying(NetworkCommissioning::Feature::kThreadNetworkInterface)) {
            attribute::create_supported_thread_features(cluster, 0);
            attribute::create_thread_version(cluster, 0);
        }
        global::attribute::create_feature_map(cluster, config->feature_map);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        /* Commands */
        command::create_scan_networks(cluster);
        command::create_remove_network(cluster);
        command::create_connect_network(cluster);
        command::create_reorder_network(cluster);
        command::create_scan_networks_response(cluster);
        command::create_network_config_response(cluster);
        command::create_connect_network_response(cluster);

        cluster::set_init_and_shutdown_callbacks(cluster, ESPMatterNetworkCommissioningClusterServerInitCallback,
                                                 ESPMatterNetworkCommissioningClusterServerShutdownCallback);
    }

    /* Commands */
    if (config->feature_map & chip::to_underlying(NetworkCommissioning::Feature::kWiFiNetworkInterface)) {
        command::create_add_or_update_wifi_network(cluster);
    }
    if (config->feature_map & chip::to_underlying(NetworkCommissioning::Feature::kThreadNetworkInterface)) {
        command::create_add_or_update_thread_network(cluster);
    }

    return cluster;
}
} /* network_commissioning */

namespace general_diagnostics {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, GeneralDiagnostics::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, GeneralDiagnostics::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        static const auto plugin_server_init_cb = CALL_ONCE(MatterGeneralDiagnosticsPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        attribute::create_network_interfaces(cluster, NULL, 0, 0);
        attribute::create_reboot_count(cluster, 0);
        attribute::create_up_time(cluster, 0);
        attribute::create_test_event_triggers_enabled(cluster, 0);
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        /* Commands */
        command::create_test_event_trigger(cluster);
        command::create_time_snap_shot(cluster);
        command::create_time_snap_shot_response(cluster);

        cluster::set_init_and_shutdown_callbacks(cluster, ESPMatterGeneralDiagnosticsClusterServerInitCallback,
                                                 ESPMatterGeneralDiagnosticsClusterServerShutdownCallback);
    }

    event::create_boot_reason(cluster);

    return cluster;
}
} /* general_diagnostics */

namespace administrator_commissioning {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, AdministratorCommissioning::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, AdministratorCommissioning::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        static const auto plugin_server_init_cb = CALL_ONCE(MatterAdministratorCommissioningPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        attribute::create_window_status(cluster, 0);
        attribute::create_admin_fabric_index(cluster, 0);
        attribute::create_admin_vendor_id(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        /* Commands */
        command::create_open_commissioning_window(cluster);
        command::create_revoke_commissioning(cluster);

        cluster::set_init_and_shutdown_callbacks(cluster, ESPMatterAdministratorCommissioningClusterServerInitCallback,
                                                 ESPMatterAdministratorCommissioningClusterServerShutdownCallback);
    }

    return cluster;
}
} /* administrator_commissioning */

namespace operational_credentials {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, OperationalCredentials::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, OperationalCredentials::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        static const auto plugin_server_init_cb = CALL_ONCE(MatterOperationalCredentialsPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        attribute::create_nocs(cluster, NULL, 0, 0);
        attribute::create_supported_fabrics(cluster, 0);
        attribute::create_commissioned_fabrics(cluster, 0);
        attribute::create_fabrics(cluster, NULL, 0, 0);
        attribute::create_trusted_root_certificates(cluster, NULL, 0, 0);
        attribute::create_current_fabric_index(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

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
        command::create_set_vid_verification_statement(cluster);
        command::create_sign_vid_verification_request(cluster);
        command::create_sign_vid_verification_response(cluster);
    }

    return cluster;
}
} /* operational_credentials */

namespace group_key_management {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, GroupKeyManagement::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, GroupKeyManagement::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        static const auto plugin_server_init_cb = CALL_ONCE(MatterGroupKeyManagementPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        global::attribute::create_cluster_revision(cluster, 0);
        attribute::create_group_key_map(cluster, NULL, 0, 0);
        attribute::create_group_table(cluster, NULL, 0, 0);
        attribute::create_max_groups_per_fabric(cluster, 0);
        attribute::create_max_group_keys_per_fabric(cluster, 0);

        command::create_key_set_write(cluster);
        command::create_key_set_read(cluster);
        command::create_key_set_remove(cluster);
        command::create_key_set_read_all_indices(cluster);
        command::create_key_set_read_response(cluster);
        command::create_key_set_read_all_indices_response(cluster);
    }

    return cluster;
}
} /* group_key_management */

namespace wifi_network_diagnostics {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, WiFiNetworkDiagnostics::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, WiFiNetworkDiagnostics::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        static const auto plugin_server_init_cb = CALL_ONCE(MatterWiFiNetworkDiagnosticsPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        attribute::create_bssid(cluster, NULL, 0);
        attribute::create_security_type(cluster, nullable<uint8_t>());
        attribute::create_wifi_version(cluster, nullable<uint8_t>());
        attribute::create_channel_number(cluster, nullable<uint16_t>());
        attribute::create_rssi(cluster, nullable<int8_t>());

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);
    }

    return cluster;
}
} /* wifi_network_diagnostics */

namespace thread_network_diagnostics {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, ThreadNetworkDiagnostics::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, ThreadNetworkDiagnostics::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        static const auto plugin_server_init_cb = CALL_ONCE(MatterThreadNetworkDiagnosticsPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

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
        global::attribute::create_cluster_revision(cluster, cluster_revision);
    }

    return cluster;
}
} /* thread_network_diagnostics */

namespace ethernet_network_diagnostics {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, EthernetNetworkDiagnostics::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, EthernetNetworkDiagnostics::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        static const auto plugin_server_init_cb = CALL_ONCE(MatterEthernetNetworkDiagnosticsPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);


        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);
    }

    return cluster;
}
} /* ethernet_network_diagnostics */

namespace time_synchronization {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, TimeSynchronization::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, TimeSynchronization::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        if (config && config->delegate != nullptr) {
            static const auto delegate_init_cb = TimeSynchronizationDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }

        static const auto plugin_server_init_cb = CALL_ONCE(MatterTimeSynchronizationPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);
        attribute::create_utc_time(cluster, nullable<uint64_t>());
        attribute::create_granularity(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        /* Commands */
        command::create_set_utc_time(cluster);
    }

    event::create_time_failure(cluster);

    return cluster;
}
} /* time_synchronization */

namespace unit_localization {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, UnitLocalization::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, UnitLocalization::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);
    }

    return cluster;
}
} /* unit_localization */

namespace bridged_device_basic_information {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, BridgedDeviceBasicInformation::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, BridgedDeviceBasicInformation::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        // There is not PluginServer(Client)InitCallback for this cluster
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);
        attribute::create_unique_id(cluster, NULL, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        if (config) {
            attribute::create_reachable(cluster, config->reachable);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    event::create_reachable_changed(cluster);

    return cluster;
}
} /* bridged_device_basic_information */

namespace power_source {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, PowerSource::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, PowerSource::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));

        static const auto plugin_server_init_cb = CALL_ONCE(MatterPowerSourcePluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /** Attributes not managed internally **/
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_status(cluster, config->status);
        attribute::create_order(cluster, config->order, 0x00, 0xFF);
        attribute::create_description(cluster, config->description, strlen(config->description));
        attribute::create_endpoint_list(cluster, NULL, 0, 0);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, config->feature_flags);

        // check against O.a feature conformance for Wired and Battery
        VALIDATE_FEATURES_EXACT_ONE("PowerSource,Battery", feature::wired::get_id(), feature::battery::get_id());

        /* Features */
        if (has(feature::wired::get_id())) {
            feature::wired::add(cluster, &(config->features.wired));
        }
        if (has(feature::battery::get_id())) {
            feature::battery::add(cluster, &(config->features.battery));
            if (has(feature::rechargeable::get_id())) {
                feature::rechargeable::add(cluster, &(config->features.rechargeable));
            }
            if (has(feature::replaceable::get_id())) {
                feature::replaceable::add(cluster, &(config->features.replaceable));
            }
        }
    }

    return cluster;
}
} /* power_source */

namespace icd_management {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, IcdManagement::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, IcdManagement::Id));
#if CONFIG_ENABLE_ICD_SERVER
    if (flags & CLUSTER_FLAG_SERVER) {
        static const auto plugin_server_init_cb = CALL_ONCE(MatterIcdManagementPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);
        attribute::create_idle_mode_duration(cluster, CONFIG_ICD_IDLE_MODE_INTERVAL_SEC);
        attribute::create_active_mode_duration(cluster, CONFIG_ICD_ACTIVE_MODE_INTERVAL_MS);
        attribute::create_active_mode_threshold(cluster, CONFIG_ICD_ACTIVE_MODE_THRESHOLD_MS);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);
    }
#endif // CONFIG_ENABLE_ICD_SERVER
    return cluster;
}

} /* icd_management */

namespace user_label {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, UserLabel::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, UserLabel::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        static const auto plugin_server_init_cb = CALL_ONCE(MatterUserLabelPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        attribute::create_label_list(cluster, NULL, 0, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);
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
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, FixedLabel::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        static const auto plugin_server_init_cb = CALL_ONCE(MatterFixedLabelPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        attribute::create_label_list(cluster, NULL, 0, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);
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
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, Identify::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        static const auto plugin_server_init_cb = CALL_ONCE(MatterIdentifyPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        set_add_bounds_callback(cluster, identify::add_bounds_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);
        if (config) {
            attribute::create_identify_time(cluster, config->identify_time);
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
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, Groups::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        static const auto plugin_server_init_cb = CALL_ONCE(MatterGroupsPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, static_cast<uint32_t>(Groups::Feature::kGroupNames));

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);
        if (config) {
            attribute::create_group_name_support(cluster, chip::to_underlying(Groups::NameSupportBitmap::kGroupNames));
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

namespace scenes_management {
const function_generic_t function_list[] = {
    (function_generic_t)emberAfScenesManagementClusterServerInitCallback,
    (function_generic_t)MatterScenesManagementClusterServerShutdownCallback,
};
const int function_flags = CLUSTER_FLAG_INIT_FUNCTION | CLUSTER_FLAG_SHUTDOWN_FUNCTION;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, ScenesManagement::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, ScenesManagement::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        static const auto plugin_server_init_cb = CALL_ONCE(MatterScenesManagementPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        set_add_bounds_callback(cluster, scenes_management::add_bounds_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);
        if (config) {
            attribute::create_scene_table_size(cluster, config->scene_table_size);
            attribute::create_fabric_scene_info(cluster, NULL, 0, 0);
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
} /* scenes_management */

namespace on_off {
const function_generic_t function_list[] = {
    (function_generic_t)emberAfOnOffClusterServerInitCallback,
    (function_generic_t)MatterOnOffClusterServerShutdownCallback,
};
const int function_flags = CLUSTER_FLAG_INIT_FUNCTION | CLUSTER_FLAG_SHUTDOWN_FUNCTION;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, OnOff::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, OnOff::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        static const auto plugin_server_init_cb = CALL_ONCE(MatterOnOffPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        set_add_bounds_callback(cluster, on_off::add_bounds_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);
        if (config) {
            attribute::create_on_off(cluster, config->on_off);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    /* Commands */
    command::create_off(cluster);

    return cluster;
}
} /* on_off */

namespace level_control {
const function_generic_t function_list[] = {
    (function_generic_t)emberAfLevelControlClusterServerInitCallback,
    (function_generic_t)MatterLevelControlClusterServerShutdownCallback,
};
const int function_flags = CLUSTER_FLAG_INIT_FUNCTION | CLUSTER_FLAG_SHUTDOWN_FUNCTION;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, LevelControl::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, LevelControl::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        static const auto plugin_server_init_cb = CALL_ONCE(MatterLevelControlPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        set_add_bounds_callback(cluster, level_control::add_bounds_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);
        if (config) {
            attribute::create_current_level(cluster, config->current_level);
            attribute::create_on_level(cluster, config->on_level);
            attribute::create_options(cluster, config->options);
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

    return cluster;
}
} /* level_control */

namespace color_control {
const function_generic_t function_list[] = {
    (function_generic_t)emberAfColorControlClusterServerInitCallback,
    (function_generic_t)MatterColorControlClusterServerShutdownCallback,
};
const int function_flags = CLUSTER_FLAG_INIT_FUNCTION | CLUSTER_FLAG_SHUTDOWN_FUNCTION;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, ColorControl::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, ColorControl::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        static const auto plugin_server_init_cb = CALL_ONCE(MatterColorControlPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        set_add_bounds_callback(cluster, color_control::add_bounds_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);
        if (config) {
            attribute::create_color_mode(cluster, config->color_mode);
            attribute::create_color_control_options(cluster, config->color_control_options);
            attribute::create_enhanced_color_mode(cluster, config->enhanced_color_mode);
            attribute::create_color_capabilities(cluster, config->color_capabilities);
            attribute::create_number_of_primaries(cluster, config->number_of_primaries);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }

        /* Attributes managed internally */
        for (uint8_t idx = 1; idx <= config->number_of_primaries.value_or(0); ++idx) {
            attribute::create_primary_n_x(cluster, 0, idx);
            attribute::create_primary_n_y(cluster, 0, idx);
            attribute::create_primary_n_intensity(cluster, nullable<uint8_t>(), idx);
        }
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
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, FanControl::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        if (config && config->delegate != nullptr) {
            static const auto delegate_init_cb = FanControlDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        static const auto plugin_server_init_cb = CALL_ONCE(MatterFanControlPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        set_add_bounds_callback(cluster, fan_control::add_bounds_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);
        if (config) {
            attribute::create_fan_mode(cluster, config->fan_mode);
            attribute::create_fan_mode_sequence(cluster, config->fan_mode_sequence);
            attribute::create_percent_setting(cluster, config->percent_setting);
            attribute::create_percent_current(cluster, config->percent_current);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
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
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, Thermostat::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));

        if (config -> delegate != nullptr) {
            static const auto delegate_init_cb = ThermostatDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        static const auto plugin_server_init_cb = CALL_ONCE(MatterThermostatPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        set_add_bounds_callback(cluster, thermostat::add_bounds_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);
        attribute::create_local_temperature(cluster, config->local_temperature);
        attribute::create_control_sequence_of_operation(cluster, config->control_sequence_of_operation);
        attribute::create_system_mode(cluster, config->system_mode);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, config->feature_flags);

        /* Features */
        if (has(feature::auto_mode::get_id())) {
            feature::auto_mode::add(cluster, &(config->features.auto_mode));
            feature::heating::add(cluster, &(config->features.heating));
            feature::cooling::add(cluster, &(config->features.cooling));
        } else {
            // HEAT and COOL must be checked against O.a+
            VALIDATE_FEATURES_AT_LEAST_ONE("Heat,Cool",
                                      feature::heating::get_id(), feature::cooling::get_id());

            if(has(feature::heating::get_id())) {
                feature::heating::add(cluster, &(config->features.heating));
            }
            if (has(feature::cooling::get_id())) {
                feature::cooling::add(cluster, &(config->features.cooling));
            }
        }

        if (has(feature::occupancy::get_id())) {
            feature::occupancy::add(cluster, &(config->features.occupancy));
        }
        if (has(feature::setback::get_id())) {
            feature::setback::add(cluster, &(config->features.setback));
        }
        if (has(feature::local_temperature_not_exposed::get_id())) {
            feature::local_temperature_not_exposed::add(cluster, &(config->features.local_temperature_not_exposed));
        }
        if (has(feature::matter_schedule_configuration::get_id())) {
            feature::matter_schedule_configuration::add(cluster, &(config->features.matter_schedule_configuration));
        }
        if (has(feature::presets::get_id())) {
            feature::presets::add(cluster);
        }
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }

    /* Commands */
    command::create_setpoint_raise_lower(cluster);

    // matter_schedule_configuration and presets contains the attributes with T i.e. atomic quality
    // hence, we need to create the atomic request and response commands
    if (has(feature::matter_schedule_configuration::get_id()) || has(feature::presets::get_id())) {
        command::create_atomic_request(cluster);
        command::create_atomic_response(cluster);
    }

    return cluster;
}
} /* thermostat */

namespace thermostat_user_interface_configuration {
const function_generic_t function_list[] = {
    (function_generic_t)MatterThermostatClusterServerPreAttributeChangedCallback,
};
const int function_flags = CLUSTER_FLAG_PRE_ATTRIBUTE_CHANGED_FUNCTION;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, ThermostatUserInterfaceConfiguration::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, ThermostatUserInterfaceConfiguration::Id));

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        set_add_bounds_callback(cluster, thermostat_user_interface_configuration::add_bounds_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);
        if (config) {
            attribute::create_temperature_display_mode(cluster, config->temperature_display_mode);
            attribute::create_keypad_lockout(cluster, config->keypad_lockout);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    return cluster;
}
} /* thermostat_user_interface_configuration */

namespace air_quality {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, AirQuality::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, AirQuality::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        attribute::create_air_quality(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }

    return cluster;
}
} /* air_quality */

namespace resource_monitoring {
typedef void (*delegate_init_cb_t)(void *, uint16_t);

template <typename T, delegate_init_cb_t delegate_init_cb = nullptr>
static cluster_t *create(endpoint_t *endpoint, T *config, uint8_t flags, uint32_t cluster_id,
                         uint32_t cluster_revision,
                         const function_generic_t *function_list = nullptr,
                         const int function_flags = CLUSTER_FLAG_NONE)
{
    cluster_t *cluster = cluster::create(endpoint, cluster_id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster, id: 0x%08" PRIX32, cluster_id));

    if (flags & CLUSTER_FLAG_SERVER) {
        if (config && config->delegate != nullptr) {
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        attribute::create_change_indication(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }
    return cluster;
}
} /* resource_monitoring */

namespace hepa_filter_monitoring {
cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    return resource_monitoring::create<config_t, HepaFilterMonitoringDelegateInitCB>(endpoint, config, flags,HepaFilterMonitoring::Id, cluster_revision);
}
} /* hepa_filter_monitoring */

namespace activated_carbon_filter_monitoring {
cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    return resource_monitoring::create<config_t, ActivatedCarbonFilterMonitoringDelegateInitCB>(endpoint, config, flags,ActivatedCarbonFilterMonitoring::Id, cluster_revision);
}
} /* activated_carbon_filter_monitoring */

namespace concentration_measurement {

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags, uint32_t cluster_id)
{
    cluster_t *cluster = cluster::create(endpoint, cluster_id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster, id: 0x%08" PRIX32, cluster_id));

    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));

        add_function_list(cluster, NULL, CLUSTER_FLAG_NONE);

        // For all concentration measurement cluster Attribute Id of measurement medium is 0x09
        // Hence, using the hard coded value
        attribute::create_measurement_medium(cluster, config->measurement_medium);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, config->feature_flags);

        // check against O.a+ feature conformance for Numeric Measurement and Level Indication
        VALIDATE_FEATURES_AT_LEAST_ONE("NumericMeasurement,LevelIndication",
                                      feature::numeric_measurement::get_id(), feature::level_indication::get_id());

        if(has(feature::numeric_measurement::get_id())) {
            feature::numeric_measurement::add(cluster, &(config->features.numeric_measurement));
            if (has(feature::peak_measurement::get_id())) {
                feature::peak_measurement::add(cluster, &(config->features.peak_measurement));
            }
            if (has(feature::average_measurement::get_id())) {
                feature::average_measurement::add(cluster, &(config->features.average_measurement));
            }
        }
        if (has(feature::level_indication::get_id())) {
            feature::level_indication::add(cluster, &(config->features.level_indication));
            if (has(feature::medium_level::get_id())) {
                feature::medium_level::add(cluster);
            }
            if (has(feature::critical_level::get_id())) {
                feature::critical_level::add(cluster);
            }
        }
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }

    return cluster;
}

} // concentration_measurement

namespace carbon_monoxide_concentration_measurement {

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    return concentration_measurement::create(endpoint, config, flags, CarbonMonoxideConcentrationMeasurement::Id);
}

} /* carbon_monoxide_concentration_measurement */

namespace carbon_dioxide_concentration_measurement {

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    return concentration_measurement::create(endpoint, config, flags, CarbonDioxideConcentrationMeasurement::Id);
}

} /* carbon_dioxide_concentration_measurement */

namespace nitrogen_dioxide_concentration_measurement {

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    return concentration_measurement::create(endpoint, config, flags, NitrogenDioxideConcentrationMeasurement::Id);
}

} /* nitrogen_dioxide_concentration_measurement */

namespace ozone_concentration_measurement {

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    return concentration_measurement::create(endpoint, config, flags, OzoneConcentrationMeasurement::Id);
}

} /* ozone_concentration_measurement */

namespace formaldehyde_concentration_measurement {

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    return concentration_measurement::create(endpoint, config, flags, FormaldehydeConcentrationMeasurement::Id);
}

} /* formaldehyde_concentration_measurement */

namespace pm1_concentration_measurement {

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    return concentration_measurement::create(endpoint, config, flags, Pm1ConcentrationMeasurement::Id);
}

} /* pm1_concentration_measurement */

namespace pm25_concentration_measurement {

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    return concentration_measurement::create(endpoint, config, flags, Pm25ConcentrationMeasurement::Id);
}

} /* pm25_concentration_measurement */

namespace pm10_concentration_measurement {

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    return concentration_measurement::create(endpoint, config, flags, Pm10ConcentrationMeasurement::Id);
}

} /* pm10_concentration_measurement */

namespace radon_concentration_measurement {

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    return concentration_measurement::create(endpoint, config, flags, RadonConcentrationMeasurement::Id);
}

} /* radon_concentration_measurement */

namespace total_volatile_organic_compounds_concentration_measurement {

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    return concentration_measurement::create(endpoint, config, flags, TotalVolatileOrganicCompoundsConcentrationMeasurement::Id);
}
} /* total_volatile_organic_compounds_concentration_measurement */

namespace operational_state {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, OperationalState::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, OperationalState::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        if (config && config->delegate != nullptr) {
            static const auto delegate_init_cb = OperationalStateDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        attribute::create_phase_list(cluster, NULL, 0, 0);
        attribute::create_current_phase(cluster, 0);
        attribute::create_operational_state_list(cluster, NULL, 0, 0);
        attribute::create_operational_state(cluster, 0);
        attribute::create_operational_error(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }

    event::create_operational_error(cluster);

    return cluster;
}
} /* operational_state */

namespace laundry_washer_mode {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, LaundryWasherMode::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, LaundryWasherMode::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        if (config && config->delegate != nullptr) {
            static const auto delegate_init_cb = LaundryWasherModeDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);
        if (config) {
            mode_base::attribute::create_current_mode(cluster, config->current_mode);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }

    return cluster;
}
} /* laundry_washer_mode */

namespace laundry_washer_controls {

const function_generic_t function_list[] = {
    (function_generic_t)MatterLaundryWasherControlsClusterServerPreAttributeChangedCallback,
};
const int function_flags = CLUSTER_FLAG_PRE_ATTRIBUTE_CHANGED_FUNCTION;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, LaundryWasherControls::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, LaundryWasherControls::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));

        if (config -> delegate != nullptr) {
            static const auto delegate_init_cb = LaundryWasherControlsDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, config->feature_flags);

        // check against O.a+ feature conformance for Spin and Rinse
        VALIDATE_FEATURES_AT_LEAST_ONE("Spin,Rinse",
                                      feature::spin::get_id(), feature::rinse::get_id());

        if (has(feature::spin::get_id())) {
            feature::spin::add(cluster, &(config->features.spin));
        }
        if (has(feature::rinse::get_id())) {
            feature::rinse::add(cluster, &(config->features.rinse));
        }
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }

    return cluster;
}
} /* laundry_washer_controls */

namespace laundry_dryer_controls {

const function_generic_t function_list[] = {
    (function_generic_t)MatterLaundryDryerControlsClusterServerPreAttributeChangedCallback,
};
const int function_flags = CLUSTER_FLAG_PRE_ATTRIBUTE_CHANGED_FUNCTION;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, LaundryDryerControls::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, LaundryDryerControls::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        if (config && config->delegate != nullptr) {
            static const auto delegate_init_cb = LaundryDryerControlsDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        attribute::create_supported_dryness_levels(cluster, NULL, 0, 0);
        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);
        if (config) {
            attribute::create_selected_dryness_level(cluster, config->selected_dryness_level);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }

    return cluster;
}
} /* laundry_dryer_controls */

namespace dish_washer_mode {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, DishwasherMode::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, DishwasherMode::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        add_function_list(cluster, function_list, function_flags);

        if (config && config->delegate != nullptr) {
            static const auto delegate_init_cb = DishWasherModeDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);
        if (config) {
            mode_base::attribute::create_current_mode(cluster, config->current_mode);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }

    return cluster;
}
} /* dish_washer_mode */

namespace dish_washer_alarm {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, DishwasherAlarm::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, DishwasherAlarm::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        if (config && config->delegate != nullptr) {
            static const auto delegate_init_cb = DishwasherAlarmDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }

    return cluster;
}
} /* dish_washer_alarm */

namespace smoke_co_alarm {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, SmokeCoAlarm::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, SmokeCoAlarm::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));

        add_function_list(cluster, function_list, function_flags);

        attribute::create_expressed_state(cluster, 0);
        attribute::create_battery_alert(cluster, 0);
        attribute::create_test_in_progress(cluster, false);
        attribute::create_hardware_fault_alert(cluster, false);
        attribute::create_end_of_service_alert(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, config->feature_flags);

        // check against O.a+ feature conformance for Smoke Alarm and CO Alarm
        VALIDATE_FEATURES_AT_LEAST_ONE("Smoke Alarm,CO Alarm",
                                      feature::smoke_alarm::get_id(), feature::co_alarm::get_id());

        if (has(feature::smoke_alarm::get_id())) {
            feature::smoke_alarm::add(cluster);
        }
        if (has(feature::co_alarm::get_id())) {
            feature::co_alarm::add(cluster);
        }
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }

    event::create_low_battery(cluster);
    event::create_hardware_fault(cluster);
    event::create_end_of_service(cluster);
    event::create_self_test_complete(cluster);
    event::create_all_clear(cluster);

    return cluster;
}
} /* smoke_co_alarm */

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
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, DoorLock::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        if (config && config->delegate != nullptr) {
            static const auto delegate_init_cb = DoorLockDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        static const auto plugin_server_init_cb = CALL_ONCE(MatterDoorLockPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);
        if (config) {
            attribute::create_lock_state(cluster, config->lock_state);
            attribute::create_lock_type(cluster, config->lock_type);
            attribute::create_actuator_enabled(cluster, config->actuator_enabled);
            attribute::create_operating_mode(cluster, config->operating_mode, 0x0, 0x4);
            attribute::create_supported_operating_modes(cluster, config->supported_operating_modes);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }

    event::create_door_lock_alarm(cluster);
    event::create_lock_operation(cluster);
    event::create_lock_operation_error(cluster);

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

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, WindowCovering::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, WindowCovering::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));

        static const auto plugin_server_init_cb = CALL_ONCE(MatterWindowCoveringPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        if (config->delegate != nullptr) {
            static const auto delegate_init_cb = WindowCoveringDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        add_function_list(cluster, function_list, function_flags);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_type(cluster, config->type);
        attribute::create_config_status(cluster, config->config_status);
        attribute::create_operational_status(cluster, config->operational_status);
        attribute::create_end_product_type(cluster, config->end_product_type);
        attribute::create_mode(cluster, config->mode);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, config->feature_flags);

        // check against O.a+ feature conformance for Lift and Tilt
        VALIDATE_FEATURES_AT_LEAST_ONE("Lift,Tilt",
                                      feature::lift::get_id(), feature::tilt::get_id());

        if (has(feature::lift::get_id())) {
            feature::lift::add(cluster);
            // optional if lift is supported
            if (has(feature::position_aware_lift::get_id())) {
                feature::position_aware_lift::add(cluster, &(config->features.position_aware_lift));
            }
        }
        if (has(feature::tilt::get_id())) {
            feature::tilt::add(cluster);
            // optional if tilt is supported
            if (has(feature::position_aware_tilt::get_id())) {
                feature::position_aware_tilt::add(cluster, &(config->features.position_aware_tilt));
            }
        }
        // optional if absolute position is supported
        if (has(feature::absolute_position::get_id())) {
            feature::absolute_position::add(cluster, &(config->features.absolute_position));
        }
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }

    /* Commands */
    command::create_up_or_open(cluster);
    command::create_down_or_close(cluster);
    command::create_stop_motion(cluster);

    return cluster;
}
} /* window_covering */


namespace switch_cluster {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, Switch::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, Switch::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));

        static const auto plugin_server_init_cb = CALL_ONCE(MatterSwitchPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_number_of_positions(cluster, config->number_of_positions);
        attribute::create_current_position(cluster, config->current_position);
        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, config->feature_flags);

        // check against O.a feature conformance Latching and Momentary Switch
        VALIDATE_FEATURES_EXACT_ONE("Latching Switch,Momentary Switch",
                                   feature::latching_switch::get_id(), feature::momentary_switch::get_id());

        if (has(feature::latching_switch::get_id())) {
            feature::latching_switch::add(cluster);
        }
        if (has(feature::momentary_switch::get_id())) {
            feature::momentary_switch::add(cluster);
        }

        // feature present conditions are checked in the feature add API
        // so we need the return value checks here
        if (has(feature::action_switch::get_id())) {
            VerifyOrReturnValue(feature::action_switch::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
            // add MSM, conformance: AS,[MS & MSR]
            VerifyOrReturnValue(feature::momentary_switch_multi_press::add(cluster, &(config->features.momentary_switch_multi_press)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (has(feature::momentary_switch_release::get_id())) {
            VerifyOrReturnValue(feature::momentary_switch_release::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (has(feature::momentary_switch_long_press::get_id())) {
            VerifyOrReturnValue(feature::momentary_switch_long_press::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (has(feature::momentary_switch_multi_press::get_id())) {
            // momentary_switch_multi_press::add() checks against !AS feature so okay to call it twice even if AS is present
            VerifyOrReturnValue(feature::momentary_switch_multi_press::add(cluster, &(config->features.momentary_switch_multi_press)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
    } // if (flags & CLUSTER_FLAG_SERVER)

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
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
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, TemperatureMeasurement::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        static const auto plugin_server_init_cb = CALL_ONCE(MatterTemperatureMeasurementPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);
        if (config) {
            attribute::create_temperature_measured_value(cluster, config->measured_value);
            attribute::create_temperature_min_measured_value(cluster, config->min_measured_value);
            attribute::create_temperature_max_measured_value(cluster, config->max_measured_value);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
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
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, RelativeHumidityMeasurement::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        static const auto plugin_server_init_cb = CALL_ONCE(MatterRelativeHumidityMeasurementPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);
        if (config) {
            attribute::create_relative_humidity_measured_value(cluster, config->measured_value);
            attribute::create_relative_humidity_min_measured_value(cluster, config->min_measured_value);
            attribute::create_relative_humidity_max_measured_value(cluster, config->max_measured_value);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
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
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, OccupancySensing::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));

        static const auto plugin_server_init_cb = CALL_ONCE(MatterOccupancySensingPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_occupancy(cluster, config->occupancy);
        attribute::create_occupancy_sensor_type(cluster, config->occupancy_sensor_type);
        attribute::create_occupancy_sensor_type_bitmap(cluster, config->occupancy_sensor_type_bitmap);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, config->feature_flags);

        // check against O.a+ feature conformance for occupancy sensing
        VALIDATE_FEATURES_AT_LEAST_ONE("Other,Passive Infrared,Ultrasonic,Physical Contact,Active Infrared,Radar,RF Sensing,Vision",
                                      feature::other::get_id(), feature::passive_infrared::get_id(),
                                      feature::ultrasonic::get_id(), feature::physical_contact::get_id(),
                                      feature::active_infrared::get_id(), feature::radar::get_id(),
                                      feature::rf_sensing::get_id(), feature::vision::get_id());

        if (has(feature::other::get_id())) {
            feature::other::add(cluster);
        }
        if (has(feature::passive_infrared::get_id())) {
            feature::passive_infrared::add(cluster);
        }
        if (has(feature::ultrasonic::get_id())) {
            feature::ultrasonic::add(cluster);
        }
        if (has(feature::physical_contact::get_id())) {
            feature::physical_contact::add(cluster);
        }
        if (has(feature::active_infrared::get_id())) {
            feature::active_infrared::add(cluster);
        }
        if (has(feature::radar::get_id())) {
            feature::radar::add(cluster);
        }
        if (has(feature::rf_sensing::get_id())) {
            feature::rf_sensing::add(cluster);
        }
        if (has(feature::vision::get_id())) {
            feature::vision::add(cluster);
        }
    } // if (flags & CLUSTER_FLAG_SERVER)

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
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
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, BooleanState::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        static const auto plugin_server_init_cb = CALL_ONCE(MatterBooleanStatePluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);
        if (config) {
            attribute::create_state_value(cluster, config->state_value);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }

    return cluster;
}
} /* boolean_state */

namespace boolean_state_configuration {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, BooleanStateConfiguration::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, BooleanStateConfiguration::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        if (config && config->delegate != nullptr) {
            static const auto delegate_init_cb = BooleanStateConfigurationDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        static const auto plugin_server_init_cb = CALL_ONCE(MatterBooleanStateConfigurationPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }

    return cluster;
}
} /* boolean_state_configuration */

namespace localization_configuration {
const function_generic_t function_list[] = {
    (function_generic_t)emberAfLocalizationConfigurationClusterServerInitCallback,
    (function_generic_t)MatterLocalizationConfigurationClusterServerPreAttributeChangedCallback};
const int function_flags = CLUSTER_FLAG_INIT_FUNCTION | CLUSTER_FLAG_PRE_ATTRIBUTE_CHANGED_FUNCTION;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, LocalizationConfiguration::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, LocalizationConfiguration::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        static const auto plugin_server_init_cb = CALL_ONCE(MatterLocalizationConfigurationPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes not managed internally */
        global::attribute::create_feature_map(cluster, 0);

        global::attribute::create_cluster_revision(cluster, cluster_revision);
        if (config) {
            /* Attributes not managed internally */
            attribute::create_active_locale(cluster, config->active_locale, strlen(config->active_locale));

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
    (function_generic_t)MatterTimeFormatLocalizationClusterServerPreAttributeChangedCallback};
const int function_flags = CLUSTER_FLAG_INIT_FUNCTION | CLUSTER_FLAG_PRE_ATTRIBUTE_CHANGED_FUNCTION;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, TimeFormatLocalization::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, TimeFormatLocalization::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        static const auto plugin_server_init_cb = CALL_ONCE(MatterTimeFormatLocalizationPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes not managed internally */
        global::attribute::create_feature_map(cluster, 0);

        if (config) {
            /* Attributes not managed internally */
            global::attribute::create_cluster_revision(cluster, cluster_revision);

            /* Attributes managed internally */
            attribute::create_hour_format(cluster, config->hour_format);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
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
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, IlluminanceMeasurement::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        static const auto plugin_server_init_cb = CALL_ONCE(MatterIlluminanceMeasurementPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /** Attributes not managed internally **/
        global::attribute::create_cluster_revision(cluster, cluster_revision);
        if (config) {
            attribute::create_illuminance_measured_value(cluster, config->illuminance_measured_value, 0x0000, 0xFFFF);
            attribute::create_illuminance_min_measured_value(cluster, config->illuminance_min_measured_value, 0x0001,
                                                             0xFFFD);
            attribute::create_illuminance_max_measured_value(cluster, config->illuminance_max_measured_value, 0x0002,
                                                             0xFFFE);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
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
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, PressureMeasurement::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        static const auto plugin_server_init_cb = CALL_ONCE(MatterPressureMeasurementPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /** Attributes not managed internally **/
        global::attribute::create_cluster_revision(cluster, cluster_revision);
        if (config) {
            attribute::create_pressure_measured_value(cluster, config->pressure_measured_value);
            attribute::create_pressure_min_measured_value(cluster, config->pressure_min_measured_value);
            attribute::create_pressure_max_measured_value(cluster, config->pressure_max_measured_value);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
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
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, FlowMeasurement::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        static const auto plugin_server_init_cb = CALL_ONCE(MatterFlowMeasurementPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /** Attributes not managed internally **/
        global::attribute::create_cluster_revision(cluster, cluster_revision);
        if (config) {
            attribute::create_flow_measured_value(cluster, config->flow_measured_value);
            attribute::create_flow_min_measured_value(cluster, config->flow_min_measured_value);
            attribute::create_flow_max_measured_value(cluster, config->flow_max_measured_value);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
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
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, PumpConfigurationAndControl::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));

        static const auto plugin_server_init_cb = CALL_ONCE(MatterPumpConfigurationAndControlPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /** Attributes not managed internally **/
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_max_pressure(cluster, config->max_pressure);
        attribute::create_max_speed(cluster, config->max_speed);
        attribute::create_max_flow(cluster, config->max_flow);
        attribute::create_effective_operation_mode(cluster, config->effective_operation_mode);
        attribute::create_effective_control_mode(cluster, config->effective_control_mode);
        attribute::create_capacity(cluster, config->capacity);
        attribute::create_operation_mode(cluster, config->operation_mode);
        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, config->feature_flags);

        // check against O.a+ feature conformance for pump configuration and control
        VALIDATE_FEATURES_AT_LEAST_ONE("ConstantPressure,CompensatedPressure,ConstantFlow,ConstantSpeed,ConstantTemperature",
                                      feature::constant_pressure::get_id(), feature::compensated_pressure::get_id(),
                                      feature::constant_flow::get_id(), feature::constant_speed::get_id(),
                                      feature::constant_temperature::get_id());

        if (has(feature::constant_pressure::get_id())) {
            feature::constant_pressure::add(cluster, &config->features.constant_pressure);
        }
        if (has(feature::compensated_pressure::get_id())) {
            feature::compensated_pressure::add(cluster, &config->features.compensated_pressure);
        }
        if (has(feature::constant_flow::get_id())) {
            feature::constant_flow::add(cluster, &config->features.constant_flow);
        }
        if (has(feature::constant_speed::get_id())) {
            feature::constant_speed::add(cluster, &config->features.constant_speed);
        }
        if (has(feature::constant_temperature::get_id())) {
            feature::constant_temperature::add(cluster, &config->features.constant_temperature);
        }
        // add optional features
        if (has(feature::automatic::get_id())) {
            feature::automatic::add(cluster);
        }
        if (has(feature::local_operation::get_id())) {
            feature::local_operation::add(cluster);
        }
    } // if (flags & CLUSTER_FLAG_SERVER)

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }

    return cluster;
}
} /* pump_configuration_and_control */

namespace mode_select {
const function_generic_t function_list[] = {
    (function_generic_t)emberAfModeSelectClusterServerInitCallback,
    (function_generic_t)MatterModeSelectClusterServerPreAttributeChangedCallback,
};
const int function_flags = CLUSTER_FLAG_INIT_FUNCTION | CLUSTER_FLAG_PRE_ATTRIBUTE_CHANGED_FUNCTION;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, ModeSelect::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, ModeSelect::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        if (config && config->delegate != nullptr) {
            static const auto delegate_init_cb = ModeSelectDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        static const auto plugin_server_init_cb = CALL_ONCE(MatterModeSelectPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        attribute::create_supported_modes(cluster, NULL, 0, 0);
        /** Attributes not managed internally **/
        global::attribute::create_cluster_revision(cluster, cluster_revision);
        if (config) {
            attribute::create_mode_select_description(cluster, config->mode_select_description, strlen(config->mode_select_description));
            attribute::create_standard_namespace(cluster, config->standard_namespace);
            attribute::create_current_mode(cluster, config->current_mode);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }
    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }

    /* Commands */
    command::create_change_to_mode(cluster);

    return cluster;
}
} /* mode_select */

namespace diagnostic_logs {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, DiagnosticLogs::Id, flags);

    if (flags & CLUSTER_FLAG_SERVER) {
        if (config && config->delegate != nullptr) {
            static const auto delegate_init_cb = DiagnosticLogsDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        static const auto plugin_server_init_cb = CALL_ONCE(MatterDiagnosticLogsPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }

    /* commands */
    command::create_retrieve_logs_request(cluster);
    command::create_retrieve_logs_response(cluster);

    return cluster;
}
} /* diagnostic_logs */

namespace software_diagnostics {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, SoftwareDiagnostics::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, SoftwareDiagnostics::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        static const auto plugin_server_init_cb = CALL_ONCE(MatterSoftwareDiagnosticsPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);
        cluster::set_init_and_shutdown_callbacks(cluster, ESPMatterSoftwareDiagnosticsClusterServerInitCallback,
                                                 ESPMatterSoftwareDiagnosticsClusterServerShutdownCallback);
    }

    return cluster;
}
} /* software_diagnostics */

namespace temperature_control {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, TemperatureControl::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, TemperatureControl::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));

        static const auto plugin_server_init_cb = CALL_ONCE(MatterTemperatureControlPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, config->feature_flags);

        // check against O.a feature conformance for TN, TL
        VALIDATE_FEATURES_EXACT_ONE("TemperatureNumber,TemperatureLevel",
                                   feature::temperature_number::get_id(), feature::temperature_level::get_id());

        if (has(feature::temperature_number::get_id())) {
            feature::temperature_number::add(cluster, &(config->features.temperature_number));
            if (has(feature::temperature_step::get_id())) {
                feature::temperature_step::add(cluster, &(config->features.temperature_step));
            }
        }
        if (has(feature::temperature_level::get_id())) {
            feature::temperature_level::add(cluster, &(config->features.temperature_level));
        }
    } // if (flags & CLUSTER_FLAG_SERVER)

    /* Commands */
    command::create_set_temperature(cluster);

    return cluster;
}
} /* temperature_control */

namespace refrigerator_alarm {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, RefrigeratorAlarm::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, RefrigeratorAlarm::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        static const auto plugin_server_init_cb = CALL_ONCE(MatterRefrigeratorAlarmPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);
        if (config) {
            attribute::create_mask(cluster, config->mask);
            attribute::create_state(cluster, config->state);
            attribute::create_supported(cluster, config->supported);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    return cluster;
}
} /* refrigerator_alarm */

namespace refrigerator_and_tcc_mode {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, RefrigeratorAndTemperatureControlledCabinetMode::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, RefrigeratorAndTemperatureControlledCabinetMode::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        if (config && config->delegate != nullptr) {
            static const auto delegate_init_cb = RefrigeratorAndTCCModeDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

	    mode_base::attribute::create_supported_modes(cluster, NULL, 0, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);
        if (config) {
            mode_base::attribute::create_current_mode(cluster, config->current_mode);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    /* Commands */
    mode_base::command::create_change_to_mode(cluster);
    mode_base::command::create_change_to_mode_response(cluster);

    return cluster;
}
} /* refrigerator_and_tcc_mode */

namespace rvc_run_mode {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, RvcRunMode::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, RvcRunMode::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        if (config && config->delegate != nullptr) {
            static const auto delegate_init_cb = RvcRunModeDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        mode_base::attribute::create_supported_modes(cluster, NULL, 0, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);
        if (config) {
            mode_base::attribute::create_current_mode(cluster, config->current_mode);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    /* Commands */
    mode_base::command::create_change_to_mode(cluster);
    mode_base::command::create_change_to_mode_response(cluster);

    return cluster;
}
} /* rvc_run_mode */

namespace rvc_clean_mode {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, RvcCleanMode::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, RvcCleanMode::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        if (config && config->delegate != nullptr) {
            static const auto delegate_init_cb = RvcCleanModeDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);
        mode_base::attribute::create_supported_modes(cluster, NULL, 0, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);
        if (config) {
            mode_base::attribute::create_current_mode(cluster, config->current_mode);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    /* Commands */
    mode_base::command::create_change_to_mode(cluster);
    mode_base::command::create_change_to_mode_response(cluster);

    return cluster;
}
} /* rvc_clean_mode */

namespace microwave_oven_mode {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, MicrowaveOvenMode::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, MicrowaveOvenMode::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        if (config && config->delegate != nullptr) {
            static const auto delegate_init_cb = MicrowaveOvenModeDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);
        mode_base::attribute::create_supported_modes(cluster, NULL, 0, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);
        if (config) {
            mode_base::attribute::create_current_mode(cluster, config->current_mode);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    return cluster;
}
} /* microwave_oven_mode */

namespace microwave_oven_control {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, MicrowaveOvenControl::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, MicrowaveOvenControl::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));

        if (config -> delegate != nullptr) {
            static const auto delegate_init_cb = MicrowaveOvenControlDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        microwave_oven_control::attribute::create_cook_time(cluster, 0);
        microwave_oven_control::attribute::create_max_cook_time(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, config->feature_flags);

        // check against O.a feature conformance for microwave oven control
        VALIDATE_FEATURES_EXACT_ONE("PowerAsNumber,PowerInWatts",
                                   feature::power_as_number::get_id(), feature::power_in_watts::get_id());

        if (has(feature::power_as_number::get_id())) {
            feature::power_as_number::add(cluster);

            // power number limits optionally depends on power as number
            if (has(feature::power_number_limits::get_id())) {
                feature::power_number_limits::add(cluster);
            }
        }
        if (has(feature::power_in_watts::get_id())) {
            feature::power_in_watts::add(cluster);
        }
    } // if (flags & CLUSTER_FLAG_SERVER)

    /* Commands */
    command::create_set_cooking_parameters(cluster);

    return cluster;
}
} /* microwave_oven_control */

namespace rvc_operational_state {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, RvcOperationalState::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, RvcOperationalState::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        operational_state::attribute::create_phase_list(cluster, NULL, 0, 0);
        operational_state::attribute::create_current_phase(cluster, 0);
        operational_state::attribute::create_operational_state_list(cluster, NULL, 0, 0);
        operational_state::attribute::create_operational_state(cluster, 0);
        operational_state::attribute::create_operational_error(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }

    return cluster;
}
} /* rvc_operational_state */

namespace keypad_input {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, KeypadInput::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, KeypadInput::Id));

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        if (config && config->delegate != nullptr) {
            static const auto delegate_init_cb = KeypadInputDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);


        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);
    }

    /* Commands */
    command::create_send_key(cluster);
    command::create_send_key_response(cluster);

    return cluster;
}
} /* keypad_input */

namespace power_topology {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, PowerTopology::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, PowerTopology::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));

        if (config -> delegate != nullptr) {
            static const auto delegate_init_cb = PowerTopologyDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        add_function_list(cluster, function_list, function_flags);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, config->feature_flags);

        // check against O.a feature conformance for power topology
        VALIDATE_FEATURES_EXACT_ONE("NodeTopology,TreeTopology,SetTopology",
                                   feature::node_topology::get_id(), feature::tree_topology::get_id(),
                                   feature::set_topology::get_id());

        if (has(feature::node_topology::get_id())) {
            feature::node_topology::add(cluster);
        }
        if (has(feature::tree_topology::get_id())) {
            feature::tree_topology::add(cluster);
        }
        if (has(feature::set_topology::get_id())) {
            feature::set_topology::add(cluster);
            if (has(feature::dynamic_power_flow::get_id())) {
                feature::dynamic_power_flow::add(cluster);
            }
        }
    } // if (flags & CLUSTER_FLAG_SERVER)

    return cluster;
}
} /* power_topology */

namespace electrical_power_measurement {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, ElectricalPowerMeasurement::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, ElectricalPowerMeasurement::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));

        if (config -> delegate != nullptr) {
            static const auto delegate_init_cb = ElectricalPowerMeasurementDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        add_function_list(cluster, function_list, function_flags);

        attribute::create_power_mode(cluster, 0);
        attribute::create_number_of_measurement_types(cluster, 0);
        attribute::create_accuracy(cluster, NULL, 0, 0);
        attribute::create_active_power(cluster, 0);
        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, config->feature_flags);

        // check against O.a+ feature conformance for electrical power measurement
        VALIDATE_FEATURES_AT_LEAST_ONE("Direct Current,Alternating Current",
                                      feature::direct_current::get_id(), feature::alternating_current::get_id());

        if (has(feature::direct_current::get_id())) {
            feature::direct_current::add(cluster);
        }
        if (has(feature::alternating_current::get_id())) {
            feature::alternating_current::add(cluster);

            // polyphase power, harmonics, and power quality optionally depends on alternating current
            if (has(feature::polyphase_power::get_id())) {
                feature::polyphase_power::add(cluster);
            }
            if (has(feature::harmonics::get_id())) {
                feature::harmonics::add(cluster);
            }
            if (has(feature::power_quality::get_id())) {
                feature::power_quality::add(cluster);
            }
        }
    } // if (flags & CLUSTER_FLAG_SERVER)

    return cluster;
}
} /* electrical_power_measurement */

namespace electrical_energy_measurement {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, ElectricalEnergyMeasurement::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, ElectricalEnergyMeasurement::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));

        add_function_list(cluster, function_list, function_flags);

        attribute::create_accuracy(cluster, NULL, 0, 0);
        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, config->feature_flags);

        // check against O.a+ feature conformance for electrical energy measurement
        VALIDATE_FEATURES_AT_LEAST_ONE("ImportedEnergy,ExportedEnergy",
                                      feature::imported_energy::get_id(), feature::exported_energy::get_id());
        VALIDATE_FEATURES_AT_LEAST_ONE("CumulativeEnergy,PeriodicEnergy",
                                      feature::cumulative_energy::get_id(), feature::periodic_energy::get_id());

        if (has(feature::imported_energy::get_id())) {
            feature::imported_energy::add(cluster);
        }
        if (has(feature::exported_energy::get_id())) {
            feature::exported_energy::add(cluster);
        }
        if (has(feature::cumulative_energy::get_id())) {
            feature::cumulative_energy::add(cluster);
        }
        if (has(feature::periodic_energy::get_id())) {
            feature::periodic_energy::add(cluster);
        }
    } // if (flags & CLUSTER_FLAG_SERVER)

    return cluster;
}
} /* electrical_energy_measurement */

namespace energy_evse_mode {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, EnergyEvseMode::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, EnergyEvseMode::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        if (config && config->delegate != nullptr) {
            static const auto delegate_init_cb = EnergyEvseModeDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);
        mode_base::attribute::create_supported_modes(cluster, NULL, 0, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);
        if (config) {
            mode_base::attribute::create_current_mode(cluster, config->current_mode);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    /* Commands */
    mode_base::command::create_change_to_mode(cluster);
    mode_base::command::create_change_to_mode_response(cluster);

    return cluster;
}
} /* energy_evse_mode */

namespace energy_evse {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, EnergyEvse::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, EnergyEvse::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        if (config && config->delegate != nullptr) {
            static const auto delegate_init_cb = EnergyEvseDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);
        attribute::create_state(cluster, 0);
        attribute::create_supply_state(cluster, 0);
        attribute::create_fault_state(cluster, 0);
        attribute::create_charging_enabled_until(cluster, 0);
        attribute::create_circuit_capacity(cluster, 0);
        attribute::create_minimum_charge_current(cluster, 0);
        attribute::create_maximum_charge_current(cluster, 0);
        attribute::create_session_id(cluster, 0);
        attribute::create_session_duration(cluster, 0);
        attribute::create_session_energy_charged(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);
    }
    feature::charging_preferences::add(cluster);

    /* Commands */
    command::create_disable(cluster);
    command::create_enable_charging(cluster);

    return cluster;
}
} /* energy_evse */

namespace valve_configuration_and_control {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, ValveConfigurationAndControl::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, ValveConfigurationAndControl::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        if (config && config->delegate != nullptr) {
            static const auto delegate_init_cb = ValveConfigurationAndControlDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        static const auto plugin_server_init_cb = CALL_ONCE(MatterValveConfigurationAndControlPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);
        attribute::create_remaining_duration(cluster, 0);

        /** Attributes not managed internally **/
        global::attribute::create_cluster_revision(cluster, cluster_revision);
        if (config) {
            attribute::create_open_duration(cluster, config->open_duration);
            attribute::create_default_open_duration(cluster, config->default_open_duration);
            attribute::create_current_state(cluster, config->current_state);
            attribute::create_target_state(cluster, config->target_state);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }

    /* Commands */
    command::create_open(cluster);
    command::create_close(cluster);

    return cluster;
}
} /* valve_configuration_and_control */

namespace device_energy_management {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, DeviceEnergyManagement::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, DeviceEnergyManagement::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        if (config && config->delegate != nullptr) {
            static const auto delegate_init_cb = DeviceEnergyManagementDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        static const auto plugin_server_init_cb = CALL_ONCE(MatterDeviceEnergyManagementPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes should managed by application */
        attribute::create_esa_type(cluster, 0);
        attribute::create_esa_can_generate(cluster, 0);
        attribute::create_esa_state(cluster, 0);
        attribute::create_abs_min_power(cluster, 0);
        attribute::create_abs_max_power(cluster, 0);
        /** Attributes not managed internally **/
        global::attribute::create_cluster_revision(cluster, cluster_revision);
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }

    return cluster;
}
} /* device_energy_management */

namespace device_energy_management_mode {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, DeviceEnergyManagementMode::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, DeviceEnergyManagementMode::Id));

    if (flags & CLUSTER_FLAG_SERVER) {
        if (config && config->delegate != nullptr) {
            static const auto delegate_init_cb = DeviceEnergyManagementModeDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);
        mode_base::attribute::create_supported_modes(cluster, NULL, 0, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);
        if (config) {
            mode_base::attribute::create_current_mode(cluster, config->current_mode);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    /* Commands */
    mode_base::command::create_change_to_mode_response(cluster);
    mode_base::command::create_change_to_mode(cluster);

    return cluster;
}
} /* device_energy_management_mode */

namespace application_basic {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, ApplicationBasic::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, ApplicationBasic::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        if (config && config->delegate != nullptr) {
            static const auto delegate_init_cb = ApplicationBasicDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        static const auto plugin_server_init_cb = CALL_ONCE(MatterApplicationBasicPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes should managed by application */
        attribute::create_application_name(cluster, NULL, 0);
        attribute::create_application(cluster, NULL, 0 , 0);
        attribute::create_status(cluster, 0);
        attribute::create_application_version(cluster, NULL, 0);
        attribute::create_allowed_vendor_list(cluster, NULL, 0 , 0);
        /** Attributes not managed internally **/
        global::attribute::create_cluster_revision(cluster, cluster_revision);
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }

    return cluster;
}
} /* application_basic */

namespace thread_border_router_management {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, ThreadBorderRouterManagement::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster"));
    if (flags & CLUSTER_FLAG_SERVER) {
        if (config && config->delegate != nullptr) {
            static const auto delegate_init_cb = ThreadBorderRouterManagementDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        static const auto plugin_server_init_cb = CALL_ONCE(MatterThreadBorderRouterManagementPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);
        attribute::create_border_router_name(cluster, nullptr, 0);
        attribute::create_border_agent_id(cluster, nullptr, 0);
        attribute::create_thread_version(cluster, 0);
        attribute::create_interface_enabled(cluster, false);
        nullable<uint64_t> active_dataset_timestamp, pending_dataset_timestamp;
        attribute::create_active_dataset_timestamp(cluster, active_dataset_timestamp);
        attribute::create_pending_dataset_timestamp(cluster, pending_dataset_timestamp);

        /** Attributes not managed internally **/
        global::attribute::create_cluster_revision(cluster, cluster_revision);
    }

    /* Commands */
    command::create_get_active_dataset_request(cluster);
    command::create_get_pending_dataset_request(cluster);
    command::create_dataset_response(cluster);
    command::create_set_active_dataset_request(cluster);

    return cluster;
}

} /* thread_border_router_management */

namespace wifi_network_management {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, WiFiNetworkManagement::Id, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }
    if (flags & CLUSTER_FLAG_SERVER) {
        static const auto plugin_server_init_cb = CALL_ONCE(MatterWiFiNetworkManagementPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);
        attribute::create_ssid(cluster, nullptr, 0);
        attribute::create_passphrase_surrogate(cluster, 0);

        /** Attributes not managed internally **/
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        cluster::set_init_and_shutdown_callbacks(cluster, ESPMatterWiFiNetworkDiagnosticsClusterServerInitCallback,
                                                 ESPMatterWiFiNetworkDiagnosticsClusterServerShutdownCallback);
    }

    /* Commands */
    command::create_network_passphrase_request(cluster);
    command::create_network_passphrase_response(cluster);

    return cluster;
}

} /* wifi_network_management */

namespace thread_network_directory {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, ThreadNetworkDirectory::Id, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }
    if (flags & CLUSTER_FLAG_SERVER) {
        static const auto plugin_server_init_cb = CALL_ONCE(MatterThreadNetworkDirectoryPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);
        attribute::create_preferred_extended_pan_id(cluster, nullptr, 0);
        attribute::create_thread_networks(cluster, nullptr, 0, 0);
        attribute::create_thread_network_table_size(cluster, 0);

        /** Attributes not managed internally **/
        global::attribute::create_cluster_revision(cluster, cluster_revision);
    }

    /* Commands */
    command::create_add_network(cluster);
    command::create_remove_network(cluster);
    command::create_get_operational_dataset(cluster);
    command::create_operational_dataset_response(cluster);

    return cluster;
}
} /* thread_network_directory */

namespace service_area {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, ServiceArea::Id, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }
    if (flags & CLUSTER_FLAG_SERVER) {
        if (config && config->delegate != nullptr) {
            static const auto delegate_init_cb = ServiceAreaDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        static const auto plugin_server_init_cb = CALL_ONCE(MatterServiceAreaPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);
        attribute::create_supported_areas(cluster, nullptr, 0, 0);
        attribute::create_selected_areas(cluster, nullptr, 0, 0);

        /** Attributes not managed internally **/
        global::attribute::create_cluster_revision(cluster, cluster_revision);
    }

    /* Commands */
    command::create_select_areas(cluster);
    command::create_select_areas_response(cluster);

    return cluster;
}
} /* service_area */

namespace water_heater_management {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, WaterHeaterManagement::Id, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }
    if (flags & CLUSTER_FLAG_SERVER) {
        if (config && config->delegate != nullptr) {
            static const auto delegate_init_cb = WaterHeaterManagementDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        static const auto plugin_server_init_cb = CALL_ONCE(MatterWaterHeaterManagementPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /** Attributes not managed internally **/
        global::attribute::create_cluster_revision(cluster, cluster_revision);
        attribute::create_heater_types(cluster, config->heater_types);
        attribute::create_heat_demand(cluster, config->heat_demand);
        attribute::create_boost_state(cluster, config->boost_state);
    }
    /* Commands */
    command::create_boost(cluster);
    command::create_cancel_boost(cluster);

    event::create_boost_started(cluster);
    event::create_boost_ended(cluster);
    return cluster;
}

} /* water_heater_management */

namespace water_heater_mode {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, WaterHeaterMode::Id, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    if (flags & CLUSTER_FLAG_SERVER) {
        if (config && config->delegate != nullptr) {
            static const auto delegate_init_cb = WaterHeaterModeDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);
        mode_base::attribute::create_supported_modes(cluster, NULL, 0, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);
        if (config) {
            mode_base::attribute::create_current_mode(cluster, config->current_mode);
        } else {
            ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
        }
    }

    /* Commands */
    mode_base::command::create_change_to_mode(cluster);
    mode_base::command::create_change_to_mode_response(cluster);

    return cluster;
}
} /* water_heater_mode */

namespace energy_preference {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, EnergyPreference::Id, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));

        if (config -> delegate != nullptr) {
            static const auto delegate_init_cb = EnergyPreferenceDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        static const auto plugin_server_init_cb = CALL_ONCE(MatterEnergyPreferencePluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /** Attributes not managed internally **/
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, config->feature_flags);

        // check against O.a+ feature conformance for energy preference
        VALIDATE_FEATURES_AT_LEAST_ONE("EnergyBalance,LowPowerModeSensitivity",
                                      feature::energy_balance::get_id(), feature::low_power_mode_sensitivity::get_id());

        if (has(feature::energy_balance::get_id())) {
            feature::energy_balance::add(cluster, &(config->features.energy_balance));
        }
        if (has(feature::low_power_mode_sensitivity::get_id())) {
            feature::low_power_mode_sensitivity::add(cluster, &(config->features.low_power_mode_sensitivity));
        }
    } // if (flags & CLUSTER_FLAG_SERVER)

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }

    return cluster;
}
} /* energy_preference */

namespace commissioner_control {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, CommissionerControl::Id, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }
    if (flags & CLUSTER_FLAG_SERVER) {
        if (config && config->delegate != nullptr) {
            static const auto delegate_init_cb = CommissionerControlDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        static const auto plugin_server_init_cb = CALL_ONCE(MatterCommissionerControlPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /** Attributes not managed internally **/
        global::attribute::create_cluster_revision(cluster, cluster_revision);
        attribute::create_supported_device_categories(cluster, config->supported_device_categories);
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }

    /* Commands */
    command::create_request_commissioning_approval(cluster);
    command::create_commission_node(cluster);
    command::create_reverse_open_commissioning_window(cluster);

    event::create_commissioning_request_result(cluster);
    return cluster;
}
} /* commissioner_control */

namespace ecosystem_information {
const function_generic_t *function_list = NULL;
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = cluster::create(endpoint, EcosystemInformation::Id, flags);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }
    if (flags & CLUSTER_FLAG_SERVER) {
        static const auto plugin_server_init_cb = CALL_ONCE(MatterEcosystemInformationPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);
        attribute::create_device_directory(cluster, nullptr, 0, 0);
        attribute::create_location_directory(cluster, nullptr, 0, 0);

        /** Attributes not managed internally **/
        global::attribute::create_cluster_revision(cluster, cluster_revision);
    }

    return cluster;
}
} /* ecosystem_information */

// namespace binary_input_basic {
//     // ToDo
// } /* binary_input_basic */

// namespace pulse_width_modulation {
//     // ToDo
// } /* pulse_width_modulation */

// namespace powersource_configuration {
//     // ToDo
// } /* powersource_configuration */

// namespace proxy_configuration {
//     // ToDo
// } /* proxy_configuration */

// namespace proxy_discovery {
//     // ToDo
// } /* proxy_discovery */

// namespace proxy_valid {
//     // ToDo
// } /* proxy_valid */

// namespace barrier_control {
//     // ToDo
// } /* barrier_control */

// namespace ballast_configuration {
//     // ToDo
// } /* ballast_configuration */

// namespace wakeonlan {
//     // ToDo
// } /* wakeonlan */

// namespace channel {
//     // ToDo
// } /* channel */

// namespace target_navigator {
//     // ToDo
// } /* target_navigator */

// namespace media_playback {
//     // ToDo
// } /* media_playback */

// namespace media_input {
//     // ToDo
// } /* media_input */

// namespace lowpower {
//     // ToDo
// } /* lowpower */

// namespace content_launcher {
//     // ToDo
// } /* content_launcher */

// namespace audio_output {
//     // ToDo
// } /* audio_output */

} /* cluster */
} /* esp_matter */
