/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <esp_err.h>
#include <esp_rmaker_core.h>

// Rainmaker Controller device
#define ESP_RMAKER_DEVICE_CONTROLLER     "esp.device.controller"

// Rainmaker Controller service
#define ESP_RMAKER_SERVICE_CONTROLLER    "esp.service.rmaker-user-auth"

// Rainmaker Controller parameters
#define ESP_RMAKER_DEF_BASE_URL_NAME            "BaseURL"
#define ESP_RMAKER_PARAM_BASE_URL               "esp.param.base-url"
#define ESP_RMAKER_DEF_USER_TOKEN_NAME          "UserToken"
#define ESP_RMAKER_PARAM_USER_TOKEN             "esp.param.user-token"

esp_rmaker_param_t *rainmaker_controller_base_url_param_create(const char *param_name);
esp_rmaker_param_t *rainmaker_controller_user_token_param_create(const char *param_name);

esp_rmaker_device_t *rainmaker_controller_service_create(const char *serv_name, esp_rmaker_device_write_cb_t write_cb,
                                                         esp_rmaker_device_read_cb_t read_cb, void *priv_data);
