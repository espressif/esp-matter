/***************************************************************************//**
 * @file
 * @brief CPC UART PRIMARY driver configuration file.
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

// <<< Use Configuration Wizard in Context Menu >>>

#ifndef SL_CPC_DRV_UART_USART_VCOM_PRIMARY_CONFIG_H
#define SL_CPC_DRV_UART_USART_VCOM_PRIMARY_CONFIG_H

// <h> CPC-Primary UART Driver Configuration

// <o SL_CPC_DRV_UART_VCOM_RX_QUEUE_SIZE> Number of frame that can be queued in the driver receive queue
// <i> Default: 10
#define SL_CPC_DRV_UART_VCOM_RX_QUEUE_SIZE            10

// <o SL_CPC_DRV_UART_VCOM_TX_QUEUE_SIZE> Number of frame that can be queued in the driver transmit queue
// <i> Default: 10
#define SL_CPC_DRV_UART_VCOM_TX_QUEUE_SIZE            10

// <o SL_CPC_DRV_UART_VCOM_BAUDRATE> UART Baudrate
// <i> Default: 115200
#define SL_CPC_DRV_UART_VCOM_BAUDRATE              115200
// </h>

// <<< end of configuration section >>>

// <<< sl:start pin_tool >>>
// <usart signal=TX,RX> SL_CPC_DRV_UART_VCOM
// $[USART_SL_CPC_DRV_UART_VCOM]
#define SL_CPC_DRV_UART_VCOM_PERIPHERAL          USART0
#define SL_CPC_DRV_UART_VCOM_PERIPHERAL_NO       0

// USART0 TX on PD02
#define SL_CPC_DRV_UART_VCOM_TX_PORT             gpioPortD
#define SL_CPC_DRV_UART_VCOM_TX_PIN              2

// USART0 RX on PD03
#define SL_CPC_DRV_UART_VCOM_RX_PORT             gpioPortD
#define SL_CPC_DRV_UART_VCOM_RX_PIN              3

// [USART_SL_CPC_DRV_UART_VCOM]$
// <<< sl:end pin_tool >>>

#endif /* SL_CPC_DRV_UART_VCOM_PRIMARY_CONFIG_H */
