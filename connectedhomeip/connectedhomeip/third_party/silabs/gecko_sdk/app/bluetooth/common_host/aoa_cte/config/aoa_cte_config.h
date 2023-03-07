/***************************************************************************//**
 * @file
 * @brief CTE Handle module config for AoA locator
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

#ifndef AOA_CTE_CONFIG_H
#define AOA_CTE_CONFIG_H

// Default CTE Mode
#define AOA_CTE_DEFAULT_MODE    AOA_CTE_TYPE_SILABS

// Scanner timing values in units of 0.625 ms
#define AOA_CTE_SCAN_INTERVAL   160
#define AOA_CTE_SCAN_WINDOW     160

// Scan mode, 0: passive, 1: active
#define AOA_CTE_SCAN_MODE     0

// Measurement interval expressed as the number of connection events.
#define AOA_CTE_SAMPLING_INTERVAL          3

// Minimum CTE length requested in 8 us units. Ranges from 16 to 160 us.
#define AOA_CTE_MIN_LENGTH                 20

// Maximum number of sampled CTEs in each advertising interval.
// 0: Sample and report all available CTEs.
#define AOA_CTE_COUNT                      0

// Switching and sampling slots in us (1 or 2).
#define AOA_CTE_SLOT_DURATION              1

#endif /* AOA_CTE_CONFIG_H */
