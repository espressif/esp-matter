/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_log.h>
#include <stdlib.h>
#include <string.h>

#include <device.h>
#include <esp_matter.h>
#include <esp_matter_console.h>
#include <light_driver.h>

#include <app_driver.h>

static const char *TAG = "app_driver";

#define STANDARD_BRIGHTNESS 100
#define STANDARD_HUE 360
#define STANDARD_SATURATION 100
#define STANDARD_TEMPERATURE 100

#define MATTER_BRIGHTNESS 255
#define MATTER_HUE 255
#define MATTER_SATURATION 255
#define MATTER_TEMPERATURE 255

static uint32_t app_driver_light_get_attribute(int endpoint_id, int cluster_id, int attribute_id)
{
    uint32_t value = 0;
    if (endpoint_id == ESP_MATTER_COLOR_DIMMABLE_LIGHT_ENDPOINT_ID) {
        if (cluster_id == ZCL_ON_OFF_CLUSTER_ID) {
            if (attribute_id == ZCL_ON_OFF_ATTRIBUTE_ID) {
                value = (uint32_t)light_driver_get_power();
            }
        } else if (cluster_id == ZCL_LEVEL_CONTROL_CLUSTER_ID) {
            if (attribute_id == ZCL_CURRENT_LEVEL_ATTRIBUTE_ID) {
                value = (uint32_t)light_driver_get_brightness();
                value = REMAP_TO_RANGE(value, STANDARD_BRIGHTNESS, MATTER_BRIGHTNESS);
            }
        } else if (cluster_id == ZCL_COLOR_CONTROL_CLUSTER_ID) {
            if (attribute_id == ZCL_COLOR_CONTROL_CURRENT_HUE_ATTRIBUTE_ID) {
                value = (uint32_t)light_driver_get_hue();
                value = REMAP_TO_RANGE(value, STANDARD_HUE, MATTER_HUE);
            } else if (attribute_id == ZCL_COLOR_CONTROL_CURRENT_SATURATION_ATTRIBUTE_ID) {
                value = (uint32_t)light_driver_get_saturation();
                value = REMAP_TO_RANGE(value, STANDARD_SATURATION, MATTER_SATURATION);
            }
        }
    }
    return value;
}

static esp_err_t app_driver_console_handler(int argc, char **argv)
{
    if (argc == 5 && strncmp(argv[0], "set", sizeof("set")) == 0) {
        int endpoint_id = strtol((const char *)&argv[1][2], NULL, 16);
        int cluster_id = strtol((const char *)&argv[2][2], NULL, 16);
        int attribute_id = strtol((const char *)&argv[3][2], NULL, 16);
        int value = atoi(argv[4]);

        esp_matter_attr_val_t val = esp_matter_int(value);

        /* Change val if bool */
        if (cluster_id == ZCL_ON_OFF_CLUSTER_ID && attribute_id == ZCL_ON_OFF_ATTRIBUTE_ID) {
            val.type = ESP_MATTER_VAL_TYPE_BOOLEAN;
            val.val.b = (bool)value;
        }
        esp_matter_attribute_update(endpoint_id, cluster_id, attribute_id, val);
    } else if (argc == 4 && strncmp(argv[0], "get", sizeof("get")) == 0) {
        int endpoint_id = strtol((const char *)&argv[1][2], NULL, 16);
        int cluster_id = strtol((const char *)&argv[2][2], NULL, 16);
        int attribute_id = strtol((const char *)&argv[3][2], NULL, 16);
        int value = app_driver_light_get_attribute(endpoint_id, cluster_id, attribute_id);

        esp_matter_attr_val_t val = esp_matter_int(value);

        /* Change val if bool */
        if (cluster_id == ZCL_ON_OFF_CLUSTER_ID && attribute_id == ZCL_ON_OFF_ATTRIBUTE_ID) {
            val.type = ESP_MATTER_VAL_TYPE_BOOLEAN;
            val.val.b = (bool)value;
        }
        esp_matter_attribute_val_print(endpoint_id, cluster_id, attribute_id, val);
    } else {
        ESP_LOGE(TAG, "Incorrect arguments");
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}

static void app_driver_register_commands()
{
    esp_matter_console_command_t command = {
        .name = "driver",
        .description = "This can be used to simulate on-device control. "
                       "Usage: matter esp driver <set|get> <endpoint_id> <cluster_id> <attribute_id> [value]. "
                       "Example1: matter esp driver set 0x1001 0x0006 0x0000 1. "
                       "Example2: matter esp driver get 0x1001 0x0006 0x0000.",
        .handler = app_driver_console_handler,
    };
    esp_matter_console_add_command(&command);
}

/* Do any conversions/remapping for the actual value here */
static esp_err_t app_driver_light_set_power(esp_matter_attr_val_t val)
{
    return light_driver_set_power(val.val.b);
}

static esp_err_t app_driver_light_set_brightness(esp_matter_attr_val_t val)
{
    int value = REMAP_TO_RANGE(val.val.i, MATTER_BRIGHTNESS, STANDARD_BRIGHTNESS);
    return light_driver_set_brightness(value);
}

static esp_err_t app_driver_light_set_hue(esp_matter_attr_val_t val)
{
    int value = REMAP_TO_RANGE(val.val.i, MATTER_HUE, STANDARD_HUE);
    return light_driver_set_hue(value);
}

static esp_err_t app_driver_light_set_saturation(esp_matter_attr_val_t val)
{
    int value = REMAP_TO_RANGE(val.val.i, MATTER_SATURATION, STANDARD_SATURATION);
    return light_driver_set_saturation(value);
}

esp_err_t app_driver_attribute_update(int endpoint_id, int cluster_id, int attribute_id, esp_matter_attr_val_t val)
{
    esp_err_t err = ESP_OK;
    if (endpoint_id == ESP_MATTER_COLOR_DIMMABLE_LIGHT_ENDPOINT_ID) {
        if (cluster_id == ZCL_ON_OFF_CLUSTER_ID) {
            if (attribute_id == ZCL_ON_OFF_ATTRIBUTE_ID) {
                err = app_driver_light_set_power(val);
            }
        } else if (cluster_id == ZCL_LEVEL_CONTROL_CLUSTER_ID) {
            if (attribute_id == ZCL_CURRENT_LEVEL_ATTRIBUTE_ID) {
                err = app_driver_light_set_brightness(val);
            }
        } else if (cluster_id == ZCL_COLOR_CONTROL_CLUSTER_ID) {
            if (attribute_id == ZCL_COLOR_CONTROL_CURRENT_HUE_ATTRIBUTE_ID) {
                err = app_driver_light_set_hue(val);
            } else if (attribute_id == ZCL_COLOR_CONTROL_CURRENT_SATURATION_ATTRIBUTE_ID) {
                err = app_driver_light_set_saturation(val);
            }
        }
    }
    return err;
}

esp_err_t app_driver_init()
{
    device_init();
    app_driver_register_commands();
    return ESP_OK;
}
