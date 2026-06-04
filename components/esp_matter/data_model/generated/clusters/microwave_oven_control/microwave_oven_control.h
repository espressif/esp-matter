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
namespace microwave_oven_control {

namespace feature {
namespace power_as_number {
typedef struct config {
    uint8_t power_setting;
    config() : power_setting(0) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* power_as_number */

namespace power_number_limits {
typedef struct config {
    uint8_t min_power;
    uint8_t max_power;
    uint8_t power_step;
    config() : min_power(0), max_power(0), power_step(0) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* power_number_limits */

} /* feature */

namespace attribute {
attribute_t *create_cook_time(cluster_t *cluster, uint32_t value);
attribute_t *create_max_cook_time(cluster_t *cluster, uint32_t value);
attribute_t *create_power_setting(cluster_t *cluster, uint8_t value);
attribute_t *create_min_power(cluster_t *cluster, uint8_t value);
attribute_t *create_max_power(cluster_t *cluster, uint8_t value);
attribute_t *create_power_step(cluster_t *cluster, uint8_t value);
attribute_t *create_watt_rating(cluster_t *cluster, uint16_t value);
} /* attribute */

namespace command {
command_t *create_set_cooking_parameters(cluster_t *cluster);
command_t *create_add_more_time(cluster_t *cluster);
} /* command */

typedef struct config {
    uint32_t cook_time;
    uint32_t max_cook_time;
    void *delegate;
    struct {
        feature::power_as_number::config_t power_as_number;
        feature::power_number_limits::config_t power_number_limits;
    } features;
    uint32_t feature_flags;
    config() : cook_time(0), max_cook_time(0), delegate(nullptr), feature_flags(0) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* microwave_oven_control */
} /* cluster */
} /* esp_matter */
