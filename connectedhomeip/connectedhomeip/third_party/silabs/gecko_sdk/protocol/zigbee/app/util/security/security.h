/***************************************************************************//**
 * @file
 * @brief functions for manipulating security for Trust Center and
 * non Trust Center nodes.
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

#ifndef SILABS_APP_UTIL_SECURITY_SECURITY_H
#define SILABS_APP_UTIL_SECURITY_SECURITY_H

#if !defined emberKeyContents
  #define emberKeyContents(key) (((EmberKeyData*)(key))->contents)
#endif

// Trust Center Functions
bool trustCenterInit(EmberKeyData* preconfiguredKey,
                     EmberKeyData* networkKey);
void trustCenterPermitJoins(bool allow);
bool trustCenterIsPermittingJoins(void);

// Non Trust Center functions
bool nodeSecurityInit(EmberKeyData* preconfiguredKey);

// Common functions
extern uint8_t addressCacheSize;
#define securityAddressCacheGetSize() (addressCacheSize + 0)
void securityAddressCacheInit(uint8_t securityAddressCacheStartIndex,
                              uint8_t securityAddressCacheSize);
void securityAddToAddressCache(EmberNodeId nodeId, EmberEUI64 nodeEui64);

#endif // SILABS_APP_UTIL_SECURITY_SECURITY_H
