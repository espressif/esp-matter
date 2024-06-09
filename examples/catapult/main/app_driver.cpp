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
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <app_reset.h>

#include <esp_matter.h>
#include "bsp/esp-bsp.h"

#include <app_priv.h>

using namespace chip::app::Clusters;
using namespace esp_matter;
using namespace esp_matter::cluster;

static const char *TAG = "app_driver";
extern uint16_t light_endpoint_id;

// Define the number of servos
#define NUM_SERVOS 4 

// Define GPIO pin connected to the signal input of the servo
//#define SERVO1_GPIO 13
//#define SERVO2_GPIO 25
//#define SERVO3_GPIO 33
//#define SERVO4_GPIO 32

#define SERVO1_GPIO 13
#define SERVO2_GPIO 12
#define SERVO3_GPIO 14
#define SERVO4_GPIO 25

#define release_open        20  //OK
#define release_locked      130 //OK
#define tensioner_loose     30  //OK
#define tensioner_charged   110 //OK
#define lid_closed          0   //OK
#define lid_open            165 //OK
#define lid_reload          115
#define reload_closed       125 //OK
#define reload_open         30  //OK

// Define servo names
const char* servo_names[NUM_SERVOS] = {"Release", "Tensioner", "Lid", "Reload"};

// Define GPIO pins for each servo
const int servo_pins[NUM_SERVOS] = {SERVO1_GPIO, SERVO2_GPIO, SERVO3_GPIO, SERVO4_GPIO};

// Define two separate units
const mcpwm_unit_t servo_pwm_unit[NUM_SERVOS] = {MCPWM_UNIT_0, MCPWM_UNIT_1, MCPWM_UNIT_0, MCPWM_UNIT_1};

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
int angle_to_duty(int angle){
    int result = static_cast<int>((static_cast<double>(angle) / SERVO_MAX_DEGREE) * (SERVO_MAX_PULSEWIDTH - SERVO_MIN_PULSEWIDTH) + SERVO_MIN_PULSEWIDTH);
    return result;
}

static void drive_servo_smoothly(int servo_id, int start, int end, int total_delay) {
    // ALERT: delay_time_ms cannot be less than 10!! A certain value of num_steps implies a certain minimum total delay!!
    const int num_steps = 100;
    const int delay_time_ms = total_delay / num_steps;
    const int start_duty = angle_to_duty(start);
    const int end_duty = angle_to_duty(end);
    
    // Iterate through a number of steps to smoothly change the duty cycle from start to end
    // The angle varies from 0 to pi/2, creating a sinusoidal increment in duty cycle
    for (int i = 0; i < num_steps; i++) {
        double angle = M_PI/2 * i / (num_steps - 1); // Angle ranges from 0 to pi/2
        double duty = sin(angle) * sin(angle) * (end_duty - start_duty) + start_duty;
        mcpwm_set_duty_in_us(servo_pwm_unit[servo_id], servo_pwm_timer[servo_id], servo_pwm_generator[servo_id], duty);
        // Wait for a short duration
        vTaskDelay(delay_time_ms / portTICK_PERIOD_MS);
    }
}

static void drive_servo_linearly(int servo_id, int start, int end, int total_delay) {
    // ALERT: delay_time_ms cannot be less than 10!! A certain value of num_steps implies a certain minimum total delay!!
    const int num_steps = 100;
    const int delay_time_ms = total_delay / num_steps;
    const int start_duty = angle_to_duty(start);
    const int end_duty = angle_to_duty(end);
    
    // Iterate through a number of steps to linearly change the duty cycle from start to end
    for (int i = 0; i < num_steps; i++) {
        //double duty = i / num_steps * (end_duty - start_duty) + start_duty;
        double duty = static_cast<double>(i) / num_steps * (end_duty - start_duty) + start_duty;
        mcpwm_set_duty_in_us(servo_pwm_unit[servo_id], servo_pwm_timer[servo_id], servo_pwm_generator[servo_id], duty);
        // Wait for a short duration
        vTaskDelay(delay_time_ms / portTICK_PERIOD_MS);
    }
}

static void launch(void *pvParameters) {
    mcpwm_config_t pwm_config;
    pwm_config.frequency = 50;  // Set frequency to 50Hz, suitable for most servos
    pwm_config.cmpr_a = 0;      // Initial duty cycle
    pwm_config.counter_mode = MCPWM_UP_COUNTER;
    pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
    //mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);
    //mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_1, &pwm_config);

    // Initialize and configure MCPWM for each servo
    for (int servo_id = 0; servo_id < NUM_SERVOS; servo_id++) {
        ESP_LOGI(TAG, "Initializing %s with GPIO pin %d", servo_names[servo_id], servo_pins[servo_id]);
        mcpwm_gpio_init(servo_pwm_unit[servo_id], servo_pwm_signals[servo_id], servo_pins[servo_id]);
        // Use separate timers for each servo
        mcpwm_init(servo_pwm_unit[servo_id], servo_pwm_timer[servo_id], &pwm_config);
        mcpwm_set_duty_in_us(servo_pwm_unit[servo_id], servo_pwm_timer[servo_id], servo_pwm_generator[servo_id], 0);
    }

    //vTaskDelay(5000 / portTICK_PERIOD_MS);
    int duty = angle_to_duty(tensioner_loose);
    mcpwm_set_duty_in_us(servo_pwm_unit[1], servo_pwm_timer[1], servo_pwm_generator[1], duty);
    duty = angle_to_duty(reload_closed);
    mcpwm_set_duty_in_us(servo_pwm_unit[3], servo_pwm_timer[3], servo_pwm_generator[3], duty);
    vTaskDelay(300 / portTICK_PERIOD_MS);
    duty = angle_to_duty(release_locked);
    mcpwm_set_duty_in_us(servo_pwm_unit[0], servo_pwm_timer[0], servo_pwm_generator[0], duty);
    duty = angle_to_duty(lid_closed);
    mcpwm_set_duty_in_us(servo_pwm_unit[2], servo_pwm_timer[2], servo_pwm_generator[2], duty);
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    // Open lid
    drive_servo_smoothly(2, lid_closed, lid_open, 1500);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    // Charge tensioner
    drive_servo_linearly(1, tensioner_loose, tensioner_charged, 1000);
    vTaskDelay(200 / portTICK_PERIOD_MS);

    // Release lock
    drive_servo_linearly(0, release_locked, release_open, 1000);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    // Fold tensioner
    drive_servo_smoothly(1, tensioner_charged, tensioner_loose, 1000);
    vTaskDelay(300 / portTICK_PERIOD_MS);

    // Lock the release and toggle reload
    duty = angle_to_duty(release_locked);
    mcpwm_set_duty_in_us(servo_pwm_unit[0], servo_pwm_timer[0], servo_pwm_generator[0], duty);
    duty = angle_to_duty(reload_open);
    mcpwm_set_duty_in_us(servo_pwm_unit[3], servo_pwm_timer[3], servo_pwm_generator[3], duty);
    vTaskDelay(300 / portTICK_PERIOD_MS);
    duty = angle_to_duty(reload_closed);
    mcpwm_set_duty_in_us(servo_pwm_unit[3], servo_pwm_timer[3], servo_pwm_generator[3], duty);
    vTaskDelay(300 / portTICK_PERIOD_MS);

    // Tilt lid to reload
    duty = angle_to_duty(lid_reload);
    mcpwm_set_duty_in_us(servo_pwm_unit[2], servo_pwm_timer[2], servo_pwm_generator[2], duty);
    vTaskDelay(500 / portTICK_PERIOD_MS);

    // Close lid
    drive_servo_smoothly(2, lid_reload, lid_closed, 2000);

    // Stop PWM generation for each servo
    for (int servo_id = 0; servo_id < NUM_SERVOS; servo_id++) {
        mcpwm_set_duty_in_us(servo_pwm_unit[servo_id], servo_pwm_timer[servo_id], servo_pwm_generator[servo_id], 0);
        mcpwm_stop(servo_pwm_unit[servo_id], servo_pwm_timer[servo_id]);
    }
    // Delete the task from within the function
    vTaskDelete(NULL);
}

static esp_err_t app_driver_light_set_power(led_indicator_handle_t handle, esp_matter_attr_val_t *val)
{
    if (val->val.b) {
        ESP_LOGI(TAG, "================================== Catapult power ON signal received: %d", val->val.b);
        xTaskCreate(&launch,         // Function to run
                "launch_task",   // Task name
                4096,                 // Stack size (in words, not bytes)
                NULL,                 // No parameter to pass to the task
                5,                    // Priority (higher number means higher priority)
                NULL);                // Task handle (can be NULL if not needed)

        //launch();

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
    button_config_t config = {
        .type = BUTTON_TYPE_GPIO,
        .gpio_button_config = {
            .gpio_num = 17,
            .active_level = 0,
        }
    };
    button_handle_t handle = iot_button_create(&config);
    iot_button_register_cb(handle, BUTTON_PRESS_DOWN, app_driver_button_toggle_cb, NULL);
    gpio_set_pull_mode((gpio_num_t)17, GPIO_PULLUP_ONLY);
    
    return (app_driver_handle_t)handle;
}

app_driver_handle_t app_driver_reset_button_init()
{
    /* Initialize button */
    button_config_t config = {
        .type = BUTTON_TYPE_GPIO,
        .gpio_button_config = {
            .gpio_num = 16,
            .active_level = 0,
        }
    };
    button_handle_t handle = iot_button_create(&config);
    app_reset_button_register(handle);

    // Enable internal pull-up resistor for GPIO 17
    gpio_set_pull_mode((gpio_num_t)16, GPIO_PULLUP_ONLY);
    
    return (app_driver_handle_t)handle;
}