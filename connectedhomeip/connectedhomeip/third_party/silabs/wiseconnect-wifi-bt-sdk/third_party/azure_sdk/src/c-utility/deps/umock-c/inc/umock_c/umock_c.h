// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef UMOCK_C_H
#define UMOCK_C_H

#ifdef __cplusplus
#include <cstdlib>
#else
#include <stdlib.h>
#endif

#include "azure_macro_utils/macro_utils.h"
#include "umock_c/umockcallrecorder.h"

/* Define UMOCK_STATIC to static to make mocks private to compilation unit */
#ifndef UMOCK_STATIC
#define UMOCK_STATIC
#endif

#define UMOCK_C_ERROR_CODE_VALUES \
        UMOCK_C_ARG_INDEX_OUT_OF_RANGE, \
        UMOCK_C_MALLOC_ERROR, \
        UMOCK_C_INVALID_ARGUMENT_BUFFER, \
        UMOCK_C_COMPARE_CALL_ERROR, \
        UMOCK_C_RESET_CALLS_ERROR, \
        UMOCK_C_CAPTURE_RETURN_ALREADY_USED, \
        UMOCK_C_NULL_ARGUMENT, \
        UMOCK_C_INVALID_PAIRED_CALLS, \
        UMOCK_C_REGISTER_TYPE_FAILED, \
        UMOCK_C_ERROR

MU_DEFINE_ENUM(UMOCK_C_ERROR_CODE, UMOCK_C_ERROR_CODE_VALUES)

/* This is the type for the error callback by which umock_c shall indicate errors to the user */
typedef void(*ON_UMOCK_C_ERROR)(UMOCK_C_ERROR_CODE error_code);

#define IGNORED_PTR_ARG (NULL)
#define IGNORED_NUM_ARG (0)

#define REGISTER_GLOBAL_MOCK_HOOK(mock_function, mock_hook_function) \
    MU_C2(set_global_mock_hook_,mock_function)(mock_hook_function);

#define REGISTER_GLOBAL_MOCK_RETURN(mock_function, return_value) \
    MU_C2(set_global_mock_return_,mock_function)(return_value);

#define REGISTER_GLOBAL_MOCK_FAIL_RETURN(mock_function, fail_return_value) \
    MU_C2(set_global_mock_fail_return_,mock_function)(fail_return_value);

#define REGISTER_GLOBAL_MOCK_RETURNS(mock_function, return_value, fail_return_value) \
    MU_C2(set_global_mock_returns_,mock_function)(return_value, fail_return_value);

/* Codes_SRS_UMOCK_C_LIB_01_013: [STRICT_EXPECTED_CALL shall record that a certain call is expected.] */
#define STRICT_EXPECTED_CALL(call) \
    MU_C2(get_auto_ignore_args_function_, call)(MU_C2(umock_c_strict_expected_,call), #call)

#define EXPECTED_CALL(call) \
    MU_C2(umock_c_expected_,call)

#define DECLARE_UMOCK_POINTER_TYPE_FOR_TYPE(value_type, alias) \
    char* MU_C3(stringify_func_,alias,ptr)(const value_type** value) \
    { \
        char temp_buffer[32]; \
        char* result; \
        size_t length = sprintf(temp_buffer, "%p", (void*)*value); \
        if (length < 0) \
        { \
            result = NULL; \
        } \
        else \
        { \
            result = (char*)malloc(length + 1); \
            if (result != NULL) \
            { \
                (void)memcpy(result, temp_buffer, length + 1); \
            } \
        } \
        return result; \
    } \
    int MU_C3(are_equal_func_,alias,ptr)(const value_type** left, const value_type** right) \
    { \
        return *left == *right; \
    } \
    int MU_C3(copy_func_,alias,ptr)(value_type** destination, const value_type** source) \
    { \
        *destination = (value_type*)*source; \
        return 0; \
    } \
    void MU_C3(free_func_,alias,ptr)(value_type** value) \
    { \
        (void)value; \
    } \

#ifdef __cplusplus
extern "C" {
#endif

void umock_c_indicate_error(UMOCK_C_ERROR_CODE error_code);
int umock_c_init(ON_UMOCK_C_ERROR on_umock_c_error);
void umock_c_deinit(void);
void umock_c_reset_all_calls(void);
const char* umock_c_get_actual_calls(void);
const char* umock_c_get_expected_calls(void);
UMOCKCALLRECORDER_HANDLE umock_c_get_call_recorder(void);
int umock_c_set_call_recorder(UMOCKCALLRECORDER_HANDLE umockc_call_recorder);

#ifdef __cplusplus
}
#endif

/* Codes_SRS_UMOCK_C_LIB_01_065: [REGISTER_UMOCK_VALUE_TYPE shall register the type identified by value_type to be usable by umock_c for argument and return types and instruct umock_c which functions to use for getting the stringify, are_equal, copy and free.]*/
/* Codes_SRS_UMOCK_C_LIB_01_197: [ If REGISTER_UMOCK_VALUE_TYPE fails, the on_error callback shall be called with UMOCK_C_REGISTER_TYPE_FAILED. ]*/
#define REGISTER_UMOCK_VALUE_TYPE_ALL(value_type, stringify_func, are_equal_func, copy_func, free_func) \
{ \
    if (umocktypes_register_type(MU_TOSTRING(value_type), (UMOCKTYPE_STRINGIFY_FUNC)stringify_func, (UMOCKTYPE_ARE_EQUAL_FUNC)are_equal_func, (UMOCKTYPE_COPY_FUNC)copy_func, (UMOCKTYPE_FREE_FUNC)free_func) != 0) \
    { \
        umock_c_indicate_error(UMOCK_C_REGISTER_TYPE_FAILED); \
    } \
}

/* Codes_SRS_UMOCK_C_LIB_01_066: [If only the value_type is specified in the macro invocation then the stringify, are_equal, copy and free function names shall be automatically derived from the type as: umockvalue_stringify_value_type, umockvalue_are_equal_value_type, umockvalue_copy_value_type, umockvalue_free_value_type.]*/
#define REGISTER_UMOCK_VALUE_TYPE_ONLY_TYPE(value_type) \
    REGISTER_UMOCK_VALUE_TYPE_ALL (value_type, MU_C2(umock_stringify_,value_type), MU_C2(umock_are_equal_,value_type), MU_C2(umock_copy_,value_type), MU_C2(umock_free_,value_type))

#if _MSC_VER
#define REGISTER_UMOCK_VALUE_TYPE(...) \
    MU_IF(MU_DIV2(MU_COUNT_ARG(__VA_ARGS__)), REGISTER_UMOCK_VALUE_TYPE_ALL, REGISTER_UMOCK_VALUE_TYPE_ONLY_TYPE) MU_LPAREN __VA_ARGS__)
#else
#define REGISTER_UMOCK_VALUE_TYPE(...) \
    MU_IF(MU_DIV2(MU_COUNT_ARG(__VA_ARGS__)), REGISTER_UMOCK_VALUE_TYPE_ALL, REGISTER_UMOCK_VALUE_TYPE_ONLY_TYPE) (__VA_ARGS__)
#endif

/* Codes_SRS_UMOCK_C_LIB_01_149: [ REGISTER_UMOCK_ALIAS_TYPE registers a new alias type for another type. ]*/
/* Codes_SRS_UMOCK_C_LIB_02_001: [ If the types do not have the same size the on_error callback shall be called with UMOCK_C_REGISTER_TYPE_FAILED. ]*/
/* Codes_SRS_UMOCK_C_LIB_01_198: [ If REGISTER_UMOCK_ALIAS_TYPE fails, the on_error callback shall be called with UMOCK_C_REGISTER_TYPE_FAILED. ]*/
#define REGISTER_UMOCK_ALIAS_TYPE(value_type, is_value_type) \
    if ((sizeof(value_type)!=sizeof(is_value_type)) || umocktypes_register_alias_type(MU_TOSTRING(value_type), MU_TOSTRING(is_value_type)) != 0) \
    { \
        umock_c_indicate_error(UMOCK_C_REGISTER_TYPE_FAILED); \
    } \

#define UMOCK_TYPE(value_type) \
    (const char*)(const void*)(const value_type*)(const void*)MU_TOSTRING(value_type)

#include "umock_c/umock_c_internal.h"
#include "umock_c/umock_c_prod.h"

#endif /* UMOCK_C_H */
