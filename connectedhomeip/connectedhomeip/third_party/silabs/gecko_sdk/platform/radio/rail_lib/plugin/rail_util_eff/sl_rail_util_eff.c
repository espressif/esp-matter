/***************************************************************************//**
 * @file
 * @brief Implements an API for initializing and controlling the EFF.
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

#include "sl_rail_util_eff_config.h"
#include "sl_rail_util_eff.h"

#ifndef SL_RAIL_UTIL_EFF_CTRL0_PORT
#error "SL_RAIL_UTIL_EFF_CTRL0 undefined"
#endif //SL_RAIL_UTIL_EFF_CTRL0_PORT

#ifndef SL_RAIL_UTIL_EFF_CTRL1_PORT
#error "SL_RAIL_UTIL_EFF_CTRL1 undefined"
#endif //SL_RAIL_UTIL_EFF_CTRL1_PORT

#ifndef SL_RAIL_UTIL_EFF_CTRL2_PORT
#error "SL_RAIL_UTIL_EFF_CTRL2 undefined"
#endif //SL_RAIL_UTIL_EFF_CTRL2_PORT

#ifndef SL_RAIL_UTIL_EFF_CTRL3_PORT
#error "SL_RAIL_UTIL_EFF_CTRL3 undefined"
#endif //SL_RAIL_UTIL_EFF_CTRL3_PORT

#ifndef SL_RAIL_UTIL_EFF_TEST_PORT
#error "SL_RAIL_UTIL_EFF_TEST undefined"
#endif //SL_RAIL_UTIL_EFF_TEST_PORT

#ifndef SL_RAIL_UTIL_EFF_SENSE_PORT
#error "SL_RAIL_UTIL_EFF_SENSE undefined"
#endif //SL_RAIL_UTIL_EFF_SENSE_PORT

#if ((SL_RAIL_UTIL_EFF_LNA_MODE_RURAL_ENABLE == 0) \
  && (SL_RAIL_UTIL_EFF_LNA_MODE_URBAN_ENABLE == 0) \
  && (SL_RAIL_UTIL_EFF_LNA_MODE_BYPASS_ENABLE == 0))
#error "At least one EFF LNA mode must be enabled"
#endif

#ifndef SL_RAIL_UTIL_EFF_RURAL_URBAN_MV
#define SL_RAIL_UTIL_EFF_RURAL_URBAN_MV   120U
#endif
#ifndef SL_RAIL_UTIL_EFF_URBAN_BYPASS_MV
#define SL_RAIL_UTIL_EFF_URBAN_BYPASS_MV  130U
#endif
#ifndef SL_RAIL_UTIL_EFF_URBAN_DWELL_TIME_MS
#define SL_RAIL_UTIL_EFF_URBAN_DWELL_TIME_MS   30000UL
#endif
#ifndef SL_RAIL_UTIL_EFF_BYPASS_DWELL_TIME_MS
#define SL_RAIL_UTIL_EFF_BYPASS_DWELL_TIME_MS  30000UL
#endif
#ifndef SL_RAIL_UTIL_EFF_TEMPERATURE_THRESHOLD_EFF_DEGREES_K
#define SL_RAIL_UTIL_EFF_TEMPERATURE_THRESHOLD_EFF_DEGREES_K 373U
#endif
#ifndef SL_RAIL_UTIL_EFF_TEMPERATURE_THRESHOLD_INTERNAL_DEGREES_K
#define SL_RAIL_UTIL_EFF_TEMPERATURE_THRESHOLD_INTERNAL_DEGREES_K 373U
#endif

RAIL_Status_t sl_rail_util_eff_init(void)
{
  RAIL_EffConfig_t effConfig = {
    .device = SL_RAIL_UTIL_EFF_DEVICE,
    .ctrl0Port = SL_RAIL_UTIL_EFF_CTRL0_PORT,
    .ctrl0Pin = SL_RAIL_UTIL_EFF_CTRL0_PIN,
    .ctrl1Port = SL_RAIL_UTIL_EFF_CTRL1_PORT,
    .ctrl1Pin = SL_RAIL_UTIL_EFF_CTRL1_PIN,
    .ctrl2Port = SL_RAIL_UTIL_EFF_CTRL2_PORT,
    .ctrl2Pin = SL_RAIL_UTIL_EFF_CTRL2_PIN,
    .ctrl3Port = SL_RAIL_UTIL_EFF_CTRL3_PORT,
    .ctrl3Pin = SL_RAIL_UTIL_EFF_CTRL3_PIN,
    .testPort = SL_RAIL_UTIL_EFF_TEST_PORT,
    .testPin = SL_RAIL_UTIL_EFF_TEST_PIN,
    .sensePort = SL_RAIL_UTIL_EFF_SENSE_PORT,
    .sensePin = SL_RAIL_UTIL_EFF_SENSE_PIN,
    .enabledLnaModes = (((SL_RAIL_UTIL_EFF_LNA_MODE_RURAL_ENABLE) ? RAIL_EFF_LNA_MODE_RURAL : 0U)
                        | ((SL_RAIL_UTIL_EFF_LNA_MODE_URBAN_ENABLE) ? RAIL_EFF_LNA_MODE_URBAN : 0U)
                        | ((SL_RAIL_UTIL_EFF_LNA_MODE_BYPASS_ENABLE) ? RAIL_EFF_LNA_MODE_BYPASS : 0U)),
    .ruralUrbanMv = SL_RAIL_UTIL_EFF_RURAL_URBAN_MV,
    .urbanBypassMv = SL_RAIL_UTIL_EFF_URBAN_BYPASS_MV,
    .urbanDwellTimeMs = SL_RAIL_UTIL_EFF_URBAN_DWELL_TIME_MS,
    .bypassDwellTimeMs = SL_RAIL_UTIL_EFF_BYPASS_DWELL_TIME_MS,
    .clpcSlowLoopTarget = SL_RAIL_UTIL_EFF_CLPC_SLOW_LOOP_TARGET,
    .clpcSlowLoopSlope = SL_RAIL_UTIL_EFF_CLPC_SLOW_LOOP_SLOPE,
    .clpcFastLoopTarget = SL_RAIL_UTIL_EFF_CLPC_FAST_LOOP_TARGET,
    .clpcFastLoopSlope = SL_RAIL_UTIL_EFF_CLPC_FAST_LOOP_SLOPE,
    .clpcEnable = SL_RAIL_UTIL_EFF_CLPC_ENABLE,
    .maxTxContinuousPowerDbm = RAIL_UTIL_EFF_MAX_TX_CONTINUOUS_POWER_DBM,
    .maxTxDutyCycle = RAIL_UTIL_EFF_MAX_TX_DUTY_CYCLE,
    .effTempThreshold = SL_RAIL_UTIL_EFF_TEMPERATURE_THRESHOLD_EFF_DEGREES_K,
    .internalTempThreshold = SL_RAIL_UTIL_EFF_TEMPERATURE_THRESHOLD_INTERNAL_DEGREES_K,
  };

  return RAIL_ConfigEff(RAIL_EFR32_HANDLE, &effConfig);
}
