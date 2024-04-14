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

// Define GPIO pin connected to the signal input of the servo
#define SERVO1_GPIO 13
#define SERVO2_GPIO 25
#define SERVO3_GPIO 33
#define SERVO4_GPIO 32

#define NUM_SERVOS 4 // Define the number of servos

// Define servo names
const char* servo_names[NUM_SERVOS] = {"Tensioner", "Release", "Lid", "Reload"};

// Define GPIO pins for each servo
const int servo_pins[NUM_SERVOS] = {SERVO1_GPIO, SERVO2_GPIO, SERVO3_GPIO, SERVO4_GPIO};

// Define two separate timers
const mcpwm_timer_t servo_pwm_timer[NUM_SERVOS] = {MCPWM_TIMER_0, MCPWM_TIMER_0, MCPWM_TIMER_1, MCPWM_TIMER_1};

// Define MCPWM output signals for each servo
const mcpwm_io_signals_t servo_pwm_signals[NUM_SERVOS] = {MCPWM0A, MCPWM0B, MCPWM1A, MCPWM1B};

// Define MCPWM generators for each servo
const mcpwm_generator_t servo_pwm_generator[NUM_SERVOS] = {MCPWM_OPR_A, MCPWM_OPR_B, MCPWM_OPR_A, MCPWM_OPR_B};


// Define parameters for the servo motor
#define SERVO_MIN_PULSEWIDTH 500  // Minimum pulse width in microseconds
#define SERVO_MAX_PULSEWIDTH 2500 // Maximum pulse width in microseconds
#define SERVO_MAX_DEGREE 180       // Maximum angle in degrees servo can rotate

// ELIA: Following code TOGGLES the OnOff attribute.
static void toggle_OnOff_attribute() {
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

/* Do any conversions/remapping for the actual value here */
/*ELIA MODS*/
static esp_err_t app_driver_light_set_power(led_indicator_handle_t handle, esp_matter_attr_val_t *val)
{
    if (val->val.b) {
        ESP_LOGI(TAG, "================================== Catapult power ON signal received: %d", val->val.b);
        
        mcpwm_config_t pwm_config;
        pwm_config.frequency = 50;  // Set frequency to 50Hz, suitable for most servos
        pwm_config.cmpr_a = 0;      // Initial duty cycle
        pwm_config.counter_mode = MCPWM_UP_COUNTER;
        pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
        mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);
        mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_1, &pwm_config);

        // Initialize and configure MCPWM for each servo
        for (int servo_id = 0; servo_id < NUM_SERVOS; servo_id++) {
            ESP_LOGI(TAG, "Initializing %s with GPIO pin %d", servo_names[servo_id], servo_pins[servo_id]);
            mcpwm_gpio_init(MCPWM_UNIT_0, servo_pwm_signals[servo_id], servo_pins[servo_id]);
            
        }
        // Rotate each servo from 0 to 90 degrees
        for (int i = 0; i <= SERVO_MAX_DEGREE; i++) {
            for (int servo_id = 0; servo_id < NUM_SERVOS; servo_id++) {
                mcpwm_set_duty_in_us(MCPWM_UNIT_0, servo_pwm_timer[servo_id], servo_pwm_generator[servo_id], 
                                        SERVO_MIN_PULSEWIDTH + ((SERVO_MAX_PULSEWIDTH - SERVO_MIN_PULSEWIDTH) * i / SERVO_MAX_DEGREE));
            }
            vTaskDelay(20 / portTICK_PERIOD_MS); // Wait 20ms between each step
        }

        // Rotate each servo from 90 to 0 degrees
        for (int i = SERVO_MAX_DEGREE; i >= 0; i--) {
            for (int servo_id = 0; servo_id < NUM_SERVOS; servo_id++) {
                mcpwm_set_duty_in_us(MCPWM_UNIT_0, servo_pwm_timer[servo_id], servo_pwm_generator[servo_id], 
                                        SERVO_MIN_PULSEWIDTH + ((SERVO_MAX_PULSEWIDTH - SERVO_MIN_PULSEWIDTH) * i / SERVO_MAX_DEGREE));
            }
            vTaskDelay(20 / portTICK_PERIOD_MS); // Wait 20ms between each step
        }

        // Stop PWM generation for each servo
        for (int servo_id = 0; servo_id < NUM_SERVOS; servo_id++) {
            mcpwm_set_duty_in_us(MCPWM_UNIT_0, servo_pwm_timer[servo_id], servo_pwm_generator[servo_id], 0);
            mcpwm_stop(MCPWM_UNIT_0, servo_pwm_timer[servo_id]);
        }

        /*
        // Initialize MCPWM
        mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, SERVO_GPIO);
        // Configure MCPWM
        mcpwm_config_t pwm_config;
        pwm_config.frequency = 50;  // Set frequency to 50Hz, suitable for most servos
        pwm_config.cmpr_a = 0;      // Initial duty cycle
        pwm_config.counter_mode = MCPWM_UP_COUNTER;
        pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
        mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);
        

        // Rotate servo from 0 to 180 degrees
        for (int i = 0; i <= SERVO_MAX_DEGREE; i++) {
            mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, 
                                    SERVO_MIN_PULSEWIDTH + ((SERVO_MAX_PULSEWIDTH - SERVO_MIN_PULSEWIDTH) * i / SERVO_MAX_DEGREE));
            vTaskDelay(20 / portTICK_PERIOD_MS); // Wait 20ms between each step
        }
        // Rotate servo from 180 to 0 degrees
        for (int i = SERVO_MAX_DEGREE; i >= 0; i--) {
            mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, 
                                    SERVO_MIN_PULSEWIDTH + ((SERVO_MAX_PULSEWIDTH - SERVO_MIN_PULSEWIDTH) * i / SERVO_MAX_DEGREE));
            vTaskDelay(20 / portTICK_PERIOD_MS); // Wait 20ms between each step
        }
        
        // Set PWM duty cycle to 0
        mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, 0);

        // Deinitialize MCPWM
        mcpwm_stop(MCPWM_UNIT_0, MCPWM_TIMER_0);
        */


        ESP_LOGI(TAG, "ELIA ================================== Attempting to turn OFF the OnOff attribute");
        toggle_OnOff_attribute();
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
    toggle_OnOff_attribute();
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
