/***************************************************************************//**
 *@brief Bluetooth LE Power Control feature initialization
 *******************************************************************************
 *# License
 * <b> Copyright 2020 Silicon Laboratories Inc.www.silabs.com </b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement(MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

// TODO, BG-10780: This special init file is only used in the controller-only
// build. The host stack build handles the init via the new feature definition
// mechanism. The controller should migrate to use the same mechanism and the
// same config instance.

#include "sl_status.h"
#include "sl_bt_power_control_config.h"
#include "sl_bt_ll_config.h"
#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif

#define SL_BT_POWER_CONTROL_CONFIG                              \
  {                                                             \
    .activate_power_control = SL_BT_ACTIVATE_POWER_CONTROL,     \
    .golden_rssi_min_1m = SL_BT_GOLDEN_RSSI_MIN_1M,             \
    .golden_rssi_max_1m = SL_BT_GOLDEN_RSSI_MAX_1M,             \
    .golden_rssi_min_2m = SL_BT_GOLDEN_RSSI_MIN_2M,             \
    .golden_rssi_max_2m = SL_BT_GOLDEN_RSSI_MAX_2M,             \
    .golden_rssi_min_coded_s8 = SL_BT_GOLDEN_RSSI_MIN_CODED_S8, \
    .golden_rssi_max_coded_s8 = SL_BT_GOLDEN_RSSI_MAX_CODED_S8, \
    .golden_rssi_min_coded_s2 = SL_BT_GOLDEN_RSSI_MIN_CODED_S2, \
    .golden_rssi_max_coded_s2 = SL_BT_GOLDEN_RSSI_MAX_CODED_S2, \
  }

extern sl_status_t ll_connPowerControlEnable(const sl_bt_ll_power_control_config_t *);
extern sl_status_t ll_initDefaultPowerLevelRange(int16_t minPower, int16_t maxPower);

sl_status_t sl_bt_init_power_control()
{
  sl_status_t st = ll_initDefaultPowerLevelRange(SL_BT_DEFAULT_MIN_POWER_LEVEL,
                                                 SL_BT_DEFAULT_MAX_POWER_LEVEL);
  if (st) {
    return st;
  }

  sl_bt_ll_power_control_config_t config = SL_BT_POWER_CONTROL_CONFIG;
  return ll_connPowerControlEnable(&config);
}
