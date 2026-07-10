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
namespace switch_cluster {

namespace feature {
namespace latching_switch {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* latching_switch */

namespace momentary_switch {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* momentary_switch */

namespace momentary_switch_release {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* momentary_switch_release */

namespace momentary_switch_long_press {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* momentary_switch_long_press */

namespace momentary_switch_multi_press {
typedef struct config {
    uint8_t multi_press_max;
    config() : multi_press_max(2) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* momentary_switch_multi_press */

namespace action_switch {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* action_switch */

} /* feature */

namespace attribute {
attribute_t *create_number_of_positions(cluster_t *cluster, uint8_t value);
attribute_t *create_current_position(cluster_t *cluster, uint8_t value);
attribute_t *create_multi_press_max(cluster_t *cluster, uint8_t value);
} /* attribute */

namespace event {
event_t *create_switch_latched(cluster_t *cluster);
event_t *create_initial_press(cluster_t *cluster);
event_t *create_long_press(cluster_t *cluster);
event_t *create_short_release(cluster_t *cluster);
event_t *create_long_release(cluster_t *cluster);
event_t *create_multi_press_ongoing(cluster_t *cluster);
event_t *create_multi_press_complete(cluster_t *cluster);
esp_err_t send_switch_latched(chip::EndpointId endpoint, uint8_t new_position);
esp_err_t send_initial_press(chip::EndpointId endpoint, uint8_t new_position);
esp_err_t send_long_press(chip::EndpointId endpoint, uint8_t new_position);
esp_err_t send_short_release(chip::EndpointId endpoint, uint8_t previous_position);
esp_err_t send_long_release(chip::EndpointId endpoint, uint8_t previous_position);
esp_err_t send_multi_press_ongoing(chip::EndpointId endpoint, uint8_t new_position, uint8_t count);
esp_err_t send_multi_press_complete(chip::EndpointId endpoint, uint8_t new_position, uint8_t count);
} /* event */

typedef struct config {
    uint8_t number_of_positions;
    uint8_t current_position;
    struct {
        feature::momentary_switch_multi_press::config_t momentary_switch_multi_press;
    } features;
    uint32_t feature_flags;
    config() : number_of_positions(2), current_position(0), feature_flags(0) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* switch_cluster */
} /* cluster */
} /* esp_matter */
