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

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif

#include "sl_status.h"
#include "sl_iostream.h"
#include "sl_iostream_uart.h"
#include "sli_iostream_uart.h"
#include "sl_iostream_usart.h"
#include "sl_atomic.h"

#if (defined(SL_CATALOG_POWER_MANAGER_PRESENT))
#include "sl_power_manager.h"
#endif

#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "em_device.h"
#include "em_core.h"
#include "em_usart.h"
#include "em_gpio.h"

/*******************************************************************************
 *********************   LOCAL FUNCTION PROTOTYPES   ***************************
 ******************************************************************************/

static sl_status_t usart_tx(void *context,
                            char c);

static void usart_enable_rx(void *context);

static sl_status_t usart_deinit(void *context);

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * USART Stream init
 ******************************************************************************/
sl_status_t sl_iostream_usart_init(sl_iostream_uart_t *iostream_uart,
                                   sl_iostream_uart_config_t *uart_config,
                                   USART_InitAsync_TypeDef *init,
                                   sl_iostream_usart_config_t *config,
                                   sl_iostream_usart_context_t *usart_context)
{
  sl_status_t status;
#if (_SILICON_LABS_32B_SERIES > 0)
  bool cts = false;
  bool rts = false;
#endif

  status = sli_iostream_uart_context_init(iostream_uart, &usart_context->context, uart_config, usart_tx, usart_enable_rx, usart_deinit, 1, 1);
  if (status != SL_STATUS_OK) {
    return status;
  }

  usart_context->usart = config->usart;

  //Save useful config info to usart context
  usart_context->clock = config->clock;
  usart_context->tx_pin = config->tx_pin;
  usart_context->tx_port = config->tx_port;
  usart_context->rx_pin = config->rx_pin;
  usart_context->rx_port = config->rx_port;
#if (_SILICON_LABS_32B_SERIES > 0)
  usart_context->cts_pin = config->cts_pin;
  usart_context->cts_port = config->cts_port;
  usart_context->rts_pin = config->rts_pin;
  usart_context->rts_port = config->rts_port;
#endif

  // Enable peripheral clocks
#if defined(_CMU_HFPERCLKEN0_MASK)
  CMU_ClockEnable(cmuClock_HFPER, true);
#endif
  // Configure GPIO pins
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure TX and RX GPIOs
  GPIO_PinModeSet(config->tx_port, config->tx_pin, gpioModePushPull, 1);
  GPIO_PinModeSet(config->rx_port, config->rx_pin, gpioModeInputPull, 1);

  CMU_ClockEnable(config->clock, true);

  // Configure USART for basic async operation
  init->enable = usartDisable;
  USART_InitAsync(config->usart, init);

#if defined(GPIO_USART_ROUTEEN_TXPEN)
  // Enable pins at correct USART/USART location
  GPIO->USARTROUTE[config->usart_index].ROUTEEN = GPIO_USART_ROUTEEN_TXPEN | GPIO_USART_ROUTEEN_RXPEN;
  GPIO->USARTROUTE[config->usart_index].TXROUTE = (config->tx_port << _GPIO_USART_TXROUTE_PORT_SHIFT)
                                                  | (config->tx_pin << _GPIO_USART_TXROUTE_PIN_SHIFT);
  GPIO->USARTROUTE[config->usart_index].RXROUTE = (config->rx_port << _GPIO_USART_RXROUTE_PORT_SHIFT)
                                                  | (config->rx_pin << _GPIO_USART_RXROUTE_PIN_SHIFT);

#elif defined(USART_ROUTEPEN_RXPEN)
  // Enable pins at correct USART/USART location
  config->usart->ROUTEPEN |= USART_ROUTEPEN_RXPEN | USART_ROUTEPEN_TXPEN;
  config->usart->ROUTELOC0 = (config->usart->ROUTELOC0 & ~(_USART_ROUTELOC0_TXLOC_MASK | _USART_ROUTELOC0_RXLOC_MASK))
                             | (config->usart_tx_location << _USART_ROUTELOC0_TXLOC_SHIFT)
                             | (config->usart_rx_location << _USART_ROUTELOC0_RXLOC_SHIFT);
  config->usart->ROUTEPEN = USART_ROUTEPEN_RXPEN | USART_ROUTEPEN_TXPEN;
#else
  config->usart->ROUTE = USART_ROUTE_RXPEN | USART_ROUTE_TXPEN | (config->usart_location << _USART_ROUTE_LOCATION_SHIFT);
#endif

  // Configure GPIOs for hwflowcontrol
 #if (_SILICON_LABS_32B_SERIES > 0)
  usart_context->flags = 0;
  switch (init->hwFlowControl) {
    case usartHwFlowControlNone:
      break;
    case usartHwFlowControlCts:
      cts = true;
      usart_context->flags = SLI_IOSTREAM_UART_FLAG_CTS;
      break;
    case usartHwFlowControlRts:
      rts = true;
      usart_context->flags = SLI_IOSTREAM_UART_FLAG_RTS;
      break;
    case usartHwFlowControlCtsAndRts:
      cts = true;
      rts = true;
      usart_context->flags = SLI_IOSTREAM_UART_FLAG_CTS | SLI_IOSTREAM_UART_FLAG_RTS;
      break;
    default:
      return SL_STATUS_INVALID_CONFIGURATION;
  }

  if (cts == true) {
    GPIO_PinModeSet(config->cts_port, config->cts_pin, gpioModeInputPull, 0);

 #if defined(_USART_ROUTEPEN_RTSPEN_MASK) && defined(_USART_ROUTEPEN_CTSPEN_MASK)
    config->usart->ROUTELOC1 = (config->usart_cts_location << _USART_ROUTELOC1_CTSLOC_SHIFT);
    config->usart->CTRLX    |= USART_CTRLX_CTSEN;
    config->usart->ROUTEPEN |= USART_ROUTEPEN_CTSPEN;
 #elif defined(_GPIO_USART_ROUTEEN_MASK)
    GPIO->USARTROUTE_SET[config->usart_index].CTSROUTE = (config->cts_port << _GPIO_USART_CTSROUTE_PORT_SHIFT)
                                                         | (config->cts_pin << _GPIO_USART_CTSROUTE_PIN_SHIFT);
    config->usart->CTRLX_SET = USART_CTRLX_CTSEN;
 #endif
  }
  if (rts == true) {
    GPIO_PinModeSet(config->rts_port, config->rts_pin, gpioModePushPull, 0);
 #if defined(_USART_ROUTEPEN_RTSPEN_MASK) && defined(_USART_ROUTEPEN_CTSPEN_MASK)
    config->usart->ROUTELOC1 |= (config->usart_rts_location << _USART_ROUTELOC1_RTSLOC_SHIFT);
    config->usart->ROUTEPEN |= USART_ROUTEPEN_RTSPEN;

 #elif defined(_GPIO_USART_ROUTEEN_MASK)
    GPIO->USARTROUTE_SET[config->usart_index].ROUTEEN = GPIO_USART_ROUTEEN_RTSPEN;
    GPIO->USARTROUTE_SET[config->usart_index].RTSROUTE = (config->rts_port << _GPIO_USART_RTSROUTE_PORT_SHIFT)
                                                         | (config->rts_pin << _GPIO_USART_RTSROUTE_PIN_SHIFT);
 #endif
  }
 #endif  // Configure GPIOs for hwflowcontrol

 #if defined(SL_CATALOG_POWER_MANAGER_PRESENT) && defined(_SILICON_LABS_32B_SERIES_2)
  usart_context->tx_port = config->tx_port;
  usart_context->tx_pin = config->tx_pin;
 #endif

  // Enable RX interrupts
  USART_IntEnable(config->usart, USART_IF_RXDATAV);

  // Finally enable it
  USART_Enable(config->usart, usartEnable);

  return SL_STATUS_OK;
}

/**************************************************************************//**
 * @brief USART IRQ Handler
 *****************************************************************************/
void sl_iostream_usart_irq_handler(void *stream_context)
{
  sl_iostream_usart_context_t *usart_context = (sl_iostream_usart_context_t *)stream_context;

  if (usart_context->usart->STATUS & USART_STATUS_RXDATAV) {
    if (sli_uart_is_rx_space_avail(stream_context)) {
      // There is room for data in the RX buffer so we store the data
      uint8_t c = USART_Rx(usart_context->usart);
      sli_uart_push_rxd_data(stream_context, c);
    } else {
      // The RX buffer is full so we must wait for the usart_read()
      // function to make some more room in the buffer. RX interrupts are
      // disabled to let the ISR exit. The RX interrupt will be enabled in
      // usart_read().
      USART_IntDisable(usart_context->usart, USART_IF_RXDATAV);
    }
  }
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
  if (usart_context->usart->IF & USART_IF_TXC) {
    bool idle;
    USART_IntClear(usart_context->usart, USART_IF_TXC);
    USART_IntDisable(usart_context->usart, USART_IF_TXC);
    idle = sli_uart_txc(stream_context);
    if (idle == false) {
      USART_IntEnable(usart_context->usart, USART_IF_TXC);
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
static sl_status_t usart_tx(void *context,
                            char c)
{
  sl_iostream_usart_context_t *usart_context = (sl_iostream_usart_context_t *)context;

  USART_Tx(usart_context->usart, (uint8_t)c);

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT) && !defined(SL_IOSTREAM_UART_FLUSH_TX_BUFFER)
  // Enable TX interrupts
  USART_IntEnable(usart_context->usart, USART_IF_TXC);
#endif

#if defined(SL_IOSTREAM_UART_FLUSH_TX_BUFFER)
  /* Wait until transmit buffer is empty */
  while (!(USART_StatusGet(usart_context->usart) & USART_STATUS_TXBL)) ;
#endif

  return SL_STATUS_OK;
}
/***************************************************************************//**
 * Enable ISR on Rx
 ******************************************************************************/
static void usart_enable_rx(void *context)
{
  sl_iostream_usart_context_t *usart_context = (sl_iostream_usart_context_t *)context;
  USART_IntEnable(usart_context->usart, USART_IF_RXDATAV);
}

/***************************************************************************//**
 * USART Stream De-init.
 ******************************************************************************/
static sl_status_t usart_deinit(void *context)
{
  sl_iostream_usart_context_t *usart_context = (sl_iostream_usart_context_t *)context;

  // Wait until transfer is completed
  while (!(USART_StatusGet(usart_context->usart) & USART_STATUS_TXBL)) {
  }

  // De-Configure TX and RX GPIOs
  GPIO_PinModeSet(usart_context->tx_port, usart_context->tx_pin, gpioModeDisabled, 0);
  GPIO_PinModeSet(usart_context->rx_port, usart_context->rx_pin, gpioModeDisabled, 0);

#if (_SILICON_LABS_32B_SERIES > 0)
  // De-Configure Flow Control GPIOs
  if (usart_context->flags && SLI_IOSTREAM_UART_FLAG_CTS) {
    GPIO_PinModeSet(usart_context->cts_port, usart_context->cts_pin, gpioModeDisabled, 0);
  }
  if (usart_context->flags && SLI_IOSTREAM_UART_FLAG_RTS) {
    GPIO_PinModeSet(usart_context->rts_port, usart_context->rts_pin, gpioModeDisabled, 0);
  }
#endif

  // Disable USART peripheral
  USART_Enable(usart_context->usart, usartDisable);

  // Disable USART Clock
  CMU_ClockEnable(usart_context->clock, false);

  return SL_STATUS_OK;
}
