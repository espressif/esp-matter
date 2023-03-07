/***************************************************************************//**
 * @file
 * @brief NCP host application module.
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef NCP_HOST_H
#define NCP_HOST_H

#include "sl_status.h"
#include "host_comm.h"

#if defined(SECURITY) && SECURITY == 1
#include "ncp_sec_host.h"

#else // defined(SECURITY) && SECURITY == 1
// Dummy macros
#define NCP_SEC_OPTSTRING
#define NCP_SEC_USAGE
#define NCP_SEC_OPTIONS
#endif //defined(SECURITY) && SECURITY == 1

// Optstring argument for getopt.
#define NCP_HOST_OPTSTRING  HOST_COMM_OPTSTRING NCP_SEC_OPTSTRING

// Usage info.
#define NCP_HOST_USAGE  HOST_COMM_USAGE NCP_SEC_USAGE

// Options info.
#define NCP_HOST_OPTIONS  HOST_COMM_OPTIONS NCP_SEC_OPTIONS

/**************************************************************************//**
 * Initialize NCP connection.
 *
 * @retval SL_STATUS_OK Connection successful.
 * @retval SL_STATUS_INVALID_PARAMETER Invalid options.
 *****************************************************************************/
sl_status_t ncp_host_init(void);

/**************************************************************************//**
 * Set NCP connection options.
 *
 * @param[in] option Option to set.
 * @param[in] value Value of the option.
 *
 * @retval SL_STATUS_OK Option set successfully.
 * @retval SL_STATUS_NOT_FOUND Unknown option.
 *****************************************************************************/
sl_status_t ncp_host_set_option(char option, char *value);

/**************************************************************************//**
 * Deinitialize NCP connection.
 *****************************************************************************/
void ncp_host_deinit(void);

/**************************************************************************//**
 * Write data to NCP.
 *
 * @param[in] len Number of bytes to write.
 * @param[in] data Data to write.
 *****************************************************************************/
void ncp_host_tx(uint32_t len, uint8_t *data);

/**************************************************************************//**
 * Read data from NCP.
 *
 * @param[in] len Number of bytes to read.
 * @param[in] data Read data.
 *
 * @return Number of bytes read, -1 on error.
 *****************************************************************************/
int32_t ncp_host_rx(uint32_t len, uint8_t *data);

/**************************************************************************//**
 * Peek if readable data exists.
 *
 * @return Number of bytes on the buffer, -1 on error.
 *****************************************************************************/
int32_t ncp_host_peek(void);

#endif // NCP_HOST_H
