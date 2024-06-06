/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdlib.h>
#include <string.h>

#include "math.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_timer.h"
#include "fan_hal_bldc.h"
#include "iot_button.h"

#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S3
#define PI 3.14159265f

motor_parameter_t motor_parameter;

void bldc_variable_init()
{
    memset(&motor_parameter, 0x00, sizeof(motor_parameter));
    motor_parameter.target_speed = 0; /*!< motor target speed */
    motor_parameter.min_speed = 50;    /*!< motor min speed */
    motor_parameter.max_speed = DEFAULT_SPEED_MAX * DEFAULT_SPEED2MOTOR;
}

#define LIMIT(VALUE, MIN, MAX) \
    ((VALUE) < (MIN) ? (MIN) : ((VALUE) > (MAX) ? (MAX) : (VALUE)))

static bldc_control_handle_t bldc_control_handle = NULL;

static void hal_bldc_button_ctrl(void *arg, void *data)
{
    motor_parameter.is_start = !motor_parameter.is_start;
}

static int hal_bldc_natural_speed(int min_speed, int max_speed, float noise_level, int t)
{
    double k = (1 + sin(t * 2 * PI / 16)) / 2;
    if (k >= noise_level) {
        k = noise_level;
    } else if (k <= 1 - noise_level) {
        k = 1 - noise_level;
    }
    k = (k - (1 - noise_level)) / (noise_level - (1 - noise_level));
    int speed = k * (max_speed - min_speed) + min_speed;
    return speed;
}

static void hal_bldc_timer_cb(void *args)
{
    static int t = 0;
    static int min_speed = 300;
    static int max_speed = DEFAULT_SPEED_MAX * DEFAULT_SPEED2MOTOR;
    if (++t > 15) {
        t = 0;
    }
    if (motor_parameter.is_start && motor_parameter.is_natural) {
        motor_parameter.target_speed = hal_bldc_natural_speed(min_speed, max_speed - 200, 0.8, t);
        bldc_control_set_speed_rpm(bldc_control_handle, motor_parameter.target_speed);
    }
}

esp_err_t hal_bldc_button_ctrl_init(gpio_num_t pin)
{
    button_config_t cfg = {
        .type = BUTTON_TYPE_GPIO,
        .gpio_button_config = {
            .gpio_num = pin,
            .active_level = 0,
        },
    };
    button_handle_t btn = iot_button_create(&cfg);

    if (iot_button_register_cb(btn, BUTTON_PRESS_DOWN, hal_bldc_button_ctrl, &btn) != ESP_OK) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t hal_bldc_init(dir_enum_t direction)
{
    switch_config_t_t upper_switch_config = {
        .control_type = CONTROL_TYPE_MCPWM,
        .bldc_mcpwm = {
            .group_id = 0,
            .gpio_num = {CONFIG_UPPER_SWITCH_PHASE_U_GPIO, CONFIG_UPPER_SWITCH_PHASE_V_GPIO, CONFIG_UPPER_SWITCH_PHASE_W_GPIO},
        },
    };

    switch_config_t_t lower_switch_config = {
        .control_type = CONTROL_TYPE_GPIO,
        .bldc_gpio[0] = {
            .gpio_num = CONFIG_LOWER_SWITCH_PHASE_U_GPIO,
            .gpio_mode = GPIO_MODE_OUTPUT,
        },
        .bldc_gpio[1] = {
            .gpio_num = CONFIG_LOWER_SWITCH_PHASE_V_GPIO,
            .gpio_mode = GPIO_MODE_OUTPUT,
        },
        .bldc_gpio[2] = {
            .gpio_num = CONFIG_LOWER_SWITCH_PHASE_W_GPIO,
            .gpio_mode = GPIO_MODE_OUTPUT,
        },
    };

    bldc_zero_cross_comparer_config_t zero_cross_comparer_config = {
        .comparer_gpio[0] = {
            .gpio_num = CONFIG_ZERO_CROSS_COMPARER_PHASE_U_GPIO,
            .gpio_mode = GPIO_MODE_INPUT,
            .active_level = 0,
        },
        .comparer_gpio[1] = {
            .gpio_num = CONFIG_ZERO_CROSS_COMPARER_PHASE_V_GPIO,
            .gpio_mode = GPIO_MODE_INPUT,
            .active_level = 0,
        },
        .comparer_gpio[2] = {
            .gpio_num = CONFIG_ZERO_CROSS_COMPARER_PHASE_W_GPIO,
            .gpio_mode = GPIO_MODE_INPUT,
            .active_level = 0,
        },
    };

    bldc_control_config_t config = {
        .speed_mode = SPEED_CLOSED_LOOP,
        .control_mode = BLDC_SIX_STEP,
        .alignment_mode = ALIGNMENT_COMPARER,
        .six_step_config = {
            .lower_switch_active_level = 0,
            .upper_switch_config = upper_switch_config,
            .lower_switch_config = lower_switch_config,
            .mos_en_config.has_enable = false,
        },
        .zero_cross_comparer_config = zero_cross_comparer_config,
    };

    if (bldc_control_init(&bldc_control_handle, &config) != ESP_OK) {
        return ESP_FAIL;
    }

    if (bldc_control_set_dir(bldc_control_handle, direction) != ESP_OK) {
        return ESP_FAIL;
    }

    if (hal_bldc_button_ctrl_init(CONFIG_POWER_BUTTON_GPIO) != ESP_OK) {
        return ESP_FAIL;
    }

    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &hal_bldc_timer_cb,
        .name = "periodic"
    };

    esp_timer_handle_t periodic_bldc_timer;
    if (esp_timer_create(&periodic_timer_args, &periodic_bldc_timer) != ESP_OK) {
        return ESP_FAIL;
    }

    if (esp_timer_start_periodic(periodic_bldc_timer, 1000 * 1000) != ESP_OK) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t hal_bldc_start_stop(uint8_t status)
{
    if (status == 0) {
        return bldc_control_stop(bldc_control_handle);
    } else {
        motor_parameter.is_start = 1;
        return bldc_control_start(bldc_control_handle, motor_parameter.target_speed);
    }
}

esp_err_t hal_bldc_set_fanmode(uint8_t status)
{
    if (status == 0) {
        motor_parameter.is_start = 0;
        motor_parameter.target_speed = 0;
        hal_bldc_start_stop(status);
    } else if (status == 1) {
        motor_parameter.target_speed = LOW_MODE_PERCENT * DEFAULT_SPEED2MOTOR;
        motor_parameter.target_speed = LIMIT(motor_parameter.target_speed, motor_parameter.min_speed, motor_parameter.max_speed);
        if (motor_parameter.is_start == 1) {
            bldc_control_set_speed_rpm(bldc_control_handle, motor_parameter.target_speed);
        } else {
            hal_bldc_start_stop(status);
        }
    } else if (status == 2) {
        motor_parameter.target_speed = MED_MODE_PERCENT * DEFAULT_SPEED2MOTOR;
        motor_parameter.target_speed = LIMIT(motor_parameter.target_speed, motor_parameter.min_speed, motor_parameter.max_speed);
        if (motor_parameter.is_start == 1) {
            bldc_control_set_speed_rpm(bldc_control_handle, motor_parameter.target_speed);
        } else {
            hal_bldc_start_stop(status);
        }
    } else if (status == 3) {
        motor_parameter.target_speed = HIGH_MODE_PERCENT * DEFAULT_SPEED2MOTOR;
        motor_parameter.target_speed = LIMIT(motor_parameter.target_speed, motor_parameter.min_speed, motor_parameter.max_speed);
        if (motor_parameter.is_start == 1) {
            bldc_control_set_speed_rpm(bldc_control_handle, motor_parameter.target_speed);
        } else {
            hal_bldc_start_stop(status);
        }
    }
    return ESP_OK;
}

esp_err_t hal_bldc_set_percent_speed(uint16_t percent)
{
    percent = percent > 100 ? 0:percent;
    motor_parameter.target_speed = (uint16_t)(percent * DEFAULT_SPEED_MAX * 0.01 * DEFAULT_SPEED2MOTOR);
    motor_parameter.target_speed = LIMIT(motor_parameter.target_speed, motor_parameter.min_speed, motor_parameter.max_speed);
    if (percent != 0) {
        if (motor_parameter.is_start == 0) {
            motor_parameter.is_start = 1;
            bldc_control_start(bldc_control_handle, motor_parameter.target_speed);
        } else {
            bldc_control_set_speed_rpm(bldc_control_handle, motor_parameter.target_speed);
        }
    }
    return ESP_OK;
}

esp_err_t hal_bldc_set_speed(uint16_t speed)
{
    motor_parameter.target_speed = speed * DEFAULT_SPEED2MOTOR;
    motor_parameter.target_speed = LIMIT(motor_parameter.target_speed, motor_parameter.min_speed, motor_parameter.max_speed);
    return bldc_control_set_speed_rpm(bldc_control_handle, motor_parameter.target_speed);
}

esp_err_t hal_bldc_set_wind(uint8_t status)
{
    if (status == 2) {
        motor_parameter.is_natural = 1;
        return ESP_OK;
    } else {
        motor_parameter.is_natural = 0;
        return ESP_OK;
    }
}

esp_err_t hal_bldc_set_airflow_direction(bool status)
{
    if (status == 0) {
        bldc_control_set_dir(bldc_control_handle, CW);
    } else {
        bldc_control_set_dir(bldc_control_handle, CCW);
    }
    return ESP_OK;
}

esp_err_t hal_bldc_status()
{
    return motor_parameter.is_start;
}

esp_err_t hal_bldc_speed()
{
    return motor_parameter.target_speed;
}
#endif
