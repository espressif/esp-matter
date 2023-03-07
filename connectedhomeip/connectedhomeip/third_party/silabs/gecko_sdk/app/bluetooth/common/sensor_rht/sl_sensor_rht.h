/***************************************************************************//**
 * @file
 * @brief Relative Humidity and Temperature sensor header
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

#ifndef SL_SENSOR_RHT_H
#define SL_SENSOR_RHT_H

#include <stdint.h>
#include "sl_status.h"

/**************************************************************************//**
 * Initialize Relative Humidity and Temperature sensor.
 *
 * \note With certain boards (e.g. 4166A, 4184A, 4184B), using this
 * initialization function enables other sensors, because they're on the same
 * enable pin. Please take that into account when using this function.
 * @return Status of the operation.
 *****************************************************************************/
sl_status_t sl_sensor_rht_init(void);

/**************************************************************************//**
 * Deinitialize Relative Humidity and Temperature sensor.
 *
 * \warning With certain boards (e.g. 4166A, 4184A, 4184B), using this
 * deinitialization function disables other sensors, because they're on the same
 * enable pin. Please use with caution.
 *****************************************************************************/
void sl_sensor_rht_deinit(void);

/**************************************************************************//**
 * Getter for Relative Humidity and Temperature sensor measurement data.
 * @param[out] rh Relative humidity (in 0.001 percent).
 * @param[out] t Temperature (in 0.001 Celsius).
 * @return Status of the operation.
 *****************************************************************************/
sl_status_t sl_sensor_rht_get(uint32_t *rh, int32_t *t);

#endif // SL_SENSOR_RHT_H
