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
#include "sl_status.h"
#include "sl_iostream.h"
#include "sl_iostream_uart.h"
#include "sli_iostream_uart.h"
#include "sl_iostream_leuart.h"
#if (defined(SL_COMPONENT_CATALOG_PRESENT))
#include "sl_component_catalog.h"
#endif
#if (defined(SL_CATALOG_POWER_MANAGER_PRESENT))
#include "sl_power_manager.h"
#endif
#include "sl_atomic.h"

#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "em_leuart.h"
#include "em_device.h"
#include "em_core.h"
#include "em_leuart.h"
#include "em_gpio.h"

/*******************************************************************************
 *********************   LOCAL FUNCTION PROTOTYPES   ***************************
 ******************************************************************************/

static sl_status_t leuart_tx(void *context,
                             char c);

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT) && !defined(SL_IOSTREAM_UART_FLUSH_TX_BUFFER)
static void leuart_tx_completed(void *context, bool enable);
#endif

static void leuart_enable_rx(void *context);

static sl_status_t leuart_deinit(void *context);

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * LEUART Stream init
 ******************************************************************************/
sl_status_t sl_iostream_leuart_init(sl_iostream_uart_t *iostream_uart,
                                    sl_iostream_uart_config_t *uart_config,
                                    LEUART_Init_TypeDef *init,
                                    sl_iostream_leuart_config_t *leuart_config,
                                    sl_iostream_leuart_context_t *leuart_context)
{
  sl_status_t status;

  status = sli_iostream_uart_context_init(iostream_uart,
                                          &leuart_context->context,
                                          uart_config,
                                          leuart_tx,
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT) && !defined(SL_IOSTREAM_UART_FLUSH_TX_BUFFER)
                                          leuart_tx_completed,
#else
                                          NULL,
#endif
                                          leuart_enable_rx,
                                          leuart_deinit,
                                          2,
                                          2);
  if (status != SL_STATUS_OK) {
    return status;
  }

  leuart_context->clock = leuart_config->clock;
  leuart_context->rx_pin = leuart_config->rx_pin;
  leuart_context->rx_port = leuart_config->rx_port;
  leuart_context->tx_pin = leuart_config->tx_pin;
  leuart_context->tx_port = leuart_config->tx_port;

  leuart_context->leuart = leuart_config->leuart;

  // Configure GPIO pins
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure TX and RX GPIOs
  GPIO_PinModeSet(leuart_config->tx_port, leuart_config->tx_pin, gpioModePushPull, 1);
  GPIO_PinModeSet(leuart_config->rx_port, leuart_config->rx_pin, gpioModeInputPull, 1);

  // Enable LEUART clock
  CMU_ClockEnable(leuart_config->clock, true);

  // Do not prescale clock
  CMU_ClockDivSet(leuart_config->clock, cmuClkDiv_1);

  // Reset and initialize the LEUART peripheral
  init->enable = leuartDisable;
  LEUART_Reset(leuart_config->leuart);
  LEUART_Init(leuart_config->leuart, init);
  leuart_context->leuart = leuart_config->leuart;

#if defined(LEUART_ROUTEPEN_RXPEN)
  leuart_context->leuart->ROUTEPEN = LEUART_ROUTEPEN_RXPEN | LEUART_ROUTEPEN_TXPEN;
  leuart_context->leuart->ROUTELOC0 = (leuart_context->leuart->ROUTELOC0
                                       & ~(_LEUART_ROUTELOC0_TXLOC_MASK
                                           | _LEUART_ROUTELOC0_RXLOC_MASK) )
                                      | (leuart_config->tx_location << _LEUART_ROUTELOC0_TXLOC_SHIFT)
                                      | (leuart_config->rx_location << _LEUART_ROUTELOC0_RXLOC_SHIFT);
#else
  leuart_context->leuart->ROUTE = LEUART_ROUTE_RXPEN | LEUART_ROUTE_TXPEN
                                  | (leuart_config->location << _LEUART_ROUTE_LOCATION_SHIFT);
#endif

  // Enable RX interrupts
  LEUART_IntEnable(leuart_context->leuart, LEUART_IF_RXDATAV);

  // Finally enable it
  LEUART_Enable(leuart_context->leuart, leuartEnable);

  return SL_STATUS_OK;
}

/**************************************************************************//**
 * @brief LEUART IRQ Handler
 *****************************************************************************/
void sl_iostream_leuart_irq_handler(void *stream_context)
{
  sl_iostream_leuart_context_t *leuart_context = (sl_iostream_leuart_context_t *)stream_context;

  if (leuart_context->leuart->IF & LEUART_IF_RXDATAV) {
    if (sli_uart_is_rx_space_avail(stream_context)) {
      // There is room for data in the RX buffer so we store the data
      uint8_t c = LEUART_Rx(leuart_context->leuart);
      sli_uart_push_rxd_data(stream_context, c);
    } else {
      // The RX buffer is full so we must wait for the leuart_read()
      // function to make some more room in the buffer. RX interrupts are
      // disabled to let the ISR exit. The RX interrupt will be enabled in
      // leuart_read().
      LEUART_IntDisable(leuart_context->leuart, LEUART_IF_RXDATAV);
    }
  }

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT) && !defined(SL_IOSTREAM_UART_FLUSH_TX_BUFFER)
  if (leuart_context->leuart->IF & LEUART_IF_TXC) {
    LEUART_IntClear(leuart_context->leuart, LEUART_IF_TXC);
    // Check if the Status register has the TXC flag as well since the flag will clean itself
    // if other transmissions are queued contrary to the IF flag
    if ((LEUART_StatusGet(leuart_context->leuart) & _LEUART_STATUS_TXC_MASK) != 0) {
      sli_uart_txc(stream_context);
    }
  }
#endif
}

/*******************************************************************************
 **************************   LOCAL FUNCTIONS   ********************************
 ******************************************************************************/

/***************************************************************************//**
 * Internal stream write implementation
 ******************************************************************************/
static sl_status_t leuart_tx(void *context,
                             char c)
{
  sl_iostream_leuart_context_t *leuart_context = (sl_iostream_leuart_context_t *)context;

  LEUART_Tx(leuart_context->leuart, (uint8_t)c);

#if defined(SL_IOSTREAM_UART_FLUSH_TX_BUFFER)
  /* Wait until transmit buffer is empty */
  while (!(LEUART_StatusGet(leuart_context->leuart) & LEUART_STATUS_TXBL)) ;
#endif

  return SL_STATUS_OK;
}

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT) && !defined(SL_IOSTREAM_UART_FLUSH_TX_BUFFER)
/***************************************************************************//**
 * Enable/Disable LEUART Tx Complete (TXC) Interrupt
 ******************************************************************************/
static void leuart_tx_completed(void *context, bool enable)
{
  (void)context;
  (void)enable;

  sl_iostream_leuart_context_t *leuart_context = (sl_iostream_leuart_context_t *)context;
  if (enable) {
    LEUART_IntEnable(leuart_context->leuart, LEUART_IF_TXC);
  } else {
    LEUART_IntDisable(leuart_context->leuart, LEUART_IF_TXC);
    LEUART_IntClear(leuart_context->leuart, LEUART_IF_TXC);
  }
}
#endif

/***************************************************************************//**
 * Enable ISR on Rx
 ******************************************************************************/
static void leuart_enable_rx(void *context)
{
  sl_iostream_leuart_context_t *leuart_context = (sl_iostream_leuart_context_t *)context;
  LEUART_IntEnable(leuart_context->leuart, LEUART_IF_RXDATAV);
}

/***************************************************************************//**
 * LEUART Stream De-init.
 ******************************************************************************/
static sl_status_t leuart_deinit(void *context)
{
  sl_iostream_leuart_context_t *leuart_context = (sl_iostream_leuart_context_t *)context;

  // Wait until transfert is completed
  while (!(LEUART_StatusGet(leuart_context->leuart) & LEUART_STATUS_TXBL)) {
  }

  // De-Configure TX and RX GPIOs
  GPIO_PinModeSet(leuart_context->tx_port, leuart_context->tx_pin, gpioModeDisabled, 0);
  GPIO_PinModeSet(leuart_context->rx_port, leuart_context->rx_pin, gpioModeDisabled, 0);

  // Disable USART peripheral
  LEUART_Enable(leuart_context->leuart, leuartDisable);

  // Disable USART Clock
  CMU_ClockEnable(leuart_context->clock, false);

  return SL_STATUS_OK;
}
