/***************************************************************************//**
 * @file sl_token_api.h
 * @brief Header file for the Legacy HAL "shim" layer support for the token API
 * @version 5.6.0
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories, Inc, www.silabs.com</b>
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
#ifndef _TOKEN_API_H_
#define _TOKEN_API_H_

#ifndef PGM
#define PGM     const
#endif
#include "stack/include/ember-types.h"

#ifdef TOKEN_MANAGER_TEST
void token_manager_test_assert_handler(const char *filename, int linenumber);
#define ASSERT_HANDLER token_manager_test_assert_handler
#else
void halInternalAssertFailed(const char * filename, int linenumber);
#define ASSERT_HANDLER halInternalAssertFailed
#endif
#ifndef __SOURCEFILE__
#define __SOURCEFILE__ __FILE__
#endif // __SOURCEFILE__
#undef  assert // Replace assert from #include <assert.h> with this one
#define assert(condition)  \
  do { if (!(condition)) { \
         ASSERT_HANDLER(__SOURCEFILE__, __LINE__); } } while (0)

#include "sl_token_manufacturing_api.h"

#define DEFINETYPES
#include "stack/config/token-stack.h"
#undef DEFINETYPES

#define DEFINETOKENS

#define TOKEN_DEF(name, creator, iscnt, isidx, type, arraysize, ...) \
  TOKEN_##name,
enum {
    #include "stack/config/token-stack.h"
  TOKEN_COUNT
};
#undef TOKEN_DEF

//Define the size (in bytes) of all tokens/
#define TOKEN_DEF(name, creator, iscnt, isidx, type, arraysize, ...) \
  TOKEN_##name##_SIZE = sizeof(type),
enum {
  #include "stack/config/token-stack.h"
};
#undef TOKEN_DEF

#undef DEFINETOKENS

#define halCommonGetToken(data, token) \
  halInternalGetTokenData(data, token, 0x7F, token##_SIZE)

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

#define halCommonIncrementCounterToken(token) \
  halInternalIncrementCounterToken(token)

EmberStatus halStackInitTokens(void);

void halInternalGetTokenData(void *data, uint16_t token, uint8_t index, uint8_t len);

void halInternalSetTokenData(uint16_t token, uint8_t index, void *data, uint8_t len);

void halInternalIncrementCounterToken(uint8_t token);

#endif // _TOKEN_API_H_
