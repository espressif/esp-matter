/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <nvs_flash.h>

#include <esp_rmaker_core.h>
#include <esp_rmaker_standard_params.h>
#include <esp_rmaker_standard_devices.h>
#include <esp_rmaker_standard_types.h>
#include <esp_rmaker_ota.h>
#include <esp_rmaker_schedule.h>
#include <esp_rmaker_console.h>

#include <app_rainmaker.h>
#include <app_driver.h>
#include "app_constants.h"

esp_rmaker_device_t *light_device;

static void update_rmaker_power(bool power)
{
    esp_rmaker_param_t *param = esp_rmaker_device_get_param_by_type(light_device, ESP_RMAKER_PARAM_POWER);
    if (!param) {
        ESP_LOGE(APP_LOG_TAG, "Param type not found");
        return;
    }
    esp_rmaker_param_update_and_report(param, esp_rmaker_bool(power));
}

static void update_rmaker_brightness(uint8_t brightness)
{
    uint8_t brightness_rainmaker = REMAP_TO_RANGE(brightness, BRIGHTNESS_ATTRIBUTE_MAX, BRIGHTNESS_MAX);
    esp_rmaker_param_t *param = esp_rmaker_device_get_param_by_type(light_device, ESP_RMAKER_PARAM_BRIGHTNESS);
    if (!param) {
        ESP_LOGE(APP_LOG_TAG, "Param type not found");
        return;
    }
    esp_rmaker_param_update_and_report(param, esp_rmaker_int(brightness_rainmaker));
}

static void update_rmaker_hue(uint16_t hue)
{
    esp_rmaker_param_t *param = esp_rmaker_device_get_param_by_type(light_device, ESP_RMAKER_PARAM_HUE);
    if (!param) {
        ESP_LOGE(APP_LOG_TAG, "Param type not found");
        return;
    }
    esp_rmaker_param_update_and_report(param, esp_rmaker_int(hue));
}

static void update_rmaker_saturation(uint8_t saturation)
{
    esp_rmaker_param_t *param = esp_rmaker_device_get_param_by_type(light_device, ESP_RMAKER_PARAM_SATURATION);
    if (!param) {
        ESP_LOGE(APP_LOG_TAG, "Param type not found");
        return;
    }
    esp_rmaker_param_update_and_report(param, esp_rmaker_int(saturation));
}

/* Callback to handle commands received from the RainMaker cloud */
static esp_err_t write_cb(const esp_rmaker_device_t *device, const esp_rmaker_param_t *param,
                          const esp_rmaker_param_val_t val, void *priv_data, esp_rmaker_write_ctx_t *ctx)
{
    if (ctx) {
        ESP_LOGI(APP_LOG_TAG, "Received write request via : %s", esp_rmaker_device_cb_src_to_str(ctx->src));
    }
    const char *device_name = esp_rmaker_device_get_name(device);
    const char *param_name = esp_rmaker_param_get_name(param);
    if (strcmp(param_name, ESP_RMAKER_DEF_POWER_NAME) == 0) {
        ESP_LOGI(APP_LOG_TAG, "Received value = %s for %s - %s",
                 val.val.b ? "true" : "false", device_name, param_name);
        app_driver_update_and_report_power(val.val.b, APP_DRIVER_SRC_RAINMAKER);
    } else if (strcmp(param_name, ESP_RMAKER_DEF_BRIGHTNESS_NAME) == 0) {
        ESP_LOGI(APP_LOG_TAG, "Received value = %d for %s - %s",
                 val.val.i, device_name, param_name);
        uint8_t brightness = REMAP_TO_RANGE(val.val.i, BRIGHTNESS_MAX, BRIGHTNESS_ATTRIBUTE_MAX);
        app_driver_update_and_report_brightness(brightness, APP_DRIVER_SRC_RAINMAKER);
    } else if (strcmp(param_name, ESP_RMAKER_DEF_HUE_NAME) == 0) {
        ESP_LOGI(APP_LOG_TAG, "Received value = %d for %s - %s",
                 val.val.i, device_name, param_name);
        app_driver_update_and_report_hue(val.val.i, APP_DRIVER_SRC_RAINMAKER);
    } else if (strcmp(param_name, ESP_RMAKER_DEF_SATURATION_NAME) == 0) {
        ESP_LOGI(APP_LOG_TAG, "Received value = %d for %s - %s",
                 val.val.i, device_name, param_name);
        app_driver_update_and_report_saturation(val.val.i, APP_DRIVER_SRC_RAINMAKER);
    } else {
        /* Silently ignoring invalid params */
        return ESP_OK;
    }
    esp_rmaker_param_update_and_report(param, val);
    return ESP_OK;
}

void app_rmaker_init()
{
    app_driver_param_callback_t callbacks = {
        .update_power = update_rmaker_power,
        .update_brightness = update_rmaker_brightness,
        .update_hue = update_rmaker_hue,
        .update_saturation = update_rmaker_saturation,
        .update_temperature = NULL,
    };

    /* Initialize the ESP RainMaker Agent.
     * Note that this should be called after app_wifi_init() but before app_wifi_start()
     * */
    esp_rmaker_console_init();

    esp_rmaker_config_t rainmaker_cfg = {
        .enable_time_sync = false,
    };
    esp_rmaker_node_t *node = esp_rmaker_node_init(&rainmaker_cfg, "ESP RainMaker Device", "Lightbulb");
    if (!node) {
        ESP_LOGE(APP_LOG_TAG, "Could not initialise node. Aborting!!!");
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        abort();
    }

    /* Create a device and add the relevant parameters to it */
    light_device = esp_rmaker_lightbulb_device_create("Light", NULL, app_driver_get_power());
    esp_rmaker_device_add_cb(light_device, write_cb, NULL);

    esp_rmaker_device_add_param(light_device,
                                esp_rmaker_brightness_param_create(ESP_RMAKER_DEF_BRIGHTNESS_NAME, app_driver_get_brightness()));

    esp_rmaker_device_add_param(light_device, esp_rmaker_hue_param_create(ESP_RMAKER_DEF_HUE_NAME, app_driver_get_hue()));

    esp_rmaker_device_add_param(light_device,
                                esp_rmaker_saturation_param_create(ESP_RMAKER_DEF_SATURATION_NAME, app_driver_get_saturation()));

    esp_rmaker_node_add_device(node, light_device);

    /* Enable OTA */
    esp_rmaker_ota_config_t ota_config = {
        .server_cert = ESP_RMAKER_OTA_DEFAULT_SERVER_CERT,
    };
    esp_rmaker_ota_enable(&ota_config, OTA_USING_PARAMS);

    /* Enable timezone service which will be require for setting appropriate timezone
     * from the phone apps for scheduling to work correctly.
     * For more information on the various ways of setting timezone, please check
     * https://rainmaker.espressif.com/docs/time-service.html.
     */
    esp_rmaker_timezone_service_enable();

    /* Enable scheduling. */
    esp_rmaker_schedule_enable();

    /* Start the ESP RainMaker Agent */
    esp_rmaker_start();

    app_driver_register_src(APP_DRIVER_SRC_RAINMAKER, &callbacks);
}
