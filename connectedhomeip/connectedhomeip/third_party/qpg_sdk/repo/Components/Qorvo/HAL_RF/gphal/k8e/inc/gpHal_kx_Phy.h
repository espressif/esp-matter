/*
 * Copyright (c) 2020, Qorvo Inc
 *
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 *
 */

#ifndef _GPHAL_KX_PHY_H_
#define _GPHAL_KX_PHY_H_

typedef UInt8 gpHal_FllChannel_t;

// The first FLL index that is a BLE channel (corresponds to the number of 15.4 channels)
#define GP_HAL_BLE_PHY_START_INDEX                   0
#define GP_HAL_BLE_FLL_START_INDEX                  16

#define GP_HAL_CONVERT_BLEPHY_TO_FLL_CHANNEL(phyChannel)   (phyChannel - GP_HAL_BLE_PHY_START_INDEX + GP_HAL_BLE_FLL_START_INDEX)
#define GP_HAL_CONVERT_FLL_TO_BLEPHY_CHANNEL(fllChannel)   (fllChannel + GP_HAL_BLE_PHY_START_INDEX - GP_HAL_BLE_FLL_START_INDEX)

#define GP_HAL_MAC_PHY_START_INDEX                   11
#define GP_HAL_MAC_FLL_START_INDEX                    0

#define GP_HAL_CONVERT_MACPHY_TO_FLL_CHANNEL(phyChannel)   ((gpHal_FllChannel_t)(phyChannel - GP_HAL_MAC_PHY_START_INDEX + GP_HAL_MAC_FLL_START_INDEX))
#define GP_HAL_CONVERT_FLL_TO_MACPHY_CHANNEL(fllChannel)   (           ((UInt8) fllChannel) + GP_HAL_MAC_PHY_START_INDEX - GP_HAL_MAC_FLL_START_INDEX)


#endif /* _GPHAL_KX_PHY_H_ */
