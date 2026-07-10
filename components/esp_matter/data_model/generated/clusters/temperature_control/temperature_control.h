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
namespace temperature_control {

namespace feature {
namespace temperature_number {
typedef struct config {
    int16_t temperature_setpoint;
    int16_t min_temperature;
    int16_t max_temperature;
    config() : temperature_setpoint(0), min_temperature(0), max_temperature(0) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* temperature_number */

namespace temperature_level {
typedef struct config {
    uint8_t selected_temperature_level;
    config() : selected_temperature_level(0) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* temperature_level */

namespace temperature_step {
typedef struct config {
    int16_t step;
    config() : step(1) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* temperature_step */

} /* feature */

namespace attribute {
attribute_t *create_temperature_setpoint(cluster_t *cluster, int16_t value);
attribute_t *create_min_temperature(cluster_t *cluster, int16_t value);
attribute_t *create_max_temperature(cluster_t *cluster, int16_t value);
attribute_t *create_step(cluster_t *cluster, int16_t value);
attribute_t *create_selected_temperature_level(cluster_t *cluster, uint8_t value);
attribute_t *create_supported_temperature_levels(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
} /* attribute */

namespace command {
command_t *create_set_temperature(cluster_t *cluster);
} /* command */

typedef struct config {
    void *delegate;
    struct {
        feature::temperature_number::config_t temperature_number;
        feature::temperature_level::config_t temperature_level;
        feature::temperature_step::config_t temperature_step;
    } features;
    uint32_t feature_flags;
    config() : delegate(nullptr), feature_flags(0) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* temperature_control */
} /* cluster */
} /* esp_matter */
