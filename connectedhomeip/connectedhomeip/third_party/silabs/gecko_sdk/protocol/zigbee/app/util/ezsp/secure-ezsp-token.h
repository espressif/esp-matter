/***************************************************************************//**
 * @file
 * @brief
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

// NOTE: when building non-studio generated image define this macro
#ifdef NON_STUDIO_BUILD
#include "app/xncp/xncp-token.h"
#endif

#define CREATOR_SECURE_EZSP_SECURITY_KEY 0x5240  // RE
#define NVM3KEY_SECURE_EZSP_SECURITY_KEY (NVM3KEY_DOMAIN_USER | 0x5240)

#ifdef DEFINETYPES
typedef struct {
  /** This is the key byte data. */
  uint8_t contents[EMBER_ENCRYPTION_KEY_SIZE];
} tokTypeSecureEzspSecurityKey;
#endif //DEFINETYPES

#ifdef DEFINETOKENS
DEFINE_BASIC_TOKEN(SECURE_EZSP_SECURITY_KEY,
                   tokTypeSecureEzspSecurityKey,
                   { { 0, } })
#endif //DEFINETOKENS
