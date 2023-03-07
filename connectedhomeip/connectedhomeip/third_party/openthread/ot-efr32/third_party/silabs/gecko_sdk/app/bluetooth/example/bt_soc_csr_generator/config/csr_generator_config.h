/***************************************************************************//**
 * @file
 * @brief SoC Certificate Generator Configuration
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#ifndef CSR_GENERATOR_CONFIG_H
#define CSR_GENERATOR_CONFIG_H

#define KEY_USAGE_SIGN_VERIFY PSA_KEY_USAGE_SIGN_MESSAGE | PSA_KEY_USAGE_VERIFY_MESSAGE
#define KEY_USAGE_DERIVE      PSA_KEY_USAGE_DERIVE
#define SIGN_VERIFY_ALGO      PSA_ALG_ECDSA(PSA_ALG_SHA_256)

#define CSR_PROTOCOL_BLE      0
#define CSR_PROTOCOL_BTMESH   1

// <<< Use Configuration Wizard in Context Menu >>>

// <q CSR_GENERATOR_CONFIG_GENERATE_STATIC_AUTH> Static Authentication Data
// <i> Generating Static Authentication Data.
#define CSR_GENERATOR_CONFIG_GENERATE_STATIC_AUTH               0

// <q CSR_GENERATOR_CONFIG_GENERATE_EC_KEY> Device EC key
// <i> Generating device EC key.
#define CSR_GENERATOR_CONFIG_GENERATE_EC_KEY                    1

// <q CSR_GENERATOR_CONFIG_ADD_STATIC_AUTH_TO_CSR> Static data to CSR
// <i> Add Static Auth data to CSR.
#define CSR_GENERATOR_CONFIG_ADD_STATIC_AUTH_TO_CSR             0

// <q CSR_GENERATOR_CONFIG_CERTIFICATE_ON_DEVICE> Certificate on device
// <i> Should the device hold the certificate or not.
#define CSR_GENERATOR_CONFIG_CERTIFICATE_ON_DEVICE              1

// <o CSR_GENERATOR_CSR_RAM_ADDRESS> Certificate Request RAM address
// <i> Default: 0x20010000
// <i> The RAM address where the Certificate Request is stored.
#define CSR_GENERATOR_CSR_RAM_ADDRESS                           0x20010000

// <o CSR_GENERATOR_NVM3_REGION> NVM3 key region to use
// <i> Default: 0x60000
// <i> The NVM3 key region which will be used to save security keys and certificates.
#define CSR_GENERATOR_NVM3_REGION                           0x60000

// <o CSR_GENERATOR_KEY_LOCATION> Key location
// <PSA_KEY_LOCATION_SLI_SE_OPAQUE=> Vault
// <PSA_KEY_LOCATION_LOCAL_STORAGE=> Local Storage
// <i> Default: PSA_KEY_LOCATION_LOCAL_STORAGE
#define CSR_GENERATOR_KEY_LOCATION                              PSA_KEY_LOCATION_LOCAL_STORAGE

// <o CSR_GENERATOR_EC_KEY_USAGE> Key Usage for EC key pair
// <KEY_USAGE_SIGN_VERIFY=> Use for message signing and verification
// <KEY_USAGE_DERIVE=> Use for key derivation
// <i> Default: PSA_KEY_LOCATION_LOCAL_STORAGE
#define CSR_GENERATOR_EC_KEY_USAGE KEY_USAGE_SIGN_VERIFY

// <o CSR_GENERATOR_EC_KEY_ALGO> Key algorithm
// <SIGN_VERIFY_ALGO=> Algorithm for sign and verification.
// <PSA_ALG_ECDH=> Algorithm for key derivation.
// <i> Default: SIGN_VERIFY_ALGO
#define CSR_GENERATOR_EC_KEY_ALGO SIGN_VERIFY_ALGO

// <h> Certification Subject Data

// <s CSR_GENERATOR_SUBJECT_COUNTRY> Country Identifier
// <i> Country Identifier for CSR generation.
#define CSR_GENERATOR_SUBJECT_COUNTRY    "FI"

// <s CSR_GENERATOR_SUBJECT_STATE> State Identifier
// <i> State Identifier for CSR generation.
#define CSR_GENERATOR_SUBJECT_STATE    "Uusimaa"

// <s CSR_GENERATOR_SUBJECT_LOCALITY> Locality Identifier
// <i> Locality Identifier for CSR generation.
#define CSR_GENERATOR_SUBJECT_LOCALITY    "Espoo"

// <s CSR_GENERATOR_SUBJECT_ORGANIZATION> Organization Identifier
// <i> Organization Unit Identifier for CSR generation.
#define CSR_GENERATOR_SUBJECT_ORGANIZATION    "Silicon Labs"

// <s CSR_GENERATOR_SUBJECT_ORGANIZATION_UNIT> Organization Unit Identifier
// <i> Organization Unit Identifier for CSR generation.
#define CSR_GENERATOR_SUBJECT_ORGANIZATION_UNIT    "Wireless"

// </h>

// <<< end of configuration section >>>

// Validate configuration
#if CSR_GENERATOR_CONFIG_ADD_STATIC_AUTH_TO_CSR == 1 && CSR_GENERATOR_CONFIG_CERTIFICATE_ON_DEVICE == 1
#error "Including static authentication data in the CSR and storing the " \
  "resulting certificate on the device is insecure and not permitted."
#endif

#endif // CSR_GENERATOR_CONFIG_H
