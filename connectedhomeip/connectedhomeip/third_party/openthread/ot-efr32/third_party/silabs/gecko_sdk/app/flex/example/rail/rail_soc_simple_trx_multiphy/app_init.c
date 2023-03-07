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
#include "app_init.h"

#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "app_task_init.h"
#endif

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
/**************************************************************************//**
 * The function calculates scan times based on the Radio Configuration
 *
 * @param None
 * @returns None
 *****************************************************************************/
static inline void set_scan_times(void);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
/// The variable shows whether only one channel is used for rx, or both (scan mode)
extern volatile bool scan_mode;

/// Scan times shall be adaptively set based on the actual Radio Configuration settings
RAIL_Time_t scan_times[ACTIVE_CHANNEL_CONFIG_NUMS];

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
  // Get RAIL handle, used later by the application
  RAIL_Handle_t rail_handle = sl_rail_util_get_handle(SL_RAIL_UTIL_HANDLE_INST0);

  // Turn OFF LEDs
  sl_led_turn_off(&sl_led_led0);
  sl_led_turn_off(&sl_led_led1);

  // CLI info message
  print_sample_app_name("Simple TRX Multiphy");

  set_scan_times();

  app_log_info("Scantimes configured to:\n");
  uint32_t preamble_us = 0;
  for ( int i = 0; i < ACTIVE_CHANNEL_CONFIG_NUMS; i++) {
    app_log_info("ch%d: %ldus; ", i, scan_times[i]);
    preamble_us += scan_times[i];
  }
  app_log_info("\n");

  // Start scan mode
  set_next_state(S_START_RECEIVING);
  scan_mode = true;

  return rail_handle;
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
/******************************************************************************
 * The function calculates scan times based on the Radio Configuration
 *****************************************************************************/
static inline void set_scan_times(void)
{
  RAIL_Handle_t rail_handle = sl_rail_util_get_handle(SL_RAIL_UTIL_HANDLE_INST0);
  for (uint16_t i = 0; i < ACTIVE_CHANNEL_CONFIG_NUMS; ++i) {
    RAIL_PrepareChannel(rail_handle, i);
    uint32_t bitrate = RAIL_GetBitRate(rail_handle);
    scan_times[i] = (((PREAMBLE_LENGTH * PREAMBLE_OVERSAMPLING * MICRO_SEC) / bitrate) + CHANGING_TIME_IN_MICROSEC);
  }
}
