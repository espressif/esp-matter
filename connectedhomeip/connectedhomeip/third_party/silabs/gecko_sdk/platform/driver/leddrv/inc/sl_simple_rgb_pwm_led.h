/***************************************************************************//**
 * @file
 * @brief Simple RGB PWM LED Driver
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_SIMPLE_RGB_PWM_LED_H
#define SL_SIMPLE_RGB_PWM_LED_H

#include "sl_led.h"
#include "sl_pwm_led.h"
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
 * @addtogroup simple_rgb_pwm_led Simple RGB PWM LED Driver
 * @brief Simple Red/Green/Blue PWM LED Driver
 * @{
 ******************************************************************************/

/*******************************************************************************
 ******************************   DEFINES   ************************************
 ******************************************************************************/

#define SL_SIMPLE_RGB_PWM_LED_POLARITY_ACTIVE_HIGH 0U  ///< LED Active polarity High
#define SL_SIMPLE_RGB_PWM_LED_POLARITY_ACTIVE_LOW  1U  ///< LED Active polarity Low

#define SL_SIMPLE_RGB_PWM_LED_COLOR_R              0U  ///< LED Red
#define SL_SIMPLE_RGB_PWM_LED_COLOR_G              1U  ///< LED Green
#define SL_SIMPLE_RGB_PWM_LED_COLOR_B              2U  ///< LED Blue

#define SL_SIMPLE_RGB_PWM_LED_NUM_CC_REQUIRED      3U  ///< Number of Timer Capture Channels required (1 for each RGB color)

/*******************************************************************************
 *****************************   DATA TYPES   **********************************
 ******************************************************************************/

/// A Simple RGB LED context
typedef struct {
  sl_led_pwm_t      *red;       ///< PWM LED instance for red color
  sl_led_pwm_t      *green;     ///< PWM LED instance for green color
  sl_led_pwm_t      *blue;      ///< PWM LED instance for blue color

  TIMER_TypeDef     *timer;     ///< PWM timer (common instance for all LEDs)
  uint32_t          frequency;  ///< PWM frequency
  uint32_t          resolution; ///< PWM resolution (dimming steps)
  sl_led_state_t    state;      ///< Current state (ON/OFF)
} sl_simple_rgb_pwm_led_context_t;

/// A Simple RGB LED PWM instance
typedef struct {
  sl_led_t       led_common;                       ///< Inherit from the Common LED Driver
  void           (*set_rgb_color)(void *context,
                                  uint16_t red,
                                  uint16_t green,
                                  uint16_t blue);  ///< Member function to set RGB
  void           (*get_rgb_color)(void *context,
                                  uint16_t *red,
                                  uint16_t *green,
                                  uint16_t *blue); ///< Member function to get RGB
} sl_led_rgb_pwm_t;

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

/***************************************************************************//**
 * Initialize an RGB PWM LED driver.
 *
 * @param[in] led_handle         Pointer to rgb-pwm-led specific data.
 *
 * @return    Status Code:
 *              - SL_STATUS_OK   Success
 *              - SL_STATUS_FAIL Init error
 ******************************************************************************/
sl_status_t sl_simple_rgb_pwm_led_init(void *rgb);

/***************************************************************************//**
 * Turn on an RBG LED.
 *   Turns on at previously set color levels
 *   If no previous levels set, turns on at max level for all RGB LEDs
 *
 * @param[in] led_handle         Pointer to rgb_pwm-led specific data.
 *
 ******************************************************************************/
void sl_simple_rgb_pwm_led_turn_on(void *rgb);

/***************************************************************************//**
 * Turn off an RGB LED.
 *
 * @param[in] led_handle         Pointer to rgb-pwm-led specific data.
 *
 ******************************************************************************/
void sl_simple_rgb_pwm_led_turn_off(void *rgb);

/***************************************************************************//**
 * Toggle an RGB LED.
 *  The toggle "ON" behavior is as defined for sl_simple_rgb_pwm_led_turn_on()

 * @param[in] led_handle         Pointer to rgb-pwm-led specific data.
 *
 ******************************************************************************/
void sl_simple_rgb_pwm_led_toggle(void *rgb);

/***************************************************************************//**
 * Get status of an RGB LED.
 *
 * @param[in] led_handle         Pointer to rgb-pwm-led specific data.
 *
 * @return    sl_led_state_t     Current state of RGB LED.
 *                               0 for Red, Green, Blue and White LEDs are all OFF
 *                               1 for Red, Green, Blue or White LED is ON
 ******************************************************************************/
sl_led_state_t sl_simple_rgb_pwm_led_get_state(void *rgb);

/***************************************************************************//**
 * Set color mixing and dimming level of an RGB LED.
 *
 * @param[in] led_handle         Pointer to rgb-pwm-led specific data:
 *
 * @param[in] red                Red color level (PWM duty-cycle [0-65535])
 * @param[in] green              Green color level (PWM duty-cycle [0-65535])
 * @param[in] blue               Blue color level (PWM duty-cycle [0-65535])
 *
 ******************************************************************************/
void sl_simple_rgb_pwm_led_set_color(void *rgb,
                                     uint16_t red,
                                     uint16_t green,
                                     uint16_t blue);

/***************************************************************************//**
 * Get current color mixing and dimming level of an RGB LED.
 *
 * @note Will return the last stored levels regardless of the current ON/OFF
 * state. Call sl_simple_rgb_pwm_led_get_state() to determine if the RGB LED
 * is actually ON or OFF.
 *
 * @param[in]  led_handle        Pointer to rgb-pwm-led specific data:
 *
 * @param[out] red               Red color level (PWM duty-cycle [0-65535])
 * @param[out] green             Green color level (PWM duty-cycle [0-65535])
 * @param[out] blue              Blue color level (PWM duty-cycle [0-65535])
 *
 ******************************************************************************/
void sl_simple_rgb_pwm_led_get_color(void *rgb,
                                     uint16_t *red,
                                     uint16_t *green,
                                     uint16_t *blue);

/*******************************************************************************
 ****************   API extensions to the Common LED Driver   ******************
 ******************************************************************************/

void sl_led_set_rgb_color(const sl_led_rgb_pwm_t *rgb,   ///< LED Instance handle
                          uint16_t red,                           ///< LED red intensity
                          uint16_t green,                         ///< LED green intensity
                          uint16_t blue                           ///< LED blue intensity
                          );  ///< LED set RGB color

void sl_led_get_rgb_color(const sl_led_rgb_pwm_t *rgb,    ///< LED Instance handle
                          uint16_t * red,                           ///< LED red intensity
                          uint16_t * green,                         ///< LED green intensity
                          uint16_t * blue                           ///< LED blue intensity
                          );  ///< LED get RGB setting

/** @} (end group simple_rgb_pwm_led) */
/** @} (end group led) */

// ******** THE REST OF THE FILE IS DOCUMENTATION ONLY !***********************
/// @addtogroup simple_rgb_pwm_led Simple RGB PWM LED Driver
/// @{
///
///   @details
///
///   @n @section rgb_led_intro Introduction
///
///   The Simple RGB PWM LED Driver is a module for the LED driver that provides functionality
///   for controlling Red/Green/Blue LEDs that are driven by PWM.
///
///   @n @section rgb_led_config RGB PWM LED Configuration
///
///   RGB PWM LEDs use the @ref sl_led_t struct, and their own structs
///   @ref sl_simple_rgb_pwm_led_context_t and @ref sl_led_rgb_pwm_t. These are automatically
///   generated into the following files, as well as instance specific headers with macro
///   definitions in them. The samples below are for a single instance called "inst0".
///
///   @code{.c}
///// sl_simple_rgb_pwm_led_instances.c
///
///#include "em_gpio.h"
///#include "sl_simple_rgb_pwm_led.h"
///
///#include "sl_simple_rgb_pwm_led_inst0_config.h"
///
///
///
///sl_led_pwm_t red_inst0 = {
///  .port = SIMPLE_RGB_PWM_LED_INST0_PORT,
///  .pin = SIMPLE_RGB_PWM_LED_INST0_PIN,
///  .polarity = SIMPLE_RGB_PWM_LED_INST0_POLARITY,
///  .channel = SIMPLE_RGB_PWM_LED_INST0_CHANNEL,
///#if defined(SL_SIMPLE_RGB_PWM_LED_INST0_RED_LOC)
///  .location = SIMPLE_RGB_PWM_LED_INST0_LOC,
///#endif
///};
///
///sl_led_pwm_t green_inst0 = {
///  .port = SIMPLE_RGB_PWM_LED_INST0_PORT,
///  .pin = SIMPLE_RGB_PWM_LED_INST0_PIN,
///  .polarity = SIMPLE_RGB_PWM_LED_INST0_POLARITY,
///  .channel = SIMPLE_RGB_PWM_LED_INST0_CHANNEL,
///#if defined(SL_SIMPLE_RGB_PWM_LED_INST0_RED_LOC)
///  .location = SIMPLE_RGB_PWM_LED_INST0_LOC,
///#endif
///};
///
///sl_led_pwm_t blue_inst0 = {
///  .port = SIMPLE_RGB_PWM_LED_INST0_PORT,
///  .pin = SIMPLE_RGB_PWM_LED_INST0_PIN,
///  .polarity = SIMPLE_RGB_PWM_LED_INST0_POLARITY,
///  .channel = SIMPLE_RGB_PWM_LED_INST0_CHANNEL,
///#if defined(SL_SIMPLE_RGB_PWM_LED_INST0_RED_LOC)
///  .location = SIMPLE_RGB_PWM_LED_INST0_LOC,
///#endif
///};
///
///sl_simple_rgb_pwm_led_context_t simple_rgb_pwm_inst0_context = {
///  .red = &red_inst0,
///  .green = &green_inst0,
///  .blue = &blue_inst0,
///
///  .timer = SL_SIMPLE_RGB_PWM_LED_INST0_PERIPHERAL,
///  .frequency = SL_SIMPLE_RGB_PWM_LED_INST0_FREQUENCY,
///  .resolution = SL_SIMPLE_RGB_PWM_LED_INST0_RESOLUTION,
///};
///
///const sl_led_rgb_pwm_t sl_inst0 = {
///  .led_common.context = &simple_rgb_pwm_inst0_context,
///  .led_common.init = sl_simple_rgb_pwm_led_init,
///  .led_common.turn_on = sl_simple_rgb_pwm_led_turn_on,
///  .led_common.turn_off = sl_simple_rgb_pwm_led_turn_off,
///  .led_common.toggle = sl_simple_rgb_pwm_led_toggle,
///  .led_common.get_state = sl_simple_rgb_pwm_led_get_state,
///  .set_rgb_color = sl_simple_rgb_pwm_led_set_color,
///  .get_rgb_color = sl_simple_rgb_pwm_led_get_color,
///};
///
///
///
///void sl_simple_rgb_pwm_led_init_instances(void)
///{
///
///  sl_led_init((sl_led_t *)&sl_inst0);
///
///}
///   @endcode
///
///   @note The sl_simple_rgb_pwm_led_instances.c file is shown with only one instance, but if more
///         were in use they would all appear in this .c file.
///
///   @code{.c}
///// sl_simple_rgb_pwm_led_instances.h
///
///#ifndef SL_SIMPLE_RGB_PWM_LED_INSTANCES_H
///#define SL_SIMPLE_RGB_PWM_LED_INSTANCES_H
///
///#include "sl_simple_rgb_pwm_led.h"
///
///extern const sl_led_rgb_pwm_t sl_inst0;
///
///void sl_simple_rgb_pwm_led_init_instances(void);
///
///#endif // SL_SIMPLE_RGB_PWM_LED_INIT_H
///   @endcode
///
///   @note The sl_simple_rgb_pwm_led_instances.h file is shown with only one instance, but if more
///         were in use they would all appear in this .h file.
///
///   @n @section rgb_led_usage RGB PWM LED Usage
///
///   The RGB PWM Led driver provides functionality for controlling Red/Green/Blue/White
///   LEDs that are driven by PWM. The LEDs can be turned on and off and toggled, and remember
///   their color and brightness state when being turned back on. The color and brightness can be
///   set using values of 0-65535 for red, green, blue, and white. Retrieving the state gives the
///   on/off value, while retrieving the color gives the rgb values. The following
///   code shows how to control these LEDs. An LED should always be initialized before
///   calling any other functions with it.
///
///   @code{.c}
///// initialize rgb LED
///sl_led_init(&rgb_led_inst0);
///
///// turn on LED, set color to purple, turn off, toggle (would maintain purple color)
///sl_led_turn_on(&rgb_led_inst0);
///uint16_t red = 65535; // max red
///uint16_t green = 0; // no green
///uint16_t blue = 65535; // max blue
///sl_led_set_rgb_color(&rgb_led_inst0, red, green, blue);
///sl_led_turn_off(&rgb_led_inst0);
///sl_led_toggle(&rgb_led_inst0);
///
///// get the state of the led
///sl_led_state_t state = sl_led_get_state(&rgb_led_inst0);
///   @endcode
///
/// @} end group led ********************************************************/

#ifdef __cplusplus
}
#endif

#endif // SL_SIMPLE_RGB_PWM_LED_H
