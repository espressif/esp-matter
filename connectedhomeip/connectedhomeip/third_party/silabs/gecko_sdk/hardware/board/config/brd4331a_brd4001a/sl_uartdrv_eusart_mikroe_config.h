/***************************************************************************//**
 * @file
 * @brief UARTDRV_EUSART Config
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

#ifndef SL_UARTDRV_EUSART_MIKROE_CONFIG_H
#define SL_UARTDRV_EUSART_MIKROE_CONFIG_H

#include "em_eusart.h"
// <<< Use Configuration Wizard in Context Menu >>>

// <h> EUSART settings
// <o SL_UARTDRV_EUSART_MIKROE_BAUDRATE> Baud rate
// <i> Default: 115200
#define SL_UARTDRV_EUSART_MIKROE_BAUDRATE        115200

// <o SL_UARTDRV_EUSART_MIKROE_LF_MODE> Low frequency mode
// <true=> True
// <false=> False
#define SL_UARTDRV_EUSART_MIKROE_LF_MODE         false

// <o SL_UARTDRV_EUSART_MIKROE_PARITY> Parity mode to use
// <eusartNoParity=> No Parity
// <eusartEvenParity=> Even parity
// <eusartOddParity=> Odd parity
// <i> Default: eusartNoParity
#define SL_UARTDRV_EUSART_MIKROE_PARITY          eusartNoParity

// <o SL_UARTDRV_EUSART_MIKROE_STOP_BITS> Number of stop bits to use.
// <eusartStopbits0p5=> 0.5 stop bits
// <eusartStopbits1=> 1 stop bits
// <eusartStopbits1p5=> 1.5 stop bits
// <eusartStopbits2=> 2 stop bits
// <i> Default: eusartStopbits1
#define SL_UARTDRV_EUSART_MIKROE_STOP_BITS       eusartStopbits1

// <o SL_UARTDRV_EUSART_MIKROE_FLOW_CONTROL_TYPE> Flow control method
// <uartdrvFlowControlNone=> None
// <uartdrvFlowControlSw=> Software XON/XOFF
// <uartdrvFlowControlHw=> nRTS/nCTS hardware handshake
// <uartdrvFlowControlHwUart=> UART peripheral controls nRTS/nCTS
// <i> Default: uartdrvFlowControlHw
#define SL_UARTDRV_EUSART_MIKROE_FLOW_CONTROL_TYPE uartdrvFlowControlNone

// <o SL_UARTDRV_EUSART_MIKROE_OVERSAMPLING> Oversampling selection
// <eusartOVS16=> 16x oversampling
// <eusartOVS8=> 8x oversampling
// <eusartOVS6=> 6x oversampling
// <eusartOVS4=> 4x oversampling
// <eusartOVS0=> Oversampling disabled
// <i> Default: eusartOVS16
#define SL_UARTDRV_EUSART_MIKROE_OVERSAMPLING      eusartOVS16

// <o SL_UARTDRV_EUSART_MIKROE_MVDIS> Majority vote disable for 16x, 8x and 6x oversampling modes
// <eusartMajorityVoteEnable=> False
// <eusartMajorityVoteDisable=> True
// <i> Default: eusartMajorityVoteEnable
#define SL_UARTDRV_EUSART_MIKROE_MVDIS             eusartMajorityVoteEnable

// <o SL_UARTDRV_EUSART_MIKROE_RX_BUFFER_SIZE> Size of the receive operation queue
// <i> Default: 6
#define SL_UARTDRV_EUSART_MIKROE_RX_BUFFER_SIZE  6

// <o SL_UARTDRV_EUSART_MIKROE_TX_BUFFER_SIZE> Size of the transmit operation queue
// <i> Default: 6
#define SL_UARTDRV_EUSART_MIKROE_TX_BUFFER_SIZE 6
// </h>
// <<< end of configuration section >>>

// <<< sl:start pin_tool >>>
// <eusart signal=TX,RX,(CTS),(RTS)> SL_UARTDRV_EUSART_MIKROE
// $[EUSART_SL_UARTDRV_EUSART_MIKROE]
#define SL_UARTDRV_EUSART_MIKROE_PERIPHERAL      EUART0
#define SL_UARTDRV_EUSART_MIKROE_PERIPHERAL_NO   0

// EUART0 TX on PB01
#define SL_UARTDRV_EUSART_MIKROE_TX_PORT         gpioPortB
#define SL_UARTDRV_EUSART_MIKROE_TX_PIN          1

// EUART0 RX on PB02
#define SL_UARTDRV_EUSART_MIKROE_RX_PORT         gpioPortB
#define SL_UARTDRV_EUSART_MIKROE_RX_PIN          2



// [EUSART_SL_UARTDRV_EUSART_MIKROE]$
// <<< sl:end pin_tool >>>
#endif // SL_UARTDRV_EUSART_MIKROE_CONFIG_H
