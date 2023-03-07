/**************************************************************************//**
 * Copyright 2017 Silicon Laboratories, Inc.
 *
 *****************************************************************************/

#include "mpsi.h"
#include "mpsi-configuration.h"

#if defined(EMBER_STACK_BLE)
 #include "ble-callbacks.h"
#else
 #include "mpsi-callbacks.h"
#endif

#ifdef MPSI_CUSTOMER_INCLUDE_FILE
 #include MPSI_CUSTOMER_INCLUDE_FILE
#endif

static MpsiMessageHandlerMapping_t gMpsiMessageHandlerMap[] =
  MPSI_GENERATED_FUNCTION_MAP;

void emberAfPluginMpsiInitCallback(void)
{
}

bool emIsCustomMpsiMessage(uint16_t messageId)
{
  return (messageId & MPSI_CUSTOM_MESSAGE_BIT);
}

uint8_t emAfPluginMpsiMessageIdSupportedByLocalStack(uint16_t messageId)
{
  uint8_t messageIndex;
  const uint16_t numMessages = sizeof(gMpsiMessageHandlerMap)
                               / sizeof(MpsiMessageHandlerMapping_t);

  for (messageIndex = 0; messageIndex < numMessages; messageIndex++) {
    if (gMpsiMessageHandlerMap[messageIndex].messageId == messageId) {
      return MPSI_SUCCESS;
    }
  }

  return MPSI_UNSUPPORTED_COMMAND;
}

uint8_t emberAfPluginMpsiReceiveMessage(MpsiMessage_t* mpsiMessage)
{
  uint8_t status;
  uint8_t messageIndex;
  const uint16_t numMessages = sizeof(gMpsiMessageHandlerMap)
                               / sizeof(MpsiMessageHandlerMapping_t);

  if (!mpsiMessage) {
    return MPSI_INVALID_PARAMETER;
  }

  mpsiPrintln("MPSI RX: Dest app ID: %d, Msg ID: %d, Payload len: %d",
              mpsiMessage->destinationAppId,
              mpsiMessage->messageId,
              mpsiMessage->payloadLength);

  if (mpsiMessage->destinationAppId != MPSI_APP_ID) {
#if (MPSI_APP_ID_BLE == MPSI_APP_ID)
    // If we are the BLE app and we received a message meant for the Mobile App,
    // forward it now
    if (MPSI_APP_ID_MOBILE_APP == mpsiMessage->destinationAppId) {
      return emBleSendMpsiMessageToMobileApp(mpsiMessage);
    }
#endif // (MPSI_APP_ID_BLE == MPSI_APP_ID)
    return MPSI_WRONG_APP;
  }

  for (messageIndex = 0; messageIndex < numMessages; messageIndex++) {
    if (gMpsiMessageHandlerMap[messageIndex].messageId
        == mpsiMessage->messageId) {
      status = (gMpsiMessageHandlerMap[messageIndex].function)(mpsiMessage);

#if !defined(BLE_NCP_MOBILE_APP)
      // If an error was encountered, send an error response to the Mobile App
      emAfPluginMpsiProcessStatus(mpsiMessage, status);
#endif // BLE_NCP_MOBILE_APP

      return status;
    }
  }

  return MPSI_UNSUPPORTED_COMMAND;
}

uint8_t emberAfPluginMpsiSendMessage(MpsiMessage_t* mpsiMessage)
{
  uint8_t retVal = MPSI_ERROR;

  if (!mpsiMessage) {
    return MPSI_INVALID_PARAMETER;
  }

  // Sending to ourself is not allowed
  if (mpsiMessage->destinationAppId == MPSI_APP_ID) {
    return MPSI_INVALID_PARAMETER;
  } else {
#if (MPSI_APP_ID_BLE == MPSI_APP_ID)
    // If we are the BLE app and we're sending a message meant for the Mobile
    // App, forward it now
    if (MPSI_APP_ID_MOBILE_APP == mpsiMessage->destinationAppId) {
      return emBleSendMpsiMessageToMobileApp(mpsiMessage);
    }
#endif // (MPSI_APP_ID_BLE == MPSI_APP_ID)
    // Send the MPSI message to the right stack using either the MPSI Storage
    // plugin (for switched SoC) or an IPC mechanism (for switched host apps or
    // dynamic multiprotocol)
    // If we are not the BLE app and we're trying to send to the Mobile App,
    // the message will go through MPSI Storage/IPC and be picked up by the BLE
    // app, who will then forward it to the Mobile App
    retVal = mpsiSendMessageToStack(mpsiMessage);
  }

  return retVal;
}

void emAfPluginMpsiProcessStatus(MpsiMessage_t* mpsiMessage, uint8_t status)
{
// Only a host app or BLE app can send an error message back to the Mobile App
#if defined(EZSP_HOST) || defined(EMBER_STACK_BLE)
  MpsiMessage_t responseMessage;
  MpsiErrorMessage_t errorMessage;
  uint8_t bytesSerialized;
  uint8_t result;

  if (!mpsiMessage || (MPSI_SUCCESS == status)) {
    return;
  }

  errorMessage.errorCode = status;
  errorMessage.sourceApplicationId = MPSI_APP_ID;
  errorMessage.messageIdInError = mpsiMessage->messageId;

  responseMessage.destinationAppId = MPSI_APP_ID_MOBILE_APP;
  responseMessage.messageId = MPSI_MESSAGE_ID_ERROR;
  responseMessage.payloadLength = sizeof(errorMessage);

  bytesSerialized = emAfPluginMpsiSerializeSpecificMessage(
    &errorMessage,
    responseMessage.messageId,
    responseMessage.payload);

  if (0 == bytesSerialized) {
    mpsiPrintln("MPSI: failed to serialize error message of len %d",
                sizeof(errorMessage));
    return;
  }

  result = emberAfPluginMpsiSendMessage(&responseMessage);

  if (MPSI_SUCCESS != result) {
    mpsiPrintln("MPSI: failed to send error message (error %d) for message 0x%X"
                " error %d", result, errorMessage.messageIdInError, status);
  }
#endif // defined(EZSP_HOST) || defined(EMBER_STACK_BLE))
}
