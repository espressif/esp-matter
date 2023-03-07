/***************************************************************************//**
 * @brief Host functions for sending Connect management commands to the NCP.
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

#include "stack/include/api-rename.h"
#include "stack/include/ember.h"
#include "stack/include/api-rename-undef.h"

#include "cmsis-rtos-support.h"
#include "cmsis-rtos-ipc-api-gen.h"

// networkState
EmberNetworkStatus emberNetworkState(void)
{
  emAfPluginCmsisRtosAcquireCommandMutex();
  emAfPluginCmsisRtosSendBlockingCommand(EMBER_NETWORK_STATE_IPC_COMMAND_ID,
                                         "");

  EmberNetworkStatus networkStatus;
  emAfPluginCmsisRtosFetchApiParams("u",
                                    &networkStatus);
  emAfPluginCmsisRtosReleaseCommandMutex();
  return networkStatus;
}
static void networkStateCommandHandler(void)
{
  EmberNetworkStatus networkStatus = emApiNetworkState();
  emAfPluginCmsisRtosSendResponse(EMBER_NETWORK_STATE_IPC_COMMAND_ID,
                                  "u",
                                  networkStatus);
}

// stackIsUp
bool emberStackIsUp(void)
{
  emAfPluginCmsisRtosAcquireCommandMutex();
  emAfPluginCmsisRtosSendBlockingCommand(EMBER_STACK_IS_UP_IPC_COMMAND_ID,
                                         "");

  bool stackIsUp;
  emAfPluginCmsisRtosFetchApiParams("u",
                                    &stackIsUp);
  emAfPluginCmsisRtosReleaseCommandMutex();
  return stackIsUp;
}
static void stackIsUpCommandHandler(void)
{
  bool stackIsUp = emApiStackIsUp();
  emAfPluginCmsisRtosSendResponse(EMBER_STACK_IS_UP_IPC_COMMAND_ID,
                                  "u",
                                  stackIsUp);
}

#ifdef SL_CATALOG_CONNECT_AES_SECURITY_PRESENT
// setSecurityKey
EmberStatus emberSetSecurityKey(EmberKeyData *key)
{
  emAfPluginCmsisRtosAcquireCommandMutex();
  emAfPluginCmsisRtosSendBlockingCommand(EMBER_SET_SECURITY_KEY_IPC_COMMAND_ID,
                                         "b",
                                         key, sizeof(EmberKeyData));

  EmberStatus status;
  emAfPluginCmsisRtosFetchApiParams("u",
                                    &status);
  emAfPluginCmsisRtosReleaseCommandMutex();
  return status;
}
static void setSecurityKeyCommandHandler(void)
{
  EmberKeyData key;
  uint8_t length;
  emAfPluginCmsisRtosFetchApiParams("b",
                                    &key,
                                    &length);
  EmberStatus status = emApiSetSecurityKey(&key);
  emAfPluginCmsisRtosSendResponse(EMBER_SET_SECURITY_KEY_IPC_COMMAND_ID,
                                  "u",
                                  status);
}

// GetSecurityKey
EmberStatus emberGetSecurityKey(EmberKeyData *key)
{
  emAfPluginCmsisRtosAcquireCommandMutex();
  emAfPluginCmsisRtosSendBlockingCommand(EMBER_GET_SECURITY_KEY_IPC_COMMAND_ID,
                                         "b",
                                         key->contents,
                                         EMBER_ENCRYPTION_KEY_SIZE);

  EmberStatus status;

  uint8_t Size = EMBER_ENCRYPTION_KEY_SIZE;
  emAfPluginCmsisRtosFetchApiParams("ub",
                                    &status,
                                    key->contents,
                                    &Size);
  emAfPluginCmsisRtosReleaseCommandMutex();
  return status;
}
static void GetSecurityKeyCommandHandler(void)
{
  EmberKeyData key;
  uint8_t emberEncryptionKeySize = EMBER_ENCRYPTION_KEY_SIZE;
  emAfPluginCmsisRtosFetchApiParams("b",
                                    &key.contents,
                                    &emberEncryptionKeySize);
  EmberStatus status = emApiGetSecurityKey(&key);
  emAfPluginCmsisRtosSendResponse(EMBER_GET_SECURITY_KEY_IPC_COMMAND_ID,
                                  "ub",
                                  status,
                                  key.contents,
                                  emberEncryptionKeySize);
}

// setPsaSecurityKey
EmberStatus emberSetPsaSecurityKey(mbedtls_svc_key_id_t key_id)
{
  emAfPluginCmsisRtosAcquireCommandMutex();
  emAfPluginCmsisRtosSendBlockingCommand(EMBER_SET_PSA_SECURITY_KEY_IPC_COMMAND_ID,
                                         "w",
                                         key_id);

  EmberStatus status;
  emAfPluginCmsisRtosFetchApiParams("u",
                                    &status);
  emAfPluginCmsisRtosReleaseCommandMutex();
  return status;
}
static void setPsaSecurityKeyCommandHandler(void)
{
  mbedtls_svc_key_id_t key_id;
  emAfPluginCmsisRtosFetchApiParams("w",
                                    &key_id);
  EmberStatus status = emApiSetPsaSecurityKey(key_id);
  emAfPluginCmsisRtosSendResponse(EMBER_SET_PSA_SECURITY_KEY_IPC_COMMAND_ID,
                                  "u",
                                  status);
}

// RemovePsaSecurityKey
EmberStatus emberRemovePsaSecurityKey(void)
{
  emAfPluginCmsisRtosAcquireCommandMutex();
  emAfPluginCmsisRtosSendBlockingCommand(EMBER_REMOVE_PSA_SECURITY_KEY_IPC_COMMAND_ID,
                                         "");

  EmberStatus status;
  emAfPluginCmsisRtosFetchApiParams("u",
                                    &status);
  emAfPluginCmsisRtosReleaseCommandMutex();
  return status;
}
static void RemovePsaSecurityKeyCommandHandler(void)
{
  EmberStatus status = emApiRemovePsaSecurityKey();
  emAfPluginCmsisRtosSendResponse(EMBER_REMOVE_PSA_SECURITY_KEY_IPC_COMMAND_ID,
                                  "u",
                                  status);
}
#endif

// getCounter
EmberStatus emberGetCounter(EmberCounterType counterType,
                            uint32_t* count)
{
  emAfPluginCmsisRtosAcquireCommandMutex();
  emAfPluginCmsisRtosSendBlockingCommand(EMBER_GET_COUNTER_IPC_COMMAND_ID,
                                         "u",
                                         counterType);

  EmberStatus status;

  emAfPluginCmsisRtosFetchApiParams("uw",
                                    &status,
                                    count);
  emAfPluginCmsisRtosReleaseCommandMutex();
  return status;
}
static void getCounterCommandHandler(void)
{
  EmberCounterType counterType;
  uint32_t count;
  emAfPluginCmsisRtosFetchApiParams("u",
                                    &counterType);
  EmberStatus status = emApiGetCounter(counterType,
                                       &count);
  emAfPluginCmsisRtosSendResponse(EMBER_GET_COUNTER_IPC_COMMAND_ID,
                                  "uw",
                                  status,
                                  count);
}

// setRadioChannelExtended
EmberStatus emberSetRadioChannelExtended(uint16_t channel,
                                         bool persistent)
{
  emAfPluginCmsisRtosAcquireCommandMutex();
  emAfPluginCmsisRtosSendBlockingCommand(EMBER_SET_RADIO_CHANNEL_EXTENDED_IPC_COMMAND_ID,
                                         "vu",
                                         channel,
                                         persistent);

  EmberStatus status;
  emAfPluginCmsisRtosFetchApiParams("u",
                                    &status);
  emAfPluginCmsisRtosReleaseCommandMutex();
  return status;
}
static void setRadioChannelExtendedCommandHandler(void)
{
  uint16_t channel;
  bool persistent;
  emAfPluginCmsisRtosFetchApiParams("vu",
                                    &channel,
                                    &persistent);
  EmberStatus status = emApiSetRadioChannelExtended(channel,
                                                    persistent);
  emAfPluginCmsisRtosSendResponse(EMBER_SET_RADIO_CHANNEL_EXTENDED_IPC_COMMAND_ID,
                                  "u",
                                  status);
}

// getRadioChannel
uint16_t emberGetRadioChannel(void)
{
  emAfPluginCmsisRtosAcquireCommandMutex();
  emAfPluginCmsisRtosSendBlockingCommand(EMBER_GET_RADIO_CHANNEL_IPC_COMMAND_ID,
                                         "");

  uint16_t channel;
  emAfPluginCmsisRtosFetchApiParams("v",
                                    &channel);
  emAfPluginCmsisRtosReleaseCommandMutex();
  return channel;
}
static void getRadioChannelCommandHandler(void)
{
  uint16_t channel = emApiGetRadioChannel();
  emAfPluginCmsisRtosSendResponse(EMBER_GET_RADIO_CHANNEL_IPC_COMMAND_ID,
                                  "v",
                                  channel);
}

// setRadioPower
EmberStatus emberSetRadioPower(int16_t power,
                               bool persistent)
{
  emAfPluginCmsisRtosAcquireCommandMutex();
  emAfPluginCmsisRtosSendBlockingCommand(EMBER_SET_RADIO_POWER_IPC_COMMAND_ID,
                                         "vu",
                                         power,
                                         persistent);

  EmberStatus status;
  emAfPluginCmsisRtosFetchApiParams("u",
                                    &status);
  emAfPluginCmsisRtosReleaseCommandMutex();
  return status;
}
static void setRadioPowerCommandHandler(void)
{
  int16_t power;
  bool persistent;
  emAfPluginCmsisRtosFetchApiParams("vu",
                                    &power,
                                    &persistent);
  EmberStatus status = emApiSetRadioPower(power,
                                          persistent);
  emAfPluginCmsisRtosSendResponse(EMBER_SET_RADIO_POWER_IPC_COMMAND_ID,
                                  "u",
                                  status);
}

// getRadioPower
int16_t emberGetRadioPower(void)
{
  emAfPluginCmsisRtosAcquireCommandMutex();
  emAfPluginCmsisRtosSendBlockingCommand(EMBER_GET_RADIO_POWER_IPC_COMMAND_ID,
                                         "");

  int16_t power;
  emAfPluginCmsisRtosFetchApiParams("v",
                                    &power);
  emAfPluginCmsisRtosReleaseCommandMutex();
  return power;
}
static void getRadioPowerCommandHandler(void)
{
  int16_t power = emApiGetRadioPower();
  emAfPluginCmsisRtosSendResponse(EMBER_GET_RADIO_POWER_IPC_COMMAND_ID,
                                  "v",
                                  power);
}

// setRadioPowerMode
EmberStatus emberSetRadioPowerMode(bool radioOn)
{
  emAfPluginCmsisRtosAcquireCommandMutex();
  emAfPluginCmsisRtosSendBlockingCommand(EMBER_SET_RADIO_POWER_MODE_IPC_COMMAND_ID,
                                         "u",
                                         radioOn);

  EmberStatus status;
  emAfPluginCmsisRtosFetchApiParams("u",
                                    &status);
  emAfPluginCmsisRtosReleaseCommandMutex();
  return status;
}
static void setRadioPowerModeCommandHandler(void)
{
  bool radioOn;
  emAfPluginCmsisRtosFetchApiParams("u",
                                    &radioOn);
  EmberStatus status = emApiSetRadioPowerMode(radioOn);
  emAfPluginCmsisRtosSendResponse(EMBER_SET_RADIO_POWER_MODE_IPC_COMMAND_ID,
                                  "u",
                                  status);
}

// setMacParams
EmberStatus emberSetMacParams(int8_t ccaThreshold,
                              uint8_t maxCcaAttempts,
                              uint8_t minBackoffExp,
                              uint8_t maxBackoffExp,
                              uint16_t ccaBackoff,
                              uint16_t ccaDuration,
                              uint8_t maxRetries,
                              uint32_t csmaTimeout,
                              uint16_t ackTimeout)
{
  emAfPluginCmsisRtosAcquireCommandMutex();
  emAfPluginCmsisRtosSendBlockingCommand(EMBER_SET_MAC_PARAMS_IPC_COMMAND_ID,
                                         "suuuvvuwv",
                                         ccaThreshold,
                                         maxCcaAttempts,
                                         minBackoffExp,
                                         maxBackoffExp,
                                         ccaBackoff,
                                         ccaDuration,
                                         maxRetries,
                                         csmaTimeout,
                                         ackTimeout);

  EmberStatus status;
  emAfPluginCmsisRtosFetchApiParams("u",
                                    &status);
  emAfPluginCmsisRtosReleaseCommandMutex();
  return status;
}
static void setMacParamsCommandHandler(void)
{
  int8_t ccaThreshold;
  uint8_t maxCcaAttempts;
  uint8_t minBackoffExp;
  uint8_t maxBackoffExp;
  uint16_t ccaBackoff;
  uint16_t ccaDuration;
  uint8_t maxRetries;
  uint32_t csmaTimeout;
  uint16_t ackTimeout;
  emAfPluginCmsisRtosFetchApiParams("suuuvvuwv",
                                    &ccaThreshold,
                                    &maxCcaAttempts,
                                    &minBackoffExp,
                                    &maxBackoffExp,
                                    &ccaBackoff,
                                    &ccaDuration,
                                    &maxRetries,
                                    &csmaTimeout,
                                    &ackTimeout);
  EmberStatus status = emApiSetMacParams(ccaThreshold,
                                         maxCcaAttempts,
                                         minBackoffExp,
                                         maxBackoffExp,
                                         ccaBackoff,
                                         ccaDuration,
                                         maxRetries,
                                         csmaTimeout,
                                         ackTimeout);
  emAfPluginCmsisRtosSendResponse(EMBER_SET_MAC_PARAMS_IPC_COMMAND_ID,
                                  "u",
                                  status);
}

// currentStackTasks
uint16_t emberCurrentStackTasks(void)
{
  emAfPluginCmsisRtosAcquireCommandMutex();
  emAfPluginCmsisRtosSendBlockingCommand(EMBER_CURRENT_STACK_TASKS_IPC_COMMAND_ID,
                                         "");

  uint16_t currentTasks;
  emAfPluginCmsisRtosFetchApiParams("v",
                                    &currentTasks);
  emAfPluginCmsisRtosReleaseCommandMutex();
  return currentTasks;
}
static void currentStackTasksCommandHandler(void)
{
  uint16_t currentTasks = emApiCurrentStackTasks();
  emAfPluginCmsisRtosSendResponse(EMBER_CURRENT_STACK_TASKS_IPC_COMMAND_ID,
                                  "v",
                                  currentTasks);
}

// macGetParentAddress
EmberStatus emberMacGetParentAddress(EmberMacAddress *parentAddress)
{
  emAfPluginCmsisRtosAcquireCommandMutex();
  emAfPluginCmsisRtosSendBlockingCommand(EMBER_MAC_GET_PARENT_ADDRESS_IPC_COMMAND_ID,
                                         "vbu",
                                         parentAddress->addr.shortAddress,
                                         parentAddress->addr.longAddress,
                                         EUI64_SIZE,
                                         parentAddress->mode);

  EmberStatus status;

  uint8_t longAddressSize = EUI64_SIZE;
  emAfPluginCmsisRtosFetchApiParams("uvbu",
                                    &status,
                                    &parentAddress->addr.shortAddress,
                                    parentAddress->addr.longAddress,
                                    &longAddressSize,
                                    &parentAddress->mode);
  emAfPluginCmsisRtosReleaseCommandMutex();
  return status;
}
static void macGetParentAddressCommandHandler(void)
{
  EmberMacAddress parentAddress;
  uint8_t eui64Size = EUI64_SIZE;
  emAfPluginCmsisRtosFetchApiParams("vbu",
                                    &parentAddress.addr.shortAddress,
                                    &parentAddress.addr.longAddress,
                                    &eui64Size,
                                    &parentAddress.mode);
  EmberStatus status = emApiMacGetParentAddress(&parentAddress);
  emAfPluginCmsisRtosSendResponse(EMBER_MAC_GET_PARENT_ADDRESS_IPC_COMMAND_ID,
                                  "uvbu",
                                  status,
                                  parentAddress.addr.shortAddress,
                                  parentAddress.addr.longAddress,
                                  eui64Size,
                                  parentAddress.mode);
}

// calibrateCurrentChannelExtended
EmberStatus emberCalibrateCurrentChannelExtended(uint32_t calValueIn,
                                                 uint32_t* calValueOut)
{
  emAfPluginCmsisRtosAcquireCommandMutex();
  emAfPluginCmsisRtosSendBlockingCommand(EMBER_CALIBRATE_CURRENT_CHANNEL_EXTENDED_IPC_COMMAND_ID,
                                         "w",
                                         calValueIn);

  EmberStatus status;

  emAfPluginCmsisRtosFetchApiParams("uw",
                                    &status,
                                    calValueOut);
  emAfPluginCmsisRtosReleaseCommandMutex();
  return status;
}
static void calibrateCurrentChannelExtendedCommandHandler(void)
{
  uint32_t calValueIn;
  uint32_t calValueOut;
  emAfPluginCmsisRtosFetchApiParams("w",
                                    &calValueIn);
  EmberStatus status = emApiCalibrateCurrentChannelExtended(calValueIn,
                                                            &calValueOut);
  emAfPluginCmsisRtosSendResponse(EMBER_CALIBRATE_CURRENT_CHANNEL_EXTENDED_IPC_COMMAND_ID,
                                  "uw",
                                  status,
                                  calValueOut);
}

// applyIrCalibration
EmberStatus emberApplyIrCalibration(uint32_t calValue)
{
  emAfPluginCmsisRtosAcquireCommandMutex();
  emAfPluginCmsisRtosSendBlockingCommand(EMBER_APPLY_IR_CALIBRATION_IPC_COMMAND_ID,
                                         "w",
                                         calValue);

  EmberStatus status;
  emAfPluginCmsisRtosFetchApiParams("u",
                                    &status);
  emAfPluginCmsisRtosReleaseCommandMutex();
  return status;
}
static void applyIrCalibrationCommandHandler(void)
{
  uint32_t calValue;
  emAfPluginCmsisRtosFetchApiParams("w",
                                    &calValue);
  EmberStatus status = emApiApplyIrCalibration(calValue);
  emAfPluginCmsisRtosSendResponse(EMBER_APPLY_IR_CALIBRATION_IPC_COMMAND_ID,
                                  "u",
                                  status);
}

// tempCalibration
EmberStatus emberTempCalibration(void)
{
  emAfPluginCmsisRtosAcquireCommandMutex();
  emAfPluginCmsisRtosSendBlockingCommand(EMBER_TEMP_CALIBRATION_IPC_COMMAND_ID,
                                         "");

  EmberStatus status;
  emAfPluginCmsisRtosFetchApiParams("u",
                                    &status);
  emAfPluginCmsisRtosReleaseCommandMutex();
  return status;
}
static void tempCalibrationCommandHandler(void)
{
  EmberStatus status = emApiTempCalibration();
  emAfPluginCmsisRtosSendResponse(EMBER_TEMP_CALIBRATION_IPC_COMMAND_ID,
                                  "u",
                                  status);
}

// getCalType
EmberCalType emberGetCalType(void)
{
  emAfPluginCmsisRtosAcquireCommandMutex();
  emAfPluginCmsisRtosSendBlockingCommand(EMBER_GET_CAL_TYPE_IPC_COMMAND_ID,
                                         "");

  EmberCalType calType;
  emAfPluginCmsisRtosFetchApiParams("w",
                                    &calType);
  emAfPluginCmsisRtosReleaseCommandMutex();
  return calType;
}
static void getCalTypeCommandHandler(void)
{
  EmberCalType calType = emApiGetCalType();
  emAfPluginCmsisRtosSendResponse(EMBER_GET_CAL_TYPE_IPC_COMMAND_ID,
                                  "w",
                                  calType);
}

// getMaximumPayloadLength
uint8_t emberGetMaximumPayloadLength(EmberMacAddressMode srcAddressMode,
                                     EmberMacAddressMode dstAddressMode,
                                     bool interpan,
                                     bool secured)
{
  emAfPluginCmsisRtosAcquireCommandMutex();
  emAfPluginCmsisRtosSendBlockingCommand(EMBER_GET_MAXIMUM_PAYLOAD_LENGTH_IPC_COMMAND_ID,
                                         "uuuu",
                                         srcAddressMode,
                                         dstAddressMode,
                                         interpan,
                                         secured);

  uint8_t payloadLength;
  emAfPluginCmsisRtosFetchApiParams("u",
                                    &payloadLength);
  emAfPluginCmsisRtosReleaseCommandMutex();
  return payloadLength;
}
static void getMaximumPayloadLengthCommandHandler(void)
{
  EmberMacAddressMode srcAddressMode;
  EmberMacAddressMode dstAddressMode;
  bool interpan;
  bool secured;
  emAfPluginCmsisRtosFetchApiParams("uuuu",
                                    &srcAddressMode,
                                    &dstAddressMode,
                                    &interpan,
                                    &secured);
  uint8_t payloadLength = emApiGetMaximumPayloadLength(srcAddressMode,
                                                       dstAddressMode,
                                                       interpan,
                                                       secured);
  emAfPluginCmsisRtosSendResponse(EMBER_GET_MAXIMUM_PAYLOAD_LENGTH_IPC_COMMAND_ID,
                                  "u",
                                  payloadLength);
}

// setIndirectQueueTimeout
EmberStatus emberSetIndirectQueueTimeout(uint32_t timeoutMs)
{
  emAfPluginCmsisRtosAcquireCommandMutex();
  emAfPluginCmsisRtosSendBlockingCommand(EMBER_SET_INDIRECT_QUEUE_TIMEOUT_IPC_COMMAND_ID,
                                         "w",
                                         timeoutMs);

  EmberStatus status;
  emAfPluginCmsisRtosFetchApiParams("u",
                                    &status);
  emAfPluginCmsisRtosReleaseCommandMutex();
  return status;
}
static void setIndirectQueueTimeoutCommandHandler(void)
{
  uint32_t timeoutMs;
  emAfPluginCmsisRtosFetchApiParams("w",
                                    &timeoutMs);
  EmberStatus status = emApiSetIndirectQueueTimeout(timeoutMs);
  emAfPluginCmsisRtosSendResponse(EMBER_SET_INDIRECT_QUEUE_TIMEOUT_IPC_COMMAND_ID,
                                  "u",
                                  status);
}

// messageSend
EmberStatus emberMessageSend(EmberNodeId destination,
                             uint8_t endpoint,
                             uint8_t messageTag,
                             EmberMessageLength messageLength,
                             uint8_t *message,
                             EmberMessageOptions options)
{
  emAfPluginCmsisRtosAcquireCommandMutex();
  emAfPluginCmsisRtosSendBlockingCommand(EMBER_MESSAGE_SEND_IPC_COMMAND_ID,
                                         "vuuubu",
                                         destination,
                                         endpoint,
                                         messageTag,
                                         messageLength,
                                         message,
                                         messageLength,
                                         options);

  EmberStatus status;
  emAfPluginCmsisRtosFetchApiParams("u",
                                    &status);
  emAfPluginCmsisRtosReleaseCommandMutex();
  return status;
}
static void messageSendCommandHandler(void)
{
  EmberNodeId destination;
  uint8_t endpoint;
  uint8_t messageTag;
  EmberMessageLength messageLength;
  EmberMessageOptions options;
  uint8_t *message;
  emAfPluginCmsisRtosFetchApiParams("vuuupu",
                                    &destination,
                                    &endpoint,
                                    &messageTag,
                                    &messageLength,
                                    &message,
                                    &messageLength,
                                    &options);
  EmberStatus status = emApiMessageSend(destination,
                                        endpoint,
                                        messageTag,
                                        messageLength,
                                        message,
                                        options);
  emAfPluginCmsisRtosSendResponse(EMBER_MESSAGE_SEND_IPC_COMMAND_ID,
                                  "u",
                                  status);
}

// pollForData
EmberStatus emberPollForData(void)
{
  emAfPluginCmsisRtosAcquireCommandMutex();
  emAfPluginCmsisRtosSendBlockingCommand(EMBER_POLL_FOR_DATA_IPC_COMMAND_ID,
                                         "");

  EmberStatus status;
  emAfPluginCmsisRtosFetchApiParams("u",
                                    &status);
  emAfPluginCmsisRtosReleaseCommandMutex();
  return status;
}
static void pollForDataCommandHandler(void)
{
  EmberStatus status = emApiPollForData();
  emAfPluginCmsisRtosSendResponse(EMBER_POLL_FOR_DATA_IPC_COMMAND_ID,
                                  "u",
                                  status);
}

// macMessageSend
EmberStatus emberMacMessageSend(EmberMacFrame *macFrame,
                                uint8_t messageTag,
                                EmberMessageLength messageLength,
                                uint8_t *message,
                                EmberMessageOptions options)
{
  emAfPluginCmsisRtosAcquireCommandMutex();
  emAfPluginCmsisRtosSendBlockingCommand(EMBER_MAC_MESSAGE_SEND_IPC_COMMAND_ID,
                                         "vbuvbuvvuuuubu",
                                         macFrame->srcAddress.addr.shortAddress,
                                         macFrame->srcAddress.addr.longAddress,
                                         EUI64_SIZE,
                                         macFrame->srcAddress.mode,
                                         macFrame->dstAddress.addr.shortAddress,
                                         macFrame->dstAddress.addr.longAddress,
                                         EUI64_SIZE,
                                         macFrame->dstAddress.mode,
                                         macFrame->srcPanId,
                                         macFrame->dstPanId,
                                         macFrame->srcPanIdSpecified,
                                         macFrame->dstPanIdSpecified,
                                         messageTag,
                                         messageLength,
                                         message,
                                         messageLength,
                                         options);

  EmberStatus status;
  emAfPluginCmsisRtosFetchApiParams("u",
                                    &status);
  emAfPluginCmsisRtosReleaseCommandMutex();
  return status;
}
static void macMessageSendCommandHandler(void)
{
  EmberMacFrame macFrame;
  uint8_t messageTag;
  EmberMessageLength messageLength;
  EmberMessageOptions options;
  uint8_t eui64Size = EUI64_SIZE;
  uint8_t *message;
  emAfPluginCmsisRtosFetchApiParams("vbuvbuvvuuuupu",
                                    &macFrame.srcAddress.addr.shortAddress,
                                    &macFrame.srcAddress.addr.longAddress,
                                    &eui64Size,
                                    &macFrame.srcAddress.mode,
                                    &macFrame.dstAddress.addr.shortAddress,
                                    &macFrame.dstAddress.addr.longAddress,
                                    &eui64Size,
                                    &macFrame.dstAddress.mode,
                                    &macFrame.srcPanId,
                                    &macFrame.dstPanId,
                                    &macFrame.srcPanIdSpecified,
                                    &macFrame.dstPanIdSpecified,
                                    &messageTag,
                                    &messageLength,
                                    &message,
                                    &messageLength,
                                    &options);
  EmberStatus status = emApiMacMessageSend(&macFrame,
                                           messageTag,
                                           messageLength,
                                           message,
                                           options);
  emAfPluginCmsisRtosSendResponse(EMBER_MAC_MESSAGE_SEND_IPC_COMMAND_ID,
                                  "u",
                                  status);
}

// macSetPanCoordinator
EmberStatus emberMacSetPanCoordinator(bool isCoordinator)
{
  emAfPluginCmsisRtosAcquireCommandMutex();
  emAfPluginCmsisRtosSendBlockingCommand(EMBER_MAC_SET_PAN_COORDINATOR_IPC_COMMAND_ID,
                                         "u",
                                         isCoordinator);

  EmberStatus status;
  emAfPluginCmsisRtosFetchApiParams("u",
                                    &status);
  emAfPluginCmsisRtosReleaseCommandMutex();
  return status;
}
static void macSetPanCoordinatorCommandHandler(void)
{
  bool isCoordinator;
  emAfPluginCmsisRtosFetchApiParams("u",
                                    &isCoordinator);
  EmberStatus status = emApiMacSetPanCoordinator(isCoordinator);
  emAfPluginCmsisRtosSendResponse(EMBER_MAC_SET_PAN_COORDINATOR_IPC_COMMAND_ID,
                                  "u",
                                  status);
}

// setPollDestinationAddress
EmberStatus emberSetPollDestinationAddress(EmberMacAddress *destination)
{
  emAfPluginCmsisRtosAcquireCommandMutex();
  emAfPluginCmsisRtosSendBlockingCommand(EMBER_SET_POLL_DESTINATION_ADDRESS_IPC_COMMAND_ID,
                                         "vbu",
                                         destination->addr.shortAddress,
                                         destination->addr.longAddress,
                                         EUI64_SIZE,
                                         destination->mode);

  EmberStatus status;
  emAfPluginCmsisRtosFetchApiParams("u",
                                    &status);
  emAfPluginCmsisRtosReleaseCommandMutex();
  return status;
}
static void setPollDestinationAddressCommandHandler(void)
{
  EmberMacAddress destination;
  uint8_t eui64Size = EUI64_SIZE;
  emAfPluginCmsisRtosFetchApiParams("vbu",
                                    &destination.addr.shortAddress,
                                    &destination.addr.longAddress,
                                    &eui64Size,
                                    &destination.mode);
  EmberStatus status = emApiSetPollDestinationAddress(&destination);
  emAfPluginCmsisRtosSendResponse(EMBER_SET_POLL_DESTINATION_ADDRESS_IPC_COMMAND_ID,
                                  "u",
                                  status);
}

// removeChild
EmberStatus emberRemoveChild(EmberMacAddress *address)
{
  emAfPluginCmsisRtosAcquireCommandMutex();
  emAfPluginCmsisRtosSendBlockingCommand(EMBER_REMOVE_CHILD_IPC_COMMAND_ID,
                                         "vbu",
                                         address->addr.shortAddress,
                                         address->addr.longAddress,
                                         EUI64_SIZE,
                                         address->mode);

  EmberStatus status;
  emAfPluginCmsisRtosFetchApiParams("u",
                                    &status);
  emAfPluginCmsisRtosReleaseCommandMutex();
  return status;
}
static void removeChildCommandHandler(void)
{
  EmberMacAddress address;
  uint8_t eui64Size = EUI64_SIZE;
  emAfPluginCmsisRtosFetchApiParams("vbu",
                                    &address.addr.shortAddress,
                                    &address.addr.longAddress,
                                    &eui64Size,
                                    &address.mode);
  EmberStatus status = emApiRemoveChild(&address);
  emAfPluginCmsisRtosSendResponse(EMBER_REMOVE_CHILD_IPC_COMMAND_ID,
                                  "u",
                                  status);
}

// getChildFlags
EmberStatus emberGetChildFlags(EmberMacAddress *address,
                               EmberChildFlags* flags)
{
  emAfPluginCmsisRtosAcquireCommandMutex();
  emAfPluginCmsisRtosSendBlockingCommand(EMBER_GET_CHILD_FLAGS_IPC_COMMAND_ID,
                                         "vbu",
                                         address->addr.shortAddress,
                                         address->addr.longAddress,
                                         EUI64_SIZE,
                                         address->mode);

  EmberStatus status;

  emAfPluginCmsisRtosFetchApiParams("uu",
                                    &status,
                                    flags);
  emAfPluginCmsisRtosReleaseCommandMutex();
  return status;
}
static void getChildFlagsCommandHandler(void)
{
  EmberMacAddress address;
  EmberChildFlags flags;
  uint8_t eui64Size = EUI64_SIZE;
  emAfPluginCmsisRtosFetchApiParams("vbu",
                                    &address.addr.shortAddress,
                                    &address.addr.longAddress,
                                    &eui64Size,
                                    &address.mode);
  EmberStatus status = emApiGetChildFlags(&address,
                                          &flags);
  emAfPluginCmsisRtosSendResponse(EMBER_GET_CHILD_FLAGS_IPC_COMMAND_ID,
                                  "uu",
                                  status,
                                  flags);
}

// getChildInfo
EmberStatus emberGetChildInfo(EmberMacAddress *address,
                              EmberMacAddress *addressResp,
                              EmberChildFlags* flags)
{
  emAfPluginCmsisRtosAcquireCommandMutex();
  emAfPluginCmsisRtosSendBlockingCommand(EMBER_GET_CHILD_INFO_IPC_COMMAND_ID,
                                         "vbu",
                                         address->addr.shortAddress,
                                         address->addr.longAddress,
                                         EUI64_SIZE,
                                         address->mode);

  EmberStatus status;

  uint8_t longAddressSize = EUI64_SIZE;

  emAfPluginCmsisRtosFetchApiParams("uvbuu",
                                    &status,
                                    &addressResp->addr.shortAddress,
                                    addressResp->addr.longAddress,
                                    &longAddressSize,
                                    &addressResp->mode,
                                    flags);
  emAfPluginCmsisRtosReleaseCommandMutex();
  return status;
}
static void getChildInfoCommandHandler(void)
{
  EmberMacAddress address;
  EmberMacAddress addressResp;
  EmberChildFlags flags;
  uint8_t eui64Size = EUI64_SIZE;
  emAfPluginCmsisRtosFetchApiParams("vbu",
                                    &address.addr.shortAddress,
                                    &address.addr.longAddress,
                                    &eui64Size,
                                    &address.mode);
  EmberStatus status = emApiGetChildInfo(&address,
                                         &addressResp,
                                         &flags);
  emAfPluginCmsisRtosSendResponse(EMBER_GET_CHILD_INFO_IPC_COMMAND_ID,
                                  "uvbuu",
                                  status,
                                  addressResp.addr.shortAddress,
                                  addressResp.addr.longAddress,
                                  eui64Size,
                                  address.mode,
                                  flags);
}

// purgeIndirectMessages
EmberStatus emberPurgeIndirectMessages(void)
{
  emAfPluginCmsisRtosAcquireCommandMutex();
  emAfPluginCmsisRtosSendBlockingCommand(EMBER_PURGE_INDIRECT_MESSAGES_IPC_COMMAND_ID,
                                         "");

  EmberStatus status;
  emAfPluginCmsisRtosFetchApiParams("u",
                                    &status);
  emAfPluginCmsisRtosReleaseCommandMutex();
  return status;
}
static void purgeIndirectMessagesCommandHandler(void)
{
  EmberStatus status = emApiPurgeIndirectMessages();
  emAfPluginCmsisRtosSendResponse(EMBER_PURGE_INDIRECT_MESSAGES_IPC_COMMAND_ID,
                                  "u",
                                  status);
}

// macAddShortToLongAddressMapping
EmberStatus emberMacAddShortToLongAddressMapping(EmberNodeId shortId,
                                                 EmberEUI64 longId)
{
  emAfPluginCmsisRtosAcquireCommandMutex();
  emAfPluginCmsisRtosSendBlockingCommand(EMBER_MAC_ADD_SHORT_TO_LONG_ADDRESS_MAPPING_IPC_COMMAND_ID,
                                         "vb",
                                         shortId,
                                         longId, EUI64_SIZE);

  EmberStatus status;
  emAfPluginCmsisRtosFetchApiParams("u",
                                    &status);
  emAfPluginCmsisRtosReleaseCommandMutex();
  return status;
}
static void macAddShortToLongAddressMappingCommandHandler(void)
{
  EmberNodeId shortId;
  uint8_t longIdLength;
  uint8_t longId[EUI64_SIZE];
  emAfPluginCmsisRtosFetchApiParams("vb",
                                    &shortId,
                                    &longId,
                                    &longIdLength);
  EmberStatus status = emApiMacAddShortToLongAddressMapping(shortId,
                                                            longId);
  emAfPluginCmsisRtosSendResponse(EMBER_MAC_ADD_SHORT_TO_LONG_ADDRESS_MAPPING_IPC_COMMAND_ID,
                                  "u",
                                  status);
}

// macClearShortToLongAddressMappings
EmberStatus emberMacClearShortToLongAddressMappings(void)
{
  emAfPluginCmsisRtosAcquireCommandMutex();
  emAfPluginCmsisRtosSendBlockingCommand(EMBER_MAC_CLEAR_SHORT_TO_LONG_ADDRESS_MAPPINGS_IPC_COMMAND_ID,
                                         "");

  EmberStatus status;
  emAfPluginCmsisRtosFetchApiParams("u",
                                    &status);
  emAfPluginCmsisRtosReleaseCommandMutex();
  return status;
}
static void macClearShortToLongAddressMappingsCommandHandler(void)
{
  EmberStatus status = emApiMacClearShortToLongAddressMappings();
  emAfPluginCmsisRtosSendResponse(EMBER_MAC_CLEAR_SHORT_TO_LONG_ADDRESS_MAPPINGS_IPC_COMMAND_ID,
                                  "u",
                                  status);
}

// networkLeave
EmberStatus emberNetworkLeave(void)
{
  emAfPluginCmsisRtosAcquireCommandMutex();
  emAfPluginCmsisRtosSendBlockingCommand(EMBER_NETWORK_LEAVE_IPC_COMMAND_ID,
                                         "");

  EmberStatus status;
  emAfPluginCmsisRtosFetchApiParams("u",
                                    &status);
  emAfPluginCmsisRtosReleaseCommandMutex();
  return status;
}
static void networkLeaveCommandHandler(void)
{
  EmberStatus status = emApiNetworkLeave();
  emAfPluginCmsisRtosSendResponse(EMBER_NETWORK_LEAVE_IPC_COMMAND_ID,
                                  "u",
                                  status);
}

// networkInit
EmberStatus emberNetworkInit(void)
{
  emAfPluginCmsisRtosAcquireCommandMutex();
  emAfPluginCmsisRtosSendBlockingCommand(EMBER_NETWORK_INIT_IPC_COMMAND_ID,
                                         "");

  EmberStatus status;
  emAfPluginCmsisRtosFetchApiParams("u",
                                    &status);
  emAfPluginCmsisRtosReleaseCommandMutex();
  return status;
}
static void networkInitCommandHandler(void)
{
  EmberStatus status = emApiNetworkInit();
  emAfPluginCmsisRtosSendResponse(EMBER_NETWORK_INIT_IPC_COMMAND_ID,
                                  "u",
                                  status);
}

// startActiveScan
EmberStatus emberStartActiveScan(uint16_t channel)
{
  emAfPluginCmsisRtosAcquireCommandMutex();
  emAfPluginCmsisRtosSendBlockingCommand(EMBER_START_ACTIVE_SCAN_IPC_COMMAND_ID,
                                         "v",
                                         channel);

  EmberStatus status;
  emAfPluginCmsisRtosFetchApiParams("u",
                                    &status);
  emAfPluginCmsisRtosReleaseCommandMutex();
  return status;
}
static void startActiveScanCommandHandler(void)
{
  uint16_t channel;
  emAfPluginCmsisRtosFetchApiParams("v",
                                    &channel);
  EmberStatus status = emApiStartActiveScan(channel);
  emAfPluginCmsisRtosSendResponse(EMBER_START_ACTIVE_SCAN_IPC_COMMAND_ID,
                                  "u",
                                  status);
}

// startEnergyScan
EmberStatus emberStartEnergyScan(uint16_t channel,
                                 uint8_t samples)
{
  emAfPluginCmsisRtosAcquireCommandMutex();
  emAfPluginCmsisRtosSendBlockingCommand(EMBER_START_ENERGY_SCAN_IPC_COMMAND_ID,
                                         "vu",
                                         channel,
                                         samples);

  EmberStatus status;
  emAfPluginCmsisRtosFetchApiParams("u",
                                    &status);
  emAfPluginCmsisRtosReleaseCommandMutex();
  return status;
}
static void startEnergyScanCommandHandler(void)
{
  uint16_t channel;
  uint8_t samples;
  emAfPluginCmsisRtosFetchApiParams("vu",
                                    &channel,
                                    &samples);
  EmberStatus status = emApiStartEnergyScan(channel,
                                            samples);
  emAfPluginCmsisRtosSendResponse(EMBER_START_ENERGY_SCAN_IPC_COMMAND_ID,
                                  "u",
                                  status);
}

// setApplicationBeaconPayload
EmberStatus emberSetApplicationBeaconPayload(uint8_t payloadLength,
                                             uint8_t *payload)
{
  emAfPluginCmsisRtosAcquireCommandMutex();
  emAfPluginCmsisRtosSendBlockingCommand(EMBER_SET_APPLICATION_BEACON_PAYLOAD_IPC_COMMAND_ID,
                                         "b",
                                         payload,
                                         payloadLength);

  EmberStatus status;
  emAfPluginCmsisRtosFetchApiParams("u",
                                    &status);
  emAfPluginCmsisRtosReleaseCommandMutex();
  return status;
}
static void setApplicationBeaconPayloadCommandHandler(void)
{
  uint8_t payloadLength;
  uint8_t *payload;
  emAfPluginCmsisRtosFetchApiParams("p",
                                    &payload,
                                    &payloadLength);
  EmberStatus status = emApiSetApplicationBeaconPayload(payloadLength,
                                                        payload);
  emAfPluginCmsisRtosSendResponse(EMBER_SET_APPLICATION_BEACON_PAYLOAD_IPC_COMMAND_ID,
                                  "u",
                                  status);
}

// setSelectiveJoinPayload
EmberStatus emberSetSelectiveJoinPayload(uint8_t payloadLength,
                                         uint8_t *payload)
{
  emAfPluginCmsisRtosAcquireCommandMutex();
  emAfPluginCmsisRtosSendBlockingCommand(EMBER_SET_SELECTIVE_JOIN_PAYLOAD_IPC_COMMAND_ID,
                                         "b",
                                         payload,
                                         payloadLength);

  EmberStatus status;
  emAfPluginCmsisRtosFetchApiParams("u",
                                    &status);
  emAfPluginCmsisRtosReleaseCommandMutex();
  return status;
}
static void setSelectiveJoinPayloadCommandHandler(void)
{
  uint8_t payloadLength;
  uint8_t *payload;
  emAfPluginCmsisRtosFetchApiParams("p",
                                    &payload,
                                    &payloadLength);
  EmberStatus status = emApiSetSelectiveJoinPayload(payloadLength,
                                                    payload);
  emAfPluginCmsisRtosSendResponse(EMBER_SET_SELECTIVE_JOIN_PAYLOAD_IPC_COMMAND_ID,
                                  "u",
                                  status);
}

// clearSelectiveJoinPayload
EmberStatus emberClearSelectiveJoinPayload(void)
{
  emAfPluginCmsisRtosAcquireCommandMutex();
  emAfPluginCmsisRtosSendBlockingCommand(EMBER_CLEAR_SELECTIVE_JOIN_PAYLOAD_IPC_COMMAND_ID,
                                         "");

  EmberStatus status;
  emAfPluginCmsisRtosFetchApiParams("u",
                                    &status);
  emAfPluginCmsisRtosReleaseCommandMutex();
  return status;
}
static void clearSelectiveJoinPayloadCommandHandler(void)
{
  EmberStatus status = emApiClearSelectiveJoinPayload();
  emAfPluginCmsisRtosSendResponse(EMBER_CLEAR_SELECTIVE_JOIN_PAYLOAD_IPC_COMMAND_ID,
                                  "u",
                                  status);
}

// formNetwork
EmberStatus emberFormNetwork(EmberNetworkParameters *parameters)
{
  emAfPluginCmsisRtosAcquireCommandMutex();
  emAfPluginCmsisRtosSendBlockingCommand(EMBER_FORM_NETWORK_IPC_COMMAND_ID,
                                         "vvv",
                                         parameters->panId,
                                         parameters->radioTxPower,
                                         parameters->radioChannel);

  EmberStatus status;
  emAfPluginCmsisRtosFetchApiParams("u",
                                    &status);
  emAfPluginCmsisRtosReleaseCommandMutex();
  return status;
}
static void formNetworkCommandHandler(void)
{
  EmberNetworkParameters parameters;
  emAfPluginCmsisRtosFetchApiParams("vvv",
                                    &parameters.panId,
                                    &parameters.radioTxPower,
                                    &parameters.radioChannel);
  EmberStatus status = emApiFormNetwork(&parameters);
  emAfPluginCmsisRtosSendResponse(EMBER_FORM_NETWORK_IPC_COMMAND_ID,
                                  "u",
                                  status);
}

// joinNetworkExtended
EmberStatus emberJoinNetworkExtended(EmberNodeType nodeType,
                                     EmberNodeId nodeId,
                                     EmberNetworkParameters *parameters)
{
  emAfPluginCmsisRtosAcquireCommandMutex();
  emAfPluginCmsisRtosSendBlockingCommand(EMBER_JOIN_NETWORK_EXTENDED_IPC_COMMAND_ID,
                                         "uvvvv",
                                         nodeType,
                                         nodeId,
                                         parameters->panId,
                                         parameters->radioTxPower,
                                         parameters->radioChannel);

  EmberStatus status;
  emAfPluginCmsisRtosFetchApiParams("u",
                                    &status);
  emAfPluginCmsisRtosReleaseCommandMutex();
  return status;
}
static void joinNetworkExtendedCommandHandler(void)
{
  EmberNodeType nodeType;
  EmberNodeId nodeId;
  EmberNetworkParameters parameters;
  emAfPluginCmsisRtosFetchApiParams("uvvvv",
                                    &nodeType,
                                    &nodeId,
                                    &parameters.panId,
                                    &parameters.radioTxPower,
                                    &parameters.radioChannel);
  EmberStatus status = emApiJoinNetworkExtended(nodeType,
                                                nodeId,
                                                &parameters);
  emAfPluginCmsisRtosSendResponse(EMBER_JOIN_NETWORK_EXTENDED_IPC_COMMAND_ID,
                                  "u",
                                  status);
}

// permitJoining
EmberStatus emberPermitJoining(uint8_t duration)
{
  emAfPluginCmsisRtosAcquireCommandMutex();
  emAfPluginCmsisRtosSendBlockingCommand(EMBER_PERMIT_JOINING_IPC_COMMAND_ID,
                                         "u",
                                         duration);

  EmberStatus status;
  emAfPluginCmsisRtosFetchApiParams("u",
                                    &status);
  emAfPluginCmsisRtosReleaseCommandMutex();
  return status;
}
static void permitJoiningCommandHandler(void)
{
  uint8_t duration;
  emAfPluginCmsisRtosFetchApiParams("u",
                                    &duration);
  EmberStatus status = emApiPermitJoining(duration);
  emAfPluginCmsisRtosSendResponse(EMBER_PERMIT_JOINING_IPC_COMMAND_ID,
                                  "u",
                                  status);
}

// joinCommissioned
EmberStatus emberJoinCommissioned(EmberNodeType nodeType,
                                  EmberNodeId nodeId,
                                  EmberNetworkParameters *parameters)
{
  emAfPluginCmsisRtosAcquireCommandMutex();
  emAfPluginCmsisRtosSendBlockingCommand(EMBER_JOIN_COMMISSIONED_IPC_COMMAND_ID,
                                         "uvvvv",
                                         nodeType,
                                         nodeId,
                                         parameters->panId,
                                         parameters->radioTxPower,
                                         parameters->radioChannel);

  EmberStatus status;
  emAfPluginCmsisRtosFetchApiParams("u",
                                    &status);
  emAfPluginCmsisRtosReleaseCommandMutex();
  return status;
}
static void joinCommissionedCommandHandler(void)
{
  EmberNodeType nodeType;
  EmberNodeId nodeId;
  EmberNetworkParameters parameters;
  emAfPluginCmsisRtosFetchApiParams("uvvvv",
                                    &nodeType,
                                    &nodeId,
                                    &parameters.panId,
                                    &parameters.radioTxPower,
                                    &parameters.radioChannel);
  EmberStatus status = emApiJoinCommissioned(nodeType,
                                             nodeId,
                                             &parameters);
  emAfPluginCmsisRtosSendResponse(EMBER_JOIN_COMMISSIONED_IPC_COMMAND_ID,
                                  "u",
                                  status);
}

// resetNetworkState
void emberResetNetworkState(void)
{
  emAfPluginCmsisRtosAcquireCommandMutex();
  emAfPluginCmsisRtosSendBlockingCommand(EMBER_RESET_NETWORK_STATE_IPC_COMMAND_ID,
                                         "");
  emAfPluginCmsisRtosReleaseCommandMutex();
}
static void resetNetworkStateCommandHandler(void)
{
  emApiResetNetworkState();
  emAfPluginCmsisRtosSendResponse(EMBER_RESET_NETWORK_STATE_IPC_COMMAND_ID,
                                  "");
}

// frequencyHoppingSetChannelMask
EmberStatus emberFrequencyHoppingSetChannelMask(uint8_t channelMaskLength,
                                                uint8_t *channelMask)
{
  emAfPluginCmsisRtosAcquireCommandMutex();
  emAfPluginCmsisRtosSendBlockingCommand(EMBER_FREQUENCY_HOPPING_SET_CHANNEL_MASK_IPC_COMMAND_ID,
                                         "b",
                                         channelMask,
                                         channelMaskLength);

  EmberStatus status;
  emAfPluginCmsisRtosFetchApiParams("u",
                                    &status);
  emAfPluginCmsisRtosReleaseCommandMutex();
  return status;
}
static void frequencyHoppingSetChannelMaskCommandHandler(void)
{
  uint8_t channelMaskLength;
  uint8_t *channelMask;
  emAfPluginCmsisRtosFetchApiParams("p",
                                    &channelMask,
                                    &channelMaskLength);
  EmberStatus status = emApiFrequencyHoppingSetChannelMask(channelMaskLength,
                                                           channelMask);
  emAfPluginCmsisRtosSendResponse(EMBER_FREQUENCY_HOPPING_SET_CHANNEL_MASK_IPC_COMMAND_ID,
                                  "u",
                                  status);
}

// frequencyHoppingStartServer
EmberStatus emberFrequencyHoppingStartServer(void)
{
  emAfPluginCmsisRtosAcquireCommandMutex();
  emAfPluginCmsisRtosSendBlockingCommand(EMBER_FREQUENCY_HOPPING_START_SERVER_IPC_COMMAND_ID,
                                         "");

  EmberStatus status;
  emAfPluginCmsisRtosFetchApiParams("u",
                                    &status);
  emAfPluginCmsisRtosReleaseCommandMutex();
  return status;
}
static void frequencyHoppingStartServerCommandHandler(void)
{
  EmberStatus status = emApiFrequencyHoppingStartServer();
  emAfPluginCmsisRtosSendResponse(EMBER_FREQUENCY_HOPPING_START_SERVER_IPC_COMMAND_ID,
                                  "u",
                                  status);
}

// frequencyHoppingStartClient
EmberStatus emberFrequencyHoppingStartClient(EmberNodeId serverNodeId,
                                             EmberPanId serverPanId)
{
  emAfPluginCmsisRtosAcquireCommandMutex();
  emAfPluginCmsisRtosSendBlockingCommand(EMBER_FREQUENCY_HOPPING_START_CLIENT_IPC_COMMAND_ID,
                                         "vv",
                                         serverNodeId,
                                         serverPanId);

  EmberStatus status;
  emAfPluginCmsisRtosFetchApiParams("u",
                                    &status);
  emAfPluginCmsisRtosReleaseCommandMutex();
  return status;
}
static void frequencyHoppingStartClientCommandHandler(void)
{
  EmberNodeId serverNodeId;
  EmberPanId serverPanId;
  emAfPluginCmsisRtosFetchApiParams("vv",
                                    &serverNodeId,
                                    &serverPanId);
  EmberStatus status = emApiFrequencyHoppingStartClient(serverNodeId,
                                                        serverPanId);
  emAfPluginCmsisRtosSendResponse(EMBER_FREQUENCY_HOPPING_START_CLIENT_IPC_COMMAND_ID,
                                  "u",
                                  status);
}

// frequencyHoppingStop
EmberStatus emberFrequencyHoppingStop(void)
{
  emAfPluginCmsisRtosAcquireCommandMutex();
  emAfPluginCmsisRtosSendBlockingCommand(EMBER_FREQUENCY_HOPPING_STOP_IPC_COMMAND_ID,
                                         "");

  EmberStatus status;
  emAfPluginCmsisRtosFetchApiParams("u",
                                    &status);
  emAfPluginCmsisRtosReleaseCommandMutex();
  return status;
}
static void frequencyHoppingStopCommandHandler(void)
{
  EmberStatus status = emApiFrequencyHoppingStop();
  emAfPluginCmsisRtosSendResponse(EMBER_FREQUENCY_HOPPING_STOP_IPC_COMMAND_ID,
                                  "u",
                                  status);
}

// setAuxiliaryAddressFilteringEntry
EmberStatus emberSetAuxiliaryAddressFilteringEntry(EmberNodeId nodeId,
                                                   uint8_t entryIndex)
{
  emAfPluginCmsisRtosAcquireCommandMutex();
  emAfPluginCmsisRtosSendBlockingCommand(EMBER_SET_AUXILIARY_ADDRESS_FILTERING_ENTRY_IPC_COMMAND_ID,
                                         "vu",
                                         nodeId,
                                         entryIndex);

  EmberStatus status;
  emAfPluginCmsisRtosFetchApiParams("u",
                                    &status);
  emAfPluginCmsisRtosReleaseCommandMutex();
  return status;
}
static void setAuxiliaryAddressFilteringEntryCommandHandler(void)
{
  EmberNodeId nodeId;
  uint8_t entryIndex;
  emAfPluginCmsisRtosFetchApiParams("vu",
                                    &nodeId,
                                    &entryIndex);
  EmberStatus status = emApiSetAuxiliaryAddressFilteringEntry(nodeId,
                                                              entryIndex);
  emAfPluginCmsisRtosSendResponse(EMBER_SET_AUXILIARY_ADDRESS_FILTERING_ENTRY_IPC_COMMAND_ID,
                                  "u",
                                  status);
}

// getAuxiliaryAddressFilteringEntry
EmberNodeId emberGetAuxiliaryAddressFilteringEntry(uint8_t entryIndex)
{
  emAfPluginCmsisRtosAcquireCommandMutex();
  emAfPluginCmsisRtosSendBlockingCommand(EMBER_GET_AUXILIARY_ADDRESS_FILTERING_ENTRY_IPC_COMMAND_ID,
                                         "u",
                                         entryIndex);

  EmberNodeId nodeId;
  emAfPluginCmsisRtosFetchApiParams("v",
                                    &nodeId);
  emAfPluginCmsisRtosReleaseCommandMutex();
  return nodeId;
}
static void getAuxiliaryAddressFilteringEntryCommandHandler(void)
{
  uint8_t entryIndex;
  emAfPluginCmsisRtosFetchApiParams("u",
                                    &entryIndex);
  EmberNodeId nodeId = emApiGetAuxiliaryAddressFilteringEntry(entryIndex);
  emAfPluginCmsisRtosSendResponse(EMBER_GET_AUXILIARY_ADDRESS_FILTERING_ENTRY_IPC_COMMAND_ID,
                                  "v",
                                  nodeId);
}

// startTxStream
EmberStatus emberStartTxStream(EmberTxStreamParameters parameters,
                               uint16_t channel)
{
  emAfPluginCmsisRtosAcquireCommandMutex();
  emAfPluginCmsisRtosSendBlockingCommand(EMBER_START_TX_STREAM_IPC_COMMAND_ID,
                                         "uv",
                                         parameters,
                                         channel);

  EmberStatus status;
  emAfPluginCmsisRtosFetchApiParams("u",
                                    &status);
  emAfPluginCmsisRtosReleaseCommandMutex();
  return status;
}
static void startTxStreamCommandHandler(void)
{
  EmberTxStreamParameters parameters;
  uint16_t channel;
  emAfPluginCmsisRtosFetchApiParams("uv",
                                    &parameters,
                                    &channel);
  EmberStatus status = emApiStartTxStream(parameters,
                                          channel);
  emAfPluginCmsisRtosSendResponse(EMBER_START_TX_STREAM_IPC_COMMAND_ID,
                                  "u",
                                  status);
}

// stopTxStream
EmberStatus emberStopTxStream(void)
{
  emAfPluginCmsisRtosAcquireCommandMutex();
  emAfPluginCmsisRtosSendBlockingCommand(EMBER_STOP_TX_STREAM_IPC_COMMAND_ID,
                                         "");

  EmberStatus status;
  emAfPluginCmsisRtosFetchApiParams("u",
                                    &status);
  emAfPluginCmsisRtosReleaseCommandMutex();
  return status;
}
static void stopTxStreamCommandHandler(void)
{
  EmberStatus status = emApiStopTxStream();
  emAfPluginCmsisRtosSendResponse(EMBER_STOP_TX_STREAM_IPC_COMMAND_ID,
                                  "u",
                                  status);
}

// setActiveScanDuration
EmberStatus emberSetActiveScanDuration(uint16_t durationMs)
{
  emAfPluginCmsisRtosAcquireCommandMutex();
  emAfPluginCmsisRtosSendBlockingCommand(EMBER_SET_ACTIVE_SCAN_DURATION_IPC_COMMAND_ID,
                                         "v",
                                         durationMs);

  EmberStatus status;
  emAfPluginCmsisRtosFetchApiParams("u",
                                    &status);
  emAfPluginCmsisRtosReleaseCommandMutex();
  return status;
}
static void setActiveScanDurationCommandHandler(void)
{
  uint16_t durationMs;
  emAfPluginCmsisRtosFetchApiParams("v",
                                    &durationMs);
  EmberStatus status = emApiSetActiveScanDuration(durationMs);
  emAfPluginCmsisRtosSendResponse(EMBER_SET_ACTIVE_SCAN_DURATION_IPC_COMMAND_ID,
                                  "u",
                                  status);
}

// getActiveScanDuration
uint16_t emberGetActiveScanDuration(void)
{
  emAfPluginCmsisRtosAcquireCommandMutex();
  emAfPluginCmsisRtosSendBlockingCommand(EMBER_GET_ACTIVE_SCAN_DURATION_IPC_COMMAND_ID,
                                         "");

  uint16_t durationMs;
  emAfPluginCmsisRtosFetchApiParams("v",
                                    &durationMs);
  emAfPluginCmsisRtosReleaseCommandMutex();
  return durationMs;
}
static void getActiveScanDurationCommandHandler(void)
{
  uint16_t durationMs = emApiGetActiveScanDuration();
  emAfPluginCmsisRtosSendResponse(EMBER_GET_ACTIVE_SCAN_DURATION_IPC_COMMAND_ID,
                                  "v",
                                  durationMs);
}

// getDefaultChannel
uint16_t emberGetDefaultChannel(void)
{
  emAfPluginCmsisRtosAcquireCommandMutex();
  emAfPluginCmsisRtosSendBlockingCommand(EMBER_GET_DEFAULT_CHANNEL_IPC_COMMAND_ID,
                                         "");

  uint16_t firstChannel;
  emAfPluginCmsisRtosFetchApiParams("v",
                                    &firstChannel);
  emAfPluginCmsisRtosReleaseCommandMutex();
  return firstChannel;
}
static void getDefaultChannelCommandHandler(void)
{
  uint16_t firstChannel = emApiGetDefaultChannel();
  emAfPluginCmsisRtosSendResponse(EMBER_GET_DEFAULT_CHANNEL_IPC_COMMAND_ID,
                                  "v",
                                  firstChannel);
}

//------------------------------------------------------------------------------
// Callback command dispatcher (Application side)

void emAfPluginCmsisRtosHandleIncomingApiCommand(uint16_t commandId)
{
  assert(emAfPluginCmsisRtosIsCurrentTaskStackTask());

  switch (commandId) {
    case EMBER_NETWORK_STATE_IPC_COMMAND_ID:
      networkStateCommandHandler();
      break;
    case EMBER_STACK_IS_UP_IPC_COMMAND_ID:
      stackIsUpCommandHandler();
      break;
#ifdef SL_CATALOG_CONNECT_AES_SECURITY_PRESENT
    case EMBER_SET_SECURITY_KEY_IPC_COMMAND_ID:
      setSecurityKeyCommandHandler();
      break;
    case EMBER_GET_SECURITY_KEY_IPC_COMMAND_ID:
      GetSecurityKeyCommandHandler();
      break;
    case EMBER_SET_PSA_SECURITY_KEY_IPC_COMMAND_ID:
      setPsaSecurityKeyCommandHandler();
      break;
    case EMBER_REMOVE_PSA_SECURITY_KEY_IPC_COMMAND_ID:
      RemovePsaSecurityKeyCommandHandler();
      break;
#endif
    case EMBER_GET_COUNTER_IPC_COMMAND_ID:
      getCounterCommandHandler();
      break;
    case EMBER_SET_RADIO_CHANNEL_EXTENDED_IPC_COMMAND_ID:
      setRadioChannelExtendedCommandHandler();
      break;
    case EMBER_GET_RADIO_CHANNEL_IPC_COMMAND_ID:
      getRadioChannelCommandHandler();
      break;
    case EMBER_SET_RADIO_POWER_IPC_COMMAND_ID:
      setRadioPowerCommandHandler();
      break;
    case EMBER_GET_RADIO_POWER_IPC_COMMAND_ID:
      getRadioPowerCommandHandler();
      break;
    case EMBER_SET_RADIO_POWER_MODE_IPC_COMMAND_ID:
      setRadioPowerModeCommandHandler();
      break;
    case EMBER_SET_MAC_PARAMS_IPC_COMMAND_ID:
      setMacParamsCommandHandler();
      break;
    case EMBER_CURRENT_STACK_TASKS_IPC_COMMAND_ID:
      currentStackTasksCommandHandler();
      break;
    case EMBER_MAC_GET_PARENT_ADDRESS_IPC_COMMAND_ID:
      macGetParentAddressCommandHandler();
      break;
    case EMBER_CALIBRATE_CURRENT_CHANNEL_EXTENDED_IPC_COMMAND_ID:
      calibrateCurrentChannelExtendedCommandHandler();
      break;
    case EMBER_APPLY_IR_CALIBRATION_IPC_COMMAND_ID:
      applyIrCalibrationCommandHandler();
      break;
    case EMBER_TEMP_CALIBRATION_IPC_COMMAND_ID:
      tempCalibrationCommandHandler();
      break;
    case EMBER_GET_CAL_TYPE_IPC_COMMAND_ID:
      getCalTypeCommandHandler();
      break;
    case EMBER_GET_MAXIMUM_PAYLOAD_LENGTH_IPC_COMMAND_ID:
      getMaximumPayloadLengthCommandHandler();
      break;
    case EMBER_SET_INDIRECT_QUEUE_TIMEOUT_IPC_COMMAND_ID:
      setIndirectQueueTimeoutCommandHandler();
      break;
    case EMBER_MESSAGE_SEND_IPC_COMMAND_ID:
      messageSendCommandHandler();
      break;
    case EMBER_POLL_FOR_DATA_IPC_COMMAND_ID:
      pollForDataCommandHandler();
      break;
    case EMBER_MAC_MESSAGE_SEND_IPC_COMMAND_ID:
      macMessageSendCommandHandler();
      break;
    case EMBER_MAC_SET_PAN_COORDINATOR_IPC_COMMAND_ID:
      macSetPanCoordinatorCommandHandler();
      break;
    case EMBER_SET_POLL_DESTINATION_ADDRESS_IPC_COMMAND_ID:
      setPollDestinationAddressCommandHandler();
      break;
    case EMBER_REMOVE_CHILD_IPC_COMMAND_ID:
      removeChildCommandHandler();
      break;
    case EMBER_GET_CHILD_FLAGS_IPC_COMMAND_ID:
      getChildFlagsCommandHandler();
      break;
    case EMBER_GET_CHILD_INFO_IPC_COMMAND_ID:
      getChildInfoCommandHandler();
      break;
    case EMBER_PURGE_INDIRECT_MESSAGES_IPC_COMMAND_ID:
      purgeIndirectMessagesCommandHandler();
      break;
    case EMBER_MAC_ADD_SHORT_TO_LONG_ADDRESS_MAPPING_IPC_COMMAND_ID:
      macAddShortToLongAddressMappingCommandHandler();
      break;
    case EMBER_MAC_CLEAR_SHORT_TO_LONG_ADDRESS_MAPPINGS_IPC_COMMAND_ID:
      macClearShortToLongAddressMappingsCommandHandler();
      break;
    case EMBER_NETWORK_LEAVE_IPC_COMMAND_ID:
      networkLeaveCommandHandler();
      break;
    case EMBER_NETWORK_INIT_IPC_COMMAND_ID:
      networkInitCommandHandler();
      break;
    case EMBER_START_ACTIVE_SCAN_IPC_COMMAND_ID:
      startActiveScanCommandHandler();
      break;
    case EMBER_START_ENERGY_SCAN_IPC_COMMAND_ID:
      startEnergyScanCommandHandler();
      break;
    case EMBER_SET_APPLICATION_BEACON_PAYLOAD_IPC_COMMAND_ID:
      setApplicationBeaconPayloadCommandHandler();
      break;
    case EMBER_SET_SELECTIVE_JOIN_PAYLOAD_IPC_COMMAND_ID:
      setSelectiveJoinPayloadCommandHandler();
      break;
    case EMBER_CLEAR_SELECTIVE_JOIN_PAYLOAD_IPC_COMMAND_ID:
      clearSelectiveJoinPayloadCommandHandler();
      break;
    case EMBER_FORM_NETWORK_IPC_COMMAND_ID:
      formNetworkCommandHandler();
      break;
    case EMBER_JOIN_NETWORK_EXTENDED_IPC_COMMAND_ID:
      joinNetworkExtendedCommandHandler();
      break;
    case EMBER_PERMIT_JOINING_IPC_COMMAND_ID:
      permitJoiningCommandHandler();
      break;
    case EMBER_JOIN_COMMISSIONED_IPC_COMMAND_ID:
      joinCommissionedCommandHandler();
      break;
    case EMBER_RESET_NETWORK_STATE_IPC_COMMAND_ID:
      resetNetworkStateCommandHandler();
      break;
    case EMBER_FREQUENCY_HOPPING_SET_CHANNEL_MASK_IPC_COMMAND_ID:
      frequencyHoppingSetChannelMaskCommandHandler();
      break;
    case EMBER_FREQUENCY_HOPPING_START_SERVER_IPC_COMMAND_ID:
      frequencyHoppingStartServerCommandHandler();
      break;
    case EMBER_FREQUENCY_HOPPING_START_CLIENT_IPC_COMMAND_ID:
      frequencyHoppingStartClientCommandHandler();
      break;
    case EMBER_FREQUENCY_HOPPING_STOP_IPC_COMMAND_ID:
      frequencyHoppingStopCommandHandler();
      break;
    case EMBER_SET_AUXILIARY_ADDRESS_FILTERING_ENTRY_IPC_COMMAND_ID:
      setAuxiliaryAddressFilteringEntryCommandHandler();
      break;
    case EMBER_GET_AUXILIARY_ADDRESS_FILTERING_ENTRY_IPC_COMMAND_ID:
      getAuxiliaryAddressFilteringEntryCommandHandler();
      break;
    case EMBER_START_TX_STREAM_IPC_COMMAND_ID:
      startTxStreamCommandHandler();
      break;
    case EMBER_STOP_TX_STREAM_IPC_COMMAND_ID:
      stopTxStreamCommandHandler();
      break;
    case EMBER_SET_ACTIVE_SCAN_DURATION_IPC_COMMAND_ID:
      setActiveScanDurationCommandHandler();
      break;
    case EMBER_GET_ACTIVE_SCAN_DURATION_IPC_COMMAND_ID:
      getActiveScanDurationCommandHandler();
      break;
    case EMBER_GET_DEFAULT_CHANNEL_IPC_COMMAND_ID:
      getDefaultChannelCommandHandler();
      break;
    default: {
      assert(0);
    }
  }
}
