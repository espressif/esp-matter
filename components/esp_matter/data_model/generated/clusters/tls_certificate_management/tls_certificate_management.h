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
namespace tls_certificate_management {

namespace attribute {
attribute_t *create_max_root_certificates(cluster_t *cluster, uint8_t value);
attribute_t *create_provisioned_root_certificates(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_max_client_certificates(cluster_t *cluster, uint8_t value);
attribute_t *create_provisioned_client_certificates(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
} /* attribute */

namespace command {
command_t *create_provision_root_certificate(cluster_t *cluster);
command_t *create_provision_root_certificate_response(cluster_t *cluster);
command_t *create_find_root_certificate(cluster_t *cluster);
command_t *create_find_root_certificate_response(cluster_t *cluster);
command_t *create_lookup_root_certificate(cluster_t *cluster);
command_t *create_lookup_root_certificate_response(cluster_t *cluster);
command_t *create_remove_root_certificate(cluster_t *cluster);
command_t *create_client_csr(cluster_t *cluster);
command_t *create_client_csr_response(cluster_t *cluster);
command_t *create_provision_client_certificate(cluster_t *cluster);
command_t *create_find_client_certificate(cluster_t *cluster);
command_t *create_find_client_certificate_response(cluster_t *cluster);
command_t *create_lookup_client_certificate(cluster_t *cluster);
command_t *create_lookup_client_certificate_response(cluster_t *cluster);
command_t *create_remove_client_certificate(cluster_t *cluster);
} /* command */

typedef struct config {
    uint8_t max_root_certificates;
    uint8_t max_client_certificates;
    config() : max_root_certificates(5), max_client_certificates(2) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* tls_certificate_management */
} /* cluster */
} /* esp_matter */
