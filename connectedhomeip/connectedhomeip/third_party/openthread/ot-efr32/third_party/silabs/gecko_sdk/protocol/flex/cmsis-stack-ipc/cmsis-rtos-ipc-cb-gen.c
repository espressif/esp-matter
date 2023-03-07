/***************************************************************************//**
 * @brief
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

#include PLATFORM_HEADER
#include "cmsis-rtos-ipc-config.h"

#include "stack/include/ember.h"
#include "cmsis-rtos-support.h"

#include "app_framework_callback.h"
#include "callback_dispatcher.h"
#include "cmsis-rtos-ipc-api-gen.h"

void emberStackStatusHandler(EmberStatus status)
{
  emAfPluginCmsisRtosSendCallbackCommand(EMBER_STACK_STATUS_HANDLER_IPC_COMMAND_ID,
                                         "u",
                                         status);
}
static void stackStatusCommandHandler(uint8_t *callbackParams)
{
  EmberStatus status;
  emAfPluginCmsisRtosFetchCallbackParams(callbackParams,
                                         "u",
                                         &status);

  emberAfStackStatusCallback(status);
  emberAfStackStatus(status);
}

void emberChildJoinHandler(EmberNodeType nodeType,
                           EmberNodeId nodeId)
{
  emAfPluginCmsisRtosSendCallbackCommand(EMBER_CHILD_JOIN_HANDLER_IPC_COMMAND_ID,
                                         "uv",
                                         nodeType,
                                         nodeId);
}
static void childJoinCommandHandler(uint8_t *callbackParams)
{
  EmberNodeType nodeType;
  EmberNodeId nodeId;
  emAfPluginCmsisRtosFetchCallbackParams(callbackParams,
                                         "uv",
                                         &nodeType,
                                         &nodeId);

  emberAfChildJoinCallback(nodeType,
                           nodeId);
  emberAfChildJoin(nodeType,
                   nodeId);
}

void emberRadioNeedsCalibratingHandler(void)
{
  emAfPluginCmsisRtosSendCallbackCommand(EMBER_RADIO_NEEDS_CALIBRATING_HANDLER_IPC_COMMAND_ID,
                                         "");
}
static void radioNeedsCalibratingCommandHandler(uint8_t *callbackParams)
{
  (void)callbackParams;

  emberAfRadioNeedsCalibratingCallback();
  emberAfRadioNeedsCalibrating();
}

void emberMessageSentHandler(EmberStatus status,
                             EmberOutgoingMessage *message)
{
  emAfPluginCmsisRtosSendCallbackCommand(EMBER_MESSAGE_SENT_HANDLER_IPC_COMMAND_ID,
                                         "uuvuuubuw",
                                         status,
                                         message->options,
                                         message->destination,
                                         message->endpoint,
                                         message->tag,
                                         message->length,
                                         message->payload,
                                         message->length,
                                         message->ackRssi,
                                         message->timestamp);
}
static void messageSentCommandHandler(uint8_t *callbackParams)
{
  EmberStatus status;
  EmberOutgoingMessage message;
  uint8_t payload[127];
  message.payload = payload;
  emAfPluginCmsisRtosFetchCallbackParams(callbackParams,
                                         "uuvuuubuw",
                                         &status,
                                         &message.options,
                                         &message.destination,
                                         &message.endpoint,
                                         &message.tag,
                                         &message.length,
                                         message.payload,
                                         &message.length,
                                         &message.ackRssi,
                                         &message.timestamp);

  emberAfMessageSentCallback(status,
                             &message);
  emberAfMessageSent(status,
                     &message);
}

void emberIncomingMessageHandler(EmberIncomingMessage *message)
{
  emAfPluginCmsisRtosSendCallbackCommand(EMBER_INCOMING_MESSAGE_HANDLER_IPC_COMMAND_ID,
                                         "uvuuubwu",
                                         message->options,
                                         message->source,
                                         message->endpoint,
                                         message->rssi,
                                         message->length,
                                         message->payload,
                                         message->length,
                                         message->timestamp,
                                         message->lqi);
}
static void incomingMessageCommandHandler(uint8_t *callbackParams)
{
  EmberIncomingMessage message;
  uint8_t payload[127];
  message.payload = payload;
  emAfPluginCmsisRtosFetchCallbackParams(callbackParams,
                                         "uvuuubwu",
                                         &message.options,
                                         &message.source,
                                         &message.endpoint,
                                         &message.rssi,
                                         &message.length,
                                         message.payload,
                                         &message.length,
                                         &message.timestamp,
                                         &message.lqi);

  emberAfIncomingMessageCallback(&message);
  emberAfIncomingMessage(&message);
}

void emberIncomingMacMessageHandler(EmberIncomingMacMessage *message)
{
  emAfPluginCmsisRtosSendCallbackCommand(EMBER_INCOMING_MAC_MESSAGE_HANDLER_IPC_COMMAND_ID,
                                         "uvbuvbuvvuuuuwubw",
                                         message->options,
                                         message->macFrame.srcAddress.addr.shortAddress,
                                         message->macFrame.srcAddress.addr.longAddress,
                                         EUI64_SIZE,
                                         message->macFrame.srcAddress.mode,
                                         message->macFrame.dstAddress.addr.shortAddress,
                                         message->macFrame.dstAddress.addr.longAddress,
                                         EUI64_SIZE,
                                         message->macFrame.dstAddress.mode,
                                         message->macFrame.srcPanId,
                                         message->macFrame.dstPanId,
                                         message->macFrame.srcPanIdSpecified,
                                         message->macFrame.dstPanIdSpecified,
                                         message->rssi,
                                         message->lqi,
                                         message->frameCounter,
                                         message->length,
                                         message->payload,
                                         message->length,
                                         message->timestamp);
}
static void incomingMacMessageCommandHandler(uint8_t *callbackParams)
{
  EmberIncomingMacMessage message;
  uint8_t eui64Size = EUI64_SIZE;
  uint8_t payload[127];
  message.payload = payload;
  emAfPluginCmsisRtosFetchCallbackParams(callbackParams,
                                         "uvbuvbuvvuuuuwubw",
                                         &message.options,
                                         &message.macFrame.srcAddress.addr.shortAddress,
                                         message.macFrame.srcAddress.addr.longAddress,
                                         &eui64Size,
                                         &message.macFrame.srcAddress.mode,
                                         &message.macFrame.dstAddress.addr.shortAddress,
                                         message.macFrame.dstAddress.addr.longAddress,
                                         &eui64Size,
                                         &message.macFrame.dstAddress.mode,
                                         &message.macFrame.srcPanId,
                                         &message.macFrame.dstPanId,
                                         &message.macFrame.srcPanIdSpecified,
                                         &message.macFrame.dstPanIdSpecified,
                                         &message.rssi,
                                         &message.lqi,
                                         &message.frameCounter,
                                         &message.length,
                                         message.payload,
                                         &message.length,
                                         &message.timestamp);

  emberAfIncomingMacMessageCallback(&message);
  emberAfIncomingMacMessage(&message);
}

void emberMacMessageSentHandler(EmberStatus status,
                                EmberOutgoingMacMessage *message)
{
  emAfPluginCmsisRtosSendCallbackCommand(EMBER_MAC_MESSAGE_SENT_HANDLER_IPC_COMMAND_ID,
                                         "uuvbuvbuvvuuuwubuw",
                                         status,
                                         message->options,
                                         message->macFrame.srcAddress.addr.shortAddress,
                                         message->macFrame.srcAddress.addr.longAddress,
                                         EUI64_SIZE,
                                         message->macFrame.srcAddress.mode,
                                         message->macFrame.dstAddress.addr.shortAddress,
                                         message->macFrame.dstAddress.addr.longAddress,
                                         EUI64_SIZE,
                                         message->macFrame.dstAddress.mode,
                                         message->macFrame.srcPanId,
                                         message->macFrame.dstPanId,
                                         message->macFrame.srcPanIdSpecified,
                                         message->macFrame.dstPanIdSpecified,
                                         message->tag,
                                         message->frameCounter,
                                         message->length,
                                         message->payload,
                                         message->length,
                                         message->ackRssi,
                                         message->timestamp);
}
static void macMessageSentCommandHandler(uint8_t *callbackParams)
{
  EmberStatus status;
  EmberOutgoingMacMessage message;
  uint8_t eui64Size = EUI64_SIZE;
  uint8_t payload[127];
  message.payload = payload;
  emAfPluginCmsisRtosFetchCallbackParams(callbackParams,
                                         "uuvbuvbuvvuuuwubuw",
                                         &status,
                                         &message.options,
                                         &message.macFrame.srcAddress.addr.shortAddress,
                                         message.macFrame.srcAddress.addr.longAddress,
                                         &eui64Size,
                                         &message.macFrame.srcAddress.mode,
                                         &message.macFrame.dstAddress.addr.shortAddress,
                                         message.macFrame.dstAddress.addr.longAddress,
                                         &eui64Size,
                                         &message.macFrame.dstAddress.mode,
                                         &message.macFrame.srcPanId,
                                         &message.macFrame.dstPanId,
                                         &message.macFrame.srcPanIdSpecified,
                                         &message.macFrame.dstPanIdSpecified,
                                         &message.tag,
                                         &message.frameCounter,
                                         &message.length,
                                         message.payload,
                                         &message.length,
                                         &message.ackRssi,
                                         &message.timestamp);

  emberAfMacMessageSentCallback(status,
                                &message);
  emberAfMacMessageSent(status,
                        &message);
}

void emberIncomingBeaconHandler(EmberPanId panId,
                                EmberMacAddress *source,
                                int8_t rssi,
                                bool permitJoining,
                                uint8_t beaconFieldsLength,
                                uint8_t *beaconFields,
                                uint8_t beaconPayloadLength,
                                uint8_t *beaconPayload)
{
  emAfPluginCmsisRtosSendCallbackCommand(EMBER_INCOMING_BEACON_HANDLER_IPC_COMMAND_ID,
                                         "vvbuuuubub",
                                         panId,
                                         source->addr.shortAddress,
                                         source->addr.longAddress,
                                         EUI64_SIZE,
                                         source->mode,
                                         rssi,
                                         permitJoining,
                                         beaconFieldsLength,
                                         beaconFields,
                                         beaconFieldsLength,
                                         beaconPayloadLength,
                                         beaconPayload,
                                         beaconPayloadLength);
}
static void incomingBeaconCommandHandler(uint8_t *callbackParams)
{
  EmberPanId panId;
  EmberMacAddress source;
  int8_t rssi;
  bool permitJoining;
  uint8_t beaconFieldsLength;
  uint8_t beaconPayloadLength;
  uint8_t eui64Size = EUI64_SIZE;
  static uint8_t beaconFields[EMBER_MAC_MAX_BEACON_FIELDS_LENGTH];
  static uint8_t beaconPayload[EMBER_MAC_MAX_APP_BEACON_PAYLOAD_LENGTH];
  emAfPluginCmsisRtosFetchCallbackParams(callbackParams,
                                         "vvbuuuubub",
                                         &panId,
                                         &source.addr.shortAddress,
                                         source.addr.longAddress,
                                         &eui64Size,
                                         &source.mode,
                                         &rssi,
                                         &permitJoining,
                                         &beaconFieldsLength,
                                         beaconFields,
                                         &beaconFieldsLength,
                                         &beaconPayloadLength,
                                         beaconPayload,
                                         &beaconPayloadLength);

  emberAfIncomingBeaconCallback(panId,
                                &source,
                                rssi,
                                permitJoining,
                                beaconFieldsLength,
                                beaconFields,
                                beaconPayloadLength,
                                beaconPayload);
  emberAfIncomingBeacon(panId,
                        &source,
                        rssi,
                        permitJoining,
                        beaconFieldsLength,
                        beaconFields,
                        beaconPayloadLength,
                        beaconPayload);
}

void emberActiveScanCompleteHandler(void)
{
  emAfPluginCmsisRtosSendCallbackCommand(EMBER_ACTIVE_SCAN_COMPLETE_HANDLER_IPC_COMMAND_ID,
                                         "");
}
static void activeScanCompleteCommandHandler(uint8_t *callbackParams)
{
  (void)callbackParams;

  emberAfActiveScanCompleteCallback();
  emberAfActiveScanComplete();
}

void emberEnergyScanCompleteHandler(int8_t mean,
                                    int8_t min,
                                    int8_t max,
                                    uint16_t variance)
{
  emAfPluginCmsisRtosSendCallbackCommand(EMBER_ENERGY_SCAN_COMPLETE_HANDLER_IPC_COMMAND_ID,
                                         "uuuv",
                                         mean,
                                         min,
                                         max,
                                         variance);
}
static void energyScanCompleteCommandHandler(uint8_t *callbackParams)
{
  int8_t mean;
  int8_t min;
  int8_t max;
  uint16_t variance;
  emAfPluginCmsisRtosFetchCallbackParams(callbackParams,
                                         "uuuv",
                                         &mean,
                                         &min,
                                         &max,
                                         &variance);

  emberAfEnergyScanCompleteCallback(mean,
                                    min,
                                    max,
                                    variance);
  emberAfEnergyScanComplete(mean,
                            min,
                            max,
                            variance);
}

void emberFrequencyHoppingStartClientCompleteHandler(EmberStatus status)
{
  emAfPluginCmsisRtosSendCallbackCommand(EMBER_FREQUENCY_HOPPING_START_CLIENT_COMPLETE_HANDLER_IPC_COMMAND_ID,
                                         "u",
                                         status);
}
static void frequencyHoppingStartClientCompleteCommandHandler(uint8_t *callbackParams)
{
  EmberStatus status;
  emAfPluginCmsisRtosFetchCallbackParams(callbackParams,
                                         "u",
                                         &status);

  emberAfFrequencyHoppingStartClientCompleteCallback(status);
  emberAfFrequencyHoppingStartClientComplete(status);
}

//------------------------------------------------------------------------------
// Callback command dispatcher (Application side)

void emAfPluginCmsisRtosHandleIncomingCallbackCommand(uint16_t commandId,
                                                      uint8_t *callbackParams)
{
  assert(!emAfPluginCmsisRtosIsCurrentTaskStackTask());

  switch (commandId) {
    case EMBER_STACK_STATUS_HANDLER_IPC_COMMAND_ID:
      stackStatusCommandHandler(callbackParams);
      break;
    case EMBER_CHILD_JOIN_HANDLER_IPC_COMMAND_ID:
      childJoinCommandHandler(callbackParams);
      break;
    case EMBER_RADIO_NEEDS_CALIBRATING_HANDLER_IPC_COMMAND_ID:
      radioNeedsCalibratingCommandHandler(callbackParams);
      break;
    case EMBER_MESSAGE_SENT_HANDLER_IPC_COMMAND_ID:
      messageSentCommandHandler(callbackParams);
      break;
    case EMBER_INCOMING_MESSAGE_HANDLER_IPC_COMMAND_ID:
      incomingMessageCommandHandler(callbackParams);
      break;
    case EMBER_INCOMING_MAC_MESSAGE_HANDLER_IPC_COMMAND_ID:
      incomingMacMessageCommandHandler(callbackParams);
      break;
    case EMBER_MAC_MESSAGE_SENT_HANDLER_IPC_COMMAND_ID:
      macMessageSentCommandHandler(callbackParams);
      break;
    case EMBER_INCOMING_BEACON_HANDLER_IPC_COMMAND_ID:
      incomingBeaconCommandHandler(callbackParams);
      break;
    case EMBER_ACTIVE_SCAN_COMPLETE_HANDLER_IPC_COMMAND_ID:
      activeScanCompleteCommandHandler(callbackParams);
      break;
    case EMBER_ENERGY_SCAN_COMPLETE_HANDLER_IPC_COMMAND_ID:
      energyScanCompleteCommandHandler(callbackParams);
      break;
    case EMBER_FREQUENCY_HOPPING_START_CLIENT_COMPLETE_HANDLER_IPC_COMMAND_ID:
      frequencyHoppingStartClientCompleteCommandHandler(callbackParams);
      break;
    default: {
      assert(0);
    }
  }
}
