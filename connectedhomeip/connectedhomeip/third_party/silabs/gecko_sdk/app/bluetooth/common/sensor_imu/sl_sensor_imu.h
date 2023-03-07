/***************************************************************************//**
 * @file
 * @brief Inertial Measurement Unit sensor header
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

#ifndef SL_SENSOR_IMU_H
#define SL_SENSOR_IMU_H

#include <stdbool.h>
#include "sl_status.h"

/**************************************************************************//**
 * Initialize IMU sensor.
 *****************************************************************************/
void sl_sensor_imu_init(void);

/**************************************************************************//**
 * Deinitialize IMU sensor.
 *****************************************************************************/
void sl_sensor_imu_deinit(void);

/**************************************************************************//**
 * Enable/disable IMU sensor.
 * @param[in] Enable (true) or disable (false).
 * @return Status of the operation.
 *****************************************************************************/
sl_status_t sl_sensor_imu_enable(bool enable);

/**************************************************************************//**
 * Getter for orientation and acceleration sensor measurement data.
 * @param[out] ovec Three dimensional orientation vector (in 0.01 degree).
 * @param[out] avec Three dimensional acceleration vector.
 * @return Status of the operation.
 *****************************************************************************/
sl_status_t sl_sensor_imu_get(int16_t ovec[3], int16_t avec[3]);

/**************************************************************************//**
 * Perform IMU sensor calibration.
 * @return Status of the operation.
 *****************************************************************************/
sl_status_t sl_sensor_imu_calibrate(void);

#endif // SL_SENSOR_IMU_H
