/***************************************************************************//**
 * @file
 * @brief Tokens for the Scenes plugin.
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
#include "scenes-config.h"
#else // !UC_BUILD
// In Appbuilder, the EMBER_AF_PLUGIN_SCENES_USE_TOKENS is defined
// (in auto generated AF header) if-and-only-if the token is enabled by user,
// so just redefine it to 1 else to 0.
#ifdef EMBER_AF_PLUGIN_SCENES_USE_TOKENS
#undef EMBER_AF_PLUGIN_SCENES_USE_TOKENS
#define EMBER_AF_PLUGIN_SCENES_USE_TOKENS 1
#else
#define EMBER_AF_PLUGIN_SCENES_USE_TOKENS 0
#endif // EMBER_AF_PLUGIN_SCENES_USE_TOKENS
#endif // UC_BUILD

#if (EMBER_AF_PLUGIN_SCENES_USE_TOKENS == 1)

#define CREATOR_SCENES_NUM_ENTRIES  (0x8723)
#define NVM3KEY_SCENES_NUM_ENTRIES (NVM3KEY_DOMAIN_ZIGBEE | 0x8723)

#define CREATOR_SCENES_TABLE        (0x8724)
// This key is used for an indexed token and the subsequent 0x7F keys are also reserved
#define NVM3KEY_SCENES_TABLE       (NVM3KEY_DOMAIN_ZIGBEE | 0x4080)

#ifdef DEFINETYPES
// Include or define any typedef for tokens here
#endif //DEFINETYPES
#ifdef DEFINETOKENS
// Define the actual token storage information here

DEFINE_BASIC_TOKEN(SCENES_NUM_ENTRIES, uint8_t, 0x00)
DEFINE_INDEXED_TOKEN(SCENES_TABLE,
                     EmberAfSceneTableEntry,
                     EMBER_AF_PLUGIN_SCENES_TABLE_SIZE,
                     { EMBER_AF_SCENE_TABLE_UNUSED_ENDPOINT_ID })
#endif //DEFINETOKENS

#endif // (EMBER_AF_PLUGIN_SCENES_USE_TOKENS == 1)
