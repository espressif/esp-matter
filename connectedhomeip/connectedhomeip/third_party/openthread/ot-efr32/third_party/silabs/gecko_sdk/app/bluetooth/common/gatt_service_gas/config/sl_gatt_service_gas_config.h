/***************************************************************************//**
 * @file
 * @brief Air Quality GATT Service Configuration
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

#ifndef SL_GATT_SERVICE_GAS_CONFIG_H
#define SL_GATT_SERVICE_GAS_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <o SL_GATT_SERVICE_GAS_ECO2_INVALID> Dummy Equivalent CO2 level measurement results for uninitialized sensors. <0-0xFFFF>
// <i> Default: 0xFFFF
#define SL_GATT_SERVICE_GAS_ECO2_INVALID  0xFFFF

// <o SL_GATT_SERVICE_GAS_TVOC_INVALID> Dummy Total Volatile Organic Compounds level measurement results for uninitialized sensors. <0-0xFFFF>
// <i> Default: 0xFFFF
#define SL_GATT_SERVICE_GAS_TVOC_INVALID  0xFFFF
// <<< end of configuration section >>>

#endif // SL_GATT_SERVICE_GAS_CONFIG_H
