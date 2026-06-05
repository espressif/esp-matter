/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <esp_err.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef enum {
    RAINMAKER_CONTROLLER_PARAM_TYPE_BOOL,
    RAINMAKER_CONTROLLER_PARAM_TYPE_INT,
    RAINMAKER_CONTROLLER_PARAM_TYPE_STRING,
} rainmaker_controller_param_type_t;

typedef struct {
    const char *name;
    rainmaker_controller_param_type_t type;
    union {
        bool b;
        int i;
        const char *s;
    } value;
} rainmaker_controller_param_update_t;

esp_err_t rainmaker_controller_set_node_params(const char *node_id, const char *device_name,
                                               const rainmaker_controller_param_update_t *updates,
                                               size_t update_count);
esp_err_t rainmaker_controller_get_node_config(const char *node_id, char **node_config);
esp_err_t rainmaker_controller_get_node_params(const char *node_id, char **node_params);
esp_err_t rainmaker_controller_get_node_connection_status(const char *node_id, bool *connection_status);
