/***************************************************************************//**
 * @file
 * @brief NCP reset module.
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "ncp_reset.h"
#include "sl_bt_api.h"
#include "app_log.h"
#include "sl_simple_timer.h"
#include "ncp_host.h"
#include "app_assert.h"

#define MAX_NCP_BOOT_N 5
#define BOOT_DELAY_MS 1000

static void ncp_reset_on_boot_timer_expire(sl_simple_timer_t *timer, void *data);

static bool ncp_booted;
static uint8_t ncp_boot_retries;
static sl_simple_timer_t boot_timer;

/**************************************************************************//**
 * Ble event callback
 *****************************************************************************/
sl_status_t ncp_reset_on_event(sl_bt_msg_t *evt)
{
  sl_status_t ret_val;
  // Catch boot event...
  if (ncp_booted || SL_BT_MSG_ID(evt->header) == sl_bt_evt_system_boot_id) {
    if (!ncp_booted) {
      sl_simple_timer_stop(&boot_timer);
      app_log_info("NCP booted." APP_LOG_NL);
      ncp_booted = true;
    }
    ret_val = SL_STATUS_OK;
  } else {
    ret_val = SL_STATUS_BUSY;
  }

  return ret_val;
}

/**************************************************************************//**
 * Reset NCP target
 *****************************************************************************/
void ncp_reset(void)
{
  sl_status_t sc;

  ncp_booted = false;
  ncp_boot_retries = 0;

  app_log_info("Resetting NCP target..." APP_LOG_NL);
  sc = ncp_host_flush_data();
  app_assert_status(sc);

  sl_bt_system_reset(sl_bt_system_boot_mode_normal);

  sc = sl_simple_timer_start(&boot_timer, BOOT_DELAY_MS, ncp_reset_on_boot_timer_expire, NULL, true);
  app_assert_status(sc);
}

/**************************************************************************//**
 * Boot timer callback.
 *****************************************************************************/
static void ncp_reset_on_boot_timer_expire(sl_simple_timer_t *timer, void *data)
{
  sl_status_t sc;

  (void)timer;
  (void)data;

  if (ncp_boot_retries < MAX_NCP_BOOT_N) {
    app_log_info("Resetting NCP target..." APP_LOG_NL);
    sc = ncp_host_flush_data();
    app_assert_status(sc);
    sl_bt_system_reset(sl_bt_system_boot_mode_normal);
    ncp_boot_retries++;
  } else {
    sl_simple_timer_stop(&boot_timer);
    app_assert(false, "Failed to reset NCP. Giving up.");
  }
}
