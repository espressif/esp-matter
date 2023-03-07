/***************************************************************************//**
 * @file
 * @brief IO Stream USART Component.
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_IOSTREAM_USART_H
#define SL_IOSTREAM_USART_H

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif
#include "sl_iostream.h"
#include "sl_iostream_uart.h"
#include "sl_status.h"

#include "em_cmu.h"
#include "em_gpio.h"
#include "em_usart.h"

#if (defined(SL_CATALOG_KERNEL_PRESENT))
#include "cmsis_os2.h"
#include "sl_cmsis_os2_common.h"
#endif

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
#include "sl_power_manager.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup iostream
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup iostream_usart I/O Stream USART
 * @brief I/O Stream USART
 * @details
 * ## Overview
 *
 *   The Universal Synchronous / Asynchronous Receiver / Transmitter controller(s) (USART)
 *   can be used as a UART and can, therefore, be connected to an external transceiver
 *   to communicate with another host using the serial link. Multiple instances can be
 *   instantiated as long as they are bound to a unique USART peripheral. The flow control
 *   is supported as well.
 *
 * ## Initialization
 *
 *   The stream sets itself as the default stream at the end of the initialization
 *   function.You must reconfigure the default interface if you have multiple streams
 *   in your project else the last stream initialized will be set as the system default
 *   stream.
 *
 * ## Power manager integration
 *
 *   On transmission side, I/O Stream-USART adds a requirement on EM1 until the transmit is
 *   really completed. Once the transmit is completed the requirement is removed and the
 *   system can go to a lower energy mode if allowed.
 *
 *   On the receive side, a requirement on EM1 is added if the application can receive
 *   data asynchronously. Meaning that the system can receive data when the MCU is into
 *   a sleep mode(will always be EM1 to allow the reception). Otherwise, If the system
 *   doesn't expect to receive anything, no requirement is added and the system can go
 *   to the lowest Energy mode allowed. You can control this behavior using a
 *   configuration per instance, see the following configuration:
 *
 *       SL_IOSTREAM_USART_<instance_name>_RESTRICT_ENERGY_MODE_TO_ALLOW_RECEPTION
 *
 * @{
 ******************************************************************************/

// -----------------------------------------------------------------------------
// Data Types

/// @brief I/O Stream USART config
typedef struct {
  USART_TypeDef *usart;       ///< Pointer to USART peripheral
  CMU_Clock_TypeDef clock;    ///< Peripheral Clock
  GPIO_Port_TypeDef tx_port;  ///< Transmit port
  uint8_t tx_pin;        ///< Transmit pin
  GPIO_Port_TypeDef rx_port;  ///< Receive port
  uint8_t rx_pin;        ///< Receive pin
  GPIO_Port_TypeDef cts_port; ///< Flow control, CTS port
  uint8_t  cts_pin;       ///< Flow control, CTS pin
  GPIO_Port_TypeDef rts_port; ///< Flow control, RTS port
  uint8_t rts_pin;       ///< Flow control, RTS pin
#if defined(GPIO_USART_ROUTEEN_TXPEN)
  uint8_t usart_index;        ///< Usart index. Available only on certain devices.
#elif defined(USART_ROUTEPEN_RXPEN)
  uint8_t usart_tx_location;  ///< USART Transmit location. Available only on certain devices.
  uint8_t usart_rx_location;  ///< USART Receive location. Available only on certain devices.
  uint8_t usart_cts_location; ///< USART CTS location. Available only on certain devices.
  uint8_t usart_rts_location; ///< USART RTS location. Available only on certain devices.
#else
  uint8_t usart_location;     ///< USART location. Available only on certain devices.
#endif
} sl_iostream_usart_config_t;

/// @brief I/O Stream USART context
typedef struct {
  sl_iostream_uart_context_t context; ///< usart_location
  USART_TypeDef *usart;       ///< usart
  CMU_Clock_TypeDef clock;    ///< Peripheral Clock
  GPIO_Port_TypeDef tx_port;  ///< Transmit port
  uint8_t tx_pin;             ///< Transmit pin
  GPIO_Port_TypeDef rx_port;  ///< Receive port
  uint8_t rx_pin;             ///< Receive pin
#if (_SILICON_LABS_32B_SERIES > 0)
  GPIO_Port_TypeDef cts_port; ///< Flow control, CTS port
  uint8_t cts_pin;            ///< Flow control, CTS pin
  GPIO_Port_TypeDef rts_port; ///< Flow control, RTS port
  uint8_t rts_pin;            ///< Flow control, RTS pin
  uint8_t flags;
#endif
} sl_iostream_usart_context_t;

// -----------------------------------------------------------------------------
// Prototypes

/***************************************************************************//**
 * USART Stream init.
 *
 * @param[in] iostream_uart  I/O Stream UART handle.
 *
 * @param[in] uart_config  I/O Stream UART config.
 *
 * @param[in] init  USART initialization modes.
 *
 * @param[in] usart_config  USART configuration.
 *
 * @param[in] usart_context  USART Instance context.
 *
 * @return  Status result
 ******************************************************************************/
sl_status_t sl_iostream_usart_init(sl_iostream_uart_t *iostream_uart,
                                   sl_iostream_uart_config_t *uart_config,
                                   USART_InitAsync_TypeDef *init,
                                   sl_iostream_usart_config_t *usart_config,
                                   sl_iostream_usart_context_t *usart_context);

/*******************************************************************************
 * Usart interrupt handler.
 *
 * @param[in] stream_context   Usart stream context.
 ******************************************************************************/
void sl_iostream_usart_irq_handler(void *stream_context);

/** @} (end addtogroup iostream_usart) */
/** @} (end addtogroup iostream) */

#ifdef __cplusplus
}
#endif

#endif // SL_IOSTREAM_USART_H
