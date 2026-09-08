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
#include <led_strip.h>

#include <led_driver_backend.h>

static const char *TAG = "led_driver_ws2812";

esp_err_t led_driver_backend_create(const led_driver_config_t *config, led_driver_backend_t *backend)
{
    const led_strip_config_t strip_config = {
        .strip_gpio_num = config->gpio,
        .max_leds = 1,
        .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB,
        .led_model = LED_MODEL_WS2812,
        .flags.invert_out = config->output_invert,
    };
    const led_strip_rmt_config_t rmt_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 10 * 1000 * 1000,
        .flags.with_dma = false,
    };

    led_strip_handle_t strip = NULL;
    esp_err_t err = led_strip_new_rmt_device(&strip_config, &rmt_config, &strip);
    if (err == ESP_OK) {
        err = led_strip_clear(strip);
    }
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize WS2812: %s", esp_err_to_name(err));
        if (strip != NULL) {
            led_strip_del(strip);
        }
        return err;
    }

    backend->handle = strip;
    return ESP_OK;
}

esp_err_t led_driver_backend_render(led_driver_backend_t *backend, uint16_t hue, uint8_t saturation, uint8_t brightness)
{
    led_strip_handle_t strip = (led_strip_handle_t)backend->handle;
    esp_err_t err = led_strip_set_pixel_hsv(strip, 0, hue, saturation, brightness);
    if (err != ESP_OK) {
        return err;
    }
    return led_strip_refresh(strip);
}
