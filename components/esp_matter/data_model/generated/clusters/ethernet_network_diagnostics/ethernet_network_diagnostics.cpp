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
#include <app-common/zap-generated/callback.h>
#include <app/InteractionModelEngine.h>
#include <zap_common/app/PluginApplicationCallbacks.h>
#include <ethernet_network_diagnostics.h>
#include <ethernet_network_diagnostics_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>
#include <app/ClusterCallbacks.h>

using namespace chip::app::Clusters;
using chip::app::CommandHandler;
using chip::app::DataModel::Decode;
using chip::TLV::TLVReader;
using namespace esp_matter;
using namespace esp_matter::cluster;

static const char *TAG = "ethernet_network_diagnostics_cluster";
constexpr uint16_t cluster_revision = 1;

namespace esp_matter {
namespace cluster {
namespace ethernet_network_diagnostics {

namespace feature {
namespace packet_counts {
uint32_t get_id()
{
    return PacketCounts::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_packet_rx_count(cluster, config->packet_rx_count);
    attribute::create_packet_tx_count(cluster, config->packet_tx_count);
    command::create_reset_counts(cluster);

    return ESP_OK;
}
} /* packet_counts */

namespace error_counts {
uint32_t get_id()
{
    return ErrorCounts::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_tx_err_count(cluster, config->tx_err_count);
    attribute::create_collision_count(cluster, config->collision_count);
    attribute::create_overrun_count(cluster, config->overrun_count);
    command::create_reset_counts(cluster);

    return ESP_OK;
}
} /* error_counts */

} /* feature */

namespace attribute {
attribute_t *create_phy_rate(cluster_t *cluster, nullable<uint8_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, PHYRate::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(nullable<uint8_t>(9), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_full_duplex(cluster_t *cluster, nullable<bool> value)
{
    return esp_matter::attribute::create(cluster, FullDuplex::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
}

attribute_t *create_packet_rx_count(cluster_t *cluster, uint64_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(packet_counts), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, PacketRxCount::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint64_t>(0)), esp_matter_attr_val(static_cast<uint64_t>(4294967294)));
    return attribute;
}

attribute_t *create_packet_tx_count(cluster_t *cluster, uint64_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(packet_counts), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, PacketTxCount::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint64_t>(0)), esp_matter_attr_val(static_cast<uint64_t>(4294967294)));
    return attribute;
}

attribute_t *create_tx_err_count(cluster_t *cluster, uint64_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(error_counts), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, TxErrCount::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint64_t>(0)), esp_matter_attr_val(static_cast<uint64_t>(4294967294)));
    return attribute;
}

attribute_t *create_collision_count(cluster_t *cluster, uint64_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(error_counts), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, CollisionCount::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint64_t>(0)), esp_matter_attr_val(static_cast<uint64_t>(4294967294)));
    return attribute;
}

attribute_t *create_overrun_count(cluster_t *cluster, uint64_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(error_counts), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, OverrunCount::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint64_t>(0)), esp_matter_attr_val(static_cast<uint64_t>(4294967294)));
    return attribute;
}

attribute_t *create_carrier_detect(cluster_t *cluster, nullable<bool> value)
{
    return esp_matter::attribute::create(cluster, CarrierDetect::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
}

attribute_t *create_time_since_reset(cluster_t *cluster, uint64_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, TimeSinceReset::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint64_t>(0)), esp_matter_attr_val(static_cast<uint64_t>(4294967294)));
    return attribute;
}

} /* attribute */
namespace command {
command_t *create_reset_counts(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(((has_feature(packet_counts)) || (has_feature(error_counts))), NULL);
    return esp_matter::command::create(cluster, ResetCounts::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

} /* command */

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, ethernet_network_diagnostics::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, ethernet_network_diagnostics::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        static const auto plugin_server_init_cb = CALL_ONCE(MatterEthernetNetworkDiagnosticsPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        cluster::set_init_and_shutdown_callbacks(cluster, ESPMatterEthernetNetworkDiagnosticsClusterServerInitCallback,
                                                 ESPMatterEthernetNetworkDiagnosticsClusterServerShutdownCallback);
    }

    return cluster;
}

} /* ethernet_network_diagnostics */
} /* cluster */
} /* esp_matter */
