// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/string_tokenizer.h"
#include "azure_c_shared_utility/singlylinkedlist.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/connection_string_parser.h"

#include "iothub_service_client_auth.h"

static const char* IOTHUBHOSTNAME = "HostName";
static const char* IOTHUBSHAREDACESSKEYNAME = "SharedAccessKeyName";
static const char* IOTHUBSHAREDACESSKEY = "SharedAccessKey";
static const char* IOTHUBSHAREDACESSSIGNATURE = "SharedAccessSignature";
static const char* IOTHUBDEVICEID = "DeviceId";
static const char* IOTHUBSASPREFIX = "sas=";

static void free_service_client_auth(IOTHUB_SERVICE_CLIENT_AUTH* authInfo)
{
    free(authInfo->hostname);
    free(authInfo->iothubName);
    free(authInfo->iothubSuffix);
    free(authInfo->sharedAccessKey);
    free(authInfo->keyName);
    free(authInfo->deviceId);
    free(authInfo);
}

MU_DEFINE_ENUM_STRINGS_WITHOUT_INVALID(IOTHUB_DEVICE_STATUS, IOTHUB_DEVICE_STATUS_VALUES);
MU_DEFINE_ENUM_STRINGS_WITHOUT_INVALID(IOTHUB_DEVICE_CONNECTION_STATE, IOTHUB_DEVICE_CONNECTION_STATE_VALUES);

static IOTHUB_SERVICE_CLIENT_AUTH_HANDLE create_from_connection_string(const char* connectionString, bool useSharedAccessSignature)
{
    IOTHUB_SERVICE_CLIENT_AUTH_HANDLE result;

    /*Codes_SRS_IOTHUBSERVICECLIENT_12_001: [** IoTHubServiceClientAuth_CreateFromConnectionString shall verify the input parameter and if it is NULL then return NULL **]*/
    if (connectionString == NULL)
    {
        LogError("Input parameter is NULL: connectionString");
        result = NULL;
    }
    else
    {
        /*Codes_SRS_IOTHUBSERVICECLIENT_12_002: [** IoTHubServiceClientAuth_CreateFromConnectionString shall allocate memory for a new service client instance. **] */
        result = malloc(sizeof(IOTHUB_SERVICE_CLIENT_AUTH));
        if (result == NULL)
        {
            /*Codes_SRS_IOTHUBSERVICECLIENT_12_003: [** If the allocation failed, IoTHubServiceClientAuth_CreateFromConnectionString shall return NULL **] */
            LogError("Malloc failed for IOTHUB_SERVICE_CLIENT_AUTH");
        }
        else
        {
            memset(result, 0, sizeof(*result));

            /*Codes_SRS_IOTHUBSERVICECLIENT_12_009: [** IoTHubServiceClientAuth_CreateFromConnectionString shall create a STRING_HANDLE from the given connection string by calling STRING_construct. **] */
            STRING_HANDLE connection_string;
            if ((connection_string = STRING_construct(connectionString)) == NULL)
            {
                /*Codes_SRS_IOTHUBSERVICECLIENT_12_010: [** If the STRING_construct fails, IoTHubServiceClientAuth_CreateFromConnectionString shall do clean up and return NULL. **] */
                LogError("STRING_construct failed");
                free_service_client_auth(result);
                result = NULL;
            }
            else
            {
                /*Codes_SRS_IOTHUBSERVICECLIENT_12_004: [** IoTHubServiceClientAuth_CreateFromConnectionString shall populate hostName, iotHubName, iotHubSuffix, sharedAccessKeyName, sharedAccessKeyValue from the given connection string by calling connectionstringparser_parse **] */
                MAP_HANDLE connection_string_values_map;
                if ((connection_string_values_map = connectionstringparser_parse(connection_string)) == NULL)
                {
                    /*Codes_SRS_IOTHUBSERVICECLIENT_12_005: [** If populating the IOTHUB_SERVICE_CLIENT_AUTH fails, IoTHubServiceClientAuth_CreateFromConnectionString shall do clean up and return NULL **] */
                    LogError("Tokenizing failed on connectionString");
                    free_service_client_auth(result);
                    result = NULL;
                }
                else
                {
                    STRING_TOKENIZER_HANDLE tokenizer = NULL;
                    STRING_HANDLE token_key_string = NULL;
                    STRING_HANDLE token_value_string = NULL;
                    STRING_HANDLE host_name_string = NULL;
                    STRING_HANDLE shared_access = NULL;
                    const char* hostName;
                    const char* keyName;
                    const char* deviceId;
                    const char* sharedAccess = NULL;
                    const char* iothubName;
                    const char* iothubSuffix;

                    keyName = Map_GetValueFromKey(connection_string_values_map, IOTHUBSHAREDACESSKEYNAME);
                    deviceId = Map_GetValueFromKey(connection_string_values_map, IOTHUBDEVICEID);

                    /*Codes_SRS_IOTHUBSERVICECLIENT_12_004: [** IoTHubServiceClientAuth_CreateFromConnectionString shall populate hostName, iotHubName, iotHubSuffix, sharedAccessKeyName, sharedAccessKeyValue from the given connection string by calling connectionstringparser_parse **] */
                    (void)memset(result, 0, sizeof(IOTHUB_SERVICE_CLIENT_AUTH));
                    if ((keyName == NULL) && (deviceId == NULL))
                    {
                        /*Codes_SRS_IOTHUBSERVICECLIENT_12_012: [** If the populating SharedAccessKeyName fails, IoTHubServiceClientAuth_CreateFromConnectionString shall do clean up and return NULL. **] */
                        LogError("Couldn't find %s or %s in connection string", IOTHUBSHAREDACESSKEYNAME, IOTHUBDEVICEID);
                        free_service_client_auth(result);
                        result = NULL;
                    }
                    else if ((keyName != NULL) && (deviceId != NULL))
                    {
                        LogError("Both %s and %s in connection string were set, they are mutually exclusive", IOTHUBSHAREDACESSKEYNAME, IOTHUBDEVICEID);
                        free_service_client_auth(result);
                        result = NULL;
                    }
                    else if ((hostName = Map_GetValueFromKey(connection_string_values_map, IOTHUBHOSTNAME)) == NULL)
                    {
                        /*Codes_SRS_IOTHUBSERVICECLIENT_12_011: [** If the populating HostName fails, IoTHubServiceClientAuth_CreateFromConnectionString shall do clean up and return NULL. **] */
                        LogError("Couldn't find %s in connection string", IOTHUBHOSTNAME);
                        free_service_client_auth(result);
                        result = NULL;
                    }
                    else if (!useSharedAccessSignature && (sharedAccess = Map_GetValueFromKey(connection_string_values_map, IOTHUBSHAREDACESSKEY)) == NULL)
                    {
                        /*Codes_SRS_IOTHUBSERVICECLIENT_12_013: [** If the populating SharedAccessKey fails, IoTHubServiceClientAuth_CreateFromConnectionString shall do clean up and return NULL. **] */
                        LogError("Couldn't find %s in connection string", IOTHUBSHAREDACESSKEY);
                        free_service_client_auth(result);
                        result = NULL;
                    }
                    else if (useSharedAccessSignature && (sharedAccess = Map_GetValueFromKey(connection_string_values_map, IOTHUBSHAREDACESSSIGNATURE)) == NULL)
                    {
                        /*Codes_SRS_IOTHUBSERVICECLIENT_12_013: [** If the populating SharedAccessKey fails, IoTHubServiceClientAuth_CreateFromSharedAccessSignature shall do clean up and return NULL. **] */
                        LogError("Couldn't find %s in connection string", IOTHUBSHAREDACESSSIGNATURE);
                        free_service_client_auth(result);
                        result = NULL;
                    }
                    /*Codes_SRS_IOTHUBSERVICECLIENT_12_038: [** IoTHubServiceClientAuth_CreateFromConnectionString shall create a STRING_handle from hostName by calling STRING_construct. **] */
                    else if ((host_name_string = STRING_construct(hostName)) == NULL)
                    {
                        /*Codes_SRS_IOTHUBSERVICECLIENT_12_039: [** If the STRING_construct fails, IoTHubServiceClientAuth_CreateFromConnectionString shall do clean up and return NULL. **] */
                        LogError("STRING_construct failed");
                        free_service_client_auth(result);
                        result = NULL;
                    }
                    /*Codes_SRS_IOTHUBSERVICECLIENT_12_014: [** IoTHubServiceClientAuth_CreateFromConnectionString shall create a STRING_TOKENIZER to parse HostName by calling STRING_TOKENIZER_create. **] */
                    else if ((tokenizer = STRING_TOKENIZER_create(host_name_string)) == NULL)
                    {
                        /*Codes_SRS_IOTHUBSERVICECLIENT_12_015: [** If the STRING_TOKENIZER_create fails, IoTHubServiceClientAuth_CreateFromConnectionString shall do clean up and return NULL. **] */
                        LogError("Error creating STRING tokenizer");
                        free_service_client_auth(result);
                        result = NULL;
                    }
                    /*Codes_SRS_IOTHUBSERVICECLIENT_12_016: [** IoTHubServiceClientAuth_CreateFromConnectionString shall create a new STRING_HANDLE for token key string by calling STRING_new. **] */
                    else if ((token_key_string = STRING_new()) == NULL)
                    {
                        /*Codes_SRS_IOTHUBSERVICECLIENT_12_017: [** If the STRING_new fails, IoTHubServiceClientAuth_CreateFromConnectionString shall do clean up and return NULL. **] */
                        LogError("Error creating key token STRING_HANDLE");
                        free_service_client_auth(result);
                        result = NULL;
                    }
                    /*Codes_SRS_IOTHUBSERVICECLIENT_12_018: [** IoTHubServiceClientAuth_CreateFromConnectionString shall create a new STRING_HANDLE for token value string by calling STRING_new. **] */
                    else if ((token_value_string = STRING_new()) == NULL)
                    {
                        /*Codes_SRS_IOTHUBSERVICECLIENT_12_019: [** If the STRING_new fails, IoTHubServiceClientAuth_CreateFromConnectionString shall do clean up and return NULL. **] */
                        LogError("Error creating value token STRING_HANDLE");
                        free_service_client_auth(result);
                        result = NULL;
                    }
                    /*Codes_SRS_IOTHUBSERVICECLIENT_12_020: [** IoTHubServiceClientAuth_CreateFromConnectionString shall call STRING_TOKENIZER_get_next_token to get token key string. **] */
                    else if (STRING_TOKENIZER_get_next_token(tokenizer, token_key_string, ".") != 0)
                    {
                        /*Codes_SRS_IOTHUBSERVICECLIENT_12_021: [** If the STRING_TOKENIZER_get_next_token fails, IoTHubServiceClientAuth_CreateFromConnectionString shall do clean up and return NULL. **] */
                        LogError("Error reading key token STRING");
                        free_service_client_auth(result);
                        result = NULL;
                    }
                    /*Codes_SRS_IOTHUBSERVICECLIENT_12_022: [** IoTHubServiceClientAuth_CreateFromConnectionString shall call STRING_TOKENIZER_get_next_token to get token value string. **] */
                    else if (STRING_TOKENIZER_get_next_token(tokenizer, token_value_string, "0") != 0)
                    {
                        /*Codes_SRS_IOTHUBSERVICECLIENT_12_023: [** If the STRING_TOKENIZER_get_next_token fails, IoTHubServiceClientAuth_CreateFromConnectionString shall do clean up and return NULL. **] */
                        LogError("Error reading value token STRING");
                        free_service_client_auth(result);
                        result = NULL;
                    }
                    /*Codes_SRS_IOTHUBSERVICECLIENT_12_024: [** IoTHubServiceClientAuth_CreateFromConnectionString shall allocate memory and copy hostName to result->hostName by calling mallocAndStrcpy_s. **] */
                    else if (mallocAndStrcpy_s(&result->hostname, hostName) != 0)
                    {
                        /*Codes_SRS_IOTHUBSERVICECLIENT_12_025: [** If the mallocAndStrcpy_s fails, IoTHubServiceClientAuth_CreateFromConnectionString shall do clean up and return NULL. **] */
                        LogError("mallocAndStrcpy_s failed for hostName");
                        free_service_client_auth(result);
                        result = NULL;
                    }
                    /*Codes_SRS_IOTHUBSERVICECLIENT_12_026: [** IoTHubServiceClientAuth_CreateFromConnectionString shall allocate memory and copy keyName to result->keyName by calling mallocAndStrcpy_s. **] */
                    else if ((keyName != NULL) && (mallocAndStrcpy_s(&result->keyName, keyName) != 0))
                    {
                        /*Codes_SRS_IOTHUBSERVICECLIENT_12_027: [** If the mallocAndStrcpy_s fails, IoTHubServiceClientAuth_CreateFromConnectionString shall do clean up and return NULL. **] */
                        LogError("mallocAndStrcpy_s failed for keyName");
                        free_service_client_auth(result);
                        result = NULL;
                    }
                    /*Codes_SRS_IOTHUBSERVICECLIENT_12_026: [** IoTHubServiceClientAuth_CreateFromConnectionString shall allocate memory and copy keyName to result->keyName by calling mallocAndStrcpy_s. **] */
                    else if ((deviceId != NULL) && (mallocAndStrcpy_s(&result->deviceId, deviceId) != 0))
                    {
                        /*Codes_SRS_IOTHUBSERVICECLIENT_12_027: [** If the mallocAndStrcpy_s fails, IoTHubServiceClientAuth_CreateFromConnectionString shall do clean up and return NULL. **] */
                        LogError("mallocAndStrcpy_s failed for keyName");
                        free_service_client_auth(result);
                        result = NULL;
                    }
                    /*Codes_SRS_IOTHUBSERVICECLIENT_12_028: [** IoTHubServiceClientAuth_CreateFromConnectionString shall allocate memory and copy sharedAccessKey to result->sharedAccessKey by calling mallocAndStrcpy_s. **] */
                    else if (!useSharedAccessSignature && (sharedAccess != NULL) && mallocAndStrcpy_s(&result->sharedAccessKey, sharedAccess) != 0)
                    {
                        /*Codes_SRS_IOTHUBSERVICECLIENT_12_029: [** If the mallocAndStrcpy_s fails, IoTHubServiceClientAuth_CreateFromConnectionString shall do clean up and return NULL. **] */
                        LogError("mallocAndStrcpy_s failed for sharedAccessKey");
                        free_service_client_auth(result);
                        result = NULL;
                    }
                    /*Codes_SRS_IOTHUBSERVICECLIENT_12_041: [** IoTHubServiceClientAuth_CreateFromSharedAccessSignature shall allocate memory and copy sharedAccessSignature to result->sharedAccessKey by prefixing it with "sas=". **] */
                    else if (useSharedAccessSignature &&
                        (sharedAccess != NULL) &&
                        (((shared_access = STRING_construct(IOTHUBSASPREFIX)) == NULL) ||
                        (STRING_concat(shared_access, sharedAccess) != 0) ||
                        (mallocAndStrcpy_s(&result->sharedAccessKey, STRING_c_str(shared_access)) != 0)))
                    {
                        /*Codes_SRS_IOTHUBSERVICECLIENT_12_029: [** If the mallocAndStrcpy_s fails, IoTHubServiceClientAuth_CreateFromSharedAccessSignature shall do clean up and return NULL. **] */
                        LogError("mallocAndStrcpy_s failed for sharedAccessSignature");
                        free_service_client_auth(result);
                        result = NULL;
                    }
                    /*Codes_SRS_IOTHUBSERVICECLIENT_12_034: [** IoTHubServiceClientAuth_CreateFromConnectionString shall create C string from token key string handle by calling STRING_c_str. **] */
                    else if ((iothubName = STRING_c_str(token_key_string)) == NULL)
                    {
                        /*Codes_SRS_IOTHUBSERVICECLIENT_12_035 : [** If the STRING_c_str fails, IoTHubServiceClientAuth_CreateFromConnectionString shall do clean up and return NULL. **] */
                        LogError("STRING_c_str failed for iothubName");
                        free_service_client_auth(result);
                        result = NULL;
                    }
                    /*Codes_SRS_IOTHUBSERVICECLIENT_12_036 : [** IoTHubServiceClientAuth_CreateFromConnectionString shall create C string from token value string handle by calling STRING_c_str. **] */
                    else if ((iothubSuffix = STRING_c_str(token_value_string)) == NULL)
                    {
                        /*Codes_SRS_IOTHUBSERVICECLIENT_12_037 : [** If the mallocAndStrcpy_s fails, IoTHubServiceClientAuth_CreateFromConnectionString shall do clean up and return NULL. **] */
                        LogError("STRING_c_str failed for iothubSuffix");
                        free_service_client_auth(result);
                        result = NULL;
                    }
                    /*Codes_SRS_IOTHUBSERVICECLIENT_12_030: [** IoTHubServiceClientAuth_CreateFromConnectionString shall allocate memory and copy iothubName to result->iothubName by calling mallocAndStrcpy_s. **] */
                    else if (mallocAndStrcpy_s(&result->iothubName, iothubName) != 0)
                    {
                        /*Codes_SRS_IOTHUBSERVICECLIENT_12_031 : [** If the mallocAndStrcpy_s fails, IoTHubServiceClientAuth_CreateFromConnectionString shall do clean up and return NULL. **] */
                        LogError("mallocAndStrcpy_s failed for sharedAccessKey");
                        free_service_client_auth(result);
                        result = NULL;
                    }
                    /*Codes_SRS_IOTHUBSERVICECLIENT_12_032: [** IoTHubServiceClientAuth_CreateFromConnectionString shall allocate memory and copy iothubSuffix to result->iothubSuffix by calling mallocAndStrcpy_s. **] */
                    else if (mallocAndStrcpy_s(&result->iothubSuffix, iothubSuffix) != 0)
                    {
                        /*Codes_SRS_IOTHUBSERVICECLIENT_12_033 : [** If the mallocAndStrcpy_s fails, IoTHubServiceClientAuth_CreateFromConnectionString shall do clean up and return NULL. **] */
                        LogError("mallocAndStrcpy_s failed for sharedAccessKey");
                        free_service_client_auth(result);
                        result = NULL;
                    }
                    /*Codes_SRS_IOTHUBSERVICECLIENT_12_006: [** If the IOTHUB_SERVICE_CLIENT_AUTH has been populated IoTHubServiceClientAuth_CreateFromConnectionString shall do clean up and return with a IOTHUB_SERVICE_CLIENT_AUTH_HANDLE to it **] */
                    STRING_delete(token_key_string);
                    STRING_delete(token_value_string);
                    STRING_delete(host_name_string);
                    STRING_TOKENIZER_destroy(tokenizer);
                    Map_Destroy(connection_string_values_map);
                    if (useSharedAccessSignature)
                    {
                        STRING_delete(shared_access);
                    }
                }
                STRING_delete(connection_string);
            }
        }
    }
    return result;
}

IOTHUB_SERVICE_CLIENT_AUTH_HANDLE IoTHubServiceClientAuth_CreateFromConnectionString(const char* connectionString)
{
    return create_from_connection_string(connectionString, false);
}

IOTHUB_SERVICE_CLIENT_AUTH_HANDLE IoTHubServiceClientAuth_CreateFromSharedAccessSignature(const char* connectionString)
{
    return create_from_connection_string(connectionString, true);
}

void IoTHubServiceClientAuth_Destroy(IOTHUB_SERVICE_CLIENT_AUTH_HANDLE serviceClientHandle)
{
    /*Codes_SRS_IOTHUBSERVICECLIENT_12_007: [** If the serviceClientHandle input parameter is NULL IoTHubServiceClient_Destroy shall return **]*/
    if (serviceClientHandle != NULL)
    {
        /*Codes_SRS_IOTHUBSERVICECLIENT_12_008: [** If the serviceClientHandle input parameter is not NULL IoTHubServiceClient_Destroy shall free the memory of it and return **]*/
        free_service_client_auth((IOTHUB_SERVICE_CLIENT_AUTH*)serviceClientHandle);
    }
}
