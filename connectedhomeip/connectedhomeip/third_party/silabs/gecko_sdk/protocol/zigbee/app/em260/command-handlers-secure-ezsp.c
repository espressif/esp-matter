/***************************************************************************//**
 * @file
 * @brief Secure EZSP NCP support code.
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
  EzspStatus status;

  if (securityType == SECURE_EZSP_SECURITY_TYPE_PERMANENT) {
    return EZSP_ERROR_INVALID_CALL;
  } else if (securityType == SECURE_EZSP_SECURITY_TYPE_TEMPORARY) {
    status = emSecureEzspSetSecurityKey(key);

    if (status == EMBER_SUCCESS) {
      emSecureEzspSetSecurityType(securityType);
    }

    return status;
  } else {
    return EZSP_ERROR_SECURITY_TYPE_INVALID;
  }
}

EzspStatus ezspSetSecurityParameters(SecureEzspSecurityLevel securityLevel,
                                     SecureEzspRandomNumber *hostRandomNumber,
                                     SecureEzspRandomNumber *returnNcpRandomNumber)
{
  if (securityLevel != SECURE_EZSP_SECURITY_LEVEL_ENC_MIC_32) {
    return EZSP_ERROR_SECURITY_PARAMETERS_INVALID;
  }

  if (!emSecureEzspIsSecurityKeySet()) {
    return EZSP_ERROR_SECURITY_KEY_NOT_SET;
  }

  if (!emSecureEzspParametersArePending()) {
    return EZSP_ERROR_SECURITY_PARAMETERS_ALREADY_SET;
  }

  // emberGetStrongRandomNumberArray() returns an array of 16-bit numbers, so
  // we must cast the pointer for our return value and reduce
  // the number of bytes we request by a factor of 2.
  if (emberGetStrongRandomNumberArray((uint16_t*)(SecureEzspRandomNumberContents(returnNcpRandomNumber)),
                                      SECURE_EZSP_RANDOM_NUMBER_SIZE / 2) == EMBER_INSUFFICIENT_RANDOM_DATA) {
    return EZSP_ASH_NCP_FATAL_ERROR;
  }

  emSecureEzspSetSecurityLevel(securityLevel);
  emSecureEzspGenerateSessionIds(hostRandomNumber, returnNcpRandomNumber);

  return EZSP_SUCCESS;
}

EzspStatus ezspResetToFactoryDefaults(void)
{
  if (emSecureEzspGetSecurityType() == SECURE_EZSP_SECURITY_TYPE_PERMANENT) {
    return EZSP_ERROR_INVALID_CALL;
  }

  if (emSecureEzspGetState() == SECURE_EZSP_STATE_INITIAL) {
    return EZSP_ERROR_SECURITY_KEY_NOT_SET;
  }

  return emSecureEzspReset();
}

EzspStatus ezspGetSecurityKeyStatus(SecureEzspSecurityType *returnSecurityType)
{
  if (emSecureEzspIsSecurityKeySet()) {
    *returnSecurityType = emSecureEzspGetSecurityType();
    return EZSP_ERROR_SECURITY_KEY_ALREADY_SET;
  }
  return EZSP_ERROR_SECURITY_KEY_NOT_SET;
}
