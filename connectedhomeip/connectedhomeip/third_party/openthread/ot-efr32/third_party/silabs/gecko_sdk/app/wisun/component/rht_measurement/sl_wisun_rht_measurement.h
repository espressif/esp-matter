/***************************************************************************//**
 * @file sl_wisun_rht_measurement.h
 * @brief Wi-SUN temperature and relative humidity measurement modul
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifndef __SL_WISUN_RHT_MEASUREMENT__
#define __SL_WISUN_RHT_MEASUREMENT__

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

#include <inttypes.h>
#include <sl_status.h>

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Init I2C SI7021 sensor.
 * @details Used sl_i2cspm_instances
 *****************************************************************************/
void sl_wisun_rht_init(void);

/**************************************************************************//**
 * @brief Deinit I2C SI7021 sensor.
 * @details Used sl_i2cspm_instances
 *****************************************************************************/
void sl_wisun_rht_deinit(void);

/**************************************************************************//**
 * @brief Get temperature and relative humidity.
 * @details Used sl_i2cspm_instances
 * @param[out] rh Relative Humidity in [% * 1000]
 * @param[out] t Temperature in [milliCelsius degree]
 * @return sl_status_t Status valueSL_STATUS_OK Success
 *         or SL_STATUS_TRANSMIT I2C on error
 *****************************************************************************/
sl_status_t sl_wisun_rht_get(uint32_t *rh, int32_t *t);

#endif
