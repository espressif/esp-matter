/***************************************************************************//**
 * @file
 * @brief Application Output Log code
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

#include <stdbool.h>
#include "em_common.h"
#include "sl_status.h"

#include "app.h"
#include "app_log.h"

#include "sl_bt_api.h"
#include "sl_btmesh_api.h"
#include "sl_btmesh_friend.h"
#include "sl_btmesh_ctl_server.h"
#include "sl_btmesh_lighting_server.h"
#include "sl_btmesh_provisioning_decorator.h"
#include "sl_btmesh_factory_reset.h"

#define LIGHTNESS_LEVEL_TO_PERCENTAGE(lev) ((((lev) * 100) + 99) / 65535)

// -----------------------------------------------------------------------------
// BT mesh Friend Node Callbacks

/*******************************************************************************
 * Called when the Friend Node establishes friendship with another node.
 *
 * @param[in] netkey_index Index of the network key used in friendship
 * @param[in] lpn_address Low Power Node address
 *
 ******************************************************************************/
void sl_btmesh_friend_on_friendship_established(uint16_t netkey_index,
                                                uint16_t lpn_address)
{
  app_log("BT mesh Friendship established with LPN "
          "(netkey idx: %d, lpn addr: 0x%04x)\r\n",
          netkey_index,
          lpn_address);
  (void)netkey_index;
  (void)lpn_address;
}

/*******************************************************************************
 * Called when the friendship that was successfully established with a Low Power
 * Node has been terminated.
 *
 * @param[in] netkey_index Index of the network key used in friendship
 * @param[in] lpn_address Low Power Node address
 * @param[in] reason Reason for friendship termination
 *
 ******************************************************************************/
void sl_btmesh_friend_on_friendship_terminated(uint16_t netkey_index,
                                               uint16_t lpn_address,
                                               uint16_t reason)
{
  app_log("BT mesh Friendship terminated with LPN "
          "(netkey idx: %d, lpn addr: 0x%04x, reason: 0x%04x)\r\n",
          netkey_index,
          lpn_address,
          reason);
  (void)netkey_index;
  (void)lpn_address;
  (void)reason;
}

// -----------------------------------------------------------------------------
// BT mesh CTL Callbacks

/*******************************************************************************
 * Called when the UI shall be updated with the changed CTL Model state during
 * a transition. The rate of this callback can be controlled by changing the
 * SL_BTMESH_CTL_SERVER_UI_UPDATE_PERIOD_CFG_VAL macro.
 *
 * @param[in] temperature Temperature of color.
 * @param[in] deltauv     Delta UV value.
 *
 ******************************************************************************/
void sl_btmesh_ctl_on_ui_update(uint16_t temperature,
                                uint16_t deltauv)
{
  char deltauv_str[8] = { 0 };

  sl_btmesh_ctl_server_snprint_deltauv(deltauv_str,
                                       sizeof(deltauv_str),
                                       deltauv);
  app_log("BT mesh CTL Color Temperature: %5uK\r\n", temperature);
  app_log("BT mesh CTL Delta UV: %6s\r\n", deltauv_str);
  (void)temperature;
}

// -----------------------------------------------------------------------------
// BT mesh Lightning Server Callbacks

/*******************************************************************************
 * Called when the UI shall be updated with the changed state of
 * lightning server during a transition. The rate of this callback can be
 * controlled by changing the SL_BTMESH_LIGHTING_SERVER_UI_UPDATE_PERIOD_CFG_VAL macro.
 *
 * @param[in] lightness_level lightness level (0x0001 - FFFE)
 *
 ******************************************************************************/
void sl_btmesh_lighting_server_on_ui_update(uint16_t lightness_level)
{
  app_log("BT mesh Lightness: %5u%%\r\n",
          LIGHTNESS_LEVEL_TO_PERCENTAGE(lightness_level));
  (void)lightness_level;
}

// -----------------------------------------------------------------------------
// Provisioning Decorator Callbacks

/*******************************************************************************
 * Called at node initialization time to provide provisioning information
 *
 * @param[in] provisioned  true: provisioned, false: unprovisioned
 * @param[in] address      Unicast address of the primary element of the node.
                           Ignored if unprovisioned.
 * @param[in] iv_index     IV index for the first network of the node
                           Ignored if unprovisioned.
 *
 ******************************************************************************/
void sl_btmesh_on_provision_init_status(bool provisioned,
                                        uint16_t address,
                                        uint32_t iv_index)
{
  if (provisioned) {
    app_show_btmesh_node_provisioned(address, iv_index);
  } else {
    app_log("BT mesh node is unprovisioned, "
            "started unprovisioned beaconing...\r\n");
  }
}

// -----------------------------------------------------------------------------
// Provisioning Decorator Callbacks

/*******************************************************************************
 * Called when the Provisioning starts
 *
 * @param[in] result  Result code. 0: success, non-zero: error
 *
 ******************************************************************************/
void app_show_btmesh_node_provisioning_started(uint16_t result)
{
  app_log("BT mesh node provisioning is started (result: 0x%04x)\r\n",
          result);
  (void)result;
}

/*******************************************************************************
 * Called when the Provisioning finishes successfully
 *
 * @param[in] address      Unicast address of the primary element of the node.
                           Ignored if unprovisioned.
 * @param[in] iv_index     IV index for the first network of the node
                           Ignored if unprovisioned.
 *
 ******************************************************************************/
void app_show_btmesh_node_provisioned(uint16_t address,
                                      uint32_t iv_index)
{
  app_log("BT mesh node is provisioned (address: 0x%04x, iv_index: 0x%x)\r\n",
          address,
          iv_index);
  (void)address;
  (void)iv_index;
}

/*******************************************************************************
 * Called when the Provisioning fails
 *
 * @param[in] result  Result code. 0: success, non-zero: error
 *
 ******************************************************************************/
void sl_btmesh_on_node_provisioning_failed(uint16_t result)
{
  app_log("BT mesh node provisioning failed (result: 0x%04x)\r\n", result);
  (void) result;
}

// -----------------------------------------------------------------------------
// Factory Reset Callback

/*******************************************************************************
 * Shows the node reset information
 ******************************************************************************/
void app_show_btmesh_node_reset(void)
{
  app_log("Node reset\r\n");
}

/*******************************************************************************
 * Called when full reset is established, before system reset
 ******************************************************************************/
void sl_btmesh_factory_reset_on_full_reset(void)
{
  app_log("Factory reset\r\n");
}
