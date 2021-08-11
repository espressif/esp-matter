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
#include <driver/ledc.h>
#include <hal/ledc_types.h>

#include <led_driver.h>

static const char *TAG = "led_driver_gpio";
static bool current_power = false;
static uint8_t current_brightness = 0;

esp_err_t led_driver_init(led_driver_config_t *config)
{
    ESP_LOGI(TAG, "Initializing led driver");
    esp_err_t err = ESP_OK;

    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE, // timer mode
        .duty_resolution = LEDC_TIMER_8_BIT, // resolution of PWM duty
        .timer_num = LEDC_TIMER_1, // timer index
        .freq_hz = 5000, // frequency of PWM signal
        .clk_cfg = LEDC_AUTO_CLK, // Auto select the source clock
    };
    err = ledc_timer_config(&ledc_timer);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "led_timerc_config failed");
        return err;
    }

    ledc_channel_config_t ledc_channel = {
        .gpio_num = config->gpio,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = config->channel,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_1,
        .duty = 0,
        .hpoint = 0,
    };
    err = ledc_channel_config(&ledc_channel);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "ledc_channel_config failed");
    }

    return err;
}

esp_err_t led_driver_set_power(bool power)
{
    current_power = power;
    return ESP_OK;
}

esp_err_t led_driver_set_brightness(uint8_t brightness)
{
    esp_err_t err;

    if (brightness != 0) {
        current_brightness = brightness;
    }
    if (!current_power) {
        brightness = 0;
    }

    err = ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, brightness);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "ledc_set_duty failed");
    }

    err = ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "ledc_update_duty failed");
    }
    return err;
}

esp_err_t led_driver_set_hue(uint16_t hue)
{
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t led_driver_set_saturation(uint8_t saturation)
{
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t led_driver_set_temperature(uint32_t temperature)
{
    return ESP_ERR_NOT_SUPPORTED;
}

bool led_driver_get_power()
{
    return current_power;
}

uint8_t led_driver_get_brightness()
{
    return current_brightness;
}

uint16_t led_driver_get_hue()
{
    return 0;
}

uint8_t led_driver_get_saturation()
{
    return 0;
}

uint32_t led_driver_get_temperature()
{
    return 0;   
}
