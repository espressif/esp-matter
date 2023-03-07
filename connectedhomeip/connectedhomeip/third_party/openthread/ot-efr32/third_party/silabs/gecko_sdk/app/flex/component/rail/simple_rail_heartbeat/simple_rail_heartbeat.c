/***************************************************************************//**
 * @file
 * @brief simple_rail_heartbeat.c
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
#include "app_assert.h"
#include "app_log.h"
#include "simple_rail_tx.h"
#include "sl_sleeptimer.h"
#include "simple_rail_heartbeat_config.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
/**************************************************************************//**
 * Sleeptimer callback for the heartbeat functionality. Periodically:
 * - updates tx fifo to be sent, and
 * - starts RAIL transmission
 *
 * @param handle Not used
 * @param data Not used
 * @returns None
 *****************************************************************************/
static void simple_rail_heartbeat_callback(sl_sleeptimer_timer_handle_t *handle, void *data);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
/// Periodic Timer Handle
static sl_sleeptimer_timer_handle_t hb_radio_timer_handle;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/******************************************************************************
 * The function is used for start a periodic sleeptimer for heartbeat.
 *****************************************************************************/
void sl_simple_rail_heartbeat_init(void)
{
  sl_status_t status = sl_sleeptimer_start_periodic_timer_ms(&hb_radio_timer_handle,
                                                             SIMPLE_RAIL_HEARTBEAT_TIME_PERIOD_MS,
                                                             simple_rail_heartbeat_callback,
                                                             NULL,
                                                             SIMPLE_RAIL_HEARTBEAT_TIMER_PRIORITY,
                                                             0);
  app_assert_status_f(status,
                      "[E: 0x%04x]: Failed to start periodic sleeptimer\n",
                      (int)status);
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
/******************************************************************************
 * The heartbeat radio logic is implemented in this sleeptimer callback
 *****************************************************************************/
static void simple_rail_heartbeat_callback(sl_sleeptimer_timer_handle_t *handle, void *data)
{
  // Eliminate compiler warnings
  (void) handle;
  (void) data;

  // Status of RAIL transmit request
  sl_status_t transmit_status;
  // Heartbeat number, incremented periodically
  static uint8_t heartbeat_num = 0;

  // Update 1st byte of tx fifo (heartbeat_num), and start transmission
  transmit_status = sl_simple_rail_tx_transmit(&heartbeat_num, sizeof(heartbeat_num));
  if (transmit_status != SL_STATUS_OK) {
    app_log_warning("[E: 0x%04x]: Heartbeat transmit request failed",
                    (int) transmit_status);
  }
  heartbeat_num++;
}
