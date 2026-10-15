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
namespace water_heater_mode {

namespace attribute {
attribute_t *create_supported_modes(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_current_mode(cluster_t *cluster, uint8_t value);
} /* attribute */

namespace command {
command_t *create_change_to_mode(cluster_t *cluster);
command_t *create_change_to_mode_response(cluster_t *cluster);
} /* command */

typedef struct config {
    uint8_t current_mode;
    void *delegate;
    config() : current_mode(0), delegate(nullptr) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* water_heater_mode */
} /* cluster */
} /* esp_matter */
