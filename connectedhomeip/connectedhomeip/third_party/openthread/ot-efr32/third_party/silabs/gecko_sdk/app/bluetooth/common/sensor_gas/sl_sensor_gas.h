/***************************************************************************//**
 * @file
 * @brief Air quality sensor header
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

#ifndef SL_SENSOR_GAS_H
#define SL_SENSOR_GAS_H

#include <stdint.h>
#include "sl_status.h"

/**************************************************************************//**
 * Initialize air quality sensor.
 *
 * @return Status of the operation.
 *****************************************************************************/
sl_status_t sl_sensor_gas_init(void);

/**************************************************************************//**
 * Deinitialize air quality sensor.
 *****************************************************************************/
void sl_sensor_gas_deinit(void);

/**************************************************************************//**
 * Getter for air quality sensor measurement data.
 * @param[out] eco2 Equivalent CO2 level (in ppm).
 * @param[out] tvoc Total Volatile Organic Compounds level (in ppb).
 *
 * @retval SL_STATUS_OK Measurement was successful.
 * @retval SL_STATUS_NOT_READY There is no new data ready.
 * @retval SL_STATUS_TRANSMIT I2C transmission error.
 * @retval SL_STATUS_NOT_INITIALIZED Sensor has not been initialized.
 *****************************************************************************/
sl_status_t sl_sensor_gas_get(uint16_t *eco2, uint16_t *tvoc);

#endif // SL_SENSOR_GAS_H
