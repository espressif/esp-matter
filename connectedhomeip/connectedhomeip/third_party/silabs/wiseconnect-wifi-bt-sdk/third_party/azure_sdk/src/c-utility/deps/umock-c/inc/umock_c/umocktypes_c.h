// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef UMOCKTYPES_C_H
#define UMOCKTYPES_C_H

#include "azure_macro_utils/macro_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

    int umocktypes_c_register_types(void);

#define UMOCKTYPES_HANDLERS(type, function_postfix) \
    char* MU_C2(umocktypes_stringify_,function_postfix)(type* value); \
    int MU_C2(umocktypes_are_equal_, function_postfix)(type* left, type* right); \
    int MU_C2(umocktypes_copy_, function_postfix)(type* destination, type* source); \
    void MU_C2(umocktypes_free_, function_postfix)(type* value);

UMOCKTYPES_HANDLERS(char, char)
UMOCKTYPES_HANDLERS(unsigned char, unsignedchar)
UMOCKTYPES_HANDLERS(short, short)
UMOCKTYPES_HANDLERS(unsigned short, unsignedshort)
UMOCKTYPES_HANDLERS(int, int)
UMOCKTYPES_HANDLERS(unsigned int, unsignedint)
UMOCKTYPES_HANDLERS(long, long)
UMOCKTYPES_HANDLERS(unsigned long, unsignedlong)
UMOCKTYPES_HANDLERS(long long, longlong)
UMOCKTYPES_HANDLERS(unsigned long long, unsignedlonglong)
UMOCKTYPES_HANDLERS(float, float)
UMOCKTYPES_HANDLERS(double, double)
UMOCKTYPES_HANDLERS(long double, longdouble)
UMOCKTYPES_HANDLERS(size_t, size_t)
UMOCKTYPES_HANDLERS(void*, void_ptr)

#ifdef __cplusplus
}
#endif

#endif /* UMOCKTYPES_C_H */
