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

#include <app_reset.h>
#include <app_priv.h>
#include <common_macros.h>
#include <device.h>

#include <app-common/zap-generated/attribute-type.h>
#include <app-common/zap-generated/attributes/Accessors.h>
#include "app/data-model/Nullable.h"
#include "platform/PlatformManager.h"

using namespace chip::app::Clusters;
using namespace esp_matter;

static const char *TAG = "app_driver";
extern uint16_t light_endpoint_id;
extern app_driver_handle_t light_handle;

/* Do any conversions/remapping for the actual value here */
static esp_err_t app_driver_light_set_power(led_driver_handle_t handle, bool val)
{
    return led_driver_set_power(handle, val);
}

static esp_err_t app_driver_light_set_brightness(led_driver_handle_t handle, uint8_t val)
{
    int value = REMAP_TO_RANGE(val, MATTER_BRIGHTNESS, STANDARD_BRIGHTNESS);
    return led_driver_set_brightness(handle, value);
}

static esp_err_t app_driver_light_set_hue(led_driver_handle_t handle, uint8_t val)
{
    int value = REMAP_TO_RANGE(val, MATTER_HUE, STANDARD_HUE);
    return led_driver_set_hue(handle, value);
}

static esp_err_t app_driver_light_set_saturation(led_driver_handle_t handle, uint8_t val)
{
    int value = REMAP_TO_RANGE(val, MATTER_SATURATION, STANDARD_SATURATION);
    return led_driver_set_saturation(handle, value);
}

static esp_err_t app_driver_light_set_temperature(led_driver_handle_t handle, uint16_t val)
{
    uint32_t value = REMAP_TO_RANGE_INVERSE(val, STANDARD_TEMPERATURE_FACTOR);
    return led_driver_set_temperature(handle, value);
}

static void app_driver_button_toggle_cb(void *arg, void *data)
{
    ESP_LOGI(TAG, "Toggle button pressed");

    chip::DeviceLayer::PlatformMgr().ScheduleWork([](intptr_t arg){
        bool value;
        OnOff::Attributes::OnOff::Get(light_endpoint_id, &value);
        value = !value;
        OnOff::Attributes::OnOff::Set(light_endpoint_id, value);
    });
}

esp_err_t app_driver_attribute_update(app_driver_handle_t driver_handle, uint16_t endpoint_id, uint32_t cluster_id,
                                      uint32_t attribute_id, uint8_t type,  uint8_t *val_raw, uint16_t val_raw_size)
{
    esp_err_t err = ESP_OK;
    if (endpoint_id == light_endpoint_id) {
        led_driver_handle_t handle = (led_driver_handle_t)driver_handle;
        if (cluster_id == OnOff::Id) {
            if (attribute_id == OnOff::Attributes::OnOff::Id) {
                if (type == ZCL_BOOLEAN_ATTRIBUTE_TYPE && val_raw_size == sizeof(bool)) {
                    bool val;
                    memcpy(&val, val_raw, val_raw_size);
                    err = app_driver_light_set_power(handle, val);
                }
            }
        } else if (cluster_id == LevelControl::Id) {
            if (attribute_id == LevelControl::Attributes::CurrentLevel::Id) {
                if (type == ZCL_INT8U_ATTRIBUTE_TYPE && val_raw_size == sizeof(uint8_t)) {
                    uint8_t val;
                    memcpy(&val, val_raw, val_raw_size);
                    err = app_driver_light_set_brightness(handle, val);
                }
            }
        } else if (cluster_id == ColorControl::Id) {
            if (attribute_id == ColorControl::Attributes::CurrentHue::Id) {
                if (type == ZCL_INT8U_ATTRIBUTE_TYPE && val_raw_size == sizeof(uint8_t)) {
                    uint8_t val;
                    memcpy(&val, val_raw, val_raw_size);
                    err = app_driver_light_set_hue(handle, val);
                }
            } else if (attribute_id == ColorControl::Attributes::CurrentSaturation::Id) {
                if (type == ZCL_INT8U_ATTRIBUTE_TYPE && val_raw_size == sizeof(uint8_t)) {
                    uint8_t val;
                    memcpy(&val, val_raw, val_raw_size);
                    err = app_driver_light_set_saturation(handle, val);
                }
            } else if (attribute_id == ColorControl::Attributes::ColorTemperatureMireds::Id) {
                if (type == ZCL_INT16U_ATTRIBUTE_TYPE && val_raw_size == sizeof(uint16_t)) {
                    uint16_t val;
                    memcpy(&val, val_raw, val_raw_size);
                    err = app_driver_light_set_temperature(handle, val);
                }
            }
        }
    }
    return err;
}

esp_err_t app_driver_light_set_defaults(uint16_t endpoint_id)
{
    /* When using static endpoints, i.e. using the data model from zap-generated, this needs to be done
    after esp_matter::start() */
    /* Get the default value (current value) from matter submodule and update the app_driver */
    esp_err_t err = ESP_OK;
    led_driver_handle_t handle = (led_driver_handle_t)light_handle;
    uint8_t value;
    uint16_t value_u16;
    chip::app::DataModel::Nullable<uint8_t> nullable_value;

    /* Setting brightness */
    LevelControl::Attributes::CurrentLevel::Get(endpoint_id, nullable_value);
    if (!nullable_value.IsNull()) {
        err |= app_driver_light_set_brightness(handle, nullable_value.Value());
    }

    /* Setting color */
    ColorControl::ColorModeEnum mode;
    ColorControl::Attributes::ColorMode::Get(endpoint_id, &mode);
    if (mode == ColorControl::ColorModeEnum::kCurrentHueAndCurrentSaturation) {
        /* Setting hue */
        ColorControl::Attributes::CurrentHue::Get(endpoint_id, &value);
        err |= app_driver_light_set_hue(handle, value);
        /* Setting saturation */
        ColorControl::Attributes::CurrentSaturation::Get(endpoint_id, &value);
        err |= app_driver_light_set_saturation(handle, value);
    } else if (value == (uint8_t)ColorControl::ColorMode::kColorTemperature) {
        /* Setting temperature */
        ColorControl::Attributes::ColorTemperatureMireds::Get(endpoint_id, &value_u16);
        err |= app_driver_light_set_temperature(handle, value_u16);
    } else {
        ESP_LOGE(TAG, "Color mode not supported");
    }

    /* Setting power */
    bool onoff;
    OnOff::Attributes::OnOff::Get(endpoint_id, &onoff);
    err |= app_driver_light_set_power(handle, onoff);

    return err;
}

app_driver_handle_t app_driver_light_init()
{
    /* Initialize led */
    led_driver_config_t config = led_driver_get_config();
    led_driver_handle_t handle = led_driver_init(&config);
    return (app_driver_handle_t)handle;
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
