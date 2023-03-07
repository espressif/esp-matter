// *****************************************************************************
// * button-interface.h
// *
// * Routines for counting the number of button presses to implement a complex
// * button interface.
// *
// * Copyright 2015 Silicon Laboratories, Inc.                              *80*
// *****************************************************************************

// This code uses a number of  ::EmberEventControl events to properly function.
// The application framework will generally manage the event automatically.
// Customers who do not use the framework must ensure the event is run, by
// calling either ::emberRunEvents or ::emberRunTask.

#ifndef __BUTTON_INTERFACE_H__
#define __BUTTON_INTERFACE_H__

// Polarity states for each button.
typedef enum {
  EMBER_AF_BUTTON_INTERFACE_POLARITY_ACTIVE_LO = 0x00,
  EMBER_AF_BUTTON_INTERFACE_POLARITY_ACTIVE_HI = 0x01,
} HalButtonInterfacePolarity;

// ------------------------------------------------------------------------------
// Forward declaration of plugin provided callbacks

/** @brief Callback generated when button 0 is released after being held a short
 * amount of time
 *
 * @note This callback will be called when button 0 transitions from its active
 * to non-active state if it was held for a shorter amount of time than
 * specified as a long press by the button 0 plugin option.
 *
 * @param timePressedMs  The amount time in milliseconds the button was pressed.
 */
void emberAfPluginButtonInterfaceButton0PressedShortCallback(
  uint16_t timePressedMs);

/** @brief Callback generated when button 1 is released after being held a short
 * amount of time
 *
 * @note This callback will be called when button 1 transitions from its active
 * to non-active state if it was held for a shorter amount of time than
 * specified as a long press by the button 1 plugin option.
 *
 * @param timePressedMs  The amount time in milliseconds the button was pressed.
 */
void emberAfPluginButtonInterfaceButton1PressedShortCallback(
  uint16_t timePressedMs);

/** @brief Callback generated when button 0 is released after being held a long
 * amount of time
 *
 * @note This callback will be called when button 0 transitions from its active
 * to non-active state if it was held for a longer amount of time than
 * specified as a long press by the button 1 plugin option.
 *
 * @param timePressedMs  The amount time in milliseconds the button was pressed.
 * @param pressedAtReset True if the button was pressed at reset.
 */
void emberAfPluginButtonInterfaceButton0PressedLongCallback(
  uint16_t timePressedMs,
  bool     pressedAtReset);

/** @brief Callback generated when button 1 is released after being held a long
 * amount of time
 *
 * This callback will be called when button 1 transitions from its active
 * to non-active state if it was held for a longer amount of time than
 * specified as a long press by the button 1 plugin option.
 * @param timePressedMs  The amount time in milliseconds the button was pressed.
 * @param pressedAtReset True if the button was pressed at reset.
 */
void emberAfPluginButtonInterfaceButton1PressedLongCallback(
  uint16_t timePressedMs,
  bool     pressedAtReset);

/** @brief Callback generated when button 0 has been held for a long amount
 * of time
 *
 * @note This callback will be called when button 0 has been held in its active
 * state for a long amount of time, where long is defined by plugin option.
 * Unlike emberAfPluginButtonInterfaceButton0PressedLongCallback, this callback
 * will occur immediately after the button timeout interval has passed, instead
 * of waiting until the button is released.
 *
 */
void emberAfPluginButtonInterfaceButton0PressingCallback(void);

/** @brief Callback generated when button 1 has been held for a long amount
 * of time
 *
 * @note This callback will be called when button 1 has been held in its active
 * state for a long amount of time, where long is defined by plugin option.
 * Unlike emberAfPluginButtonInterfaceButton1PressedLongCallback, this callback
 * will occur immediately after the button timeout interval has passed, instead
 * of waiting until the button is released.
 *
 */
void emberAfPluginButtonInterfaceButton1PressingCallback(void);

/** @brief Callback generated when button 0 transfers from low to high.
 *
 * @note This callback will occur immediately when the GPIO state of the button
 * transitions from low to high.
 *
 */
void emberAfPluginButtonInterfaceButton0HighCallback(void);

/** @brief Callback generated when button 0 transfers from high to low.
 *
 * @note This callback will occur immediately when the GPIO state of the button
 * transitions from low to high.
 *
 */
void emberAfPluginButtonInterfaceButton0LowCallback(void);

/** @brief Callback generated when button 1 transfers from low to high.
 *
 * @note This callback will occur immediately when the GPIO state of the button
 * transitions from low to high.
 *
 */
void emberAfPluginButtonInterfaceButton1HighCallback(void);

/** @brief Callback generated when button 1 transfers from high to low.
 *
 * @note This callback will occur immediately when the GPIO state of the button
 * transitions from low to high.
 *
 */
void emberAfPluginButtonInterfaceButton1LowCallback(void);

// ------------------------------------------------------------------------------
// Plugin public function declarations

/** @brief Get the state (pressed v not pressed) of a button
 *
 * @note This function will read and return the state of the GPIO assigned to
 * the button specified by the parameter.
 *
 * #param button  The button whose value is to be polled.
 *
 * @return This function will return one of the hal/button.h defined macros
 * to relay the state of the button: BUTTON_PRESSED if the button is pressed
 * or BUTTON_RELEASED if the button is not pressed.
 */
uint8_t halPluginButtonInterfaceButtonPoll(uint8_t button);

/** @brief Modify the active polarity of a given button
 *
 * @note This function will change a given button to act with a given active
 * polarity.
 *
 * @param button  The button whose polarity is to be modified
 * @param polarity  The polarity to set the button to.  This should be set using
 * the button-interface.h provided enum type values of BUTTON_POL_ACTIVE_LO or
 * BUTTON_POL_ACTIVE_HI.
 */
void halPluginButtonInterfaceSetButtonPolarity(
  uint8_t                    button,
  HalButtonInterfacePolarity polarity);

#endif // __BUTTON_INTERFACE_H__
