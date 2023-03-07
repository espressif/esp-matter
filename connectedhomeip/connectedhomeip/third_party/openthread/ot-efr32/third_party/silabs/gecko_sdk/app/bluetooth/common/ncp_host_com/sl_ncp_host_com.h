/***************************************************************************//**
 * @file
 * @brief Bluetooth Network Co-Processor (NCP) Host Communication Interface
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
#ifndef SL_NCP_HOST_COM_H
#define SL_NCP_HOST_COM_H

#include "sl_ncp_host_com_config.h"

/**************************************************************************//**
 * NCP host communication initialization.
 *****************************************************************************/
void sl_ncp_host_com_init(void);

/**************************************************************************//**
 * Transmit function
 *****************************************************************************/
void sl_ncp_host_com_write(uint32_t len, uint8_t *data);

/**************************************************************************//**
 * Receive function
 *****************************************************************************/
int32_t sl_ncp_host_com_read(uint32_t len, uint8_t *data);

/**************************************************************************//**
 * Gives back already received message length.
 *****************************************************************************/
int32_t sl_ncp_host_com_peek(void);

bool sl_ncp_host_is_ok_to_sleep(void);
#endif // SL_NCP_HOST_COM_H
