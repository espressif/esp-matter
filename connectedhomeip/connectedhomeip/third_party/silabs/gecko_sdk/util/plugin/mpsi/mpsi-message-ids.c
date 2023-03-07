/**************************************************************************//**
 * Copyright 2017 Silicon Laboratories, Inc.
 *
 *****************************************************************************/
#include PLATFORM_HEADER
#include "mpsi-message-ids.h"

uint16_t emberAfPluginMpsiSerialize(MpsiMessage_t *mpsiMessage, uint8_t *buffer)
{
  uint8_t* pBuffer = buffer;

  if (!mpsiMessage || !buffer) {
    return 0;
  }

  *pBuffer = mpsiMessage->destinationAppId;
  pBuffer++;

  *pBuffer = BYTE_0(mpsiMessage->messageId);
  pBuffer++;

  *pBuffer = BYTE_1(mpsiMessage->messageId);
  pBuffer++;

  *pBuffer = mpsiMessage->payloadLength;
  pBuffer++;

  MEMCOPY(pBuffer, mpsiMessage->payload, mpsiMessage->payloadLength);
  pBuffer += mpsiMessage->payloadLength;

  return (pBuffer - buffer);
}

uint16_t emberAfPluginMpsiDeserialize(uint8_t       *buffer,
                                      MpsiMessage_t *mpsiMessage)
{
  uint8_t* pBuffer = buffer;

  if (!buffer || !mpsiMessage) {
    return 0;
  }

  mpsiMessage->destinationAppId = *pBuffer;
  pBuffer++;

  mpsiMessage->messageId = *pBuffer;
  pBuffer++;

  mpsiMessage->messageId |= (*pBuffer << 8);
  pBuffer++;

  mpsiMessage->payloadLength = *pBuffer;
  pBuffer++;

  MEMCOPY(mpsiMessage->payload, pBuffer, mpsiMessage->payloadLength);
  pBuffer += mpsiMessage->payloadLength;

  return (pBuffer - buffer);
}

uint8_t emAfPluginMpsiSerializeSpecificMessage(void     *specificMpsiMessage,
                                               uint16_t  messageId,
                                               uint8_t  *buffer)
{
  uint8_t* pBuffer = buffer;
  uint8_t  numBytes;

  if (!specificMpsiMessage || !buffer) {
    return 0;
  }

  switch (messageId) {
    case MPSI_MESSAGE_ID_GET_APPS_INFO:
    {
      // MPSI_MESSAGE_ID_GET_APPS_INFO has no payload (and therefore no message
      // struct)
      break;
    }

    case MPSI_MESSAGE_ID_APPS_INFO:
      // Note that one MpsiMessage_t message may have multiple entries of this
      // message, one for each application on the chip (one running and those in
      // memory). As such, the caller should call this as many times as needed,
      // supplying a different specificMpsiMessage value and an adjusted buffer
      // pointer each time this is called
    {
      MpsiAppsInfoMessage_t* appsInfoMessage =
        (MpsiAppsInfoMessage_t*)specificMpsiMessage;

      *pBuffer = appsInfoMessage->slotId;
      pBuffer++;

      *pBuffer = appsInfoMessage->applicationId;
      pBuffer++;

      *pBuffer = BYTE_0(appsInfoMessage->applicationVersion);
      pBuffer++;

      *pBuffer = BYTE_1(appsInfoMessage->applicationVersion);
      pBuffer++;

      *pBuffer = BYTE_2(appsInfoMessage->applicationVersion);
      pBuffer++;

      *pBuffer = BYTE_3(appsInfoMessage->applicationVersion);
      pBuffer++;

      *pBuffer = BYTE_0(appsInfoMessage->maxMessageIdSupported);
      pBuffer++;

      *pBuffer = BYTE_1(appsInfoMessage->maxMessageIdSupported);
      pBuffer++;

      break;
    }

    case MPSI_MESSAGE_ID_BOOTLOAD_SLOT:
    {
      MpsiBootloadSlotMessage_t* bootloadSlotMessage =
        (MpsiBootloadSlotMessage_t*)specificMpsiMessage;

      *pBuffer = bootloadSlotMessage->slotId;
      pBuffer++;

      break;
    }

    case MPSI_MESSAGE_ID_ERROR:
    {
      MpsiErrorMessage_t* errorMessage =
        (MpsiErrorMessage_t*)specificMpsiMessage;

      *pBuffer = errorMessage->errorCode;
      pBuffer++;

      *pBuffer = errorMessage->sourceApplicationId;
      pBuffer++;

      *pBuffer = BYTE_0(errorMessage->messageIdInError);
      pBuffer++;

      *pBuffer = BYTE_1(errorMessage->messageIdInError);
      pBuffer++;

      break;
    }

    case MPSI_MESSAGE_ID_INITIATE_JOINING:
    {
      MpsiInitiateJoiningMessage_t* initiateJoiningMessage =
        (MpsiInitiateJoiningMessage_t*)specificMpsiMessage;

      *pBuffer = initiateJoiningMessage->option;
      pBuffer++;

      break;
    }

    case MPSI_MESSAGE_ID_GET_ZIGBEE_JOINING_DEVICE_INFO:
    {
      // MPSI_MESSAGE_ID_GET_ZIGBEE_JOINING_DEVICE_INFO has no payload (and
      // therefore no message struct)
      break;
    }

    case MPSI_MESSAGE_ID_ZIGBEE_JOINING_DEVICE_INFO:
    case MPSI_MESSAGE_ID_SET_ZIGBEE_JOINING_DEVICE_INFO:
    {
      MpsiZigbeeJoiningDeviceInfoMessage_t* zigbeeJoiningDeviceInfoMessage =
        (MpsiZigbeeJoiningDeviceInfoMessage_t*)specificMpsiMessage;

      numBytes = COUNTOF(zigbeeJoiningDeviceInfoMessage->eui64);
      MEMCOPY(pBuffer, zigbeeJoiningDeviceInfoMessage->eui64, numBytes);
      pBuffer += numBytes;

      *pBuffer = zigbeeJoiningDeviceInfoMessage->installCodeLength;
      pBuffer++;

      MEMCOPY(pBuffer,
              zigbeeJoiningDeviceInfoMessage->installCode,
              zigbeeJoiningDeviceInfoMessage->installCodeLength);
      pBuffer += zigbeeJoiningDeviceInfoMessage->installCodeLength;
      break;
    }

    case MPSI_MESSAGE_ID_ZIGBEE_TC_JOINING_CREDENTIALS:
    case MPSI_MESSAGE_ID_SET_ZIGBEE_TC_JOINING_CREDENTIALS:
    {
      MpsiZigbeeTrustCenterJoiningCredentialsMessage_t
      *zbTcJoiningCredentialsMessage =
        (MpsiZigbeeTrustCenterJoiningCredentialsMessage_t*)specificMpsiMessage;

      *pBuffer = BYTE_0(zbTcJoiningCredentialsMessage->channelMask);
      pBuffer++;

      *pBuffer = BYTE_1(zbTcJoiningCredentialsMessage->channelMask);
      pBuffer++;

      *pBuffer = BYTE_2(zbTcJoiningCredentialsMessage->channelMask);
      pBuffer++;

      *pBuffer = BYTE_3(zbTcJoiningCredentialsMessage->channelMask);
      pBuffer++;

      numBytes = COUNTOF(zbTcJoiningCredentialsMessage->extendedPanId);
      MEMCOPY(pBuffer,
              zbTcJoiningCredentialsMessage->extendedPanId,
              numBytes);
      pBuffer += numBytes;

      numBytes = COUNTOF(zbTcJoiningCredentialsMessage->preconfiguredKey);
      MEMCOPY(pBuffer,
              zbTcJoiningCredentialsMessage->preconfiguredKey,
              numBytes);
      pBuffer += numBytes;
      break;
    }

    default:
      break;
  }

  return (pBuffer - buffer);
}

uint8_t emAfPluginMpsiDeserializeSpecificMessage(uint8_t  *buffer,
                                                 uint16_t  messageId,
                                                 void     *specificMpsiMessage)
{
  uint8_t* pBuffer = buffer;
  uint8_t  numBytes;

  if (!buffer || !specificMpsiMessage) {
    return 0;
  }

  switch (messageId) {
    case MPSI_MESSAGE_ID_GET_APPS_INFO:
    {
      // MPSI_MESSAGE_ID_GET_APPS_INFO has no payload (and therefore no message
      // struct)
      break;
    }

    case MPSI_MESSAGE_ID_APPS_INFO:
      // Note that the buffer may have multiple entries of this message, one
      // for each application on the chip (the one running and those in memory)
      // As such, the caller should call this as many times as needed, supplying
      // an adjusted buffer pointer each time this is called
    {
      MpsiAppsInfoMessage_t* appsInfoMessage =
        (MpsiAppsInfoMessage_t*)specificMpsiMessage;

      appsInfoMessage->slotId = *pBuffer;
      pBuffer++;

      appsInfoMessage->applicationId = *pBuffer;
      pBuffer++;

      appsInfoMessage->applicationVersion = *pBuffer;
      pBuffer++;

      appsInfoMessage->applicationVersion |= *pBuffer << 8;
      pBuffer++;

      appsInfoMessage->applicationVersion |= *pBuffer << 16;
      pBuffer++;

      appsInfoMessage->applicationVersion |= *pBuffer << 24;
      pBuffer++;

      appsInfoMessage->maxMessageIdSupported = *pBuffer;
      pBuffer++;

      appsInfoMessage->maxMessageIdSupported |= *pBuffer << 8;
      pBuffer++;

      break;
    }

    case MPSI_MESSAGE_ID_BOOTLOAD_SLOT:
    {
      MpsiBootloadSlotMessage_t* bootloadSlotMessage =
        (MpsiBootloadSlotMessage_t*)specificMpsiMessage;

      bootloadSlotMessage->slotId = *pBuffer;
      pBuffer++;

      break;
    }

    case MPSI_MESSAGE_ID_ERROR:
    {
      MpsiErrorMessage_t* errorMessage =
        (MpsiErrorMessage_t*)specificMpsiMessage;

      errorMessage->errorCode = *pBuffer;
      pBuffer++;

      errorMessage->sourceApplicationId = *pBuffer;
      pBuffer++;

      errorMessage->messageIdInError = *pBuffer;
      pBuffer++;

      errorMessage->messageIdInError |= *pBuffer << 8;
      pBuffer++;

      break;
    }

    case MPSI_MESSAGE_ID_INITIATE_JOINING:
    {
      MpsiInitiateJoiningMessage_t* initiateJoiningMessage =
        (MpsiInitiateJoiningMessage_t*)specificMpsiMessage;

      initiateJoiningMessage->option = *pBuffer;
      pBuffer++;

      break;
    }

    case MPSI_MESSAGE_ID_GET_ZIGBEE_JOINING_DEVICE_INFO:
    {
      // MPSI_MESSAGE_ID_GET_ZIGBEE_JOINING_DEVICE_INFO has no payload (and
      // therefore no message struct)
      break;
    }

    case MPSI_MESSAGE_ID_ZIGBEE_JOINING_DEVICE_INFO:
    case MPSI_MESSAGE_ID_SET_ZIGBEE_JOINING_DEVICE_INFO:
    {
      MpsiZigbeeJoiningDeviceInfoMessage_t* zigbeeJoiningDeviceInfoMessage =
        (MpsiZigbeeJoiningDeviceInfoMessage_t*)specificMpsiMessage;

      numBytes = COUNTOF(zigbeeJoiningDeviceInfoMessage->eui64);
      MEMCOPY(zigbeeJoiningDeviceInfoMessage->eui64, pBuffer, numBytes);
      pBuffer += numBytes;

      zigbeeJoiningDeviceInfoMessage->installCodeLength = *pBuffer;
      pBuffer++;

      numBytes = COUNTOF(zigbeeJoiningDeviceInfoMessage->installCode);
      if (zigbeeJoiningDeviceInfoMessage->installCodeLength > numBytes) {
        return 0;
      }

      MEMCOPY(zigbeeJoiningDeviceInfoMessage->installCode,
              pBuffer,
              zigbeeJoiningDeviceInfoMessage->installCodeLength);
      pBuffer += zigbeeJoiningDeviceInfoMessage->installCodeLength;

      break;
    }

    case MPSI_MESSAGE_ID_ZIGBEE_TC_JOINING_CREDENTIALS:
    case MPSI_MESSAGE_ID_SET_ZIGBEE_TC_JOINING_CREDENTIALS:
    {
      MpsiZigbeeTrustCenterJoiningCredentialsMessage_t
      *zbTcJoiningCredentialsMessage =
        (MpsiZigbeeTrustCenterJoiningCredentialsMessage_t*)specificMpsiMessage;

      zbTcJoiningCredentialsMessage->channelMask = *pBuffer;
      pBuffer++;

      zbTcJoiningCredentialsMessage->channelMask |= *pBuffer << 8;
      pBuffer++;

      zbTcJoiningCredentialsMessage->channelMask |= *pBuffer << 16;
      pBuffer++;

      zbTcJoiningCredentialsMessage->channelMask |= *pBuffer << 24;
      pBuffer++;

      numBytes = COUNTOF(zbTcJoiningCredentialsMessage->extendedPanId);
      MEMCOPY(zbTcJoiningCredentialsMessage->extendedPanId,
              pBuffer,
              numBytes);
      pBuffer += numBytes;

      numBytes = COUNTOF(zbTcJoiningCredentialsMessage->preconfiguredKey);
      MEMCOPY(zbTcJoiningCredentialsMessage->preconfiguredKey,
              pBuffer,
              numBytes);
      pBuffer += numBytes;

      break;
    }

    default:
      break;
  }

  return (pBuffer - buffer);
}
