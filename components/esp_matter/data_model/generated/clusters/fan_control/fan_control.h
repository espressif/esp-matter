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
namespace fan_control {

namespace feature {
namespace multi_speed {
typedef struct config {
    uint8_t speed_max;
    nullable<uint8_t> speed_setting;
    uint8_t speed_current;
    config() : speed_max(1), speed_setting(0), speed_current(0) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* multi_speed */

namespace fan_auto {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* fan_auto */

namespace rocking {
typedef struct config {
    uint8_t rock_support;
    uint8_t rock_setting;
    config() : rock_support(0), rock_setting(0) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* rocking */

namespace wind {
typedef struct config {
    uint8_t wind_support;
    uint8_t wind_setting;
    config() : wind_support(0), wind_setting(0) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* wind */

namespace step {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* step */

namespace airflow_direction {
typedef struct config {
    uint8_t airflow_direction;
    config() : airflow_direction(0) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* airflow_direction */

} /* feature */

namespace attribute {
attribute_t *create_fan_mode(cluster_t *cluster, uint8_t value);
attribute_t *create_fan_mode_sequence(cluster_t *cluster, uint8_t value);
attribute_t *create_percent_setting(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_percent_current(cluster_t *cluster, uint8_t value);
attribute_t *create_speed_max(cluster_t *cluster, uint8_t value);
attribute_t *create_speed_setting(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_speed_current(cluster_t *cluster, uint8_t value);
attribute_t *create_rock_support(cluster_t *cluster, uint8_t value);
attribute_t *create_rock_setting(cluster_t *cluster, uint8_t value);
attribute_t *create_wind_support(cluster_t *cluster, uint8_t value);
attribute_t *create_wind_setting(cluster_t *cluster, uint8_t value);
attribute_t *create_airflow_direction(cluster_t *cluster, uint8_t value);
} /* attribute */

namespace command {
command_t *create_step(cluster_t *cluster);
} /* command */

typedef struct config {
    uint8_t fan_mode;
    uint8_t fan_mode_sequence;
    nullable<uint8_t> percent_setting;
    uint8_t percent_current;
    void *delegate;
    config() : fan_mode(0), fan_mode_sequence(0), percent_setting(0), percent_current(0), delegate(nullptr) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* fan_control */
} /* cluster */
} /* esp_matter */
