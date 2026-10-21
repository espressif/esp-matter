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
namespace humidistat {

namespace feature {
namespace humidifier {
typedef struct config {
    nullable<uint8_t> mist_type;
    config() : mist_type() {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* humidifier */

namespace dehumidifier {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* dehumidifier */

namespace continuous {
typedef struct config {
    bool continuous;
    config() : continuous(false) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* continuous */

namespace sensor {
typedef struct config {
    uint8_t user_setpoint;
    uint8_t min_setpoint;
    uint8_t max_setpoint;
    uint8_t step;
    uint8_t target_setpoint;
    config() : user_setpoint(0), min_setpoint(0), max_setpoint(0), step(0), target_setpoint(0) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* sensor */

namespace fan_auto {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* fan_auto */

namespace fan_only {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* fan_only */

namespace optimal {
typedef struct config {
    uint8_t target_setpoint;
    bool optimal;
    config() : target_setpoint(0), optimal(false) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* optimal */

namespace warm_mist {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* warm_mist */

namespace cold_mist {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* cold_mist */

namespace cond_pump {
typedef struct config {
    bool cond_pump_enabled;
    uint16_t cond_run_count;
    config() : cond_pump_enabled(false), cond_run_count(0) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* cond_pump */

} /* feature */

namespace attribute {
attribute_t *create_supported_modes(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_mode(cluster_t *cluster, uint8_t value);
attribute_t *create_system_state(cluster_t *cluster, uint8_t value);
attribute_t *create_user_setpoint(cluster_t *cluster, uint8_t value);
attribute_t *create_min_setpoint(cluster_t *cluster, uint8_t value);
attribute_t *create_max_setpoint(cluster_t *cluster, uint8_t value);
attribute_t *create_step(cluster_t *cluster, uint8_t value);
attribute_t *create_target_setpoint(cluster_t *cluster, uint8_t value);
attribute_t *create_mist_type(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_continuous(cluster_t *cluster, bool value);
attribute_t *create_sleep(cluster_t *cluster, bool value);
attribute_t *create_optimal(cluster_t *cluster, bool value);
attribute_t *create_cond_pump_enabled(cluster_t *cluster, bool value);
attribute_t *create_cond_run_count(cluster_t *cluster, uint16_t value);
} /* attribute */

namespace command {
command_t *create_set_settings(cluster_t *cluster);
} /* command */

typedef struct config {
    uint8_t mode;
    uint8_t system_state;
    struct {
        feature::humidifier::config_t humidifier;
        feature::continuous::config_t continuous;
        feature::sensor::config_t sensor;
        feature::optimal::config_t optimal;
        feature::cond_pump::config_t cond_pump;
    } features;
    uint32_t feature_flags;
    config() : mode(0), system_state(0), feature_flags(0) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* humidistat */
} /* cluster */
} /* esp_matter */
