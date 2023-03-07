/***************************************************************************//**
 * @file
 * @brief Packet Trace Information
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

#include "em_gpio.h"
#include "rail.h"
#include "sl_rail_util_pti.h"
#include "sl_rail_util_pti_config.h"

void sl_rail_util_pti_init(void)
{
  RAIL_PtiConfig_t railPtiConfig = {
    .mode = SL_RAIL_UTIL_PTI_MODE,
    .baud = SL_RAIL_UTIL_PTI_BAUD_RATE_HZ,
#if defined(SL_RAIL_UTIL_PTI_DOUT_PORT) && defined(SL_RAIL_UTIL_PTI_DOUT_PIN)
    .doutPort = (uint8_t)SL_RAIL_UTIL_PTI_DOUT_PORT,
    .doutPin = SL_RAIL_UTIL_PTI_DOUT_PIN,
  #ifdef SL_RAIL_UTIL_PTI_DOUT_LOC
    .doutLoc = SL_RAIL_UTIL_PTI_DOUT_LOC,
  #endif // SL_RAIL_UTIL_PTI_DOUT_LOC
#endif // dout support
#if defined(SL_RAIL_UTIL_PTI_DCLK_PORT) && defined(SL_RAIL_UTIL_PTI_DCLK_PIN)
    .dclkPort = (uint8_t)SL_RAIL_UTIL_PTI_DCLK_PORT,
    .dclkPin = SL_RAIL_UTIL_PTI_DCLK_PIN,
  #ifdef SL_RAIL_UTIL_PTI_DCLK_LOC
    .dclkLoc = SL_RAIL_UTIL_PTI_DCLK_LOC,
  #endif // SL_RAIL_UTIL_PTI_DCLK_LOC
#endif // dclk support
#if defined(SL_RAIL_UTIL_PTI_DFRAME_PORT) && defined(SL_RAIL_UTIL_PTI_DFRAME_PIN)
    .dframePort = (uint8_t)SL_RAIL_UTIL_PTI_DFRAME_PORT,
    .dframePin = SL_RAIL_UTIL_PTI_DFRAME_PIN,
  #ifdef SL_RAIL_UTIL_PTI_DFRAME_LOC
    .dframeLoc = SL_RAIL_UTIL_PTI_DFRAME_LOC,
  #endif // SL_RAIL_UTIL_PTI_DFRAME_LOC
#endif // dframe support
  };

  RAIL_ConfigPti(RAIL_EFR32_HANDLE, &railPtiConfig);
}
