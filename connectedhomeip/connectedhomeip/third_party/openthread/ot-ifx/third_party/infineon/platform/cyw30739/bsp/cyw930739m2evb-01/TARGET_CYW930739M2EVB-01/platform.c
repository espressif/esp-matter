/*
 * Copyright 2016-2021, Cypress Semiconductor Corporation (an Infineon company) or
 * an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
 *
 * This software, including source code, documentation and related
 * materials ("Software") is owned by Cypress Semiconductor Corporation
 * or one of its affiliates ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products.  Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */

#include "wiced_bt_dev.h"
#include "wiced_bt_trace.h"
#include "wiced_platform.h"
#include "platform_led.h"
#include "wiced_hal_pwm.h"
#include "wiced_rtos.h"
#include "wiced_hal_aclk.h"

/*This will be the max PWM freq we can generate
 * we have set it to 256KHz now ,can be configured as supported */
#define PWM_BASE_CLK    ((uint32_t)256000)

#define PWM_OUT_NOINVERT            (0)                 /*0-OFF,1-Invert*/
#define PWM_OUT_INVERT              (1)                 /*0-OFF,1-Invert*/
#define PWM_DISABLE                 ((uint8_t)0)
#define PWM_ENABLE                  ((uint8_t)1)
#define PLATFORM_NOT_SUPPORT        ((uint8_t)2)

platform_led_config_t platform_led_config[PLATFORM_LED_MAX] =
{
    /* Red */
    [PLATFORM_LED_1] =
    {
        .gpio_pin = WICED_GPIO_PIN_LED_1,
        .channel = PWM0,
        .invert = WICED_TRUE,
    },
    /* Green */
    [PLATFORM_LED_2] =
    {
        .gpio_pin = WICED_GPIO_PIN_LED_2,
        .channel = PWM1,
        .invert = WICED_TRUE,
    },
};

static uint8_t pwm_status[PLATFORM_LED_MAX];
static wiced_pwm_config_t pwm_param[PLATFORM_LED_MAX];

static wiced_mutex_t *pwm_aclk_mutex;
static uint32_t pwm_aclk_count;
static wiced_bool_t pwm_aclk_state = WICED_FALSE;

wiced_result_t platform_led_clk_init(void)
{
    wiced_result_t res = WICED_ERROR;

    if (pwm_aclk_mutex == NULL)
        pwm_aclk_mutex = wiced_rtos_create_mutex();

    if (pwm_aclk_mutex != NULL)
    {
        res = wiced_rtos_init_mutex(pwm_aclk_mutex);
        if (WICED_SUCCESS != res)
        {
            WICED_BT_TRACE("PWM clk mutex init failed\n");
        }
    }

    return res;
}

/*enable the ACLK, keep use count
 * PWM BASE CLK is the clk freq used by the PWM */
void pwm_clk_enable(void)
{
    wiced_rtos_lock_mutex(pwm_aclk_mutex);

    if (pwm_aclk_count == 0)
    {
        wiced_hal_aclk_enable(PWM_BASE_CLK, WICED_ACLK1, WICED_ACLK_FREQ_24_MHZ);
        pwm_aclk_state = WICED_TRUE;
    }
    pwm_aclk_count++;
    wiced_rtos_unlock_mutex(pwm_aclk_mutex);
}

/*Disable the PWM base clock*/
void pwm_clk_disable(void)
{
    wiced_rtos_lock_mutex(pwm_aclk_mutex);

    if (pwm_aclk_state == WICED_FALSE)
    {
        wiced_rtos_unlock_mutex(pwm_aclk_mutex);
        return;
    }
    pwm_aclk_count--;
    if (pwm_aclk_count == 0)
    {
        wiced_hal_aclk_disable(WICED_ACLK1);
        pwm_aclk_state = WICED_FALSE;
    }
    wiced_rtos_unlock_mutex(pwm_aclk_mutex);
}

/* pwm        - platform led config (PWM config to use)
 * frequency  - frequency of PWM to be generated
 * duty_cycle - duty cycle of the PWM in % (1 to 100)
 *
 */
wiced_result_t
platform_led_init( const platform_led_config_t* pwm, uint32_t frequency, uint32_t duty_cycle )
{
    /*check platform support LED or not*/
    if (pwm->gpio_pin >= MAX_NUM_OF_GPIO)
    {
        pwm_status[pwm->channel] = PLATFORM_NOT_SUPPORT;
        return WICED_UNSUPPORTED;
    }

    /*configure the PWM pin*/
    if (pwm_status[pwm->channel] == PWM_DISABLE)
        wiced_hal_pwm_configure_pin(pwm->gpio_pin, pwm->channel);

    if (duty_cycle > 100)
        duty_cycle = 100;

    /*convert duty cycle and frequency to toggle and init vals*/
    wiced_hal_pwm_get_params(PWM_BASE_CLK, duty_cycle, frequency, &pwm_param[pwm->channel]);
    /*Init PWM base clock*/
    platform_led_clk_init();
    pwm_clk_enable();
    return WICED_SUCCESS;
}

wiced_result_t
platform_led_reinit( const platform_led_config_t* pwm, uint32_t frequency, uint32_t duty_cycle )
{
    /*check platform support LED or not*/
    if (pwm->gpio_pin >= MAX_NUM_OF_GPIO)
    {
        pwm_status[pwm->channel] = PLATFORM_NOT_SUPPORT;
        return WICED_UNSUPPORTED;
    }

    if (duty_cycle > 100)
        duty_cycle = 100;

    /*convert duty cycle and frequency to toggle and init vals*/
    wiced_hal_pwm_get_params(PWM_BASE_CLK, duty_cycle, frequency, &pwm_param[pwm->channel]);

    /*if started lets update new set of values*/
    if (pwm_status[pwm->channel] == PWM_ENABLE)
        wiced_hal_pwm_change_values(pwm->channel, pwm_param[pwm->channel].toggle_count, pwm_param[pwm->channel].init_count);

    return WICED_SUCCESS;
}



wiced_result_t
platform_led_start( const platform_led_config_t* pwm )
{
    if (pwm == NULL)
    {
        WICED_BT_TRACE( " %s bad argument\n", __func__);
        return WICED_ERROR;
    }

    /*check platform support LED or not*/
    if (pwm_status[pwm->channel] == PLATFORM_NOT_SUPPORT)
        return WICED_UNSUPPORTED;

    //WICED_BT_TRACE( " %s \n", __func__);
    if (pwm_status[pwm->channel] == PWM_DISABLE)
    {
        wiced_hal_pwm_start(pwm->channel, PMU_CLK, pwm_param[pwm->channel].toggle_count, pwm_param[pwm->channel].init_count, pwm->invert);
        pwm_status[pwm->channel] = PWM_ENABLE;
    }
    else
    {
        wiced_hal_pwm_change_values(pwm->channel, pwm_param[pwm->channel].toggle_count, pwm_param[pwm->channel].init_count);
    }
    return WICED_SUCCESS;
}

wiced_result_t
platform_led_stop( const platform_led_config_t* pwm )
{
    wiced_pwm_config_t pwm_config;
    if (pwm == NULL)
    {
        WICED_BT_TRACE( " %s bad argument\n", __func__);
        return WICED_ERROR;
    }

    /*check platform support LED or not*/
    if (pwm_status[pwm->channel] == PLATFORM_NOT_SUPPORT)
        return WICED_UNSUPPORTED;

    if (pwm_status[pwm->channel] == PWM_ENABLE)
    {
        /*convert duty cycle and frequency to toggle and init vals*/
        wiced_hal_pwm_get_params(PWM_BASE_CLK, 0, 0, &pwm_config);
        wiced_hal_pwm_change_values(pwm->channel, pwm_config.init_count, pwm_config.toggle_count);
        //pwm_clk_disable();
        pwm_status[pwm->channel] = PWM_DISABLE;
    }
    return WICED_SUCCESS;
}

wiced_result_t
platform_led_deinit( const platform_led_config_t* pwm )
{
    if (pwm == NULL)
    {
        WICED_BT_TRACE( " %s bad argument\n", __func__);
        return WICED_ERROR;
    }

    /*check platform support LED or not*/
    if (pwm_status[pwm->channel] == PLATFORM_NOT_SUPPORT)
        return WICED_UNSUPPORTED;

    wiced_hal_pwm_disable(pwm->channel);
    pwm_clk_disable();
    pwm_status[pwm->channel] = PWM_DISABLE;

    return WICED_SUCCESS;
}
