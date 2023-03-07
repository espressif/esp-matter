/***************************************************************************//**
 * @file
 * @brief app_bluetooth.c
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
#include "sl_bluetooth.h"
#include "printf.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/**************************************************************************//**
 * Bluetooth stack event handler.
 * This overrides the dummy weak implementation.
 *****************************************************************************/
void sl_bt_on_event(sl_bt_msg_t* evt)
{
  /// Flag for indicating DFU Reset must be performed.
  bool boot_to_dfu = false;
  /// Bluetooth status indicator
  sl_status_t bt_status = 0;
  /// Bluetooth advertising set
  uint8_t adv_handle = 0xFF;

  // Handle stack events
  switch (SL_BT_MSG_ID(evt->header)) {
    ///////////////////////////////////////////////////////////////////////////
    // This event indicates the device has started and the radio is ready.   //
    // Do not call any stack command before receiving this boot event!       //
    ///////////////////////////////////////////////////////////////////////////
    case sl_bt_evt_system_boot_id:
      // Create a bluetooth advertising set
      bt_status = sl_bt_advertiser_create_set(&adv_handle);
      // Generate the advertising data from the GATT configurator
      bt_status = sl_bt_legacy_advertiser_generate_data(adv_handle,
                                                        sl_bt_advertiser_general_discoverable);
      // Set advertising parameters. 100ms advertisement interval.
      // The first parameter is advertising set handle
      // The next two parameters are minimum and maximum advertising
      // interval, both in units of (milliseconds * 1.6).
      // The last two parameters are duration and maxevents left as default..
      bt_status = sl_bt_advertiser_set_timing(0, 160, 160, 0, 0);
      // Start general advertising and enable connections.
      bt_status = sl_bt_legacy_advertiser_start(adv_handle,
                                                sl_bt_legacy_advertiser_connectable);
      break;

    ///////////////////////////////////////////////////////////////////////////
    // This event indicates that a connection was closed.                    //
    ///////////////////////////////////////////////////////////////////////////
    case sl_bt_evt_connection_closed_id:
      // Check if need to boot to OTA DFU mode
      if (boot_to_dfu) {
        // Enter to OTA DFU mode
        sl_bt_system_reset(2);
      } else {
        // Restart advertising after client has disconnected
        bt_status = sl_bt_legacy_advertiser_start(0,
                                                  sl_bt_legacy_advertiser_connectable);
      }
      break;

    ///////////////////////////////////////////////////////////////////////////
    // Add additional event handlers here as your application requires!      //
    ///////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////
    // Default event handler.                                                //
    ///////////////////////////////////////////////////////////////////////////
    default:
      break;
  }

  if (bt_status != SL_STATUS_OK) {
    printf("bt_status returned with failer error code: %04X\n", bt_status);
  }
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
