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
namespace oven_cavity_operational_state {

namespace attribute {
attribute_t *create_phase_list(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_current_phase(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_countdown_time(cluster_t *cluster, nullable<uint32_t> value);
attribute_t *create_operational_state_list(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_operational_state(cluster_t *cluster, uint8_t value);
attribute_t *create_operational_error(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
} /* attribute */

namespace command {
command_t *create_stop(cluster_t *cluster);
command_t *create_start(cluster_t *cluster);
command_t *create_operational_command_response(cluster_t *cluster);
} /* command */

namespace event {
event_t *create_operational_error(cluster_t *cluster);
event_t *create_operation_completion(cluster_t *cluster);
} /* event */

typedef struct config {
    nullable<uint8_t> current_phase;
    uint8_t operational_state;
    void *delegate;
    config() : current_phase(0), operational_state(0), delegate(nullptr) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* oven_cavity_operational_state */
} /* cluster */
} /* esp_matter */
