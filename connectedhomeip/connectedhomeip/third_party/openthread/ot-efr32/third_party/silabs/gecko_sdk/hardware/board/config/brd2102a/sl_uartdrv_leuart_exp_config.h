/***************************************************************************//**
 * @file
 * @brief UARTDRV_LEUART Config
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef SL_UARTDRV_LEUART_EXP_CONFIG_H
#define SL_UARTDRV_LEUART_EXP_CONFIG_H

#include "em_leuart.h"
// <<< Use Configuration Wizard in Context Menu >>>

// <h> UART settings
// <o SL_UARTDRV_LEUART_EXP_BAUDRATE> Baud rate
// <i> Default: 115200
#define SL_UARTDRV_LEUART_EXP_BAUDRATE        115200

// <o SL_UARTDRV_LEUART_EXP_PARITY> Parity mode to use
// <leuartNoParity=> No Parity
// <leuartEvenParity=> Even parity
// <leuartOddParity=> Odd parity
// <i> Default: leuartNoParity
#define SL_UARTDRV_LEUART_EXP_PARITY          leuartNoParity

// <o SL_UARTDRV_LEUART_EXP_STOP_BITS> Number of stop bits to use.
// <leuartStopbits1=> 1 stop bits
// <leuartStopbits2=> 2 stop bits
// <i> Default: leuartStopbits1
#define SL_UARTDRV_LEUART_EXP_STOP_BITS       leuartStopbits1

// <o SL_UARTDRV_LEUART_EXP_FLOW_CONTROL_TYPE> Flow control method
// <uartdrvFlowControlNone=> None
// <uartdrvFlowControlSw=> Software XON/XOFF
// <uartdrvFlowControlHw=> nRTS/nCTS hardware handshake
// <uartdrvFlowControlHwUart=> UART peripheral controls nRTS/nCTS
// <i> Default: uartdrvFlowControlHw
#define SL_UARTDRV_LEUART_EXP_FLOW_CONTROL_TYPE uartdrvFlowControlNone

// <o SL_UARTDRV_LEUART_EXP_RX_BUFFER_SIZE> Size of the receive operation queue
// <i> Default: 6
#define SL_UARTDRV_LEUART_EXP_RX_BUFFER_SIZE 6

// <o SL_UARTDRV_LEUART_EXP_TX_BUFFER_SIZE> Size of the transmit operation queue
// <i> Default: 6
#define SL_UARTDRV_LEUART_EXP_TX_BUFFER_SIZE 6
// </h>
// <<< end of configuration section >>>

// <<< sl:start pin_tool >>>
// <leuart signal=TX,RX> SL_UARTDRV_LEUART_EXP
// $[LEUART_SL_UARTDRV_LEUART_EXP]
#define SL_UARTDRV_LEUART_EXP_PERIPHERAL         LEUART0
#define SL_UARTDRV_LEUART_EXP_PERIPHERAL_NO      0

// LEUART0 TX on PC14
#define SL_UARTDRV_LEUART_EXP_TX_PORT            gpioPortC
#define SL_UARTDRV_LEUART_EXP_TX_PIN             14
#define SL_UARTDRV_LEUART_EXP_TX_LOC             5

// LEUART0 RX on PC15
#define SL_UARTDRV_LEUART_EXP_RX_PORT            gpioPortC
#define SL_UARTDRV_LEUART_EXP_RX_PIN             15
#define SL_UARTDRV_LEUART_EXP_RX_LOC             5

// [LEUART_SL_UARTDRV_LEUART_EXP]$
// <gpio> SL_UARTDRV_LEUART_EXP_CTS
// $[GPIO_SL_UARTDRV_LEUART_EXP_CTS]

// [GPIO_SL_UARTDRV_LEUART_EXP_CTS]$

// <gpio> SL_UARTDRV_LEUART_EXP_RTS
// $[GPIO_SL_UARTDRV_LEUART_EXP_RTS]

// [GPIO_SL_UARTDRV_LEUART_EXP_RTS]$
// <<< sl:end pin_tool >>>
#endif // SL_UARTDRV_LEUART_EXP_CONFIG_H
