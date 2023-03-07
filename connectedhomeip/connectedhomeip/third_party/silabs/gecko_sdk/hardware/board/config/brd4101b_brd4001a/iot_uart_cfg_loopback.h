/***************************************************************************//**
 * @file    iot_uart_cfg_inst.h
 * @brief   Common I/O UART instance configuration.
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

/*******************************************************************************
 *                              SAFE GUARD
 ******************************************************************************/

#ifndef _IOT_UART_CFG_LOOPBACK_H_
#define _IOT_UART_CFG_LOOPBACK_H_

/*******************************************************************************
 *                         UART Default Configs
 ******************************************************************************/

// <<< Use Configuration Wizard in Context Menu >>>

// <h>UART General Options

// <o IOT_UART_CFG_LOOPBACK_INST_NUM> Instance number
// <i> Instance number used when iot_uart_open() is called.
// <i> Default: 0
#define IOT_UART_CFG_LOOPBACK_INST_NUM                    0

// <o IOT_UART_CFG_LOOPBACK_DEFAULT_BAUDRATE> Default baud rate
// <i> Default: 115200
#define IOT_UART_CFG_LOOPBACK_DEFAULT_BAUDRATE            115200

// <o IOT_UART_CFG_LOOPBACK_DEFAULT_DATA_BITS> Default number of data bits
// <usartDatabits4=> 4 data bits
// <usartDatabits5=> 5 data bits
// <usartDatabits6=> 6 data bits
// <usartDatabits7=> 7 data bits
// <usartDatabits8=> 8 data bits
// <i> Default: usartDatabits8
#define IOT_UART_CFG_LOOPBACK_DEFAULT_DATA_BITS           usartDatabits8

// <o IOT_UART_CFG_LOOPBACK_DEFAULT_PARITY> Default parity mode
// <usartNoParity=> No Parity
// <usartEvenParity=> Even parity
// <usartOddParity=> Odd parity
// <i> Default: usartNoParity
#define IOT_UART_CFG_LOOPBACK_DEFAULT_PARITY              usartNoParity

// <o IOT_UART_CFG_LOOPBACK_DEFAULT_STOP_BITS> Default number of stop bits
// <usartStopbits0p5=> 0.5 stop bits
// <usartStopbits1=> 1 stop bits
// <usartStopbits1p5=> 1.5 stop bits
// <usartStopbits2=> 2 stop bits
// <i> Default: usartStopbits1
#define IOT_UART_CFG_LOOPBACK_DEFAULT_STOP_BITS           usartStopbits1

// <o IOT_UART_CFG_LOOPBACK_DEFAULT_FLOW_CONTROL_TYPE> Default hardware flow control
// <usartHwFlowControlNone=> None
// <usartHwFlowControlCts=> CTS
// <usartHwFlowControlRts=> RTS
// <usartHwFlowControlCtsAndRts=> CTS/RTS
// <i> Default: usartHwFlowControlNone
#define IOT_UART_CFG_LOOPBACK_DEFAULT_FLOW_CONTROL_TYPE   usartHwFlowControlNone


// <q IOT_UART_CFG_LOOPBACK_LOOPBACK> Internal Loopback
// <i> Enable USART Internal loopback
// Default: 0
#define IOT_UART_CFG_LOOPBACK_LOOPBACK                    1

// </h>

// <<< end of configuration section >>>

/*******************************************************************************
 *                        H/W PERIPHERAL CONFIG
 ******************************************************************************/

// <<< sl:start pin_tool >>>
// <usart signal=TX,RX,(CLK),(CS),(RTS),(CTS)> IOT_UART_CFG_LOOPBACK
// $[USART_IOT_UART_CFG_LOOPBACK]
#define IOT_UART_CFG_LOOPBACK_PERIPHERAL         USART0
#define IOT_UART_CFG_LOOPBACK_PERIPHERAL_NO      0

// USART0 TX on PA0
#define IOT_UART_CFG_LOOPBACK_TX_PORT            gpioPortA
#define IOT_UART_CFG_LOOPBACK_TX_PIN             0
#define IOT_UART_CFG_LOOPBACK_TX_LOC             0

// USART0 RX on PA1
#define IOT_UART_CFG_LOOPBACK_RX_PORT            gpioPortA
#define IOT_UART_CFG_LOOPBACK_RX_PIN             1
#define IOT_UART_CFG_LOOPBACK_RX_LOC             0



// USART0 RTS on PC6
#define IOT_UART_CFG_LOOPBACK_RTS_PORT           gpioPortC
#define IOT_UART_CFG_LOOPBACK_RTS_PIN            6
#define IOT_UART_CFG_LOOPBACK_RTS_LOC            6

// USART0 CTS on PB11
#define IOT_UART_CFG_LOOPBACK_CTS_PORT           gpioPortB
#define IOT_UART_CFG_LOOPBACK_CTS_PIN            11
#define IOT_UART_CFG_LOOPBACK_CTS_LOC            2

// [USART_IOT_UART_CFG_LOOPBACK]$
// <<< sl:end pin_tool >>>

/*******************************************************************************
 *                            SAFE GUARD
 ******************************************************************************/

#endif /* _IOT_UART_CFG_LOOPBACK_H_ */
