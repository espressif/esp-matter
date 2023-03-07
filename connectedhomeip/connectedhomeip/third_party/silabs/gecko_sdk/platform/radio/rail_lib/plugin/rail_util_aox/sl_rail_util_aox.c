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

#include "rail.h"
#include "rail_ble.h"
#include "sl_rail_util_aox.h"
#include "sl_rail_util_aox_config.h"
#include "em_gpio.h"

#if SL_RAIL_UTIL_AOX_ANTENNA_PIN_COUNT
static RAIL_BLE_AoxAntennaPortPins_t antennaPortPin[] = {
#ifdef SL_RAIL_UTIL_AOX_ANTENNA_PIN0_PORT
  {
    .antPort = (uint8_t)SL_RAIL_UTIL_AOX_ANTENNA_PIN0_PORT,
    .antPin = SL_RAIL_UTIL_AOX_ANTENNA_PIN0_PIN
  },
#endif //SL_RAIL_UTIL_AOX_ANTENNA_PIN0_PORT
#ifdef SL_RAIL_UTIL_AOX_ANTENNA_PIN1_PORT
  {
    .antPort = (uint8_t)SL_RAIL_UTIL_AOX_ANTENNA_PIN1_PORT,
    .antPin = SL_RAIL_UTIL_AOX_ANTENNA_PIN1_PIN
  },
#endif //SL_RAIL_UTIL_AOX_ANTENNA_PIN1_PORT
#ifdef SL_RAIL_UTIL_AOX_ANTENNA_PIN2_PORT
  {
    .antPort = (uint8_t)SL_RAIL_UTIL_AOX_ANTENNA_PIN2_PORT,
    .antPin = SL_RAIL_UTIL_AOX_ANTENNA_PIN2_PIN
  },
#endif //SL_RAIL_UTIL_AOX_ANTENNA_PIN2_PORT
#ifdef SL_RAIL_UTIL_AOX_ANTENNA_PIN3_PORT
  {
    .antPort = (uint8_t)SL_RAIL_UTIL_AOX_ANTENNA_PIN3_PORT,
    .antPin = SL_RAIL_UTIL_AOX_ANTENNA_PIN3_PIN
  },
#endif //SL_RAIL_UTIL_AOX_ANTENNA_PIN3_PORT
#ifdef SL_RAIL_UTIL_AOX_ANTENNA_PIN4_PORT
  {
    .antPort = (uint8_t)SL_RAIL_UTIL_AOX_ANTENNA_PIN4_PORT,
    .antPin = SL_RAIL_UTIL_AOX_ANTENNA_PIN4_PIN
  },
#endif //SL_RAIL_UTIL_AOX_ANTENNA_PIN4_PORT
#ifdef SL_RAIL_UTIL_AOX_ANTENNA_PIN5_PORT
  {
    .antPort = (uint8_t)SL_RAIL_UTIL_AOX_ANTENNA_PIN5_PORT,
    .antPin = SL_RAIL_UTIL_AOX_ANTENNA_PIN5_PIN
  },
#endif //SL_RAIL_UTIL_AOX_ANTENNA_PIN5_PORT
};
#endif //SL_RAIL_UTIL_AOX_ANTENNA_PIN_COUNT

static RAIL_BLE_AoxAntennaConfig_t antennaConfig = {
#if SL_RAIL_UTIL_AOX_ANTENNA_PIN_COUNT
  // If no AoX antenna pins are selected CTE can be received/transmitted on the default antenna
  .antPortPin = antennaPortPin,
#endif //SL_RAIL_UTIL_AOX_ANTENNA_PIN_COUNT
  .antCount = SL_RAIL_UTIL_AOX_ANTENNA_PIN_COUNT
};

void sl_rail_util_aox_init(void)
{
  (void) RAIL_BLE_ConfigAoxAntenna(RAIL_EFR32_HANDLE, &antennaConfig);
}
