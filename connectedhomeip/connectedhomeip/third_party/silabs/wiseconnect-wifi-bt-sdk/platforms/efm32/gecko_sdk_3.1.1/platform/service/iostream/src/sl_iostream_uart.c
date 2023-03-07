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
// Define module name for Power Manager debug feature
#define CURRENT_MODULE_NAME    "IOSTREAM_UART"

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif

#include "sl_status.h"
#include "sl_iostream.h"
#include "sl_iostream_uart.h"
#include "sli_iostream_uart.h"
#include "sl_atomic.h"

#if (defined(SL_CATALOG_KERNEL_PRESENT))
#include "cmsis_os2.h"
#include "sl_cmsis_os2_common.h"
#endif

#if (defined(SL_CATALOG_POWER_MANAGER_PRESENT))
#include "sl_power_manager.h"
#endif

#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "em_device.h"
#include "em_core.h"
#include "em_gpio.h"

/*******************************************************************************
 *********************   LOCAL FUNCTION PROTOTYPES   ***************************
 ******************************************************************************/
static sl_status_t uart_deinit(void *stream);

static sl_status_t uart_write(void *context,
                              const void *buffer,
                              size_t buffer_length);

static sl_status_t uart_read(void *context,
                             void *buffer,
                             size_t buffer_length,
                             size_t *bytes_read);

static void set_auto_cr_lf(void *context,
                           bool on);

static bool get_auto_cr_lf(void *context);

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
#if !defined(SL_CATALOG_KERNEL_PRESENT)
static sl_power_manager_on_isr_exit_t sleep_on_isr_exit(void *context);
#endif

static void set_rx_energy_mode_restriction(void *context,
                                           bool on);

static bool get_rx_energy_mode_restriction(void *context);
#endif

static uint32_t pop_byte_from_read_fifo(sl_iostream_uart_context_t *uart_context,
                                        uint8_t *c);

#if defined(SL_CATALOG_KERNEL_PRESENT)
static void set_read_block(void *context,
                           bool on);

static bool get_read_block(void *context);

static void set_rx_sem_count(sl_iostream_uart_context_t *uart_context);
#endif

extern __INLINE sl_status_t sl_iostream_uart_deinit(sl_iostream_uart_t *iostream_uart);

extern __INLINE void sl_iostream_uart_set_auto_cr_lf(sl_iostream_uart_t *iostream_uart,
                                                     bool on);

extern __INLINE bool sl_iostream_uart_get_auto_cr_lf(sl_iostream_uart_t *iostream_uart);

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
extern __INLINE void sl_iostream_uart_set_rx_energy_mode_restriction(sl_iostream_uart_t *iostream_uart,
                                                                     bool on);

extern __INLINE bool sl_iostream_uart_get_rx_energy_mode_restriction(sl_iostream_uart_t *iostream_uart);

#if !defined(SL_CATALOG_KERNEL_PRESENT)
extern __INLINE sl_power_manager_on_isr_exit_t sl_iostream_uart_sleep_on_isr_exit(sl_iostream_uart_t *iostream_uart);
#endif
#endif

#if defined(SL_CATALOG_KERNEL_PRESENT)
extern __INLINE void sl_iostream_uart_set_read_block(sl_iostream_uart_t *iostream_uart,
                                                     bool on);

extern __INLINE bool sl_iostream_uart_get_read_block(sl_iostream_uart_t *iostream_uart);
#endif

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * UART Stream init
 ******************************************************************************/
sl_status_t sli_iostream_uart_context_init(sl_iostream_uart_t *uart,
                                           sl_iostream_uart_context_t *context,
                                           sl_iostream_uart_config_t *config,
                                           sl_status_t (*tx)(void *context, char c),
                                           void (*enable_rx)(void *context),
                                           sl_status_t (*deinit)(void *context),
                                           uint8_t rx_em_req,
                                           uint8_t tx_em_req)
{
  (void)rx_em_req;
  (void)tx_em_req;

  if (config->rx_buffer == NULL) {
    return SL_STATUS_NULL_POINTER;
  }
  if (config->rx_buffer_length == 0) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Configure iostream struct and context
  memset(context, 0, sizeof(*context));
  context->rx_buffer = config->rx_buffer;
  context->rx_buffer_length = config->rx_buffer_length;
  context->lf_to_crlf = config->lf_to_crlf;
  context->tx = tx;
  context->enable_rx = enable_rx;
  context->deinit = deinit;
  context->rx_irq_number = config->rx_irq_number;
  #if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
  context->tx_irq_number = config->tx_irq_number;
  #endif
  uart->stream.context = context;

  uart->stream.write = uart_write;
  uart->stream.read = uart_read;
  uart->set_auto_cr_lf = set_auto_cr_lf;
  uart->get_auto_cr_lf = get_auto_cr_lf;
  uart->deinit = uart_deinit;

#if defined(SL_CATALOG_KERNEL_PRESENT)
  uart->set_read_block = set_read_block;
  uart->get_read_block = get_read_block;
  context->block = true;

  osMutexAttr_t m_attr;
  m_attr.attr_bits = 0u;
  m_attr.cb_mem = context->read_lock_cb;
  m_attr.cb_size = osMutexCbSize;
  context->read_lock = osMutexNew(&m_attr);
  EFM_ASSERT(context->read_lock != NULL);

  m_attr.cb_mem = context->write_lock_cb;
  context->write_lock = osMutexNew(&m_attr);
  EFM_ASSERT(context->write_lock != NULL);

  osSemaphoreAttr_t s_attr;
  s_attr.name = "Read Signal";
  s_attr.attr_bits = 0u;
  s_attr.cb_mem = context->read_signal_cb;
  s_attr.cb_size = osSemaphoreCbSize;
  context->read_signal = osSemaphoreNew(config->rx_buffer_length, 0u, &s_attr);
  EFM_ASSERT(context->read_signal != NULL);
#endif

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
#if !defined(SL_CATALOG_KERNEL_PRESENT)
  uart->sleep_on_isr_exit = sleep_on_isr_exit;
  context->sleep = SL_POWER_MANAGER_IGNORE;
#endif
  uart->set_rx_energy_mode_restriction = set_rx_energy_mode_restriction;
  uart->get_rx_energy_mode_restriction = get_rx_energy_mode_restriction;
  context->tx_em = (sl_power_manager_em_t)tx_em_req;
  context->rx_em = (sl_power_manager_em_t)rx_em_req;
  context->em_req_added = false;
  context->tx_idle = true;
  set_rx_energy_mode_restriction(context, config->rx_when_sleeping);
  NVIC_ClearPendingIRQ(config->tx_irq_number);
  NVIC_EnableIRQ(config->tx_irq_number);
#endif

  NVIC_ClearPendingIRQ(config->rx_irq_number);
  NVIC_EnableIRQ(config->rx_irq_number);

  sl_iostream_set_system_default(&uart->stream);

  return SL_STATUS_OK;
}

/**************************************************************************//**
 * @brief On ISR exit
 *****************************************************************************/
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT) && !defined(SL_CATALOG_KERNEL_PRESENT)
static sl_power_manager_on_isr_exit_t sleep_on_isr_exit(void *context)
{
  sl_iostream_uart_context_t *uart_context = (sl_iostream_uart_context_t *)context;

  if ((uart_context->tx_idle) && (uart_context->sleep == SL_POWER_MANAGER_SLEEP)) {
    sl_power_manager_on_isr_exit_t sleep = uart_context->sleep;
    uart_context->sleep = SL_POWER_MANAGER_IGNORE;
    return sleep;
  } else {
    return SL_POWER_MANAGER_IGNORE;
  }
}
#endif

/**************************************************************************//**
 * Set LF to CRLF conversion
 *****************************************************************************/
static void set_auto_cr_lf(void *context,
                           bool on)
{
  sl_iostream_uart_context_t *uart_context = (sl_iostream_uart_context_t *)context;

  sl_atomic_store(uart_context->lf_to_crlf, on);
}

/**************************************************************************//**
 * Get LF to CRLF conversion
 *****************************************************************************/
static bool get_auto_cr_lf(void *context)
{
  sl_iostream_uart_context_t *uart_context = (sl_iostream_uart_context_t *)context;
  bool conversion;

  sl_atomic_load(conversion, uart_context->lf_to_crlf);

  return conversion;
}

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
/**************************************************************************//**
 * Set Rx when sleeping
 *****************************************************************************/
static void set_rx_energy_mode_restriction(void *context,
                                           bool on)
{
  sl_iostream_uart_context_t *uart_context = (sl_iostream_uart_context_t *)context;
  bool em_req_added;

  sl_atomic_load(em_req_added, uart_context->em_req_added);
  if ((on == true)
      && (em_req_added == false)) {
    sl_power_manager_add_em_requirement(uart_context->rx_em);
    sl_atomic_store(uart_context->em_req_added, true);
  } else if ((on == false)
             && (em_req_added == true)) {
    sl_power_manager_remove_em_requirement(uart_context->rx_em);
    sl_atomic_store(uart_context->em_req_added, false);
  }
}
#endif

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
/**************************************************************************//**
 * Get Rx when sleeping
 *****************************************************************************/
static bool get_rx_energy_mode_restriction(void *context)
{
  sl_iostream_uart_context_t *uart_context = (sl_iostream_uart_context_t *)context;
  bool em_req_added;

  sl_atomic_load(em_req_added, uart_context->em_req_added);
  return em_req_added;
}
#endif

#if (defined(SL_CATALOG_KERNEL_PRESENT))
/**************************************************************************//**
 * Set read blocking mode
 *****************************************************************************/
static void set_read_block(void *context,
                           bool on)
{
  sl_iostream_uart_context_t *uart_context = (sl_iostream_uart_context_t *)context;

  sl_atomic_store(uart_context->block, on);
}
#endif

#if (defined(SL_CATALOG_KERNEL_PRESENT))
/**************************************************************************//**
 * Get read blocking mode
 *****************************************************************************/
static bool get_read_block(void *context)
{
  sl_iostream_uart_context_t *uart_context = (sl_iostream_uart_context_t *)context;
  bool block;

  sl_atomic_load(block, uart_context->block);
  return block;
}
#endif

/**************************************************************************//**
 * Is space available in rx buffer.
 *****************************************************************************/
bool sli_uart_is_rx_space_avail(void *context)
{
  sl_iostream_uart_context_t *uart_context = (sl_iostream_uart_context_t *)context;

  if (uart_context->rx_count < uart_context->rx_buffer_length) {
    return true;
  }
  return false;
}

/**************************************************************************//**
 * Push data in the rx fifo
 *****************************************************************************/
void sli_uart_push_rxd_data(void *context,
                            char c)
{
  sl_iostream_uart_context_t *uart_context = (sl_iostream_uart_context_t *)context;

  EFM_ASSERT(uart_context->rx_count < uart_context->rx_buffer_length);
  uart_context->rx_buffer[uart_context->rx_write_index] = c;
  uart_context->rx_write_index++;
  uart_context->rx_count++;
  if (uart_context->rx_write_index == uart_context->rx_buffer_length) {
    uart_context->rx_write_index = 0;
  }
#if defined(SL_CATALOG_KERNEL_PRESENT)
  {
    osKernelState_t state = osKernelGetState();
    if ((state == osKernelRunning) || (state == osKernelLocked)) {
      EFM_ASSERT(osSemaphoreRelease(uart_context->read_signal) == osOK);
    }
  }
#elif defined(SL_CATALOG_POWER_MANAGER_PRESENT)
  uart_context->sleep = SL_POWER_MANAGER_WAKEUP;
#endif
}

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
/**************************************************************************//**
 * Signal transmit complete
 *****************************************************************************/
bool sli_uart_txc(void *context)
{
  sl_iostream_uart_context_t *uart_context = (sl_iostream_uart_context_t *)context;

  if (uart_context->tx_idle == false) {
    uart_context->tx_idle = true;
    sl_power_manager_remove_em_requirement(uart_context->tx_em);
#if !defined(SL_CATALOG_KERNEL_PRESENT)
    uart_context->sleep = SL_POWER_MANAGER_SLEEP;
#endif
  }

  return uart_context->tx_idle;
}
#endif

/*******************************************************************************
 **************************   LOCAL FUNCTIONS   ********************************
 ******************************************************************************/

/***************************************************************************//**
 * UART Stream De-init
 ******************************************************************************/
static sl_status_t uart_deinit(void *stream)
{
  sl_iostream_uart_t *uart = (sl_iostream_uart_t *)stream;
  sl_iostream_uart_context_t *uart_context = (sl_iostream_uart_context_t *)uart->stream.context;
  sl_iostream_t *default_stream;
  sl_status_t status = SL_STATUS_OK;
#if (defined(SL_CATALOG_KERNEL_PRESENT))
  if (osKernelGetState() == osKernelRunning) {
    // Acquire locks to ensure no others task try to perform operation on the stream at sametime
    EFM_ASSERT(osMutexAcquire(uart_context->write_lock, osWaitForever) == osOK);  // If deinit is called twice in a
                                                                                  // row, the assert will trigger
    // Bypass lock if we print before the kernel is running
    EFM_ASSERT(osMutexAcquire(uart_context->read_lock, osWaitForever) == osOK);   // If deinit is called twice in a
                                                                                  //row, the assert will trigger
  }
#endif

  default_stream = sl_iostream_get_default();

  // Check if uart stream is the default and if it's the case,
  // remove it's reference as the default
  if ((sl_iostream_uart_t*)default_stream == uart) {
    sl_iostream_set_system_default(NULL);
  }

  NVIC_ClearPendingIRQ(uart_context->rx_irq_number);
  NVIC_DisableIRQ(uart_context->rx_irq_number);

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
  NVIC_ClearPendingIRQ(uart_context->tx_irq_number);
  NVIC_DisableIRQ(uart_context->tx_irq_number);
#endif

#if defined(SL_CATALOG_KERNEL_PRESENT)
  // Delete Kernel synchronization objects.
  EFM_ASSERT(osSemaphoreDelete(uart_context->read_signal) == osOK);
  EFM_ASSERT(osMutexDelete(uart_context->read_lock) == osOK);
  EFM_ASSERT(osMutexDelete(uart_context->write_lock) == osOK);
#endif

  // Clear iostream struct and context
  uart->stream.context = NULL;
  uart->stream.write = NULL;
  uart->stream.read = NULL;
  uart->set_auto_cr_lf = NULL;
  uart->get_auto_cr_lf = NULL;

  status = uart_context->deinit(uart_context);

  return status;
}

/***************************************************************************//**
 * Internal stream write implementation
 ******************************************************************************/
static sl_status_t uart_write(void *context,
                              const void *buffer,
                              size_t buffer_length)
{
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
  CORE_DECLARE_IRQ_STATE;
  bool idle;
#endif
  sl_iostream_uart_context_t *uart_context = (sl_iostream_uart_context_t *)context;
  char *c = (char *)buffer;
  bool cr_to_crlf = false;
#if (defined(SL_CATALOG_KERNEL_PRESENT))
  osStatus_t status;
  if (osKernelGetState() == osKernelRunning) {
    // Bypass lock if we print before the kernel is running
    status = osMutexAcquire(uart_context->write_lock, osWaitForever);

    if (status != osOK) {
      return SL_STATUS_INVALID_STATE; // Can happen if a task deinit and another try to write at sametime
    }
  }
#endif

  sl_atomic_load(cr_to_crlf, uart_context->lf_to_crlf);

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
  CORE_ENTER_CRITICAL();
  idle = uart_context->tx_idle;
  uart_context->tx_idle = true;
  CORE_EXIT_CRITICAL();
  if (idle == false) {
    sl_power_manager_remove_em_requirement(uart_context->tx_em);
  }
#endif

  if (cr_to_crlf == false) {
    for (uint32_t i = 0; i < buffer_length; i++) {
      uart_context->tx(context, *c);
      c++;
    }
  } else {
    for (uint32_t i = 0; i < buffer_length; i++) {
      if (*c == '\n') {
        uart_context->tx(context, '\r');
      }
      uart_context->tx(context, *c);
      c++;
    }
  }

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT) && !defined(SL_IOSTREAM_UART_FLUSH_TX_BUFFER)
  sl_power_manager_add_em_requirement(uart_context->tx_em);
  uart_context->tx_idle = false;
#endif

#if (defined(SL_CATALOG_KERNEL_PRESENT))
  if (osKernelGetState() == osKernelRunning) {
    // Bypass lock if we print before the kernel is running
    EFM_ASSERT(osMutexRelease(uart_context->write_lock) == osOK);
  }
#endif
  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Internal stream read implementation
 ******************************************************************************/
static sl_status_t uart_read(void *context,
                             void *buffer,
                             size_t buffer_length,
                             size_t *bytes_read)
{
  sl_iostream_uart_context_t *uart_context = (sl_iostream_uart_context_t *)context;
  uint32_t  rx_count;
  uint8_t *c = (uint8_t *)buffer;
#if (defined(SL_CATALOG_KERNEL_PRESENT))
  osStatus_t status;
  if (osKernelGetState() == osKernelRunning) {
    // Bypass lock if we print before the kernel is running
    status = osMutexAcquire(uart_context->read_lock, osWaitForever);

    if (status != osOK) {
      return SL_STATUS_INVALID_STATE; // Can happen if a task deinit and another try to read at sametime
    }

    if (uart_context->block) {
      EFM_ASSERT(osSemaphoreAcquire(uart_context->read_signal, osWaitForever) == osOK);
    }
  }
#endif

  *bytes_read = 0;
  while ((*bytes_read < buffer_length)) {
    rx_count = pop_byte_from_read_fifo(uart_context, c);
    if (rx_count == 0) {
      goto exit;
    }
    c++;
    (*bytes_read)++;
  }

  exit:
#if (defined(SL_CATALOG_KERNEL_PRESENT))
  set_rx_sem_count(uart_context);
  if (osKernelGetState() == osKernelRunning) {
    // Bypass lock if we print before the kernel is running
    EFM_ASSERT(osMutexRelease(uart_context->read_lock) == osOK);
  }
#endif

  if (*bytes_read == 0) {
    return SL_STATUS_EMPTY;
  } else {
    return SL_STATUS_OK;
  }
}

/***************************************************************************//**
 * Get 1 byte from the read FIFO
 ******************************************************************************/
static uint32_t pop_byte_from_read_fifo(sl_iostream_uart_context_t *uart_context,
                                        uint8_t *c)
{
  uint32_t rx_count;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  rx_count = uart_context->rx_count;
  CORE_EXIT_ATOMIC();

  if (rx_count == 0) {
    return rx_count;
  }

  *c = uart_context->rx_buffer[uart_context->rx_read_index];

  uart_context->rx_read_index++;
  if (uart_context->rx_read_index == uart_context->rx_buffer_length) {
    uart_context->rx_read_index = 0;
  }

  CORE_ENTER_ATOMIC();
  uart_context->rx_count--;
  CORE_EXIT_ATOMIC();

  // Unconditionally enable the RX interrupt. RX interrupts are disabled when
  // a buffer full condition is entered. This way flow control can be handled
  // automatically by the hardware

  uart_context->enable_rx(uart_context);

  return rx_count;
}

/***************************************************************************//**
 * Set receive semaphore count
 ******************************************************************************/
#if (defined(SL_CATALOG_KERNEL_PRESENT))
static void set_rx_sem_count(sl_iostream_uart_context_t *uart_context)
{
  osStatus_t status;

  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  while (osSemaphoreGetCount(uart_context->read_signal) < uart_context->rx_count) {
    status = osSemaphoreRelease(uart_context->read_signal);
    EFM_ASSERT(status == osOK);
  }

  while (osSemaphoreGetCount(uart_context->read_signal) > uart_context->rx_count) {
    status = osSemaphoreAcquire(uart_context->read_signal, 0u);
    EFM_ASSERT(status == osOK);
  }
  CORE_EXIT_ATOMIC();
}
#endif
