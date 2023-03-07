// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef UMOCKTYPES_BOOL_H
#define UMOCKTYPES_BOOL_H

#ifdef __cplusplus
#else
#include <stdbool.h>
#endif

#include "azure_macro_utils/macro_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

    int umocktypes_bool_register_types(void);

    char* umocktypes_stringify_bool(const bool* value); \
    int umocktypes_are_equal_bool(const bool* left, const bool* right); \
    int umocktypes_copy_bool(bool* destination, const bool* source); \
    void umocktypes_free_bool(bool* value);

#ifdef __cplusplus
}
#endif

#endif /* UMOCKTYPES_BOOL_H */
