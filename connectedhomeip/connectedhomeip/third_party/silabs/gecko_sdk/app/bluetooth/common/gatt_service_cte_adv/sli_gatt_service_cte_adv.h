/***************************************************************************//**
 * @file
 * @brief Constant Tone Extension GATT Service (Common advertising header)
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

#ifndef SLI_GATT_SERVICE_CTE_ADV_H
#define SLI_GATT_SERVICE_CTE_ADV_H

#include "sl_status.h"

// -----------------------------------------------------------------------------
// Advertising Constant Tone Extension Minimum Length characteristic.
typedef uint8_t adv_cte_min_len_t;
extern adv_cte_min_len_t adv_cte_min_len;
#define ADV_CTE_MIN_LEN_MIN           2      // 16 us
#define ADV_CTE_MIN_LEN_MAX           20     // 160 us

// -----------------------------------------------------------------------------
// Advertising Constant Tone Extension Minimum Transmit Count characteristic.
typedef uint8_t adv_cte_min_tx_count_t;
extern adv_cte_min_tx_count_t adv_cte_min_tx_count;
#define ADV_CTE_MIN_TX_COUNT_MIN      1      // 1 per interval
#define ADV_CTE_MIN_TX_COUNT_MAX      15     // 15 per interval

// -----------------------------------------------------------------------------
// Advertising Constant Tone Extension Interval characteristic.
typedef uint16_t adv_cte_interval_t;
extern adv_cte_interval_t adv_cte_interval;
// Minimum value is CTE type dependent.
// Maximum value is 0xFFFF for all CTE types, which equals to UINT16_MAX.
// Therefore, there is no need to check the upper limit.
extern const adv_cte_interval_t ADV_CTE_INTERVAL_MIN;

// -----------------------------------------------------------------------------
// Advertising Constant Tone Extension PHY characteristic.
typedef uint8_t adv_cte_phy_t;
extern adv_cte_phy_t adv_cte_phy;
#define ADV_CTE_PHY_1M                0
#define ADV_CTE_PHY_2M                1
#define ADV_CTE_PHY_CONVERT(p)        (((p) == ADV_CTE_PHY_2M) ? sl_bt_gap_phy_2m : sl_bt_gap_phy_1m)

/**************************************************************************//**
 * Initialize advertisement package according to CTE specifications.
 *****************************************************************************/
void adv_cte_init(void);

/**************************************************************************//**
 * Start/restart advertising with the preset parameters.
 *****************************************************************************/
sl_status_t adv_cte_start(void);

#endif // SLI_GATT_SERVICE_CTE_ADV_H
