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
#include "iothub_deviceconfiguration.h"
#include "iothub_sc_version.h"

MU_DEFINE_ENUM_STRINGS_WITHOUT_INVALID(IOTHUB_DEVICE_CONFIGURATION_RESULT, IOTHUB_DEVICE_CONFIGURATION_RESULT_VALUES);

#define  UID_LENGTH 37
#define IOTHUB_DEVICE_CONFIGURATIONS_MAX_REQUEST 20
static const char*  HTTP_HEADER_KEY_AUTHORIZATION = "Authorization";
static const char*  HTTP_HEADER_VAL_AUTHORIZATION = " ";
static const char*  HTTP_HEADER_KEY_REQUEST_ID = "Request-Id";
static const char*  HTTP_HEADER_KEY_USER_AGENT = "User-Agent";
static const char*  HTTP_HEADER_VAL_USER_AGENT = IOTHUB_SERVICE_CLIENT_TYPE_PREFIX IOTHUB_SERVICE_CLIENT_BACKSLASH IOTHUB_SERVICE_CLIENT_VERSION;
static const char*  HTTP_HEADER_KEY_ACCEPT = "Accept";
static const char*  HTTP_HEADER_VAL_ACCEPT = "application/json";
static const char*  HTTP_HEADER_KEY_CONTENT_TYPE = "Content-Type";
static const char*  HTTP_HEADER_VAL_CONTENT_TYPE = "application/json; charset=utf-8";
static const char*  HTTP_HEADER_KEY_IFMATCH = "If-Match";
static const char*  HTTP_HEADER_VAL_IFMATCH = "*";

#define CONFIGURATION_JSON_KEY_CONTENT "content"
#define CONFIGURATION_JSON_KEY_DEVICE_CONTENT "deviceContent"
#define CONFIGURATION_JSON_KEY_MODULES_CONTENT "modulesContent"
#define CONFIGURATION_JSON_KEY_SYSTEM_METRICS "systemMetrics"
#define CONFIGURATION_JSON_KEY_CUSTOM_METRICS "metrics"
#define CONFIGURATION_JSON_KEY_METRICS_RESULTS "results"
#define CONFIGURATION_JSON_KEY_METRICS_QUERIES "queries"
#define CONFIGURATION_JSON_DOT_SEPARATOR "."

static const char* CONFIGURATION_JSON_KEY_CONFIGURATION_ID = "id";
static const char* CONFIGURATION_JSON_KEY_SCHEMA_VERSION = "schemaVersion";
static const char* CONFIGURATION_JSON_KEY_TARGET_CONDITION = "targetCondition";
static const char* CONFIGURATION_JSON_KEY_CREATED_TIME = "createdTimeUtc";
static const char* CONFIGURATION_JSON_KEY_LAST_UPDATED_TIME = "lastUpdatedTimeUtc";
static const char* CONFIGURATION_JSON_KEY_PRIORITY = "priority";
static const char* CONFIGURATION_JSON_KEY_ETAG = "etag";
static const char* CONFIGURATION_JSON_KEY_LABELS = "labels";

static const char* CONFIGURATION_DEVICE_CONTENT_NODE_NAME = CONFIGURATION_JSON_KEY_CONTENT CONFIGURATION_JSON_DOT_SEPARATOR CONFIGURATION_JSON_KEY_DEVICE_CONTENT;
static const char* CONFIGURATION_MODULES_CONTENT_NODE_NAME = CONFIGURATION_JSON_KEY_CONTENT CONFIGURATION_JSON_DOT_SEPARATOR CONFIGURATION_JSON_KEY_MODULES_CONTENT;
static const char* CONFIGURATION_SYSTEM_METRICS_RESULTS_NODE_NAME = CONFIGURATION_JSON_KEY_SYSTEM_METRICS CONFIGURATION_JSON_DOT_SEPARATOR CONFIGURATION_JSON_KEY_METRICS_RESULTS;
static const char* CONFIGURATION_SYSTEM_METRICS_QUERIES_NODE_NAME = CONFIGURATION_JSON_KEY_SYSTEM_METRICS CONFIGURATION_JSON_DOT_SEPARATOR CONFIGURATION_JSON_KEY_METRICS_QUERIES;
static const char* CONFIGURATION_CUSTOM_METRICS_RESULTS_NODE_NAME = CONFIGURATION_JSON_KEY_CUSTOM_METRICS CONFIGURATION_JSON_DOT_SEPARATOR CONFIGURATION_JSON_KEY_METRICS_RESULTS;
static const char* CONFIGURATION_CUSTOM_METRICS_QUERIES_NODE_NAME = CONFIGURATION_JSON_KEY_CUSTOM_METRICS CONFIGURATION_JSON_DOT_SEPARATOR CONFIGURATION_JSON_KEY_METRICS_QUERIES;

static const char* const URL_API_VERSION = "api-version=2020-09-30";

static const char* const RELATIVE_PATH_FMT_DEVICECONFIGURATION = "/configurations/%s?%s";
static const char* const RELATIVE_PATH_FMT_DEVICECONFIGURATIONS = "/configurations/?top=%d&%s";
static const char* const RELATIVE_PATH_FMT_APPLYCONFIGURATIONCONTENT = "/devices/%s/applyConfigurationContent?%s";

static const char* const CONFIGURATION_DEFAULT_SCHEMA_VERSION = "1.0";
static const char* const CONFIGURATION_DEFAULT_ETAG = "MQ==";

typedef struct IOTHUB_SERVICE_CLIENT_DEVICE_CONFIGURATION_TAG
{
    char* hostname;
    char* sharedAccessKey;
    char* keyName;
} IOTHUB_SERVICE_CLIENT_DEVICE_CONFIGURATION;

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

static STRING_HANDLE createRelativePath(IOTHUB_DEVICECONFIGURATION_REQUEST_MODE iotHubDeviceConfigurationRequestMode, const char* id, size_t numberOfConfigurations)
{
    //IOTHUB_DEVICECONFIGURATION_REQUEST_GET                               GET      {iot hub}/configurations/{configuration id}                                    // Get single device configuration
    //IOTHUB_DEVICECONFIGURATION_REQUEST_ADD                               PUT      {iot hub}/configurations/{configuration id}                                    // Add device configuration
    //IOTHUB_DEVICECONFIGURATION_REQUEST_UPDATE                            PUT      {iot hub}/configurations/{configuration id}                                    // Update device configuration
    //IOTHUB_DEVICECONFIGURATION_REQUEST_DELETE                            DELETE   {iot hub}/configurations/{configuration id}                                    // Delete device configuration
    //IOTHUB_DEVICECONFIGURATION_REQUEST_GET_LIST                          GET      {iot hub}/configurations                                                       // Get multiple configurations
    //IOTHUB_DEVICECONFIGURATION_REQUEST_APPLY_CONFIGURATION_CONTENT       POST     {iot hub}/devices/{deviceOrModule id}/applyConfigurationContent                // Apply device configuration to device or module

    STRING_HANDLE result;

    if (iotHubDeviceConfigurationRequestMode == IOTHUB_DEVICECONFIGURATION_REQUEST_GET_LIST)
    {
        result = STRING_construct_sprintf(RELATIVE_PATH_FMT_DEVICECONFIGURATIONS, numberOfConfigurations, URL_API_VERSION);
    }
    else if ((iotHubDeviceConfigurationRequestMode == IOTHUB_DEVICECONFIGURATION_REQUEST_ADD) || (iotHubDeviceConfigurationRequestMode == IOTHUB_DEVICECONFIGURATION_REQUEST_UPDATE) || (iotHubDeviceConfigurationRequestMode == IOTHUB_DEVICECONFIGURATION_REQUEST_GET) || (iotHubDeviceConfigurationRequestMode == IOTHUB_DEVICECONFIGURATION_REQUEST_DELETE))
    {
        result = STRING_construct_sprintf(RELATIVE_PATH_FMT_DEVICECONFIGURATION, id, URL_API_VERSION);
    }
    else if (iotHubDeviceConfigurationRequestMode == IOTHUB_DEVICECONFIGURATION_REQUEST_APPLY_CONFIGURATION_CONTENT)
    {
        result = STRING_construct_sprintf(RELATIVE_PATH_FMT_APPLYCONFIGURATIONCONTENT, id, URL_API_VERSION);
    }
    else
    {
        result = NULL;
    }
    return result;
}

static HTTP_HEADERS_HANDLE createHttpHeader(IOTHUB_DEVICECONFIGURATION_REQUEST_MODE iotHubDeviceConfigurationRequestMode)
{
    /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_020: [ IoTHubDeviceConfiguration_GetConfiguration shall add the following headers to the created HTTP GET request: authorization=sasToken,Request-Id=<generatedGuid>,Accept=application/json,Content-Type=application/json,charset=utf-8 ]*/
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
    else if (HTTPHeaders_AddHeaderNameValuePair(httpHeader, HTTP_HEADER_KEY_ACCEPT, HTTP_HEADER_VAL_ACCEPT) != HTTP_HEADERS_OK)
    {
        LogError("HTTPHeaders_AddHeaderNameValuePair failed for Accept header");
        HTTPHeaders_Free(httpHeader);
        httpHeader = NULL;
    }
    else if (HTTPHeaders_AddHeaderNameValuePair(httpHeader, HTTP_HEADER_KEY_CONTENT_TYPE, HTTP_HEADER_VAL_CONTENT_TYPE) != HTTP_HEADERS_OK)
    {
        LogError("HTTPHeaders_AddHeaderNameValuePair failed for Content-Type header");
        HTTPHeaders_Free(httpHeader);
        httpHeader = NULL;
    }
    else if ((iotHubDeviceConfigurationRequestMode == IOTHUB_DEVICECONFIGURATION_REQUEST_UPDATE) || (iotHubDeviceConfigurationRequestMode == IOTHUB_DEVICECONFIGURATION_REQUEST_DELETE))
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

static IOTHUB_DEVICE_CONFIGURATION_RESULT sendHttpRequestDeviceConfiguration(IOTHUB_SERVICE_CLIENT_DEVICE_CONFIGURATION_HANDLE serviceClientDeviceConfigurationHandle, IOTHUB_DEVICECONFIGURATION_REQUEST_MODE iotHubDeviceConfigurationRequestMode, const char* id, BUFFER_HANDLE json, size_t maxConfigurationsCount, BUFFER_HANDLE responseBuffer)
{
    IOTHUB_DEVICE_CONFIGURATION_RESULT result;

    STRING_HANDLE uriResource = NULL;
    STRING_HANDLE accessKey = NULL;
    STRING_HANDLE keyName = NULL;
    HTTPAPIEX_SAS_HANDLE httpExApiSasHandle;
    HTTPAPIEX_HANDLE httpExApiHandle;
    HTTP_HEADERS_HANDLE httpHeader;

    if ((uriResource = STRING_construct(serviceClientDeviceConfigurationHandle->hostname)) == NULL)
    {
        /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_024: [ If any of the HTTPAPI call fails IoTHubDeviceConfiguration_GetConfiguration shall fail and return NULL ]*/
        LogError("STRING_construct failed for uriResource");
        result = IOTHUB_DEVICE_CONFIGURATION_ERROR;
    }
    else if ((accessKey = STRING_construct(serviceClientDeviceConfigurationHandle->sharedAccessKey)) == NULL)
    {
        /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_024: [ If any of the call fails during the HTTP creation IoTHubDeviceConfiguration_GetConfiguration shall fail and return NULL ]*/
        LogError("STRING_construct failed for accessKey");
        STRING_delete(uriResource);
        result = IOTHUB_DEVICE_CONFIGURATION_ERROR;
    }
    else if ((keyName = STRING_construct(serviceClientDeviceConfigurationHandle->keyName)) == NULL)
    {
        /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_024: [ If any of the call fails during the HTTP creation IoTHubDeviceConfiguration_GetConfiguration shall fail and return NULL ]*/
        LogError("STRING_construct failed for keyName");
        STRING_delete(accessKey);
        STRING_delete(uriResource);
        result = IOTHUB_DEVICE_CONFIGURATION_ERROR;
    }
    /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_020: [ IoTHubDeviceConfiguration_GetConfiguration shall add the following headers to the created HTTP GET request: authorization=sasToken,Request-Id=<generatedGuid>,Accept=application/json,Content-Type=application/json,charset=utf-8 ]*/
    else if ((httpHeader = createHttpHeader(iotHubDeviceConfigurationRequestMode)) == NULL)
    {
        /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_024: [ If any of the call fails during the HTTP creation IoTHubDeviceConfiguration_GetConfiguration shall fail and return NULL ]*/
        LogError("HttpHeader creation failed");
        STRING_delete(keyName);
        STRING_delete(accessKey);
        STRING_delete(uriResource);
        result = IOTHUB_DEVICE_CONFIGURATION_ERROR;
    }
    /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_021: [ IoTHubDeviceConfiguration_GetConfiguration shall create an HTTPAPIEX_SAS_HANDLE handle by calling HTTPAPIEX_SAS_Create ]*/
    else if ((httpExApiSasHandle = HTTPAPIEX_SAS_Create(accessKey, uriResource, keyName)) == NULL)
    {
        /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_025: [ If any of the HTTPAPI call fails IoTHubDeviceConfiguration_GetConfiguration shall fail and return IOTHUB_DEVICE_CONFIGURATION_HTTPAPI_ERROR ]*/
        LogError("HTTPAPIEX_SAS_Create failed");
        HTTPHeaders_Free(httpHeader);
        STRING_delete(keyName);
        STRING_delete(accessKey);
        STRING_delete(uriResource);
        result = IOTHUB_DEVICE_CONFIGURATION_HTTPAPI_ERROR;
    }
    /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_022: [ IoTHubDeviceConfiguration_GetConfiguration shall create an HTTPAPIEX_HANDLE handle by calling HTTPAPIEX_Create ]*/
    else if ((httpExApiHandle = HTTPAPIEX_Create(serviceClientDeviceConfigurationHandle->hostname)) == NULL)
    {
        /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_025: [ If any of the HTTPAPI call fails IoTHubDeviceConfiguration_GetConfiguration shall fail and return NULL ]*/
        LogError("HTTPAPIEX_Create failed");
        HTTPAPIEX_SAS_Destroy(httpExApiSasHandle);
        HTTPHeaders_Free(httpHeader);
        STRING_delete(keyName);
        STRING_delete(accessKey);
        STRING_delete(uriResource);
        result = IOTHUB_DEVICE_CONFIGURATION_HTTPAPI_ERROR;
    }
    else
    {
        HTTPAPI_REQUEST_TYPE httpApiRequestType = HTTPAPI_REQUEST_GET;
        STRING_HANDLE relativePath;
        unsigned int statusCode = 0;
        unsigned char is_error = 0;

        if ((iotHubDeviceConfigurationRequestMode == IOTHUB_DEVICECONFIGURATION_REQUEST_ADD) || (iotHubDeviceConfigurationRequestMode == IOTHUB_DEVICECONFIGURATION_REQUEST_UPDATE))
        {
            httpApiRequestType = HTTPAPI_REQUEST_PUT;
        }
        else if ((iotHubDeviceConfigurationRequestMode == IOTHUB_DEVICECONFIGURATION_REQUEST_GET) || (iotHubDeviceConfigurationRequestMode == IOTHUB_DEVICECONFIGURATION_REQUEST_GET_LIST))
        {
            httpApiRequestType = HTTPAPI_REQUEST_GET;
        }
        else if (iotHubDeviceConfigurationRequestMode == IOTHUB_DEVICECONFIGURATION_REQUEST_DELETE)
        {
            httpApiRequestType = HTTPAPI_REQUEST_DELETE;
        }
        else if (iotHubDeviceConfigurationRequestMode == IOTHUB_DEVICECONFIGURATION_REQUEST_APPLY_CONFIGURATION_CONTENT)
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
            result = IOTHUB_DEVICE_CONFIGURATION_HTTPAPI_ERROR;
        }
        else
        {
            /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_019: [ IoTHubDeviceConfiguration_GetConfiguration shall create HTTP GET request URL using the given configurationId using the following format: url/configurations/[configurationId] ]*/
            if ((relativePath = createRelativePath(iotHubDeviceConfigurationRequestMode, id, maxConfigurationsCount)) == NULL)
            {
                /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_024: [ If any of the call fails during the HTTP creation IoTHubDeviceConfiguration_GetConfiguration shall fail and return NULL ]*/
                LogError("Failure creating relative path");
                result = IOTHUB_DEVICE_CONFIGURATION_ERROR;
            }
            /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_023: [ IoTHubDeviceConfiguration_GetConfiguration shall execute the HTTP GET request by calling HTTPAPIEX_ExecuteRequest ]*/
            else if (HTTPAPIEX_SAS_ExecuteRequest(httpExApiSasHandle, httpExApiHandle, httpApiRequestType, STRING_c_str(relativePath), httpHeader, json, &statusCode, NULL, responseBuffer) != HTTPAPIEX_OK)
            {
                /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_025: [ If any of the HTTPAPI call fails IoTHubDeviceConfiguration_GetConfiguration shall fail and return NULL ]*/
                LogError("HTTPAPIEX_SAS_ExecuteRequest failed");
                STRING_delete(relativePath);
                result = IOTHUB_DEVICE_CONFIGURATION_HTTPAPI_ERROR;
            }
            else
            {
                STRING_delete(relativePath);
                if ((((iotHubDeviceConfigurationRequestMode == IOTHUB_DEVICECONFIGURATION_REQUEST_ADD) ||
                    (iotHubDeviceConfigurationRequestMode == IOTHUB_DEVICECONFIGURATION_REQUEST_GET) ||
                    (iotHubDeviceConfigurationRequestMode == IOTHUB_DEVICECONFIGURATION_REQUEST_GET_LIST) ||
                    (iotHubDeviceConfigurationRequestMode == IOTHUB_DEVICECONFIGURATION_REQUEST_UPDATE)) && (statusCode == 200)) ||
                    ((iotHubDeviceConfigurationRequestMode == IOTHUB_DEVICECONFIGURATION_REQUEST_DELETE) && (statusCode == 204)) ||
                    ((iotHubDeviceConfigurationRequestMode == IOTHUB_DEVICECONFIGURATION_REQUEST_APPLY_CONFIGURATION_CONTENT) && ((statusCode == 200) || (statusCode == 204)))
                    )
                {
                    /*CodesSRS_IOTHUBDEVICECONFIGURATION_38_030: [ Otherwise IoTHubDeviceConfiguration_GetConfiguration shall save the received deviceConfiguration to the out parameter and return with it ]*/
                    result = IOTHUB_DEVICE_CONFIGURATION_OK;
                }
                else
                {
                    /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_026: [ IoTHubDeviceConfiguration_GetConfiguration shall verify the received HTTP status code and if it is not equal to 200 then return NULL ]*/
                    LogError("Http Failure status code %d.", statusCode);
                    result = IOTHUB_DEVICE_CONFIGURATION_ERROR;
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

static JSON_Value* createConfigurationContentPayload(const IOTHUB_DEVICE_CONFIGURATION_CONTENT* configurationContent)
{
    JSON_Value* result = NULL;
    JSON_Object* root_object = NULL;

    if (configurationContent == NULL)
    {
        LogError("configuration cannot be null");
        result = NULL;
    }
    else if (((configurationContent->deviceContent == NULL) && (configurationContent->modulesContent == NULL)) ||
        ((configurationContent->deviceContent != NULL && strlen(configurationContent->deviceContent) == 0) && (configurationContent->modulesContent != NULL && strlen(configurationContent->modulesContent) == 0)))
    {
        LogError("both deviceContent and modulesContent cannot be NULL or empty");
        result = NULL;
    }
    else if ((result = json_value_init_object()) == NULL)
    {
        LogError("json_value_init_object failed");
    }
    else if ((root_object = json_value_get_object(result)) == NULL)
    {
        LogError("json_value_get_object failed");
        result = NULL;
    }
    else if ((configurationContent->deviceContent != NULL) && (strlen(configurationContent->deviceContent) > 0) && (json_object_set_value(root_object, CONFIGURATION_JSON_KEY_DEVICE_CONTENT, json_parse_string(configurationContent->deviceContent))) != JSONSuccess)
    {
        LogError("json_object_set_string failed for deviceContent");
        result = NULL;
    }
    else if ((configurationContent->modulesContent != NULL) && (strlen(configurationContent->modulesContent) > 0) && (json_object_set_value(root_object, CONFIGURATION_JSON_KEY_MODULES_CONTENT, json_parse_string(configurationContent->modulesContent))) != JSONSuccess)
    {
        LogError("json_object_set_string failed for modulesContent");
        result = NULL;
    }

    return result;
}

static JSON_Value* createConfigurationMetricsQueriesPayload(const IOTHUB_DEVICE_CONFIGURATION_METRICS_DEFINITION* configurationMetricsDefinition)
{
    JSON_Value* result = NULL;
    JSON_Value* new_value = NULL;
    JSON_Object* root_object = NULL;
    JSON_Object* new_value_object = NULL;

    if (configurationMetricsDefinition == NULL)
    {
        LogError("configuration cannot be null");
        result = NULL;
    }
    else if ((result = json_value_init_object()) == NULL || (new_value = json_value_init_object()) == NULL)
    {
        LogError("json_value_init_object failed");
        result = NULL;
    }
    else if ((root_object = json_value_get_object(result)) == NULL || (new_value_object = json_value_get_object(new_value)) == NULL)
    {
        LogError("json_value_get_object failed");
        result = NULL;
    }
    else if (json_object_set_value(root_object, CONFIGURATION_JSON_KEY_METRICS_QUERIES, new_value) != JSONSuccess)
    {
        LogError("json_object_set_value failed for deviceContent");
        result = NULL;
    }
    else
    {
        for (size_t i = 0; i < configurationMetricsDefinition->numQueries; i++)
        {
            if (json_object_set_string(new_value_object, (const char*)configurationMetricsDefinition->queryNames[i], configurationMetricsDefinition->queryStrings[i]) != JSONSuccess)
            {
                LogError("json_object_set_string failed");
                result = NULL;
                break;
            }
        }
    }

    return result;
}

static JSON_Value* createConfigurationLabelsPayload(const IOTHUB_DEVICE_CONFIGURATION_LABELS* configurationLabels)
{
    JSON_Value* result = NULL;
    JSON_Object* root_object = NULL;

    if (configurationLabels == NULL)
    {
        LogError("configurationLabels cannot be null");
        result = NULL;
    }
    else if (configurationLabels->numLabels == 0)
    {
        /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_010: [ IoTHubDeviceConfiguration_AddConfiguration shall return NULL for labels, when device configuration label list is empty ] */
        result = NULL;
    }
    else if ((result = json_value_init_object()) == NULL)
    {
        LogError("json_value_init_object failed");
        result = NULL;
    }
    else if ((root_object = json_value_get_object(result)) == NULL)
    {
        LogError("json_value_get_object failed");
        result = NULL;
    }
    else
    {
        for (size_t i = 0; i < configurationLabels->numLabels; i++)
        {
            if (json_object_set_string(root_object, (const char*)configurationLabels->labelNames[i], configurationLabels->labelValues[i]) != JSONSuccess)
            {
                LogError("json_object_set_value failed");
                result = NULL;
                break;
            }
        }
    }

    return result;
}

static BUFFER_HANDLE createConfigurationPayloadJson(const IOTHUB_DEVICE_CONFIGURATION* configuration)
{
    BUFFER_HANDLE result;

    JSON_Value* root_value = NULL;
    JSON_Object* root_object = NULL;
    JSON_Value* configurationContentJson;
    JSON_Value* configurationLabelsJson;
    JSON_Value* metricsQueriesJson;
    JSON_Value* systemMetricsQueriesJson;

    if (configuration == NULL)
    {
        LogError("configuration cannot be null");
        result = NULL;
    }
    else if (configuration->configurationId == NULL)
    {
        LogError("Configuration id cannot be NULL");
        result = NULL;
    }
    else if (((configuration->content).deviceContent == NULL) && (((configuration->content).modulesContent) == NULL))
    {
        LogError("deviceContent and modulesContent both cannot be NULL");
        result = NULL;
    }
    else if ((root_value = json_value_init_object()) == NULL)
    {
        LogError("json_value_init_object failed");
        result = NULL;
    }
    else if ((root_object = json_value_get_object(root_value)) == NULL)
    {
        LogError("json_value_get_object failed");
        result = NULL;
    }
    else if ((json_object_set_string(root_object, CONFIGURATION_JSON_KEY_CONFIGURATION_ID, configuration->configurationId)) != JSONSuccess)
    {
        LogError("json_object_set_string failed for configurationId");
        result = NULL;
    }
    else if ((json_object_set_string(root_object, CONFIGURATION_JSON_KEY_SCHEMA_VERSION, configuration->schemaVersion)) != JSONSuccess)
    {
        LogError("json_object_set_string failed for schemaVersion");
        result = NULL;
    }
    else if ((json_object_set_string(root_object, CONFIGURATION_JSON_KEY_TARGET_CONDITION, configuration->targetCondition)) != JSONSuccess)
    {
        LogError("json_object_set_string failed for targetCondition");
        result = NULL;
    }
    else if ((json_object_set_number(root_object, CONFIGURATION_JSON_KEY_PRIORITY, configuration->priority)) != JSONSuccess)
    {
        LogError("json_object_set_string failed for priority");
        result = NULL;
    }
    else if ((json_object_set_string(root_object, CONFIGURATION_JSON_KEY_ETAG, configuration->eTag)) != JSONSuccess)
    {
        LogError("json_object_set_string failed for eTag");
        result = NULL;
    }
    else if (((configuration->createdTimeUtc != NULL) && json_object_set_string(root_object, CONFIGURATION_JSON_KEY_CREATED_TIME, configuration->createdTimeUtc)) != JSONSuccess)
    {
        LogError("json_object_set_string failed for createdTimeUtc");
        result = NULL;
    }
    else if (((configuration->lastUpdatedTimeUtc != NULL) && json_object_set_string(root_object, CONFIGURATION_JSON_KEY_CREATED_TIME, configuration->lastUpdatedTimeUtc)) != JSONSuccess)
    {
        LogError("json_object_set_string failed for lastUpdatedTimeUtc");
        result = NULL;
    }
    else if (((configurationLabelsJson = createConfigurationLabelsPayload(&configuration->labels)) != NULL) && ((json_object_set_value(root_object, CONFIGURATION_JSON_KEY_LABELS, configurationLabelsJson)) != JSONSuccess))
    {
        LogError("json_object_set_string failed for configurationLabelsJson");
        result = NULL;
    }
    else if ((((configurationContentJson = createConfigurationContentPayload(&configuration->content)) != NULL) && ((json_object_set_value(root_object, CONFIGURATION_JSON_KEY_CONTENT, configurationContentJson)) != JSONSuccess)))
    {
        LogError("json_object_set_string failed for configurationContentJson");
        result = NULL;
    }
    else if (((metricsQueriesJson = createConfigurationMetricsQueriesPayload(&configuration->metricsDefinition)) != NULL) && ((json_object_set_value(root_object, CONFIGURATION_JSON_KEY_CUSTOM_METRICS, metricsQueriesJson)) != JSONSuccess))
    {
        LogError("json_object_set_string failed for metricsQueriesJson");
        result = NULL;
    }
    else if (((systemMetricsQueriesJson = createConfigurationMetricsQueriesPayload(&configuration->systemMetricsDefinition)) != NULL) && ((json_object_set_value(root_object, CONFIGURATION_JSON_KEY_SYSTEM_METRICS, systemMetricsQueriesJson)) != JSONSuccess))
    {
        LogError("json_object_set_string failed for systemMetricsQueriesJson");
        result = NULL;
    }
    else
    {
        char* serialized_string;
        if ((serialized_string = json_serialize_to_string(root_value)) == NULL)
        {
            LogError("json_serialize_to_string failed");
            result = NULL;
        }
        else
        {
            if ((result = BUFFER_create((const unsigned char*)serialized_string, strlen(serialized_string))) == NULL)
            {
                LogError("Buffer_Create failed");
                result = NULL;
            }
            json_free_serialized_string(serialized_string);
        }
    }

    json_object_clear(root_object);

    if (root_value != NULL)
        json_value_free(root_value);

    return result;
}

static IOTHUB_DEVICE_CONFIGURATION_RESULT parseDeviceConfigurationLabelsJsonObject(const JSON_Object* labelsJson, IOTHUB_DEVICE_CONFIGURATION_LABELS* labels)
{
    IOTHUB_DEVICE_CONFIGURATION_RESULT result;
    STRING_HANDLE tempLabelsName = NULL;
    STRING_HANDLE tempLabelsValue = NULL;

    if (labels == NULL)
    {
        LogError("labels cannot be NULL");
        result = IOTHUB_DEVICE_CONFIGURATION_INVALID_ARG;
    }
    else if (labelsJson == NULL)
    {
        labels->numLabels = 0;
        result = IOTHUB_DEVICE_CONFIGURATION_OK;
    }
    else
    {
        result = IOTHUB_DEVICE_CONFIGURATION_OK;

        size_t labelsCount = json_object_get_count(labelsJson);

        labels->numLabels = labelsCount;
        if (labelsCount > 0)
        {
            if ((labels->labelNames = malloc(sizeof(const char*) * labelsCount)) == NULL)
            {
                LogError("Malloc failed for IOTHUB_DEVICE_CONFIGURATION_METRICS_RESULT labelNames");
                result = IOTHUB_DEVICE_CONFIGURATION_OUT_OF_MEMORY_ERROR;
            }
            else if ((labels->labelValues = malloc(sizeof(const char*) * labelsCount)) == NULL)
            {
                LogError("Malloc failed for IOTHUB_DEVICE_CONFIGURATION_METRICS_RESULT results");
                result = IOTHUB_DEVICE_CONFIGURATION_OUT_OF_MEMORY_ERROR;
            }
            else
            {
                for (size_t i = 0; i < labelsCount; i++)
                {
                    if ((tempLabelsName = STRING_construct(json_object_get_name(labelsJson, i))) == NULL)
                    {
                        LogError("STRING_construct failed for tempLabelsName");
                        result = IOTHUB_DEVICE_CONFIGURATION_ERROR;
                    }
                    else if (!json_object_has_value(labelsJson, STRING_c_str(tempLabelsName)))
                    {
                        LogError("missing result for label %s", STRING_c_str(tempLabelsName));
                        result = IOTHUB_DEVICE_CONFIGURATION_JSON_ERROR;
                    }
                    else if (mallocAndStrcpy_s((char**)&(labels->labelNames[i]), STRING_c_str(tempLabelsName)) != 0)
                    {
                        LogError("mallocAndStrcpy_s failed for results tempLabelsName");
                        result = IOTHUB_DEVICE_CONFIGURATION_ERROR;
                    }
                    else if ((tempLabelsValue = STRING_construct(json_value_get_string(json_object_get_value_at(labelsJson, i)))) == NULL)
                    {
                        LogError("STRING_construct failed for tempMetricQueryString");
                        result = IOTHUB_DEVICE_CONFIGURATION_ERROR;
                    }
                    else if (mallocAndStrcpy_s((char**)&(labels->labelValues[i]), STRING_c_str(tempLabelsValue)) != 0)
                    {
                        LogError("mallocAndStrcpy_s failed for tempMetricQueryString");
                        result = IOTHUB_DEVICE_CONFIGURATION_ERROR;
                    }
                }
            }
        }
    }

    STRING_delete(tempLabelsName);
    STRING_delete(tempLabelsValue);

    return result;
}

static IOTHUB_DEVICE_CONFIGURATION_RESULT parseDeviceConfigurationMetricsJsonObject(const JSON_Object* metricResults, const JSON_Object* metricQueries, IOTHUB_DEVICE_CONFIGURATION_METRICS_RESULT* results, IOTHUB_DEVICE_CONFIGURATION_METRICS_DEFINITION* queries)
{
    IOTHUB_DEVICE_CONFIGURATION_RESULT result;
    size_t metricResultsCount = json_object_get_count(metricResults);
    size_t metricQueriesCount = json_object_get_count(metricQueries);
    const char* tempMetricQueryName = NULL;
    const char* tempMetricQueryString = NULL;

    if (metricResults == NULL || metricQueries == NULL || results == NULL || queries == NULL)
    {
        LogError("metricResults, metricQueries, results or queries cannot be NULL");
        result = IOTHUB_DEVICE_CONFIGURATION_INVALID_ARG;
    }
    else
    {
        result = IOTHUB_DEVICE_CONFIGURATION_OK;

        results->numQueries = metricResultsCount;
        if (metricResultsCount > 0)
        {
            if ((results->queryNames = calloc(metricResultsCount, sizeof(const char*))) == NULL)
            {
                LogError("Calloc failed for IOTHUB_DEVICE_CONFIGURATION_METRICS_RESULT queryNames");
                result = IOTHUB_DEVICE_CONFIGURATION_ERROR;
            }
            else if ((results->results = calloc(metricResultsCount, sizeof(double))) == NULL)
            {
                LogError("Calloc failed for IOTHUB_DEVICE_CONFIGURATION_METRICS_RESULT results");
                result = IOTHUB_DEVICE_CONFIGURATION_ERROR;
            }
            else
            {
                for (size_t i = 0; i < metricResultsCount; i++)
                {
                    if ((tempMetricQueryName = json_object_get_name(metricResults, i)) == NULL)
                    {
                        LogError("STRING_construct failed for tempMetricQueryName");
                        result = IOTHUB_DEVICE_CONFIGURATION_JSON_ERROR;
                    }
                    else if (mallocAndStrcpy_s((char**)&(results->queryNames[i]), tempMetricQueryName) != 0)
                    {
                        LogError("mallocAndStrcpy_s failed for queries tempMetricQueryName");
                        result = IOTHUB_DEVICE_CONFIGURATION_ERROR;
                    }
                    else
                    {
                        results->results[i] = json_object_dotget_number(metricResults, tempMetricQueryName);
                    }
                }
            }
        }

        queries->numQueries = metricQueriesCount;
        if (metricQueriesCount > 0)
        {
            if ((queries->queryNames = calloc(metricQueriesCount, sizeof(const char*))) == NULL)
            {
                LogError("Malloc failed for IOTHUB_DEVICE_CONFIGURATION_METRICS_DEFINITION queryNames");
                result = IOTHUB_DEVICE_CONFIGURATION_OUT_OF_MEMORY_ERROR;
            }
            else if ((queries->queryStrings = calloc(metricQueriesCount, sizeof(const char*))) == NULL)
            {
                LogError("Malloc failed for IOTHUB_DEVICE_CONFIGURATION_METRICS_DEFINITION queryStrings");
                result = IOTHUB_DEVICE_CONFIGURATION_OUT_OF_MEMORY_ERROR;
            }
            else
            {
                for (size_t i = 0; i < metricQueriesCount; i++)
                {
                    if ((tempMetricQueryName = json_object_get_name(metricQueries, i)) == NULL)
                    {
                        LogError("STRING_construct failed for tempMetricQueryName");
                        result = IOTHUB_DEVICE_CONFIGURATION_JSON_ERROR;
                    }
                    else if (mallocAndStrcpy_s((char**)&(queries->queryNames[i]), tempMetricQueryName) != 0)
                    {
                        LogError("mallocAndStrcpy_s failed for queries tempMetricQueryName");
                        result = IOTHUB_DEVICE_CONFIGURATION_ERROR;
                    }
                    else if ((tempMetricQueryString = json_value_get_string(json_object_get_value_at(metricQueries, i))) == NULL)
                    {
                        LogError("STRING_construct failed for tempMetricQueryString");
                        result = IOTHUB_DEVICE_CONFIGURATION_JSON_ERROR;
                    }
                    else if (mallocAndStrcpy_s((char**)&(queries->queryStrings[i]), tempMetricQueryString) != 0)
                    {
                        LogError("mallocAndStrcpy_s failed for tempMetricQueryString");
                        result = IOTHUB_DEVICE_CONFIGURATION_ERROR;
                    }
                }
            }
        }
    }

    return result;
}

static IOTHUB_DEVICE_CONFIGURATION_RESULT parseDeviceConfigurationJsonObject(JSON_Object* root_object, IOTHUB_DEVICE_CONFIGURATION* configuration)
{
    IOTHUB_DEVICE_CONFIGURATION_RESULT result;

    const char* configurationId = json_object_get_string(root_object, CONFIGURATION_JSON_KEY_CONFIGURATION_ID);
    const char* schemaVersion = json_object_get_string(root_object, CONFIGURATION_JSON_KEY_SCHEMA_VERSION);
    const char* deviceContent = json_serialize_to_string(json_object_dotget_value(root_object, CONFIGURATION_DEVICE_CONTENT_NODE_NAME));
    const char* modulesContent = json_serialize_to_string(json_object_dotget_value(root_object, CONFIGURATION_MODULES_CONTENT_NODE_NAME));
    const char* targetCondition = json_object_get_string(root_object, CONFIGURATION_JSON_KEY_TARGET_CONDITION);
    const char* createdTime = json_object_get_string(root_object, CONFIGURATION_JSON_KEY_CREATED_TIME);
    const char* lastUpdatedTime = json_object_get_string(root_object, CONFIGURATION_JSON_KEY_LAST_UPDATED_TIME);
    const char* priority = json_object_get_string(root_object, CONFIGURATION_JSON_KEY_PRIORITY);
    const char* eTag = json_object_get_string(root_object, CONFIGURATION_JSON_KEY_ETAG);

    JSON_Object* systemMetricsResults = json_object_dotget_object(root_object, CONFIGURATION_SYSTEM_METRICS_RESULTS_NODE_NAME);
    JSON_Object* systemMetricsQueries = json_object_dotget_object(root_object, CONFIGURATION_SYSTEM_METRICS_QUERIES_NODE_NAME);
    JSON_Object* customMetricsResults = json_object_dotget_object(root_object, CONFIGURATION_CUSTOM_METRICS_RESULTS_NODE_NAME);
    JSON_Object* customMetricsQueries = json_object_dotget_object(root_object, CONFIGURATION_CUSTOM_METRICS_QUERIES_NODE_NAME);

    JSON_Object* labels = json_object_dotget_object(root_object, CONFIGURATION_JSON_KEY_LABELS);

    if ((configurationId != NULL) && (mallocAndStrcpy_s((char**)&(configuration->configurationId), configurationId) != 0))
    {
        LogError("mallocAndStrcpy_s failed for configurationId");
        result = IOTHUB_DEVICE_CONFIGURATION_ERROR;
    }
    else if ((schemaVersion != NULL) && (mallocAndStrcpy_s((char**)&(configuration->schemaVersion), schemaVersion) != 0))
    {
        LogError("mallocAndStrcpy_s failed for schemaVersion");
        result = IOTHUB_DEVICE_CONFIGURATION_ERROR;
    }
    else if ((deviceContent != NULL) && (mallocAndStrcpy_s((char**)&configuration->content.deviceContent, deviceContent) != 0))
    {
        LogError("mallocAndStrcpy_s failed for content.deviceContent");
        result = IOTHUB_DEVICE_CONFIGURATION_ERROR;
    }
    else if ((modulesContent != NULL) && (mallocAndStrcpy_s((char**)&configuration->content.modulesContent, modulesContent) != 0))
    {
        LogError("mallocAndStrcpy_s failed for content.modulesContent");
        result = IOTHUB_DEVICE_CONFIGURATION_ERROR;
    }
    else if ((eTag != NULL) && (mallocAndStrcpy_s((char**)&configuration->eTag, eTag) != 0))
    {
        LogError("mallocAndStrcpy_s failed for eTag");
        result = IOTHUB_DEVICE_CONFIGURATION_ERROR;
    }
    else if ((targetCondition != NULL) && (mallocAndStrcpy_s((char**)&configuration->targetCondition, targetCondition) != 0))
    {
        LogError("mallocAndStrcpy_s failed for targetCondition");
        result = IOTHUB_DEVICE_CONFIGURATION_ERROR;
    }
    else if ((createdTime != NULL) && (mallocAndStrcpy_s((char**)&configuration->createdTimeUtc, createdTime) != 0))
    {
        LogError("mallocAndStrcpy_s failed for createdTimeUtc");
        result = IOTHUB_DEVICE_CONFIGURATION_ERROR;
    }
    else if ((lastUpdatedTime != NULL) && (mallocAndStrcpy_s((char**)&configuration->lastUpdatedTimeUtc, lastUpdatedTime) != 0))
    {
        LogError("mallocAndStrcpy_s failed for lastUpdatedTimeUtc");
        result = IOTHUB_DEVICE_CONFIGURATION_ERROR;
    }
    else
    {
        if (priority != NULL)
        {
            configuration->priority = (int)json_object_get_number(root_object, priority);
        }

        if ((result = parseDeviceConfigurationMetricsJsonObject(systemMetricsResults, systemMetricsQueries, &(configuration->systemMetricsResult), &(configuration->systemMetricsDefinition))) != IOTHUB_DEVICE_CONFIGURATION_OK)
        {
            LogError("parseDeviceConfigurationMetricsJsonObject failed for systemMetrics");
        }
        else if ((result = parseDeviceConfigurationMetricsJsonObject(customMetricsResults, customMetricsQueries, &(configuration->metricResult), &(configuration->metricsDefinition))) != IOTHUB_DEVICE_CONFIGURATION_OK)
        {
            LogError("parseDeviceConfigurationMetricsJsonObject failed for systemMetrics");
        }
        else if ((result = parseDeviceConfigurationLabelsJsonObject(labels, &(configuration->labels))) != IOTHUB_DEVICE_CONFIGURATION_OK)
        {
            LogError("parseDeviceConfigurationLabelsJsonObject failed for systemMetrics");
        }
    }

    return result;
}

void IoTHubDeviceConfiguration_FreeConfigurationMembers(IOTHUB_DEVICE_CONFIGURATION* configuration)
{
    free((void *)configuration->configurationId);
    free((char *)configuration->schemaVersion);
    free((char *)configuration->targetCondition);
    free((char *)configuration->eTag);
    free((char *)configuration->createdTimeUtc);
    free((char *)configuration->lastUpdatedTimeUtc);

    if (configuration->content.deviceContent != NULL) free((char *)configuration->content.deviceContent);
    if (configuration->content.modulesContent != NULL) free((char *)configuration->content.modulesContent);

    if (configuration->labels.numLabels > 0)
    {
        for (size_t i = 0; i < configuration->labels.numLabels; i++)
        {
            free((void *)(configuration->labels.labelNames[i]));
            free((void *)(configuration->labels.labelValues[i]));
        }

        free((void *)configuration->labels.labelNames);
        free((void *)configuration->labels.labelValues);
    }

    if (configuration->metricsDefinition.numQueries > 0)
    {
        for (size_t i = 0; i < configuration->metricsDefinition.numQueries; i++)
        {
            free((void *)(configuration->metricsDefinition.queryNames[i]));
            free((void *)(configuration->metricsDefinition.queryStrings[i]));
        }

        free((void *)configuration->metricsDefinition.queryNames);
        free((void *)configuration->metricsDefinition.queryStrings);
    }


    if (configuration->metricResult.numQueries > 0)
    {
        for (size_t i = 0; i < configuration->metricResult.numQueries; i++)
        {
            free((void *)(configuration->metricResult.queryNames[i]));
        }

        free((void *)configuration->metricResult.queryNames);
        free((void *)configuration->metricResult.results);
    }


    if (configuration->systemMetricsDefinition.numQueries > 0)
    {
        for (size_t i = 0; i < configuration->systemMetricsDefinition.numQueries; i++)
        {
            free((void *)(configuration->systemMetricsDefinition.queryNames[i]));
            free((void *)(configuration->systemMetricsDefinition.queryStrings[i]));
        }

        free((void *)configuration->systemMetricsDefinition.queryNames);
        free((void *)configuration->systemMetricsDefinition.queryStrings);
    }

    if (configuration->systemMetricsResult.numQueries > 0)
    {
        for (size_t i = 0; i < configuration->systemMetricsResult.numQueries; i++)
        {
            free((void *)(configuration->systemMetricsResult.queryNames[i]));
        }

        free((void *)configuration->systemMetricsResult.queryNames);
        free((void *)configuration->systemMetricsResult.results);
    }

    memset(configuration, 0, sizeof(*configuration));
}

static IOTHUB_DEVICE_CONFIGURATION_RESULT parseDeviceConfigurationJson(BUFFER_HANDLE jsonBuffer, IOTHUB_DEVICE_CONFIGURATION* configuration)
{
    IOTHUB_DEVICE_CONFIGURATION_RESULT result;

    /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_024: [ If the configuration out parameter is not NULL IoTHubDeviceConfiguration_AddConfiguration shall save the received configuration to the out parameter and return IOTHUB_DEVICE_CONFIGURATION_OK ] */
    /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_033: [ IoTHubDeviceConfiguration_GetConfiguration shall verify the received HTTP status code and if it is less or equal than 300 then try to parse the response JSON to configuration for configuration properties ] */
    /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_034: [ If any of the property field above missing from the JSON the property value will not be populated ] */
    if (jsonBuffer == NULL)
    {
        /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_023: [ If the JSON parsing failed, IoTHubDeviceConfiguration_AddConfiguration shall return IOTHUB_DEVICE_CONFIGURATION_ERROR ] */
        /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_035: [ If the JSON parsing failed, IoTHubDeviceConfiguration_GetConfiguration shall return IOTHUB_DEVICE_CONFIGURATION_ERROR ] */
        LogError("jsonBuffer cannot be NULL");
        result = IOTHUB_DEVICE_CONFIGURATION_ERROR;
    }
    else if (configuration == NULL)
    {
        /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_023: [ If the JSON parsing failed, IoTHubDeviceConfiguration_AddConfiguration shall return IOTHUB_DEVICE_CONFIGURATION_ERROR ] */
        /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_035: [ If the JSON parsing failed, IoTHubDeviceConfiguration_GetConfiguration shall return IOTHUB_DEVICE_CONFIGURATION_ERROR ] */
        LogError("configuration cannot be NULL");
        result = IOTHUB_DEVICE_CONFIGURATION_ERROR;
    }
    else
    {
        const char* bufferStr = NULL;
        JSON_Value* root_value = NULL;
        JSON_Object* root_object = NULL;

        if ((bufferStr = (const char*)BUFFER_u_char(jsonBuffer)) == NULL)
        {
            /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_023: [ If the JSON parsing failed, IoTHubDeviceConfiguration_AddConfiguration shall return IOTHUB_DEVICE_CONFIGURATION_ERROR ] */
            /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_035: [ If the JSON parsing failed, IoTHubDeviceConfiguration_GetConfiguration shall return IOTHUB_DEVICE_CONFIGURATION_ERROR ] */
            LogError("BUFFER_u_char failed");
            result = IOTHUB_DEVICE_CONFIGURATION_ERROR;
        }
        /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_036: [ If the received JSON is empty, IoTHubDeviceConfiguration_GetConfiguration shall return IOTHUB_DEVICE_CONFIGURATION_CONFIGURATION_NOT_EXIST ] */
        else if (strlen(bufferStr) == 0)
        {
            LogError("Returned JSON cannot be empty");
            result = IOTHUB_DEVICE_CONFIGURATION_CONFIGURATION_NOT_EXIST;
        }
        else if ((root_value = json_parse_string(bufferStr)) == NULL)
        {
            /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_023: [ If the JSON parsing failed, IoTHubDeviceConfiguration_AddConfiguration shall return IOTHUB_DEVICE_CONFIGURATION_JSON_ERROR ] */
            /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_035: [ If the JSON parsing failed, IoTHubDeviceConfiguration_GetConfiguration shall return IOTHUB_DEVICE_CONFIGURATION_JSON_ERROR ] */
            LogError("json_parse_string failed");
            result = IOTHUB_DEVICE_CONFIGURATION_JSON_ERROR;
        }
        else if ((root_object = json_value_get_object(root_value)) == NULL)
        {
            /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_023: [ If the JSON parsing failed, IoTHubDeviceConfiguration_AddConfiguration shall return IOTHUB_DEVICE_CONFIGURATION_JSON_ERROR ] */
            /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_035: [ If the JSON parsing failed, IoTHubDeviceConfiguration_GetConfiguration shall return IOTHUB_DEVICE_CONFIGURATION_JSON_ERROR ] */
            LogError("json_value_get_object failed");
            result = IOTHUB_DEVICE_CONFIGURATION_JSON_ERROR;
        }
        else
        {
            result = parseDeviceConfigurationJsonObject(root_object, configuration);
        }

        json_object_clear(root_object);
        json_value_free(root_value);
    }
    return result;
}

static void free_deviceConfiguration_handle(IOTHUB_SERVICE_CLIENT_DEVICE_CONFIGURATION* deviceConfiguration)
{
    free(deviceConfiguration->hostname);
    free(deviceConfiguration->sharedAccessKey);
    free(deviceConfiguration->keyName);
    free(deviceConfiguration);
}

IOTHUB_SERVICE_CLIENT_DEVICE_CONFIGURATION_HANDLE IoTHubDeviceConfiguration_Create(IOTHUB_SERVICE_CLIENT_AUTH_HANDLE serviceClientHandle)
{
    IOTHUB_SERVICE_CLIENT_DEVICE_CONFIGURATION* result;

    /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_001: [ If the serviceClientHandle input parameter is NULL IoTHubDeviceConfiguration_Create shall return NULL ]*/
    if (serviceClientHandle == NULL)
    {
        LogError("IotHubDeviceConfiguration_Create: serviceClientHandle is null");
        result = NULL;
    }
    else
    {
        /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_002: [ If any member of the serviceClientHandle input parameter is NULL IoTHubDeviceConfiguration_Create shall return NULL ]*/
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
            /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_003: [ IoTHubDeviceMethod_Create shall allocate memory for a new IOTHUB_SERVICE_CLIENT_DEVICE_METHOD_HANDLE instance ]*/
            result = malloc(sizeof(IOTHUB_SERVICE_CLIENT_DEVICE_CONFIGURATION));
            if (result == NULL)
            {
                /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_002: [ If the allocation failed, IoTHubDeviceConfiguration_Create shall return NULL ]*/
                LogError("Malloc failed for IOTHUB_SERVICE_CLIENT_DEVICE_CONFIGURATION");
            }
            else
            {
                memset(result, 0, sizeof(IOTHUB_SERVICE_CLIENT_DEVICE_CONFIGURATION));

                /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_005: [ If the allocation successful, IoTHubDeviceConfiguration_Create shall create a IOTHUB_SERVICE_CLIENT_DEVICE_CONFIGURATION_HANDLE from the given IOTHUB_SERVICE_CLIENT_AUTH_HANDLE and return with it ]*/
                /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_006: [ IoTHubDeviceConfiguration_Create shall allocate memory and copy hostName to result->hostName by calling mallocAndStrcpy_s. ]*/
                if (mallocAndStrcpy_s(&result->hostname, serviceClientAuth->hostname) != 0)
                {
                    /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_007: [ If the mallocAndStrcpy_s fails, IoTHubDeviceConfiguration_Create shall do clean up and return NULL. ]*/
                    LogError("mallocAndStrcpy_s failed for hostName");
                    free_deviceConfiguration_handle(result);
                    result = NULL;
                }
                /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_012: [ IoTHubDeviceConfiguration_Create shall allocate memory and copy sharedAccessKey to result->sharedAccessKey by calling mallocAndStrcpy_s. ]*/
                else if (mallocAndStrcpy_s(&result->sharedAccessKey, serviceClientAuth->sharedAccessKey) != 0)
                {
                    /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_013: [ If the mallocAndStrcpy_s fails, IoTHubDeviceConfiguration_Create shall do clean up and return NULL. ]*/
                    LogError("mallocAndStrcpy_s failed for sharedAccessKey");
                    free_deviceConfiguration_handle(result);
                    result = NULL;
                }
                /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_014: [ IoTHubDeviceConfiguration_Create shall allocate memory and copy keyName to result->keyName by calling mallocAndStrcpy_s. ]*/
                else if (mallocAndStrcpy_s(&result->keyName, serviceClientAuth->keyName) != 0)
                {
                    /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_015: [ If the mallocAndStrcpy_s fails, IoTHubDeviceConfiguration_Create shall do clean up and return NULL. ]*/
                    LogError("mallocAndStrcpy_s failed for keyName");
                    free_deviceConfiguration_handle(result);
                    result = NULL;
                }
            }
        }
    }

    return (IOTHUB_SERVICE_CLIENT_DEVICE_CONFIGURATION_HANDLE)result;
}

void IoTHubDeviceConfiguration_Destroy(IOTHUB_SERVICE_CLIENT_DEVICE_CONFIGURATION_HANDLE serviceClientDeviceConfigurationHandle)
{
    /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_016: [ If the serviceClientDeviceConfigurationHandle input parameter is NULL IoTHubDeviceConfiguration_Destroy shall return ]*/
    if (serviceClientDeviceConfigurationHandle != NULL)
    {
        /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_17: [ If the serviceClientDeviceConfigurationHandle input parameter is not NULL IoTHubDeviceConfiguration_Destroy shall free the memory of it and return ]*/
        free_deviceConfiguration_handle((IOTHUB_SERVICE_CLIENT_DEVICE_CONFIGURATION*)serviceClientDeviceConfigurationHandle);
    }
}

static IOTHUB_DEVICE_CONFIGURATION_RESULT cloneNameValueArrays_labels(IOTHUB_DEVICE_CONFIGURATION_LABELS *source, IOTHUB_DEVICE_CONFIGURATION_LABELS *target)
{
    IOTHUB_DEVICE_CONFIGURATION_RESULT result = IOTHUB_DEVICE_CONFIGURATION_OK;

    target->numLabels = source->numLabels;
    if (target->numLabels > 0)
    {
        if ((target->labelNames = malloc(sizeof(const char*) * target->numLabels)) == NULL)
        {
            LogError("Malloc failed for IOTHUB_DEVICE_CONFIGURATION_METRICS_DEFINITION labelNames");
            result = IOTHUB_DEVICE_CONFIGURATION_OUT_OF_MEMORY_ERROR;
        }
        else if ((target->labelValues = malloc(sizeof(const char*) * target->numLabels)) == NULL)
        {
            LogError("Malloc failed for IOTHUB_DEVICE_CONFIGURATION_METRICS_DEFINITION labelValues");
            result = IOTHUB_DEVICE_CONFIGURATION_OUT_OF_MEMORY_ERROR;
        }
        else
        {
            for (size_t i = 0; i < target->numLabels; i++)
            {
                if (mallocAndStrcpy_s((char**)&(target->labelNames[i]), source->labelNames[i]) != 0)
                {
                    LogError("mallocAndStrcpy_s failed for IOTHUB_DEVICE_CONFIGURATION_METRICS_DEFINITION labelNames");
                    result = IOTHUB_DEVICE_CONFIGURATION_ERROR;
                }
                else if (mallocAndStrcpy_s((char**)&(target->labelValues[i]), source->labelValues[i]) != 0)
                {
                    LogError("mallocAndStrcpy_s failed for IOTHUB_DEVICE_CONFIGURATION_METRICS_DEFINITION labelValues");
                    result = IOTHUB_DEVICE_CONFIGURATION_ERROR;
                }
            }
        }
    }

    return result;
}

static IOTHUB_DEVICE_CONFIGURATION_RESULT cloneNameValueArrays_definitions(IOTHUB_DEVICE_CONFIGURATION_METRICS_DEFINITION *source, IOTHUB_DEVICE_CONFIGURATION_METRICS_DEFINITION *target)
{
    IOTHUB_DEVICE_CONFIGURATION_RESULT result = IOTHUB_DEVICE_CONFIGURATION_OK;

    target->numQueries = source->numQueries;
    if (target->numQueries > 0)
    {
        if ((target->queryNames = malloc(sizeof(const char*) * target->numQueries)) == NULL)
        {
            LogError("Malloc failed for IOTHUB_DEVICE_CONFIGURATION_METRICS_DEFINITION queryNames");
            result = IOTHUB_DEVICE_CONFIGURATION_OUT_OF_MEMORY_ERROR;
        }
        else if ((target->queryStrings = malloc(sizeof(const char*) * target->numQueries)) == NULL)
        {
            LogError("Malloc failed for IOTHUB_DEVICE_CONFIGURATION_METRICS_DEFINITION queryStrings");
            result = IOTHUB_DEVICE_CONFIGURATION_OUT_OF_MEMORY_ERROR;
        }
        else
        {
            for (size_t i = 0; i < target->numQueries; i++)
            {
                if (mallocAndStrcpy_s((char**)&(target->queryNames[i]), source->queryNames[i]) != 0)
                {
                    LogError("mallocAndStrcpy_s failed for IOTHUB_DEVICE_CONFIGURATION_METRICS_DEFINITION queryNames");
                    result = IOTHUB_DEVICE_CONFIGURATION_ERROR;
                }
                else if (mallocAndStrcpy_s((char**)&(target->queryStrings[i]), source->queryStrings[i]) != 0)
                {
                    LogError("mallocAndStrcpy_s failed for IOTHUB_DEVICE_CONFIGURATION_METRICS_DEFINITION queryStrings");
                    result = IOTHUB_DEVICE_CONFIGURATION_ERROR;
                }
            }
        }
    }

    return result;
}

static IOTHUB_DEVICE_CONFIGURATION_RESULT cloneNameValueArrays_results(IOTHUB_DEVICE_CONFIGURATION_METRICS_RESULT *source, IOTHUB_DEVICE_CONFIGURATION_METRICS_RESULT *target)
{
    IOTHUB_DEVICE_CONFIGURATION_RESULT result = IOTHUB_DEVICE_CONFIGURATION_OK;

    target->numQueries = source->numQueries;
    if (target->numQueries > 0)
    {
        if ((target->queryNames = malloc(sizeof(const char*) * target->numQueries)) == NULL)
        {
            LogError("Malloc failed for IOTHUB_DEVICE_CONFIGURATION_METRICS_RESULT queryNames");
            result = IOTHUB_DEVICE_CONFIGURATION_OUT_OF_MEMORY_ERROR;
        }
        else if ((target->results = malloc(sizeof(double) * target->numQueries)) == NULL)
        {
            LogError("Malloc failed for IOTHUB_DEVICE_CONFIGURATION_METRICS_RESULT results");
            result = IOTHUB_DEVICE_CONFIGURATION_OUT_OF_MEMORY_ERROR;
        }
        else
        {
            for (size_t i = 0; i < target->numQueries; i++)
            {
                if (mallocAndStrcpy_s((char**)&(target->queryNames[i]), source->queryNames[i]) != 0)
                {
                    LogError("mallocAndStrcpy_s failed for IOTHUB_DEVICE_CONFIGURATION_METRICS_RESULT queryNames");
                    result = IOTHUB_DEVICE_CONFIGURATION_ERROR;
                }
                else
                {
                    target->results[i] = source->results[i];
                }
            }
        }
    }

    return result;
}

static IOTHUB_DEVICE_CONFIGURATION_RESULT clone_deviceConfiguration(IOTHUB_DEVICE_CONFIGURATION* source, IOTHUB_DEVICE_CONFIGURATION* target)
{
    IOTHUB_DEVICE_CONFIGURATION_RESULT result = IOTHUB_DEVICE_CONFIGURATION_OK;

    if ((source != NULL) && (target != NULL))
    {
        target->version = source->version;
        target->priority = source->priority;
        target->content.deviceContent = NULL;
        target->content.modulesContent = NULL;

        if (source->configurationId != NULL && mallocAndStrcpy_s((char**)&(target->configurationId), source->configurationId) != 0)
        {
            LogError("mallocAndStrcpy_s failed for IOTHUB_DEVICE_CONFIGURATION configurationId");
            result = IOTHUB_DEVICE_CONFIGURATION_ERROR;
        }
        else if (source->schemaVersion != NULL && mallocAndStrcpy_s((char**)&(target->schemaVersion), source->schemaVersion) != 0)
        {
            LogError("mallocAndStrcpy_s failed for IOTHUB_DEVICE_CONFIGURATION schemaVersion");
            result = IOTHUB_DEVICE_CONFIGURATION_ERROR;
        }
        else if (source->targetCondition != NULL && mallocAndStrcpy_s((char**)&(target->targetCondition), source->targetCondition) != 0)
        {
            LogError("mallocAndStrcpy_s failed for IOTHUB_DEVICE_CONFIGURATION targetCondition");
            result = IOTHUB_DEVICE_CONFIGURATION_ERROR;
        }
        else if (source->eTag != NULL && mallocAndStrcpy_s((char**)&(target->eTag), source->eTag) != 0)
        {
            LogError("mallocAndStrcpy_s failed for IOTHUB_DEVICE_CONFIGURATION eTag");
            result = IOTHUB_DEVICE_CONFIGURATION_ERROR;
        }
        else if (source->createdTimeUtc != NULL && mallocAndStrcpy_s((char**)&(target->createdTimeUtc), source->createdTimeUtc) != 0)
        {
            LogError("mallocAndStrcpy_s failed for IOTHUB_DEVICE_CONFIGURATION createdTimeUtc");
            result = IOTHUB_DEVICE_CONFIGURATION_ERROR;
        }
        else if (source->lastUpdatedTimeUtc != NULL && mallocAndStrcpy_s((char**)&(target->lastUpdatedTimeUtc), source->lastUpdatedTimeUtc) != 0)
        {
            LogError("mallocAndStrcpy_s failed for IOTHUB_DEVICE_CONFIGURATION lastUpdatedTimeUtc");
            result = IOTHUB_DEVICE_CONFIGURATION_ERROR;
        }
        else if (source->content.deviceContent != NULL && mallocAndStrcpy_s((char**)&(target->content.deviceContent), source->content.deviceContent) != 0)
        {
            LogError("mallocAndStrcpy_s failed for IOTHUB_DEVICE_CONFIGURATION_CONTENT deviceContent");
            result = IOTHUB_DEVICE_CONFIGURATION_ERROR;
        }
        else if (source->content.modulesContent != NULL && mallocAndStrcpy_s((char**)&(target->content.modulesContent), source->content.modulesContent) != 0)
        {
            LogError("mallocAndStrcpy_s failed for IOTHUB_DEVICE_CONFIGURATION_CONTENT modulesContent");
            result = IOTHUB_DEVICE_CONFIGURATION_ERROR;
        }
        else
        {
            if (cloneNameValueArrays_results(&(source->metricResult), &(target->metricResult)) != IOTHUB_DEVICE_CONFIGURATION_OK || cloneNameValueArrays_results(&(source->systemMetricsResult), &(target->systemMetricsResult)) != IOTHUB_DEVICE_CONFIGURATION_OK)
            {
                LogError("Cloning failed for IOTHUB_DEVICE_CONFIGURATION_METRICS_RESULT structures");
                result = IOTHUB_DEVICE_CONFIGURATION_ERROR;
            }

            if (cloneNameValueArrays_definitions(&(source->metricsDefinition), &(target->metricsDefinition)) != IOTHUB_DEVICE_CONFIGURATION_OK || cloneNameValueArrays_definitions(&(source->systemMetricsDefinition), &(target->systemMetricsDefinition)) != IOTHUB_DEVICE_CONFIGURATION_OK)
            {
                LogError("Cloning failed for IOTHUB_DEVICE_CONFIGURATION_METRICS_DEFINITION structures");
                result = IOTHUB_DEVICE_CONFIGURATION_ERROR;
            }

            if (cloneNameValueArrays_labels(&(source->labels), &(target->labels)) != IOTHUB_DEVICE_CONFIGURATION_OK)
            {
                LogError("Cloning failed for IOTHUB_DEVICE_CONFIGURATION_LABELS structures");
                result = IOTHUB_DEVICE_CONFIGURATION_ERROR;
            }
        }
    }
    return result;
}

static IOTHUB_DEVICE_CONFIGURATION_RESULT addDeviceConfigurationToLinkedList(IOTHUB_DEVICE_CONFIGURATION* iothubDeviceConfiguration, SINGLYLINKEDLIST_HANDLE deviceConfigurationList)
{
    IOTHUB_DEVICE_CONFIGURATION_RESULT result;
    IOTHUB_DEVICE_CONFIGURATION* deviceConfiguration = NULL;

    if ((deviceConfiguration = (IOTHUB_DEVICE_CONFIGURATION*)calloc(1, sizeof(IOTHUB_DEVICE_CONFIGURATION))) == NULL)
    {
        LogError("Malloc failed for deviceConfiguration");
        result = IOTHUB_DEVICE_CONFIGURATION_OUT_OF_MEMORY_ERROR;
    }
    else
    {
        if (clone_deviceConfiguration(iothubDeviceConfiguration, deviceConfiguration) != IOTHUB_DEVICE_CONFIGURATION_OK)
        {
            LogError("clone_deviceConfiguration failed for deviceConfiguration");
            result = IOTHUB_DEVICE_CONFIGURATION_ERROR;
        }
        else if ((singlylinkedlist_add(deviceConfigurationList, deviceConfiguration)) == NULL)
        {
            LogError("singlylinkedlist_add deviceConfiguration failed");
            result = IOTHUB_DEVICE_CONFIGURATION_ERROR;
        }
        else
        {
            result = IOTHUB_DEVICE_CONFIGURATION_OK;
        }

        if (result != IOTHUB_DEVICE_CONFIGURATION_OK)
        {
            IoTHubDeviceConfiguration_FreeConfigurationMembers(deviceConfiguration);
            free(deviceConfiguration);
        }
    }

    return result;
}

static void initializeDeviceConfigurationMembers(IOTHUB_DEVICE_CONFIGURATION * configuration)
{
    if (NULL != configuration)
    {
        memset(configuration, 0, sizeof(IOTHUB_DEVICE_CONFIGURATION));
    }
}

static IOTHUB_DEVICE_CONFIGURATION_RESULT parseDeviceConfigurationListJson(BUFFER_HANDLE jsonBuffer, SINGLYLINKEDLIST_HANDLE configurationList)
{
    IOTHUB_DEVICE_CONFIGURATION_RESULT result;

    const char* bufferStr = NULL;
    JSON_Value* root_value = NULL;
    JSON_Array* device_configuration_array = NULL;
    JSON_Status jsonStatus = JSONFailure;

    if (jsonBuffer == NULL)
    {
        LogError("jsonBuffer cannot be NULL");
        result = IOTHUB_DEVICE_CONFIGURATION_ERROR;
    }
    else if (configurationList == NULL)
    {
        LogError("configurationList cannot be NULL");
        result = IOTHUB_DEVICE_CONFIGURATION_ERROR;
    }
    else
    {
        if ((bufferStr = (const char*)BUFFER_u_char(jsonBuffer)) == NULL)
        {
            /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_072: [** If populating the deviceList parameter fails IoTHubDeviceConfiguration_GetConfigurations shall return IOTHUB_DEVICE_CONFIGURATION_ERROR **] */
            LogError("BUFFER_u_char failed");
            result = IOTHUB_DEVICE_CONFIGURATION_ERROR;
        }
        else if ((root_value = json_parse_string(bufferStr)) == NULL)
        {
            /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_070: [** If any of the parson API fails, IoTHubDeviceConfiguration_GetConfigurations shall return IOTHUB_DEVICE_CONFIGURATION_JSON_ERROR **] */
            LogError("json_parse_string failed");
            result = IOTHUB_DEVICE_CONFIGURATION_JSON_ERROR;
        }
        else if ((device_configuration_array = json_value_get_array(root_value)) == NULL)
        {
            /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_070: [** If any of the parson API fails, IoTHubDeviceConfiguration_GetConfigurations shall return IOTHUB_DEVICE_CONFIGURATION_JSON_ERROR **] */
            LogError("json_value_get_object failed");
            result = IOTHUB_DEVICE_CONFIGURATION_JSON_ERROR;
        }
        else
        {
            result = IOTHUB_DEVICE_CONFIGURATION_OK;

            size_t array_count = json_array_get_count(device_configuration_array);
            for (size_t i = 0; i < array_count; i++)
            {
                JSON_Object* device_configuration_object = NULL;
                IOTHUB_DEVICE_CONFIGURATION iotHubDeviceConfiguration;

                if ((device_configuration_object = json_array_get_object(device_configuration_array, i)) == NULL)
                {
                    /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_070: [** If any of the parson API fails, IoTHubDeviceConfiguration_GetConfigurations shall return IOTHUB_DEVICE_CONFIGURATION_JSON_ERROR **] */
                    LogError("json_array_get_object failed");
                    result = IOTHUB_DEVICE_CONFIGURATION_JSON_ERROR;
                }
                else
                {
                    initializeDeviceConfigurationMembers(&iotHubDeviceConfiguration);

                    result = parseDeviceConfigurationJsonObject(device_configuration_object, &iotHubDeviceConfiguration);
                    if (result != IOTHUB_DEVICE_CONFIGURATION_OK)
                    {
                        LogError("parseDeviceConfigurationJsonObject failed");
                    }
                    else
                    {
                        result = addDeviceConfigurationToLinkedList(&iotHubDeviceConfiguration, configurationList);
                        if (result != IOTHUB_DEVICE_CONFIGURATION_OK)
                        {
                            LogError("addDeviceConfigurationToLinkedList failed");
                        }
                    }
                }

                json_object_clear(device_configuration_object);
                IoTHubDeviceConfiguration_FreeConfigurationMembers(&iotHubDeviceConfiguration);

                if (result != IOTHUB_DEVICE_CONFIGURATION_OK)
                {
                    break;
                }
            }
        }
    }
    if (device_configuration_array != NULL)
    {
        if ((jsonStatus = json_array_clear(device_configuration_array)) != JSONSuccess)
        {
            /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_070: [** If any of the parson API fails, IoTHubDeviceConfiguration_GetConfigurations shall return IOTHUB_DEVICE_CONFIGURATION_JSON_ERROR **] */
            LogError("json_array_clear failed");
            result = IOTHUB_DEVICE_CONFIGURATION_JSON_ERROR;
        }
    }

    if (root_value != NULL)
    {
        json_value_free(root_value);
    }

    if (result != IOTHUB_DEVICE_CONFIGURATION_OK)
    {
        if (configurationList != NULL)
        {
            LIST_ITEM_HANDLE itemHandle = singlylinkedlist_get_head_item(configurationList);
            while (itemHandle != NULL)
            {
                IOTHUB_DEVICE_CONFIGURATION* curr_item = (IOTHUB_DEVICE_CONFIGURATION *)singlylinkedlist_item_get_value(itemHandle);
                IoTHubDeviceConfiguration_FreeConfigurationMembers(curr_item);
                free(curr_item);

                LIST_ITEM_HANDLE lastHandle = itemHandle;
                itemHandle = singlylinkedlist_get_next_item(itemHandle);
                singlylinkedlist_remove(configurationList, lastHandle);
            }
        }
    }
    return result;
}

IOTHUB_DEVICE_CONFIGURATION_RESULT IoTHubDeviceConfiguration_GetConfigurations(IOTHUB_SERVICE_CLIENT_DEVICE_CONFIGURATION_HANDLE serviceClientDeviceConfigurationHandle, size_t maxConfigurationsCount, SINGLYLINKEDLIST_HANDLE configurations)
{
    IOTHUB_DEVICE_CONFIGURATION_RESULT result;

    /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_060: [ IOTHUB_DEVICE_CONFIGURATION_RESULT IoTHubDeviceConfiguration_GetConfigurations(IOTHUB_SERVICE_CLIENT_DEVICE_CONFIGURATION_HANDLE serviceClientDeviceConfigurationHandle, const int maxConfigurationsCount, SINGLYLINKEDLIST_HANDLE configurations) shall verify the input parameters and if any of them are NULL then return IOTHUB_DEVICE_CONFIGURATION_INVALID_ARG ] */
    if ((serviceClientDeviceConfigurationHandle == NULL) || (configurations == NULL))
    {
        LogError("Input parameter cannot be NULL");
        result = IOTHUB_DEVICE_CONFIGURATION_INVALID_ARG;
    }
    /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_061: [ IOTHUB_DEVICE_CONFIGURATION_RESULT IoTHubDeviceConfiguration_GetConfigurations(IOTHUB_SERVICE_CLIENT_DEVICE_CONFIGURATION_HANDLE serviceClientDeviceConfigurationHandle, const int maxConfigurationsCount, SINGLYLINKEDLIST_HANDLE configurations) shall verify if the numberOfDevices input parameter is between 1 and 20 and if it is not then return IOTHUB_DEVICE_CONFIGURATION_INVALID_ARG ] */
    else if ((maxConfigurationsCount == 0) || (maxConfigurationsCount > IOTHUB_DEVICE_CONFIGURATIONS_MAX_REQUEST))
    {
        LogError("numberOfDevices has to be between 1 and %d", IOTHUB_DEVICE_CONFIGURATIONS_MAX_REQUEST);
        result = IOTHUB_DEVICE_CONFIGURATION_INVALID_ARG;
    }
    else
    {
        BUFFER_HANDLE responseBuffer = NULL;

        /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_109: [ IOTHUB_DEVICE_CONFIGURATION_RESULT IoTHubDeviceConfiguration_GetConfigurations(IOTHUB_SERVICE_CLIENT_DEVICE_CONFIGURATION_HANDLE serviceClientDeviceConfigurationHandle, const int maxConfigurationsCount, SINGLYLINKEDLIST_HANDLE configurations) shall allocate memory for response buffer by calling BUFFER_new ] */
        if ((responseBuffer = BUFFER_new()) == NULL)
        {
            /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_110: [ If the BUFFER_new fails, IOTHUB_DEVICE_CONFIGURATION_RESULT IoTHubDeviceConfiguration_GetConfigurations(IOTHUB_SERVICE_CLIENT_DEVICE_CONFIGURATION_HANDLE serviceClientDeviceConfigurationHandle, const int maxConfigurationsCount, SINGLYLINKEDLIST_HANDLE configurations) shall do clean up and return NULL ] */
            LogError("BUFFER_new failed for responseBuffer");
            result = IOTHUB_DEVICE_CONFIGURATION_ERROR;
        }
        /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_062: [ IOTHUB_DEVICE_CONFIGURATION_RESULT IoTHubDeviceConfiguration_GetConfigurations(IOTHUB_SERVICE_CLIENT_DEVICE_CONFIGURATION_HANDLE serviceClientDeviceConfigurationHandle, const int maxConfigurationsCount, SINGLYLINKEDLIST_HANDLE configurations) shall create HTTP GET request for numberOfDevices using the follwoing format: url/devices/?top=[numberOfDevices]&api-version ] */
        /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_063: [ IOTHUB_DEVICE_CONFIGURATION_RESULT IoTHubDeviceConfiguration_GetConfigurations(IOTHUB_SERVICE_CLIENT_DEVICE_CONFIGURATION_HANDLE serviceClientDeviceConfigurationHandle, const int maxConfigurationsCount, SINGLYLINKEDLIST_HANDLE configurations) shall add the following headers to the created HTTP GET request: authorization=sasToken,Request-Id=<generatedGuid>,Accept=application/json,Content-Type=application/json,charset=utf-8 ] */
        /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_064: [ IOTHUB_DEVICE_CONFIGURATION_RESULT IoTHubDeviceConfiguration_GetConfigurations(IOTHUB_SERVICE_CLIENT_DEVICE_CONFIGURATION_HANDLE serviceClientDeviceConfigurationHandle, const int maxConfigurationsCount, SINGLYLINKEDLIST_HANDLE configurations) shall create an HTTPAPIEX_SAS_HANDLE handle by calling HTTPAPIEX_SAS_Create ] */
        /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_065: [ IOTHUB_DEVICE_CONFIGURATION_RESULT IoTHubDeviceConfiguration_GetConfigurations(IOTHUB_SERVICE_CLIENT_DEVICE_CONFIGURATION_HANDLE serviceClientDeviceConfigurationHandle, const int maxConfigurationsCount, SINGLYLINKEDLIST_HANDLE configurations) shall create an HTTPAPIEX_HANDLE handle by calling HTTPAPIEX_Create ] */
        /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_066: [ IOTHUB_DEVICE_CONFIGURATION_RESULT IoTHubDeviceConfiguration_GetConfigurations(IOTHUB_SERVICE_CLIENT_DEVICE_CONFIGURATION_HANDLE serviceClientDeviceConfigurationHandle, const int maxConfigurationsCount, SINGLYLINKEDLIST_HANDLE configurations) shall execute the HTTP GET request by calling HTTPAPIEX_ExecuteRequest ] */
        /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_067: [ IOTHUB_DEVICE_CONFIGURATION_RESULT IoTHubDeviceConfiguration_GetConfigurations(IOTHUB_SERVICE_CLIENT_DEVICE_CONFIGURATION_HANDLE serviceClientDeviceConfigurationHandle, const int maxConfigurationsCount, SINGLYLINKEDLIST_HANDLE configurations) shall verify the received HTTP status code and if it is greater than 300 then return IOTHUB_DEVICE_CONFIGURATION_ERROR ] */
        /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_068: [ IOTHUB_DEVICE_CONFIGURATION_RESULT IoTHubDeviceConfiguration_GetConfigurations(IOTHUB_SERVICE_CLIENT_DEVICE_CONFIGURATION_HANDLE serviceClientDeviceConfigurationHandle, const int maxConfigurationsCount, SINGLYLINKEDLIST_HANDLE configurations) shall verify the received HTTP status code and if it is less or equal than 300 then try to parse the response JSON to deviceList ] */
        else if ((result = sendHttpRequestDeviceConfiguration(serviceClientDeviceConfigurationHandle, IOTHUB_DEVICECONFIGURATION_REQUEST_GET_LIST, NULL, NULL, maxConfigurationsCount, responseBuffer)) == IOTHUB_DEVICE_CONFIGURATION_ERROR)
        {
            /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_115: [ If any of the HTTPAPI call fails IOTHUB_DEVICE_CONFIGURATION_RESULT IoTHubDeviceConfiguration_GetConfigurations(IOTHUB_SERVICE_CLIENT_DEVICE_CONFIGURATION_HANDLE serviceClientDeviceConfigurationHandle, const int maxConfigurationsCount, SINGLYLINKEDLIST_HANDLE configurations) shall fail and return IOTHUB_DEVICE_CONFIGURATION_ERROR ] */
            LogError("Failure sending HTTP request for get configuration list");
        }
        else if (result == IOTHUB_DEVICE_CONFIGURATION_OK)
        {
            /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_069: [ IOTHUB_DEVICE_CONFIGURATION_RESULT IoTHubDeviceConfiguration_GetConfigurations(IOTHUB_SERVICE_CLIENT_DEVICE_CONFIGURATION_HANDLE serviceClientDeviceConfigurationHandle, const int maxConfigurationsCount, SINGLYLINKEDLIST_HANDLE configurations) shall use the parson APIs to parse the response JSON ] */
            /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_070: [ If any of the parson API fails, IOTHUB_DEVICE_CONFIGURATION_RESULT IoTHubDeviceConfiguration_GetConfigurations(IOTHUB_SERVICE_CLIENT_DEVICE_CONFIGURATION_HANDLE serviceClientDeviceConfigurationHandle, const int maxConfigurationsCount, SINGLYLINKEDLIST_HANDLE configurations) shall return IOTHUB_DEVICE_CONFIGURATION_JSON_ERROR ] */
            /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_071: [ IOTHUB_DEVICE_CONFIGURATION_RESULT IoTHubDeviceConfiguration_GetConfigurations(IOTHUB_SERVICE_CLIENT_DEVICE_CONFIGURATION_HANDLE serviceClientDeviceConfigurationHandle, const int maxConfigurationsCount, SINGLYLINKEDLIST_HANDLE configurations) shall populate the deviceList parameter with structures of type "IOTHUB_DEVICE_CONFIGURATION" ] */
            /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_072: [ If populating the deviceList parameter fails IOTHUB_DEVICE_CONFIGURATION_RESULT IoTHubDeviceConfiguration_GetConfigurations(IOTHUB_SERVICE_CLIENT_DEVICE_CONFIGURATION_HANDLE serviceClientDeviceConfigurationHandle, const int maxConfigurationsCount, SINGLYLINKEDLIST_HANDLE configurations) shall return IOTHUB_DEVICE_CONFIGURATION_ERROR ] */
            /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_073: [ If populating the deviceList parameter successful IOTHUB_DEVICE_CONFIGURATION_RESULT IoTHubDeviceConfiguration_GetConfigurations(IOTHUB_SERVICE_CLIENT_DEVICE_CONFIGURATION_HANDLE serviceClientDeviceConfigurationHandle, const int maxConfigurationsCount, SINGLYLINKEDLIST_HANDLE configurations) shall return IOTHUB_DEVICE_CONFIGURATION_OK ] */
            result = parseDeviceConfigurationListJson(responseBuffer, configurations);
        }

        /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_111: [ IOTHUB_DEVICE_CONFIGURATION_RESULT IoTHubDeviceConfiguration_GetConfigurations(IOTHUB_SERVICE_CLIENT_DEVICE_CONFIGURATION_HANDLE serviceClientDeviceConfigurationHandle, const int maxConfigurationsCount, SINGLYLINKEDLIST_HANDLE configurations) shall do clean up before return ] */
        if (responseBuffer != NULL)
        {
            BUFFER_delete(responseBuffer);
        }
    }
    return result;
}

IOTHUB_DEVICE_CONFIGURATION_RESULT IoTHubDeviceConfiguration_GetConfiguration(IOTHUB_SERVICE_CLIENT_DEVICE_CONFIGURATION_HANDLE serviceClientDeviceConfigurationHandle, const char* configurationId, IOTHUB_DEVICE_CONFIGURATION* configuration)
{
    IOTHUB_DEVICE_CONFIGURATION_RESULT result;

    /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_18: [ IoTHubDeviceConfiguration_GetConfiguration shall verify the input parameters and if any of them are NULL then return IOTHUB_DEVICE_CONFIGURATION_INVALID_ARG ]*/
    if ((serviceClientDeviceConfigurationHandle == NULL) || (configurationId == NULL))
    {
        LogError("Input parameter cannot be NULL");
        result = IOTHUB_DEVICE_CONFIGURATION_INVALID_ARG;
    }
    else
    {
        initializeDeviceConfigurationMembers(configuration);

        BUFFER_HANDLE responseBuffer;

        if ((responseBuffer = BUFFER_new()) == NULL)
        {
            LogError("BUFFER_new failed for responseBuffer");
            result = IOTHUB_DEVICE_CONFIGURATION_ERROR;
        }
        /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_026: [ IoTHubDeviceConfiguration_GetConfiguration shall create HTTP GET request URL using the given configurationId using the following format: url/devices/[configurationId]?[apiVersion]  ] */
        /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_027: [ IoTHubDeviceConfiguration_GetConfiguration shall add the following headers to the created HTTP GET request: authorization=sasToken,Request-Id=<generatedGuid>,Accept=application/json,Content-Type=application/json,charset=utf-8 ] */
        /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_028: [ IoTHubDeviceConfiguration_GetConfiguration shall create an HTTPAPIEX_SAS_HANDLE handle by calling HTTPAPIEX_SAS_Create ] */
        /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_029: [ IoTHubDeviceConfiguration_GetConfiguration shall create an HTTPAPIEX_HANDLE handle by calling HTTPAPIEX_Create ] */
        /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_030: [ IoTHubDeviceConfiguration_GetConfiguration shall execute the HTTP GET request by calling HTTPAPIEX_ExecuteRequest ] */
        else if ((result = sendHttpRequestDeviceConfiguration(serviceClientDeviceConfigurationHandle, IOTHUB_DEVICECONFIGURATION_REQUEST_GET, configurationId, NULL, (size_t)0, responseBuffer)) == IOTHUB_DEVICE_CONFIGURATION_ERROR)
        {
            /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_031: [ If any of the HTTPAPI call fails IoTHubDeviceConfiguration_GetConfiguration shall fail and return IOTHUB_DEVICE_CONFIGURATION_ERROR ] */
            LogError("Failure sending HTTP request for get device configuration");
        }
        else if (result == IOTHUB_DEVICE_CONFIGURATION_OK)
        {
            /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_033: [ IoTHubDeviceConfiguration_GetConfiguration shall verify the received HTTP status code and if it is less or equal than 300 then try to parse the response JSON to configuration for the configuration properties] */
            /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_034: [ If any of the property field above missing from the JSON the property value will not be populated ] */
            /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_035: [ If the JSON parsing failed, IoTHubDeviceConfiguration_GetConfiguration shall return IOTHUB_DEVICE_CONFIGURATION_JSON_ERROR ] */
            /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_036: [ If the received JSON is empty, IoTHubDeviceConfiguration_GetConfiguration shall return IOTHUB_DEVICE_CONFIGURATION_DEVICE_NOT_EXIST ] */
            /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_037: [ If the configuration out parameter if not NULL IoTHubDeviceConfiguration_GetConfiguration shall save the received configuration to the out parameter and return IOTHUB_DEVICE_CONFIGURATION_OK ] */
            result = parseDeviceConfigurationJson(responseBuffer, configuration);
        }

        BUFFER_delete(responseBuffer);
    }

    return result;
}

IOTHUB_DEVICE_CONFIGURATION_RESULT IoTHubDeviceConfiguration_AddConfiguration(IOTHUB_SERVICE_CLIENT_DEVICE_CONFIGURATION_HANDLE serviceClientDeviceConfigurationHandle, const IOTHUB_DEVICE_CONFIGURATION_ADD* configurationCreate, IOTHUB_DEVICE_CONFIGURATION* configuration)
{
    IOTHUB_DEVICE_CONFIGURATION_RESULT result;

    /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_007: [ IoTHubDeviceConfiguration_AddConfiguration shall verify the input parameters and if any of them are NULL then return IOTHUB_DEVICE_CONFIGURATION_INVALID_ARG ] */
    if ((serviceClientDeviceConfigurationHandle == NULL) || (configurationCreate == NULL) || (configuration == NULL))
    {
        LogError("Input parameter cannot be NULL");
        result = IOTHUB_DEVICE_CONFIGURATION_INVALID_ARG;
    }
    else
    {
        initializeDeviceConfigurationMembers(configuration);

        if (configurationCreate->configurationId == NULL)
        {
            LogError("configurationId cannot be NULL");
            result = IOTHUB_DEVICE_CONFIGURATION_INVALID_ARG;
        }
        else
        {
            /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_095: [ IoTHubDeviceConfiguration_AddConfiguration shall allocate memory for device info structure by calling malloc ] */
            IOTHUB_DEVICE_CONFIGURATION* tempConfigurationInfo;
            if ((tempConfigurationInfo = malloc(sizeof(IOTHUB_DEVICE_CONFIGURATION))) == NULL)
            {
                /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_096 : [ If the malloc fails, IoTHubDeviceConfiguration_AddConfiguration shall do clean up and return IOTHUB_DEVICE_CONFIGURATION_ERROR. ] */
                LogError("Malloc failed for tempconfiguration");
                result = IOTHUB_DEVICE_CONFIGURATION_OUT_OF_MEMORY_ERROR;
            }
            else
            {
                /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_011: [ IoTHubDeviceConfiguration_AddConfiguration shall set the "configurationId" value to the configurationCreate->configurationId ] */
                /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_011: [ IoTHubDeviceConfiguration_AddConfiguration shall set the "targetCondition" value to the configurationCreate->targetCondition ] */
                memset(tempConfigurationInfo, 0, sizeof(*tempConfigurationInfo));
                tempConfigurationInfo->configurationId = configurationCreate->configurationId;
                tempConfigurationInfo->targetCondition = configurationCreate->targetCondition;
                tempConfigurationInfo->content = configurationCreate->content;
                tempConfigurationInfo->labels = configurationCreate->labels;
                tempConfigurationInfo->metricsDefinition = configurationCreate->metrics;
                tempConfigurationInfo->priority = configurationCreate->priority;

                tempConfigurationInfo->schemaVersion = CONFIGURATION_DEFAULT_SCHEMA_VERSION;
                tempConfigurationInfo->eTag = CONFIGURATION_DEFAULT_ETAG;

                BUFFER_HANDLE configurationJsonBuffer = NULL;
                BUFFER_HANDLE responseBuffer = NULL;

                if (((tempConfigurationInfo->content).deviceContent == NULL || strlen((tempConfigurationInfo->content).deviceContent) == 0) &&
                    ((tempConfigurationInfo->content).modulesContent == NULL || strlen((tempConfigurationInfo->content).modulesContent) == 0))
                {
                    LogError("both deviceContent and modulesContent cannot be null");
                    result = IOTHUB_DEVICE_CONFIGURATION_ERROR;
                }
                /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_010: [ IoTHubDeviceConfiguration_AddConfiguration shall create a flat "key1:value2,key2:value2..." JSON representation from the given deviceOrModuleCreateInfo parameter using parson APIs ] */
                else if ((configurationJsonBuffer = createConfigurationPayloadJson(tempConfigurationInfo)) == NULL)
                {
                    /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_013: [ IoTHubDeviceConfiguration_AddConfiguration shall return IOTHUB_DEVICE_CONFIGURATION_JSON_ERROR if the JSON creation failed  ] */
                    LogError("Json creation failed");
                    result = IOTHUB_DEVICE_CONFIGURATION_JSON_ERROR;
                }
                /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_097: [ IoTHubDeviceConfiguration_AddConfiguration shall allocate memory for response buffer by calling BUFFER_new ] */
                else if ((responseBuffer = BUFFER_new()) == NULL)
                {
                    /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_098 : [ If the BUFFER_new fails, IoTHubDeviceConfiguration_AddConfiguration shall do clean up and return IOTHUB_DEVICE_CONFIGURATION_ERROR. ] */
                    LogError("BUFFER_new failed for responseBuffer");
                    result = IOTHUB_DEVICE_CONFIGURATION_ERROR;
                }
                /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_014: [ IoTHubDeviceConfiguration_AddConfiguration shall create an HTTP PUT request using the created JSON ] */
                /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_015: [ IoTHubDeviceConfiguration_AddConfiguration shall create an HTTP PUT request using the following HTTP headers: authorization=sasToken,Request-Id=<generatedGuid>,Accept=application/json,Content-Type=application/json,charset=utf-8 ] */
                /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_016: [ IoTHubDeviceConfiguration_AddConfiguration shall create an HTTPAPIEX_SAS_HANDLE handle by calling HTTPAPIEX_SAS_Create ] */
                /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_017: [ IoTHubDeviceConfiguration_AddConfiguration shall create an HTTPAPIEX_HANDLE handle by calling HTTPAPIEX_Create ] */
                /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_018: [ IoTHubDeviceConfiguration_AddConfiguration shall execute the HTTP PUT request by calling HTTPAPIEX_ExecuteRequest ] */
                else if ((result = sendHttpRequestDeviceConfiguration(serviceClientDeviceConfigurationHandle, IOTHUB_DEVICECONFIGURATION_REQUEST_ADD, tempConfigurationInfo->configurationId, configurationJsonBuffer, (size_t)0, responseBuffer)) == IOTHUB_DEVICE_CONFIGURATION_ERROR)
                {
                    /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_019: [ If any of the HTTPAPI call fails IoTHubDeviceConfiguration_AddConfiguration shall fail and return IOTHUB_DEVICE_CONFIGURATION_HTTPAPI_ERROR ] */
                    /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_099: [ If any of the call fails during the HTTP creation IoTHubDeviceConfiguration_AddConfiguration shall fail and return IOTHUB_DEVICE_CONFIGURATION_ERROR ] */
                    LogError("Failure sending HTTP request for create device");
                }
                else if (result == IOTHUB_DEVICE_CONFIGURATION_OK)
                {
                    /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_022: [ IoTHubDeviceConfiguration_AddConfiguration shall verify the received HTTP status code and if it is less or equal than 300 then try to parse the response JSON to configuration ] */
                    /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_023: [ If the JSON parsing failed, IoTHubDeviceConfiguration_AddConfiguration shall return IOTHUB_DEVICE_CONFIGURATION_JSON_ERROR ] */
                    /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_024: [ If the configuration out parameter is not NULL IoTHubDeviceConfiguration_AddConfiguration shall save the received configuration to the out parameter and return IOTHUB_DEVICE_CONFIGURATION_OK ] */
                    result = parseDeviceConfigurationJson(responseBuffer, configuration);
                }
                else
                {
                    /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_020: [ IoTHubDeviceConfiguration_AddConfiguration shall verify the received HTTP status code and if it is 409 then return IOTHUB_DEVICE_CONFIGURATION_DEVICE_EXIST ] */
                }

                /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_100: [ IoTHubDeviceConfiguration_AddConfiguration shall do clean up before return ] */
                if (responseBuffer != NULL)
                {
                    BUFFER_delete(responseBuffer);
                }
                if (configurationJsonBuffer != NULL)
                {
                    BUFFER_delete(configurationJsonBuffer);
                }
            }
            free(tempConfigurationInfo);
        }
    }

    return result;
}

IOTHUB_DEVICE_CONFIGURATION_RESULT IoTHubDeviceConfiguration_UpdateConfiguration(IOTHUB_SERVICE_CLIENT_DEVICE_CONFIGURATION_HANDLE serviceClientDeviceConfigurationHandle, const IOTHUB_DEVICE_CONFIGURATION* configuration)
{
    IOTHUB_DEVICE_CONFIGURATION_RESULT result;

    /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_007: [ IoTHubDeviceConfiguration_UpdateConfiguration shall verify the input parameters and if any of them are NULL then return IOTHUB_DEVICE_CONFIGURATION_INVALID_ARG ] */
    if ((serviceClientDeviceConfigurationHandle == NULL) || (configuration == NULL))
    {
        LogError("Input parameter cannot be NULL");
        result = IOTHUB_DEVICE_CONFIGURATION_INVALID_ARG;
    }
    else
    {
        if (configuration->configurationId == NULL)
        {
            LogError("configurationId cannot be NULL");
            result = IOTHUB_DEVICE_CONFIGURATION_INVALID_ARG;
        }
        else
        {
            BUFFER_HANDLE configurationJsonBuffer = NULL;
            BUFFER_HANDLE responseBuffer = NULL;

            /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_010: [ IoTHubDeviceConfiguration_UpdateConfiguration shall create a flat "key1:value2,key2:value2..." JSON representation from the given configuration parameter using the parson APIs ] */
            if ((configurationJsonBuffer = createConfigurationPayloadJson(configuration)) == NULL)
            {
                /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_013: [ IoTHubDeviceConfiguration_UpdateConfiguration shall return IOTHUB_DEVICE_CONFIGURATION_JSON_ERROR if the JSON creation failed  ] */
                LogError("Json creation failed");
                result = IOTHUB_DEVICE_CONFIGURATION_JSON_ERROR;
            }
            /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_097: [ IoTHubDeviceConfiguration_UpdateConfiguration shall allocate memory for response buffer by calling BUFFER_new ] */
            else if ((responseBuffer = BUFFER_new()) == NULL)
            {
                /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_098 : [ If the BUFFER_new fails, IoTHubDeviceConfiguration_UpdateConfiguration shall do clean up and return IOTHUB_DEVICE_CONFIGURATION_ERROR. ] */
                LogError("BUFFER_new failed for responseBuffer");
                result = IOTHUB_DEVICE_CONFIGURATION_ERROR;
            }
            /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_014: [ IoTHubDeviceConfiguration_UpdateConfiguration shall create an HTTP PUT request using the created JSON ] */
            /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_015: [ IoTHubDeviceConfiguration_UpdateConfiguration shall create an HTTP PUT request using the following HTTP headers: authorization=sasToken,Request-Id=<generatedGuid>,Accept=application/json,Content-Type=application/json,charset=utf-8 ] */
            /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_016: [ IoTHubDeviceConfiguration_UpdateConfiguration shall create an HTTPAPIEX_SAS_HANDLE handle by calling HTTPAPIEX_SAS_Create ] */
            /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_017: [ IoTHubDeviceConfiguration_UpdateConfiguration shall create an HTTPAPIEX_HANDLE handle by calling HTTPAPIEX_Create ] */
            /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_018: [ IoTHubDeviceConfiguration_UpdateConfiguration shall execute the HTTP PUT request by calling HTTPAPIEX_ExecuteRequest ] */
            else if ((result = sendHttpRequestDeviceConfiguration(serviceClientDeviceConfigurationHandle, IOTHUB_DEVICECONFIGURATION_REQUEST_UPDATE, configuration->configurationId, configurationJsonBuffer, (size_t)0, responseBuffer)) == IOTHUB_DEVICE_CONFIGURATION_ERROR)
            {
                /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_019: [ If any of the HTTPAPI call fails IoTHubDeviceConfiguration_UpdateConfiguration shall fail and return IOTHUB_DEVICE_CONFIGURATION_HTTPAPI_ERROR ] */
                /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_099: [ If any of the call fails during the HTTP creation IoTHubDeviceConfiguration_UpdateConfiguration shall fail and return IOTHUB_DEVICE_CONFIGURATION_ERROR ] */
                LogError("Failure sending HTTP request for update device configuration");
            }
            else
            {
                /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_020: [ IoTHubDeviceConfiguration_UpdateConfiguration shall verify the received HTTP status code and if it is 409 then return IOTHUB_DEVICE_CONFIGURATION_DEVICE_EXIST ] */
            }

            /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_100: [ IoTHubDeviceConfiguration_UpdateConfiguration shall do clean up before return ] */
            if (responseBuffer != NULL)
            {
                BUFFER_delete(responseBuffer);
            }
            if (configurationJsonBuffer != NULL)
            {
                BUFFER_delete(configurationJsonBuffer);
            }
        }
    }

    return result;
}

IOTHUB_DEVICE_CONFIGURATION_RESULT IoTHubDeviceConfiguration_DeleteConfiguration(IOTHUB_SERVICE_CLIENT_DEVICE_CONFIGURATION_HANDLE serviceClientDeviceConfigurationHandle, const char* configurationId)
{
    IOTHUB_DEVICE_CONFIGURATION_RESULT result;

    /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_052: [ IoTHubDeviceConfiguration_DeleteConfiguration shall verify the input parameters and if any of them are NULL then return IOTHUB_DEVICE_CONFIGURATION_INVALID_ARG ]*/
    if ((serviceClientDeviceConfigurationHandle == NULL) || (configurationId == NULL))
    {
        LogError("Input parameter cannot be NULL");
        result = IOTHUB_DEVICE_CONFIGURATION_INVALID_ARG;
    }
    else
    {
        /*SRS_IOTHUBDEVICECONFIGURATION_38_053: [ IoTHubDeviceConfiguration_DeleteConfiguration shall create HTTP DELETE request URL using the given configurationId using the following format : url/configurations/[configurationId]?api-version  ] */
        /*SRS_IOTHUBDEVICECONFIGURATION_38_054: [ IoTHubDeviceConfiguration_DeleteConfiguration shall add the following headers to the created HTTP GET request : authorization=sasToken,Request-Id=<generatedGuid>,Accept=application/json,Content-Type=application/json,charset=utf-8 ] */
        /*SRS_IOTHUBDEVICECONFIGURATION_38_055: [ IoTHubDeviceConfiguration_DeleteConfiguration shall create an HTTPAPIEX_SAS_HANDLE handle by calling HTTPAPIEX_SAS_Create ] */
        /*SRS_IOTHUBDEVICECONFIGURATION_38_056: [ IoTHubDeviceConfiguration_DeleteConfiguration shall create an HTTPAPIEX_HANDLE handle by calling HTTPAPIEX_Create ] */
        /*SRS_IOTHUBDEVICECONFIGURATION_38_057: [ IoTHubDeviceConfiguration_DeleteConfiguration shall execute the HTTP DELETE request by calling HTTPAPIEX_ExecuteRequest ] */
        /*SRS_IOTHUBDEVICECONFIGURATION_38_058: [ IoTHubDeviceConfiguration_DeleteConfiguration shall verify the received HTTP status code and if it is greater than 300 then return IOTHUB_DEVICE_CONFIGURATION_HTTP_STATUS_ERROR ] */
        /*SRS_IOTHUBDEVICECONFIGURATION_38_059: [ IoTHubDeviceConfiguration_DeleteConfiguration shall verify the received HTTP status code and if it is less or equal than 300 then return IOTHUB_DEVICE_CONFIGURATION_OK ] */
        result = sendHttpRequestDeviceConfiguration(serviceClientDeviceConfigurationHandle, IOTHUB_DEVICECONFIGURATION_REQUEST_DELETE, configurationId, NULL, (size_t)0, NULL);
    }

    return result;
}

IOTHUB_DEVICE_CONFIGURATION_RESULT IoTHubDeviceConfiguration_ApplyConfigurationContentToDeviceOrModule(IOTHUB_SERVICE_CLIENT_DEVICE_CONFIGURATION_HANDLE serviceClientDeviceConfigurationHandle, const char* deviceOrModuleId, const IOTHUB_DEVICE_CONFIGURATION_CONTENT* configurationContent)
{
    IOTHUB_DEVICE_CONFIGURATION_RESULT result;

    /*Codes_SRS_IOTHUBDEVICECONFIGURATION_38_052: [ IoTHubDeviceConfiguration_ApplyConfigurationContentToDeviceOrModule shall verify the input parameters and if any of them are NULL then return IOTHUB_DEVICE_CONFIGURATION_INVALID_ARG ]*/
    if ((serviceClientDeviceConfigurationHandle == NULL) || (deviceOrModuleId == NULL) || (configurationContent == NULL))
    {
        LogError("Input parameter cannot be NULL");
        result = IOTHUB_DEVICE_CONFIGURATION_INVALID_ARG;
    }
    else
    {
        BUFFER_HANDLE configurationJsonBuffer;

        JSON_Value* root_value = NULL;
        JSON_Object* root_object = NULL;
        JSON_Value* configurationContentJson;

        if ((configurationContent->deviceContent == NULL) && ((configurationContent->modulesContent) == NULL))
        {
            LogError("deviceContent and modulesContent both cannot be NULL");
            configurationJsonBuffer = NULL;
        }
        else if ((root_value = json_value_init_object()) == NULL)
        {
            LogError("json_value_init_object failed");
            configurationJsonBuffer = NULL;
        }
        else if ((root_object = json_value_get_object(root_value)) == NULL)
        {
            LogError("json_value_get_object failed");
            configurationJsonBuffer = NULL;
        }
        else if ((((configurationContentJson = createConfigurationContentPayload(configurationContent)) != NULL) && ((json_object_set_value(root_object, CONFIGURATION_JSON_KEY_CONTENT, configurationContentJson)) != JSONSuccess)))
        {
            LogError("json_object_set_string failed for configurationContentJson");
            configurationJsonBuffer = NULL;
        }
        else
        {
            char* serialized_string;
            if ((serialized_string = json_serialize_to_string(root_value)) == NULL)
            {
                LogError("json_serialize_to_string failed");
                configurationJsonBuffer = NULL;
            }
            else
            {
                if ((configurationJsonBuffer = BUFFER_create((const unsigned char*)serialized_string, strlen(serialized_string))) == NULL)
                {
                    LogError("Buffer_Create failed");
                }
                json_free_serialized_string(serialized_string);
            }
        }

        json_object_clear(root_object);

        if (root_value != NULL)
            json_value_free(root_value);

        /*SRS_IOTHUBDEVICECONFIGURATION_38_053: [ IoTHubDeviceConfiguration_ApplyConfigurationContentToDeviceOrModule shall create HTTP POST request URL using the given deviceOrModuleId using the following format : url/devices/[deviceOrModuleId]?api-version  ] */
        /*SRS_IOTHUBDEVICECONFIGURATION_38_054: [ IoTHubDeviceConfiguration_ApplyConfigurationContentToDeviceOrModule shall add the following headers to the created HTTP POST request : authorization=sasToken,Request-Id=<generatedGuid>,Accept=application/json,Content-Type=application/json,charset=utf-8 ] */
        /*SRS_IOTHUBDEVICECONFIGURATION_38_055: [ IoTHubDeviceConfiguration_ApplyConfigurationContentToDeviceOrModule shall create an HTTPAPIEX_SAS_HANDLE handle by calling HTTPAPIEX_SAS_Create ] */
        /*SRS_IOTHUBDEVICECONFIGURATION_38_056: [ IoTHubDeviceConfiguration_ApplyConfigurationContentToDeviceOrModule shall create an HTTPAPIEX_HANDLE handle by calling HTTPAPIEX_Create ] */
        /*SRS_IOTHUBDEVICECONFIGURATION_38_057: [ IoTHubDeviceConfiguration_ApplyConfigurationContentToDeviceOrModule shall execute the HTTP POST request by calling HTTPAPIEX_ExecuteRequest ] */
        /*SRS_IOTHUBDEVICECONFIGURATION_38_058: [ IoTHubDeviceConfiguration_ApplyConfigurationContentToDeviceOrModule shall verify the received HTTP status code and if it is greater than 300 then return IOTHUB_DEVICE_CONFIGURATION_HTTP_STATUS_ERROR ] */
        /*SRS_IOTHUBDEVICECONFIGURATION_38_059: [ IoTHubDeviceConfiguration_ApplyConfigurationContentToDeviceOrModule shall verify the received HTTP status code and if it is equal to 200 or 204 then return IOTHUB_DEVICE_CONFIGURATION_OK ] */
        result = sendHttpRequestDeviceConfiguration(serviceClientDeviceConfigurationHandle, IOTHUB_DEVICECONFIGURATION_REQUEST_APPLY_CONFIGURATION_CONTENT, deviceOrModuleId, configurationJsonBuffer, (size_t)0, NULL);
    }

    return result;
}
