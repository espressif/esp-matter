/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <nvs_flash.h>
#include <string.h>

#include <esp_rmaker_core.h>
#include <esp_rmaker_ota.h>
#include <esp_rmaker_schedule.h>
#include <esp_rmaker_standard_devices.h>
#include <esp_rmaker_standard_params.h>
#include <esp_rmaker_standard_types.h>
#include <esp_rmaker_user_mapping.h>

#include "app_constants.h"
#include <app_rainmaker.h>
#include <esp_matter.h>
#include <esp_matter_console.h>
#include <esp_matter_standard.h>

#define APP_RAINMAKER_NAME "RainMaker"
static const char *TAG = "app_rainmaker";

esp_rmaker_device_t *light_device;

static esp_err_t app_rainmaker_console_handler(int argc, char **argv)
{
    if (argc == 3 && strncmp(argv[0], "add-user", sizeof("add-user")) == 0) {
        printf("%s: Starting user-node mapping\n", TAG);
        if (esp_rmaker_start_user_node_mapping(argv[1], argv[2]) != ESP_OK) {
            return -1;
        }
    } else {
        printf("%s: Invalid Usage.\n", TAG);
        return -1;
    }
    return 0;
}

static void app_rainmaker_register_commands()
{
    esp_matter_console_command_t command = {
        .name = "rainmaker",
        .description = "Initiate ESP RainMaker User-Node mapping from the node. Usage: chip esp rainmaker add-user "
                       "<user_id> <secret_key>",
        .handler = app_rainmaker_console_handler,
    };
    esp_matter_console_add_command(&command);
}

static esp_rmaker_param_val_t esp_rmaker_get_rmaker_val(esp_matter_attr_val_t val)
{
    if (val.type == ESP_MATTER_VAL_TYPE_BOOLEAN) {
        return esp_rmaker_bool(val.val.b);
    } else if (val.type == ESP_MATTER_VAL_TYPE_INTEGER) {
        return esp_rmaker_int(val.val.i);
    } else if (val.type == ESP_MATTER_VAL_TYPE_FLOAT) {
        return esp_rmaker_float(val.val.f);
    } else if (val.type == ESP_MATTER_VAL_TYPE_STRING) {
        return esp_rmaker_str(val.val.s);
    } else if (val.type == ESP_MATTER_VAL_TYPE_OBJECT) {
        return esp_rmaker_obj(val.val.s);
    } else if (val.type == ESP_MATTER_VAL_TYPE_ARRAY) {
        return esp_rmaker_array(val.val.s);
    } else {
        ESP_LOGE(TAG, "Invalid val type: %d", val.type);
    }
    return esp_rmaker_int(0);
}

static esp_matter_attr_val_t esp_rmaker_get_matter_val(esp_rmaker_param_val_t val)
{
    if (val.type == RMAKER_VAL_TYPE_BOOLEAN) {
        return esp_matter_bool(val.val.b);
    } else if (val.type == RMAKER_VAL_TYPE_INTEGER) {
        return esp_matter_int(val.val.i);
    } else if (val.type == RMAKER_VAL_TYPE_FLOAT) {
        return esp_matter_float(val.val.f);
    } else if (val.type == RMAKER_VAL_TYPE_STRING) {
        return esp_matter_str(val.val.s);
    } else if (val.type == RMAKER_VAL_TYPE_OBJECT) {
        return esp_matter_obj(val.val.s);
    } else if (val.type == RMAKER_VAL_TYPE_ARRAY) {
        return esp_matter_array(val.val.s);
    } else {
        ESP_LOGE(TAG, "Invalid val type: %d", val.type);
    }
    return esp_matter_int(0);
}

/* Callback to handle changes received from other sources */
static esp_err_t app_rainmaker_attribute_update(const char *endpoint, const char *attribute, esp_matter_attr_val_t val,
                                                void *priv_data)
{
    const esp_rmaker_node_t *node = esp_rmaker_get_node();
    esp_rmaker_device_t *device = esp_rmaker_node_get_device_by_name(node, endpoint);
    esp_rmaker_param_t *param = esp_rmaker_device_get_param_by_name(device, attribute);
    if (!device || !param) {
        ESP_LOGE(TAG, "Incorrect endpoint or attribute. endpoint: %s, attribute: %s", endpoint, attribute);
        return ESP_ERR_INVALID_ARG;
    }
    esp_rmaker_param_val_t rmaker_val = esp_rmaker_get_rmaker_val(val);

    esp_rmaker_param_update_and_report(param, rmaker_val);
    return ESP_OK;
}

/* Callback to handle commands received from the RainMaker cloud */
static esp_err_t write_cb(const esp_rmaker_device_t *device, const esp_rmaker_param_t *param,
                          const esp_rmaker_param_val_t val, void *priv_data, esp_rmaker_write_ctx_t *ctx)
{
    if (ctx) {
        ESP_LOGI(TAG, "Received write request via : %s", esp_rmaker_device_cb_src_to_str(ctx->src));
    }
    const char *device_name = esp_rmaker_device_get_name(device);
    const char *param_name = esp_rmaker_param_get_name(param);
    if (!device_name || !param_name) {
        ESP_LOGE(TAG, "Incorrect device_name or param_name");
        return ESP_ERR_INVALID_ARG;
    }
    esp_matter_attr_val_t matter_val = esp_rmaker_get_matter_val(val);

    esp_matter_attribute_notify(APP_RAINMAKER_NAME, device_name, param_name, matter_val);
    esp_rmaker_param_update_and_report(param, val);
    return ESP_OK;
}

esp_err_t app_rainmaker_init()
{
    /* Initialize the ESP RainMaker Agent.
     * Note that this should be called after app_wifi_init() but before app_wifi_start()
     * */
    esp_rmaker_config_t rainmaker_cfg = {
        .enable_time_sync = false,
    };
    esp_rmaker_node_t *node = esp_rmaker_node_init(&rainmaker_cfg, "ESP RainMaker Device", "Lightbulb");
    if (!node) {
        ESP_LOGE(TAG, "Could not initialise node.");
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        return ESP_FAIL;
    }

    /* Create a device and add the relevant parameters to it */
    light_device = esp_rmaker_lightbulb_device_create(ESP_MATTER_ENDPOINT_LIGHT, NULL, DEFAULT_POWER);
    esp_rmaker_device_add_cb(light_device, write_cb, NULL);

    esp_rmaker_device_add_param(light_device,
                                esp_rmaker_brightness_param_create(ESP_MATTER_ATTR_BRIGHTNESS, DEFAULT_BRIGHTNESS));

    esp_rmaker_device_add_param(light_device, esp_rmaker_hue_param_create(ESP_MATTER_ATTR_HUE, DEFAULT_HUE));

    esp_rmaker_device_add_param(light_device,
                                esp_rmaker_saturation_param_create(ESP_MATTER_ATTR_SATURATION, DEFAULT_SATURATION));

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

    esp_matter_attribute_callback_add(APP_RAINMAKER_NAME, app_rainmaker_attribute_update, NULL);
    app_rainmaker_register_commands();
    return ESP_OK;
}
