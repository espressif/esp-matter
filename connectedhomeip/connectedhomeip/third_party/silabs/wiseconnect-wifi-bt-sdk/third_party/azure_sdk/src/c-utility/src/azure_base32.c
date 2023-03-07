// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include "azure_c_shared_utility/buffer_.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/strings.h"

#include "azure_c_shared_utility/azure_base32.h"

static const unsigned char BASE32_EQUAL_SIGN = 32;

static const char BASE32_VALUES[] = "abcdefghijklmnopqrstuvwxyz234567=";
#define TARGET_BLOCK_SIZE       5
#define INVALID_CHAR_POS        260

#define BASE32_INPUT_SIZE       8

#define ASCII_VALUE_MAX         0x80

static size_t base32_encoding_length(size_t src_len)
{
    return (((src_len + TARGET_BLOCK_SIZE - 1) / TARGET_BLOCK_SIZE) * 8);
}

static size_t base32_decoding_length(size_t src_len)
{
    return ((src_len*TARGET_BLOCK_SIZE) / 8);
}

static unsigned char convert_value_to_base32_char(unsigned char value)
{
    unsigned char result;

    if (value >= 50 && value <= 55)
    {
        result = 0x1a+(value-50);
    }
    else if (value == 61)
    {
        result = 0x20;
    }
    else if ((value <= 49) || (value >= 56 && value <= 64))
    {
        result = 0xFF;
    }
    else if (value >= 65 && value <= 90)
    {
        result = 0x00 + (value - 65);
    }
    else if (value >= 91 && value <= 96)
    {
        result = 0xFF;
    }
    else if (value >= 97 && value <= 123)
    {
        result = 0x00 + (value - 97);
    }
    else // value > 123
    {
        result = 0xFF;
    }
    return result;
}

static char* base32_encode_impl(const unsigned char* source, size_t src_size)
{
    char* result;

    // Allocate target buffer
    size_t output_len = base32_encoding_length(src_size);
    /* Codes_SRS_BASE32_07_009: [ base32_encode_impl shall allocate the buffer to the size of the encoding value. ] */
    if ((result = (char*)malloc(output_len + 1)) == NULL)
    {
        LogError("Failure allocating output buffer");
    }
    else
    {
        const unsigned char* iterator = source;
        size_t block_len = 0;
        size_t result_len = 0;
        unsigned char pos1 = 0;
        unsigned char pos2 = 0;
        unsigned char pos3 = 0;
        unsigned char pos4 = 0;
        unsigned char pos5 = 0;
        unsigned char pos6 = 0;
        unsigned char pos7 = 0;
        unsigned char pos8 = 0;

        memset(result, 0, output_len + 1);

        // Go through the source buffer sectioning off blocks of 5
        /* Codes_SRS_BASE32_07_010: [ base32_encode_impl shall look through source and separate each block into 5 bit chunks ] */
        while (src_size >= 1 && result != NULL)
        {
            pos1 = pos2 = pos3 = pos4 = pos5 = pos6 = pos7 = pos8 = 0;
            block_len = src_size > TARGET_BLOCK_SIZE ? TARGET_BLOCK_SIZE : src_size;
            // Fall through switch block to process the 5 (or smaller) block
            switch (block_len)
            {
            case 5:
                pos8 = (iterator[4] & 0x1f);
                pos7 = ((iterator[4] & 0xe0) >> 5);
                // fall through
            case 4:
                pos7 |= ((iterator[3] & 0x03) << 3);
                pos6 = ((iterator[3] & 0x7c) >> 2);
                pos5 = ((iterator[3] & 0x80) >> 7);
                // fall through
            case 3:
                pos5 |= ((iterator[2] & 0x0f) << 1);
                pos4 = ((iterator[2] & 0xf0) >> 4);
                // fall through
            case 2:
                pos4 |= ((iterator[1] & 0x01) << 4);
                pos3 = ((iterator[1] & 0x3e) >> 1);
                pos2 = ((iterator[1] & 0xc0) >> 6);
                // fall through
            case 1:
                pos2 |= ((iterator[0] & 0x07) << 2);
                pos1 = ((iterator[0] & 0xf8) >> 3);
                break;
            }
            // Move the iterator the block size
            iterator += block_len;
            // and decrement the src_size;
            src_size -= block_len;

            /* Codes_SRS_BASE32_07_012: [ If the src_size is not divisible by 8, base32_encode_impl shall pad the remaining places with =. ] */
            switch (block_len)
            {
                case 1: pos3 = pos4 = 32; // fall through
                case 2: pos5 = 32; // fall through
                case 3: pos6 = pos7 = 32; // fall through
                case 4: pos8 = 32; // fall through
                case 5:
                    break;
            }

            /* Codes_SRS_BASE32_07_011: [ base32_encode_impl shall then map the 5 bit chunks into one of the BASE32 values (a-z,2,3,4,5,6,7) values. ] */
            result[result_len++] = BASE32_VALUES[pos1];
            result[result_len++] = BASE32_VALUES[pos2];
            result[result_len++] = BASE32_VALUES[pos3];
            result[result_len++] = BASE32_VALUES[pos4];
            result[result_len++] = BASE32_VALUES[pos5];
            result[result_len++] = BASE32_VALUES[pos6];
            result[result_len++] = BASE32_VALUES[pos7];
            result[result_len++] = BASE32_VALUES[pos8];
        }
    }
    return result;
}

static BUFFER_HANDLE base32_decode_impl(const char* source)
{
    BUFFER_HANDLE result;

    size_t src_length = strlen(source);
    if (src_length % BASE32_INPUT_SIZE != 0)
    {
        /* Codes_SRS_BASE32_07_021: [ If the source length is not evenly divisible by 8, base32_decode_impl shall return NULL. ] */
        LogError("Failure invalid input length %lu", (unsigned long)src_length);
        result = NULL;
    }
    else
    {
        size_t dest_size = 0;
        unsigned char* temp_buffer;
        unsigned char* dest_buff;
        bool continue_processing = true;
        unsigned char input[8];
        const char* iterator = source;

        /* Codes_SRS_BASE32_07_022: [ base32_decode_impl shall allocate a temp buffer to store the in process value. ] */
        size_t allocation_len = base32_decoding_length(src_length);
        if ((temp_buffer = (unsigned char*)malloc(allocation_len)) == NULL)
        {
            /* Codes_SRS_BASE32_07_023: [ If an error is encountered, base32_decode_impl shall return NULL. ] */
            LogError("Failure allocating buffer");
            result = NULL;
        }
        else
        {
            dest_buff = temp_buffer;
            while (*iterator != '\0')
            {
                size_t index = 0;
                /* Codes_SRS_BASE32_07_024: [ base32_decode_impl shall loop through and collect 8 characters from the source variable. ] */
                for (index = 0; index < BASE32_INPUT_SIZE; index++)
                {
                    input[index] = *iterator;
                    iterator++;
                    if (input[index] >= ASCII_VALUE_MAX)
                    {
                        LogError("Failure source encoding");
                        continue_processing = false;
                        break;
                    }

                    input[index] = convert_value_to_base32_char(input[index]);
                }

                if (!continue_processing)
                {
                    result = NULL;
                    break;
                }
                else if ((dest_size + TARGET_BLOCK_SIZE) > allocation_len)
                {
                    LogError("Failure target length exceeded");
                    result = NULL;
                    continue_processing = false;
                    break;
                }
                else
                {
                    // Codes_SRS_BASE32_07_025: [ base32_decode_impl shall group 5 bytes at a time into the temp buffer. ]
                    *dest_buff++ = ((input[0] & 0x1f) << 3) | ((input[1] & 0x1c) >> 2);
                    *dest_buff++ = ((input[1] & 0x03) << 6) | ((input[2] & 0x1f) << 1) | ((input[3] & 0x10) >> 4);
                    *dest_buff++ = ((input[3] & 0x0f) << 4) | ((input[4] & 0x1e) >> 1);
                    *dest_buff++ = ((input[4] & 0x01) << 7) | ((input[5] & 0x1f) << 2) | ((input[6] & 0x18) >> 3);
                    *dest_buff++ = ((input[6] & 0x07) << 5) | (input[7] & 0x1f);
                    dest_size += TARGET_BLOCK_SIZE;
                    // If there is padding remove it
                    // Because we are packing 5 bytes into an 8 byte variable we need to check every other
                    // variable for padding
                    if (input[7] == BASE32_EQUAL_SIGN)
                    {
                        --dest_size;
                        if (input[5] == BASE32_EQUAL_SIGN)
                        {
                            --dest_size;
                            if (input[4] == BASE32_EQUAL_SIGN)
                            {
                                --dest_size;
                                if (input[2] == BASE32_EQUAL_SIGN)
                                {
                                    --dest_size;
                                }
                            }
                        }
                    }
                }
            }

            if (!continue_processing)
            {
                result = NULL;
            }
            else
            {
                /* Codes_SRS_BASE32_07_026: [ Once base32_decode_impl is complete it shall create a BUFFER with the temp buffer. ] */
                result = BUFFER_create(temp_buffer, dest_size);
                if (result == NULL)
                {
                    LogError("Failure: BUFFER_create failed to create decoded buffer");
                }
            }
            free(temp_buffer);
        }
    }
    return result;
}

BUFFER_HANDLE Azure_Base32_Decode(STRING_HANDLE handle)
{
    BUFFER_HANDLE result;
    if (handle == NULL)
    {
        /* Codes_SRS_BASE32_07_016: [ If source is NULL Azure_Base32_Decode shall return NULL. ] */
        LogError("invalid parameter handle");
        result = NULL;
    }
    else
    {
        const char* str_source = STRING_c_str(handle);
        if (str_source == NULL)
        {
            /* Codes_SRS_BASE32_07_027: [ If the string in source value is NULL, Azure_Base32_Decode shall return NULL. ] */
            LogError("NULL value specified in string");
            result = NULL;
        }
        else
        {
            /* Codes_SRS_BASE32_07_018: [ Azure_Base32_Decode shall call base32_decode_impl to decode the base64 value. ] */
            result = base32_decode_impl(str_source);
        }
    }
    /* Codes_SRS_BASE32_07_017: [ On success Azure_Base32_Decode shall return a BUFFER_HANDLE that contains the decoded bytes for source. ] */
    return result;
}

BUFFER_HANDLE Azure_Base32_Decode_String(const char* source)
{
    BUFFER_HANDLE result;
    if (source == NULL)
    {
        /* Codes_SRS_BASE32_07_008: [ If source is NULL Azure_Base32_Decode_String shall return NULL. ] */
        LogError("invalid parameter source=NULL");
        result = NULL;
    }
    else
    {
        /* Codes_SRS_BASE32_07_020: [ Azure_Base32_Decode_String shall call base32_decode_impl to decode the base64 value. ] */
        result = base32_decode_impl(source);
    }
    /* Codes_SRS_BASE32_07_019: [ On success Azure_Base32_Decode_String shall return a BUFFER_HANDLE that contains the decoded bytes for source. ] */
    return result;
}

char* Azure_Base32_Encode_Bytes(const unsigned char* source, size_t size)
{
    char* result;
    if (source == NULL)
    {
        /* Codes_SRS_BASE32_07_004: [ If source is NULL Azure_Base32_Encode_Bytes shall return NULL. ] */
        result = NULL;
        LogError("Failure: Invalid input parameter source");
    }
    else if (size == 0)
    {
        /* Codes_SRS_BASE32_07_005: [ If size is 0 Azure_Base32_Encode_Bytes shall return an empty string. ] */
        if ((result = malloc(1)) != NULL)
        {
            strcpy(result, "");
        }
        else
        {
            LogError("unable to allocate memory for result");
        }
    }
    else
    {
        /* Codes_SRS_BASE32_07_007: [ Azure_Base32_Encode_Bytes shall call into base32_Encode_impl to encode the source data. ] */
        result = base32_encode_impl(source, size);
        if (result == NULL)
        {
            /* Codes_SRS_BASE32_07_014: [ Upon failure Azure_Base32_Encode_Bytes shall return NULL. ] */
            LogError("encoding of unsigned char failed.");
        }
    }
    /* Codes_SRS_BASE32_07_006: [ If successful Azure_Base32_Encode_Bytes shall return the base32 value of input. ] */
    return result;
}

STRING_HANDLE Azure_Base32_Encode(BUFFER_HANDLE source)
{
    STRING_HANDLE result;
    if (source == NULL)
    {
        /* Codes_SRS_BASE32_07_001: [ If source is NULL Azure_Base32_Encode shall return NULL. ] */
        result = NULL;
        LogError("Failure: Invalid input parameter");
    }
    else
    {
        size_t input_len = BUFFER_length(source);
        const unsigned char* input_value = BUFFER_u_char(source);
        if (input_value == NULL || input_len == 0)
        {
            /* Codes_SRS_BASE32_07_015: [ If size is 0 Azure_Base32_Encode shall return an empty string. ] */
            result = STRING_new();
            if (result == NULL)
            {
                LogError("Failure constructing new string.");
            }
        }
        else
        {
            /* Codes_SRS_BASE32_07_003: [ Azure_Base32_Encode shall call into base32_Encode_impl to encode the source data. ] */
            char* encoded = base32_encode_impl(input_value, input_len);
            if (encoded == NULL)
            {
                /* Codes_SRS_BASE32_07_014: [ Upon failure Azure_Base32_Encode shall return NULL. ] */
                LogError("base32 encode implementation failed.");
                result = NULL;
            }
            else
            {
                /* Codes_SRS_BASE32_07_012: [ Azure_Base32_Encode shall wrap the base32_Encode_impl result into a STRING_HANDLE. ] */
                result = STRING_construct(encoded);
                if (result == NULL)
                {
                    /* Codes_SRS_BASE32_07_014: [ Upon failure Azure_Base32_Encode shall return NULL. ] */
                    LogError("string construction failed.");
                }
                free(encoded);
            }
        }
    }
    /* Codes_SRS_BASE32_07_002: [ If successful Azure_Base32_Encode shall return the base32 value of source. ] */
    return result;
}
