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
#include <thread_network_diagnostics.h>
#include <thread_network_diagnostics_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>
#include <app/ClusterCallbacks.h>

using namespace chip::app::Clusters;
using chip::app::CommandHandler;
using chip::app::DataModel::Decode;
using chip::TLV::TLVReader;
using namespace esp_matter;
using namespace esp_matter::cluster;

static const char *TAG = "thread_network_diagnostics_cluster";
constexpr uint16_t cluster_revision = 3;

namespace esp_matter {
namespace cluster {
namespace thread_network_diagnostics {

namespace feature {
namespace packet_counts {
uint32_t get_id()
{
    return PacketCounts::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());

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
    attribute::create_overrun_count(cluster, config->overrun_count);
    command::create_reset_counts(cluster);

    return ESP_OK;
}
} /* error_counts */

namespace mle_counts {
uint32_t get_id()
{
    return MLECounts::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* mle_counts */

namespace mac_counts {
uint32_t get_id()
{
    return MACCounts::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* mac_counts */

} /* feature */

namespace attribute {
attribute_t *create_channel(cluster_t *cluster, nullable<uint16_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, Channel::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint16_t>(0)), esp_matter_attr_val(nullable<uint16_t>(65534)));
    return attribute;
}

attribute_t *create_routing_role(cluster_t *cluster, nullable<uint8_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, RoutingRole::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(nullable<uint8_t>(6), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_network_name(cluster_t *cluster, char *value, uint16_t length)
{
    VerifyOrReturnValue(length <= k_max_network_name_length + 1, NULL, ESP_LOGE(TAG, "Could not create attribute, string length out of bound"));
    return esp_matter::attribute::create(cluster, NetworkName::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, length), k_max_network_name_length + 1);
}

attribute_t *create_pan_id(cluster_t *cluster, nullable<uint16_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, PanId::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint16_t>(0)), esp_matter_attr_val(nullable<uint16_t>(65534)));
    return attribute;
}

attribute_t *create_extended_pan_id(cluster_t *cluster, nullable<uint64_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, ExtendedPanId::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint64_t>(0)), esp_matter_attr_val(nullable<uint64_t>(4294967294)));
    return attribute;
}

attribute_t *create_mesh_local_prefix(cluster_t *cluster, uint8_t *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, MeshLocalPrefix::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, length));
}

attribute_t *create_overrun_count(cluster_t *cluster, uint64_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(error_counts), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, OverrunCount::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint64_t>(0)), esp_matter_attr_val(static_cast<uint64_t>(4294967294)));
    return attribute;
}

attribute_t *create_neighbor_table(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, NeighborTable::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

attribute_t *create_route_table(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, RouteTable::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

attribute_t *create_partition_id(cluster_t *cluster, nullable<uint32_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, PartitionId::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint32_t>(0)), esp_matter_attr_val(nullable<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_weighting(cluster_t *cluster, nullable<uint16_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, Weighting::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint16_t>(0)), esp_matter_attr_val(nullable<uint16_t>(255)));
    return attribute;
}

attribute_t *create_data_version(cluster_t *cluster, nullable<uint16_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, DataVersion::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint16_t>(0)), esp_matter_attr_val(nullable<uint16_t>(255)));
    return attribute;
}

attribute_t *create_stable_data_version(cluster_t *cluster, nullable<uint16_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, StableDataVersion::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint16_t>(0)), esp_matter_attr_val(nullable<uint16_t>(255)));
    return attribute;
}

attribute_t *create_leader_router_id(cluster_t *cluster, nullable<uint8_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, LeaderRouterId::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint8_t>(0)), esp_matter_attr_val(nullable<uint8_t>(62)));
    return attribute;
}

attribute_t *create_detached_role_count(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, DetachedRoleCount::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(65534)));
    return attribute;
}

attribute_t *create_child_role_count(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, ChildRoleCount::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(65534)));
    return attribute;
}

attribute_t *create_router_role_count(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, RouterRoleCount::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(65534)));
    return attribute;
}

attribute_t *create_leader_role_count(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, LeaderRoleCount::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(65534)));
    return attribute;
}

attribute_t *create_attach_attempt_count(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, AttachAttemptCount::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(65534)));
    return attribute;
}

attribute_t *create_partition_id_change_count(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, PartitionIdChangeCount::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(65534)));
    return attribute;
}

attribute_t *create_better_partition_attach_attempt_count(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, BetterPartitionAttachAttemptCount::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(65534)));
    return attribute;
}

attribute_t *create_parent_change_count(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, ParentChangeCount::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(65534)));
    return attribute;
}

attribute_t *create_tx_total_count(cluster_t *cluster, uint32_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, TxTotalCount::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(0)), esp_matter_attr_val(static_cast<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_tx_unicast_count(cluster_t *cluster, uint32_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, TxUnicastCount::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(0)), esp_matter_attr_val(static_cast<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_tx_broadcast_count(cluster_t *cluster, uint32_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, TxBroadcastCount::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(0)), esp_matter_attr_val(static_cast<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_tx_ack_requested_count(cluster_t *cluster, uint32_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, TxAckRequestedCount::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(0)), esp_matter_attr_val(static_cast<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_tx_acked_count(cluster_t *cluster, uint32_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, TxAckedCount::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(0)), esp_matter_attr_val(static_cast<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_tx_no_ack_requested_count(cluster_t *cluster, uint32_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, TxNoAckRequestedCount::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(0)), esp_matter_attr_val(static_cast<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_tx_data_count(cluster_t *cluster, uint32_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, TxDataCount::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(0)), esp_matter_attr_val(static_cast<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_tx_data_poll_count(cluster_t *cluster, uint32_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, TxDataPollCount::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(0)), esp_matter_attr_val(static_cast<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_tx_beacon_count(cluster_t *cluster, uint32_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, TxBeaconCount::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(0)), esp_matter_attr_val(static_cast<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_tx_beacon_request_count(cluster_t *cluster, uint32_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, TxBeaconRequestCount::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(0)), esp_matter_attr_val(static_cast<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_tx_other_count(cluster_t *cluster, uint32_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, TxOtherCount::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(0)), esp_matter_attr_val(static_cast<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_tx_retry_count(cluster_t *cluster, uint32_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, TxRetryCount::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(0)), esp_matter_attr_val(static_cast<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_tx_direct_max_retry_expiry_count(cluster_t *cluster, uint32_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, TxDirectMaxRetryExpiryCount::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(0)), esp_matter_attr_val(static_cast<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_tx_indirect_max_retry_expiry_count(cluster_t *cluster, uint32_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, TxIndirectMaxRetryExpiryCount::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(0)), esp_matter_attr_val(static_cast<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_tx_err_cca_count(cluster_t *cluster, uint32_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, TxErrCcaCount::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(0)), esp_matter_attr_val(static_cast<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_tx_err_abort_count(cluster_t *cluster, uint32_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, TxErrAbortCount::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(0)), esp_matter_attr_val(static_cast<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_tx_err_busy_channel_count(cluster_t *cluster, uint32_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, TxErrBusyChannelCount::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(0)), esp_matter_attr_val(static_cast<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_rx_total_count(cluster_t *cluster, uint32_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, RxTotalCount::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(0)), esp_matter_attr_val(static_cast<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_rx_unicast_count(cluster_t *cluster, uint32_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, RxUnicastCount::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(0)), esp_matter_attr_val(static_cast<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_rx_broadcast_count(cluster_t *cluster, uint32_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, RxBroadcastCount::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(0)), esp_matter_attr_val(static_cast<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_rx_data_count(cluster_t *cluster, uint32_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, RxDataCount::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(0)), esp_matter_attr_val(static_cast<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_rx_data_poll_count(cluster_t *cluster, uint32_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, RxDataPollCount::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(0)), esp_matter_attr_val(static_cast<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_rx_beacon_count(cluster_t *cluster, uint32_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, RxBeaconCount::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(0)), esp_matter_attr_val(static_cast<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_rx_beacon_request_count(cluster_t *cluster, uint32_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, RxBeaconRequestCount::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(0)), esp_matter_attr_val(static_cast<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_rx_other_count(cluster_t *cluster, uint32_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, RxOtherCount::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(0)), esp_matter_attr_val(static_cast<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_rx_address_filtered_count(cluster_t *cluster, uint32_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, RxAddressFilteredCount::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(0)), esp_matter_attr_val(static_cast<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_rx_dest_addr_filtered_count(cluster_t *cluster, uint32_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, RxDestAddrFilteredCount::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(0)), esp_matter_attr_val(static_cast<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_rx_duplicated_count(cluster_t *cluster, uint32_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, RxDuplicatedCount::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(0)), esp_matter_attr_val(static_cast<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_rx_err_no_frame_count(cluster_t *cluster, uint32_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, RxErrNoFrameCount::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(0)), esp_matter_attr_val(static_cast<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_rx_err_unknown_neighbor_count(cluster_t *cluster, uint32_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, RxErrUnknownNeighborCount::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(0)), esp_matter_attr_val(static_cast<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_rx_err_invalid_src_addr_count(cluster_t *cluster, uint32_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, RxErrInvalidSrcAddrCount::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(0)), esp_matter_attr_val(static_cast<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_rx_err_sec_count(cluster_t *cluster, uint32_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, RxErrSecCount::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(0)), esp_matter_attr_val(static_cast<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_rx_err_fcs_count(cluster_t *cluster, uint32_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, RxErrFcsCount::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(0)), esp_matter_attr_val(static_cast<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_rx_err_other_count(cluster_t *cluster, uint32_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, RxErrOtherCount::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(0)), esp_matter_attr_val(static_cast<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_active_timestamp(cluster_t *cluster, nullable<uint64_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, ActiveTimestamp::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint64_t>(0)), esp_matter_attr_val(nullable<uint64_t>(4294967294)));
    return attribute;
}

attribute_t *create_pending_timestamp(cluster_t *cluster, nullable<uint64_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, PendingTimestamp::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint64_t>(0)), esp_matter_attr_val(nullable<uint64_t>(4294967294)));
    return attribute;
}

attribute_t *create_delay(cluster_t *cluster, nullable<uint32_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, Delay::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint32_t>(0)), esp_matter_attr_val(nullable<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_security_policy(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, SecurityPolicy::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_channel_page_0_mask(cluster_t *cluster, uint8_t *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, ChannelPage0Mask::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, length));
}

attribute_t *create_operational_dataset_components(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, OperationalDatasetComponents::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_active_network_faults_list(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, ActiveNetworkFaultsList::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
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
event_t *create_connection_status(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, ConnectionStatus::Id);
}

event_t *create_network_fault_change(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, NetworkFaultChange::Id);
}

} /* event */

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, thread_network_diagnostics::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, thread_network_diagnostics::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        static const auto plugin_server_init_cb = CALL_ONCE(MatterThreadNetworkDiagnosticsPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_channel(cluster, config->channel);
        attribute::create_routing_role(cluster, config->routing_role);
        attribute::create_network_name(cluster, config->network_name, sizeof(config->network_name));
        attribute::create_pan_id(cluster, config->pan_id);
        attribute::create_extended_pan_id(cluster, config->extended_pan_id);
        attribute::create_mesh_local_prefix(cluster, config->mesh_local_prefix, sizeof(config->mesh_local_prefix));
        attribute::create_partition_id(cluster, config->partition_id);
        attribute::create_weighting(cluster, config->weighting);
        attribute::create_data_version(cluster, config->data_version);
        attribute::create_stable_data_version(cluster, config->stable_data_version);
        attribute::create_leader_router_id(cluster, config->leader_router_id);
        attribute::create_channel_page_0_mask(cluster, config->channel_page_0_mask, sizeof(config->channel_page_0_mask));
        attribute::create_neighbor_table(cluster, NULL, 0, 0);
        attribute::create_route_table(cluster, NULL, 0, 0);
        attribute::create_security_policy(cluster, NULL, 0, 0);
        attribute::create_operational_dataset_components(cluster, NULL, 0, 0);
        attribute::create_active_network_faults_list(cluster, NULL, 0, 0);

        cluster::set_init_and_shutdown_callbacks(cluster, ESPMatterThreadNetworkDiagnosticsClusterServerInitCallback,
                                                 ESPMatterThreadNetworkDiagnosticsClusterServerShutdownCallback);
    }

    return cluster;
}

} /* thread_network_diagnostics */
} /* cluster */
} /* esp_matter */
