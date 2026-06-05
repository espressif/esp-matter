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
#include <wi_fi_network_diagnostics.h>
#include <wi_fi_network_diagnostics_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>
#include <app/ClusterCallbacks.h>

using namespace chip::app::Clusters;
using chip::app::CommandHandler;
using chip::app::DataModel::Decode;
using chip::TLV::TLVReader;
using namespace esp_matter;
using namespace esp_matter::cluster;

static const char *TAG = "wi_fi_network_diagnostics_cluster";
constexpr uint16_t cluster_revision = 1;

namespace esp_matter {
namespace cluster {
namespace wi_fi_network_diagnostics {

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
    attribute::create_beacon_rx_count(cluster, config->beacon_rx_count);
    attribute::create_packet_multicast_rx_count(cluster, config->packet_multicast_rx_count);
    attribute::create_packet_multicast_tx_count(cluster, config->packet_multicast_tx_count);
    attribute::create_packet_unicast_rx_count(cluster, config->packet_unicast_rx_count);
    attribute::create_packet_unicast_tx_count(cluster, config->packet_unicast_tx_count);

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
    attribute::create_beacon_lost_count(cluster, config->beacon_lost_count);
    attribute::create_overrun_count(cluster, config->overrun_count);
    command::create_reset_counts(cluster);

    return ESP_OK;
}
} /* error_counts */

} /* feature */

namespace attribute {
attribute_t *create_bssid(cluster_t *cluster, uint8_t *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, BSSID::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, length));
}

attribute_t *create_security_type(cluster_t *cluster, nullable<uint8_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, SecurityType::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(nullable<uint8_t>(5), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_wi_fi_version(cluster_t *cluster, nullable<uint8_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, WiFiVersion::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(nullable<uint8_t>(6), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_channel_number(cluster_t *cluster, nullable<uint16_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, ChannelNumber::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint16_t>(0)), esp_matter_attr_val(nullable<uint16_t>(65534)));
    return attribute;
}

attribute_t *create_rssi(cluster_t *cluster, nullable<int8_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, RSSI::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<int8_t>(-120)), esp_matter_attr_val(nullable<int8_t>(0)));
    return attribute;
}

attribute_t *create_beacon_lost_count(cluster_t *cluster, nullable<uint32_t> value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(error_counts), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, BeaconLostCount::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint32_t>(0)), esp_matter_attr_val(nullable<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_beacon_rx_count(cluster_t *cluster, nullable<uint32_t> value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(packet_counts), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, BeaconRxCount::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint32_t>(0)), esp_matter_attr_val(nullable<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_packet_multicast_rx_count(cluster_t *cluster, nullable<uint32_t> value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(packet_counts), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, PacketMulticastRxCount::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint32_t>(0)), esp_matter_attr_val(nullable<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_packet_multicast_tx_count(cluster_t *cluster, nullable<uint32_t> value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(packet_counts), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, PacketMulticastTxCount::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint32_t>(0)), esp_matter_attr_val(nullable<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_packet_unicast_rx_count(cluster_t *cluster, nullable<uint32_t> value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(packet_counts), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, PacketUnicastRxCount::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint32_t>(0)), esp_matter_attr_val(nullable<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_packet_unicast_tx_count(cluster_t *cluster, nullable<uint32_t> value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(packet_counts), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, PacketUnicastTxCount::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint32_t>(0)), esp_matter_attr_val(nullable<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_current_max_rate(cluster_t *cluster, nullable<uint64_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, CurrentMaxRate::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint64_t>(0)), esp_matter_attr_val(nullable<uint64_t>(4294967294)));
    return attribute;
}

attribute_t *create_overrun_count(cluster_t *cluster, nullable<uint64_t> value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(error_counts), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, OverrunCount::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint64_t>(0)), esp_matter_attr_val(nullable<uint64_t>(4294967294)));
    return attribute;
}

} /* attribute */
namespace command {
command_t *create_reset_counts(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(error_counts), NULL);
    return esp_matter::command::create(cluster, ResetCounts::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

} /* command */

namespace event {
event_t *create_disconnection(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, Disconnection::Id);
}

event_t *create_association_failure(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, AssociationFailure::Id);
}

event_t *create_connection_status(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, ConnectionStatus::Id);
}

} /* event */

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, wi_fi_network_diagnostics::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, wi_fi_network_diagnostics::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        static const auto plugin_server_init_cb = CALL_ONCE(MatterWiFiNetworkDiagnosticsPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_bssid(cluster, config->bssid, sizeof(config->bssid));
        attribute::create_security_type(cluster, config->security_type);
        attribute::create_wi_fi_version(cluster, config->wi_fi_version);
        attribute::create_channel_number(cluster, config->channel_number);
        attribute::create_rssi(cluster, config->rssi);

        cluster::set_init_and_shutdown_callbacks(cluster, ESPMatterWiFiNetworkDiagnosticsClusterServerInitCallback,
                                                 ESPMatterWiFiNetworkDiagnosticsClusterServerShutdownCallback);
    }

    return cluster;
}

} /* wi_fi_network_diagnostics */
} /* cluster */
} /* esp_matter */
