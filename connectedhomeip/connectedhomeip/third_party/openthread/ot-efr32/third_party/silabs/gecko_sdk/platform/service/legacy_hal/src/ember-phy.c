/***************************************************************************//**
 * @file ember-phy.c
 * @brief ember-phy related stubs for the Legacy HAL "shim" layer
 * @version 5.6.0
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories, Inc, www.silabs.com</b>
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

#include PLATFORM_HEADER
#include "../inc/hal.h"
#include "rail_types.h"
#include "pa_conversions_efr32.h"
#include "sl_rail_util_pa_config.h"

// In newer SDKs this content should come from the rail_util_pa component. For
// backwards compatibility reasons we define the same structures here using the
// old names.
static const RAIL_TxPowerConfig_t paInit2p4 =
{
  .mode = SL_RAIL_UTIL_PA_SELECTION_2P4GHZ,
  .voltage = SL_RAIL_UTIL_PA_VOLTAGE_MV,
  .rampTime = SL_RAIL_UTIL_PA_RAMP_TIME_US,
};
const RAIL_TxPowerConfig_t* halInternalPa2p4GHzInit = &paInit2p4;

#if RAIL_FEAT_SUBGIG_RADIO
static const RAIL_TxPowerConfig_t paInitSub =
{
  .mode = SL_RAIL_UTIL_PA_SELECTION_SUBGHZ,
  .voltage = SL_RAIL_UTIL_PA_VOLTAGE_MV,
  .rampTime = SL_RAIL_UTIL_PA_RAMP_TIME_US,
};
const RAIL_TxPowerConfig_t* halInternalPaSubGHzInit = &paInitSub;
#else
const RAIL_TxPowerConfig_t* halInternalPaSubGHzInit = NULL;
#endif
