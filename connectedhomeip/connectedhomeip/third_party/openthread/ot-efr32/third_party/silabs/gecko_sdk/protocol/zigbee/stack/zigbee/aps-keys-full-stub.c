/***************************************************************************//**
 * @file
 * @brief Stub implementation of additional key functionality that is
 * necessary for a Full Zigbee Pro Stack.
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

const EmberLibraryStatus emSecurityLinkKeysLibraryStatus = EMBER_LIBRARY_IS_STUB;

void emApsSecurityReadFrameCounters(void)
{
}

EmberStatus emGetKeyTableEntry(uint8_t index, EmberKeyStruct *result)
{
  (void)index;
  (void)result;
  return EMBER_LIBRARY_NOT_PRESENT;
}

EmberStatus emberGetKeyTableEntry(uint8_t index, EmberKeyStruct *result)
{
  (void)index;
  (void)result;
  return EMBER_LIBRARY_NOT_PRESENT;
}

uint8_t emberFindKeyTableEntry(EmberEUI64 address, bool linkKey)
{
  (void)address;
  (void)linkKey;
  return 0xFF;
}

EmberStatus emSetKeyTableEntry(bool erase,
                               uint8_t index,
                               EmberEUI64 address,
                               EmberKeyData* keyData)
{
  (void)erase;
  (void)index;
  (void)address;
  (void)keyData;
  return EMBER_LIBRARY_NOT_PRESENT;
}

EmberStatus emberAddOrUpdateKeyTableEntry(EmberEUI64 address,
                                          bool linkKey,
                                          EmberKeyData* keyData)
{
  (void)address;
  (void)linkKey;
  (void)keyData;
  return EMBER_LIBRARY_NOT_PRESENT;
}

bool emProcessApplicationLinkKey(EmberEUI64 partnerEui64,
                                 bool amInitiator,
                                 EmberKeyData* keyData)
{
  (void)partnerEui64;
  (void)amInitiator;
  (void)keyData;
  return false;
}

EmberStatus emUpdateKeyState(uint8_t index,
                             uint8_t setFlags,
                             uint8_t clearFlags)
{
  (void)index;
  (void)setFlags;
  (void)clearFlags;
  return EMBER_LIBRARY_NOT_PRESENT;
}

void emNoteSleepyDeviceInKeyTable(EmberEUI64 eui64, bool sleepy)
{
  (void)eui64;
  (void)sleepy;
}

EmberStatus emberClearKeyTable(void)
{
  return EMBER_LIBRARY_NOT_PRESENT;
}
