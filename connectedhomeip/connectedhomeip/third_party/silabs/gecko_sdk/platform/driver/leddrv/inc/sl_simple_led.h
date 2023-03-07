/***************************************************************************//**
 * @file
 * @brief Simple LED Driver
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef SL_SIMPLE_LED_H
#define SL_SIMPLE_LED_H

#include "sl_led.h"
#include "em_gpio.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup led
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup simple_led Simple LED Driver
 * @brief Simple LED Driver can be used to execute basic LED functionalities
 *        such as on, off, toggle, or retrive the on/off status on Silicon Labs
 *        devices. Subsequent sections provide more insight into this module.
 * @{
 ******************************************************************************/

/*******************************************************************************
 ******************************   DEFINES   ************************************
 ******************************************************************************/

#define SL_SIMPLE_LED_POLARITY_ACTIVE_LOW  0U ///< LED Active polarity Low
#define SL_SIMPLE_LED_POLARITY_ACTIVE_HIGH 1U ///< LED Active polarity High

/*******************************************************************************
 *****************************   DATA TYPES   **********************************
 ******************************************************************************/

typedef uint8_t sl_led_polarity_t;    ///< LED GPIO polarities (active high/low)

/// A Simple LED instance
typedef struct {
  GPIO_Port_TypeDef port;             ///< LED port
  uint8_t           pin;              ///< LED pin
  sl_led_polarity_t polarity;         ///< Initial state of LED
} sl_simple_led_context_t;

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

/***************************************************************************//**
 * Initialize the simple LED driver.
 *
 * @param[in] led_handle        Pointer to simple-led specific data:
 *                                - sl_simple_led_context_t
 *
 * @return    Status Code:
 *              - SL_STATUS_OK
 ******************************************************************************/
sl_status_t sl_simple_led_init(void *led_handle);

/***************************************************************************//**
 * Turn on a simple LED.
 *
 * @param[in] led_handle        Pointer to simple-led specific data:
 *                                - sl_simple_led_context_t
 *
 ******************************************************************************/
void sl_simple_led_turn_on(void *led_handle);

/***************************************************************************//**
 * Turn off a simple LED.
 *
 * @param[in] led_handle        Pointer to simple-led specific data:
 *                                - sl_simple_led_context_t
 *
 ******************************************************************************/
void sl_simple_led_turn_off(void *led_handle);

/***************************************************************************//**
 * Toggle a simple LED.
 *
 * @param[in] led_handle        Pointer to simple-led specific data:
 *                                - sl_simple_led_context_t
 *
 ******************************************************************************/
void sl_simple_led_toggle(void *led_handle);

/***************************************************************************//**
 * Get the current state of the simple LED.
 *
 * @param[in] led_handle       Pointer to simple-led specific data:
 *                               - sl_simple_led_context_t
 *
 * @return    sl_led_state_t   Current state of simple LED. 1 for on, 0 for off
 ******************************************************************************/
sl_led_state_t sl_simple_led_get_state(void *led_handle);

/** @} (end group simple_led) */
/** @} (end group led) */

// ******** THE REST OF THE FILE IS DOCUMENTATION ONLY !***********************
/// @addtogroup simple_led Simple LED Driver
/// @{
///
///   @details
///
///   @li @ref simple_led_intro
///   @li @ref simple_led_config
///   @li @ref simple_led_usage
///
///   @n @section simple_led_intro Introduction
///
///   The Simple LED driver is a module of the LED driver that provides the functionality
///   to control simple on/off LEDs.
///
///   @n @section simple_led_config Simple LED Configuration
///
///   Simple LEDs use the @ref sl_led_t struct and their @ref sl_simple_led_context_t
///   struct. These are automatically generated into the following files, as well as
///   instance specific headers with macro definitions in them. The samples below
///   are for a single instance called "inst0".
///
///   @code{.c}
///// sl_simple_led_instances.c
///
///#include "sl_simple_led.h"
///#include "em_gpio.h"
///#include "sl_simple_led_inst0_config.h"
///
///sl_simple_led_context_t simple_inst0_context = {
///  .port = SL_SIMPLE_LED_INST0_PORT,
///  .pin = SL_SIMPLE_LED_INST0_PIN,
///  .polarity = SL_SIMPLE_LED_INST0_POLARITY,
///};
///
///const sl_led_t sl_led_inst0 = {
///  .context = &simple_inst0_context,
///  .init = sl_simple_led_init,
///  .turn_on = sl_simple_led_turn_on,
///  .turn_off = sl_simple_led_turn_off,
///  .toggle = sl_simple_led_toggle,
///  .get_state = sl_simple_led_get_state,
///};
///
///void sl_simple_led_init_instances(void)
///{
///  sl_led_init(&sl_led_inst0);
///}
///   @endcode
///
///   @note The sl_simple_led_instances.c file is shown with only one instance, but if more
///         were in use they would all appear in this .c file.
///
///   @code{.c}
///// sl_simple_led_instances.h
///
///#ifndef SL_SIMPLE_LED_INSTANCES_H
///#define SL_SIMPLE_LED_INSTANCES_H
///
///#include "sl_simple_led.h"
///
///extern const sl_led_t sl_led_inst0;
///
///void sl_simple_led_init_instances(void);
///
///#endif // SL_SIMPLE_LED_INIT_H
///   @endcode
///
///   @note The sl_simple_led_instances.h file is shown with only one instance, but if more
///         were in use they would all appear in this .h file.
///
///   @n @section simple_led_usage Simple LED Usage
///
///   The simple LED driver is for LEDs with basic on off functionality, and there
///   are no additional functions beyond those in the common driver. The LEDs can be
///   turned on and off, toggled, and their on/off state can be retrieved. The following
///   code shows how to control these LEDs. An LED should always be initialized before
///   calling any other functions with it.
///
///   @code{.c}
///// initialize simple LED
///sl_led_init(&simple_led_inst0);
///
///// turn on LED, turn off LED, and toggle
///sl_led_turn_on(&simple_led_inst0);
///sl_led_turn_off(&simple_led_inst0);
///sl_led_toggle(&simple_led_inst0);
///
///// get the state of the led
///sl_led_state_t state = sl_led_get_state(&simple_led_instance0);
///   @endcode
///
/// @} end group simple_led ********************************************************/

#ifdef __cplusplus
}
#endif

#endif // SL_SIMPLE_LED_H
