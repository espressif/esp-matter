/***************************************************************************//**
 * @file
 * @brief Code for multi PAN stubs.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "hal/hal.h"
#include "core/multi-pan.h"
#include "core/ember-multi-network.h" // for emNetworkDescriptor and emPanInfoData
#include "mac-child.h" // for sl_mac_child_entry_t
#include "stack/include/ember-types-internal.h" // for PAN_ID_OKAY

const EmberLibraryStatus emMultiPanLibraryStatus = EMBER_LIBRARY_IS_STUB;

#ifndef EMBER_MULTI_NETWORK_STRIPPED

extern sl_mac_child_entry_t emChildTableData[];
extern uint16_t emChildStatusData[];
extern uint32_t emChildTimersData[];
extern uint16_t emChildLqiData[];
extern uint8_t emEndDeviceTimeoutData[];
extern uint8_t emRouteRecordTableData[];
extern EmBroadcastTableEntry emBroadcastTableData[];

void emNetworkPanInfoDataInit(uint8_t nwkIndex, EmberNetworkInfo *curNetwork)
{
  (void)nwkIndex;
  curNetwork->panInfoData->childTable = &(emChildTableData[0]);
  curNetwork->panInfoData->childStatus = &(emChildStatusData[0]);
  curNetwork->panInfoData->childTimers = &(emChildTimersData[0]);
  curNetwork->panInfoData->childLqi = &(emChildLqiData[0]);
  curNetwork->panInfoData->endDeviceTimeout = &(emEndDeviceTimeoutData[0]);
  curNetwork->panInfoData->endDeviceChildCount = 0;
  curNetwork->panInfoData->routeRecordTable = &(emRouteRecordTableData[0]);
  curNetwork->panInfoData->broadcastTable = &(emBroadcastTableData[0]);
  curNetwork->panInfoData->broadcastHead = 0;
  curNetwork->panInfoData->fastLinkStatusCount = 0;
  curNetwork->panInfoData->permitJoining = false;
  curNetwork->panInfoData->macPermitAssociation = false;
  curNetwork->panInfoData->allowRejoinsWithWellKnownKey = false;
  curNetwork->panInfoData->parentAnnounceIndex = 0;
  curNetwork->panInfoData->totalInitialChildren = 0;
  curNetwork->panInfoData->newPanId = PAN_ID_OKAY;
}

void emSetChildTableForCurrentNetwork(void)
{
}

void emGetChildTokenForCurrentNetwork(void *data,
                                      uint16_t tokenAddress,
                                      uint8_t childIndex)
{
  switch (tokenAddress) {
    case TOKEN_STACK_CHILD_TABLE: {
      halCommonGetIndexedToken(data, TOKEN_STACK_CHILD_TABLE, childIndex);
      break;
    }

    case TOKEN_STACK_ADDITIONAL_CHILD_DATA: {
      halCommonGetIndexedToken(data, TOKEN_STACK_ADDITIONAL_CHILD_DATA, childIndex);
      break;
    }

    default:
      break;
  }
}

void emSetChildTokenForCurrentNetwork(uint16_t tokenAddress,
                                      uint8_t childIndex,
                                      void *data)
{
  switch (tokenAddress) {
    case TOKEN_STACK_CHILD_TABLE: {
      halCommonSetIndexedToken(TOKEN_STACK_CHILD_TABLE, childIndex, data);
      break;
    }

    case TOKEN_STACK_ADDITIONAL_CHILD_DATA: {
      halCommonSetIndexedToken(TOKEN_STACK_ADDITIONAL_CHILD_DATA, childIndex, data);
      break;
    }

    default:
      break;
  }
}

EmberEvent* emGetForkedEvent(EmberEvent *list)
{
  return list;
}
uint8_t emGetNetworkIndexFromEvent(EmberEvent *event,
                                   EmberEvent *list)
{
  return EMBER_NULL_NETWORK_INDEX;
}

uint8_t emGetNetworkIndexForForkedGlobal(void)
{
  return 0;
}

bool emIsPanIdInUse(EmberPanId panId)
{
  return false;
}
// temporary hack to avoid deadstripping this function ( EMZIGBEE-8393 )
// for libaries build on Raspi
void fakeCallToMultiPanInit(void)
{
  emMultiPanInit();
}

#endif // EMBER_MULTI_NETWORK_STRIPPED
