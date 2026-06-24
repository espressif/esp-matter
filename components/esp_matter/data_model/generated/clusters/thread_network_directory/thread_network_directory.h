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
namespace thread_network_directory {

const uint8_t k_max_preferred_extended_pan_id_length = 8u;
namespace attribute {
attribute_t *create_preferred_extended_pan_id(cluster_t *cluster, uint8_t * value, uint16_t length);
attribute_t *create_thread_networks(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_thread_network_table_size(cluster_t *cluster, uint8_t value);
} /* attribute */

namespace command {
command_t *create_add_network(cluster_t *cluster);
command_t *create_remove_network(cluster_t *cluster);
command_t *create_get_operational_dataset(cluster_t *cluster);
command_t *create_operational_dataset_response(cluster_t *cluster);
} /* command */

typedef struct config {
    uint8_t preferred_extended_pan_id[k_max_preferred_extended_pan_id_length];
    uint8_t thread_network_table_size;
    config() : preferred_extended_pan_id{0}, thread_network_table_size(0) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* thread_network_directory */
} /* cluster */
} /* esp_matter */
