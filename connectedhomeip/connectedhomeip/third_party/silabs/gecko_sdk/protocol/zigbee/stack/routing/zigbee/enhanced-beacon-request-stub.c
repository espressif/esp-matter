/***************************************************************************//**
 * @file
 * @brief Implementation of Enhanced Beacon request generation
 * and parsing for Zigbee Pro.
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
#include "stack/include/ember-types-internal.h"
#include "stack/include/library.h"
#include "upper-mac.h"

//=============================================================================
// Returns true if the device should continue to process, false if it
// should be dropped.

const EmberLibraryStatus emEnhancedBeaconRequestLibraryStatus = EMBER_LIBRARY_IS_STUB;

EmberMessageBuffer deviceEnhancedBeaconRequestFilterList = EMBER_NULL_MESSAGE_BUFFER;
uint8_t joinListUpdateId = 0;

void emMarkEnhancedBeaconBuffers(void)
{
}

bool emZigbeeProcessEnhancedBeaconRequest(PacketHeader header,
                                          EmMacFrameInfoElementParseResult result,
                                          EmMacInfoElementField* infoElementsArray)
{
  (void)header;
  (void)infoElementsArray;
  // If the frame contains MAC Info Elements (IEs) we don't process it.
  if (result == EM_MAC_FRAME_INFO_ELEMENTS_NOT_PRESENT) {
    return true;
  }
  return false;
}

EmberStatus emZigbeeSendEnhancedBeaconRequest(uint8_t mac_index, bool firstTimeJoin)
{
  (void)mac_index;
  (void)firstTimeJoin;
  return EMBER_LIBRARY_NOT_PRESENT;
}

bool emCheckEBRFilterList(uint8_t * longId)
{
  (void)longId;
  return true;
}

void emJoinListAdd(EmberEUI64 longId)
{
  (void)longId;
}

void emJoinListDelete(EmberEUI64 longId)
{
  (void)longId;
}

void emJoinListClear(void)
{
}

EmberMessageBuffer emGetJoinListPointer(void)
{
  return EMBER_NULL_MESSAGE_BUFFER;
}

int8_t emFindEbrPowerByEui64(EmberEUI64 eui64)
{
  (void)eui64;
  return MAX_RADIO_POWER_USER_PROVIDED;
}

void emRemoveEbrPowerAndEui64Entry(EmberEUI64 eui64)
{
  (void)eui64;
}

int8_t emberChildPower(uint8_t childIndex)
{
  (void)childIndex;
  return MAX_RADIO_POWER_USER_PROVIDED;
}

void emberSetChildPower(uint8_t childIndex, int8_t newPower)
{
  (void)childIndex;
  (void)newPower;
}

uint8_t emGetEbrPowerAndEuiListPointer(uint8_t **buffer)
{
  (void)buffer;
  return 0;
}

bool emAddEbrEuiAndPowerEntry(EmberEUI64 eui64, int8_t newPower)
{
  (void)eui64;
  (void)newPower;
  return false;
}

void emMacSendEnhancedBeacon(uint8_t mac_index)
{
  (void)mac_index;
}
