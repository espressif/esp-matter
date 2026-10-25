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

#include <stdint.h>

#include <driver/ledc.h>
#include <esp_log.h>

#include <led_driver_backend.h>

static const char *TAG = "led_driver_ledc";

esp_err_t led_driver_backend_create(const led_driver_config_t *config, led_driver_backend_t *backend)
{
    if (config->channel < 0 || config->channel >= LEDC_CHANNEL_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    const ledc_timer_config_t timer_config = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_8_BIT,
        .timer_num = LEDC_TIMER_1,
        .freq_hz = 5000,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    esp_err_t err = ledc_timer_config(&timer_config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure LEDC timer: %s", esp_err_to_name(err));
        return err;
    }

    const ledc_channel_config_t channel_config = {
        .gpio_num = config->gpio,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = (ledc_channel_t)config->channel,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_1,
        .duty = 0,
        .hpoint = 0,
        .flags.output_invert = config->output_invert,
    };
    err = ledc_channel_config(&channel_config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure LEDC channel: %s", esp_err_to_name(err));
        return err;
    }

    backend->handle = (void *)(uintptr_t)(config->channel + 1);
    return ESP_OK;
}

esp_err_t led_driver_backend_render(led_driver_backend_t *backend, uint16_t hue, uint8_t saturation, uint8_t brightness)
{
    (void)hue;
    (void)saturation;
    ledc_channel_t channel = (ledc_channel_t)((uintptr_t)backend->handle - 1);
    esp_err_t err = ledc_set_duty(LEDC_LOW_SPEED_MODE, channel, brightness);
    if (err != ESP_OK) {
        return err;
    }
    return ledc_update_duty(LEDC_LOW_SPEED_MODE, channel);
}
