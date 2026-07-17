/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_log.h>

#include <esp_matter.h>
#include <app_priv.h>
#include <common_macros.h>
#include <button_gpio.h>
#include <device.h>
#include <led_driver.h>

using namespace chip::app::Clusters;
using namespace esp_matter;

static const char *TAG = "app_driver";
extern uint16_t light_endpoint_id;

static led_driver_handle_t s_light_handle = nullptr;
static uint16_t s_current_x = 0;
static uint16_t s_current_y = 0;

esp_err_t app_driver_light_set_power(app_driver_handle_t handle, esp_matter_attr_val_t *val)
{
    return led_driver_set_power((led_driver_handle_t)handle, val->val.b);
}

esp_err_t app_driver_light_set_brightness(app_driver_handle_t handle, esp_matter_attr_val_t *val)
{
    int value = REMAP_TO_RANGE(val->val.u8, MATTER_BRIGHTNESS, STANDARD_BRIGHTNESS);
    return led_driver_set_brightness((led_driver_handle_t)handle, value);
}

static esp_err_t app_driver_light_set_hue(app_driver_handle_t handle, esp_matter_attr_val_t *val)
{
    int value = REMAP_TO_RANGE(val->val.u8, MATTER_HUE, STANDARD_HUE);
    return led_driver_set_hue((led_driver_handle_t)handle, value);
}

static esp_err_t app_driver_light_set_saturation(app_driver_handle_t handle, esp_matter_attr_val_t *val)
{
    int value = REMAP_TO_RANGE(val->val.u8, MATTER_SATURATION, STANDARD_SATURATION);
    return led_driver_set_saturation((led_driver_handle_t)handle, value);
}

static esp_err_t app_driver_light_set_temperature(app_driver_handle_t handle, esp_matter_attr_val_t *val)
{
    uint32_t value = REMAP_TO_RANGE_INVERSE(val->val.u16, STANDARD_TEMPERATURE_FACTOR);
    return led_driver_set_temperature((led_driver_handle_t)handle, value);
}

esp_err_t app_driver_light_set_xy(app_driver_handle_t handle, uint16_t x, uint16_t y)
{
    s_current_x = x;
    s_current_y = y;
    return led_driver_set_xy((led_driver_handle_t)handle, x, y);
}

static void app_driver_button_toggle_cb(void *arg, void *data)
{
    ESP_LOGI(TAG, "Toggle button pressed");

    attribute_t *attribute =
        attribute::get(light_endpoint_id, OnOff::Id, OnOff::Attributes::OnOff::Id);
    esp_matter_attr_val_t val;
    attribute::get_val(attribute, &val);
    val.val.b = !val.val.b;
    attribute::update(light_endpoint_id, OnOff::Id, OnOff::Attributes::OnOff::Id, &val);
}

esp_err_t app_driver_attribute_update(app_driver_handle_t driver_handle, uint16_t endpoint_id,
                                      uint32_t cluster_id, uint32_t attribute_id, esp_matter_attr_val_t *val)
{
    if (endpoint_id != light_endpoint_id) {
        return ESP_OK;
    }

    esp_err_t err = ESP_OK;
    if (cluster_id == OnOff::Id && attribute_id == OnOff::Attributes::OnOff::Id) {
        err = app_driver_light_set_power(driver_handle, val);
    } else if (cluster_id == LevelControl::Id && attribute_id == LevelControl::Attributes::CurrentLevel::Id) {
        err = app_driver_light_set_brightness(driver_handle, val);
    } else if (cluster_id == ColorControl::Id) {
        if (attribute_id == ColorControl::Attributes::CurrentHue::Id) {
            err = app_driver_light_set_hue(driver_handle, val);
        } else if (attribute_id == ColorControl::Attributes::CurrentSaturation::Id) {
            err = app_driver_light_set_saturation(driver_handle, val);
        } else if (attribute_id == ColorControl::Attributes::ColorTemperatureMireds::Id) {
            err = app_driver_light_set_temperature(driver_handle, val);
        } else if (attribute_id == ColorControl::Attributes::CurrentX::Id) {
            err = app_driver_light_set_xy(driver_handle, val->val.u16, s_current_y);
        } else if (attribute_id == ColorControl::Attributes::CurrentY::Id) {
            err = app_driver_light_set_xy(driver_handle, s_current_x, val->val.u16);
        }
    }

    return err;
}

esp_err_t app_driver_light_set_defaults(uint16_t endpoint_id)
{
    app_driver_handle_t handle = (app_driver_handle_t)endpoint::get_priv_data(endpoint_id);
    esp_matter_attr_val_t val;
    esp_err_t err = ESP_OK;

    attribute_t *attribute =
        attribute::get(endpoint_id, LevelControl::Id, LevelControl::Attributes::CurrentLevel::Id);
    attribute::get_val(attribute, &val);
    err |= app_driver_light_set_brightness(handle, &val);

    attribute = attribute::get(endpoint_id, ColorControl::Id, ColorControl::Attributes::ColorMode::Id);
    attribute::get_val(attribute, &val);
    if (val.val.u8 == (uint8_t)ColorControl::ColorMode::kCurrentHueAndCurrentSaturation) {
        attribute = attribute::get(endpoint_id, ColorControl::Id, ColorControl::Attributes::CurrentHue::Id);
        attribute::get_val(attribute, &val);
        err |= app_driver_light_set_hue(handle, &val);

        attribute = attribute::get(endpoint_id, ColorControl::Id, ColorControl::Attributes::CurrentSaturation::Id);
        attribute::get_val(attribute, &val);
        err |= app_driver_light_set_saturation(handle, &val);
    } else if (val.val.u8 == (uint8_t)ColorControl::ColorMode::kColorTemperature) {
        attribute =
            attribute::get(endpoint_id, ColorControl::Id, ColorControl::Attributes::ColorTemperatureMireds::Id);
        attribute::get_val(attribute, &val);
        err |= app_driver_light_set_temperature(handle, &val);
    } else if (val.val.u8 == (uint8_t)ColorControl::ColorMode::kCurrentXAndCurrentY) {
        attribute = attribute::get(endpoint_id, ColorControl::Id, ColorControl::Attributes::CurrentX::Id);
        attribute::get_val(attribute, &val);
        s_current_x = val.val.u16;

        attribute = attribute::get(endpoint_id, ColorControl::Id, ColorControl::Attributes::CurrentY::Id);
        attribute::get_val(attribute, &val);
        s_current_y = val.val.u16;

        err |= app_driver_light_set_xy(handle, s_current_x, s_current_y);
    } else {
        ESP_LOGE(TAG, "Color mode not supported");
    }

    attribute = attribute::get(endpoint_id, OnOff::Id, OnOff::Attributes::OnOff::Id);
    attribute::get_val(attribute, &val);
    err |= app_driver_light_set_power(handle, &val);

    return err;
}

app_driver_handle_t app_driver_light_get_handle()
{
    return (app_driver_handle_t)s_light_handle;
}

app_driver_handle_t app_driver_light_init()
{
    led_driver_config_t config = led_driver_get_config();
    s_light_handle = led_driver_init(&config);
    return (app_driver_handle_t)s_light_handle;
}

app_driver_handle_t app_driver_button_init()
{
    button_handle_t handle = nullptr;
    const button_config_t btn_cfg = {};
    const button_gpio_config_t btn_gpio_cfg = button_driver_get_config();

    if (iot_button_new_gpio_device(&btn_cfg, &btn_gpio_cfg, &handle) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create button device");
        return nullptr;
    }

    iot_button_register_cb(handle, BUTTON_PRESS_DOWN, nullptr, app_driver_button_toggle_cb, nullptr);
    return (app_driver_handle_t)handle;
}
