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
#include <light_driver.h>

#include <app_driver.h>

static const char *TAG = "app_driver";
extern int light_endpoint_id;

#define STANDARD_BRIGHTNESS 100
#define STANDARD_HUE 360
#define STANDARD_SATURATION 100
#define STANDARD_TEMPERATURE 100

#define MATTER_BRIGHTNESS 255
#define MATTER_HUE 255
#define MATTER_SATURATION 255
#define MATTER_TEMPERATURE 255

/* Do any conversions/remapping for the actual value here */
static esp_err_t app_driver_light_set_power(esp_matter_attr_val_t *val)
{
    return light_driver_set_power(val->val.b);
}

static esp_err_t app_driver_light_set_brightness(esp_matter_attr_val_t *val)
{
    int value = REMAP_TO_RANGE(val->val.u8, MATTER_BRIGHTNESS, STANDARD_BRIGHTNESS);
    return light_driver_set_brightness(value);
}

static esp_err_t app_driver_light_set_hue(esp_matter_attr_val_t *val)
{
    int value = REMAP_TO_RANGE(val->val.u8, MATTER_HUE, STANDARD_HUE);
    return light_driver_set_hue(value);
}

static esp_err_t app_driver_light_set_saturation(esp_matter_attr_val_t *val)
{
    int value = REMAP_TO_RANGE(val->val.u8, MATTER_SATURATION, STANDARD_SATURATION);
    return light_driver_set_saturation(value);
}

esp_err_t app_driver_attribute_update(int endpoint_id, int cluster_id, int attribute_id, esp_matter_attr_val_t *val)
{
    esp_err_t err = ESP_OK;
    if (endpoint_id == light_endpoint_id) {
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

esp_err_t app_driver_attribute_set_defaults()
{
    /* When using static endpoints, i.e. using the data model from zap-generated, this needs to be done
    after esp_matter_start() */
    /* Get the default value (current value) from matter submodule and update the app_driver */
    esp_err_t err = ESP_OK;
    uint8_t value;
    int endpoint_id = 0;
    int cluster_id = 0;
    int attribute_id = 0;
    esp_matter_attr_val_t val = esp_matter_invalid(NULL);

    endpoint_id = light_endpoint_id;
    cluster_id = ZCL_ON_OFF_CLUSTER_ID;
    attribute_id = ZCL_ON_OFF_ATTRIBUTE_ID;
    esp_matter_attribute_get_val_raw(endpoint_id, cluster_id, attribute_id, &value, sizeof(uint8_t));
    val = esp_matter_bool(value);
    err |= app_driver_attribute_update(endpoint_id, cluster_id, attribute_id, &val);

    endpoint_id = light_endpoint_id;
    cluster_id = ZCL_LEVEL_CONTROL_CLUSTER_ID;
    attribute_id = ZCL_CURRENT_LEVEL_ATTRIBUTE_ID;
    esp_matter_attribute_get_val_raw(endpoint_id, cluster_id, attribute_id, &value, sizeof(uint8_t));
    val = esp_matter_uint8(value);
    err |= app_driver_attribute_update(endpoint_id, cluster_id, attribute_id, &val);

    endpoint_id = light_endpoint_id;
    cluster_id = ZCL_COLOR_CONTROL_CLUSTER_ID;
    attribute_id = ZCL_COLOR_CONTROL_CURRENT_HUE_ATTRIBUTE_ID;
    esp_matter_attribute_get_val_raw(endpoint_id, cluster_id, attribute_id, &value, sizeof(uint8_t));
    val = esp_matter_uint8(value);
    err |= app_driver_attribute_update(endpoint_id, cluster_id, attribute_id, &val);

    endpoint_id = light_endpoint_id;
    cluster_id = ZCL_COLOR_CONTROL_CLUSTER_ID;
    attribute_id = ZCL_COLOR_CONTROL_CURRENT_SATURATION_ATTRIBUTE_ID;
    esp_matter_attribute_get_val_raw(endpoint_id, cluster_id, attribute_id, &value, sizeof(uint8_t));
    val = esp_matter_uint8(value);
    err |= app_driver_attribute_update(endpoint_id, cluster_id, attribute_id, &val);

    return err;
}

esp_err_t app_driver_init()
{
    ESP_LOGI(TAG, "Initialising driver");
    device_init();
    /* Attribute defaults are set after esp_matter_start() from app_main() */
    return ESP_OK;
}
