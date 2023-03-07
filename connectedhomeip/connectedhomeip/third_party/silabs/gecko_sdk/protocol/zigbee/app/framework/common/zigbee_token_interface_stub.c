/***************************************************************************//**
 * @brief ZigBee Application Framework token management code.
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "stack/include/ember.h"

//------------------------------------------------------------------------------
// API Stubs
// Weak implementation to return the library not available incase the token
// manager is not present.

WEAK(uint8_t emberGetTokenCount(void))
{
  return 0;
}

WEAK(EmberStatus emberGetTokenInfo(uint8_t index,
                                   EmberTokenInfo *tokenInfo))
{
  (void) index;
  (void) tokenInfo;
  return EMBER_LIBRARY_NOT_PRESENT;
}

WEAK(EmberStatus emberGetTokenData(uint32_t token,
                                   uint32_t index,
                                   EmberTokenData *tokenData))
{
  (void) token;
  (void) index;
  (void) tokenData;
  return EMBER_LIBRARY_NOT_PRESENT;
}

WEAK(EmberStatus emberSetTokenData(uint32_t token,
                                   uint32_t index,
                                   EmberTokenData *tokenData))
{
  (void) token;
  (void) index;
  (void) tokenData;
  return EMBER_LIBRARY_NOT_PRESENT;
}
