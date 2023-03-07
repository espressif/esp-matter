/***************************************************************************//**
 * @file
 * @brief Air quality sensor
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

#include <stddef.h>
#include <stdbool.h>
#include "sl_board_control.h"
#include "sl_ccs811.h"
#include "sl_sensor_select.h"
#include "app_assert.h"
#include "sl_sensor_gas.h"

// -----------------------------------------------------------------------------
// Configuration

#define GAS_MEASURE_MODE  CCS811_MEASURE_MODE_DRIVE_MODE_1SEC

// -----------------------------------------------------------------------------
// Private variables

static bool initialized = false;

// -----------------------------------------------------------------------------
// Public function definitions

sl_status_t sl_sensor_gas_init(void)
{
  sl_status_t sc;
  sl_i2cspm_t *gas_sensor = sl_sensor_select(SL_BOARD_SENSOR_GAS);
  sc = sl_board_enable_sensor(SL_BOARD_SENSOR_GAS);
  app_assert((SL_STATUS_OK == sc) && (NULL != gas_sensor),
             "[E: %#04x] Gas sensor not available\n",
             sc);
  sc = sl_ccs811_init(gas_sensor);
  if (sc == SL_STATUS_OK) {
    sc = sl_ccs811_set_measure_mode(gas_sensor, GAS_MEASURE_MODE);
    app_assert_status(sc);
    initialized = true;
  } else {
    initialized = false;
  }

  return sc;
}

void sl_sensor_gas_deinit(void)
{
  (void)sl_board_disable_sensor(SL_BOARD_SENSOR_GAS);
  initialized = false;
}

sl_status_t sl_sensor_gas_get(uint16_t *eco2, uint16_t *tvoc)
{
  sl_status_t sc = SL_STATUS_NOT_READY;

  if (initialized) {
    sl_i2cspm_t *gas_sensor = sl_sensor_select(SL_BOARD_SENSOR_GAS);
    if (sl_ccs811_is_data_available(gas_sensor)) {
      sc = sl_ccs811_get_measurement(gas_sensor, eco2, tvoc);
    }
  } else {
    sc = SL_STATUS_NOT_INITIALIZED;
  }

  return sc;
}
