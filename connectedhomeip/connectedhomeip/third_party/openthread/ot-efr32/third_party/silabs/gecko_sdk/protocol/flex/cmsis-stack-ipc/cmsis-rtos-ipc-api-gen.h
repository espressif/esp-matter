/***************************************************************************//**
 * @brief Enumerations for vNCP.
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

// *** Generated file. Do not edit! ***
// vNCP Version: 1.0

#ifndef __CMSIS_RTOS_IPC_API_GEN_H__
#define __CMSIS_RTOS_IPC_API_GEN_H__

#define VNCP_VERSION 0x0100
//------------------------------------------------------------------------------
// Frame IDs
enum {
// Core
  EMBER_NETWORK_STATE_IPC_COMMAND_ID                                   = 0x6902,
  EMBER_STACK_IS_UP_IPC_COMMAND_ID                                     = 0x6903,
  EMBER_SET_SECURITY_KEY_IPC_COMMAND_ID                                = 0x6904,
  EMBER_GET_SECURITY_KEY_IPC_COMMAND_ID                                = 0x694F,
  EMBER_SET_PSA_SECURITY_KEY_IPC_COMMAND_ID                            = 0x694C,
  EMBER_REMOVE_PSA_SECURITY_KEY_IPC_COMMAND_ID                         = 0x694D,
  EMBER_GET_COUNTER_IPC_COMMAND_ID                                     = 0x6905,
  EMBER_SET_RADIO_CHANNEL_EXTENDED_IPC_COMMAND_ID                      = 0x694B,
  EMBER_SET_RADIO_CHANNEL_IPC_COMMAND_ID                               = 0x6906,
  EMBER_GET_RADIO_CHANNEL_IPC_COMMAND_ID                               = 0x6907,
  EMBER_SET_RADIO_POWER_IPC_COMMAND_ID                                 = 0x6908,
  EMBER_GET_RADIO_POWER_IPC_COMMAND_ID                                 = 0x6909,
  EMBER_SET_RADIO_POWER_MODE_IPC_COMMAND_ID                            = 0x690A,
  EMBER_SET_MAC_PARAMS_IPC_COMMAND_ID                                  = 0x690B,
  EMBER_CURRENT_STACK_TASKS_IPC_COMMAND_ID                             = 0x690C,
  EMBER_OK_TO_NAP_IPC_COMMAND_ID                                       = 0x690D,
  EMBER_OK_TO_HIBERNATE_IPC_COMMAND_ID                                 = 0x690E,
  EMBER_GET_EUI64_IPC_COMMAND_ID                                       = 0x690F,
  EMBER_MAC_GET_PARENT_ADDRESS_IPC_COMMAND_ID                          = 0x693A,
  EMBER_IS_LOCAL_EUI64_IPC_COMMAND_ID                                  = 0x6910,
  EMBER_GET_NODE_ID_IPC_COMMAND_ID                                     = 0x6911,
  EMBER_GET_PAN_ID_IPC_COMMAND_ID                                      = 0x6912,
  EMBER_GET_PARENT_ID_IPC_COMMAND_ID                                   = 0x6913,
  EMBER_GET_NODE_TYPE_IPC_COMMAND_ID                                   = 0x6914,
  EMBER_GET_CSP_VERSION_IPC_COMMAND_ID                                 = 0x6915,
  EMBER_CALIBRATE_CURRENT_CHANNEL_IPC_COMMAND_ID                       = 0x692F,
  EMBER_CALIBRATE_CURRENT_CHANNEL_EXTENDED_IPC_COMMAND_ID              = 0x6946,
  EMBER_APPLY_IR_CALIBRATION_IPC_COMMAND_ID                            = 0x6947,
  EMBER_TEMP_CALIBRATION_IPC_COMMAND_ID                                = 0x6948,
  EMBER_GET_CAL_TYPE_IPC_COMMAND_ID                                    = 0x6949,
  EMBER_GET_MAXIMUM_PAYLOAD_LENGTH_IPC_COMMAND_ID                      = 0x6930,
  EMBER_SET_INDIRECT_QUEUE_TIMEOUT_IPC_COMMAND_ID                      = 0x694A,
  EMBER_STACK_STATUS_HANDLER_IPC_COMMAND_ID                            = 0x6300,
  EMBER_CHILD_JOIN_HANDLER_IPC_COMMAND_ID                              = 0x6301,
  EMBER_RADIO_NEEDS_CALIBRATING_HANDLER_IPC_COMMAND_ID                 = 0x6330,
// Messaging
  EMBER_MESSAGE_SEND_IPC_COMMAND_ID                                    = 0x6916,
  EMBER_POLL_FOR_DATA_IPC_COMMAND_ID                                   = 0x6917,
  EMBER_MAC_MESSAGE_SEND_IPC_COMMAND_ID                                = 0x692B,
  EMBER_MAC_SET_PAN_COORDINATOR_IPC_COMMAND_ID                         = 0x6938,
  EMBER_SET_POLL_DESTINATION_ADDRESS_IPC_COMMAND_ID                    = 0x692D,
  EMBER_REMOVE_CHILD_IPC_COMMAND_ID                                    = 0x6931,
  EMBER_GET_CHILD_FLAGS_IPC_COMMAND_ID                                 = 0x6932,
  EMBER_GET_CHILD_INFO_IPC_COMMAND_ID                                  = 0x6933,
  EMBER_PURGE_INDIRECT_MESSAGES_IPC_COMMAND_ID                         = 0x692E,
  EMBER_MAC_ADD_SHORT_TO_LONG_ADDRESS_MAPPING_IPC_COMMAND_ID           = 0x693E,
  EMBER_MAC_CLEAR_SHORT_TO_LONG_ADDRESS_MAPPINGS_IPC_COMMAND_ID        = 0x693F,
  EMBER_NETWORK_LEAVE_IPC_COMMAND_ID                                   = 0x6940,
  EMBER_MESSAGE_SENT_HANDLER_IPC_COMMAND_ID                            = 0x6302,
  EMBER_INCOMING_MESSAGE_HANDLER_IPC_COMMAND_ID                        = 0x6303,
  EMBER_INCOMING_MAC_MESSAGE_HANDLER_IPC_COMMAND_ID                    = 0x630B,
  EMBER_MAC_MESSAGE_SENT_HANDLER_IPC_COMMAND_ID                        = 0x630C,
// Network Management
  EMBER_NETWORK_INIT_IPC_COMMAND_ID                                    = 0x6918,
  EMBER_START_ACTIVE_SCAN_IPC_COMMAND_ID                               = 0x6919,
  EMBER_START_ENERGY_SCAN_IPC_COMMAND_ID                               = 0x691A,
  EMBER_SET_APPLICATION_BEACON_PAYLOAD_IPC_COMMAND_ID                  = 0x691B,
  EMBER_SET_SELECTIVE_JOIN_PAYLOAD_IPC_COMMAND_ID                      = 0x693C,
  EMBER_CLEAR_SELECTIVE_JOIN_PAYLOAD_IPC_COMMAND_ID                    = 0x693D,
  EMBER_FORM_NETWORK_IPC_COMMAND_ID                                    = 0x691C,
  EMBER_JOIN_NETWORK_EXTENDED_IPC_COMMAND_ID                           = 0x691D,
  EMBER_JOIN_NETWORK_IPC_COMMAND_ID                                    = 0x691E,
  EMBER_MAC_FORM_NETWORK_IPC_COMMAND_ID                                = 0x693B,
  EMBER_PERMIT_JOINING_IPC_COMMAND_ID                                  = 0x691F,
  EMBER_JOIN_COMMISSIONED_IPC_COMMAND_ID                               = 0x6920,
  EMBER_RESET_NETWORK_STATE_IPC_COMMAND_ID                             = 0x6921,
  EMBER_GET_STANDALONE_BOOTLOADER_INFO_IPC_COMMAND_ID                  = 0x6922,
  EMBER_LAUNCH_STANDALONE_BOOTLOADER_IPC_COMMAND_ID                    = 0x6923,
  EMBER_FREQUENCY_HOPPING_SET_CHANNEL_MASK_IPC_COMMAND_ID              = 0x692C,
  EMBER_FREQUENCY_HOPPING_START_SERVER_IPC_COMMAND_ID                  = 0x6926,
  EMBER_FREQUENCY_HOPPING_START_CLIENT_IPC_COMMAND_ID                  = 0x6927,
  EMBER_FREQUENCY_HOPPING_STOP_IPC_COMMAND_ID                          = 0x6928,
  EMBER_SET_AUXILIARY_ADDRESS_FILTERING_ENTRY_IPC_COMMAND_ID           = 0x6929,
  EMBER_GET_AUXILIARY_ADDRESS_FILTERING_ENTRY_IPC_COMMAND_ID           = 0x692A,
  EMBER_START_TX_STREAM_IPC_COMMAND_ID                                 = 0x6943,
  EMBER_STOP_TX_STREAM_IPC_COMMAND_ID                                  = 0x6944,
  EMBER_SET_ACTIVE_SCAN_DURATION_IPC_COMMAND_ID                        = 0x6941,
  EMBER_GET_ACTIVE_SCAN_DURATION_IPC_COMMAND_ID                        = 0x6942,
  EMBER_GET_DEFAULT_CHANNEL_IPC_COMMAND_ID                             = 0x6945,
  EMBER_INCOMING_BEACON_HANDLER_IPC_COMMAND_ID                         = 0x6339,
  EMBER_ACTIVE_SCAN_COMPLETE_HANDLER_IPC_COMMAND_ID                    = 0x6305,
  EMBER_ENERGY_SCAN_COMPLETE_HANDLER_IPC_COMMAND_ID                    = 0x6306,
  EMBER_GET_STANDALONE_BOOTLOADER_INFO_RETURN_IPC_COMMAND_ID           = 0x6307,
  EMBER_LAUNCH_STANDALONE_BOOTLOADER_RETURN_IPC_COMMAND_ID             = 0x6308,
  EMBER_FREQUENCY_HOPPING_START_CLIENT_COMPLETE_HANDLER_IPC_COMMAND_ID = 0x630A,
// EMBER_TEST
  EMBER_ECHO_IPC_COMMAND_ID                                            = 0x6924,
  EMBER_ECHO_RETURN_IPC_COMMAND_ID                                     = 0x6309,
// APP_USES_SOFTWARE_FLOW_CONTROL
  EMBER_START_XON_XOFF_TEST_IPC_COMMAND_ID                             = 0x6925,
};

#endif // __CMSIS_RTOS_IPC_API_GEN_H__
