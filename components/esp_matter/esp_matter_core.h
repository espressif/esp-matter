// Copyright 2021 Espressif Systems (Shanghai) PTE LTD
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

#pragma once

#include <app/DeviceProxy.h>
#include <app/InteractionModelEngine.h>
#include <app/util/af-types.h>
#include <esp_err.h>
#include <esp_matter_attribute_utils.h>

using chip::app::ConcreteCommandPath;
using chip::DeviceLayer::ChipDeviceEvent;
using chip::TLV::TLVReader;

namespace esp_matter {

typedef size_t handle_t;
typedef handle_t node_t;
typedef handle_t endpoint_t;
typedef handle_t cluster_t;
typedef handle_t attribute_t;
typedef handle_t command_t;

typedef void (*event_callback_t)(const ChipDeviceEvent *event, intptr_t arg);

esp_err_t start(event_callback_t callback);

namespace lock {
typedef enum status {
    FAILED,
    ALREADY_TAKEN,
    SUCCESS,
} status_t;

status_t chip_stack_lock(uint32_t ticks_to_wait);
/* unlock() should only be called if chip_stack_lock() returns SUCCESS */
esp_err_t chip_stack_unlock();
} /* lock */

namespace node {
node_t *create_raw();
node_t *get();
} /* node */

namespace endpoint {
endpoint_t *create(node_t *node, uint8_t flags);
esp_err_t destroy(node_t *node, endpoint_t *endpoint);
endpoint_t *get(node_t *node, int endpoint_id);
endpoint_t *get_first(node_t *node);
endpoint_t *get_next(endpoint_t *endpoint);
int get_id(endpoint_t *endpoint);
esp_err_t set_device_type_id(endpoint_t *endpoint, int device_type_id);
int get_device_type_id(int endpoint_id);
/* Endpoint enable: It only needs to be called for endpoints created after calling esp_matter::start(). It should be
 * called after all the clusters, attributes and commands have been added to the created endpoint. */
esp_err_t enable(endpoint_t *endpoint);
} /* endpoint */

namespace cluster {
typedef void (*plugin_server_init_callback_t)();
typedef void (*plugin_client_init_callback_t)();
typedef void (*function_generic_t)();

cluster_t *create(endpoint_t *endpoint, int cluster_id, uint8_t flags);
cluster_t *get(endpoint_t *endpoint, int cluster_id);
cluster_t *get_first(endpoint_t *endpoint);
cluster_t *get_next(cluster_t *cluster);
int get_id(cluster_t *cluster);
esp_err_t set_plugin_server_init_callback(cluster_t *cluster, plugin_server_init_callback_t callback);
esp_err_t set_plugin_client_init_callback(cluster_t *cluster, plugin_client_init_callback_t callback);
plugin_server_init_callback_t get_plugin_server_init_callback(cluster_t *cluster);
plugin_client_init_callback_t get_plugin_client_init_callback(cluster_t *cluster);
esp_err_t add_function_list(cluster_t *cluster, const function_generic_t *function_list, int function_flags);
} /* cluster */

namespace attribute {
typedef enum callback_type {
    PRE_ATTRIBUTE,
    POST_ATTRIBUTE,
} callback_type_t;

/** Callback for attribute value changed
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
typedef esp_err_t (*callback_t)(callback_type_t type, int endpoint_id, int cluster_id, int attribute_id,
                                esp_matter_attr_val_t *val, void *priv_data);
esp_err_t set_callback(callback_t callback, void *priv_data);

attribute_t *create(cluster_t *cluster, int attribute_id, uint8_t flags, esp_matter_attr_val_t val);
attribute_t *get(cluster_t *cluster, int attribute_id);
attribute_t *get_first(cluster_t *cluster);
attribute_t *get_next(attribute_t *attribute);
int get_id(attribute_t *attribute);

/** Attribute val APIs */
esp_err_t set_val(attribute_t *attribute, esp_matter_attr_val_t *val);
esp_err_t get_val(attribute_t *attribute, esp_matter_attr_val_t *val);
esp_err_t get_val_raw(int endpoint_id, int cluster_id, int attribute_id, uint8_t *value, uint16_t attribute_size);
esp_err_t add_bounds(attribute_t *attribute, esp_matter_attr_val_t min, esp_matter_attr_val_t max);
esp_matter_attr_bounds_t *get_bounds(attribute_t *attribute);

} /* attribute */

namespace command {
typedef esp_err_t (*callback_t)(const ConcreteCommandPath &command_path, TLVReader &tlv_data, void *opaque_ptr);

command_t *create(cluster_t *cluster, int command_id, uint8_t flags, callback_t callback);
command_t *get(cluster_t *cluster, int command_id);
command_t *get_first(cluster_t *cluster);
command_t *get_next(command_t *command);
int get_id(command_t *command);
callback_t get_callback(command_t *command);
int get_flags(command_t *command);

} /* command */

/* Client APIs */
namespace client {
typedef chip::DeviceProxy peer_device_t;
typedef void (*command_callback_t)(peer_device_t *peer_device, int remote_endpoint_id, void *priv_data);

void binding_init();
void binding_manager_init();
esp_err_t connect(int fabric_index, int node_id, int remote_endpoint_id);
esp_err_t set_command_callback(command_callback_t callback, void *priv_data);
esp_err_t cluster_update(int endpoint_id, int cluster_id);
} /* client */
} /* esp_matter */
