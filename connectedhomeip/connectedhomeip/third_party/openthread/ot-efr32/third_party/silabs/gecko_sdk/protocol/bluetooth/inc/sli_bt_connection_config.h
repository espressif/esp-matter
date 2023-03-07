/***************************************************************************//**
 * @file sli_bt_connection_config.h
 * @brief Configuration types for "connection"
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

#ifndef SLI_BT_CONNECTION_CONFIG_H
#define SLI_BT_CONNECTION_CONFIG_H

#include "sli_bt_config_defs.h"

/**
 * @brief Configuration structure used by "connection"
 */
typedef struct {
  uint8_t max_connections; ///< Set to value of SL_BT_CONFIG_MAX_CONNECTIONS_SUM
} sli_bt_connection_config_t;

#endif // SLI_BT_CONNECTION_CONFIG_H
