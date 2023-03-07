/***************************************************************************//**
 * @file
 * @brief Constant Tone Extension GATT Service (Silabs proprietary) configuration
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

#ifndef SL_GATT_SERVICE_CTE_SILABS_CONFIG_H
#define SL_GATT_SERVICE_CTE_SILABS_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <o SL_GATT_SERVICE_CTE_SILABS_ADV_INTERVAL> Silabs CTE advertising interval in units of 0.625 ms <32-65535>
// <i> Default: 32
// <i> Ranges from 20 ms to 40.96 s.
#define SL_GATT_SERVICE_CTE_SILABS_ADV_INTERVAL   32

// <o SL_GATT_SERVICE_CTE_SILABS_MIN_LEN> Advertising Constant Tone Extension Minimum Length in units of 8 us <2-20>
// <i> Default: 20
// <i> Ranges from 16 us to 160 us.
#define SL_GATT_SERVICE_CTE_SILABS_MIN_LEN        20

// <o SL_GATT_SERVICE_CTE_SILABS_MIN_TX_COUNT> Advertising Constant Tone Extension Minimum Transmit Count characteristic. <1-15>
// <i> Default: 1
// <i> Ranges from 1 to 15.
#define SL_GATT_SERVICE_CTE_SILABS_MIN_TX_COUNT    1

// <o SL_GATT_SERVICE_CTE_SILABS_ADV_CTE_PHY> Advertising Constant Tone Extension PHY characteristic.
// <0=> 1M PHY CTE
// <1=> 2M PHY CTE
// <i> Default: 0
#define SL_GATT_SERVICE_CTE_SILABS_ADV_CTE_PHY  0

// <<< end of configuration section >>>

#endif // SL_GATT_SERVICE_CTE_SILABS_CONFIG_H
