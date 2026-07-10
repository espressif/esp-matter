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
namespace tls_client_management {

namespace attribute {
attribute_t *create_max_provisioned(cluster_t *cluster, uint8_t value);
attribute_t *create_provisioned_endpoints(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
} /* attribute */

namespace command {
command_t *create_provision_endpoint(cluster_t *cluster);
command_t *create_provision_endpoint_response(cluster_t *cluster);
command_t *create_find_endpoint(cluster_t *cluster);
command_t *create_find_endpoint_response(cluster_t *cluster);
command_t *create_remove_endpoint(cluster_t *cluster);
} /* command */

typedef struct config {
    uint8_t max_provisioned;
    config() : max_provisioned(5) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* tls_client_management */
} /* cluster */
} /* esp_matter */
