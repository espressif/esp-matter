// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef UUID_H
#define UUID_H

#ifdef __cplusplus
#include <cstddef>
#include <cstdint>
#else
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#endif /* __cplusplus */

#include "umock_c/umock_c_prod.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef unsigned char UUID_T[16];

/* These 2 strings can be conveniently used directly in printf statements
  Notice that PRI_UUID has to be used like any other print format specifier, meaning it
  has to be preceded with % */
#define PRI_UUID        "02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x"
#define UUID_FORMAT_VALUES(uuid) \
    (uuid)[0], (uuid)[1], (uuid)[2], (uuid)[3], (uuid)[4], (uuid)[5], (uuid)[6], (uuid)[7], \
    (uuid)[8], (uuid)[9], (uuid)[10], (uuid)[11], (uuid)[12], (uuid)[13], (uuid)[14], (uuid)[15]

#define UUID_FORMAT_VALUES_OR_NULL(uuid) \
    ((uuid) == NULL) ? 0 : (uuid)[0], ((uuid) == NULL) ? 0 : (uuid)[1], ((uuid) == NULL) ? 0 : (uuid)[2], ((uuid) == NULL) ? 0 : (uuid)[3], \
    ((uuid) == NULL) ? 0 : (uuid)[4], ((uuid) == NULL) ? 0 : (uuid)[5], ((uuid) == NULL) ? 0 : (uuid)[6], ((uuid) == NULL) ? 0 : (uuid)[7], \
    ((uuid) == NULL) ? 0 : (uuid)[8], ((uuid) == NULL) ? 0 : (uuid)[9], ((uuid) == NULL) ? 0 : (uuid)[10], ((uuid) == NULL) ? 0 : (uuid)[11], \
    ((uuid) == NULL) ? 0 : (uuid)[12], ((uuid) == NULL) ? 0 : (uuid)[13], ((uuid) == NULL) ? 0 : (uuid)[14], ((uuid) == NULL) ? 0 : (uuid)[15] \

/* @brief               Generates a true UUID
*  @param uuid          A pre-allocated buffer for the bytes of the generated UUID
*  @returns             Zero if no failures occur, non-zero otherwise.
*/
MOCKABLE_FUNCTION(, int, UUID_generate, UUID_T*, uuid);

/* @brief               Gets the UUID value (byte sequence) of an well-formed UUID string.
*  @param uuid_string   A null-terminated well-formed UUID string (e.g., "7f907d75-5e13-44cf-a1a3-19a01a2b4528").
*  @param uuid          Sequence of bytes representing an UUID.
*  @returns             Zero if no failures occur, non-zero otherwise.
*/
MOCKABLE_FUNCTION(, int, UUID_from_string, const char*, uuid_string, UUID_T*, uuid);

/* @brief               Gets the string representation of the UUID value.
*  @param uuid          Sequence of bytes representing an UUID.
*  @returns             A null-terminated string representation of the UUID value provided (e.g., "7f907d75-5e13-44cf-a1a3-19a01a2b4528").
*/
MOCKABLE_FUNCTION(, char*, UUID_to_string, const UUID_T*, uuid);

#ifdef __cplusplus
}
#endif

#endif /* UUID_H */
