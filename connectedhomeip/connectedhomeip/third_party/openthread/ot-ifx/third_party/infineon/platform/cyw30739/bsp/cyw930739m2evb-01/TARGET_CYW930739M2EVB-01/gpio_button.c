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

/** @file
 *
 * GPIO-button implementation
 *
 */

#include "wiced.h"
#include "wiced_hal_gpio.h"
#include "wiced_rtos.h"
#include "gpio_button.h"
#include "wiced_platform.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Static Function Declarations
 ******************************************************/

static void gpio_button_interrupt_handler( void* args, uint8_t pin );

/******************************************************
 *               Variable Definitions
 ******************************************************/

static gpio_button_state_change_callback_t button_state_change_callback;

/******************************************************
 *               Function Definitions
 ******************************************************/

wiced_result_t gpio_button_init( const gpio_button_t* button )
{
	if(button == NULL)
		return WICED_BADARG;
#ifndef USE_PLATFORM_BUTTON_CONFIG
	wiced_hal_gpio_configure_pin((uint32_t)button->gpio, (uint32_t)GPIO_INPUT_ENABLE,
			(uint32_t)(( button->polarity == WICED_ACTIVE_HIGH )? INPUT_PULL_UP: INPUT_PULL_DOWN));
#endif

    return WICED_SUCCESS;
}

wiced_result_t gpio_button_deinit( const gpio_button_t* button )
{
	if(button == NULL)
		return WICED_BADARG;
#ifndef USE_PLATFORM_BUTTON_CONFIG
	wiced_hal_gpio_configure_pin((uint32_t)button->gpio, (uint32_t)(GPIO_INPUT_DISABLE|GPIO_INTERRUPT_DISABLE),
			(uint32_t)(INPUT_PULL_DOWN));
#endif

    return WICED_SUCCESS;

}

wiced_result_t gpio_button_register_state_change_callback( gpio_button_state_change_callback_t callback )
{
    if ( !callback )
    {
        return WICED_BADARG;
    }

    button_state_change_callback = callback;

    return WICED_SUCCESS;
}

wiced_result_t gpio_button_enable( const gpio_button_t* button )
{
#ifndef USE_PLATFORM_BUTTON_CONFIG
    wiced_gpio_irq_trigger_t trigger;

	if(button == NULL)
		return WICED_BADARG;

    if ( button->trigger == 0 )
    {
        trigger = ( ( button->polarity == WICED_ACTIVE_LOW ) ? IRQ_TRIGGER_RISING_EDGE : IRQ_TRIGGER_FALLING_EDGE );
    }
    else
    {
        trigger = button->trigger;
    }

    wiced_hal_gpio_configure_pin( (uint32_t)button->gpio, WICED_GPIO_BUTTON_SETTINGS((uint32_t)trigger),
		(uint32_t)(( button->polarity == WICED_ACTIVE_HIGH )? INPUT_PULL_UP: INPUT_PULL_DOWN));
#endif

    wiced_hal_gpio_register_pin_for_interrupt((uint16_t) button->gpio,
		gpio_button_interrupt_handler, (void*)button);

    return WICED_SUCCESS;
}

wiced_result_t gpio_button_disable( const gpio_button_t* button )
{
	if(button == NULL)
		return WICED_BADARG;

#ifndef USE_PLATFORM_BUTTON_CONFIG
	wiced_hal_gpio_configure_pin((uint32_t)button->gpio, (uint32_t)(GPIO_INPUT_DISABLE|GPIO_INTERRUPT_DISABLE),
			(uint32_t)(INPUT_PULL_DOWN));
#endif

    return WICED_SUCCESS;
}

wiced_bool_t gpio_button_get_value( const gpio_button_t* button )
{
    wiced_bool_t value;

	if(button == NULL)
		return WICED_BADARG;

    value = (wiced_bool_t)wiced_hal_gpio_get_pin_input_status((uint32_t)button->gpio );
    return value;
}


static void gpio_button_interrupt_handler( void* args, uint8_t pin )
{
    const gpio_button_t* button = (const gpio_button_t*)args;
    wiced_bool_t   gpio_state;
    wiced_bool_t   is_pressed;
    (void)(pin);	//unused

    //WICED_BT_TRACE("%s - %d\n", __func__,pin);
    if( !button_state_change_callback || !button )
    {
        return;
    }

    gpio_state = (wiced_bool_t)wiced_hal_gpio_get_pin_input_status((uint32_t)button->gpio );

    is_pressed = ( button->polarity == WICED_ACTIVE_HIGH ) ? ( (gpio_state  == WICED_FALSE ) ? WICED_TRUE : WICED_FALSE ) : ( (gpio_state == WICED_FALSE ) ? WICED_FALSE : WICED_TRUE );

    button_state_change_callback( (void*)button, is_pressed );
}
