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
 * one or more of the following defines: RETARGET_USART1,
 * RETARGET_VCOM.
 *
 * This table shows the supported configurations and the resulting serial
 * output device.
 *
 * +----------------------------------------------------------------------+
 * | Defines                            | Serial Output (Locations)       |
 * |----------------------------------------------------------------------+
 * | RETARGET_USART0                    | USART0  (Rx #2, Tx #2)          |
 * | RETARGET_USART1                    | USART1  (Rx #1, Tx #1)          |
 * | RETARGET_LEUART0                   | LEUART0 (Rx #5, Tx #5)          |
 * | RETARGET_VCOM                      | VCOM using USART1               |
 * +----------------------------------------------------------------------+
 *
 * Note that the default configuration is the same as RETARGET_USART1.
 *
 ******************************************************************************/

#if defined(RETARGET_VCOM)
#define RETARGET_USART1    /* Use USART1 by default for VCOM. */
#endif

#if defined(RETARGET_USART1)
  #define RETARGET_IRQ_NAME    USART1_IRQHandler            /* UART IRQ Handler */
  #define RETARGET_CLK         cmuClock_USART1              /* HFPER Clock */
  #define RETARGET_IRQn        USART1_IRQn                  /* IRQ number */
  #define RETARGET_UART        USART1                       /* UART instance */
  #define RETARGET_TX          USART_Tx                     /* Set TX to USART_Tx */
  #define RETARGET_RX          USART_Rx                     /* Set RX to USART_Rx */
  #define RETARGET_TX_LOCATION _USART_ROUTELOC0_TXLOC_LOC1  /* Location of of USART TX pin */
  #define RETARGET_RX_LOCATION _USART_ROUTELOC0_RXLOC_LOC1  /* Location of of USART RX pin */
  #define RETARGET_TXPORT      gpioPortD                    /* UART transmission port */
  #define RETARGET_TXPIN       0                            /* UART transmission pin */
  #define RETARGET_RXPORT      gpioPortD                    /* UART reception port */
  #define RETARGET_RXPIN       1                            /* UART reception pin */
  #define RETARGET_USART       1                            /* Includes em_usart.h */

#elif defined(RETARGET_USART0)
  #define RETARGET_IRQ_NAME    USART0_IRQHandler            /* UART IRQ Handler */
  #define RETARGET_CLK         cmuClock_USART0              /* HFPER Clock */
  #define RETARGET_IRQn        USART0_IRQn                  /* IRQ number */
  #define RETARGET_UART        USART0                       /* UART instance */
  #define RETARGET_TX          USART_Tx                     /* Set TX to USART_Tx */
  #define RETARGET_RX          USART_Rx                     /* Set RX to USART_Rx */
  #define RETARGET_TX_LOCATION _USART_ROUTELOC0_TXLOC_LOC2  /* Location of of USART TX pin */
  #define RETARGET_RX_LOCATION _USART_ROUTELOC0_RXLOC_LOC2  /* Location of of USART RX pin */
  #define RETARGET_TXPORT      gpioPortC                    /* UART transmission port */
  #define RETARGET_TXPIN       11                           /* UART transmission pin */
  #define RETARGET_RXPORT      gpioPortC                    /* UART reception port */
  #define RETARGET_RXPIN       10                           /* UART reception pin */
  #define RETARGET_USART       1                            /* Includes em_usart.h */

#elif defined(RETARGET_LEUART0)
  #define RETARGET_IRQ_NAME    LEUART0_IRQHandler            /* UART IRQ Handler */
  #define RETARGET_CLK         cmuClock_LEUART0              /* HFPER Clock */
  #define RETARGET_IRQn        LEUART0_IRQn                  /* IRQ number */
  #define RETARGET_UART        LEUART0                       /* UART instance */
  #define RETARGET_TX          LEUART_Tx                     /* Set TX to USART_Tx */
  #define RETARGET_RX          LEUART_Rx                     /* Set RX to USART_Rx */
  #define RETARGET_TX_LOCATION _LEUART_ROUTELOC0_TXLOC_LOC5  /* Location of of USART TX pin */
  #define RETARGET_RX_LOCATION _LEUART_ROUTELOC0_RXLOC_LOC5  /* Location of of USART RX pin */
  #define RETARGET_TXPORT      gpioPortC                    /* UART transmission port */
  #define RETARGET_TXPIN       14                            /* UART transmission pin */
  #define RETARGET_RXPORT      gpioPortC                    /* UART reception port */
  #define RETARGET_RXPIN       15                            /* UART reception pin */
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
#else
  #define RETARGET_PERIPHERAL_ENABLE()
#endif

#endif
