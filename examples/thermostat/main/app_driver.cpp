/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdlib.h>
#include <string.h>

#include <esp_log.h>
#include <esp_matter.h>

#include <app_priv.h>
#include <device.h>

using namespace chip::app::Clusters;
using namespace esp_matter;

static const char *TAG = "app_driver";
extern uint16_t thermostat_endpoint_id;

/* Do any conversions/remapping for the actual value here */
// Stub: implement actual driver logic for thermostat later
static esp_err_t app_driver_thermostat_set_value(void *handle, esp_matter_attr_val_t *val)
{
    // TODO: Implement driver logic for thermostat
    return ESP_OK;
}

static void app_driver_button_toggle_cb(void *arg, void *data)
{
    ESP_LOGI(TAG, "Button pressed (stub for thermostat)");
    // TODO: Implement button logic for thermostat
}

esp_err_t app_driver_attribute_update(app_driver_handle_t driver_handle, uint16_t endpoint_id, uint32_t cluster_id,
                                      uint32_t attribute_id, esp_matter_attr_val_t *val)
{
    esp_err_t err = ESP_OK;
    if (endpoint_id == thermostat_endpoint_id) {
        // TODO: Implement attribute update logic for thermostat
        err = app_driver_thermostat_set_value(driver_handle, val);
    }
    return err;
}

esp_err_t app_driver_thermostat_set_defaults(uint16_t endpoint_id)
{
    esp_err_t err = ESP_OK;
    // TODO: Set default values for thermostat attributes if needed
    return err;
}

app_driver_handle_t app_driver_thermostat_init()
{
    // TODO: Initialize thermostat driver (stub for now)
    return NULL;
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

    iot_button_register_cb(handle, BUTTON_PRESS_DOWN, NULL, app_driver_button_toggle_cb, NULL);
    return (app_driver_handle_t)handle;
}
