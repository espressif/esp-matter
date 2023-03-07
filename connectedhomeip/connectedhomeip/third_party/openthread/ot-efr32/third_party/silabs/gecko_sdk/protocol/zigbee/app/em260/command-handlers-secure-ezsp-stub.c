/***************************************************************************//**
 * @file
 * @brief Implements secure EZSP command handler stubs.
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

#include "stack/include/ember.h"
#include "app/util/ezsp/ezsp-protocol.h"

EzspStatus ezspSetSecurityKey(EmberKeyData *key,
                              SecureEzspSecurityType securityType)
{
  (void)key;
  (void)securityType;
  return EZSP_ERROR_INVALID_CALL;
}

EzspStatus ezspSetSecurityParameters(SecureEzspSecurityLevel securityLevel,
                                     SecureEzspRandomNumber *hostRandomNumber,
                                     SecureEzspRandomNumber *returnNcpRandomNumber)
{
  (void)securityLevel;
  (void)hostRandomNumber;
  (void)returnNcpRandomNumber;
  return EZSP_ERROR_INVALID_CALL;
}

EzspStatus ezspResetToFactoryDefaults(void)
{
  return EZSP_ERROR_INVALID_CALL;
}

EzspStatus ezspGetSecurityKeyStatus(SecureEzspSecurityType *returnSecurityType)
{
  (void)returnSecurityType;
  return EZSP_ERROR_INVALID_CALL;
}
