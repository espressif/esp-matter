// Copyright 2026 Espressif Systems (Shanghai) PTE LTD
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

/* THIS IS A GENERATED FILE, DO NOT EDIT */

#include <esp_log.h>
#include <esp_matter_core.h>
#include <esp_matter.h>

#include <app-common/zap-generated/cluster-enums.h>
#include <zap_common/app/PluginApplicationCallbacks.h>
#include <network_commissioning.h>
#include <network_commissioning_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>
#include <app/ClusterCallbacks.h>

using namespace chip::app::Clusters;
using namespace esp_matter;
using namespace esp_matter::cluster;

static const char *TAG = "esp_matter_cluster";
constexpr uint16_t cluster_revision = 2;

namespace esp_matter {
namespace cluster {
namespace network_commissioning {

namespace feature {
namespace wi_fi_network_interface {
uint32_t get_id()
{
    return WiFiNetworkInterface::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_scan_max_time_seconds(cluster, config->scan_max_time_seconds);
    attribute::create_connect_max_time_seconds(cluster, config->connect_max_time_seconds);
    attribute::create_supported_wi_fi_bands(cluster, NULL, 0, 0);
    command::create_scan_networks(cluster);
    command::create_scan_networks_response(cluster);
    command::create_add_or_update_wi_fi_network(cluster);
    command::create_remove_network(cluster);
    command::create_network_config_response(cluster);
    command::create_connect_network(cluster);
    command::create_connect_network_response(cluster);
    command::create_reorder_network(cluster);

    return ESP_OK;
}
} /* wi_fi_network_interface */

namespace thread_network_interface {
uint32_t get_id()
{
    return ThreadNetworkInterface::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_scan_max_time_seconds(cluster, config->scan_max_time_seconds);
    attribute::create_connect_max_time_seconds(cluster, config->connect_max_time_seconds);
    attribute::create_supported_thread_features(cluster, config->supported_thread_features);
    attribute::create_thread_version(cluster, config->thread_version);
    command::create_scan_networks(cluster);
    command::create_scan_networks_response(cluster);
    command::create_add_or_update_thread_network(cluster);
    command::create_remove_network(cluster);
    command::create_network_config_response(cluster);
    command::create_connect_network(cluster);
    command::create_connect_network_response(cluster);
    command::create_reorder_network(cluster);

    return ESP_OK;
}
} /* thread_network_interface */

namespace ethernet_network_interface {
uint32_t get_id()
{
    return EthernetNetworkInterface::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* ethernet_network_interface */

} /* feature */

namespace attribute {
attribute_t *create_max_networks(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, MaxNetworks::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(1)), esp_matter_attr_val(static_cast<uint8_t>(254)));
    return attribute;
}

attribute_t *create_networks(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, Networks::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

attribute_t *create_scan_max_time_seconds(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(((has_feature(wi_fi_network_interface)) || (has_feature(thread_network_interface))), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, ScanMaxTimeSeconds::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0)), esp_matter_attr_val(static_cast<uint8_t>(254)));
    return attribute;
}

attribute_t *create_connect_max_time_seconds(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(((has_feature(wi_fi_network_interface)) || (has_feature(thread_network_interface))), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, ConnectMaxTimeSeconds::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0)), esp_matter_attr_val(static_cast<uint8_t>(254)));
    return attribute;
}

attribute_t *create_interface_enabled(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, InterfaceEnabled::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
}

attribute_t *create_last_networking_status(cluster_t *cluster, nullable<uint8_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, LastNetworkingStatus::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(nullable<uint8_t>(12), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_last_network_id(cluster_t *cluster, uint8_t *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, LastNetworkID::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, length));
}

attribute_t *create_last_connect_error_value(cluster_t *cluster, nullable<int32_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, LastConnectErrorValue::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<int32_t>(-2147483648)), esp_matter_attr_val(nullable<int32_t>(2147483646)));
    return attribute;
}

attribute_t *create_supported_wi_fi_bands(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(wi_fi_network_interface), NULL);
    return esp_matter::attribute::create(cluster, SupportedWiFiBands::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

attribute_t *create_supported_thread_features(cluster_t *cluster, uint16_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(thread_network_interface), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, SupportedThreadFeatures::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_bitmap));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0), esp_matter_attr_val::uint_sub_type::k_bitmap), esp_matter_attr_val(static_cast<uint16_t>(65535), esp_matter_attr_val::uint_sub_type::k_bitmap));
    return attribute;
}

attribute_t *create_thread_version(cluster_t *cluster, uint16_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(thread_network_interface), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, ThreadVersion::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(65534)));
    return attribute;
}

} /* attribute */
namespace command {
command_t *create_scan_networks(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(((has_feature(wi_fi_network_interface)) || (has_feature(thread_network_interface))), NULL);
    return esp_matter::command::create(cluster, ScanNetworks::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_scan_networks_response(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(((has_feature(wi_fi_network_interface)) || (has_feature(thread_network_interface))), NULL);
    return esp_matter::command::create(cluster, ScanNetworksResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_add_or_update_wi_fi_network(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(wi_fi_network_interface), NULL);
    return esp_matter::command::create(cluster, AddOrUpdateWiFiNetwork::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_add_or_update_thread_network(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(thread_network_interface), NULL);
    return esp_matter::command::create(cluster, AddOrUpdateThreadNetwork::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_remove_network(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(((has_feature(wi_fi_network_interface)) || (has_feature(thread_network_interface))), NULL);
    return esp_matter::command::create(cluster, RemoveNetwork::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_network_config_response(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(((has_feature(wi_fi_network_interface)) || (has_feature(thread_network_interface))), NULL);
    return esp_matter::command::create(cluster, NetworkConfigResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_connect_network(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(((has_feature(wi_fi_network_interface)) || (has_feature(thread_network_interface))), NULL);
    return esp_matter::command::create(cluster, ConnectNetwork::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_connect_network_response(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(((has_feature(wi_fi_network_interface)) || (has_feature(thread_network_interface))), NULL);
    return esp_matter::command::create(cluster, ConnectNetworkResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_reorder_network(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(((has_feature(wi_fi_network_interface)) || (has_feature(thread_network_interface))), NULL);
    return esp_matter::command::create(cluster, ReorderNetwork::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

} /* command */

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, network_commissioning::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, network_commissioning::Id));
#if !defined(CONFIG_CUSTOM_NETWORK_CONFIG)
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        static const auto plugin_server_init_cb = CALL_ONCE(MatterNetworkCommissioningPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, config->feature_flags);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_max_networks(cluster, config->max_networks);
        attribute::create_interface_enabled(cluster, config->interface_enabled);
        attribute::create_last_networking_status(cluster, config->last_networking_status);
        attribute::create_last_network_id(cluster, config->last_network_id, sizeof(config->last_network_id));
        attribute::create_last_connect_error_value(cluster, config->last_connect_error_value);
        attribute::create_networks(cluster, NULL, 0, 0);

        uint32_t feature_map = config->feature_flags;
        VALIDATE_FEATURES_EXACT_ONE("WiFiNetworkInterface,ThreadNetworkInterface,EthernetNetworkInterface",
                                    feature::wi_fi_network_interface::get_id(), feature::thread_network_interface::get_id(), feature::ethernet_network_interface::get_id());
        if (feature_map & feature::wi_fi_network_interface::get_id()) {
            VerifyOrReturnValue(feature::wi_fi_network_interface::add(cluster, &(config->features.wi_fi_network_interface)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::thread_network_interface::get_id()) {
            VerifyOrReturnValue(feature::thread_network_interface::add(cluster, &(config->features.thread_network_interface)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::ethernet_network_interface::get_id()) {
            VerifyOrReturnValue(feature::ethernet_network_interface::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }

        cluster::set_init_and_shutdown_callbacks(cluster, ESPMatterNetworkCommissioningClusterServerInitCallback,
                                                 ESPMatterNetworkCommissioningClusterServerShutdownCallback);
    }
#endif // !defined(CONFIG_CUSTOM_NETWORK_CONFIG)

    return cluster;
}

} /* network_commissioning */
} /* cluster */
} /* esp_matter */
