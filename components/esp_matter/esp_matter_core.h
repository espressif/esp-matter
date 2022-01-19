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

#include <esp_err.h>
#include <esp_matter_attribute.h>
#include <esp_matter_command.h>

using chip::DeviceLayer::ChipDeviceEvent;

typedef size_t esp_matter_handle_t;
typedef esp_matter_handle_t esp_matter_node_t;
typedef esp_matter_handle_t esp_matter_endpoint_t;
typedef esp_matter_handle_t esp_matter_cluster_t;
typedef esp_matter_handle_t esp_matter_attribute_t;
typedef esp_matter_handle_t esp_matter_command_t;

typedef enum esp_matter_callback_type {
    ESP_MATTER_CALLBACK_TYPE_PRE_ATTRIBUTE,
    ESP_MATTER_CALLBACK_TYPE_POST_ATTRIBUTE,
} esp_matter_callback_type_t;

/** Callback for attribute value changed
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
typedef esp_err_t (*esp_matter_attribute_callback_t)(esp_matter_callback_type_t type, int endpoint_id, int cluster_id,
                                                     int attribute_id, esp_matter_attr_val_t val, void *priv_data);

typedef void (*esp_matter_event_callback_t)(const ChipDeviceEvent *event, intptr_t arg);

typedef void (*esp_matter_cluster_plugin_server_init_callback_t)();

typedef void (*esp_matter_cluster_plugin_client_init_callback_t)();

/** Initializing APIs */
esp_err_t esp_matter_attribute_callback_set(esp_matter_attribute_callback_t callback, void *priv_data);
esp_err_t esp_matter_start(esp_matter_event_callback_t callback);

/** Node APIs */
esp_matter_node_t *esp_matter_node_create_raw();
esp_matter_node_t *esp_matter_node_get();

/** Endpoint APIs */
esp_matter_endpoint_t *esp_matter_endpoint_create_raw(esp_matter_node_t *node, int endpoint_id, uint8_t flags);
esp_err_t esp_matter_endpoint_delete(esp_matter_node_t *node, esp_matter_endpoint_t *endpoint);
esp_matter_endpoint_t *esp_matter_endpoint_get(esp_matter_node_t *node, int endpoint_id);
esp_matter_endpoint_t *esp_matter_endpoint_get_first(esp_matter_node_t *node);
esp_matter_endpoint_t *esp_matter_endpoint_get_next(esp_matter_endpoint_t *endpoint);
int esp_matter_endpoint_get_id(esp_matter_endpoint_t *endpoint);
/* Endpoint enable: It only needs to be called for endpoints created after calling esp_matter_start(). It should be
 * called after all the clusters, attributes and commands have been added to the created endpoint. */
esp_err_t esp_matter_endpoint_enable(esp_matter_endpoint_t *endpoint);

/** Cluster APIs */
esp_matter_cluster_t *esp_matter_cluster_create(esp_matter_endpoint_t *endpoint, int cluster_id, uint8_t flags);
esp_matter_cluster_t *esp_matter_cluster_get(esp_matter_endpoint_t *endpoint, int cluster_id);
esp_matter_cluster_t *esp_matter_cluster_get_first(esp_matter_endpoint_t *endpoint);
esp_matter_cluster_t *esp_matter_cluster_get_next(esp_matter_cluster_t *cluster);
int esp_matter_cluster_get_id(esp_matter_cluster_t *cluster);
esp_err_t esp_matter_cluster_set_plugin_server_init_callback(esp_matter_cluster_t *cluster,
                                                            esp_matter_cluster_plugin_server_init_callback_t callback);
esp_err_t esp_matter_cluster_set_plugin_client_init_callback(esp_matter_cluster_t *cluster,
                                                            esp_matter_cluster_plugin_client_init_callback_t callback);
esp_matter_cluster_plugin_server_init_callback_t esp_matter_cluster_get_plugin_server_init_callback(
                                                                                        esp_matter_cluster_t *cluster);
esp_matter_cluster_plugin_client_init_callback_t esp_matter_cluster_get_plugin_client_init_callback(
                                                                                        esp_matter_cluster_t *cluster);

/** Attribute APIs */
esp_matter_attribute_t *esp_matter_attribute_create(esp_matter_cluster_t *cluster, int attribute_id, uint8_t flags,
                                                    esp_matter_attr_val_t val);
esp_matter_attribute_t *esp_matter_attribute_get(esp_matter_cluster_t *cluster, int attribute_id);
esp_matter_attribute_t *esp_matter_attribute_get_first(esp_matter_cluster_t *cluster);
esp_matter_attribute_t *esp_matter_attribute_get_next(esp_matter_attribute_t *attribute);
int esp_matter_attribute_get_id(esp_matter_attribute_t *attribute);

/** Attribute val APIs */
esp_err_t esp_matter_attribute_set_val(esp_matter_attribute_t *attribute, esp_matter_attr_val_t val);
esp_matter_attr_val_t esp_matter_attribute_get_val(esp_matter_attribute_t *attribute);
esp_err_t esp_matter_attribute_get_val_raw(int endpoint_id, int cluster_id, int attribute_id, uint8_t *value,
                                           uint16_t attribute_size);

/** Command APIs */
esp_matter_command_t *esp_matter_command_create(esp_matter_cluster_t *cluster, int command_id, uint8_t flags,
                                                esp_matter_command_callback_t callback);
esp_matter_command_t *esp_matter_command_get(esp_matter_cluster_t *cluster, int command_id);
esp_matter_command_t *esp_matter_command_get_first(esp_matter_cluster_t *cluster);
esp_matter_command_t *esp_matter_command_get_next(esp_matter_command_t *command);
int esp_matter_command_get_id(esp_matter_command_t *command);
esp_matter_command_callback_t esp_matter_command_get_callback(esp_matter_command_t *command);
int esp_matter_command_get_flags(esp_matter_command_t *command);
esp_err_t esp_matter_command_set_custom_callback(esp_matter_command_custom_callback_t callback, void *priv_data);
