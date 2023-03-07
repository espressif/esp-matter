// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef JSONENCODER_H
#define JSONENCODER_H

#include "azure_macro_utils/macro_utils.h"
#include "azure_c_shared_utility/strings.h"

#ifdef __cplusplus
#include "cstddef"
extern "C" {
#else
#include "stddef.h"
#endif

#include "multitree.h"

#define JSON_ENCODER_RESULT_VALUES           \
JSON_ENCODER_OK,                             \
JSON_ENCODER_INVALID_ARG,                    \
JSON_ENCODER_ALREADY_EXISTS,                 \
JSON_ENCODER_MULTITREE_ERROR,                \
JSON_ENCODER_TOSTRING_FUNCTION_ERROR,        \
JSON_ENCODER_ERROR

MU_DEFINE_ENUM_WITHOUT_INVALID(JSON_ENCODER_RESULT, JSON_ENCODER_RESULT_VALUES);

#define JSON_ENCODER_TOSTRING_RESULT_VALUES  \
JSON_ENCODER_TOSTRING_OK,                    \
JSON_ENCODER_TOSTRING_INVALID_ARG,           \
JSON_ENCODER_TOSTRING_ERROR

MU_DEFINE_ENUM_WITHOUT_INVALID(JSON_ENCODER_TOSTRING_RESULT, JSON_ENCODER_TOSTRING_RESULT_VALUES);

typedef JSON_ENCODER_TOSTRING_RESULT(*JSON_ENCODER_TOSTRING_FUNC)(STRING_HANDLE, const void* value);

#include "umock_c/umock_c_prod.h"

MOCKABLE_FUNCTION(, JSON_ENCODER_TOSTRING_RESULT, JSONEncoder_CharPtr_ToString, STRING_HANDLE, destination, const void*, value);
MOCKABLE_FUNCTION(, JSON_ENCODER_RESULT, JSONEncoder_EncodeTree, MULTITREE_HANDLE, treeHandle, STRING_HANDLE, destination, JSON_ENCODER_TOSTRING_FUNC, toStringFunc);

#ifdef __cplusplus
}
#endif


#endif /* JSONENCODER_H */
