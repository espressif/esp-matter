/***************************************************************************//**
 * @file
 * @brief Zigbee GP token definitions used by the stack.
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

//these are declared in the config/ember-configuration.c
extern uint8_t emGpProxyTableSize;
extern EmberGpProxyTableEntry emGpProxyTable[];

extern uint8_t emGpIncomingFCTokenTableSize;
extern uint8_t emGpIncomingFCTokenTimeout;

//#define EMBER_GP_PROXY_TABLE_ENTRY_STATUS_MASK 0x01

uint8_t emGpProxyTableEntryInUse(uint8_t index);
void emGpClearProxyTable(void);

//the follwoing gets called every sec and may be(?) every time a proxy table entry token is re-written
void emGpWriteIncomingFCToToken(uint8_t index);

/*
 #define emGpProxyTableEntryInUse(index)                                  \
   ((emGpProxyTable[(index)].status                                          \
    & EMBER_GP_PROXY_TABLE_ENTRY_STATUS_MASK)                        \
    == EMBER_GP_PROXY_TABLE_ENTRY_STATUS_ACTIVE)
 */

#define emGpProxyTableEntryUnused(index) \
  (emGpProxyTable[(index)].status        \
   == EMBER_GP_PROXY_TABLE_ENTRY_STATUS_UNUSED)

#define  emGpProxyTableGetAddr(index) \
  (&(emGpProxyTable[(index)].gpd))

#define emGpProxyTableSetSecurityFrameCounter(index, sfc) \
  (emGpProxyTable[(index)].gpdSecurityFrameCounter = (sfc))

#define emGpProxyTableGetSecurityFrameCounter(index) \
  (emGpProxyTable[(index)].gpdSecurityFrameCounter)

#define emGpProxyTableGetOptions(index) \
  (emGpProxyTable[(index)].options)

#define emGpProxyTableSetOptions(index, o) \
  (emGpProxyTable[(index)].options = (o))

#define emGpProxyTableHasSecurity(index)                                                  \
  ((emGpProxyTable[(index)].options & EMBER_AF_GP_PROXY_TABLE_ENTRY_OPTIONS_SECURITY_USE) \
   || (emGpProxyTable[(index)].securityOptions))

#define emGpProxyTableGetSecurityOptions(index) \
  (emGpProxyTable[(index)].securityOptions)

#define emGpProxyTableSetSecurityOptions(index, o) \
  (emGpProxyTable[(index)].securityOptions = (o))

#define emGpProxyTableGetSinkList(index) \
  (emGpProxyTable[(index)].sinkList)

#define emGpProxyTableEntryHasLinkKey(index) \
  (((emGpProxyTable[(index)].securityOptions) & 0x1C))

#define emGpProxyTableGetSecurityKey(index) \
  (emGpProxyTable[(index)].gpdKey)

#define emGpProxyTableSetStatus(index, s) \
  (emGpProxyTable[(index)].status = (s))

#define emGpProxyTableGetStatus(index) \
  (emGpProxyTable[(index)].status)

#define emGpProxyTableGetAssignedAlias(index) \
  (emGpProxyTable[(index)].assignedAlias)

#define emGpProxyTableGetGroupcastRadius(index) \
  (emGpProxyTable[(index)].groupcastRadius)

#define emGpProxyTableSetInRange(index) \
  (emGpProxyTable[(index)].options |= GP_PROXY_TABLE_OPTIONS_IN_RANGE)
#define emGpProxyTableClearInRange(index) \
  (emGpProxyTable[(index)].options &= (~GP_PROXY_TABLE_OPTIONS_IN_RANGE))

#define emGpProxyTableSetFirstToForward(index) \
  (emGpProxyTable[(index)].options |= EMBER_AF_GP_PROXY_TABLE_ENTRY_OPTIONS_FIRST_TO_FORWARD)
#define emGpProxyTableClearFirstToForward(index) \
  (emGpProxyTable[(index)].options &= (~EMBER_AF_GP_PROXY_TABLE_ENTRY_OPTIONS_FIRST_TO_FORWARD))

void emGpProxyTableInit(void);
EmberStatus emGpProxyTableSetEntry(uint8_t proxyIndex,
                                   EmberGpProxyTableEntry *entry);
EmberStatus emberGpProxyTableGetEntry(uint8_t proxyIndex,
                                      EmberGpProxyTableEntry *entry);
uint8_t emGpProxyTableGetFreeEntryIndex(void);
uint8_t emberGpProxyTableLookup(EmberGpAddress *addr);
uint8_t emGpProxyTableFindOrAllocateEntry(EmberGpAddress *addr);
//void emGpProxyTableAddSink(uint8_t index,uint16_t options,EmberEUI64 sinkIeeeAddress,EmberNodeId sinkNwkAddress,uint16_t sinkGroupId,uint32_t gpdSecurityFrameCounter,uint8_t *gpdKey,uint16_t assignedAlias,uint8_t forwardingRadius);
void emGpProxyTableAddSink(uint8_t index,
                           //      uint16_t options,
                           uint8_t commMode,
                           EmberEUI64 sinkIeeeAddress,
                           EmberNodeId sinkNwkAddress,
                           uint16_t sinkGroupId,
//                           uint32_t gpdSecurityFrameCounter,
//                          uint8_t *gpdKey,
                           uint16_t assignedAlias
                           //                        uint8_t forwardingRadius)
                           );
void emGpProxyTableRemoveSink(uint8_t index, EmberEUI64 sinkIeeeAddress, uint16_t sinkGroupId, uint16_t assignedAlias);
void emGpProxyTableRemoveEntry(uint8_t index);
void emGpProxyTableSetKey(uint8_t index, uint8_t * gpdKey, EmberGpKeyType securityKeyType);
void emGpProxyTableGetKey(uint8_t index, EmberKeyData *key);
bool emGpAddressMatch(const EmberGpAddress *a1, const EmberGpAddress *a2);
bool emberGpProxyTableProcessGpPairing(uint32_t options,
                                       EmberGpAddress* addr,
                                       uint8_t commMode,
                                       uint16_t sinkNwkAddress,
                                       uint16_t sinkGroupId,
                                       uint16_t assignedAlias,
                                       uint8_t* sinkIeeeAddress,
                                       EmberKeyData *gpdKey,
                                       uint32_t gpdSecurityFrameCounter,
                                       uint8_t forwardingRadius);

void emClearGpTxQueue(void);
