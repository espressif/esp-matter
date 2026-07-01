/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <rainmaker_controller_node.h>

#include <esp_err.h>
#include <esp_rmaker_core.h>

#include <stdbool.h>
#include <stddef.h>

typedef esp_err_t (*rainmaker_controller_group_nodes_cb_t)(char *nodes_json, size_t nodes_json_len,
                                                           const char *group_id);
typedef esp_err_t (*rainmaker_controller_node_params_cb_t)(const char *node_id, const char *payload,
                                                           size_t payload_len);
typedef esp_err_t (*rainmaker_controller_node_connectivity_cb_t)(const char *node_id, bool connected);

typedef struct {
    rainmaker_controller_group_nodes_cb_t group_nodes_cb;
    rainmaker_controller_node_params_cb_t node_params_cb;
    rainmaker_controller_node_connectivity_cb_t node_connectivity_cb;
} rainmaker_controller_config_t;

/* Init RainMaker controller services. */
esp_err_t rainmaker_controller_init(esp_rmaker_node_t *node, const rainmaker_controller_config_t *config);

/* Start syncing after Matter bridge initialization is complete. */
void rainmaker_controller_start(void);
