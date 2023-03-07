/***************************************************************************//**
 * @file
 * @brief Multi PAN token definitions used by the stack.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

// Noted: Our current multi-network feature is restricted to have 2 networks,
// check the definition of EMBER_SUPPORTED_NETWORKS. So here we only define
// one more set of tokens for child tables. In the future, if we supports more
// networks we could add more sets of tokens here.
// In addition, the max array size of indexed tokens is 127 and the max child
// table size is 64, so we cannot simply use 1 indexed token for 2 child tables.

// MULTI-PAN CREATORS
#define CREATOR_MULTI_PAN_STACK_CHILD_TABLE                  0xE550
#define CREATOR_MULTI_PAN_STACK_ADDITIONAL_CHILD_DATA        0xE551

// MULTI-PAN KEYS
// This key is used for an indexed token and the subsequent 0x7F keys are also reserved.
#define NVM3KEY_MULTI_PAN_STACK_CHILD_TABLE                (NVM3KEY_DOMAIN_ZIGBEE | 0x0800)
// This key is used for an indexed token and the subsequent 0x7F keys are also reserved.
#define NVM3KEY_MULTI_PAN_STACK_ADDITIONAL_CHILD_DATA      (NVM3KEY_DOMAIN_ZIGBEE | 0x0880)

// We only reserve token space for multi PAN child table
// when multiple ZC and ZR devices devices are present.
#ifdef UC_BUILD
#if (EMBER_ZC_AND_ZR_DEVICE_COUNT > 1)
  #define EMBER_MULTI_PAN_CHILD_TABLE_TOKEN_SIZE EMBER_CHILD_TABLE_SIZE
#else
  #define EMBER_MULTI_PAN_CHILD_TABLE_TOKEN_SIZE 0
#endif
#else // !UC_BUILD
#if (EMBER_AF_ZC_AND_ZR_DEVICE_COUNT > 1)
  #define EMBER_MULTI_PAN_CHILD_TABLE_TOKEN_SIZE EMBER_CHILD_TABLE_SIZE
#else
  #define EMBER_MULTI_PAN_CHILD_TABLE_TOKEN_SIZE 0
#endif
#endif // UC_BUILD

#ifdef DEFINETOKENS
DEFINE_INDEXED_TOKEN(MULTI_PAN_STACK_CHILD_TABLE,
                     tokTypeStackChildTable,
                     EMBER_MULTI_PAN_CHILD_TABLE_TOKEN_SIZE,
                     { 0, })
DEFINE_INDEXED_TOKEN(MULTI_PAN_STACK_ADDITIONAL_CHILD_DATA,
                     tokTypeStackAdditionalChildData,
                     EMBER_MULTI_PAN_CHILD_TABLE_TOKEN_SIZE,
                     { 0x0F, })
#endif //DEFINETOKENS
