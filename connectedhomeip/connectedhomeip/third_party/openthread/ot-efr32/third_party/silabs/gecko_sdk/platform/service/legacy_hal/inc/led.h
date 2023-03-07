/***************************************************************************//**
 * @file
 * @brief See @ref led for documentation.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifndef __LED_H__
#define __LED_H__

enum HalBoardLedPins {
#if BSP_LED_COUNT <= 1
  BOARDLED0 = 0,
  BOARDLED1 = BOARDLED0,
  BOARDLED2 = BOARDLED0,
  BOARDLED3 = BOARDLED0,
  BOARD_ACTIVITY_LED  = BOARDLED0,
  BOARD_HEARTBEAT_LED = BOARDLED0
#endif
#if BSP_LED_COUNT == 2
  BOARDLED0 = 0,
  BOARDLED1 = 1,
  BOARDLED2 = BOARDLED0,
  BOARDLED3 = BOARDLED1,
  BOARD_ACTIVITY_LED  = BOARDLED0,
  BOARD_HEARTBEAT_LED = BOARDLED1
#endif
#if BSP_LED_COUNT == 3
  BOARDLED0 = 0,
  BOARDLED1 = 1,
  BOARDLED2 = 2,
  BOARDLED3 = BOARDLED1,
  BOARD_ACTIVITY_LED  = BOARDLED0,
  BOARD_HEARTBEAT_LED = BOARDLED1
#endif
#if BSP_LED_COUNT >= 4
  BOARDLED0 = 0,
  BOARDLED1 = 1,
  BOARDLED2 = 2,
  BOARDLED3 = 3,
  BOARD_ACTIVITY_LED  = BOARDLED0,
  BOARD_HEARTBEAT_LED = BOARDLED1
#endif
};

/***************************************************************************//**
 * @addtogroup legacyhal
 * @{
 ******************************************************************************/

/** @addtogroup led LED
 *  @brief  Sample API funtions for controlling LEDs
 *
 * When specifying an LED to use, always use the BOARDLEDx definitions that
 * are defined within the BOARD_HEADER.
 *
 * See led.h for source code.
 *@{
 */

/** @brief Configures GPIOs pertaining to the control of LEDs.
 */
void halInternalInitLed(void);

/** @brief Ensures that the definitions from the BOARD_HEADER
 *  are always used as parameters to the LED functions.
 */
#if defined(STACK) || defined(MINIMAL_HAL)
typedef uint8_t HalBoardLed;
#else
typedef enum HalBoardLedPins HalBoardLed;
#endif
// Note: Even though many compilers will use 16 bits for an enum instead of 8,
//  we choose to use an enum here.  The possible compiler inefficiency does not
//  affect stack-based parameters and local variables, which is the
//  general case for led paramters.

/** @brief Atomically wraps an XOR or similar operation for a single GPIO
 *  pin attached to an LED.
 *
 *  @param led  Identifier (from BOARD_HEADER) for the LED to be toggled.
 */
void halToggleLed(HalBoardLed led);

/** @brief Turns on (sets) a GPIO pin connected to an LED so that the LED
 *  turns on.
 *
 *  @param led  Identifier (from BOARD_HEADER) for the LED to turn on.
 */
void halSetLed(HalBoardLed led);

/** @brief Turns off (clears) a GPIO pin connected to an LED, which turns
 *  off the LED.
 *
 *  @param led  Identifier (from BOARD_HEADER) for the LED to turn off.
 */
void halClearLed(HalBoardLed led);

/** @brief Called by the stack to indicate activity over the radio (for
 *  both transmission and reception). It is called once with \c turnOn true and
 *  shortly thereafter with \c turnOn false.
 *
 *  Typically does something interesting, such as change the state of
 *  an LED.
 *
 *  @param turnOn  See Usage.
 */
void halStackIndicateActivity(bool turnOn);

/** @} (end addtogroup led) */
/** @} (end addtogroup legacyhal) */

#endif // __LED_H__
