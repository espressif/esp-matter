/***************************************************************************//**
 * @file sli_bt_advertiser_config.c
 * @brief Configuration instance for "advertiser"
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
#include "sli_bt_advertiser_config.h"

#if defined(SL_CATALOG_BLUETOOTH_FEATURE_ADVERTISER_PRESENT)
#include "sl_bluetooth.h" // For SL_BT_COMPONENT_ADVERTISERS
#include "sl_bluetooth_config.h"

// Note that advertiser configuration is not dictated directly by the advertiser
// component's configuration file. The top-level Bluetooth stack configuration
// calculates a sum of all advertiser needs, and that value is used to configure
// the advertiser feature.
SLI_BT_DEFINE_FEATURE_CONFIG(bt, advertiser, sli_bt_advertiser_config_t) = {
  .max_advertisers = SL_BT_CONFIG_MAX_ADVERTISERS
};

#endif // SL_CATALOG_BLUETOOTH_FEATURE_ADVERTISER_PRESENT
