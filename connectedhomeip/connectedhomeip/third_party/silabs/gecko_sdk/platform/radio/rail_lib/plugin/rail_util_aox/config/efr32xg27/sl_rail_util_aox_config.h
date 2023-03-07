/***************************************************************************//**
 * @file
 * @brief
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

#ifndef SL_RAIL_UTIL_AOX_CONFIG_H
#define SL_RAIL_UTIL_AOX_CONFIG_H

#include "rail_types.h"

// <<< Use Configuration Wizard in Context Menu >>>
// <o SL_RAIL_UTIL_AOX_ANTENNA_PIN_COUNT> Number of AoX Antenna Pins
// <0-6:1>
// <i> Default: 0
#define SL_RAIL_UTIL_AOX_ANTENNA_PIN_COUNT 0
#if SL_RAIL_UTIL_AOX_ANTENNA_PIN_COUNT == 0
#warning "The AoX component is only needed to configure multiple AoX antennas!"
#endif //SL_RAIL_UTIL_AOX_ANTENNA_PIN_COUNT == 0
// <<< end of configuration section >>>

// <<< sl:start pin_tool >>>

// CTE antenna GPIO 0
// <gpio optional=true> SL_RAIL_UTIL_AOX_ANTENNA_PIN0
// $[GPIO_SL_RAIL_UTIL_AOX_ANTENNA_PIN0]
// #define SL_RAIL_UTIL_AOX_ANTENNA_PIN0_PORT               gpioPortA
// #define SL_RAIL_UTIL_AOX_ANTENNA_PIN0_PIN                0
// [GPIO_SL_RAIL_UTIL_AOX_ANTENNA_PIN0]$

// CTE antenna GPIO 1
// <gpio optional=true> SL_RAIL_UTIL_AOX_ANTENNA_PIN1
// $[GPIO_SL_RAIL_UTIL_AOX_ANTENNA_PIN1]
// #define SL_RAIL_UTIL_AOX_ANTENNA_PIN1_PORT               gpioPortA
// #define SL_RAIL_UTIL_AOX_ANTENNA_PIN1_PIN                1
// [GPIO_SL_RAIL_UTIL_AOX_ANTENNA_PIN1]$

// CTE antenna GPIO 2
// <gpio optional=true> SL_RAIL_UTIL_AOX_ANTENNA_PIN2
// $[GPIO_SL_RAIL_UTIL_AOX_ANTENNA_PIN2]
// #define SL_RAIL_UTIL_AOX_ANTENNA_PIN2_PORT               gpioPortA
// #define SL_RAIL_UTIL_AOX_ANTENNA_PIN2_PIN                2
// [GPIO_SL_RAIL_UTIL_AOX_ANTENNA_PIN2]$

// CTE antenna GPIO 3
// <gpio optional=true> SL_RAIL_UTIL_AOX_ANTENNA_PIN3
// $[GPIO_SL_RAIL_UTIL_AOX_ANTENNA_PIN3]
// #define SL_RAIL_UTIL_AOX_ANTENNA_PIN3_PORT               gpioPortA
// #define SL_RAIL_UTIL_AOX_ANTENNA_PIN3_PIN                3
// [GPIO_SL_RAIL_UTIL_AOX_ANTENNA_PIN3]$

// CTE antenna GPIO 4
// <gpio optional=true> SL_RAIL_UTIL_AOX_ANTENNA_PIN4
// $[GPIO_SL_RAIL_UTIL_AOX_ANTENNA_PIN4]
// #define SL_RAIL_UTIL_AOX_ANTENNA_PIN4_PORT               gpioPortA
// #define SL_RAIL_UTIL_AOX_ANTENNA_PIN4_PIN                4
// [GPIO_SL_RAIL_UTIL_AOX_ANTENNA_PIN4]$

// CTE antenna GPIO 5
// <gpio optional=true> SL_RAIL_UTIL_AOX_ANTENNA_PIN5
// $[GPIO_SL_RAIL_UTIL_AOX_ANTENNA_PIN5]
// #define SL_RAIL_UTIL_AOX_ANTENNA_PIN5_PORT               gpioPortA
// #define SL_RAIL_UTIL_AOX_ANTENNA_PIN5_PIN                5
// [GPIO_SL_RAIL_UTIL_AOX_ANTENNA_PIN5]$

// <<< sl:end pin_tool >>>

#endif // SL_RAIL_UTIL_AOX_CONFIG_H
