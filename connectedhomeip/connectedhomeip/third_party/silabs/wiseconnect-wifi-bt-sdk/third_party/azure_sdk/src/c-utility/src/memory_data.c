// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <inttypes.h>

#include "azure_c_shared_utility/uuid.h"

#include "azure_c_shared_utility/memory_data.h"

IMPLEMENT_MOCKABLE_FUNCTION(, void, read_uint8_t, const unsigned char*, source, uint8_t*, destination)
{
    /*Codes_SRS_MEMORY_DATA_02_041: [ read_uint8_t shall write in destination the byte at source ]*/
    *destination = *source;
}

IMPLEMENT_MOCKABLE_FUNCTION(, void, read_uint16_t, const unsigned char*, source, uint16_t*, destination)
{
    /*Codes_SRS_MEMORY_DATA_02_042: [ read_uint16_t shall write in destination the bytes at source MSB first and return. ]*/
    *destination = 
        (source[0]<<8)+
        (source[1])
        ;
}

IMPLEMENT_MOCKABLE_FUNCTION(, void, read_uint32_t, const unsigned char*, source, uint32_t*, destination)
{
    /*Codes_SRS_MEMORY_DATA_02_043: [ read_uint32_t shall write in destination the bytes at source MSB first. ]*/
    *destination =
        (source[0] << 24) +
        (source[1] << 16) +
        (source[2] << 8) +
        (source[3])
        ;
}

IMPLEMENT_MOCKABLE_FUNCTION(, void, read_uint64_t, const unsigned char*, source, uint64_t*, destination)
{
    /*Codes_SRS_MEMORY_DATA_02_044: [ read_uint64_t shall write in destination the bytes at source MSB first. ]*/
    *destination =
        ((uint64_t)source[0] << 56) +
        ((uint64_t)source[1] << 48) +
        ((uint64_t)source[2] << 40) +
        ((uint64_t)source[3] << 32) +
        ((uint64_t)source[4] << 24) +
        ((uint64_t)source[5] << 16) +
        ((uint64_t)source[6] << 8) +
        ((uint64_t)source[7])
        ;
}

IMPLEMENT_MOCKABLE_FUNCTION(, void, write_uint8_t, unsigned char*, destination, uint8_t, value)
{
    /*Codes_SRS_MEMORY_DATA_02_050: [ write_uint8_t shall write in destination the byte of value. ]*/
    destination[0] = value;
}

IMPLEMENT_MOCKABLE_FUNCTION(, void, write_uint16_t, unsigned char*, destination, uint16_t, value)
{
    /*Codes_SRS_MEMORY_DATA_02_051: [ write_uint16_t shall write in destination the bytes of value MSB first. ]*/
    destination[0] = (value>>8) & 0xFF;
    destination[1] = (value) & 0xFF;
}

IMPLEMENT_MOCKABLE_FUNCTION(, void, write_uint32_t, unsigned char*, destination, uint32_t, value)
{
    /*Codes_SRS_MEMORY_DATA_02_052: [ write_uint32_t shall write in destination the bytes of value MSB first. ]*/
    destination[0] = (value >> 24)&0xFF;
    destination[1] = (value >> 16)&0xFF;
    destination[2] = (value >> 8) & 0xFF;
    destination[3] = (value)&0xFF;
}

IMPLEMENT_MOCKABLE_FUNCTION(, void, write_uint64_t, unsigned char*, destination, uint64_t, value)
{
    /*Codes_SRS_MEMORY_DATA_02_053: [ write_uint64_t shall write in destination the bytes of value MSB first. ]*/
    destination[0] = (value >> 56)&0xFF;
    destination[1] = (value >> 48)&0xFF;
    destination[2] = (value >> 40)&0xFF;
    destination[3] = (value >> 32)&0xFF;
    destination[4] = (value >> 24)&0xFF;
    destination[5] = (value >> 16)&0xFF;
    destination[6] = (value >> 8)&0xFF;
    destination[7] = (value)&0xFF;
}

IMPLEMENT_MOCKABLE_FUNCTION(, void, write_int8_t, unsigned char*, destination, int8_t, value)
{
    /*Codes_SRS_MEMORY_DATA_02_054: [ write_int8_t shall write at destination the byte of value. ]*/
    destination[0] = value;
}

IMPLEMENT_MOCKABLE_FUNCTION(, void, write_int16_t, unsigned char*, destination, int16_t, value)
{
    /*Codes_SRS_MEMORY_DATA_02_055: [ write_int16_t shall write at destination the bytes of value starting with MSB. ]*/
    destination[0] = (value >> 8) & 0xFF;
    destination[1] = (value) & 0xFF;
}

IMPLEMENT_MOCKABLE_FUNCTION(, void, write_int32_t, unsigned char*, destination, int32_t, value)
{
    /*Codes_SRS_MEMORY_DATA_02_056: [ write_int32_t shall write at destination the bytes of value starting with MSB ]*/
    destination[0] = (value >> 24) & 0xFF;
    destination[1] = (value >> 16) & 0xFF;
    destination[2] = (value >> 8) & 0xFF;
    destination[3] = (value) & 0xFF;
}

IMPLEMENT_MOCKABLE_FUNCTION(, void, write_int64_t, unsigned char*, destination, int64_t, value)
{
    /*Codes_SRS_MEMORY_DATA_02_057: [ write_int64_t shall write at destination the bytes of value starting with MSB. ]*/
    destination[0] = (value >> 56) & 0xFF;
    destination[1] = (value >> 48) & 0xFF;
    destination[2] = (value >> 40) & 0xFF;
    destination[3] = (value >> 32) & 0xFF;
    destination[4] = (value >> 24) & 0xFF;
    destination[5] = (value >> 16) & 0xFF;
    destination[6] = (value >> 8) & 0xFF;
    destination[7] = (value) & 0xFF;
}

IMPLEMENT_MOCKABLE_FUNCTION(, void, write_uuid_t, unsigned char*, destination, const UUID_T, value)
{
    /*Codes_SRS_MEMORY_DATA_02_058: [ write_uuid_t shall write at destination the bytes of value ]*/
    (void)memcpy(destination, (void*)value, sizeof(UUID_T));
}

IMPLEMENT_MOCKABLE_FUNCTION(, void, read_int8_t, const unsigned char*, source, int8_t*, destination)
{
    /*Codes_SRS_MEMORY_DATA_02_045: [ read_int8_t shall write in destination the signed byte at source. ]*/
    *destination = (int8_t)(source[0]);
}

IMPLEMENT_MOCKABLE_FUNCTION(, void, read_int16_t, const unsigned char*, source, int16_t*, destination)
{
    /*Codes_SRS_MEMORY_DATA_02_046: [ read_int16_t shall write in destination the bytes at source MSB first. ]*/
    *destination = ((int16_t)source[0]<<8)+
        (source[1])
        ;
}

IMPLEMENT_MOCKABLE_FUNCTION(, void, read_int32_t, const unsigned char*, source, int32_t*, destination)
{
    /*Codes_SRS_MEMORY_DATA_02_047: [ read_int32_t shall write in destination the bytes at source MSB first. ]*/
    *destination = ((int32_t)source[0] << 24) +
        (source[1] << 16) +
        (source[2] << 8) +
        (source[3])
        ;
}

IMPLEMENT_MOCKABLE_FUNCTION(, void, read_int64_t, const unsigned char*, source, int64_t*, destination)
{
    /*Codes_SRS_MEMORY_DATA_02_048: [ read_int64_t shall write in destination the bytes at source MSB first. ]*/
    *destination = (((int64_t)source[0]) << 56) +
        (((int64_t)source[1]) << 48) +
        (((int64_t)source[2]) << 40) +
        (((int64_t)source[3]) << 32) +
        (((int64_t)source[4]) << 24) +
        (((int64_t)source[5]) << 16) +
        (((int64_t)source[6]) << 8) +
        (((int64_t)source[7]))
        ;
}

IMPLEMENT_MOCKABLE_FUNCTION(, void, read_uuid_t, const unsigned char*, source, UUID_T*, destination)
{
    /*Codes_SRS_MEMORY_DATA_02_049: [ read_uuid_t shall write in destination the bytes at source. ]*/
    (void)memcpy(destination, source, sizeof(UUID_T));
}