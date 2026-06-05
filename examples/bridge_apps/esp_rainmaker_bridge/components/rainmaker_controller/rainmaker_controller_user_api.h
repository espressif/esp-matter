/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <esp_err.h>

#include <stdbool.h>

/* Internal rainmaker_controller component API. Do not include from app code. */
bool rainmaker_controller_is_user_api_ready(void);
esp_err_t rainmaker_controller_user_api_init_or_update(void);
void rainmaker_controller_user_api_reset_login(void);
esp_err_t rainmaker_controller_get_bridge_group_nodes(const char *group_id, char **nodes_buffer);
esp_err_t rainmaker_controller_send_node_params(const char *payload);
