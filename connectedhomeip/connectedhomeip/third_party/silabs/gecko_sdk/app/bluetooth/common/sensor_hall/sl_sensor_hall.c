/***************************************************************************//**
 * @file
 * @brief Hall sensor
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

#include <math.h>
#include "sl_board_control.h"
#include "sl_si7210.h"
#include "app_assert.h"
#include "sl_sensor_select.h"
#include "sl_sensor_hall.h"

// -----------------------------------------------------------------------------
// Configuration

#define HALL_THRESHOLD    3.0f  /* mT */
#define HALL_HYSTERESIS   0.5f  /* mT */
#define HALL_POLARITY     0x00  /* Omnipolar field polarity */
#define HALL_SCALE        20000 /* uT */

// -----------------------------------------------------------------------------
// Private variables

static bool initialized = false;

// -----------------------------------------------------------------------------
// Public function definitions

sl_status_t sl_sensor_hall_init(void)
{
  sl_status_t sc;
  sl_si7210_configure_t hall_config = {
    .threshold = HALL_THRESHOLD,
    .hysteresis = HALL_HYSTERESIS,
    .polarity = HALL_POLARITY,
    .output_invert = false
  };
  sl_i2cspm_t *hall_sensor = sl_sensor_select(SL_BOARD_SENSOR_HALL);
  sc = sl_board_enable_sensor(SL_BOARD_SENSOR_HALL);
  app_assert((SL_STATUS_OK == sc) && (NULL != hall_sensor),
             "[E: %#04x] Si7210 sensor not available\n",
             sc);

  sc = sl_si7210_init(hall_sensor);
  if (SL_STATUS_OK == sc) {
    sc = sl_si7210_configure(hall_sensor, &hall_config);
    app_assert_status(sc);
    initialized = true;
  } else {
    initialized = false;
  }

  return sc;
}

void sl_sensor_hall_deinit(void)
{
  (void)sl_board_disable_sensor(SL_BOARD_SENSOR_HALL);
  initialized = false;
}

sl_status_t sl_sensor_hall_get(float *field_strength, bool *alert, bool *tamper)
{
  sl_status_t sc;

  if (initialized) {
    float fs_abs;
    // store previous alert state to implement hysteresis
    static bool alert_local = false;
    // measure field strength
    sl_i2cspm_t *hall_sensor = sl_sensor_select(SL_BOARD_SENSOR_HALL);
    sc = sl_si7210_measure(hall_sensor, HALL_SCALE, field_strength);
    if (SL_STATUS_OK != sc) {
      // measurement failed
      return sc;
    }
    // get absolute value for threshold comparisons (because of omnipolar config)
    fs_abs = fabsf(*field_strength);
    // check alert threshold with hysteresis
    if (fs_abs < (HALL_THRESHOLD - HALL_HYSTERESIS)) {
      alert_local = false;
    } else if (fs_abs > (HALL_THRESHOLD + HALL_HYSTERESIS)) {
      alert_local = true;
    }
    *alert = alert_local;
    // check tamper threshold
    if (fs_abs > sl_si7210_get_tamper_threshold()) {
      *tamper = true;
    } else {
      *tamper = false;
    }
  } else {
    sc = SL_STATUS_NOT_INITIALIZED;
  }

  return sc;
}
