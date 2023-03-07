/***************************************************************************//**
 * @file
 * @brief Token defines for the Green Power Translation Table plugin.
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

#ifdef UC_BUILD
#include "green-power-translation-table-config.h"
#else // !UC_BUILD
// In Appbuilder, the EMBER_AF_PLUGIN_GREEN_POWER_TRANSLATION_TABLE_USE_TOKENS is defined
// (in auto generated AF header) if-and-only-if the token is enabled by user,
// so just redefine it to 1 else to 0.
#ifdef EMBER_AF_PLUGIN_GREEN_POWER_TRANSLATION_TABLE_USE_TOKENS
#undef EMBER_AF_PLUGIN_GREEN_POWER_TRANSLATION_TABLE_USE_TOKENS
#define EMBER_AF_PLUGIN_GREEN_POWER_TRANSLATION_TABLE_USE_TOKENS 1
#else
#define EMBER_AF_PLUGIN_GREEN_POWER_TRANSLATION_TABLE_USE_TOKENS 0
#endif // EMBER_AF_PLUGIN_GREEN_POWER_TRANSLATION_TABLE_USE_TOKENS
#endif // UC_BUILD

// Green Power Translation Table Tokens
#if (EMBER_AF_PLUGIN_GREEN_POWER_TRANSLATION_TABLE_USE_TOKENS == 1)

#define CREATOR_TRANSLATION_TABLE     (0x8729)
#define CREATOR_CUSTOMIZED_TABLE      (0x872A)
#define CREATOR_ADDITIONALINFO_TABLE  (0x872B)
#define CREATOR_ADDITIONALINFO_TABLE_VALID_ENTRIES  (0x872C)
#define CREATOR_TRANSLATION_TABLE_TOTAL_ENTRIES     (0x872D)
#define CREATOR_ADDITIONALINFO_TABLE_TOTAL_ENTRIES  (0x872E)

// NVM3 indexed tokens need special consideration for key number assigment
// Ref :
//     Section 3.1.2  Special Considerations for Indexed Tokens
//  of an1135-using-third-generation-nonvolatile-memory.pdf

// This key is used for an indexed token and the subsequent 0x7F keys are also reserved
#define NVM3KEY_TRANSLATION_TABLE    (NVM3KEY_DOMAIN_ZIGBEE | 0x8729)
// This key is used for an indexed token and the subsequent 0x7F keys are also reserved
#define NVM3KEY_CUSTOMIZED_TABLE     (NVM3KEY_DOMAIN_ZIGBEE | 0x87A9)
// This key is used for an indexed token and the subsequent 0x7F keys are also reserved
#define NVM3KEY_ADDITIONALINFO_TABLE (NVM3KEY_DOMAIN_ZIGBEE | 0x8829)
// This key is used for an indexed token and the subsequent 0x7F keys are also reserved
#define NVM3KEY_ADDITIONALINFO_TABLE_VALID_ENTRIES (NVM3KEY_DOMAIN_ZIGBEE | 0x88A9)

// Basic Tokens
#define NVM3KEY_TRANSLATION_TABLE_TOTAL_ENTRIES (NVM3KEY_DOMAIN_ZIGBEE | 0x8929)
#define NVM3KEY_ADDITIONALINFO_TABLE_TOTAL_ENTRIES (NVM3KEY_DOMAIN_ZIGBEE | 0x892A)

#ifdef DEFINETYPES
// Include or define any typedef for tokens here
#include "green-power-translation-table.h"
#endif //DEFINETYPES

#ifdef DEFINETOKENS
// Define the actual token storage information here

#ifndef UC_BUILD
// Not needed for the UC and will be cleaned up.
#ifndef EMBER_AF_PLUGIN_GREEN_POWER_TRANSLATION_TABLE_CUSTOMIZED_GPD_TRANSLATION_TABLE_SIZE
#define EMBER_AF_PLUGIN_GREEN_POWER_TRANSLATION_TABLE_CUSTOMIZED_GPD_TRANSLATION_TABLE_SIZE (20)
#endif

#ifndef EMBER_AF_PLUGIN_GREEN_POWER_TRANSLATION_TABLE_TRANSLATION_TABLE_SIZE
#define EMBER_AF_PLUGIN_GREEN_POWER_TRANSLATION_TABLE_TRANSLATION_TABLE_SIZE 30
#endif

#endif // !UC_BUILD

#define EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ADDITIONALINFO_TABLE_SIZE EMBER_AF_PLUGIN_GREEN_POWER_TRANSLATION_TABLE_TRANSLATION_TABLE_SIZE

DEFINE_INDEXED_TOKEN(TRANSLATION_TABLE,
                     EmGpCommandTranslationTableEntry,
                     EMBER_AF_PLUGIN_GREEN_POWER_TRANSLATION_TABLE_TRANSLATION_TABLE_SIZE,
                     { 0x00, 0x00, 0x00, 0x00, 0x00, { 0x00000000 }, 0x00, 0x00 })
DEFINE_INDEXED_TOKEN(CUSTOMIZED_TABLE,
                     EmberAfGreenPowerServerGpdSubTranslationTableEntry,
                     EMBER_AF_PLUGIN_GREEN_POWER_TRANSLATION_TABLE_CUSTOMIZED_GPD_TRANSLATION_TABLE_SIZE,
                     { 0x00, 0x00, 0x00, 0x0000, 0x0000, 0x00, 0x00, 0x00, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } })
DEFINE_INDEXED_TOKEN(ADDITIONALINFO_TABLE,
                     EmberGpTranslationTableAdditionalInfoBlockOptionRecordField,
                     EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ADDITIONALINFO_TABLE_SIZE,
                     { 0x00, 0x00, { 0x00, 0x00, 0x0000, 0x0000, 0x00, 0x00, 0x0000 } })
DEFINE_INDEXED_TOKEN(ADDITIONALINFO_TABLE_VALID_ENTRIES,
                     uint8_t,
                     EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ADDITIONALINFO_TABLE_SIZE,
                     { 0x00 })
DEFINE_BASIC_TOKEN(TRANSLATION_TABLE_TOTAL_ENTRIES, uint8_t, 0x00)
DEFINE_BASIC_TOKEN(ADDITIONALINFO_TABLE_TOTAL_ENTRIES, uint8_t, 0x00)
#endif //DEFINETOKENS
#endif //EMBER_AF_PLUGIN_GREEN_POWER_TRANSLATION_TABLE_USE_TOKENS
