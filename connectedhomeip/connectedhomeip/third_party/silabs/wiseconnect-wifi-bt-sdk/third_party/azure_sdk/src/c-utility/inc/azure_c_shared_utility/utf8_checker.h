// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef UTF8_CHECKER_H
#define UTF8_CHECKER_H

#ifdef __cplusplus
#include <cstddef>
#else
#include <stdbool.h>
#include <stddef.h>
#endif

#include "umock_c/umock_c_prod.h"

#ifdef __cplusplus
extern "C" {
#endif

MOCKABLE_FUNCTION(, bool, utf8_checker_is_valid_utf8, const unsigned char*, utf8_str, size_t, length);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* UTF8_CHECKER_H */
