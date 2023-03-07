/***************************************************************************//**
 * @file
 * @brief Header file for App. Framework security code
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

#ifndef SILABS_AF_SECURITY_H
#define SILABS_AF_SECURITY_H

/**
 * @brief The size of the CRC that is appended to an installation code.
 */
#define EMBER_INSTALL_CODE_CRC_SIZE 2

/**
 * @brief The number of sizes of acceptable installation codes used in
 *  Certificate Based Key Establishment (CBKE).
 */
#define EMBER_NUM_INSTALL_CODE_SIZES 4

/**
 * @brief Various sizes of valid installation codes that are stored in the
 *  manufacturing tokens. Note that each size includes 2 bytes of CRC appended
 *  to the end of the installation code.
 */
#define EMBER_INSTALL_CODE_SIZES { 6  + EMBER_INSTALL_CODE_CRC_SIZE, \
                                   8  + EMBER_INSTALL_CODE_CRC_SIZE, \
                                   12 + EMBER_INSTALL_CODE_CRC_SIZE, \
                                   16 + EMBER_INSTALL_CODE_CRC_SIZE }

void getLinkKeyFromCli(EmberKeyData* returnData);
void getNetworkKeyFromCli(EmberKeyData* returnData);

// If this flag is true we clear the link key table before forming or joining.
// If false, we skip clearing the link key once and we set this flag back to
// true.
extern bool emberAfClearLinkKeyTableUponFormingOrJoining;

extern const EmberAfSecurityProfileData emAfSecurityProfileData[];
const EmberAfSecurityProfileData *emAfGetCurrentSecurityProfileData(void);

void emAfClearLinkKeyTable(void);

EmberStatus emAfInstallCodeToKey(uint8_t* installCode, // includes CRC
                                 uint8_t length,       // includes CRC length
                                 EmberKeyData *key);

EmberStatus zaTrustCenterSetJoinPolicy(EmberJoinDecision decision);

#if !(defined(EMBER_AF_HAS_SECURITY_PROFILE_HA)     \
  || defined(EMBER_AF_HAS_SECURITY_PROFILE_HA12)    \
  || defined(EMBER_AF_HAS_SECURITY_PROFILE_SE_TEST) \
  || defined(EMBER_AF_HAS_SECURITY_PROFILE_SE_FULL))
// For no security, simply #define the security init routines to no-ops.
// For ZigBee 3.0 security, plugins handle the initialization of security.
// We need to include the real ones for any of the non-z3, non-none types.
// The logic is inverted like this because of the multi-network case.
  #ifndef USE_REAL_SECURITY_PROTOTYPES
    #define zaNodeSecurityInit(centralizedNetwork)
    #define zaTrustCenterSecurityInit(centralizedNetwork)
    #define zaTrustCenterSecurityPolicyInit()
  #endif
#else // All other security profiles.
EmberStatus zaNodeSecurityInit(bool centralizedNetwork);
EmberStatus zaTrustCenterSecurityInit(bool centralizedNetwork);
EmberStatus zaTrustCenterSecurityPolicyInit(void);
#endif

#endif // SILABS_AF_SECURITY_H
