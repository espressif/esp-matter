/***************************************************************************//**
 * @brief Core Connect stack functions
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

// The public APIs in this file should always be renamed.
#if defined(SKIP_API_RENAME)
#undef SKIP_API_RENAME
#endif

#include "stack/include/ember.h"
#include "hal/hal.h"
#include "sl_component_catalog.h"
#include "stack/include/radio-stream.h"

// Frequency Hopping

#ifndef SL_CATALOG_CONNECT_FREQUENCY_HOPPING_PRESENT

EmberStatus emGetFrequencyHoppingLibraryStatus(void)
{
  return EMBER_LIBRARY_IS_STUB;
}

uint8_t emFrequencyHoppingGetCurrentChannelIndex(void)
{
  return 0;
}

uint32_t emFrequencyHoppingGetCurrentTimestampUs(void)
{
  return 0;
}

bool emFrequencyHoppingOngoing(void)
{
  return false;
}

void emMarkFrequencyHoppingBuffers(void)
{
}

bool emFrequencyHoppingMacActivityHandler(void)
{
  return true;
}

void emFrequencyHoppingEventHandler(void)
{
}

void emIncomingMacFrequencyHoppingInfoRequestHandler(EmberNodeId sourceNodeId,
                                                     EmberPanId sourcePanId,
                                                     uint8_t control)
{
  (void)sourceNodeId;
  (void)sourcePanId;
  (void)control;
}

void emMacFrequencyHoppingInfoRequestSentHandler(EmberStatus status)
{
  (void)status;
}

void emIncomingMacFrequencyHoppingInfoHandler(EmberNodeId sourceNodeId,
                                              EmberPanId sourcePanId,
                                              uint8_t control,
                                              uint16_t seed,
                                              uint8_t channelIndex,
                                              uint32_t timestamp)
{
  (void)sourceNodeId;
  (void)sourcePanId;
  (void)control;
  (void)seed;
  (void)channelIndex;
  (void)timestamp;
}

void emMacFrequencyHoppingInfoSentHandler(EmberStatus status)
{
  (void)status;
}

bool emFrequencyHoppingClientMaybeResync(ClientResyncCallback callback)
{
  (void)callback;

  return false;
}

EmberStatus emberFrequencyHoppingSetChannelMask(uint8_t channelMaskLength,
                                                uint8_t *channelMask)
{
  (void)channelMaskLength;
  (void)channelMask;

  return EMBER_LIBRARY_NOT_PRESENT;
}

EmberStatus emberFrequencyHoppingStartServer(void)
{
  return EMBER_LIBRARY_NOT_PRESENT;
}

EmberStatus emberFrequencyHoppingStartClient(EmberNodeId serverNodeId,
                                             EmberPanId serverPanId)
{
  (void)serverNodeId;
  (void)serverPanId;

  return EMBER_LIBRARY_NOT_PRESENT;
}

EmberStatus emberFrequencyHoppingStop(void)
{
  return EMBER_LIBRARY_NOT_PRESENT;
}

#endif // SL_CATALOG_CONNECT_FREQUENCY_HOPPING_PRESENT

// Parent Support

#ifndef SL_CATALOG_CONNECT_PARENT_SUPPORT_PRESENT

extern EmberNodeId emNewChildShortId;

EmberStatus emGetParentSupportLibraryStatus(void)
{
  return EMBER_LIBRARY_IS_STUB;
}

EmberStatus emberRemoveChild(EmberMacAddress *address)
{
  (void)address;

  return EMBER_INVALID_CALL;
}

EmberStatus emParentSubmit(EmberNodeId macSource,
                           EmberNodeId nwkSource,
                           EmberNodeId nwkDestination,
                           uint8_t endpoint,
                           uint8_t packetTag,
                           EmberMessageLength packetLength,
                           uint8_t *packet,
                           EmberMessageOptions options)
{
  (void)macSource;
  (void)nwkSource;
  (void)nwkDestination;
  (void)endpoint;
  (void)packetTag;
  (void)packetLength;
  (void)packet;
  (void)options;

  return EMBER_LIBRARY_NOT_PRESENT;
}

EmberNodeId emParentGetNewChildId(void)
{
  if (emNewChildShortId == EMBER_NULL_NODE_ID) {
    return halCommonGetRandom();
  } else {
    return emNewChildShortId;
  }
}

void emParentInitiateShortAddressRequest(EmberEUI64 longId,
                                         EmberNodeId requestedNodeId)
{
  (void)longId;

  emNewChildShortId = requestedNodeId;
}

void emParentEventHandler(void)
{
}

void emNwkIncomingRangeExtenderUpdateRequestHandler(EmberMessageOptions options)
{
  (void)options;
}

void emNwkIncomingShortAddressResponseHandler(EmberNodeId allocatedId,
                                              EmberMessageOptions options)
{
  (void)allocatedId;
  (void)options;
}

bool emPendingRemovalChildHandler(uint8_t childIndex, bool rxIsDataPoll)
{
  (void)childIndex;
  (void)rxIsDataPoll;

  return false;
}

void emNwkLeaveRequestSentHandler(EmberStatus status,
                                  EmberNodeId destination)
{
  (void)status;
  (void)destination;
}

void emNwkIncomingLeaveNotificationHandler(EmberNodeId source,
                                           EmberMessageOptions options)
{
  (void)source;
  (void)options;
}

// Indirect Queue

void emIndirectQueueInit(void)
{
}

void emMarkIndirectQueueBuffers(void)
{
}

void emIndirectQueueEventHandler(void)
{
}

void emIncomingMacDataRequestIndirectQueueHandler(uint8_t *packet)
{
  (void)packet;
}

bool emIndirectQueueIsEmpty(void)
{
  return true;
}

bool emIndirectQueueLookupByAddress(EmberMacAddress *destAddr)
{
  (void)destAddr;

  return false;
}

EmberStatus emIndirectQueueAddPacket(EmberNodeId source,
                                     EmberNodeId destination,
                                     uint8_t endpoint,
                                     uint8_t packetTag,
                                     uint8_t *packet,
                                     EmberMessageLength packetLength,
                                     EmberMessageOptions options)
{
  (void)source;
  (void)destination;
  (void)endpoint;
  (void)packetTag;
  (void)packet;
  (void)packetLength;
  (void)options;

  return EMBER_LIBRARY_NOT_PRESENT;
}

EmberStatus emberPurgeIndirectMessages(void)
{
  return EMBER_LIBRARY_NOT_PRESENT;
}

EmberStatus emberSetIndirectQueueTimeout(uint32_t timeoutMs)
{
  (void)timeoutMs;

  return EMBER_LIBRARY_NOT_PRESENT;
}

// Child Table

EmberStatus emberGetChildFlags(EmberMacAddress *address,
                               EmberChildFlags *flags)
{
  (void)address;
  (void)flags;

  return EMBER_INVALID_CALL;
}

EmberStatus emberGetChildInfo(EmberMacAddress *address,
                              EmberMacAddress *addressResp,
                              EmberChildFlags *flags)
{
  (void)address;
  (void)addressResp;
  (void)flags;

  return EMBER_INVALID_CALL;
}

void emChildTableInit(void)
{
}

void emChildTableEventHandler(void)
{
}

void emChildTableLoadFromToken(void)
{
}

uint8_t emChildTableGetUnusedEntryIndex(void)
{
  return 0;
}

uint8_t emChildTableLookUp(EmberNodeId shortId,
                           EmberEUI64 longId)
{
  (void)shortId;
  (void)longId;

  return 0xFF;
}

EmberStatus emChildTableAddOrUpdateEntry(EmberNodeId shortId,
                                         EmberEUI64 longId,
                                         uint8_t macCapabilities)
{
  (void)shortId;
  (void)longId;
  (void)macCapabilities;

  return EMBER_SUCCESS;
}

EmberStatus emChildTableRemoveEntry(EmberNodeId shortId,
                                    EmberEUI64 longId)
{
  (void)shortId;
  (void)longId;

  return EMBER_SUCCESS;
}

bool emChildTableDataPending(EmberNodeId shortId,
                             EmberEUI64 longId)
{
  (void)shortId;
  (void)longId;

  return false;
}

void emChildTableNotifyActivity(uint8_t *packet, bool incoming)
{
  (void)packet;
  (void)incoming;
}

bool emChildTableCheckAndUpdateFrameCounter(EmberNodeId shortId,
                                            uint32_t frameCounter)
{
  (void)shortId;
  (void)frameCounter;

  return false;
}

EmberNodeId emGetChildShortId(uint8_t childIndex)
{
  (void)childIndex;

  return 0xFFFF;
}

// Coordinator Support

void emCoordinatorInit(void)
{
}

void emMarkCoordinatorBuffers(void)
{
}

EmberNodeId emCoordinatorCheckOrAllocateShortId(EmberNodeId requestedNodeId)
{
  (void)requestedNodeId;

  return EMBER_NULL_NODE_ID;
}

void emCoordinatorResetLastAssignedId(void)
{
}

void emChildTableEntryChangedHandler(uint8_t childTableIndex)
{
  (void)childTableIndex;
}

void emNwkIncomingShortAddressRequestHandler(EmberNodeId source,
                                             EmberNodeId requestedShortId,
                                             EmberEUI64 endDeviceLongId,
                                             EmberMessageOptions options)
{
  (void)source;
  (void)requestedShortId;
  (void)endDeviceLongId;
  (void)options;
}

void emNwkIncomingRangeExtenderUpdateHandler(EmberNodeId source,
                                             EmberMessageOptions options,
                                             uint8_t *shortIdList,
                                             uint8_t shortIdListLengthBytes)
{
  (void)source;
  (void)options;
  (void)shortIdList;
  (void)shortIdListLengthBytes;
}

#endif // SL_CATALOG_CONNECT_PARENT_SUPPORT_PRESENT

// Mac Queue

#ifndef SL_CATALOG_CONNECT_MAC_QUEUE_PRESENT

EmberStatus emGetMacQueueLibraryStatus(void)
{
  return EMBER_LIBRARY_IS_STUB;
}

void emMacQueueInit(void)
{
}

void emMarkMacQueueBuffers(void)
{
}

EmberStatus emMacOutgoingQueueSubmit(EmberNodeId destination,
                                     uint8_t tag,
                                     uint8_t headerLength,
                                     uint8_t *header,
                                     EmberMessageLength payloadLength,
                                     uint8_t *payload,
                                     EmberMessageOptions options)
{
  (void)destination;
  (void)tag;
  (void)headerLength;
  (void)header;
  (void)payloadLength;
  (void)payload;
  (void)options;

  return EMBER_LIBRARY_NOT_PRESENT;
}

bool emMacOutgoingDequeue(EmberOutgoingMessage *packet)
{
  (void)packet;

  return false;
}

bool emMacOutgoingQueueIsEmpty(void)
{
  return true;
}

EmberStatus emMacIncomingQueueSubmit(EmberMessageLength payloadLength,
                                     uint8_t *payload,
                                     int8_t rssi)
{
  (void)payloadLength;
  (void)payload;
  (void)rssi;

  return EMBER_LIBRARY_NOT_PRESENT;
}

bool emMacIncomingDequeue(EmberOutgoingMessage *packet)
{
  (void)packet;

  return false;
}

bool emMacIncomingQueueIsEmpty(void)
{
  return true;
}

#endif // SL_CATALOG_CONNECT_MAC_QUEUE_PRESENT

// Stack counter

#ifndef SL_CATALOG_CONNECT_STACK_COUNTERS_PRESENT

void emCountersInit(void)
{
}

void emCounterHandler(EmberCounterType counterType, uint8_t count)
{
  (void)counterType;
  (void)count;
}

EmberStatus emberGetCounter(EmberCounterType counterType, uint32_t *count)
{
  (void)counterType;
  (void)count;

  return EMBER_LIBRARY_NOT_PRESENT;
}

#endif // SL_CATALOG_CONNECT_STACK_COUNTERS_PRESENT

// AES security

#ifndef SL_CATALOG_CONNECT_AES_SECURITY_PRESENT

void emSecurityAesInit(void)
{
}

void emAddAuxiliaryMacHeaderAes(uint8_t *finger,
                                EmberMessageLength payloadLength,
                                EmberMessageOptions options)
{
  (void)finger;
  (void)payloadLength;
  (void)options;
}

EmberStatus emEncryptAndSignPacketAes(uint8_t *packet)
{
  (void)packet;

  return EMBER_LIBRARY_NOT_PRESENT;
}

EmberStatus emDecryptAndAuthenticatePacketAes(uint8_t *packet)
{
  (void)packet;

  return EMBER_LIBRARY_NOT_PRESENT;
}

EmberLibraryStatus emGetAesSecurityLibraryStatus(void)
{
  return EMBER_LIBRARY_IS_STUB;
}

typedef void* mbedtls_svc_key_id_t;
EmberStatus emberSetPsaSecurityKey(mbedtls_svc_key_id_t key_id)
{
  (void)key_id;
  return EMBER_LIBRARY_IS_STUB;
}

uint8_t emGetSecurityLevel(void)
{
  return 0;
}

EmberStatus emSetSecurityLevel(uint8_t level)
{
  (void)level;
  return EMBER_LIBRARY_IS_STUB;
}

bool emLocalNodeSupportsSecurity(void)
{
  return false;
}
void emAddAuxiliaryMacHeader(uint8_t *finger,
                             EmberMessageLength payloadLength,
                             EmberMessageOptions options)
{
  (void)finger;
  (void)payloadLength;
  (void)options;
  return;
}

uint32_t emGetNextOutgoingFrameCounter(void)
{
  return 0;
}

EmberStatus emEncryptAndSignPacket(uint8_t *packet)
{
  (void)packet;
  return EMBER_LIBRARY_IS_STUB;
}

EmberStatus emDecryptAndAuthenticatePacket(uint8_t *packet,
                                           bool checkFrameCounter)
{
  (void)packet;
  (void)checkFrameCounter;
  return EMBER_LIBRARY_IS_STUB;
}

void emSecurityInit(void)
{
  return;
}

void emLoadFrameCounterFromToken(void)
{
  return;
}

void emMarkSecurityBuffers(void)
{
  return;
}

#endif // SL_CATALOG_CONNECT_AES_SECURITY_PRESENT

#ifndef SL_CATALOG_CONNECT_RADIO_STREAM_PRESENT

EmberLibraryStatus emGetRadioStreamLibraryStatus(void)
{
  return EMBER_LIBRARY_IS_STUB;
}

EmberStatus emberStartTxStream(EmberTxStreamParameters parameters, uint16_t channel)
{
  (void)parameters;
  (void)channel;

  return EMBER_LIBRARY_NOT_PRESENT;
}

EmberStatus emberStopTxStream(void)
{
  return EMBER_LIBRARY_NOT_PRESENT;
}

#endif // SL_CATALOG_CONNECT_RADIO_STREAM_PRESENT
