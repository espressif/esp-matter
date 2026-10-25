// Copyright 2026 Espressif Systems (Shanghai) CO LTD
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

#include <esp_log.h>

#include <led_driver_backend.h>

static const char *TAG = "led_driver_hollow";

esp_err_t led_driver_backend_create(const led_driver_config_t *config, led_driver_backend_t *backend)
{
    (void)config;
    ESP_LOGI(TAG, "Initializing hollow LED driver");
    backend->handle = NULL;
    return ESP_OK;
}

esp_err_t led_driver_backend_render(led_driver_backend_t *backend, uint16_t hue, uint8_t saturation, uint8_t brightness)
{
    (void)backend;
    ESP_LOGI(TAG, "Render HSV: %u, %u, %u", hue, saturation, brightness);
    return ESP_OK;
}
