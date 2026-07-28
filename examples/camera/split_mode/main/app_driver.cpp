/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_log.h>
#include <stdlib.h>
#include <string.h>

#include <app_priv.h>
#include <esp_matter.h>

#include <button_gpio.h>
#include <device.h>
#include <led_driver.h>

using namespace chip::app::Clusters;
using namespace esp_matter;

static const char *TAG = "app_driver";
extern uint16_t camera_endpoint_id;

bool deferred_offer = false;

static void app_driver_button_toggle_cb(void *arg, void *data)
{
    ESP_LOGI(TAG, "Toggle button pressed");
    uint16_t endpoint_id = camera_endpoint_id;
}
static void app_driver_button_double_click_cb(void *arg, void *data)
{
    ESP_LOGI(TAG, "Double Click");
    if (deferred_offer) {
        ESP_LOGI(TAG, "Moving out of Standby Mode.");
        deferred_offer = false;
    } else {
        ESP_LOGI(TAG, "Putting Camera in Standby Mode.");
        deferred_offer = true;
    }
}

app_driver_handle_t app_driver_button_init()
{
    /* Initialize button */
    button_handle_t handle = NULL;
    const button_config_t btn_cfg = {0};
    const button_gpio_config_t btn_gpio_cfg = button_driver_get_config();

    if (iot_button_new_gpio_device(&btn_cfg, &btn_gpio_cfg, &handle) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create button device");
        return NULL;
    }

    iot_button_register_cb(handle, BUTTON_PRESS_DOWN, NULL,
                           app_driver_button_toggle_cb, NULL);
    iot_button_register_cb(handle, BUTTON_DOUBLE_CLICK, NULL,
                           app_driver_button_double_click_cb, NULL);
    return (app_driver_handle_t)handle;
}
