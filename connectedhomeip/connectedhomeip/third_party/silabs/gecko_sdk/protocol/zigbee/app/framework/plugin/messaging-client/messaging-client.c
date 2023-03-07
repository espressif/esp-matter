/***************************************************************************//**
 * @file
 * @brief Routines for the Messaging Client plugin, which implements the client
 *        side of the Messaging cluster.
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

#include "../../include/af.h"
#include "../../util/common.h"
#include "messaging-client.h"

#ifdef UC_BUILD
#include "messaging-client-config.h"
#include "zap-cluster-command-parser.h"
#include "esi-management.h"
#else // !UC_BUILD
#include "app/framework/plugin/esi-management/esi-management.h"
#endif // UC_BUILD

static EmberAfPluginMessagingClientMessage messageTable[EMBER_AF_MESSAGING_CLUSTER_CLIENT_ENDPOINT_COUNT];

#define MESSAGE_CONTROL_INTER_PAN_TRANSMISSION_ONLY (0x2)
/**
 * The callback function passed to the ESI management plugin. It handles
 * ESI entry deletions.
 */
static void esiDeletionCallback(uint8_t esiIndex)
{
  uint8_t i;
  for (i = 0; i < EMBER_AF_MESSAGING_CLUSTER_CLIENT_ENDPOINT_COUNT; i++) {
    messageTable[i].esiBitmask &= ~BIT(esiIndex);
  }
}

void emberAfMessagingClusterClientInitCallback(uint8_t endpoint)
{
  emAfPluginMessagingClientClearMessage(endpoint);
  // Subscribing for ESI Management plugin deletion announcements.
  emberAfPluginEsiManagementSubscribeToDeletionAnnouncements(esiDeletionCallback);
}

void emAfPluginMessagingClientClearMessage(uint8_t endpoint)
{
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_MESSAGING_CLUSTER_ID);
  if ( ep != 0xFF ) {
    messageTable[ep].valid             = false;
    messageTable[ep].active            = false;
    messageTable[ep].messageId         = 0x00000000UL;
    messageTable[ep].messageControl    = 0x00;
    messageTable[ep].startTime         = 0x00000000UL;
    messageTable[ep].endTime           = 0x00000000UL;
    messageTable[ep].durationInMinutes = 0x0000;
    messageTable[ep].esiBitmask        = 0;
    messageTable[ep].message[0]        = 0;
  }
}

void emberAfMessagingClusterClientTickCallback(uint8_t endpoint)
{
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_MESSAGING_CLUSTER_ID);

  if (ep == 0xFF) {
    return;
  }

  // The tick is used to deactivate messages that are active or to activate
  // messages that are inactive.  So, first, just flip the active state and
  // notify the application of the change.
  messageTable[ep].active = !messageTable[ep].active;
  if (messageTable[ep].active) {
    emberAfPluginMessagingClientDisplayMessageCallback(&messageTable[ep]);
  } else {
    emberAfPluginMessagingClientCancelMessageCallback(&messageTable[ep]);
  }

  // If the new state is active and the duration is not "until changed," then we
  // have to schedule a tick to deactivate the message after the duration has
  // elapsed.  Otherwise, cancel the tick because no further processing is
  // required as either the message is no longer active or it will live until
  // changed.
  if (messageTable[ep].active
      && (messageTable[ep].durationInMinutes
          != ZCL_MESSAGING_CLUSTER_DURATION_UNTIL_CHANGED)) {
    slxu_zigbee_zcl_schedule_client_tick(endpoint,
                                         ZCL_MESSAGING_CLUSTER_ID,
                                         ((messageTable[ep].endTime
                                           - messageTable[ep].startTime)
                                          * MILLISECOND_TICKS_PER_SECOND));
  }
}

//-----------------------
// ZCL commands callbacks

#ifdef UC_BUILD

bool emberAfMessagingClusterDisplayMessageCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_messaging_cluster_display_message_command_t cmd_data;
  uint8_t endpoint = emberAfCurrentEndpoint();
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_MESSAGING_CLUSTER_ID);
  uint32_t now = emberAfGetCurrentTime();
  uint8_t esiIndex = emberAfPluginEsiManagementUpdateEsiAndGetIndex(cmd);

  if (zcl_decode_messaging_cluster_display_message_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  emberAfMessagingClusterPrint("RX: DisplayMessage"
                               " 0x%4x, 0x%x, 0x%4x, 0x%2x, \"",
                               cmd_data.messageId,
                               cmd_data.messageControl,
                               cmd_data.startTime,
                               cmd_data.durationInMinutes);
  emberAfMessagingClusterPrintString(cmd_data.message);
  emberAfMessagingClusterPrintln("\"");

  if (emberAfPluginMessagingClientPreDisplayMessageCallback(cmd_data.messageId,
                                                            cmd_data.messageControl,
                                                            cmd_data.startTime,
                                                            cmd_data.durationInMinutes,
                                                            cmd_data.message,
                                                            cmd_data.optionalExtendedMessageControl)) {
    goto kickout;
  }

  if (ep == 0xFF) {
    return false;
  }

  // Use of Inter-PAN transmission is deprecated.
  if ((cmd_data.messageControl & 0x03) == MESSAGE_CONTROL_INTER_PAN_TRANSMISSION_ONLY) {
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_INVALID_FIELD);
    return true;
  }

  if (messageTable[ep].active) {
    // First check whether it is a duplicate message
    if (cmd_data.messageId == messageTable[ep].messageId) {
      //Duplicate message from a different ESI, add the ESI to the bitmask
      if (esiIndex < EMBER_AF_PLUGIN_ESI_MANAGEMENT_ESI_TABLE_SIZE
          && (messageTable[ep].esiBitmask & BIT(esiIndex)) == 0) {
        messageTable[ep].esiBitmask |= BIT(esiIndex);
      }
      // Either way, we send back a default response.
      emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
      return true;
    } else {
      // If we already have another message, notify the application that it
      // should no longer display it.
      messageTable[ep].active = false;
      emberAfPluginMessagingClientCancelMessageCallback(&messageTable[ep]);
      slxu_zigbee_zcl_deactivate_client_tick(emberAfCurrentEndpoint(),
                                             ZCL_MESSAGING_CLUSTER_ID);
    }
  }

  if (esiIndex < EMBER_AF_PLUGIN_ESI_MANAGEMENT_ESI_TABLE_SIZE) {
    messageTable[ep].esiBitmask = BIT(esiIndex);
  }

  messageTable[ep].clientEndpoint =
    emberAfCurrentCommand()->apsFrame->destinationEndpoint;
  messageTable[ep].messageId         = cmd_data.messageId;
  messageTable[ep].messageControl    = cmd_data.messageControl;
  messageTable[ep].startTime         = (cmd_data.startTime == ZCL_MESSAGING_CLUSTER_START_TIME_NOW
                                        ? now
                                        : cmd_data.startTime);
  messageTable[ep].endTime           = (cmd_data.durationInMinutes == ZCL_MESSAGING_CLUSTER_DURATION_UNTIL_CHANGED
                                        ? ZCL_MESSAGING_CLUSTER_END_TIME_NEVER
                                        : messageTable[ep].startTime + cmd_data.durationInMinutes * 60);
  messageTable[ep].durationInMinutes = cmd_data.durationInMinutes;
  emberAfCopyString(messageTable[ep].message, cmd_data.message, EMBER_AF_PLUGIN_MESSAGING_CLIENT_MESSAGE_SIZE);

  if (messageTable[ep].startTime <= now) {
    if (messageTable[ep].durationInMinutes
        == ZCL_MESSAGING_CLUSTER_DURATION_UNTIL_CHANGED) {
      // If a message starts in the past and the duration is "until changed," it
      // gets marked as active and no tick is scheduled becaues the message will
      // live until we receive a new message or a cancel command.
      messageTable[ep].active = true;
      emberAfPluginMessagingClientDisplayMessageCallback(&messageTable[ep]);
    } else if (messageTable[ep].endTime > now) {
      // If a message starts in the past and the duration has not elapsed, it
      // gets marked as active and a tick is scheduled for the time remaining in
      // the duration to wake us up and mark the message as inactive.
      messageTable[ep].active = true;
      emberAfPluginMessagingClientDisplayMessageCallback(&messageTable[ep]);
      slxu_zigbee_zcl_schedule_client_tick(emberAfCurrentEndpoint(),
                                           ZCL_MESSAGING_CLUSTER_ID,
                                           ((messageTable[ep].endTime - now)
                                            * MILLISECOND_TICKS_PER_SECOND));
    } else {
      // Finally, if a message starts in the past and the duration has elapsed,
      // it gets marked as inactive and no tick is scheduled because no further
      // processing is needed.
      messageTable[ep].active = false;
    }
  } else {
    // If a message starts in the future, it gets marked as inactive and a tick
    // is scheduled to wake us up at startTime to mark the message as active.
    messageTable[ep].active = false;
    slxu_zigbee_zcl_schedule_client_tick(emberAfCurrentEndpoint(),
                                         ZCL_MESSAGING_CLUSTER_ID,
                                         ((messageTable[ep].startTime - now)
                                          * MILLISECOND_TICKS_PER_SECOND));
  }

  // Once we receive a message, the data in the structure becomes valid and
  // remains valid forever.  When the message expires or is canceled, it gets
  // marked as inactive, but stays valid.  This allows the application to
  // display or confirm old messages.
  messageTable[ep].valid = true;

  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  kickout:
  return true;
}

bool emberAfMessagingClusterCancelMessageCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_messaging_cluster_cancel_message_command_t cmd_data;
  uint8_t endpoint = emberAfCurrentEndpoint();
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_MESSAGING_CLUSTER_ID);
  EmberAfStatus status;

  if (zcl_decode_messaging_cluster_cancel_message_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS
      || ep == 0xFF) {
    return false;
  }

  emberAfMessagingClusterPrintln("RX: CancelMessage 0x%4x, 0x%x",
                                 cmd_data.messageId,
                                 cmd_data.messageControl);

  if (messageTable[ep].active && messageTable[ep].messageId == cmd_data.messageId) {
    messageTable[ep].active = false;
    messageTable[ep].messageControl = cmd_data.messageControl;
    emberAfPluginMessagingClientCancelMessageCallback(&messageTable[ep]);
    slxu_zigbee_zcl_deactivate_client_tick(emberAfCurrentEndpoint(),
                                           ZCL_MESSAGING_CLUSTER_ID);
    status = EMBER_ZCL_STATUS_SUCCESS;
  } else {
    status = EMBER_ZCL_STATUS_NOT_FOUND;
  }

  emberAfSendImmediateDefaultResponse(status);
  return true;
}

#else // !UC_BUILD

bool emberAfMessagingClusterDisplayMessageCallback(uint32_t messageId,
                                                   uint8_t messageControl,
                                                   uint32_t startTime,
                                                   uint16_t durationInMinutes,
                                                   uint8_t* msg,
                                                   uint8_t optionalExtendedMessageControl)
{
  uint8_t endpoint = emberAfCurrentEndpoint();
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_MESSAGING_CLUSTER_ID);
  uint32_t now = emberAfGetCurrentTime();
  uint8_t esiIndex =
    emberAfPluginEsiManagementUpdateEsiAndGetIndex(emberAfCurrentCommand());

  emberAfMessagingClusterPrint("RX: DisplayMessage"
                               " 0x%4x, 0x%x, 0x%4x, 0x%2x, \"",
                               messageId,
                               messageControl,
                               startTime,
                               durationInMinutes);
  emberAfMessagingClusterPrintString(msg);
  emberAfMessagingClusterPrintln("\"");

  if (emberAfPluginMessagingClientPreDisplayMessageCallback(messageId,
                                                            messageControl,
                                                            startTime,
                                                            durationInMinutes,
                                                            msg,
                                                            optionalExtendedMessageControl)) {
    goto kickout;
  }

  if (ep == 0xFF) {
    return false;
  }

  // Use of Inter-PAN transmission is deprecated.
  if ((messageControl & 0x03) == MESSAGE_CONTROL_INTER_PAN_TRANSMISSION_ONLY) {
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_INVALID_FIELD);
    return true;
  }

  if (messageTable[ep].active) {
    // First check whether it is a duplicate message
    if (messageId == messageTable[ep].messageId) {
      //Duplicate message from a different ESI, add the ESI to the bitmask
      if (esiIndex < EMBER_AF_PLUGIN_ESI_MANAGEMENT_ESI_TABLE_SIZE
          && (messageTable[ep].esiBitmask & BIT(esiIndex)) == 0) {
        messageTable[ep].esiBitmask |= BIT(esiIndex);
      }
      // Either way, we send back a default response.
      emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
      return true;
    } else {
      // If we already have another message, notify the application that it
      // should no longer display it.
      messageTable[ep].active = false;
      emberAfPluginMessagingClientCancelMessageCallback(&messageTable[ep]);
      slxu_zigbee_zcl_deactivate_client_tick(emberAfCurrentEndpoint(),
                                             ZCL_MESSAGING_CLUSTER_ID);
    }
  }

  if (esiIndex < EMBER_AF_PLUGIN_ESI_MANAGEMENT_ESI_TABLE_SIZE) {
    messageTable[ep].esiBitmask = BIT(esiIndex);
  }

  messageTable[ep].clientEndpoint =
    emberAfCurrentCommand()->apsFrame->destinationEndpoint;
  messageTable[ep].messageId         = messageId;
  messageTable[ep].messageControl    = messageControl;
  messageTable[ep].startTime         = (startTime == ZCL_MESSAGING_CLUSTER_START_TIME_NOW
                                        ? now
                                        : startTime);
  messageTable[ep].endTime           = (durationInMinutes == ZCL_MESSAGING_CLUSTER_DURATION_UNTIL_CHANGED
                                        ? ZCL_MESSAGING_CLUSTER_END_TIME_NEVER
                                        : messageTable[ep].startTime + durationInMinutes * 60);
  messageTable[ep].durationInMinutes = durationInMinutes;
  emberAfCopyString(messageTable[ep].message, msg, EMBER_AF_PLUGIN_MESSAGING_CLIENT_MESSAGE_SIZE);

  if (messageTable[ep].startTime <= now) {
    if (messageTable[ep].durationInMinutes
        == ZCL_MESSAGING_CLUSTER_DURATION_UNTIL_CHANGED) {
      // If a message starts in the past and the duration is "until changed," it
      // gets marked as active and no tick is scheduled becaues the message will
      // live until we receive a new message or a cancel command.
      messageTable[ep].active = true;
      emberAfPluginMessagingClientDisplayMessageCallback(&messageTable[ep]);
    } else if (messageTable[ep].endTime > now) {
      // If a message starts in the past and the duration has not elapsed, it
      // gets marked as active and a tick is scheduled for the time remaining in
      // the duration to wake us up and mark the message as inactive.
      messageTable[ep].active = true;
      emberAfPluginMessagingClientDisplayMessageCallback(&messageTable[ep]);
      slxu_zigbee_zcl_schedule_client_tick(emberAfCurrentEndpoint(),
                                           ZCL_MESSAGING_CLUSTER_ID,
                                           ((messageTable[ep].endTime - now)
                                            * MILLISECOND_TICKS_PER_SECOND));
    } else {
      // Finally, if a message starts in the past and the duration has elapsed,
      // it gets marked as inactive and no tick is scheduled because no further
      // processing is needed.
      messageTable[ep].active = false;
    }
  } else {
    // If a message starts in the future, it gets marked as inactive and a tick
    // is scheduled to wake us up at startTime to mark the message as active.
    messageTable[ep].active = false;
    slxu_zigbee_zcl_schedule_client_tick(emberAfCurrentEndpoint(),
                                         ZCL_MESSAGING_CLUSTER_ID,
                                         ((messageTable[ep].startTime - now)
                                          * MILLISECOND_TICKS_PER_SECOND));
  }

  // Once we receive a message, the data in the structure becomes valid and
  // remains valid forever.  When the message expires or is canceled, it gets
  // marked as inactive, but stays valid.  This allows the application to
  // display or confirm old messages.
  messageTable[ep].valid = true;

  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  kickout:
  return true;
}

bool emberAfMessagingClusterCancelMessageCallback(uint32_t messageId,
                                                  uint8_t messageControl)
{
  uint8_t endpoint = emberAfCurrentEndpoint();
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_MESSAGING_CLUSTER_ID);
  EmberAfStatus status;

  if (ep == 0xFF) {
    return false;
  }

  emberAfMessagingClusterPrintln("RX: CancelMessage 0x%4x, 0x%x",
                                 messageId,
                                 messageControl);

  if (messageTable[ep].active && messageTable[ep].messageId == messageId) {
    messageTable[ep].active = false;
    messageTable[ep].messageControl = messageControl;
    emberAfPluginMessagingClientCancelMessageCallback(&messageTable[ep]);
    slxu_zigbee_zcl_deactivate_client_tick(emberAfCurrentEndpoint(),
                                           ZCL_MESSAGING_CLUSTER_ID);
    status = EMBER_ZCL_STATUS_SUCCESS;
  } else {
    status = EMBER_ZCL_STATUS_NOT_FOUND;
  }

  emberAfSendImmediateDefaultResponse(status);
  return true;
}

#endif // UC_BUILD

void emAfPluginMessagingClientPrintInfo(uint8_t endpoint)
{
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_MESSAGING_CLUSTER_ID);

  if (ep == 0xFF) {
    return;
  }

  emberAfMessagingClusterPrintln("= Client Message =");
  emberAfMessagingClusterFlush();

  emberAfMessagingClusterPrintln(" vld: %p", (messageTable[ep].valid ? "YES" : "NO"));
  emberAfMessagingClusterPrintln(" act: %p", (messageTable[ep].active ? "YES" : "NO"));
  emberAfMessagingClusterPrintln("  id: 0x%4x", messageTable[ep].messageId);
  emberAfMessagingClusterPrintln("  mc: 0x%x", messageTable[ep].messageControl);
  emberAfMessagingClusterPrintln("  st: 0x%4x", messageTable[ep].startTime);
  emberAfMessagingClusterPrintln("  et: 0x%4x", messageTable[ep].endTime);
  emberAfMessagingClusterPrintln("time: 0x%4x", emberAfGetCurrentTime());
  emberAfMessagingClusterPrintln(" dur: 0x%2x", messageTable[ep].durationInMinutes);
  emberAfMessagingClusterFlush();
  emberAfMessagingClusterPrint(" mes: \"");
  emberAfMessagingClusterPrintString(messageTable[ep].message);
  emberAfMessagingClusterPrintln("\"");
  emberAfMessagingClusterFlush();
}

EmberAfStatus emberAfPluginMessagingClientConfirmMessage(uint8_t endpoint)
{
  EmberAfStatus status = EMBER_ZCL_STATUS_NOT_FOUND;
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_MESSAGING_CLUSTER_ID);
  uint8_t i;

  if (ep == 0xFF) {
    return EMBER_ZCL_STATUS_FAILURE;
  }

  if (messageTable[ep].valid && messageTable[ep].active ) {
    for (i = 0; i < EMBER_AF_PLUGIN_ESI_MANAGEMENT_ESI_TABLE_SIZE; i++) {
      EmberAfPluginEsiManagementEsiEntry *esiEntry =
        emberAfPluginEsiManagementEsiLookUpByIndex(i);
      EmberNodeId nodeId;
      if ((messageTable[ep].esiBitmask & BIT(i)) == 0 || esiEntry == NULL) {
        continue;
      }

      nodeId = emberLookupNodeIdByEui64(esiEntry->eui64);
      emberAfFillCommandMessagingClusterMessageConfirmation(messageTable[ep].messageId,
                                                            emberAfGetCurrentTime(),
                                                            0x00,
                                                            "");
      // The source and destination are reversed for the confirmation.
      emberAfSetCommandEndpoints(messageTable[ep].clientEndpoint, esiEntry->endpoint);
      status = ((emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, nodeId)
                 == EMBER_SUCCESS)
                ? EMBER_ZCL_STATUS_SUCCESS
                : EMBER_ZCL_STATUS_FAILURE);
    }
  } else {
    emberAfMessagingClusterPrintln("Err: No Valid Msg");
  }
  return status;
}

#ifdef UC_BUILD

uint32_t emberAfMessagingClusterClientCommandParse(sl_service_opcode_t opcode,
                                                   sl_service_function_context_t *context)
{
  (void)opcode;

  EmberAfClusterCommand *cmd = (EmberAfClusterCommand *)context->data;
  bool wasHandled = false;

  if (!cmd->mfgSpecific) {
    switch (cmd->commandId) {
      case ZCL_DISPLAY_MESSAGE_COMMAND_ID:
      {
        wasHandled = emberAfMessagingClusterDisplayMessageCallback(cmd);
        break;
      }
      case ZCL_CANCEL_MESSAGE_COMMAND_ID:
      {
        wasHandled = emberAfMessagingClusterCancelMessageCallback(cmd);
        break;
      }
    }
  }

  return ((wasHandled)
          ? EMBER_ZCL_STATUS_SUCCESS
          : EMBER_ZCL_STATUS_UNSUP_COMMAND);
}

#endif // UC_BUILD
