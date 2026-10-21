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
#include <commissioning_proxy.h>
#include <commissioning_proxy_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>
#include <app/ClusterCallbacks.h>

using namespace chip::app::Clusters;
using namespace esp_matter;
using namespace esp_matter::cluster;

static const char *TAG = "esp_matter_cluster";
constexpr uint16_t cluster_revision = 1;

namespace esp_matter {
namespace cluster {
namespace commissioning_proxy {

namespace feature {
namespace wi_fi_network_interface {
uint32_t get_id()
{
    return WiFiNetworkInterface::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* wi_fi_network_interface */

namespace background_scan {
uint32_t get_id()
{
    return BackgroundScan::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_max_cached_results(cluster, config->max_cached_results);
    attribute::create_num_cached_results(cluster, config->num_cached_results);
    attribute::create_cache_timeout(cluster, config->cache_timeout);
    attribute::create_cached_results(cluster, NULL, 0, 0);
    command::create_proxy_back_ground_scan_start_request(cluster);
    command::create_proxy_back_ground_scan_stop_request(cluster);

    return ESP_OK;
}
} /* background_scan */

} /* feature */

namespace attribute {
attribute_t *create_transport(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, Transport::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_bitmap));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(Transport::Min), esp_matter_attr_val::uint_sub_type::k_bitmap), esp_matter_attr_val(static_cast<uint8_t>(Transport::Max), esp_matter_attr_val::uint_sub_type::k_bitmap));
    return attribute;
}

attribute_t *create_scan_max_time(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, ScanMaxTime::Id, ATTRIBUTE_FLAG_WRITABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(ScanMaxTime::Min)), esp_matter_attr_val(static_cast<uint8_t>(ScanMaxTime::Max)));
    return attribute;
}

attribute_t *create_max_sessions(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, MaxSessions::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(MaxSessions::Min)), esp_matter_attr_val(static_cast<uint8_t>(MaxSessions::Max)));
    return attribute;
}

attribute_t *create_max_cached_results(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(background_scan), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, MaxCachedResults::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(MaxCachedResults::Min)), esp_matter_attr_val(static_cast<uint8_t>(MaxCachedResults::Max)));
    return attribute;
}

attribute_t *create_num_cached_results(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(background_scan), NULL);
    return esp_matter::attribute::create(cluster, NumCachedResults::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
}

attribute_t *create_cache_timeout(cluster_t *cluster, uint16_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(background_scan), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, CacheTimeout::Id, ATTRIBUTE_FLAG_WRITABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(CacheTimeout::Min)), esp_matter_attr_val(static_cast<uint16_t>(CacheTimeout::Max)));
    return attribute;
}

attribute_t *create_cached_results(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(background_scan), NULL);
    return esp_matter::attribute::create(cluster, CachedResults::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_wi_fi_band(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, WiFiBand::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_bitmap));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(WiFiBand::Min), esp_matter_attr_val::uint_sub_type::k_bitmap), esp_matter_attr_val(static_cast<uint8_t>(WiFiBand::Max), esp_matter_attr_val::uint_sub_type::k_bitmap));
    return attribute;
}

} /* attribute */
namespace command {
command_t *create_proxy_connect_request(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ProxyConnectRequest::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_proxy_connect_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ProxyConnectResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_proxy_disconnect_request(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ProxyDisconnectRequest::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_proxy_scan_request(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ProxyScanRequest::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_proxy_scan_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ProxyScanResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_proxy_back_ground_scan_start_request(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(background_scan), NULL);
    return esp_matter::command::create(cluster, ProxyBackGroundScanStartRequest::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_proxy_back_ground_scan_stop_request(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(background_scan), NULL);
    return esp_matter::command::create(cluster, ProxyBackGroundScanStopRequest::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_proxy_message_request(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ProxyMessageRequest::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_proxy_message_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ProxyMessageResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

} /* command */

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, commissioning_proxy::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, commissioning_proxy::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        static const auto plugin_server_init_cb = CALL_ONCE(MatterCommissioningProxyPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_transport(cluster, config->transport);
        attribute::create_scan_max_time(cluster, config->scan_max_time);
        attribute::create_max_sessions(cluster, config->max_sessions);
        command::create_proxy_connect_request(cluster);
        command::create_proxy_connect_response(cluster);
        command::create_proxy_disconnect_request(cluster);
        command::create_proxy_scan_request(cluster);
        command::create_proxy_scan_response(cluster);
        command::create_proxy_message_request(cluster);
        command::create_proxy_message_response(cluster);

        cluster::set_init_and_shutdown_callbacks(cluster, ESPMatterCommissioningProxyClusterServerInitCallback,
                                                 ESPMatterCommissioningProxyClusterServerShutdownCallback);
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }
    return cluster;
}

} /* commissioning_proxy */
} /* cluster */
} /* esp_matter */
