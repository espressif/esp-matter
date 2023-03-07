/***************************************************************************//**
 * @file sli_bt_connection_config.c
 * @brief Configuration instanc for "connection"
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
#include "sli_bt_connection_config.h"

#if defined(SL_CATALOG_BLUETOOTH_FEATURE_CONNECTION_PRESENT)
#include "sl_bluetooth.h" // For SL_BT_COMPONENT_CONNECTIONS
#include "sl_bluetooth_config.h"

// Note that connection configuration is not dictated directly by the connection
// component's configuration file. The top-level Bluetooth stack configuration
// calculates a sum of all connection needs, and that value is used to configure
// the connection feature.
SLI_BT_DEFINE_FEATURE_CONFIG(bt, connection, sli_bt_connection_config_t) = {
  .max_connections = SL_BT_CONFIG_MAX_CONNECTIONS_SUM
};

#endif // SL_CATALOG_BLUETOOTH_FEATURE_CONNECTION_PRESENT
