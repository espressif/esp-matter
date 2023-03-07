/***************************************************************************//**
 * @file
 * @brief NCP Daemon
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

#ifndef SL_NCP_DAEMON_H
#define SL_NCP_DAEMON_H

#include "config.h"

typedef struct {
  int fd;
  unsigned recv_len;
  char recv_buf[MAX_PACKET_SIZE * 2];
}sl_bgapi_context_t;

/***********************************************************************************************//**
 *  \brief  Reset BGAPI.
 *  \param ctx BGAPI context to reset.
 **************************************************************************************************/
void sl_bgapi_reset(sl_bgapi_context_t *ctx);

/***********************************************************************************************//**
 *  \brief  Initialize BGAPI context.
 *  \param  ctx BGAPI context to initialize.
 *  \param fd file descriptor of socket.
 **************************************************************************************************/
void sl_bgapi_context_init(sl_bgapi_context_t *ctx, int fd);

/***********************************************************************************************//**
 *  \brief  Set NCP Target to a context.
 *  \param  ctx BGAPI context.
 **************************************************************************************************/
void sl_bgapi_set_ncp_target(sl_bgapi_context_t *ctx);

/***********************************************************************************************//**
 *  \brief  Receive BGAPI data.
 *  \param  ctx BGAPI context.
 *  \param buf pointer to new BGAPI data.
 *  \param len length of new BGAPI data.
 **************************************************************************************************/
void sl_bgapi_recv_data(sl_bgapi_context_t *ctx, char *buf, int len);

/***********************************************************************************************//**
 *  \brief  Process BGAPI callback.
 *  \param  ctx BGAPI context.
 *  \param buf pointer to new BGAPI data.
 *  \param len length of new BGAPI data.
 *  \param encrypted Encryption state.
 **************************************************************************************************/
void sl_bgapi_process_packet_cb(sl_bgapi_context_t *ctx, char *buf, int len, int encrypted);

/***********************************************************************************************//**
 *  \brief  Function to initate encrcyption with the target.
 *  \param  public_key Public Key.
 *  \param host_iv_to_target Initialization Vector to Target.
 *  \param host_iv_to_target Initialization Vector to Host.
 *  \return  0 on success, -1 on failure.
 **************************************************************************************************/
int sl_bgapi_user_cmd_increase_security(uint8_t *public_key, uint8_t *host_iv_to_target, uint8_t *host_iv_to_host);

#endif
