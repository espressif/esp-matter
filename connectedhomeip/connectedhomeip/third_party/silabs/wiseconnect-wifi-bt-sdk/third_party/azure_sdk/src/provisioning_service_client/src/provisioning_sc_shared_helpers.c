// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <stdbool.h>

#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/crt_abstractions.h"

#include "prov_service_client/provisioning_sc_shared_helpers.h"
#include "parson.h"

int mallocAndStrcpy_overwrite(char** dest, const char* source)
{
    int result = 0;
    char* temp = NULL;

    if (dest == NULL || source == NULL)
    {
        LogError("Invalid input");
        result = MU_FAILURE;
    }
    else if (mallocAndStrcpy_s(&temp, source) != 0)
    {
        LogError("Failed to copy value from source");
        result = MU_FAILURE;
    }
    else
    {
        free(*dest);
        *dest = temp;
    }

    return result;
}

int copy_json_string_field(char** dest, JSON_Object* root_object, const char* json_key)
{
    int result = 0;

    const char* string = json_object_get_string(root_object, json_key);
    if (string != NULL)
    {
        if (mallocAndStrcpy_s(dest, string) != 0)
        {
            result = MU_FAILURE;
        }
    }

    return result;
}

int json_serialize_and_set_struct(JSON_Object* root_object, const char* json_key, void* structure, TO_JSON_FUNCTION toJson, bool is_required)
{
    int result;

    if (!is_required && structure == NULL)
    {
        result = 0;
    }
    else
    {
        JSON_Value* struct_val;
        if (structure == NULL)
        {
            LogError("NULL structure");
            result = MU_FAILURE;
        }
        else if ((struct_val = toJson(structure)) == NULL)
        {
            LogError("Failed converting structure to JSON Value");
            result = MU_FAILURE;
        }
        else if (json_object_set_value(root_object, json_key, struct_val) != JSONSuccess)
        {
            LogError("Failed to set JSON Value in JSON Object");
            result = MU_FAILURE;
        }
        else
        {
            result = 0;
        }
    }

    return result;
}

int json_deserialize_and_get_struct(void** dest, JSON_Object* root_object, const char* json_key, FROM_JSON_FUNCTION fromJson, bool is_required)
{
    int result;

    JSON_Object* struct_object = json_object_get_object(root_object, json_key);
    if (!is_required && struct_object == NULL)
    {
        result = 0;
    }
    else if (is_required && struct_object == NULL)
    {
        LogError("object required");
        result = MU_FAILURE;
    }
    else
    {
        if ((*dest = fromJson(struct_object)) == NULL)
        {
            LogError("Failed to deserialize from JSON");
            result = MU_FAILURE;
        }
        else
        {
            result = 0;
        }
    }

    return result;
}

static JSON_Value* struct_array_toJson(void* arr[], size_t len, TO_JSON_FUNCTION toJson)
{
    JSON_Value* json_array_val = NULL;
    JSON_Array* json_array = NULL;

    if ((json_array_val = json_value_init_array()) == NULL)
    {
        LogError("json_value_init_array failed");
    }
    else if ((json_array = json_value_get_array(json_array_val)) == NULL)
    {
        LogError("json_value_get_array failed");
        json_value_free(json_array_val);
        json_array_val = NULL;
    }
    else
    {
        for (size_t i = 0; i < len; i++)
        {
            JSON_Value* model;
            if ((model = toJson(arr[i])) == NULL)
            {
                LogError("Failed to deserialize model at index %lu", (unsigned long)i);
                json_value_free(json_array_val);
                json_array_val = NULL;
                break;
            }
            else if (json_array_append_value(json_array, model) != JSONSuccess)
            {
                LogError("Unable to append model to JSON array");
                json_value_free(json_array_val);
                json_array_val = NULL;
                break;
            }
        }
    }

    return json_array_val;
}

static void free_struct_arr(void* arr[], size_t len)
{
    if (len > 0)
    {
        for (size_t i = 0; i < len; i++)
        {
            free(arr[i]);
        }
    }
    free(arr);
}

void** struct_array_fromJson(JSON_Array* json_arr, size_t len, FROM_JSON_FUNCTION fromJson)
{
    void** struct_arr = NULL;

    if (len > 0)
    {
        struct_arr = malloc(len * sizeof(void*));
        if (struct_arr == NULL)
        {
            LogError("Failed to allocate memory for struct array");
        }
        else
        {
            JSON_Object* element_obj;
            for (size_t i = 0; i < len; i++)
            {
                if ((element_obj = json_array_get_object(json_arr, i)) == NULL)
                {
                    LogError("Failed to retrieve object at index %lu from JSON Array", (unsigned long)i);
                    free_struct_arr(struct_arr, i);
                    struct_arr = NULL;
                    break;
                }
                else if ((struct_arr[i] = fromJson(element_obj)) == NULL)
                {
                    LogError("Failed to deserialize object at index %lu from JSON Array", (unsigned long)i);
                    free_struct_arr(struct_arr, i);
                    struct_arr = NULL;
                    break;
                }
            }
        }
    }

    return struct_arr;
}

int json_serialize_and_set_struct_array(JSON_Object* root_object, const char* json_key, void* arr[], size_t len, TO_JSON_FUNCTION element_toJson)
{
    int result;

    JSON_Value* arr_val;
    if (arr== NULL)
    {
        LogError("NULL structure");
        result = MU_FAILURE;
    }
    else if ((arr_val = struct_array_toJson(arr, len, element_toJson)) == NULL)
    {
        LogError("Failed converting structure to JSON Value");
        result = MU_FAILURE;
    }
    else if (json_object_set_value(root_object, json_key, arr_val) != JSONSuccess)
    {
        LogError("Failed to set JSON Value in JSON Object");
        result = MU_FAILURE;
    }
    else
    {
        result = 0;
    }

    return result;
}

int json_deserialize_and_get_struct_array(void*** dest_arr, size_t* dest_len, JSON_Object* root_object, const char* json_key, FROM_JSON_FUNCTION element_fromJson)
{
    int result = 0;

    if (dest_arr == NULL || dest_len == NULL)
    {
        LogError("NULL pointer given");
        result = MU_FAILURE;
    }
    else
    {
        JSON_Array* json_arr;
        if ((json_arr = json_object_get_array(root_object, json_key)) == NULL)
        {
            LogError("Can't get JSON array");
            result = MU_FAILURE;
        }
        else if ((*dest_len = json_array_get_count(json_arr)) > 0)
        {
            if ((*dest_arr = struct_array_fromJson(json_arr, *dest_len, element_fromJson)) == NULL)
            {
                LogError("Failed to deserialize from JSON");
                result = MU_FAILURE;
            }
        }
    }

    return result;
}
