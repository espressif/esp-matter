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
#include <led_driver.h>

#include <app_reset.h>
#include <app_priv.h>

using namespace esp_matter;

static const char *TAG = "app_driver";
extern uint16_t light_endpoint_id;

/* Do any conversions/remapping for the actual value here */
static esp_err_t app_driver_light_set_power(esp_matter_attr_val_t *val)
{
    return led_driver_set_power(val->val.b);
}

static esp_err_t app_driver_light_set_brightness(esp_matter_attr_val_t *val)
{
    int value = REMAP_TO_RANGE(val->val.u8, MATTER_BRIGHTNESS, STANDARD_BRIGHTNESS);
    return led_driver_set_brightness(value);
}

static esp_err_t app_driver_light_set_hue(esp_matter_attr_val_t *val)
{
    int value = REMAP_TO_RANGE(val->val.u8, MATTER_HUE, STANDARD_HUE);
    return led_driver_set_hue(value);
}

static esp_err_t app_driver_light_set_saturation(esp_matter_attr_val_t *val)
{
    int value = REMAP_TO_RANGE(val->val.u8, MATTER_SATURATION, STANDARD_SATURATION);
    return led_driver_set_saturation(value);
}

static esp_err_t app_driver_light_set_temperature(esp_matter_attr_val_t *val)
{
    uint32_t value = REMAP_TO_RANGE_INVERSE(val->val.u16, STANDARD_TEMPERATURE_FACTOR);
    return led_driver_set_temperature(value);
}

static void app_driver_button_toggle_cb(void *arg)
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

esp_err_t app_driver_attribute_update(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id,
                                      esp_matter_attr_val_t *val)
{
    esp_err_t err = ESP_OK;
    if (endpoint_id == light_endpoint_id) {
        if (cluster_id == OnOff::Id) {
            if (attribute_id == OnOff::Attributes::OnOff::Id) {
                err = app_driver_light_set_power(val);
            }
        } else if (cluster_id == LevelControl::Id) {
            if (attribute_id == LevelControl::Attributes::CurrentLevel::Id) {
                err = app_driver_light_set_brightness(val);
            }
        } else if (cluster_id == ColorControl::Id) {
            if (attribute_id == ColorControl::Attributes::CurrentHue::Id) {
                err = app_driver_light_set_hue(val);
            } else if (attribute_id == ColorControl::Attributes::CurrentSaturation::Id) {
                err = app_driver_light_set_saturation(val);
            } else if (attribute_id == ColorControl::Attributes::ColorTemperature::Id) {
                err = app_driver_light_set_temperature(val);
            }
        }
    }
    return err;
}

esp_err_t app_driver_attribute_set_defaults()
{
    /* When using static endpoints, i.e. using the data model from zap-generated, this needs to be done
    after esp_matter::start() */
    /* Get the default value (current value) from matter submodule and update the app_driver */
    esp_err_t err = ESP_OK;
    uint8_t value;
    uint16_t value_u16;
    uint16_t endpoint_id = 0;
    uint32_t cluster_id = 0;
    uint32_t attribute_id = 0;
    esp_matter_attr_val_t val = esp_matter_invalid(NULL);

    endpoint_id = light_endpoint_id;
    cluster_id = OnOff::Id;
    attribute_id = OnOff::Attributes::OnOff::Id;
    attribute::get_val_raw(endpoint_id, cluster_id, attribute_id, &value, sizeof(uint8_t));
    val = esp_matter_bool(value);
    err |= app_driver_attribute_update(endpoint_id, cluster_id, attribute_id, &val);

    endpoint_id = light_endpoint_id;
    cluster_id = LevelControl::Id;
    attribute_id = LevelControl::Attributes::CurrentLevel::Id;
    attribute::get_val_raw(endpoint_id, cluster_id, attribute_id, &value, sizeof(uint8_t));
    val = esp_matter_uint8(value);
    err |= app_driver_attribute_update(endpoint_id, cluster_id, attribute_id, &val);

    endpoint_id = light_endpoint_id;
    cluster_id = ColorControl::Id;
    attribute_id = ColorControl::Attributes::CurrentHue::Id;
    attribute::get_val_raw(endpoint_id, cluster_id, attribute_id, &value, sizeof(uint8_t));
    val = esp_matter_uint8(value);
    err |= app_driver_attribute_update(endpoint_id, cluster_id, attribute_id, &val);

    endpoint_id = light_endpoint_id;
    cluster_id = ColorControl::Id;
    attribute_id = ColorControl::Attributes::CurrentSaturation::Id;
    attribute::get_val_raw(endpoint_id, cluster_id, attribute_id, &value, sizeof(uint8_t));
    val = esp_matter_uint8(value);
    err |= app_driver_attribute_update(endpoint_id, cluster_id, attribute_id, &val);

    endpoint_id = light_endpoint_id;
    cluster_id = ColorControl::Id;
    attribute_id = ColorControl::Attributes::ColorTemperature::Id;
    attribute::get_val_raw(endpoint_id, cluster_id, attribute_id, (uint8_t *)&value_u16, sizeof(uint16_t));
    val = esp_matter_uint16(value_u16);
    err |= app_driver_attribute_update(endpoint_id, cluster_id, attribute_id, &val);

    return err;
}

esp_err_t app_driver_init()
{
    ESP_LOGI(TAG, "Initialising driver");

    /* Initialize button */
    button_config_t button_config = button_driver_get_config();
    button_handle_t handle = iot_button_create(&button_config);
    iot_button_register_cb(handle, BUTTON_PRESS_DOWN, app_driver_button_toggle_cb);
    app_reset_button_register(handle);

    /* Initialize led */
    led_driver_config_t led_config = led_driver_get_config();
    led_driver_init(&led_config);

    /* Attribute defaults are set after esp_matter::start() from app_main() */
    return ESP_OK;
}
