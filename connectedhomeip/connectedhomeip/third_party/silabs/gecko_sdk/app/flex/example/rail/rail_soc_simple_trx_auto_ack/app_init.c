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
#include "rail.h"
#include "rail_config.h"
#include "sl_rail_util_init.h"
#include "app_process.h"
#include "sl_simple_led_instances.h"
#include "app_log.h"

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
  RAIL_Status_t status;
  // Get RAIL handle, used later by the application
  RAIL_Handle_t rail_handle = sl_rail_util_get_handle(SL_RAIL_UTIL_HANDLE_INST0);

  // Turn OFF LEDs
  sl_led_turn_off(&sl_led_led0);
#if defined(SL_CATALOG_LED1_PRESENT)
  sl_led_turn_off(&sl_led_led1);
#endif

  // Setup state timings for Auto-ACK
  RAIL_StateTiming_t timings = { 0 };
  timings.idleToTx = 100;
  timings.idleToRx = 100;
  timings.rxToTx = 192;
  // Make txToRx a little lower than desired. See documentation on RAIL_ConfigAutoAck.
  timings.txToRx = 182;
  timings.rxSearchTimeout = 0;
  timings.txToRxSearchTimeout = 0;

  status = RAIL_SetStateTiming(rail_handle, &timings);
  if (status != RAIL_STATUS_NO_ERROR) {
    app_log_warning("After RAIL_SetStateTiming() result:%d ", status);
  }

  // Setup Auto-ACK message
  RAIL_AutoAckConfig_t autoAckConfig = {
    .enable = true,
    .ackTimeout = 30000,
    // "error" param ignored
    .rxTransitions = { RAIL_RF_STATE_RX, RAIL_RF_STATE_RX },
    // "error" param ignored
    .txTransitions = { RAIL_RF_STATE_RX, RAIL_RF_STATE_RX }
  };

  uint8_t ackData[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10 };
  status = RAIL_WriteAutoAckFifo(rail_handle, ackData, sizeof(ackData));
  if (status != RAIL_STATUS_NO_ERROR) {
    app_log_warning("After RAIL_WriteAutoAckFifo() result:%d ", status);
  }
  // Enable Auto-ACK
  status = RAIL_ConfigAutoAck(rail_handle, &autoAckConfig);
  if (status != RAIL_STATUS_NO_ERROR) {
    app_log_warning("After RAIL_ConfigAutoAck() result:%d ", status);
  }

  // CLI info message
  print_sample_app_name("Simple TRX with Auto-ACK");

  return rail_handle;
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
