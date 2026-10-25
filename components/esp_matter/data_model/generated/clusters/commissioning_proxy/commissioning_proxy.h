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
namespace commissioning_proxy {

namespace feature {
namespace wi_fi_network_interface {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* wi_fi_network_interface */

namespace background_scan {
typedef struct config {
    uint8_t max_cached_results;
    uint8_t num_cached_results;
    uint16_t cache_timeout;
    config() : max_cached_results(1), num_cached_results(0), cache_timeout(1) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* background_scan */

} /* feature */

namespace attribute {
attribute_t *create_transport(cluster_t *cluster, uint8_t value);
attribute_t *create_scan_max_time(cluster_t *cluster, uint8_t value);
attribute_t *create_max_sessions(cluster_t *cluster, uint8_t value);
attribute_t *create_max_cached_results(cluster_t *cluster, uint8_t value);
attribute_t *create_num_cached_results(cluster_t *cluster, uint8_t value);
attribute_t *create_cache_timeout(cluster_t *cluster, uint16_t value);
attribute_t *create_cached_results(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_wi_fi_band(cluster_t *cluster, uint8_t value);
} /* attribute */

namespace command {
command_t *create_proxy_connect_request(cluster_t *cluster);
command_t *create_proxy_connect_response(cluster_t *cluster);
command_t *create_proxy_disconnect_request(cluster_t *cluster);
command_t *create_proxy_scan_request(cluster_t *cluster);
command_t *create_proxy_scan_response(cluster_t *cluster);
command_t *create_proxy_back_ground_scan_start_request(cluster_t *cluster);
command_t *create_proxy_back_ground_scan_stop_request(cluster_t *cluster);
command_t *create_proxy_message_request(cluster_t *cluster);
command_t *create_proxy_message_response(cluster_t *cluster);
} /* command */

typedef struct config {
    uint8_t transport;
    uint8_t scan_max_time;
    uint8_t max_sessions;
    config() : transport(1), scan_max_time(1), max_sessions(1) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* commissioning_proxy */
} /* cluster */
} /* esp_matter */
