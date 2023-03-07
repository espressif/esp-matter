/***************************************************************************//**
 * @file sli_bt_pawr_advertiser_config.h
 * @brief Configuration types for "pawr_advertiser"
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

#ifndef SLI_BT_PAWR_ADVERTISER_CONFIG_H
#define SLI_BT_PAWR_ADVERTISER_CONFIG_H

#include "sli_bt_config_defs.h"

/**
 * @brief Configuration structure used by "pawr_advertiser"
 *
 * See "sl_bt_pawr_advertiser_config.h" for detailed description of each
 * configuration parameter.
 */
typedef struct {
  uint8_t max_pawr_advertisers; ///< Set to value of SL_BT_CONFIG_MAX_PAWR_ADVERTISERS
} sli_bt_pawr_advertiser_config_t;

#endif // SLI_BT_PAWR_ADVERTISER_CONFIG_H
