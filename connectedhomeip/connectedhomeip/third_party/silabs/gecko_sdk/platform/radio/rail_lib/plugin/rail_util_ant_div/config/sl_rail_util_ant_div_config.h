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

#ifndef SL_RAIL_UTIL_ANT_DIV_CONFIG_H
#define SL_RAIL_UTIL_ANT_DIV_CONFIG_H

#include "rail_types.h"

#define SL_RAIL_UTIL_ANT_DIV_DISABLED  (0U)
#define SL_RAIL_UTIL_ANT_DIV_ANTENNA0  (1U) // antenna0
#define SL_RAIL_UTIL_ANT_DIV_ANTENNA1  (2U) // antenna1
#define SL_RAIL_UTIL_ANT_DIV_DIVERSITY (3U) // antenna0 | antenna1

// <<< Use Configuration Wizard in Context Menu >>>

// <h> Chip-external Antenna Diversity Configuration
// <o SL_RAIL_UTIL_ANT_DIV_RX_MODE> RX Antenna Diversity Mode
// <SL_RAIL_UTIL_ANT_DIV_DISABLED=> Disable Antenna Diversity (Don't alter antenna selection)
// <SL_RAIL_UTIL_ANT_DIV_DIVERSITY=> Enable Antenna Diversity (Switch between antennas dynamically)
// <SL_RAIL_UTIL_ANT_DIV_ANTENNA0=> Use Antenna 0 Only
// <SL_RAIL_UTIL_ANT_DIV_ANTENNA1=> Use Antenna 1 Only
// <i> Default: SL_RAIL_UTIL_ANT_DIV_DISABLED
#define SL_RAIL_UTIL_ANT_DIV_RX_MODE  SL_RAIL_UTIL_ANT_DIV_DISABLED
// <o SL_RAIL_UTIL_ANT_DIV_TX_MODE> TX Antenna Diversity Mode
// <SL_RAIL_UTIL_ANT_DIV_DISABLED=> Disable Antenna Diversity (Don't alter antenna selection)
// <SL_RAIL_UTIL_ANT_DIV_DIVERSITY=> Enable Antenna Diversity (Switch between antennas dynamically)
// <SL_RAIL_UTIL_ANT_DIV_ANTENNA0=> Use Antenna 0 Only
// <SL_RAIL_UTIL_ANT_DIV_ANTENNA1=> Use Antenna 1 Only
// <i> Default: SL_RAIL_UTIL_ANT_DIV_DISABLED
#define SL_RAIL_UTIL_ANT_DIV_TX_MODE  SL_RAIL_UTIL_ANT_DIV_DISABLED
// <e SL_RAIL_UTIL_ANT_DIV_RX_RUNTIME_PHY_SELECT> Enable/Disable Runtime PHY Select
// <i> Default: 0
#define SL_RAIL_UTIL_ANT_DIV_RX_RUNTIME_PHY_SELECT 0
// </e>
// </h>

// <<< end of configuration section >>>

// <<< sl:start pin_tool >>>

// <modem signal=(ANT0),(ANT1)> SL_RAIL_UTIL_ANT_DIV
// $[MODEM_SL_RAIL_UTIL_ANT_DIV]
#define SL_RAIL_UTIL_ANT_DIV_PERIPHERAL     MODEM

// [MODEM_SL_RAIL_UTIL_ANT_DIV]$

// <<< sl:end pin_tool >>>

#endif // SL_RAIL_UTIL_ANT_DIV_CONFIG_H
