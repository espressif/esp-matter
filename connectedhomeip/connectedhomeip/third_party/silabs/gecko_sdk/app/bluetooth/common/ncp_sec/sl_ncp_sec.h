/***************************************************************************//**
 * @file
 * @brief Secure NCP functionality
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

#ifndef NCP_SEC_H_
#define NCP_SEC_H_

#include <stdbool.h>
#include <stdint.h>
#include "sl_ncp_config.h"
#include "sl_bt_api.h"

#define CMD_BUF_SIZE SL_NCP_EVT_BUF_SIZE

#define SL_NCP_SEC_EVT_PROCESS    ((uint32_t)0x0001)  ///< Process event.
#define SL_NCP_SEC_CMD_PROCESS    ((uint32_t)0x0002)  ///< Process command.
#define SL_NCP_SEC_RSP_PROCESS    ((uint32_t)0x0004)  ///< Process response.

/***************************************************************************//**
 * @brief
 *   Tells whether the given SL_BT_API message is encrypted.
 *
 * @param[in] msg
 *   Pointer to the SL_BT_API message.
 *
 * @return
 *   True if the message is encrypted, otherwise False.
 *
 ******************************************************************************/
bool sl_ncp_sec_is_encrypted(volatile void* msg);

/***************************************************************************//**
 * @brief
 * Process the event packet for security. The event will be encrypted
 * if the transport is in encryption mode.
 *
 * @param[in] msg
 *   Pointer to the event packet
 *
 * @return
 *   Pointer to the packet ready to send over NCP transport
 ******************************************************************************/
sl_bt_msg_t* sl_ncp_sec_process_event(sl_bt_msg_t* msg);

/***************************************************************************//**
 * @brief
 *   Process the response packet for security. The response will be encrypted
 *   if the command it is responding to was encrypted.
 *
 * @param[in] msg
 *   Pointer to the response packet
 * @param[in] cmd_was_encrypted
 *   Indicates whether the command this packet is responding to was encrypted
 *
 * @return
 *   Pointer to the packet ready to send over NCP transport
 ******************************************************************************/
sl_bt_msg_t* sl_ncp_sec_process_response(sl_bt_msg_t* msg,
                                         bool cmd_was_encrypted);

/***************************************************************************//**
 * @brief
 * Main state machine for secure NCP handling.
 * Increases security in case of ncp_sec_increase_security user command arrives.
 * Automatically decripts received messages if encription is enabled.
 * In case of error assembles an error message and sends it out.
 *
 * @param[in] msg
 *   Pointer to the received message buffer
 *
 * @return
 *   SL_NCP_SEC_EVT_PROCESS if event was generated, and it has to be processed
 *   SL_NCP_SEC_CMD_PROCESS if the command has to be processed further
 *   SL_NCP_SEC_RSP_PROCESS if the command was processed and the response has to
 *     be processed further
 *   or the combination of above (like cmd and rsp has to be processed as well)
 ******************************************************************************/
uint32_t sl_ncp_sec_command_handler(uint8_t* data);

#endif /* NCP_SEC_H_ */
