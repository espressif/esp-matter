// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef DONT_USE_UPLOADTOBLOB

#include <stdlib.h>
#include <string.h>
#include "azure_c_shared_utility/optimize_size.h"
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/string_tokenizer.h"
#include "azure_c_shared_utility/doublylinkedlist.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/tickcounter.h"
#include "azure_c_shared_utility/httpapiexsas.h"
#include "azure_c_shared_utility/shared_util_options.h"
#include "azure_c_shared_utility/urlencode.h"

#include "iothub_client_core_ll.h"
#include "iothub_client_options.h"
#include "internal/iothub_client_private.h"
#include "iothub_client_version.h"
#include "iothub_transport_ll.h"
#include "parson.h"
#include "internal/iothub_client_ll_uploadtoblob.h"
#include "internal/iothub_client_authorization.h"
#include "internal/blob.h"

#define API_VERSION "?api-version=2016-11-14"

#ifdef WINCE
#include <stdarg.h>
// Returns number of characters copied.
int snprintf(char * s, size_t n, const char * format, ...)
{
    int result;
    va_list args;
    va_start(args, format);
    result = vsnprintf(s, n, format, args);
    va_end(args);
    return result;
}
#endif

/*Codes_SRS_IOTHUBCLIENT_LL_02_085: [ IoTHubClient_LL_UploadToBlob shall use the same authorization as step 1. to prepare and perform a HTTP request with the following parameters: ]*/
static const char* const RESPONSE_BODY_FORMAT = "{\"correlationId\":\"%s\", \"isSuccess\":%s, \"statusCode\":%d, \"statusDescription\":\"%s\"}";
static const char* const RESPONSE_BODY_ABORTED_MESSAGE = "file upload aborted";
static const char* const RESPONSE_BODY_FAILED_MESSAGE = "client not able to connect with the server";
static const char* const RESPONSE_BODY_ERROR_RETURN_CODE = "-1";
static const char* const RESPONSE_BODY_ERROR_BOOLEAN_STRING = "false";

#define INDEFINITE_TIME                            ((time_t)-1)

static const char* const EMPTY_STRING = "";
static const char* const HEADER_AUTHORIZATION = "Authorization";
static const char* const HEADER_APP_JSON = "application/json";

typedef struct UPLOADTOBLOB_X509_CREDENTIALS_TAG
{
    char* x509certificate;
    char* x509privatekey;
} UPLOADTOBLOB_X509_CREDENTIALS;

typedef enum UPOADTOBLOB_CURL_VERBOSITY_TAG
{
    UPOADTOBLOB_CURL_VERBOSITY_UNSET,
    UPOADTOBLOB_CURL_VERBOSITY_ON,
    UPOADTOBLOB_CURL_VERBOSITY_OFF
} UPOADTOBLOB_CURL_VERBOSITY;

typedef struct IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE_DATA_TAG
{
    const char* deviceId;
    char* hostname;
    IOTHUB_AUTHORIZATION_HANDLE authorization_module;
    IOTHUB_CREDENTIAL_TYPE cred_type;
    union 
    {
        UPLOADTOBLOB_X509_CREDENTIALS x509_credentials;
        char* supplied_sas_token;
    } credentials;
    
    char* certificates;
    HTTP_PROXY_OPTIONS http_proxy_options;
    UPOADTOBLOB_CURL_VERBOSITY curl_verbosity_level;
    size_t blob_upload_timeout_secs;
}IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE_DATA;

typedef struct BLOB_UPLOAD_CONTEXT_TAG
{
    const unsigned char* blobSource; /* source to upload */
    size_t blobSourceSize; /* size of the source */
    size_t remainingSizeToUpload; /* size not yet uploaded */
} BLOB_UPLOAD_CONTEXT;

static int send_http_sas_request(IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE_DATA* upload_client, const char* uri_resource, HTTPAPIEX_HANDLE http_api_handle, const char* relative_path, HTTP_HEADERS_HANDLE request_header, BUFFER_HANDLE blobBuffer, BUFFER_HANDLE response_buff)
{
    int result;
    unsigned int statusCode;

    /*Codes_SRS_IOTHUBCLIENT_LL_02_089: [ If creating the HTTPAPIEX_SAS_HANDLE fails then IoTHubClient_LL_UploadMultipleBlocksToBlob(Ex) shall fail and return IOTHUB_CLIENT_ERROR. ]*/
    HTTPAPIEX_SAS_HANDLE http_sas_handle = HTTPAPIEX_SAS_Create_From_String(IoTHubClient_Auth_Get_DeviceKey(upload_client->authorization_module), uri_resource, EMPTY_STRING);
    if (http_sas_handle == NULL)
    {
        LogError("unable to HTTPAPIEX_SAS_Create");
        result = MU_FAILURE;
    }
    else
    {
        /*Codes_SRS_IOTHUBCLIENT_LL_32_003: [ IoTHubClient_LL_UploadMultipleBlocksToBlob(Ex) shall execute HTTPAPIEX_ExecuteRequest passing the following information for arguments: ]*/
        if (HTTPAPIEX_SAS_ExecuteRequest(http_sas_handle, http_api_handle, HTTPAPI_REQUEST_POST, relative_path, request_header,
            blobBuffer, &statusCode, NULL, response_buff) != HTTPAPIEX_OK)
        {
            /*Codes_SRS_IOTHUBCLIENT_LL_02_076: [ If HTTPAPIEX_ExecuteRequest call fails then IoTHubClient_LL_UploadMultipleBlocksToBlob(Ex) shall fail and return IOTHUB_CLIENT_ERROR. ]*/
            result = MU_FAILURE;
            LogError("unable to HTTPAPIEX_ExecuteRequest");
        }
        else if (statusCode >= 300)
        {
            result = MU_FAILURE;
            LogError("HTTP code was %u", statusCode);
        }
        else
        {
            result = 0;
        }
        HTTPAPIEX_SAS_Destroy(http_sas_handle);
    }
    return result;
}

static int send_http_request(HTTPAPIEX_HANDLE http_api_handle, const char* relative_path, HTTP_HEADERS_HANDLE request_header, BUFFER_HANDLE blobBuffer, BUFFER_HANDLE response_buff)
{
    int result;
    unsigned int statusCode;
    /*Codes_SRS_IOTHUBCLIENT_LL_32_003: [ IoTHubClient_LL_UploadMultipleBlocksToBlob(Ex) shall execute HTTPAPIEX_ExecuteRequest passing the following information for arguments: ]*/
    if (HTTPAPIEX_ExecuteRequest(http_api_handle, HTTPAPI_REQUEST_POST, relative_path, request_header,
        blobBuffer, &statusCode, NULL, response_buff) != HTTPAPIEX_OK)
    {
        /*Codes_SRS_IOTHUBCLIENT_LL_02_076: [ If HTTPAPIEX_ExecuteRequest call fails then IoTHubClient_LL_UploadMultipleBlocksToBlob(Ex) shall fail and return IOTHUB_CLIENT_ERROR. ]*/
        result = MU_FAILURE;
        LogError("unable to HTTPAPIEX_ExecuteRequest");
    }
    else if (statusCode >= 300)
    {
        result = MU_FAILURE;
        LogError("HTTP code was %u", statusCode);
    }
    else
    {
        result = 0;
    }
    return result;
}

static int parse_result_json(const char* json_response, STRING_HANDLE correlation_id, STRING_HANDLE sas_uri)
{
    int result;

    JSON_Object* json_obj;
    /*Codes_SRS_IOTHUBCLIENT_LL_02_081: [ Otherwise, IoTHubClient_LL_UploadMultipleBlocksToBlob(Ex) shall use parson to extract and save the following information from the response buffer: correlationID and SasUri. ]*/
    JSON_Value* json = json_parse_string(json_response);
    if (json == NULL)
    {
        /*Codes_SRS_IOTHUBCLIENT_LL_02_082: [ If extracting and saving the correlationId or SasUri fails then IoTHubClient_LL_UploadMultipleBlocksToBlob(Ex) shall fail and return IOTHUB_CLIENT_ERROR. ]*/
        LogError("unable to json_parse_string");
        result = MU_FAILURE;

    }
    else
    {
        if ((json_obj = json_value_get_object(json)) == NULL)
        {
            LogError("unable to get json_value_get_object");
            result = MU_FAILURE;
        }
        else
        {
            const char* json_corr_id;
            const char* json_hostname;
            const char* json_container_name;
            const char* json_blob_name;
            const char* json_sas_token;
            STRING_HANDLE filename;
            if ((json_corr_id = json_object_get_string(json_obj, "correlationId")) == NULL)
            {
                LogError("unable to retrieve correlation Id from json");
                result = MU_FAILURE;
            }
            else if ((json_hostname = json_object_get_string(json_obj, "hostName")) == NULL)
            {
                LogError("unable to retrieve hostname Id from json");
                result = MU_FAILURE;
            }
            else if ((json_container_name = json_object_get_string(json_obj, "containerName")) == NULL)
            {
                LogError("unable to retrieve container name Id from json");
                result = MU_FAILURE;
            }
            else if ((json_blob_name = json_object_get_string(json_obj, "blobName")) == NULL)
            {
                LogError("unable to retrieve blob name Id from json");
                result = MU_FAILURE;
            }
            else if ((json_sas_token = json_object_get_string(json_obj, "sasToken")) == NULL)
            {
                LogError("unable to retrieve sas token from json");
                result = MU_FAILURE;
            }
            /*Codes_SRS_IOTHUBCLIENT_LL_32_008: [ The returned file name shall be URL encoded before passing back to the cloud. ]*/
            else if ((filename = URL_EncodeString(json_blob_name)) == NULL)
            {
                /*Codes_SRS_IOTHUBCLIENT_LL_32_009: [ If URL_EncodeString fails then IoTHubClient_LL_UploadMultipleBlocksToBlob(Ex) shall fail and return IOTHUB_CLIENT_ERROR. ]*/
                LogError("unable to URL encode of filename");
                result = MU_FAILURE;
            }
            else
            {
                if (STRING_sprintf(sas_uri, "https://%s/%s/%s%s", json_hostname, json_container_name, STRING_c_str(filename), json_sas_token) != 0)
                {
                    /*Codes_SRS_IOTHUBCLIENT_LL_02_082: [ If extracting and saving the correlationId or SasUri fails then IoTHubClient_LL_UploadMultipleBlocksToBlob(Ex) shall fail and return IOTHUB_CLIENT_ERROR. ]*/
                    LogError("unable to construct uri string");
                    result = MU_FAILURE;
                }
                else if (STRING_copy(correlation_id, json_corr_id) != 0)
                {
                    /*Codes_SRS_IOTHUBCLIENT_LL_02_082: [ If extracting and saving the correlationId or SasUri fails then IoTHubClient_LL_UploadMultipleBlocksToBlob(Ex) shall fail and return IOTHUB_CLIENT_ERROR. ]*/
                    LogError("unable to copy correlation Id");
                    result = MU_FAILURE;
                    STRING_empty(sas_uri);
                }
                else
                {
                    result = 0;
                }
                STRING_delete(filename);
            }
        }
        json_value_free(json);
    }
    return result;
}

IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE IoTHubClient_LL_UploadToBlob_Create(const IOTHUB_CLIENT_CONFIG* config, IOTHUB_AUTHORIZATION_HANDLE auth_handle)
{
    IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE_DATA* upload_data;
    if (auth_handle == NULL || config == NULL)
    {
        LogError("Invalid arguments auth_handle: %p, config: %p", auth_handle, config);
        upload_data = NULL;
    }
    else
    {
        upload_data = malloc(sizeof(IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE_DATA));
        if (upload_data == NULL)
        {
            LogError("Failed malloc allocation");
            /*return as is*/
        }
        else
        {
            memset(upload_data, 0, sizeof(IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE_DATA));

            upload_data->authorization_module = auth_handle;

            size_t iotHubNameLength = strlen(config->iotHubName);
            size_t iotHubSuffixLength = strlen(config->iotHubSuffix);
            upload_data->hostname = malloc(iotHubNameLength + 1 + iotHubSuffixLength + 1); /*first +1 is because "." the second +1 is because \0*/
            if (upload_data->hostname == NULL)
            {
                LogError("Failed malloc allocation");
                free(upload_data);
                upload_data = NULL;
            }
            else if ((upload_data->deviceId = IoTHubClient_Auth_Get_DeviceId(upload_data->authorization_module)) == NULL)
            {
                LogError("Failed retrieving device ID");
                free(upload_data->hostname);
                free(upload_data);
                upload_data = NULL;
            }
            else
            {
                char* insert_pos = (char*)upload_data->hostname;
                (void)memcpy((char*)insert_pos, config->iotHubName, iotHubNameLength);
                insert_pos += iotHubNameLength;
                *insert_pos = '.';
                insert_pos += 1;
                (void)memcpy(insert_pos, config->iotHubSuffix, iotHubSuffixLength); /*+1 will copy the \0 too*/
                insert_pos += iotHubSuffixLength;
                *insert_pos = '\0';

                upload_data->cred_type = IoTHubClient_Auth_Get_Credential_Type(upload_data->authorization_module);
                // If the credential type is unknown then it means that we are using x509 because the certs need to get
                // passed down later in the process.
                if (upload_data->cred_type == IOTHUB_CREDENTIAL_TYPE_UNKNOWN || upload_data->cred_type == IOTHUB_CREDENTIAL_TYPE_X509)
                {
                    upload_data->cred_type = IOTHUB_CREDENTIAL_TYPE_X509;
                    upload_data->credentials.x509_credentials.x509certificate = NULL;
                    upload_data->credentials.x509_credentials.x509privatekey = NULL;
                }
                else if (upload_data->cred_type == IOTHUB_CREDENTIAL_TYPE_X509_ECC)
                {
                    if (IoTHubClient_Auth_Get_x509_info(upload_data->authorization_module, &upload_data->credentials.x509_credentials.x509certificate, &upload_data->credentials.x509_credentials.x509privatekey) != 0)
                    {
                        LogError("Failed getting x509 certificate information");
                        free(upload_data->hostname);
                        free(upload_data);
                        upload_data = NULL;
                    }
                }
                else if (upload_data->cred_type == IOTHUB_CREDENTIAL_TYPE_SAS_TOKEN)
                {
                    upload_data->credentials.supplied_sas_token = IoTHubClient_Auth_Get_SasToken(upload_data->authorization_module, NULL, 0, EMPTY_STRING);
                    if (upload_data->credentials.supplied_sas_token == NULL)
                    {
                        LogError("Failed retrieving supplied sas token");
                        free(upload_data->hostname);
                        free(upload_data);
                        upload_data = NULL;
                    }
                }
            }
        }
    }
    return (IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE)upload_data;

}

/*returns 0 when correlationId, sasUri contain data*/
static int IoTHubClient_LL_UploadToBlob_step1and2(IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE_DATA* upload_data, HTTPAPIEX_HANDLE iotHubHttpApiExHandle, HTTP_HEADERS_HANDLE requestHttpHeaders, const char* destinationFileName, STRING_HANDLE correlationId, STRING_HANDLE sasUri)
{
    int result;

    /*Codes_SRS_IOTHUBCLIENT_LL_02_066: [ IoTHubClient_LL_UploadMultipleBlocksToBlob(Ex) shall create an HTTP relative path formed from "/devices/" + deviceId + "/files/" + "?api-version=API_VERSION". ]*/
    STRING_HANDLE relativePath = STRING_construct_sprintf("/devices/%s/files/%s", upload_data->deviceId, API_VERSION);
    if (relativePath == NULL)
    {
        /*Codes_SRS_IOTHUBCLIENT_LL_02_067: [ If creating the relativePath fails then IoTHubClient_LL_UploadMultipleBlocksToBlob(Ex) shall fail and return IOTHUB_CLIENT_ERROR. ]*/
        LogError("Failure constructing string");
        result = MU_FAILURE;
    }
    else
    {
        /*Codes_SRS_IOTHUBCLIENT_LL_32_001: [ IoTHubClient_LL_UploadMultipleBlocksToBlob(Ex) shall create a JSON string formed from "{ \"blobName\": \" + destinationFileName + "\" }" */
        STRING_HANDLE blobName = STRING_construct_sprintf("{ \"blobName\": \"%s\" }", destinationFileName);
        if (blobName == NULL)
        {
            /*Codes_SRS_IOTHUBCLIENT_LL_32_002: [ If creating the JSON string fails then IoTHubClient_LL_UploadMultipleBlocksToBlob(Ex) shall fail and return IOTHUB_CLIENT_ERROR. ]*/
            LogError("Failure constructing string");
            result = MU_FAILURE;
        }
        else
        {
            BUFFER_HANDLE responseContent;
            size_t len = STRING_length(blobName);
            BUFFER_HANDLE blobBuffer = BUFFER_create((const unsigned char *)STRING_c_str(blobName), len);
            if (blobBuffer == NULL)
            {
                /*Codes_SRS_IOTHUBCLIENT_LL_32_002: [ If creating the JSON string fails then IoTHubClient_LL_UploadMultipleBlocksToBlob(Ex) shall fail and return IOTHUB_CLIENT_ERROR. ]*/
                LogError("unable to create BUFFER");
                result = MU_FAILURE;
            }
            else
            {
                /*Codes_SRS_IOTHUBCLIENT_LL_02_068: [ IoTHubClient_LL_UploadMultipleBlocksToBlob(Ex) shall create an HTTP responseContent BUFFER_HANDLE. ]*/
                if ((responseContent = BUFFER_new()) == NULL)
                {
                    /*Codes_SRS_IOTHUBCLIENT_LL_02_069: [ If creating the HTTP response buffer handle fails then IoTHubClient_LL_UploadMultipleBlocksToBlob(Ex) shall fail and return IOTHUB_CLIENT_ERROR. ]*/
                    result = MU_FAILURE;
                    LogError("unable to BUFFER_new");
                }
                else
                {
                    /*Codes_SRS_IOTHUBCLIENT_LL_02_072: [ IoTHubClient_LL_UploadMultipleBlocksToBlob(Ex) shall add the following name:value to request HTTP headers: ] "Content-Type": "application/json" "Accept": "application/json" "User-Agent": "iothubclient/" IOTHUB_SDK_VERSION*/
                    /*Codes_SRS_IOTHUBCLIENT_LL_02_107: [ - "Authorization" header shall not be build. ]*/
                    if (!(
                        (HTTPHeaders_AddHeaderNameValuePair(requestHttpHeaders, "Content-Type", HEADER_APP_JSON) == HTTP_HEADERS_OK) &&
                        (HTTPHeaders_AddHeaderNameValuePair(requestHttpHeaders, "Accept", HEADER_APP_JSON) == HTTP_HEADERS_OK) &&
                        (HTTPHeaders_AddHeaderNameValuePair(requestHttpHeaders, "User-Agent", "iothubclient/" IOTHUB_SDK_VERSION) == HTTP_HEADERS_OK) &&
                        ((upload_data->cred_type == IOTHUB_CREDENTIAL_TYPE_X509 || upload_data->cred_type == IOTHUB_CREDENTIAL_TYPE_X509_ECC) ||
                        (HTTPHeaders_AddHeaderNameValuePair(requestHttpHeaders, HEADER_AUTHORIZATION, EMPTY_STRING) == HTTP_HEADERS_OK))
                        ))
                    {
                        /*Codes_SRS_IOTHUBCLIENT_LL_02_071: [ If creating the HTTP headers fails then IoTHubClient_LL_UploadMultipleBlocksToBlob(Ex) shall fail and return IOTHUB_CLIENT_ERROR. ]*/
                        LogError("unable to HTTPHeaders_AddHeaderNameValuePair");
                        result = MU_FAILURE;
                    }
                    else
                    {
                        int wasIoTHubRequestSuccess = 0; /*!=0 means responseContent has a buffer that should be parsed by parson after executing the below switch*/
                        /* set the result to error by default */
                        result = MU_FAILURE;
                        switch (upload_data->cred_type)
                        {
                            default:
                            {
                                /*wasIoTHubRequestSuccess takes care of the return value*/
                                LogError("Internal Error: unexpected value in auth schema = %d", upload_data->cred_type);
                                result = MU_FAILURE;
                                break;
                            }
                            case IOTHUB_CREDENTIAL_TYPE_X509_ECC:
                            case IOTHUB_CREDENTIAL_TYPE_X509:
                            {
                                if (send_http_request(iotHubHttpApiExHandle, STRING_c_str(relativePath), requestHttpHeaders, blobBuffer, responseContent) != 0)
                                {
                                    /*Codes_SRS_IOTHUBCLIENT_LL_02_076: [ If HTTPAPIEX_ExecuteRequest call fails then IoTHubClient_LL_UploadMultipleBlocksToBlob(Ex) shall fail and return IOTHUB_CLIENT_ERROR. ]*/
                                    result = MU_FAILURE;
                                    LogError("unable to HTTPAPIEX_ExecuteRequest");
                                }
                                else
                                {
                                    wasIoTHubRequestSuccess = 1;
                                }
                                break;
                            }
                            case IOTHUB_CREDENTIAL_TYPE_DEVICE_KEY:
                            case IOTHUB_CREDENTIAL_TYPE_DEVICE_AUTH:
                            {
                                STRING_HANDLE uri_resource = STRING_construct_sprintf("%s/devices/%s", upload_data->hostname, upload_data->deviceId);
                                if (uri_resource == NULL)
                                {
                                    /*Codes_SRS_IOTHUBCLIENT_LL_02_089: [ If creating the HTTPAPIEX_SAS_HANDLE fails then IoTHubClient_LL_UploadMultipleBlocksToBlob(Ex) shall fail and return IOTHUB_CLIENT_ERROR. ]*/
                                    result = MU_FAILURE;
                                    LogError("Failure constructing string");
                                }
                                else
                                {
                                    if (upload_data->cred_type == IOTHUB_CREDENTIAL_TYPE_DEVICE_AUTH)
                                    {
                                        time_t curr_time;
                                        if ((curr_time = get_time(NULL)) == INDEFINITE_TIME)
                                        {
                                            result = MU_FAILURE;
                                            LogError("failure retrieving time");
                                        }
                                        else
                                        {
                                            uint64_t expiry = (uint64_t)(difftime(curr_time, 0) + 3600);
                                            char* sas_token = IoTHubClient_Auth_Get_SasToken(upload_data->authorization_module, STRING_c_str(uri_resource), expiry, EMPTY_STRING);
                                            if (sas_token == NULL)
                                            {
                                                result = MU_FAILURE;
                                                LogError("unable to retrieve sas token");
                                            }
                                            else
                                            {
                                                if (HTTPHeaders_ReplaceHeaderNameValuePair(requestHttpHeaders, HEADER_AUTHORIZATION, sas_token) != HTTP_HEADERS_OK)
                                                {
                                                    /*Codes_SRS_IOTHUBCLIENT_LL_02_074: [ If adding "Authorization" fails then IoTHubClient_LL_UploadMultipleBlocksToBlob(Ex) shall fail and return IOTHUB_CLIENT_ERROR ]*/
                                                    result = MU_FAILURE;
                                                    LogError("unable to HTTPHeaders_AddHeaderNameValuePair");
                                                }
                                                else if (send_http_request(iotHubHttpApiExHandle, STRING_c_str(relativePath), requestHttpHeaders, blobBuffer, responseContent) != 0)
                                                {
                                                    /*Codes_SRS_IOTHUBCLIENT_LL_02_076: [ If HTTPAPIEX_ExecuteRequest call fails then IoTHubClient_LL_UploadMultipleBlocksToBlob(Ex) shall fail and return IOTHUB_CLIENT_ERROR. ]*/
                                                    result = MU_FAILURE;
                                                    LogError("unable to HTTPAPIEX_ExecuteRequest");
                                                }
                                                else
                                                {
                                                    wasIoTHubRequestSuccess = 1;
                                                }
                                            }
                                            free(sas_token);
                                        }
                                    }
                                    else
                                    {
                                        if (send_http_sas_request(upload_data, STRING_c_str(uri_resource), iotHubHttpApiExHandle, STRING_c_str(relativePath), requestHttpHeaders, blobBuffer, responseContent) != 0)
                                        {
                                            /*Codes_SRS_IOTHUBCLIENT_LL_02_076: [ If HTTPAPIEX_ExecuteRequest call fails then IoTHubClient_LL_UploadMultipleBlocksToBlob(Ex) shall fail and return IOTHUB_CLIENT_ERROR. ]*/
                                            result = MU_FAILURE;
                                            LogError("unable to HTTPAPIEX_ExecuteRequest");
                                        }
                                        else
                                        {
                                            wasIoTHubRequestSuccess = 1;
                                        }
                                    }
                                    STRING_delete(uri_resource);
                                }
                                break;
                            }
                            case IOTHUB_CREDENTIAL_TYPE_SAS_TOKEN:
                            {
                                /*Codes_SRS_IOTHUBCLIENT_LL_02_073: [ If the credentials used to create handle have "sasToken" then IoTHubClient_LL_UploadMultipleBlocksToBlob(Ex) shall add the following HTTP request headers: ]*/
                                if (HTTPHeaders_ReplaceHeaderNameValuePair(requestHttpHeaders, HEADER_AUTHORIZATION, upload_data->credentials.supplied_sas_token) != HTTP_HEADERS_OK)
                                {
                                    /*Codes_SRS_IOTHUBCLIENT_LL_02_074: [ If adding "Authorization" fails then IoTHubClient_LL_UploadMultipleBlocksToBlob(Ex) shall fail and return IOTHUB_CLIENT_ERROR ]*/
                                    result = MU_FAILURE;
                                    LogError("unable to HTTPHeaders_AddHeaderNameValuePair");
                                }
                                else if (send_http_request(iotHubHttpApiExHandle, STRING_c_str(relativePath), requestHttpHeaders, blobBuffer, responseContent) != 0)
                                {
                                    /*Codes_SRS_IOTHUBCLIENT_LL_02_076: [ If HTTPAPIEX_ExecuteRequest call fails then IoTHubClient_LL_UploadMultipleBlocksToBlob(Ex) shall fail and return IOTHUB_CLIENT_ERROR. ]*/
                                    result = MU_FAILURE;
                                    LogError("unable to HTTPAPIEX_ExecuteRequest");
                                }
                                else
                                {
                                    wasIoTHubRequestSuccess = 1;
                                }
                                break;
                            }
                        }
                        if (wasIoTHubRequestSuccess == 1)
                        {
                            const unsigned char*responseContent_u_char = BUFFER_u_char(responseContent);
                            size_t responseContent_length = BUFFER_length(responseContent);
                            STRING_HANDLE responseAsString = STRING_from_byte_array(responseContent_u_char, responseContent_length);
                            if (responseAsString == NULL)
                            {
                                result = MU_FAILURE;
                                LogError("unable to get the response as string");
                            }
                            else
                            {
                                if (parse_result_json(STRING_c_str(responseAsString), correlationId, sasUri) != 0)
                                {
                                    /*Codes_SRS_IOTHUBCLIENT_LL_02_082: [ If extracting and saving the correlationId or SasUri fails then IoTHubClient_LL_UploadMultipleBlocksToBlob(Ex) shall fail and return IOTHUB_CLIENT_ERROR. ]*/
                                    LogError("unable to parse json result");
                                    result = MU_FAILURE;
                                }
                                else
                                {
                                    result = 0;
                                }
                                STRING_delete(responseAsString);
                            }
                        }
                    }
                    BUFFER_delete(responseContent);
                }
                BUFFER_delete(blobBuffer);
            }
            STRING_delete(blobName);
        }
        STRING_delete(relativePath);
    }
    return result;
}

/*returns 0 when the IoTHub has been informed about the file upload status*/
static int IoTHubClient_LL_UploadToBlob_step3(IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE_DATA* upload_data, HTTPAPIEX_HANDLE iotHubHttpApiExHandle, HTTP_HEADERS_HANDLE requestHttpHeaders, BUFFER_HANDLE messageBody)
{
    int result;
    /*here is step 3. depending on the outcome of step 2 it needs to inform IoTHub about the file upload status*/
    /*if step 1 failed, there's nothing that step 3 needs to report.*/
    /*this POST "tries" to happen*/

    /*Codes_SRS_IOTHUBCLIENT_LL_02_085: [ IoTHubClient_LL_UploadMultipleBlocksToBlob(Ex) shall use the same authorization as step 1. to prepare and perform a HTTP request with the following parameters: ]*/
    STRING_HANDLE relativePathNotification = STRING_construct_sprintf("/devices/%s/files/notifications/%s", upload_data->deviceId, API_VERSION);
    if (relativePathNotification == NULL)
    {
        result = MU_FAILURE;
        LogError("Failure constructing string");
    }
    else
    {
        /*Codes_SRS_IOTHUBCLIENT_LL_02_086: [ If performing the HTTP request fails then IoTHubClient_LL_UploadMultipleBlocksToBlob(Ex) shall fail and return IOTHUB_CLIENT_ERROR. ]*/
        switch (upload_data->cred_type)
        {
            default:
            {
                LogError("internal error: unknown authorization Scheme");
                result = MU_FAILURE;
                break;
            }
            case IOTHUB_CREDENTIAL_TYPE_SAS_TOKEN:
            case IOTHUB_CREDENTIAL_TYPE_X509:
            case IOTHUB_CREDENTIAL_TYPE_X509_ECC:
            case IOTHUB_CREDENTIAL_TYPE_DEVICE_AUTH:
            {
                if (send_http_request(iotHubHttpApiExHandle, STRING_c_str(relativePathNotification), requestHttpHeaders, messageBody, NULL) != 0)
                {
                    LogError("unable to execute HTTPAPIEX_ExecuteRequest");
                    result = MU_FAILURE;
                }
                else
                {
                    result = 0;
                }
                break;
            }
            case IOTHUB_CREDENTIAL_TYPE_DEVICE_KEY:
            {
                STRING_HANDLE uriResource = STRING_construct_sprintf("%s/devices/%s/files/notifications", upload_data->hostname, upload_data->deviceId);
                if (uriResource == NULL)
                {
                    LogError("Failure constructing string");
                    result = MU_FAILURE;
                }
                else
                {
                    if (send_http_sas_request(upload_data, STRING_c_str(uriResource), iotHubHttpApiExHandle, STRING_c_str(relativePathNotification), requestHttpHeaders, messageBody, NULL) != 0)
                    {
                        LogError("unable to execute HTTPAPIEX_ExecuteRequest");
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                    STRING_delete(uriResource);
                }
                break;
            }
        }
        STRING_delete(relativePathNotification);
    }
    return result;
}

// this callback splits the source data into blocks to be fed to IoTHubClient_LL_UploadMultipleBlocksToBlob(Ex)_Impl
static IOTHUB_CLIENT_FILE_UPLOAD_GET_DATA_RESULT FileUpload_GetData_Callback(IOTHUB_CLIENT_FILE_UPLOAD_RESULT result, unsigned char const ** data, size_t* size, void* context)
{
    BLOB_UPLOAD_CONTEXT* uploadContext = (BLOB_UPLOAD_CONTEXT*) context;

    if (data == NULL || size == NULL)
    {
        // This is the last call, nothing to do
    }
    else if (result != FILE_UPLOAD_OK)
    {
        // Last call failed
        *data = NULL;
        *size = 0;
    }
    else if (uploadContext->remainingSizeToUpload == 0)
    {
        // Everything has been uploaded
        *data = NULL;
        *size = 0;
    }
    else
    {
        // Upload next block
        size_t thisBlockSize = (uploadContext->remainingSizeToUpload > BLOCK_SIZE) ? BLOCK_SIZE : uploadContext->remainingSizeToUpload;
        *data = (unsigned char*)uploadContext->blobSource + (uploadContext->blobSourceSize - uploadContext->remainingSizeToUpload);
        *size = thisBlockSize;
        uploadContext->remainingSizeToUpload -= thisBlockSize;
    }

    return IOTHUB_CLIENT_FILE_UPLOAD_GET_DATA_OK;
}

static HTTPAPIEX_RESULT set_transfer_timeout(IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE_DATA* upload_data, HTTPAPIEX_HANDLE iotHubHttpApiExHandle)
{
    HTTPAPIEX_RESULT result;
    if (upload_data->blob_upload_timeout_secs != 0)
    {
        // Convert the timeout to milliseconds for curl
        long http_timeout = (long)upload_data->blob_upload_timeout_secs * 1000;
        result = HTTPAPIEX_SetOption(iotHubHttpApiExHandle, OPTION_HTTP_TIMEOUT, &http_timeout);
    }
    else
    {
        result = HTTPAPIEX_OK;
    }
    return result;
}

IOTHUB_CLIENT_RESULT IoTHubClient_LL_UploadMultipleBlocksToBlob_Impl(IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE handle, const char* destinationFileName, IOTHUB_CLIENT_FILE_UPLOAD_GET_DATA_CALLBACK_EX getDataCallbackEx, void* context)
{
    IOTHUB_CLIENT_RESULT result;

    /*Codes_SRS_IOTHUBCLIENT_LL_02_061: [ If handle is NULL then IoTHubClient_LL_UploadMultipleBlocksToBlob(Ex) shall fail and return IOTHUB_CLIENT_INVALID_ARG. ]*/
    /*Codes_SRS_IOTHUBCLIENT_LL_02_062: [ If destinationFileName is NULL then IoTHubClient_LL_UploadMultipleBlocksToBlob(Ex) shall fail and return IOTHUB_CLIENT_INVALID_ARG. ]*/

    if (handle == NULL || destinationFileName == NULL || getDataCallbackEx == NULL)
    {
        LogError("invalid argument detected handle=%p destinationFileName=%p getDataCallbackEx=%p", handle, destinationFileName, getDataCallbackEx);
        result = IOTHUB_CLIENT_INVALID_ARG;
    }
    else
    {
        IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE_DATA* upload_data = (IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE_DATA*)handle;

        /*Codes_SRS_IOTHUBCLIENT_LL_02_064: [ IoTHubClient_LL_UploadMultipleBlocksToBlob(Ex) shall create an HTTPAPIEX_HANDLE to the IoTHub hostname. ]*/
        HTTPAPIEX_HANDLE iotHubHttpApiExHandle = HTTPAPIEX_Create(upload_data->hostname);
        /*Codes_SRS_IOTHUBCLIENT_LL_02_065: [ If creating the HTTPAPIEX_HANDLE fails then IoTHubClient_LL_UploadMultipleBlocksToBlob(Ex) shall fail and return IOTHUB_CLIENT_ERROR. ]*/
        if (iotHubHttpApiExHandle == NULL)
        {
            LogError("unable to HTTPAPIEX_Create");
            result = IOTHUB_CLIENT_ERROR;
        }
        /*Codes_SRS_IOTHUBCLIENT_LL_30_020: [ If the blob_upload_timeout_secs option has been set to non-zero, IoTHubClient_LL_UploadMultipleBlocksToBlob(Ex) shall set the timeout on the underlying transport accordingly. ]*/
        else if (set_transfer_timeout(upload_data, iotHubHttpApiExHandle) != HTTPAPIEX_OK)
        {
            LogError("unable to set blob transfer timeout");
            result = IOTHUB_CLIENT_ERROR;
        }
        else
        {
            if (upload_data->curl_verbosity_level != UPOADTOBLOB_CURL_VERBOSITY_UNSET)
            {
                size_t curl_verbose = (upload_data->curl_verbosity_level == UPOADTOBLOB_CURL_VERBOSITY_ON);
                (void)HTTPAPIEX_SetOption(iotHubHttpApiExHandle, OPTION_CURL_VERBOSE, &curl_verbose);
            }

            /*transmit the x509certificate and x509privatekey*/
            /*Codes_SRS_IOTHUBCLIENT_LL_02_106: [ - x509certificate and x509privatekey saved options shall be passed on the HTTPAPIEX_SetOption ]*/
            if ((upload_data->cred_type == IOTHUB_CREDENTIAL_TYPE_X509 || upload_data->cred_type == IOTHUB_CREDENTIAL_TYPE_X509_ECC) &&
                ((HTTPAPIEX_SetOption(iotHubHttpApiExHandle, OPTION_X509_CERT, upload_data->credentials.x509_credentials.x509certificate) != HTTPAPIEX_OK) ||
                (HTTPAPIEX_SetOption(iotHubHttpApiExHandle, OPTION_X509_PRIVATE_KEY, upload_data->credentials.x509_credentials.x509privatekey) != HTTPAPIEX_OK))
                )
            {
                LogError("unable to HTTPAPIEX_SetOption for x509 certificate");
                result = IOTHUB_CLIENT_ERROR;
            }
            else
            {
                /*Codes_SRS_IOTHUBCLIENT_LL_02_111: [ If certificates is non-NULL then certificates shall be passed to HTTPAPIEX_SetOption with optionName TrustedCerts. ]*/
                if ((upload_data->certificates != NULL) && (HTTPAPIEX_SetOption(iotHubHttpApiExHandle, OPTION_TRUSTED_CERT, upload_data->certificates) != HTTPAPIEX_OK))
                {
                    LogError("unable to set TrustedCerts!");
                    result = IOTHUB_CLIENT_ERROR;
                }
                else
                {

                    if (upload_data->http_proxy_options.host_address != NULL)
                    {
                        HTTP_PROXY_OPTIONS proxy_options;
                        proxy_options = upload_data->http_proxy_options;

                        if (HTTPAPIEX_SetOption(iotHubHttpApiExHandle, OPTION_HTTP_PROXY, &proxy_options) != HTTPAPIEX_OK)
                        {
                            LogError("unable to set http proxy!");
                            result = IOTHUB_CLIENT_ERROR;
                        }
                        else
                        {
                            result = IOTHUB_CLIENT_OK;
                        }
                    }
                    else
                    {
                        result = IOTHUB_CLIENT_OK;
                    }

                    if (result != IOTHUB_CLIENT_ERROR)
                    {
                        STRING_HANDLE sasUri;
                        STRING_HANDLE correlationId;
                        if ((correlationId = STRING_new()) == NULL)
                        {
                            LogError("unable to STRING_new");
                            result = IOTHUB_CLIENT_ERROR;
                        }
                        else if ((sasUri = STRING_new()) == NULL)
                        {
                            LogError("unable to create sas uri");
                            result = IOTHUB_CLIENT_ERROR;
                            STRING_delete(correlationId);
                        }
                        else
                        {
                            /*Codes_SRS_IOTHUBCLIENT_LL_02_070: [ IoTHubClient_LL_UploadMultipleBlocksToBlob(Ex) shall create request HTTP headers. ]*/
                            HTTP_HEADERS_HANDLE requestHttpHeaders = HTTPHeaders_Alloc(); /*these are build by step 1 and used by step 3 too*/
                            if (requestHttpHeaders == NULL)
                            {
                                LogError("unable to HTTPHeaders_Alloc");
                                result = IOTHUB_CLIENT_ERROR;
                            }
                            else
                            {
                                /*do step 1*/
                                if (IoTHubClient_LL_UploadToBlob_step1and2(upload_data, iotHubHttpApiExHandle, requestHttpHeaders, destinationFileName, correlationId, sasUri) != 0)
                                {
                                    LogError("error in IoTHubClient_LL_UploadToBlob_step1");
                                    result = IOTHUB_CLIENT_ERROR;
                                }
                                else
                                {
                                    /*do step 2.*/

                                    unsigned int httpResponse;
                                    BUFFER_HANDLE responseToIoTHub = BUFFER_new();
                                    if (responseToIoTHub == NULL)
                                    {
                                        result = IOTHUB_CLIENT_ERROR;
                                        LogError("unable to BUFFER_new");
                                    }
                                    else
                                    {
                                        /*Codes_SRS_IOTHUBCLIENT_LL_02_083: [ IoTHubClient_LL_UploadMultipleBlocksToBlob(Ex) shall call Blob_UploadFromSasUri and capture the HTTP return code and HTTP body. ]*/
                                        BLOB_RESULT uploadMultipleBlocksResult = Blob_UploadMultipleBlocksFromSasUri(STRING_c_str(sasUri), getDataCallbackEx, context, &httpResponse, responseToIoTHub, upload_data->certificates, &(upload_data->http_proxy_options));
                                        if (uploadMultipleBlocksResult == BLOB_ABORTED)
                                        {
                                            /*Codes_SRS_IOTHUBCLIENT_LL_99_008: [ If step 2 is aborted by the client, then the HTTP message body shall look like:  ]*/
                                            LogInfo("Blob_UploadFromSasUri aborted file upload");

                                            STRING_HANDLE aborted_response = STRING_construct_sprintf(RESPONSE_BODY_FORMAT,
                                                                                        STRING_c_str(correlationId),
                                                                                        RESPONSE_BODY_ERROR_BOOLEAN_STRING,
                                                                                        RESPONSE_BODY_ERROR_RETURN_CODE,
                                                                                        RESPONSE_BODY_ABORTED_MESSAGE);
                                            if(aborted_response == NULL)
                                            {
                                                LogError("STRING_construct_sprintf failed");
                                                result = IOTHUB_CLIENT_ERROR;
                                            }
                                            else
                                            {
                                                size_t response_length = STRING_length(aborted_response);
                                                if (BUFFER_build(responseToIoTHub, (const unsigned char*)STRING_c_str(aborted_response), response_length) == 0)
                                                {
                                                    if (IoTHubClient_LL_UploadToBlob_step3(upload_data, iotHubHttpApiExHandle, requestHttpHeaders, responseToIoTHub) != 0)
                                                    {
                                                        LogError("IoTHubClient_LL_UploadToBlob_step3 failed");
                                                        result = IOTHUB_CLIENT_ERROR;
                                                    }
                                                    else
                                                    {
                                                        /*Codes_SRS_IOTHUBCLIENT_LL_99_009: [ If step 2 is aborted by the client and if step 3 succeeds, then `IoTHubClient_LL_UploadMultipleBlocksToBlob(Ex)` shall return `IOTHUB_CLIENT_OK`. ] */
                                                        result = IOTHUB_CLIENT_OK;
                                                    }
                                                }
                                                else
                                                {
                                                    LogError("Unable to BUFFER_build, can't perform IoTHubClient_LL_UploadToBlob_step3");
                                                    result = IOTHUB_CLIENT_ERROR;
                                                }
                                                STRING_delete(aborted_response);
                                            }
                                        }
                                        else if (uploadMultipleBlocksResult != BLOB_OK)
                                        {
                                            /*Codes_SRS_IOTHUBCLIENT_LL_02_084: [ If Blob_UploadFromSasUri fails then IoTHubClient_LL_UploadMultipleBlocksToBlob(Ex) shall fail and return IOTHUB_CLIENT_ERROR. ]*/
                                            LogError("unable to Blob_UploadFromSasUri");

                                            /*do step 3*/ /*try*/
                                            /*Codes_SRS_IOTHUBCLIENT_LL_02_091: [ If step 2 fails without establishing an HTTP dialogue, then the HTTP message body shall look like: ]*/
                                            STRING_HANDLE failed_response = STRING_construct_sprintf(RESPONSE_BODY_FORMAT, 
                                                                                        STRING_c_str(correlationId), 
                                                                                        RESPONSE_BODY_ERROR_BOOLEAN_STRING,
                                                                                        RESPONSE_BODY_ERROR_RETURN_CODE,
                                                                                        RESPONSE_BODY_FAILED_MESSAGE);
                                            if(failed_response == NULL)
                                            {
                                                LogError("STRING_construct_sprintf failed");
                                                result = IOTHUB_CLIENT_ERROR;
                                            }
                                            else
                                            {
                                                size_t response_length = STRING_length(failed_response);
                                                if (BUFFER_build(responseToIoTHub, (const unsigned char*)STRING_c_str(failed_response), response_length) == 0)
                                                {
                                                    if (IoTHubClient_LL_UploadToBlob_step3(upload_data, iotHubHttpApiExHandle, requestHttpHeaders, responseToIoTHub) != 0)
                                                    {
                                                        LogError("IoTHubClient_LL_UploadToBlob_step3 failed");
                                                    }
                                                }
                                                STRING_delete(failed_response);
                                                result = IOTHUB_CLIENT_ERROR;
                                            }
                                        }
                                        else
                                        {
                                            /*must make a json*/
                                            unsigned char * response = BUFFER_u_char(responseToIoTHub);
                                            STRING_HANDLE req_string;
                                            req_string = STRING_construct_sprintf(RESPONSE_BODY_FORMAT,
                                                                                        STRING_c_str(correlationId),
                                                                                        ((httpResponse < 300) ? "true" : "false"),
                                                                                        httpResponse, 
                                                                                        (response == NULL ? (const unsigned char*)"" : response));
                                            if (req_string == NULL)
                                            {
                                                LogError("Failure constructing string");
                                                result = IOTHUB_CLIENT_ERROR;
                                            }
                                            else
                                            {
                                                /*do again snprintf*/
                                                BUFFER_HANDLE toBeTransmitted = NULL;
                                                size_t req_string_len = STRING_length(req_string);
                                                const char* required_string = STRING_c_str(req_string);
                                                if ((toBeTransmitted = BUFFER_create((const unsigned char*)required_string, req_string_len)) == NULL)
                                                {
                                                    LogError("unable to BUFFER_create");
                                                    result = IOTHUB_CLIENT_ERROR;
                                                }
                                                else
                                                {
                                                    if (IoTHubClient_LL_UploadToBlob_step3(upload_data, iotHubHttpApiExHandle, requestHttpHeaders, toBeTransmitted) != 0)
                                                    {
                                                        LogError("IoTHubClient_LL_UploadToBlob_step3 failed");
                                                        result = IOTHUB_CLIENT_ERROR;
                                                    }
                                                    else
                                                    {
                                                        result = (httpResponse < 300) ? IOTHUB_CLIENT_OK : IOTHUB_CLIENT_ERROR;
                                                    }
                                                    BUFFER_delete(toBeTransmitted);
                                                }
                                                STRING_delete(req_string);
                                            }
                                        }
                                        BUFFER_delete(responseToIoTHub);
                                    }
                                }
                                HTTPHeaders_Free(requestHttpHeaders);
                            }
                            STRING_delete(sasUri);
                            STRING_delete(correlationId);
                        }
                    }
                }
            }
            HTTPAPIEX_Destroy(iotHubHttpApiExHandle);
        }

        /*Codes_SRS_IOTHUBCLIENT_LL_99_003: [ If `IoTHubClient_LL_UploadMultipleBlocksToBlob(Ex)` return `IOTHUB_CLIENT_OK`, it shall call `getDataCallbackEx` with `result` set to `FILE_UPLOAD_OK`, and `data` and `size` set to NULL. ]*/
        /*Codes_SRS_IOTHUBCLIENT_LL_99_004: [ If `IoTHubClient_LL_UploadMultipleBlocksToBlob(Ex)` does not return `IOTHUB_CLIENT_OK`, it shall call `getDataCallbackEx` with `result` set to `FILE_UPLOAD_ERROR`, and `data` and `size` set to NULL. ]*/
        (void)getDataCallbackEx(result == IOTHUB_CLIENT_OK ? FILE_UPLOAD_OK : FILE_UPLOAD_ERROR, NULL, NULL, context);
    }
    return result;
}

IOTHUB_CLIENT_RESULT IoTHubClient_LL_UploadToBlob_Impl(IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE handle, const char* destinationFileName, const unsigned char* source, size_t size)
{
    IOTHUB_CLIENT_RESULT result;

    if (handle == NULL || destinationFileName == NULL)
    {
        LogError("Invalid parameter handle:%p destinationFileName:%p", handle, destinationFileName);
        result = IOTHUB_CLIENT_INVALID_ARG;
    }
    /*Codes_SRS_IOTHUBCLIENT_LL_02_063: [ If source is NULL and size is greater than 0 then IoTHubClient_LL_UploadToBlob shall fail and return IOTHUB_CLIENT_INVALID_ARG. ]*/
    else if (source == NULL && size > 0)
    {
        LogError("Invalid source and size combination: source=%p size=%lu", source, (unsigned long)size);
        result = IOTHUB_CLIENT_INVALID_ARG;
    }
    else
    {
        /*Codes_SRS_IOTHUBCLIENT_LL_99_001: [ `IoTHubClient_LL_UploadToBlob` shall create a struct containing the `source`, the `size`, and the remaining size to upload.]*/
        BLOB_UPLOAD_CONTEXT context;
        context.blobSource = source;
        context.blobSourceSize = size;
        context.remainingSizeToUpload = size;

        /*Codes_SRS_IOTHUBCLIENT_LL_99_002: [ `IoTHubClient_LL_UploadToBlob` shall call `IoTHubClient_LL_UploadMultipleBlocksToBlob_Impl` with `FileUpload_GetData_Callback` as `getDataCallbackEx` and pass the struct created at step SRS_IOTHUBCLIENT_LL_99_001 as `context` ]*/
        result = IoTHubClient_LL_UploadMultipleBlocksToBlob_Impl(handle, destinationFileName, FileUpload_GetData_Callback, &context);
    }
    return result;
}

void IoTHubClient_LL_UploadToBlob_Destroy(IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE handle)
{
    if (handle == NULL)
    {
        LogError("unexpected NULL argument");
    }
    else
    {
        IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE_DATA* upload_data = (IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE_DATA*)handle;

        if (upload_data->cred_type == IOTHUB_CREDENTIAL_TYPE_X509 || upload_data->cred_type == IOTHUB_CREDENTIAL_TYPE_X509_ECC)
        {
            free(upload_data->credentials.x509_credentials.x509certificate);
            free(upload_data->credentials.x509_credentials.x509privatekey);
        }
        else if (upload_data->cred_type == IOTHUB_CREDENTIAL_TYPE_SAS_TOKEN)
        {
            free(upload_data->credentials.supplied_sas_token);
        }

        free((void*)upload_data->hostname);
        if (upload_data->certificates != NULL)
        {
            free(upload_data->certificates);
        }
        if (upload_data->http_proxy_options.host_address != NULL)
        {
            free((char *)upload_data->http_proxy_options.host_address);
        }
        if (upload_data->http_proxy_options.username != NULL)
        {
            free((char *)upload_data->http_proxy_options.username);
        }
        if (upload_data->http_proxy_options.password != NULL)
        {
            free((char *)upload_data->http_proxy_options.password);
        }
        free(upload_data);
    }
}

IOTHUB_CLIENT_RESULT IoTHubClient_LL_UploadToBlob_SetOption(IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE handle, const char* optionName, const void* value)
{
    IOTHUB_CLIENT_RESULT result;
    /*Codes_SRS_IOTHUBCLIENT_LL_02_110: [ If parameter handle is NULL then IoTHubClient_LL_UploadToBlob_SetOption shall fail and return IOTHUB_CLIENT_ERROR. ]*/
    if (handle == NULL)
    {
        LogError("invalid argument detected: IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE handle=%p, const char* optionName=%s, const void* value=%p", handle, optionName, value);
        result = IOTHUB_CLIENT_ERROR;
    }
    else
    {
        IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE_DATA* upload_data = (IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE_DATA*)handle;

        /*Codes_SRS_IOTHUBCLIENT_LL_02_100: [ x509certificate - then value then is a null terminated string that contains the x509 certificate. ]*/
        if (strcmp(optionName, OPTION_X509_CERT) == 0)
        {
            /*Codes_SRS_IOTHUBCLIENT_LL_02_109: [ If the authentication scheme is NOT x509 then IoTHubClient_LL_UploadToBlob_SetOption shall return IOTHUB_CLIENT_INVALID_ARG. ]*/
            if (upload_data->cred_type != IOTHUB_CREDENTIAL_TYPE_X509)
            {
                LogError("trying to set a x509 certificate while the authentication scheme is not x509");
                result = IOTHUB_CLIENT_INVALID_ARG;
            }
            else
            {
                /*Codes_SRS_IOTHUBCLIENT_LL_02_103: [ The options shall be saved. ]*/
                /*try to make a copy of the certificate*/
                char* temp;
                if (mallocAndStrcpy_s(&temp, value) != 0)
                {
                    /*Codes_SRS_IOTHUBCLIENT_LL_02_104: [ If saving fails, then IoTHubClient_LL_UploadToBlob_SetOption shall fail and return IOTHUB_CLIENT_ERROR. ]*/
                    LogError("unable to mallocAndStrcpy_s");
                    result = IOTHUB_CLIENT_ERROR;
                }
                else
                {
                    /*Codes_SRS_IOTHUBCLIENT_LL_02_105: [ Otherwise IoTHubClient_LL_UploadToBlob_SetOption shall succeed and return IOTHUB_CLIENT_OK. ]*/
                    if (upload_data->credentials.x509_credentials.x509certificate != NULL) /*free any previous values, if any*/
                    {
                        free(upload_data->credentials.x509_credentials.x509certificate);
                    }
                    upload_data->credentials.x509_credentials.x509certificate = temp;
                    result = IOTHUB_CLIENT_OK;
                }
            }
        }
        /*Codes_SRS_IOTHUBCLIENT_LL_02_101: [ x509privatekey - then value is a null terminated string that contains the x509 privatekey. ]*/
        else if (strcmp(optionName, OPTION_X509_PRIVATE_KEY) == 0)
        {
            /*Codes_SRS_IOTHUBCLIENT_LL_02_109: [ If the authentication scheme is NOT x509 then IoTHubClient_LL_UploadToBlob_SetOption shall return IOTHUB_CLIENT_INVALID_ARG. ]*/
            if (upload_data->cred_type != IOTHUB_CREDENTIAL_TYPE_X509)
            {
                LogError("trying to set a x509 privatekey while the authentication scheme is not x509");
                result = IOTHUB_CLIENT_INVALID_ARG;
            }
            else
            {
                /*Codes_SRS_IOTHUBCLIENT_LL_02_103: [ The options shall be saved. ]*/
                /*try to make a copy of the privatekey*/
                char* temp;
                if (mallocAndStrcpy_s(&temp, value) != 0)
                {
                    /*Codes_SRS_IOTHUBCLIENT_LL_02_104: [ If saving fails, then IoTHubClient_LL_UploadToBlob_SetOption shall fail and return IOTHUB_CLIENT_ERROR. ]*/
                    LogError("unable to mallocAndStrcpy_s");
                    result = IOTHUB_CLIENT_ERROR;
                }
                else
                {
                    /*Codes_SRS_IOTHUBCLIENT_LL_02_105: [ Otherwise IoTHubClient_LL_UploadToBlob_SetOption shall succeed and return IOTHUB_CLIENT_OK. ]*/
                    if (upload_data->credentials.x509_credentials.x509privatekey != NULL) /*free any previous values, if any*/
                    {
                        free(upload_data->credentials.x509_credentials.x509privatekey);
                    }
                    upload_data->credentials.x509_credentials.x509privatekey = temp;
                    result = IOTHUB_CLIENT_OK;
                }
            }
        }
        else if (strcmp(OPTION_TRUSTED_CERT, optionName) == 0)
        {
            if (value == NULL)
            {
                LogError("NULL is a not a valid value for TrustedCerts");
                result = IOTHUB_CLIENT_INVALID_ARG;
            }
            else
            {
                char* tempCopy;
                if (mallocAndStrcpy_s(&tempCopy, value) != 0)
                {
                    LogError("failure in mallocAndStrcpy_s");
                    result = IOTHUB_CLIENT_ERROR;
                }
                else
                {
                    if (upload_data->certificates != NULL)
                    {
                        free(upload_data->certificates);
                    }
                    upload_data->certificates = tempCopy;
                    result = IOTHUB_CLIENT_OK;
                }
            }
        }
        /*Codes_SRS_IOTHUBCLIENT_LL_32_008: [ OPTION_HTTP_PROXY - then the value will be a pointer to HTTP_PROXY_OPTIONS structure. ]*/
        else if (strcmp(optionName, OPTION_HTTP_PROXY) == 0)
        {
            HTTP_PROXY_OPTIONS* proxy_options = (HTTP_PROXY_OPTIONS *)value;

            if (proxy_options->host_address == NULL)
            {
                /* Codes_SRS_IOTHUBCLIENT_LL_32_006: [ If `host_address` is NULL, `IoTHubClient_LL_UploadToBlob_SetOption` shall fail and return `IOTHUB_CLIENT_INVALID_ARG`. ]*/
                LogError("NULL host_address in proxy options");
                result = IOTHUB_CLIENT_INVALID_ARG;
            }
            /* Codes_SRS_IOTHUBCLIENT_LL_32_007: [ If only one of `username` and `password` is NULL, `IoTHubClient_LL_UploadToBlob_SetOption` shall fail and return `IOTHUB_CLIENT_INVALID_ARG`. ]*/
            else if (((proxy_options->username == NULL) || (proxy_options->password == NULL)) &&
                (proxy_options->username != proxy_options->password))
            {
                LogError("Only one of username and password for proxy settings was NULL");
                result = IOTHUB_CLIENT_INVALID_ARG;
            }
            else
            {
                if (upload_data->http_proxy_options.host_address != NULL)
                {
                    free((char *)upload_data->http_proxy_options.host_address);
                    upload_data->http_proxy_options.host_address = NULL;
                }
                if (upload_data->http_proxy_options.username != NULL)
                {
                    free((char *)upload_data->http_proxy_options.username);
                    upload_data->http_proxy_options.username = NULL;
                }
                if (upload_data->http_proxy_options.password != NULL)
                {
                    free((char *)upload_data->http_proxy_options.password);
                    upload_data->http_proxy_options.password = NULL;
                }

                upload_data->http_proxy_options.port = proxy_options->port;

                if (mallocAndStrcpy_s((char **)(&upload_data->http_proxy_options.host_address), proxy_options->host_address) != 0)
                {
                    LogError("failure in mallocAndStrcpy_s - upload_data->http_proxy_options.host_address");
                    result = IOTHUB_CLIENT_ERROR;
                }
                else if (proxy_options->username != NULL && mallocAndStrcpy_s((char **)(&upload_data->http_proxy_options.username), proxy_options->username) != 0)
                {
                    LogError("failure in mallocAndStrcpy_s - upload_data->http_proxy_options.username");
                    result = IOTHUB_CLIENT_ERROR;
                }
                else if (proxy_options->password != NULL && mallocAndStrcpy_s((char **)(&upload_data->http_proxy_options.password), proxy_options->password) != 0)
                {
                    LogError("failure in mallocAndStrcpy_s - upload_data->http_proxy_options.password");
                    result = IOTHUB_CLIENT_ERROR;
                }
                else
                {
                    result = IOTHUB_CLIENT_OK;
                }
            }
        }
        else if (strcmp(optionName, OPTION_CURL_VERBOSE) == 0)
        {
            upload_data->curl_verbosity_level = ((*(bool*)value) == 0) ? UPOADTOBLOB_CURL_VERBOSITY_OFF : UPOADTOBLOB_CURL_VERBOSITY_ON;
            result = IOTHUB_CLIENT_OK;
        }
        else if (strcmp(optionName, OPTION_BLOB_UPLOAD_TIMEOUT_SECS) == 0)
        {
            upload_data->blob_upload_timeout_secs = *(size_t*)value;
            result = IOTHUB_CLIENT_OK;
        }
        else
        {
            /*Codes_SRS_IOTHUBCLIENT_LL_02_102: [ If an unknown option is presented then IoTHubClient_LL_UploadToBlob_SetOption shall return IOTHUB_CLIENT_INVALID_ARG. ]*/
            result = IOTHUB_CLIENT_INVALID_ARG;
        }
    }
    return result;
}

#endif /*DONT_USE_UPLOADTOBLOB*/


