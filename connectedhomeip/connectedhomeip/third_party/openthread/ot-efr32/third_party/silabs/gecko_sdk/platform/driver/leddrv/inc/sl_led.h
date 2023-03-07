/***************************************************************************//**
 * @file
 * @brief LED Driver
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

#ifndef SL_LED_H
#define SL_LED_H

#include <stdint.h>
#include "sl_status.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup led LED Driver
 * @brief Generic LED Driver
 * @{
 ******************************************************************************/

/*******************************************************************************
 ******************************   DEFINES   ************************************
 ******************************************************************************/

#define SL_LED_CURRENT_STATE_OFF  0U            ///< LED state off
#define SL_LED_CURRENT_STATE_ON   1U            ///< LED state on

/*******************************************************************************
 *****************************   DATA TYPES   **********************************
 ******************************************************************************/

typedef uint8_t sl_led_state_t;                 ///< LED state

/// A LED instance
typedef struct {
  void           *context;                        ///< The context for this LED instance
  sl_status_t    (*init)(void *context);          ///< Member function to initialize LED instance
  void           (*turn_on)(void *context);       ///< Member function to turn on LED
  void           (*turn_off)(void *context);      ///< Member function to turn off LED
  void           (*toggle)(void *context);        ///< Member function to toggle LED
  sl_led_state_t (*get_state)(void *context);     ///< Member function to retrieve LED state
} sl_led_t;

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

/***************************************************************************//**
 * Initialize the LED driver. Call this function before any other LED
 * function. Initializes the selected LED GPIO, mode, and polarity.
 *
 * @param[in] led_handle    Pointer to instance of sl_led_t to initialize
 *
 * @return    Status Code:
 *              - SL_STATUS_OK
 ******************************************************************************/
sl_status_t sl_led_init(const sl_led_t *led_handle);

/***************************************************************************//**
 * Turn on the LED.
 *
 * @param[in] led_handle    Pointer to instance of sl_led_t to turn on
 ******************************************************************************/
void sl_led_turn_on(const sl_led_t *led_handle);

/***************************************************************************//**
 * Turn off the LED.
 *
 * @param[in] led_handle    Pointer to instance of sl_led_t to turn off
 ******************************************************************************/
void sl_led_turn_off(const sl_led_t *led_handle);

/***************************************************************************//**
 * Toggle the LED. Turn it on if it is off, and off if it is on.
 *
 * @param[in] led_handle    Pointer to instance of sl_led_t to toggle
 ******************************************************************************/
void sl_led_toggle(const sl_led_t *led_handle);

/***************************************************************************//**
 * Get the current state of the LED.
 *
 * @param[in] led_handle         Pointer to instance of sl_led_t to check
 *
 * @return    sl_led_state_t     Current state of LED. 1 for on, 0 for off
 ******************************************************************************/
sl_led_state_t sl_led_get_state(const sl_led_t *led_handle);

/** @} (end group led) */

// ******** THE REST OF THE FILE IS DOCUMENTATION ONLY !***********************
/// @addtogroup led LED API
/// @{
///
///   @details
///
///   @li @ref leddrv_intro
///   @li @ref leddrv_config
///   @li @ref leddrv_usage
///
///   @n @section leddrv_intro Introduction
///
///   The LED driver is a platfom level software module that manages the control of
///   various types of LEDs. There are currently two types of LEDs supported by the
///   LED driver:
///
///   @li @ref simple_led
///   @li @ref simple_rgbw_pwm_led
///
///   The common LED functions are called through the generic LED driver, while other
///   functions specific to a certain type of LED are called directly through their own
///   driver.
///
///   @n @section leddrv_config Configuration
///
///   All LED instances are configured with an @ref sl_led_t struct and a type specific
///   context struct, and sometimes additional structs. There is also a typedef
///   sl_led_state_t to get the state of an LED. These structs are automatically
///   generated after an LED is set up using Simplicity Studio's wizard, along with a
///   function definition for initializing all LEDs of that type. Specific setups for the
///   various LED types are in the following sections.
///
///   @li @ref simple_led_config
///   @li @ref rgbw_led_config
///
///   @n @section leddrv_usage Usage
///
///   Once the LED structs are defined, the common LED functions can be called being passed an instance
///   of sl_led_t, which will be redirected to calling the type specific version of that function. The
///   common functions include the following:
///
///   @li @ref sl_led_init
///   @li @ref sl_led_turn_on
///   @li @ref sl_led_turn_off
///   @li @ref sl_led_toggle
///   @li @ref sl_led_get_state
///
///   These functions allow for initializing the LED, turning it on and off, toggling it, and getting
///   the current on/off state of it. Other functions are called through the specific type of LED's API
///   directly. The usages of the different types of LEDs are described in detail in the following sections.
///
///   @li @ref simple_led_usage
///   @li @ref rgbw_led_usage
///
/// @} end group led ********************************************************/

#ifdef __cplusplus
}
#endif

#endif // SL_LED_H
