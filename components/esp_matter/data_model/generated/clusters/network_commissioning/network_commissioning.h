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
namespace network_commissioning {

const uint8_t k_max_last_network_id_length = 32u;
namespace feature {
namespace wi_fi_network_interface {
typedef struct config {
    uint8_t scan_max_time_seconds;
    uint8_t connect_max_time_seconds;
    config() : scan_max_time_seconds(0), connect_max_time_seconds(0) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* wi_fi_network_interface */

namespace thread_network_interface {
typedef struct config {
    uint8_t scan_max_time_seconds;
    uint8_t connect_max_time_seconds;
    uint16_t supported_thread_features;
    uint16_t thread_version;
    config() : scan_max_time_seconds(0), connect_max_time_seconds(0), supported_thread_features(0), thread_version(0) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* thread_network_interface */

namespace ethernet_network_interface {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* ethernet_network_interface */

} /* feature */

namespace attribute {
attribute_t *create_max_networks(cluster_t *cluster, uint8_t value);
attribute_t *create_networks(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_scan_max_time_seconds(cluster_t *cluster, uint8_t value);
attribute_t *create_connect_max_time_seconds(cluster_t *cluster, uint8_t value);
attribute_t *create_interface_enabled(cluster_t *cluster, bool value);
attribute_t *create_last_networking_status(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_last_network_id(cluster_t *cluster, uint8_t * value, uint16_t length);
attribute_t *create_last_connect_error_value(cluster_t *cluster, nullable<int32_t> value);
attribute_t *create_supported_wi_fi_bands(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_supported_thread_features(cluster_t *cluster, uint16_t value);
attribute_t *create_thread_version(cluster_t *cluster, uint16_t value);
} /* attribute */

namespace command {
command_t *create_scan_networks(cluster_t *cluster);
command_t *create_scan_networks_response(cluster_t *cluster);
command_t *create_add_or_update_wi_fi_network(cluster_t *cluster);
command_t *create_add_or_update_thread_network(cluster_t *cluster);
command_t *create_remove_network(cluster_t *cluster);
command_t *create_network_config_response(cluster_t *cluster);
command_t *create_connect_network(cluster_t *cluster);
command_t *create_connect_network_response(cluster_t *cluster);
command_t *create_reorder_network(cluster_t *cluster);
} /* command */

typedef struct config {
    uint8_t max_networks;
    bool interface_enabled;
    nullable<uint8_t> last_networking_status;
    uint8_t last_network_id[k_max_last_network_id_length];
    nullable<int32_t> last_connect_error_value;
    struct {
        feature::wi_fi_network_interface::config_t wi_fi_network_interface;
        feature::thread_network_interface::config_t thread_network_interface;
    } features;
    uint32_t feature_flags;
    config() : max_networks(1), interface_enabled(true), last_networking_status(0), last_network_id{0}, last_connect_error_value(0), feature_flags(0) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* network_commissioning */
} /* cluster */
} /* esp_matter */
