/***************************************************************************//**
 * @file
 * @brief Header file of a sample of a custom EZSP protocol.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

// This is set according to the ZigBee specification's manufacturer ID list
// Ember's manufacturer ID value is 0x1002.
#define XNCP_MANUFACTURER_ID  EMBER_MANUFACTURER_ID
#define XNCP_VERSION_NUMBER   0xBEEF

// For this sample app all the custom EZSP messages are formatted as follows:
//   <Custom command ID:1>
//   <parameters:0-118>

// -----------------------------------------------------------------------------
// Protocol specs:

// ----------------------
// SET_POWER_MODE
// Command: Command ID (1 byte) + power mode (1 byte)
// Response: Status (1 byte)
// ----------------------
// GET_POWER_MODE
// Command: Command ID (1 byte)
// Response: Status (1 byte) + power mode (1 byte)
// ----------------------
// ADD_CLUSTER_TO_FILTERING_LIST
// Command: Command ID (1 byte) + cluster ID (2 bytes)
// Response: Status (1 byte)
// ----------------------
// REMOVE_CLUSTER_TO_FILTERING_LIST
// Command: Command ID (1 byte) + cluster ID (2 bytes)
// Response: Status (1 byte)
// ----------------------
// GET_CLUSTER_TO_FILTERING_LIST
// Command: Command ID (1 byte)
// Response: Status (1 byte)  + num. entries (1 byte) + cluster IDs (variable)
// ----------------------
// ENABLE_PERIODIC_REPORTS
// Command: Command ID (1 byte) + time interval (secs.) (2 bytes)
// Response: Status (1 byte)
// ----------------------
// DISABLE_PERIODIC_REPORTS
// Command: Command ID (1 byte)
// Response: Status (1 byte)
// ----------------------
// SET_CUSTOM_TOKEN
// Command: Command ID (1 byte) + nodeType (1 byte) + nodeID (2 bytes) +
//          + panID (2 bytes)
// Response: Status (1 byte)
// ----------------------
// GET_CUSTOM_TOKEN
// Command: Command ID (1 byte)
// Response: Status (1 byte) + nodeType (1 byte) + nodeID (2 bytes) +
//           + panID (2 bytes)
// ----------------------
// CALLBACK_REPORT
// Command: Command ID (1 byte) + report count (2 bytes)
// Response: -

// -----------------------------------------------------------------------------

// Custom command IDs
enum {
  // HOST -> XNCP
  EMBER_CUSTOM_EZSP_COMMAND_SET_POWER_MODE                               = 0x00,
  EMBER_CUSTOM_EZSP_COMMAND_GET_POWER_MODE                               = 0x01,
  EMBER_CUSTOM_EZSP_COMMAND_ADD_CLUSTER_TO_FILTERING_LIST                = 0x02,
  EMBER_CUSTOM_EZSP_COMMAND_REMOVE_CLUSTER_TO_FILTERING_LIST             = 0x03,
  EMBER_CUSTOM_EZSP_COMMAND_GET_CLUSTER_FILTERING_LIST                   = 0x04,
  EMBER_CUSTOM_EZSP_COMMAND_ENABLE_PERIODIC_REPORTS                      = 0x05,
  EMBER_CUSTOM_EZSP_COMMAND_DISABLE_PERIODIC_REPORTS                     = 0x06,
  EMBER_CUSTOM_EZSP_COMMAND_SET_CUSTOM_TOKEN                             = 0x07,
  EMBER_CUSTOM_EZSP_COMMAND_GET_CUSTOM_TOKEN                             = 0x08,
  EMBER_CUSTOM_EZSP_COMMAND_SET_BLE_ADVERTISEMENT_PARAMS                 = 0x09,
  EMBER_CUSTOM_EZSP_COMMAND_SET_BLE_CONNECTION_PARAMS                    = 0x0A,
  EMBER_CUSTOM_EZSP_COMMAND_SET_BLE_ADVERTISEMENT_MODE                   = 0x0B,
  EMBER_CUSTOM_EZSP_COMMAND_OPEN_BLE_CONNECTION                          = 0x0C,
  EMBER_CUSTOM_EZSP_COMMAND_PRINT_BLE_CONNECTION_TABLE                   = 0x0D,
  EMBER_CUSTOM_EZSP_COMMAND_INIT_APP_GP_TX_QUEUE                         = 0x0E,
  EMBER_CUSTOM_EZSP_COMMAND_SET_APP_GP_TX_QUEUE                          = 0x0F,
  EMBER_CUSTOM_EZSP_COMMAND_SEND_APP_GP_RAW                              = 0x10,
  EMBER_CUSTOM_EZSP_COMMAND_GET_APP_GP_TX_QUEUE                          = 0x11,
  EMBER_CUSTOM_EZSP_COMMAND_SET_APP_GP_TX_QUEUE_MAX_SIZE                 = 0x12,
  EMBER_CUSTOM_EZSP_COMMAND_GET_APP_GP_TX_QUEUE_MAX_SIZE                 = 0x13,
  EMBER_CUSTOM_EZSP_COMMAND_GET_APP_GP_TX_QUEUE_COUNT                    = 0x14,
  EMBER_CUSTOM_EZSP_COMMAND_REMOVE_APP_GP_TX_QUEUE                       = 0x05,

  // XNCP -> HOST
  EMBER_CUSTOM_EZSP_CALLBACK_REPORT                                      = 0x80,
  EMBER_CUSTOM_EZSP_CALLBACK_APP_GP_TX_QUEUE_EVENT                       = 0x81,
};

// Power modes
enum {
  EMBER_XNCP_NORMAL_MODE,
  EMBER_XNCP_LOW_POWER_MODE,
  EMBER_XNCP_RESERVED // always last in the enum
};
