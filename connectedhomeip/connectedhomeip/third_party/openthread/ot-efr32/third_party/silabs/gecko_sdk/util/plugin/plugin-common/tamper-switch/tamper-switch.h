// *****************************************************************************
// * tamper-switch.h
// *
// * This plugin uses the button-interface plugin to create a tamper switch.  It
// * will activate itself once it detects a very long press on the button, and
// * generate callbacks whenever it detects the device has been tampered with.
// *
// * Copyright 2015 Silicon Laboratories, Inc.                              *80*
// *****************************************************************************

// Some implementations of this function may make use of ::EmberEventControl
// events.  If so, the application framework will generally manage the events
// automatically.  Customers who do not use the framework must ensure the events
// are run, by calling either ::emberRunEvents or ::emberRunTask.  See the
// specific implementation for more information.

#ifndef __TAMPER_SWITCH_H__
#define __TAMPER_SWITCH_H__

// ******************************************************************************
// Perform all I2C transactions necessary to read the temperature from the
// SI7503, convert it to milliDegrees Celsius, and return it.
// ******************************************************************************

/** @brief Initialize the tamper switch hardware
 *
 * This function will initialize all hardware required for the tamper switch
 * plugin to function properly.
 */
void halTamperSwitchInitialize(void);

/** @brief Get the state (pushed or not) of the tamper switch
 *
 * This function can be used to determine if the tamper switch is currently
 * active (pressed).  It does take button polarity into consideration.
 *
 * @return  1 if the switch is pressed, 0 if the switch is not pressed.
 */
uint8_t halTamperSwitchGetValue(void);

/** @brief Disarm the tamper switch
 *
 * This function will remove the tamper switch from tamper detect mode.
 *
 * @return  None
 */
void halTamperSwitchDisarm(void);

#endif // __TAMPER_SWITCH_H__
