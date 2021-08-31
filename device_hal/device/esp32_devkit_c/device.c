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

#include <light_driver.h>
#include <button_driver.h>

#define DEVICE_VERSION_1_0

#ifdef DEVICE_VERSION_1_0
#define LED_GPIO_PIN 12             /* GPIO_NUM_12 */
#define LED_CHANNEL  0              /* LEDC_CHANNEL_0 */
#endif

static const char *TAG = "device";

static esp_err_t device_light_init()
{
    light_driver_config_t config = {
        .gpio = LED_GPIO_PIN,
        .channel = LED_CHANNEL,
    };
    return light_driver_init(&config);
}

static esp_err_t device_button_init()
{
    return button_driver_init(NULL);
}

esp_err_t device_init()
{
    ESP_LOGI(TAG, "Initializing device");
    device_light_init();
    device_button_init();
    return ESP_OK;
}
