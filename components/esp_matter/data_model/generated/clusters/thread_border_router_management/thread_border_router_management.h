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
namespace thread_border_router_management {

const uint8_t k_max_border_router_name_length = 63u;
const uint8_t k_max_border_agent_id_length = 16u;
namespace feature {
namespace pan_change {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* pan_change */

} /* feature */

namespace attribute {
attribute_t *create_border_router_name(cluster_t *cluster, char * value, uint16_t length);
attribute_t *create_border_agent_id(cluster_t *cluster, uint8_t * value, uint16_t length);
attribute_t *create_thread_version(cluster_t *cluster, uint16_t value);
attribute_t *create_interface_enabled(cluster_t *cluster, bool value);
attribute_t *create_active_dataset_timestamp(cluster_t *cluster, nullable<uint64_t> value);
attribute_t *create_pending_dataset_timestamp(cluster_t *cluster, nullable<uint64_t> value);
} /* attribute */

namespace command {
command_t *create_get_active_dataset_request(cluster_t *cluster);
command_t *create_get_pending_dataset_request(cluster_t *cluster);
command_t *create_dataset_response(cluster_t *cluster);
command_t *create_set_active_dataset_request(cluster_t *cluster);
command_t *create_set_pending_dataset_request(cluster_t *cluster);
} /* command */

typedef struct config {
    char border_router_name[k_max_border_router_name_length + 1];
    uint8_t border_agent_id[k_max_border_agent_id_length];
    uint16_t thread_version;
    bool interface_enabled;
    nullable<uint64_t> active_dataset_timestamp;
    nullable<uint64_t> pending_dataset_timestamp;
    void *delegate;
    config() : border_router_name{0}, border_agent_id{0}, thread_version(0), interface_enabled(false), active_dataset_timestamp(0), pending_dataset_timestamp(0), delegate(nullptr) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* thread_border_router_management */
} /* cluster */
} /* esp_matter */
