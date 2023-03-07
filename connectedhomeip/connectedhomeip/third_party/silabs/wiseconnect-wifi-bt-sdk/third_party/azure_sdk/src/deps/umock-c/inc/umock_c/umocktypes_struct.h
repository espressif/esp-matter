// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef UMOCKTYPES_STRUCT_H
#define UMOCKTYPES_STRUCT_H

#ifdef __cplusplus
#include <cstdint>
#include <cstdio>
extern "C" {
#else
#include <stdint.h>
#include <stdio.h>
#endif

#include "azure_macro_utils/macro_utils.h"

#include "umockalloc.h"
#include "umocktypes.h"
#include "umock_log.h"

/*Codes_SRS_UMOCKTYPES_STRUCT_42_001: [ umocktypes_stringify_<type> shall call umocktypes_stringify for each field in type. ]*/
#define UMOCK_STRUCT_STRINGIFY_STRUCT_FIELD(count, field_type, field_name) \
    char* MU_C2(temp_, field_name) = umocktypes_stringify(MU_TOSTRING(field_type), &value->field_name); \
    if (MU_C2(temp_, field_name) == NULL) \
    { \
        UMOCK_LOG("Failed stringify for field: " MU_TOSTRING(field_name)); \
        is_error = true; \
    } \

/*Codes_SRS_UMOCKTYPES_STRUCT_42_003: [ umocktypes_stringify_<type> shall free all of the stringified fields. ]*/
#define UMOCK_STRUCT_STRINGIFY_STRUCT_FIELD_CLEANUP(count, field_type, field_name) \
    if (MU_C2(temp_, field_name) != NULL) \
    { \
        umockalloc_free(MU_C2(temp_, field_name)); \
    } \

#define UMOCK_STRUCT_STRINGIFY_STRUCT_FIELD_FORMAT_STRING(count, field_type, field_name) \
    MU_TOSTRING(field_name) "=%s" MU_IF(MU_ISEMPTY(MU_IFCOMMA(count)),,",")

#define UMOCK_STRUCT_STRINGIFY_STRUCT_FIELD_NAME(count, field_type, field_name) \
    MU_C2(temp_, field_name) MU_IFCOMMA(count)

/*Codes_SRS_UMOCKTYPES_STRUCT_42_007: [ umocktypes_are_equal_<type> shall call umocktypes_are_equal for each field in type. ]*/
#define UMOCK_STRUCT_ARE_EQUAL_STRUCT_FIELD(count, field_type, field_name) \
    umocktypes_are_equal(MU_TOSTRING(field_type), &left->field_name, &right->field_name) &

/*Codes_SRS_UMOCKTYPES_STRUCT_42_012: [ umocktypes_copy_<type> shall call umocktypes_copy for each field in type. ]*/
#define UMOCK_STRUCT_COPY_STRUCT_FIELD(count, field_type, field_name) \
    umocktypes_copy(MU_TOSTRING(field_type), &destination->field_name, &source->field_name) |

/*Codes_SRS_UMOCKTYPES_STRUCT_42_002: [ umocktypes_stringify_<type> shall generate a string containing all stringified fields in type and return it. ]*/
/*Codes_SRS_UMOCKTYPES_STRUCT_42_004: [ If there are any errors then umocktypes_stringify_<type> shall fail and return NULL. ]*/
#define UMOCK_DEFINE_TYPE_STRUCT_STRINGIFY(type, ...) \
    static char* MU_C2(umocktypes_stringify_, type)(const type* value) \
    { \
        char* result; \
        if (value == NULL) \
        { \
            UMOCK_LOG("stringify failed: NULL " MU_TOSTRING(type)); \
            result = NULL; \
        } \
        else \
        { \
            bool is_error = false; \
            MU_FOR_EACH_2_COUNTED(UMOCK_STRUCT_STRINGIFY_STRUCT_FIELD, __VA_ARGS__) \
            if (is_error) \
            { \
                UMOCK_LOG("Failed to stringify some fields for " MU_TOSTRING(type)); \
                result = NULL; \
            } \
            else \
            { \
                int size_needed = snprintf(NULL, 0, "{" MU_TOSTRING(type) ":" \
                    MU_FOR_EACH_2_COUNTED(UMOCK_STRUCT_STRINGIFY_STRUCT_FIELD_FORMAT_STRING, __VA_ARGS__) \
                    "}", \
                    MU_FOR_EACH_2_COUNTED(UMOCK_STRUCT_STRINGIFY_STRUCT_FIELD_NAME, __VA_ARGS__) \
                    ); \
                if (size_needed < 0) \
                { \
                    UMOCK_LOG("snprintf failed to get size for " MU_TOSTRING(type)); \
                    result = NULL; \
                } \
                else \
                { \
                    result = (char*)umockalloc_malloc((size_needed + 1) * sizeof(char)); \
                    if (result == NULL) \
                    { \
                        UMOCK_LOG("umockalloc_malloc(%zu) failed", (size_needed + 1) * sizeof(char)); \
                    } \
                    else \
                    { \
                        if (snprintf(result, size_needed + 1, "{" MU_TOSTRING(type) ":" \
                            MU_FOR_EACH_2_COUNTED(UMOCK_STRUCT_STRINGIFY_STRUCT_FIELD_FORMAT_STRING, __VA_ARGS__) \
                            "}", \
                            MU_FOR_EACH_2_COUNTED(UMOCK_STRUCT_STRINGIFY_STRUCT_FIELD_NAME, __VA_ARGS__) \
                            ) != size_needed) \
                        { \
                            UMOCK_LOG("snprintf failed for " MU_TOSTRING(type)); \
                            umockalloc_free(result); \
                            result = NULL; \
                        } \
                    } \
                } \
            } \
            MU_FOR_EACH_2_COUNTED(UMOCK_STRUCT_STRINGIFY_STRUCT_FIELD_CLEANUP, __VA_ARGS__) \
        } \
        return result; \
    }

/*Codes_SRS_UMOCKTYPES_STRUCT_42_005: [ If left is NULL then umocktypes_are_equal_<type> shall fail and return -1. ]*/
/*Codes_SRS_UMOCKTYPES_STRUCT_42_006: [ If right is NULL then umocktypes_are_equal_<type> shall fail and return -1. ]*/
/*Codes_SRS_UMOCKTYPES_STRUCT_42_008: [ If any call to umocktypes_are_equal does not return 1 then umocktypes_are_equal_<type> shall return 0. ]*/
/*Codes_SRS_UMOCKTYPES_STRUCT_42_009: [ Otherwise, umocktypes_are_equal_<type> shall return 1. ]*/
#define UMOCK_DEFINE_TYPE_STRUCT_ARE_EQUAL(type, ...) \
    static int MU_C2(umocktypes_are_equal_, type)(const type* left, const type* right) \
    { \
        int result; \
        if ((left == NULL) || (right == NULL)) \
        { \
            UMOCK_LOG("Invalid args: const " MU_TOSTRING(type) "* left = %p, const " MU_TOSTRING(type) "* right = %p", left, right); \
            result = -1; \
        } \
        else \
        { \
            result = \
                MU_FOR_EACH_2_COUNTED(UMOCK_STRUCT_ARE_EQUAL_STRUCT_FIELD, __VA_ARGS__) \
                1; \
        } \
        return result; \
    }

/*Codes_SRS_UMOCKTYPES_STRUCT_42_010: [ If destination is NULL then umocktypes_copy_<type> shall fail and return a non-zero value. ]*/
/*Codes_SRS_UMOCKTYPES_STRUCT_42_011: [ If source is NULL then umocktypes_copy_<type> shall fail and return a non-zero value. ]*/
/*Codes_SRS_UMOCKTYPES_STRUCT_42_013: [ If any call to umocktypes_copy does not return 0 then umocktypes_copy_<type> shall return a non-zero value. ]*/
/*Codes_SRS_UMOCKTYPES_STRUCT_42_014: [ Otherwise umocktypes_copy_<type> shall return 0. ]*/
#define UMOCK_DEFINE_TYPE_STRUCT_COPY(type, ...) \
    static int MU_C2(umocktypes_copy_, type)(type* destination, const type* source) \
    { \
        int result; \
        if ((destination == NULL) || (source == NULL)) \
        { \
            UMOCK_LOG("Invalid args: const " MU_TOSTRING(type) "* destination = %p, const " MU_TOSTRING(type) "* source = %p", destination, source); \
            result = MU_FAILURE; \
        } \
        else \
        { \
            result = \
                MU_FOR_EACH_2_COUNTED(UMOCK_STRUCT_COPY_STRUCT_FIELD, __VA_ARGS__) \
                0; \
        } \
        return result; \
    }


/*Codes_SRS_UMOCKTYPES_STRUCT_42_015: [ umocktypes_free_<type> shall call umocktypes_free for each field in type. ]*/
#define UMOCK_STRUCT_FREE_STRUCT_FIELD(count, field_type, field_name) \
    umocktypes_free(MU_TOSTRING(field_type), &value->field_name);

#define UMOCK_DEFINE_TYPE_STRUCT_FREE(type, ...) \
    static void MU_C2(umocktypes_free_, type)(type* value) \
    { \
        if (value != NULL) \
        { \
            MU_FOR_EACH_2_COUNTED(UMOCK_STRUCT_FREE_STRUCT_FIELD, __VA_ARGS__) \
        } \
    }

#define UMOCK_DEFINE_TYPE_STRUCT(type, ...) \
    UMOCK_DEFINE_TYPE_STRUCT_STRINGIFY(type, __VA_ARGS__) \
    UMOCK_DEFINE_TYPE_STRUCT_ARE_EQUAL(type, __VA_ARGS__) \
    UMOCK_DEFINE_TYPE_STRUCT_COPY(type, __VA_ARGS__) \
    UMOCK_DEFINE_TYPE_STRUCT_FREE(type, __VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif /* UMOCKTYPES_STRUCT_H */
