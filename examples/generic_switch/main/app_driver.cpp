/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_log.h>
#include <stdlib.h>
#include <string.h>

#include <esp_matter.h>

#include <app_priv.h>
#include <iot_button.h>
#include <button_gpio.h>

#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S3
#define BUTTON_GPIO_PIN GPIO_NUM_0
#else // CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32C6 || CONFIG_IDF_TARGET_ESP32H2 || CONFIG_IDF_TARGET_ESP32C2
#define BUTTON_GPIO_PIN GPIO_NUM_9
#endif

using namespace chip::app::Clusters;
using namespace esp_matter;
using namespace esp_matter::cluster;

static const char *TAG = "app_driver";

static void driver_set_switch_current_position(uint16_t endpoint_id, uint8_t position)
{
    esp_matter_attr_val_t val(position);
    esp_err_t err = esp_matter::attribute::update(endpoint_id, Switch::Id, Switch::Attributes::CurrentPosition::Id, &val);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Switch CurrentPosition update failed: %s", esp_err_to_name(err));
    }
}

esp_err_t app_driver_attribute_update(app_driver_handle_t driver_handle, uint16_t endpoint_id, uint32_t cluster_id,
                                      uint32_t attribute_id, esp_matter_attr_val_t *val)
{
    esp_err_t err = ESP_OK;
    return err;
}

#if CONFIG_GENERIC_SWITCH_TYPE_LATCHING
static uint8_t latching_switch_previous_position = 0;
static void app_driver_button_switch_latched(void *arg, void *data)
{
    ESP_LOGI(TAG, "Switch lached pressed");
    gpio_button * button = (gpio_button*)data;
    int switch_endpoint_id = (button != NULL) ? get_endpoint(button) : 1;
    // Press moves Position from 0 (idle) to 1 (press) and vice versa
    uint8_t newPosition = (latching_switch_previous_position == 1) ? 0 : 1;
    latching_switch_previous_position = newPosition;
    chip::DeviceLayer::SystemLayer().ScheduleLambda([switch_endpoint_id, newPosition]() {
        driver_set_switch_current_position(static_cast<uint16_t>(switch_endpoint_id), newPosition);
        // SwitchLatched event takes newPosition as event data
        switch_cluster::event::send_switch_latched(switch_endpoint_id, newPosition);
    });
}
#endif
#if CONFIG_GENERIC_SWITCH_TYPE_MOMENTARY
static int current_number_of_presses_counted = 1;
static bool is_multipress = 0;
static uint8_t idlePosition    = 0;

static void app_driver_button_initial_pressed(void *arg, void *data)
{
    if (!is_multipress) {
        ESP_LOGI(TAG, "Initial button pressed");
        gpio_button * button = (gpio_button*)data;
        int switch_endpoint_id = (button != NULL) ? get_endpoint(button) : 1;
        // Press moves Position from 0 (idle) to 1 (press)
        uint8_t newPosition     = 1;
        chip::DeviceLayer::SystemLayer().ScheduleLambda([switch_endpoint_id, newPosition]() {
            driver_set_switch_current_position(static_cast<uint16_t>(switch_endpoint_id), newPosition);
            // InitialPress event takes newPosition as event data
            switch_cluster::event::send_initial_press(switch_endpoint_id, newPosition);
        });
        is_multipress = 1;
    }
}

static void app_driver_button_release(void *arg, void *data)
{

    gpio_button *button = (gpio_button *)data;
    int switch_endpoint_id = (button != NULL) ? get_endpoint(button) : 1;
    chip::DeviceLayer::SystemLayer().ScheduleLambda([switch_endpoint_id]() {
        driver_set_switch_current_position(static_cast<uint16_t>(switch_endpoint_id), idlePosition);
    });
}

static void app_driver_button_multipress_ongoing(void *arg, void *data)
{
    ESP_LOGI(TAG, "Multipress Ongoing");
    gpio_button * button = (gpio_button *)data;
    int switch_endpoint_id = (button != NULL) ? get_endpoint(button) : 1;
    // Press moves Position from 0 (idle) to 1 (press)
    uint8_t newPosition = 1;
    current_number_of_presses_counted++;
    uint16_t endpoint_id = switch_endpoint_id;
    uint32_t cluster_id = Switch::Id;
    uint32_t attribute_id = Switch::Attributes::FeatureMap::Id;

    attribute_t *attribute = attribute::get(endpoint_id, cluster_id, attribute_id);

    esp_matter_attr_val_t val;
    attribute::get_val(attribute, &val);

    uint32_t feature_map = val.val.u32;
    uint32_t msm_feature_map = switch_cluster::feature::momentary_switch_multi_press::get_id();
    uint32_t as_feature_map = switch_cluster::feature::action_switch::get_id();
    if (((feature_map & msm_feature_map) == msm_feature_map) && ((feature_map & as_feature_map) != as_feature_map)) {
        chip::DeviceLayer::SystemLayer().ScheduleLambda([switch_endpoint_id, newPosition]() {
            driver_set_switch_current_position(static_cast<uint16_t>(switch_endpoint_id), newPosition);
            // MultiPress Ongoing event takes newPosition and current_number_of_presses_counted as event data
            switch_cluster::event::send_multi_press_ongoing(switch_endpoint_id, newPosition, current_number_of_presses_counted);
        });
    }
}

static void app_driver_button_multipress_complete(void *arg, void *data)
{
    ESP_LOGI(TAG, "Multipress Complete");
    gpio_button * button = (gpio_button *)data;
    int switch_endpoint_id = (button != NULL) ? get_endpoint(button) : 1;
    // Press moves Position from 0 (idle) to 1 (press)
    uint8_t previousPosition = 1;
    uint16_t endpoint_id = switch_endpoint_id;
    uint32_t cluster_id = Switch::Id;
    uint32_t attribute_id = Switch::Attributes::MultiPressMax::Id;

    attribute_t *attribute = attribute::get(endpoint_id, cluster_id, attribute_id);

    esp_matter_attr_val_t val;
    attribute::get_val(attribute, &val);
    uint8_t multipress_max = val.val.u8;
    int total_number_of_presses_counted = (current_number_of_presses_counted > multipress_max) ? 0 : current_number_of_presses_counted;
    chip::DeviceLayer::SystemLayer().ScheduleLambda([switch_endpoint_id, previousPosition, total_number_of_presses_counted]() {
        driver_set_switch_current_position(static_cast<uint16_t>(switch_endpoint_id), idlePosition);
        // MultiPress Complete event takes previousPosition and total_number_of_presses_counted as event data
        switch_cluster::event::send_multi_press_complete(switch_endpoint_id, previousPosition, total_number_of_presses_counted);
        // Reset current_number_of_presses_counted to initial value
        current_number_of_presses_counted = 1;
    });
    is_multipress = 0;
}
#endif

app_driver_handle_t app_driver_button_init(gpio_button * button)
{
    /* Initialize button */
    button_handle_t handle = NULL;
    const button_config_t btn_cfg = {0};

    if (button != NULL) {
        const button_gpio_config_t btn_gpio_cfg = {
            .gpio_num = button->GPIO_PIN_VALUE,
            .active_level = 0,
        };
        if (iot_button_new_gpio_device(&btn_cfg, &btn_gpio_cfg, &handle) != ESP_OK) {
            ESP_LOGE(TAG, "Failed to create button device");
            return NULL;
        }
    } else {
        const button_gpio_config_t btn_gpio_cfg = {
            .gpio_num = BUTTON_GPIO_PIN,
            .active_level = 0,
        };
        if (iot_button_new_gpio_device(&btn_cfg, &btn_gpio_cfg, &handle) != ESP_OK) {
            ESP_LOGE(TAG, "Failed to create button device");
            return NULL;
        }
    }

#if CONFIG_GENERIC_SWITCH_TYPE_LATCHING
    iot_button_register_cb(handle, BUTTON_PRESS_DOWN, NULL, app_driver_button_switch_latched, button);
#endif

#if CONFIG_GENERIC_SWITCH_TYPE_MOMENTARY
    iot_button_register_cb(handle, BUTTON_PRESS_DOWN, NULL, app_driver_button_initial_pressed, button);
    iot_button_register_cb(handle, BUTTON_PRESS_UP, NULL, app_driver_button_release, button);
    iot_button_register_cb(handle, BUTTON_PRESS_REPEAT, NULL, app_driver_button_multipress_ongoing, button);
    iot_button_register_cb(handle, BUTTON_PRESS_REPEAT_DONE, NULL, app_driver_button_multipress_complete, button);
#endif
    return (app_driver_handle_t)handle;
}
