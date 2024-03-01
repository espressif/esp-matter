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

#include <color_format.h>
#include <esp_log.h>
#include <led_driver.h>

static const char *TAG = "led_driver_hollow";

led_driver_handle_t led_driver_init(led_driver_config_t *config)
{
    ESP_LOGI(TAG, "Initializing light driver");
    /* Initialize the driver here */

    return NULL;
}

esp_err_t led_driver_set_power(led_driver_handle_t handle, bool power)
{
    ESP_LOGI(TAG, "Setting power to: %d", power);
    /* Set the power state here */

    return ESP_OK;
}

esp_err_t led_driver_set_brightness(led_driver_handle_t handle, uint8_t brightness)
{
    ESP_LOGI(TAG, "Setting brightness to: %d", brightness);
    /* Set the brightness level here */

    return ESP_OK;
}

esp_err_t led_driver_set_hue(led_driver_handle_t handle, uint16_t hue)
{
    ESP_LOGI(TAG, "Setting hue to: %d", hue);
    /* Set the color hue here */

    return ESP_OK;
}

esp_err_t led_driver_set_saturation(led_driver_handle_t handle, uint8_t saturation)
{
    ESP_LOGI(TAG, "Setting saturation to: %d", saturation);
    /* Set the color saturation here */

    return ESP_OK;
}

esp_err_t led_driver_set_temperature(led_driver_handle_t handle, uint32_t temperature)
{
    ESP_LOGI(TAG, "Setting temperature to: %lu", temperature);
    /* Set the color temp here*/

    return ESP_OK;
}
