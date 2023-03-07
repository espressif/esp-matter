/***************************************************************************//**
 * @file
 * @brief Stub version of the Certificate Based Key Exchange library.  See non-stub
 * file for more information.
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
#include "stack/include/ember-types-internal.h"

CryptoOperation emNextCryptoOperation = NULL_OPERATION;
uint8_t* partnerEuiBigEndian = NULL;
// Normally these bools never change in the stub library. However for testing
// it is easier to use the stub version of the library, and allow them to be
// changed.
EMBER_TEST_EXTERNAL_CONST bool emKeysAuthorizedByDefault  = true;
EMBER_TEST_EXTERNAL_CONST bool emAppKeyRequestsAreAllowed = true;
bool emUseStaticEmpheralKeys = false;
const EmberLibraryStatus emCbkeCoreLibraryStatus = EMBER_LIBRARY_IS_STUB;
//------------------------------------------------------------------------------

bool emIsCbkeEnabled(void)
{
  return false;
}

bool emCbkeIsIdle(void)
{
  return true;
}

void emCbkeTick(void)
{
}

void emPrepareForCbkeOperation(void)
{
}

void emPrepForEccOperation(uint8_t index, bool start)
{
  (void)index;
  (void)start;
}

void startupRadio(void)
{
}

int emWatchdogTickle(void)
{
  return EMBER_ERR_FATAL;
}

bool emAreKeysAuthorizedByDefault(void)
{
  // By default without the CBKE library we assume that all keys added
  // (via emberSetInitialSecurityState() or emberAddOrUpdateKeyTableEntry())
  // are authorized for APS data messages.
  return emKeysAuthorizedByDefault;         // true
}

bool emDoesSecurityPolicyAllowAppKeyRequests(EmberEUI64 partner1,
                                             EmberEUI64 partner2)
{
  (void)partner1;
  (void)partner2;
  return emAppKeyRequestsAreAllowed;       // true
}

EmberStatus emValidatePartnerLinkKeyRequest(EmberEUI64 partner)
{
  (void)partner;
  return EMBER_LIBRARY_NOT_PRESENT;
}

int emRandomDataGenerator(void *buffer, uint32_t size)
{
  (void)buffer;
  (void)size;
  return EMBER_ERR_FATAL;
}

int emHashFunction(uint8_t* digest, uint32_t size, uint8_t* data)
{
  (void)digest;
  (void)size;
  (void)data;
  return EMBER_ERR_FATAL;
}
void copyEui64BigEndian(bool mine, uint8_t* eui64)
{
  (void)mine;
  (void)eui64;
}
