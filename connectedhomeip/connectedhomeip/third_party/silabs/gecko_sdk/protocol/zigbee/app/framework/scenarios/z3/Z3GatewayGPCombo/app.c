/***************************************************************************//**
 * @file app.c
 * @brief Callbacks implementation and application specific code.
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

#include "af.h"
#include "app/framework/util/af-main.h"
#include "app/framework/util/util.h"

#include "app/util/zigbee-framework/zigbee-device-common.h"
#include "stack/include/trust-center.h"
#include "zap-cluster-command-parser.h"
#include <stdlib.h>
#include "xncp-sample-custom-ezsp-protocol.h"

// The number of tokens that can be written using ezspSetToken and read using
// ezspGetToken.
#define MFGSAMP_NUM_EZSP_TOKENS 8
// The size of the tokens that can be written using ezspSetToken and read using
// ezspGetToken.
#define MFGSAMP_EZSP_TOKEN_SIZE 8
// The number of manufacturing tokens.
#define MFGSAMP_NUM_EZSP_MFG_TOKENS 11
// The size of the largest EZSP Mfg token, EZSP_MFG_CBKE_DATA. Please refer to
// app/util/ezsp/ezsp-enum.h.
#define MFGSAMP_EZSP_TOKEN_MFG_MAXSIZE 92

#define GREEN_POWER_MAX_FRAME_LENGTH 70
#define GREEN_POWER_COMMAND_INDEX 0

extern EmberStatus emberAfTrustCenterStartNetworkKeyUpdate(void);

//----------------------
// ZCL commands handling

static void ias_ace_cluster_arm_command_handler(uint8_t armMode,
                                                uint8_t* armDisarmCode,
                                                uint8_t zoneId)
{
  uint16_t armDisarmCodeLength = emberAfStringLength(armDisarmCode);
  EmberNodeId sender = emberGetSender();
  uint16_t i;

  sl_zigbee_app_debug_print("IAS ACE Arm Received %04X", armMode);

  // Start i at 1 to skip over leading character in the byte array as it is the
  // length byte
  for (i = 1; i < armDisarmCodeLength; i++) {
    sl_zigbee_app_debug_print("%c", armDisarmCode[i]);
  }
  sl_zigbee_app_debug_print(" %02X\n", zoneId);

  emberAfFillCommandIasAceClusterArmResponse(armMode);
  emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, sender);
}

static void ias_ace_cluster_bypass_command_handler(uint8_t numberOfZones,
                                                   uint8_t* zoneIds,
                                                   uint8_t* armDisarmCode)
{
  EmberNodeId sender = emberGetSender();
  uint8_t i;

  sl_zigbee_app_debug_print("IAS ACE Cluster Bypass for zones ");

  for (i = 0; i < numberOfZones; i++) {
    sl_zigbee_app_debug_print("%d ", zoneIds[i]);
  }
  sl_zigbee_app_debug_print("\n");

  emberAfFillCommandIasAceClusterBypassResponse(numberOfZones,
                                                zoneIds,
                                                numberOfZones);
  emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, sender);
}

static uint32_t zcl_ias_ace_cluster_server_command_handler(sl_service_opcode_t opcode,
                                                           sl_service_function_context_t *context)
{
  (void)opcode;

  EmberAfClusterCommand *cmd = (EmberAfClusterCommand *)context->data;

  switch (cmd->commandId) {
    case ZCL_ARM_COMMAND_ID:
    {
      sl_zcl_ias_ace_cluster_arm_command_t cmd_data;

      if (zcl_decode_ias_ace_cluster_arm_command(cmd, &cmd_data)
          != EMBER_ZCL_STATUS_SUCCESS) {
        return EMBER_ZCL_STATUS_UNSUP_COMMAND;
      }

      ias_ace_cluster_arm_command_handler(cmd_data.armMode,
                                          cmd_data.armDisarmCode,
                                          cmd_data.zoneId);
      return EMBER_ZCL_STATUS_SUCCESS;
    }
    case ZCL_BYPASS_COMMAND_ID:
    {
      sl_zcl_ias_ace_cluster_bypass_command_t cmd_data;

      if (zcl_decode_ias_ace_cluster_bypass_command(cmd, &cmd_data)
          != EMBER_ZCL_STATUS_SUCCESS) {
        return EMBER_ZCL_STATUS_UNSUP_COMMAND;
      }

      ias_ace_cluster_bypass_command_handler(cmd_data.numberOfZones,
                                             cmd_data.zoneIds,
                                             cmd_data.armDisarmCode);

      return EMBER_ZCL_STATUS_SUCCESS;
    }
  }

  return EMBER_ZCL_STATUS_UNSUP_COMMAND;
}

//----------------------
// Implemented Callbacks

/** @brief Init
 * Application init function
 */
void emberAfMainInitCallback(void)
{
  // Subscribe to ZCL commands for the IAS_ACE cluster, server side.
  sl_zigbee_subscribe_to_zcl_commands(ZCL_IAS_ACE_CLUSTER_ID,
                                      0xFFFF,
                                      ZCL_DIRECTION_CLIENT_TO_SERVER,
                                      zcl_ias_ace_cluster_server_command_handler);
}

/** @brief Green power server notification forward callback
 *
 * This function is called by the green power server plugin to notify the
 * application of a Green Power Gp Notification of an incoming gpd command.
 * Return true to handle in application.
 *
 * @returns true if application handled it and plugin will not process it anymore.
 * else return false to process the notification by the plugin  Ver.: always
 */
bool emberAfGreenPowerClusterGpNotificationForwardCallback(uint16_t options,
                                                           EmberGpAddress * addr,
                                                           uint32_t gpdSecurityFrameCounter,
                                                           uint8_t gpdCommandId,
                                                           uint8_t * gpdCommandPayload,
                                                           uint16_t gppShortAddress,
                                                           uint8_t  gppDistance)
{
  sl_zigbee_app_debug_print("Received Notification GpdAdd [AppId = %d, SrcId = 0x%04X] for Cmd = %d withRxAfterTx = %d\n",
                            addr->applicationId,
                            addr->id.sourceId,
                            gpdCommandId,
                            (options & 0x0800) ? true : false);
  return false;
}

// EZSP Custom Frame handler for messages from host
void ezspCustomFrameHandler(int8u payloadLength,
                            int8u* payload)
{
  sl_zigbee_app_debug_print("EZSP Custom Frame : ");
  sl_zigbee_app_debug_print_buffer(payload, payloadLength, true);
  sl_zigbee_app_debug_print("\n");
}

//-------------------------------------
// Custom CLI commands and related code

// The manufacturing tokens are enumerated in app/util/ezsp/ezsp-protocol.h.
// The names are enumerated here to make it easier for the user.
const char * ezspMfgTokenNames[] =
{
  "EZSP_MFG_CUSTOM_VERSION...",
  "EZSP_MFG_STRING...........",
  "EZSP_MFG_BOARD_NAME.......",
  "EZSP_MFG_MANUF_ID.........",
  "EZSP_MFG_PHY_CONFIG.......",
  "EZSP_MFG_BOOTLOAD_AES_KEY.",
  "EZSP_MFG_ASH_CONFIG.......",
  "EZSP_MFG_EZSP_STORAGE.....",
  "EZSP_STACK_CAL_DATA.......",
  "EZSP_MFG_CBKE_DATA........",
  "EZSP_MFG_INSTALLATION_CODE"
};

// Called to dump all of the tokens. This dumps the indices, the names,
// and the values using ezspGetToken and ezspGetMfgToken. The indices
// are used for read and write functions below.
void mfgappTokenDump(sl_cli_command_arg_t *arguments)
{
  (void)arguments;

  EmberStatus status;
  uint8_t tokenData[MFGSAMP_EZSP_TOKEN_MFG_MAXSIZE];
  uint8_t index, i, tokenLength;

  // first go through the tokens accessed using ezspGetToken
  sl_zigbee_app_debug_print("(data shown little endian)\n");
  sl_zigbee_app_debug_print("Tokens:\n");
  sl_zigbee_app_debug_print("idx  value:\n");
  for (index = 0; index < MFGSAMP_NUM_EZSP_TOKENS; index++) {
    // get the token data here
    status = ezspGetToken(index, tokenData);
    sl_zigbee_app_debug_print("[%d]", index);
    if (status == EMBER_SUCCESS) {
      // Print out the token data
      for (i = 0; i < MFGSAMP_EZSP_TOKEN_SIZE; i++) {
        sl_zigbee_app_debug_print(" %02X", tokenData[i]);
      }
      sl_zigbee_app_debug_print("\n");
    } else {
      // handle when ezspGetToken returns an error
      sl_zigbee_app_debug_print(" ... error 0x%x ...\n", status);
    }
  }

  // now go through the tokens accessed using ezspGetMfgToken
  // the manufacturing tokens are enumerated in app/util/ezsp/ezsp-protocol.h
  // this file contains an array (ezspMfgTokenNames) representing the names.
  sl_zigbee_app_debug_print("Manufacturing Tokens:\n");
  sl_zigbee_app_debug_print("idx  token name                 len   value\n");
  for (index = 0; index < MFGSAMP_NUM_EZSP_MFG_TOKENS; index++) {
    // ezspGetMfgToken returns a length, be careful to only access
    // valid token indices.
    tokenLength = ezspGetMfgToken(index, tokenData);
    sl_zigbee_app_debug_print("[%x] %s: 0x%x:\n",
                              index, ezspMfgTokenNames[index], tokenLength);

    // Print out the token data
    for (i = 0; i < tokenLength; i++) {
      if ((i != 0) && ((i % 8) == 0)) {
        sl_zigbee_app_debug_print("\n");
        sl_zigbee_app_debug_print("                                    :");
      }
      sl_zigbee_app_debug_print(" %02X", tokenData[i]);
    }
    sl_zigbee_app_debug_print("\n");
  }
  sl_zigbee_app_debug_print("\n");
}

void changeNwkKeyCommand(sl_cli_command_arg_t *arguments)
{
  (void)arguments;

  EmberStatus status = emberAfTrustCenterStartNetworkKeyUpdate();

  if (status != EMBER_SUCCESS) {
    sl_zigbee_app_debug_print("Change Key Error %02X\n", status);
  } else {
    sl_zigbee_app_debug_print("Change Key Success\n");
  }
}

static void dcPrintKey(uint8_t label, uint8_t *key)
{
  uint8_t i;
  sl_zigbee_app_debug_print("key %x: \n", label);
  for (i = 0; i < EMBER_ENCRYPTION_KEY_SIZE; i++) {
    sl_zigbee_app_debug_print("%02X", key[i]);
  }
  sl_zigbee_app_debug_print("\n");
}

void printNextKeyCommand(sl_cli_command_arg_t *arguments)
{
  (void)arguments;

  EmberKeyStruct nextNwkKey;
  EmberStatus status;

  status = emberGetKey(EMBER_NEXT_NETWORK_KEY,
                       &nextNwkKey);

  if (status != EMBER_SUCCESS) {
    sl_zigbee_app_debug_print("Error getting key\n");
  } else {
    dcPrintKey(1, nextNwkKey.key.contents);
  }
}

void versionCommand(sl_cli_command_arg_t *arguments)
{
  (void)arguments;

  sl_zigbee_app_debug_print("Version:  0.1 Alpha\n");
  sl_zigbee_app_debug_print(" %s\n", __DATE__);
  sl_zigbee_app_debug_print(" %s\n", __TIME__);
  sl_zigbee_app_debug_print("\n");
#ifdef EMBER_TEST
  sl_zigbee_app_debug_print("Print formatter test : 0x%x=0x12, 0x%02x=0x1234 0x%04x=0x12345678\n",
                            0x12, 0x1234, 0x12345678);
#endif
}

void setTxPowerCommand(sl_cli_command_arg_t *arguments)
{
  int8_t dBm = sl_cli_get_argument_int8(arguments, 0);

  emberSetRadioPower(dBm);
}

/* This sample application demostrates an NCP using a custom protocol to
 * communicate with the host. As an example protocol, the NCP has defined
 * commands so that the host can drive Green Power on the NCP.
 *
 * The host sends custom EZSP commands to the NCP, and the NCP acts on them
 * based on the functionality in the code found below.
 * This sample application is meant to be paired with the ncp-uart-hw-gp
 * sample application in the NCP Application Framework.
 */
static void sendGreenPowerCommand(uint8_t command,
                                  uint8_t* possibleParam,
                                  uint8_t paramLength,
                                  bool replyExpected)
{
  EmberStatus status;
  uint8_t commandLength = 0;
  uint8_t commandPayload[GREEN_POWER_MAX_FRAME_LENGTH];
  uint8_t replyLength = GREEN_POWER_MAX_FRAME_LENGTH;
  uint8_t replyPayload[GREEN_POWER_MAX_FRAME_LENGTH];

  if ((possibleParam != NULL) && (paramLength > 0)) {
    commandLength = paramLength;
    if (paramLength > GREEN_POWER_MAX_FRAME_LENGTH) {
      commandLength = GREEN_POWER_MAX_FRAME_LENGTH;
    }
    MEMCOPY(commandPayload, possibleParam, commandLength);
  } else {
    return;
  }
  // Send the command to the NCP.
  status = ezspCustomFrame(commandLength,
                           commandPayload,
                           &replyLength,
                           replyPayload);
  sl_zigbee_app_debug_print("Send custom frame Command Id = 0x%X Status = 0x%02X\n", command, status);
  sl_zigbee_app_debug_print("replyExpected = 0x%X replyLength = 0x%X\n", replyExpected, replyLength);
  // If we were expecting a response, display it.
  if (replyExpected
      && status == 0
      && replyLength > 0) {
    sl_zigbee_app_debug_print("Response : ");
    sl_zigbee_app_debug_print_buffer(replyPayload, replyLength, true);
    sl_zigbee_app_debug_print("\n");
  }
}

void gpAppGpTxQueueInit(sl_cli_command_arg_t *arguments)
{
  (void)arguments;

  uint8_t idx = 0;
  uint8_t commandPayload[GREEN_POWER_MAX_FRAME_LENGTH];
  commandPayload[idx++] = EMBER_CUSTOM_EZSP_COMMAND_INIT_APP_GP_TX_QUEUE;
  sendGreenPowerCommand(commandPayload[GREEN_POWER_COMMAND_INDEX],
                        commandPayload,
                        idx,
                        false);
}

void gpAppGpTxQueueSet(sl_cli_command_arg_t *arguments)
{
  uint8_t idx = 0;
  uint8_t commandPayload[GREEN_POWER_MAX_FRAME_LENGTH];
  size_t dataLength = 0;
  uint8_t *data;

  commandPayload[idx] = EMBER_CUSTOM_EZSP_COMMAND_SET_APP_GP_TX_QUEUE;
  idx += sizeof(uint8_t);
  commandPayload[idx] = sl_cli_get_argument_uint8(arguments, 0); // AppId = 0
  idx += sizeof(uint8_t);
  uint32_t srCId = sl_cli_get_argument_uint32(arguments, 1);
  commandPayload[idx++] = srCId >> 0;
  commandPayload[idx++] = srCId >> 8;
  commandPayload[idx++] = srCId >> 16;
  commandPayload[idx++] = srCId >> 24;
  commandPayload[idx++] = sl_cli_get_argument_uint8(arguments, 2); // command Id

  data = sl_cli_get_argument_hex(arguments, 3, &dataLength);
  (void) memcpy(&commandPayload[idx], data, dataLength);
  idx += dataLength;

  sendGreenPowerCommand(commandPayload[GREEN_POWER_COMMAND_INDEX],
                        commandPayload,
                        idx,
                        false);
}

void gpAppGpTxQueueRemove(sl_cli_command_arg_t *arguments)
{
  uint8_t idx = 0;
  uint8_t commandPayload[GREEN_POWER_MAX_FRAME_LENGTH];
  commandPayload[idx] = EMBER_CUSTOM_EZSP_COMMAND_REMOVE_APP_GP_TX_QUEUE;
  idx += sizeof(uint8_t);
  commandPayload[idx] = sl_cli_get_argument_uint8(arguments, 0); // AppId = 0
  idx += sizeof(uint8_t);
  uint32_t srCId = sl_cli_get_argument_uint32(arguments, 1);
  commandPayload[idx++] = srCId >> 0;
  commandPayload[idx++] = srCId >> 8;
  commandPayload[idx++] = srCId >> 16;
  commandPayload[idx++] = srCId >> 24;
  sendGreenPowerCommand(commandPayload[GREEN_POWER_COMMAND_INDEX],
                        commandPayload,
                        idx,
                        false);
}

void gpAppGpTxQueueGet(sl_cli_command_arg_t *arguments)
{
  uint8_t idx = 0;
  uint8_t commandPayload[GREEN_POWER_MAX_FRAME_LENGTH];
  commandPayload[idx] = EMBER_CUSTOM_EZSP_COMMAND_GET_APP_GP_TX_QUEUE;
  idx += sizeof(uint8_t);
  commandPayload[idx] = sl_cli_get_argument_uint8(arguments, 0); // AppId = 0
  idx += sizeof(uint8_t);
  uint32_t srCId = sl_cli_get_argument_uint32(arguments, 1);
  commandPayload[idx++] = srCId >> 0;
  commandPayload[idx++] = srCId >> 8;
  commandPayload[idx++] = srCId >> 16;
  commandPayload[idx++] = srCId >> 24;
  sendGreenPowerCommand(commandPayload[GREEN_POWER_COMMAND_INDEX],
                        commandPayload,
                        idx,
                        true);
}

void gpAppGpSetTxQueueSize(sl_cli_command_arg_t *arguments)
{
  uint8_t idx = 0;
  uint8_t commandPayload[GREEN_POWER_MAX_FRAME_LENGTH];
  commandPayload[idx] = EMBER_CUSTOM_EZSP_COMMAND_SET_APP_GP_TX_QUEUE_MAX_SIZE;
  idx += sizeof(uint8_t);
  uint16_t maxTxGpQueueSize = sl_cli_get_argument_uint16(arguments, 0);
  commandPayload[idx++] = maxTxGpQueueSize >> 0;
  commandPayload[idx++] = maxTxGpQueueSize >> 8;
  sendGreenPowerCommand(commandPayload[GREEN_POWER_COMMAND_INDEX],
                        commandPayload,
                        idx,
                        false);
}
void gpAppGpGetTxQueueSize(sl_cli_command_arg_t *arguments)
{
  uint8_t idx = 0;
  uint8_t commandPayload[GREEN_POWER_MAX_FRAME_LENGTH];
  commandPayload[idx] = EMBER_CUSTOM_EZSP_COMMAND_GET_APP_GP_TX_QUEUE_MAX_SIZE;
  idx += sizeof(uint8_t);
  sendGreenPowerCommand(commandPayload[GREEN_POWER_COMMAND_INDEX],
                        commandPayload,
                        idx,
                        true);
}
void gpAppGpGetTxQueueCount(sl_cli_command_arg_t *arguments)
{
  uint8_t idx = 0;
  uint8_t commandPayload[GREEN_POWER_MAX_FRAME_LENGTH];
  commandPayload[idx] = EMBER_CUSTOM_EZSP_COMMAND_GET_APP_GP_TX_QUEUE_COUNT;
  idx += sizeof(uint8_t);
  sendGreenPowerCommand(commandPayload[GREEN_POWER_COMMAND_INDEX],
                        commandPayload,
                        idx,
                        true);
}

void gpAppGpSendGpRaw(sl_cli_command_arg_t *arguments)
{
  uint8_t idx = 0;
  uint8_t commandPayload[GREEN_POWER_MAX_FRAME_LENGTH];
  size_t dataLength = 0;
  uint8_t *data;

  commandPayload[idx] = EMBER_CUSTOM_EZSP_COMMAND_SEND_APP_GP_RAW;
  idx += sizeof(uint8_t);
  commandPayload[idx] = sl_cli_get_argument_uint8(arguments, 0); // Channel
  idx += sizeof(uint8_t);
  commandPayload[idx] = sl_cli_get_argument_uint8(arguments, 1); // delay
  idx += sizeof(uint8_t);

  data = sl_cli_get_argument_hex(arguments, 3, &dataLength);
  (void) memcpy(&commandPayload[idx], data, dataLength);
  idx += dataLength;

  sendGreenPowerCommand(commandPayload[GREEN_POWER_COMMAND_INDEX],
                        commandPayload,
                        idx,
                        false);
}
