/***************************************************************************//**
 * @file
 * @brief IO Stream LEUART Component.
 * @version x.y.z
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

#ifndef SL_IOSTREAM_LEUART_H
#define SL_IOSTREAM_LEUART_H

#include "sl_status.h"
#include "sl_iostream.h"
#include "sl_iostream_uart.h"

#include "em_cmu.h"
#include "em_gpio.h"
#include "em_leuart.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup iostream
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup iostream_leuart I/O Stream LEUART
 * @brief I/O Stream LEUART
 * @details
 * ## Overview
 *
 *   The Low Energy Universal Asynchronous Receiver/Transmitter controller(s) (LEUART)
 *   can be used as a UART and can, therefore, be connected to an external transceiver
 *   to communicate with another host using the serial link. Multiple instances can be
 *   instantiated as long as they are bound to a unique LEUART peripheral.
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
 *   On transmission side, I/O Stream-LEUART adds a requirement on EM2 until the transmit is
 *   really completed. Once the transmit is completed the requirement is removed and the
 *   system can go to a lower energy mode if allowed.
 *
 *   On the receive side, a requirement on EM2 is added if the application can receive
 *   data asynchronously. Meaning that the system can receive data when the MCU is in
 *   sleep mode (will always be EM1 or EM2 to allow the reception). Otherwise, If the
 *   system doesn't expect to receive anything, no requirement is added and the system
 *   can go to the lowest Energy mode allowed. You can control this behavior using a
 *   configuration per instance, see the following configuration:
 *
 *       SL_IOSTREAM_LEUART_<instance_name>_RESTRICT_ENERGY_MODE_TO_ALLOW_RECEPTION
 * @{
 ******************************************************************************/

// -----------------------------------------------------------------------------
// Data Types

/// @brief Struct representing an I/O Stream LEUART configuration.
typedef struct {
  LEUART_TypeDef *leuart;     ///< Pointer to LEUART peripheral
  IRQn_Type irq_number;       ///< IRQ number
  CMU_Clock_TypeDef clock;    ///< Peripheral Clock
  GPIO_Port_TypeDef tx_port;  ///< Transmit port
  unsigned int tx_pin;        ///< Transmit pin
  GPIO_Port_TypeDef rx_port;  ///< Receive port
  unsigned int rx_pin;        ///< Receive pin
#if defined(LEUART_ROUTEPEN_RXPEN)
  uint8_t tx_location;        ///< LEUART Transmit location. Only available on certain devices (mutually exclusive with location).
  uint8_t rx_location;        ///< LEUART Receive location. Only available on some devices (mutually exclusive with location).
#else
  uint8_t location;           ///< LEUART location. Only available on certain devices (mutually exclusive with rx_location and tx_location).
#endif
} sl_iostream_leuart_config_t;

/// @brief Struct representing an I/O Stream LEUART context.
typedef struct {
  sl_iostream_uart_context_t context; ///< underlying uart context
  LEUART_TypeDef *leuart;             ///< leuart instance handle
  CMU_Clock_TypeDef clock;            ///< Peripheral Clock
  GPIO_Port_TypeDef tx_port;          ///< Transmit port
  unsigned int tx_pin;                ///< Transmit pin
  GPIO_Port_TypeDef rx_port;          ///< Receive port
  unsigned int rx_pin;                ///< Receive pin
} sl_iostream_leuart_context_t;

// -----------------------------------------------------------------------------
// Prototypes

/***************************************************************************//**
 * LEUART Stream init.
 *
 * @param[in] iostream_uart  I/O Stream UART handle.
 *
 * @param[in] uart_config  I/O Stream UART config.
 *
 * @param[in] init  LEUART initialization modes.
 *
 * @param[in] leuart_config   LEUART configuration.
 *
 * @param[in] leuart_context  LEUART Instance context.
 *
 * @return  Status result
 ******************************************************************************/
sl_status_t sl_iostream_leuart_init(sl_iostream_uart_t *iostream_uart,
                                    sl_iostream_uart_config_t *uart_config,
                                    LEUART_Init_TypeDef *init,
                                    sl_iostream_leuart_config_t *leuart_config,
                                    sl_iostream_leuart_context_t *leuart_context);

/*******************************************************************************
 * LEUART interrupt handler.
 *
 * @param[in] stream_context   USART stream context.
 ******************************************************************************/
void sl_iostream_leuart_irq_handler(void *stream_context);

/** @} (end addtogroup iostream_leuart) */
/** @} (end addtogroup iostream) */

#ifdef __cplusplus
}
#endif

#endif // SL_IOSTREAM_LEUART_H
