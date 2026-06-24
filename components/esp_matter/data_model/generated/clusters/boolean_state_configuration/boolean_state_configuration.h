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
namespace boolean_state_configuration {

namespace feature {
namespace visual {
typedef struct config {
    uint8_t alarms_active;
    uint8_t alarms_supported;
    config() : alarms_active(0), alarms_supported(0) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* visual */

namespace audible {
typedef struct config {
    uint8_t alarms_active;
    uint8_t alarms_supported;
    config() : alarms_active(0), alarms_supported(0) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* audible */

namespace alarm_suppress {
typedef struct config {
    uint8_t alarms_suppressed;
    config() : alarms_suppressed(0) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* alarm_suppress */

namespace sensitivity_level {
typedef struct config {
    uint8_t current_sensitivity_level;
    uint8_t supported_sensitivity_levels;
    config() : current_sensitivity_level(0), supported_sensitivity_levels(0) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* sensitivity_level */

namespace fault_events {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* fault_events */

} /* feature */

namespace attribute {
attribute_t *create_current_sensitivity_level(cluster_t *cluster, uint8_t value);
attribute_t *create_supported_sensitivity_levels(cluster_t *cluster, uint8_t value);
attribute_t *create_default_sensitivity_level(cluster_t *cluster, uint8_t value);
attribute_t *create_alarms_active(cluster_t *cluster, uint8_t value);
attribute_t *create_alarms_suppressed(cluster_t *cluster, uint8_t value);
attribute_t *create_alarms_enabled(cluster_t *cluster, uint8_t value);
attribute_t *create_alarms_supported(cluster_t *cluster, uint8_t value);
attribute_t *create_sensor_fault(cluster_t *cluster, uint8_t value);
} /* attribute */

namespace command {
command_t *create_suppress_alarm(cluster_t *cluster);
command_t *create_enable_disable_alarm(cluster_t *cluster);
} /* command */

namespace event {
event_t *create_alarms_state_changed(cluster_t *cluster);
event_t *create_sensor_fault(cluster_t *cluster);
} /* event */

typedef struct config {
    void *delegate;
    config() : delegate(nullptr) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* boolean_state_configuration */
} /* cluster */
} /* esp_matter */
