/***************************************************************************//**
 * @file sli_bt_periodic_adv_config.c
 * @brief Configuration instance for "periodic_adv"
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include "sl_component_catalog.h"
#include "sli_bt_periodic_adv_config.h"

// This deprecated "periodic_adv" component does not have a dedicated
// configuration file if the Bluetooth stack is present. In that case we can
// always instantiate the config using configuration from the Bluetooth stack
// and configure all advertisers to be capable of periodic advertising.
#if defined(SL_CATALOG_BLUETOOTH_PRESENT)

#include "sl_bluetooth.h" // For SL_BT_COMPONENT_ADVERTISERS
#include "sl_bluetooth_config.h"
SLI_BT_DEFINE_FEATURE_CONFIG(bt, periodic_adv, sli_bt_periodic_adv_config_t) = {
  .max_periodic_advertisers = SL_BT_CONFIG_MAX_ADVERTISERS
};

#else // SL_CATALOG_BLUETOOTH_PRESENT

// Bluetooth stack is not present. Instantiate the configuration if the
// component is present and use its dedicated configuration.
#if defined(SL_CATALOG_BLUETOOTH_FEATURE_PERIODIC_ADV_PRESENT)
#include "sl_bluetooth_periodic_adv_config.h"
SLI_BT_DEFINE_FEATURE_CONFIG(bt, periodic_adv, sli_bt_periodic_adv_config_t) = {
  .max_periodic_advertisers = SL_BT_CONFIG_MAX_PERIODIC_ADVERTISERS
};
#endif // SL_CATALOG_BLUETOOTH_FEATURE_PERIODIC_ADV_PRESENT

#endif // SL_CATALOG_BLUETOOTH_PRESENT
