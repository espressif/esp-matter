/***************************************************************************//**
 * @file
 * @brief BT Mesh Time Server module
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

#include "em_common.h"
#include "sl_status.h"
#include "sl_bt_api.h"
#include "sl_btmesh_api.h"
#include "sl_btmesh_dcd.h"

#ifdef SL_COMPONENT_CATALOG_PRESENT
#include "sl_component_catalog.h"
#endif // SL_COMPONENT_CATALOG_PRESENT

#ifdef SL_CATALOG_APP_LOG_PRESENT
#include "app_log.h"
#endif // SL_CATALOG_APP_LOG_PRESENT

#include "sl_btmesh_time_server.h"
#include "sl_btmesh_time_server_config.h"

// Warning! The app_btmesh_util shall be included after the component configuration
// header file in order to provide the component specific logging macro.
#include "app_btmesh_util.h"

/***************************************************************************//**
 * @addtogroup Time Server
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * Time initialization.
 * This should be called at each boot if provisioning is already done.
 * Otherwise this function should be called after provisioning is completed.
 *
 * @return Status of the initialization operation.
 *         Returns bg_err_success (0) if succeed, non-zero otherwise.
 ******************************************************************************/
uint16_t sl_btmesh_time_init(void)
{
  // Initialize time server models
  sl_status_t result = sl_btmesh_time_server_init(BTMESH_TIME_SERVER_MAIN);
  if (result) {
    log("sl_btmesh_time_server_init failed, code 0x%x\r\n", result);
  }
  return result;
}

/***************************************************************************//**
 * Handling of time server time updated event.
 *
 * @param[in] evt  Pointer to time server time updated event.
 ******************************************************************************/
static void handle_time_server_time_updated_event(
  sl_btmesh_evt_time_server_time_updated_t *evt)
{
  log("evt:gecko_evt_mesh_time_server_time_updated_id, tai_seconds=0x");
  // print only 40 bits as this is size of TAI Seconds
  log("%2.2x", (uint8_t)((evt->tai_seconds >> 32) & 0xFF));
  log("%4.4x", (uint16_t)((evt->tai_seconds >> 16) & 0xFFFF));
  log("%4.4x, ", (uint16_t)(evt->tai_seconds & 0xFFFF));
  log("subsecond=%u, uncertainty=%u ms, time_authority=%u, tai_utc_delta=%ld, \
time_zone_offset=%d\r\n",
      evt->subsecond,
      (uint16_t)evt->uncertainty * 10,
      evt->time_authority,
      evt->tai_utc_delta,
      evt->time_zone_offset);
  (void)evt;
}

/***************************************************************************//**
 * Handling of time server time zone offset updated event.
 *
 * @param[in] evt  Pointer to time server time zone offset updated event.
 ******************************************************************************/
static void handle_time_server_time_zone_offset_updated_event(
  sl_btmesh_evt_time_server_time_zone_offset_updated_t *evt)
{
  log("evt:gecko_evt_mesh_time_server_time_zone_offset_updated_id, \
zone_offset_current=%d, zone_offset_new=%d, tai_of_zone_change=0x",
      evt->time_zone_offset_current,
      evt->time_zone_offset_new);
  // print only 40 bits as this is size of TAI of Zone Change
  log("%2.2x", (uint8_t)((evt->tai_of_zone_change >> 32) & 0xFF));
  log("%4.4x", (uint16_t)((evt->tai_of_zone_change >> 16) & 0xFFFF));
  log("%4.4x", (uint16_t)(evt->tai_of_zone_change & 0xFFFF));
  log("\r\n");
  (void)evt;
}

/***************************************************************************//**
 * Handling of time server tai utc delta updated event.
 *
 * @param[in] evt  Pointer to time server tai utc delta updated event.
 ******************************************************************************/
static void handle_time_server_tai_utc_delta_updated_event(
  sl_btmesh_evt_time_server_tai_utc_delta_updated_t *evt)
{
  log("evt:gecko_evt_mesh_time_server_tai_utc_delta_updated_id, \
tai_utc_delta_current=%ld, tai_utc_delta_new=%ld, tai_of_delta_change=0x",
      evt->tai_utc_delta_current,
      evt->tai_utc_delta_new);
  // print only 40 bits as this is size of TAI of Delta Change
  log("%2.2x", (uint8_t)((evt->tai_of_delta_change >> 32) & 0xFF));
  log("%4.4x", (uint16_t)((evt->tai_of_delta_change >> 16) & 0xFFFF));
  log("%4.4x", (uint16_t)(evt->tai_of_delta_change & 0xFFFF));
  log("\r\n");
  (void)evt;
}

/***************************************************************************//**
 * Handling of time server time role updated event.
 *
 * @param[in] evt  Pointer to time server time role updated event.
 ******************************************************************************/
static void handle_time_server_time_role_updated_event(
  sl_btmesh_evt_time_server_time_role_updated_t *evt)
{
  log("evt:gecko_evt_mesh_time_server_time_role_updated_id, time_role=");
  switch (evt->time_role) {
    case sl_btmesh_time_client_time_role_none:
      log("None");
      break;

    case sl_btmesh_time_client_time_role_authority:
      log("Authority");
      break;

    case sl_btmesh_time_client_time_role_relay:
      log("Relay");
      break;

    case sl_btmesh_time_client_time_role_client:
      log("Client");
      break;

    default:
      break;
  }
  log("\r\n");
}

/*******************************************************************************
 * Handling of mesh time events.
 * It handles:
 *  - time_server_time_updated
 *  - time_server_time_zone_offset_updated
 *  - time_server_tai_utc_delta_updated
 *  - time_server_time_role_updated
 *
 * @param[in] evt  Pointer to incoming time event.
 ******************************************************************************/
void sl_btmesh_time_server_on_event(sl_btmesh_msg_t *evt)
{
  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_btmesh_evt_time_server_time_updated_id:
      handle_time_server_time_updated_event(
        &(evt->data.evt_time_server_time_updated));
      break;

    case sl_btmesh_evt_time_server_time_zone_offset_updated_id:
      handle_time_server_time_zone_offset_updated_event(
        &(evt->data.evt_time_server_time_zone_offset_updated));
      break;

    case sl_btmesh_evt_time_server_tai_utc_delta_updated_id:
      handle_time_server_tai_utc_delta_updated_event(
        &(evt->data.evt_time_server_tai_utc_delta_updated));
      break;

    case sl_btmesh_evt_time_server_time_role_updated_id:
      handle_time_server_time_role_updated_event(
        &(evt->data.evt_time_server_time_role_updated));
      break;

    case sl_btmesh_evt_node_initialized_id:
      if (evt->data.evt_node_initialized.provisioned) {
        sl_btmesh_time_init();
      }
      break;

    case sl_btmesh_evt_node_provisioned_id:
      sl_btmesh_time_init();
      break;

    default:
      break;
  }
}

/** @} (end addtogroup Time Server) */
