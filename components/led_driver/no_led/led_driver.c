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

static const char *TAG = "led_driver_no_led";

esp_err_t led_driver_init(led_driver_config_t *config)
{
    ESP_LOGI(TAG, "Initializing led driver");
    return ESP_OK;
}

esp_err_t led_driver_set_power(bool power)
{
    return ESP_OK;
}

esp_err_t led_driver_set_brightness(uint8_t brightness)
{
    return ESP_OK;
}

esp_err_t led_driver_set_hue(uint16_t hue)
{
    return ESP_OK;
}

esp_err_t led_driver_set_saturation(uint8_t saturation)
{
    return ESP_OK;
}

esp_err_t led_driver_set_temperature(uint32_t temperature)
{
    return ESP_OK;
}

bool led_driver_get_power()
{
    return false;
}

uint8_t led_driver_get_brightness()
{
    return 0;
}

uint16_t led_driver_get_hue()
{
    return 0;
}

uint8_t led_driver_get_saturation()
{
    return 0;
}

uint32_t led_driver_get_temperature()
{
    return 0;
}
