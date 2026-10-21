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
namespace electrical_alarm {

namespace feature {
namespace reset {
typedef struct config {
    uint32_t latch;
    config() : latch(0) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* reset */

namespace adjustable_thresholds {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* adjustable_thresholds */

namespace over_voltage {
typedef struct config {
    int64_t over_voltage_threshold;
    config() : over_voltage_threshold(4611686018427387904) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* over_voltage */

namespace under_voltage {
typedef struct config {
    int64_t under_voltage_threshold;
    config() : under_voltage_threshold(0) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* under_voltage */

namespace over_frequency {
typedef struct config {
    int64_t over_frequency_threshold;
    config() : over_frequency_threshold(1000000) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* over_frequency */

namespace under_frequency {
typedef struct config {
    int64_t under_frequency_threshold;
    config() : under_frequency_threshold(0) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* under_frequency */

namespace over_power {
typedef struct config {
    int64_t over_power_threshold;
    config() : over_power_threshold(4611686018427387904) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* over_power */

namespace under_power {
typedef struct config {
    int64_t under_power_threshold;
    config() : under_power_threshold(-4611686018427387904) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* under_power */

namespace over_current {
typedef struct config {
    int64_t over_current_threshold;
    config() : over_current_threshold(4611686018427387904) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* over_current */

namespace under_current {
typedef struct config {
    int64_t under_current_threshold;
    config() : under_current_threshold(-4611686018427387904) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* under_current */

namespace power_import {
typedef struct config {
    int64_t power_import_threshold;
    config() : power_import_threshold(4611686018427387904) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* power_import */

namespace power_export {
typedef struct config {
    int64_t power_export_threshold;
    config() : power_export_threshold(-4611686018427387904) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* power_export */

} /* feature */

namespace attribute {
attribute_t *create_mask(cluster_t *cluster, uint32_t value);
attribute_t *create_latch(cluster_t *cluster, uint32_t value);
attribute_t *create_state(cluster_t *cluster, uint32_t value);
attribute_t *create_supported(cluster_t *cluster, uint32_t value);
attribute_t *create_over_voltage_threshold(cluster_t *cluster, int64_t value);
attribute_t *create_under_voltage_threshold(cluster_t *cluster, int64_t value);
attribute_t *create_over_frequency_threshold(cluster_t *cluster, int64_t value);
attribute_t *create_under_frequency_threshold(cluster_t *cluster, int64_t value);
attribute_t *create_over_power_threshold(cluster_t *cluster, int64_t value);
attribute_t *create_under_power_threshold(cluster_t *cluster, int64_t value);
attribute_t *create_over_current_threshold(cluster_t *cluster, int64_t value);
attribute_t *create_under_current_threshold(cluster_t *cluster, int64_t value);
attribute_t *create_power_import_threshold(cluster_t *cluster, int64_t value);
attribute_t *create_power_export_threshold(cluster_t *cluster, int64_t value);
} /* attribute */

namespace command {
command_t *create_reset(cluster_t *cluster);
command_t *create_modify_enabled_alarms(cluster_t *cluster);
command_t *create_set_electrical_alarm_thresholds(cluster_t *cluster);
} /* command */

namespace event {
event_t *create_notify(cluster_t *cluster);
} /* event */

typedef struct config {
    uint32_t mask;
    uint32_t state;
    uint32_t supported;
    struct {
        feature::reset::config_t reset;
        feature::over_voltage::config_t over_voltage;
        feature::under_voltage::config_t under_voltage;
        feature::over_frequency::config_t over_frequency;
        feature::under_frequency::config_t under_frequency;
        feature::over_power::config_t over_power;
        feature::under_power::config_t under_power;
        feature::over_current::config_t over_current;
        feature::under_current::config_t under_current;
        feature::power_import::config_t power_import;
        feature::power_export::config_t power_export;
    } features;
    uint32_t feature_flags;
    config() : mask(0), state(0), supported(0), feature_flags(0) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* electrical_alarm */
} /* cluster */
} /* esp_matter */
