// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef MEMORY_DATA_H
#define MEMORY_DATA_H

#ifdef __cplusplus
#include <cstdint>
#else
#include <stdint.h>
#endif

#include "azure_c_shared_utility/uuid.h"
#include "umock_c/umock_c_prod.h"

#ifdef __cplusplus
extern "C" {
#endif

MOCKABLE_INTERFACE(memory_data,
    FUNCTION(, void, read_uint8_t,  const unsigned char*, source, uint8_t*, destination),
    FUNCTION(, void, read_uint16_t, const unsigned char*, source, uint16_t*, destination),
    FUNCTION(, void, read_uint32_t, const unsigned char*, source, uint32_t*, destination),
    FUNCTION(, void, read_uint64_t, const unsigned char*, source, uint64_t*, destination),

    FUNCTION(, void, read_int8_t,  const unsigned char*, source,  int8_t*,  destination),
    FUNCTION(, void, read_int16_t, const unsigned char*, source, int16_t*, destination),
    FUNCTION(, void, read_int32_t, const unsigned char*, source, int32_t*, destination),
    FUNCTION(, void, read_int64_t, const unsigned char*, source, int64_t*, destination),

    FUNCTION(, void, read_uuid_t, const unsigned char*, source, UUID_T*, destination),

    FUNCTION(, void, write_uint8_t,  unsigned char*, destination, uint8_t,  value),
    FUNCTION(, void, write_uint16_t, unsigned char*, destination, uint16_t, value),
    FUNCTION(, void, write_uint32_t, unsigned char*, destination, uint32_t, value),
    FUNCTION(, void, write_uint64_t, unsigned char*, destination, uint64_t, value),

    FUNCTION(, void, write_int8_t,  unsigned char*, destination, int8_t,  value),
    FUNCTION(, void, write_int16_t, unsigned char*, destination, int16_t, value),
    FUNCTION(, void, write_int32_t, unsigned char*, destination, int32_t, value),
    FUNCTION(, void, write_int64_t, unsigned char*, destination, int64_t, value),

    FUNCTION(, void, write_uuid_t, unsigned char*, destination, const UUID_T, value)
)

#ifdef __cplusplus
}
#endif


#endif /*MEMORY_DATA_H*/

