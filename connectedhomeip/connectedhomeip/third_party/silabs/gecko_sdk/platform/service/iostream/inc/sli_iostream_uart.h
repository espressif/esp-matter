/***************************************************************************//**
 * @file
 * @brief IO Stream UART internal APIs.
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

#ifndef SLI_IOSTREAM_UART_H
#define SLI_IOSTREAM_UART_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif

#include "sl_status.h"
#include "sl_iostream.h"
#include "sl_iostream_uart.h"

#define SLI_IOSTREAM_UART_FLAG_CTS             0x01
#define SLI_IOSTREAM_UART_FLAG_RTS             0x02
#define SLI_IOSTREAM_UART_FLAG_HIGH_FREQUENCY  0x04

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

sl_status_t sli_iostream_uart_context_init(sl_iostream_uart_t *uart,
                                           sl_iostream_uart_context_t *context,
                                           sl_iostream_uart_config_t *config,
                                           sl_status_t (*tx)(void *context, char c),
                                           void (*tx_completed)(void *context, bool enable),
                                           void (*enable_rx)(void *context),
                                           sl_status_t (*deinit)(void *context),
                                           uint8_t rx_em_req,
                                           uint8_t tx_em_req);

bool sli_uart_is_rx_space_avail(void *context);

void sli_uart_push_rxd_data(void *context,
                            char c);

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT) && !defined(SL_IOSTREAM_UART_FLUSH_TX_BUFFER)
void sli_uart_txc(void *context);
#endif

#ifdef __cplusplus
}
#endif

#endif /* SLI_IOSTREAM_UART_H */
