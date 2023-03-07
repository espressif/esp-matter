// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>

#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/crt_abstractions.h"

#include "prov_service_client/provisioning_sc_query.h"
#include "prov_service_client/provisioning_sc_json_const.h"
#include "prov_service_client/provisioning_sc_shared_helpers.h"
#include "prov_service_client/provisioning_sc_models_serializer.h"
#include "parson.h"

void queryResponse_free(PROVISIONING_QUERY_RESPONSE* query_resp)
{
    size_t i;
    if (query_resp != NULL)
    {
        if (query_resp->response_arr_type == QUERY_TYPE_INDIVIDUAL_ENROLLMENT)
        {
            if (query_resp->response_arr.ie != NULL)
            {
                for (i = 0; i < query_resp->response_arr_size; i++)
                {
                    individualEnrollment_destroy(query_resp->response_arr.ie[i]);
                }
                free(query_resp->response_arr.ie);
            }
        }
        else if (query_resp->response_arr_type == QUERY_TYPE_ENROLLMENT_GROUP)
        {
            if (query_resp->response_arr.eg != NULL)
            {
                for (i = 0; i < query_resp->response_arr_size; i++)
                {
                    enrollmentGroup_destroy(query_resp->response_arr.eg[i]);
                }
                free(query_resp->response_arr.eg);
            }
        }
        else if (query_resp->response_arr_type == QUERY_TYPE_DEVICE_REGISTRATION_STATE)
        {
            if (query_resp->response_arr.drs != NULL)
            {
                for (i = 0; i < query_resp->response_arr_size; i++)
                {
                    deviceRegistrationState_destroy(query_resp->response_arr.drs[i]);
                }
                free(query_resp->response_arr.drs);
            }
        }
        free(query_resp);
    }
}

static JSON_Value* querySpecification_toJson(const PROVISIONING_QUERY_SPECIFICATION* query_spec)
{
    JSON_Value* root_value = NULL;
    JSON_Object* root_object = NULL;

    //Setup
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
    else if (json_object_set_string(root_object, QUERY_SPECIFICATION_JSON_KEY_QUERY, query_spec->query_string) != JSONSuccess)
    {
        LogError("Failed to set %s in JSON string", QUERY_SPECIFICATION_JSON_KEY_QUERY);
        json_value_free(root_value);
        root_value = NULL;
    }

    return root_value;
}

static int queryResponse_get_type_specific_deserialization_info(PROVISIONING_QUERY_RESPONSE* new_query_resp, void**** response_arr_ptr_ptr, FROM_JSON_FUNCTION* fromJson)
{
    int result;
    PROVISIONING_QUERY_TYPE type = new_query_resp->response_arr_type;

    if (type == QUERY_TYPE_INDIVIDUAL_ENROLLMENT)
    {
        *response_arr_ptr_ptr = (void***)&(new_query_resp->response_arr.ie);
        *fromJson = (FROM_JSON_FUNCTION)individualEnrollment_fromJson;
        result = 0;
    }
    else if (type == QUERY_TYPE_ENROLLMENT_GROUP)
    {
        *response_arr_ptr_ptr = (void***)&(new_query_resp->response_arr.eg);
        *fromJson = (FROM_JSON_FUNCTION)enrollmentGroup_fromJson;
        result = 0;
    }
    else if (type == QUERY_TYPE_DEVICE_REGISTRATION_STATE)
    {
        *response_arr_ptr_ptr = (void***)&(new_query_resp->response_arr.drs);
        *fromJson = (FROM_JSON_FUNCTION)deviceRegistrationState_fromJson;
        result = 0;
    }
    else
    {
        LogError("Unrecognized type");
        *response_arr_ptr_ptr = NULL;
        *fromJson = NULL;
        result = MU_FAILURE;
    }
    return result;
}

static PROVISIONING_QUERY_RESPONSE* queryResponse_fromJson(JSON_Array* root_array, PROVISIONING_QUERY_TYPE type)
{
    PROVISIONING_QUERY_RESPONSE* new_query_resp = NULL;

    if (root_array == NULL)
    {
        LogError("No Query Response in JSON");
    }
    else if ((new_query_resp = malloc(sizeof(PROVISIONING_QUERY_RESPONSE))) == NULL)
    {
        LogError("Allocation of Query Response failed");
    }
    else
    {
        memset(new_query_resp, 0, sizeof(PROVISIONING_QUERY_RESPONSE));
        new_query_resp->response_arr_type = type;
        new_query_resp->response_arr_size = json_array_get_count(root_array);

        void*** generic_response_arr_ptr;
        FROM_JSON_FUNCTION fromJson;

        if (queryResponse_get_type_specific_deserialization_info(new_query_resp, &generic_response_arr_ptr, &fromJson) != 0)
        {
            LogError("Unable to deserialize that array type");
            queryResponse_free(new_query_resp);
            new_query_resp = NULL;
        }
        else
        {
            if (new_query_resp->response_arr_size <= 0)
            {
                *generic_response_arr_ptr = NULL;
            }
            else
            {
                if ((*generic_response_arr_ptr = struct_array_fromJson(root_array, new_query_resp->response_arr_size, fromJson)) == NULL)
                {
                    LogError("Failed to deserialize array of query results");
                    queryResponse_free(new_query_resp);
                    new_query_resp = NULL;
                }
            }
        }
    }

    return new_query_resp;
}

char* querySpecification_serializeToJson(const PROVISIONING_QUERY_SPECIFICATION* query_spec)
{
    char* result = NULL;
    char* serialized_string = NULL;
    JSON_Value* root_value = NULL;

    if (query_spec == NULL)
    {
        LogError("Invalid query specification");
    }
    else if (query_spec->version != PROVISIONING_QUERY_SPECIFICATION_VERSION_1)
    {
        LogError("Invalid version");
    }
    else if ((root_value = querySpecification_toJson(query_spec)) == NULL)
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

PROVISIONING_QUERY_RESPONSE* queryResponse_deserializeFromJson(const char* json_string, PROVISIONING_QUERY_TYPE type)
{
     PROVISIONING_QUERY_RESPONSE* new_result = NULL;
     JSON_Value* root_value = NULL;
     JSON_Array* root_array = NULL;

     if (json_string == NULL)
     {
         LogError("Cannot deserialize NULL");
     }
     else if (type == QUERY_TYPE_INVALID)
     {
         LogError("Invalid query type");
     }
     else if ((root_value = json_parse_string(json_string)) == NULL)
     {
         LogError("Parsing JSON string failed");
     }
     else if ((root_array = json_value_get_array(root_value)) == NULL)
     {
         LogError("Creating JSON array failed");
         json_value_free(root_value);
     }
     else
     {
         if ((new_result = queryResponse_fromJson(root_array, type)) == NULL)
         {
             LogError("Creating new Query Response failed");
         }
         json_value_free(root_value); //implicitly frees root_array
         root_value = NULL;
     }

     return new_result;
}

PROVISIONING_QUERY_TYPE queryType_stringToEnum(const char* string)
{
    PROVISIONING_QUERY_TYPE result;

    if (string == NULL)
    {
        result = QUERY_TYPE_INVALID;
    }
    else if (strcmp(string, QUERY_RESPONSE_HEADER_ITEM_TYPE_VALUE_INDIVIDUAL_ENROLLMENT) == 0)
    {
        result = QUERY_TYPE_INDIVIDUAL_ENROLLMENT;
    }
    else if (strcmp(string, QUERY_RESPONSE_HEADER_ITEM_TYPE_VALUE_ENROLLMENT_GROUP) == 0)
    {
        result = QUERY_TYPE_ENROLLMENT_GROUP;
    }
    else if (strcmp(string, QUERY_RESPONSE_HEADER_ITEM_TYPE_VALUE_DEVICE_REGISTRATION_STATE) == 0)
    {
        result = QUERY_TYPE_DEVICE_REGISTRATION_STATE;
    }
    else
    {
        result = QUERY_TYPE_INVALID;
    }

    return result;
}
