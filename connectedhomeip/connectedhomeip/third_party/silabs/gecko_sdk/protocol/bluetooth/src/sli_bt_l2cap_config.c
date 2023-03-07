/***************************************************************************//**
 * @file sli_bt_l2cap_config.c
 * @brief Configuration instance for "l2cap"
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
#include "sli_bt_l2cap_config.h"

#if defined(SL_CATALOG_BLUETOOTH_FEATURE_L2CAP_PRESENT)
#include "sl_bluetooth_l2cap_config.h"

SLI_BT_DEFINE_FEATURE_CONFIG(bt, l2cap, sli_bt_l2cap_config_t) = {
  .max_l2cap_coc_channels = SL_BT_CONFIG_USER_L2CAP_COC_CHANNELS
};

#endif // SL_CATALOG_BLUETOOTH_FEATURE_L2CAP_PRESENT
