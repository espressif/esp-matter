/***************************************************************************//**
 * @file
 * @brief Tokens for the Manufacturing Library CLI plugin.
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

/**
 * Custom Application Tokens
 */
#define CREATOR_MFG_LIB_ENABLED  (0x000d)
#define NVM3KEY_MFG_LIB_ENABLED  (NVM3KEY_DOMAIN_USER | 0x000d)

#ifdef DEFINETYPES
// Include or define any typedef for tokens here
#endif //DEFINETYPES
#ifdef DEFINETOKENS
// Define the actual token storage information here

DEFINE_BASIC_TOKEN(MFG_LIB_ENABLED, uint8_t, 0)

#endif //DEFINETOKENS
