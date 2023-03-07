/***************************************************************************//**
 * @file
 * @brief Power supply measurement header
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

#ifndef SL_POWER_SUPPLY_H
#define SL_POWER_SUPPLY_H

#include <stdint.h>
#include <stdbool.h>

#define SL_POWER_SUPPLY_TYPE_UNKNOWN 0 ///< Unknown power supply type
#define SL_POWER_SUPPLY_TYPE_USB     1 ///< The board powered from the USB connector
#define SL_POWER_SUPPLY_TYPE_AA      2 ///< The board powered from AA batteries
#define SL_POWER_SUPPLY_TYPE_AAA     3 ///< The board powered from AAA batteries
#define SL_POWER_SUPPLY_TYPE_CR2032  4 ///< The board powered from a CR2032 battery

/***************************************************************************//**
 * Probe the connected supply and determine its type.
 *
 * @note The results can be acquired with \ref sl_power_supply_get_characteristics.
 ******************************************************************************/
void sl_power_supply_probe(void);

/***************************************************************************//**
 * Retrieve the supply characteristic variables.
 *
 * @param[out] type Supply type.
 * @param[out] voltage Supply voltage.
 * @param[out] ir Internal resistance of the supply.
 ******************************************************************************/
void sl_power_supply_get_characteristics(uint8_t *type, float *voltage, float *ir);

/***************************************************************************//**
 * Getter for the power supply type.
 *
 * @return Power supply type represented as an integer.
 ******************************************************************************/
uint8_t sl_power_supply_get_type(void);

/***************************************************************************//**
 * Checks if the current power supply has low power capability.
 *
 * @return True if the supply is low power type, false otherwise.
 ******************************************************************************/
bool sl_power_supply_is_low_power(void);

/***************************************************************************//**
 * Measure the supply voltage by averaging multiple readings.
 *
 * @param[in] avg Number of measurements to average.
 * @return The measured voltage.
 ******************************************************************************/
float sl_power_supply_measure_voltage(unsigned int avg);

/***************************************************************************//**
 * Measure the battery level.
 *
 * @return The estimated battery capacity level in percent.
 ******************************************************************************/
uint8_t sl_power_supply_get_battery_level(void);

#endif // SL_POWER_SUPPLY_H
