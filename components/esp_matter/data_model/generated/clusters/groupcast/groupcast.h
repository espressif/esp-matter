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
namespace groupcast {

namespace feature {
namespace listener {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* listener */

namespace sender {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* sender */

namespace per_group {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* per_group */

} /* feature */

namespace attribute {
attribute_t *create_membership(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_max_membership_count(cluster_t *cluster, uint16_t value);
attribute_t *create_max_mcast_addr_count(cluster_t *cluster, uint16_t value);
attribute_t *create_used_mcast_addr_count(cluster_t *cluster, uint16_t value);
attribute_t *create_fabric_under_test(cluster_t *cluster, uint8_t value);
} /* attribute */

namespace command {
command_t *create_join_group(cluster_t *cluster);
command_t *create_leave_group(cluster_t *cluster);
command_t *create_leave_group_response(cluster_t *cluster);
command_t *create_update_group_key(cluster_t *cluster);
command_t *create_configure_auxiliary_acl(cluster_t *cluster);
command_t *create_groupcast_testing(cluster_t *cluster);
} /* command */

namespace event {
event_t *create_groupcast_testing(cluster_t *cluster);
} /* event */

typedef struct config {
    uint16_t max_membership_count;
    uint16_t max_mcast_addr_count;
    uint16_t used_mcast_addr_count;
    uint8_t fabric_under_test;
    uint32_t feature_flags;
    config() : max_membership_count(10), max_mcast_addr_count(1), used_mcast_addr_count(0), fabric_under_test(0), feature_flags(0) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* groupcast */
} /* cluster */
} /* esp_matter */
