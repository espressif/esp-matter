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

#include "sl_btmesh_api.h"
#include "sl_btmesh_lpn.h"
#include "sl_btmesh_factory_reset.h"
#include "sl_btmesh_provisioning_decorator.h"

/***************************************************************************//**
 * @addtogroup btmesh_lpn_cb
 * @brief BT mesh Low Power Node Callbacks
 * @{
 ******************************************************************************/

/*******************************************************************************
 *  Called when the Low Power Node is initialized.
 ******************************************************************************/
void sl_btmesh_lpn_on_init(void)
{
  app_log("BT mesh LPN on\r\n");
}

/*******************************************************************************
 *  Called when the Low Power Node is deinitialized.
 ******************************************************************************/
void sl_btmesh_lpn_on_deinit(void)
{
  app_log("BT mesh LPN off\r\n");
}

/*******************************************************************************
 *  Called when the Low Power Node establishes friendship with another node
 ******************************************************************************/
void sl_btmesh_lpn_on_friendship_established(uint16_t node_address)
{
  app_log("BT mesh LPN with friend (node address: 0x%04x)\r\n", node_address);
  (void)node_address;
}

/*******************************************************************************
 *  Called when the friendship establishment attempt of Low Power Node fails
 ******************************************************************************/
void sl_btmesh_lpn_on_friendship_failed(uint16_t reason)
{
  app_log("BT mesh No friend (reason: 0x%04x)\r\n", reason);
  (void)reason;
}

/*******************************************************************************
 *  Called when friendship that was successfully established has been terminated
 ******************************************************************************/
void sl_btmesh_lpn_on_friendship_terminated(uint16_t reason)
{
  app_log("BT mesh Friend lost (reason: 0x%04x)\r\n", reason);
  (void)reason;
}

/** @} */ // btmesh_lpn_cb

/***************************************************************************//**
 * @addtogroup prov_decor_cb
 * @brief Provisioning Decorator Callbacks
 * @{
 ******************************************************************************/
/*******************************************************************************
 *  Called at node initialization time to provide provisioning information
 ******************************************************************************/
void sl_btmesh_on_provision_init_status(bool provisioned,
                                        uint16_t address,
                                        uint32_t iv_index)
{
  if (provisioned) {
    app_show_btmesh_node_provisioned(address, iv_index);
  } else {
    app_log("BT mesh node is unprovisioned, started unprovisioned beaconing...\r\n");
  }
}

/*******************************************************************************
 *  Called from sl_btmesh_on_node_provisioning_started callback in app.c
 ******************************************************************************/
void app_show_btmesh_node_provisioning_started(uint16_t result)
{
  app_log("BT mesh node provisioning is started (result: 0x%04x)\r\n",
          result);
  (void)result;
}

/*******************************************************************************
 *  Called from sl_btmesh_on_node_provisioned callback in app.c
 ******************************************************************************/
void app_show_btmesh_node_provisioned(uint16_t address, uint32_t iv_index)
{
  app_log("BT mesh node is provisioned (address: 0x%04x, iv_index: 0x%x)\r\n",
          address,
          iv_index);
  (void)address;
  (void)iv_index;
}

/*******************************************************************************
 *  Called when the Provisioning fails
 ******************************************************************************/
void sl_btmesh_on_node_provisioning_failed(uint16_t result)
{
  app_log("BT mesh node provisioning failed (result: 0x%04x)\r\n", result);
  (void)result;
}

/** @} */ // prov_decor_cb

/***************************************************************************//**
 * @addtogroup fact_rst_cb
 * @brief Factory Reset Callbacks
 * @{
 ******************************************************************************/
/*******************************************************************************
 * Called when full reset is established, before system reset
 ******************************************************************************/
void sl_btmesh_factory_reset_on_full_reset(void)
{
  app_log("Factory reset\r\n");
}

/*******************************************************************************
 * Called when node reset is established, before system reset
 ******************************************************************************/
void sl_btmesh_factory_reset_on_node_reset(void)
{
  app_log("Node reset\r\n");
}

/** @} */ // fact_rst_cb
