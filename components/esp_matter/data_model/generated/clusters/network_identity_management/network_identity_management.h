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
namespace network_identity_management {

namespace attribute {
attribute_t *create_active_network_identities(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_clients(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_client_table_size(cluster_t *cluster, uint16_t value);
} /* attribute */

namespace command {
command_t *create_add_client(cluster_t *cluster);
command_t *create_add_client_response(cluster_t *cluster);
command_t *create_remove_client(cluster_t *cluster);
command_t *create_query_identity(cluster_t *cluster);
command_t *create_query_identity_response(cluster_t *cluster);
command_t *create_import_admin_secret(cluster_t *cluster);
command_t *create_export_admin_secret(cluster_t *cluster);
command_t *create_export_admin_secret_response(cluster_t *cluster);
} /* command */

typedef struct config {
    uint16_t client_table_size;
    config() : client_table_size(500) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* network_identity_management */
} /* cluster */
} /* esp_matter */
