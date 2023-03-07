// Copyright 2016 Silicon Laboratories, Inc.                                *80*

#ifndef __BULB_PWM_DRIVER_H__
#define __BULB_PWM_DRIVER_H__

#include "hal/micro/micro.h"

#include PLATFORM_HEADER
#include "include/error.h"
#include "hal/micro/cortexm3/flash.h"

/** @brief Macro to signal that we wish to use the default frequency for the
 *  PWM driver.
 */
#define HAL_BULB_PWM_DRIVER_USE_DEFAULT_FREQUENCY 0xffff

/** @brief Macro to signal that we wish to blink forever.  It is used for the
 * count argument for the blink APIs below.
 */
#define HAL_BULB_PWM_DRIVER_BLINK_FOREVER         0xFF

/**
 * @brief Software handle for the white PWM.  Note:  this can be any 8-bit
 * integer so long as it is unique among the PWM handles.
 */
#define HAL_BULBPWM_WHITE_ID            1
/**
 * @brief Software handle for the red PWM.  Note:  this can be any 8-bit
 * integer so long as it is unique among the PWM handles.
 */
#define HAL_BULBPWM_RED_ID              2
/**
 * @brief Software handle for the green PWM.  Note:  this can be any 8-bit
 * integer so long as it is unique among the PWM handles.
 */
#define HAL_BULBPWM_GREEN_ID            3
/**
 * @brief Software handle for the blue PWM.  Note:  this can be any 8-bit
 * integer so long as it is unique among the PWM handles.
 */
#define HAL_BULBPWM_BLUE_ID             4
/**
 * @brief Software handle for the low temperature PWM.  Note:  this can be
 * any 8-bit integer so long as it is unique among the PWM handles.
 */
#define HAL_BULBPWM_LOWTEMP_ID          5
/**
 * @brief Software handle for the status PWM.  Note:  this can be any 8-bit
 * integer so long as it is unique among the PWM handles.
 */
#define HAL_BULBPWM_STATUS_ID           6
/**
 * @brief Software handle for the amber PWM.  Note:  this can be any 8-bit
 * integer so long as it is unique among the PWM handles.
 */
#define HAL_BULBPWM_AMBER_ID           7

/** @brief Return the ticks per PWM period.
 *
 * This function will examine the frequency configuration and determine the
 * number of PWM ticks required to implement that frequency.
 *
 * @return An ::uint16_t value that is the number of PWM ticks per period.
 */
uint16_t halBulbPwmDriverTicksPerPeriod(void);

/** @brief Return the ticks per microsecond.
 *
 * This function will return the number of PWM ticks per microsecond.
 *
 * @return An ::uint16_t value that is the number of PWM ticks per microsecond.
 */
uint16_t halBulbPwmDriverTicksPerMicrosecond(void);

/** @brief Function to drive selected pwm.
 *
 * This function will set the compare registers for the specified PWM channel.
 * Note:  the channels are specified by the
 *
 * @param value:  value for the PWM comparison register.
 *
 * @param pwm:  PWM channel to set (as defined by the board header file).
 *
 */
void halBulbPwmDriverSetPwmLevel(uint16_t value, uint8_t pwm);

/** @brief Function to turn on the LED output.
 *
 * Function to turn the LED on full brightness as an indication to the
 * user.  After the time, the LED will be reset to the appropriate values
 * as determined by the level, on/off, and color control cluster (if
 * appropriate).
 *
 * @param time:  Number of seconds to turn the LED on.  0 means forever.
 */
void halBulbPwmDriverLedOn(uint8_t time);

/** @brief Function to turn off the LED output.
 *
 * Function to turn the LED off as an indication to the user.  After the time,
 * the LED will be reset to the appropriate values as determined by the level,
 * on/off, and color control cluster (if appropriate).
 *
 * @param time:  Number of seconds to turn the LED off.  0 means forever.
 */
void halBulbPwmDriverLedOff(uint8_t time);

/** @brief Blink the LED.
 *
 * Function to blink the LED as an indication to the user.  Note:  this will
 * blink the LED symmetrically.  If asymmetric blinking is required, please
 * use the function ::emberAfPluginBulbPwmDriverLedBlinkPattern(...).
 *
 * @param count:  Number of times to blink.  HAL_BULB_PWM_DRIVER_BLINK_FOREVER
 * means forever.
 *
 * @param blinkTime:  Amount of time the bulb will be on or off during the
 * blink.
 */
void halBulbPwmDriverLedBlink(uint8_t count, uint16_t blinkTime);

/** @brief Blink a pattern on the LED.
 *
 * Function to blink a pattern on the LED.  User sets up a pattern of on/off
 * events.
 *
 * @param count:  Number of times to blink the pattern.
 * HAL_BULB_PWM_DRIVER_BLINK_FOREVER means forever
 *
 * @param length:  Length of the pattern.  20 is the maximum length.
 *
 * @param pattern[]:  Series of on/off times for the blink pattern.
 *
 */
void halBulbPwmDriverLedBlinkPattern(uint8_t  count,
                                     uint8_t  length,
                                     uint16_t *pattern);

/** @brief Function to turn on the status LED output.
 *
 * Function to turn the status LED on.  This is intended to be used to indicate
 * network status on the lighting reference design hardware.
 *
 * @param time:  Number of seconds to turn the LED on.  0 means forever.
 */
void halBulbPwmDriverStatusOn(uint8_t time);

/** @brief Function to turn off the status LED output.
 *
 * Function to turn the status LED off.  This is intended to be used to
 * indicate network status on the lighting reference design hardware.
 *
 * @param time:  Number of seconds to turn the LED off.  0 means forever.
 */
void halBulbPwmDriverStatusOff(uint8_t time);

/** @brief Blink the status LED.
 *
 * Function to blink the status LED.  This is intended to be used to indicate
 * network status on the lighting reference design hardware
 *
 * @param count:  Number of times to blink.  HAL_BULB_PWM_DRIVER_BLINK_FOREVER
 * means forever.
 *
 * @param blinkTime:  Amount of time the bulb will be on or off during the
 * blink.
 */
void halBulbPwmDriverStatusBlink(uint8_t count, uint16_t blinkTime);

/** @brief Blink a pattern on the status LED.
 *
 * Function to blink a pattern on the status LED.  This is intended to be used
 * to indicate network status on the lighting reference design hardware.
 *
 * @param count:  Number of times to blink the pattern.
 * HAL_BULB_PWM_DRIVER_BLINK_FOREVER means forever
 *
 * @param length:  Length of the pattern.  20 is the maximum length.
 *
 * @param pattern[]:  Series of on/off times for the blink pattern.
 *
 */
void halBulbPwmDriverStatusBlinkPattern(uint8_t  count,
                                        uint8_t  length,
                                        uint16_t *pattern);

/** @brief Turn status LED on.
 *
 * Function to turn the status LED on.
 *
 */
void halBulbPwmDriverStatusLedOn(void);

/** @brief Turn status LED off.
 *
 * Function to turn the status LED off.
 *
 */
void halBulbPwmDriverStatusLedOff(void);

/** @brief This callback is generated during blinking behavior when it is time
 * to turn the bulb on.  While the plugin will determine when to blink the
 * bulb on or off, it is up to this callback to determine how to turn the bulb
 * on.
 *
 * @appusage Should be implemented by an application layer configuration
 * plugin.
 *
 */
void halBulbPwmDriverBlinkOnCallback(void);

/** @brief This callback is generated during blinking behavior when it is time
 * to turn the bulb off.  While the plugin will determine when to blink the
 * bulb on or off, it is up to this callback to determine how to turn the bulb
 * off.
 *
 * @appusage Should be implemented by an application layer configuration
 * plugin.
 *
 */
void halBulbPwmDriverBlinkOffCallback(void);

/** @brief This callback is generated when the application layer makes a
 * call to initiate blinking behavior.  It warns the application layer PWM
 * code to not attempt to drive the LEDs directly and interfere with the
 * blinking behavior.
 *
 * @appusage Should be implemented by an application layer configuration
 * plugin.
 *
 */
void halBulbPwmDriverBlinkStartCallback(void);

/** @brief This callback is generated when the current blinking command
 * finishes.  The application layer PWM code must then determine what the
 * bulb drive should be, based on the current appliation layer attributes
 * (i.e. level, on/off, color XY, etc.)
 *
 * @appusage Should be implemented by an application layer configuration
 * plugin.
 *
 */
void halBulbPwmDriverBlinkStopCallback(void);

#ifndef BULB_PWM_WHITE
  #define BULB_PWM_WHITE   HAL_BULBPWM_WHITE_ID
  #define BULB_PWM_LOWTEMP HAL_BULBPWM_LOWTEMP_ID
  #define BULB_PWM_RED     HAL_BULBPWM_RED_ID
  #define BULB_PWM_GREEN   HAL_BULBPWM_GREEN_ID
  #define BULB_PWM_BLUE    HAL_BULBPWM_BLUE_ID
#endif

#endif
