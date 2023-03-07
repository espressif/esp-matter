/******************************************************************************

 @file keys_utils.h

 @brief Utility functions for the launch pad keys

 Group: CMCU, LPC
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2017-2022, Texas Instruments Incorporated
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
#ifndef KEY_UTILS_H
#define KEY_UTILS_H


/******************************************************************************
 Includes
 *****************************************************************************/
/* Board Header files */
#include "ti_drivers_config.h"

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************
 Constants
 *****************************************************************************/
/* Left Key ID */
#define KEYS_LEFT              0x01
/* Right Key ID */
#define KEYS_RIGHT             0x02

/* Debounce timeout in milliseconds */
#define KEY_DEBOUNCE_TIMEOUT  200

/******************************************************************************
 Typedefs
 *****************************************************************************/

/* Key Press Callback function typedef */
typedef void (*KeyUtils_PressedCB_t)(uint8_t keysPressed);


/******************************************************************************
 External Functions
 *****************************************************************************/
/**
 * @brief   Enable interrupts for keys on GPIOs.
 *
 * @param   keyCb - application key pressed callback
 *
 */
extern void KeysUtils_initialize(KeyUtils_PressedCB_t keyCb);

#ifdef __cplusplus
}
#endif

#endif /* KEY_UTILS_H */
