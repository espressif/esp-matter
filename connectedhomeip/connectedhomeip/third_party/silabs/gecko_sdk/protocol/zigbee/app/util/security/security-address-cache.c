/***************************************************************************//**
 * @file
 * @brief This is the code used by nodes to manage
 * the address table cache.  Devices may need this address
 *  table cache in order to properly decrypt APS Encrypted messages sent
 *  by other devices (i.e. APS_UPDATE_DEVICE).  The source address
 *  of the sending device may not be present in the APS Auxiliary Header
 *  and therefore the stack will have to look it up.  The stack automatically
 *  consults the address table when it is trying to determine the IEEE
 *  address from a node ID, so this code utilizes a small portion of the
 *  address table to cache the data it needs.
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

//------------------------------------------------------------------------------
// INCLUDES

#include PLATFORM_HEADER //compiler/micro specifics, types

#ifdef EZSP_HOST
// Includes needed for ember related functions for the EZSP host
  #include "stack/include/error.h"
  #include "stack/include/ember-types.h"
  #include "app/util/ezsp/ezsp-protocol.h"
  #include "app/util/ezsp/ezsp.h"
  #include "app/util/ezsp/ezsp-utils.h"
  #include "app/util/ezsp/serial-interface.h"
#else
// Includes needed for ember related functions for the EM250
  #include "stack/include/ember.h"
#endif // EZSP_HOST
#include "stack/include/error.h"
#include "hal/hal.h"
#include "app/util/security/security.h"

//------------------------------------------------------------------------------

uint8_t addressCacheSize = 0;
static uint8_t addressCacheStartIndex;
static uint8_t nextIndex;                 // relative to addressCacheStartIndex

//------------------------------------------------------------------------------

void securityAddressCacheInit(uint8_t securityAddressCacheStartIndex,
                              uint8_t securityAddressCacheSize)
{
  addressCacheStartIndex = securityAddressCacheStartIndex;
  addressCacheSize = securityAddressCacheSize;
  nextIndex = 0;
}

//------------------------------------------------------------------------------
// The Trust Center Address cache keeps track of EUI64s for the purpose of
// sending APS Encrypted commands.  During joining the Trust Center
// will send an APS Encrypted command to the parent of the device and needs to
// know their long address for creating the Nonce used in encryption.

// We simply loop through our cache and overwrite the last one received.

void securityAddToAddressCache(EmberNodeId nodeId, EmberEUI64 nodeEui64)
{
  uint8_t index = nextIndex;
  uint8_t i;

  if (addressCacheSize == 0) {
    return;
  }

  if (nodeId >= EMBER_BROADCAST_ADDRESS) {
    return;
  }

  // Search through our cache for an existing IEEE with the same info.
  // If it exists update that.
  for (i = 0; i < addressCacheSize; i++) {
    EmberEUI64 eui64;
    emberGetAddressTableRemoteEui64(addressCacheStartIndex + i, eui64);
    if (MEMCOMPARE(eui64, nodeEui64, EUI64_SIZE) == 0) {
      index = i;
      break;
    }
  }

  if (index == nextIndex) {
    nextIndex += 1;
    if (nextIndex == addressCacheSize) {
      nextIndex = 0;
    }
  }

  index += addressCacheStartIndex;
  if (emberSetAddressTableRemoteEui64(index, nodeEui64)
      == EMBER_SUCCESS) {
    emberSetAddressTableRemoteNodeId(index, nodeId);
  }
}
