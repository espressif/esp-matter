/***************************************************************************//**
 * @file
 * @brief Cortex-M3 Manufacturing token system
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
#ifndef __MFG_TOKEN_H__
#define __MFG_TOKEN_H__

// The manufacturing tokens live in the Info Blocks, while all other tokens
// live in the Simulated EEPROM or NVM3.  This requires the token names to be
// defined as different data (mfg tokens are memory address, all others are an
// enum).

//-- Build structure defines

/**
 * @description Simple declarations of all of the token types so that they can
 * be referenced from anywhere in the code base.
 */
#define DEFINETYPES
// Multiple inclusion of unguarded token-related header files is by design; suppress violation.
//cstat !MISRAC2012-Dir-4.10
  #include "sl_token_manufacturing.h"
#undef DEFINETYPES

#if defined(_SILICON_LABS_32B_SERIES_2)

// Unlike Series1 EFR devices, Series2 devices do not have an explicit
// flash space for LockBits.  Series 2 uses the top page of main
// flash to store LockBits.  The define LOCKBITS_BASE is then used like
// all EFR devices to access the LockBits memory.
// In non-Series2 devices the top page  of flash is free for all existing prior uses.
#define LOCKBITS_BASE ((FLASH_BASE + FLASH_SIZE) - FLASH_PAGE_SIZE)

#endif // _SILICON_LABS_32B_SERIES_2

//-- Build parameter links
#define DEFINETOKENS

/**
 * @description Macro for translating token defs into address variables
 * that point to the correct location in the Info Blocks.  (This is the
 * extern, the actual definition is found in hal/micro/cortexm3/token.c)
 *
 * @param name: The name of the token.
 *
 * @param TOKEN_##name##_ADDRESS: The address in EEPROM at which the token
 * will be stored.  This parameter is generated with a macro above.
 */
#define TOKEN_MFG(name, creator, iscnt, isidx, type, arraysize, ...) \
  extern const uint16_t TOKEN_##name;
// Multiple inclusion of unguarded token-related header files is by design; suppress violation.
//cstat !MISRAC2012-Dir-4.10
  #include "sl_token_manufacturing.h"
#undef TOKEN_MFG

/**
 * @description Macro for translating token definitions into size variables.
 * This provides a convenience for abstracting the 'sizeof(type)' anywhere.
 *
 * @param name: The name of the token.
 *
 * @param type: The token type.  The types are found in token-stack.h.
 */
#define TOKEN_MFG(name, creator, iscnt, isidx, type, arraysize, ...) \
  TOKEN_##name##_SIZE = sizeof(type),
enum {
  // Multiple inclusion of unguarded token-related header files is by design; suppress violation.
  //cstat !MISRAC2012-Dir-4.10
    #include "sl_token_manufacturing.h"
};

#undef TOKEN_MFG
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
#define TOKEN_MFG(name, creator, iscnt, isidx, type, arraysize, ...) \
  typedef type TOKEN_##name##_TYPE;
// Multiple inclusion of unguarded token-related header files is by design; suppress violation.
//cstat !MISRAC2012-Dir-4.10
  #include "sl_token_manufacturing.h"
#undef TOKEN_MFG

#undef TOKEN_NEXT_ADDRESS

#define DEFINEADDRESSES

/**
 * @description Macro for creating a 'region' element in the enum below.  This
 * creates an element in the enum that provides a starting point (address) for
 * subsequent tokens to align against.
 *
 * @param region: The name to give to the element in the address enum..
 *
 * @param address: The address in EEPROM where the region begins.
 */
#define TOKEN_NEXT_ADDRESS(region, address) \
  TOKEN_##region##_NEXT_ADDRESS = ((address) - 1),

/**
 * @description Macro for creating ADDRESS and END elements for each token in
 * the enum below.  The ADDRESS element is linked to from the the normal
 * TOKEN_##name macro and provides the value passed into the internal token
 * system calls.  The END element is a placeholder providing the starting
 * point for the ADDRESS of the next positioned token.
 *
 * @param name: The name of the token.
 *
 * @param arraysize: The number of elements in an indexed token (arraysize=1
 * for scalar tokens).
 */
#define TOKEN_MFG(name, creator, iscnt, isidx, type, arraysize, ...) \
  TOKEN_##name##_ADDRESS,                                            \
  TOKEN_##name##_END = TOKEN_##name##_ADDRESS                        \
                       + (TOKEN_##name##_SIZE * arraysize) - 1,

/**
 * @description The enum that operates on the two macros above.
 */
enum {
  // Multiple inclusion of unguarded token-related header files is by design; suppress violation.
  //cstat !MISRAC2012-Dir-4.10
  #include "sl_token_manufacturing.h"
};

#undef TOKEN_MFG
#undef DEFINEADDRESSES

#undef DEFINETOKENS

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/**
 * @description Copies the token value from non-volatile storage into a RAM
 * location.  This is the internal function that the exposed API
 * (halCommonGetMfgToken) expands out to.  The
 * API simplifies the access into this function by hiding the size parameter.
 *
 * @note Only the public function should be called since the public
 * function provides the correct parameters.
 *
 * @param data: A pointer to where the data being read should be placed.
 *
 * @param token: The name of the token to get data from.  On this platform
 * that name is defined as an address.
 *
 * @param index: The index to access.  If the token being accessed is not an
 * indexed token, this parameter is set by the API to be 0x7F.
 *
 * @param len: The length of the token being worked on.  This value is
 * automatically set by the API to be the size of the token.
 */
void halInternalGetMfgTokenData(void *data, uint16_t token, uint8_t index, uint32_t len);

/**
 * @description Sets the value of a token in non-volatile storage.  This is
 * the internal function that the exposed API (halCommonSetMfgToken)
 * expands out to.  The API simplifies the access into this function
 * by hiding the size parameter.
 *
 * @note Only the public function should be called since the public
 * function provides the correct parameters.
 *
 * <b>NOTE:</b> CIB manufacturing tokens can only be written by on-chip
 * code if the token is currently unprogrammed.
 *
 * <b>REMEMBER:</b> The flash hardware requires writing to 16bit aligned
 * addresses with a length that is multiples of 16bits.
 *
 * @param token: The name of the token to get data from.  On this platform
 * that name is defined as an address.
 *
 * @param data: A pointer to the data being written.
 *
 * @param len: The length of the token being worked on.  This value is
 * automatically set by the API to be the size of the token.
 */
void halInternalSetMfgTokenData(uint16_t token, void *data, uint32_t len);

//Link the public API to the private internal instance.
#define halCommonGetMfgToken(data, token) \
  halInternalGetMfgTokenData(data, token, 0x7F, token##_SIZE)

//Link the public API to the private internal instance.
#define halCommonGetIndexedMfgToken(data, token, index) \
  halInternalGetMfgTokenData(data, token, index, token##_SIZE)

// Link the public API to the private internal instance.
#define halCommonGetMfgTokenData(data, token, offset, size) \
  halInternalGetMfgTokenData(data, token, offset, size)

//Link the public API to the private internal instance.
#define halCommonSetMfgToken(token, data) \
  halInternalSetMfgTokenData(token, data, token##_SIZE)

#define halCommonSetMfgTokenData(token, data, size) \
  halInternalSetMfgTokenData(token, data, size)

#endif //DOXYGEN_SHOULD_SKIP_THIS

#endif //__MFG_TOKEN_H__
