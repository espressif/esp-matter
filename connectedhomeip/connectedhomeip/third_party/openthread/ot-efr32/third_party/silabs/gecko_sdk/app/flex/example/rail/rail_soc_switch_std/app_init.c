/***************************************************************************//**
 * @file
 * @brief app_init.c
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include <stdint.h>

#include "sl_component_catalog.h"
#include "em_system.h"
#include "sl_component_catalog.h"
#include "rail.h"
#include "sl_simple_led_instances.h"
#include "app_log.h"
#include "app_process.h"
#ifdef SL_CATALOG_FLEX_IEEE802154_SUPPORT_PRESENT
  #include "sl_flex_util_802154_init.h"
  #include  "sl_flex_ieee802154_support.h"
#elif defined SL_CATALOG_FLEX_BLE_SUPPORT_PRESENT
  #include "sl_flex_util_ble_init.h"
#else
#endif

#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "app_task_init.h"
#endif

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
#ifdef SL_CATALOG_FLEX_IEEE802154_SUPPORT_PRESENT
extern sl_flex_ieee802154_status_t comm_status;
extern sl_flex_ieee802154_frame_t tx_frame;
#endif
// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/******************************************************************************
 * Print sample app name
 *****************************************************************************/
SL_WEAK void print_sample_app_name(const char* app_name)
{
  app_log_info("%s\n", app_name);
}

/******************************************************************************
 * The function is used for some basic initialization related to the app.
 *****************************************************************************/
RAIL_Handle_t app_init(void)
{
  RAIL_Status_t status = RAIL_STATUS_NO_ERROR;
  // Get RAIL handle, used later by the application
  RAIL_Handle_t rail_handle = sl_flex_util_get_handle();

  // sets the RAIL TX FIFO
  (void)app_set_rail_tx_fifo(rail_handle);

#ifdef SL_CATALOG_FLEX_IEEE802154_SUPPORT_PRESENT
  // init the selected protocol for IEEE
  sl_flex_ieee802154_protocol_init(rail_handle, SL_FLEX_UTIL_INIT_PROTOCOL_INSTANCE_DEFAULT);
  // updates the status for changes
  sl_flex_ieee802154_update_status(&comm_status, &tx_frame);
#endif

  // Start reception
#ifdef SL_CATALOG_FLEX_IEEE802154_SUPPORT_PRESENT
  status = RAIL_StartRx(rail_handle, sl_flex_ieee802154_get_channel(), NULL);
#elif defined SL_CATALOG_FLEX_BLE_SUPPORT_PRESENT
  status = RAIL_StartRx(rail_handle, BLE_CHANNEL, NULL);
#else
#endif
  if (status != RAIL_STATUS_NO_ERROR) {
    app_log_warning("After initialization RAIL_StartRx() result:%d \n", status);
  }

  // Turn OFF LEDs
  sl_led_turn_off(&sl_led_led0);
  sl_led_turn_off(&sl_led_led1);
  init_display();

  // CLI info message
#ifdef SL_CATALOG_FLEX_IEEE802154_SUPPORT_PRESENT
  print_sample_app_name("Switch Standard on IEEE 802.15.4");
#elif defined SL_CATALOG_FLEX_BLE_SUPPORT_PRESENT
  print_sample_app_name("Switch Standard on BLE");
#else
  print_sample_app_name("No STD(IEEE 802.15.4 or BLE) starts");
#endif
  return rail_handle;
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
