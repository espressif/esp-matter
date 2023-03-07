/***************************************************************************//**
 * @file
 * @brief Core application logic.
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
#include "app_log.h"
#include "app_assert.h"
#include "sl_bluetooth.h"
#include "app.h"
#include "dtm.h"
#include "sl_iostream_init_usart_instances.h"
#include "sl_board_control.h"

enum signal{
  signal_testmode_command_ready = 1,
};

// IOStream instance used for communication with the DTM tester equipment
sl_iostream_t *dtm_iostream_handle;

void app_write_response(uint8_t data)
{
  sl_iostream_putchar(dtm_iostream_handle, data);
}

static const testmode_config_t testmode_config = {
  .write_response_byte = app_write_response,
  .get_ticks = sl_sleeptimer_get_tick_count,
  .ticks_per_second = 32768,
  .command_ready_signal = signal_testmode_command_ready,
};

/**************************************************************************//**
 * Application Init.
 *****************************************************************************/
SL_WEAK void app_init(void)
{
  app_log_info("soc_dtm initialised\n");

  /////////////////////////////////////////////////////////////////////////////
  // Put your additional application init code here!                         //
  // This is called once during start-up.                                    //
  /////////////////////////////////////////////////////////////////////////////

  // Default IOStream instance is used for communication with the DTM tester
  // equipment.
  dtm_iostream_handle = sl_iostream_get_default();

  // Enable VCOM
  sl_board_enable_vcom();

  testmode_init(&testmode_config);
}

/**************************************************************************//**
 * Application Process Action.
 *****************************************************************************/
SL_WEAK void app_process_action(void)
{
  /////////////////////////////////////////////////////////////////////////////
  // Put your additional application code here!                              //
  // This is called infinitely.                                              //
  // Do not call blocking functions from here!                               //
  /////////////////////////////////////////////////////////////////////////////

  uint8_t data = 0;

  if (SL_STATUS_OK == sl_iostream_read(dtm_iostream_handle, &data, 1, NULL)) {
    testmode_process_command_byte(data);
  }
}

/**************************************************************************//**
 * Bluetooth stack event handler.
 * This overrides the dummy weak implementation.
 *
 * @param[in] evt Event coming from the Bluetooth stack.
 *****************************************************************************/
void sl_bt_on_event(sl_bt_msg_t *evt)
{
  testmode_handle_gecko_event(evt);
}
