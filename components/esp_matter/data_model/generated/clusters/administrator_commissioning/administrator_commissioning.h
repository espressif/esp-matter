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
namespace administrator_commissioning {

namespace feature {
namespace basic {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* basic */

} /* feature */

namespace attribute {
attribute_t *create_window_status(cluster_t *cluster, uint8_t value);
attribute_t *create_admin_fabric_index(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_admin_vendor_id(cluster_t *cluster, nullable<uint16_t> value);
} /* attribute */

namespace command {
command_t *create_open_commissioning_window(cluster_t *cluster);
command_t *create_open_basic_commissioning_window(cluster_t *cluster);
command_t *create_revoke_commissioning(cluster_t *cluster);
} /* command */

typedef struct config {
    uint8_t window_status;
    nullable<uint8_t> admin_fabric_index;
    nullable<uint16_t> admin_vendor_id;
    config() : window_status(0), admin_fabric_index(), admin_vendor_id() {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* administrator_commissioning */
} /* cluster */
} /* esp_matter */
