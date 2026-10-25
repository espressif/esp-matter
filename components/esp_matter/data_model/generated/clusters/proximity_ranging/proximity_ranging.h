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
namespace proximity_ranging {

const uint8_t k_max_wi_fi_dev_ik_length = 16u;
const uint8_t k_max_blt_dev_ik_length = 16u;
namespace feature {
namespace wi_fi_usd_proximity_detection {
typedef struct config {
    uint8_t wi_fi_dev_ik[k_max_wi_fi_dev_ik_length];
    config() {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* wi_fi_usd_proximity_detection */

namespace bluetooth_channel_sounding {
typedef struct config {
    uint8_t blt_dev_ik[k_max_blt_dev_ik_length];
    uint8_t bltcs_security_level;
    uint8_t bltcs_mode_capability;
    config() : bltcs_security_level(0), bltcs_mode_capability(0) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* bluetooth_channel_sounding */

namespace ble_beacon_rssi {
typedef struct config {
    uint64_t ble_device_id;
    config() : ble_device_id(0) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* ble_beacon_rssi */

} /* feature */

namespace attribute {
attribute_t *create_ranging_capabilities(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_wi_fi_dev_ik(cluster_t *cluster, uint8_t * value, uint16_t length);
attribute_t *create_ble_device_id(cluster_t *cluster, uint64_t value);
attribute_t *create_blt_dev_ik(cluster_t *cluster, uint8_t * value, uint16_t length);
attribute_t *create_bltcs_security_level(cluster_t *cluster, uint8_t value);
attribute_t *create_bltcs_mode_capability(cluster_t *cluster, uint8_t value);
attribute_t *create_session_id_list(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_ranging_constraints(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
} /* attribute */

namespace command {
command_t *create_start_ranging_request(cluster_t *cluster);
command_t *create_start_ranging_response(cluster_t *cluster);
command_t *create_stop_ranging_request(cluster_t *cluster);
} /* command */

namespace event {
event_t *create_ranging_result(cluster_t *cluster);
event_t *create_ranging_session_status(cluster_t *cluster);
} /* event */

typedef struct config {
    struct {
        feature::wi_fi_usd_proximity_detection::config_t wi_fi_usd_proximity_detection;
        feature::bluetooth_channel_sounding::config_t bluetooth_channel_sounding;
        feature::ble_beacon_rssi::config_t ble_beacon_rssi;
    } features;
    uint32_t feature_flags;
    config() : feature_flags(0) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* proximity_ranging */
} /* cluster */
} /* esp_matter */
