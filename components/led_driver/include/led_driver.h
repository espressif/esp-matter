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

#pragma once
#include <esp_err.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct {
    int gpio;
    int channel;
} led_driver_config_t;

esp_err_t led_driver_init(led_driver_config_t *config);
esp_err_t led_driver_set_power(bool power);
esp_err_t led_driver_set_brightness(uint8_t brightness);
esp_err_t led_driver_set_hue(uint16_t hue);
esp_err_t led_driver_set_saturation(uint8_t saturation);
esp_err_t led_driver_set_temperature(uint32_t temperature);

bool led_driver_get_power(void);
uint8_t led_driver_get_brightness(void);
uint16_t led_driver_get_hue(void);
uint8_t led_driver_get_saturation(void);
uint32_t led_driver_get_temperature(void);

#ifdef __cplusplus
}
#endif
