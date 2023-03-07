/***************************************************************************//**
 * @file
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

#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include "stack/include/library.h"

EmberStatus emGpInit(void)
{
  return EMBER_SUCCESS;
}

void emGpProxyTableInit(void)
{
}
void emGpIncomingMessageHandler(PacketHeader header)
{
  (void)header;
}
//CGP Stub

// CGP-DATA.request
void emCGpSend(bool useCca,
               EmberGpAddress *src,
               EmberGpAddress *dst,
               uint8_t gpdCommandId,
               EmberMessageBuffer asdu)
{
  (void)useCca;
  (void)src;
  (void)dst;
  (void)gpdCommandId;
  (void)asdu;
  return;
}

//CGP-DATA.confirm
void emCGpSentHandler(EmberStatus status, uint8_t gpMpduHandle)
{
  (void)status;
  (void)gpMpduHandle;
  return;
}

//dGP-DATA.indication
void emDGpIncomingMessageHandler(uint8_t lqi,
                                 uint8_t sequenceNumber,
                                 EmberCGpAddress const *src,
                                 EmberCGpAddress const *dst,
                                 PacketHeader header)
//uint8_t gpMpduLength,
//uint8_t const *gpMpdu);
{
  (void)lqi;
  (void)sequenceNumber;
  (void)src;
  (void)dst;
  (void)header;
  return;
}

//GP-DATA.request
EmberStatus emberDGpSend(bool action,
                         bool useCca,
                         EmberGpAddress *addr,
                         uint8_t gpdCommandId,
                         uint8_t gpdAsduLength,
                         uint8_t const *gpdAsdu,
                         uint8_t gpepHandle,
                         uint16_t gpTxQueueEntryLifetimeMs)
{
  (void)action;
  (void)useCca;
  (void)addr;
  (void)gpdCommandId;
  (void)gpdAsduLength;
  (void)gpdAsdu;
  (void)gpepHandle;
  (void)gpTxQueueEntryLifetimeMs;
  return EMBER_LIBRARY_NOT_PRESENT;
}

//GP-DATA.indication
EmberStatus emberGpIncomingMessageHandler(uint8_t lqi,
                                          uint8_t sequenceNumber,
                                          EmberCGpAddress const *src,
                                          //EmberGpApplication const *application,
                                          EmberGpApplicationId applicationId,
                                          EmberGpSecurityLevel gpdfSecurityLevel,
                                          bool autoCommissioning,
                                          bool rxAfterTx,
                                          uint32_t srcID,
                                          uint8_t endpoint,
                                          uint32_t gpdSecurityFrameCounter,
                                          uint8_t gpdCommandId,
                                          uint8_t gpdAsduLength,
                                          uint8_t const *gpdAsdu,
                                          uint32_t mic,
                                          uint8_t const *gpMpdu,
                                          uint8_t gpepHandle,
                                          uint16_t gpTxQueueEntryLifetime)
{
  (void)lqi;
  (void)sequenceNumber;
  (void)src;
  (void)applicationId;
  (void)gpdfSecurityLevel;
  (void)autoCommissioning;
  (void)rxAfterTx;
  (void)srcID;
  (void)endpoint;
  (void)gpdSecurityFrameCounter;
  (void)gpdCommandId;
  (void)gpdAsduLength;
  (void)gpdAsdu;
  (void)mic;
  (void)gpMpdu;
  (void)gpepHandle;
  (void)gpTxQueueEntryLifetime;
  return EMBER_LIBRARY_NOT_PRESENT;
}

//GP-SEC.request
void emberGpSecurityRequest(EmberGpApplicationId applicationId,
                            uint32_t srcId,
                            EmberEUI64 ieee,
                            uint8_t endpoint,
                            EmberGpSecurityLevel gpdfSecurityLevel,
                            EmberGpKeyType gpdfKeyType,
                            EmberGpSecurityFrameCounter gpdSecurityFrameCounter,
                            uint8_t dgpHandle)
{
  (void)applicationId;
  (void)srcId;
  (void)ieee;
  (void)endpoint;
  (void)gpdfSecurityLevel;
  (void)gpdfKeyType;
  (void)gpdSecurityFrameCounter;
  (void)dgpHandle;
  return;
}

//GP-SEC.response
void emberGpSecurityResponse(EmberStatus status,
                             uint8_t dgpHandle,
                             EmberGpApplicationId applicationId,
                             uint32_t srcID,
                             EmberEUI64 ieee,
                             uint8_t endpoint,
                             EmberGpSecurityLevel gpdfSecurityLevel,
                             EmberGpKeyType gpdf_KeyType,
                             EmberKeyData gpdKey,
                             EmberGpSecurityFrameCounter gpdSecurityFrameCounter)
{
  (void)status;
  (void)dgpHandle;
  (void)applicationId;
  (void)srcID;
  (void)ieee;
  (void)endpoint;
  (void)gpdfSecurityLevel;
  (void)gpdf_KeyType;
  (void)gpdKey;
  (void)gpdSecurityFrameCounter;
  return;
}

void emReadGpTokens(void)
{
  return;
}

bool emIsGpIdConflict(EmberNodeId shortId)
{
  (void)shortId;
  return false;
}

void emUpdateGpProxyTable(uint8_t *longId, EmberNodeId shortId)
{
  (void)longId;
  (void)shortId;
}

void emGpTxEventHandler(void)
{
}

bool emberGpProxyTableProcessGpPairing(uint32_t options,
                                       EmberGpAddress* addr,
                                       uint8_t commMode,
                                       uint16_t sinkNwkAddress,
                                       uint16_t sinkGroupId,
                                       uint16_t assignedAlias,
                                       uint8_t* sinkIeeeAddress,
                                       EmberKeyData* gpdKey,
                                       uint32_t gpdSecurityFrameCounter,
                                       uint8_t forwardingRadius)
{
  (void)options;
  (void)addr;
  (void)commMode;
  (void)sinkNwkAddress;
  (void)sinkGroupId;
  (void)assignedAlias;
  (void)sinkIeeeAddress;
  (void)gpdKey;
  (void)gpdSecurityFrameCounter;
  (void)forwardingRadius;
  return false;
}

uint8_t emberGpProxyTableLookup(EmberGpAddress *addr)
{
  (void)addr;
  return 0xFF;
}
EmberStatus emberGpProxyTableGetEntry(uint8_t proxyIndex,
                                      EmberGpProxyTableEntry *entry)
{
  (void)proxyIndex;
  (void)entry;
  return EMBER_LIBRARY_NOT_PRESENT;
}

EmberStatus emberGpSinkTableGetEntry(uint8_t sinkIndex,
                                     EmberGpSinkTableEntry *entry)
{
  (void)sinkIndex;
  (void)entry;
  return EMBER_LIBRARY_NOT_PRESENT;
}

uint8_t emberGpSinkTableLookup(EmberGpAddress *addr)
{
  (void)addr;
  return 0xFF;
}

EmberStatus emberGpSinkTableSetEntry(uint8_t sinkTableIndex,
                                     EmberGpSinkTableEntry *entry)
{
  (void)sinkTableIndex;
  (void)entry;
  return EMBER_LIBRARY_NOT_PRESENT;
}

uint8_t emberGpSinkTableFindOrAllocateEntry(EmberGpAddress *addr)
{
  (void)addr;
  return 0xFF;
}

void emberGpSinkTableRemoveEntry(uint8_t index)
{
  (void)index;
}

void emberGpSinkTableClearAll(void)
{
}
void emberGpSinkTableInit(void)
{
}

void emberGpSinkTableSetSecurityFrameCounter(uint8_t index,
                                             uint32_t sfc)
{
  (void)index;
  (void)sfc;
}
