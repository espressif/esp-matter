/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_log.h>
#include <stdlib.h>
#include <string.h>
//#include "driver/ledc.h"
#include "driver/mcpwm.h"
#include "soc/mcpwm_periph.h"
#include <device.h>

#include <esp_matter.h>
#include "bsp/esp-bsp.h"

#include <app_priv.h>

using namespace chip::app::Clusters;
using namespace esp_matter;
using namespace esp_matter::cluster;

static const char *TAG = "app_driver";
extern uint16_t light_endpoint_id;

/*ELIA start*/
// Define GPIO pin connected to the signal input of the servo
#define SERVO_GPIO 13

// Define parameters for the servo motor
#define SERVO_MIN_PULSEWIDTH 500  // Minimum pulse width in microseconds
#define SERVO_MAX_PULSEWIDTH 2500 // Maximum pulse width in microseconds
#define SERVO_MAX_DEGREE 90       // Maximum angle in degrees servo can rotate

static void toggle_OnOff_attribute(){
    uint16_t endpoint_id_e = light_endpoint_id;
    uint32_t cluster_id_e = OnOff::Id;
    uint32_t attribute_id_e = OnOff::Attributes::OnOff::Id;

    node_t *node_e = node::get();
    endpoint_t *endpoint_e = endpoint::get(node_e, endpoint_id_e);
    cluster_t *cluster_e = cluster::get(endpoint_e, cluster_id_e);
    attribute_t *attribute_e = attribute::get(cluster_e, attribute_id_e);

    esp_matter_attr_val_t val_e = esp_matter_invalid(NULL);
    attribute::get_val(attribute_e, &val_e);
    val_e.val.b = !val_e.val.b;
    attribute::update(endpoint_id_e, cluster_id_e, attribute_id_e, &val_e);

}
/*ELIA end*/

/* Do any conversions/remapping for the actual value here */
/*ELIA MODS*/
static esp_err_t app_driver_light_set_power(led_indicator_handle_t handle, esp_matter_attr_val_t *val)
{
    if (val->val.b) {
        ESP_LOGI(TAG, "================================== Catapult power ON signal received: %d", val->val.b);
        // Initialize MCPWM
        mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, SERVO_GPIO);

        // Configure MCPWM
        mcpwm_config_t pwm_config;
        pwm_config.frequency = 50;  // Set frequency to 50Hz, suitable for most servos
        pwm_config.cmpr_a = 0;      // Initial duty cycle
        pwm_config.counter_mode = MCPWM_UP_COUNTER;
        pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
        mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);

        // Rotate servo from 0 to 90 degrees
        for (int i = 0; i <= SERVO_MAX_DEGREE; i++) {
            mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, 
                                    SERVO_MIN_PULSEWIDTH + ((SERVO_MAX_PULSEWIDTH - SERVO_MIN_PULSEWIDTH) * i / SERVO_MAX_DEGREE));
            vTaskDelay(20 / portTICK_PERIOD_MS); // Wait 20ms between each step
        }
        // Rotate servo from 90 to 0 degrees
        for (int i = SERVO_MAX_DEGREE; i >= 0; i--) {
            mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, 
                                    SERVO_MIN_PULSEWIDTH + ((SERVO_MAX_PULSEWIDTH - SERVO_MIN_PULSEWIDTH) * i / SERVO_MAX_DEGREE));
            vTaskDelay(20 / portTICK_PERIOD_MS); // Wait 20ms between each step
        }

        // Set PWM duty cycle to 0
        mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, 0);

        // Deinitialize MCPWM
        mcpwm_stop(MCPWM_UNIT_0, MCPWM_TIMER_0);

        ESP_LOGI(TAG, "ELIA ================================== Attempting to turn off the OnOff attribute");
        // ELIA: Following code TOGGLES the OnOff attribute. It is duplicate of the app_driver_button_toggle_cb with _e to avoid conflicts
        toggle_OnOff_attribute()
    }
    return ESP_OK;
}

static esp_err_t app_driver_light_set_brightness(led_indicator_handle_t handle, esp_matter_attr_val_t *val)
{
    int value = REMAP_TO_RANGE(val->val.u8, MATTER_BRIGHTNESS, STANDARD_BRIGHTNESS);
#if BSP_LED_NUM > 0
    return led_indicator_set_brightness(handle, value);
#else
    ESP_LOGI(TAG, "LED set brightness: %d", value);
    return ESP_OK;
#endif
}

static void app_driver_button_toggle_cb(void *arg, void *data)
{
    ESP_LOGI(TAG, "Toggle button pressed");
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

esp_err_t app_driver_attribute_update(app_driver_handle_t driver_handle, uint16_t endpoint_id, uint32_t cluster_id,
                                      uint32_t attribute_id, esp_matter_attr_val_t *val)
{
    esp_err_t err = ESP_OK;
    if (endpoint_id == light_endpoint_id) {
        led_indicator_handle_t handle = (led_indicator_handle_t)driver_handle;
        if (cluster_id == OnOff::Id) {
            if (attribute_id == OnOff::Attributes::OnOff::Id) {
                err = app_driver_light_set_power(handle, val);
            }
        } else if (cluster_id == LevelControl::Id) {
            if (attribute_id == LevelControl::Attributes::CurrentLevel::Id) {
                err = app_driver_light_set_brightness(handle, val);
            }
        } 
    }
    return err;
}

esp_err_t app_driver_light_set_defaults(uint16_t endpoint_id)
{
    esp_err_t err = ESP_OK;
    void *priv_data = endpoint::get_priv_data(endpoint_id);
    led_indicator_handle_t handle = (led_indicator_handle_t)priv_data;
    node_t *node = node::get();
    endpoint_t *endpoint = endpoint::get(node, endpoint_id);
    cluster_t *cluster = NULL;
    attribute_t *attribute = NULL;
    esp_matter_attr_val_t val = esp_matter_invalid(NULL);

    /* Setting brightness */
    cluster = cluster::get(endpoint, LevelControl::Id);
    attribute = attribute::get(cluster, LevelControl::Attributes::CurrentLevel::Id);
    attribute::get_val(attribute, &val);
    err |= app_driver_light_set_brightness(handle, &val);

    /* Setting power */
    cluster = cluster::get(endpoint, OnOff::Id);
    attribute = attribute::get(cluster, OnOff::Attributes::OnOff::Id);
    attribute::get_val(attribute, &val);
    err |= app_driver_light_set_power(handle, &val);

    return err;
}

app_driver_handle_t app_driver_light_init()
{
#if BSP_LED_NUM > 0
    /* Initialize led */
    led_indicator_handle_t leds[BSP_LED_NUM];
    ESP_ERROR_CHECK(bsp_led_indicator_create(leds, NULL, BSP_LED_NUM));
    led_indicator_set_hsv(leds[0], SET_HSV(DEFAULT_HUE, DEFAULT_SATURATION, DEFAULT_BRIGHTNESS));
    
    return (app_driver_handle_t)leds[0];
#else
    return NULL;
#endif
}

app_driver_handle_t app_driver_button_init()
{
    /* Initialize button */
    //button_handle_t btns[BSP_BUTTON_NUM];
    //ESP_ERROR_CHECK(bsp_iot_button_create(btns, NULL, BSP_BUTTON_NUM));
    //ESP_ERROR_CHECK(iot_button_register_cb(btns[0], BUTTON_PRESS_DOWN, app_driver_button_toggle_cb, NULL));

    button_config_t config = button_driver_get_config();
    button_handle_t handle = iot_button_create(&config);
    iot_button_register_cb(handle, BUTTON_PRESS_DOWN, app_driver_button_toggle_cb, NULL);
    
    return (app_driver_handle_t)handle;
}
