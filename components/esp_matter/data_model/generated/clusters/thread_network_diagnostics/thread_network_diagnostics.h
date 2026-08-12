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

#pragma once
#include <esp_matter_data_model.h>

namespace esp_matter {
namespace cluster {
namespace thread_network_diagnostics {

const uint8_t k_max_network_name_length = 16u;
const uint8_t k_max_mesh_local_prefix_length = 17u;
const uint8_t k_max_channel_page_0_mask_length = 4u;
namespace feature {
namespace packet_counts {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* packet_counts */

namespace error_counts {
typedef struct config {
    uint64_t overrun_count;
    config() : overrun_count(0) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* error_counts */

namespace mle_counts {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* mle_counts */

namespace mac_counts {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* mac_counts */

} /* feature */

namespace attribute {
attribute_t *create_channel(cluster_t *cluster, nullable<uint16_t> value);
attribute_t *create_routing_role(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_network_name(cluster_t *cluster, char * value, uint16_t length);
attribute_t *create_pan_id(cluster_t *cluster, nullable<uint16_t> value);
attribute_t *create_extended_pan_id(cluster_t *cluster, nullable<uint64_t> value);
attribute_t *create_mesh_local_prefix(cluster_t *cluster, uint8_t * value, uint16_t length);
attribute_t *create_overrun_count(cluster_t *cluster, uint64_t value);
attribute_t *create_neighbor_table(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_route_table(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_partition_id(cluster_t *cluster, nullable<uint32_t> value);
attribute_t *create_weighting(cluster_t *cluster, nullable<uint16_t> value);
attribute_t *create_data_version(cluster_t *cluster, nullable<uint16_t> value);
attribute_t *create_stable_data_version(cluster_t *cluster, nullable<uint16_t> value);
attribute_t *create_leader_router_id(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_detached_role_count(cluster_t *cluster, uint16_t value);
attribute_t *create_child_role_count(cluster_t *cluster, uint16_t value);
attribute_t *create_router_role_count(cluster_t *cluster, uint16_t value);
attribute_t *create_leader_role_count(cluster_t *cluster, uint16_t value);
attribute_t *create_attach_attempt_count(cluster_t *cluster, uint16_t value);
attribute_t *create_partition_id_change_count(cluster_t *cluster, uint16_t value);
attribute_t *create_better_partition_attach_attempt_count(cluster_t *cluster, uint16_t value);
attribute_t *create_parent_change_count(cluster_t *cluster, uint16_t value);
attribute_t *create_tx_total_count(cluster_t *cluster, uint32_t value);
attribute_t *create_tx_unicast_count(cluster_t *cluster, uint32_t value);
attribute_t *create_tx_broadcast_count(cluster_t *cluster, uint32_t value);
attribute_t *create_tx_ack_requested_count(cluster_t *cluster, uint32_t value);
attribute_t *create_tx_acked_count(cluster_t *cluster, uint32_t value);
attribute_t *create_tx_no_ack_requested_count(cluster_t *cluster, uint32_t value);
attribute_t *create_tx_data_count(cluster_t *cluster, uint32_t value);
attribute_t *create_tx_data_poll_count(cluster_t *cluster, uint32_t value);
attribute_t *create_tx_beacon_count(cluster_t *cluster, uint32_t value);
attribute_t *create_tx_beacon_request_count(cluster_t *cluster, uint32_t value);
attribute_t *create_tx_other_count(cluster_t *cluster, uint32_t value);
attribute_t *create_tx_retry_count(cluster_t *cluster, uint32_t value);
attribute_t *create_tx_direct_max_retry_expiry_count(cluster_t *cluster, uint32_t value);
attribute_t *create_tx_indirect_max_retry_expiry_count(cluster_t *cluster, uint32_t value);
attribute_t *create_tx_err_cca_count(cluster_t *cluster, uint32_t value);
attribute_t *create_tx_err_abort_count(cluster_t *cluster, uint32_t value);
attribute_t *create_tx_err_busy_channel_count(cluster_t *cluster, uint32_t value);
attribute_t *create_rx_total_count(cluster_t *cluster, uint32_t value);
attribute_t *create_rx_unicast_count(cluster_t *cluster, uint32_t value);
attribute_t *create_rx_broadcast_count(cluster_t *cluster, uint32_t value);
attribute_t *create_rx_data_count(cluster_t *cluster, uint32_t value);
attribute_t *create_rx_data_poll_count(cluster_t *cluster, uint32_t value);
attribute_t *create_rx_beacon_count(cluster_t *cluster, uint32_t value);
attribute_t *create_rx_beacon_request_count(cluster_t *cluster, uint32_t value);
attribute_t *create_rx_other_count(cluster_t *cluster, uint32_t value);
attribute_t *create_rx_address_filtered_count(cluster_t *cluster, uint32_t value);
attribute_t *create_rx_dest_addr_filtered_count(cluster_t *cluster, uint32_t value);
attribute_t *create_rx_duplicated_count(cluster_t *cluster, uint32_t value);
attribute_t *create_rx_err_no_frame_count(cluster_t *cluster, uint32_t value);
attribute_t *create_rx_err_unknown_neighbor_count(cluster_t *cluster, uint32_t value);
attribute_t *create_rx_err_invalid_src_addr_count(cluster_t *cluster, uint32_t value);
attribute_t *create_rx_err_sec_count(cluster_t *cluster, uint32_t value);
attribute_t *create_rx_err_fcs_count(cluster_t *cluster, uint32_t value);
attribute_t *create_rx_err_other_count(cluster_t *cluster, uint32_t value);
attribute_t *create_active_timestamp(cluster_t *cluster, nullable<uint64_t> value);
attribute_t *create_pending_timestamp(cluster_t *cluster, nullable<uint64_t> value);
attribute_t *create_delay(cluster_t *cluster, nullable<uint32_t> value);
attribute_t *create_security_policy(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_channel_page_0_mask(cluster_t *cluster, uint8_t * value, uint16_t length);
attribute_t *create_operational_dataset_components(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_active_network_faults_list(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
} /* attribute */

namespace command {
command_t *create_reset_counts(cluster_t *cluster);
} /* command */

namespace event {
event_t *create_connection_status(cluster_t *cluster);
event_t *create_network_fault_change(cluster_t *cluster);
} /* event */

typedef struct config {
    nullable<uint16_t> channel;
    nullable<uint8_t> routing_role;
    char network_name[k_max_network_name_length + 1];
    nullable<uint16_t> pan_id;
    nullable<uint64_t> extended_pan_id;
    uint8_t mesh_local_prefix[k_max_mesh_local_prefix_length];
    nullable<uint32_t> partition_id;
    nullable<uint16_t> weighting;
    nullable<uint16_t> data_version;
    nullable<uint16_t> stable_data_version;
    nullable<uint8_t> leader_router_id;
    uint8_t channel_page_0_mask[k_max_channel_page_0_mask_length];
    config() : channel(), routing_role(), network_name{0}, pan_id(), extended_pan_id(), mesh_local_prefix{0}, partition_id(), weighting(), data_version(), stable_data_version(), leader_router_id(), channel_page_0_mask{0} {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* thread_network_diagnostics */
} /* cluster */
} /* esp_matter */
