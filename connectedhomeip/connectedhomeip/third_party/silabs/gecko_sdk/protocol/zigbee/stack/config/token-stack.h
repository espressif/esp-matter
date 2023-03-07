/***************************************************************************//**
 * @file
 * @brief Definitions for stack tokens.
 * See @ref token_stack for documentation.
 *
 * The file token-stack.h should not be included directly.
 * It is accessed by the other token files.
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
 * @addtogroup token_stack
 *
 * These tokens are divided into three sections (the three main
 * types of tokens mentioned in token.h):
 * - manufacturing
 * - stack
 * - application
 *
 * For a full explanation of the tokens, see hal/micro/token.h.
 * See token-stack.h for source code.
 *
 * A set of tokens is predefined in the APPLICATION DATA section at the
 * end of token-stack.h because these tokens are required by the stack,
 * but they are classified as application tokens since they are sized by the
 * application via its CONFIGURATION_HEADER.
 *
 * The user application can include its own tokens in a header file similar
 * to this one. The macro ::APPLICATION_TOKEN_HEADER should be defined to equal
 * the name of the header file in which application tokens are defined.
 * See the APPLICATION DATA section at the end of token-stack.h
 * for examples of token definitions.
 *
 * Since token-stack.h contains both the typedefs and the token defs, two
 * \#defines are used to select which one is needed when this file is included.
 * \#define DEFINETYPES is used to select the type definitions and
 * \#define DEFINETOKENS is used to select the token definitions.
 * See token.h and token.c to see how these are used.
 *
 * @{
 */

/**
 * @brief By default, tokens are automatically located after the previous token.
 *
 * If a token needs to be placed at a specific location,
 * one of the DEFINE_FIXED_* definitions should be used. This macro is
 * inherently used in the DEFINE_FIXED_* definition to locate a token and
 * under special circumstances (such as manufacturing tokens) it may be
 * explicitly used.
 *
 * @param region   A name for the next region being located.
 * @param address  The address of the beginning of the next region.
 */
  #ifndef DEFINEADDRESSES
  #define TOKEN_NEXT_ADDRESS(region, address)
  #endif

// The basic TOKEN_DEF macro should not be used directly since the simplified
//  definitions are safer to use. For completeness of information, the basic
//  macro has the following format:
//
//  TOKEN_DEF(name,creator,iscnt,isidx,type,arraysize,...)
//  name - The root name used for the token
//  creator - a code used to uniquely identify the token when using SimEE
//  nvm3key - a key used to uniquely identify the token when using NVM3
//  iscnt - a bool flag that is set to identify a counter token
//  isidx - a bool flag that is set to identify an indexed token
//  type - the basic type or typdef of the token
//  arraysize - the number of elements making up an indexed token
//  ... - initializers used when reseting the tokens to default values
//
//
// The following convenience macros are used to simplify the definition
//  process for commonly specified parameters to the basic TOKEN_DEF macro.
//  DEFINE_BASIC_TOKEN(name, type, ...)
//  DEFINE_INDEXED_TOKEN(name, type, arraysize, ...)
//  DEFINE_COUNTER_TOKEN(name, type, ...)
//  DEFINE_FIXED_BASIC_TOKEN(name, type, address, ...)
//  DEFINE_FIXED_INDEXED_TOKEN(name, type, arraysize, address, ...)
//  DEFINE_FIXED_COUNTER_TOKEN(name, type, address, ...)
//  DEFINE_MFG_TOKEN(name, type, address, ...)
//

/**
 * @name Convenience Macros
 * @brief The following convenience macros are used to simplify the definition
 * process for commonly specified parameters to the basic TOKEN_DEF macro.
 * See hal/micro/token.h for a more complete explanation.
 *@{
 */
#define DEFINE_BASIC_TOKEN(name, type, ...) \
  TOKEN_DEF(name, CREATOR_##name, 0, 0, type, 1, __VA_ARGS__)

#define DEFINE_COUNTER_TOKEN(name, type, ...) \
  TOKEN_DEF(name, CREATOR_##name, 1, 0, type, 1, __VA_ARGS__)

#define DEFINE_INDEXED_TOKEN(name, type, arraysize, ...) \
  TOKEN_DEF(name, CREATOR_##name, 0, 1, type, (arraysize), __VA_ARGS__)

#define DEFINE_FIXED_BASIC_TOKEN(name, type, address, ...) \
  TOKEN_NEXT_ADDRESS(name, (address))                      \
  TOKEN_DEF(name, CREATOR_##name, 0, 0, type, 1, __VA_ARGS__)

#define DEFINE_FIXED_COUNTER_TOKEN(name, type, address, ...) \
  TOKEN_NEXT_ADDRESS(name, (address))                        \
  TOKEN_DEF(name, CREATOR_##name, 1, 0, type, 1, __VA_ARGS__)

#define DEFINE_FIXED_INDEXED_TOKEN(name, type, arraysize, address, ...) \
  TOKEN_NEXT_ADDRESS(name, (address))                                   \
  TOKEN_DEF(name, CREATOR_##name, 0, 1, type, (arraysize), __VA_ARGS__)

#define DEFINE_MFG_TOKEN(name, type, address, ...) \
  TOKEN_NEXT_ADDRESS(name, (address))              \
  TOKEN_MFG(name, CREATOR_##name, 0, 0, type, 1, __VA_ARGS__)

/** @} END Convenience Macros */

// The Simulated EEPROM and NVM3 unit tests define their own tokens.
#if !defined(SIM_EEPROM_TEST) && !defined(NVM3_TEST)

// The creator codes are here in one list instead of next to their token
// definitions so comparison of the codes is easier. The only requirement
// on these creator definitions is that they all must be unique. A favorite
// method for picking creator codes is to use two ASCII characters
// to make the codes more memorable.

/**
 * @name Creator Codes
 * @brief The CREATOR is used as a distinct identifier tag for the
 * token.
 *
 * The CREATOR is necessary because the token name is defined
 * differently depending on the hardware platform. Therefore, the CREATOR
 * ensures that token definitions and data stay tagged and known. The only
 * requirement is that each creator definition must be unique.
 * See hal/micro/token.h for a more complete explanation.
 *@{
 */

// STACK CREATORS
#define CREATOR_STACK_NVDATA_VERSION                         0xFF01
#define CREATOR_STACK_BOOT_COUNTER                           0xE263
#define CREATOR_STACK_NONCE_COUNTER                          0xE563
#define CREATOR_STACK_ANALYSIS_REBOOT                        0xE162
#define CREATOR_STACK_KEYS                                   0xEB79
#define CREATOR_STACK_NODE_DATA                              0xEE64
#define CREATOR_STACK_CLASSIC_DATA                           0xE364
#define CREATOR_STACK_ALTERNATE_KEY                          0xE475
#define CREATOR_STACK_APS_FRAME_COUNTER                      0xE123
#define CREATOR_STACK_TRUST_CENTER                           0xE124
#define CREATOR_STACK_NETWORK_MANAGEMENT                     0xE125
#define CREATOR_STACK_PARENT_INFO                            0xE126
#define CREATOR_STACK_PARENT_ADDITIONAL_INFO                 0xE127
#define CREATOR_STACK_MULTI_PHY_NWK_INFO                     0xE128
#define CREATOR_STACK_MIN_RECEIVED_RSSI                      0xE129
// Restored EUI64
#define CREATOR_STACK_RESTORED_EUI64                         0xE12A

// MULTI-NETWORK STACK CREATORS
#define CREATOR_MULTI_NETWORK_STACK_KEYS                     0xE210
#define CREATOR_MULTI_NETWORK_STACK_NODE_DATA                0xE211
#define CREATOR_MULTI_NETWORK_STACK_ALTERNATE_KEY            0xE212
#define CREATOR_MULTI_NETWORK_STACK_TRUST_CENTER             0xE213
#define CREATOR_MULTI_NETWORK_STACK_NETWORK_MANAGEMENT       0xE214
#define CREATOR_MULTI_NETWORK_STACK_PARENT_INFO              0xE215

// A temporary solution for multi-network nwk counters:
// This counter will be used on the network with index 1.
#define CREATOR_MULTI_NETWORK_STACK_NONCE_COUNTER            0xE220
#define CREATOR_MULTI_NETWORK_STACK_PARENT_ADDITIONAL_INFO   0xE221

// GP stack tokens.
#define CREATOR_STACK_GP_DATA                                0xE258
#define CREATOR_STACK_GP_PROXY_TABLE                         0xE259
#define CREATOR_STACK_GP_SINK_TABLE                          0xE25A
#define CREATOR_STACK_GP_INCOMING_FC                         0xE25B
#define CREATOR_STACK_GP_INCOMING_FC_IN_SINK                 0xE25C
// APP CREATORS
#define CREATOR_STACK_BINDING_TABLE                          0xE274
#define CREATOR_STACK_CHILD_TABLE                            0xFF0D
#define CREATOR_STACK_KEY_TABLE                              0xE456
#define CREATOR_STACK_CERTIFICATE_TABLE                      0xE500
#define CREATOR_STACK_ZLL_DATA                               0xE501
#define CREATOR_STACK_ZLL_SECURITY                           0xE502
#define CREATOR_STACK_ADDITIONAL_CHILD_DATA                  0xE503

/** @} END Creator Codes  */

/**
 * @name NVM3 Object Keys
 * @brief The NVM3 object key is used as a distinct identifier tag for a
 * token stored in NVM3.
 *
 * Every token must have a defined NVM3 object key and the object key
 * must be unique. The object key defined must be in the following
 * format:
 * NVM3KEY_tokenname
 *
 * where tokenname is the name of the token without NVM3KEY_ or TOKEN_
 * prefix.

 *@{
 */

// NVM3KEY domain base keys
#define NVM3KEY_DOMAIN_USER    0x00000U
#define NVM3KEY_DOMAIN_ZIGBEE  0x10000U
#define NVM3KEY_DOMAIN_COMMON  0x80000U

// STACK KEYS
#define NVM3KEY_STACK_NVDATA_VERSION                       (NVM3KEY_DOMAIN_ZIGBEE | 0xFF01)
#define NVM3KEY_STACK_BOOT_COUNTER                         (NVM3KEY_DOMAIN_ZIGBEE | 0xE263)
#define NVM3KEY_STACK_NONCE_COUNTER                        (NVM3KEY_DOMAIN_ZIGBEE | 0xE563)
#define NVM3KEY_STACK_ANALYSIS_REBOOT                      (NVM3KEY_DOMAIN_ZIGBEE | 0xE162)
#define NVM3KEY_STACK_KEYS                                 (NVM3KEY_DOMAIN_ZIGBEE | 0xEB79)
#define NVM3KEY_STACK_NODE_DATA                            (NVM3KEY_DOMAIN_ZIGBEE | 0xEE64)
#define NVM3KEY_STACK_CLASSIC_DATA                         (NVM3KEY_DOMAIN_ZIGBEE | 0xE364)
#define NVM3KEY_STACK_ALTERNATE_KEY                        (NVM3KEY_DOMAIN_ZIGBEE | 0xE475)
#define NVM3KEY_STACK_APS_FRAME_COUNTER                    (NVM3KEY_DOMAIN_ZIGBEE | 0xE123)
#define NVM3KEY_STACK_TRUST_CENTER                         (NVM3KEY_DOMAIN_ZIGBEE | 0xE124)
#define NVM3KEY_STACK_NETWORK_MANAGEMENT                   (NVM3KEY_DOMAIN_ZIGBEE | 0xE125)
#define NVM3KEY_STACK_PARENT_INFO                          (NVM3KEY_DOMAIN_ZIGBEE | 0xE126)
#define NVM3KEY_STACK_PARENT_ADDITIONAL_INFO               (NVM3KEY_DOMAIN_ZIGBEE | 0xE127)
#define NVM3KEY_STACK_MULTI_PHY_NWK_INFO                   (NVM3KEY_DOMAIN_ZIGBEE | 0xE128)
#define NVM3KEY_STACK_MIN_RECEIVED_RSSI                    (NVM3KEY_DOMAIN_ZIGBEE | 0xE129)
// Restored EUI64
#define NVM3KEY_STACK_RESTORED_EUI64                       (NVM3KEY_DOMAIN_ZIGBEE | 0xE12A)

// MULTI-NETWORK STACK KEYS
// This key is used for an indexed token and the subsequent 0x7F keys are also reserved.
#define NVM3KEY_MULTI_NETWORK_STACK_KEYS                   (NVM3KEY_DOMAIN_ZIGBEE | 0x0000)
// This key is used for an indexed token and the subsequent 0x7F keys are also reserved.
#define NVM3KEY_MULTI_NETWORK_STACK_NODE_DATA              (NVM3KEY_DOMAIN_ZIGBEE | 0x0080)
// This key is used for an indexed token and the subsequent 0x7F keys are also reserved.
#define NVM3KEY_MULTI_NETWORK_STACK_ALTERNATE_KEY          (NVM3KEY_DOMAIN_ZIGBEE | 0x0100)
// This key is used for an indexed token and the subsequent 0x7F keys are also reserved.
#define NVM3KEY_MULTI_NETWORK_STACK_TRUST_CENTER           (NVM3KEY_DOMAIN_ZIGBEE | 0x0180)
// This key is used for an indexed token and the subsequent 0x7F keys are also reserved.
#define NVM3KEY_MULTI_NETWORK_STACK_NETWORK_MANAGEMENT     (NVM3KEY_DOMAIN_ZIGBEE | 0x0200)
// This key is used for an indexed token and the subsequent 0x7F keys are also reserved.
#define NVM3KEY_MULTI_NETWORK_STACK_PARENT_INFO            (NVM3KEY_DOMAIN_ZIGBEE | 0x0280)

// Temporary solution for multi-network nwk counters:
// This counter will be used on the network with index 1.
#define NVM3KEY_MULTI_NETWORK_STACK_NONCE_COUNTER          (NVM3KEY_DOMAIN_ZIGBEE | 0xE220)
// This key is used for an indexed token and the subsequent 0x7F keys are also reserved
#define NVM3KEY_MULTI_NETWORK_STACK_PARENT_ADDITIONAL_INFO (NVM3KEY_DOMAIN_ZIGBEE | 0x0300)

// GP stack tokens.
#define NVM3KEY_STACK_GP_DATA                              (NVM3KEY_DOMAIN_ZIGBEE | 0xE258)
// This key is used for an indexed token and the subsequent 0x7F keys are also reserved.
#define NVM3KEY_STACK_GP_PROXY_TABLE                       (NVM3KEY_DOMAIN_ZIGBEE | 0x0380)
// This key is used for an indexed token and the subsequent 0x7F keys are also reserved.
#define NVM3KEY_STACK_GP_SINK_TABLE                        (NVM3KEY_DOMAIN_ZIGBEE | 0x0400)
// This key is used for an indexed token and the subsequent 0x7F keys are also reserved
#define NVM3KEY_STACK_GP_INCOMING_FC                       (NVM3KEY_DOMAIN_ZIGBEE | 0x0480)

// APP KEYS
// This key is used for an indexed token and the subsequent 0x7F keys are also reserved.
#define NVM3KEY_STACK_BINDING_TABLE                        (NVM3KEY_DOMAIN_ZIGBEE | 0x0500)
// This key is used for an indexed token and the subsequent 0x7F keys are also reserved.
#define NVM3KEY_STACK_CHILD_TABLE                          (NVM3KEY_DOMAIN_ZIGBEE | 0x0580)
// This key is used for an indexed token and the subsequent 0x7F keys are also reserved.
#define NVM3KEY_STACK_KEY_TABLE                            (NVM3KEY_DOMAIN_ZIGBEE | 0x0600)
// This key is used for an indexed token and the subsequent 0x7F keys are also reserved.
#define NVM3KEY_STACK_CERTIFICATE_TABLE                    (NVM3KEY_DOMAIN_ZIGBEE | 0x0680)
#define NVM3KEY_STACK_ZLL_DATA                             (NVM3KEY_DOMAIN_ZIGBEE | 0xE501)
#define NVM3KEY_STACK_ZLL_SECURITY                         (NVM3KEY_DOMAIN_ZIGBEE | 0xE502)
// This key is used for an indexed token and the subsequent 0x7F keys are also reserved.
#define NVM3KEY_STACK_ADDITIONAL_CHILD_DATA                (NVM3KEY_DOMAIN_ZIGBEE | 0x0700)

// This key is used for an indexed token and the subsequent 0x7F keys are also reserved
#define NVM3KEY_STACK_GP_INCOMING_FC_IN_SINK               (NVM3KEY_DOMAIN_ZIGBEE | 0x0780)
/** @} END NVM3 Object Keys  */

//////////////////////////////////////////////////////////////////////////////
// MANUFACTURING DATA
// Pull in the platform-specific manufacturing tokens
#if defined(CORTEXM3)
// Cortex-M3 handles MFG tokens separately via mfg-token.h.
#elif defined(EZSP_HOST) || defined(ZIGBEE_STACK_ON_HOST)
// No MFG tokens on hosts or Zigbeed.
#elif defined(EMBER_TEST)
  #include "hal/micro/unix/simulation/token-manufacturing.h"
#else
  #error no platform defined
#endif

//////////////////////////////////////////////////////////////////////////////
// STACK DATA
// *The addresses of these tokens must not change.*

/**
 * @brief The current version number of the stack tokens.
 * MSB is the version, LSB is a complement.
 *
 * See hal/micro/token.h for a more complete explanation.
 */
#define CURRENT_STACK_TOKEN_VERSION 0x03FC //MSB is version, LSB is complement.

#ifdef DEFINETYPES
typedef uint16_t tokTypeStackNvdataVersion;
typedef uint32_t tokTypeStackBootCounter;
//This was introduced to save the type of keep alives supported by the parent.
//Bits 0-2 are currently in use, bit 2 covers power negotiation.
//Bits 3-14 are reserved. Bit 15 is used in alt-mac
typedef uint16_t tokTypeStackParentAdditionalInfo;
typedef uint16_t tokTypeStackAnalysisReboot;
typedef uint32_t tokTypeStackNonceCounter;
typedef int8_t tokTypeRSSI;
typedef struct {
  uint8_t networkKey[16];
  uint8_t activeKeySeqNum;
} tokTypeStackKeys;
typedef struct {
  uint16_t panId;
  int8_t radioTxPower;
  uint8_t radioFreqChannel;
  uint8_t stackProfile;
  uint8_t nodeType;
  uint16_t zigbeeNodeId;
  uint8_t extendedPanId[8];
} tokTypeStackNodeData;
typedef struct {
  uint16_t mode;
  uint8_t eui64[8];
  uint8_t key[16];
} tokTypeStackTrustCenter;
typedef struct {
  uint32_t activeChannels;
  uint16_t managerNodeId;
  uint8_t updateId;
} tokTypeStackNetworkManagement;
typedef struct {
  uint8_t parentEui[8];
  uint16_t parentNodeId;
} tokTypeStackParentInfo;
typedef struct {
  int8_t radioTxPower;
  uint8_t radioFreqChannel;
  uint32_t pg28ChannelMask;
  uint32_t pg29ChannelMask;
  uint32_t pg30ChannelMask;
  uint32_t pg31ChannelMask;
  uint8_t optionsMask;
} tokTypeStackMultiPhyNwkInfo;
typedef struct {
  uint8_t restoredEui[8];
} tokTypeStackRestoredEui64;
#endif //DEFINETYPES

#ifdef DEFINETOKENS
// The stack tokens also need to be stored at well-defined locations.
//  None of these addresses should ever change without extremely great care.
#define STACK_VERSION_LOCATION    128 //  2   bytes
#define STACK_APS_NONCE_LOCATION  130 //  4   bytes
#define STACK_ALT_NWK_KEY_LOCATION 134 // 17  bytes (key + sequence number)
// reserved                       151     1   bytes
#define STACK_BOOT_COUNT_LOCATION 152 //  2   bytes
// reserved                       154     2   bytes
#define STACK_NONCE_LOCATION      156 //  4   bytes
// reserved                       160     1   bytes
#define STACK_REBOOT_LOCATION     161 //  2   bytes
// reserved                       163     7   bytes
#define STACK_KEYS_LOCATION       170 //  17  bytes
// reserved                       187     5   bytes
#define STACK_NODE_DATA_LOCATION  192 //  16  bytes
#define STACK_CLASSIC_LOCATION    208 //  26  bytes
#define STACK_TRUST_CENTER_LOCATION 234 //26  bytes
// reserved                       260     8   bytes
#define STACK_NETWORK_MANAGEMENT_LOCATION 268 //  7   bytes
#define STACK_PARENT_INFO_LOCATION            276 //10 bytes
#define STACK_PARENT_ADDITIONAL_INFO_LOCATION 286 //2 bytes
#define STACK_MULTI_PHY_NWK_INFO_LOCATION  288    // 19 bytes
#define STACK_MIN_RECEIVED_RSSI_LOCATION    307 // 1 byte
#define STACK_RESTORED_EUI64_LOCATION       308 // 8 bytes
// reserved                       316     98 bytes

DEFINE_FIXED_BASIC_TOKEN(STACK_NVDATA_VERSION,
                         tokTypeStackNvdataVersion,
                         STACK_VERSION_LOCATION,
                         CURRENT_STACK_TOKEN_VERSION)
DEFINE_FIXED_COUNTER_TOKEN(STACK_APS_FRAME_COUNTER,
                           tokTypeStackNonceCounter,
                           STACK_APS_NONCE_LOCATION,
                           0x00000000)
DEFINE_FIXED_BASIC_TOKEN(STACK_ALTERNATE_KEY,
                         tokTypeStackKeys,
                         STACK_ALT_NWK_KEY_LOCATION,
                         { { 0, } })
DEFINE_FIXED_COUNTER_TOKEN(STACK_BOOT_COUNTER,
                           tokTypeStackBootCounter,
                           STACK_BOOT_COUNT_LOCATION,
                           0x0000)
DEFINE_FIXED_COUNTER_TOKEN(STACK_NONCE_COUNTER,
                           tokTypeStackNonceCounter,
                           STACK_NONCE_LOCATION,
                           0x00000000)
DEFINE_FIXED_BASIC_TOKEN(STACK_ANALYSIS_REBOOT,
                         tokTypeStackAnalysisReboot,
                         STACK_REBOOT_LOCATION,
                         0x0000)
DEFINE_FIXED_BASIC_TOKEN(STACK_KEYS,
                         tokTypeStackKeys,
                         STACK_KEYS_LOCATION,
                         { { 0, } })
DEFINE_FIXED_BASIC_TOKEN(STACK_NODE_DATA,
                         tokTypeStackNodeData,
                         STACK_NODE_DATA_LOCATION,
                         { 0xFFFF, -1, 0, 0x00, 0x00, 0x0000 })
DEFINE_FIXED_BASIC_TOKEN(STACK_TRUST_CENTER,
                         tokTypeStackTrustCenter,
                         STACK_TRUST_CENTER_LOCATION,
                         { 0, })
DEFINE_FIXED_BASIC_TOKEN(STACK_NETWORK_MANAGEMENT,
                         tokTypeStackNetworkManagement,
                         STACK_NETWORK_MANAGEMENT_LOCATION,
                         { 0, 0xFFFF, 0 })
DEFINE_FIXED_BASIC_TOKEN(STACK_PARENT_INFO,
                         tokTypeStackParentInfo,
                         STACK_PARENT_INFO_LOCATION,
                         { { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, 0xFFFF })
DEFINE_FIXED_BASIC_TOKEN(STACK_PARENT_ADDITIONAL_INFO,
                         tokTypeStackParentAdditionalInfo,
                         STACK_PARENT_ADDITIONAL_INFO_LOCATION,
                         0x0000)
DEFINE_FIXED_BASIC_TOKEN(STACK_MULTI_PHY_NWK_INFO,
                         tokTypeStackMultiPhyNwkInfo,
                         STACK_MULTI_PHY_NWK_INFO_LOCATION,
                         { -1, 0, 0, 0, 0, 0, 0x80 })
DEFINE_FIXED_BASIC_TOKEN(STACK_MIN_RECEIVED_RSSI,
                         tokTypeRSSI,
                         STACK_MIN_RECEIVED_RSSI_LOCATION,
                         { -100, })
DEFINE_FIXED_BASIC_TOKEN(STACK_RESTORED_EUI64,
                         tokTypeStackRestoredEui64,
                         STACK_RESTORED_EUI64_LOCATION,
                         { { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF } })
#endif //DEFINETOKENS

//////////////////////////////////////////////////////////////////////////////
// PHY DATA
//
#include "token-phy.h"

//////////////////////////////////////////////////////////////////////////////
// MULTI-NETWORK STACK TOKENS: Tokens for the networks with index > 0.
// The 0-index network info is stored in the usual tokens.

#ifdef DEFINETOKENS
#ifndef SMART_ENERGY_STACK_TOKENS
//#if !defined(EMBER_MULTI_NETWORK_STRIPPED)
// EMBER_MULTI_NETWORK_STRIPPED is commented out to work around
// EMZIGBEE-2676 / EMHAL-1530.  This header is used somewhere
// that doesn't include ember.h, so it leads to conflicting
// definitions. Eating the extra tokens is much easier than double
// building every library that touches a token.
#define EXTRA_NETWORKS_NUMBER (EMBER_SUPPORTED_NETWORKS - 1)
DEFINE_INDEXED_TOKEN(MULTI_NETWORK_STACK_KEYS,
                     tokTypeStackKeys,
                     EXTRA_NETWORKS_NUMBER,
                     { { 0, } })
DEFINE_INDEXED_TOKEN(MULTI_NETWORK_STACK_NODE_DATA,
                     tokTypeStackNodeData,
                     EXTRA_NETWORKS_NUMBER,
                     { 0, })
DEFINE_INDEXED_TOKEN(MULTI_NETWORK_STACK_ALTERNATE_KEY,
                     tokTypeStackKeys,
                     EXTRA_NETWORKS_NUMBER,
                     { { 0, } })
DEFINE_INDEXED_TOKEN(MULTI_NETWORK_STACK_TRUST_CENTER,
                     tokTypeStackTrustCenter,
                     EXTRA_NETWORKS_NUMBER,
                     { 0, })
DEFINE_INDEXED_TOKEN(MULTI_NETWORK_STACK_NETWORK_MANAGEMENT,
                     tokTypeStackNetworkManagement,
                     EXTRA_NETWORKS_NUMBER,
                     { 0, })
DEFINE_INDEXED_TOKEN(MULTI_NETWORK_STACK_PARENT_INFO,
                     tokTypeStackParentInfo,
                     EXTRA_NETWORKS_NUMBER,
                     { { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, 0xFFFF })
// Temporary solution for NWK counter token: the following is used for 1-index
// network.
DEFINE_COUNTER_TOKEN(MULTI_NETWORK_STACK_NONCE_COUNTER,
                     tokTypeStackNonceCounter,
                     0x00000000)
DEFINE_INDEXED_TOKEN(MULTI_NETWORK_STACK_PARENT_ADDITIONAL_INFO,
                     tokTypeStackParentAdditionalInfo,
                     EXTRA_NETWORKS_NUMBER,
                     { 0, })
//#endif // EMBER_MULTI_NETWORK_STRIPPED
#endif // SMART_ENERGY_STACK_TOKENS
#endif // DEFINETOKENS

//////////////////////////////////////////////////////////////////////////////
// APPLICATION DATA
// *If a fixed application token is desired, its address must be above 384.*

#ifdef DEFINETYPES
typedef uint8_t tokTypeStackBindingTable[13];
typedef uint8_t tokTypeStackChildTable[11];
typedef uint8_t tokTypeStackKeyTable[25];
typedef uint8_t tokTypeStackAdditionalChildData;
// Certificate Table Entry
//   Certificate:    48-bytes
//   CA Public Key:  22-bytes
//   Private Key:    21-bytes
//   Flags:          1-byte
#define TOKEN_CERTIFICATE_TABLE_ENTRY_SIZE (48 + 22 + 21 + 1)
#define TOKEN_CERTIFICATE_TABLE_ENTRY_FLAGS_INDEX (TOKEN_CERTIFICATE_TABLE_ENTRY_SIZE - 1)
typedef uint8_t tokTypeStackCertificateTable[TOKEN_CERTIFICATE_TABLE_ENTRY_SIZE];
#endif //DEFINETYPES

// The following application tokens are required by the stack, but are sized by
//  the application via its CONFIGURATION_HEADER, which is why they are present
//  within the application data section. Any special application-defined
//  tokens will follow.
// NOTE: changing the size of these tokens within the CONFIGURATION_HEADER
//  WILL automatically move any custom application tokens that are defined
//  in the APPLICATION_TOKEN_HEADER.
#ifdef DEFINETOKENS
// Application tokens start at location 384 and are automatically positioned.
TOKEN_NEXT_ADDRESS(APP, 384)
DEFINE_INDEXED_TOKEN(STACK_BINDING_TABLE,
                     tokTypeStackBindingTable,
                     EMBER_BINDING_TABLE_SIZE,
                     { 0, })
DEFINE_INDEXED_TOKEN(STACK_CHILD_TABLE,
                     tokTypeStackChildTable,
                     EMBER_CHILD_TABLE_SIZE,
                     { 0, })
DEFINE_INDEXED_TOKEN(STACK_KEY_TABLE,
                     tokTypeStackKeyTable,
                     EMBER_KEY_TABLE_SIZE,
                     { 0, })
DEFINE_INDEXED_TOKEN(STACK_CERTIFICATE_TABLE,
                     tokTypeStackCertificateTable,
                     EMBER_CERTIFICATE_TABLE_SIZE,
                     { 0, })
DEFINE_INDEXED_TOKEN(STACK_ADDITIONAL_CHILD_DATA,
                     tokTypeStackAdditionalChildData,
                     EMBER_CHILD_TABLE_SIZE,
                     { 0x0F, })
#endif //DEFINETOKENS

#ifndef SMART_ENERGY_STACK_TOKENS
// These must appear before the application header so that the token
// numbering is consistent regardless of whether application tokens are
// defined.
//#if defined(EMBER_AF_PLUGIN_ZLL_LIBRARY) || defined(EMBER_TEST)
  #include "stack/zll/zll-token-config.h"
//#endif
#ifdef UC_BUILD
  #if defined(SL_CATALOG_ZIGBEE_GREEN_POWER_PRESENT) || defined(EMBER_TEST)
    #include "stack/gp/gp-token-config.h"
  #endif // (SL_CATALOG_ZIGBEE_GREEN_POWER_PRESENT) || (EMBER_TEST)
#else // !UC_BUILD
//#if defined(EMBER_AF_PLUGIN_GP_LIBRARY) || defined(EMBER_TEST)
  #include "stack/gp/gp-token-config.h"
//#endif
#endif // UC_BUILD
  #include "stack/core/multi-pan-token-config.h"
#endif  // SMART_ENERGY_STACK_TOKENS

#ifdef UC_BUILD
 #ifdef SL_CATALOG_ZIGBEE_ZCL_FRAMEWORK_CORE_PRESENT
   #include "app/framework/util/af-token.h"
 #endif
 #ifdef SL_CATALOG_ZIGBEE_NCP_FRAMEWORK_PRESENT
   #include "app/framework/util/af-ncp-token.h"
 #endif
#else
 #ifdef APPLICATION_TOKEN_HEADER
  #include APPLICATION_TOKEN_HEADER
 #endif
#endif // UC_BUILD

//The tokens defined below are test tokens. They are normally not used
//but are left here as a convenience so test tokens do not have to
//be recreated. If test code needs temporary, non-volatile storage,
//uncomment and alter the set below as needed.
//#define CREATOR_TT01 1
//#define CREATOR_TT02 2
//#define CREATOR_TT03 3
//#define CREATOR_TT04 4
//#define CREATOR_TT05 5
//#define CREATOR_TT06 6
//#ifdef DEFINETYPES
//typedef uint32_t tokTypeTT01;
//typedef uint32_t tokTypeTT02;
//typedef uint32_t tokTypeTT03;
//typedef uint32_t tokTypeTT04;
//typedef uint16_t tokTypeTT05;
//typedef uint16_t tokTypeTT06;
//#endif //DEFINETYPES
//#ifdef DEFINETOKENS
//#define TT01_LOCATION 1
//#define TT02_LOCATION 2
//#define TT03_LOCATION 3
//#define TT04_LOCATION 4
//#define TT05_LOCATION 5
//#define TT06_LOCATION 6
//DEFINE_FIXED_BASIC_TOKEN(TT01, tokTypeTT01, TT01_LOCATION, 0x0000)
//DEFINE_FIXED_BASIC_TOKEN(TT02, tokTypeTT02, TT02_LOCATION, 0x0000)
//DEFINE_FIXED_BASIC_TOKEN(TT03, tokTypeTT03, TT03_LOCATION, 0x0000)
//DEFINE_FIXED_BASIC_TOKEN(TT04, tokTypeTT04, TT04_LOCATION, 0x0000)
//DEFINE_FIXED_BASIC_TOKEN(TT05, tokTypeTT05, TT05_LOCATION, 0x0000)
//DEFINE_FIXED_BASIC_TOKEN(TT06, tokTypeTT06, TT06_LOCATION, 0x0000)
//#endif //DEFINETOKENS

#else // !defined(SIM_EEPROM_TEST) && !defined(NVM3_TEST)

//The Simulated EEPROM and NVM3 unit tests define their tokens via the
//APPLICATION_TOKEN_HEADER macro.
#ifdef UC_BUILD
 #ifdef SL_CATALOG_ZIGBEE_ZCL_FRAMEWORK_CORE_PRESENT
   #include "app/framework/util/af-token.h"
 #endif
 #ifdef SL_CATALOG_ZIGBEE_NCP_FRAMEWORK_PRESENT
   #include "app/framework/util/af-ncp-token.h"
 #endif
#else
 #ifdef APPLICATION_TOKEN_HEADER
  #include APPLICATION_TOKEN_HEADER
 #endif
#endif // UC_BUILD

#endif // !defined(SIM_EEPROM_TEST) && !defined(NVM3_TEST)

#ifndef DEFINEADDRESSES
  #undef TOKEN_NEXT_ADDRESS
#endif

/** @} END addtogroup */

/**
 * <!-- HIDDEN
 * @page 2p5_to_3p0
 * <hr>
 * The file token-stack.h is described in @ref token_stack and includes
 * the following:
 * <ul>
 * <li> <b>New items</b>
 *   - ::CREATOR_STACK_ALTERNATE_KEY
 *   - ::CREATOR_STACK_APS_FRAME_COUNTER
 *   - ::CREATOR_STACK_LINK_KEY_TABLE
 *   .
 * <li> <b>Changed items</b>
 *   -
 *   -
 *   .
 * <li> <b>Removed items</b>
 *   - ::CREATOR_STACK_DISCOVERY_CACHE
 *   - ::CREATOR_STACK_APS_INDIRECT_BINDING_TABLE
 *   .
 * </ul>
 * HIDDEN -->
 */
