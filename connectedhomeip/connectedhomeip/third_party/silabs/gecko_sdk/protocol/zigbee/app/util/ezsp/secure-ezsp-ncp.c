/***************************************************************************//**
 * @file
 * @brief Implements NCP-side secure EZSP functionality.
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
#include "hal/hal.h"
#include "app/util/ezsp/ezsp-protocol.h"

//------------------------------------------------------------------------------
// Internal functions and handlers.

EzspStatus emSecureEzspInit(void)
{
  if (emSecureEzspIsSecurityKeySet()) {
    emSecureEzspSetSecurityType(SECURE_EZSP_SECURITY_TYPE_TEMPORARY);

    tokTypeSecureEzspSecurityKey tok;
    halCommonGetToken(&tok, TOKEN_SECURE_EZSP_SECURITY_KEY);
    MEMMOVE(emberKeyContents(emSecureEzspGetSecurityKey()),
            tok.contents,
            EMBER_ENCRYPTION_KEY_SIZE);
    emSecureEzspSetState(SECURE_EZSP_STATE_PARAMETERS_PENDING);
  }
  return EZSP_SUCCESS;
}

EzspStatus emSecureEzspSetSecurityKey(EmberKeyData *securityKey)
{
  if (emSecureEzspIsSecurityKeySet()) {
    return EZSP_ERROR_SECURITY_KEY_ALREADY_SET;
  }
  tokTypeSecureEzspSecurityKey tok;
  MEMMOVE(tok.contents,
          emberKeyContents(securityKey),
          EMBER_ENCRYPTION_KEY_SIZE);
  halCommonSetToken(TOKEN_SECURE_EZSP_SECURITY_KEY, &tok);
  emSecureEzspSetState(SECURE_EZSP_STATE_PARAMETERS_PENDING);

  MEMMOVE(emberKeyContents(emSecureEzspGetSecurityKey()),
          emberKeyContents(securityKey),
          EMBER_ENCRYPTION_KEY_SIZE);
  return EZSP_SUCCESS;
}

EzspStatus emSecureEzspReset(void)
{
  // Reset temporary token
  tokTypeSecureEzspSecurityKey tok;
  MEMSET(&tok, 0x00, sizeof(tokTypeSecureEzspSecurityKey));
  halCommonSetToken(TOKEN_SECURE_EZSP_SECURITY_KEY, &tok);

  // Leave network
  emberLeaveNetwork();

  // Deinit secure EZSP parameters
  emSecureEzspDeInit();

  return EZSP_SUCCESS;
}
