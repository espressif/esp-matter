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
namespace level_control {

namespace feature {
namespace on_off {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* on_off */

namespace lighting {
typedef struct config {
    uint16_t remaining_time;
    nullable<uint8_t> start_up_current_level;
    config() : remaining_time(0), start_up_current_level(0) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* lighting */

} /* feature */

namespace attribute {
attribute_t *create_current_level(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_remaining_time(cluster_t *cluster, uint16_t value);
attribute_t *create_min_level(cluster_t *cluster, uint8_t value);
attribute_t *create_max_level(cluster_t *cluster, uint8_t value);
attribute_t *create_options(cluster_t *cluster, uint8_t value);
attribute_t *create_on_off_transition_time(cluster_t *cluster, uint16_t value);
attribute_t *create_on_level(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_on_transition_time(cluster_t *cluster, nullable<uint16_t> value);
attribute_t *create_off_transition_time(cluster_t *cluster, nullable<uint16_t> value);
attribute_t *create_default_move_rate(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_start_up_current_level(cluster_t *cluster, nullable<uint8_t> value);
} /* attribute */

namespace command {
command_t *create_move_to_level(cluster_t *cluster);
command_t *create_move(cluster_t *cluster);
command_t *create_step(cluster_t *cluster);
command_t *create_stop(cluster_t *cluster);
command_t *create_move_to_level_with_on_off(cluster_t *cluster);
command_t *create_move_with_on_off(cluster_t *cluster);
command_t *create_step_with_on_off(cluster_t *cluster);
command_t *create_stop_with_on_off(cluster_t *cluster);
} /* command */

typedef struct config {
    nullable<uint8_t> current_level;
    uint8_t min_level;
    uint8_t max_level;
    uint8_t options;
    nullable<uint8_t> on_level;
    config() : current_level(0), min_level(1), max_level(254), options(0), on_level(0) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* level_control */
} /* cluster */
} /* esp_matter */
