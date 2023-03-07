/*
 * Copyright (c) 2021, Qorvo Inc
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 * $Change$
 * $DateTime$
 */

/** @file "qvIO_IO.h"
 *
 *  IO wrapper IO API
 *
 *  Declarations of the IO specific public functions and enumerations of qvIO.
*/

#ifndef _QVIO_H_
#define _QVIO_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/*****************************************************************************
 *                    Enum Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/*! LED indexes */
#define LED_RED   0
#define LED_GREEN 1
#define LED_WHITE 2

/*! PWM color LED channels */
#define PWM_CHANNEL_RED   (0)
#define PWM_CHANNEL_GREEN (1)
#define PWM_CHANNEL_BLUE  (2)
#define PWM_CHANNEL_WHITE_COOL  (4)
#define PWM_CHANNEL_WHITE_WARM  (5)

/*! Button and switch (SW3) designations */
#define BTN_SW1     (0)
#define BTN_SW2     (1)
#define BTN_SW3     (2)
#define BTN_SW4     (3)
#define BTN_SW5     (4)

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/** @brief Callback type for button press callback */
typedef void (*qvIO_pBtnCback)(uint8_t btnIdx, bool btnPressed);

/*****************************************************************************
 *                    Public Function Prototypes
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 *                    LED/BTN API
 *****************************************************************************/

/** @brief Set LED ON or OFF.
*
*   @param ledNr                     The index of the LED that is controlled.
*   @param state                     LED on (true) or off (false).
*/
bool qvIO_LedSet(uint8_t ledNr, bool state);

/** @brief Blink a LED with specified on and off period.
*
*   @param ledNr                     The index of the LED that is controlled.
*   @param onMs                      How many msec should the ON state last.
*   @param offMs                     How many msec should the OFF state last.
*/
bool qvIO_LedBlink(uint8_t ledNr, uint16_t onMs, uint16_t offMs);

/** @brief Store internally an upper layer callback for signaling button presses.
*
*   @param btnCback                  Pointer to the button handler to be stored internally.
*/
void qvIO_SetBtnCallback(qvIO_pBtnCback btnCback);

/** @brief Initialize UART for use.
 *
*/

void qvIO_UartInit(void);

/** @brief Transmit data over the UART
 *
 *  @param length         Length of data to send
 *  @param txBuffer       Pointer to a buffer to transmit
*/
void qvIO_UartTxData(uint8_t length, const char* txBuffer);

/** @brief Get received UART data
 *
 *  @param length          Max length of data to retrieve
 *  @param txBuffer        Pointer to a buffer to received the data in.
 *                         It is assumed to be large enough to hold 'length' bytes
 *
 *  @return receivedLength Amount of bytes returned in the buffer.
 *                         If more data was pending, only 'length' bytes are returned.
*/
uint8_t qvIO_UartReadRxData(uint8_t length, char* rxBuffer);


/** @brief turns color LED on or off
*
*   @param onoff              true for on, false for off
*/
void qvIO_PWMColorOnOff(bool onoff);

/** @brief sets RGB color of led 255 == 100%
*
*   @param r                    intensity of red (0-255)
*   @param g                    intensity of green (0-255)
*   @param b                    intensity of blue (0-255)
*/
void qvIO_PWMSetColor(uint8_t r, uint8_t g, uint8_t b);

/** @brief sets brightness of led 255 = 100%
*
*   @param channel              pwm channel
*   @param level                intensity of level (0-255)
*/
void qvIO_PWMSetLevel(uint8_t channel, uint8_t level);

/** @brief Initialize IO interface for use.
 *
*/

void qvIO_Init(void);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //_QVIO_H_
