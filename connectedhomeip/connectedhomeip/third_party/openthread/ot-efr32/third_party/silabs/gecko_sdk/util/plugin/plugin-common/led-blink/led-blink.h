// *******************************************************************
// * led-blink.h
// *
// * API to allow for basic control of activity LEDs, including on/off
// * functionality and blink pattern creation.
// *
// * Copyright 2015 Silicon Laboratories, Inc.                              *80*
// *******************************************************************

// This code uses a number of  ::EmberEventControl events to properly function.
// The application framework will generally manage the event automatically.
// Customers who do not use the framework must ensure the event is run, by
// calling either ::emberRunEvents or ::emberRunTask.

#ifndef __LED_BLINK_H__
#define __LED_BLINK_H__

/** @brief Set which LED is controlled by the LedBlink functions
 *
 * @note The led blink API is currently limited to one LED.  This function will
 * set which LED should be designated as the activity LED.
 *
 * @param led  The led to set as the activity LED.  This should either be the
 * macro name of the led (ex: BOARDLED0) or the micro-common macro style
 * reference to the led (ex: PORTA_PIN(6)).
 */
void halLedBlinkSetActivityLed(uint8_t led);

/** @brief Set the activity LED to blink a given number of times.
 *
 * @note This function will cause the activity LED to blink a user provided
 * number of times at a user provided rate.
 *
 * @param count  The number of times to blink the LED
 * @param blinkTimeMs  The amount of time (in ms) the LED should be on and off
 */
void halLedBlinkBlink(uint8_t count, uint16_t blinkTimeMs);

/** @brief Cause the activity LED to blink in a specified pattern.
 *
 * @note This function will cause the activity LED to blink a user specified
 * pattern.
 *
 * @param count  The number of times to cycle through the pattern
 * @param length  The number entries in the pattern array
 * @param pattern  An array of on and off times (in mS) that describes the
 * blink pattern.  The first integer must describe an on time (can be 0), but
 * the last integer is not required to be an off time.  Maximum pattern length
 * is 20 unsigned 16 bit entries.
 */
void halLedBlinkPattern(uint8_t count, uint8_t length, uint16_t *pattern);

/** @brief Turn the Activity LED off.
 *
 * @note This function will turn the activity LED off.  If a value of 0 is
 * passed in, it will keep the LED off indefinitely.  Otherwise, it will turn
 * the LED back on after the specified amount of time has passed.
 *
 * @param time  The amount of time (in mS) to turn the LED off.  If 0, the LED
 * will remain off indefinitely.
 */
void halLedBlinkLedOff(uint8_t timeMs);

/** @brief Turn the Activity LED on.
 *
 * @note This function will turn the activity LED on.  If a value of 0 is
 * passed in, it will keep the LED on indefinitely.  Otherwise, it will turn
 * the LED back off after the specified amount of time has passed.
 *
 * @param time  The amount of time (in mS) to turn the LED on.  If 0, the LED
 * will remain on indefinitely.
 */
void halLedBlinkLedOn(uint8_t timeMs);

/** @brief Set which LEDs are controlled by the MultiLedBlink functions
 *
 * @note The multiple led blink API is currently limited to two LEDs. This
 * function should be called twice with different "led" parameter to activate
 * both of them. This should be done prior to any other multiple LED operations.
 *
 * @param led  The led to set as the activity LED. This should either be the
 * macro name of the led (ex: BOARDLED0) or the micro-common macro style
 * reference to the led (ex: PORTA_PIN(6)).
 */
void halMultiLedBlinkSetActivityLeds(uint8_t led);

/** @brief Set the activity LED to blink a given number of times, for multiple
 * LED control
 *
 * @note This function will cause the activity LED to blink a user provided
 * number of times at a user provided rate.
 *
 * @param count  The number of times to blink the LED
 * @param blinkTimeMs  The amount of time (in ms) the LED should be on and off
 * @param led The led to blink.  This should either be the macro name
 * of the led (ex: BOARDLED0) or the micro-common macro style reference to the
 * led (ex: PORTA_PIN(6)).
 */
void halMultiLedBlinkBlink(uint8_t count, uint16_t blinkTimeMs, uint8_t led);

/** @brief Cause the activity LED to blink in a specified pattern, for multiple
 * LED control
 *
 * @note This function will cause the activity LED to blink a user specified
 * pattern.
 *
 * @param count  The number of times to cycle through the pattern
 * @param length  The number entries in the pattern array
 * @param pattern  An array of on and off times (in mS) that describes the
 * blink pattern.  The first integer must describe an on time (can be 0), but
 * the last integer is not required to be an off time.  Maximum pattern length
 * is 20 unsigned 16 bit entries.
 * @param led The led to blink.  This should either be the macro name
 * of the led (ex: BOARDLED0) or the micro-common macro style reference to the
 * led (ex: PORTA_PIN(6)).
 */
void halMultiLedBlinkPattern(uint8_t  count,
                             uint8_t  length,
                             uint16_t *pattern,
                             uint8_t  led);

/** @brief Turn the Activity LED off, for multiple LED control
 *
 * @note This function will turn the activity LED off.  If a value of 0 is
 * passed in, it will keep the LED off indefinitely.  Otherwise, it will turn
 * the LED back on after the specified amount of time has passed.
 *
 * @param time  The amount of time (in mS) to turn the LED off.  If 0, the LED
 * will remain off indefinitely.
 * @param led The led to be turned off.  This should either be the macro name
 * of the led (ex: BOARDLED0) or the micro-common macro style reference to the
 * led (ex: PORTA_PIN(6)).
 */
void halMultiLedBlinkLedOff(uint8_t timeMs, uint8_t led);

/** @brief Turn the Activity LED on, for multiple LED control
 *
 * @note This function will turn the activity LED on.  If a value of 0 is
 * passed in, it will keep the LED on indefinitely.  Otherwise, it will turn
 * the LED back off after the specified amount of time has passed.
 *
 * @param time  The amount of time (in mS) to turn the LED on.  If 0, the LED
 * will remain on indefinitely.
 * @param led The led to be turned on.  This should either be the macro name
 * of the led (ex: BOARDLED0) or the micro-common macro style reference to the
 * led (ex: PORTA_PIN(6)).
 */
void halMultiLedBlinkLedOn(uint8_t timeMs, uint8_t led);

/** @brief Clear a GPIO in a sleepy system
 *
 * @note This function will set a bit to zero for a sleepy system and ensure
 * that it remains zero when the device enters and exits sleep.
 *
 * @param port is the GPIO port of the pin to be cleared.
 * @param pin is the GPIO pin to be cleared.
 */
void halLedBlinkSleepyClearGpio(uint8_t port, uint8_t pin);

void halMultiBlinkSetLedActive(uint8_t led);

/** @brief Set a GPIO in a sleepy system
 *
 * @note This function will set a bit for a sleepy system and ensure that it
 * remains set when the device enters and exits sleep.
 *
 * @param port is the GPIO port of the pin to be cleared.
 * @param pin is the GPIO pin to be cleared.
 */
void halLedBlinkSleepySetGpio(uint8_t port, uint8_t pin);

#endif // __LED_BLINK_H__
