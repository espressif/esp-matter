/***************************************************************************//**
 * @file
 * @brief I2C sensor select utility for BRD4166A
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

#include <stddef.h>
#include "sl_sensor_select.h"
#include "sl_board_control_config.h"
#include "sl_i2cspm_instances.h"

#if defined(BRD4166A_SUPPORT)
// More than one I2C instance present, we need to switch between them.
#include "brd4166a_support.h"
#else
// Only one I2C instance present, let's use this dummy macro.
#define sl_thunderboard_require_i2c(x)
#endif

sl_i2cspm_t* sl_sensor_select(sl_board_sensor_t sensor)
{
  sl_i2cspm_t *i2cspm_sensor = sl_i2cspm_sensor;

  switch (sensor) {
    case SL_BOARD_SENSOR_RHT:
      sl_thunderboard_require_i2c(SL_THUNDERBOARD_I2C_BUS_SELECT_ENV);
      break;
    case SL_BOARD_SENSOR_LIGHT:
      sl_thunderboard_require_i2c(SL_THUNDERBOARD_I2C_BUS_SELECT_ENV);
      break;
    case SL_BOARD_SENSOR_PRESSURE:
      sl_thunderboard_require_i2c(SL_THUNDERBOARD_I2C_BUS_SELECT_ENV);
      break;
    case SL_BOARD_SENSOR_HALL:
      sl_thunderboard_require_i2c(SL_THUNDERBOARD_I2C_BUS_SELECT_HALL);
      break;
    case SL_BOARD_SENSOR_GAS:
      sl_thunderboard_require_i2c(SL_THUNDERBOARD_I2C_BUS_SELECT_GAS);
      break;
    default:
      i2cspm_sensor = NULL;
      break;
  }

  return i2cspm_sensor;
}
