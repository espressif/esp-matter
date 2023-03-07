/***************************************************************************//**
 * @file
 * @brief CLI for the Events plugin, based on the Events cluster.
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

#include "app/framework/include/af.h"
#include "app/framework/util/common.h"

#define LOG_PAYLOAD_CONTROL_MASK          0x0F
#define NUMBER_OF_EVENTS_MASK             0xF0
#define eventCrossesFrameBoundary(lpc)    ((lpc) & EventCrossesFrameBoundary)
#define numberOfEvents(lpc)               (((lpc) & NUMBER_OF_EVENTS_MASK) >> 4)

#ifdef UC_BUILD

#include "sl_component_catalog.h"
#include "zap-cluster-command-parser.h"

bool emberAfEventsClusterPublishEventCallback(EmberAfClusterCommand *cmd)
{
#ifdef SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT
  sl_zcl_events_cluster_publish_event_command_t cmd_data;

  if (zcl_decode_events_cluster_publish_event_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  emberAfEventsClusterPrint("RX: PublishEvent 0x%x, 0x%2x, 0x%4x, 0x%x, 0x%x",
                            cmd_data.logId,
                            cmd_data.eventId,
                            cmd_data.eventTime,
                            cmd_data.eventControl,
                            *cmd_data.eventData);

  uint8_t eventDataLen = emberAfStringLength(cmd_data.eventData);
  if (eventDataLen > 0) {
    emberAfEventsClusterPrint(", ");
    emberAfEventsClusterPrintString(cmd_data.eventData);
  }
  emberAfEventsClusterPrintln("");
#endif // SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT

  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

bool emberAfEventsClusterPublishEventLogCallback(EmberAfClusterCommand *cmd)
{
#ifdef SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT
  sl_zcl_events_cluster_publish_event_log_command_t cmd_data;

  if (zcl_decode_events_cluster_publish_event_log_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  emberAfEventsClusterPrint("RX: PublishEventLog 0x%2x, 0x%x, 0x%x, 0x%x",
                            cmd_data.totalNumberOfEvents,
                            cmd_data.commandIndex,
                            cmd_data.totalCommands,
                            cmd_data.logPayloadControl);

  uint16_t logPayloadLen = (cmd->bufLen
                            - (cmd->payloadStartIndex
                               + sizeof(cmd_data.totalNumberOfEvents)
                               + sizeof(cmd_data.commandIndex)
                               + sizeof(cmd_data.totalCommands)
                               + sizeof(cmd_data.logPayloadControl)));
  uint16_t logPayloadIndex = 0;
  uint8_t i;
  if (NULL != cmd_data.logPayload) {
    for (i = 0; i < numberOfEvents(cmd_data.logPayloadControl); i++) {
      uint8_t logId;
      uint16_t eventId;
      uint32_t eventTime;
      uint8_t *eventData;
      uint8_t eventDataLen;
      logId = emberAfGetInt8u(cmd_data.logPayload, logPayloadIndex, logPayloadLen);
      logPayloadIndex++;
      eventId = emberAfGetInt16u(cmd_data.logPayload, logPayloadIndex, logPayloadLen);
      logPayloadIndex += 2;
      eventTime = emberAfGetInt32u(cmd_data.logPayload, logPayloadIndex, logPayloadLen);
      logPayloadIndex += 4;
      eventData = cmd_data.logPayload + logPayloadIndex;
      eventDataLen = emberAfGetInt8u(cmd_data.logPayload, logPayloadIndex, logPayloadLen);
      logPayloadIndex += (1 + eventDataLen);
      emberAfEventsClusterPrint(" [");
      emberAfEventsClusterPrint("0x%x, 0x%2x, 0x%4x, 0x%x", logId, eventId, eventTime, eventDataLen);
      if (eventDataLen > 0) {
        emberAfEventsClusterPrint(", ");
        emberAfEventsClusterPrintString(eventData);
      }
      emberAfEventsClusterPrint("]");
    }
  }
  emberAfEventsClusterPrintln("");
#endif // SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT

  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

bool emberAfEventsClusterClearEventLogResponseCallback(EmberAfClusterCommand *cmd)
{
#ifdef SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT
  sl_zcl_events_cluster_clear_event_log_response_command_t cmd_data;

  if (zcl_decode_events_cluster_clear_event_log_response_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  emberAfEventsClusterPrintln("RX: ClearEventLogResponse 0x%x",
                              cmd_data.clearedEventsLogs);
#endif // SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT

  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

#else // UC_BUILD

bool emberAfEventsClusterPublishEventCallback(uint8_t logId,
                                              uint16_t eventId,
                                              uint32_t eventTime,
                                              uint8_t eventControl,
                                              uint8_t* eventData)
{
  emberAfEventsClusterPrint("RX: PublishEvent 0x%x, 0x%2x, 0x%4x, 0x%x, 0x%x",
                            logId,
                            eventId,
                            eventTime,
                            eventControl,
                            *eventData);

#if ((defined(EMBER_AF_PRINT_ENABLE) && defined(EMBER_AF_PRINT_EVENTS_CLUSTER)) || defined(UC_BUILD))
  uint8_t eventDataLen = emberAfStringLength(eventData);
  if (eventDataLen > 0) {
    emberAfEventsClusterPrint(", ");
    emberAfEventsClusterPrintString(eventData);
  }
  emberAfEventsClusterPrintln("");
#endif // ((defined(EMBER_AF_PRINT_ENABLE) && defined(EMBER_AF_PRINT_EVENTS_CLUSTER)) || defined(UC_BUILD))

  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

bool emberAfEventsClusterPublishEventLogCallback(uint16_t totalNumberOfEvents,
                                                 uint8_t commandIndex,
                                                 uint8_t totalCommands,
                                                 uint8_t logPayloadControl,
                                                 uint8_t* logPayload)
{
  emberAfEventsClusterPrint("RX: PublishEventLog 0x%2x, 0x%x, 0x%x, 0x%x",
                            totalNumberOfEvents,
                            commandIndex,
                            totalCommands,
                            logPayloadControl);

#if ((defined(EMBER_AF_PRINT_ENABLE) && defined(EMBER_AF_PRINT_EVENTS_CLUSTER)) || defined(UC_BUILD))
  uint16_t logPayloadLen = (emberAfCurrentCommand()->bufLen
                            - (emberAfCurrentCommand()->payloadStartIndex
                               + sizeof(totalNumberOfEvents)
                               + sizeof(commandIndex)
                               + sizeof(totalCommands)
                               + sizeof(logPayloadControl)));
  uint16_t logPayloadIndex = 0;
  uint8_t i;
  if (NULL != logPayload) {
    for (i = 0; i < numberOfEvents(logPayloadControl); i++) {
      uint8_t logId;
      uint16_t eventId;
      uint32_t eventTime;
      uint8_t *eventData;
      uint8_t eventDataLen;
      logId = emberAfGetInt8u(logPayload, logPayloadIndex, logPayloadLen);
      logPayloadIndex++;
      eventId = emberAfGetInt16u(logPayload, logPayloadIndex, logPayloadLen);
      logPayloadIndex += 2;
      eventTime = emberAfGetInt32u(logPayload, logPayloadIndex, logPayloadLen);
      logPayloadIndex += 4;
      eventData = logPayload + logPayloadIndex;
      eventDataLen = emberAfGetInt8u(logPayload, logPayloadIndex, logPayloadLen);
      logPayloadIndex += (1 + eventDataLen);
      emberAfEventsClusterPrint(" [");
      emberAfEventsClusterPrint("0x%x, 0x%2x, 0x%4x, 0x%x", logId, eventId, eventTime, eventDataLen);
      if (eventDataLen > 0) {
        emberAfEventsClusterPrint(", ");
        emberAfEventsClusterPrintString(eventData);
      }
      emberAfEventsClusterPrint("]");
    }
  }
  emberAfEventsClusterPrintln("");
#endif // ((defined(EMBER_AF_PRINT_ENABLE) && defined(EMBER_AF_PRINT_EVENTS_CLUSTER)) || defined(UC_BUILD))

  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

bool emberAfEventsClusterClearEventLogResponseCallback(uint8_t clearedEventsLogs)
{
  emberAfEventsClusterPrintln("RX: ClearEventLogResponse 0x%x",
                              clearedEventsLogs);

  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

#endif // UC_BUILD

#ifdef UC_BUILD

uint32_t emberAfEventsClusterClientCommandParse(sl_service_opcode_t opcode,
                                                sl_service_function_context_t *context)
{
  (void)opcode;

  EmberAfClusterCommand *cmd = (EmberAfClusterCommand *)context->data;
  bool wasHandled = false;

  if (!cmd->mfgSpecific) {
    switch (cmd->commandId) {
      case ZCL_PUBLISH_EVENT_COMMAND_ID:
      {
        wasHandled = emberAfEventsClusterPublishEventCallback(cmd);
        break;
      }
      case ZCL_PUBLISH_EVENT_LOG_COMMAND_ID:
      {
        wasHandled = emberAfEventsClusterPublishEventLogCallback(cmd);
        break;
      }
      case ZCL_CLEAR_EVENT_LOG_RESPONSE_COMMAND_ID:
      {
        wasHandled = emberAfEventsClusterClearEventLogResponseCallback(cmd);
        break;
      }
    }
  }

  return ((wasHandled)
          ? EMBER_ZCL_STATUS_SUCCESS
          : EMBER_ZCL_STATUS_UNSUP_COMMAND);
}

#endif // UC_BUILD
