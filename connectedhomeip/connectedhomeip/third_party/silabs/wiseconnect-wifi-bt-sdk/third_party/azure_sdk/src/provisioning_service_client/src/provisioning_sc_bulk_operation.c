// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>

#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/crt_abstractions.h"

#include "prov_service_client/provisioning_sc_bulk_operation.h"
#include "prov_service_client/provisioning_sc_models_serializer.h"
#include "prov_service_client/provisioning_sc_shared_helpers.h"
#include "prov_service_client/provisioning_sc_json_const.h"
#include "prov_service_client/provisioning_sc_enrollment.h"
#include "parson.h"

static const char* bulkOperation_mode_toString(PROVISIONING_BULK_OPERATION_MODE mode)
{
    const char* result;
    if (mode == BULK_OP_CREATE)
    {
        result = BULK_ENROLLMENT_OPERATION_MODE_JSON_VALUE_CREATE;
    }
    else if (mode == BULK_OP_UPDATE)
    {
        result = BULK_ENROLLMENT_OPERATION_MODE_JSON_VALUE_UPDATE;
    }
    else if (mode == BULK_OP_UPDATE_IF_MATCH_ETAG)
    {
        result = BULK_ENROLLMENT_OPERATION_MODE_JSON_VALUE_UPDATE_IF_MATCH_ETAG;
    }
    else if (mode == BULK_OP_DELETE)
    {
        result = BULK_ENROLLMENT_OPERATION_MODE_JSON_VALUE_DELETE;
    }
    else
    {
        result = NULL;
    }

    return result;
}

static JSON_Value* bulkOperation_toJson(const PROVISIONING_BULK_OPERATION* bulk_op)
{
    JSON_Value* root_value = NULL;
    JSON_Object* root_object = NULL;

    const char* mode_str = NULL;

    //setup
    if ((root_value = json_value_init_object()) == NULL)
    {
        LogError("json_value_init_object failed");
    }
    else if ((root_object = json_value_get_object(root_value)) == NULL)
    {
        LogError("json_value_get_object failed");
        json_value_free(root_value);
        root_value = NULL;
    }

    //Set data
    else if (((mode_str = bulkOperation_mode_toString(bulk_op->mode)) == NULL) || (json_object_set_string(root_object, BULK_ENROLLMENT_OPERATION_JSON_KEY_MODE, mode_str) != JSONSuccess))
    {
        LogError("Failed to set '%s' in JSON string", BULK_ENROLLMENT_OPERATION_JSON_KEY_MODE);
        json_value_free(root_value);
        root_value = NULL;
    }
    else
    {
        //in future, add logic here to decide which toJson function is used  depending on bulk_op->type
        TO_JSON_FUNCTION element_toJson = (TO_JSON_FUNCTION)individualEnrollment_toJson;

        if (json_serialize_and_set_struct_array(root_object, BULK_ENROLLMENT_OPERATION_JSON_KEY_ENROLLMENTS, (void**)bulk_op->enrollments.ie, bulk_op->num_enrollments, element_toJson) != 0)
        {
            LogError("Failed to set '%s' in JSON string", BULK_ENROLLMENT_OPERATION_JSON_KEY_ENROLLMENTS);
            json_value_free(root_value);
            root_value = NULL;
        }
    }

    return root_value;
}

static void bulkOperationError_free(PROVISIONING_BULK_OPERATION_ERROR* error)
{
    if (error != NULL)
    {
        free(error->registration_id);
        free(error->error_status);
        free(error);
    }
}

PROVISIONING_BULK_OPERATION_ERROR* bulkOperationError_fromJson(JSON_Object* root_object)
{
    PROVISIONING_BULK_OPERATION_ERROR* new_error = NULL;

    if (root_object == NULL)
    {
        LogError("No error in JSON");
    }
    else if ((new_error = malloc(sizeof(PROVISIONING_BULK_OPERATION_ERROR))) == NULL)
    {
        LogError("Allocation of Bulk Operation Error failed");
    }
    else
    {
        memset(new_error, 0, sizeof(PROVISIONING_BULK_OPERATION_ERROR));

        if (copy_json_string_field(&(new_error->registration_id), root_object, BULK_ENROLLMENT_OPERATION_ERROR_JSON_KEY_REG_ID) != 0)
        {
            LogError("Failed to set '%s' in Bulk Operation Error", BULK_ENROLLMENT_OPERATION_ERROR_JSON_KEY_REG_ID);
            bulkOperationError_free(new_error);
            new_error = NULL;
        }
        else if (copy_json_string_field(&(new_error->error_status), root_object, BULK_ENROLLMENT_OPERATION_ERROR_JSON_KEY_ERROR_STATUS) != 0)
        {
            LogError("Failed to set '%s' in Bulk Operation Error", BULK_ENROLLMENT_OPERATION_ERROR_JSON_KEY_ERROR_STATUS);
            bulkOperationError_free(new_error);
            new_error = NULL;
        }
        else
        {
            new_error->error_code = (int32_t)json_object_get_number(root_object, BULK_ENROLLMENT_OPERATION_ERROR_JSON_KEY_ERROR_CODE);
        }
    }

    return new_error;
}

static PROVISIONING_BULK_OPERATION_RESULT* bulkOperationResult_fromJson(JSON_Object* root_object)
{
    PROVISIONING_BULK_OPERATION_RESULT* new_result = NULL;

    if (root_object == NULL)
    {
        LogError("No error in JSON");
    }
    else if ((new_result = malloc(sizeof(PROVISIONING_BULK_OPERATION_RESULT))) == NULL)
    {
        LogError("Allocation of Bulk Operation Error failed");
    }
    else
    {
        memset(new_result, 0, sizeof(PROVISIONING_BULK_OPERATION_RESULT));
        int bool_res;

        if ((bool_res = json_object_get_boolean(root_object, BULK_ENROLLMENT_OPERATION_RESULT_JSON_KEY_IS_SUCCESSFUL)) == -1)
        {
            LogError("Failed to set '%s' in Bulk Operation Result", BULK_ENROLLMENT_OPERATION_ERROR_JSON_KEY_REG_ID);
            bulkOperationResult_free(new_result);
            new_result = NULL;
        }
        else if (json_deserialize_and_get_struct_array((void***)&(new_result->errors), &(new_result->num_errors), root_object, BULK_ENROLLMENT_OPERATION_RESULT_JSON_KEY_ERRORS, (FROM_JSON_FUNCTION)bulkOperationError_fromJson) != 0)
        {
            LogError("Failed to deserialize Bulk Operation Errors");
            bulkOperationResult_free(new_result);
            new_result = NULL;
        }
        else
        {
            new_result->is_successful = (bool)bool_res;
        }
    }

    return new_result;
}

char* bulkOperation_serializeToJson(const PROVISIONING_BULK_OPERATION* bulk_op)
{
    char* result = NULL;
    char* serialized_string = NULL;
    JSON_Value* root_value = NULL;

    if (bulk_op == NULL || bulk_op->num_enrollments < 1 || bulk_op->enrollments.ie == NULL)
    {
        LogError("Invalid bulk operation");
    }
    else if (bulk_op->version != PROVISIONING_BULK_OPERATION_VERSION_1)
    {
        LogError("Invalid Version");
    }
    else if ((root_value = bulkOperation_toJson(bulk_op)) == NULL)
    {
        LogError("Creating json object failed");
    }
    else if ((serialized_string = json_serialize_to_string(root_value)) == NULL)
    {
        LogError("Failed to serialize to JSON");
    }
    else if (mallocAndStrcpy_s(&result, serialized_string) != 0)
    {
        LogError("Failed to copy serialized string");
    }

    if (root_value != NULL)
    {
        json_value_free(root_value);
        root_value = NULL;
    }
    if (serialized_string != NULL)
    {
        json_free_serialized_string(serialized_string);
        serialized_string = NULL;
    }

    return result;
}

PROVISIONING_BULK_OPERATION_RESULT* bulkOperationResult_deserializeFromJson(const char* json_string)
{
    PROVISIONING_BULK_OPERATION_RESULT* new_result = NULL;
    JSON_Value* root_value = NULL;
    JSON_Object* root_object = NULL;

    if (json_string == NULL)
    {
        LogError("Cannot deserialize NULL");
    }
    else if ((root_value = json_parse_string(json_string)) == NULL)
    {
        LogError("Parsing JSON string failed");
    }
    else if ((root_object = json_value_get_object(root_value)) == NULL)
    {
        LogError("Creating JSON object failed");
    }
    else
    {
        if ((new_result = bulkOperationResult_fromJson(root_object)) == NULL)
        {
            LogError("Creating new Bulk Operation Result failed");
        }
        json_value_free(root_value); //implicitly frees root_object
        root_value = NULL;
    }

    return new_result;
}

void bulkOperationResult_free(PROVISIONING_BULK_OPERATION_RESULT* bulk_op_result)
{
    if (bulk_op_result != NULL)
    {
        if (bulk_op_result->num_errors > 0)
        {
            for (size_t i = 0; i < bulk_op_result->num_errors; i++)
            {
                bulkOperationError_free(bulk_op_result->errors[i]);
            }
            free(bulk_op_result->errors);
        }
        free(bulk_op_result);
    }
}
