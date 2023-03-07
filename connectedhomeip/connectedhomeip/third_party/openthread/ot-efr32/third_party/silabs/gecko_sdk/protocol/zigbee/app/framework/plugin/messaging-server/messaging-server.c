/***************************************************************************//**
 * @file
 * @brief Routines for the Messaging Server plugin, which implements the server
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
#include "messaging-server.h"

#ifdef UC_BUILD
#include "zap-cluster-command-parser.h"
#endif

// The internal message is stored in the same structure type that is defined
// publicly.  The internal state of the message is stored in the
// messageStatusControl field
static EmberAfPluginMessagingServerMessage msgTable[EMBER_AF_MESSAGING_CLUSTER_SERVER_ENDPOINT_COUNT];

// These bits are used by the messageStatusControl to indicate whether or not
// a message is valid, active, or if it is a "send now" message
#define VALID  BIT(0)
#define ACTIVE BIT(1)
#define NOW    BIT(2)

#define messageIsValid(ep)   (msgTable[ep].messageStatusControl & VALID)
#define messageIsActive(ep)  (msgTable[ep].messageStatusControl & ACTIVE)
#define messageIsNow(ep)     (msgTable[ep].messageStatusControl & NOW)
#define messageIsForever(ep) (msgTable[ep].durationInMinutes == ZCL_MESSAGING_CLUSTER_DURATION_UNTIL_CHANGED)

static bool messageIsCurrentOrScheduled(uint8_t endpoint)
{
  uint8_t ep = emberAfFindClusterServerEndpointIndex(endpoint, ZCL_MESSAGING_CLUSTER_ID);

  if (ep == 0xFF) {
    return false;
  }

  return (messageIsValid(ep)
          && messageIsActive(ep)
          && (messageIsForever(ep)
              || (emberAfGetCurrentTime()
                  < msgTable[ep].startTime + (uint32_t)msgTable[ep].durationInMinutes * 60)));
}

void emberAfMessagingClusterServerInitCallback(uint8_t endpoint)
{
  uint8_t ep = emberAfFindClusterServerEndpointIndex(endpoint, ZCL_MESSAGING_CLUSTER_ID);

  if (ep == 0xFF) {
    return;
  }

  msgTable[ep].messageStatusControl &= ~VALID;
}

//----------------------
// ZCL command callbacks

bool emberAfMessagingClusterGetLastMessageCallback(void)
{
  uint8_t endpoint = emberAfCurrentEndpoint();
  EmberAfPluginMessagingServerMessage message;
  emberAfMessagingClusterPrintln("RX: GetLastMessage");
  if (emberAfPluginMessagingServerGetMessage(endpoint, &message)) {
    emberAfFillCommandMessagingClusterDisplayMessage(message.messageId,
                                                     message.messageControl,
                                                     message.startTime,
                                                     message.durationInMinutes,
                                                     message.message,
                                                     message.extendedMessageControl);
    emberAfGetCommandApsFrame()->options |= EMBER_APS_OPTION_SOURCE_EUI64;
    emberAfSendResponse();
  } else {
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_NOT_FOUND);
  }
  return true;
}

#ifdef UC_BUILD

bool emberAfMessagingClusterMessageConfirmationCallback(EmberAfClusterCommand *cmd)
{
  (void)cmd;

#ifdef SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT
  sl_zcl_messaging_cluster_message_confirmation_command_t cmd_data;

  if (zcl_decode_messaging_cluster_message_confirmation_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  emberAfMessagingClusterPrintln("RX: MessageConfirmation 0x%4x, 0x%4x",
                                 cmd_data.messageId,
                                 cmd_data.confirmationTime);
#endif // SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT

  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

#else // !UC_BUILD

bool emberAfMessagingClusterMessageConfirmationCallback(uint32_t messageId,
                                                        uint32_t confirmationTime,
                                                        uint8_t messageConfirmationControl,
                                                        uint8_t *messageResponse)
{
  emberAfMessagingClusterPrintln("RX: MessageConfirmation 0x%4x, 0x%4x",
                                 messageId,
                                 confirmationTime);
  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

#endif // UC_BUILD

bool emberAfPluginMessagingServerGetMessage(uint8_t endpoint,
                                            EmberAfPluginMessagingServerMessage *message)
{
  uint8_t ep = emberAfFindClusterServerEndpointIndex(endpoint, ZCL_MESSAGING_CLUSTER_ID);

  if (ep == 0xFF) {
    return false;
  }

  MEMMOVE(message, &msgTable[ep], sizeof(EmberAfPluginMessagingServerMessage));

  // Clear out our internal bits from the message control.
  message->messageStatusControl &= ~ZCL_MESSAGING_CLUSTER_RESERVED_MASK;

  // If the message is expired or it has an absolute time, set the start time
  // and duration to the original start time and duration.  For "start now"
  // messages that are current or scheduled, set the start time to the special
  // value for "now" and set the duration to the remaining time, if it is not
  // already the special value for "until changed."
  if (messageIsCurrentOrScheduled(endpoint) && messageIsNow(ep)) {
    message->startTime = ZCL_MESSAGING_CLUSTER_START_TIME_NOW;
    if (!messageIsForever(ep)) {
      message->durationInMinutes -= ((emberAfGetCurrentTime() - msgTable[ep].startTime)
                                     / 60);
    }
  }
  return messageIsCurrentOrScheduled(endpoint);
}

void emberAfPluginMessagingServerSetMessage(uint8_t endpoint,
                                            const EmberAfPluginMessagingServerMessage *message)
{
  uint8_t ep = emberAfFindClusterServerEndpointIndex(endpoint, ZCL_MESSAGING_CLUSTER_ID);

  if (ep == 0xFF) {
    return;
  }

  if (message == NULL) {
    msgTable[ep].messageStatusControl &= ~ACTIVE;
    return;
  }

  MEMMOVE(&msgTable[ep], message, sizeof(EmberAfPluginMessagingServerMessage));

  // Rember if this is a "start now" message, but store the start time as the
  // current time so the duration can be adjusted.
  if (msgTable[ep].startTime == ZCL_MESSAGING_CLUSTER_START_TIME_NOW) {
    msgTable[ep].messageStatusControl |= NOW;
    msgTable[ep].startTime = emberAfGetCurrentTime();
  } else {
    msgTable[ep].messageStatusControl &= ~NOW;
  }

  msgTable[ep].messageStatusControl |= (VALID | ACTIVE);
}

void emAfPluginMessagingServerPrintInfo(uint8_t endpoint)
{
  uint8_t ep = emberAfFindClusterServerEndpointIndex(endpoint, ZCL_MESSAGING_CLUSTER_ID);

  if (ep == 0xFF) {
    return;
  }

  emberAfMessagingClusterPrintln("= Server Message =");
  emberAfMessagingClusterFlush();

  emberAfMessagingClusterPrintln(" vld: %s", (messageIsValid(ep) ? "YES" : "NO"));
  emberAfMessagingClusterPrintln(" act: %s", (messageIsCurrentOrScheduled(endpoint)
                                              ? "YES"
                                              : "NO"));
  emberAfMessagingClusterPrintln("  id: 0x%4x", msgTable[ep].messageId);
  emberAfMessagingClusterPrintln("  mc: 0x%x",
                                 (msgTable[ep].messageControl
                                  & ~ZCL_MESSAGING_CLUSTER_RESERVED_MASK));
  emberAfMessagingClusterPrintln("  st: 0x%4x", msgTable[ep].startTime);
  emberAfMessagingClusterPrintln(" now: %s", (messageIsNow(ep) ? "YES" : "NO"));
  emberAfMessagingClusterPrintln("time: 0x%4x", emberAfGetCurrentTime());
  emberAfMessagingClusterPrintln(" dur: 0x%2x", msgTable[ep].durationInMinutes);
  emberAfMessagingClusterFlush();
  emberAfMessagingClusterPrint(" mes: \"");
  emberAfMessagingClusterPrintString(msgTable[ep].message);
  emberAfMessagingClusterPrintln("\"");
  emberAfMessagingClusterFlush();
}

void emberAfPluginMessagingServerDisplayMessage(EmberNodeId nodeId,
                                                uint8_t srcEndpoint,
                                                uint8_t dstEndpoint)
{
  EmberStatus status;
  EmberAfPluginMessagingServerMessage message;
  if (!emberAfPluginMessagingServerGetMessage(srcEndpoint, &message)) {
    emberAfMessagingClusterPrintln("invalid msg");
    return;
  }

  emberAfFillCommandMessagingClusterDisplayMessage(message.messageId,
                                                   message.messageControl,
                                                   message.startTime,
                                                   message.durationInMinutes,
                                                   message.message,
                                                   message.extendedMessageControl);
  emberAfSetCommandEndpoints(srcEndpoint, dstEndpoint);
  emberAfGetCommandApsFrame()->options |= EMBER_APS_OPTION_SOURCE_EUI64;
  status = emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, nodeId);
  if (status != EMBER_SUCCESS) {
    emberAfMessagingClusterPrintln("Error in display %x", status);
  }
}

void emberAfPluginMessagingServerCancelMessage(EmberNodeId nodeId,
                                               uint8_t srcEndpoint,
                                               uint8_t dstEndpoint)
{
  EmberStatus status;
  EmberAfPluginMessagingServerMessage message;

  // Nullify the current message before sending the cancellation.
  emberAfPluginMessagingServerSetMessage(srcEndpoint, NULL);

  // Then send the response
  emberAfPluginMessagingServerGetMessage(srcEndpoint, &message);

  emberAfFillCommandMessagingClusterCancelMessage(message.messageId,
                                                  message.messageControl);
  emberAfSetCommandEndpoints(srcEndpoint, dstEndpoint);
  emberAfGetCommandApsFrame()->options |= EMBER_APS_OPTION_SOURCE_EUI64;
  status = emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, nodeId);
  if (status != EMBER_SUCCESS) {
    emberAfMessagingClusterPrintln("Error in cancel %x", status);
  }
}

#ifdef UC_BUILD

uint32_t emberAfMessagingClusterServerCommandParse(sl_service_opcode_t opcode,
                                                   sl_service_function_context_t *context)
{
  (void)opcode;

  EmberAfClusterCommand *cmd = (EmberAfClusterCommand *)context->data;
  bool wasHandled = false;

  if (!cmd->mfgSpecific) {
    switch (cmd->commandId) {
      case ZCL_GET_LAST_MESSAGE_COMMAND_ID:
      {
        wasHandled = emberAfMessagingClusterGetLastMessageCallback();
        break;
      }
      case ZCL_MESSAGE_CONFIRMATION_COMMAND_ID:
      {
        wasHandled = emberAfMessagingClusterMessageConfirmationCallback(cmd);
        break;
      }
      default:
        break;
    }
  }

  return ((wasHandled)
          ? EMBER_ZCL_STATUS_SUCCESS
          : EMBER_ZCL_STATUS_UNSUP_COMMAND);
}

#endif // UC_BUILD
