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
namespace window_covering {

namespace feature {
namespace lift {
typedef struct config {
    nullable<uint16_t> target_position_lift_percent_100ths;
    nullable<uint16_t> current_position_lift_percent_100ths;
    config() : target_position_lift_percent_100ths(), current_position_lift_percent_100ths() {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* lift */

namespace tilt {
typedef struct config {
    nullable<uint16_t> target_position_tilt_percent_100ths;
    nullable<uint16_t> current_position_tilt_percent_100ths;
    config() : target_position_tilt_percent_100ths(), current_position_tilt_percent_100ths() {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* tilt */

namespace position_aware_lift {
typedef struct config {
    nullable<uint16_t> target_position_lift_percent_100ths;
    nullable<uint16_t> current_position_lift_percent_100ths;
    config() : target_position_lift_percent_100ths(), current_position_lift_percent_100ths() {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* position_aware_lift */

namespace position_aware_tilt {
typedef struct config {
    nullable<uint16_t> target_position_tilt_percent_100ths;
    nullable<uint16_t> current_position_tilt_percent_100ths;
    config() : target_position_tilt_percent_100ths(), current_position_tilt_percent_100ths() {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* position_aware_tilt */

} /* feature */

namespace attribute {
attribute_t *create_type(cluster_t *cluster, uint8_t value);
attribute_t *create_number_of_actuations_lift(cluster_t *cluster, uint16_t value);
attribute_t *create_number_of_actuations_tilt(cluster_t *cluster, uint16_t value);
attribute_t *create_config_status(cluster_t *cluster, uint8_t value);
attribute_t *create_current_position_lift_percentage(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_current_position_tilt_percentage(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_operational_status(cluster_t *cluster, uint8_t value);
attribute_t *create_target_position_lift_percent_100ths(cluster_t *cluster, nullable<uint16_t> value);
attribute_t *create_target_position_tilt_percent_100ths(cluster_t *cluster, nullable<uint16_t> value);
attribute_t *create_end_product_type(cluster_t *cluster, uint8_t value);
attribute_t *create_current_position_lift_percent_100ths(cluster_t *cluster, nullable<uint16_t> value);
attribute_t *create_current_position_tilt_percent_100ths(cluster_t *cluster, nullable<uint16_t> value);
attribute_t *create_mode(cluster_t *cluster, uint8_t value);
attribute_t *create_safety_status(cluster_t *cluster, uint16_t value);
} /* attribute */

namespace command {
command_t *create_up_or_open(cluster_t *cluster);
command_t *create_down_or_close(cluster_t *cluster);
command_t *create_stop_motion(cluster_t *cluster);
command_t *create_go_to_lift_percentage(cluster_t *cluster);
command_t *create_go_to_tilt_percentage(cluster_t *cluster);
} /* command */

typedef struct config {
    uint8_t type;
    uint8_t config_status;
    uint8_t operational_status;
    uint8_t end_product_type;
    uint8_t mode;
    void *delegate;
    struct {
        feature::lift::config_t lift;
        feature::tilt::config_t tilt;
        feature::position_aware_lift::config_t position_aware_lift;
        feature::position_aware_tilt::config_t position_aware_tilt;
    } features;
    uint32_t feature_flags;
    config() : type(0), config_status(0), operational_status(0), end_product_type(0), mode(0), delegate(nullptr), feature_flags(0) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* window_covering */
} /* cluster */
} /* esp_matter */
