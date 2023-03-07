// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include "umock_c/umockalloc.h"

void* umockalloc_malloc(size_t size)
{
    /* Codes_SRS_UMOCKALLOC_01_001: [ umockalloc_malloc shall call malloc, while passing the size argument to malloc. ] */
    /* Codes_SRS_UMOCKALLOC_01_002: [ umockalloc_malloc shall return the result of malloc. ]*/
    return malloc(size);
}

void* umockalloc_calloc(size_t nmemb, size_t size)
{
    /* Codes_SRS_UMOCKALLOC_09_001: [ umockalloc_calloc shall call calloc, while passing the number of members and size arguments to calloc. ] */
    /* Codes_SRS_UMOCKALLOC_09_002: [ umockalloc_calloc shall return the result of calloc. ]*/
    return calloc(nmemb, size);
}

void* umockalloc_realloc(void* ptr, size_t size)
{
    /* Codes_SRS_UMOCKALLOC_01_003: [ umockalloc_realloc shall call realloc, while passing the ptr and size arguments to realloc. ] */
    /* Codes_SRS_UMOCKALLOC_01_004: [ umockalloc_realloc shall return the result of realloc. ]*/
    return realloc(ptr, size);
}

void umockalloc_free(void* ptr)
{
    /* Codes_SRS_UMOCKALLOC_01_005: [ umockalloc_free shall call free, while passing the ptr argument to free. ]*/
    free(ptr);
}

char* umockc_stringify_buffer(const void* bytes, size_t length)
{
    size_t string_length = 2 + (4 * length);
    char* result;
    if (length > 1)
    {
        string_length += length - 1;
    }

    result = (char*)umockalloc_malloc(string_length + 1);
    if (result != NULL)
    {
        size_t i;

        result[0] = '[';
        for (i = 0; i < length; i++)
        {
            if (sprintf(result + 1 + (i * 5), "0x%02X ", ((const unsigned char*)bytes)[i]) < 0)
            {
                break;
            }
        }

        if (i < length)
        {
            umockalloc_free(result);
            result = NULL;
        }
        else
        {
            result[string_length - 1] = ']';
            result[string_length] = '\0';
        }
    }

    return result;
}
