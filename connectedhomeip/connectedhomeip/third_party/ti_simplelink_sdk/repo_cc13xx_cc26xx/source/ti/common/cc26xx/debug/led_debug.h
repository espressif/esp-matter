/******************************************************************************

 @file  led_debug.h

 @brief This module contains the definitions for the  functionality of a
        bim debug utility.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2012-2022, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/
#ifndef LED_DEBUG_H
#define LED_DEBUG_H

#ifdef __cplusplus
extern "C"
{
#endif
/*******************************************************************************
 *                                          Includes
 */

#include "common/flash/no_rtos/extFlash/bsp.h"

/*******************************************************************************
 * MACROS
 */

#define RED_LED    IOID_6
#define GREEN_LED  IOID_7

/******************************************************************************
 * FUNCTIONS
 */

/**
 * @fn      delay
 *
 * @brief   Causes Proecessor to execute NOP's for the specified time delayMs
 *
 * @param   delayMs - desired delay in milliseconds
 *
 * @return  none
 */
extern void delay(uint32_t delayMs);

/**
 * @fn      blinkLed
 *
 * @brief   Blinks a led 'n' times, duty-cycle 50-50
 *
 * @param   led - led identifier
 * @param   nBlinks - number of blinks
 * @param   periodMs - period of each blink in ms
 *
 * @return  none
 */
extern void blinkLed(uint32_t led, uint8_t nBlinks, uint32_t periodMs);

/**
 * @fn      powerUpGpio
 *
 * @brief   Powers up the Gpio peripheral and sets appropriate pins as output
 *
 * @param   none
 *
 * @return  none
 */
extern void powerUpGpio(void);

/**
 * @fn      powerDownGpio
 *
 * @brief   Powers down the Gpio peripheral. Note: the External Flash close
 *          will do this also.
 *
 * @param   none
 *
 * @return  none
 */
extern void powerDownGpio(void);

/**
 * @fn      lightRedLed
 *
 * @brief   Lights RED LED
 *
 * @param   none
 *
 * @return  none
 */
extern void lightRedLed(void);

/**
 * @fn      lightGreenLed
 *
 * @brief   Lights RED LED
 *
 * @param   none
 *
 * @return  none
 */
extern void lightGreenLed(void);

#endif /* LED_DEBUG_H */
