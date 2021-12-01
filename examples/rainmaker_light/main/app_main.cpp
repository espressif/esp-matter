/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "app_constants.h"
#include "app_driver.h"
#include "app_matter.h"
#include "app_qrcode.h"
#include "app_rainmaker.h"
#include "esp_matter.h"
#include "esp_matter_console.h"
#include "esp_matter_standard.h"

#include "esp_err.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lib/shell/Engine.h"

#define APP_MAIN_NAME "Main"
static const char *TAG = "app_main";

static esp_err_t app_main_attribute_update(const char *endpoint, const char *attribute, esp_matter_attr_val_t val,
                                           void *priv_data)
{
    /* Just adding this callback to notify the application */
    switch (val.type) {
    case ESP_MATTER_VAL_TYPE_BOOLEAN:
        ESP_LOGD(TAG, "%s's %s is %d", endpoint, attribute, val.val.b);
        break;

    case ESP_MATTER_VAL_TYPE_INTEGER:
        ESP_LOGD(TAG, "%s's %s is %d", endpoint, attribute, val.val.i);
        break;

    case ESP_MATTER_VAL_TYPE_FLOAT:
        ESP_LOGD(TAG, "%s's %s is %f", endpoint, attribute, val.val.f);
        break;

    case ESP_MATTER_VAL_TYPE_STRING:
    case ESP_MATTER_VAL_TYPE_OBJECT:
    case ESP_MATTER_VAL_TYPE_ARRAY:
        ESP_LOGD(TAG, "%s's %s is %s", endpoint, attribute, val.val.s);
        break;

    default:
        ESP_LOGD(TAG, "%s's %s is <invalid value>", endpoint, attribute);
        break;
    }
    return ESP_OK;
}

extern "C" void app_main()
{
    /* Initialize the ESP NVS layer */
    ESP_ERROR_CHECK(nvs_flash_init());

    /* Initialize esp_matter */
    esp_matter_init();
    esp_matter_attribute_callback_add(APP_MAIN_NAME, app_main_attribute_update, NULL);

    /* Initialize driver */
    app_driver_init();

    /* Initialize chip */
    ESP_ERROR_CHECK(app_matter_init());
    app_qrcode_print();

    /* Initialize rainmaker */
    app_rainmaker_init();

    /* Set the default attribute values */
    esp_matter_attribute_notify(APP_MAIN_NAME, ESP_MATTER_ENDPOINT_LIGHT, ESP_MATTER_ATTR_POWER,
                                esp_matter_bool(DEFAULT_POWER));
    esp_matter_attribute_notify(APP_MAIN_NAME, ESP_MATTER_ENDPOINT_LIGHT, ESP_MATTER_ATTR_BRIGHTNESS,
                                esp_matter_int(DEFAULT_BRIGHTNESS));
    esp_matter_attribute_notify(APP_MAIN_NAME, ESP_MATTER_ENDPOINT_LIGHT, ESP_MATTER_ATTR_HUE,
                                esp_matter_int(DEFAULT_HUE));
    esp_matter_attribute_notify(APP_MAIN_NAME, ESP_MATTER_ENDPOINT_LIGHT, ESP_MATTER_ATTR_SATURATION,
                                esp_matter_int(DEFAULT_SATURATION));

#if CONFIG_ENABLE_CHIP_SHELL
    esp_matter_console_diagnostics_register_commands();
    esp_matter_console_init();
#endif
}
