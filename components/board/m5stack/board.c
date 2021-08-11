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
#include <button_driver.h>

static const char *TAG = "board";

static esp_err_t board_led_init()
{
    led_driver_config_t config = {
        .gpio = 32,             /* PIN_NUM_BCKL for M5Stack TFT */
        .channel = 7,           /* LEDC_CHANNEL_7 */
    };
    return led_driver_init(&config);
}

static esp_err_t board_button_init()
{
    return button_driver_init(NULL);
}

esp_err_t board_init()
{
    ESP_LOGI(TAG, "Initializing board");
    board_led_init();
    board_button_init();
    return ESP_OK;
}
