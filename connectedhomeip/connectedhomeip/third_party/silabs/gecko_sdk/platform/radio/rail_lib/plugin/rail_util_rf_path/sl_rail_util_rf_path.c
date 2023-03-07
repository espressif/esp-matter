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
#include "sl_rail_util_rf_path.h"
#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_1)  || defined(_SILICON_LABS_32B_SERIES_2_CONFIG_3)
#include "sl_rail_util_rf_path_config.h"
#endif //defined(_SILICON_LABS_32B_SERIES_2_CONFIG_1)  || defined(_SILICON_LABS_32B_SERIES_2_CONFIG_3)

void sl_rail_util_rf_path_init(void)
{
  RAIL_AntennaConfig_t antennaConfig = { 0 };
#if defined(SL_RAIL_UTIL_RF_PATH_INT_RF_PATH_MODE) // efr32xg2x chip-specific
  antennaConfig.defaultPath = SL_RAIL_UTIL_RF_PATH_INT_RF_PATH_MODE;
#endif // internal rf path
  (void) RAIL_ConfigAntenna(RAIL_EFR32_HANDLE, &antennaConfig);
}
