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
namespace joint_fabric_administrator {

namespace attribute {
attribute_t *create_administrator_fabric_index(cluster_t *cluster, nullable<uint8_t> value);
} /* attribute */

namespace command {
command_t *create_icaccsr_request(cluster_t *cluster);
command_t *create_icaccsr_response(cluster_t *cluster);
command_t *create_add_icac(cluster_t *cluster);
command_t *create_icac_response(cluster_t *cluster);
command_t *create_open_joint_commissioning_window(cluster_t *cluster);
command_t *create_transfer_anchor_request(cluster_t *cluster);
command_t *create_transfer_anchor_response(cluster_t *cluster);
command_t *create_transfer_anchor_complete(cluster_t *cluster);
command_t *create_announce_joint_fabric_administrator(cluster_t *cluster);
} /* command */

typedef struct config {
    nullable<uint8_t> administrator_fabric_index;
    config() : administrator_fabric_index(0) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* joint_fabric_administrator */
} /* cluster */
} /* esp_matter */
