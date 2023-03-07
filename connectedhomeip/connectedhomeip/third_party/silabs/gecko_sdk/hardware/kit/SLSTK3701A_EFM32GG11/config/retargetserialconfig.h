/***************************************************************************//**
 * @file
 * @brief Provide stdio retargeting configuration parameters.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#ifndef __SILICON_LABS_RETARGETSERIALCONFIG_H__
#define __SILICON_LABS_RETARGETSERIALCONFIG_H__

#include "bsp.h"

/***************************************************************************//**
 *
 * When retargeting serial output the user can choose which peripheral
 * to use as the serial output device. This choice is made by configuring
 * one or more of the following defines: RETARGET_USART4, RETARGET_LEUART0,
 * RETARGET_VCOM.
 *
 * This table shows the supported configurations and the resulting serial
 * output device.
 *
 * +----------------------------------------------------------------------+
 * | Defines                            | Serial Output (Locations)       |
 * |----------------------------------------------------------------------+
 * | None                               | USART4  (Rx #4, Tx #4)          |
 * | RETARGET_USART4                    | USART4  (Rx #4, Tx #4)          |
 * | RETARGET_LEUART0                   | LEUART0 (Rx #2, Tx #2)          |
 * | RETARGET_VCOM                      | VCOM using USART4               |
 * | RETARGET_USART4 and RETARGET_VCOM  | VCOM using USART4               |
 * | RETARGET_LEUART0 and RETARGET_VCOM | Not supported by EFM32GG11.     |
 * +----------------------------------------------------------------------+
 *
 * Note that the default configuration is the same as RETARGET_USART4.
 *
 ******************************************************************************/

#if !defined(RETARGET_USART4)  \
  && !defined(RETARGET_USART5) \
  && !defined(RETARGET_LEUART0)
#define RETARGET_USART4    /* Use USART4 by default. */
#endif

#if defined(RETARGET_USART4)
  #define RETARGET_IRQ_NAME    USART4_RX_IRQHandler         /* UART IRQ Handler */
  #define RETARGET_CLK         cmuClock_USART4              /* HFPER Clock */
  #define RETARGET_IRQn        USART4_RX_IRQn               /* IRQ number */
  #define RETARGET_UART        USART4                       /* UART instance */
  #define RETARGET_TX          USART_Tx                     /* Set TX to USART_Tx */
  #define RETARGET_RX          USART_Rx                     /* Set RX to USART_Rx */
  #define RETARGET_TX_LOCATION _USART_ROUTELOC0_TXLOC_LOC4  /* Location of of USART TX pin */
  #define RETARGET_RX_LOCATION _USART_ROUTELOC0_RXLOC_LOC4  /* Location of of USART RX pin */
  #define RETARGET_TXPORT      gpioPortH                    /* UART transmission port */
  #define RETARGET_TXPIN       4                            /* UART transmission pin */
  #define RETARGET_RXPORT      gpioPortH                    /* UART reception port */
  #define RETARGET_RXPIN       5                            /* UART reception pin */
  #define RETARGET_USART       1                            /* Includes em_usart.h */

#elif defined(RETARGET_USART5)
  #define RETARGET_IRQ_NAME    USART5_RX_IRQHandler         /* UART IRQ Handler */
  #define RETARGET_CLK         cmuClock_USART5              /* HFPER Clock */
  #define RETARGET_IRQn        USART5_RX_IRQn               /* IRQ number */
  #define RETARGET_UART        USART5                       /* UART instance */
  #define RETARGET_TX          USART_Tx                     /* Set TX to USART_Tx */
  #define RETARGET_RX          USART_Rx                     /* Set RX to USART_Rx */
  #define RETARGET_TX_LOCATION _USART_ROUTELOC0_TXLOC_LOC0  /* Location of of USART TX pin */
  #define RETARGET_RX_LOCATION _USART_ROUTELOC0_RXLOC_LOC0  /* Location of of USART RX pin */
  #define RETARGET_TXPORT      gpioPortE                    /* UART transmission port */
  #define RETARGET_TXPIN       8                            /* UART transmission pin */
  #define RETARGET_RXPORT      gpioPortE                    /* UART reception port */
  #define RETARGET_RXPIN       9                            /* UART reception pin */
  #define RETARGET_USART       1                            /* Includes em_usart.h */

#elif defined(RETARGET_LEUART0)
  #define RETARGET_IRQ_NAME    LEUART0_IRQHandler           /* LEUART IRQ Handler */
  #define RETARGET_CLK         cmuClock_LEUART0             /* HFPER Clock */
  #define RETARGET_IRQn        LEUART0_IRQn                 /* IRQ number */
  #define RETARGET_UART        LEUART0                      /* LEUART instance */
  #define RETARGET_TX          LEUART_Tx                    /* Set TX to LEUART_Tx */
  #define RETARGET_RX          LEUART_Rx                    /* Set RX to LEUART_Rx */
  #define RETARGET_TX_LOCATION _LEUART_ROUTELOC0_TXLOC_LOC5 /* Location of of LEUART TX pin */
  #define RETARGET_RX_LOCATION _LEUART_ROUTELOC0_RXLOC_LOC5 /* Location of of LEUART RX pin */
  #define RETARGET_TXPORT      gpioPortC                    /* LEUART transmission port */
  #define RETARGET_TXPIN       14                           /* LEUART transmission pin */
  #define RETARGET_RXPORT      gpioPortC                    /* LEUART reception port */
  #define RETARGET_RXPIN       15                           /* LEUART reception pin */
  #define RETARGET_LEUART      1                            /* Includes em_leuart.h */

#else
#error "Illegal USART selection."
#endif

#if defined(RETARGET_VCOM)
  #define RETARGET_PERIPHERAL_ENABLE() \
  GPIO_PinModeSet(BSP_BCC_ENABLE_PORT, \
                  BSP_BCC_ENABLE_PIN,  \
                  gpioModePushPull,    \
                  1);

// Expect use of USART4 if RETARGET_VCOM is defined.
#if !defined(RETARGET_USART4)
#error "Unsupported serial port for VCOM."
#endif
#else
  #define RETARGET_PERIPHERAL_ENABLE()
#endif

#endif
