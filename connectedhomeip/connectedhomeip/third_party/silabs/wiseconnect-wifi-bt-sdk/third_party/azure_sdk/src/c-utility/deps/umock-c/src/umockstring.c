// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stddef.h>
#include <string.h>
#include "umock_c/umockalloc.h"
#include "umock_c/umockstring.h"
#include "umock_c/umock_log.h"

char* umockstring_clone(const char* source)
{
    char* result;

    if (source == NULL)
    {
        /* Codes_UMOCK_STRING_01_005: [ If umockstring_clone is called with a NULL source, it shall return NULL. ]*/
        UMOCK_LOG("umockstring_clone called with NULL source");
        result = NULL;
    }
    else
    {
        size_t string_length = strlen(source);

        /* Codes_UMOCK_STRING_01_001: [ umockstring_clone shall allocate memory for the cloned string (including the NULL terminator). ]*/
        /* Codes_UMOCK_STRING_01_003: [ On success umockstring_clone shall return a pointer to the newly allocated memory containing the copy of the string. ]*/
        result = (char*)umockalloc_malloc(string_length + 1);
        if (result == NULL)
        {
            /* Codes_UMOCK_STRING_01_004: [ If allocating the memory fails, umockstring_clone shall return NULL. ]*/
            UMOCK_LOG("Error allocating memory for string clone");
        }
        else
        {
            /* Codes_UMOCK_STRING_01_002: [ umockstring_clone shall copy the string to the newly allocated memory (including the NULL terminator). ]*/
            (void)memcpy(result, source, string_length + 1);
        }
    }

    return result;
}
