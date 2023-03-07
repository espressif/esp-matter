// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef UMOCK_C_INTERNAL_H
#define UMOCK_C_INTERNAL_H

#ifdef __cplusplus
#include <cstdlib>
#include <cstdio>
#else
#include <stdlib.h>
#include <stdio.h>
#endif

#include "azure_macro_utils/macro_utils.h"
#include "umock_c/umocktypes.h"
#include "umock_c/umockcall.h"
#include "umock_c/umockcallrecorder.h"
#include "umock_c/umock_c.h"
#include "umock_c/umock_log.h"
#include "umock_c/umockalloc.h"
#include "umock_c/umockcallpairs.h"
#include "umock_c/umockstring.h"
#include "umock_c/umockautoignoreargs.h"

#ifdef _MSC_VER
#define UMOCK_C_WEAK
#elif __GNUC__
// GCC needs weak for the reals
#define UMOCK_C_WEAK __attribute__ ((weak))
#else
// at the mercy of that compiler
#define UMOCK_C_WEAK
#endif

#ifdef __cplusplus
extern "C" {
#endif

void umock_c_indicate_error(UMOCK_C_ERROR_CODE error_code);
UMOCKCALL_HANDLE umock_c_get_last_expected_call(void);
int umock_c_add_expected_call(UMOCKCALL_HANDLE mock_call);
int umock_c_add_actual_call(UMOCKCALL_HANDLE mock_call, UMOCKCALL_HANDLE* matched_call);

typedef struct ARG_BUFFER_TAG
{
    void* bytes;
    size_t length;
} ARG_BUFFER;

typedef int(*TRACK_CREATE_FUNC_TYPE)(PAIRED_HANDLES* paired_handles, const void* handle, const char* handle_type, size_t handle_type_size);
typedef int(*TRACK_DESTROY_FUNC_TYPE)(PAIRED_HANDLES* paired_handles, const void* handle);

#define MOCK_ENABLED 0
#define MOCK_DISABLED 1

#define ARG_IS_IGNORED -1
#define ARG_IS_NOT_IGNORED 0

#define FAIL_RETURN_VALUE_SET -1
#define FAIL_RETURN_VALUE_NOT_SET 0

#define RETURN_VALUE_SET -1
#define RETURN_VALUE_NOT_SET 0

#define COUNT_OF(A) (sizeof(A) / sizeof((A)[0]))

#define GET_USED_ARGUMENT_TYPE(mock_call, arg_name, arg_type) \
    mock_call->MU_C2(override_argument_type_,arg_name) != NULL ? mock_call->MU_C2(override_argument_type_,arg_name) : #arg_type

#define COPY_ARG_TO_MOCK_STRUCT(arg_type, arg_name) umocktypes_copy(#arg_type, (void*)&mock_call_data->arg_name, (void*)&arg_name);
#define DECLARE_MOCK_CALL_STRUCT_STACK(arg_type, arg_name) arg_type arg_name;
#define MARK_ARG_AS_NOT_IGNORED(arg_type, arg_name) mock_call_data->MU_C2(is_ignored_, arg_name) = ARG_IS_NOT_IGNORED;
#define CLEAR_VALIDATE_ARG_VALUE(arg_type, arg_name) mock_call_data->MU_C2(validate_arg_value_pointer_, arg_name) = NULL;
#define CLEAR_OVERRIDE_ARGUMENT_TYPE(arg_type, arg_name) mock_call_data->MU_C2(override_argument_type_, arg_name) = NULL;
#define CLEAR_CAPTURE_ARG_VALUE(arg_type, arg_name) mock_call_data->MU_C2(capture_arg_value_pointer_, arg_name) = NULL;
#define MARK_ARG_AS_IGNORED(arg_type, arg_name) mock_call_data->MU_C2(is_ignored_, arg_name) = ARG_IS_IGNORED;
#define CLEAR_OUT_ARG_BUFFERS(count, arg_type, arg_name) \
    MU_C2(mock_call_data->out_arg_buffer_,arg_name).bytes = NULL; \
    mock_call_data->out_arg_buffers[COUNT_OF(mock_call_data->out_arg_buffers) - MU_DIV2(count)] = &MU_C2(mock_call_data->out_arg_buffer_,arg_name);
#define CLEAR_VALIDATE_ARG_BUFFERS(count, arg_type, arg_name) mock_call_data->validate_arg_buffers[COUNT_OF(mock_call_data->validate_arg_buffers) - MU_DIV2(count)].bytes = NULL;
#define FREE_ARG_VALUE(count, arg_type, arg_name) umocktypes_free(GET_USED_ARGUMENT_TYPE(typed_mock_call_data, arg_name, arg_type), (void*)&typed_mock_call_data->arg_name);
#define FREE_OUT_ARG_BUFFERS(count, arg_type, arg_name) umockalloc_free(typed_mock_call_data->out_arg_buffers[COUNT_OF(typed_mock_call_data->out_arg_buffers) - MU_DIV2(count)]->bytes);
#define FREE_VALIDATE_ARG_BUFFERS(count, arg_type, arg_name) umockalloc_free(typed_mock_call_data->validate_arg_buffers[COUNT_OF(typed_mock_call_data->validate_arg_buffers) - MU_DIV2(count)].bytes);
#define FREE_OVERRIDE_ARGUMENT_TYPE(count, arg_type, arg_name) umockalloc_free(typed_mock_call_data->MU_C2(override_argument_type_, arg_name));

#define COPY_IGNORE_ARG(arg_type, arg_name) \
    result->MU_C2(is_ignored_, arg_name) = typed_mock_call_data->MU_C2(is_ignored_, arg_name);

#define COPY_VALIDATE_ARG_VALUE(arg_type, arg_name) \
    result->MU_C2(validate_arg_value_pointer_, arg_name) = typed_mock_call_data->MU_C2(validate_arg_value_pointer_, arg_name);

#define COPY_OVERRIDE_ARGUMENT_TYPE(arg_type, arg_name) \
    result->MU_C2(override_argument_type_, arg_name) = (typed_mock_call_data->MU_C2(override_argument_type_, arg_name) == NULL) ? NULL : umockstring_clone(typed_mock_call_data->MU_C2(override_argument_type_, arg_name));

#define COPY_CAPTURE_ARG_VALUE(arg_type, arg_name) \
    result->MU_C2(capture_arg_value_pointer_, arg_name) = typed_mock_call_data->MU_C2(capture_arg_value_pointer_, arg_name);

#define COPY_ARG_VALUE(arg_type, arg_name) umocktypes_copy(GET_USED_ARGUMENT_TYPE(typed_mock_call_data, arg_name, arg_type), (void*)&result->arg_name, (void*)&typed_mock_call_data->arg_name);
#define COPY_OUT_ARG_BUFFERS(count, arg_type, arg_name) \
    result->out_arg_buffers[COUNT_OF(result->out_arg_buffers) - MU_DIV2(count)] = &result->MU_C2(out_arg_buffer_,arg_name); \
    result->out_arg_buffers[COUNT_OF(result->out_arg_buffers) - MU_DIV2(count)]->length = typed_mock_call_data->out_arg_buffers[COUNT_OF(typed_mock_call_data->out_arg_buffers) - MU_DIV2(count)]->length; \
    if (typed_mock_call_data->out_arg_buffers[COUNT_OF(typed_mock_call_data->out_arg_buffers) - MU_DIV2(count)]->bytes != NULL) \
    { \
        result->out_arg_buffers[COUNT_OF(result->out_arg_buffers) - MU_DIV2(count)]->bytes = umockalloc_malloc(typed_mock_call_data->out_arg_buffers[COUNT_OF(typed_mock_call_data->out_arg_buffers) - MU_DIV2(count)]->length); \
        (void)memcpy(result->out_arg_buffers[COUNT_OF(result->out_arg_buffers) - MU_DIV2(count)]->bytes, typed_mock_call_data->out_arg_buffers[COUNT_OF(typed_mock_call_data->out_arg_buffers) - MU_DIV2(count)]->bytes, typed_mock_call_data->out_arg_buffers[COUNT_OF(typed_mock_call_data->out_arg_buffers) - MU_DIV2(count)]->length); \
    } \
    else \
    { \
        result->out_arg_buffers[COUNT_OF(result->out_arg_buffers) - MU_DIV2(count)]->bytes = NULL; \
    }

#define COPY_VALIDATE_ARG_BUFFERS(count, arg_type, arg_name) \
    result->validate_arg_buffers[COUNT_OF(result->validate_arg_buffers) - MU_DIV2(count)].length = typed_mock_call_data->validate_arg_buffers[COUNT_OF(typed_mock_call_data->validate_arg_buffers) - MU_DIV2(count)].length; \
    if (typed_mock_call_data->validate_arg_buffers[COUNT_OF(typed_mock_call_data->validate_arg_buffers) - MU_DIV2(count)].bytes != NULL) \
    { \
        result->validate_arg_buffers[COUNT_OF(result->validate_arg_buffers) - MU_DIV2(count)].bytes = umockalloc_malloc(typed_mock_call_data->validate_arg_buffers[COUNT_OF(typed_mock_call_data->validate_arg_buffers) - MU_DIV2(count)].length); \
        (void)memcpy(result->validate_arg_buffers[COUNT_OF(result->validate_arg_buffers) - MU_DIV2(count)].bytes, typed_mock_call_data->validate_arg_buffers[COUNT_OF(typed_mock_call_data->validate_arg_buffers) - MU_DIV2(count)].bytes, typed_mock_call_data->validate_arg_buffers[COUNT_OF(typed_mock_call_data->validate_arg_buffers) - MU_DIV2(count)].length); \
    } \
    else \
    { \
        result->validate_arg_buffers[COUNT_OF(result->validate_arg_buffers) - MU_DIV2(count)].bytes = NULL; \
    }

#define ONLY_FIRST_ARG(arg0type, arg0name, ...) arg0name
#define FILL_ARG_IN_METADATA(arg_type, arg_name) {MU_TOSTRING(arg_type), MU_TOSTRING(arg_name) },
#define ARG_IN_SIGNATURE(count, arg_type, arg_name) arg_type arg_name MU_IFCOMMA(count)
#define ARG_NAME_ONLY_IN_CALL(count, arg_type, arg_name) arg_name MU_IFCOMMA(count)
#define ONLY_FIRST_ARG_NAME_IN_CALL(count, arg_type, arg_name) count
#define ARG_ASSIGN_IN_ARRAY(arg_type, arg_name) arg_name_local
#define DECLARE_VALIDATE_ARG_VALUE(arg_type, arg_name) void* MU_C2(validate_arg_value_pointer_,arg_name);
#define DECLARE_CAPTURE_ARG_VALUE(arg_type, arg_name) void* MU_C2(capture_arg_value_pointer_,arg_name);
#define DECLARE_IGNORE_FLAG_FOR_ARG(arg_type, arg_name) int MU_C2(is_ignored_,arg_name) : 1;
#define DECLARE_OVERRIDE_ARGUMENT_TYPE_FOR_ARG(arg_type, arg_name) char* MU_C2(override_argument_type_,arg_name);
#define DECLARE_OUT_ARG_BUFFER_FOR_ARG(arg_type, arg_name) ARG_BUFFER MU_C2(out_arg_buffer_,arg_name);
#define COPY_IGNORE_ARG_BY_NAME_TO_MODIFIER(name, arg_type, arg_name) MU_C2(mock_call_modifier->IgnoreArgument_,arg_name) = MU_C4(ignore_argument_func_,name,_,arg_name);
#define COPY_VALIDATE_ARG_BY_NAME_TO_MODIFIER(name, arg_type, arg_name) MU_C2(mock_call_modifier->ValidateArgument_,arg_name) = MU_C4(validate_argument_func_,name,_,arg_name);
#define COPY_COPY_OUT_ARGUMENT_BUFFER_BY_NAME_TO_MODIFIER(name, arg_type, arg_name) MU_C2(mock_call_modifier->CopyOutArgumentBuffer_,arg_name) = MU_C4(copy_out_argument_buffer_func_,name,_,arg_name);
#define COPY_VALIDATE_ARGUMENT_VALUE_BY_NAME_TO_MODIFIER(name, arg_type, arg_name) MU_C2(mock_call_modifier->ValidateArgumentValue_,arg_name) = MU_C4(validate_argument_value_func_,name,_,arg_name);
#define COPY_VALIDATE_ARGUMENT_VALUE_AS_TYPE_BY_NAME_TO_MODIFIER(name, arg_type, arg_name) MU_C3(mock_call_modifier->ValidateArgumentValue_,arg_name,_AsType) = MU_C4(validate_argument_value_as_type_func_,name,_,arg_name);
#define COPY_CAPTURE_ARGUMENT_VALUE_BY_NAME_TO_MODIFIER(name, arg_type, arg_name) MU_C2(mock_call_modifier->CaptureArgumentValue_,arg_name) = MU_C4(capture_argument_value_func_,name,_,arg_name);
#define COPY_OUT_ARG_VALUE_FROM_MATCHED_CALL(count, arg_type, arg_name) \
    if (matched_call_data->out_arg_buffers[COUNT_OF(matched_call_data->out_arg_buffers) - MU_DIV2(count)]->bytes != NULL) \
    { \
        (void)memcpy(*((void**)(&arg_name)), matched_call_data->out_arg_buffers[COUNT_OF(matched_call_data->out_arg_buffers) - MU_DIV2(count)]->bytes, matched_call_data->out_arg_buffers[COUNT_OF(matched_call_data->out_arg_buffers) - MU_DIV2(count)]->length); \
    } \

#define STRINGIFY_ARGS_DECLARE_RESULT_VAR(count, arg_type, arg_name) \
    char* MU_C2(arg_name,_stringified) \
    = (MU_C2(typed_mock_call_data->validate_arg_value_pointer_, arg_name) != NULL) ? \
      umocktypes_stringify(MU_TOSTRING(arg_type), (void*)MU_C2(typed_mock_call_data->validate_arg_value_pointer_, arg_name)) : \
      ((typed_mock_call_data->validate_arg_buffers[COUNT_OF(typed_mock_call_data->out_arg_buffers) - MU_DIV2(count)].bytes != NULL) ? \
        umockc_stringify_buffer(typed_mock_call_data->validate_arg_buffers[COUNT_OF(typed_mock_call_data->validate_arg_buffers) - MU_DIV2(count)].bytes, typed_mock_call_data->validate_arg_buffers[COUNT_OF(typed_mock_call_data->validate_arg_buffers) - MU_DIV2(count)].length) : \
        umocktypes_stringify(MU_TOSTRING(arg_type), (void*)&typed_mock_call_data->arg_name));

#define STRINGIFY_ARGS_CHECK_ARG_STRINGIFY_SUCCESS(arg_type, arg_name) if (MU_C2(arg_name,_stringified) == NULL) is_error = 1;
#define STRINGIFY_ARGS_DECLARE_ARG_STRING_LENGTH(arg_type, arg_name) size_t MU_C2(arg_name,_stringified_length) = strlen(MU_C2(arg_name,_stringified));
#define STRINGIFY_ARGS_COUNT_LENGTH(arg_type, arg_name) args_string_length += MU_C2(arg_name,_stringified_length);
#define STRINGIFY_ARGS_FREE_STRINGIFIED_ARG(arg_type, arg_name) umockalloc_free(MU_C2(arg_name,_stringified));
#define STRINGIFY_ARGS_COPY_ARG_STRING(arg_type, arg_name) \
    if (arg_index > 0) \
    { \
        result[current_pos] = ','; \
        current_pos++; \
    } \
    (void)memcpy(result + current_pos, MU_C2(arg_name,_stringified), MU_C2(arg_name,_stringified_length) + 1); \
    current_pos += MU_C2(arg_name, _stringified_length); \
    arg_index++;

/* Codes_SRS_UMOCK_C_LIB_01_096: [If the content of the code under test buffer and the buffer supplied to ValidateArgumentBuffer does not match then this should be treated as a mismatch in argument comparison for that argument.]*/
#define ARE_EQUAL_FOR_ARG(count, arg_type, arg_name) \
    if (result && \
        (((typed_left->validate_arg_buffers[COUNT_OF(typed_left->validate_arg_buffers) - MU_DIV2(count)].bytes != NULL) && (memcmp(*((void**)&typed_right->arg_name), typed_left->validate_arg_buffers[COUNT_OF(typed_left->validate_arg_buffers) - MU_DIV2(count)].bytes, typed_left->validate_arg_buffers[COUNT_OF(typed_left->validate_arg_buffers) - MU_DIV2(count)].length) != 0)) \
        || ((typed_right->validate_arg_buffers[COUNT_OF(typed_right->validate_arg_buffers) - MU_DIV2(count)].bytes != NULL) && (memcmp(*((void**)&typed_left->arg_name), typed_right->validate_arg_buffers[COUNT_OF(typed_right->validate_arg_buffers) - MU_DIV2(count)].bytes, typed_right->validate_arg_buffers[COUNT_OF(typed_right->validate_arg_buffers) - MU_DIV2(count)].length) != 0))) \
        ) \
    { \
        result = 0; \
    } \
    if ((result == 1) && (MU_C2(typed_left->is_ignored_, arg_name) == ARG_IS_NOT_IGNORED) \
        && (MU_C2(typed_right->is_ignored_, arg_name) == ARG_IS_NOT_IGNORED)) \
    { \
        void* left_value; \
        void* right_value; \
        if (MU_C2(typed_left->validate_arg_value_pointer_, arg_name) != NULL) \
        { \
            left_value = (void*)MU_C2(typed_left->validate_arg_value_pointer_, arg_name); \
        } \
        else \
        { \
            left_value = (void*)&typed_left->arg_name; \
        } \
        if (MU_C2(typed_right->validate_arg_value_pointer_, arg_name) != NULL) \
        { \
            right_value = (void*)MU_C2(typed_right->validate_arg_value_pointer_, arg_name); \
        } \
        else \
        { \
            right_value = (void*)&typed_right->arg_name; \
        } \
        result = umocktypes_are_equal(GET_USED_ARGUMENT_TYPE(typed_left, arg_name, arg_type), left_value, right_value); \
    }

#define DECLARE_MOCK_CALL_MODIFIER(name, ...) \
        MU_C2(mock_call_modifier_,name) mock_call_modifier; \
        MU_C2(fill_mock_call_modifier_,name)(&mock_call_modifier);

#define DECLARE_IGNORE_ARGUMENT_FUNCTION_PROTOTYPE(name, arg_type, arg_name) \
    static MU_C2(mock_call_modifier_,name) MU_C4(ignore_argument_func_,name,_,arg_name)(void);

#define DECLARE_VALIDATE_ARGUMENT_FUNCTION_PROTOTYPE(name, arg_type, arg_name) \
    static MU_C2(mock_call_modifier_,name) MU_C4(validate_argument_func_,name,_,arg_name)(void);

#define DECLARE_COPY_OUT_ARGUMENT_BUFFER_FUNCTION_PROTOTYPE(name, arg_type, arg_name) \
    static MU_C2(mock_call_modifier_,name) MU_C4(copy_out_argument_buffer_func_,name,_,arg_name)(const void* bytes, size_t length);

#define DECLARE_VALIDATE_ARGUMENT_VALUE_FUNCTION_PROTOTYPE(name, arg_type, arg_name) \
    static MU_C2(mock_call_modifier_,name) MU_C4(validate_argument_value_func_,name,_,arg_name)(arg_type* arg_value);

#define DECLARE_VALIDATE_ARGUMENT_VALUE_AS_TYPE_FUNCTION_PROTOTYPE(name, arg_type, arg_name) \
    static MU_C2(mock_call_modifier_,name) MU_C4(validate_argument_value_as_type_func_,name,_,arg_name)(const char* type_name);

#define DECLARE_CAPTURE_ARGUMENT_VALUE_FUNCTION_PROTOTYPE(name, arg_type, arg_name) \
    static MU_C2(mock_call_modifier_,name) MU_C4(capture_argument_value_func_,name,_,arg_name)(arg_type* arg_value);

#define IGNORE_ARGUMENT_FUNCTION_IN_MODIFIERS(name, arg_type, arg_name) \
    MU_C4(ignore_argument_func_type_,name,_,arg_name) MU_C2(IgnoreArgument_,arg_name);

#define VALIDATE_ARGUMENT_FUNCTION_IN_MODIFIERS(name, arg_type, arg_name) \
    MU_C4(validate_argument_func_type_,name,_,arg_name) MU_C2(ValidateArgument_,arg_name);

#define COPY_OUT_ARGUMENT_BUFFER_FUNCTION_IN_MODIFIERS(name, arg_type, arg_name) \
    MU_C4(copy_out_argument_buffer_func_type_,name,_,arg_name) MU_C2(CopyOutArgumentBuffer_,arg_name);

#define VALIDATE_ARGUMENT_VALUE_FUNCTION_IN_MODIFIERS(name, arg_type, arg_name) \
    MU_C4(validate_argument_value_func_type_,name,_,arg_name) MU_C2(ValidateArgumentValue_,arg_name);

#define CAPTURE_ARGUMENT_VALUE_FUNCTION_IN_MODIFIERS(name, arg_type, arg_name) \
    MU_C4(capture_argument_value_func_type_,name,_,arg_name) MU_C2(CaptureArgumentValue_,arg_name);

#define VALIDATE_ARGUMENT_VALUE_AS_TYPE_FUNCTION_IN_MODIFIERS(name, arg_type, arg_name) \
    MU_C4(validate_argument_value_as_type_func_type_,name,_,arg_name) MU_C3(ValidateArgumentValue_,arg_name,_AsType);

#define ARG_RELATED_FUNCTIONS_IN_MODIFIERS(name, arg_type, arg_name) \
    IGNORE_ARGUMENT_FUNCTION_IN_MODIFIERS(name, arg_type, arg_name) \
    VALIDATE_ARGUMENT_FUNCTION_IN_MODIFIERS(name, arg_type, arg_name) \
    COPY_OUT_ARGUMENT_BUFFER_FUNCTION_IN_MODIFIERS(name, arg_type, arg_name) \
    VALIDATE_ARGUMENT_VALUE_FUNCTION_IN_MODIFIERS(name, arg_type, arg_name) \
    VALIDATE_ARGUMENT_VALUE_AS_TYPE_FUNCTION_IN_MODIFIERS(name, arg_type, arg_name) \
    CAPTURE_ARGUMENT_VALUE_FUNCTION_IN_MODIFIERS(name, arg_type, arg_name) \

#define DECLARE_IGNORE_ARGUMENT_FUNCTION_TYPE(name, arg_type, arg_name) \
    typedef struct MU_C2(_mock_call_modifier_,name) (*MU_C4(ignore_argument_func_type_,name,_,arg_name))(void);

#define DECLARE_VALIDATE_ARGUMENT_FUNCTION_TYPE(name, arg_type, arg_name) \
    typedef struct MU_C2(_mock_call_modifier_,name) (*MU_C4(validate_argument_func_type_,name,_,arg_name))(void);

#define DECLARE_COPY_OUT_ARGUMENT_BUFFER_FUNCTION_TYPE(name, arg_type, arg_name) \
    typedef struct MU_C2(_mock_call_modifier_,name) (*MU_C4(copy_out_argument_buffer_func_type_,name,_,arg_name))(const void* bytes, size_t length);

#define DECLARE_VALIDATE_ARGUMENT_VALUE_FUNCTION_TYPE(name, arg_type, arg_name) \
    typedef struct MU_C2(_mock_call_modifier_,name) (*MU_C4(validate_argument_value_func_type_,name,_,arg_name))(arg_type* arg_value);

#define DECLARE_VALIDATE_ARGUMENT_AS_TYPE_VALUE_FUNCTION_TYPE(name, arg_type, arg_name) \
    typedef struct MU_C2(_mock_call_modifier_,name) (*MU_C4(validate_argument_value_as_type_func_type_,name,_,arg_name))(const char* type_name);

#define DECLARE_CAPTURE_ARGUMENT_VALUE_FUNCTION_TYPE(name, arg_type, arg_name) \
    typedef struct MU_C2(_mock_call_modifier_,name) (*MU_C4(capture_argument_value_func_type_,name,_,arg_name))(arg_type* arg_value);

#define DECLARE_ARG_RELATED_FUNCTIONS(name, arg_type, arg_name) \
    DECLARE_IGNORE_ARGUMENT_FUNCTION_TYPE(name, arg_type, arg_name) \
    DECLARE_VALIDATE_ARGUMENT_FUNCTION_TYPE(name, arg_type, arg_name) \
    DECLARE_COPY_OUT_ARGUMENT_BUFFER_FUNCTION_TYPE(name, arg_type, arg_name) \
    DECLARE_VALIDATE_ARGUMENT_VALUE_FUNCTION_TYPE(name, arg_type, arg_name) \
    DECLARE_VALIDATE_ARGUMENT_AS_TYPE_VALUE_FUNCTION_TYPE(name, arg_type, arg_name) \
    DECLARE_CAPTURE_ARGUMENT_VALUE_FUNCTION_TYPE(name, arg_type, arg_name)

#define IGNORE_ARGUMENT_FUNCTION_IN_ARRAY(name, arg_type, arg_name) \
    &MU_C4(ignore_argument_func_,name,_,arg_name),

#define VALIDATE_ARGUMENT_FUNCTION_IN_ARRAY(name, arg_type, arg_name) \
    &MU_C4(validate_argument_func_,name,_,arg_name),

/* These 2 macros are used to check if a type is "void" or not */
#define TEST_void 0
#define IS_NOT_VOID(x) \
    MU_IF(MU_C2(TEST_,x), 1, 0)

/* Codes_SRS_UMOCK_C_LIB_01_076: [The IgnoreAllArguments call modifier shall record that for that specific call all arguments will be ignored for that specific call.] */
#define IMPLEMENT_IGNORE_ALL_ARGUMENTS_FUNCTION(return_type, name, ...) \
    static MU_C2(mock_call_modifier_,name) MU_C2(ignore_all_arguments_func_,name)(void) \
    { \
        MU_C2(mock_call_, name)* mock_call_data = (MU_C2(mock_call_, name)*)umockcall_get_call_data(umock_c_get_last_expected_call()); \
        DECLARE_MOCK_CALL_MODIFIER(name) \
        MU_IF(MU_COUNT_ARG(__VA_ARGS__), MU_FOR_EACH_2(MARK_ARG_AS_IGNORED, __VA_ARGS__),) \
        return mock_call_modifier; \
    } \

/* Codes_SRS_UMOCK_C_LIB_01_077: [The ValidateAllArguments call modifier shall record that for that specific call all arguments will be validated.] */
#define IMPLEMENT_VALIDATE_ALL_ARGUMENTS_FUNCTION(return_type, name, ...) \
    static MU_C2(mock_call_modifier_,name) MU_C2(validate_all_arguments_func_,name)(void) \
    { \
        MU_C2(mock_call_, name)* mock_call_data = (MU_C2(mock_call_, name)*)umockcall_get_call_data(umock_c_get_last_expected_call()); \
        DECLARE_MOCK_CALL_MODIFIER(name) \
        if (mock_call_data == NULL) \
        { \
            UMOCK_LOG("ValidateAllArguments called without having an expected call."); \
            umock_c_indicate_error(UMOCK_C_ERROR); \
        } \
        else \
        { \
            MU_IF(MU_COUNT_ARG(__VA_ARGS__), MU_FOR_EACH_2(MARK_ARG_AS_NOT_IGNORED, __VA_ARGS__), ) \
        } \
        return mock_call_modifier; \
    } \

/* Codes_SRS_UMOCK_C_LIB_01_078: [The IgnoreArgument_{arg_name} call modifier shall record that the argument identified by arg_name will be ignored for that specific call.] */
#define IMPLEMENT_IGNORE_ARGUMENT_BY_NAME_FUNCTION(name, arg_type, arg_name) \
    static MU_C2(mock_call_modifier_,name) MU_C4(ignore_argument_func_,name,_,arg_name)(void) \
    { \
        MU_C2(mock_call_, name)* mock_call_data = (MU_C2(mock_call_, name)*)umockcall_get_call_data(umock_c_get_last_expected_call()); \
        DECLARE_MOCK_CALL_MODIFIER(name) \
        if (mock_call_data == NULL) \
        { \
            UMOCK_LOG("IgnoreArgument_%s called without having an expected call.", MU_TOSTRING(arg_name)); \
            umock_c_indicate_error(UMOCK_C_ERROR); \
        } \
        else \
        { \
            MU_C2(mock_call_data->is_ignored_,arg_name) = ARG_IS_IGNORED; \
        } \
        return mock_call_modifier; \
    } \

/* Codes_SRS_UMOCK_C_LIB_01_079: [The ValidateArgument_{arg_name} call modifier shall record that the argument identified by arg_name will be validated for that specific call.]*/
#define IMPLEMENT_VALIDATE_ARGUMENT_BY_NAME_FUNCTION(name, arg_type, arg_name) \
    static MU_C2(mock_call_modifier_,name) MU_C4(validate_argument_func_,name,_,arg_name)(void) \
    { \
        MU_C2(mock_call_, name)* mock_call_data = (MU_C2(mock_call_, name)*)umockcall_get_call_data(umock_c_get_last_expected_call()); \
        DECLARE_MOCK_CALL_MODIFIER(name) \
        if (mock_call_data == NULL) \
        { \
            UMOCK_LOG("ValidateArgument_%s called without having an expected call.", MU_TOSTRING(arg_name)); \
            umock_c_indicate_error(UMOCK_C_ERROR); \
        } \
        else \
        { \
            MU_C2(mock_call_data->is_ignored_,arg_name) = ARG_IS_NOT_IGNORED; \
        } \
        return mock_call_modifier; \
    } \

/* Codes_SRS_UMOCK_C_LIB_01_080: [The IgnoreArgument call modifier shall record that the indexth argument will be ignored for that specific call.]*/
/* Codes_SRS_UMOCK_C_LIB_01_081: [If the index is out of range umock_c shall raise an error with the code UMOCK_C_ARG_INDEX_OUT_OF_RANGE.] */
#define IMPLEMENT_IGNORE_ARGUMENT_FUNCTION(return_type, name, ...) \
    static MU_C2(mock_call_modifier_,name) MU_C2(ignore_argument_func_,name)(size_t arg_index) \
    { \
        MU_C2(mock_call_, name)* mock_call_data = (MU_C2(mock_call_, name)*)umockcall_get_call_data(umock_c_get_last_expected_call()); \
        DECLARE_MOCK_CALL_MODIFIER(name) \
        if (mock_call_data == NULL) \
        { \
            UMOCK_LOG("IgnoreArgument called without having an expected call."); \
            umock_c_indicate_error(UMOCK_C_ERROR); \
        } \
        else \
        { \
            MU_IF(MU_COUNT_ARG(__VA_ARGS__), \
                if ((arg_index < 1) || (arg_index > (sizeof(MU_C2(ignore_one_argument_array_,name)) / sizeof(MU_C2(ignore_one_argument_array_,name)[0])))) \
                { \
                    UMOCK_LOG("Bad argument index in call to IgnoreArgument %zu.", arg_index); \
                    umock_c_indicate_error(UMOCK_C_ARG_INDEX_OUT_OF_RANGE); \
                } \
                else \
                { \
                    MU_C2(ignore_one_argument_array_,name)[arg_index - 1](); \
                }, \
            ) \
        } \
        return mock_call_modifier; \
    } \

/* Codes_SRS_UMOCK_C_LIB_01_082: [The ValidateArgument call modifier shall record that the indexth argument will be validated for that specific call.]*/
/* Codes_SRS_UMOCK_C_LIB_01_083: [If the index is out of range umock_c shall raise an error with the code UMOCK_C_ARG_INDEX_OUT_OF_RANGE.]*/
#define IMPLEMENT_VALIDATE_ARGUMENT_FUNCTION(return_type, name, ...) \
    static MU_C2(mock_call_modifier_,name) MU_C2(validate_argument_func_,name)(size_t arg_index) \
    { \
        MU_C2(mock_call_, name)* mock_call_data = (MU_C2(mock_call_, name)*)umockcall_get_call_data(umock_c_get_last_expected_call()); \
        DECLARE_MOCK_CALL_MODIFIER(name) \
        if (mock_call_data == NULL) \
        { \
            UMOCK_LOG("ValidateArgument called without having an expected call."); \
            umock_c_indicate_error(UMOCK_C_ERROR); \
        } \
        else \
        { \
            MU_IF(MU_COUNT_ARG(__VA_ARGS__), \
                if ((arg_index < 1) || (arg_index > (sizeof(MU_C2(validate_one_argument_array_,name)) / sizeof(MU_C2(validate_one_argument_array_,name)[0])))) \
                { \
                    UMOCK_LOG("Bad argument index in call to ValidateArgument %zu.", arg_index); \
                    umock_c_indicate_error(UMOCK_C_ARG_INDEX_OUT_OF_RANGE); \
                } \
                else \
                { \
                    MU_C2(validate_one_argument_array_,name)[arg_index - 1](); \
                }, \
            ) \
        } \
        return mock_call_modifier; \
    } \

/* Codes_SRS_UMOCK_C_LIB_01_084: [The SetReturn call modifier shall record that when an actual call is matched with the specific expected call, it shall return the result value to the code under test.] */
#define IMPLEMENT_SET_RETURN_FUNCTION(return_type, name, ...) \
    static MU_C2(mock_call_modifier_,name) MU_C2(set_return_func_,name)(return_type return_value) \
    { \
        MU_C2(mock_call_, name)* mock_call_data = (MU_C2(mock_call_, name)*)umockcall_get_call_data(umock_c_get_last_expected_call()); \
        DECLARE_MOCK_CALL_MODIFIER(name) \
        if (mock_call_data == NULL) \
        { \
            UMOCK_LOG("SetReturn called without having an expected call."); \
            umock_c_indicate_error(UMOCK_C_ERROR); \
        } \
        else \
        { \
            mock_call_data->return_value_set = RETURN_VALUE_SET; \
            if (umocktypes_copy(#return_type, (void*)&mock_call_data->return_value, (void*)&return_value) != 0) \
            { \
                UMOCK_LOG("Could not copy return value of type %s.", MU_TOSTRING(return_type)); \
                umock_c_indicate_error(UMOCK_C_ERROR); \
            } \
        } \
        return mock_call_modifier; \
    }

/* Codes_SRS_UMOCK_C_LIB_01_085: [The SetFailReturn call modifier shall record a fail return value.]*/
#define IMPLEMENT_SET_FAIL_RETURN_FUNCTION(return_type, name, ...) \
    static MU_C2(mock_call_modifier_,name) MU_C2(set_fail_return_func_,name)(return_type return_value) \
    { \
        MU_C2(mock_call_, name)* mock_call_data = (MU_C2(mock_call_, name)*)umockcall_get_call_data(umock_c_get_last_expected_call()); \
        DECLARE_MOCK_CALL_MODIFIER(name) \
        if (mock_call_data == NULL) \
        { \
            UMOCK_LOG("SetFailReturn called without having an expected call."); \
            umock_c_indicate_error(UMOCK_C_ERROR); \
        } \
        else \
        { \
            mock_call_data->fail_return_value_set = FAIL_RETURN_VALUE_SET; \
            if (umocktypes_copy(#return_type, (void*)&mock_call_data->fail_return_value, (void*)&return_value) != 0) \
            { \
                UMOCK_LOG("Could not copy fail return value of type %s.", MU_TOSTRING(return_type)); \
                umock_c_indicate_error(UMOCK_C_ERROR); \
            } \
        } \
        return mock_call_modifier; \
    }

/* Codes_SRS_UMOCK_C_LIB_01_116: [ The argument targetted by CopyOutArgument shall also be marked as ignored. ] */
/* Codes_SRS_UMOCK_C_LIB_01_088: [The memory shall be copied.]*/
/* Codes_SRS_UMOCK_C_LIB_01_091: [If the index is out of range umock_c shall raise an error with the code UMOCK_C_ARG_INDEX_OUT_OF_RANGE.]*/
/* Codes_SRS_UMOCK_C_LIB_01_117: [ If any memory allocation error occurs, umock_c shall raise an error with the code UMOCK_C_MALLOC_ERROR. ]*/
/* Codes_SRS_UMOCK_C_LIB_01_118: [ If any other error occurs, umock_c shall raise an error with the code UMOCK_C_ERROR. ]*/
/* Codes_SRS_UMOCK_C_LIB_01_092: [If bytes is NULL or length is 0, umock_c shall raise an error with the code UMOCK_C_INVALID_ARGUMENT_BUFFER.] */
/* Codes_SRS_UMOCK_C_LIB_01_089: [The buffers for previous CopyOutArgumentBuffer calls shall be freed.]*/
/* Codes_SRS_UMOCK_C_LIB_01_133: [ If several calls to CopyOutArgumentBuffer are made, only the last buffer shall be kept. ]*/
#define IMPLEMENT_COPY_OUT_ARGUMENT_BUFFER_FUNCTION(return_type, name, ...) \
    static MU_C2(mock_call_modifier_, name) MU_C2(copy_out_argument_buffer_func_, name)(size_t index, const void* bytes, size_t length) \
    { \
        DECLARE_MOCK_CALL_MODIFIER(name) \
        if ((index < 1) || (index > MU_DIV2(MU_COUNT_ARG(__VA_ARGS__)))) \
        { \
            UMOCK_LOG("Bad argument index in CopyOutArgumentBuffer: %zu.", index); \
            umock_c_indicate_error(UMOCK_C_ARG_INDEX_OUT_OF_RANGE); \
        } \
        else if ((bytes == NULL) || (length == 0)) \
        { \
            UMOCK_LOG("Bad arguments to CopyOutArgumentBuffer: bytes = %p, length = %zu.", bytes, length); \
            umock_c_indicate_error(UMOCK_C_INVALID_ARGUMENT_BUFFER); \
        } \
        else \
        { \
            MU_C2(mock_call_, name)* mock_call_data = (MU_C2(mock_call_, name)*)umockcall_get_call_data(umock_c_get_last_expected_call()); \
            if (mock_call_data == NULL) \
            { \
                UMOCK_LOG("CopyOutArgumentBuffer called without having an expected call."); \
                umock_c_indicate_error(UMOCK_C_ERROR); \
            } \
            else \
            { \
                ARG_BUFFER* arg_buffer = mock_call_data->out_arg_buffers[index - 1]; \
                umockalloc_free(arg_buffer->bytes); \
                arg_buffer->bytes = umockalloc_malloc(length); \
                if (arg_buffer->bytes == NULL) \
                { \
                    UMOCK_LOG("Could not allocate memory for out argument buffers."); \
                    umock_c_indicate_error(UMOCK_C_MALLOC_ERROR); \
                } \
                else \
                { \
                    (void)memcpy(arg_buffer->bytes, bytes, length); \
                    arg_buffer->length = length; \
                    mock_call_modifier.IgnoreArgument(index); \
                } \
            } \
        } \
        return mock_call_modifier; \
    } \

/* Codes_SRS_UMOCK_C_LIB_01_154: [ The CopyOutArgumentBuffer_{arg_name} call modifier shall copy the memory pointed to by bytes and being length bytes so that it is later injected as an out argument when the code under test calls the mock function. ] */
/* Codes_SRS_UMOCK_C_LIB_01_163: [ The buffers for previous CopyOutArgumentBuffer calls shall be freed. ]*/
/* Codes_SRS_UMOCK_C_LIB_01_156: [ If several calls to CopyOutArgumentBuffer are made, only the last buffer shall be kept. ]*/
/* Codes_SRS_UMOCK_C_LIB_01_155: [ The memory shall be copied. ]*/
/* Codes_SRS_UMOCK_C_LIB_01_158: [ If bytes is NULL or length is 0, umock_c shall raise an error with the code UMOCK_C_INVALID_ARGUMENT_BUFFER. ] */
#define IMPLEMENT_COPY_OUT_ARGUMENT_BUFFER_BY_NAME_FUNCTION(name, arg_type, arg_name) \
    static MU_C2(mock_call_modifier_,name) MU_C4(copy_out_argument_buffer_func_,name,_,arg_name)(const void* bytes, size_t length) \
    { \
        DECLARE_MOCK_CALL_MODIFIER(name) \
        if ((bytes == NULL) || (length == 0)) \
        { \
            UMOCK_LOG("Bad arguments to CopyOutArgumentBuffer: bytes = %p, length = %zu.", bytes, length); \
            umock_c_indicate_error(UMOCK_C_INVALID_ARGUMENT_BUFFER); \
        } \
        else \
        { \
            MU_C2(mock_call_, name)* mock_call_data = (MU_C2(mock_call_, name)*)umockcall_get_call_data(umock_c_get_last_expected_call()); \
            if (mock_call_data == NULL) \
            { \
                UMOCK_LOG("CopyOutArgumentBuffer called without having an expected call."); \
                umock_c_indicate_error(UMOCK_C_ERROR); \
            } \
            else \
            { \
                ARG_BUFFER* arg_buffer = &MU_C2(mock_call_data->out_arg_buffer_, arg_name); \
                umockalloc_free(arg_buffer->bytes); \
                arg_buffer->bytes = umockalloc_malloc(length); \
                if (arg_buffer->bytes == NULL) \
                { \
                    UMOCK_LOG("Could not allocate memory for out argument buffers."); \
                    umock_c_indicate_error(UMOCK_C_MALLOC_ERROR); \
                } \
                else \
                { \
                    (void)memcpy(arg_buffer->bytes, bytes, length); \
                    arg_buffer->length = length; \
                    MU_C2(mock_call_modifier.IgnoreArgument_, arg_name)(); \
                } \
            } \
        } \
        return mock_call_modifier; \
    } \

#define IMPLEMENT_COPY_OUT_ARGUMENT_FUNCTION(return_type, name, ...) \
    static MU_C2(mock_call_modifier_,name) MU_C2(copy_out_argument_func_,name)(size_t arg_index, void* value) \
    { \
        DECLARE_MOCK_CALL_MODIFIER(name) \
        (void)value; \
        (void)arg_index; \
        return mock_call_modifier; \
    } \

/* Codes_SRS_UMOCK_C_LIB_01_095: [The ValidateArgumentBuffer call modifier shall copy the memory pointed to by bytes and being length bytes so that it is later compared against a pointer type argument when the code under test calls the mock function.] */
/* Codes_SRS_UMOCK_C_LIB_01_097: [ValidateArgumentBuffer shall implicitly perform an IgnoreArgument on the indexth argument.]*/
/* Codes_SRS_UMOCK_C_LIB_01_099: [If the index is out of range umock_c shall raise an error with the code UMOCK_C_ARG_INDEX_OUT_OF_RANGE.]*/
/* Codes_SRS_UMOCK_C_LIB_01_100: [If bytes is NULL or length is 0, umock_c shall raise an error with the code UMOCK_C_INVALID_ARGUMENT_BUFFER.] */
/* Codes_SRS_UMOCK_C_LIB_01_131: [ The memory pointed by bytes shall be copied. ]*/
/* Codes_SRS_UMOCK_C_LIB_01_132: [ If several calls to ValidateArgumentBuffer are made, only the last buffer shall be kept. ]*/
/* Codes_SRS_UMOCK_C_LIB_01_130: [ The buffers for previous ValidateArgumentBuffer calls shall be freed. ]*/
#define IMPLEMENT_VALIDATE_ARGUMENT_BUFFER_FUNCTION(return_type, name, ...) \
    static MU_C2(mock_call_modifier_,name) MU_C2(validate_argument_buffer_func_,name)(size_t index, const void* bytes, size_t length) \
    { \
        DECLARE_MOCK_CALL_MODIFIER(name) \
        if ((index < 1) || (index > MU_DIV2(MU_COUNT_ARG(__VA_ARGS__)))) \
        { \
            UMOCK_LOG("Bad argument index in ValidateArgumentBuffer: %zu.", index); \
            umock_c_indicate_error(UMOCK_C_ARG_INDEX_OUT_OF_RANGE); \
        } \
        else if ((bytes == NULL) || (length == 0)) \
        { \
            UMOCK_LOG("Bad arguments to ValidateArgumentBuffer: bytes = %p, length = %zu.", bytes, length); \
            umock_c_indicate_error(UMOCK_C_INVALID_ARGUMENT_BUFFER); \
        } \
        else \
        { \
            MU_C2(mock_call_, name)* mock_call_data = (MU_C2(mock_call_, name)*)umockcall_get_call_data(umock_c_get_last_expected_call()); \
            if (mock_call_data == NULL) \
            { \
                UMOCK_LOG("ValidateArgumentBuffer called without having an expected call."); \
                umock_c_indicate_error(UMOCK_C_ERROR); \
            } \
            else \
            { \
                umockalloc_free(mock_call_data->validate_arg_buffers[index - 1].bytes); \
                mock_call_data->validate_arg_buffers[index - 1].bytes = umockalloc_malloc(length); \
                if (mock_call_data->validate_arg_buffers[index - 1].bytes == NULL) \
                { \
                    UMOCK_LOG("Could not allocate memory for validating argument buffers."); \
                    umock_c_indicate_error(UMOCK_C_MALLOC_ERROR); \
                } \
                else \
                { \
                    (void)memcpy(mock_call_data->validate_arg_buffers[index - 1].bytes, bytes, length); \
                    mock_call_data->validate_arg_buffers[index - 1].length = length; \
                    mock_call_modifier.IgnoreArgument(index); \
                } \
            } \
        } \
        return mock_call_modifier; \
    } \

/* Codes_SRS_UMOCK_C_LIB_01_101: [The IgnoreAllCalls call modifier shall record that all calls matching the expected call shall be ignored. If no matching call occurs no missing call shall be reported.]*/
/* Codes_SRS_UMOCK_C_LIB_01_208: [ If no matching call occurs no missing call shall be reported. ]*/
#define IMPLEMENT_IGNORE_ALL_CALLS_FUNCTION(return_type, name, ...) \
    static MU_C2(mock_call_modifier_,name) MU_C2(ignore_all_calls_func_,name)(void) \
    { \
        UMOCKCALL_HANDLE last_expected_call = umock_c_get_last_expected_call(); \
        DECLARE_MOCK_CALL_MODIFIER(name) \
        if (last_expected_call == NULL) \
        { \
            UMOCK_LOG("Cannot get last expected call."); \
            umock_c_indicate_error(UMOCK_C_ERROR); \
        } \
        else \
        { \
            MU_C2(mock_call_, name)* mock_call_data = (MU_C2(mock_call_, name)*)umockcall_get_call_data(last_expected_call); \
            if (mock_call_data == NULL) \
            { \
                UMOCK_LOG("ValidateArgumentBuffer called without having an expected call."); \
                umock_c_indicate_error(UMOCK_C_ERROR); \
            } \
            else \
            { \
                if (umockcall_set_ignore_all_calls(last_expected_call, 1) != 0) \
                { \
                    UMOCK_LOG("Cannot set the ignore_all_calls value on the last expected call."); \
                    umock_c_indicate_error(UMOCK_C_ERROR); \
                } \
            } \
        } \
        return mock_call_modifier; \
    } \

/* Codes_SRS_UMOCK_C_LIB_01_179: [ The CaptureReturn call modifier shall copy the return value that is being returned to the code under test when an actual call is matched with the expected call. ]*/
/* Codes_SRS_UMOCK_C_LIB_01_180: [ If CaptureReturn is called multiple times for the same call, an error shall be indicated with the code UMOCK_C_CAPTURE_RETURN_ALREADY_USED. ]*/
/* Codes_SRS_UMOCK_C_LIB_01_182: [ If captured_return_value is NULL, umock_c shall raise an error with the code UMOCK_C_NULL_ARGUMENT. ]*/
#define IMPLEMENT_CAPTURE_RETURN_FUNCTION(return_type, name, ...) \
    static MU_C2(mock_call_modifier_,name) MU_C2(capture_return_func_,name)(return_type* captured_return_value) \
    { \
        DECLARE_MOCK_CALL_MODIFIER(name) \
        if (captured_return_value == NULL) \
        { \
            UMOCK_LOG("NULL captured_return_value."); \
            umock_c_indicate_error(UMOCK_C_NULL_ARGUMENT); \
        } \
        else \
        { \
            MU_C2(mock_call_, name)* mock_call_data = (MU_C2(mock_call_, name)*)umockcall_get_call_data(umock_c_get_last_expected_call()); \
            if (mock_call_data == NULL) \
            { \
                UMOCK_LOG("CaptureReturn called without having an expected call."); \
                umock_c_indicate_error(UMOCK_C_ERROR); \
            } \
            else \
            { \
                if (mock_call_data->captured_return_value != NULL) \
                { \
                    UMOCK_LOG("CaptureReturn called multiple times."); \
                    umock_c_indicate_error(UMOCK_C_CAPTURE_RETURN_ALREADY_USED); \
                } \
                else \
                { \
                    mock_call_data->captured_return_value = captured_return_value; \
                } \
            } \
        } \
        return mock_call_modifier; \
    }

/* Codes_SRS_UMOCK_C_LIB_01_183: [ The ValidateArgumentValue_{arg_name} shall validate that the value of an argument matches the value pointed by arg_value. ]*/
/* Codes_SRS_UMOCK_C_LIB_01_184: [ If arg_value is NULL, umock_c shall raise an error with the code UMOCK_C_NULL_ARGUMENT. ]*/
/* Codes_SRS_UMOCK_C_LIB_01_185: [ The ValidateArgumentValue_{arg_name} modifier shall inhibit comparing with any value passed directly as an argument in the expected call. ]*/
/* Codes_SRS_UMOCK_C_LIB_01_186: [ The ValidateArgumentValue_{arg_name} shall implicitly do a ValidateArgument for the arg_name argument, making sure the argument is not ignored. ]*/
#define IMPLEMENT_VALIDATE_ARGUMENT_VALUE_BY_NAME_FUNCTION(name, arg_type, arg_name) \
    static MU_C2(mock_call_modifier_,name) MU_C4(validate_argument_value_func_,name,_,arg_name)(arg_type* arg_value) \
    { \
        DECLARE_MOCK_CALL_MODIFIER(name) \
        if (arg_value == NULL) \
        { \
            UMOCK_LOG("NULL argument to ValidateArgumentValue_%s.", MU_TOSTRING(arg_name)); \
            umock_c_indicate_error(UMOCK_C_NULL_ARGUMENT); \
        } \
        else \
        { \
            MU_C2(mock_call_, name)* mock_call_data = (MU_C2(mock_call_, name)*)umockcall_get_call_data(umock_c_get_last_expected_call()); \
            if (mock_call_data == NULL) \
            { \
                UMOCK_LOG("ValidateArgumentValue_%s called without having an expected call.", MU_TOSTRING(arg_name)); \
                umock_c_indicate_error(UMOCK_C_ERROR); \
            } \
            else \
            { \
                mock_call_data->MU_C2(is_ignored_, arg_name) = ARG_IS_NOT_IGNORED; \
                mock_call_data->MU_C2(validate_arg_value_pointer_, arg_name) = (void*)arg_value; \
            } \
        } \
        return mock_call_modifier; \
    }

/* Codes_SRS_UMOCK_C_LIB_01_199: [ ValidateArgumentValue_{arg_name}_AsType shall ensure that validation of the argument arg_name is done as if the argument is of type type_name. ]*/
/* Codes_SRS_UMOCK_C_LIB_01_200: [ If type_name is NULL, umock_c shall raise an error with the code UMOCK_C_NULL_ARGUMENT. ]*/
/* Codes_SRS_UMOCK_C_LIB_01_202: [ If storing the argument value as the new type fails, umock_c shall raise an error with the code UMOCK_C_COPY_ARGUMENT_ERROR. ]*/
#define IMPLEMENT_VALIDATE_ARGUMENT_VALUE_AS_TYPE_BY_NAME_FUNCTION(name, arg_type, arg_name) \
    static MU_C2(mock_call_modifier_,name) MU_C4(validate_argument_value_as_type_func_,name,_,arg_name)(const char* type_name) \
    { \
        DECLARE_MOCK_CALL_MODIFIER(name) \
        if (type_name == NULL) \
        { \
            UMOCK_LOG("NULL argument to ValidateArgumentValue_%s_AsType.", MU_TOSTRING(arg_name)); \
            umock_c_indicate_error(UMOCK_C_NULL_ARGUMENT); \
        } \
        else \
        { \
            MU_C2(mock_call_, name)* mock_call_data = (MU_C2(mock_call_, name)*)umockcall_get_call_data(umock_c_get_last_expected_call()); \
            if (mock_call_data == NULL) \
            { \
                UMOCK_LOG("ValidateArgumentValue_%s called without having an expected call.", MU_TOSTRING(arg_name)); \
                umock_c_indicate_error(UMOCK_C_ERROR); \
            } \
            else \
            { \
                char* cloned_type_name = umockstring_clone(type_name); \
                if (cloned_type_name == NULL) \
                { \
                    umockalloc_free(cloned_type_name); \
                    UMOCK_LOG("Cannot allocate memory to copy type_name in ValidateArgumentValue_%s_AsType for type name %s.", MU_TOSTRING(arg_name), type_name); \
                    umock_c_indicate_error(UMOCK_C_MALLOC_ERROR); \
                } \
                else \
                { \
                    void* temp = umockalloc_malloc(sizeof(arg_type)); \
                    if (temp == NULL) \
                    { \
                        umockalloc_free(cloned_type_name); \
                        UMOCK_LOG("Cannot allocate memory for the temporary argument value in ValidateArgumentValue_%s.", MU_TOSTRING(arg_name)); \
                        umock_c_indicate_error(UMOCK_C_ERROR); \
                    } \
                    else \
                    { \
                        if (umocktypes_copy(GET_USED_ARGUMENT_TYPE(mock_call_data, arg_name, arg_type), (void*)temp, (void*)&mock_call_data->arg_name) != 0) \
                        { \
                            umockalloc_free(cloned_type_name); \
                            UMOCK_LOG("Cannot copy argument in ValidateArgumentValue_%s.", MU_TOSTRING(arg_name)); \
                            umock_c_indicate_error(UMOCK_C_ERROR); \
                        } \
                        else \
                        { \
                            umocktypes_free(GET_USED_ARGUMENT_TYPE(mock_call_data, arg_name, arg_type), (void*)&mock_call_data->arg_name); \
                            if (umocktypes_copy(type_name, (void*)&mock_call_data->arg_name, (void*)temp) != 0) \
                            { \
                                umockalloc_free(cloned_type_name); \
                                UMOCK_LOG("Cannot copy argument as new type in ValidateArgumentValue_%s.", MU_TOSTRING(arg_name)); \
                                umock_c_indicate_error(UMOCK_C_ERROR); \
                            } \
                            else \
                            { \
                                umocktypes_free(GET_USED_ARGUMENT_TYPE(mock_call_data, arg_name, arg_type), (void*)temp); \
                                umockalloc_free(mock_call_data->MU_C2(override_argument_type_, arg_name)); \
                                mock_call_data->MU_C2(override_argument_type_, arg_name) = cloned_type_name; \
                            } \
                        } \
                        umockalloc_free(temp); \
                    } \
                } \
            } \
        } \
        return mock_call_modifier; \
    }

/* Codes_SRS_UMOCK_C_LIB_01_209: [ The CaptureArgumentValue_{arg_name} shall copy the value of the argument at the time of the call to arg_value. ]*/
/* Codes_SRS_UMOCK_C_LIB_01_211: [ The CaptureArgumentValue_{arg_name} shall not change the how the argument is validated. ]*/
#define IMPLEMENT_CAPTURE_ARGUMENT_VALUE_BY_NAME_FUNCTION(name, arg_type, arg_name) \
    static MU_C2(mock_call_modifier_,name) MU_C4(capture_argument_value_func_,name,_,arg_name)(arg_type* arg_value) \
    { \
        DECLARE_MOCK_CALL_MODIFIER(name) \
        if (arg_value == NULL) \
        { \
            UMOCK_LOG("NULL argument to CaptureArgumentValue_%s.", MU_TOSTRING(arg_name)); \
            umock_c_indicate_error(UMOCK_C_NULL_ARGUMENT); \
        } \
        else \
        { \
            MU_C2(mock_call_, name)* mock_call_data = (MU_C2(mock_call_, name)*)umockcall_get_call_data(umock_c_get_last_expected_call()); \
            if (mock_call_data == NULL) \
            { \
                UMOCK_LOG("CaptureArgumentValue_%s called without having an expected call.", MU_TOSTRING(arg_name)); \
                umock_c_indicate_error(UMOCK_C_ERROR); \
            } \
            else \
            { \
                mock_call_data->MU_C2(capture_arg_value_pointer_, arg_name) = (void*)arg_value; \
            } \
        } \
        return mock_call_modifier; \
    }

/* Codes_SRS_UMOCK_C_LIB_31_209: [ call_cannot_fail_func__{name} call modifier shall record that when performing failure case run, this call should be skipped. ]*/
/* Codes_SRS_UMOCK_C_LIB_31_210: [ If recording that the call cannot fail is unsuccessful, umock shall raise with the error code UMOCK_C_ERROR.  ]*/
#define IMPLEMENT_SET_CALL_CANNOT_FAIL(return_type, name, ...) \
        static MU_C2(mock_call_modifier_,name) MU_C2(call_cannot_fail_func_,name)(void) \
        { \
            UMOCKCALL_HANDLE last_expected_call = umock_c_get_last_expected_call(); \
            DECLARE_MOCK_CALL_MODIFIER(name) \
            if (last_expected_call == NULL) \
            { \
                UMOCK_LOG("Cannot get last expected call."); \
                umock_c_indicate_error(UMOCK_C_ERROR); \
            } \
            else \
            { \
                MU_C2(mock_call_, name)* mock_call_data = (MU_C2(mock_call_, name)*)umockcall_get_call_data(last_expected_call); \
                if (mock_call_data == NULL) \
                { \
                    UMOCK_LOG("call_cannot_fail called without having an expected call."); \
                    umock_c_indicate_error(UMOCK_C_ERROR); \
                } \
                else \
                { \
                    if (umockcall_set_call_can_fail(last_expected_call, 0) != 0) \
                    { \
                        UMOCK_LOG("Cannot set call can fail value on the last expected call."); \
                        umock_c_indicate_error(UMOCK_C_ERROR); \
                    } \
                } \
            } \
            return mock_call_modifier; \
        } \


#define IMPLEMENT_MOCK_FUNCTION(function_prefix, args_ignored, return_type, name, ...) \
    MU_C2(mock_call_modifier_,name) UMOCK_STATIC MU_C2(function_prefix,name)(MU_IF(MU_COUNT_ARG(__VA_ARGS__),,void) MU_FOR_EACH_2_COUNTED(ARG_IN_SIGNATURE, __VA_ARGS__)) \
    { \
        UMOCKCALL_HANDLE mock_call; \
        MU_C2(mock_call_,name)* mock_call_data = (MU_C2(mock_call_,name)*)umockalloc_malloc(sizeof(MU_C2(mock_call_,name))); \
        DECLARE_MOCK_CALL_MODIFIER(name) \
        MU_FOR_EACH_2(COPY_ARG_TO_MOCK_STRUCT, __VA_ARGS__) \
        MU_IF(args_ignored, MU_FOR_EACH_2(MARK_ARG_AS_IGNORED, __VA_ARGS__), MU_FOR_EACH_2(MARK_ARG_AS_NOT_IGNORED, __VA_ARGS__)) \
        MU_FOR_EACH_2_COUNTED(CLEAR_OUT_ARG_BUFFERS, __VA_ARGS__) \
        MU_FOR_EACH_2_COUNTED(CLEAR_VALIDATE_ARG_BUFFERS, __VA_ARGS__) \
        MU_FOR_EACH_2(CLEAR_VALIDATE_ARG_VALUE, __VA_ARGS__) \
        MU_FOR_EACH_2(CLEAR_CAPTURE_ARG_VALUE, __VA_ARGS__) \
        MU_FOR_EACH_2(CLEAR_OVERRIDE_ARGUMENT_TYPE, __VA_ARGS__) \
        MU_IF(IS_NOT_VOID(return_type), \
            mock_call_data->return_value_set = RETURN_VALUE_NOT_SET; \
            mock_call_data->captured_return_value = NULL; \
            mock_call_data->fail_return_value_set = FAIL_RETURN_VALUE_NOT_SET; \
        ,) \
        mock_call = umockcall_create(#name, mock_call_data, MU_C2(mock_call_data_copy_func_,name), MU_C2(mock_call_data_free_func_,name), MU_C2(mock_call_data_stringify_,name), MU_C2(mock_call_data_are_equal_,name)); \
        if (mock_call == NULL) \
        { \
            UMOCK_LOG("Failed creating mock call."); \
            umock_c_indicate_error(UMOCK_C_ERROR); \
        } \
        else \
        { \
            (void)umock_c_add_expected_call(mock_call); \
            (void)umockcall_set_call_can_fail(mock_call, MU_IF(IS_NOT_VOID(return_type), 1, 0)); \
        } \
        return mock_call_modifier; \
    } \

#define IMPLEMENT_STRICT_EXPECTED_MOCK(return_type, name, ...) \
    IMPLEMENT_MOCK_FUNCTION(umock_c_strict_expected_, 0, return_type, name, __VA_ARGS__)

#define IMPLEMENT_EXPECTED_MOCK(return_type, name, ...) \
    IMPLEMENT_MOCK_FUNCTION(umock_c_expected_, 1, return_type, name, __VA_ARGS__)

/* Codes_SRS_UMOCK_C_LIB_01_104: [The REGISTER_GLOBAL_MOCK_HOOK shall register a mock hook to be called every time the mocked function is called by production code.]*/
/* Codes_SRS_UMOCK_C_LIB_01_107: [If there are multiple invocations of REGISTER_GLOBAL_MOCK_HOOK, the last one shall take effect over the previous ones.] */
/* Codes_SRS_UMOCK_C_LIB_01_134: [ REGISTER_GLOBAL_MOCK_HOOK called with a NULL hook unregisters a previously registered hook. ]*/
#define IMPLEMENT_REGISTER_GLOBAL_MOCK_HOOK(return_type, name, ...) \
    UMOCK_STATIC void MU_C2(set_global_mock_hook_,name)(MU_C2(mock_hook_func_type_, name) mock_return_hook) \
    { \
        MU_C2(mock_hook_,name) = mock_return_hook; \
    } \

/* Codes_SRS_UMOCK_C_LIB_01_108: [The REGISTER_GLOBAL_MOCK_RETURN shall register a return value to always be returned by a mock function.]*/
/* Codes_SRS_UMOCK_C_LIB_01_109: [If there are multiple invocations of REGISTER_GLOBAL_MOCK_RETURN, the last one shall take effect over the previous ones.]*/
/* Codes_SRS_UMOCK_C_LIB_01_141: [ If any error occurs during REGISTER_GLOBAL_MOCK_RETURN, umock_c shall raise an error with the code UMOCK_C_ERROR. ]*/
#define IMPLEMENT_REGISTER_GLOBAL_MOCK_RETURN(return_type, name, ...) \
    MU_IF(IS_NOT_VOID(return_type), UMOCK_STATIC void MU_C2(set_global_mock_return_, name)(return_type return_value) \
    { \
        MU_C2(get_mock_call_return_values_,name)()->success_value = return_value; \
    }, ) \

/* Codes_SRS_UMOCK_C_LIB_01_111: [The REGISTER_GLOBAL_MOCK_FAIL_RETURN shall register a fail return value to be returned by a mock function when marked as failed in the expected calls.]*/
/* Codes_SRS_UMOCK_C_LIB_01_112: [If there are multiple invocations of REGISTER_GLOBAL_FAIL_MOCK_RETURN, the last one shall take effect over the previous ones.]*/
/* Codes_SRS_UMOCK_C_LIB_01_142: [ If any error occurs during REGISTER_GLOBAL_MOCK_FAIL_RETURN, umock_c shall raise an error with the code UMOCK_C_ERROR. ]*/
#define IMPLEMENT_REGISTER_GLOBAL_MOCK_FAIL_RETURN(return_type, name, ...) \
    MU_IF(IS_NOT_VOID(return_type), UMOCK_STATIC void MU_C2(set_global_mock_fail_return_, name)(return_type fail_return_value) \
    { \
        MU_C2(get_mock_call_return_values_,name)()->failure_value = fail_return_value; \
    }, ) \

/* Codes_SRS_UMOCK_C_LIB_01_113: [The REGISTER_GLOBAL_MOCK_RETURNS shall register both a success and a fail return value associated with a mock function.]*/
/* Codes_SRS_UMOCK_C_LIB_01_114: [If there are multiple invocations of REGISTER_GLOBAL_MOCK_RETURNS, the last one shall take effect over the previous ones.]*/
/* Codes_SRS_UMOCK_C_LIB_01_143: [ If any error occurs during REGISTER_GLOBAL_MOCK_RETURNS, umock_c shall raise an error with the code UMOCK_C_ERROR. ]*/
#define IMPLEMENT_REGISTER_GLOBAL_MOCK_RETURNS(return_type, name, ...) \
    MU_IF(IS_NOT_VOID(return_type), UMOCK_STATIC void MU_C2(set_global_mock_returns_, name)(return_type return_value, return_type fail_return_value) \
    { \
        MU_C2(get_mock_call_return_values_,name)()->success_value = return_value; \
        MU_C2(get_mock_call_return_values_,name)()->failure_value = fail_return_value; \
    }, ) \

#define DECLARE_VALIDATE_ONE_ARGUMENT_FUNC_TYPE(name) \
    typedef struct MU_C2(_mock_call_modifier_, name) (*MU_C2(validate_one_argument_func_type_, name))(void);

#define COPY_RETURN_VALUE(return_type, name) \
    result_C8417226_7442_49B4_BBB9_9CA816A21EB7 = MU_C2(get_mock_call_return_values_, name)()->success_value;

typedef struct MOCK_CALL_ARG_METADATA_TAG
{
    const char* type;
    const char* name;
} MOCK_CALL_ARG_METADATA;

typedef struct MOCK_CALL_METADATA_TAG
{
    const char* return_type;
    const char* name;
    size_t arg_count;
    const MOCK_CALL_ARG_METADATA* args;
} MOCK_CALL_METADATA;

#define UNUSED_ARG(arg_type, arg_name) \
    (void)arg_name;

/* Codes_SRS_UMOCK_C_LIB_01_205: [ If IGNORED_PTR_ARG or IGNORED_NUM_ARG is used as an argument value with STRICT_EXPECTED_CALL, the argument shall be automatically ignored. ]*/
/* Codes_SRS_UMOCK_C_LIB_01_206: [ IGNORED_PTR_ARG shall be defined as NULL so that it can be used for pointer type arguments. ]*/
/* Codes_SRS_UMOCK_C_LIB_01_207: [ IGNORED_NUM_ARG shall be defined to 0 so that it can be used for numeric type arguments. ]*/
#define AUTO_IGNORE_ARG(arg_type, arg_name) \
    if (umockautoignoreargs_is_call_argument_ignored(call_as_string, arg_index++, &is_ignored) != 0) \
    { \
        UMOCK_LOG("Failed parsing argument %s value from the call.", MU_TOSTRING(arg_name)); \
    } \
    else \
    { \
        if (is_ignored) \
        { \
            result.MU_C2(IgnoreArgument_, arg_name)(); \
        } \
    } \

#define COPY_CAPTURED_ARGUMENT_VALUE(arg_type, arg_name) \
    if (matched_call_data->MU_C2(capture_arg_value_pointer_,arg_name) != NULL) \
    { \
        if (umocktypes_copy(#arg_type, (void*)matched_call_data->MU_C2(capture_arg_value_pointer_,arg_name), (void*)&arg_name) != 0) \
        { \
            UMOCK_LOG("Could not copy captured argument value of type %s.", MU_TOSTRING(arg_type)); \
            umock_c_indicate_error(UMOCK_C_ERROR); \
        } \
    } \

/* Codes_SRS_UMOCK_C_LIB_01_004: [If ENABLE_MOCKS is defined, MOCKABLE_FUNCTION shall generate the declaration of the function and code for the mocked function, thus allowing setting up of expectations in test functions.] */
/* Codes_SRS_UMOCK_C_LIB_01_014: [For each argument the argument value shall be stored for later comparison with actual calls.] */
/* Codes_SRS_UMOCK_C_LIB_01_017: [No arguments shall be saved by default, unless other modifiers state it.]*/
/* Codes_SRS_UMOCK_C_LIB_01_074: [When an expected call is recorded a call modifier interface in the form of a structure containing function pointers shall be returned to the caller.] */
/* Codes_SRS_UMOCK_C_LIB_01_075: [The last modifier in a chain overrides previous modifiers if any collision occurs.]*/
/* Codes_SRS_UMOCK_C_LIB_01_127: [ IgnoreAllArguments shall only be available for mock functions that have arguments. ]*/
/* Codes_SRS_UMOCK_C_LIB_01_126: [ ValidateAllArguments shall only be available for mock functions that have arguments. ]*/
/* Codes_SRS_UMOCK_C_LIB_01_125: [ IgnoreArgument_{arg_name} shall only be available for mock functions that have arguments. ]*/
/* Codes_SRS_UMOCK_C_LIB_01_124: [ ValidateArgument_{arg_name} shall only be available for mock functions that have arguments. **]*/
/* Codes_SRS_UMOCK_C_LIB_01_123: [ IgnoreArgument shall only be available for mock functions that have arguments. ]*/
/* Codes_SRS_UMOCK_C_LIB_01_122: [ ValidateArgument shall only be available for mock functions that have arguments. ]*/
/* Codes_SRS_UMOCK_C_LIB_01_121: [ SetReturn shall only be available if the return type is not void. ]*/
/* Codes_SRS_UMOCK_C_LIB_01_120: [ SetFailReturn shall only be available if the return type is not void. ]*/
/* Codes_SRS_UMOCK_C_LIB_01_119: [ CopyOutArgumentBuffer shall only be available for mock functions that have arguments. ]*/
/* Codes_SRS_UMOCK_C_LIB_01_128: [ CopyOutArgument shall only be available for mock functions that have arguments. ]*/
/* Codes_SRS_UMOCK_C_LIB_01_129: [ ValidateArgumentBuffer shall only be available for mock functions that have arguments. ]*/
/* Codes_SRS_UMOCK_C_LIB_01_105: [The hook\92s result shall be returned by the mock to the production code.]*/
/* Codes_SRS_UMOCK_C_LIB_01_106: [The signature for the hook shall be assumed to have exactly the same arguments and return as the mocked function.]*/
/* Codes_SRS_UMOCK_C_LIB_01_135: [ All parameters passed to the mock shall be passed down to the mock hook. ]*/
/* Codes_SRS_UMOCK_C_LIB_01_148: [ If call comparison fails an error shall be indicated by calling the error callback with UMOCK_C_COMPARE_CALL_ERROR. ]*/
/* Codes_SRS_UMOCK_C_LIB_01_136: [ When multiple return values are set for a mock function by using different means (such as SetReturn), the following order shall be in effect: ]*/
/* Codes_SRS_UMOCK_C_LIB_01_137: [ - If a return value has been specified for an expected call then that value shall be returned. ]*/
/* Codes_SRS_UMOCK_C_LIB_01_138: [ - If a global mock hook has been specified then it shall be called and its result returned. ]*/
/* Codes_SRS_UMOCK_C_LIB_01_139: [ - If a global return value has been specified then it shall be returned. ]*/
/* Codes_SRS_UMOCK_C_LIB_01_140: [ - Otherwise the value of a static variable of the same type as the return type shall be returned. ]*/
/* Codes_SRS_UMOCK_C_LIB_01_188: [ The create call shall have a non-void return type. ]*/
#define MOCKABLE_FUNCTION_UMOCK_INTERNAL_WITH_MOCK_NO_CODE(do_returns, return_type, name, ...) \
    typedef return_type (*MU_C2(mock_hook_func_type_, name))(MU_IF(MU_COUNT_ARG(__VA_ARGS__),,void) MU_FOR_EACH_2_COUNTED(ARG_IN_SIGNATURE, __VA_ARGS__)); \
    return_type MU_C2(real_, name)(MU_IF(MU_COUNT_ARG(__VA_ARGS__),,void) MU_FOR_EACH_2_COUNTED(ARG_IN_SIGNATURE, __VA_ARGS__)) UMOCK_C_WEAK; \
    static MU_C2(mock_hook_func_type_,name) MU_C2(mock_hook_,name) = NULL; \
    static TRACK_CREATE_FUNC_TYPE MU_C2(track_create_destroy_pair_malloc_,name) = NULL; \
    static TRACK_DESTROY_FUNC_TYPE MU_C2(track_create_destroy_pair_free_,name) = NULL; \
    static PAIRED_HANDLES MU_C2(paired_handles_,name); \
    static PAIRED_HANDLES* MU_C2(used_paired_handles_,name) = NULL; \
    static const MOCK_CALL_ARG_METADATA MU_C2(mock_call_args_metadata_,name)[MU_IF(MU_COUNT_ARG(__VA_ARGS__), MU_DIV2(MU_COUNT_ARG(__VA_ARGS__)), 1)] \
        = { MU_IF(MU_COUNT_ARG(__VA_ARGS__),,NULL) MU_FOR_EACH_2(FILL_ARG_IN_METADATA, __VA_ARGS__) }; \
    static const MOCK_CALL_METADATA MU_C2(mock_call_metadata_,name) = {MU_TOSTRING(return_type), MU_TOSTRING(name), MU_DIV2(MU_COUNT_ARG(__VA_ARGS__)), \
        MU_C2(mock_call_args_metadata_,name) }; \
    struct MU_C2(_mock_call_modifier_,name); \
    MU_IF(IS_NOT_VOID(return_type), \
    typedef struct MU_C3(UMOCK_RETURNS_VALUES_STRUCT_, name, _TAG) \
    { \
        return_type success_value; \
        return_type failure_value; \
        int initialized : 1; \
    } MU_C2(UMOCK_RETURNS_VALUES_STRUCT_, name); \
    static MU_C2(UMOCK_RETURNS_VALUES_STRUCT_, name) MU_C2(mock_call_return_values_, name); \
    static MU_C2(UMOCK_RETURNS_VALUES_STRUCT_, name)* MU_C2(get_mock_call_return_values_, name)(void); \
        MU_IF(IS_NOT_VOID(return_type), \
        MU_IF(do_returns,, \
            static MU_C2(UMOCK_RETURNS_VALUES_STRUCT_, name)* MU_C2(get_mock_call_return_values_, name)(void) \
            { \
                MU_C2(UMOCK_RETURNS_VALUES_STRUCT_, name)* result = &MU_C2(mock_call_return_values_, name); \
                return result; \
            } \
        ) \
    ,) \
    typedef struct MU_C2(_mock_call_modifier_,name) (*MU_C2(set_return_func_type_,name))(return_type return_value); \
    typedef struct MU_C2(_mock_call_modifier_,name) (*MU_C2(set_fail_return_func_type_,name))(return_type return_value); \
    typedef struct MU_C2(_mock_call_modifier_,name) (*MU_C2(call_cannot_fail_func_type_,name))(void); \
    typedef struct MU_C2(_mock_call_modifier_,name) (*MU_C2(capture_return_func_type_,name))(return_type* captured_return_value);,) \
    typedef struct MU_C2(_mock_call_modifier_,name) (*MU_C2(ignore_all_calls_func_type_,name))(void); \
    MU_IF(MU_COUNT_ARG(__VA_ARGS__),typedef struct MU_C2(_mock_call_modifier_,name) (*MU_C2(ignore_all_arguments_func_type_,name))(void); \
    typedef struct MU_C2(_mock_call_modifier_,name) (*MU_C2(validate_all_arguments_func_type_,name))(void); \
    typedef struct MU_C2(_mock_call_modifier_,name) (*MU_C2(copy_out_argument_func_type_,name))(size_t arg_index, void* value); \
    typedef struct MU_C2(_mock_call_modifier_,name) (*MU_C2(ignore_argument_func_type_,name))(size_t arg_index); \
    typedef struct MU_C2(_mock_call_modifier_,name) (*MU_C2(validate_argument_func_type_,name))(size_t arg_index); \
    typedef struct MU_C2(_mock_call_modifier_,name) (*MU_C2(validate_argument_buffer_func_type_,name))(size_t index, const void* bytes, size_t length); \
    typedef struct MU_C2(_mock_call_modifier_,name) (*MU_C2(copy_out_argument_buffer_func_type_,name))(size_t index, const void* bytes, size_t length); \
    MU_FOR_EACH_2_KEEP_1(DECLARE_ARG_RELATED_FUNCTIONS, name, __VA_ARGS__),) \
    typedef struct MU_C2(_mock_call_modifier_,name) \
    { \
        MU_C2(ignore_all_calls_func_type_,name) IgnoreAllCalls; \
        MU_IF(IS_NOT_VOID(return_type),MU_C2(set_return_func_type_,name) SetReturn; \
        MU_C2(set_fail_return_func_type_,name) SetFailReturn; \
        MU_C2(call_cannot_fail_func_type_,name) CallCannotFail; \
        MU_C2(capture_return_func_type_,name) CaptureReturn;,) \
        MU_IF(MU_COUNT_ARG(__VA_ARGS__),MU_C2(ignore_all_arguments_func_type_,name) IgnoreAllArguments; \
        MU_C2(validate_all_arguments_func_type_,name) ValidateAllArguments; \
        MU_C2(copy_out_argument_func_type_,name) CopyOutArgument; \
        MU_C2(ignore_argument_func_type_,name) IgnoreArgument; \
        MU_C2(validate_argument_func_type_,name) ValidateArgument; \
        MU_C2(validate_argument_buffer_func_type_,name) ValidateArgumentBuffer; \
        MU_C2(copy_out_argument_buffer_func_type_,name) CopyOutArgumentBuffer; \
        MU_FOR_EACH_2_KEEP_1(ARG_RELATED_FUNCTIONS_IN_MODIFIERS, name, __VA_ARGS__),) \
    } MU_C2(mock_call_modifier_,name); \
    static MU_C2(mock_call_modifier_,name) MU_C2(ignore_all_calls_func_,name)(void); \
    MU_IF(IS_NOT_VOID(return_type),static MU_C2(mock_call_modifier_,name) MU_C2(set_return_func_,name)(return_type return_value); \
    static MU_C2(mock_call_modifier_,name) MU_C2(set_fail_return_func_,name)(return_type return_value); \
    static MU_C2(mock_call_modifier_,name) MU_C2(call_cannot_fail_func_,name)(void); \
    static MU_C2(mock_call_modifier_,name) MU_C2(capture_return_func_,name)(return_type* captured_return_value);,) \
    MU_IF(MU_COUNT_ARG(__VA_ARGS__),static MU_C2(mock_call_modifier_,name) MU_C2(ignore_all_arguments_func_,name)(void); \
    static MU_C2(mock_call_modifier_,name) MU_C2(validate_all_arguments_func_,name)(void); \
    static MU_C2(mock_call_modifier_,name) MU_C2(copy_out_argument_func_,name)(size_t arg_index, void* value); \
    static MU_C2(mock_call_modifier_,name) MU_C2(ignore_argument_func_,name)(size_t arg_index); \
    static MU_C2(mock_call_modifier_,name) MU_C2(validate_argument_func_,name)(size_t arg_index); \
    static MU_C2(mock_call_modifier_,name) MU_C2(validate_argument_buffer_func_,name)(size_t index, const void* bytes, size_t length); \
    static MU_C2(mock_call_modifier_,name) MU_C2(copy_out_argument_buffer_func_,name)(size_t index, const void* bytes, size_t length); \
    MU_FOR_EACH_2_KEEP_1(DECLARE_IGNORE_ARGUMENT_FUNCTION_PROTOTYPE, name, __VA_ARGS__) \
    MU_FOR_EACH_2_KEEP_1(DECLARE_VALIDATE_ARGUMENT_FUNCTION_PROTOTYPE, name, __VA_ARGS__) \
    MU_FOR_EACH_2_KEEP_1(DECLARE_COPY_OUT_ARGUMENT_BUFFER_FUNCTION_PROTOTYPE, name, __VA_ARGS__) \
    MU_FOR_EACH_2_KEEP_1(DECLARE_VALIDATE_ARGUMENT_VALUE_FUNCTION_PROTOTYPE, name, __VA_ARGS__) \
    MU_FOR_EACH_2_KEEP_1(DECLARE_VALIDATE_ARGUMENT_VALUE_AS_TYPE_FUNCTION_PROTOTYPE, name, __VA_ARGS__) \
    MU_FOR_EACH_2_KEEP_1(DECLARE_CAPTURE_ARGUMENT_VALUE_FUNCTION_PROTOTYPE, name, __VA_ARGS__) \
    typedef struct MU_C2(_mock_call_modifier_,name) (*MU_C2(ignore_one_argument_func_type_,name))(void);,) \
    MU_IF(MU_COUNT_ARG(__VA_ARGS__), static const MU_C2(ignore_one_argument_func_type_,name) MU_C2(ignore_one_argument_array_,name)[] = \
    {,) \
        MU_FOR_EACH_2_KEEP_1(IGNORE_ARGUMENT_FUNCTION_IN_ARRAY, name, __VA_ARGS__) \
    MU_IF(MU_COUNT_ARG(__VA_ARGS__), }; \
    DECLARE_VALIDATE_ONE_ARGUMENT_FUNC_TYPE(name) \
    static const MU_C2(validate_one_argument_func_type_,name) MU_C2(validate_one_argument_array_,name)[] = \
    {,) \
        MU_FOR_EACH_2_KEEP_1(VALIDATE_ARGUMENT_FUNCTION_IN_ARRAY, name, __VA_ARGS__) \
    MU_IF(MU_COUNT_ARG(__VA_ARGS__),};,) \
    static void MU_C2(fill_mock_call_modifier_,name)(MU_C2(mock_call_modifier_,name)* mock_call_modifier) \
    { \
        MU_IF(IS_NOT_VOID(return_type),mock_call_modifier->SetReturn = MU_C2(set_return_func_,name); \
        mock_call_modifier->SetFailReturn = MU_C2(set_fail_return_func_,name); \
        mock_call_modifier->CallCannotFail = MU_C2(call_cannot_fail_func_,name); \
        mock_call_modifier->CaptureReturn = MU_C2(capture_return_func_,name);,) \
        MU_IF(MU_COUNT_ARG(__VA_ARGS__),mock_call_modifier->IgnoreAllArguments = MU_C2(ignore_all_arguments_func_,name); \
        mock_call_modifier->ValidateAllArguments = MU_C2(validate_all_arguments_func_,name); \
        mock_call_modifier->CopyOutArgument = MU_C2(copy_out_argument_func_,name); \
        mock_call_modifier->IgnoreArgument = MU_C2(ignore_argument_func_,name); \
        mock_call_modifier->ValidateArgument = MU_C2(validate_argument_func_,name); \
        mock_call_modifier->ValidateArgumentBuffer = MU_C2(validate_argument_buffer_func_,name); \
        mock_call_modifier->CopyOutArgumentBuffer = MU_C2(copy_out_argument_buffer_func_,name); \
        MU_FOR_EACH_2_KEEP_1(COPY_IGNORE_ARG_BY_NAME_TO_MODIFIER, name, __VA_ARGS__) \
        MU_FOR_EACH_2_KEEP_1(COPY_VALIDATE_ARG_BY_NAME_TO_MODIFIER, name, __VA_ARGS__) \
        MU_FOR_EACH_2_KEEP_1(COPY_COPY_OUT_ARGUMENT_BUFFER_BY_NAME_TO_MODIFIER, name, __VA_ARGS__) \
        MU_FOR_EACH_2_KEEP_1(COPY_VALIDATE_ARGUMENT_VALUE_BY_NAME_TO_MODIFIER, name, __VA_ARGS__) \
        MU_FOR_EACH_2_KEEP_1(COPY_VALIDATE_ARGUMENT_VALUE_AS_TYPE_BY_NAME_TO_MODIFIER, name, __VA_ARGS__),) \
        MU_FOR_EACH_2_KEEP_1(COPY_CAPTURE_ARGUMENT_VALUE_BY_NAME_TO_MODIFIER, name, __VA_ARGS__) \
        mock_call_modifier->IgnoreAllCalls = MU_C2(ignore_all_calls_func_,name); \
    } \
    typedef struct MU_C2(_mock_call_,name) \
    { \
        MU_IF(IS_NOT_VOID(return_type),return_type return_value; \
        return_type fail_return_value; \
        return_type* captured_return_value;,) \
        MU_IF(MU_COUNT_ARG(__VA_ARGS__), MU_FOR_EACH_2(DECLARE_MOCK_CALL_STRUCT_STACK, __VA_ARGS__) \
        MU_FOR_EACH_2(DECLARE_OUT_ARG_BUFFER_FOR_ARG, __VA_ARGS__) \
        ARG_BUFFER* out_arg_buffers[MU_IF(MU_COUNT_ARG(__VA_ARGS__), MU_DIV2(MU_COUNT_ARG(__VA_ARGS__)),1)]; \
        ARG_BUFFER validate_arg_buffers[MU_IF(MU_COUNT_ARG(__VA_ARGS__), MU_DIV2(MU_COUNT_ARG(__VA_ARGS__)),1)]; \
        MU_FOR_EACH_2(DECLARE_VALIDATE_ARG_VALUE, __VA_ARGS__) \
        MU_FOR_EACH_2(DECLARE_CAPTURE_ARG_VALUE, __VA_ARGS__) \
        MU_FOR_EACH_2(DECLARE_IGNORE_FLAG_FOR_ARG, __VA_ARGS__) \
        MU_FOR_EACH_2(DECLARE_OVERRIDE_ARGUMENT_TYPE_FOR_ARG, __VA_ARGS__),) \
        MU_IF(IS_NOT_VOID(return_type),int fail_return_value_set : 1; \
        int return_value_set : 1;,) \
        MU_IF(MU_COUNT_ARG(__VA_ARGS__), , MU_IF(IS_NOT_VOID(return_type),, int dummy : 1;)) \
    } MU_C2(mock_call_,name); \
    typedef MU_C2(mock_call_modifier_,name) (*MU_C3(auto_ignore_args_function_,name,_type))(MU_C2(mock_call_modifier_,name) call_modifier, const char* call_as_string); \
    MU_C2(mock_call_modifier_,name) UMOCK_STATIC MU_C2(auto_ignore_args_function_,name)(MU_C2(mock_call_modifier_,name) call_modifier, const char* call_as_string) \
    { \
        MU_C2(mock_call_modifier_,name) result = call_modifier; \
        MU_IF(MU_COUNT_ARG(__VA_ARGS__), \
        int is_ignored; \
        int arg_index = 1; ,) \
        (void)call_as_string; \
        MU_FOR_EACH_2(AUTO_IGNORE_ARG, __VA_ARGS__) \
        return result; \
    } \
    MU_C3(auto_ignore_args_function_,name,_type) UMOCK_STATIC MU_C2(get_auto_ignore_args_function_,name)(MU_IF(MU_COUNT_ARG(__VA_ARGS__),,void) MU_FOR_EACH_2_COUNTED(ARG_IN_SIGNATURE, __VA_ARGS__)) \
    { \
        MU_FOR_EACH_2(UNUSED_ARG, __VA_ARGS__) \
        return MU_C2(auto_ignore_args_function_,name); \
    } \
    UMOCK_STATIC char* MU_C2(mock_call_data_stringify_,name)(void* mock_call_data) \
    { \
        char* result; \
        MU_IF(MU_COUNT_ARG(__VA_ARGS__), MU_C2(mock_call_,name)* typed_mock_call_data = (MU_C2(mock_call_,name)*)mock_call_data;,) \
        int is_error = 0; \
        size_t args_string_length = 0; \
        MU_FOR_EACH_2_COUNTED(STRINGIFY_ARGS_DECLARE_RESULT_VAR, __VA_ARGS__) \
        MU_IF(MU_COUNT_ARG(__VA_ARGS__),,(void)mock_call_data;) \
        MU_FOR_EACH_2(STRINGIFY_ARGS_CHECK_ARG_STRINGIFY_SUCCESS, __VA_ARGS__) \
        (void)mock_call_data; \
        if (is_error != 0) \
        { \
            result = NULL; \
        } \
        else \
        { \
            MU_FOR_EACH_2(STRINGIFY_ARGS_DECLARE_ARG_STRING_LENGTH, __VA_ARGS__) \
            MU_FOR_EACH_2(STRINGIFY_ARGS_COUNT_LENGTH, __VA_ARGS__) \
            MU_IF(MU_COUNT_ARG(__VA_ARGS__),  args_string_length += MU_COUNT_ARG(__VA_ARGS__) - 1;,) \
            result = (char*)umockalloc_malloc(args_string_length + 1); \
            if (result != NULL) \
            { \
                if (args_string_length == 0) \
                { \
                    result[0] = '\0'; \
                } \
                else \
                { \
                    MU_IF(MU_COUNT_ARG(__VA_ARGS__), \
                    size_t current_pos = 0; \
                    size_t arg_index = 0; \
                    MU_FOR_EACH_2(STRINGIFY_ARGS_COPY_ARG_STRING, __VA_ARGS__), ) \
                } \
            } \
        } \
        MU_FOR_EACH_2(STRINGIFY_ARGS_FREE_STRINGIFIED_ARG, __VA_ARGS__) \
        return result; \
    } \
    UMOCK_STATIC int MU_C2(mock_call_data_are_equal_,name)(void* left, void* right) \
    { \
        int result; \
        if (left == right) \
        { \
            result = 1; \
        } \
        else if ((left == NULL) || (right == NULL)) \
        { \
            result = 0; \
        } \
        else \
        { \
            result = 1; \
            MU_IF(MU_COUNT_ARG(__VA_ARGS__), \
            { \
                MU_C2(mock_call_,name)* typed_left = (MU_C2(mock_call_,name)*)left; \
                MU_C2(mock_call_,name)* typed_right = (MU_C2(mock_call_,name)*)right; \
                MU_FOR_EACH_2_COUNTED(ARE_EQUAL_FOR_ARG, __VA_ARGS__) \
            }, ) \
        } \
        return result; \
    } \
    UMOCK_STATIC void MU_C2(mock_call_data_free_func_,name)(void* mock_call_data) \
    { \
        MU_C2(mock_call_,name)* typed_mock_call_data = (MU_C2(mock_call_,name)*)mock_call_data; \
        MU_FOR_EACH_2_COUNTED(FREE_ARG_VALUE, __VA_ARGS__) \
        MU_FOR_EACH_2_COUNTED(FREE_OUT_ARG_BUFFERS, __VA_ARGS__) \
        MU_FOR_EACH_2_COUNTED(FREE_VALIDATE_ARG_BUFFERS, __VA_ARGS__) \
        MU_FOR_EACH_2_COUNTED(FREE_OVERRIDE_ARGUMENT_TYPE, __VA_ARGS__) \
        MU_IF(IS_NOT_VOID(return_type),if (typed_mock_call_data->return_value_set == RETURN_VALUE_SET) \
        { \
            umocktypes_free(MU_TOSTRING(return_type), (void*)&typed_mock_call_data->return_value); \
        } \
        if (typed_mock_call_data->fail_return_value_set == FAIL_RETURN_VALUE_SET) \
        { \
            umocktypes_free(MU_TOSTRING(return_type), (void*)&typed_mock_call_data->fail_return_value); \
        },) \
        umockalloc_free(typed_mock_call_data); \
    } \
    UMOCK_STATIC void* MU_C2(mock_call_data_copy_func_,name)(void* mock_call_data) \
    { \
        MU_C2(mock_call_,name)* result = (MU_C2(mock_call_,name)*)umockalloc_malloc(sizeof(MU_C2(mock_call_,name))); \
        MU_IF(MU_COUNT_ARG(__VA_ARGS__), MU_C2(mock_call_,name)* typed_mock_call_data = (MU_C2(mock_call_,name)*)mock_call_data;,) \
        MU_IF(IS_NOT_VOID(return_type), MU_C2(mock_call_,name)* typed_mock_call_data_result = (MU_C2(mock_call_,name)*)mock_call_data;,) \
        (void)mock_call_data; \
        MU_FOR_EACH_2(COPY_IGNORE_ARG, __VA_ARGS__) \
        MU_FOR_EACH_2(COPY_ARG_VALUE, __VA_ARGS__) \
        MU_FOR_EACH_2_COUNTED(COPY_OUT_ARG_BUFFERS, __VA_ARGS__) \
        MU_FOR_EACH_2_COUNTED(COPY_VALIDATE_ARG_BUFFERS, __VA_ARGS__) \
        MU_FOR_EACH_2(COPY_VALIDATE_ARG_VALUE, __VA_ARGS__) \
        MU_FOR_EACH_2(COPY_OVERRIDE_ARGUMENT_TYPE, __VA_ARGS__) \
        MU_FOR_EACH_2(COPY_CAPTURE_ARG_VALUE, __VA_ARGS__) \
        MU_IF(IS_NOT_VOID(return_type), \
        result->return_value_set = typed_mock_call_data_result->return_value_set; \
        result->fail_return_value_set = typed_mock_call_data_result->fail_return_value_set; \
        if (typed_mock_call_data_result->return_value_set == RETURN_VALUE_SET) \
        { \
            umocktypes_copy(MU_TOSTRING(return_type), (void*)&result->return_value, (void*)&typed_mock_call_data_result->return_value); \
        } \
        result->captured_return_value = typed_mock_call_data_result->captured_return_value; \
        if (typed_mock_call_data_result->fail_return_value_set == FAIL_RETURN_VALUE_SET) \
        { \
            umocktypes_copy(MU_TOSTRING(return_type), (void*)&result->fail_return_value, (void*)&typed_mock_call_data_result->fail_return_value); \
        },) \
        return result; \
    } \
    MU_IF(IS_NOT_VOID(return_type), \
    IMPLEMENT_SET_RETURN_FUNCTION(return_type, name, __VA_ARGS__) \
    IMPLEMENT_SET_FAIL_RETURN_FUNCTION(return_type, name, __VA_ARGS__) \
    IMPLEMENT_SET_CALL_CANNOT_FAIL(return_type, name, __VA_ARGS__) \
    IMPLEMENT_CAPTURE_RETURN_FUNCTION(return_type, name, __VA_ARGS__),) \
    MU_IF(MU_COUNT_ARG(__VA_ARGS__),IMPLEMENT_IGNORE_ALL_ARGUMENTS_FUNCTION(return_type, name, __VA_ARGS__) \
    IMPLEMENT_VALIDATE_ALL_ARGUMENTS_FUNCTION(return_type, name, __VA_ARGS__) \
    MU_FOR_EACH_2_KEEP_1(IMPLEMENT_IGNORE_ARGUMENT_BY_NAME_FUNCTION, name, __VA_ARGS__) \
    MU_FOR_EACH_2_KEEP_1(IMPLEMENT_VALIDATE_ARGUMENT_BY_NAME_FUNCTION, name, __VA_ARGS__) \
    IMPLEMENT_IGNORE_ARGUMENT_FUNCTION(return_type, name, __VA_ARGS__) \
    IMPLEMENT_VALIDATE_ARGUMENT_FUNCTION(return_type, name, __VA_ARGS__) \
    IMPLEMENT_COPY_OUT_ARGUMENT_BUFFER_FUNCTION(return_type, name, __VA_ARGS__) \
    MU_FOR_EACH_2_KEEP_1(IMPLEMENT_COPY_OUT_ARGUMENT_BUFFER_BY_NAME_FUNCTION, name, __VA_ARGS__) \
    IMPLEMENT_COPY_OUT_ARGUMENT_FUNCTION(return_type, name, __VA_ARGS__) \
    IMPLEMENT_VALIDATE_ARGUMENT_BUFFER_FUNCTION(return_type, name, __VA_ARGS__) \
    MU_FOR_EACH_2_KEEP_1(IMPLEMENT_VALIDATE_ARGUMENT_VALUE_BY_NAME_FUNCTION, name, __VA_ARGS__) \
    MU_FOR_EACH_2_KEEP_1(IMPLEMENT_VALIDATE_ARGUMENT_VALUE_AS_TYPE_BY_NAME_FUNCTION, name, __VA_ARGS__),) \
    MU_FOR_EACH_2_KEEP_1(IMPLEMENT_CAPTURE_ARGUMENT_VALUE_BY_NAME_FUNCTION, name, __VA_ARGS__) \
    IMPLEMENT_IGNORE_ALL_CALLS_FUNCTION(return_type, name, __VA_ARGS__) \
    IMPLEMENT_REGISTER_GLOBAL_MOCK_HOOK(return_type, name, __VA_ARGS__) \
    IMPLEMENT_REGISTER_GLOBAL_MOCK_RETURN(return_type, name, __VA_ARGS__) \
    IMPLEMENT_REGISTER_GLOBAL_MOCK_FAIL_RETURN(return_type, name, __VA_ARGS__) \
    IMPLEMENT_REGISTER_GLOBAL_MOCK_RETURNS(return_type, name, __VA_ARGS__) \
    IMPLEMENT_STRICT_EXPECTED_MOCK(return_type, name, __VA_ARGS__) \
    IMPLEMENT_EXPECTED_MOCK(return_type, name, __VA_ARGS__) \

/* Codes_SRS_UMOCK_C_LIB_01_193: [ When a destroy_call happens the memory block associated with the argument passed to it shall be freed. ] */
/* Codes_SRS_UMOCK_C_LIB_01_195: [ If any error occurs during the destroy_call related then umock_c shall raise an error with the code UMOCK_C_ERROR. ]*/
/* Codes_SRS_UMOCK_C_LIB_01_194: [ If the first argument passed to destroy_call is not found in the list of tracked handles (returned by create_call) then umock_c shall raise an error with the code UMOCK_C_INVALID_PAIRED_CALLS. ]*/
#define MOCKABLE_FUNCTION_BODY_WITHOUT_RETURN(modifiers, return_type, name, ...) \
    UMOCK_STATIC return_type modifiers name(MU_IF(MU_COUNT_ARG(__VA_ARGS__),,void) MU_FOR_EACH_2_COUNTED(ARG_IN_SIGNATURE, __VA_ARGS__)) \
    { \
        UMOCKCALL_HANDLE mock_call; \
        UMOCKCALL_HANDLE matched_call; \
        MU_IF(IS_NOT_VOID(return_type), \
        unsigned int result_value_set_C8417226_7442_49B4_BBB9_9CA816A21EB7 = 0; \
        unsigned int fail_result_value_set_C8417226_7442_49B4_BBB9_9CA816A21EB7 = 0; \
        void* captured_return_value_C8417226_7442_49B4_BBB9_9CA816A21EB7 = NULL;,) \
        MU_IF(IS_NOT_VOID(return_type),TRACK_CREATE_FUNC_TYPE track_create_destroy_pair_malloc_local_C8417226_7442_49B4_BBB9_9CA816A21EB7 = MU_C2(track_create_destroy_pair_malloc_,name); \
            PAIRED_HANDLES* used_paired_handles_local_C8417226_7442_49B4_BBB9_9CA816A21EB7 = MU_C2(used_paired_handles_,name); \
            const char* return_type_string_C8417226_7442_49B4_BBB9_9CA816A21EB7 = MU_TOSTRING(return_type); \
            (void)return_type_string_C8417226_7442_49B4_BBB9_9CA816A21EB7; \
            (void)track_create_destroy_pair_malloc_local_C8417226_7442_49B4_BBB9_9CA816A21EB7; \
            (void)used_paired_handles_local_C8417226_7442_49B4_BBB9_9CA816A21EB7;,) \
        MU_IF(IS_NOT_VOID(return_type),return_type result_C8417226_7442_49B4_BBB9_9CA816A21EB7 = MU_C2(get_mock_call_return_values_,name)()->success_value;,) \
        MU_C2(mock_call_,name)* matched_call_data; \
        MU_C2(mock_call_,name)* mock_call_data = (MU_C2(mock_call_,name)*)umockalloc_malloc(sizeof(MU_C2(mock_call_,name))); \
        MU_FOR_EACH_2(COPY_ARG_TO_MOCK_STRUCT, __VA_ARGS__) \
        MU_FOR_EACH_2(MARK_ARG_AS_NOT_IGNORED, __VA_ARGS__) \
        MU_FOR_EACH_2_COUNTED(CLEAR_OUT_ARG_BUFFERS, __VA_ARGS__) \
        MU_FOR_EACH_2_COUNTED(CLEAR_VALIDATE_ARG_BUFFERS, __VA_ARGS__) \
        MU_FOR_EACH_2(CLEAR_VALIDATE_ARG_VALUE, __VA_ARGS__) \
        MU_FOR_EACH_2(CLEAR_CAPTURE_ARG_VALUE, __VA_ARGS__) \
        MU_FOR_EACH_2(CLEAR_OVERRIDE_ARGUMENT_TYPE, __VA_ARGS__) \
        MU_IF(IS_NOT_VOID(return_type),mock_call_data->return_value_set = RETURN_VALUE_NOT_SET; \
        mock_call_data->captured_return_value = NULL; \
        mock_call_data->fail_return_value_set = FAIL_RETURN_VALUE_NOT_SET;,) \
        mock_call = umockcall_create(#name, mock_call_data, MU_C2(mock_call_data_copy_func_,name), MU_C2(mock_call_data_free_func_,name), MU_C2(mock_call_data_stringify_,name), MU_C2(mock_call_data_are_equal_,name)); \
        if (mock_call == NULL) \
        { \
            MU_IF(IS_NOT_VOID(return_type),COPY_RETURN_VALUE(return_type, name),) \
            UMOCK_LOG("Could not create a mock call in the actual call for %s.", MU_TOSTRING(name)); \
            umock_c_indicate_error(UMOCK_C_ERROR); \
        } \
        else \
        { \
            if (umock_c_add_actual_call(mock_call, &matched_call) != 0) \
            { \
                umockcall_destroy(mock_call); \
                UMOCK_LOG("Could not add an actual call for %s.", MU_TOSTRING(name)); \
                umock_c_indicate_error(UMOCK_C_COMPARE_CALL_ERROR); \
            } \
            if (matched_call != NULL) \
            { \
                matched_call_data = (MU_C2(mock_call_,name)*)umockcall_get_call_data(matched_call); \
                if (matched_call_data != NULL) \
                { \
                    MU_FOR_EACH_2(COPY_CAPTURED_ARGUMENT_VALUE, __VA_ARGS__) \
                } \
                MU_IF(IS_NOT_VOID(return_type),if (matched_call_data != NULL) \
                { \
                    captured_return_value_C8417226_7442_49B4_BBB9_9CA816A21EB7 = (void*)matched_call_data->captured_return_value; \
                    if (umockcall_get_fail_call(matched_call)) \
                    { \
                        if (matched_call_data->fail_return_value_set == FAIL_RETURN_VALUE_SET) \
                        { \
                            result_C8417226_7442_49B4_BBB9_9CA816A21EB7 = matched_call_data->fail_return_value; \
                        } \
                        else \
                        { \
                            result_C8417226_7442_49B4_BBB9_9CA816A21EB7 = MU_C2(get_mock_call_return_values_, name)()->failure_value; \
                        } \
                        result_value_set_C8417226_7442_49B4_BBB9_9CA816A21EB7 = 1; \
                        fail_result_value_set_C8417226_7442_49B4_BBB9_9CA816A21EB7 = 1; \
                    } \
                    else if (matched_call_data->return_value_set == RETURN_VALUE_SET) \
                    { \
                        result_C8417226_7442_49B4_BBB9_9CA816A21EB7 = matched_call_data->return_value; \
                        result_value_set_C8417226_7442_49B4_BBB9_9CA816A21EB7 = 1; \
                    } \
                    else \
                    { \
                        if (MU_C2(mock_hook_, name) != NULL) \
                        { \
                            MU_IF(IS_NOT_VOID(return_type),result_C8417226_7442_49B4_BBB9_9CA816A21EB7 =,) MU_C2(mock_hook_, name)(MU_FOR_EACH_2_COUNTED(ARG_NAME_ONLY_IN_CALL, __VA_ARGS__)); \
                            MU_IF(IS_NOT_VOID(return_type),result_value_set_C8417226_7442_49B4_BBB9_9CA816A21EB7 = 1;,) \
                        } \
                    } \
                }, \
                if (MU_C2(mock_hook_, name) != NULL) \
                { \
                    MU_C2(mock_hook_, name)(MU_FOR_EACH_2_COUNTED(ARG_NAME_ONLY_IN_CALL, __VA_ARGS__)); \
                } \
                ) \
                MU_IF(MU_COUNT_ARG(__VA_ARGS__), MU_FOR_EACH_2_COUNTED(COPY_OUT_ARG_VALUE_FROM_MATCHED_CALL, __VA_ARGS__),) \
            } \
            else \
            { \
                if (MU_C2(mock_hook_, name) != NULL) \
                { \
                    MU_IF(IS_NOT_VOID(return_type),result_C8417226_7442_49B4_BBB9_9CA816A21EB7 =,) MU_C2(mock_hook_, name)(MU_FOR_EACH_2_COUNTED(ARG_NAME_ONLY_IN_CALL, __VA_ARGS__)); \
                    MU_IF(IS_NOT_VOID(return_type),result_value_set_C8417226_7442_49B4_BBB9_9CA816A21EB7 = 1;,) \
                } \
            } \
        } \
        MU_IF(MU_COUNT_ARG(__VA_ARGS__), if (MU_C2(track_create_destroy_pair_free_, name) != NULL) \
        { \
            if (MU_C2(track_create_destroy_pair_free_, name)(MU_C2(used_paired_handles_, name), (void*)&ONLY_FIRST_ARG(__VA_ARGS__, 1)) != 0) \
            { \
                UMOCK_LOG("Could not track the destroy call for %s.", MU_TOSTRING(name)); \
                umock_c_indicate_error(UMOCK_C_ERROR); \
            } \
        },) \
        { \

#define UMOCK_GENERATE_DEFAULT_RETURNS(success_return_value, failure_return_value) \
            = { success_return_value, failure_return_value }; \
            if (!result->initialized) \
            { \
                result->success_value = temp.success_value; \
                result->failure_value = temp.failure_value; \
            } \
            return result; \
        } \

/* Codes_SRS_UMOCK_C_LIB_01_188: [ The create call shall have a non-void return type. ]*/
/* Codes_SRS_UMOCK_C_LIB_01_191: [ At each create_call a memory block shall be allocated so that it can be reported as a leak by any memory checker. ]*/
/* Codes_SRS_UMOCK_C_LIB_01_192: [ If any error occurs during the create_call related then umock_c shall raise an error with the code UMOCK_C_ERROR. ]*/
/* Codes_SRS_UMOCK_C_LIB_01_204: [ Tracking of paired calls shall not be done if the actual call to the create_call is using the SetFailReturn call modifier. ]*/
#define MOCKABLE_FUNCTION_UMOCK_INTERNAL_WITH_MOCK(do_returns, modifiers, return_type, name, ...) \
    MOCKABLE_FUNCTION_UMOCK_INTERNAL_WITH_MOCK_NO_CODE(do_returns, return_type, name, __VA_ARGS__) \
    MOCKABLE_FUNCTION_BODY_WITHOUT_RETURN(modifiers, return_type, name, __VA_ARGS__) \
            MU_IF(IS_NOT_VOID(return_type), \
            if (result_value_set_C8417226_7442_49B4_BBB9_9CA816A21EB7 == 0) \
            { \
                COPY_RETURN_VALUE(return_type, name) \
            }; \
            if (captured_return_value_C8417226_7442_49B4_BBB9_9CA816A21EB7 != NULL) \
            { \
                (void)memcpy(captured_return_value_C8417226_7442_49B4_BBB9_9CA816A21EB7, (void*)&result_C8417226_7442_49B4_BBB9_9CA816A21EB7, sizeof(result_C8417226_7442_49B4_BBB9_9CA816A21EB7)); \
            } \
            if ((track_create_destroy_pair_malloc_local_C8417226_7442_49B4_BBB9_9CA816A21EB7 != NULL) && (fail_result_value_set_C8417226_7442_49B4_BBB9_9CA816A21EB7 == 0)) \
            { \
                if (track_create_destroy_pair_malloc_local_C8417226_7442_49B4_BBB9_9CA816A21EB7(used_paired_handles_local_C8417226_7442_49B4_BBB9_9CA816A21EB7, (const void*)&result_C8417226_7442_49B4_BBB9_9CA816A21EB7, return_type_string_C8417226_7442_49B4_BBB9_9CA816A21EB7, sizeof(result_C8417226_7442_49B4_BBB9_9CA816A21EB7)) != 0) \
                { \
                    UMOCK_LOG("Could not track the create call for %s.", MU_TOSTRING(name)); \
                    umock_c_indicate_error(UMOCK_C_ERROR); \
                } \
            } \
            return result_C8417226_7442_49B4_BBB9_9CA816A21EB7;,) \
        } \
    } \
    MU_IF(IS_NOT_VOID(return_type), \
        MU_IF(do_returns, \
            static MU_C2(UMOCK_RETURNS_VALUES_STRUCT_, name)* MU_C2(get_mock_call_return_values_, name)(void) \
            { \
                MU_C2(UMOCK_RETURNS_VALUES_STRUCT_, name)* result = &MU_C2(mock_call_return_values_, name); \
                const MU_C2(UMOCK_RETURNS_VALUES_STRUCT_, name) temp \
                UMOCK_GENERATE_DEFAULT_RETURNS,) \
        ,) \

/* Codes_SRS_UMOCK_C_LIB_01_150: [ MOCK_FUNCTION_WITH_CODE shall define a mock function and allow the user to embed code between this define and a MOCK_FUNCTION_END call. ]*/
#define MOCK_FUNCTION_WITH_CODE(modifiers, return_type, name, ...) \
    MOCKABLE_FUNCTION_UMOCK_INTERNAL_WITH_MOCK_NO_CODE(0, return_type, name, __VA_ARGS__) \
    MOCKABLE_FUNCTION_BODY_WITHOUT_RETURN(modifiers, return_type, name, __VA_ARGS__) \

#define MOCKABLE_FUNCTION_INTERNAL_WITH_CODE(modifiers, return_type, name, ...) \
    MOCKABLE_FUNCTION_UMOCK_INTERNAL_WITH_MOCK_NO_CODE(0, return_type, name, __VA_ARGS__) \
    MOCKABLE_FUNCTION_BODY_WITHOUT_RETURN(modifiers, return_type, name, __VA_ARGS__) \
    MU_IF(IS_NOT_VOID(return_type), if (result_value_set_C8417226_7442_49B4_BBB9_9CA816A21EB7 == 0),) \
    { \

/* Codes_SRS_UMOCK_C_LIB_01_188: [ The create call shall have a non-void return type. ]*/
/* Codes_SRS_UMOCK_C_LIB_01_191: [ At each create_call a memory block shall be allocated so that it can be reported as a leak by any memory checker. ]*/
/* Codes_SRS_UMOCK_C_LIB_01_192: [ If any error occurs during the create_call related then umock_c shall raise an error with the code UMOCK_C_ERROR. ]*/
/* Codes_SRS_UMOCK_C_LIB_01_204: [ Tracking of paired calls shall not be done if the actual call to the create_call is using the SetFailReturn call modifier. ]*/
#define MOCK_FUNCTION_END(...) \
            MU_IF(MU_COUNT_ARG(__VA_ARGS__), if (result_value_set_C8417226_7442_49B4_BBB9_9CA816A21EB7 == 0) \
            { \
                result_C8417226_7442_49B4_BBB9_9CA816A21EB7 = __VA_ARGS__; \
            }; \
            if (captured_return_value_C8417226_7442_49B4_BBB9_9CA816A21EB7 != NULL) \
            { \
                (void)memcpy(captured_return_value_C8417226_7442_49B4_BBB9_9CA816A21EB7, (void*)&result_C8417226_7442_49B4_BBB9_9CA816A21EB7, sizeof(result_C8417226_7442_49B4_BBB9_9CA816A21EB7)); \
            } \
            if ((track_create_destroy_pair_malloc_local_C8417226_7442_49B4_BBB9_9CA816A21EB7 != NULL) && (fail_result_value_set_C8417226_7442_49B4_BBB9_9CA816A21EB7 == 0)) \
            { \
                if (track_create_destroy_pair_malloc_local_C8417226_7442_49B4_BBB9_9CA816A21EB7(used_paired_handles_local_C8417226_7442_49B4_BBB9_9CA816A21EB7, (const void*)&result_C8417226_7442_49B4_BBB9_9CA816A21EB7, return_type_string_C8417226_7442_49B4_BBB9_9CA816A21EB7, sizeof(result_C8417226_7442_49B4_BBB9_9CA816A21EB7)) != 0) \
                { \
                    UMOCK_LOG("Could not track the create call for %s.", MU_TOSTRING(name)); \
                    umock_c_indicate_error(UMOCK_C_ERROR); \
                } \
            } \
            return result_C8417226_7442_49B4_BBB9_9CA816A21EB7;,) \
        } \
    }

#define MOCKABLE_FUNCTION_END(...) \
            } \
            MOCK_FUNCTION_END(__VA_ARGS__)

/* Codes_SRS_UMOCK_C_LIB_01_187: [ REGISTER_UMOCKC_PAIRED_CREATE_DESTROY_CALLS shall register with umock two calls that are expected to be paired. ]*/
/* Codes_SRS_UMOCK_C_LIB_01_190: [ If create_call or destroy_call do not obey these rules, at the time of calling REGISTER_UMOCKC_PAIRED_CREATE_DESTROY_CALLS umock_c shall raise an error with the code UMOCK_C_INVALID_PAIRED_CALLS. ]*/
/* Codes_SRS_UMOCK_C_LIB_01_189: [ The destroy call shall take as argument at least one argument. The type of the first argument shall be of the same type as the return type for the create_call. ]*/
#define REGISTER_UMOCKC_PAIRED_CREATE_DESTROY_CALLS(create_call, destroy_call) \
    if ((strcmp(MU_C2(mock_call_metadata_,create_call).return_type, "void") == 0) || \
        (MU_C2(mock_call_metadata_,destroy_call).arg_count == 0) || \
        (strcmp(MU_C2(mock_call_metadata_,create_call).return_type, MU_C2(mock_call_metadata_, destroy_call).args[0].type) != 0)) \
    { \
        umock_c_indicate_error(UMOCK_C_INVALID_PAIRED_CALLS); \
    } \
    else \
    { \
        MU_C2(track_create_destroy_pair_malloc_, create_call) = umockcallpairs_track_create_paired_call; \
        MU_C2(track_create_destroy_pair_free_, destroy_call) = umockcallpairs_track_destroy_paired_call; \
        MU_C2(used_paired_handles_, create_call) = &MU_C2(paired_handles_, create_call); \
        MU_C2(used_paired_handles_, destroy_call) = &MU_C2(paired_handles_, create_call); \
    }

/* Codes_SRS_UMOCK_C_LIB_01_219: [ REGISTER_GLOBAL_INTERFACE_HOOKS shall register as mock hooks the real functions for all the functions in a mockable interface. ]*/
#define REGISTER_GLOBAL_INTERFACE_HOOKS(interface_name) \
    MU_C2(register_reals_, interface_name)()

/* Codes_SRS_UMOCK_C_LIB_01_220: [ UMOCK_REAL shall produce the name of the real function generated by umock. ]*/
#define UMOCK_REAL(function_name) \
    MU_C2(real_, function_name)

#ifdef __cplusplus
    }
#endif

#endif /* UMOCK_C_INTERNAL_H */
