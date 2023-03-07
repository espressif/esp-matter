/***************************************************************************//**
 * @file
 * @brief IOSTREAM_EUSART Config.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_IOSTREAM_EUSART_EXP_CONFIG_H
#define SL_IOSTREAM_EUSART_EXP_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h>EUART settings

// <q SL_IOSTREAM_EUSART_EXP_ENABLE_HIGH_FREQUENCY> Enable High frequency mode
// <i> Default: 1
#define SL_IOSTREAM_EUSART_EXP_ENABLE_HIGH_FREQUENCY        1

// <o SL_IOSTREAM_EUSART_EXP_BAUDRATE> Baud rate
// <i> Default: 115200
#define SL_IOSTREAM_EUSART_EXP_BAUDRATE              115200

// <o SL_IOSTREAM_EUSART_EXP_PARITY> Parity mode to use
// <eusartNoParity=> No Parity
// <eusartEvenParity=> Even parity
// <eusartOddParity=> Odd parity
// <i> Default: eusartNoParity
#define SL_IOSTREAM_EUSART_EXP_PARITY                eusartNoParity

// <o SL_IOSTREAM_EUSART_EXP_STOP_BITS> Number of stop bits to use.
// <eusartStopbits0p5=> 0.5 stop bits
// <eusartStopbits1=> 1 stop bits
// <eusartStopbits1p5=> 1.5 stop bits
// <eusartStopbits2=> 2 stop bits
// <i> Default: eusartStopbits1
#define SL_IOSTREAM_EUSART_EXP_STOP_BITS             eusartStopbits1

// <o SL_IOSTREAM_EUSART_EXP_FLOW_CONTROL_TYPE> Flow control
// <eusartHwFlowControlNone=> None
// <eusartHwFlowControlCts=> CTS
// <eusartHwFlowControlRts=> RTS
// <eusartHwFlowControlCtsAndRts=> CTS/RTS
// <uartFlowControlSoftware=> Software Flow control (XON/XOFF)
// <i> Default: eusartHwFlowControlNone
#define SL_IOSTREAM_EUSART_EXP_FLOW_CONTROL_TYPE     eusartHwFlowControlNone

// <o SL_IOSTREAM_EUSART_EXP_RX_BUFFER_SIZE> Receive buffer size
// <i> Default: 32
#define SL_IOSTREAM_EUSART_EXP_RX_BUFFER_SIZE    32

// <q SL_IOSTREAM_EUSART_EXP_CONVERT_BY_DEFAULT_LF_TO_CRLF> Convert \n to \r\n
// <i> It can be changed at runtime using the C API.
// <i> Default: 0
#define SL_IOSTREAM_EUSART_EXP_CONVERT_BY_DEFAULT_LF_TO_CRLF     0

// <q SL_IOSTREAM_EUSART_EXP_RESTRICT_ENERGY_MODE_TO_ALLOW_RECEPTION> Restrict the energy mode to allow the reception.
// <i> Default: 1
// <i> Limits the lowest energy mode the system can sleep to in order to keep the reception on. May cause higher power consumption.
#define SL_IOSTREAM_EUSART_EXP_RESTRICT_ENERGY_MODE_TO_ALLOW_RECEPTION    1

// </h>

// <<< end of configuration section >>>

// <<< sl:start pin_tool >>>
// <eusart signal=TX,RX,(CTS),(RTS)> SL_IOSTREAM_EUSART_EXP
// $[EUSART_SL_IOSTREAM_EUSART_EXP]
#define SL_IOSTREAM_EUSART_EXP_PERIPHERAL        EUSART0
#define SL_IOSTREAM_EUSART_EXP_PERIPHERAL_NO     0

// EUSART0 TX on PA05
#define SL_IOSTREAM_EUSART_EXP_TX_PORT           gpioPortA
#define SL_IOSTREAM_EUSART_EXP_TX_PIN            5

// EUSART0 RX on PA06
#define SL_IOSTREAM_EUSART_EXP_RX_PORT           gpioPortA
#define SL_IOSTREAM_EUSART_EXP_RX_PIN            6



// [EUSART_SL_IOSTREAM_EUSART_EXP]$
// <<< sl:end pin_tool >>>

#endif
