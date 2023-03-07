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
#ifndef __PLAT_TOKEN_H__
#define __PLAT_TOKEN_H__

//no special handling for the manufacturing tokens
#define TOKEN_MFG TOKEN_DEF

//-- Build structure defines

/**
 * @description Simple declarations of all of the token types so that they can
 * be referenced from anywhere in the code base.
 */
#define DEFINETYPES
  #include "stack/config/token-stack.h"
#undef DEFINETYPES

//-- Build parameter links
#define DEFINETOKENS

/**
 * @description Macro for translating token definitions into size variables.
 * This provides a convenience for abstracting the 'sizeof(type)' anywhere.
 *
 * @param name: The name of the token.
 *
 * @param type: The token type.  The types are found in token-stack.h.
 */
#define TOKEN_DEF(name, creator, iscnt, isidx, type, arraysize, ...) \
  TOKEN_##name##_SIZE = sizeof(type),
enum {
    #include "stack/config/token-stack.h"
};

#undef TOKEN_DEF

// #define DEFINEADDRESSES
// *
//  * @description Macro for creating a 'region' element in the enum below.  This
//  * creates an element in the enum that provides a starting point (address) for
//  * subsequent tokens to align against.  ( See hal/micro/avr-atmega/token.c for
//  * the instances of TOKEN_NEXT_ADDRESS() );
//  *
//  * @param region: The name to give to the element in the address enum..
//  *
//  * @param address: The address in EEPROM where the region begins.
// //
// #define TOKEN_NEXT_ADDRESS(region, address)
//   TOKEN_##region##_NEXT_ADDRESS = ((address) - 1),

/**
 * @description Macro for creating ADDRESS and END elements for each token in
 * the enum below.  The ADDRESS element is linked to from the the normal
 * TOKEN_##name macro and provides the value passed into the internal token
 * system calls.  The END element is a placeholder providing the starting
 * point for the ADDRESS of the next dynamically positioned token.
 *
 * @param name: The name of the token.
 *
 * @param arraysize: The number of elements in an indexed token (arraysize=1
 * for scalar tokens).
 */
#define TOKEN_DEF(name, creator, iscnt, isidx, type, arraysize, ...) \
  TOKEN_##name##_ADDRESS,                                            \
  TOKEN_##name##_END = TOKEN_##name##_ADDRESS                        \
                       + (TOKEN_##name##_SIZE * arraysize) - 1,

/**
 * @description The enum that operates on the two macros above.  Also provides
 * an indentifier so the address of the top of the token system can be known.
 */
enum {
  #include "stack/config/token-stack.h"
  TOKEN_MAXIMUM_SIZE
};

#undef TOKEN_DEF
#undef TOKEN_NEXT_ADDRESS
#undef DEFINEADDRESSES

/**
 * @description Enum for translating token defs into a number.  This number is
 * used as an index into the Address Lookup table.  The Address Lookup table
 * is defined in hal/micro/generic/token-ram.c.  The reason for the lookup
 * table is so mupltiple stack files can be built once using this header, then
 * each image being created can rebuild token-ram.c with it's own set of
 * configuration parameters and still map the tokens properly.
 *
 * The special entry TOKEN_COUNT is always at the top of the enum, allowing
 * the token system to know how many tokens there are.
 *
 * @param name: The name of the token.
 */
#define TOKEN_DEF(name, creator, iscnt, isidx, type, arraysize, ...) \
  TOKEN_##name,
enum {
    #include "stack/config/token-stack.h"
  TOKEN_COUNT
};

#undef TOKEN_DEF

/**
 * @description Macro for typedef'ing the CamelCase token type found in
 * token-stack.h to a capitalized TOKEN style name that ends in _TYPE.
 * This macro allows other macros below to use 'token##_TYPE' to declare
 * a local copy of that token.
 *
 * @param name: The name of the token.
 *
 * @param type: The token type.  The types are found in token-stack.h.
 */
#define TOKEN_DEF(name, creator, iscnt, isidx, type, arraysize, ...) \
  typedef type TOKEN_##name##_TYPE;
  #include "stack/config/token-stack.h"
#undef TOKEN_DEF
#undef DEFINETOKENS

/**
 * @description Copies the token value from non-volatile storage into a RAM
 * location.  This is the internal function that the two exposed APIs
 * (halCommonGetToken and halCommonGetIndexedToken) expand out to.  The
 * API simplifies the access into this function by hiding the size parameter
 * and hiding the value 0 used for the index parameter in scalar tokens.
 *
 * @param data: A pointer to where the data being read should be placed.
 *
 * @param token: The name of the token to get data from.  On this platform
 * that name is defined as an address.
 *
 * @param index: The index to access.  If the token being accessed is not an
 * indexed token, this parameter is set by the API to be 0.
 *
 * @param len: The length of the token being worked on.  This value is
 * automatically set by the API to be the size of the token.
 */
void halInternalGetTokenData(void *data, uint16_t token, uint8_t index, uint8_t len);

/**
 * @description Sets the value of a token in non-volatile storage.  This is
 * the internal function that the two exposed APIs (halCommonSetToken and
 * halCommonSetIndexedToken) expand out to.  The API simplifies the access
 * into this function by hiding the size parameter and hiding the value 0
 * used for the index parameter in scalar tokens.
 *
 * @param token: The name of the token to get data from.  On this platform
 * that name is defined as an address.
 *
 * @param index: The index to access.  If the token being accessed is not an
 * indexed token, this parameter is set by the API to be 0.
 *
 * @param data: A pointer to the data being written.
 *
 * @param len: The length of the token being worked on.  This value is
 * automatically set by the API to be the size of the token.
 */
void halInternalSetTokenData(uint16_t token, uint8_t index, void *data, uint8_t len);

// See hal/micro/token.h for the full explanation of the token API as
// instantiated below.

#define halCommonGetToken(data, token) \
  halInternalGetTokenData(data, token, 0x7F, token##_SIZE)

#define halCommonGetMfgToken(data, token) \
  halInternalGetMfgTokenData(data, token, 0x7F, token##_SIZE)

void halInternalSetMfgTokenData(uint16_t token, void *data, uint8_t len);

#define halCommonGetIndexedToken(data, token, index) \
  halInternalGetTokenData(data, token, index, token##_SIZE)

#define halStackGetIndexedToken(data, token, index, size) \
  halInternalGetTokenData(data, token, index, size)

#define halCommonSetToken(token, data) \
  halInternalSetTokenData(token, 0x7F, data, token##_SIZE)

#define halCommonSetIndexedToken(token, index, data) \
  halInternalSetTokenData(token, index, data, token##_SIZE)

#define halStackSetIndexedToken(token, index, data, size) \
  halInternalSetTokenData(token, index, data, size)

#define halCommonIncrementCounterToken(token)               \
  do {                                                      \
    token##_TYPE data;                                      \
    halInternalGetTokenData(&data, token, 0, token##_SIZE); \
    data++;                                                 \
    halInternalSetTokenData(token, 0, &data, token##_SIZE); \
  } while (0)

#undef TOKEN_MFG

#endif // __PLAT_TOKEN_H__
