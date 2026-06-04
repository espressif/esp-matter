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
namespace joint_fabric_datastore {

namespace attribute {
attribute_t *create_anchor_root_ca(cluster_t *cluster, uint8_t * value, uint16_t length);
attribute_t *create_anchor_node_id(cluster_t *cluster, uint64_t value);
attribute_t *create_anchor_vendor_id(cluster_t *cluster, uint16_t value);
attribute_t *create_friendly_name(cluster_t *cluster, char * value, uint16_t length);
attribute_t *create_group_key_set_list(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_group_list(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_node_list(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_admin_list(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_status(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_endpoint_group_id_list(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_endpoint_binding_list(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_node_key_set_list(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_node_acl_list(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_node_endpoint_list(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
} /* attribute */

namespace command {
command_t *create_add_key_set(cluster_t *cluster);
command_t *create_update_key_set(cluster_t *cluster);
command_t *create_remove_key_set(cluster_t *cluster);
command_t *create_add_group(cluster_t *cluster);
command_t *create_update_group(cluster_t *cluster);
command_t *create_remove_group(cluster_t *cluster);
command_t *create_add_admin(cluster_t *cluster);
command_t *create_update_admin(cluster_t *cluster);
command_t *create_remove_admin(cluster_t *cluster);
command_t *create_add_pending_node(cluster_t *cluster);
command_t *create_refresh_node(cluster_t *cluster);
command_t *create_update_node(cluster_t *cluster);
command_t *create_remove_node(cluster_t *cluster);
command_t *create_update_endpoint_for_node(cluster_t *cluster);
command_t *create_add_group_id_to_endpoint_for_node(cluster_t *cluster);
command_t *create_remove_group_id_from_endpoint_for_node(cluster_t *cluster);
command_t *create_add_binding_to_endpoint_for_node(cluster_t *cluster);
command_t *create_remove_binding_from_endpoint_for_node(cluster_t *cluster);
command_t *create_add_acl_to_node(cluster_t *cluster);
command_t *create_remove_acl_from_node(cluster_t *cluster);
} /* command */

typedef struct config {
    config() {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* joint_fabric_datastore */
} /* cluster */
} /* esp_matter */
