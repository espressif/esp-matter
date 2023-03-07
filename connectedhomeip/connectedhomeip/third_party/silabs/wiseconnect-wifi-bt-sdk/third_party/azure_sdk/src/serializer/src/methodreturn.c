// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <limits.h>

#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/strings.h"
#include "parson.h"

#include "methodreturn.h"

typedef struct METHODRETURN_HANDLE_DATA_TAG
{
    METHODRETURN_DATA data;
}METHODRETURN_HANDLE_DATA;

bool is_json_present_and_unparsable(const char* jsonValue)
{
    bool is_present_and_unparsable;
    if (jsonValue == NULL)
    {
        // Null json is not considered invalid here
        is_present_and_unparsable = false;
    }
    else
    {
        JSON_Value* temp = json_parse_string(jsonValue);
        if (temp == NULL)
        {
            is_present_and_unparsable = true;
        }
        else
        {
            json_value_free(temp);
            is_present_and_unparsable = false;
        }
    }
    return is_present_and_unparsable;
}

METHODRETURN_HANDLE MethodReturn_Create(int statusCode, const char* jsonValue)
{
    METHODRETURN_HANDLE result;
    /*Codes_SRS_METHODRETURN_02_009: [ If jsonValue is not a JSON value then MethodReturn_Create shall fail and return NULL. ]*/
    if (is_json_present_and_unparsable(jsonValue))
    {
        LogError("%s is not JSON", jsonValue);
        result = NULL;
    }
    else
    {
        result = (METHODRETURN_HANDLE_DATA*)calloc(1, sizeof(METHODRETURN_HANDLE_DATA));
        if (result == NULL)
        {
            /*Codes_SRS_METHODRETURN_02_002: [ If any failure is encountered then MethodReturn_Create shall return NULL ]*/
            LogError("unable to malloc");
            /*return as is*/
        }
        else
        {
            if (jsonValue == NULL)
            {
                /*Codes_SRS_METHODRETURN_02_001: [ MethodReturn_Create shall create a non-NULL handle containing statusCode and a clone of jsonValue. ]*/
                result->data.jsonValue = NULL;
                result->data.statusCode = statusCode;
            }
            else
            {
                if (mallocAndStrcpy_s(&(result->data.jsonValue), jsonValue) != 0)
                {
                    LogError("failure in mallocAndStrcpy_s");
                    free(result);
                    result = NULL;
                }
                else
                {
                    /*Codes_SRS_METHODRETURN_02_001: [ MethodReturn_Create shall create a non-NULL handle containing statusCode and a clone of jsonValue. ]*/
                    result->data.statusCode = statusCode;
                }
            }
        }
    }

    return result;
}

void MethodReturn_Destroy(METHODRETURN_HANDLE handle)
{
    if (handle == NULL)
    {
        /*Codes_SRS_METHODRETURN_02_003: [ If handle is NULL then MethodReturn_Destroy shall return. ]*/
        LogError("invalid argument METHODRETURN_HANDLE handle=%p", handle);
    }
    else
    {
        /*Codes_SRS_METHODRETURN_02_004: [ Otherwise, MethodReturn_Destroy shall free all used resources by handle. ]*/
        if (handle->data.jsonValue != NULL)
        {
            free(handle->data.jsonValue);
        }
        free(handle);
    }
}


const METHODRETURN_DATA* MethodReturn_GetReturn(METHODRETURN_HANDLE handle)
{
    const METHODRETURN_DATA* result;
    if (handle == NULL)
    {
        /*Codes_SRS_METHODRETURN_02_010: [ If handle is NULL then MethodReturn_GetReturn shall fail and return NULL. ]*/
        result = NULL;
    }
    else
    {
        /*Codes_SRS_METHODRETURN_02_011: [ Otherwise, MethodReturn_GetReturn shall return a non-NULL const pointer to a METHODRETURN_DATA. ]*/
        result = &(handle->data);
    }
    return result;
}
