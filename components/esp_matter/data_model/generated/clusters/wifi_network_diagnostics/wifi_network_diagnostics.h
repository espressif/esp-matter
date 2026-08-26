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
namespace wifi_network_diagnostics {

const uint8_t k_max_bssid_length = 6u;
namespace feature {
namespace packet_counts {
typedef struct config {
    nullable<uint32_t> beacon_rx_count;
    nullable<uint32_t> packet_multicast_rx_count;
    nullable<uint32_t> packet_multicast_tx_count;
    nullable<uint32_t> packet_unicast_rx_count;
    nullable<uint32_t> packet_unicast_tx_count;
    config() : beacon_rx_count(0), packet_multicast_rx_count(0), packet_multicast_tx_count(0), packet_unicast_rx_count(0), packet_unicast_tx_count(0) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* packet_counts */

namespace error_counts {
typedef struct config {
    nullable<uint32_t> beacon_lost_count;
    nullable<uint64_t> overrun_count;
    config() : beacon_lost_count(0), overrun_count(0) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* error_counts */

} /* feature */

namespace attribute {
attribute_t *create_bssid(cluster_t *cluster, uint8_t * value, uint16_t length);
attribute_t *create_security_type(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_wi_fi_version(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_channel_number(cluster_t *cluster, nullable<uint16_t> value);
attribute_t *create_rssi(cluster_t *cluster, nullable<int8_t> value);
attribute_t *create_beacon_lost_count(cluster_t *cluster, nullable<uint32_t> value);
attribute_t *create_beacon_rx_count(cluster_t *cluster, nullable<uint32_t> value);
attribute_t *create_packet_multicast_rx_count(cluster_t *cluster, nullable<uint32_t> value);
attribute_t *create_packet_multicast_tx_count(cluster_t *cluster, nullable<uint32_t> value);
attribute_t *create_packet_unicast_rx_count(cluster_t *cluster, nullable<uint32_t> value);
attribute_t *create_packet_unicast_tx_count(cluster_t *cluster, nullable<uint32_t> value);
attribute_t *create_current_max_rate(cluster_t *cluster, nullable<uint64_t> value);
attribute_t *create_overrun_count(cluster_t *cluster, nullable<uint64_t> value);
} /* attribute */

namespace command {
command_t *create_reset_counts(cluster_t *cluster);
} /* command */

namespace event {
event_t *create_disconnection(cluster_t *cluster);
event_t *create_association_failure(cluster_t *cluster);
event_t *create_connection_status(cluster_t *cluster);
} /* event */

typedef struct config {
    uint8_t bssid[k_max_bssid_length];
    nullable<uint8_t> security_type;
    nullable<uint8_t> wi_fi_version;
    nullable<uint16_t> channel_number;
    nullable<int8_t> rssi;
    config() : bssid{0}, security_type(), wi_fi_version(), channel_number(), rssi() {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* wifi_network_diagnostics */
} /* cluster */
} /* esp_matter */
