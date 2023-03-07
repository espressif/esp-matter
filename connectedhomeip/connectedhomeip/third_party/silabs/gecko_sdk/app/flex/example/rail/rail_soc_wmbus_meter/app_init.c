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
#include <stdlib.h>
#include "em_cmu.h"
#include "sl_rail_util_init.h"
#include "app_init.h"
#include "app_process.h"
#include "sl_simple_led_instances.h"
#include "sl_wmbus_support.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
/// Which rx channel will be used, it will be calculated from the TX channel
uint16_t rx_channel = 0U;

/// Access number for the WMBus protocol
uint8_t access_number = 0U;

/// Time for calculation for the proper sending timing
extern RAIL_Time_t last_tx_start_time;

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
/// Security key for the app
static const uint8_t crypto_key[] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                                      0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff };
// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/******************************************************************************
 * The function is used for some basic initialization related to the app.
 *****************************************************************************/
RAIL_Handle_t app_init(void)
{
  // Get RAIL handle, used later by the application
  RAIL_Handle_t rail_handle = sl_rail_util_get_handle(SL_RAIL_UTIL_HANDLE_INST0);

  // Initialization of WMBUS
  WMBUS_Init();

  // Get the mode of WMBUS
  WMBUS_Mode_t mode = WMBUS_GetSelectedMode();

  sl_led_turn_off(&sl_led_led0);
  sl_led_turn_off(&sl_led_led1);

  if ( get_wmbus_accessibility() == WMBUS_ACCESSIBILITY_UNLIMITED_ACCESS ) {
    RAIL_StateTransitions_t transitions = {
      .error = RAIL_RF_STATE_RX,
      .success = RAIL_RF_STATE_RX,
    };
    RAIL_SetRxTransitions(rail_handle, &transitions);
  }

  if ( mode == WMBUS_MODE_T_METER || mode == WMBUS_MODE_T_COLLECTOR || mode == WMBUS_MODE_C ) {
    rx_channel = TX_CHANNEL + 1; //asymmetric config, rx will be on a different channel
  } else {
    rx_channel = TX_CHANNEL;
  }

  //calibrate IR on rx channel before starting the main loop, since it takes longer than most rx operation
  RAIL_PrepareChannel(rail_handle, rx_channel);
  RAIL_CalibrateIr(rail_handle, NULL);

  WMBUSframe_crypto5Init();
  WMBUSframe_crypto5SetKey(crypto_key);

  access_number = (uint8_t)(rand() % 256);

  set_next_state(S_SCHEDULE_TX);
  last_tx_start_time = RAIL_GetTime();

  return rail_handle;
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
