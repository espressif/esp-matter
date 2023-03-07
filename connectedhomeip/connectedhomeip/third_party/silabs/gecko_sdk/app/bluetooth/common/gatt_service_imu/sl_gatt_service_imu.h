/***************************************************************************//**
 * @file
 * @brief Inertial Measurement Unit GATT Service header
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

#ifndef SL_GATT_SERVICE_IMU_H
#define SL_GATT_SERVICE_IMU_H

#include "sl_bt_api.h"

/**************************************************************************//**
 * Bluetooth stack event handler.
 * @param[in] evt Event coming from the Bluetooth stack.
 *****************************************************************************/
void sl_gatt_service_imu_on_event(sl_bt_msg_t *evt);

/**************************************************************************//**
 * IMU GATT service event handler.
 *****************************************************************************/
void sl_gatt_service_imu_step(void);

/**************************************************************************//**
 * Getter for Orientation and Acceleration characteristic values.
 * @param[out] ovec Three dimensional orientation vector (in 0.01 degree).
 * @param[out] avec Three dimensional acceleration vector.
 * @return Status of the operation.
 * @note To be implemented in user code.
 *****************************************************************************/
sl_status_t sl_gatt_service_imu_get(int16_t ovec[3], int16_t avec[3]);

/**************************************************************************//**
 * Called when a calibration was requested by the GATT client.
 * @return Status of the operation.
 * @note To be implemented in user code.
 *****************************************************************************/
sl_status_t sl_gatt_service_imu_calibrate(void);

/**************************************************************************//**
 * Enable/disable IMU sensor.
 * @param[in] Enable (true) or disable (false).
 * @note To be implemented in user code.
 *****************************************************************************/
void sl_gatt_service_imu_enable(bool enable);

#endif // SL_GATT_SERVICE_IMU_H
