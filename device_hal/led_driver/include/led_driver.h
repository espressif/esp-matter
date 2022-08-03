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
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int gpio;
    int channel;
} led_driver_config_t;

typedef void *led_driver_handle_t;

led_driver_handle_t led_driver_init(led_driver_config_t *config);
esp_err_t led_driver_set_power(led_driver_handle_t handle, bool power);
esp_err_t led_driver_set_brightness(led_driver_handle_t handle, uint8_t brightness);
esp_err_t led_driver_set_hue(led_driver_handle_t handle, uint16_t hue);
esp_err_t led_driver_set_saturation(led_driver_handle_t handle, uint8_t saturation);
esp_err_t led_driver_set_temperature(led_driver_handle_t handle, uint32_t temperature);

#ifdef __cplusplus
}
#endif
