/***************************************************************************//**
 * @file sli_bt_sync_config.c
 * @brief Configuration instance for "sync"
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
#include "sli_bt_sync_config.h"

#if defined(SL_CATALOG_BLUETOOTH_FEATURE_SYNC_PRESENT)
#include "sl_bluetooth_periodic_sync_config.h"

SLI_BT_DEFINE_FEATURE_CONFIG(bt, sync, sli_bt_sync_config_t) = {
  .max_periodic_syncs = SL_BT_CONFIG_MAX_PERIODIC_ADVERTISING_SYNC
};

#endif // SL_CATALOG_BLUETOOTH_FEATURE_SYNC_PRESENT
