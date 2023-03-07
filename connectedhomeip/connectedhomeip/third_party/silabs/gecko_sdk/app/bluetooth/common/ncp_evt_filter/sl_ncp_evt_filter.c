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

#include <stdint.h>
#include "sl_status.h"
#include "sl_ncp_evt_filter.h"
#include "sl_ncp_evt_filter_config.h"
#include "sl_ncp_evt_filter_common.h"

static evt_filter_t evt_filter_array[SL_NCP_EVT_FILTER_ARRAY_LENGTH];
static uint8_t evt_pos = 0;

static sl_status_t evt_filter_add_to_array(evt_filter_t event);
static bool evt_filter_search_in_array(evt_filter_t event);
static sl_status_t evt_filter_remove_from_array(evt_filter_t event);
static sl_status_t evt_filter_reset_array(void);
static bool evt_filter_check_is_valid_user_command(user_cmd_manage_event_filter_t *cmd);

// -----------------------------------------------------------------------------
// Public functions (API implementation)

/**************************************************************************//**
 * User command (manage_event_filter) handler.
 *****************************************************************************/
void sl_ncp_evt_filter_handler(user_cmd_manage_event_filter_t *cmd)
{
  sl_status_t status = SL_STATUS_FAIL;

  //check length of commands
  if (false == evt_filter_check_is_valid_user_command(cmd)) {
    sl_bt_send_rsp_user_manage_event_filter(status);
    return;
  }

  switch (cmd->hdr.id) {
    // -------------------------------
    // Add event filter
    case SL_NCP_EVT_FILTER_CMD_ADD_ID:
      status = evt_filter_add_to_array(cmd->evt);
      break;

    // -------------------------------
    // Remove event filter
    case SL_NCP_EVT_FILTER_CMD_REMOVE_ID:
      status = evt_filter_remove_from_array(cmd->evt);
      break;

    // -------------------------------
    // Reset event filter
    case SL_NCP_EVT_FILTER_CMD_RESET_ID:
      status = evt_filter_reset_array();
      break;

    // -------------------------------
    default:
      // Unknown subcommand, send response with failure.
      break;
  }

  sl_bt_send_rsp_user_manage_event_filter(status);
}

/***************************************************************************//**
 * Checks if the given event is filtered or not.
 ******************************************************************************/
bool sl_ncp_evt_filter_is_filtered(uint32_t header)
{
  return evt_filter_search_in_array(header);
}

/***************************************************************************//**
 * Checks the payload length and header length.
 *
 * @param[in] cmd: Contains the len, cmd id and event.
 * @return Returns false if lengths are not valid true otherwise.
 ******************************************************************************/
static bool evt_filter_check_is_valid_user_command(user_cmd_manage_event_filter_t *cmd)
{
  bool valid = false;

  switch (cmd->hdr.id) {
    case SL_NCP_EVT_FILTER_CMD_ADD_ID:
      if (SL_NCP_EVT_FILTER_CMD_ADD_LEN == cmd->hdr.len) {
        valid = true;
      }
      break;

    case SL_NCP_EVT_FILTER_CMD_REMOVE_ID:
      if (SL_NCP_EVT_FILTER_CMD_REMOVE_LEN == cmd->hdr.len) {
        valid = true;
      }
      break;

    case SL_NCP_EVT_FILTER_CMD_RESET_ID:
      if (SL_NCP_EVT_FILTER_CMD_RESET_LEN == cmd->hdr.len) {
        valid = true;
      }
      break;

    default:
      break;
  }

  return valid;
}

/***************************************************************************//**
 * Clears the content of the event filter array.
 *
 * @return Returns ok
 ******************************************************************************/
static sl_status_t evt_filter_reset_array(void)
{
  sl_status_t status = SL_STATUS_OK;
  memset(evt_filter_array, 0, sizeof(evt_filter_array));
  evt_pos = 0;
  return status;
}

/***************************************************************************//**
 * Deletes the given event in the event filter array.
 *
 * @param[in] event
 * @return Returns ok or not_found or empty
 ******************************************************************************/
static sl_status_t evt_filter_remove_from_array(evt_filter_t event)
{
  uint8_t i, j;
  sl_status_t status = SL_STATUS_NOT_FOUND;
  uint8_t evt_pos_tmp = evt_pos;

  if (evt_pos > 0) {
    for (i = 0; i < evt_pos; i++) {
      if (event == evt_filter_array[i]) {
        evt_filter_array[i] = 0;
        status = SL_STATUS_OK;
        evt_pos -= 1;

        //move element one space ahead from the removed element
        for (j = i; j < evt_pos_tmp - 1; j++) {
          evt_filter_array[j] = evt_filter_array[j + 1];
          evt_filter_array[j + 1] = 0;
        }
        break;
      }
    }
  } else {
    status = SL_STATUS_EMPTY;
  }

  return status;
}

/***************************************************************************//**
 * Stores the given event in the event filter array.
 *
 * @param[in] event
 * @return Returns ok or full or already exists
 ******************************************************************************/
static sl_status_t evt_filter_add_to_array(evt_filter_t event)
{
  sl_status_t status = SL_STATUS_OK;

  if (evt_pos == SL_NCP_EVT_FILTER_ARRAY_LENGTH) {
    status = SL_STATUS_FULL;
  } else if (false == evt_filter_search_in_array(event)) {
    evt_filter_array[evt_pos] = event;
    evt_pos++;
  } else {
    status = SL_STATUS_ALREADY_EXISTS;
  }
  return status;
}

/***************************************************************************//**
 * Searches the given event in the event filter array.
 *
 * @param[in] event
 * @return Returns true if finds, false otherwise.
 ******************************************************************************/
static bool evt_filter_search_in_array(evt_filter_t event)
{
  uint8_t i;
  bool catch = false;

  for (i = 0; i < evt_pos; i++) {
    if (event == evt_filter_array[i]) {
      catch = true;
      break;
    }
  }
  return catch;
}
