// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <time.h>
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/agenttime.h"
#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/buffer_.h"
#include "azure_c_shared_utility/sastoken.h"
#include "azure_c_shared_utility/httpheaders.h"
#include "azure_c_shared_utility/httpapiex.h"
#include "azure_c_shared_utility/httpapiexsas.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/crt_abstractions.h"

#define SHARED_ACCESS_SIGNATURE_PREFIX "sas="

typedef struct HTTPAPIEX_SAS_STATE_TAG
{
    char* key;
    char* uriResource;
    char* keyName;
} HTTPAPIEX_SAS_STATE;

static HTTPAPIEX_SAS_STATE* construct_httpex_sas(const char* key, const char* uriResource, const char* keyName)
{
    HTTPAPIEX_SAS_STATE* result;

    result = (HTTPAPIEX_SAS_STATE*)malloc(sizeof(HTTPAPIEX_SAS_STATE));
    if (result == NULL)
    {
        LogError("Failure allocating HTTPAPIEX_SAS_Create.");
    }
    else
    {
        (void)memset(result, 0, sizeof(HTTPAPIEX_SAS_STATE));
        if (mallocAndStrcpy_s(&result->key, key) != 0)
        {
            /*Codes_SRS_HTTPAPIEXSAS_06_004: [If there are any other errors in the instantiation of this handle then HTTPAPIEX_SAS_Create shall return NULL.]*/
            LogError("Failure allocating sas key.");
            HTTPAPIEX_SAS_Destroy(result);
            result = NULL;
        }
        else if (mallocAndStrcpy_s(&result->uriResource, uriResource) != 0)
        {
            /*Codes_SRS_HTTPAPIEXSAS_06_004: [If there are any other errors in the instantiation of this handle then HTTPAPIEX_SAS_Create shall return NULL.]*/
            LogError("Failure allocating sas uriResource.");
            HTTPAPIEX_SAS_Destroy(result);
            result = NULL;
        }
        else if (keyName != NULL && mallocAndStrcpy_s(&result->keyName, keyName) != 0)
        {
            /*Codes_SRS_HTTPAPIEXSAS_06_004: [If there are any other errors in the instantiation of this handle then HTTPAPIEX_SAS_Create shall return NULL.]*/
            LogError("Failure allocating sas keyName.");
            HTTPAPIEX_SAS_Destroy(result);
            result = NULL;
        }
    }
    return result;
}

HTTPAPIEX_SAS_HANDLE HTTPAPIEX_SAS_Create_From_String(const char* key, const char* uriResource, const char* keyName)
{
    HTTPAPIEX_SAS_HANDLE result = NULL;
    if (key == NULL || uriResource == NULL)
    {
        /* Codes_SRS_HTTPAPIEXSAS_07_001: [ If the parameter key or uriResource is NULL then HTTPAPIEX_SAS_Create_From_String shall return NULL. ] */
        LogError("Invalid parameter key: %p, uriResource: %p", key, uriResource);
        result = NULL;
    }
    else
    {
        /* Codes_SRS_HTTPAPIEXSAS_07_002: [ If there are any other errors in the instantiation of this handle then HTTPAPIEX_SAS_Create_From_String shall return NULL. ] */
        result = construct_httpex_sas(key, uriResource, keyName);
    }
    /* Codes_SRS_HTTPAPIEXSAS_07_003: [ HTTPAPIEX_SAS_Create_From_String shall create a new instance of HTTPAPIEX_SAS and return a non-NULL handle to it ] */
    return result;
}

HTTPAPIEX_SAS_HANDLE HTTPAPIEX_SAS_Create(STRING_HANDLE key, STRING_HANDLE uriResource, STRING_HANDLE keyName)
{
    HTTPAPIEX_SAS_HANDLE result = NULL;
    if (key == NULL)
    {
        /*Codes_SRS_HTTPAPIEXSAS_06_001: [If the parameter key is NULL then HTTPAPIEX_SAS_Create shall return NULL.]*/
        LogError("No key passed to HTTPAPIEX_SAS_Create.");
    }
    else if (uriResource == NULL)
    {
        /*Codes_SRS_HTTPAPIEXSAS_06_002: [If the parameter uriResource is NULL then HTTPAPIEX_SAS_Create shall return NULL.]*/
        LogError("No uri resource passed to HTTPAPIEX_SAS_Create.");
    }
    else
    {
        /*Codes_SRS_HTTPAPIEXSAS_06_003: [The parameter keyName for HTTPAPIEX_SAS_Create is optional and can be NULL.]*/
        /*Codes_SRS_HTTPAPIEXSAS_01_001: [ HTTPAPIEX_SAS_Create shall create a new instance of HTTPAPIEX_SAS and return a non-NULL handle to it. ]*/
        const char* keyStr = STRING_c_str(key);
        const char* uriStr = STRING_c_str(uriResource);
        const char* keyNameStr = STRING_c_str(keyName);
        result = construct_httpex_sas(keyStr, uriStr, keyNameStr);
    }
    return result;
}

void HTTPAPIEX_SAS_Destroy(HTTPAPIEX_SAS_HANDLE handle)
{
    /*Codes_SRS_HTTPAPIEXSAS_06_005: [If the parameter handle is NULL then HTTAPIEX_SAS_Destroy shall do nothing and return.]*/
    HTTPAPIEX_SAS_STATE* state = (HTTPAPIEX_SAS_STATE*)handle;
    if (state)
    {
        /*Codes_SRS_HTTPAPIEXSAS_06_006: [HTTAPIEX_SAS_Destroy shall deallocate any structures denoted by the parameter handle.]*/
        if (state->key)
        {
            free(state->key);
        }
        if (state->uriResource)
        {
            free(state->uriResource);
        }
        if (state->keyName)
        {
            free(state->keyName);
        }
        free(state);
    }
}

HTTPAPIEX_RESULT HTTPAPIEX_SAS_ExecuteRequest(HTTPAPIEX_SAS_HANDLE sasHandle, HTTPAPIEX_HANDLE handle, HTTPAPI_REQUEST_TYPE requestType, const char* relativePath, HTTP_HEADERS_HANDLE requestHttpHeadersHandle, BUFFER_HANDLE requestContent, unsigned int* statusCode, HTTP_HEADERS_HANDLE responseHeadersHandle, BUFFER_HANDLE responseContent)
{
    /*Codes_SRS_HTTPAPIEXSAS_06_007: [If the parameter sasHandle is NULL then HTTPAPIEX_SAS_ExecuteRequest shall simply invoke HTTPAPIEX_ExecuteRequest with the remaining parameters (following sasHandle) as its arguments and shall return immediately with the result of that call as the result of HTTPAPIEX_SAS_ExecuteRequest.]*/
    if (sasHandle != NULL)
    {
        /*Codes_SRS_HTTPAPIEXSAS_06_008: [if the parameter requestHttpHeadersHandle is NULL then fallthrough.]*/
        if (requestHttpHeadersHandle != NULL)
        {
            /*Codes_SRS_HTTPAPIEXSAS_06_009: [HTTPHeaders_FindHeaderValue shall be invoked with the requestHttpHeadersHandle as its first argument and the string "Authorization" as its second argument.]*/
            /*Codes_SRS_HTTPAPIEXSAS_06_010: [If the return result of the invocation of HTTPHeaders_FindHeaderValue is NULL then fallthrough.]*/
            if (HTTPHeaders_FindHeaderValue(requestHttpHeadersHandle, "Authorization") != NULL)
            {
                HTTPAPIEX_SAS_STATE* state = (HTTPAPIEX_SAS_STATE*)sasHandle;
                /*Codes_SRS_HTTPAPIEXSAS_06_018: [A value of type time_t that shall be known as currentTime is obtained from calling get_time.]*/
                time_t currentTime = get_time(NULL);
                /*Codes_SRS_HTTPAPIEXSAS_06_019: [If the value of currentTime is (time_t)-1 is then fallthrough.]*/
                if (currentTime == (time_t)-1)
                {
                    LogError("Time does not appear to be working.");
                }
                else
                {
                    STRING_HANDLE newSASToken = NULL;
                    if (strncmp(state->key, SHARED_ACCESS_SIGNATURE_PREFIX, 4) == 0)
                    {
                        /*Codes_SRS_HTTPAPIEXSAS_06_017: [If state->key is prefixed with "sas=", SharedAccessSignature will be used rather than created.  STRING_construct will be invoked.]*/
                        newSASToken = STRING_construct(&state->key[4]);
                    }
                    else
                    {
                        /*Codes_SRS_HTTPAPIEXSAS_06_011: [SASToken_Create shall be invoked.]*/
                        /*Codes_SRS_HTTPAPIEXSAS_06_012: [If the return result of SASToken_Create is NULL then fallthrough.]*/
                        uint64_t expiry = (uint64_t)(difftime(currentTime, 0) + 3600);
                        newSASToken = SASToken_CreateString(state->key, state->uriResource, state->keyName, expiry);
                    }

                    if (newSASToken != NULL)
                    {
                        /*Codes_SRS_HTTPAPIEXSAS_06_013: [HTTPHeaders_ReplaceHeaderNameValuePair shall be invoked with "Authorization" as its second argument and STRING_c_str (newSASToken) as its third argument.]*/
                        if (HTTPHeaders_ReplaceHeaderNameValuePair(requestHttpHeadersHandle, "Authorization", STRING_c_str(newSASToken)) != HTTP_HEADERS_OK)
                        {
                            /*Codes_SRS_HTTPAPIEXSAS_06_014: [If the result of the invocation of HTTPHeaders_ReplaceHeaderNameValuePair is NOT HTTP_HEADERS_OK then fallthrough.]*/
                            LogError("Unable to replace the old SAS Token.");
                        }
                        /*Codes_SRS_HTTPAPIEXSAS_06_015: [STRING_delete(newSASToken) will be invoked.]*/
                        STRING_delete(newSASToken);
                    }
                    else
                    {
                        LogError("Unable to create a new SAS token.");
                    }
                }
            }
        }
    }
    /*Codes_SRS_HTTPAPIEXSAS_06_016: [HTTPAPIEX_ExecuteRequest with the remaining parameters (following sasHandle) as its arguments will be invoked and the result of that call is the result of HTTPAPIEX_SAS_ExecuteRequest.]*/
    return HTTPAPIEX_ExecuteRequest(handle,requestType,relativePath,requestHttpHeadersHandle,requestContent,statusCode,responseHeadersHandle,responseContent);
}
