// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef UMOCKTYPES_STDINT_H
#define UMOCKTYPES_STDINT_H

#ifdef __cplusplus
#include <cstdint>
#else
#include <stdint.h>
#endif

#include "azure_macro_utils/macro_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

    int umocktypes_stdint_register_types(void);

#define UMOCKTYPES_STDINT_HANDLERS(type, function_postfix) \
    char* MU_C2(umocktypes_stringify_,function_postfix)(type* value); \
    int MU_C2(umocktypes_are_equal_, function_postfix)(type* left, type* right); \
    int MU_C2(umocktypes_copy_, function_postfix)(type* destination, type* source); \
    void MU_C2(umocktypes_free_, function_postfix)(type* value);

UMOCKTYPES_STDINT_HANDLERS(uint8_t, uint8_t)
UMOCKTYPES_STDINT_HANDLERS(int8_t, int8_t)
UMOCKTYPES_STDINT_HANDLERS(uint16_t, uint16_t)
UMOCKTYPES_STDINT_HANDLERS(int16_t, int16_t)
UMOCKTYPES_STDINT_HANDLERS(uint32_t, uint32_t)
UMOCKTYPES_STDINT_HANDLERS(int32_t, int32_t)
UMOCKTYPES_STDINT_HANDLERS(uint64_t, uint64_t)
UMOCKTYPES_STDINT_HANDLERS(int64_t, int64_t)

#ifdef __cplusplus
}
#endif

#endif /* UMOCKTYPES_STDINT_H */
