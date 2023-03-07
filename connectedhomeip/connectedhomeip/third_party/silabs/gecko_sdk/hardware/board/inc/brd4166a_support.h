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

#ifndef BRD4166A_SUPPORT_H
#define BRD4166A_SUPPORT_H

#include "sl_status.h"
#include "sl_enum.h"

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************//**
* @defgroup brd4166a_support Thunderboard Sense 2 Support
* @brief Board support functions for Thunderboard Sense 2 (BRD4166A)
* @details The Thunderboard Sense 2 has several I2C buses connected to
  the various sensors that are all connected to the EFR32. This module provides
  functions to route pin connections of an I2C peripheral to the various
  I2C buses. This allows time multiplexed access to several
  I2C buses using only one I2C peripheral.
* @{
******************************************************************************/

/**************************************************************************//**
* @brief
*   I2C bus select
* @details
* - Environmental sensor group
*   - RH/Temp (Si7021)
*   - UV/ALS (Si1133)
*   - Barometric Pressure (BMP280)
* - Hall effect sensor (Si7210)
* - Indoor air quality sensor (CCS811)
******************************************************************************/
SL_ENUM_GENERIC(sl_thunderboard_i2c_bus_select_t, int) {
  SL_THUNDERBOARD_I2C_BUS_SELECT_ENV        = (1 << 0),   ///<  The I2C bus of the environmental sensors selected
  SL_THUNDERBOARD_I2C_BUS_SELECT_GAS        = (1 << 1),   ///<  The I2C bus of the gas sensors selected
  SL_THUNDERBOARD_I2C_BUS_SELECT_HALL       = (1 << 2),   ///<  The I2C bus of the Hall sensor selected
};

/***************************************************************************//**
 * @brief
 *    Set up the route register of the I2C device to use the correct
 *    set of pins
 * * @param[in] select
 *    The I2C bus route to use (Environmental sensors, Gas sensor, Hall
 *    sensor)
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
sl_status_t sl_thunderboard_require_i2c(sl_thunderboard_i2c_bus_select_t select);

#ifdef __cplusplus
}
#endif
/** @} */

#endif // BRD4166A_SUPPORT_H
