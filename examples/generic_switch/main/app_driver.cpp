/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_log.h>
#include <stdlib.h>
#include <string.h>

#include "bsp/esp-bsp.h"
#include <esp_matter.h>
#include <app-common/zap-generated/attributes/Accessors.h>

#include <app_priv.h>

using namespace chip::app::Clusters;
using namespace esp_matter;
using namespace esp_matter::cluster;

static const char *TAG = "app_driver";

esp_err_t app_driver_attribute_update(app_driver_handle_t driver_handle, uint16_t endpoint_id, uint32_t cluster_id,
                                      uint32_t attribute_id, esp_matter_attr_val_t *val)
{
    esp_err_t err = ESP_OK;
    return err;
}

#if CONFIG_GENERIC_SWITCH_TYPE_LATCHING
static void app_driver_button_switch_latched(void *arg, void *data)
{
    ESP_LOGI(TAG, "Switch lached pressed");
    gpio_button * button = (gpio_button*)data;
    int switch_endpoint_id = (button != NULL) ? get_endpoint(button) : 1;
    // Press moves Position from 0 (idle) to 1 (press)
    uint8_t newPosition = 1;
    lock::chip_stack_lock(portMAX_DELAY);
    chip::app::Clusters::Switch::Attributes::CurrentPosition::Set(switch_endpoint_id, newPosition);
    // SwitchLatched event takes newPosition as event data
    switch_cluster::event::send_switch_latched(switch_endpoint_id, newPosition);
    lock::chip_stack_unlock();
}
#endif
#if CONFIG_GENERIC_SWITCH_TYPE_MOMENTARY
static void app_driver_button_initial_pressed(void *arg, void *data)
{
    ESP_LOGI(TAG, "Initial button pressed");
    gpio_button * button = (gpio_button*)data;
    int switch_endpoint_id = (button != NULL) ? get_endpoint(button) : 1;
    // Press moves Position from 0 (idle) to 1 (press)
    uint8_t newPosition = 1;
    lock::chip_stack_lock(portMAX_DELAY);
    chip::app::Clusters::Switch::Attributes::CurrentPosition::Set(switch_endpoint_id, newPosition);
    // InitialPress event takes newPosition as event data
    switch_cluster::event::send_initial_press(switch_endpoint_id, newPosition);
    lock::chip_stack_unlock();
}

static void app_driver_button_release(void *arg, void *data)
{
    gpio_button *button = (gpio_button *)data;
    int switch_endpoint_id = (button != NULL) ? get_endpoint(button) : 1;
    if(iot_button_get_ticks_time((button_handle_t)arg) < 5000){
    ESP_LOGI(TAG, "Short button release");
    // Release moves Position from 1 (press) to 0 (idle)
    uint8_t previousPosition = 1;
    uint8_t newPosition = 0;
    lock::chip_stack_lock(portMAX_DELAY);
    chip::app::Clusters::Switch::Attributes::CurrentPosition::Set(switch_endpoint_id, newPosition);
    // ShortRelease event takes previousPosition as event data
    switch_cluster::event::send_short_release(switch_endpoint_id, previousPosition);
    lock::chip_stack_unlock();
    }
    else{
    ESP_LOGI(TAG, "Long button release");
    // Release moves Position from 1 (press) to 0 (idle)
    uint8_t previousPosition = 1;
    uint8_t newPosition = 0;
    lock::chip_stack_lock(portMAX_DELAY);
    chip::app::Clusters::Switch::Attributes::CurrentPosition::Set(switch_endpoint_id, newPosition);
    // LongRelease event takes previousPositionPosition as event data
    switch_cluster::event::send_long_release(switch_endpoint_id, previousPosition);
    lock::chip_stack_unlock();
    }
}

static void app_driver_button_long_pressed(void *arg, void *data)
{
    ESP_LOGI(TAG, "Long button pressed ");
    gpio_button *button = (gpio_button *)data;
    int switch_endpoint_id = (button != NULL) ? get_endpoint(button) : 1;
    // Press moves Position from 0 (idle) to 1 (press)
    uint8_t newPosition = 1;
    lock::chip_stack_lock(portMAX_DELAY);
    chip::app::Clusters::Switch::Attributes::CurrentPosition::Set(switch_endpoint_id, newPosition);
    // LongPress event takes newPosition as event data
    switch_cluster::event::send_long_press(switch_endpoint_id, newPosition);
    lock::chip_stack_unlock();
}

static int current_number_of_presses_counted = 1;

static void app_driver_button_multipress_ongoing(void *arg, void *data)
{
    ESP_LOGI(TAG, "Multipress Ongoing");
    gpio_button * button = (gpio_button *)data;
    int switch_endpoint_id = (button != NULL) ? get_endpoint(button) : 1;
    // Press moves Position from 0 (idle) to 1 (press)
    uint8_t newPosition = 1;
    current_number_of_presses_counted++;
    lock::chip_stack_lock(portMAX_DELAY);
    chip::app::Clusters::Switch::Attributes::CurrentPosition::Set(switch_endpoint_id, newPosition);
    // MultiPress Ongoing event takes newPosition and current_number_of_presses_counted as event data
    switch_cluster::event::send_multi_press_ongoing(switch_endpoint_id, newPosition, current_number_of_presses_counted);
    lock::chip_stack_unlock();
}

static void app_driver_button_multipress_complete(void *arg, void *data)
{
    ESP_LOGI(TAG, "Multipress Complete");
    gpio_button * button = (gpio_button *)data;
    int switch_endpoint_id = (button != NULL) ? get_endpoint(button) : 1;
    // Press moves Position from 0 (idle) to 1 (press)
    uint8_t previousPosition = 1;
    uint8_t newPosition = 0;
    int total_number_of_presses_counted = current_number_of_presses_counted;
    lock::chip_stack_lock(portMAX_DELAY);
    chip::app::Clusters::Switch::Attributes::CurrentPosition::Set(switch_endpoint_id, newPosition);
    // MultiPress Complete event takes previousPosition and total_number_of_presses_counted as event data
    switch_cluster::event::send_multi_press_complete(switch_endpoint_id, previousPosition, total_number_of_presses_counted);
    // Reset current_number_of_presses_counted to initial value
    current_number_of_presses_counted = 1;
    lock::chip_stack_unlock();
}
#endif

app_driver_handle_t app_driver_button_init(gpio_button * button)
{
    /* Initialize button */
    button_handle_t btns[BSP_BUTTON_NUM];
    ESP_ERROR_CHECK(bsp_iot_button_create(btns, NULL, BSP_BUTTON_NUM));

#if CONFIG_GENERIC_SWITCH_TYPE_LATCHING
    iot_button_register_cb(btns[0], BUTTON_DOUBLE_CLICK, app_driver_button_switch_latched, button);
#endif

#if CONFIG_GENERIC_SWITCH_TYPE_MOMENTARY
    iot_button_register_cb(btns[0], BUTTON_PRESS_DOWN, app_driver_button_initial_pressed, button);
    iot_button_register_cb(btns[0], BUTTON_PRESS_UP, app_driver_button_release, button);
    iot_button_register_cb(btns[0], BUTTON_LONG_PRESS_START, app_driver_button_long_pressed, button);
    iot_button_register_cb(btns[0], BUTTON_PRESS_REPEAT, app_driver_button_multipress_ongoing, button);
    iot_button_register_cb(btns[0], BUTTON_PRESS_REPEAT_DONE, app_driver_button_multipress_complete, button);
#endif
    return (app_driver_handle_t)btns[0];
}
