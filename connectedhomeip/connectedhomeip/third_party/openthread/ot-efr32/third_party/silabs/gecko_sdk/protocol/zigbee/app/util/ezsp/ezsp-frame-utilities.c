/***************************************************************************//**
 * @file
 * @brief Functions for reading and writing command and response frames.
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

#include PLATFORM_HEADER

#include <stdarg.h>
#include "stack/include/ember-types.h"
#include "ezsp-protocol.h"
#include "ezsp-frame-utilities.h"

uint8_t* ezspReadPointer;
uint8_t* ezspWritePointer;

uint8_t fetchInt8u(void)
{
  return *ezspReadPointer++;
}

void fetchInt8uArray(uint8_t length, uint8_t *contents)
{
  MEMMOVE(contents, ezspReadPointer, length);
  ezspReadPointer += length;
}

uint8_t *fetchInt8uPointer(uint8_t length)
{
  uint8_t *result = ezspReadPointer;
  ezspReadPointer += length;
  return result;
}

void appendInt8u(uint8_t value)
{
  if (ezspWritePointer - ezspFrameContents < EZSP_MAX_FRAME_LENGTH) {
    *ezspWritePointer = value;
  }
  ezspWritePointer++;
}

void appendInt16u(uint16_t value)
{
  appendInt8u(LOW_BYTE(value));
  appendInt8u(HIGH_BYTE(value));
}

uint16_t fetchInt16u(void)
{
  uint8_t low = fetchInt8u();
  uint8_t high = fetchInt8u();
  return HIGH_LOW_TO_INT(high, low);
}

void appendInt32u(uint32_t value)
{
  appendInt16u((uint16_t)(value & 0xFFFF));
  appendInt16u((uint16_t)(value >> 16 & 0xFFFF));
}

uint32_t fetchInt32u(void)
{
  uint16_t low = fetchInt16u();
  return (((uint32_t) fetchInt16u()) << 16) + low;
}

void appendInt8uArray(uint8_t length, uint8_t *contents)
{
  if (ezspWritePointer - ezspFrameContents + length <= EZSP_MAX_FRAME_LENGTH) {
    MEMMOVE(ezspWritePointer, contents, length);
  }
  ezspWritePointer += length;
}

void appendInt16uArray(uint8_t length, uint16_t *contents)
{
  uint8_t i;
  for (i = 0; i < length; i++) {
    appendInt16u(contents[i]);
  }
}

void fetchInt16uArray(uint8_t length, uint16_t *contents)
{
  uint8_t i;
  for (i = 0; i < length; i++) {
    contents[i] = fetchInt16u();
  }
}

void fetchEmberNetworkParameters(EmberNetworkParameters *value)
{
  fetchInt8uArray(EXTENDED_PAN_ID_SIZE, value->extendedPanId);
  value->panId = fetchInt16u();
  value->radioTxPower = fetchInt8u();
  value->radioChannel = fetchInt8u();
  value->joinMethod = fetchInt8u();
  value->nwkManagerId = fetchInt16u();
  value->nwkUpdateId = fetchInt8u();
  value->channels = fetchInt32u();
}

void fetchEmberMultiPhyRadioParameters(EmberMultiPhyRadioParameters *parameters)
{
  parameters->radioTxPower = fetchInt8u();
  parameters->radioPage = fetchInt8u();
  parameters->radioChannel =  fetchInt8u();
}

void appendEmberApsFrame(EmberApsFrame *value)
{
  appendInt16u(value->profileId);
  appendInt16u(value->clusterId);
  appendInt8u(value->sourceEndpoint);
  appendInt8u(value->destinationEndpoint);
  appendInt16u(value->options);
  appendInt16u(value->groupId);
  appendInt8u(value->sequence);
}

void fetchEmberApsFrame(EmberApsFrame *value)
{
  value->profileId = fetchInt16u();
  value->clusterId = fetchInt16u();
  value->sourceEndpoint = fetchInt8u();
  value->destinationEndpoint = fetchInt8u();
  value->options = fetchInt16u();
  value->groupId = fetchInt16u();
  value->sequence = fetchInt8u();
}

void appendEmberMulticastTableEntry(EmberMulticastTableEntry *value)
{
  appendInt16u(value->multicastId);
  appendInt8u(value->endpoint);
}

void fetchEmberMulticastTableEntry(EmberMulticastTableEntry *value)
{
  value->multicastId = fetchInt16u();
  value->endpoint    = fetchInt8u();
}

void fetchEmberBeaconClassificationParams(EmberBeaconClassificationParams *value)
{
  value->minRssiForReceivingPkts  = fetchInt8u();  //there is no fetchInt8(), is it ok?
  value->beaconClassificationMask = fetchInt16u();
}

void appendEmberBeaconClassificationParams(EmberBeaconClassificationParams *value)
{
  appendInt8u(value->minRssiForReceivingPkts);
  appendInt16u(value->beaconClassificationMask);
}

void fetchEmberNeighborTableEntry(EmberNeighborTableEntry *value)
{
  value->shortId = fetchInt16u();
  value->averageLqi = fetchInt8u();
  value->inCost = fetchInt8u();
  value->outCost = fetchInt8u();
  value->age = fetchInt8u();
  fetchInt8uArray(EUI64_SIZE, value->longId);
}

void fetchEmberRouteTableEntry(EmberRouteTableEntry *value)
{
  value->destination = fetchInt16u();
  value->nextHop = fetchInt16u();
  value->status = fetchInt8u();
  value->age = fetchInt8u();
  value->concentratorType = fetchInt8u();
  value->routeRecordState = fetchInt8u();
}

void appendEmberInitialSecurityState(EmberInitialSecurityState *value)
{
  appendInt16u(value->bitmask);
  appendEmberKeyData(&(value->preconfiguredKey));
  appendEmberKeyData(&(value->networkKey));
  appendInt8u(value->networkKeySequenceNumber);
  appendInt8uArray(EUI64_SIZE, value->preconfiguredTrustCenterEui64);
}

void fetchEmberCurrentSecurityState(EmberCurrentSecurityState *value)
{
  value->bitmask = fetchInt16u();
  fetchInt8uArray(EUI64_SIZE, value->trustCenterLongAddress);
}

void fetchEmberKeyStruct(EmberKeyStruct *value)
{
  value->bitmask = fetchInt16u();
  value->type = fetchInt8u();
  fetchEmberKeyData(&(value->key));
  value->outgoingFrameCounter = fetchInt32u();
  value->incomingFrameCounter = fetchInt32u();
  value->sequenceNumber = fetchInt8u();
  fetchInt8uArray(EUI64_SIZE, value->partnerEUI64);
}

void fetchEmberChildData(EmberChildData *value)
{
  fetchInt8uArray(EUI64_SIZE, value->eui64);
  value->type = fetchInt8u();
  value->id = fetchInt16u();
  value->phy = fetchInt8u();
  value->power = fetchInt8u();
  value->timeout = fetchInt8u();
}

void fetchEmberZigbeeNetwork(EmberZigbeeNetwork *value)
{
  value->channel = fetchInt8u();
  value->panId = fetchInt16u();
  fetchInt8uArray(EXTENDED_PAN_ID_SIZE, value->extendedPanId);
  value->allowingJoin = fetchInt8u();
  value->stackProfile = fetchInt8u();
  value->nwkUpdateId = fetchInt8u();
}

void appendEmberZigbeeNetwork(EmberZigbeeNetwork *value)
{
  appendInt8u(value->channel);
  appendInt16u(value->panId);
  appendInt8uArray(EXTENDED_PAN_ID_SIZE, value->extendedPanId);
  appendInt8u(value->allowingJoin);
  appendInt8u(value->stackProfile);
  appendInt8u(value->nwkUpdateId);
}

void appendEmberAesMmoHashContext(EmberAesMmoHashContext* context)
{
  appendInt8uArray(EMBER_AES_HASH_BLOCK_SIZE, context->result);
  appendInt32u(context->length);
}

void fetchEmberAesMmoHashContext(EmberAesMmoHashContext* context)
{
  fetchInt8uArray(EMBER_AES_HASH_BLOCK_SIZE, context->result);
  context->length = fetchInt32u();
}

void appendEmberNetworkInitStruct(const EmberNetworkInitStruct* networkInitStruct)
{
  appendInt16u(networkInitStruct->bitmask);
}

void fetchEmberNetworkInitStruct(EmberNetworkInitStruct* networkInitStruct)
{
  networkInitStruct->bitmask = fetchInt16u();
}

void appendEmberVersionStruct(const EmberVersion* versionStruct)
{
  appendInt16u(versionStruct->build);
  appendInt8u(versionStruct->major);
  appendInt8u(versionStruct->minor);
  appendInt8u(versionStruct->patch);
  appendInt8u(versionStruct->special);
  appendInt8u(versionStruct->type);
}

void fetchEmberVersionStruct(EmberVersion* versionStruct)
{
  versionStruct->build = fetchInt16u();
  versionStruct->major = fetchInt8u();
  versionStruct->minor = fetchInt8u();
  versionStruct->special = fetchInt8u();
  versionStruct->type  = fetchInt8u();
}

void appendEmberZllNetwork(EmberZllNetwork *network)
{
  appendEmberZigbeeNetwork(&(network->zigbeeNetwork));
  appendEmberZllSecurityAlgorithmData(&(network->securityAlgorithm));
  appendInt8uArray(EUI64_SIZE, network->eui64);
  appendInt16u(network->nodeId);
  appendInt16u(network->state);
  appendInt8u(network->nodeType);
  appendInt8u(network->numberSubDevices);
  appendInt8u(network->totalGroupIdentifiers);
  appendInt8u(network->rssiCorrection);
}

void fetchEmberZllNetwork(EmberZllNetwork* network)
{
  fetchEmberZigbeeNetwork(&(network->zigbeeNetwork));
  fetchEmberZllSecurityAlgorithmData(&(network->securityAlgorithm));
  fetchInt8uArray(EUI64_SIZE, network->eui64);
  network->nodeId = fetchInt16u();
  network->state = fetchInt16u();
  network->nodeType = fetchInt8u();
  network->numberSubDevices = fetchInt8u();
  network->totalGroupIdentifiers = fetchInt8u();
  network->rssiCorrection = fetchInt8u();
}

void appendEmberZllSecurityAlgorithmData(EmberZllSecurityAlgorithmData* data)
{
  appendInt32u(data->transactionId);
  appendInt32u(data->responseId);
  appendInt16u(data->bitmask);
}

void fetchEmberZllSecurityAlgorithmData(EmberZllSecurityAlgorithmData* data)
{
  data->transactionId = fetchInt32u();
  data->responseId    = fetchInt32u();
  data->bitmask       = fetchInt16u();
}

void appendEmberZllInitialSecurityState(EmberZllInitialSecurityState* state)
{
  appendInt32u(state->bitmask);
  appendInt8u(state->keyIndex);
  appendEmberKeyData(&((state)->encryptionKey));
  appendEmberKeyData(&((state)->preconfiguredKey));
}

void appendEmberTokTypeStackZllData(EmberTokTypeStackZllData *data)
{
  appendInt32u(data->bitmask);
  appendInt16u(data->freeNodeIdMin);
  appendInt16u(data->freeNodeIdMax);
  appendInt16u(data->myGroupIdMin);
  appendInt16u(data->freeGroupIdMin);
  appendInt16u(data->freeGroupIdMax);
  appendInt8u(data->rssiCorrection);
}

void fetchEmberTokTypeStackZllData(EmberTokTypeStackZllData *data)
{
  data->bitmask = fetchInt32u();
  data->freeNodeIdMin = fetchInt16u();
  data->freeNodeIdMax = fetchInt16u();
  data->myGroupIdMin = fetchInt16u();
  data->freeGroupIdMin = fetchInt16u();
  data->freeGroupIdMax = fetchInt16u();
  data->rssiCorrection = fetchInt8u();
}

void appendEmberTokTypeStackZllSecurity(EmberTokTypeStackZllSecurity *security)
{
  appendInt32u(security->bitmask);
  appendInt8u(security->keyIndex);
  appendInt8uArray(16, security->encryptionKey);
  appendInt8uArray(16, security->preconfiguredKey);
}

void fetchEmberTokTypeStackZllSecurity(EmberTokTypeStackZllSecurity *security)
{
  security->bitmask = fetchInt32u();
  security->keyIndex = fetchInt8u();
  fetchInt8uArray(EMBER_ENCRYPTION_KEY_SIZE, security->encryptionKey);
  fetchInt8uArray(EMBER_ENCRYPTION_KEY_SIZE, security->preconfiguredKey);
}

void appendEmberGpAddress(EmberGpAddress *value)
{
  appendInt8u(value->applicationId);
  if (value->applicationId == 0) {
    appendInt32u(value->id.sourceId);
    appendInt32u(value->id.sourceId); //filler
  } else if (value->applicationId == 2 ) {
    appendInt8uArray(8, value->id.gpdIeeeAddress);
  }
  appendInt8u(value->endpoint);
}

void fetchEmberGpAddress(EmberGpAddress *value)
{
  value->applicationId = fetchInt8u();
  if (value->applicationId == 0) {
    value->id.sourceId = fetchInt32u();
    value->id.sourceId = fetchInt32u();
  } else if (value->applicationId == 2) {
    fetchInt8uArray(EUI64_SIZE, value->id.gpdIeeeAddress);
  }
  value->endpoint = fetchInt8u();
}

void fetchEmberGpSinkList(EmberGpSinkListEntry *value)
{
  for (uint8_t i = 0; i < 2; i++) {
    value->type = (EmberGpSinkType) fetchInt8u();

    switch (value->type) {
      case EMBER_GP_SINK_TYPE_FULL_UNICAST:
      case EMBER_GP_SINK_TYPE_LW_UNICAST:
      case EMBER_GP_SINK_TYPE_UNUSED:
      default:
        value->target.unicast.sinkNodeId = fetchInt16u();
        fetchInt8uArray(EUI64_SIZE, value->target.unicast.sinkEUI);
        break;

      case EMBER_GP_SINK_TYPE_D_GROUPCAST:
      case EMBER_GP_SINK_TYPE_GROUPCAST:
        value->target.groupcast.alias = fetchInt16u();
        value->target.groupcast.groupID = fetchInt16u();
        fetchInt16u(); //filler
        fetchInt16u();
        fetchInt16u();
        break;
    }

    value++;
  }
}

void appendEmberGpSinkList(EmberGpSinkListEntry *value)
{
  for (uint8_t i = 0; i < 2; i++) {
    appendInt8u(value->type);
    switch (value->type) {
      case EMBER_GP_SINK_TYPE_FULL_UNICAST:
      case EMBER_GP_SINK_TYPE_LW_UNICAST:
      case EMBER_GP_SINK_TYPE_UNUSED:
      default:
        appendInt16u(value->target.unicast.sinkNodeId);
        appendInt8uArray(8, value->target.unicast.sinkEUI);

        break;

      case EMBER_GP_SINK_TYPE_D_GROUPCAST:
      case EMBER_GP_SINK_TYPE_GROUPCAST:
        appendInt16u(value->target.groupcast.alias);
        appendInt16u(value->target.groupcast.groupID);
        //filler
        appendInt16u(value->target.groupcast.alias);
        appendInt16u(value->target.groupcast.groupID);
        appendInt16u(value->target.groupcast.alias);
        break;
    }

    value++;
  }
}

void fetchEmberGpProxyTableEntry(EmberGpProxyTableEntry *value)
{
  value->status = fetchInt8u();
  value->options = fetchInt32u();
  fetchEmberGpAddress(&(value->gpd));
  value->assignedAlias = fetchInt16u();
  value->securityOptions  = fetchInt8u();
  value->gpdSecurityFrameCounter = fetchInt32u();
  fetchEmberKeyData(&(value->gpdKey));
  fetchEmberGpSinkList(value->sinkList);
  value->groupcastRadius = fetchInt8u();
  value->searchCounter = fetchInt8u();
}
void appendEmberGpProxyTableEntry(EmberGpProxyTableEntry *value)
{
  appendInt8u(value->status);
  appendInt32u(value->options);
  appendEmberGpAddress(&(value->gpd));
  appendInt16u(value->assignedAlias);
  appendInt8u(value->securityOptions);
  appendInt32u(value->gpdSecurityFrameCounter);
  appendEmberKeyData(&(value->gpdKey));
  appendEmberGpSinkList(value->sinkList);
  appendInt8u(value->groupcastRadius);
  appendInt8u(value->searchCounter);
}

void fetchEmberGpSinkTableEntry(EmberGpSinkTableEntry *value)
{
  value->status = fetchInt8u();
  value->options = fetchInt16u();
  fetchEmberGpAddress(&(value->gpd));
  value->deviceId = fetchInt8u();
  fetchEmberGpSinkList(value->sinkList);
  value->assignedAlias = fetchInt16u();
  value->groupcastRadius = fetchInt8u();
  value->securityOptions  = fetchInt8u();
  value->gpdSecurityFrameCounter = fetchInt32u();
  fetchEmberKeyData(&(value->gpdKey));
}

void appendEmberGpSinkTableEntry(EmberGpSinkTableEntry *value)
{
  appendInt8u(value->status);
  appendInt16u(value->options);
  appendEmberGpAddress(&(value->gpd));
  appendInt8u(value->deviceId);
  appendEmberGpSinkList(value->sinkList);
  appendInt16u(value->assignedAlias);
  appendInt8u(value->groupcastRadius);
  appendInt8u(value->securityOptions);
  appendInt32u(value->gpdSecurityFrameCounter);
  appendEmberKeyData(&(value->gpdKey));
}

void appendEmberDutyCycleLimits(EmberDutyCycleLimits* limits)
{
  appendInt16u(limits->limitThresh);
  appendInt16u(limits->critThresh);
  appendInt16u(limits->suspLimit);
}

void fetchEmberDutyCycleLimits(EmberDutyCycleLimits* limits)
{
  limits->limitThresh = fetchInt16u();
  limits->critThresh = fetchInt16u();
  limits->suspLimit = fetchInt16u();
}

void appendEmberPerDeviceDutyCycle(uint8_t maxDevices,
                                   EmberPerDeviceDutyCycle* arrayOfDeviceDutyCycles)
{
  uint8_t i;

  appendInt16u(maxDevices);
  for (i = 0; i < maxDevices; i++) {
    appendInt16u(arrayOfDeviceDutyCycles[i].nodeId);
    appendInt16u(arrayOfDeviceDutyCycles[i].dutyCycleConsumed);
  }
}

void fetchEmberPerDeviceDutyCycle(EmberPerDeviceDutyCycle* arrayOfDeviceDutyCycles)
{
  uint8_t i;
  uint8_t maxDevices;

  maxDevices = fetchInt8u();
  for (i = 0; i < maxDevices; i++) {
    arrayOfDeviceDutyCycles[i].nodeId = fetchInt16u();
    arrayOfDeviceDutyCycles[i].dutyCycleConsumed = (EmberDutyCycleHectoPct) fetchInt16u();
  }
}

void formatBytesInToStructEmberPerDeviceDutyCycle(uint8_t* perDeviceDutyCycles,
                                                  EmberPerDeviceDutyCycle* arrayOfDeviceDutyCycles)
{
  uint8_t i;
  uint8_t maxDevices;

  maxDevices = perDeviceDutyCycles[0];
  perDeviceDutyCycles++;

  for (i = 0; i < maxDevices; i++) {
    arrayOfDeviceDutyCycles[i].nodeId = HIGH_LOW_TO_INT(perDeviceDutyCycles[1],
                                                        perDeviceDutyCycles[0]);
    arrayOfDeviceDutyCycles[i].dutyCycleConsumed = HIGH_LOW_TO_INT(perDeviceDutyCycles[3],
                                                                   perDeviceDutyCycles[2]);
    perDeviceDutyCycles = perDeviceDutyCycles + 4;
  }
}

void formatStructEmberPerDeviceDutyCycleIntoBytes(uint8_t maxDevices,
                                                  EmberPerDeviceDutyCycle *arrayOfDeviceDutyCycles,
                                                  uint8_t* perDeviceDutyCycles)
{
  uint8_t i;

  perDeviceDutyCycles[0] = maxDevices;
  perDeviceDutyCycles++;

  for (i = 0; i < maxDevices; i++) {
    perDeviceDutyCycles[0]  = LOW_BYTE(arrayOfDeviceDutyCycles[i].nodeId);
    perDeviceDutyCycles[1]  = HIGH_BYTE(arrayOfDeviceDutyCycles[i].nodeId);
    perDeviceDutyCycles[2]  = LOW_BYTE(arrayOfDeviceDutyCycles[i].dutyCycleConsumed);
    perDeviceDutyCycles[3]  = HIGH_BYTE(arrayOfDeviceDutyCycles[i].dutyCycleConsumed);

    perDeviceDutyCycles = perDeviceDutyCycles + 4;
  }
}

void emAfFetchOrAppend(bool fetch, const char * format, ...)
{
  char *c = (char *)format;
  uintptr_t ptr;
  va_list args;
  va_start(args, format);

  while (*c) {
    ptr = va_arg(args, uintptr_t);
    switch (*c) {
      case 's': {
        int8_t *realPointer = (int8_t *)ptr;
        if (fetch) {
          *realPointer = (int8_t)fetchInt8u();
        } else {
          appendInt8u((uint8_t)*realPointer);
        }
      }
      break;
      case 'u': {
        uint8_t *realPointer = (uint8_t *)ptr;
        if (fetch) {
          *realPointer = fetchInt8u();
        } else {
          appendInt8u(*realPointer);
        }
      }
      break;
      case 'v': {
        uint16_t *realPointer = (uint16_t *)ptr;
        if (fetch) {
          *realPointer = fetchInt16u();
        } else {
          appendInt16u(*realPointer);
        }
      }
      break;
      case 'w': {
        uint32_t *realPointer = (uint32_t *)ptr;
        if (fetch) {
          *realPointer = fetchInt32u();
        } else {
          appendInt32u((uint32_t)*realPointer);
        }
      }
      break;
      case 'b': {
        uint8_t *realArray = (uint8_t *)ptr;
        uint8_t *lengthPointer = (uint8_t *)va_arg(args, uintptr_t);
        if (fetch) {
          fetchInt8uArray(*lengthPointer, realArray);
        } else {
          appendInt8uArray(*lengthPointer, realArray);
        }
      }
      break;
      case 'p': {
        uint8_t **realPointer = (uint8_t **)ptr;
        uint8_t *lengthPointer = (uint8_t *)va_arg(args, uintptr_t);
        if (fetch) {
          *realPointer = fetchInt8uPointer(*lengthPointer);
        } else {
          ; // cannot append a pointer
        }
      }
      break;
      default:
        ; // meh
    }
    c++;
  }

  va_end(args);
}

void appendEmberTokenData(EmberTokenData * tokenData)
{
  appendInt32u(tokenData->size);
  appendInt8uArray((uint8_t)tokenData->size, (uint8_t *)tokenData->data);
}

void fetchEmberTokenData(EmberTokenData * tokenData)
{
  tokenData->size = fetchInt32u();
  fetchInt8uArray((uint8_t)tokenData->size, (uint8_t *)tokenData->data);
}

void fetchEmberTokenInfo(EmberTokenInfo *tokenInfo)
{
  tokenInfo->nvm3Key = fetchInt32u();
  tokenInfo->isCnt = fetchInt8u();
  tokenInfo->isIdx = fetchInt8u();
  tokenInfo->size = fetchInt8u();
  tokenInfo->arraySize = fetchInt8u();
}

void appendEmberTokenInfo(EmberTokenInfo *tokenInfo)
{
  appendInt32u(tokenInfo->nvm3Key);
  appendInt8u(tokenInfo->isCnt);
  appendInt8u(tokenInfo->isIdx);
  appendInt8u(tokenInfo->size);
  appendInt8u(tokenInfo->arraySize);
}
