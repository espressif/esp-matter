// Copyright 2021 Espressif Systems (Shanghai) CO LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License

#include <esp_log.h>
#include <iot_button.h>

static const char *TAG = "button_driver_hollow";

button_handle_t iot_button_create(const button_config_t *config)
{
    ESP_LOGI(TAG, "Button create");
    return NULL;
}

esp_err_t iot_button_delete(button_handle_t btn_handle)
{
    ESP_LOGI(TAG, "Button delete");
    return ESP_OK;
}

esp_err_t iot_button_register_cb(button_handle_t btn_handle, button_event_t event, button_cb_t cb, void *usr_data)
{
    ESP_LOGI(TAG, "Button register callback");
    return ESP_OK;
}

esp_err_t iot_button_unregister_cb(button_handle_t btn_handle, button_event_t event)
{
    ESP_LOGI(TAG, "Button unregister callback");
    return ESP_OK;
}

button_event_t iot_button_get_event(button_handle_t btn_handle)
{
    ESP_LOGI(TAG, "Button get event");
    return 0;
}

uint8_t iot_button_get_repeat(button_handle_t btn_handle)
{
    ESP_LOGI(TAG, "Button get repeat");
    return 0;
}
