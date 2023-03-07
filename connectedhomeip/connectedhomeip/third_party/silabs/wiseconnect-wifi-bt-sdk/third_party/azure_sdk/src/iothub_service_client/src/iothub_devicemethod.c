// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <ctype.h>
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
#include "iothub_devicemethod.h"
#include "iothub_sc_version.h"

MU_DEFINE_ENUM_STRINGS_WITHOUT_INVALID(IOTHUB_DEVICE_METHOD_RESULT, IOTHUB_DEVICE_METHOD_RESULT_VALUES);

#define IOTHUB_DEVICE_METHOD_REQUEST_MODE_VALUES    \
    IOTHUB_DEVICEMETHOD_REQUEST_INVOKE

MU_DEFINE_ENUM(IOTHUB_DEVICEMETHOD_REQUEST_MODE, IOTHUB_DEVICE_METHOD_REQUEST_MODE_VALUES);


#define  HTTP_HEADER_KEY_AUTHORIZATION  "Authorization"
#define  HTTP_HEADER_VAL_AUTHORIZATION  " "
#define  HTTP_HEADER_KEY_REQUEST_ID  "Request-Id"
#define  HTTP_HEADER_KEY_USER_AGENT  "User-Agent"
#define  HTTP_HEADER_VAL_USER_AGENT  IOTHUB_SERVICE_CLIENT_TYPE_PREFIX IOTHUB_SERVICE_CLIENT_BACKSLASH IOTHUB_SERVICE_CLIENT_VERSION
#define  HTTP_HEADER_KEY_ACCEPT  "Accept"
#define  HTTP_HEADER_VAL_ACCEPT  "application/json"
#define  HTTP_HEADER_KEY_CONTENT_TYPE  "Content-Type"
#define  HTTP_HEADER_VAL_CONTENT_TYPE  "application/json; charset=utf-8"
#define UID_LENGTH 37

static const char* const URL_API_VERSION = "?api-version=2020-09-30";
static const char* const RELATIVE_PATH_FMT_DEVICEMETHOD = "/twins/%s/methods%s";
static const char* const RELATIVE_PATH_FMT_DEVICEMETHOD_MODULE = "/twins/%s/modules/%s/methods%s";

// Note: The timeout field specified in this JSON is not honored by IoT Hub.  See
// https://github.com/Azure/azure-iot-sdk-c/issues/1378 for details.
static const char* const RELATIVE_PATH_FMT_DEVIECMETHOD_PAYLOAD = "{\"methodName\":\"%s\",\"responseTimeoutInSeconds\":%d,\"connectTimeoutInSeconds\":60,\"payload\":%s}";

/** @brief Structure to store IoTHub authentication information
*/
typedef struct IOTHUB_SERVICE_CLIENT_DEVICE_METHOD_TAG
{
    char* hostname;
    char* sharedAccessKey;
    char* keyName;
} IOTHUB_SERVICE_CLIENT_DEVICE_METHOD;

static IOTHUB_DEVICE_METHOD_RESULT parseResponseJson(BUFFER_HANDLE responseJson, int* responseStatus, unsigned char** responsePayload, size_t* responsePayloadSize)
{
    IOTHUB_DEVICE_METHOD_RESULT result;
    JSON_Value* root_value;
    JSON_Object* json_object;
    JSON_Value* statusJsonValue;
    JSON_Value* payloadJsonValue;
    char* payload;
    STRING_HANDLE jsonStringHandle;
    const char* jsonStr;
    unsigned char* bufferStr;

    if ((bufferStr = BUFFER_u_char(responseJson)) == NULL)
    {
        LogError("BUFFER_u_char failed");
        result = IOTHUB_DEVICE_METHOD_ERROR;
    }
    else if ((jsonStringHandle = STRING_from_byte_array(bufferStr, BUFFER_length(responseJson))) == NULL)
    {
        LogError("STRING_construct_n failed");
        result = IOTHUB_DEVICE_METHOD_ERROR;
    }
    else if ((jsonStr = STRING_c_str(jsonStringHandle)) == NULL)
    {
        LogError("STRING_c_str failed");
        STRING_delete(jsonStringHandle);
        result = IOTHUB_DEVICE_METHOD_ERROR;
    }
    else if ((root_value = json_parse_string(jsonStr)) == NULL)
    {
        LogError("json_parse_string failed");
        STRING_delete(jsonStringHandle);
        result = IOTHUB_DEVICE_METHOD_ERROR;
    }
    else if ((json_object = json_value_get_object(root_value)) == NULL)
    {
        LogError("json_value_get_object failed");
        STRING_delete(jsonStringHandle);
        json_value_free(root_value);
        result = IOTHUB_DEVICE_METHOD_ERROR;
    }
    else if ((statusJsonValue = json_object_get_value(json_object, "status")) == NULL)
    {
        LogError("json_object_get_value failed for status");
        STRING_delete(jsonStringHandle);
        json_value_free(root_value);
        result = IOTHUB_DEVICE_METHOD_ERROR;
    }
    else if ((payloadJsonValue = json_object_get_value(json_object, "payload")) == NULL)
    {
        LogError("json_object_get_value failed for payload");
        STRING_delete(jsonStringHandle);
        json_value_free(root_value);
        result = IOTHUB_DEVICE_METHOD_ERROR;
    }
    else if ((payload = json_serialize_to_string(payloadJsonValue)) == NULL)
    {
        LogError("json_serialize_to_string failed for payload");
        STRING_delete(jsonStringHandle);
        json_value_free(root_value);
        result = IOTHUB_DEVICE_METHOD_ERROR;
    }
    else
    {
        *responseStatus = (int)json_value_get_number(statusJsonValue);
        *responsePayload = (unsigned char *)payload;
        *responsePayloadSize = strlen(payload);

        STRING_delete(jsonStringHandle);
        json_value_free(root_value);
        result = IOTHUB_DEVICE_METHOD_OK;
    }

    return result;
}

static BUFFER_HANDLE createMethodPayloadJson(const char* methodName, unsigned int timeout, const char* payload)
{
    STRING_HANDLE stringHandle;
    const char* stringHandle_c_str;
    BUFFER_HANDLE result;

    if ((stringHandle = STRING_construct_sprintf(RELATIVE_PATH_FMT_DEVIECMETHOD_PAYLOAD, methodName, timeout, payload)) == NULL)
    {
        LogError("STRING_construct_sprintf failed");
        result = NULL;
    }
    else if ((stringHandle_c_str = STRING_c_str(stringHandle)) == NULL)
    {
        LogError("STRING_c_str failed");
        STRING_delete(stringHandle);
        result = NULL;
    }
    else
    {
        result = BUFFER_create((const unsigned char*)stringHandle_c_str, strlen(stringHandle_c_str));
        STRING_delete(stringHandle);
    }
    return result;
}

static const char* generateGuid(void)
{
    char* result;

    if ((result = malloc(UID_LENGTH)) != NULL)
    {
        result[0] = '\0';
        if (UniqueId_Generate(result, UID_LENGTH) != UNIQUEID_OK)
        {
            LogError("UniqueId_Generate failed");
            free((void*)result);
            result = NULL;
        }
    }
    return (const char*)result;
}

static STRING_HANDLE createRelativePath(IOTHUB_DEVICEMETHOD_REQUEST_MODE iotHubDeviceMethodRequestMode, const char* deviceId, const char* moduleId)
{
    STRING_HANDLE result;

    if (iotHubDeviceMethodRequestMode == IOTHUB_DEVICEMETHOD_REQUEST_INVOKE)
    {
        if (moduleId != NULL)
        {
            result = STRING_construct_sprintf(RELATIVE_PATH_FMT_DEVICEMETHOD_MODULE, deviceId, moduleId, URL_API_VERSION);
        }
        else
        {
            result = STRING_construct_sprintf(RELATIVE_PATH_FMT_DEVICEMETHOD, deviceId, URL_API_VERSION);
        }
    }
    else
    {
        result = NULL;
    }
    return result;
}

static HTTP_HEADERS_HANDLE createHttpHeader()
{
    /*Codes_SRS_IOTHUBDEVICEMETHOD_12_020: [ IoTHubDeviceMethod_GetTwin shall add the following headers to the created HTTP GET request: authorization=sasToken,Request-Id=1001,Accept=application/json,Content-Type=application/json,charset=utf-8 ]*/
    HTTP_HEADERS_HANDLE httpHeader;
    const char* guid;

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
        free((void*)guid);
        HTTPHeaders_Free(httpHeader);
        httpHeader = NULL;
    }
    else if (HTTPHeaders_AddHeaderNameValuePair(httpHeader, HTTP_HEADER_KEY_USER_AGENT, HTTP_HEADER_VAL_USER_AGENT) != HTTP_HEADERS_OK)
    {
        LogError("HTTPHeaders_AddHeaderNameValuePair failed for User-Agent header");
        free((void*)guid);
        HTTPHeaders_Free(httpHeader);
        httpHeader = NULL;
    }
    else if (HTTPHeaders_AddHeaderNameValuePair(httpHeader, HTTP_HEADER_KEY_CONTENT_TYPE, HTTP_HEADER_VAL_CONTENT_TYPE) != HTTP_HEADERS_OK)
    {
        LogError("HTTPHeaders_AddHeaderNameValuePair failed for Content-Type header");
        free((void*)guid);
        HTTPHeaders_Free(httpHeader);
        httpHeader = NULL;
    }
    else
    {
        free((void*)guid);
    }

    return httpHeader;
}

static IOTHUB_DEVICE_METHOD_RESULT sendHttpRequestDeviceMethod(IOTHUB_SERVICE_CLIENT_DEVICE_METHOD_HANDLE serviceClientDeviceMethodHandle, IOTHUB_DEVICEMETHOD_REQUEST_MODE iotHubDeviceMethodRequestMode, const char* deviceId, const char* moduleId, BUFFER_HANDLE deviceJsonBuffer, BUFFER_HANDLE responseBuffer)
{
    IOTHUB_DEVICE_METHOD_RESULT result;

    STRING_HANDLE uriResource;
    STRING_HANDLE accessKey;
    STRING_HANDLE keyName;
    HTTPAPIEX_SAS_HANDLE httpExApiSasHandle;
    HTTPAPIEX_HANDLE httpExApiHandle;
    HTTP_HEADERS_HANDLE httpHeader;

    if ((uriResource = STRING_construct(serviceClientDeviceMethodHandle->hostname)) == NULL)
    {
        LogError("STRING_construct failed for uriResource");
        result = IOTHUB_DEVICE_METHOD_ERROR;
    }
    else if ((accessKey = STRING_construct(serviceClientDeviceMethodHandle->sharedAccessKey)) == NULL)
    {
        LogError("STRING_construct failed for accessKey");
        STRING_delete(uriResource);
        result = IOTHUB_DEVICE_METHOD_ERROR;
    }
    else if ((keyName = STRING_construct(serviceClientDeviceMethodHandle->keyName)) == NULL)
    {
        LogError("STRING_construct failed for keyName");
        STRING_delete(accessKey);
        STRING_delete(uriResource);
        result = IOTHUB_DEVICE_METHOD_ERROR;
    }
    else if ((httpHeader = createHttpHeader()) == NULL)
    {
        LogError("HttpHeader creation failed");
        STRING_delete(keyName);
        STRING_delete(accessKey);
        STRING_delete(uriResource);
        result = IOTHUB_DEVICE_METHOD_ERROR;
    }
    else if ((httpExApiSasHandle = HTTPAPIEX_SAS_Create(accessKey, uriResource, keyName)) == NULL)
    {
        LogError("HTTPAPIEX_SAS_Create failed");
        HTTPHeaders_Free(httpHeader);
        STRING_delete(keyName);
        STRING_delete(accessKey);
        STRING_delete(uriResource);
        result = IOTHUB_DEVICE_METHOD_HTTPAPI_ERROR;
    }
    else if ((httpExApiHandle = HTTPAPIEX_Create(serviceClientDeviceMethodHandle->hostname)) == NULL)
    {
        LogError("HTTPAPIEX_Create failed");
        HTTPAPIEX_SAS_Destroy(httpExApiSasHandle);
        HTTPHeaders_Free(httpHeader);
        STRING_delete(keyName);
        STRING_delete(accessKey);
        STRING_delete(uriResource);
        result = IOTHUB_DEVICE_METHOD_HTTPAPI_ERROR;
    }
    else
    {
        HTTPAPI_REQUEST_TYPE httpApiRequestType = HTTPAPI_REQUEST_GET;
        STRING_HANDLE relativePath;
        unsigned int statusCode = 0;
        unsigned char is_error = 0;

        if (iotHubDeviceMethodRequestMode == IOTHUB_DEVICEMETHOD_REQUEST_INVOKE)
        {
            httpApiRequestType = HTTPAPI_REQUEST_POST;
        }
        else
        {
            is_error = 1;
        }

        if (is_error)
        {
            LogError("Invalid request type");
            result = IOTHUB_DEVICE_METHOD_HTTPAPI_ERROR;
        }
        else
        {
            if ((relativePath = createRelativePath(iotHubDeviceMethodRequestMode, deviceId, moduleId)) == NULL)
            {
                LogError("Failure creating relative path");
                result = IOTHUB_DEVICE_METHOD_ERROR;
            }
            else if (HTTPAPIEX_SAS_ExecuteRequest(httpExApiSasHandle, httpExApiHandle, httpApiRequestType, STRING_c_str(relativePath), httpHeader, deviceJsonBuffer, &statusCode, NULL, responseBuffer) != HTTPAPIEX_OK)
            {
                LogError("HTTPAPIEX_SAS_ExecuteRequest failed");
                STRING_delete(relativePath);
                result = IOTHUB_DEVICE_METHOD_HTTPAPI_ERROR;
            }
            else
            {
                STRING_delete(relativePath);
                if (statusCode == 200)
                {
                    result = IOTHUB_DEVICE_METHOD_OK;
                }
                else
                {
                    LogError("Http Failure status code %d.", statusCode);
                    result = IOTHUB_DEVICE_METHOD_ERROR;
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

IOTHUB_SERVICE_CLIENT_DEVICE_METHOD_HANDLE IoTHubDeviceMethod_Create(IOTHUB_SERVICE_CLIENT_AUTH_HANDLE serviceClientHandle)
{
    IOTHUB_SERVICE_CLIENT_DEVICE_METHOD_HANDLE result;

    /*Codes_SRS_IOTHUBDEVICEMETHOD_12_001: [ If the serviceClientHandle input parameter is NULL IoTHubDeviceMethod_Create shall return NULL ]*/
    if (serviceClientHandle == NULL)
    {
        LogError("serviceClientHandle input parameter cannot be NULL");
        result = NULL;
    }
    else
    {
        /*Codes_SRS_IOTHUBDEVICEMETHOD_12_002: [ If any member of the serviceClientHandle input parameter is NULL IoTHubDeviceMethod_Create shall return NULL ]*/
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
            /*Codes_SRS_IOTHUBDEVICEMETHOD_12_003: [ IoTHubDeviceMethod_Create shall allocate memory for a new IOTHUB_SERVICE_CLIENT_DEVICE_METHOD_HANDLE instance ]*/
            result = malloc(sizeof(IOTHUB_SERVICE_CLIENT_DEVICE_METHOD));
            if (result == NULL)
            {
                /*Codes_SRS_IOTHUBDEVICEMETHOD_12_004: [ If the allocation failed, IoTHubDeviceMethod_Create shall return NULL ]*/
                LogError("Malloc failed for IOTHUB_SERVICE_CLIENT_DEVICE_METHOD");
            }
            else
            {
                /*Codes_SRS_IOTHUBDEVICEMETHOD_12_005: [ If the allocation successful, IoTHubDeviceMethod_Create shall create a IOTHUB_SERVICE_CLIENT_DEVICE_METHOD_HANDLE from the given IOTHUB_SERVICE_CLIENT_AUTH_HANDLE and return with it ]*/
                /*Codes_SRS_IOTHUBDEVICEMETHOD_12_006: [ IoTHubDeviceMethod_Create shall allocate memory and copy hostName to result->hostName by calling mallocAndStrcpy_s. ]*/
                if (mallocAndStrcpy_s(&result->hostname, serviceClientAuth->hostname) != 0)
                {
                    /*Codes_SRS_IOTHUBDEVICEMETHOD_12_007: [ If the mallocAndStrcpy_s fails, IoTHubDeviceMethod_Create shall do clean up and return NULL. ]*/
                    LogError("mallocAndStrcpy_s failed for hostName");
                    free(result);
                    result = NULL;
                }
                /*Codes_SRS_IOTHUBDEVICEMETHOD_12_012: [ IoTHubDeviceMethod_Create shall allocate memory and copy sharedAccessKey to result->sharedAccessKey by calling mallocAndStrcpy_s. ]*/
                else if (mallocAndStrcpy_s(&result->sharedAccessKey, serviceClientAuth->sharedAccessKey) != 0)
                {
                    /*Codes_SRS_IOTHUBDEVICEMETHOD_12_013: [ If the mallocAndStrcpy_s fails, IoTHubDeviceMethod_Create shall do clean up and return NULL. ]*/
                    LogError("mallocAndStrcpy_s failed for sharedAccessKey");
                    free(result->hostname);
                    free(result);
                    result = NULL;
                }
                /*Codes_SRS_IOTHUBDEVICEMETHOD_12_014: [ IoTHubDeviceMethod_Create shall allocate memory and copy keyName to result->keyName by calling mallocAndStrcpy_s. ]*/
                else if (mallocAndStrcpy_s(&result->keyName, serviceClientAuth->keyName) != 0)
                {
                    /*Codes_SRS_IOTHUBDEVICEMETHOD_12_015: [ If the mallocAndStrcpy_s fails, IoTHubDeviceMethod_Create shall do clean up and return NULL. ]*/
                    LogError("mallocAndStrcpy_s failed for keyName");
                    free(result->hostname);
                    free(result->sharedAccessKey);
                    free(result);
                    result = NULL;
                }
            }
        }
    }
    return result;
}

void IoTHubDeviceMethod_Destroy(IOTHUB_SERVICE_CLIENT_DEVICE_METHOD_HANDLE serviceClientDeviceMethodHandle)
{
    /*Codes_SRS_IOTHUBDEVICEMETHOD_12_016: [ If the serviceClientDeviceMethodHandle input parameter is NULL IoTHubDeviceMethod_Destroy shall return ]*/
    if (serviceClientDeviceMethodHandle != NULL)
    {
        /*Codes_SRS_IOTHUBDEVICEMETHOD_12_017: [ If the serviceClientDeviceMethodHandle input parameter is not NULL IoTHubDeviceMethod_Destroy shall free the memory of it and return ]*/
        IOTHUB_SERVICE_CLIENT_DEVICE_METHOD* serviceClientDeviceMethod = (IOTHUB_SERVICE_CLIENT_DEVICE_METHOD*)serviceClientDeviceMethodHandle;

        free(serviceClientDeviceMethod->hostname);
        free(serviceClientDeviceMethod->sharedAccessKey);
        free(serviceClientDeviceMethod->keyName);
        free(serviceClientDeviceMethod);
    }
}

static IOTHUB_DEVICE_METHOD_RESULT IoTHubDeviceMethod_DeviceOrModuleInvoke(IOTHUB_SERVICE_CLIENT_DEVICE_METHOD_HANDLE serviceClientDeviceMethodHandle, const char* deviceId, const char* moduleId,  const char* methodName, const char* methodPayload, unsigned int timeout, int* responseStatus, unsigned char** responsePayload, size_t* responsePayloadSize)
{
    IOTHUB_DEVICE_METHOD_RESULT result;

    /*Codes_SRS_IOTHUBDEVICEMETHOD_12_031: [ IoTHubDeviceMethod_Invoke(Module) shall verify the input parameters and if any of them (except the timeout) are NULL then return IOTHUB_DEVICE_METHOD_INVALID_ARG ]*/
    if ((serviceClientDeviceMethodHandle == NULL) || (deviceId == NULL) || (methodName == NULL) || (methodPayload == NULL) || (responseStatus == NULL) || (responsePayload == NULL) || (responsePayloadSize == NULL))
    {
        LogError("Input parameter cannot be NULL");
        result = IOTHUB_DEVICE_METHOD_INVALID_ARG;
    }
    else
    {
        BUFFER_HANDLE httpPayloadBuffer;
        BUFFER_HANDLE responseBuffer;

        /*Codes_SRS_IOTHUBDEVICEMETHOD_12_032: [ IoTHubDeviceMethod_Invoke(Module) shall create a BUFFER_HANDLE from methodName, timeout and methodPayload by calling BUFFER_create ]*/
        if ((httpPayloadBuffer = createMethodPayloadJson(methodName, timeout, methodPayload)) == NULL)
        {
            /*Codes_SRS_IOTHUBDEVICEMETHOD_12_033: [ If the creation fails, IoTHubDeviceMethod_Invoke(Module) shall return IOTHUB_DEVICE_METHOD_ERROR ]*/
            LogError("BUFFER creation failed for httpPayloadBuffer");
            result = IOTHUB_DEVICE_METHOD_ERROR;
        }
        /*Codes_SRS_IOTHUBDEVICEMETHOD_12_034: [ IoTHubDeviceMethod_Invoke(Module) shall allocate memory for response buffer by calling BUFFER_new ]*/
        else if ((responseBuffer = BUFFER_new()) == NULL)
        {
            /*Codes_SRS_IOTHUBDEVICEMETHOD_12_035: [ If the allocation failed, IoTHubDeviceMethod_Invoke(Module) shall return IOTHUB_DEVICE_METHOD_ERROR ]*/
            LogError("BUFFER_new failed for responseBuffer");
            BUFFER_delete(httpPayloadBuffer);
            result = IOTHUB_DEVICE_METHOD_ERROR;
        }
        /*Codes_SRS_IOTHUBDEVICEMETHOD_12_039: [ IoTHubDeviceMethod_Invoke(Module) shall create an HTTP POST request using methodPayloadBuffer ]*/
        /*Codes_SRS_IOTHUBDEVICEMETHOD_12_040: [ IoTHubDeviceMethod_Invoke(Module) shall create an HTTP POST request using the following HTTP headers: authorization=sasToken,Request-Id=1001,Accept=application/json,Content-Type=application/json,charset=utf-8 ]*/
        /*Codes_SRS_IOTHUBDEVICEMETHOD_12_041: [ IoTHubDeviceMethod_Invoke(Module) shall create an HTTPAPIEX_SAS_HANDLE handle by calling HTTPAPIEX_SAS_Create ]*/
        /*Codes_SRS_IOTHUBDEVICEMETHOD_12_042: [ IoTHubDeviceMethod_Invoke(Module) shall create an HTTPAPIEX_HANDLE handle by calling HTTPAPIEX_Create ]*/
        /*Codes_SRS_IOTHUBDEVICEMETHOD_12_043: [ IoTHubDeviceMethod_Invoke(Module) shall execute the HTTP POST request by calling HTTPAPIEX_ExecuteRequest ]*/
        else if (sendHttpRequestDeviceMethod(serviceClientDeviceMethodHandle, IOTHUB_DEVICEMETHOD_REQUEST_INVOKE, deviceId, moduleId, httpPayloadBuffer, responseBuffer) != IOTHUB_DEVICE_METHOD_OK)
        {
            /*Codes_SRS_IOTHUBDEVICEMETHOD_12_044: [ If any of the call fails during the HTTP creation IoTHubDeviceMethod_Invoke(Module) shall fail and return IOTHUB_DEVICE_METHOD_HTTPAPI_ERROR ]*/
            /*Codes_SRS_IOTHUBDEVICEMETHOD_12_045: [ If any of the HTTPAPI call fails IoTHubDeviceMethod_Invoke(Module) shall fail and return IOTHUB_DEVICE_METHOD_HTTPAPI_ERROR ]*/
            /*Codes_SRS_IOTHUBDEVICEMETHOD_12_046: [ IoTHubDeviceMethod_Invoke(Module) shall verify the received HTTP status code and if it is not equal to 200 then return IOTHUB_DEVICE_METHOD_ERROR ]*/
            LogError("Failure sending HTTP request for device method invoke");
            BUFFER_delete(responseBuffer);
            BUFFER_delete(httpPayloadBuffer);
            result = IOTHUB_DEVICE_METHOD_ERROR;
        }
        /*Codes_SRS_IOTHUBDEVICEMETHOD_12_049: [ Otherwise IoTHubDeviceMethod_Invoke(Module) shall save the received status and payload to the corresponding out parameter and return with IOTHUB_DEVICE_METHOD_OK ]*/
        else if ((parseResponseJson(responseBuffer, responseStatus, responsePayload, responsePayloadSize)) != IOTHUB_DEVICE_METHOD_OK)
        {
            /*Codes_SRS_IOTHUBDEVICEMETHOD_12_047: [ If parsing the response fails IoTHubDeviceMethod_Invoke(Module) shall return IOTHUB_DEVICE_METHOD_ERROR ]*/
            LogError("Failure parsing response");
            BUFFER_delete(responseBuffer);
            BUFFER_delete(httpPayloadBuffer);
            result = IOTHUB_DEVICE_METHOD_ERROR;
        }
        else
        {
            /*Codes_SRS_IOTHUBDEVICEMETHOD_12_049: [ Otherwise IoTHubDeviceMethod_Invoke(Module) shall save the received status and payload to the corresponding out parameter and return with IOTHUB_DEVICE_METHOD_OK ]*/
            result = IOTHUB_DEVICE_METHOD_OK;

            BUFFER_delete(responseBuffer);
            BUFFER_delete(httpPayloadBuffer);
        }
    }
    return result;

}

IOTHUB_DEVICE_METHOD_RESULT IoTHubDeviceMethod_Invoke(IOTHUB_SERVICE_CLIENT_DEVICE_METHOD_HANDLE serviceClientDeviceMethodHandle, const char* deviceId, const char* methodName, const char* methodPayload, unsigned int timeout, int* responseStatus, unsigned char** responsePayload, size_t* responsePayloadSize)
{
    return IoTHubDeviceMethod_DeviceOrModuleInvoke(serviceClientDeviceMethodHandle, deviceId, NULL, methodName, methodPayload, timeout, responseStatus, responsePayload, responsePayloadSize);
}

IOTHUB_DEVICE_METHOD_RESULT IoTHubDeviceMethod_InvokeModule(IOTHUB_SERVICE_CLIENT_DEVICE_METHOD_HANDLE serviceClientDeviceMethodHandle, const char* deviceId, const char* moduleId,  const char* methodName, const char* methodPayload, unsigned int timeout, int* responseStatus, unsigned char** responsePayload, size_t* responsePayloadSize)
{
    IOTHUB_DEVICE_METHOD_RESULT result;

    if (moduleId == NULL)
    {
        /*SRS_IOTHUBDEVICEMETHOD_31_050: [ IoTHubDeviceMethod_InvokeModule shall return IOTHUB_DEVICE_METHOD_INVALID_ARG if moduleId is NULL. ]*/
        LogError("moduleId input parameter cannot be NULL");
        result = IOTHUB_DEVICE_METHOD_INVALID_ARG;
    }
    else
    {
        result = IoTHubDeviceMethod_DeviceOrModuleInvoke(serviceClientDeviceMethodHandle, deviceId, moduleId, methodName, methodPayload, timeout, responseStatus, responsePayload, responsePayloadSize);
    }

    return result;
}

