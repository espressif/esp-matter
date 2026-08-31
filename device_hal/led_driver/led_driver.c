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

#include <stdlib.h>

#include <esp_log.h>

#include <led_driver.h>
#include <led_driver_backend.h>
#include <led_driver_utils.h>

#define LED_DRIVER_PERCENT_MAX 100
#define LED_DRIVER_HUE_DEGREES_MAX 360
#define LED_DRIVER_TEMPERATURE_KELVIN_MAX 1000000

static const char *TAG = "led_driver";

typedef struct {
    led_driver_backend_t backend;
    uint16_t hue;
    uint8_t saturation;
    uint8_t last_nonzero_brightness;
    bool power;
} led_driver_t;

static uint8_t led_driver_percent_to_byte(uint8_t percent)
{
    if (percent > LED_DRIVER_PERCENT_MAX) {
        percent = LED_DRIVER_PERCENT_MAX;
    }
    return (uint8_t)(((uint32_t)percent * UINT8_MAX + 50) / 100);
}

static uint8_t led_driver_restored_brightness(const led_driver_t *driver)
{
    return driver->power ? led_driver_percent_to_byte(driver->last_nonzero_brightness) : 0;
}

static esp_err_t led_driver_render(led_driver_t *driver, uint8_t brightness)
{
    return led_driver_backend_render(&driver->backend, driver->hue, driver->saturation, brightness);
}

led_driver_handle_t led_driver_init(led_driver_config_t *config)
{
    if (config == NULL) {
        ESP_LOGE(TAG, "LED driver configuration cannot be NULL");
        return NULL;
    }

    led_driver_t *driver = calloc(1, sizeof(led_driver_t));
    if (driver == NULL) {
        ESP_LOGE(TAG, "Failed to allocate LED driver");
        return NULL;
    }

    if (led_driver_backend_create(config, &driver->backend) != ESP_OK) {
        free(driver);
        return NULL;
    }
    return (led_driver_handle_t)driver;
}

esp_err_t led_driver_set_power(led_driver_handle_t handle, bool power)
{
    if (handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    led_driver_t *driver = (led_driver_t *)handle;

    driver->power = power;
    return led_driver_render(driver, led_driver_restored_brightness(driver));
}

esp_err_t led_driver_set_brightness(led_driver_handle_t handle, uint8_t brightness)
{
    if (handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    led_driver_t *driver = (led_driver_t *)handle;
    if (brightness > LED_DRIVER_PERCENT_MAX) {
        brightness = LED_DRIVER_PERCENT_MAX;
    }

    if (brightness != 0) {
        driver->last_nonzero_brightness = brightness;
    }
    return led_driver_render(driver, driver->power ? led_driver_percent_to_byte(brightness) : 0);
}

esp_err_t led_driver_set_hue(led_driver_handle_t handle, uint16_t hue)
{
#if LED_DRIVER_BACKEND_SUPPORTS_COLOR
    if (handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    led_driver_t *driver = (led_driver_t *)handle;
    if (hue >= LED_DRIVER_HUE_DEGREES_MAX) {
        hue = 0;
    }

    driver->hue = hue;
    return led_driver_render(driver, led_driver_restored_brightness(driver));
#else  // !LED_DRIVER_BACKEND_SUPPORTS_COLOR
    (void)hue;
    return handle == NULL ? ESP_ERR_INVALID_ARG : ESP_ERR_NOT_SUPPORTED;
#endif // LED_DRIVER_BACKEND_SUPPORTS_COLOR
}

esp_err_t led_driver_set_saturation(led_driver_handle_t handle, uint8_t saturation)
{
#if LED_DRIVER_BACKEND_SUPPORTS_COLOR
    if (handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    led_driver_t *driver = (led_driver_t *)handle;
    if (saturation > LED_DRIVER_PERCENT_MAX) {
        saturation = LED_DRIVER_PERCENT_MAX;
    }

    driver->saturation = led_driver_percent_to_byte(saturation);
    return led_driver_render(driver, led_driver_restored_brightness(driver));
#else  // !LED_DRIVER_BACKEND_SUPPORTS_COLOR
    (void)saturation;
    return handle == NULL ? ESP_ERR_INVALID_ARG : ESP_ERR_NOT_SUPPORTED;
#endif // LED_DRIVER_BACKEND_SUPPORTS_COLOR
}

esp_err_t led_driver_set_temperature(led_driver_handle_t handle, uint32_t temperature)
{
#if LED_DRIVER_BACKEND_SUPPORTS_COLOR
    if (handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    led_driver_t *driver = (led_driver_t *)handle;
    if (temperature > LED_DRIVER_TEMPERATURE_KELVIN_MAX) {
        temperature = LED_DRIVER_TEMPERATURE_KELVIN_MAX;
    }

    HS_color_t color = temp_to_hs(temperature);
    driver->hue = color.hue;
    driver->saturation = color.saturation;
    return led_driver_render(driver, led_driver_restored_brightness(driver));
#else  // !LED_DRIVER_BACKEND_SUPPORTS_COLOR
    (void)temperature;
    return handle == NULL ? ESP_ERR_INVALID_ARG : ESP_ERR_NOT_SUPPORTED;
#endif // LED_DRIVER_BACKEND_SUPPORTS_COLOR
}

esp_err_t led_driver_set_xy(led_driver_handle_t handle, uint16_t x, uint16_t y)
{
#if LED_DRIVER_BACKEND_SUPPORTS_COLOR
    if (handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    led_driver_t *driver = (led_driver_t *)handle;

    HS_color_t color = xy_to_hs(x, y);
    driver->hue = color.hue;
    driver->saturation = color.saturation;
    return led_driver_render(driver, led_driver_restored_brightness(driver));
#else  // !LED_DRIVER_BACKEND_SUPPORTS_COLOR
    (void)x;
    (void)y;
    return handle == NULL ? ESP_ERR_INVALID_ARG : ESP_ERR_NOT_SUPPORTED;
#endif // LED_DRIVER_BACKEND_SUPPORTS_COLOR
}
