/***************************************************************************//**
 * @file
 * @brief Relative Humidity and Temperature sensor (Mock)
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
#include "sl_sensor_rht.h"
#include "tempdrv.h"

// -----------------------------------------------------------------------------
// Configuration

#define RH_MIN        0  // Relative humidity minimum (in 0.001 percent)
#define RH_MAX   100000  // Relative humidity maximum (in 0.001 percent)
#define T_MIN    -20000  // Temperature minimum (in 0.001 Celsius)
#define T_MAX     20000  // Temperature maximum (in 0.001 Celsius)

// -----------------------------------------------------------------------------
// Public function definitions

static bool has_tempdrv = false; // has temperature driver and it is inited
static int32_t temperature_millicelsius = T_MIN; // Temperature value
static uint32_t humidity_millipercent = RH_MIN; // Relative Humidity value

sl_status_t sl_sensor_rht_init(void)
{
  Ecode_t er;
  // Init TEMPDRV
  er = TEMPDRV_Init();
  if (er != ECODE_EMDRV_TEMPDRV_OK) {
    has_tempdrv = false;
    return SL_STATUS_INITIALIZATION;
  }
  // Enable TEMPDRV
  er = TEMPDRV_Enable(true);
  if (er != ECODE_EMDRV_TEMPDRV_OK) {
    TEMPDRV_DeInit();
    has_tempdrv = false;
    return SL_STATUS_INITIALIZATION;
  }
  // Reset to base values
  temperature_millicelsius = T_MIN;
  humidity_millipercent = RH_MIN;
  has_tempdrv = true;
  return SL_STATUS_OK;
}

void sl_sensor_rht_deinit(void)
{
  TEMPDRV_DeInit();
  return;
}

sl_status_t sl_sensor_rht_get(uint32_t *rh, int32_t *t)
{
  sl_status_t sc = SL_STATUS_OK;

  if (has_tempdrv) {
    // Get temperature from TEMPDRV
    int32_t temperature_celsius = (int32_t)TEMPDRV_GetTemp();
    temperature_millicelsius = temperature_celsius * 1000;
    (*t) = temperature_millicelsius;
  } else {
    // Simulating temperature values
    (*t) = temperature_millicelsius;
    temperature_millicelsius += 1000;
    if (temperature_millicelsius > T_MAX) {
      temperature_millicelsius = T_MIN;
    }
  }
  // Simulating humidity values
  (*rh) = humidity_millipercent;
  humidity_millipercent += 1000;
  if (humidity_millipercent > RH_MAX) {
    humidity_millipercent = RH_MIN;
  }
  return sc;
}
