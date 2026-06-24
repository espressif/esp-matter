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
namespace closure_dimension {

namespace feature {
namespace positioning {
typedef struct config {
    uint16_t resolution;
    uint16_t step_value;
    config() : resolution(0), step_value(0) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* positioning */

namespace motion_latching {
typedef struct config {
    uint8_t latch_control_modes;
    config() : latch_control_modes(0) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* motion_latching */

namespace unit {
typedef struct config {
    uint8_t unit;
    config() : unit(0) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* unit */

namespace limitation {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* limitation */

namespace speed {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* speed */

namespace translation {
typedef struct config {
    uint8_t translation_direction;
    config() : translation_direction(0) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* translation */

namespace rotation {
typedef struct config {
    uint8_t rotation_axis;
    uint8_t overflow;
    config() : rotation_axis(0), overflow(0) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* rotation */

namespace modulation {
typedef struct config {
    uint8_t modulation_type;
    config() : modulation_type(0) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* modulation */

} /* feature */

namespace attribute {
attribute_t *create_current_state(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_target_state(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_resolution(cluster_t *cluster, uint16_t value);
attribute_t *create_step_value(cluster_t *cluster, uint16_t value);
attribute_t *create_unit(cluster_t *cluster, uint8_t value);
attribute_t *create_unit_range(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_limit_range(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_translation_direction(cluster_t *cluster, uint8_t value);
attribute_t *create_rotation_axis(cluster_t *cluster, uint8_t value);
attribute_t *create_overflow(cluster_t *cluster, uint8_t value);
attribute_t *create_modulation_type(cluster_t *cluster, uint8_t value);
attribute_t *create_latch_control_modes(cluster_t *cluster, uint8_t value);
} /* attribute */

namespace command {
command_t *create_set_target(cluster_t *cluster);
command_t *create_step(cluster_t *cluster);
} /* command */

typedef struct config {
    void *delegate;
    struct {
        feature::positioning::config_t positioning;
        feature::motion_latching::config_t motion_latching;
        feature::unit::config_t unit;
        feature::translation::config_t translation;
        feature::rotation::config_t rotation;
        feature::modulation::config_t modulation;
    } features;
    uint32_t feature_flags;
    config() : delegate(nullptr), feature_flags(0) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* closure_dimension */
} /* cluster */
} /* esp_matter */
