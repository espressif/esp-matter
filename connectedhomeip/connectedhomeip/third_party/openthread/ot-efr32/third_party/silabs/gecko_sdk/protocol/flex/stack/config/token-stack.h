/***************************************************************************//**
 * @file
 * The file token-stack.h should not be included directly.
 * It is accessed by the other token files.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

//force include some sections for doxygen
#ifdef DOXYGEN
#define DEFINETYPES
#endif

/**
 * @addtogroup token_stack
 * @brief Definitions for stack tokens.
 *
 * Stack tokens are used by the stack to store information in non-volatile
 * memory. A typical use case is to store the network information, so
 * after an accidental reset it can be part of the network without going through
 * the association process again.
 *
 * @note For the application tokens, refer to @ref tokens and AN1154.
 *
 * @warning While stack tokens can be accessed through the @ref tokens API,
 * they <b>must not be written directly</b>. Most stack tokens have APIs to
 * read/write them. This documentation is intended for those who need more
 * information on the internal details of the connect stack.
 *
 * See token-stack.h for source code.
 * @{
 */

// The basic TOKEN_DEF macro should not be used directly since the simplified
//  definitions are safer to use. For completeness of information, the basic
//  macro has the following format:
//
//  TOKEN_DEF(name,creator,iscnt,isidx,type,arraysize,...)
//  name - The root name used for the token
//  creator - a "creator code" used to uniquely identify the token
//  iscnt - a boolean flag that is set to identify a counter token
//  isidx - a boolean flag that is set to identify an indexed token
//  type - the basic type or typdef of the token
//  arraysize - the number of elements making up an indexed token
//  ... - initializers used when reseting the tokens to default values
//
//
// The following convenience macros are used to simplify the definition
//  process for commonly specified parameters to the basic TOKEN_DEF macro
//  DEFINE_BASIC_TOKEN(name, type, ...)
//  DEFINE_INDEXED_TOKEN(name, type, arraysize, ...)
//  DEFINE_COUNTER_TOKEN(name, type, ...)
//  DEFINE_FIXED_BASIC_TOKEN(name, type, address, ...)
//  DEFINE_FIXED_INDEXED_TOKEN(name, type, arraysize, address, ...)
//  DEFINE_FIXED_COUNTER_TOKEN(name, type, address, ...)
//  DEFINE_MFG_TOKEN(name, type, address, ...)
//

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/**
 * @name Convenience Macros
 * @brief The following convenience macros are used to simplify the definition
 * process for commonly specified parameters to the basic TOKEN_DEF macro.
 * See @ref token for a more detailed explanation.
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
/** @} END Convenience Macros */
#endif //DOXYGEN_SHOULD_SKIP_THIS

// The Simulated EEPROM unit tests define all of their own tokens.
#ifndef SIM_EEPROM_TEST

// The creator codes are here in one list instead of next to their token
// definitions so comparison of the code is easier.  The only requirement
// on these creator definitions is that they all must be unique.  A favorite
// method for picking creator codes is to use two ASCII characters
// to make the codes more memorable.

#ifndef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @name Creator Codes
 * @brief The CREATOR is used as a distinct identifier tag for the
 * token.
 *
 * The CREATOR is necessary because the token name is defined
 * differently depending on the hardware platform. Therefore, the CREATOR
 * ensures that token definitions and data stay tagged and known. The only
 * requirement is that each creator definition must be unique.
 * See @ref token for a more detailed explanation.
 *@{
 */

// STACK CREATORS
#define CREATOR_STACK_NVDATA_VERSION                         0x0001
#define CREATOR_STACK_NODE_DATA                              0x0003
#define CREATOR_STACK_SECURITY_KEY                           0x0004
#define CREATOR_STACK_NONCE_COUNTER                          0x0005
#define CREATOR_STACK_CHILD_TABLE                            0x0006
#define CREATOR_STACK_LAST_ASSIGNED_ID                       0x0007
#define CREATOR_STACK_BOOT_COUNTER                           0x0008
#define CREATOR_STACK_PARENT_LONG_ID                         0x0009

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
#define NVM3KEY_DOMAIN_CONNECT 0x30000U
#define NVM3KEY_DOMAIN_COMMON  0x80000U

// STACK KEYS
#define NVM3KEY_STACK_NVDATA_VERSION           (NVM3KEY_DOMAIN_CONNECT | 0x0001)
#define NVM3KEY_STACK_NODE_DATA                (NVM3KEY_DOMAIN_CONNECT | 0x0003)
#define NVM3KEY_STACK_SECURITY_KEY             (NVM3KEY_DOMAIN_CONNECT | 0x0004)
#define NVM3KEY_STACK_NONCE_COUNTER            (NVM3KEY_DOMAIN_CONNECT | 0x0005)
// This key is used for an indexed token and the subsequent 0x7F keys are also
// reserved.
#define NVM3KEY_STACK_CHILD_TABLE              (NVM3KEY_DOMAIN_CONNECT | 0x0010)
#define NVM3KEY_STACK_LAST_ASSIGNED_ID         (NVM3KEY_DOMAIN_CONNECT | 0x0090)
#define NVM3KEY_STACK_BOOT_COUNTER             (NVM3KEY_DOMAIN_CONNECT | 0x0091)
#define NVM3KEY_STACK_PARENT_LONG_ID           (NVM3KEY_DOMAIN_CONNECT | 0x0092)

/** @} END NVM3 Object Keys  */

#endif //DOXYGEN_SHOULD_SKIP_THIS

//////////////////////////////////////////////////////////////////////////////
// MANUFACTURING DATA
// Since the manufacturing data is platform specific, we pull in the proper
// file here.
#if defined(CORTEXM3)
// cortexm3 handles mfg tokens separately via mfg-token.h
#elif defined(EMBER_TEST)
  #include "hal/micro/cortexm3/efm32/token-manufacturing.h"
#else
  #error no platform defined
#endif

//////////////////////////////////////////////////////////////////////////////
// STACK DATA
// *the addresses of these tokens must not change*

/**
 * @brief The current version number of the stack tokens.
 * MSB is the version. LSB is a complement.
 *
 * See hal/micro/token.h for a more complete explanation.
 */
#define CURRENT_STACK_TOKEN_VERSION 0x03FC //MSB is version, LSB is complement

#ifdef DEFINETYPES

/**
 * @name Token types
 * @brief The types used for each stack token.
 *@{
 */

/**
 * @brief Type for TOKEN_STACK_NVDATA_VERSION. Keeps the version number of
 * stack tokens.
 */
typedef uint16_t tokTypeStackNvdataVersion;

/**
 * @brief Type for TOKEN_STACK_NONCE_COUNTER. Used to make sure that Nonce used
 * for security is not repeated even after unexpected reboot.
 */
typedef uint32_t tokTypeStackNonceCounter;

/**
 * @struct tokTypeStackKey
 * @brief Type for TOKEN_STACK_SECURITY_KEY. Keeps the security key for MAC
 * layer security.
 */
typedef struct {
  uint8_t networkKey[16]; /**< The key itself */
} tokTypeStackKey;

/**
 * @struct tokTypeStackNodeData
 * @brief Type for TOKEN_STACK_NODE_DATA. Generic information of the node is
 * stored in this token
 */
typedef struct {
  uint16_t panId; /**< The PanId of the device */
  int16_t radioTxPower; /**< The TX power configured for the device in deci-dBm */
  uint16_t radioFreqChannel; /**< The radio channel configured for the device */
  uint8_t nodeType; /**< The @ref EmberNodeType configured for the device */
  uint16_t nodeId; /**< The NodeId (short address) of the device */
  uint16_t parentId; /**< The NodeId of the device's parent, if any */
} tokTypeStackNodeData;

/**
 * @struct tokTypeStackChildTableEntry
 * @brief Type of an element of TOKEN_STACK_CHILD_TABLE (indexed token). Keeps
 * children information of a device, which has parent support enabled.
 */
typedef struct {
  EmberEUI64 longId; /**< The Long Id of the child */
  EmberNodeId shortId; /**< The NodeId of the child */
  uint8_t flags; /**< Flags for the child required by the stack */
} tokTypeStackChildTableEntry;

/**
 * @brief Type for TOKEN_STACK_LAST_ASSIGNED_ID. Stores the last assigned NodeId
 * if the device is @ref EMBER_STAR_COORDINATOR.
 */
typedef uint16_t tokTypeStackLastAllocatedId;

/**
 * @brief Type for TOKEN_STACK_BOOT_COUNTER. Increments at boot
 * (during @ref emberInit()).
 */
typedef uint32_t tokTypeStackBootCounter;

/**
 * @brief Type for TOKEN_STACK_PARENT_LONG_ID. Stores the Long Id of the parent
 * of this device. Only used for @ref EMBER_MAC_MODE_DEVICE and
 * @ref EMBER_MAC_MODE_SLEEPY_DEVICE device types.
 */
typedef EmberEUI64 tokTypeParentLongId;

/** @} END Token types  */
#endif //DEFINETYPES

#ifdef DEFINETOKENS

DEFINE_BASIC_TOKEN(STACK_NVDATA_VERSION,
                   tokTypeStackNvdataVersion,
                   CURRENT_STACK_TOKEN_VERSION)

DEFINE_BASIC_TOKEN(STACK_SECURITY_KEY,
                   tokTypeStackKey,
                   { 0, })

DEFINE_BASIC_TOKEN(STACK_NODE_DATA,
                   tokTypeStackNodeData,
                   { 0xFFFF, 0, 0xFF, 0xFF, 0xFFFF, 0xFFFF })

DEFINE_BASIC_TOKEN(STACK_LAST_ASSIGNED_ID,
                   tokTypeStackLastAllocatedId,
                   0x0000)

DEFINE_INDEXED_TOKEN(STACK_CHILD_TABLE,
                     tokTypeStackChildTableEntry,
                     EMBER_CHILD_TABLE_TOKEN_SIZE,
                     { 0, })

DEFINE_COUNTER_TOKEN(STACK_NONCE_COUNTER,
                     tokTypeStackNonceCounter,
                     0x00000000)

DEFINE_COUNTER_TOKEN(STACK_BOOT_COUNTER,
                     tokTypeStackBootCounter,
                     0x0000)

DEFINE_BASIC_TOKEN(STACK_PARENT_LONG_ID,
                   tokTypeParentLongId,
                   { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF })

#endif //DEFINETOKENS

#ifdef MAC_DEBUG_TOKEN
//////////////////////////////////////////////////////////////////////////////
// MAC DEBUG DATA
#include "stack/mac/token-mac-debug.h"
#endif

///////////////////////////////////////////////////////////////////////////////
// APPLICATION DATA

#include "sl_token_manager_config.h"
#if (SL_TOKEN_MANAGER_CUSTOM_TOKENS_PRESENT)
  #include SL_TOKEN_MANAGER_CUSTOM_TOKEN_HEADER
#endif

#else //SIM_EEPROM_TEST

//The Simulated EEPROM unit tests define all of their tokens via the
//APPLICATION_TOKEN_HEADER macro.
  #ifdef APPLICATION_TOKEN_HEADER
    #include APPLICATION_TOKEN_HEADER
  #endif

#endif //SIM_EEPROM_TEST

/** @} END addtogroup */
