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
namespace thermostat_mode {

namespace feature {
namespace core_modes {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* core_modes */

} /* feature */

namespace attribute {
attribute_t *create_supported_modes(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_current_mode(cluster_t *cluster, uint8_t value);
attribute_t *create_start_up_mode(cluster_t *cluster, uint8_t value);
} /* attribute */

namespace command {
command_t *create_change_to_mode(cluster_t *cluster);
command_t *create_change_to_mode_response(cluster_t *cluster);
} /* command */

typedef struct config {
    config() {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* thermostat_mode */
} /* cluster */
} /* esp_matter */
