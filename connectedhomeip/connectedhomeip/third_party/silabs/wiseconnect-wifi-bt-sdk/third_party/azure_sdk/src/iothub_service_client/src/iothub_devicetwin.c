// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <ctype.h>
#include "azure_c_shared_utility/optimize_size.h"
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/string_tokenizer.h"
#include "azure_c_shared_utility/singlylinkedlist.h"
#include "azure_c_shared_utility/buffer_.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/httpapiex.h"
#include "azure_c_shared_utility/httpapiexsas.h"
#include "azure_c_shared_utility/azure_base64.h"
#include "azure_c_shared_utility/uniqueid.h"
#include "azure_c_shared_utility/connection_string_parser.h"

#include "parson.h"
#include "iothub_devicetwin.h"
#include "iothub_sc_version.h"

#define IOTHUB_TWIN_REQUEST_MODE_VALUES    \
    IOTHUB_TWIN_REQUEST_GET,               \
    IOTHUB_TWIN_REQUEST_UPDATE,            \
    IOTHUB_TWIN_REQUEST_REPLACE_TAGS,      \
    IOTHUB_TWIN_REQUEST_REPLACE_DESIRED,   \
    IOTHUB_TWIN_REQUEST_UPDATE_DESIRED

MU_DEFINE_ENUM(IOTHUB_TWIN_REQUEST_MODE, IOTHUB_TWIN_REQUEST_MODE_VALUES);
MU_DEFINE_ENUM_STRINGS_WITHOUT_INVALID(IOTHUB_DEVICE_TWIN_RESULT, IOTHUB_DEVICE_TWIN_RESULT_VALUES);

#define  HTTP_HEADER_KEY_AUTHORIZATION  "Authorization"
#define  HTTP_HEADER_VAL_AUTHORIZATION  " "
#define  HTTP_HEADER_KEY_REQUEST_ID  "Request-Id"
#define  HTTP_HEADER_KEY_USER_AGENT  "User-Agent"
#define  HTTP_HEADER_VAL_USER_AGENT  IOTHUB_SERVICE_CLIENT_TYPE_PREFIX IOTHUB_SERVICE_CLIENT_BACKSLASH IOTHUB_SERVICE_CLIENT_VERSION
#define  HTTP_HEADER_KEY_ACCEPT  "Accept"
#define  HTTP_HEADER_VAL_ACCEPT  "application/json"
#define  HTTP_HEADER_KEY_CONTENT_TYPE  "Content-Type"
#define  HTTP_HEADER_VAL_CONTENT_TYPE  "application/json; charset=utf-8"
#define  HTTP_HEADER_KEY_IFMATCH  "If-Match"
#define  HTTP_HEADER_VAL_IFMATCH  "*"
#define UID_LENGTH 37

static const char* URL_API_VERSION = "?api-version=2020-09-30";

static const char* RELATIVE_PATH_FMT_TWIN = "/twins/%s%s";
static const char* RELATIVE_PATH_FMT_TWIN_MODULE = "/twins/%s/modules/%s%s";


/** @brief Structure to store IoTHub authentication information
*/
typedef struct IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_TAG
{
    char* hostname;
    char* sharedAccessKey;
    char* keyName;
} IOTHUB_SERVICE_CLIENT_DEVICE_TWIN;

static const char* generateGuid(void)
{
    char* result;

    if ((result = malloc(UID_LENGTH)) != NULL)
    {
        result[0] = '\0';
        if (UniqueId_Generate(result, UID_LENGTH) != UNIQUEID_OK)
        {
            free((void*)result);
            result = NULL;
        }
    }
    return (const char*)result;
}

static STRING_HANDLE createRelativePath(IOTHUB_TWIN_REQUEST_MODE iotHubTwinRequestMode, const char* deviceId, const char* moduleId)
{
    //IOTHUB_TWIN_REQUEST_GET               GET      {iot hub}/twins/{device id}                     // Get device twin
    //IOTHUB_TWIN_REQUEST_UPDATE            PATCH    {iot hub}/twins/{device id}                     // Partally update device twin
    //IOTHUB_TWIN_REQUEST_REPLACE_TAGS      PUT      {iot hub}/twins/{device id}/tags                // Replace update tags
    //IOTHUB_TWIN_REQUEST_REPLACE_DESIRED   PUT      {iot hub}/twins/{device id}/properties/desired  // Replace update desired properties
    //IOTHUB_TWIN_REQUEST_UPDATE_DESIRED    PATCH    {iot hub}/twins/{device id}/properties/desired  // Partially update desired properties

    STRING_HANDLE result;

    if ((iotHubTwinRequestMode == IOTHUB_TWIN_REQUEST_GET) || (iotHubTwinRequestMode == IOTHUB_TWIN_REQUEST_UPDATE))
    {
        if (moduleId == NULL)
        {
            result = STRING_construct_sprintf(RELATIVE_PATH_FMT_TWIN, deviceId, URL_API_VERSION);
        }
        else
        {
            result = STRING_construct_sprintf(RELATIVE_PATH_FMT_TWIN_MODULE, deviceId, moduleId, URL_API_VERSION);
        }
    }
    else
    {
        result = NULL;
    }
    return result;
}

static HTTP_HEADERS_HANDLE createHttpHeader(IOTHUB_TWIN_REQUEST_MODE iotHubTwinRequestMode)
{
    /*Codes_SRS_IOTHUBDEVICETWIN_12_020: [ IoTHubDeviceTwin_GetTwin shall add the following headers to the created HTTP GET request: authorization=sasToken,Request-Id=1001,Accept=application/json,Content-Type=application/json,charset=utf-8 ]*/
    HTTP_HEADERS_HANDLE httpHeader;
    const char* guid = NULL;

    if ((httpHeader = HTTPHeaders_Alloc()) == NULL)
    {
        LogError("HTTPHeaders_Alloc failed");
    }
    else if (HTTPHeaders_AddHeaderNameValuePair(httpHeader, HTTP_HEADER_KEY_AUTHORIZATION, HTTP_HEADER_VAL_AUTHORIZATION) != HTTP_HEADERS_OK)
    {
        LogError("HTTPHeaders_AddHeaderNameValuePair failed for Authorization header");
        HTTPHeaders_Free(httpHeader);
        httpHeader = NULL;
    }
    else if ((guid = generateGuid()) == NULL)
    {
        LogError("GUID creation failed");
        HTTPHeaders_Free(httpHeader);
        httpHeader = NULL;
    }
    else if (HTTPHeaders_AddHeaderNameValuePair(httpHeader, HTTP_HEADER_KEY_REQUEST_ID, guid) != HTTP_HEADERS_OK)
    {
        LogError("HTTPHeaders_AddHeaderNameValuePair failed for RequestId header");
        HTTPHeaders_Free(httpHeader);
        httpHeader = NULL;
    }
    else if (HTTPHeaders_AddHeaderNameValuePair(httpHeader, HTTP_HEADER_KEY_USER_AGENT, HTTP_HEADER_VAL_USER_AGENT) != HTTP_HEADERS_OK)
    {
        LogError("HTTPHeaders_AddHeaderNameValuePair failed for User-Agent header");
        HTTPHeaders_Free(httpHeader);
        httpHeader = NULL;
    }
    else if (HTTPHeaders_AddHeaderNameValuePair(httpHeader, HTTP_HEADER_KEY_CONTENT_TYPE, HTTP_HEADER_VAL_CONTENT_TYPE) != HTTP_HEADERS_OK)
    {
        LogError("HTTPHeaders_AddHeaderNameValuePair failed for Content-Type header");
        HTTPHeaders_Free(httpHeader);
        httpHeader = NULL;
    }
    else if (iotHubTwinRequestMode != IOTHUB_TWIN_REQUEST_GET)
    {
        if (HTTPHeaders_AddHeaderNameValuePair(httpHeader, HTTP_HEADER_KEY_IFMATCH, HTTP_HEADER_VAL_IFMATCH) != HTTP_HEADERS_OK)
        {
            LogError("HTTPHeaders_AddHeaderNameValuePair failed for If-Match header");
            HTTPHeaders_Free(httpHeader);
            httpHeader = NULL;
        }
    }
    free((void*)guid);

    return httpHeader;
}

static IOTHUB_DEVICE_TWIN_RESULT sendHttpRequestTwin(IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE serviceClientDeviceTwinHandle, IOTHUB_TWIN_REQUEST_MODE iotHubTwinRequestMode, const char* deviceName, const char* moduleId, BUFFER_HANDLE deviceJsonBuffer, BUFFER_HANDLE responseBuffer)
{
    IOTHUB_DEVICE_TWIN_RESULT result;

    STRING_HANDLE uriResource = NULL;
    STRING_HANDLE accessKey = NULL;
    STRING_HANDLE keyName = NULL;
    HTTPAPIEX_SAS_HANDLE httpExApiSasHandle;
    HTTPAPIEX_HANDLE httpExApiHandle;
    HTTP_HEADERS_HANDLE httpHeader;

    if ((uriResource = STRING_construct(serviceClientDeviceTwinHandle->hostname)) == NULL)
    {
        /*Codes_SRS_IOTHUBDEVICETWIN_12_024: [ If any of the HTTPAPI call fails IoTHubDeviceTwin_GetTwin shall fail and return NULL ]*/
        LogError("STRING_construct failed for uriResource");
        result = IOTHUB_DEVICE_TWIN_ERROR;
    }
    else if ((accessKey = STRING_construct(serviceClientDeviceTwinHandle->sharedAccessKey)) == NULL)
    {
        /*Codes_SRS_IOTHUBDEVICETWIN_12_024: [ If any of the call fails during the HTTP creation IoTHubDeviceTwin_GetTwin shall fail and return NULL ]*/
        LogError("STRING_construct failed for accessKey");
        STRING_delete(uriResource);
        result = IOTHUB_DEVICE_TWIN_ERROR;
    }
    else if ((keyName = STRING_construct(serviceClientDeviceTwinHandle->keyName)) == NULL)
    {
        /*Codes_SRS_IOTHUBDEVICETWIN_12_024: [ If any of the call fails during the HTTP creation IoTHubDeviceTwin_GetTwin shall fail and return NULL ]*/
        LogError("STRING_construct failed for keyName");
        STRING_delete(accessKey);
        STRING_delete(uriResource);
        result = IOTHUB_DEVICE_TWIN_ERROR;
    }
    /*Codes_SRS_IOTHUBDEVICETWIN_12_020: [ IoTHubDeviceTwin_GetTwin shall add the following headers to the created HTTP GET request: authorization=sasToken,Request-Id=1001,Accept=application/json,Content-Type=application/json,charset=utf-8 ]*/
    else if ((httpHeader = createHttpHeader(iotHubTwinRequestMode)) == NULL)
    {
        /*Codes_SRS_IOTHUBDEVICETWIN_12_024: [ If any of the call fails during the HTTP creation IoTHubDeviceTwin_GetTwin shall fail and return NULL ]*/
        LogError("HttpHeader creation failed");
        STRING_delete(keyName);
        STRING_delete(accessKey);
        STRING_delete(uriResource);
        result = IOTHUB_DEVICE_TWIN_ERROR;
    }
    /*Codes_SRS_IOTHUBDEVICETWIN_12_021: [ IoTHubDeviceTwin_GetTwin shall create an HTTPAPIEX_SAS_HANDLE handle by calling HTTPAPIEX_SAS_Create ]*/
    else if ((httpExApiSasHandle = HTTPAPIEX_SAS_Create(accessKey, uriResource, keyName)) == NULL)
    {
        /*Codes_SRS_IOTHUBDEVICETWIN_12_025: [ If any of the HTTPAPI call fails IoTHubDeviceTwin_GetTwin shall fail and return IOTHUB_DEVICE_TWIN_HTTPAPI_ERROR ]*/
        LogError("HTTPAPIEX_SAS_Create failed");
        HTTPHeaders_Free(httpHeader);
        STRING_delete(keyName);
        STRING_delete(accessKey);
        STRING_delete(uriResource);
        result = IOTHUB_DEVICE_TWIN_HTTPAPI_ERROR;
    }
    /*Codes_SRS_IOTHUBDEVICETWIN_12_022: [ IoTHubDeviceTwin_GetTwin shall create an HTTPAPIEX_HANDLE handle by calling HTTPAPIEX_Create ]*/
    else if ((httpExApiHandle = HTTPAPIEX_Create(serviceClientDeviceTwinHandle->hostname)) == NULL)
    {
        /*Codes_SRS_IOTHUBDEVICETWIN_12_025: [ If any of the HTTPAPI call fails IoTHubDeviceTwin_GetTwin shall fail and return NULL ]*/
        LogError("HTTPAPIEX_Create failed");
        HTTPAPIEX_SAS_Destroy(httpExApiSasHandle);
        HTTPHeaders_Free(httpHeader);
        STRING_delete(keyName);
        STRING_delete(accessKey);
        STRING_delete(uriResource);
        result = IOTHUB_DEVICE_TWIN_HTTPAPI_ERROR;
    }
    else
    {
        HTTPAPI_REQUEST_TYPE httpApiRequestType = HTTPAPI_REQUEST_GET;
        STRING_HANDLE relativePath;
        unsigned int statusCode = 0;
        unsigned char is_error = 0;

        //IOTHUB_TWIN_REQUEST_GET               GET      {iot hub}/twins/{device id}                     // Get device twin
        //IOTHUB_TWIN_REQUEST_UPDATE            PATCH    {iot hub}/twins/{device id}                     // Partally update device twin
        //IOTHUB_TWIN_REQUEST_REPLACE_TAGS      PUT      {iot hub}/twins/{device id}/tags                // Replace update tags
        //IOTHUB_TWIN_REQUEST_REPLACE_DESIRED   PUT      {iot hub}/twins/{device id}/properties/desired  // Replace update desired properties
        //IOTHUB_TWIN_REQUEST_UPDATE_DESIRED    PATCH    {iot hub}/twins/{device id}/properties/desired  // Partially update desired properties

        if ((iotHubTwinRequestMode == IOTHUB_TWIN_REQUEST_REPLACE_TAGS) || (iotHubTwinRequestMode == IOTHUB_TWIN_REQUEST_REPLACE_DESIRED))
        {
            httpApiRequestType = HTTPAPI_REQUEST_PUT;
        }
        else if ((iotHubTwinRequestMode == IOTHUB_TWIN_REQUEST_UPDATE) || (iotHubTwinRequestMode == IOTHUB_TWIN_REQUEST_UPDATE_DESIRED))
        {
            httpApiRequestType = HTTPAPI_REQUEST_PATCH;
        }
        else if (iotHubTwinRequestMode == IOTHUB_TWIN_REQUEST_GET)
        {
            httpApiRequestType = HTTPAPI_REQUEST_GET;
        }
        else
        {
            is_error = 1;
        }

        if (is_error)
        {
            LogError("Invalid request type");
            result = IOTHUB_DEVICE_TWIN_HTTPAPI_ERROR;
        }
        else
        {
            /*Codes_SRS_IOTHUBDEVICETWIN_12_019: [ IoTHubDeviceTwin_GetTwin shall create HTTP GET request URL using the given deviceId using the following format: url/twins/[deviceId] ]*/
            if ((relativePath = createRelativePath(iotHubTwinRequestMode, deviceName, moduleId)) == NULL)
            {
                /*Codes_SRS_IOTHUBDEVICETWIN_12_024: [ If any of the call fails during the HTTP creation IoTHubDeviceTwin_GetTwin shall fail and return NULL ]*/
                LogError("Failure creating relative path");
                result = IOTHUB_DEVICE_TWIN_ERROR;
            }
            /*Codes_SRS_IOTHUBDEVICETWIN_12_023: [ IoTHubDeviceTwin_GetTwin shall execute the HTTP GET request by calling HTTPAPIEX_ExecuteRequest ]*/
            else if (HTTPAPIEX_SAS_ExecuteRequest(httpExApiSasHandle, httpExApiHandle, httpApiRequestType, STRING_c_str(relativePath), httpHeader, deviceJsonBuffer, &statusCode, NULL, responseBuffer) != HTTPAPIEX_OK)
            {
                /*Codes_SRS_IOTHUBDEVICETWIN_12_025: [ If any of the HTTPAPI call fails IoTHubDeviceTwin_GetTwin shall fail and return NULL ]*/
                LogError("HTTPAPIEX_SAS_ExecuteRequest failed");
                STRING_delete(relativePath);
                result = IOTHUB_DEVICE_TWIN_HTTPAPI_ERROR;
            }
            else
            {
                STRING_delete(relativePath);
                if (statusCode == 200)
                {
                    /*CodesSRS_IOTHUBDEVICETWIN_12_030: [ Otherwise IoTHubDeviceTwin_GetTwin shall save the received deviceTwin to the out parameter and return with it ]*/
                    result = IOTHUB_DEVICE_TWIN_OK;
                }
                else
                {
                    /*Codes_SRS_IOTHUBDEVICETWIN_12_026: [ IoTHubDeviceTwin_GetTwin shall verify the received HTTP status code and if it is not equal to 200 then return NULL ]*/
                    LogError("Http Failure status code %d.", statusCode);
                    result = IOTHUB_DEVICE_TWIN_ERROR;
                }
            }
        }
        HTTPAPIEX_Destroy(httpExApiHandle);
        HTTPAPIEX_SAS_Destroy(httpExApiSasHandle);
        HTTPHeaders_Free(httpHeader);
        STRING_delete(keyName);
        STRING_delete(accessKey);
        STRING_delete(uriResource);
    }
    return result;
}

static void free_devicetwin_handle(IOTHUB_SERVICE_CLIENT_DEVICE_TWIN* deviceTwin)
{
    free(deviceTwin->hostname);
    free(deviceTwin->sharedAccessKey);
    free(deviceTwin->keyName);
    free(deviceTwin);
}

IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE IoTHubDeviceTwin_Create(IOTHUB_SERVICE_CLIENT_AUTH_HANDLE serviceClientHandle)
{
    IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE result;

    /*Codes_SRS_IOTHUBDEVICETWIN_12_001: [ If the serviceClientHandle input parameter is NULL IoTHubDeviceTwin_Create shall return NULL ]*/
    if (serviceClientHandle == NULL)
    {
        LogError("serviceClientHandle input parameter cannot be NULL");
        result = NULL;
    }
    else
    {
        /*Codes_SRS_IOTHUBDEVICETWIN_12_002: [ If any member of the serviceClientHandle input parameter is NULL IoTHubDeviceTwin_Create shall return NULL ]*/
        IOTHUB_SERVICE_CLIENT_AUTH* serviceClientAuth = (IOTHUB_SERVICE_CLIENT_AUTH*)serviceClientHandle;

        if (serviceClientAuth->hostname == NULL)
        {
            LogError("authInfo->hostName input parameter cannot be NULL");
            result = NULL;
        }
        else if (serviceClientAuth->iothubName == NULL)
        {
            LogError("authInfo->iothubName input parameter cannot be NULL");
            result = NULL;
        }
        else if (serviceClientAuth->iothubSuffix == NULL)
        {
            LogError("authInfo->iothubSuffix input parameter cannot be NULL");
            result = NULL;
        }
        else if (serviceClientAuth->keyName == NULL)
        {
            LogError("authInfo->keyName input parameter cannot be NULL");
            result = NULL;
        }
        else if (serviceClientAuth->sharedAccessKey == NULL)
        {
            LogError("authInfo->sharedAccessKey input parameter cannot be NULL");
            result = NULL;
        }
        else
        {
            /*Codes_SRS_IOTHUBDEVICETWIN_12_003: [ IoTHubDeviceTwin_Create shall allocate memory for a new IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE instance ]*/
            result = malloc(sizeof(IOTHUB_SERVICE_CLIENT_DEVICE_TWIN));
            if (result == NULL)
            {
                /*Codes_SRS_IOTHUBDEVICETWIN_12_004: [ If the allocation failed, IoTHubDeviceTwin_Create shall return NULL ]*/
                LogError("Malloc failed for IOTHUB_SERVICE_CLIENT_DEVICE_TWIN");
            }
            else
            {
                memset(result, 0, sizeof(*result));

                /*Codes_SRS_IOTHUBDEVICETWIN_12_005: [ If the allocation successful, IoTHubDeviceTwin_Create shall create a IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE from the given IOTHUB_SERVICE_CLIENT_AUTH_HANDLE and return with it ]*/
                /*Codes_SRS_IOTHUBDEVICETWIN_12_006: [ IoTHubDeviceTwin_Create shall allocate memory and copy hostName to result->hostName by calling mallocAndStrcpy_s. ]*/
                if (mallocAndStrcpy_s(&result->hostname, serviceClientAuth->hostname) != 0)
                {
                    /*Codes_SRS_IOTHUBDEVICETWIN_12_007: [ If the mallocAndStrcpy_s fails, IoTHubDeviceTwin_Create shall do clean up and return NULL. ]*/
                    LogError("mallocAndStrcpy_s failed for hostName");
                    free_devicetwin_handle(result);
                    result = NULL;
                }
                /*Codes_SRS_IOTHUBDEVICETWIN_12_012: [ IoTHubDeviceTwin_Create shall allocate memory and copy sharedAccessKey to result->sharedAccessKey by calling mallocAndStrcpy_s. ]*/
                else if (mallocAndStrcpy_s(&result->sharedAccessKey, serviceClientAuth->sharedAccessKey) != 0)
                {
                    /*Codes_SRS_IOTHUBDEVICETWIN_12_013: [ If the mallocAndStrcpy_s fails, IoTHubDeviceTwin_Create shall do clean up and return NULL. ]*/
                    LogError("mallocAndStrcpy_s failed for sharedAccessKey");
                    free_devicetwin_handle(result);
                    result = NULL;
                }
                /*Codes_SRS_IOTHUBDEVICETWIN_12_014: [ IoTHubDeviceTwin_Create shall allocate memory and copy keyName to result->keyName by calling mallocAndStrcpy_s. ]*/
                else if (mallocAndStrcpy_s(&result->keyName, serviceClientAuth->keyName) != 0)
                {
                    /*Codes_SRS_IOTHUBDEVICETWIN_12_015: [ If the mallocAndStrcpy_s fails, IoTHubDeviceTwin_Create shall do clean up and return NULL. ]*/
                    LogError("mallocAndStrcpy_s failed for keyName");
                    free_devicetwin_handle(result);
                    result = NULL;
                }
            }
        }
    }
    return result;
}

void IoTHubDeviceTwin_Destroy(IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE serviceClientDeviceTwinHandle)
{
    /*Codes_SRS_IOTHUBDEVICETWIN_12_016: [ If the serviceClientDeviceTwinHandle input parameter is NULL IoTHubDeviceTwin_Destroy shall return ]*/
    if (serviceClientDeviceTwinHandle != NULL)
    {
        /*Codes_SRS_IOTHUBDEVICETWIN_12_017: [ If the serviceClientDeviceTwinHandle input parameter is not NULL IoTHubDeviceTwin_Destroy shall free the memory of it and return ]*/
        free_devicetwin_handle((IOTHUB_SERVICE_CLIENT_DEVICE_TWIN*)serviceClientDeviceTwinHandle);
    }
}

static int malloc_and_copy_uchar(char** strDestination, BUFFER_HANDLE strSource)
{
    int result;
    if ((strDestination == NULL) || (strSource == NULL))
    {
        /* If strDestination or strSource is a NULL pointer[...] function return line number where error is spotted */
        LogError("invalid parameter strDestination or strSource");
        result = MU_FAILURE;
    }
    else
    {
        size_t buffer_size = BUFFER_length(strSource);
        char *temp = malloc(buffer_size + 1);
        if (temp == NULL)
        {
            LogError("failed to malloc");
            result = MU_FAILURE;
        }
        else
        {
            *strDestination = memcpy(temp, BUFFER_u_char(strSource), buffer_size);
            temp[buffer_size] = '\0';
            result = 0;
        }
    }
    return result;
}

char* IoTHubDeviceTwin_GetDeviceOrModuleTwin(IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE serviceClientDeviceTwinHandle, const char* deviceId, const char* moduleId)
{
    char* result;

    /*Codes_SRS_IOTHUBDEVICETWIN_12_018: [ IoTHubDeviceTwin_GetTwin shall verify the input parameters and if any of them are NULL then return IOTHUB_DEVICE_TWIN_INVALID_ARG ]*/
    if ((serviceClientDeviceTwinHandle == NULL) || (deviceId == NULL))
    {
        LogError("Input parameter cannot be NULL");
        result = NULL;
    }
    else
    {
        BUFFER_HANDLE responseBuffer;

        if ((responseBuffer = BUFFER_new()) == NULL)
        {
            LogError("BUFFER_new failed for responseBuffer");
            result = NULL;
        }
        /*Codes_SRS_IOTHUBDEVICETWIN_12_019: [ IoTHubDeviceTwin_GetTwin shall create HTTP GET request URL using the given deviceId using the following format: url/twins/[deviceId] ]*/
        /*Codes_SRS_IOTHUBDEVICETWIN_12_020: [ IoTHubDeviceTwin_GetTwin shall add the following headers to the created HTTP GET request: authorization=sasToken,Request-Id=1001,Accept=application/json,Content-Type=application/json,charset=utf-8 ]*/
        /*Codes_SRS_IOTHUBDEVICETWIN_12_021: [ IoTHubDeviceTwin_GetTwin shall create an HTTPAPIEX_SAS_HANDLE handle by calling HTTPAPIEX_SAS_Create ]*/
        /*Codes_SRS_IOTHUBDEVICETWIN_12_022: [ IoTHubDeviceTwin_GetTwin shall create an HTTPAPIEX_HANDLE handle by calling HTTPAPIEX_Create ]*/
        /*Codes_SRS_IOTHUBDEVICETWIN_12_023: [ IoTHubDeviceTwin_GetTwin shall execute the HTTP GET request by calling HTTPAPIEX_ExecuteRequest ]*/
        else if (sendHttpRequestTwin(serviceClientDeviceTwinHandle, IOTHUB_TWIN_REQUEST_GET, deviceId, moduleId, NULL, responseBuffer) != IOTHUB_DEVICE_TWIN_OK)
        {
            /*Codes_SRS_IOTHUBDEVICETWIN_12_024: [ If any of the call fails during the HTTP creation IoTHubDeviceTwin_GetTwin shall fail and return NULL ]*/
            /*Codes_SRS_IOTHUBDEVICETWIN_12_025: [ If any of the HTTPAPI call fails IoTHubDeviceTwin_GetTwin shall fail and return NULL ]*/
            LogError("Failure sending HTTP request for create device");
            BUFFER_delete(responseBuffer);
            result = NULL;
        }
        else
        {
            /*Codes_SRS_IOTHUBDEVICETWIN_12_030: [ Otherwise IoTHubDeviceTwin_GetTwin shall save the received `deviceTwin` to the out parameter and return with it ]*/
            if (malloc_and_copy_uchar(&result, responseBuffer) != 0)
            {
                LogError("failed to copy response");
                result = NULL;
            }
            BUFFER_delete(responseBuffer);
        }
    }
    return result;

}

char* IoTHubDeviceTwin_GetTwin(IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE serviceClientDeviceTwinHandle, const char* deviceId)
{
    return IoTHubDeviceTwin_GetDeviceOrModuleTwin(serviceClientDeviceTwinHandle, deviceId, NULL);
}

char* IoTHubDeviceTwin_GetModuleTwin(IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE serviceClientDeviceTwinHandle, const char* deviceId, const char* moduleId)
{
    char* result;
    if ((serviceClientDeviceTwinHandle == NULL) || (deviceId == NULL) || (moduleId == NULL))
    {
        LogError("Input parameter cannot be NULL");
        result = NULL;
    }
    else
    {
        result = IoTHubDeviceTwin_GetDeviceOrModuleTwin(serviceClientDeviceTwinHandle, deviceId, moduleId);
    }

    return result;
}

char* IoTHubDeviceTwin_UpdateDeviceOrModuleTwin(IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE serviceClientDeviceTwinHandle, const char* deviceId, const char* moduleId, const char* deviceTwinJson)
{
    char* result;

    /*CodesSRS_IOTHUBDEVICETWIN_12_031: [ IoTHubDeviceTwin_UpdateTwin shall verify the input parameters and if any of them are NULL then return NULL ]*/
    if ((serviceClientDeviceTwinHandle == NULL) || (deviceId == NULL) || (deviceTwinJson == NULL))
    {
        LogError("Input parameter cannot be NULL");
        result = NULL;
    }
    else
    {
        BUFFER_HANDLE updateJson;
        BUFFER_HANDLE responseBuffer;

        /*CodesSRS_IOTHUBDEVICETWIN_12_032: [ IoTHubDeviceTwin_UpdateTwin shall create a BUFFER_HANDLE from deviceTwinJson by calling BUFFER_create ]*/
        if ((updateJson = BUFFER_create((const unsigned char*)deviceTwinJson, strlen(deviceTwinJson))) == NULL)
        {
            /*CodesSRS_IOTHUBDEVICETWIN_12_033: [ If the creation fails, IoTHubDeviceTwin_UpdateTwin shall return NULL ]*/
            LogError("BUFFER_create failed for deviceTwinJson");
            result = NULL;
        }
        /*CodesSRS_IOTHUBDEVICETWIN_12_034: [ IoTHubDeviceTwin_UpdateTwin shall allocate memory for response buffer by calling BUFFER_new ]*/
        /*CodesSRS_IOTHUBDEVICETWIN_12_035: [ If the allocation failed, IoTHubDeviceTwin_UpdateTwin shall return NULL ]*/
        else if ((responseBuffer = BUFFER_new()) == NULL)
        {
            /*CodesSRS_IOTHUBDEVICETWIN_12_034: [ If the creation fails, IoTHubDeviceTwin_UpdateTwin shall return NULL ]*/
            LogError("BUFFER_new failed for responseBuffer");
            BUFFER_delete(updateJson);
            result = NULL;
        }
        /*CodesSRS_IOTHUBDEVICETWIN_12_039: [ IoTHubDeviceTwin_UpdateTwin shall create an HTTP PATCH request using deviceTwinJson ]*/
        /*CodesSRS_IOTHUBDEVICETWIN_12_040: [ IoTHubDeviceTwin_UpdateTwin shall create an HTTP PATCH request using the createdfollowing HTTP headers: authorization=sasToken,Request-Id=1001,Accept=application/json,Content-Type=application/json,charset=utf-8 ]*/
        /*CodesSRS_IOTHUBDEVICETWIN_12_041: [ IoTHubDeviceTwin_UpdateTwin shall create an HTTPAPIEX_SAS_HANDLE handle by calling HTTPAPIEX_SAS_Create ]*/
        /*CodesSRS_IOTHUBDEVICETWIN_12_042: [ IoTHubDeviceTwin_UpdateTwin shall create an HTTPAPIEX_HANDLE handle by calling HTTPAPIEX_Create ]*/
        /*CodesSRS_IOTHUBDEVICETWIN_12_043: [ IoTHubDeviceTwin_UpdateTwin shall execute the HTTP PATCH request by calling HTTPAPIEX_ExecuteRequest ]*/
        else if (sendHttpRequestTwin(serviceClientDeviceTwinHandle, IOTHUB_TWIN_REQUEST_UPDATE, deviceId, moduleId, updateJson, responseBuffer) != IOTHUB_DEVICE_TWIN_OK)
        {
            /*CodesSRS_IOTHUBDEVICETWIN_12_044: [ If any of the call fails during the HTTP creation IoTHubDeviceTwin_UpdateTwin shall fail and return NULL ]*/
            /*CodesSRS_IOTHUBDEVICETWIN_12_045: [ If any of the HTTPAPI call fails IoTHubDeviceTwin_UpdateTwin shall fail and return NULL ]*/
            /*CodesSRS_IOTHUBDEVICETWIN_12_046: [ IoTHubDeviceTwin_UpdateTwin shall verify the received HTTP status code and if it is not equal to 200 then return NULL ]*/
            LogError("Failure sending HTTP request for create device");
            BUFFER_delete(responseBuffer);
            BUFFER_delete(updateJson);
            result = NULL;
        }
        else
        {
            /*CodesSRS_IOTHUBDEVICETWIN_12_047: [ Otherwise IoTHubDeviceTwin_UpdateTwin shall save the received updated device twin to the out parameter and return with it ]*/
            if (malloc_and_copy_uchar(&result, responseBuffer) != 0)
            {
                LogError("failed to copy response");
                result = NULL;
            }
            BUFFER_delete(responseBuffer);
            BUFFER_delete(updateJson);
        }
    }
    return result;

}

char* IoTHubDeviceTwin_UpdateTwin(IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE serviceClientDeviceTwinHandle, const char* deviceId, const char* deviceTwinJson)
{
    return IoTHubDeviceTwin_UpdateDeviceOrModuleTwin(serviceClientDeviceTwinHandle, deviceId, NULL, deviceTwinJson);
}

char* IoTHubDeviceTwin_UpdateModuleTwin(IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE serviceClientDeviceTwinHandle, const char* deviceId, const char* moduleId, const char* moduleTwinJson)
{
    char* result;
    if ((serviceClientDeviceTwinHandle == NULL) || (deviceId == NULL) || (moduleId == NULL) || (moduleTwinJson == NULL))
    {
        LogError("Input parameter cannot be NULL");
        result = NULL;
    }
    else
    {
        result = IoTHubDeviceTwin_UpdateDeviceOrModuleTwin(serviceClientDeviceTwinHandle, deviceId, moduleId, moduleTwinJson);
    }

    return result;
}




