/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include "driver/gpio.h"
#include "esp_timer.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    STEP_CW = 0,
    STEP_CCW,
} stepper_dir_t;

typedef struct {
    uint8_t is_start;
    gpio_num_t pin[4];
    stepper_dir_t dir;
    esp_timer_handle_t stepper_timer;
} stepper_motor_t;

extern stepper_motor_t stepper_motor;

/**
 * @brief stepper init
 *
 * @param pin_1
 * @param pin_2
 * @param pin_3
 * @param pin_4
 * @return esp_err_t
 */
esp_err_t hal_stepper_motor_init(gpio_num_t pin_1, gpio_num_t pin_2, gpio_num_t pin_3, gpio_num_t pin_4);

/**
 * @brief set stepper motor status
 *
 * @return esp_err_t
 */
esp_err_t hal_stepper_motor_set_rock(int status);

#ifdef __cplusplus
}
#endif
