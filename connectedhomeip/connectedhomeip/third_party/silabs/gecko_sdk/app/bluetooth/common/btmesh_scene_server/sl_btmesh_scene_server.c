/***************************************************************************//**
 * @file
 * @brief BT mesh scene server module
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

#include "app_assert.h"
#include "sl_btmesh_scene_server.h"

/***************************************************************************//**
 * @addtogroup Scene Server
 * @{
 ******************************************************************************/

/*******************************************************************************
 * Scenes initialization.
 * This should be called at each boot if provisioning is already done.
 * Otherwise this function should be called after provisioning is completed.
 *
 * @param[in] element  Index of the element where scenes models are initialized.
 *
 * @return Status of the initialization operation.
 *         Returns bg_err_success (0) if succeed, non-zero otherwise.
 ******************************************************************************/
uint16_t sl_btmesh_scenes_init(void)
{
  // Initialize scenes server models
  sl_status_t result;
  result = sl_btmesh_scene_server_init(BTMESH_SCENE_SERVER_MAIN);
  app_assert_status_f(result, "Failed to init scene server model\n");

  result = sl_btmesh_scene_setup_server_init(BTMESH_SCENE_SERVER_MAIN);
  app_assert_status_f(result, "Failed to init scene setup server model\n");

  return result;
}

/*******************************************************************************
 * Handling of mesh scene events.
 *
 * It is called automatically by the Universal Configurator Framework
 *
 * @param[in] evt  Pointer to incoming scene server event.
 ******************************************************************************/
void sl_btmesh_scene_server_on_event(sl_btmesh_msg_t *evt)
{
  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_btmesh_evt_node_provisioned_id:
      sl_btmesh_scenes_init();
      break;
    case sl_btmesh_evt_node_initialized_id:
      if (evt->data.evt_node_initialized.provisioned) {
        sl_btmesh_scenes_init();
      }
      break;
  }
}

/** @} (end addtogroup Scene Server) */
