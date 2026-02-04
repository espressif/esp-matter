/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_rmaker_core.h>
#include <rainmaker_controller_std.h>

esp_rmaker_param_t *rainmaker_controller_base_url_param_create(const char *param_name)
{
    esp_rmaker_param_t *param = esp_rmaker_param_create(param_name, ESP_RMAKER_PARAM_BASE_URL,
                                                        esp_rmaker_str(""), PROP_FLAG_READ | PROP_FLAG_WRITE | PROP_FLAG_PERSIST);
    return param;
}

esp_rmaker_param_t *rainmaker_controller_user_token_param_create(const char *param_name)
{
    esp_rmaker_param_t *param = esp_rmaker_param_create(param_name, ESP_RMAKER_PARAM_USER_TOKEN,
                                                        esp_rmaker_str(""), PROP_FLAG_WRITE | PROP_FLAG_PERSIST);
    return param;
}

esp_rmaker_device_t *rainmaker_controller_service_create(const char *serv_name, esp_rmaker_device_write_cb_t write_cb,
                                                         esp_rmaker_device_read_cb_t read_cb, void *priv_data)
{
    esp_rmaker_device_t *service = esp_rmaker_service_create(serv_name, ESP_RMAKER_SERVICE_CONTROLLER, priv_data);
    if (service) {
        esp_rmaker_device_add_cb(service, write_cb, read_cb);
        esp_rmaker_device_add_param(service, rainmaker_controller_base_url_param_create(ESP_RMAKER_DEF_BASE_URL_NAME));
        esp_rmaker_device_add_param(service, rainmaker_controller_user_token_param_create(ESP_RMAKER_DEF_USER_TOKEN_NAME));
    }
    return service;
}
