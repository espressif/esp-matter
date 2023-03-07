/***************************************************************************//**
 * @file
 * @brief Factory Reset module
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

#include "sl_status.h"
#include "sl_bt_api.h"
#include "sl_btmesh_api.h"
#include "em_common.h"
#include "app_assert.h"
#include "sl_simple_timer.h"

#ifdef SL_COMPONENT_CATALOG_PRESENT
#include "sl_component_catalog.h"
#endif // SL_COMPONENT_CATALOG_PRESENT

#ifdef SL_CATALOG_CLI_PRESENT
#include "sl_cli.h"
#endif // SL_CATALOG_CLI_PRESENT

#include "sl_btmesh_factory_reset.h"

/***************************************************************************//**
 * @addtogroup FactoryReset
 * @{
 ******************************************************************************/

/// Callback has no parameters
#define NO_CALLBACK_DATA               (void *)NULL
/// timeout for factory init
#define FACTORY_RESET_TIMEOUT          2000

/// timer callback
static sl_simple_timer_t factory_reset_timer;

///timer callback prototype
static void factory_reset_timer_cb(sl_simple_timer_t *handle, void *data);

/*******************************************************************************
 * This function is called to initiate node reset.
 ******************************************************************************/
void sl_btmesh_initiate_node_reset()
{
  sl_status_t sc = SL_STATUS_OK;
  // Perform a factory reset of the node. This removes all the keys
  // and other settings that have been configured for this node
  sc = sl_btmesh_node_reset();
  app_assert_status_f(sc, "Failed to reset node\n");

  // Application callback on node reset
  sl_btmesh_factory_reset_on_node_reset();

  // Reboot after a small delay
  sc = sl_simple_timer_start(&factory_reset_timer,
                             FACTORY_RESET_TIMEOUT,
                             factory_reset_timer_cb,
                             NO_CALLBACK_DATA,
                             false);
  app_assert_status_f(sc, "Failed to start Factory reset timer\n");
}

/*******************************************************************************
 * This function is called to initiate full reset.
 ******************************************************************************/
void sl_btmesh_initiate_full_reset()
{
  sl_status_t sc = SL_STATUS_OK;
  // Perform a factory reset of the node. This removes all the keys
  // and other settings that have been configured for this node
  sc = sl_btmesh_node_reset();
  app_assert_status_f(sc, "Failed to reset node\n");
  // Perform a full reset by erasing PS storage.
  sc = sl_bt_nvm_erase_all();
  app_assert_status_f(sc, "Failed to erase NVM\n");

  // Application callback on full reset
  sl_btmesh_factory_reset_on_full_reset();

  // Reboot after a small delay
  sc = sl_simple_timer_start(&factory_reset_timer,
                             FACTORY_RESET_TIMEOUT,
                             factory_reset_timer_cb,
                             NO_CALLBACK_DATA,
                             false);
  app_assert_status_f(sc, "Failed to start Factory reset timer\n");
}

/*******************************************************************************
 * Handling of node reset event.
 *
 * @param[in] evt   Pointer to incoming time event.
 ******************************************************************************/
void sl_btmesh_factory_reset_on_event(sl_btmesh_msg_t *evt)
{
  sl_status_t sc = SL_STATUS_OK;
  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_btmesh_evt_node_reset_id:
      // Application callback on node reset
      sl_btmesh_factory_reset_on_node_reset();

      // Reboot after a small delay
      sc = sl_simple_timer_start(&factory_reset_timer,
                                 FACTORY_RESET_TIMEOUT,
                                 factory_reset_timer_cb,
                                 NO_CALLBACK_DATA,
                                 false);
      app_assert_status_f(sc, "Failed to start Factory reset timer\n");
      break;

    default:
      break;
  }
}

/***************************************************************************//**
 * Timer Callback
 ******************************************************************************/
static void factory_reset_timer_cb(sl_simple_timer_t *handle, void *data)
{
  (void)data;
  (void)handle;
  sl_bt_system_reset(0);
}

/***************************************************************************//**
 * CLI Callback
 * @param[in] arguments pointer to CLI arguments
 ******************************************************************************/
#ifdef SL_CATALOG_CLI_PRESENT
void factory_reset_from_cli(sl_cli_command_arg_t *arguments)
{
  (void)arguments;
  sl_btmesh_initiate_full_reset();
}

void node_reset_from_cli(sl_cli_command_arg_t *arguments)
{
  (void)arguments;
  sl_btmesh_initiate_node_reset();
}
#endif // SL_CATALOG_CLI_PRESENT

/***************************************************************************//**
 * Weak implementation of node reset callback
 ******************************************************************************/
SL_WEAK void sl_btmesh_factory_reset_on_node_reset(void)
{
}

/***************************************************************************//**
 * Weak implementation of full reset callback
 ******************************************************************************/
SL_WEAK void sl_btmesh_factory_reset_on_full_reset(void)
{
}

/** @} (end addtogroup FactoryReset) */
