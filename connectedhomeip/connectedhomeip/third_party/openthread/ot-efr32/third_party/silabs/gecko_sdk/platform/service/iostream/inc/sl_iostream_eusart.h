/***************************************************************************//**
 * @file
 * @brief IO Stream EUSART Component.
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

#ifndef SL_IOSTREAM_EUSART_H
#define SL_IOSTREAM_EUSART_H

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif
#include "sl_iostream.h"
#include "sl_iostream_uart.h"
#include "sl_status.h"
#include "sl_slist.h"

#include "em_cmu.h"
#include "em_gpio.h"
#include "em_eusart.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup iostream
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup iostream_eusart I/O Stream EUSART
 * @brief I/O Stream EUSART
 * @details
 * ## Overview
 *
 *   The Enhanced Universal Synchronous / Asynchronous Receiver / Transmitter controller(s)
 *   (EUSART) can be used as a UART and can, therefore, be connected to an external transceiver
 *   to communicate with another host using the serial link. Multiple instances can be
 *   instantiated as long as they are bound to a unique EUSART peripheral. The flow control
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
 *   On transmission side, I/O Stream-EUSART adds a requirement on EM1 or EM2, depending on
 *   the frequency mode, until the transmit is really completed. Once the transmit is
 *   completed the requirement is removed and the system can go to a lower energy mode if allowed.
 *
 *   On the receive side, a requirement on EM1 or EM2, depending on the frequency mode, is
 *   added if the application can receive data asynchronously. Meaning that the system can
 *   receive data when the MCU is in sleep mode (EM1 or EM2 to allow the reception). Allowing
 *   the reception in sleep mode will obviously increase the power consumption.
 *   Otherwise, If the system doesn't expect to receive anything, no requirement is added and
 *   the system can go to the lowest Energy mode allowed. You can control this behavior using a
 *   configuration per instance, see the following configuration:
 *
 *       SL_IOSTREAM_EUSART_<instance_name>_RESTRICT_ENERGY_MODE_TO_ALLOW_RECEPTION
 *
 * @{
 ******************************************************************************/
/// @brief Struct representing an I/O Stream EUSART configuration.
typedef struct {
  EUSART_TypeDef *eusart;                       ///< Pointer to EUSART peripheral
  EUSART_HwFlowControl_TypeDef flow_control;    ///< Flow control
  bool enable_high_frequency;                   ///< enable_high_frequency
  CMU_Clock_TypeDef clock;                      ///< Peripheral Clock
#if defined(EUSART_COUNT)
  unsigned int port_index;                      ///< Port index for GPIO routing
#endif
  GPIO_Port_TypeDef tx_port;                    ///< Transmit port
  unsigned int tx_pin;                          ///< Transmit pin
  GPIO_Port_TypeDef rx_port;                    ///< Receive port
  unsigned int rx_pin;                          ///< Receive pin
  GPIO_Port_TypeDef cts_port;                   ///< Flow control, CTS port
  unsigned int cts_pin;                         ///< Flow control, CTS pin
  GPIO_Port_TypeDef rts_port;                   ///< Flow control, RTS port
  unsigned int rts_pin;                         ///< Flow control, RTS pin
} sl_iostream_eusart_config_t;

/// @brief Struct representing an I/O Stream EUSART context.
typedef struct {
  sl_iostream_uart_context_t context;           ///< context
  EUSART_TypeDef *eusart;                       ///< eusart
  CMU_Clock_TypeDef clock;                      ///< Peripheral Clock
  GPIO_Port_TypeDef tx_port;                    ///< Transmit port
  unsigned int tx_pin;                          ///< Transmit pin
  GPIO_Port_TypeDef rx_port;                    ///< Receive port
  unsigned int rx_pin;                          ///< Receive pin
  GPIO_Port_TypeDef cts_port;                   ///< Flow control, CTS port
  unsigned int cts_pin;                         ///< Flow control, CTS pin
  GPIO_Port_TypeDef rts_port;                   ///< Flow control, RTS port
  unsigned int rts_pin;                         ///< Flow control, RTS pin
  uint8_t flags;                                ///< Configuration flags
#if (defined(SL_CATALOG_POWER_MANAGER_PRESENT))
  sl_slist_node_t node;
#endif
} sl_iostream_eusart_context_t;

// -----------------------------------------------------------------------------
// Prototypes

/***************************************************************************//**
 * EUSART Stream init.
 *
 * @param[in] iostream_uart  I/O Stream UART handle.
 *
 * @param[in] uart_config  I/O Stream UART config.
 *
 * @param[in] init  UART initialization modes.
 *
 * @param[in] eusart_config  EUSART configuration.
 *
 * @param[in] eusart_context  EUSART Instance context.
 *
 * @return  Status result
 ******************************************************************************/
sl_status_t sl_iostream_eusart_init(sl_iostream_uart_t *iostream_uart,
                                    sl_iostream_uart_config_t *uart_config,
                                    EUSART_UartInit_TypeDef *init,
                                    sl_iostream_eusart_config_t *eusart_config,
                                    sl_iostream_eusart_context_t *eusart_context);

/*******************************************************************************
 * EUSART interrupt handler.
 *
 * @param[in] stream_context   eusart stream context.
 ******************************************************************************/
void sl_iostream_eusart_irq_handler(void *stream_context);

/** @} (end addtogroup iostream_eusart) */
/** @} (end addtogroup iostream) */

#ifdef __cplusplus
}
#endif

#endif /* SL_IOSTREAM_EUSART_H */
