/***************************************************************************//**
 * @file
 * @brief Default configuration values for AoA angle estimation.
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

#ifndef AOA_ANGLE_CONFIG_H
#define AOA_ANGLE_CONFIG_H

#include <math.h>
#include "sl_rtl_clib_api.h"

// AoA estimator mode
#define AOA_ANGLE_AOX_MODE                       SL_RTL_AOA_MODE_REAL_TIME_BASIC

// Antenna array type
#define AOA_ANGLE_ANTENNA_ARRAY_TYPE             SL_RTL_AOX_ARRAY_TYPE_4x4_DP_URA

// Reference RSSI value of the asset tag at 1.0 m distance in dBm.
#define AOA_ANGLE_TAG_TX_POWER                   (-45.0)

// Filter weight applied on the estimated distance. Ranges from 0 to 1.
#define AOA_ANGLE_FILTERING_AMOUNT               0.6f

// Direction correction will be cleared if this amount of IQ reports are received
// without receiving a correction message.
#define AOA_ANGLE_CORRECTION_TIMEOUT             5

// Correction values with a sequence number more than MAX_CORRECTION_DELAY apart
// from the last IQ report are considered outdated and will be ignored.
#define AOA_ANGLE_MAX_CORRECTION_DELAY           3

// Minimum CTE length requested in 8 us units. Ranges from 16 to 160 us.
#define AOA_ANGLE_CTE_MIN_LENGTH                 20

// Switching and sampling slots in us (1 or 2).
#define AOA_ANGLE_CTE_SLOT_DURATION              1

#endif // AOA_ANGLE_CONFIG_H
