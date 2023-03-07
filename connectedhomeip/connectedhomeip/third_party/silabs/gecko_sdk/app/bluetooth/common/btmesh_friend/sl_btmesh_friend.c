/***************************************************************************//**
 * @file
 * @brief Friend implementation
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
#include <stdio.h>

#include "em_common.h"
#include "sl_status.h"
#include "sl_bt_api.h"
#include "sl_btmesh_api.h"

#ifdef SL_COMPONENT_CATALOG_PRESENT
#include "sl_component_catalog.h"
#endif // SL_COMPONENT_CATALOG_PRESENT

#ifdef SL_CATALOG_APP_LOG_PRESENT
#include "app_log.h"
#endif // SL_CATALOG_APP_LOG_PRESENT

#include "sl_btmesh_friend.h"
#include "sl_btmesh_friend_config.h"

// Warning! The app_btmesh_util shall be included after the component configuration
// header file in order to provide the component specific logging macro.
#include "app_btmesh_util.h"

/***************************************************************************//**
 * @addtogroup friend
 * @{
 ******************************************************************************/

/*******************************************************************************
 * Initialize LPN functionality with configuration and friendship establishment.
 ******************************************************************************/
void sl_btmesh_friend_feature_init(void)
{
  sl_status_t result = 0;

  //Initialize Friend functionality
  log("Friend mode initialization\r\n");
  result = sl_btmesh_friend_init();
  if (result) {
    log("Friend initialization failed 0x%lx\r\n", result);
  }
}

/*******************************************************************************
 *  Handling of mesh friend events.
 *
 *  @param[in] evt  Pointer to incoming friend event.
 ******************************************************************************/
void sl_btmesh_friend_on_event(sl_btmesh_msg_t *evt)
{
  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_btmesh_evt_node_initialized_id:
      if (evt->data.evt_node_initialized.provisioned) {
        sl_btmesh_friend_feature_init();
      }
      break;

    case sl_btmesh_evt_friend_friendship_established_id:
      sl_btmesh_friend_on_friendship_established(
        evt->data.evt_friend_friendship_established.netkey_index,
        evt->data.evt_friend_friendship_established.lpn_address);
      break;

    case sl_btmesh_evt_friend_friendship_terminated_id:
      sl_btmesh_friend_on_friendship_terminated(
        evt->data.evt_friend_friendship_terminated.netkey_index,
        evt->data.evt_friend_friendship_terminated.lpn_address,
        evt->data.evt_friend_friendship_terminated.reason);
      break;

    case sl_btmesh_evt_node_provisioned_id:
      sl_btmesh_friend_feature_init();
      break;

    default:
      break;
  }
}

// Weak implementation of Callbacks
SL_WEAK void sl_btmesh_friend_on_friendship_established(uint16_t netkey_index,
                                                        uint16_t lpn_address)
{
  (void) netkey_index;
  (void) lpn_address;
}

SL_WEAK void sl_btmesh_friend_on_friendship_terminated(uint16_t netkey_index,
                                                       uint16_t lpn_address,
                                                       uint16_t reason)
{
  (void) netkey_index;
  (void) lpn_address;
  (void) reason;
}

/** @} (end addtogroup friend) */
