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
#include <driver/rmt.h>
#include <esp_log.h>
#include <led_strip.h>
#include <led_driver.h>

static const char *TAG = "led_driver_ws2812";
static bool current_power = false;
static uint8_t current_brightness = 0;
static uint32_t current_temp = 0;
static HS_color_t current_HS = {0, 0};
static RGB_color_t mRGB;

led_driver_handle_t led_driver_init(led_driver_config_t *config)
{
    ESP_LOGI(TAG, "Initializing light driver");
    esp_err_t err = ESP_OK;
    rmt_config_t rmt_cfg = RMT_DEFAULT_CONFIG_TX(config->gpio, config->channel);
    rmt_cfg.clk_div = 2;
    err = rmt_config(&rmt_cfg);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "rmt_cfg failed");
        return NULL;
    }
    err = rmt_driver_install(rmt_cfg.channel, 0, 0);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "rmt_driver_install failed");
        return NULL;
    }

    led_strip_config_t strip_config = LED_STRIP_DEFAULT_CONFIG(1, (led_strip_dev_t)rmt_cfg.channel);
    led_strip_t *strip = led_strip_new_rmt_ws2812(&strip_config);
    if (!strip) {
        ESP_LOGE(TAG, "W2812 driver install failed");
        return NULL;
    }
    return (led_driver_handle_t)strip;
}

esp_err_t led_driver_set_power(led_driver_handle_t handle, bool power)
{
    current_power = power;
    return led_driver_set_brightness(handle, current_brightness);
}

esp_err_t led_driver_set_RGB(led_driver_handle_t handle)
{
    esp_err_t err = ESP_OK;
    if (!handle) {
        ESP_LOGE(TAG, "led driver handle cannot be NULL");
        err = ESP_FAIL;
    } else {
        led_strip_t *strip = (led_strip_t *)handle;
        err = strip->set_pixel(strip, 0, mRGB.red, mRGB.green, mRGB.blue);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "strip_set_pixel failed");
            return err;
        }
        ESP_LOGI(TAG, "led set r:%d, g:%d, b:%d", mRGB.red, mRGB.green, mRGB.blue);
        err = strip->refresh(strip, 100);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "strip_refresh failed");
        }
    }
    return err;
}

esp_err_t led_driver_set_brightness(led_driver_handle_t handle, uint8_t brightness)
{
    if (brightness != 0) {
        current_brightness = brightness;
    }
    if (!current_power) {
        brightness = 0;
    }
    hsv_to_rgb(current_HS, brightness, &mRGB);
    return led_driver_set_RGB(handle);
}

esp_err_t led_driver_set_hue(led_driver_handle_t handle, uint16_t hue)
{
    uint8_t brightness = current_power ? current_brightness : 0;
    current_HS.hue = hue;
    hsv_to_rgb(current_HS, brightness, &mRGB);
    return led_driver_set_RGB(handle);
}

esp_err_t led_driver_set_saturation(led_driver_handle_t handle, uint8_t saturation)
{
    uint8_t brightness = current_power ? current_brightness : 0;
    current_HS.saturation = saturation;
    hsv_to_rgb(current_HS, brightness, &mRGB);
    return led_driver_set_RGB(handle);
}

esp_err_t led_driver_set_temperature(led_driver_handle_t handle, uint32_t temperature)
{
    uint8_t brightness = current_power ? current_brightness : 0;
    current_temp = temperature;
    temp_to_hs(current_temp, &current_HS);
    hsv_to_rgb(current_HS, brightness, &mRGB);
    return led_driver_set_RGB(handle);
}
