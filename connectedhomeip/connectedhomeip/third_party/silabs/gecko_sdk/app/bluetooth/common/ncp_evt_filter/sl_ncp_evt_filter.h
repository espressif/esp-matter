/***************************************************************************//**
 * @file
 * @brief Bluetooth Network Co-Processor (NCP) Event Filter Interface
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

#ifndef SL_NCP_EVT_FILTER_H
#define SL_NCP_EVT_FILTER_H

#include <stdint.h>
#include <stdbool.h>
#include "sl_bt_api.h"

typedef uint32_t evt_filter_t;

PACKSTRUCT(struct user_cmd_manage_event_filter {
  struct {
    uint8_t len;
    uint8_t id;
  } hdr;
  evt_filter_t evt;
});

typedef struct user_cmd_manage_event_filter user_cmd_manage_event_filter_t;

/**************************************************************************//**
 * User command (manage_event_filter) handler.
 * This function processes the event filter command which is in the cmd
 * parameter and according to it adds/removes/resets the events in the
 * event filter array.
 *
 * @param[in] cmd payload of manage event filter
 *****************************************************************************/
void sl_ncp_evt_filter_handler(user_cmd_manage_event_filter_t *cmd);

/***************************************************************************//**
 * Checks if the given event is filtered or not.
 *
 * @param[in] header incoming Bluetooth stack event header
 * @return Returns true if filtered, false otherwise.
 ******************************************************************************/
bool sl_ncp_evt_filter_is_filtered(uint32_t header);

#endif // SL_NCP_EVT_FILTER_H
