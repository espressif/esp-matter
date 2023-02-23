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
#include <led_driver.h>

#define LED_GPIO_PIN GPIO_NUM_8
#define LED_CHANNEL 0 /* LEDC_CHANNEL_0 */
#define BUTTON_GPIO_PIN GPIO_NUM_4

static const char *TAG = "device";

led_driver_config_t led_driver_get_config()
{
    led_driver_config_t config = {
        .gpio = LED_GPIO_PIN,
        .channel = LED_CHANNEL,
    };
    return config;
}

button_config_t button_driver_get_config()
{
    button_config_t config = {
        .type = BUTTON_TYPE_GPIO,
        .gpio_button_config = {
            .gpio_num = BUTTON_GPIO_PIN,
            .active_level = 0,
        }
    };
    return config;
}
