// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <errno.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <wchar.h>
#include "umock_c/umocktypes.h"
#include "azure_macro_utils/macro_utils.h"
#include "umock_c/umocktypes_wcharptr.h"
#include "umock_c/umockalloc.h"
#include "umock_c/umock_log.h"

static void mbstate_t_init(mbstate_t* ps)
{
    (void)memset(ps, 0, sizeof(*ps));
    const wchar_t empty[] = L"";
    const wchar_t* srcp = empty;
    (void)wcsrtombs(NULL, &srcp, 0, ps);
}

char* umocktypes_stringify_wcharptr(const wchar_t** value)
{
    char* result;

    if (value == NULL)
    {
        /* Codes_SRS_UMOCKTYPES_WCHARPTR_01_004: [ If value is NULL, umocktypes_stringify_wcharptr shall return NULL. ]*/
        UMOCK_LOG("umocktypes_stringify_wcharptr: NULL value.");
        result = NULL;
    }
    else
    {
        if (*value == NULL)
        {
            result = (char*)umockalloc_malloc(sizeof("NULL"));
            if (result != NULL)
            {
                (void)memcpy(result, "NULL", sizeof("NULL"));
            }
        }
        else
        {
            /* Codes_SRS_UMOCKTYPES_WCHARPTR_01_002: [ umocktypes_stringify_wcharptr shall return a string containing the string representation of value, enclosed by quotes ("value"). ] */
            const wchar_t* same_as_source = *value;
            mbstate_t state;
            mbstate_t_init(&state);
            size_t num_characters = wcsrtombs(NULL, &same_as_source, 0, &state);
            if (num_characters == (size_t)(-1))
            {
                UMOCK_LOG("failure to get the length of the string: %s", strerror(errno));
                result = NULL;
            }
            else
            {
                result = (char*)umockalloc_malloc(num_characters + 3);
                /* Codes_SRS_UMOCKTYPES_WCHARPTR_01_003: [ If allocating a new string to hold the string representation fails, umocktypes_stringify_wcharptr shall return NULL. ]*/
                if (result == NULL)
                {
                    UMOCK_LOG("umocktypes_stringify_wcharptr: Cannot allocate memory for result.");
                }
                else
                {
                    size_t num_characters2 = wcsrtombs(result + 1, &same_as_source, num_characters + 1, &state);
                    if (num_characters2 != num_characters)
                    {
                        UMOCK_LOG("unexpected inconsistency in wcsrtombs");
                        umockalloc_free(result);
                        result = NULL;
                    }
                    else
                    {
                        result[0] = '\"';
                        result[num_characters + 1] = '\"';
                        result[num_characters + 2] = '\0';
                        // OK
                    }
                }
            }
        }
    }

    return result;
}

/* Codes_SRS_UMOCKTYPES_WCHARPTR_01_005: [ umocktypes_are_equal_wcharptr shall compare the 2 strings pointed to by left and right. ] */
int umocktypes_are_equal_wcharptr(const wchar_t** left, const wchar_t** right)
{
    int result;

    if (
        /* Codes_SRS_UMOCKTYPES_WCHARPTR_42_001: [ If left is NULL, umocktypes_are_equal_wcharptr shall return -1. ]*/
        (left == NULL) ||
        /* Codes_SRS_UMOCKTYPES_WCHARPTR_42_002: [ If right is NULL, umocktypes_are_equal_wcharptr shall return -1. ]*/
        (right == NULL))
    {
        UMOCK_LOG("umocktypes_are_equal_wcharptr: Bad arguments:left = %p, right = %p.", left, right);
        result = -1;
    }
    else if (*left == *right)
    {
        /* Codes_SRS_UMOCKTYPES_WCHARPTR_01_007: [ If left and right are equal, umocktypes_are_equal_wcharptr shall return 1. ]*/
        result = 1;
    }
    else if ((*left == NULL) || (*right == NULL))
    {
        /* Codes_SRS_UMOCKTYPES_WCHARPTR_01_008: [ If only one of the left and right argument is NULL, umocktypes_are_equal_wcharptr shall return 0. ] */
        result = 0;
    }
    else
    {
        /* Codes_SRS_UMOCKTYPES_WCHARPTR_01_009: [ If the string pointed to by left is equal to the string pointed to by right, umocktypes_are_equal_wcharptr shall return 1. ]*/
        /* Codes_SRS_UMOCKTYPES_WCHARPTR_01_010: [ If the string pointed to by left is different than the string pointed to by right, umocktypes_are_equal_wcharptr shall return 0. ]*/
        /* Codes_SRS_UMOCKTYPES_WCHARPTR_01_006: [ The comparison shall be case sensitive. ]*/
        result = (wcscmp(*left, *right) == 0) ? 1 : 0;
    }

    return result;
}

int umocktypes_copy_wcharptr(wchar_t** destination, const wchar_t** source)
{
    int result;

    /* Codes_SRS_UMOCKTYPES_WCHARPTR_01_013: [ If source or destination are NULL, umocktypes_copy_wcharptr shall return a non-zero value. ]*/
    if ((destination == NULL) || (source == NULL))
    {
        UMOCK_LOG("umocktypes_copy_wcharptr: Bad arguments: destination = %p, source = %p.",
            destination, source);
        result = __LINE__;
    }
    else
    {
        if (*source == NULL)
        {
            *destination = NULL;
            result = 0;
        }
        else
        {
            size_t source_length = wcslen(*source);
            /* Codes_SRS_UMOCKTYPES_WCHARPTR_01_012: [ The number of bytes allocated shall accommodate the string pointed to by source. ]*/
            /* Codes_SRS_UMOCKTYPES_WCHARPTR_01_011: [ umocktypes_copy_wcharptr shall allocate a new sequence of chars by using umockalloc_malloc. ]*/
            /* Codes_SRS_UMOCKTYPES_WCHARPTR_01_015: [ The newly allocated string shall be returned in the destination argument. ]*/
            *destination = (wchar_t*)umockalloc_malloc((source_length + 1) * sizeof(wchar_t));
            if (*destination == NULL)
            {
                /* Codes_SRS_UMOCKTYPES_WCHARPTR_01_036: [ If allocating the memory for the new string fails, umocktypes_copy_wcharptr shall fail and return a non-zero value. ]*/
                UMOCK_LOG("umocktypes_copy_wcharptr: Failed allocating memory for the destination string.");
                result = __LINE__;
            }
            else
            {
                /* Codes_SRS_UMOCKTYPES_WCHARPTR_01_014: [ umocktypes_copy_wcharptr shall copy the string pointed to by source to the newly allocated memory. ]*/
                (void)memcpy(*destination, *source, (source_length + 1) * sizeof(wchar_t));
                /* Codes_SRS_UMOCKTYPES_WCHARPTR_01_016: [ On success umocktypes_copy_wcharptr shall return 0. ]*/
                result = 0;
            }
        }
    }

    return result;
}

void umocktypes_free_wcharptr(wchar_t** value)
{
    /* Codes_SRS_UMOCKTYPES_WCHARPTR_01_018: [ If value is NULL, umocktypes_free_wcharptr shall do nothing. ] */
    if (value != NULL)
    {
        /* Codes_SRS_UMOCKTYPES_WCHARPTR_01_017: [ umocktypes_free_wcharptr shall free the string pointed to by value. ]*/
        umockalloc_free(*value);
    }
}

char* umocktypes_stringify_const_wcharptr(const wchar_t** value)
{
    char* result;

    if (value == NULL)
    {
        /* Codes_SRS_UMOCKTYPES_WCHARPTR_01_020: [ If value is NULL, umocktypes_stringify_const_wcharptr shall return NULL. ]*/
        UMOCK_LOG("umocktypes_stringify_const_wcharptr: NULL value.");
        result = NULL;
    }
    else
    {
        if (*value == NULL)
        {
            result = (char*)umockalloc_malloc(sizeof("NULL"));
            if (result == NULL)
            {
                UMOCK_LOG("umocktypes_stringify_const_wcharptr: Cannot allocate memory for result string.");
            }
            else
            {
                (void)memcpy(result, "NULL", sizeof("NULL"));
            }
        }
        else
        {
            /* Codes_SRS_UMOCKTYPES_WCHARPTR_01_019: [ umocktypes_stringify_const_wcharptr shall return a string containing the string representation of value, enclosed by quotes ("value"). ] */
            const wchar_t* same_as_source = *value;
            mbstate_t state;
            mbstate_t_init(&state);
            size_t num_characters = wcsrtombs(NULL, &same_as_source, 0, &state);
            if (num_characters == (size_t)(-1))
            {
                UMOCK_LOG("failure to get the length of the string: %s", strerror(errno));
                result = NULL;
            }
            else
            {
                result = (char*)umockalloc_malloc(num_characters + 3);
                /* Codes_SRS_UMOCKTYPES_WCHARPTR_01_021: [ If allocating a new string to hold the string representation fails, umocktypes_stringify_const_wcharptr shall return NULL. ]*/
                if (result != NULL)
                {
                    size_t num_characters2 = wcsrtombs(result + 1, &same_as_source, num_characters + 1, &state);
                    if (num_characters2 != num_characters)
                    {
                        UMOCK_LOG("unexpected inconsistency in wcsrtombs");
                        umockalloc_free(result);
                        result = NULL;
                    }
                    else
                    {
                        result[0] = '\"';
                        result[num_characters + 1] = '\"';
                        result[num_characters + 2] = '\0';
                        // OK
                    }
                }
            }
        }
    }

    return result;
}

/* Codes_SRS_UMOCKTYPES_WCHARPTR_01_022: [ umocktypes_are_equal_const_wcharptr shall compare the 2 strings pointed to by left and right. ] */
int umocktypes_are_equal_const_wcharptr(const wchar_t** left, const wchar_t** right)
{
    int result;

    /* Codes_SRS_UMOCKTYPES_WCHARPTR_01_024: [ If left and right are equal, umocktypes_are_equal_const_wcharptr shall return 1. ]*/
    if (left == right)
    {
        result = 1;
    }
    else if ((left == NULL) || (right == NULL))
    {
        /* Codes_SRS_UMOCKTYPES_WCHARPTR_01_025: [ If only one of the left and right argument is NULL, umocktypes_are_equal_const_wcharptr shall return 0. ] */
        result = 0;
    }
    else
    {
        /* Codes_SRS_UMOCKTYPES_WCHARPTR_01_026: [ If the string pointed to by left is equal to the string pointed to by right, umocktypes_are_equal_const_wcharptr shall return 1. ]*/
        /* Codes_SRS_UMOCKTYPES_WCHARPTR_01_027: [ If the string pointed to by left is different than the string pointed to by right, umocktypes_are_equal_const_wcharptr shall return 0. ]*/
        /* Codes_SRS_UMOCKTYPES_WCHARPTR_01_023: [ The comparison shall be case sensitive. ]*/
        if (*left == *right)
        {
            result = 1;
        }
        else
        {
            if ((*left == NULL) || (*right == NULL))
            {
                result = 0;
            }
            else
            {
                result = (wcscmp(*left, *right) == 0) ? 1 : 0;
            }
        }
    }

    return result;
}

int umocktypes_copy_const_wcharptr(const wchar_t** destination, const wchar_t** source)
{
    int result;

    /* Codes_SRS_UMOCKTYPES_WCHARPTR_01_033: [ If source or destination are NULL, umocktypes_copy_const_wcharptr shall return a non-zero value. ]*/
    if ((destination == NULL) || (source == NULL))
    {
        UMOCK_LOG("umocktypes_copy_const_wcharptr: Bad arguments: destination = %p, source = %p.",
            destination, source);
        result = __LINE__;
    }
    else
    {
        if (*source == NULL)
        {
            *destination = NULL;
            result = 0;
        }
        else
        {
            size_t source_length = wcslen(*source);
            /* Codes_SRS_UMOCKTYPES_WCHARPTR_01_029: [ The number of bytes allocated shall accommodate the string pointed to by source. ]*/
            /* Codes_SRS_UMOCKTYPES_WCHARPTR_01_028: [ umocktypes_copy_const_wcharptr shall allocate a new sequence of chars by using umockalloc_malloc. ]*/
            /* Codes_SRS_UMOCKTYPES_WCHARPTR_01_031: [ The newly allocated string shall be returned in the destination argument. ]*/
            *destination = (wchar_t*)umockalloc_malloc((source_length + 1) * sizeof(wchar_t));
            if (*destination == NULL)
            {
                /* Codes_SRS_UMOCKTYPES_WCHARPTR_01_037: [ If allocating the memory for the new string fails, umocktypes_copy_const_wcharptr shall fail and return a non-zero value. ]*/
                UMOCK_LOG("umocktypes_copy_const_wcharptr: Cannot allocate memory for destination string.");
                result = __LINE__;
            }
            else
            {
                /* Codes_SRS_UMOCKTYPES_WCHARPTR_01_030: [ umocktypes_copy_const_wcharptr shall copy the string pointed to by source to the newly allocated memory. ]*/
                (void)memcpy((void*)*destination, *source, (source_length + 1) * sizeof(wchar_t));
                /* Codes_SRS_UMOCKTYPES_WCHARPTR_01_032: [ On success umocktypes_copy_const_wcharptr shall return 0. ]*/
                result = 0;
            }
        }
    }

    return result;
}

void umocktypes_free_const_wcharptr(const wchar_t** value)
{
    /* Codes_SRS_UMOCKTYPES_WCHARPTR_01_035: [ If value is NULL, umocktypes_free_const_wcharptr shall do nothing. ] */
    if (value != NULL)
    {
        /* Codes_SRS_UMOCKTYPES_WCHARPTR_01_034: [ umocktypes_free_const_wcharptr shall free the string pointed to by value. ]*/
        umockalloc_free((void*)*value);
    }
}

int umocktypes_wcharptr_register_types(void)
{
    int result;

    /* Codes_SRS_UMOCKTYPES_WCHARPTR_01_001: [ umocktypes_wcharptr_register_types shall register support for the types char\* and const char\* by using the REGISTER_UMOCK_VALUE_TYPE macro provided by umockc. ]*/
    if ((REGISTER_TYPE(wchar_t*, wcharptr) != 0) ||
        (REGISTER_TYPE(const wchar_t*, const_wcharptr) != 0) ||
        (REGISTER_TYPE(wchar_t const*, const_wcharptr) != 0))
    {
        /* Codes_SRS_UMOCKTYPES_WCHARPTR_01_039: [ If registering any of the types fails, umocktypes_wcharptr_register_types shall fail and return a non-zero value. ]*/
        UMOCK_LOG("umocktypes_wcharptr_register_types: Cannot register types.");
        result = __LINE__;
    }
    else
    {
        /* Codes_SRS_UMOCKTYPES_WCHARPTR_01_038: [ On success, umocktypes_wcharptr_register_types shall return 0. ]*/
        result = 0;
    }

    return result;
}
