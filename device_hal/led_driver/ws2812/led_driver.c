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
#include <esp_log.h>
#include <led_driver.h>
#include <led_strip.h>

// Numbers of the LED in the strip
#define LED_STRIP_LED_COUNT 1
// let the driver choose a proper memory block size automatically
#define LED_STRIP_MEMORY_BLOCK_WORDS 0 
// 10MHz resolution, 1 tick = 0.1us (led strip needs a high resolution)
#define LED_STRIP_RMT_RES_HZ  (10 * 1000 * 1000)

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

    // LED strip general initialization, according to your led board design
    led_strip_config_t strip_config = {
        .strip_gpio_num = config->gpio,       // The GPIO that connected to the LED strip's data line
        .max_leds = LED_STRIP_LED_COUNT,      // The number of LEDs in the strip,
        .led_model = LED_MODEL_WS2812,        // LED strip model
        .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB, // The color order of the strip: GRB
    };

    // LED strip backend configuration: RMT
    led_strip_rmt_config_t rmt_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,        // different clock source can lead to different power consumption
        .resolution_hz = LED_STRIP_RMT_RES_HZ, // RMT counter clock frequency
        .mem_block_symbols = LED_STRIP_MEMORY_BLOCK_WORDS, // the memory block size used by the RMT channel
    };

    // LED Strip object handle
    led_strip_handle_t led_strip;
    err = led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "W2812 driver install failed");
        return NULL;
    }
    return (led_driver_handle_t)led_strip;
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
        led_strip_handle_t strip = (led_strip_handle_t)handle;
        err = led_strip_set_pixel(strip, 0, mRGB.red, mRGB.green, mRGB.blue);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "strip_set_pixel failed");
            return err;
        }
        ESP_LOGI(TAG, "led set r:%d, g:%d, b:%d", mRGB.red, mRGB.green, mRGB.blue);
        err = led_strip_refresh(strip);
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

esp_err_t led_driver_set_xy(led_driver_handle_t handle, uint16_t x, uint16_t y)
{
    uint8_t brightness = current_power ? current_brightness : 0;
    XY_color_t xy_color = {x, y};
    xy_to_rgb(xy_color, brightness, &mRGB);
    return led_driver_set_RGB(handle);
}
