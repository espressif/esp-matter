/***************************************************************************//**
 * @file
 * @brief RAIL Util for EFF0 configuration file.
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_RAIL_UTIL_EFF_CONFIG_H
#define SL_RAIL_UTIL_EFF_CONFIG_H

#include "em_gpio.h"

// <<< Use Configuration Wizard in Context Menu >>>

// <h> EFF Configuration
// <o SL_RAIL_UTIL_EFF_DEVICE> Select connected EFF device
// <RAIL_EFF_DEVICE_NONE=> No connected EFF device
// <RAIL_EFF_DEVICE_EFF01Z11=> EFF01Z11
// <RAIL_EFF_DEVICE_EFF01A12=> EFF01A12
// <i> Default: RAIL_EFF_DEVICE_EFF01A12
#define SL_RAIL_UTIL_EFF_DEVICE   RAIL_EFF_DEVICE_EFF01A12
// <q SL_RAIL_UTIL_EFF_LNA_MODE_RURAL_ENABLE> Enable Rural LNA Mode
// <i> Default: 1
#define SL_RAIL_UTIL_EFF_LNA_MODE_RURAL_ENABLE  1
// <q SL_RAIL_UTIL_EFF_LNA_MODE_URBAN_ENABLE> Enable Urban LNA Mode
// <i> Default: 1
#define SL_RAIL_UTIL_EFF_LNA_MODE_URBAN_ENABLE  1
// <q SL_RAIL_UTIL_EFF_LNA_MODE_BYPASS_ENABLE> Enable Bypass LNA Mode
// <i> Default: 1
#define SL_RAIL_UTIL_EFF_LNA_MODE_BYPASS_ENABLE 1
// <o SL_RAIL_UTIL_EFF_RURAL_URBAN_MV> Trip point between rural and urban modes, in millivolts.
// <5-1250:1>
// <i> Default: 120
#define SL_RAIL_UTIL_EFF_RURAL_URBAN_MV 120
// <o SL_RAIL_UTIL_EFF_URBAN_BYPASS_MV> Trip point between urban and bypass modes, in millivolts.
// <5-1250:1>
// <i> Default: 130
#define SL_RAIL_UTIL_EFF_URBAN_BYPASS_MV 130
// <o SL_RAIL_UTIL_EFF_URBAN_DWELL_TIME_MS> Length of time to stay in urban mode before transitioning to rural mode, in milliseconds.
// <1-105000:1>
// <i> Default: 30000
#define SL_RAIL_UTIL_EFF_URBAN_DWELL_TIME_MS 30000
// <o SL_RAIL_UTIL_EFF_BYPASS_DWELL_TIME_MS> Length of time to stay in bypass mode before transitioning to urban or rural mode, in milliseconds.
// <1-105000:1>
// <i> Default: 30000
#define SL_RAIL_UTIL_EFF_BYPASS_DWELL_TIME_MS 30000
// <o SL_RAIL_UTIL_EFF_CLPC_SLOW_LOOP_TARGET> Target for CLPC slow loop, in milliwatts.
// <5-2000:1>
// <i> Default: 630
#define SL_RAIL_UTIL_EFF_CLPC_SLOW_LOOP_TARGET 630
// <o SL_RAIL_UTIL_EFF_CLPC_SLOW_LOOP_SLOPE> Relationship between delta-GAINDIG/delta-power to find new GAINDIG value
// <1-100:1>
// <i> Default: 100
#define SL_RAIL_UTIL_EFF_CLPC_SLOW_LOOP_SLOPE 100
// <o SL_RAIL_UTIL_EFF_CLPC_FAST_LOOP_TARGET> Target for CLPC fast loop, in millivolts.
// <5-1500:1>
// <i> Default: 700
#define SL_RAIL_UTIL_EFF_CLPC_FAST_LOOP_TARGET 700
// <o SL_RAIL_UTIL_EFF_CLPC_FAST_LOOP_SLOPE> Relationship between delta-GAINDIG/delta-AUXADC to find new GAINDIG value
// <1-100:1>
// <i> Default: 100
#define SL_RAIL_UTIL_EFF_CLPC_FAST_LOOP_SLOPE 99
// <o SL_RAIL_UTIL_EFF_CLPC_ENABLE> Select CLPC mode
// <RAIL_EFF_CLPC_DISABLED=> Disable CLPC power control. Flare modes will only be transmit/receive.
// <RAIL_EFF_CLPC_MODE_CHANGE=> Allow mode changes/measurements, but no power changes
// <RAIL_EFF_CLPC_POWER_SLOW=> Allow only Slow loop power changes
// <RAIL_EFF_CLPC_POWER_FAST=> Allow only Fast loop power changes
// <RAIL_EFF_CLPC_POWER_BOTH=> Allow full power control
// <i> Default: RAIL_EFF_CLPC_POWER_BOTH
#define SL_RAIL_UTIL_EFF_CLPC_ENABLE RAIL_EFF_CLPC_MODE_CHANGE
// <o RAIL_UTIL_EFF_MAX_TX_CONTINUOUS_POWER_DBM> Maximum continuous transfer power in dBm
// <20-30:1>
// <i> Default: 20
#define RAIL_UTIL_EFF_MAX_TX_CONTINUOUS_POWER_DBM 20
// <o RAIL_UTIL_EFF_MAX_TX_DUTY_CYCLE> Maximum transmit duty cycle as a percentage
// <10-100:1>
// <i> Default: 50
#define RAIL_UTIL_EFF_MAX_TX_DUTY_CYCLE   50
// <o SL_RAIL_UTIL_EFF_TEMPERATURE_THRESHOLD_EFF_DEGREES_K> Temperature of EFF above which transmit is not allowed, in degrees Kelvin
// <1-398:1>
// <i> Default: 373
#define SL_RAIL_UTIL_EFF_TEMPERATURE_THRESHOLD_EFF_DEGREES_K 373
// <o SL_RAIL_UTIL_EFF_TEMPERATURE_THRESHOLD_INTERNAL_DEGREES_K> Chip's internal temperature above which transmit is not allowed, in degrees Kelvin
// <1-398:1>
// <i> Default: 373
#define SL_RAIL_UTIL_EFF_TEMPERATURE_THRESHOLD_INTERNAL_DEGREES_K 373
// </h>

// <<< end of configuration section >>>

// <<< sl:start pin_tool >>>

// CTRL0 GPIO
// <gpio> SL_RAIL_UTIL_EFF_CTRL0
// $[GPIO_SL_RAIL_UTIL_EFF_CTRL0]
#define SL_RAIL_UTIL_EFF_CTRL0_PORT           gpioPortC
#define SL_RAIL_UTIL_EFF_CTRL0_PIN            4
// [GPIO_SL_RAIL_UTIL_EFF_CTRL0]$

// CTRL1 GPIO
// <gpio> SL_RAIL_UTIL_EFF_CTRL1
// $[GPIO_SL_RAIL_UTIL_EFF_CTRL1]
#define SL_RAIL_UTIL_EFF_CTRL1_PORT           gpioPortC
#define SL_RAIL_UTIL_EFF_CTRL1_PIN            5
// [GPIO_SL_RAIL_UTIL_EFF_CTRL1]$

// CTRL2 GPIO
// <gpio> SL_RAIL_UTIL_EFF_CTRL2
// $[GPIO_SL_RAIL_UTIL_EFF_CTRL2]
#define SL_RAIL_UTIL_EFF_CTRL2_PORT           gpioPortC
#define SL_RAIL_UTIL_EFF_CTRL2_PIN            6
// [GPIO_SL_RAIL_UTIL_EFF_CTRL2]$

// CTRL3 GPIO
// <gpio> SL_RAIL_UTIL_EFF_CTRL3
// $[GPIO_SL_RAIL_UTIL_EFF_CTRL3]
#define SL_RAIL_UTIL_EFF_CTRL3_PORT           gpioPortC
#define SL_RAIL_UTIL_EFF_CTRL3_PIN            7
// [GPIO_SL_RAIL_UTIL_EFF_CTRL3]$

// TEST GPIO
// <gpio> SL_RAIL_UTIL_EFF_TEST
// $[GPIO_SL_RAIL_UTIL_EFF_TEST]
#define SL_RAIL_UTIL_EFF_TEST_PORT            gpioPortC
#define SL_RAIL_UTIL_EFF_TEST_PIN             3
// [GPIO_SL_RAIL_UTIL_EFF_TEST]$

// SENSE GPIO
// <gpio> SL_RAIL_UTIL_EFF_SENSE
// $[GPIO_SL_RAIL_UTIL_EFF_SENSE]
#define SL_RAIL_UTIL_EFF_SENSE_PORT           gpioPortC
#define SL_RAIL_UTIL_EFF_SENSE_PIN            8
// [GPIO_SL_RAIL_UTIL_EFF_SENSE]$

// <<< sl:end pin_tool >>>

#endif // SL_RAIL_UTIL_EFF_CONFIG_H
