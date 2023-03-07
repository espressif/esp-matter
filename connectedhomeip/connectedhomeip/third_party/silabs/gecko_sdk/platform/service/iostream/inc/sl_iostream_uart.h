/***************************************************************************//**
 * @file
 * @brief IO Stream UART Component.
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

#ifndef SL_IOSTREAM_UART_TYPE_H
#define SL_IOSTREAM_UART_TYPE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif

#include "em_device.h"
#include "sl_status.h"
#include "sl_iostream.h"

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
#include "sl_power_manager.h"
#endif

#if (defined(SL_CATALOG_KERNEL_PRESENT))
#include "cmsis_os2.h"
#include "sl_cmsis_os2_common.h"
#endif

/***************************************************************************//**
 * @addtogroup iostream
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup iostream_uart I/O Stream UART
 * @brief I/O Stream UART
 * @details
 * ## Overview
 *
 *   UART layer provides a set of standard APIs that can be used with all type of
 *   UART interface.
 *
 * ## Initialization
 *
 *   Each UART stream type provides its initalization with parameters specific to them.
 * @{
 ******************************************************************************/

// -----------------------------------------------------------------------------
// Data Types

#define uartFlowControlNone  0
#define uartFlowControlSoftware    0xFFFF

/// @brief I/O Stream UART stream object
typedef struct {
  sl_iostream_t stream;                                               ///< stream
  sl_status_t (*deinit)(void *stream);                                ///< uart deinit
  void (*set_auto_cr_lf)(void *context, bool on);                     ///< set_auto_cr_lf
  bool (*get_auto_cr_lf)(void *context);                              ///< get_auto_cr_lf
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
  void (*set_rx_energy_mode_restriction)(void *context, bool on);     ///< set_rx_energy_mode_restriction. Available only when Power Manager present.
  bool (*get_rx_energy_mode_restriction)(void *context);              ///< get_rx_energy_mode_restriction. Available only when Power Manager present.
#if !defined(SL_CATALOG_KERNEL_PRESENT) || defined(DOXYGEN)
  sl_power_manager_on_isr_exit_t (*sleep_on_isr_exit)(void *context); ///< sleep_on_isr_exit. Available only when Power Manager present and kernel not present.
#endif
#endif
#if defined(SL_CATALOG_KERNEL_PRESENT)
  void (*set_read_block)(void *context, bool on);                     ///< set_read_block. Available only when kernel present.
  bool (*get_read_block)(void *context);                              ///< get_read_block. Available only when kernel present.
#endif
} sl_iostream_uart_t;

/// @brief I/O Stream UART config
typedef struct {
  IRQn_Type rx_irq_number;  ///< rx_irq_number
  IRQn_Type tx_irq_number;  ///< tx_irq_number
  uint8_t *rx_buffer;       ///< rx_buffer
  size_t rx_buffer_length;  ///< rx_buffer_length
  bool lf_to_crlf;          ///< lf_to_crlf
  bool rx_when_sleeping;    ///< rx_when_sleeping
  bool sw_flow_control;     ///< sw_flow_control
} sl_iostream_uart_config_t;

/// @brief I/O Stream UART context
typedef struct {
  sl_status_t (*tx)(void *context, char c); ///< Tx function pointer
  void (*tx_completed)(void *context, bool enable); ///< Pointer to a function handling the Tx Completed event
  void (*enable_rx)(void *context);         ///< Pointer to a function determining whether rx is enabled
  sl_status_t (*deinit)(void *context);     ///< DeInit function pointer
  uint32_t rx_read_index;                   ///< Index in buffer to be read
  uint32_t rx_write_index;                  ///< Index in buffer to be written to
  uint32_t rx_count;                        ///< Keeps track of how much data which are stored in the buffer
  uint8_t *rx_buffer;                       ///< buffer to store data
  size_t rx_buffer_length;                  ///< rx_buffer_length
  bool lf_to_crlf;                          ///< lf_to_crlf
  bool sw_flow_control;                     ///< software flow control
  bool xon;                                 ///< Transmitter enabled
  bool remote_xon;                          ///< Remote Transmitter enabled
  IRQn_Type rx_irq_number;                  ///< Receive IRQ Number
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
  IRQn_Type tx_irq_number;                  ///< Transmit IRQ Number
  volatile bool tx_idle;                    ///< tx_idle. Available only when Power Manager present.
  bool em_req_added;                        ///< em_req_added. Available only when Power Manager present.
  sl_power_manager_em_t rx_em;              ///< rx_em. Available only when Power Manager present.
  sl_power_manager_em_t tx_em;              ///< tx_em. Available only when Power Manager present.
#endif
#if defined(SL_CATALOG_KERNEL_PRESENT)
  bool block;                                  ///< block. Available only when kernel present.
  osMutexId_t read_lock;                       ///< read_lock. Available only when kernel present.
  __ALIGNED(4) uint8_t read_lock_cb[osMutexCbSize];         ///< read_lock control block. Available only when kernel present.
  osSemaphoreId_t read_signal;                 ///< read_signal. Available only when kernel present.
  __ALIGNED(4) uint8_t read_signal_cb[osSemaphoreCbSize];   ///< read_signal control block. Available only when kernel present.
  osMutexId_t write_lock;                      ///< write_lock. Available only when kernel present.
  __ALIGNED(4) uint8_t write_lock_cb[osMutexCbSize];        ///< write_lock control block. Available only when kernel present.
#elif defined(SL_CATALOG_POWER_MANAGER_PRESENT) || defined(DOXYGEN)
  sl_power_manager_on_isr_exit_t sleep;     ///< sleep. Available only when kernel not present and Power Manager present.
#endif
} sl_iostream_uart_context_t;

// -----------------------------------------------------------------------------
// Prototypes

/***************************************************************************//**
 * UART Stream De-init.
 *
 * @param[in] iostream_uart  UART context.
 *
 * @return Status result
 ******************************************************************************/
__STATIC_INLINE sl_status_t sl_iostream_uart_deinit(sl_iostream_uart_t *iostream_uart)
{
  return iostream_uart->deinit(iostream_uart);
}

/***************************************************************************//**
 * Configure Automatic line conversion.
 *
 * @param[in] iostream_uart  UART context.
 *
 * @param[in] on  If true, automatic LF to CRLF conversion will be enabled.
 ******************************************************************************/
__STATIC_INLINE void sl_iostream_uart_set_auto_cr_lf(sl_iostream_uart_t *iostream_uart,
                                                     bool on)
{
  iostream_uart->set_auto_cr_lf(iostream_uart->stream.context, on);
}

/***************************************************************************//**
 * Get Automatic line conversion.
 *
 * @param[in] iostream_uart   UART context.
 *
 * @return Auto-conversion mode.
 ******************************************************************************/
__STATIC_INLINE bool sl_iostream_uart_get_auto_cr_lf(sl_iostream_uart_t *iostream_uart)
{
  return iostream_uart->get_auto_cr_lf(iostream_uart->stream.context);
}

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
/***************************************************************************//**
 * Add or remove energy mode restriction to enable/disable reception when the
 * system goes to sleep.
 *
 * @param[in] iostream_uart  UART context.
 *
 * @param[in] on  If true, will be able to receive data when sleeping. i.e it
 *                affects the lowest power level that the system can go.
 *                Otherwise, it might not be possible to receive data when sleeping.
 ******************************************************************************/
__STATIC_INLINE void sl_iostream_uart_set_rx_energy_mode_restriction(sl_iostream_uart_t *iostream_uart,
                                                                     bool on)
{
  iostream_uart->set_rx_energy_mode_restriction(iostream_uart->stream.context, on);
}

/***************************************************************************//**
 * Get reception energy mode restriction configuration.
 *
 * @param[in] iostream_uart  UART context.
 *
 * @return Sleep configuration.
 ******************************************************************************/
__STATIC_INLINE bool sl_iostream_uart_get_rx_energy_mode_restriction(sl_iostream_uart_t *iostream_uart)
{
  return iostream_uart->get_rx_energy_mode_restriction(iostream_uart->stream.context);
}
#endif

#if defined(SL_CATALOG_KERNEL_PRESENT)
/***************************************************************************//**
 * Configure Read blocking mode.
 *
 * @param[in] iostream_uart  UART context.
 *
 * @param[in] on  If false, the read API will be non-blocking. Otherwise the
 *                read API will block until data is received.
 ******************************************************************************/
__STATIC_INLINE void sl_iostream_uart_set_read_block(sl_iostream_uart_t *iostream_uart,
                                                     bool on)
{
  iostream_uart->set_read_block(iostream_uart->stream.context, on);
}

/***************************************************************************//**
 * Get read API block configuration.
 *
 * @param[in] iostream_uart  UART context.
 *
 * @return Block mode.
 ******************************************************************************/
__STATIC_INLINE bool sl_iostream_uart_get_read_block(sl_iostream_uart_t *iostream_uart)
{
  return iostream_uart->get_read_block(iostream_uart->stream.context);
}
#endif

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT) && !defined(SL_CATALOG_KERNEL_PRESENT)
/***************************************************************************//**
 * Get the UART status on isr exit (sleep, wakeup or ignore).
 *
 * @param[in] iostream_uart  UART context.
 *
 * @return Sleep status:
 *            SL_POWER_MANAGER_IGNORE; UART has not generated the ISR
 *            SL_POWER_MANAGER_WAKEUP; UART generated the ISR and the system must wakeup
 *            SL_POWER_MANAGER_SLEEP; UART generated the ISR and the system can go back to sleep.
 ******************************************************************************/
__STATIC_INLINE sl_power_manager_on_isr_exit_t sl_iostream_uart_sleep_on_isr_exit(sl_iostream_uart_t *iostream_uart)
{
  return iostream_uart->sleep_on_isr_exit(iostream_uart->stream.context);
}
#endif

/** @} (end addtogroup iostream_uart) */
/** @} (end addtogroup iostream) */

#ifdef __cplusplus
}
#endif

#endif // SL_IOSTREAM_UART_H
