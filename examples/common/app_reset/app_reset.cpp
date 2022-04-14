/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

/* It is recommended to copy this code in your example so that you can modify as per your application's needs,
 * especially for the indicator calbacks, button_factory_reset_pressed_cb() and button_factory_reset_released_cb().
 */

#include <esp_log.h>
#include <device.h>
#include <esp_matter.h>

static const char *TAG = "app_reset";
static bool perform_factory_reset = false;

static void button_factory_reset_pressed_cb(void *arg)
{
    if (!perform_factory_reset) {
        ESP_LOGI(TAG, "Factory reset triggered. Release the button to start factory reset.");
        perform_factory_reset = true;
    }
}

static void button_factory_reset_released_cb(void *arg)
{
    if (perform_factory_reset) {
        ESP_LOGI(TAG, "Starting factory reset");
        esp_matter::factory_reset();
        perform_factory_reset = false;
    }
}

esp_err_t app_reset_button_register(button_handle_t handle)
{
    esp_err_t err = ESP_OK;
    err |= iot_button_register_cb(handle, BUTTON_LONG_PRESS_HOLD, button_factory_reset_pressed_cb);
    err |= iot_button_register_cb(handle, BUTTON_PRESS_UP, button_factory_reset_released_cb);
    return err;
}
