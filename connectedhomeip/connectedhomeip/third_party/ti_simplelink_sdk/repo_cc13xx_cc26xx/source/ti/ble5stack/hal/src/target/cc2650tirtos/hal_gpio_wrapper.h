/******************************************************************************

 @file  hal_gpio_wrapper.h

 @brief This file contains a simplified GPIO mapping for debug.

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2013-2022, Texas Instruments Incorporated
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

#ifndef HAL_GPIO_WRAPPER_H
#define HAL_GPIO_WRAPPER_H

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * INCLUDES
 */

/*******************************************************************************
 * MACROS
 */

#ifdef DEBUG_GPIO

#define HAL_GPIO_INIT()                                                        \
  {                                                                            \
    IOCPinTypeGpioOutput( HAL_GPIO_1 );   /* SmartRF06EB LED3    P403.2 */     \
    IOCPinTypeGpioOutput( HAL_GPIO_2 );   /* SmartRF06EB LED4    P403.4 */     \
    IOCPinTypeGpioOutput( HAL_GPIO_3 );   /* SmartRF06EB LED1    P404.20 */    \
    IOCPinTypeGpioOutput( HAL_GPIO_4 );   /* SmartRF06EB LED2    P405.4 */     \
    IOCPinTypeGpioOutput( HAL_GPIO_5 );   /* SmartRF06EB NC      P403.6 */     \
    IOCPinTypeGpioOutput( HAL_GPIO_6 );   /* SmartRF06EB BTN_L   P403.8 */     \
    IOCPinTypeGpioOutput( HAL_GPIO_7 );   /* SmartRF06EB BTN_R   P403.10 */    \
    IOCPinTypeGpioOutput( HAL_GPIO_8 );   /* SmartRF06EB BTN_UP  P403.12 */    \
  }

#define HAL_GPIO_SET(dio)                                                      \
  {                                                                            \
    GPIO_writeDio((dio), 1);                                                   \
  }

#define HAL_GPIO_CLR(dio)                                                      \
  {                                                                            \
    GPIO_writeDio((dio), 0);                                                   \
  }

#else // !DEBUG_GPIO

#define HAL_GPIO_INIT()
#define HAL_GPIO_SET(dio)
#define HAL_GPIO_CLR(dio)

#endif // DEBUG_GPIO

/*******************************************************************************
 * CONSTANTS
 */

#define HAL_GPIO_1                IOID_7   // GPIO11 SmartRF06EB LED3    P403.2
#define HAL_GPIO_2                IOID_6   // GPI012 SmartRF06EB LED4    P403.4
#define HAL_GPIO_3                IOID_25  // GPIO24 SmartRF06EB LED1    P404.20
#define HAL_GPIO_4                IOID_27  // GPIO22 SmartRF06EB LED2    P405.4
#define HAL_GPIO_5                IOID_1   // GPIO17 SmartRF06EB NC      P403.6
#define HAL_GPIO_6                IOID_15  // GPIO3  SmartRF06EB BTN_L   P403.8
#define HAL_GPIO_7                IOID_18  // GPIO31 SmartRF06EB BTN_R   P403.10
#define HAL_GPIO_8                IOID_19  // GPIO30 SmartRF06EB BTN_UP  P403.12

/*******************************************************************************
 * TYPEDEFS
 */

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */

#ifdef __cplusplus
}
#endif

#endif /* HAL_GPIO_WRAPPER_H */

