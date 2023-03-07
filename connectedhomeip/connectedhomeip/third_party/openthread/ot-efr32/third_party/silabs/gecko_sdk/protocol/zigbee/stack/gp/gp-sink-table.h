/***************************************************************************//**
 * @file
 * @brief Zigbee GP sink definitions used by the framework and stack.
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

#ifndef GP_SINK_TABLE
#define GP_SINK_TABLE

#ifdef __cplusplus
extern "C" {
#endif

//#include "gp-proxy-table.h"

extern uint8_t emGpIncomingFCInSinkTokenTableSize;
extern uint8_t emGpIncomingFCInSinkTokenTimeout;

//these are declared in the config/ember-configuration.c
extern uint8_t emGpSinkTableSize;
extern EmberGpSinkTableEntry emGpSinkTable[];

#define EMBER_GP_SINK_TABLE_SECURITY_MASK 0x1F

void emGpWriteIncomingFCInSinkToToken(uint8_t index);

void emberGpSinkTableClearAll(void);

#define emGpSinkTableEntryUnused(index)         \
  ((emGpSinkTable[(index)].status               \
    == EMBER_GP_SINK_TABLE_ENTRY_STATUS_UNUSED) \
   || (emGpSinkTable[(index)].status == 0))

#define emGpSinkTableEntryActive(index) \
  (emGpSinkTable[(index)].status        \
   == EMBER_GP_SINK_TABLE_ENTRY_STATUS_ACTIVE)

#define  emGpSinkTableGetAddr(index) \
  (&(emGpSinkTable[(index)].gpd))

#define emGpSinkTableSetSecurityFrameCounter(index, sfc) \
  (emGpSinkTable[(index)].gpdSecurityFrameCounter = (sfc))

#define emGpSinkTableGetSecurityFrameCounter(index) \
  (emGpSinkTable[(index)].gpdSecurityFrameCounter)

#define emGpSinkTableGetOptions(index) \
  (emGpSinkTable[(index)].options)

#define emGpSinkTableSetOptions(index, o) \
  (emGpSinkTable[(index)].options = o)

#define emGpSinkTableHasSecurity(index) \
  (emGpSinkTable[(index)].options & EMBER_AF_GP_SINK_TABLE_ENTRY_OPTIONS_SECURITY_USE)

#define emGpSinkTableGetSecurityOptions(index) \
  (emGpSinkTable[(index)].securityOptions)

#define emGpSinkTableSetSecurityOptions(index, o) \
  ((emGpSinkTable[(index)].securityOptions = o))

#define emGpSinkTableGetSinkList(index) \
  (emGpSinkTable[(index)].sinkList)

#define emGpSinkTableEntryHasLinkKey(index) \
  (((emGpSinkTable[(index)].securityOptions) & EMBER_GP_SINK_TABLE_SECURITY_MASK))

#define emGpSinkTableGetSecurityKey(index) \
  (emGpSinkTable[(index)].gpdKey)

#define emGpSinkTableSetStatus(index, s) \
  (emGpSinkTable[(index)].status = (s))

#define emGpSinkTableGetStatus(index) \
  (emGpSinkTable[(index)].status)

#define emGpSinkTableSetInRange(index) \
  (emGpSinkTable[(index)].options |= GP_SINK_TABLE_OPTIONS_IN_RANGE)

#define emGpSinkTableSetAssignedAlias(index, s) \
  (emGpSinkTable[(index)].assignedAlias = (s))

#define emGpSinkTableGetAssignedAlias(index) \
  (emGpSinkTable[(index)].assignedAlias)

#define emGpSinkTableSetDeviceId(index, s) \
  (emGpSinkTable[(index)].deviceId = (s))

#define emGpSinkTableGetDeviceId(index) \
  (emGpSinkTable[(index)].deviceId)

#define emGpSinkTableSetGroupcastRadius(index, s) \
  (emGpSinkTable[(index)].groupcastRadius = (s))

#define emGpSinkTableGetGroupcastRadius(index) \
  (emGpSinkTable[(index)].groupcastRadius)

bool emGpAddressMatch(const EmberGpAddress *a1, const EmberGpAddress *a2);
void emberGpSinkTableInit(void);
EmberStatus emberGpSinkTableSetEntry(uint8_t sinkTableIndex,
                                     EmberGpSinkTableEntry *entry);
EmberStatus emberGpSinkTableGetEntry(uint8_t sinkTableIndex,
                                     EmberGpSinkTableEntry *entry);
uint8_t emberGpSinkTableEntryInUse(uint8_t sinkTableIndex);
uint8_t emGpSinkTableGetFreeEntryIndex(void);

uint8_t emberGpSinkTableFindOrAllocateEntry(EmberGpAddress *addr);

void emGpSinkTableAddGroup(uint8_t index,
                           uint16_t sinkGroupId,
                           uint16_t alias);
bool emGpSinkTableRemoveGroup(uint8_t index,
                              uint16_t sinkGroupId,
                              uint16_t assignedAlias);
uint8_t emberGpSinkTableLookup(EmberGpAddress *addr);
void emberGpSinkTableRemoveEntry(uint8_t index);
void emberGpSinkTableSetSecurityFrameCounter(uint8_t index,
                                             uint32_t sfc);

#ifdef __cplusplus
}
#endif

#endif //GP_SINK_TABLE
