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
#include <device.h>

using namespace chip::app::Clusters;
using namespace esp_matter;

static const char *TAG = "app_driver";
extern uint16_t light_endpoint_id;
extern app_driver_handle_t light_handle;

/* Do any conversions/remapping for the actual value here */
static esp_err_t app_driver_light_set_power(led_driver_handle_t handle, esp_matter_attr_val_t *val)
{
    return led_driver_set_power(handle, val->val.b);
}

static esp_err_t app_driver_light_set_brightness(led_driver_handle_t handle, esp_matter_attr_val_t *val)
{
    int value = REMAP_TO_RANGE(val->val.u8, MATTER_BRIGHTNESS, STANDARD_BRIGHTNESS);
    return led_driver_set_brightness(handle, value);
}

static esp_err_t app_driver_light_set_hue(led_driver_handle_t handle, esp_matter_attr_val_t *val)
{
    int value = REMAP_TO_RANGE(val->val.u8, MATTER_HUE, STANDARD_HUE);
    return led_driver_set_hue(handle, value);
}

static esp_err_t app_driver_light_set_saturation(led_driver_handle_t handle, esp_matter_attr_val_t *val)
{
    int value = REMAP_TO_RANGE(val->val.u8, MATTER_SATURATION, STANDARD_SATURATION);
    return led_driver_set_saturation(handle, value);
}

static esp_err_t app_driver_light_set_temperature(led_driver_handle_t handle, esp_matter_attr_val_t *val)
{
    uint32_t value = REMAP_TO_RANGE_INVERSE(val->val.u16, STANDARD_TEMPERATURE_FACTOR);
    return led_driver_set_temperature(handle, value);
}

static void app_driver_button_toggle_cb(void *arg, void *data)
{
    ESP_LOGI(TAG, "Toggle button pressed");
    uint16_t endpoint_id = light_endpoint_id;
    uint32_t cluster_id = OnOff::Id;
    uint32_t attribute_id = OnOff::Attributes::OnOff::Id;

    uint8_t value;
    attribute::get_val_raw(endpoint_id, cluster_id, attribute_id, &value, sizeof(uint8_t));
    esp_matter_attr_val_t val = esp_matter_bool(value);
    val.val.b = !val.val.b;
    attribute::update(endpoint_id, cluster_id, attribute_id, &val);
}

esp_err_t app_driver_attribute_update(app_driver_handle_t driver_handle, uint16_t endpoint_id, uint32_t cluster_id,
                                      uint32_t attribute_id, esp_matter_attr_val_t *val)
{
    esp_err_t err = ESP_OK;
    if (endpoint_id == light_endpoint_id) {
        led_driver_handle_t handle = (led_driver_handle_t)driver_handle;
        if (cluster_id == OnOff::Id) {
            if (attribute_id == OnOff::Attributes::OnOff::Id) {
                err = app_driver_light_set_power(handle, val);
            }
        } else if (cluster_id == LevelControl::Id) {
            if (attribute_id == LevelControl::Attributes::CurrentLevel::Id) {
                err = app_driver_light_set_brightness(handle, val);
            }
        } else if (cluster_id == ColorControl::Id) {
            if (attribute_id == ColorControl::Attributes::CurrentHue::Id) {
                err = app_driver_light_set_hue(handle, val);
            } else if (attribute_id == ColorControl::Attributes::CurrentSaturation::Id) {
                err = app_driver_light_set_saturation(handle, val);
            } else if (attribute_id == ColorControl::Attributes::ColorTemperatureMireds::Id) {
                err = app_driver_light_set_temperature(handle, val);
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
    uint32_t cluster_id = 0;
    uint32_t attribute_id = 0;
    esp_matter_attr_val_t val = esp_matter_invalid(NULL);

    /* Setting brightness */
    cluster_id = LevelControl::Id;
    attribute_id = LevelControl::Attributes::CurrentLevel::Id;
    attribute::get_val_raw(endpoint_id, cluster_id, attribute_id, &value, sizeof(uint8_t));
    val = esp_matter_uint8(value);
    err |= app_driver_light_set_brightness(handle, &val);

    /* Setting color */
    cluster_id = ColorControl::Id;
    attribute_id = ColorControl::Attributes::CurrentHue::Id;
    attribute::get_val_raw(endpoint_id, cluster_id, attribute_id, &value, sizeof(uint8_t));
    if (value == (uint8_t)ColorControl::ColorMode::kCurrentHueAndCurrentSaturation) {
        /* Setting hue */
        attribute_id = ColorControl::Attributes::CurrentHue::Id;
        attribute::get_val_raw(endpoint_id, cluster_id, attribute_id, &value, sizeof(uint8_t));
        val = esp_matter_uint8(value);
        err |= app_driver_light_set_hue(handle, &val);
        /* Setting saturation */
        attribute_id = ColorControl::Attributes::CurrentSaturation::Id;
        attribute::get_val_raw(endpoint_id, cluster_id, attribute_id, &value, sizeof(uint8_t));
        val = esp_matter_uint8(value);
        err |= app_driver_light_set_saturation(handle, &val);
    } else if (value == (uint8_t)ColorControl::ColorMode::kColorTemperature) {
        /* Setting temperature */
        attribute_id = ColorControl::Attributes::ColorTemperatureMireds::Id;
        attribute::get_val_raw(endpoint_id, cluster_id, attribute_id, (uint8_t *)&value_u16, sizeof(uint16_t));
        val = esp_matter_uint16(value_u16);
        err |= app_driver_light_set_temperature(handle, &val);
    } else {
        ESP_LOGE(TAG, "Color mode not supported");
    }

    /* Setting power */
    cluster_id = OnOff::Id;
    attribute_id = OnOff::Attributes::OnOff::Id;
    attribute::get_val_raw(endpoint_id, cluster_id, attribute_id, &value, sizeof(uint8_t));
    val = esp_matter_bool(value);
    err |= app_driver_light_set_power(handle, &val);

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
