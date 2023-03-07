/***************************************************************************//**
 * @file
 * @brief Relative Humidity and Temperature sensor
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
#include "sl_board_control.h"
#include "sl_si70xx.h"
#include "sl_sensor_select.h"
#include "app_assert.h"
#include "sl_sensor_rht.h"

// -----------------------------------------------------------------------------
// Configuration

#define RHT_ADDRESS  SI7021_ADDR

// -----------------------------------------------------------------------------
// Private variables

static bool initialized = false;

// -----------------------------------------------------------------------------
// Public function definitions

sl_status_t sl_sensor_rht_init(void)
{
  sl_status_t sc;
  sl_i2cspm_t *rht_sensor = sl_sensor_select(SL_BOARD_SENSOR_RHT);
  (void)sl_board_enable_sensor(SL_BOARD_SENSOR_RHT);
  app_assert(NULL != rht_sensor,
             "Si70xx sensor not available\n");
  sc = sl_si70xx_init(rht_sensor, RHT_ADDRESS);
  if (SL_STATUS_OK == sc) {
    initialized = true;
  } else {
    initialized = false;
  }
  return sc;
}

void sl_sensor_rht_deinit(void)
{
  (void)sl_board_disable_sensor(SL_BOARD_SENSOR_RHT);
  initialized = false;
}

sl_status_t sl_sensor_rht_get(uint32_t *rh, int32_t *t)
{
  sl_status_t sc;

  if (initialized) {
    sl_i2cspm_t *rht_sensor = sl_sensor_select(SL_BOARD_SENSOR_RHT);
    sc = sl_si70xx_measure_rh_and_temp(rht_sensor, RHT_ADDRESS, rh, t);
  } else {
    sc = SL_STATUS_NOT_INITIALIZED;
  }

  return sc;
}
