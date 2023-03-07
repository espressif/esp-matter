/***************************************************************************//**
 * @file sli_bt_dynamic_gattdb_config.h
 * @brief Configuration types for "dynamic_gattdb"
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

#ifndef SLI_BT_DYNAMIC_GATTDB_CONFIG_H
#define SLI_BT_DYNAMIC_GATTDB_CONFIG_H

#include "sli_bt_config_defs.h"

/**
 * @brief Configuration structure used by "dynamic_gattdb"
 */
typedef struct {
  uint8_t max_dynamic_gattdbs; ///< Set to value of SL_BT_CONFIG_MAX_DYNAMIC_GATTDBS_SUM
} sli_bt_dynamic_gattdb_config_t;

#endif // SLI_BT_DYNAMIC_GATTDB_CONFIG_H
