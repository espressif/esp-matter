/******************************************************************************

 @file  npi_config.h

 @brief This file contains the Network Processor Interface (NPI), which
        abstracts the physical link between the Application
        Processor (AP) and the Network Processor (NP). The NPI serves as the
        HAL's client for the SPI and UART drivers, and provides
        API and callback services for its client.

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2015-2022, Texas Instruments Incorporated
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
#ifndef NPI_CONFIG_H
#define NPI_CONFIG_H

#ifdef __cplusplus
extern "C"
{
#endif

// ****************************************************************************
// includes
// ****************************************************************************

#include <ti_drivers_config.h>

// ****************************************************************************
// defines
// ****************************************************************************

#if !defined(NPI_SPI_CONFIG)
#define NPI_SPI_CONFIG CONFIG_SPI_1
#endif

#ifndef NPI_FLOW_CTRL
  #ifdef POWER_SAVING
    #define NPI_FLOW_CTRL       1
  #else
    #define NPI_FLOW_CTRL       0
  #endif
#elif !(NPI_FLOW_CTRL == 0) && !(NPI_FLOW_CTRL == 1)
  #error "NPI ERROR: NPI_FLOW_CTRL can only be assigned 0 (disabled) or 1 (enabled)"
#endif

#if (NPI_FLOW_CTRL == 1)
  #ifdef NPI_USE_SPI
    //NOTE: Board_SPI1 SPI module must be used for NPI.
    #define MRDY_PIN CONFIG_GPIO_BTN1
    #define SRDY_PIN CONFIG_GPIO_BTN2
  #elif defined(NPI_USE_UART)
    #define MRDY_PIN CONFIG_GPIO_BTN1
    #define SRDY_PIN CONFIG_GPIO_BTN2
  #endif
  #define SRDY_ENABLE()                   PIN_setOutputValue(hNpiHandshakePins, SRDY_PIN, 0) /* RTS low */
  #define SRDY_DISABLE()                  PIN_setOutputValue(hNpiHandshakePins, SRDY_PIN, 1) /* RTS high */
#else
  #define SRDY_ENABLE()
  #define SRDY_DISABLE()
#endif // NPI_FLOW_CTRL = 1


#if defined( CC26X2 ) || defined( CC13X2 ) || defined( CC13X2P )
  #ifndef NPI_TL_BUF_SIZE
    #define NPI_TL_BUF_SIZE         1680
  #endif
#else // CC26XX_R2 || CC1350LP_7XD ...
  #ifndef NPI_TL_BUF_SIZE
    #define NPI_TL_BUF_SIZE         330
  #endif
#endif // CC26X2 || CC13X2


#define NPI_SPI_PAYLOAD_SIZE    255
#define NPI_SPI_HDR_LEN         4

#ifdef NPI_USE_SPI
  #if (NPI_TL_BUF_SIZE - NPI_SPI_HDR_LEN) < NPI_SPI_PAYLOAD_SIZE
    #define NPI_MAX_FRAG_SIZE       (NPI_TL_BUF_SIZE - NPI_SPI_HDR_LEN)
  #else
    #define NPI_MAX_FRAG_SIZE       NPI_SPI_PAYLOAD_SIZE
  #endif
#elif NPI_USE_UART
  #define NPI_MAX_FRAG_SIZE       NPI_TL_BUF_SIZE
#else
  #error "NPI ERROR: NPI_USE_UART or NPI_USE_SPI must be defined."
#endif

// ****************************************************************************
// typedefs
// ****************************************************************************

//*****************************************************************************
// globals
//*****************************************************************************

//*****************************************************************************
// function prototypes
//*****************************************************************************

#ifdef __cplusplus
}
#endif

#endif /* NPI_CONFIG_H */
