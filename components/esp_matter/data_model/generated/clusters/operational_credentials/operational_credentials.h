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
namespace operational_credentials {

namespace attribute {
attribute_t *create_no_cs(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_fabrics(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_supported_fabrics(cluster_t *cluster, uint8_t value);
attribute_t *create_commissioned_fabrics(cluster_t *cluster, uint8_t value);
attribute_t *create_trusted_root_certificates(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_current_fabric_index(cluster_t *cluster, uint8_t value);
} /* attribute */

namespace command {
command_t *create_attestation_request(cluster_t *cluster);
command_t *create_attestation_response(cluster_t *cluster);
command_t *create_certificate_chain_request(cluster_t *cluster);
command_t *create_certificate_chain_response(cluster_t *cluster);
command_t *create_csr_request(cluster_t *cluster);
command_t *create_csr_response(cluster_t *cluster);
command_t *create_add_noc(cluster_t *cluster);
command_t *create_update_noc(cluster_t *cluster);
command_t *create_noc_response(cluster_t *cluster);
command_t *create_update_fabric_label(cluster_t *cluster);
command_t *create_remove_fabric(cluster_t *cluster);
command_t *create_add_trusted_root_certificate(cluster_t *cluster);
command_t *create_set_vid_verification_statement(cluster_t *cluster);
command_t *create_sign_vid_verification_request(cluster_t *cluster);
command_t *create_sign_vid_verification_response(cluster_t *cluster);
} /* command */

typedef struct config {
    uint8_t supported_fabrics;
    uint8_t commissioned_fabrics;
    uint8_t current_fabric_index;
    config() : supported_fabrics(5), commissioned_fabrics(0), current_fabric_index(0) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* operational_credentials */
} /* cluster */
} /* esp_matter */
