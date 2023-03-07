/***************************************************************************//**
 * @file
 * @brief Provide stdio retargeting configuration parameters
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

#ifndef RETARGETSERIALCONFIG_H
#define RETARGETSERIALCONFIG_H

/* Override if needed with commandline parameter -DRETARGET_xxx */

#if !defined(RETARGET_USART0) && !defined(RETARGET_VCOM)
#define RETARGET_USART0    /* Use USART0 by default. */
#endif

#if defined(RETARGET_USART0) || defined(RETARGET_VCOM)
  #define RETARGET_IRQ_NAME    USART0_RX_IRQHandler         /* UART IRQ Handler            */
  #define RETARGET_CLK         cmuClock_USART0              /* HFPER Clock                 */
  #define RETARGET_IRQn        USART0_RX_IRQn               /* IRQ number                  */
  #define RETARGET_UART        USART0                       /* UART instance               */
  #define RETARGET_TX          USART_Tx                     /* Set TX to USART_Tx          */
  #define RETARGET_RX          USART_Rx                     /* Set RX to USART_Rx          */

  #define RETARGET_TX_LOCATION 0                            /* Location of of USART TX pin */
  #define RETARGET_RX_LOCATION 0                            /* Location of of USART RX pin */

  #define RETARGET_TXPORT      gpioPortA                    /* UART transmission port      */
  #define RETARGET_TXPIN       0                            /* UART transmission pin       */
  #define RETARGET_RXPORT      gpioPortA                    /* UART reception port         */
  #define RETARGET_RXPIN       1                            /* UART reception pin          */
  #define RETARGET_USART       1                            /* Includes em_usart.h         */

  #define RETARGET_PERIPHERAL_ENABLE()

#else
#error "Illegal USART selection."
#endif

#endif // RETARGETSERIALCONFIG_H
