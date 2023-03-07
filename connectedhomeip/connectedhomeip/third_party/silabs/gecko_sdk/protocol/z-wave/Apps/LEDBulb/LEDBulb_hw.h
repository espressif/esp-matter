/**
 * @file
 * Platform abstraction for LED Bulb application
 * 
 * @copyright 2021 Silicon Laboratories Inc.
 */
#ifndef LEDBULB_HW_H_
#define LEDBULB_HW_H_

#include <CC_ColorSwitch.h>
#include <CC_MultilevelSwitch_Support.h>

/**
 * Initialize hardware modules specific to LED Bulb application.
 *
 * @param[in] multilevel_switch_max Maximum value for multilevel switch.
 * @param[in] color_switch_max      Maximum value for color switch.
 */
void LEDBulb_hw_init(uint8_t multilevel_switch_max, uint8_t color_switch_max);

/**
 * Callback for RED component of color switch. Called when color component is changed.
 *
 * @param[in] colorComponent Pointer to corresponding @ref s_colorComponent object.
 */
void LEDBulb_hw_callback_RED(s_colorComponent * colorComponent);

/**
 * Callback for GREEN component of color switch. Called when color component is changed.
 *
 * @param[in] colorComponent Pointer to corresponding @ref s_colorComponent object.
 */
void LEDBulb_hw_callback_GREEN(s_colorComponent * colorComponent);

/**
 * Callback for BLUE component of color switch. Called when color component is changed.
 *
 * @param[in] colorComponent Pointer to corresponding @ref s_colorComponent object.
 */
void LEDBulb_hw_callback_BLUE(s_colorComponent * colorComponent);

/**
 * Multilevel switch handler. Called when value of the switch is changed.
 *
 * @param[in] p_switch Pointer to corresponding @ref cc_multilevel_switch_t object.
 */
void LEDBulb_hw_multilevel_switch_handler(cc_multilevel_switch_t * p_switch);

#endif /* LEDBULB_HW_H_ */
