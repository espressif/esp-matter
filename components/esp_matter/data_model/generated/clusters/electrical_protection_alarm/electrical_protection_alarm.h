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
namespace electrical_protection_alarm {

namespace feature {
namespace short_circuit {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* short_circuit */

namespace over_load {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* over_load */

namespace over_voltage {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* over_voltage */

namespace surge_protection {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* surge_protection */

namespace residual_current {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* residual_current */

namespace arc_fault {
typedef struct config {
    nullable<uint8_t> arc_cause;
    config() : arc_cause() {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* arc_fault */

namespace self_test {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* self_test */

} /* feature */

namespace attribute {
attribute_t *create_mask(cluster_t *cluster, uint32_t value);
attribute_t *create_state(cluster_t *cluster, uint32_t value);
attribute_t *create_supported(cluster_t *cluster, uint32_t value);
attribute_t *create_arc_cause(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_over_load_rating(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_over_voltage_rating(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_surge_protection_rating(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_short_circuit_rating(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_residual_current_rating(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_arc_fault_rating(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
} /* attribute */

namespace command {
command_t *create_modify_enabled_alarms(cluster_t *cluster);
} /* command */

namespace event {
event_t *create_notify(cluster_t *cluster);
} /* event */

typedef struct config {
    uint32_t mask;
    uint32_t state;
    uint32_t supported;
    struct {
        feature::arc_fault::config_t arc_fault;
    } features;
    uint32_t feature_flags;
    config() : mask(0), state(0), supported(0), feature_flags(0) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* electrical_protection_alarm */
} /* cluster */
} /* esp_matter */
