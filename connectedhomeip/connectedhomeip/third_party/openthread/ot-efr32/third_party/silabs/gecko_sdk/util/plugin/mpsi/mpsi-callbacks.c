/**************************************************************************//**
 * Copyright 2017 Silicon Laboratories, Inc.
 *
 *****************************************************************************/

#include "mpsi.h"
#include "mpsi-callbacks.h"
#include "mpsi-configuration.h"
#include <stdlib.h>

#ifdef EMBER_AF_API_AF_SECURITY_HEADER
 #include EMBER_AF_API_AF_SECURITY_HEADER
#endif // EMBER_AF_API_AF_SECURITY_HEADER

#ifdef EMBER_AF_PLUGIN_SLOT_MANAGER
 #include "slot-manager.h"
#endif // EMBER_AF_PLUGIN_SLOT_MANAGER

#ifdef EMBER_AF_API_NETWORK_STEERING
 #include EMBER_AF_API_NETWORK_STEERING
#endif  // EMBER_AF_API_NETWORK_STEERING

#ifdef EMBER_AF_API_NETWORK_CREATOR_SECURITY
 #include EMBER_AF_API_NETWORK_CREATOR_SECURITY
#endif // EMBER_AF_API_NETWORK_CREATOR_SECURITY

// Sync this to bootloader-interface.c
#ifndef CUSTOMER_APPLICATION_VERSION
 #define CUSTOMER_APPLICATION_VERSION 0
#endif // CUSTOMER_APPLICATION_VERSION

#define WILDCARD_EUI64  { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }
#define MPSI_PERMIT_JOIN_TIME_SEC   254

#define MPSI_GBL_METADATA_BUFFER_LEN        32
#define MPSI_GBL_METADATA_MAX_MESSAGE_KEY   "*MPSI Max Message ID:"
#define MPSI_GBL_METADATA_END_KEY_CHARACTER '*'

char     gGblMetadataBuffer[MPSI_GBL_METADATA_BUFFER_LEN];
uint8_t  gGblMetadataBufferIndex;
uint16_t gMaxMessageIdSupported;

// Forward declarations
#if defined (EMBER_AF_PLUGIN_SLOT_MANAGER)
void      gblMetadataCallbackFunc(uint32_t address,
                                  uint8_t *data,
                                  size_t length,
                                  void *context);
uint16_t  getMaxMessageIdSupportedBySlot(uint32_t slotId, uint32_t capabilities);
uint8_t   bootloaderAppTypeToMpsiAppType(uint32_t type);
#endif // EMBER_AF_PLUGIN_SLOT_MANAGER

uint8_t mpsiHandleMessageGetAppsInfo(MpsiMessage_t* mpsiMessage)
{
  MpsiMessage_t response;
  MpsiAppsInfoMessage_t appsInfoMessage;
  uint8_t bytesSerialized;

  if (!mpsiMessage) {
    return MPSI_INVALID_PARAMETER;
  }

  response.destinationAppId = MPSI_APP_ID_MOBILE_APP;
  response.messageId = MPSI_MESSAGE_ID_APPS_INFO;
  response.payloadLength = 0;

  // First put in ourself in the response
  appsInfoMessage.slotId = INVALID_SLOT;
  appsInfoMessage.applicationId = MPSI_APP_ID;
  appsInfoMessage.applicationVersion = CUSTOMER_APPLICATION_VERSION;
  appsInfoMessage.maxMessageIdSupported = MPSI_MESSAGE_ID_MAX_ID;

  bytesSerialized = emAfPluginMpsiSerializeSpecificMessage(&appsInfoMessage,
                                                           response.messageId,
                                                           response.payload);
  if (0 == bytesSerialized) {
    mpsiPrintln("MPSI (0x%x) error: serialize error with len %d",
                response.messageId, sizeof(appsInfoMessage));
    return MPSI_ERROR;
  }
  response.payloadLength += bytesSerialized;

#if defined (EMBER_AF_PLUGIN_SLOT_MANAGER)
  // Now fill out the rest based on what's in the slots
  uint32_t slot = 0;
  SlotManagerSlotInfo_t slotInfo;
  while (SLOT_MANAGER_SUCCESS
         == emberAfPluginSlotManagerGetSlotInfo(slot, &slotInfo)) {
    appsInfoMessage.slotId = slot;
    appsInfoMessage.applicationId =
      bootloaderAppTypeToMpsiAppType(slotInfo.slotAppInfo.type);
    appsInfoMessage.applicationVersion = slotInfo.slotAppInfo.version;
    appsInfoMessage.maxMessageIdSupported =
      getMaxMessageIdSupportedBySlot(slot, slotInfo.slotAppInfo.capabilities);
    bytesSerialized = emAfPluginMpsiSerializeSpecificMessage(
      &appsInfoMessage,
      response.messageId,
      response.payload + response.payloadLength);

    if (0 == bytesSerialized) {
      mpsiPrintln("MPSI (0x%x) error: serialize error with len %d",
                  response.messageId, sizeof(appsInfoMessage));
      return MPSI_ERROR;
    }
    response.payloadLength += bytesSerialized;

    slot++;
  }
#endif // EMBER_AF_PLUGIN_SLOT_MANAGER
  return emberAfPluginMpsiSendMessage(&response);
}

uint8_t mpsiHandleMessageAppsInfo(MpsiMessage_t* mpsiMessage)
{
  if (!mpsiMessage) {
    return MPSI_INVALID_PARAMETER;
  }

  // Nothing needs to be done here for the stacks currently
  // Receiving this message is of value to the Mobile App

  return MPSI_SUCCESS;
}

uint8_t mpsiHandleMessageBootloadSlot(MpsiMessage_t* mpsiMessage)
{
#if defined (EMBER_AF_PLUGIN_SLOT_MANAGER)
  MpsiBootloadSlotMessage_t message;
  uint8_t bytesDeserialized;
  uint8_t status;

  if (!mpsiMessage) {
    return MPSI_INVALID_PARAMETER;
  }

  if (mpsiMessage->payloadLength != sizeof(message)) {
    return MPSI_INVALID_PARAMETER;
  }

  bytesDeserialized = emAfPluginMpsiDeserializeSpecificMessage(
    mpsiMessage->payload,
    mpsiMessage->messageId,
    &message);

  if (mpsiMessage->payloadLength != bytesDeserialized) {
    mpsiPrintln("MPSI (0x%x) error: deserialize error with len %d (%d)",
                mpsiMessage->messageId, bytesDeserialized,
                mpsiMessage->payloadLength);
    return MPSI_INVALID_PARAMETER;
  }

  // This won't return if it succeeds
  status = emberAfPluginSlotManagerVerifyAndBootloadSlot(message.slotId);

  return (SLOT_MANAGER_SUCCESS == status) ? MPSI_SUCCESS : MPSI_ERROR;
#else
  return MPSI_UNSUPPORTED_COMMAND;
#endif // EMBER_AF_PLUGIN_SLOT_MANAGER
}

uint8_t mpsiHandleMessageError(MpsiMessage_t* mpsiMessage)
{
  if (!mpsiMessage) {
    return MPSI_INVALID_PARAMETER;
  }

  // Nothing needs to be done here for the stacks currently
  // Receiving this message is of value to the Mobile App

  return MPSI_SUCCESS;
}

uint8_t mpsiHandleMessageInitiateJoining(MpsiMessage_t* mpsiMessage)
{
  MpsiInitiateJoiningMessage_t message;
  uint8_t bytesDeserialized;

  if (!mpsiMessage) {
    return MPSI_INVALID_PARAMETER;
  }

  if (mpsiMessage->payloadLength != sizeof(message)) {
    return MPSI_INVALID_PARAMETER;
  }

  bytesDeserialized = emAfPluginMpsiDeserializeSpecificMessage(
    mpsiMessage->payload,
    mpsiMessage->messageId,
    &message);

  if (mpsiMessage->payloadLength != bytesDeserialized) {
    mpsiPrintln("MPSI (0x%x) error: deserialize error with len %d (%d)",
                mpsiMessage->messageId, bytesDeserialized,
                mpsiMessage->payloadLength);
    return MPSI_INVALID_PARAMETER;
  }

#if defined (EMBER_STACK_ZIGBEE)
  EmberNodeType nodeType;
  EmberStatus status = emberAfGetNodeType(&nodeType);
  if (EMBER_SUCCESS == status) {
    emberAfPermitJoin(MPSI_PERMIT_JOIN_TIME_SEC, false); // no broadcast
  } else if (EMBER_NOT_JOINED == status) {
#ifdef EMBER_AF_API_NETWORK_STEERING
    emberAfPluginNetworkSteeringStart();
#else
    return MPSI_INVALID_FUNCTION;
#endif // EMBER_AF_API_NETWORK_STEERING
  }
#else
  return MPSI_UNSUPPORTED_COMMAND;
#endif // EMBER_STACK_ZIGBEE

  return MPSI_SUCCESS;
}

uint8_t mpsiHandleMessageGetZigbeeJoiningDeviceInfo(MpsiMessage_t* mpsiMessage)
{
  if (!mpsiMessage) {
    return MPSI_INVALID_PARAMETER;
  }

  // The BLE side will handle fetching the EUI and Install Code and send it to
  // the Mobile App

  return MPSI_UNSUPPORTED_COMMAND;
}

uint8_t mpsiHandleMessageZigbeeJoiningDeviceInfo(MpsiMessage_t* mpsiMessage)
{
  if (!mpsiMessage) {
    return MPSI_INVALID_PARAMETER;
  }

  // Nothing needs to be done here for the stacks currently
  // Receiving this message is of value to the Mobile App

  return MPSI_SUCCESS;
}

uint8_t mpsiHandleMessageSetZigbeeJoiningDeviceInfo(MpsiMessage_t* mpsiMessage)
{
#if defined (EMBER_STACK_ZIGBEE)
  MpsiZigbeeJoiningDeviceInfoMessage_t message;
  EmberStatus status;
  EmberKeyData key;
  uint8_t bytesDeserialized;

  if (!mpsiMessage) {
    return MPSI_INVALID_PARAMETER;
  }

  bytesDeserialized = emAfPluginMpsiDeserializeSpecificMessage(
    mpsiMessage->payload,
    mpsiMessage->messageId,
    &message);

  if (mpsiMessage->payloadLength != bytesDeserialized) {
    mpsiPrintln("MPSI (0x%x) error: deserialize error with len %d (%d)",
                mpsiMessage->messageId, bytesDeserialized,
                mpsiMessage->payloadLength);
    return MPSI_INVALID_PARAMETER;
  }

  // Convert the install code to a key
  status = emAfInstallCodeToKey(message.installCode,
                                message.installCodeLength,
                                &key);

  if (EMBER_SUCCESS != status) {
    mpsiPrint("MPSI (0x%x) error: ", mpsiMessage->messageId);
    if (EMBER_SECURITY_DATA_INVALID == status) {
      mpsiPrintln("CRC mismatch");
    } else if (EMBER_BAD_ARGUMENT == status) {
      mpsiPrintln("invalid installation code length (%d)",
                  message.installCodeLength);
    } else {
      mpsiPrintln("hash error (0x%x)", status);
    }
    return MPSI_INVALID_PARAMETER;
  }

  // Set the key in the transient key table
  status = addTransientLinkKey(message.eui64, &key);
  if (EMBER_SUCCESS != status) {
    mpsiPrintln("MPSI (0x%x) error: failed to update key table (0x%x)",
                mpsiMessage->messageId, status);
    return MPSI_ERROR;
  }

  return MPSI_SUCCESS;
#else
  return MPSI_UNSUPPORTED_COMMAND;
#endif // EMBER_STACK_ZIGBEE
}

uint8_t mpsiHandleMessageGetZigbeeTrustCenterJoiningCredentials(
  MpsiMessage_t* mpsiMessage)
{
#if defined (EMBER_STACK_ZIGBEE)
  MpsiMessage_t response;
  MpsiZigbeeTrustCenterJoiningCredentialsMessage_t tcJoiningCredentialsMessage;
  EmberNodeType nodeType;
  EmberNetworkParameters parameters;
  EmberStatus status;
  EmberKeyData key;
  uint8_t i;
  EmberEUI64 wildcardEui64 = WILDCARD_EUI64;

  if (!mpsiMessage) {
    return MPSI_INVALID_PARAMETER;
  }

  // If we're not up on a network, return an error
  status = getNetworkParameters(&nodeType, &parameters);
  if (EMBER_SUCCESS != status) {
    return (EMBER_NOT_JOINED == status) ? MPSI_INVALID_FUNCTION : MPSI_ERROR;
  }

  // Generate a random key
  for (i = 0; i < COUNTOF(key.contents); i++) {
    key.contents[i] = (uint8_t)halCommonGetRandom();
  }

  // Construct the message response
  response.destinationAppId = MPSI_APP_ID_MOBILE_APP;
  response.messageId = MPSI_MESSAGE_ID_ZIGBEE_TC_JOINING_CREDENTIALS;

  tcJoiningCredentialsMessage.channelMask = BIT32(parameters.radioChannel);
  MEMCOPY(tcJoiningCredentialsMessage.extendedPanId,
          parameters.extendedPanId,
          COUNTOF(tcJoiningCredentialsMessage.extendedPanId));
  MEMCOPY(tcJoiningCredentialsMessage.preconfiguredKey,
          key.contents,
          COUNTOF(tcJoiningCredentialsMessage.preconfiguredKey));

  response.payloadLength = emAfPluginMpsiSerializeSpecificMessage(
    &tcJoiningCredentialsMessage,
    response.messageId,
    response.payload);

  if (0 == response.payloadLength) {
    mpsiPrintln("MPSI (0x%x) error: serialize error with len %d",
                response.messageId, sizeof(tcJoiningCredentialsMessage));
    return MPSI_ERROR;
  }

  // Add the key in the key table with a wildcard EUI
  status = addTransientLinkKey(wildcardEui64, &key);
  if (EMBER_SUCCESS != status) {
    mpsiPrintln("MPSI (0x%x) error: failed to update key table (0x%x)",
                mpsiMessage->messageId, status);
    return MPSI_ERROR;
  }

  // Open the network for joining
  status = emberAfPermitJoin(MPSI_PERMIT_JOIN_TIME_SEC, false); // no broadcast
  if (EMBER_SUCCESS != status) {
    mpsiPrintln("MPSI (0x%x) error: failed to open network for joining (0x%x)",
                mpsiMessage->messageId, status);
    return MPSI_ERROR;
  }

  return emberAfPluginMpsiSendMessage(&response);
#else
  return MPSI_UNSUPPORTED_COMMAND;
#endif
}

uint8_t mpsiHandleMessageZigbeeTrustCenterJoiningCredentials(
  MpsiMessage_t* mpsiMessage)
{
  if (!mpsiMessage) {
    return MPSI_INVALID_PARAMETER;
  }

  // Nothing needs to be done here for the stacks currently
  // Receiving this message is of value to the Mobile App

  return MPSI_SUCCESS;
}

uint8_t mpsiHandleMessageSetZigbeeTrustCenterJoiningCredentials(
  MpsiMessage_t* mpsiMessage)
{
#if defined (EMBER_STACK_ZIGBEE) && defined(EMBER_AF_API_NETWORK_STEERING)
  MpsiZigbeeTrustCenterJoiningCredentialsMessage_t message;
  //EmberStatus status;
  uint8_t bytesDeserialized;

  if (!mpsiMessage) {
    return MPSI_INVALID_PARAMETER;
  }

  bytesDeserialized = emAfPluginMpsiDeserializeSpecificMessage(
    mpsiMessage->payload,
    mpsiMessage->messageId,
    &message);

  if (mpsiMessage->payloadLength != bytesDeserialized) {
    mpsiPrintln("MPSI (0x%x) error: deserialize error with len %d (%d)",
                mpsiMessage->messageId, bytesDeserialized,
                mpsiMessage->payloadLength);
    return MPSI_INVALID_PARAMETER;
  }

  // Update the Network Steering plugin...

  // .. to use the received channel mask
  emAfPluginNetworkSteeringSetChannelMask(message.channelMask, false);

  // .. to filter on the received extended PAN ID
  emAfPluginNetworkSteeringSetExtendedPanIdFilter(message.extendedPanId,
                                                  true);  // enable filter

  // .. and to use the key received in this MPSI message
  emAfPluginNetworkSteeringSetConfiguredKey(message.preconfiguredKey,
                                            true); // use key when joining

  // Finally, kick off network steering to try to join
  emberAfPluginNetworkSteeringStart();

  return MPSI_SUCCESS;
#else
  return MPSI_UNSUPPORTED_COMMAND;
#endif // EMBER_STACK_ZIGBEE
}

#if defined (EMBER_AF_PLUGIN_SLOT_MANAGER)
void gblMetadataCallbackFunc(uint32_t address, uint8_t *data, size_t length,
                             void *context)
{
  char    *maxMessageIdString;
  uint8_t minLength;

  if (!data || (0 == length)) {
    return;
  }

  // We can get keys split over chunks, which means this callback can be
  // called multiple times for one key value
  // As such, when we read metadata, if it matches our key formats, we record
  // the number of bytes read
  if (0 == gGblMetadataBufferIndex) {
    // Check as much as the data as we can. This is up to the length of the key
    // that we're looking for
    minLength = length < (sizeof(MPSI_GBL_METADATA_MAX_MESSAGE_KEY) - 1)
                ? length : (sizeof(MPSI_GBL_METADATA_MAX_MESSAGE_KEY) - 1);

    if (MEMCOMPARE(MPSI_GBL_METADATA_MAX_MESSAGE_KEY, data, minLength) == 0) {
      if (length > sizeof(gGblMetadataBuffer)) {
        length = sizeof(gGblMetadataBuffer);
      }
      MEMCOPY(gGblMetadataBuffer, data, length);
      gGblMetadataBufferIndex += length;
    }
  } else {
    if (length > (sizeof(gGblMetadataBuffer) - gGblMetadataBufferIndex)) {
      length = sizeof(gGblMetadataBuffer) - gGblMetadataBufferIndex;
    }
    MEMCOPY(gGblMetadataBuffer + gGblMetadataBufferIndex, data, length);
    gGblMetadataBufferIndex += length;
  }

  if (gGblMetadataBufferIndex) {
    // If we read something, do we have the whole key yet?
    if (MPSI_GBL_METADATA_END_KEY_CHARACTER
        == gGblMetadataBuffer[gGblMetadataBufferIndex - 1]) {
      // Strip the key value - start with the whole buffer
      maxMessageIdString = gGblMetadataBuffer;
      // Skip the prefix
      maxMessageIdString += sizeof(MPSI_GBL_METADATA_MAX_MESSAGE_KEY) - 1;
      // Make the final asterisk a null terminator
      gGblMetadataBuffer[gGblMetadataBufferIndex - 1] = '\0';
      // And convert
      gMaxMessageIdSupported = (uint16_t)atoi(maxMessageIdString);
    }
  }
}

uint16_t getMaxMessageIdSupportedBySlot(uint32_t slotId, uint32_t capabilities)
{
  // Assume no support
  gMaxMessageIdSupported = 0;

  // Do we have basic support?
  // This macro is defined when the application is compiled with the MPSI plugin
  if (capabilities
      & (1u << APPLICATION_PROPERTIES_CAPABILITIES_MPSI_SUPPORT_BIT)) {
    gMaxMessageIdSupported = MPSI_INITIAL_MAX_MESSAGE_ID_SUPPORTED;
  }

  // Digging further, does the GBL have a metadata tag that indicates the max
  // MPSI message ID supported?
  // This routine calls into the Slot Manager to inspect the GBL in storage
  // As the bootloader code inspects each chunk of data, if it finds metadata
  // tags, it calls the callback function passed in as an argument
  // It is then up to the callback code to inspect the metadata for the data-of-
  // interest
  gGblMetadataBufferIndex = 0;
  (void)emberAfPluginSlotManagerGetMetadataTagsFromGbl(slotId,
                                                       gblMetadataCallbackFunc);

  return gMaxMessageIdSupported;
}

uint8_t bootloaderAppTypeToMpsiAppType(uint32_t type)
{
  uint8_t mpsiAppId = MPSI_APP_ID_NONE;

  switch (type) {
    case APPLICATION_TYPE_ZIGBEE:
      mpsiAppId = MPSI_APP_ID_ZIGBEE;
      break;
    case APPLICATION_TYPE_THREAD:
      mpsiAppId = MPSI_APP_ID_THREAD;
      break;
    case APPLICATION_TYPE_FLEX:
      mpsiAppId = MPSI_APP_ID_CONNECT;
      break;
    case APPLICATION_TYPE_BLUETOOTH_APP:
      mpsiAppId = MPSI_APP_ID_BLE;
      break;
    case APPLICATION_TYPE_MCU:
      mpsiAppId = MPSI_APP_ID_MCU;
      break;
    // APPLICATION_TYPE_BLUETOOTH is left to MPSI_APP_ID_NONE intentionally.
    // Such an app type means that there is no real BLE application only
    // standalone stack section presents.
    case APPLICATION_TYPE_BLUETOOTH:
    case 0:
    default:
      break;
  }

  return mpsiAppId;
}
#endif // EMBER_AF_PLUGIN_SLOT_MANAGER
