/***************************************************************************/ /**
 * @file
 * @brief CPC Driver Interface
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

#ifndef SLI_CPC_DRV_H
#define SLI_CPC_DRV_H

#include "sli_cpc.h"
#include "sl_status.h"
#include <stddef.h>
#include <stdarg.h>
#include <stdbool.h>

// -----------------------------------------------------------------------------
// SPI Default configuration value
#ifndef SL_CPC_DRV_SPI_FRAME_LENGTH
#define SL_CPC_DRV_SPI_FRAME_LENGTH          8
#endif
#ifndef SL_CPC_DRV_SPI_BIT_ORDER
#define SL_CPC_DRV_SPI_BIT_ORDER             spidrvBitOrderMsbFirst
#endif
#ifndef SL_CPC_DRV_SPI_BIT_ORSL_CPC_DRV_SPI_CLOCK_MODEDER
#define SL_CPC_DRV_SPI_CLOCK_MODE            spidrvClockMode0
#endif

typedef struct {
  bool use_raw_rx_buffer;
  bool preprocess_hdlc_header;
  bool uart_flowcontrol;
} sli_cpc_drv_capabilities_t;

#ifdef __cplusplus
extern "C"
{
#endif

// -----------------------------------------------------------------------------
// Core to driver commands

/***************************************************************************//**
 * Initializes CPC driver.
 *
 * @return 0 if successful. Error code otherwise.
 ******************************************************************************/
sl_status_t sli_cpc_drv_init(void);

/***************************************************************************//**
 * Gets CPC driver capabilities.
 *
 * @param capabilities Pointer to structure that will receive the driver
 *                     capabilities.
 ******************************************************************************/
void sli_cpc_drv_get_capabilities(sli_cpc_drv_capabilities_t *capabilities);

/***************************************************************************//**
 * Reads data from driver.
 *
 * @param buffer_handle Pointer to variable that will receive receive the buffer handle.
 *
 * @param payload_rx_len Pointer to variable that will receive the received
 *                       payload length, in bytes.
 *
 * @return 0 if successful. Error code otherwise.
 ******************************************************************************/
sl_status_t sli_cpc_drv_read_data(sl_cpc_buffer_handle_t **buffer_handle, uint16_t *payload_rx_len);

/***************************************************************************//**
 * Checks if driver is ready to transmit.
 *
 * @return true if driver is ready to accept a tx. false otherwise.
 ******************************************************************************/
bool sli_cpc_drv_is_transmit_ready(void);

/***************************************************************************//**
 * Transmits data.
 *
 * @param buffer_handle Pointer to the buffer handle.
 *
 * @param payload_tx_len Payload length to transmit, in bytes.
 *
 * @return 0 if successful. Error code otherwise.
 ******************************************************************************/
sl_status_t sli_cpc_drv_transmit_data(sl_cpc_buffer_handle_t *buffer_handle, uint16_t payload_tx_len);

// -----------------------------------------------------------------------------
// Driver to core notifications

/***************************************************************************//**
 * Notifies core of tx completion.
 *
 * @param buffer_handle Pointer to the buffer handle that was transmitted.
 ******************************************************************************/
void sli_cpc_drv_notify_tx_complete(sl_cpc_buffer_handle_t *buffer_handle);

/***************************************************************************//**
 * Notifies core of rx completion.
 ******************************************************************************/
void sli_cpc_drv_notify_rx_data(void);

/** @} (end addtogroup cpc) */

#ifdef __cplusplus
}
#endif

#endif // SLI_CPC_DRV_H
