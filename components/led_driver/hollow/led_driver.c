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
#include <led_driver.h>
#include <color_format.h>

static const char *TAG = "led_driver_hollow";
static bool current_power = false;
static uint8_t current_brightness = 0;
static HS_color_t current_HS = {0, 0};
static uint32_t current_temperature = 6600;

esp_err_t led_driver_init(led_driver_config_t *config)
{
    ESP_LOGI(TAG, "Initializing led driver");
    /* Initialize the driver here */

    return ESP_OK;
}

esp_err_t led_driver_set_power(bool power)
{
    ESP_LOGI(TAG, "Setting power to: %d", power);
    /* Set the power state here */

    return ESP_OK;
}

esp_err_t led_driver_set_brightness(uint8_t brightness)
{
    ESP_LOGI(TAG, "Setting brightness to: %d", brightness);
    /* Set the brightness level here */

    return ESP_OK;
}

esp_err_t led_driver_set_hue(uint16_t hue)
{
    ESP_LOGI(TAG, "Setting hue to: %d", hue);
    /* Set the color hue here */

    return ESP_OK;
}

esp_err_t led_driver_set_saturation(uint8_t saturation)
{
    ESP_LOGI(TAG, "Setting saturation to: %d", saturation);
    /* Set the color saturation here */

    return ESP_OK;
}

esp_err_t led_driver_set_temperature(uint32_t temperature)
{
    ESP_LOGI(TAG, "Setting temperature to: %d", temperature);
    /* Set the color temp here*/

    return ESP_OK;
}

bool led_driver_get_power()
{
    return current_power;
}

uint8_t led_driver_get_brightness()
{
    return current_brightness;
}

uint16_t led_driver_get_hue()
{
    return current_HS.hue;
}

uint8_t led_driver_get_saturation()
{
    return current_HS.saturation;
}

uint32_t led_driver_get_temperature()
{
    return current_temperature;
}
