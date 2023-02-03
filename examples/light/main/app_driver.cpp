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

#include <app_priv.h>
#include "driver/gpio.h"

using namespace chip::app::Clusters;
using namespace esp_matter;

static const char *TAG = "app_driver";
extern uint16_t light_endpoint_id;

#define GPIO_OUTPUT_IO_0    GPIO_NUM_23
#define GPIO_OUTPUT_IO_1    GPIO_NUM_19
#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<GPIO_OUTPUT_IO_0) | (1ULL<<GPIO_OUTPUT_IO_1))

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
    ESP_LOGI(TAG, "Toggle buttonq pressed");
    uint16_t endpoint_id = light_endpoint_id;
    uint32_t cluster_id = OnOff::Id;
    uint32_t attribute_id = OnOff::Attributes::OnOff::Id;

    node_t *node = node::get();
    endpoint_t *endpoint = endpoint::get(node, endpoint_id);
    cluster_t *cluster = cluster::get(endpoint, cluster_id);
    attribute_t *attribute = attribute::get(cluster, attribute_id);

    esp_matter_attr_val_t val = esp_matter_invalid(NULL);
    attribute::get_val(attribute, &val);
    val.val.b = !val.val.b;
    attribute::update(endpoint_id, cluster_id, attribute_id, &val);
}
static void app_driver_button2_toggle_cb(void *arg, void *data)
{
    ESP_LOGI(TAG, "Toggle button2 pressed");
    uint16_t endpoint_id = light_endpoint_id +1;
    uint32_t cluster_id = OnOff::Id;
    uint32_t attribute_id = OnOff::Attributes::OnOff::Id;

    node_t *node = node::get();
    endpoint_t *endpoint = endpoint::get(node, endpoint_id);
    cluster_t *cluster = cluster::get(endpoint, cluster_id);
    attribute_t *attribute = attribute::get(cluster, attribute_id);

    esp_matter_attr_val_t val = esp_matter_invalid(NULL);
    attribute::get_val(attribute, &val);
    val.val.b = !val.val.b;
    attribute::update(endpoint_id, cluster_id, attribute_id, &val);
}

esp_err_t app_driver_attribute_update(app_driver_handle_t driver_handle, uint16_t endpoint_id, uint32_t cluster_id,
                                      uint32_t attribute_id, esp_matter_attr_val_t *val)
{
    esp_err_t err = ESP_OK;
    ESP_LOGI(TAG, "app_driver_attribute_update xxxx Endpoint %d",endpoint_id);
    if(endpoint_id == light_endpoint_id)
    {
        if(cluster_id == OnOff::Id)
        {
            if (attribute_id == OnOff::Attributes::OnOff::Id) 
            {
                gpio_set_level(GPIO_OUTPUT_IO_0, val->val.b);
            }
        }
    }
    if(endpoint_id == light_endpoint_id + 1)
    {
        if(cluster_id == OnOff::Id)
        {
            if (attribute_id == OnOff::Attributes::OnOff::Id) 
            {
                gpio_set_level(GPIO_OUTPUT_IO_1, val->val.b);
            }
        }
    }
    // if (endpoint_id == light_endpoint_id) {
    //     led_driver_handle_t handle = (led_driver_handle_t)driver_handle;
    //     if (cluster_id == OnOff::Id) {
    //         if (attribute_id == OnOff::Attributes::OnOff::Id) {
    //             err = app_driver_light_set_power(handle, val);
    //         }
    //     } else if (cluster_id == LevelControl::Id) {
    //         if (attribute_id == LevelControl::Attributes::CurrentLevel::Id) {
    //             err = app_driver_light_set_brightness(handle, val);
    //         }
    //     } else if (cluster_id == ColorControl::Id) {
    //         if (attribute_id == ColorControl::Attributes::CurrentHue::Id) {
    //             err = app_driver_light_set_hue(handle, val);
    //         } else if (attribute_id == ColorControl::Attributes::CurrentSaturation::Id) {
    //             err = app_driver_light_set_saturation(handle, val);
    //         } else if (attribute_id == ColorControl::Attributes::ColorTemperatureMireds::Id) {
    //             err = app_driver_light_set_temperature(handle, val);
    //         }
    //     }
    // }
    return err;
}

esp_err_t app_driver_light_set_defaults(uint16_t endpoint_id)
{
    esp_err_t err = ESP_OK;
    void *priv_data = endpoint::get_priv_data(endpoint_id);
    led_driver_handle_t handle = (led_driver_handle_t)priv_data;
    node_t *node = node::get();
    endpoint_t *endpoint = endpoint::get(node, endpoint_id);
    cluster_t *cluster = NULL;
    attribute_t *attribute = NULL;
    esp_matter_attr_val_t val = esp_matter_invalid(NULL);

    // /* Setting brightness */
    // cluster = cluster::get(endpoint, LevelControl::Id);
    // attribute = attribute::get(cluster, LevelControl::Attributes::CurrentLevel::Id);
    // attribute::get_val(attribute, &val);
    // err |= app_driver_light_set_brightness(handle, &val);

    // /* Setting color */
    // cluster = cluster::get(endpoint, ColorControl::Id);
    // attribute = attribute::get(cluster, ColorControl::Attributes::ColorMode::Id);
    // attribute::get_val(attribute, &val);
    // if (val.val.u8 == EMBER_ZCL_COLOR_MODE_CURRENT_HUE_AND_CURRENT_SATURATION) {
    //     /* Setting hue */
    //     attribute = attribute::get(cluster, ColorControl::Attributes::CurrentHue::Id);
    //     attribute::get_val(attribute, &val);
    //     err |= app_driver_light_set_hue(handle, &val);
    //     /* Setting saturation */
    //     attribute = attribute::get(cluster, ColorControl::Attributes::CurrentSaturation::Id);
    //     attribute::get_val(attribute, &val);
    //     err |= app_driver_light_set_saturation(handle, &val);
    // } else if (val.val.u8 == EMBER_ZCL_COLOR_MODE_COLOR_TEMPERATURE) {
    //     /* Setting temperature */
    //     attribute = attribute::get(cluster, ColorControl::Attributes::ColorTemperatureMireds::Id);
    //     attribute::get_val(attribute, &val);
    //     err |= app_driver_light_set_temperature(handle, &val);
    // } else {
    //     ESP_LOGE(TAG, "Color mode not supported");
    // }

    /* Setting power */
    cluster = cluster::get(endpoint, OnOff::Id);
    attribute = attribute::get(cluster, OnOff::Attributes::OnOff::Id);
    attribute::get_val(attribute, &val);
    err |= app_driver_light_set_power(handle, &val);

    return err;
}

app_driver_handle_t app_driver_light_init()
{
    /* Relay init oc cho */
        //zero-initialize the config structure.
    gpio_config_t io_conf = {};
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    //disable pull-down mode
    //io_conf.pull_down_en = 0;
    //disable pull-up mode
    //io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);
    /* Initialize led */
    led_driver_config_t config = led_driver_get_config();
    led_driver_handle_t handle = led_driver_init(&config);
    return (app_driver_handle_t)handle;
}

app_driver_handle_t app_driver_button_init()
{
    /* Initialize button */
   // button_config_t config = button_driver_get_config();
    button_config_t config_bt1 = {
        .type = BUTTON_TYPE_GPIO,
        .gpio_button_config = {
            .gpio_num = GPIO_NUM_4,
            .active_level = 0,
        }
    };
    button_config_t config_bt2 = {
        .type = BUTTON_TYPE_GPIO,
        .gpio_button_config = {
            .gpio_num = GPIO_NUM_15,
            .active_level = 0,
        }
    };
    button_handle_t handle1 = iot_button_create(&config_bt1);
    button_handle_t handle2 = iot_button_create(&config_bt2);

    iot_button_register_cb(handle1, BUTTON_PRESS_DOWN, app_driver_button_toggle_cb, NULL);
    iot_button_register_cb(handle2, BUTTON_PRESS_DOWN, app_driver_button2_toggle_cb, NULL);
    return (app_driver_handle_t)handle1;
}
