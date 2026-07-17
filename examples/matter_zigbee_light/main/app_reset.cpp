/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_log.h>
#include <esp_matter.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <iot_button.h>

#include <app_priv.h>
#include <esp_zigbee.h>

static const char *TAG = "app_reset";

namespace {

constexpr int kFactoryResetStopPollMs = 100;
constexpr int kFactoryResetStopMaxPolls = 100;

bool s_perform_factory_reset = false;

void button_factory_reset_pressed_cb(void *arg, void *data)
{
    if (!s_perform_factory_reset) {
        ESP_LOGI(TAG, "Factory reset triggered. Release the button to start factory reset.");
        s_perform_factory_reset = true;
    }
}

void button_factory_reset_released_cb(void *arg, void *data)
{
    if (!s_perform_factory_reset) {
        return;
    }

    ESP_LOGI(TAG, "Starting factory reset");
    s_perform_factory_reset = false;

    if (app_zigbee_stack_is_running() && !ezb_bdb_is_factory_new()) {
        esp_zigbee_factory_reset();
    } else {
        esp_matter::factory_reset();
    }
}

} // namespace

esp_err_t app_reset_button_register(void *handle)
{
    if (!handle) {
        ESP_LOGE(TAG, "Handle cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }

    button_handle_t button_handle = (button_handle_t)handle;
    esp_err_t err = ESP_OK;
    err |= iot_button_register_cb(button_handle, BUTTON_LONG_PRESS_HOLD, nullptr, button_factory_reset_pressed_cb,
                                  nullptr);
    err |= iot_button_register_cb(button_handle, BUTTON_PRESS_UP, nullptr, button_factory_reset_released_cb, nullptr);
    return err;
}
