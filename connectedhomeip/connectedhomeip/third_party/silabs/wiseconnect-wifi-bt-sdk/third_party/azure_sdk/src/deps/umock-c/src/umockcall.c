// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stddef.h>
#include <string.h>
#include "umock_c/umockcall.h"
#include "umock_c/umockalloc.h"
#include "umock_c/umock_log.h"

typedef struct UMOCKCALL_TAG
{
    char* function_name;
    void* umockcall_data;
    UMOCKCALL_DATA_COPY_FUNC umockcall_data_copy;
    UMOCKCALL_DATA_FREE_FUNC umockcall_data_free;
    UMOCKCALL_DATA_STRINGIFY_FUNC umockcall_data_stringify;
    UMOCKCALL_DATA_ARE_EQUAL_FUNC umockcall_data_are_equal;
    unsigned int fail_call : 1;
    unsigned int ignore_all_calls : 1;
    unsigned int call_can_fail : 1;
} UMOCKCALL;

UMOCKCALL_HANDLE umockcall_create(const char* function_name, void* umockcall_data, UMOCKCALL_DATA_COPY_FUNC umockcall_data_copy, UMOCKCALL_DATA_FREE_FUNC umockcall_data_free, UMOCKCALL_DATA_STRINGIFY_FUNC umockcall_data_stringify, UMOCKCALL_DATA_ARE_EQUAL_FUNC umockcall_data_are_equal)
{
    UMOCKCALL* result;

    if ((function_name == NULL) ||
        (umockcall_data == NULL) ||
        (umockcall_data_copy == NULL) ||
        (umockcall_data_free == NULL) ||
        (umockcall_data_stringify == NULL) ||
        (umockcall_data_are_equal == NULL))
    {
        /* Codes_SRS_UMOCKCALL_01_003: [ If any of the arguments are NULL, umockcall_create shall fail and return NULL. ] */
        UMOCK_LOG("umockcall: Cannot create call, invalid arguments: function_name = %p, umockcall_data = %p, umockcall_data_free = %p, umockcall_data_stringify = %p, umockcall_data_are_equal = %p.",
            function_name, umockcall_data, umockcall_data_free, umockcall_data_stringify, umockcall_data_are_equal);
        result = NULL;
    }
    else
    {
        /* Codes_SRS_UMOCKCALL_01_001: [ umockcall_create shall create a new instance of a umock call and on success it shall return a non-NULL handle to it. ] */
        result = (UMOCKCALL*)umockalloc_malloc(sizeof(UMOCKCALL));
        /* Codes_SRS_UMOCKCALL_01_002: [ If allocating memory for the umock call instance fails, umockcall_create shall return NULL. ] */
        if (result != NULL)
        {
            size_t function_name_length = strlen(function_name);
            result->function_name = (char*)umockalloc_malloc(function_name_length + 1);
            if (result->function_name == NULL)
            {
                /* Codes_SRS_UMOCKCALL_01_002: [ If allocating memory for the umock call instance fails, umockcall_create shall return NULL. ] */
                UMOCK_LOG("umockcall: Cannot allocate memory for the call function name.");
                umockalloc_free(result);
                result = NULL;
            }
            else
            {
                (void)memcpy(result->function_name, function_name, function_name_length + 1);
                result->umockcall_data = umockcall_data;
                result->umockcall_data_copy = umockcall_data_copy;
                result->umockcall_data_free = umockcall_data_free;
                result->umockcall_data_stringify = umockcall_data_stringify;
                result->umockcall_data_are_equal = umockcall_data_are_equal;
                result->fail_call = 0;
                result->ignore_all_calls = 0;
            }
        }
    }

    return result;
}

void umockcall_destroy(UMOCKCALL_HANDLE umockcall)
{
    /* Codes_SRS_UMOCKCALL_01_005: [ If the umockcall argument is NULL then umockcall_destroy shall do nothing. ]*/
    if (umockcall != NULL)
    {
        /* Codes_SRS_UMOCKCALL_01_004: [ umockcall_destroy shall free a previously allocated umock call instance. ] */
        umockcall->umockcall_data_free(umockcall->umockcall_data);
        umockalloc_free(umockcall->function_name);
        umockalloc_free(umockcall);
    }
}

/* Codes_SRS_UMOCKCALL_01_006: [ umockcall_are_equal shall compare the two mock calls and return whether they are equal or not. ] */
int umockcall_are_equal(UMOCKCALL_HANDLE left, UMOCKCALL_HANDLE right)
{
    int result;

    if (left == right)
    {
        /* Codes_SRS_UMOCKCALL_01_024: [ If both left and right pointers are equal, umockcall_are_equal shall return 1. ] */
        result = 1;
    }
    else if ((left == NULL) || (right == NULL))
    {
        /* Codes_SRS_UMOCKCALL_01_015: [ If only one of the left or right arguments are NULL, umockcall_are_equal shall return 0. ] */
        result = 0;
    }
    else
    {
        if (left->umockcall_data_are_equal != right->umockcall_data_are_equal)
        {
            /* Codes_SRS_UMOCKCALL_01_014: [ If the two calls have different are_equal functions that have been passed to umockcall_create then the calls shall be considered different and 0 shall be returned. ] */
            result = 0;
        }
        else if (strcmp(left->function_name, right->function_name) != 0)
        {
            /* Codes_SRS_UMOCKCALL_01_025: [ If the function name does not match for the 2 calls, umockcall_are_equal shall return 0. ]*/
            result = 0;
        }
        else
        {
            /* Codes_SRS_UMOCKCALL_01_026: [ The call data shall be evaluated by calling the umockcall_data_are_equal function passed in umockcall_create while passing as arguments the umockcall_data associated with each call handle. ]*/
            switch (left->umockcall_data_are_equal(left->umockcall_data, right->umockcall_data))
            {
            default:
                /* Codes_SRS_UMOCKCALL_01_029: [ If the underlying umockcall_data_are_equal fails (returns anything else than 0 or 1), then umockcall_are_equal shall fail and return -1. ] */
                UMOCK_LOG("umockcall: comparing call data failed.");
                result = -1;
                break;
            case 1:
                /* Codes_SRS_UMOCKCALL_01_027: [ If the underlying umockcall_data_are_equal returns 1, then umockcall_are_equal shall return 1. ]*/
                result = 1;
                break;
            case 0:
                /* Codes_SRS_UMOCKCALL_01_028: [ If the underlying umockcall_data_are_equal returns 0, then umockcall_are_equal shall return 0. ]*/
                result = 0;
                break;
            }
        }
    }

    return result;
}

char* umockcall_stringify(UMOCKCALL_HANDLE umockcall)
{
    char* result;

    if (umockcall == NULL)
    {
        /* Codes_SRS_UMOCKCALL_01_017: [ If the umockcall argument is NULL, umockcall_stringify shall return NULL. ]*/
        UMOCK_LOG("umockcall: NULL umockcall in stringify.");
        result = NULL;
    }
    else
    {
        /* Codes_SRS_UMOCKCALL_01_019: [ To obtain the arguments string, umockcall_stringify shall call the umockcall_data_stringify function passed to umockcall_create and pass to it the umockcall_data pointer (also given in umockcall_create). ]*/
        char* stringified_args = umockcall->umockcall_data_stringify(umockcall->umockcall_data);
        if (stringified_args == NULL)
        {
            /* Codes_SRS_UMOCKCALL_01_020: [ If the underlying umockcall_data_stringify call fails, umockcall_stringify shall fail and return NULL. ]*/
            UMOCK_LOG("umockcall: umockcall data stringify failed.");
            result = NULL;
        }
        else
        {
            /* Codes_SRS_UMOCKCALL_01_016: [ umockcall_stringify shall return a string representation of the mock call in the form "[function_name(arguments)]". ] */
            size_t function_name_length = strlen(umockcall->function_name);
            size_t stringified_args_length = strlen(stringified_args);

            /* 4 because () and [] */
            size_t call_length = function_name_length + stringified_args_length + 4;

            /* Codes_SRS_UMOCKCALL_01_018: [ The returned string shall be a newly allocated string and it is to be freed by the caller. ]*/
            result = (char*)umockalloc_malloc(call_length + 1);
            /* Codes_SRS_UMOCKCALL_01_021: [ If not enough memory can be allocated for the string to be returned, umockcall_stringify shall fail and return NULL. ]*/
            if (result != NULL)
            {
                result[0] = '[';
                (void)memcpy(&result[1], umockcall->function_name, function_name_length);
                result[function_name_length + 1] = '(';
                (void)memcpy(&result[function_name_length + 2], stringified_args, stringified_args_length);
                result[function_name_length + stringified_args_length + 2] = ')';
                result[function_name_length + stringified_args_length + 3] = ']';
                result[function_name_length + stringified_args_length + 4] = '\0';
            }

            /* Codes_SRS_UMOCKCALL_01_030: [ umockcall_stringify shall free the string obtained from umockcall_data_stringify. ]*/
            umockalloc_free(stringified_args);
        }
    }

    return result;
}

void* umockcall_get_call_data(UMOCKCALL_HANDLE umockcall)
{
    void* umockcall_data;

    if (umockcall == NULL)
    {
        /* Codes_SRS_UMOCKCALL_01_023: [ If umockcall is NULL, umockcall_get_call_data shall return NULL. ]*/
        UMOCK_LOG("umockcall: NULL umockcall in getting call data.");
        umockcall_data = NULL;
    }
    else
    {
        /* Codes_SRS_UMOCKCALL_01_022: [ umockcall_get_call_data shall return the associated umock call data that was passed to umockcall_create. ]*/
        umockcall_data = umockcall->umockcall_data;
    }

    return umockcall_data;
}

/* Codes_SRS_UMOCKCALL_01_031: [ umockcall_clone shall clone a umock call and on success it shall return a handle to the newly cloned call. ] */
UMOCKCALL_HANDLE umockcall_clone(UMOCKCALL_HANDLE umockcall)
{
    UMOCKCALL_HANDLE result;

    if (umockcall == NULL)
    {
        /* Codes_SRS_UMOCKCALL_01_032: [ If umockcall is NULL, umockcall_clone shall return NULL. ]*/
        UMOCK_LOG("umockcall_clone: NULL umockcall in getting call data.");
        result = NULL;
    }
    else
    {
        result = (UMOCKCALL*)umockalloc_malloc(sizeof(UMOCKCALL));
        if (result == NULL)
        {
            /* Codes_SRS_UMOCKCALL_01_043: [ If allocating memory for the new umock call fails, umockcall_clone shall return NULL. ]*/
            UMOCK_LOG("umockcall_clone: Failed allocating memory for new copied call.");
        }
        else
        {
            size_t function_name_length = strlen(umockcall->function_name);
            result->function_name = (char*)umockalloc_malloc(function_name_length + 1);
            if (result->function_name == NULL)
            {
                /* Codes_SRS_UMOCKCALL_01_036: [ If allocating memory for the function name fails, umockcall_clone shall return NULL. ]*/
                UMOCK_LOG("umockcall_clone: Failed allocating memory for new copied call function name.");
                umockalloc_free(result);
                result = NULL;
            }
            else
            {
                /* Codes_SRS_UMOCKCALL_01_035: [ umockcall_clone shall copy also the function name. ]*/
                (void)memcpy(result->function_name, umockcall->function_name, function_name_length + 1);

                /* Codes_SRS_UMOCKCALL_01_033: [ The call data shall be cloned by calling the umockcall_data_copy function passed in umockcall_create and passing as argument the umockcall_data value passed in umockcall_create. ]*/
                result->umockcall_data = umockcall->umockcall_data_copy(umockcall->umockcall_data);
                if (result->umockcall_data == NULL)
                {
                    /* Codes_SRS_UMOCKCALL_01_034: [ If umockcall_data_copy fails then umockcall_clone shall return NULL. ]*/
                    UMOCK_LOG("umockcall_clone: Failed copying call data.");
                    umockalloc_free(result->function_name);
                    umockalloc_free(result);
                    result = NULL;
                }
                else
                {
                    /* Codes_SRS_UMOCKCALL_01_037: [ umockcall_clone shall also copy all the functions passed to umockcall_create (umockcall_data_copy, umockcall_data_free, umockcall_data_are_equal, umockcall_data_stringify). ]*/
                    result->umockcall_data_are_equal = umockcall->umockcall_data_are_equal;
                    result->umockcall_data_copy = umockcall->umockcall_data_copy;
                    result->umockcall_data_free = umockcall->umockcall_data_free;
                    result->umockcall_data_stringify = umockcall->umockcall_data_stringify;
                    result->ignore_all_calls = umockcall->ignore_all_calls;
                    result->call_can_fail = umockcall->call_can_fail;
                    result->fail_call = umockcall->fail_call;
                }
            }
        }
    }

    return result;
}

int umockcall_set_fail_call(UMOCKCALL_HANDLE umockcall, int fail_call)
{
    int result;

    if (umockcall == NULL)
    {
        /* Codes_SRS_UMOCKCALL_01_039: [ If umockcall is NULL, umockcall_set_fail_call shall return a non-zero value. ]*/
        UMOCK_LOG("umockcall_set_fail_call: NULL umockcall.");
        result = __LINE__;
    }
    else
    {
        switch (fail_call)
        {
        default:
            /* Codes_SRS_UMOCKCALL_01_040: [ If a value different than 0 and 1 is passed as fail_call, umockcall_set_fail_call shall return a non-zero value. ]*/
            UMOCK_LOG("umockcall_set_fail_call: Invalid fail_call value: %d.", fail_call);
            result = __LINE__;
            break;
        case 0:
            /* Codes_SRS_UMOCKCALL_01_038: [ umockcall_set_fail_call shall store the fail_call value, associating it with the umockcall call instance. ]*/
            umockcall->fail_call = 0;
            /* Codes_SRS_UMOCKCALL_01_044: [ On success umockcall_set_fail_call shall return 0. ]*/
            result = 0;
            break;
        case 1:
            /* Codes_SRS_UMOCKCALL_01_038: [ umockcall_set_fail_call shall store the fail_call value, associating it with the umockcall call instance. ]*/
            umockcall->fail_call = 1;
            /* Codes_SRS_UMOCKCALL_01_044: [ On success umockcall_set_fail_call shall return 0. ]*/
            result = 0;
            break;
        }
    }

    return result;
}

int umockcall_get_fail_call(UMOCKCALL_HANDLE umockcall)
{
    int result;

    if (umockcall == NULL)
    {
        /* Codes_SRS_UMOCKCALL_01_042: [ If umockcall is NULL, umockcall_get_fail_call shall return -1. ]*/
        UMOCK_LOG("NULL umokcall argument.");
        result = -1;
    }
    else
    {
        /* Codes_SRS_UMOCKCALL_01_041: [ umockcall_get_fail_call shall retrieve the fail_call value, associated with the umockcall call instance. ]*/
        result = umockcall->fail_call ? 1 : 0;
    }

    return result;
}

int umockcall_set_ignore_all_calls(UMOCKCALL_HANDLE umockcall, int ignore_all_calls)
{
    int result;

    if (umockcall == NULL)
    {
        /* Codes_SRS_UMOCKCALL_01_047: [ If umockcall is NULL, umockcall_set_ignore_all_calls shall return a non-zero value. ]*/
        UMOCK_LOG("umockcall_set_fail_call: NULL umockcall.");
        result = __LINE__;
    }
    else
    {
        switch (ignore_all_calls)
        {
        default:
            /* Codes_SRS_UMOCKCALL_01_048: [ If a value different than 0 and 1 is passed as ignore_all_calls, umockcall_set_ignore_all_calls shall return a non-zero value. ]*/
            UMOCK_LOG("umockcall_set_fail_call: Invalid ignore_all_calls value: %d.", ignore_all_calls);
            result = __LINE__;
            break;
        case 0:
            /* Codes_SRS_UMOCKCALL_01_045: [ umockcall_set_ignore_all_calls shall store the ignore_all_calls value, associating it with the umockcall call instance. ]*/
            umockcall->ignore_all_calls = 0;
            /* Codes_SRS_UMOCKCALL_01_046: [ On success umockcall_set_ignore_all_calls shall return 0. ]*/
            result = 0;
            break;
        case 1:
            /* Codes_SRS_UMOCKCALL_01_045: [ umockcall_set_ignore_all_calls shall store the ignore_all_calls value, associating it with the umockcall call instance. ]*/
            umockcall->ignore_all_calls = 1;
            /* Codes_SRS_UMOCKCALL_01_046: [ On success umockcall_set_ignore_all_calls shall return 0. ]*/
            result = 0;
            break;
        }
    }

    return result;
}

int umockcall_get_ignore_all_calls(UMOCKCALL_HANDLE umockcall)
{
    int result;

    if (umockcall == NULL)
    {
        /* Codes_SRS_UMOCKCALL_01_050: [ If umockcall is NULL, umockcall_get_ignore_all_calls shall return -1. ]*/
        UMOCK_LOG("NULL umokcall argument.");
        result = -1;
    }
    else
    {
        /* Codes_SRS_UMOCKCALL_01_049: [ umockcall_get_ignore_all_calls shall retrieve the ignore_all_calls value, associated with the umockcall call instance. ]*/
        result = umockcall->ignore_all_calls ? 1 : 0;
    }

    return result;
}

int umockcall_set_call_can_fail(UMOCKCALL_HANDLE umockcall, int call_can_fail)
{
    int result;

    if (umockcall == NULL)
    {
        /* Codes_SRS_UMOCKCALL_31_051: [ If umockcall is NULL, umockcall_set_call_can_fail shall return -1. ]*/
        UMOCK_LOG("umockcall_set_fail_call: NULL umockcall.");
        result = -1;
    }
    else
    {
        switch (call_can_fail)
        {
        default:
            /* Codes_SRS_UMOCKCALL_31_052: [ If a value different than 0 and 1 is passed as umockcall_set_call_can_fail, umockcall_set_call_can_fail shall return -1. ]*/
            UMOCK_LOG("umockcall_c_set_can_fail: Invalid ignore_all_calls value: %d.", call_can_fail);
            result = -1;
            break;
        case 0:
            /* Codes_SRS_UMOCKCALL_31_053: [ umockcall_set_call_can_fail shall store the call_can_fail value, associating it with the umockcall call instance. ]*/
            umockcall->call_can_fail = 0;
            result = 0;
            break;
        case 1:
            /* Codes_SRS_UMOCKCALL_31_053: [ umockcall_set_call_can_fail shall store the call_can_fail value, associating it with the umockcall call instance. ]*/
            umockcall->call_can_fail = 1;
            result = 0;
            break;
        }
    }

    return result;
}

int umockcall_get_call_can_fail(UMOCKCALL_HANDLE umockcall)
{
    int result;

    if (umockcall == NULL)
    {
        /* Codes_SRS_UMOCKCALL_31_054: [ If umockcall is NULL, umockcall_get_call_can_fail shall return -1. ]*/
        UMOCK_LOG("NULL umockcall argument.");
        result = -1;
    }
    else
    {
        /* Codes_SRS_UMOCKCALL_31_055: [ umockcall_get_call_can_fail shall retrieve the call_can_fail value, associated with the umockcall call instance. ]*/
        result = umockcall->call_can_fail ? 1 : 0;
    }

    return result;
}


