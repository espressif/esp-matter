/***************************************************************************//**
 * @file
 * @brief Bluetooth Network Co-Processor (NCP) Interface
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

#ifndef SL_NCP_H
#define SL_NCP_H

#include <stdbool.h>
#include <stdint.h>
#include "sl_status.h"
#include "sl_ncp_config.h"
#include "sl_bt_api.h"
#include "sl_power_manager.h"

/**************************************************************************//**
 * NCP initialization function.
 *****************************************************************************/
void sl_ncp_init(void);

/**************************************************************************//**
 * NCP process action function.
 *****************************************************************************/
void sl_ncp_step(void);

/**************************************************************************//**
 * Local event processor
 *****************************************************************************/
bool sl_ncp_local_evt_process(sl_bt_msg_t *evt);

/**************************************************************************//**
 * User command (message_to_target) handler callback.
 *
 * Handle user defined commands received from NCP-host.
 *
 * @param[in] data Data received from NCP through UART.
 *****************************************************************************/
void sl_ncp_user_cmd_message_to_target_cb(void *data);

/**************************************************************************//**
 * Send user command (message_to_target) response.
 *
 * Send response to user defined (message_to_target) command to NCP-host.
 *
 * @param[out] result Result of the response to the command received.
 * @param[out] len Message length.
 * @param[out] data Data to send to NCP.
 *****************************************************************************/
void sl_ncp_user_cmd_message_to_target_rsp(sl_status_t result,
                                           uint8_t len,
                                           uint8_t *data);

/**************************************************************************//**
 * Send user event (message_to_host).
 *
 * Send user defined (message_to_host) event to NCP-host.
 *
 * @param[out] len Message length.
 * @param[out] data Data to send to NCP.
 *****************************************************************************/
void sl_ncp_user_evt_message_to_host(uint8_t len, uint8_t *data);

/**************************************************************************//**
 * Check if NCP allows go to sleep
 *****************************************************************************/
bool sli_ncp_is_ok_to_sleep(void);

/**************************************************************************//**
 * Routine for power manager handler
 *****************************************************************************/
sl_power_manager_on_isr_exit_t sli_ncp_sleep_on_isr_exit(void);

#endif // SL_NCP_H
