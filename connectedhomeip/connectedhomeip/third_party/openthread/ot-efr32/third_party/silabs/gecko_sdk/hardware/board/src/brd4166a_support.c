/***************************************************************************//**
 * @file
 * @brief Thunderboard Sense 2 support
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
#include "sl_i2cspm_sensor_config.h"
#include "brd4166a_support.h"
#include "em_device.h"
#include "em_gpio.h"

static bool pin_mode_set = false;
/***************************************************************************//**
 * @brief
 *    Sets up the route register of the I2C device to use the correct
 *    set of pins
 *
 * @param[in] select
 *    The I2C bus route to use (Environmental sensors, Gas sensor, Hall
 *    sensor)
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
sl_status_t sl_thunderboard_require_i2c(sl_thunderboard_i2c_bus_select_t select)
{
  sl_status_t status = SL_STATUS_OK;

  if (!pin_mode_set){

    // Set correct GPIO mode for all I2C GPIO pins
    GPIO_PinModeSet(gpioPortC, 5, gpioModeWiredAndPullUp, 1);
    GPIO_PinModeSet(gpioPortC, 4, gpioModeWiredAndPullUp, 1);
    GPIO_PinModeSet(gpioPortB, 9, gpioModeWiredAndPullUp, 1);
    GPIO_PinModeSet(gpioPortB, 8, gpioModeWiredAndPullUp, 1);
    GPIO_PinModeSet(gpioPortB, 7, gpioModeWiredAndPullUp, 1);
    GPIO_PinModeSet(gpioPortB, 6, gpioModeWiredAndPullUp, 1);

    pin_mode_set = true;
  }
  switch ( select ) {
    case SL_THUNDERBOARD_I2C_BUS_SELECT_ENV:
      SL_I2CSPM_SENSOR_PERIPHERAL->ROUTEPEN = 0;
      SL_I2CSPM_SENSOR_PERIPHERAL->ROUTELOC0 = ((17 << _I2C_ROUTELOC0_SDALOC_SHIFT) | (17 << _I2C_ROUTELOC0_SCLLOC_SHIFT));
      SL_I2CSPM_SENSOR_PERIPHERAL->ROUTEPEN = (I2C_ROUTEPEN_SCLPEN | I2C_ROUTEPEN_SDAPEN);
      break;
    case SL_THUNDERBOARD_I2C_BUS_SELECT_GAS:
      SL_I2CSPM_SENSOR_PERIPHERAL->ROUTEPEN = 0;
      SL_I2CSPM_SENSOR_PERIPHERAL->ROUTELOC0 = ((6 << _I2C_ROUTELOC0_SDALOC_SHIFT) | (6 << _I2C_ROUTELOC0_SCLLOC_SHIFT));
      SL_I2CSPM_SENSOR_PERIPHERAL->ROUTEPEN = (I2C_ROUTEPEN_SCLPEN | I2C_ROUTEPEN_SDAPEN);
      break;
    case SL_THUNDERBOARD_I2C_BUS_SELECT_HALL:
      SL_I2CSPM_SENSOR_PERIPHERAL->ROUTEPEN = 0;
      SL_I2CSPM_SENSOR_PERIPHERAL->ROUTELOC0 = ((8 << _I2C_ROUTELOC0_SDALOC_SHIFT) | (8 << _I2C_ROUTELOC0_SCLLOC_SHIFT));
      SL_I2CSPM_SENSOR_PERIPHERAL->ROUTEPEN = (I2C_ROUTEPEN_SCLPEN | I2C_ROUTEPEN_SDAPEN);
      break;
    default:
      status = SL_STATUS_INVALID_PARAMETER;
      break;
  }

  return status;
}
