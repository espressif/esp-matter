// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef PROVISIONING_SC_SHARED_HELPERS_H
#define PROVISIONING_SC_SHARED_HELPERS_H

#include "azure_macro_utils/macro_utils.h"
#include "umock_c/umock_c_prod.h"
#include "parson.h"

#ifdef __cplusplus
extern "C" {
    #include <cstdlib>
#else
    #include <stdlib.h>
    #include <stdbool.h>
#endif /* __cplusplus */

typedef JSON_Value*(*TO_JSON_FUNCTION)(void* handle);
typedef void*(*FROM_JSON_FUNCTION)(JSON_Object* root_object);

MOCKABLE_FUNCTION(, int, mallocAndStrcpy_overwrite, char**, dest, const char*, source);
MOCKABLE_FUNCTION(, int, copy_json_string_field, char**, dest, JSON_Object*, root_object, const char*, json_key);
MOCKABLE_FUNCTION(, int, json_serialize_and_set_struct, JSON_Object*, root_object, const char*, json_key, void*, structure, TO_JSON_FUNCTION, toJson, bool, is_required);
MOCKABLE_FUNCTION(, int, json_deserialize_and_get_struct, void**, dest, JSON_Object*, root_object, const char*, json_key, FROM_JSON_FUNCTION, fromJson, bool, is_required);
MOCKABLE_FUNCTION(, int, json_serialize_and_set_struct_array, JSON_Object*, root_object, const char*, json_key, void**, arr, size_t, len, TO_JSON_FUNCTION, element_toJson);
MOCKABLE_FUNCTION(, int, json_deserialize_and_get_struct_array, void***, dest_arr, size_t*, dest_len, JSON_Object*, root_object, const char*, json_key, FROM_JSON_FUNCTION, element_fromJson)
MOCKABLE_FUNCTION(, void**, struct_array_fromJson, JSON_Array*, json_arr, size_t, len, FROM_JSON_FUNCTION, fromJson);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* PROVISIONING_SC_SHARED_HELPERS_H */