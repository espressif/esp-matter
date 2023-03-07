// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef USE_EDGE_MODULES
//tryping to compile iothub_client_edge.c while the symbol USE_EDGE_MODULES is not defined
#else

#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/constbuffer.h"
#include "azure_c_shared_utility/httpapiex.h"
#include "azure_c_shared_utility/httpapiexsas.h"
#include "azure_c_shared_utility/uniqueid.h"
#include "azure_c_shared_utility/shared_util_options.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/envvariable.h"

#include "parson.h"

#include "internal/iothub_client_private.h"
#include "internal/iothub_client_authorization.h"
#include "iothub_client_core_common.h"
#include "iothub_client_version.h"
#include "internal/iothub_client_edge.h"

#define  HTTP_HEADER_KEY_AUTHORIZATION  "Authorization"
#define  HTTP_HEADER_VAL_AUTHORIZATION  " "
#define  HTTP_HEADER_KEY_MODULE_ID  "x-ms-edge-moduleId"
#define  HTTP_HEADER_VAL_MODULE_ID  " "
#define  HTTP_HEADER_KEY_REQUEST_ID  "Request-Id"
#define  HTTP_HEADER_KEY_USER_AGENT  "User-Agent"
#define  HTTP_HEADER_VAL_USER_AGENT  CLIENT_DEVICE_TYPE_PREFIX CLIENT_DEVICE_BACKSLASH IOTHUB_SDK_VERSION
#define  HTTP_HEADER_KEY_CONTENT_TYPE  "Content-Type"
#define  HTTP_HEADER_VAL_CONTENT_TYPE  "application/json; charset=utf-8"
#define  UID_LENGTH 37

#define SASTOKEN_LIFETIME 3600

static const char* const URL_API_VERSION = "?api-version=2020-09-30";
static const char* const RELATIVE_PATH_FMT_MODULE_METHOD = "/twins/%s/modules/%s/methods%s";
static const char* const RELATIVE_PATH_FMT_DEVICE_METHOD = "/twins/%s/methods%s";

// Note: The timeout field specified in this JSON is not honored by IoT Edge.  See
// https://github.com/Azure/azure-iot-sdk-c/issues/1378 for details.
static const char* const PAYLOAD_FMT = "{\"methodName\":\"%s\",\"timeout\":%d,\"payload\":%s}";
static const char* const SCOPE_FMT = "%s/devices/%s/modules/%s";

static const char* ENVIRONMENT_VAR_EDGEHUB_CACERTIFICATEFILE = "EdgeModuleCACertificateFile";


typedef struct IOTHUB_CLIENT_EDGE_HANDLE_DATA_TAG
{
    char* hostname;
    char* deviceId;
    char* moduleId;
    IOTHUB_AUTHORIZATION_HANDLE authorizationHandle;
} IOTHUB_CLIENT_EDGE_HANDLE_DATA;


IOTHUB_CLIENT_EDGE_HANDLE IoTHubClient_EdgeHandle_Create(const IOTHUB_CLIENT_CONFIG* config, IOTHUB_AUTHORIZATION_HANDLE authorizationHandle, const char* module_id)
{
    IOTHUB_CLIENT_EDGE_HANDLE_DATA* handleData;

    if ((config == NULL) || (authorizationHandle == NULL) || (module_id == NULL))
    {
        LogError("input cannot be NULL");
        handleData = NULL;
    }
    else if ((handleData = malloc(sizeof(IOTHUB_CLIENT_EDGE_HANDLE_DATA))) == NULL)
    {
        LogError("memory allocation error");
        handleData = NULL;
    }
    else
    {
        memset(handleData, 0, sizeof(IOTHUB_CLIENT_EDGE_HANDLE_DATA));
        handleData->authorizationHandle = authorizationHandle;

        if (mallocAndStrcpy_s(&(handleData->deviceId), config->deviceId) != 0)
        {
            LogError("Failed to copy string");
            IoTHubClient_EdgeHandle_Destroy(handleData);
            handleData = NULL;
        }
        else if (mallocAndStrcpy_s(&(handleData->moduleId), module_id) != 0)
        {
            LogError("Failed to copy string");
            IoTHubClient_EdgeHandle_Destroy(handleData);
            handleData = NULL;
        }
        else if (mallocAndStrcpy_s(&(handleData->hostname), config->protocolGatewayHostName) != 0)
        {
            LogError("Failed to copy string");
            IoTHubClient_EdgeHandle_Destroy(handleData);
            handleData = NULL;
        }
    }

    return (IOTHUB_CLIENT_EDGE_HANDLE)handleData;
}

void IoTHubClient_EdgeHandle_Destroy(IOTHUB_CLIENT_EDGE_HANDLE methodHandle)
{
    if (methodHandle != NULL)
    {
        free(methodHandle->hostname);
        free(methodHandle->deviceId);
        free(methodHandle->moduleId);
        //Do not free authorizationHandle for now, since its a pointer to something owned by Core_LL_Handle
        free(methodHandle);
    }
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

static HTTP_HEADERS_HANDLE createHttpHeader()
{
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
    else if (HTTPHeaders_AddHeaderNameValuePair(httpHeader, HTTP_HEADER_KEY_MODULE_ID, HTTP_HEADER_VAL_MODULE_ID) != HTTP_HEADERS_OK)
    {
        LogError("HTTPHeaders_AddHeaderNameValuePair failed for module ID header");
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

static IOTHUB_CLIENT_RESULT populateHttpHeader(HTTP_HEADERS_HANDLE httpHeader,  IOTHUB_CLIENT_EDGE_HANDLE moduleMethodHandle)
{
    IOTHUB_CLIENT_RESULT result;
    STRING_HANDLE scope;
    const char* scope_s;
    STRING_HANDLE moduleHeader;
    const char* moduleHeader_s;
    char* sastoken;

    if ((scope = STRING_construct_sprintf(SCOPE_FMT, moduleMethodHandle->hostname, moduleMethodHandle->deviceId, moduleMethodHandle->moduleId)) == NULL)
    {
        LogError("Failed constructing scope");
        HTTPHeaders_Free(httpHeader);
        result = IOTHUB_CLIENT_ERROR;
    }
    else if ((scope_s = STRING_c_str(scope)) == NULL)
    {
        LogError("SasToken generation failed");
        HTTPHeaders_Free(httpHeader);
        STRING_delete(scope);
        result = IOTHUB_CLIENT_ERROR;
    }
    else if ((sastoken = IoTHubClient_Auth_Get_SasToken(moduleMethodHandle->authorizationHandle, scope_s, SASTOKEN_LIFETIME, NULL)) == NULL)
    {
        LogError("SasToken generation failed");
        HTTPHeaders_Free(httpHeader);
        STRING_delete(scope);
        result = IOTHUB_CLIENT_ERROR;
    }
    else if (HTTPHeaders_ReplaceHeaderNameValuePair(httpHeader, HTTP_HEADER_KEY_AUTHORIZATION, sastoken) != HTTP_HEADERS_OK)
    {
        LogError("Failure updating Http Headers");
        HTTPHeaders_Free(httpHeader);
        STRING_delete(scope);
        free(sastoken);
        result = IOTHUB_CLIENT_ERROR;
    }
    else if ((moduleHeader = STRING_construct_sprintf("%s/%s", moduleMethodHandle->deviceId, moduleMethodHandle->moduleId)) == NULL)
    {
        LogError("Failure updating Http Headers");
        HTTPHeaders_Free(httpHeader);
        STRING_delete(scope);
        free(sastoken);
        result = IOTHUB_CLIENT_ERROR;
    }
    else if ((moduleHeader_s = STRING_c_str(moduleHeader)) == NULL)
    {
        LogError("Failure updating Http Headers");
        HTTPHeaders_Free(httpHeader);
        STRING_delete(scope);
        free(sastoken);
        STRING_delete(moduleHeader);
        result = IOTHUB_CLIENT_ERROR;
    }
    else if (HTTPHeaders_ReplaceHeaderNameValuePair(httpHeader, HTTP_HEADER_KEY_MODULE_ID, moduleHeader_s) != HTTP_HEADERS_OK)
    {
        LogError("Failure updating Http Headers");
        HTTPHeaders_Free(httpHeader);
        STRING_delete(scope);
        free(sastoken);
        STRING_delete(moduleHeader);
        result = IOTHUB_CLIENT_ERROR;
    }
    else
    {
        STRING_delete(scope);
        STRING_delete(moduleHeader);
        free(sastoken);
        result = IOTHUB_CLIENT_OK;
    }

    return result;
}

static IOTHUB_CLIENT_RESULT parseResponseJson(BUFFER_HANDLE responseJson, int* responseStatus, unsigned char** responsePayload, size_t* responsePayloadSize)
{
    IOTHUB_CLIENT_RESULT result;
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
        result = IOTHUB_CLIENT_ERROR;
    }
    else if ((jsonStringHandle = STRING_from_byte_array(bufferStr, BUFFER_length(responseJson))) == NULL)
    {
        LogError("STRING_construct_n failed");
        result = IOTHUB_CLIENT_ERROR;
    }
    else if ((jsonStr = STRING_c_str(jsonStringHandle)) == NULL)
    {
        LogError("STRING_c_str failed");
        STRING_delete(jsonStringHandle);
        result = IOTHUB_CLIENT_ERROR;
    }
    else if ((root_value = json_parse_string(jsonStr)) == NULL)
    {
        LogError("json_parse_string failed");
        STRING_delete(jsonStringHandle);
        result = IOTHUB_CLIENT_ERROR;
    }
    else if ((json_object = json_value_get_object(root_value)) == NULL)
    {
        LogError("json_value_get_object failed");
        STRING_delete(jsonStringHandle);
        json_value_free(root_value);
        result = IOTHUB_CLIENT_ERROR;
    }
    else if ((statusJsonValue = json_object_get_value(json_object, "status")) == NULL)
    {
        LogError("json_object_get_value failed for status");
        STRING_delete(jsonStringHandle);
        json_value_free(root_value);
        result = IOTHUB_CLIENT_ERROR;
    }
    else if ((payloadJsonValue = json_object_get_value(json_object, "payload")) == NULL)
    {
        LogError("json_object_get_value failed for payload");
        STRING_delete(jsonStringHandle);
        json_value_free(root_value);
        result = IOTHUB_CLIENT_ERROR;
    }
    else if ((payload = json_serialize_to_string(payloadJsonValue)) == NULL)
    {
        LogError("json_serialize_to_string failed for payload");
        STRING_delete(jsonStringHandle);
        json_value_free(root_value);
        result = IOTHUB_CLIENT_ERROR;
    }
    else
    {
        *responseStatus = (int)json_value_get_number(statusJsonValue);
        *responsePayload = (unsigned char *)payload;
        *responsePayloadSize = strlen(payload);

        STRING_delete(jsonStringHandle);
        json_value_free(root_value);
        result = IOTHUB_CLIENT_OK;
    }

    return result;
}

static BUFFER_HANDLE createMethodPayloadJson(const char* methodName, unsigned int timeout, const char* payload)
{
    STRING_HANDLE stringHandle;
    const char* stringHandle_c_str;
    BUFFER_HANDLE result;

    if ((stringHandle = STRING_construct_sprintf(PAYLOAD_FMT, methodName, timeout, payload)) == NULL)
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

static STRING_HANDLE createRelativePath(const char* deviceId, const char* moduleId)
{
    STRING_HANDLE result;

    if (moduleId != NULL)
    {
        result = STRING_construct_sprintf(RELATIVE_PATH_FMT_MODULE_METHOD, deviceId, moduleId, URL_API_VERSION);
    }
    else
    {
        result = STRING_construct_sprintf(RELATIVE_PATH_FMT_DEVICE_METHOD, deviceId, URL_API_VERSION);
    }

    return result;
}

static IOTHUB_CLIENT_RESULT sendHttpRequestMethod(IOTHUB_CLIENT_EDGE_HANDLE moduleMethodHandle, const char* deviceId, const char* moduleId, BUFFER_HANDLE deviceJsonBuffer, BUFFER_HANDLE responseBuffer)
{
    IOTHUB_CLIENT_RESULT result;

    HTTPAPIEX_HANDLE httpExApiHandle;
    HTTP_HEADERS_HANDLE httpHeader;
    STRING_HANDLE relativePath;
    const char* relativePath_s;
    char* trustedCertificate;
    unsigned int statusCode = 0;

    // The environment variable ENVIRONMENT_VAR_EDGEHUB_CACERTIFICATEFILE is *optional*; it will not be present in 
    // fact in the vast majority of production scenarios.  Its presence has underlying layer override where it 
    // retrieves trusted certificates from.
    const char* caTrustedCertificateFile = environment_get_variable(ENVIRONMENT_VAR_EDGEHUB_CACERTIFICATEFILE);

    if ((httpHeader = createHttpHeader()) == NULL)
    {
        LogError("HttpHeader creation failed");
        result = IOTHUB_CLIENT_ERROR;
    }
    else if (populateHttpHeader(httpHeader, moduleMethodHandle) != IOTHUB_CLIENT_OK)
    {
        LogError("HttpHeader creation failed");
        result = IOTHUB_CLIENT_ERROR;
    }
    else if ((relativePath = createRelativePath(deviceId, moduleId)) == NULL)
    {
        LogError("Failure creating relative path");
        HTTPHeaders_Free(httpHeader);
        result = IOTHUB_CLIENT_ERROR;
    }
    else if ((relativePath_s = STRING_c_str(relativePath)) == NULL)
    {
        LogError("HTTPAPIEX_Create failed");
        HTTPHeaders_Free(httpHeader);
        STRING_delete(relativePath);
        result = IOTHUB_CLIENT_ERROR;
    }
    else if ((httpExApiHandle = HTTPAPIEX_Create(moduleMethodHandle->hostname)) == NULL)
    {
        LogError("HTTPAPIEX_Create failed");
        HTTPHeaders_Free(httpHeader);
        STRING_delete(relativePath);
        result = IOTHUB_CLIENT_ERROR;
    }
    else if ((trustedCertificate = IoTHubClient_Auth_Get_TrustBundle(moduleMethodHandle->authorizationHandle, caTrustedCertificateFile)) == NULL)
    {
        LogError("Failed to get TrustBundle");
        HTTPHeaders_Free(httpHeader);
        STRING_delete(relativePath);
        HTTPAPIEX_Destroy(httpExApiHandle);
        result = IOTHUB_CLIENT_ERROR;
    }
    else
    {
        if (HTTPAPIEX_SetOption(httpExApiHandle, OPTION_TRUSTED_CERT, trustedCertificate) != HTTPAPIEX_OK)
        {
            LogError("Setting trusted certificate failed");
            result = IOTHUB_CLIENT_ERROR;
        }
        else if (HTTPAPIEX_ExecuteRequest(httpExApiHandle, HTTPAPI_REQUEST_POST, relativePath_s, httpHeader, deviceJsonBuffer, &statusCode, NULL, responseBuffer) != HTTPAPIEX_OK)
        {
            LogError("HTTPAPIEX_ExecuteRequest failed");
            result = IOTHUB_CLIENT_ERROR;
        }
        else
        {
            if (statusCode >= 200 && statusCode < 300)
            {
                result = IOTHUB_CLIENT_OK;
            }
            else
            {
                LogError("Http Failure status code %d.", statusCode);
                result = IOTHUB_CLIENT_ERROR;
            }
        }

        HTTPHeaders_Free(httpHeader);
        STRING_delete(relativePath);
        HTTPAPIEX_Destroy(httpExApiHandle);
        free(trustedCertificate);
    }

    return result;
}

static IOTHUB_CLIENT_RESULT IoTHubClient_Edge_GenericMethodInvoke(IOTHUB_CLIENT_EDGE_HANDLE moduleMethodHandle, const char* deviceId, const char* moduleId, const char* methodName, const char* methodPayload, unsigned int timeout, int* responseStatus, unsigned char** responsePayload, size_t* responsePayloadSize)
{
    IOTHUB_CLIENT_RESULT result;

    BUFFER_HANDLE httpPayloadBuffer;
    BUFFER_HANDLE responseBuffer;

    if ((httpPayloadBuffer = createMethodPayloadJson(methodName, timeout, methodPayload)) == NULL)
    {
        LogError("BUFFER creation failed for httpPayloadBuffer");
        result = IOTHUB_CLIENT_ERROR;
    }
    else if ((responseBuffer = BUFFER_new()) == NULL)
    {
        LogError("BUFFER_new failed for responseBuffer");
        BUFFER_delete(httpPayloadBuffer);
        result = IOTHUB_CLIENT_ERROR;
    }
    else if (sendHttpRequestMethod(moduleMethodHandle, deviceId, moduleId, httpPayloadBuffer, responseBuffer) != IOTHUB_CLIENT_OK)
    {
        LogError("Failure sending HTTP request for device method invoke");
        BUFFER_delete(responseBuffer);
        BUFFER_delete(httpPayloadBuffer);
        result = IOTHUB_CLIENT_ERROR;
    }
    else if ((parseResponseJson(responseBuffer, responseStatus, responsePayload, responsePayloadSize)) != IOTHUB_CLIENT_OK)
    {
        LogError("Failure parsing response");
        BUFFER_delete(responseBuffer);
        BUFFER_delete(httpPayloadBuffer);
        result = IOTHUB_CLIENT_ERROR;
    }
    else
    {
        result = IOTHUB_CLIENT_OK;

        BUFFER_delete(responseBuffer);
        BUFFER_delete(httpPayloadBuffer);
    }

    return result;
}

IOTHUB_CLIENT_RESULT IoTHubClient_Edge_DeviceMethodInvoke(IOTHUB_CLIENT_EDGE_HANDLE moduleMethodHandle, const char* deviceId,  const char* methodName, const char* methodPayload, unsigned int timeout, int* responseStatus, unsigned char** responsePayload, size_t* responsePayloadSize)
{
    IOTHUB_CLIENT_RESULT result;

    if ((moduleMethodHandle == NULL) || (deviceId == NULL) || (methodName == NULL) || (methodPayload == NULL) || (responseStatus == NULL) || (responsePayload == NULL) || (responsePayloadSize == NULL))
    {
        LogError("Input parameter cannot be NULL");
        result = IOTHUB_CLIENT_INVALID_ARG;
    }
    else
    {
        result = IoTHubClient_Edge_GenericMethodInvoke(moduleMethodHandle, deviceId, NULL, methodName, methodPayload, timeout, responseStatus, responsePayload, responsePayloadSize);
    }
    return result;
}

IOTHUB_CLIENT_RESULT IoTHubClient_Edge_ModuleMethodInvoke(IOTHUB_CLIENT_EDGE_HANDLE moduleMethodHandle, const char* deviceId, const char* moduleId, const char* methodName, const char* methodPayload, unsigned int timeout, int* responseStatus, unsigned char** responsePayload, size_t* responsePayloadSize)
{
    IOTHUB_CLIENT_RESULT result;

    if ((moduleMethodHandle == NULL) || (deviceId == NULL) || (moduleId == NULL) || (methodName == NULL) || (methodPayload == NULL) || (responseStatus == NULL) || (responsePayload == NULL) || (responsePayloadSize == NULL))
    {
        LogError("Input parameter cannot be NULL");
        result = IOTHUB_CLIENT_INVALID_ARG;
    }
    else
    {
        result = IoTHubClient_Edge_GenericMethodInvoke(moduleMethodHandle, deviceId, moduleId, methodName, methodPayload, timeout, responseStatus, responsePayload, responsePayloadSize);
    }
    return result;
}
#endif /* USE_EDGE_MODULES */
