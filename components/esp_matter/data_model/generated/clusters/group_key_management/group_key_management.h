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
namespace group_key_management {

namespace attribute {
attribute_t *create_group_key_map(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_group_table(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_max_groups_per_fabric(cluster_t *cluster, uint16_t value);
attribute_t *create_max_group_keys_per_fabric(cluster_t *cluster, uint16_t value);
} /* attribute */

namespace command {
command_t *create_key_set_write(cluster_t *cluster);
command_t *create_key_set_read(cluster_t *cluster);
command_t *create_key_set_read_response(cluster_t *cluster);
command_t *create_key_set_remove(cluster_t *cluster);
command_t *create_key_set_read_all_indices(cluster_t *cluster);
command_t *create_key_set_read_all_indices_response(cluster_t *cluster);
} /* command */

typedef struct config {
    uint16_t max_groups_per_fabric;
    uint16_t max_group_keys_per_fabric;
    config() : max_groups_per_fabric(0), max_group_keys_per_fabric(1) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* group_key_management */
} /* cluster */
} /* esp_matter */
