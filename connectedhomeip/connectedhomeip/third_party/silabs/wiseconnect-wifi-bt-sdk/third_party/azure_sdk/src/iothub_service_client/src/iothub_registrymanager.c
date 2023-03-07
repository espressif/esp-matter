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
#include "azure_c_shared_utility/connection_string_parser.h"

#include "parson.h"
#include "iothub_registrymanager.h"
#include "iothub_sc_version.h"

#define IOTHUB_DEVICE_EX_VERSION_LATEST IOTHUB_DEVICE_EX_VERSION_1
#define IOTHUB_REGISTRY_DEVICE_CREATE_EX_VERSION_LATEST IOTHUB_REGISTRY_DEVICE_CREATE_EX_VERSION_1
#define IOTHUB_REGISTRY_DEVICE_UPDATE_EX_VERSION_LATEST IOTHUB_REGISTRY_DEVICE_CREATE_EX_VERSION_1
#define IOTHUB_MODULE_VERSION_LATEST IOTHUB_MODULE_VERSION_1
#define IOTHUB_REGISTRY_MODULE_CREATE_VERSION_LATEST IOTHUB_REGISTRY_MODULE_CREATE_VERSION_1
#define IOTHUB_REGISTRY_MODULE_UPDATE_VERSION_LATEST IOTHUB_REGISTRY_MODULE_UPDATE_VERSION_1


#define IOTHUB_REQUEST_MODE_VALUES    \
    IOTHUB_REQUEST_CREATE,            \
    IOTHUB_REQUEST_GET,               \
    IOTHUB_REQUEST_UPDATE,            \
    IOTHUB_REQUEST_DELETE,            \
    IOTHUB_REQUEST_GET_DEVICE_LIST,   \
    IOTHUB_REQUEST_GET_STATISTICS     \

MU_DEFINE_ENUM(IOTHUB_REQUEST_MODE, IOTHUB_REQUEST_MODE_VALUES);

MU_DEFINE_ENUM_STRINGS_WITHOUT_INVALID(IOTHUB_REGISTRYMANAGER_RESULT, IOTHUB_REGISTRYMANAGER_RESULT_VALUES);
MU_DEFINE_ENUM_STRINGS_WITHOUT_INVALID(IOTHUB_REGISTRYMANAGER_AUTH_METHOD, IOTHUB_REGISTRYMANAGER_AUTH_METHOD_VALUES);

#define  HTTP_HEADER_KEY_AUTHORIZATION  "Authorization"
#define  HTTP_HEADER_VAL_AUTHORIZATION  " "
#define  HTTP_HEADER_KEY_REQUEST_ID  "Request-Id"
#define  HTTP_HEADER_VAL_REQUEST_ID  "1001"
#define  HTTP_HEADER_KEY_USER_AGENT  "User-Agent"
#define  HTTP_HEADER_VAL_USER_AGENT  IOTHUB_SERVICE_CLIENT_TYPE_PREFIX IOTHUB_SERVICE_CLIENT_BACKSLASH IOTHUB_SERVICE_CLIENT_VERSION
#define  HTTP_HEADER_KEY_ACCEPT  "Accept"
#define  HTTP_HEADER_VAL_ACCEPT  "application/json"
#define  HTTP_HEADER_KEY_CONTENT_TYPE  "Content-Type"
#define  HTTP_HEADER_VAL_CONTENT_TYPE  "application/json; charset=utf-8"
#define  HTTP_HEADER_KEY_IFMATCH  "If-Match"
#define  HTTP_HEADER_VAL_IFMATCH  "*"

static size_t IOTHUB_DEVICES_MAX_REQUEST = 1000;

static const char* DEVICE_JSON_KEY_DEVICE_NAME = "deviceId";
static const char* DEVICE_JSON_KEY_MODULE_NAME = "moduleId";
static const char* DEVICE_JSON_KEY_DEVICE_AUTH_TYPE = "authentication.type";
static const char* DEVICE_JSON_KEY_DEVICE_AUTH_SAS = "sas";
static const char* DEVICE_JSON_KEY_DEVICE_AUTH_SELF_SIGNED = "selfSigned";
static const char* DEVICE_JSON_KEY_DEVICE_AUTH_CERTIFICATE_AUTHORITY = "certificateAuthority";
static const char* DEVICE_JSON_KEY_DEVICE_AUTH_NONE = "none";
static const char* DEVICE_JSON_KEY_DEVICE_PRIMARY_KEY = "authentication.symmetricKey.primaryKey";
static const char* DEVICE_JSON_KEY_DEVICE_SECONDARY_KEY = "authentication.symmetricKey.secondaryKey";
static const char* DEVICE_JSON_KEY_DEVICE_PRIMARY_THUMBPRINT = "authentication.x509Thumbprint.primaryThumbprint";
static const char* DEVICE_JSON_KEY_DEVICE_SECONDARY_THUMBPRINT = "authentication.x509Thumbprint.secondaryThumbprint";
static const char* DEVICE_JSON_KEY_CAPABILITIES_IOTEDGE = "capabilities.iotEdge";

static const char* DEVICE_JSON_KEY_DEVICE_GENERATION_ID = "generationId";
static const char* DEVICE_JSON_KEY_DEVICE_ETAG = "etag";

static const char* DEVICE_JSON_KEY_DEVICE_CONNECTIONSTATE = "connectionState";
static const char* DEVICE_JSON_KEY_DEVICE_CONNECTIONSTATEUPDATEDTIME = "connectionStateUpdatedTime";

static const char* DEVICE_JSON_KEY_DEVICE_STATUS = "status";
static const char* DEVICE_JSON_KEY_DEVICE_STATUSREASON = "statusReason";
static const char* DEVICE_JSON_KEY_DEVICE_STATUSUPDATEDTIME = "statusUpdatedTime";

static const char* DEVICE_JSON_KEY_DEVICE_LASTACTIVITYTIME = "lastActivityTime";
static const char* DEVICE_JSON_KEY_DEVICE_CLOUDTODEVICEMESSAGECOUNT = "cloudToDeviceMessageCount";

static const char* DEVICE_JSON_KEY_DEVICE_ISMANAGED = "isManaged";
static const char* DEVICE_JSON_KEY_DEVICE_CONFIGURATION = "configuration";
static const char* DEVICE_JSON_KEY_DEVICE_DEVICEROPERTIES = "deviceProperties";
static const char* DEVICE_JSON_KEY_DEVICE_SERVICEPROPERTIES = "serviceProperties";
static const char* DEVICE_JSON_KEY_MANAGED_BY = "managedBy";

static const char* DEVICE_JSON_KEY_TOTAL_DEVICECOUNT = "totalDeviceCount";
static const char* DEVICE_JSON_KEY_ENABLED_DEVICECCOUNT = "enabledDeviceCount";
static const char* DEVICE_JSON_KEY_DISABLED_DEVICECOUNT = "disabledDeviceCount";

static const char* DEVICE_JSON_DEFAULT_VALUE_ENABLED = "enabled";
static const char* DEVICE_JSON_DEFAULT_VALUE_DISABLED = "disabled";
static const char* DEVICE_JSON_DEFAULT_VALUE_CONNECTED = "Connected";
static const char* DEVICE_JSON_DEFAULT_VALUE_TRUE = "true";

static const char* URL_API_VERSION = "api-version=2020-09-30";

static const char* RELATIVE_PATH_FMT_CRUD = "/devices/%s?%s";
static const char* RELATIVE_PATH_MODULE_FMT_CRUD = "/devices/%s/modules/%s?%s";
static const char* RELATIVE_PATH_FMT_LIST = "/devices/?top=%s&%s";
static const char* RELATIVE_PATH_FMT_STAT = "/statistics/devices?%s";
static const char* RELATIVE_PATH_FMT_MODULE_LIST = "/devices/%s/modules?%s";

typedef enum {IOTHUB_REGISTRYMANAGER_MODEL_TYPE_DEVICE, IOTHUB_REGISTRYMANAGER_MODEL_TYPE_MODULE} IOTHUB_REGISTRYMANAGER_MODEL_TYPE;

typedef struct IOTHUB_DEVICE_OR_MODULE_TAG
{
    IOTHUB_REGISTRYMANAGER_MODEL_TYPE type;

    const char* deviceId;
    const char* primaryKey;
    const char* secondaryKey;
    const char* generationId;
    const char* eTag;
    IOTHUB_DEVICE_CONNECTION_STATE connectionState;
    const char* connectionStateUpdatedTime;
    IOTHUB_DEVICE_STATUS status;
    const char* statusReason;
    const char* statusUpdatedTime;
    const char* lastActivityTime;
    size_t cloudToDeviceMessageCount;

    bool isManaged;
    const char* configuration;
    const char* deviceProperties;
    const char* serviceProperties;
    IOTHUB_REGISTRYMANAGER_AUTH_METHOD authMethod;

    //Device exclusive fields
    bool iotEdge_capable;

    //Module exclusive fields
    const char* moduleId;
    const char* managedBy;
} IOTHUB_DEVICE_OR_MODULE;

typedef struct IOTHUB_REGISTRY_DEVICE_OR_MODULE_CREATE_TAG
{
    IOTHUB_REGISTRYMANAGER_MODEL_TYPE type;
    const char* deviceId;
    const char* primaryKey;
    const char* secondaryKey;
    IOTHUB_REGISTRYMANAGER_AUTH_METHOD authMethod;
    //Device exclusive fields
    bool iotEdge_capable;
    //Module exclusive fields
    const char* moduleId;
    const char* managedBy;
} IOTHUB_REGISTRY_DEVICE_OR_MODULE_CREATE;

typedef struct IOTHUB_REGISTRY_DEVICE_OR_MODULE_UPDATE_TAG
{
    IOTHUB_REGISTRYMANAGER_MODEL_TYPE type;
    const char* deviceId;
    const char* primaryKey;
    const char* secondaryKey;
    IOTHUB_DEVICE_STATUS status;
    IOTHUB_REGISTRYMANAGER_AUTH_METHOD authMethod;
    //Device exclusive fields
    bool iotEdge_capable;
    //Module exclusive fields
    const char* moduleId;
    const char* managedBy;
} IOTHUB_REGISTRY_DEVICE_OR_MODULE_UPDATE;

static void initializeDeviceOrModuleInfoMembers(IOTHUB_DEVICE_OR_MODULE* deviceOrModuleInfo)
{
    if (NULL != deviceOrModuleInfo)
    {
        memset(deviceOrModuleInfo, 0, sizeof(IOTHUB_DEVICE_OR_MODULE));
        deviceOrModuleInfo->connectionState = IOTHUB_DEVICE_CONNECTION_STATE_DISCONNECTED;
        deviceOrModuleInfo->status = IOTHUB_DEVICE_STATUS_DISABLED;
    }
}

void IoTHubRegistryManager_FreeDeviceExMembers(IOTHUB_DEVICE_EX* deviceInfo)
{
    free((void*)deviceInfo->deviceId);
    free((void*)deviceInfo->primaryKey);
    free((void*)deviceInfo->secondaryKey);
    free((void*)deviceInfo->generationId);
    free((void*)deviceInfo->eTag);
    free((void*)deviceInfo->connectionStateUpdatedTime);
    free((void*)deviceInfo->statusReason);
    free((void*)deviceInfo->statusUpdatedTime);
    free((void*)deviceInfo->lastActivityTime);
    free((void*)deviceInfo->configuration);
    free((void*)deviceInfo->deviceProperties);
    free((void*)deviceInfo->serviceProperties);
    memset(deviceInfo, 0, sizeof(*deviceInfo));
}

void IoTHubRegistryManager_FreeModuleMembers(IOTHUB_MODULE* moduleInfo)
{
    free((void*)moduleInfo->deviceId);
    free((void*)moduleInfo->primaryKey);
    free((void*)moduleInfo->secondaryKey);
    free((void*)moduleInfo->generationId);
    free((void*)moduleInfo->eTag);
    free((void*)moduleInfo->connectionStateUpdatedTime);
    free((void*)moduleInfo->statusReason);
    free((void*)moduleInfo->statusUpdatedTime);
    free((void*)moduleInfo->lastActivityTime);
    free((void*)moduleInfo->configuration);
    free((void*)moduleInfo->deviceProperties);
    free((void*)moduleInfo->serviceProperties);
    free((void*)moduleInfo->moduleId);
    free((void*)moduleInfo->managedBy);
    memset(moduleInfo, 0, sizeof(*moduleInfo));
}

// Frees memory allocated building up deviceInfo, but *NOT* deviceInfo itself as we don't own this
static void free_deviceOrModule_members(IOTHUB_DEVICE_OR_MODULE* deviceInfo)
{
    free((void*)deviceInfo->deviceId);
    free((void*)deviceInfo->primaryKey);
    free((void*)deviceInfo->secondaryKey);
    free((void*)deviceInfo->generationId);
    free((void*)deviceInfo->eTag);
    free((void*)deviceInfo->connectionStateUpdatedTime);
    free((void*)deviceInfo->statusReason);
    free((void*)deviceInfo->statusUpdatedTime);
    free((void*)deviceInfo->lastActivityTime);
    free((void*)deviceInfo->configuration);
    free((void*)deviceInfo->deviceProperties);
    free((void*)deviceInfo->serviceProperties);
    free((void*)deviceInfo->moduleId);
    free((void*)deviceInfo->managedBy);
    memset(deviceInfo, 0, sizeof(*deviceInfo));
}

static void free_device_members(IOTHUB_DEVICE* deviceInfo)
{
    free((void*)deviceInfo->deviceId);
    free((void*)deviceInfo->primaryKey);
    free((void*)deviceInfo->secondaryKey);
    free((void*)deviceInfo->generationId);
    free((void*)deviceInfo->eTag);
    free((void*)deviceInfo->connectionStateUpdatedTime);
    free((void*)deviceInfo->statusReason);
    free((void*)deviceInfo->statusUpdatedTime);
    free((void*)deviceInfo->lastActivityTime);
    free((void*)deviceInfo->configuration);
    free((void*)deviceInfo->deviceProperties);
    free((void*)deviceInfo->serviceProperties);
    memset(deviceInfo, 0, sizeof(*deviceInfo));
}

static void move_deviceOrModule_members_to_device(IOTHUB_DEVICE_OR_MODULE* deviceOrModule, IOTHUB_DEVICE* device)
{
    if ((deviceOrModule != NULL) && (device != NULL))
    {
        device->deviceId = deviceOrModule->deviceId;
        device->primaryKey = deviceOrModule->primaryKey;
        device->secondaryKey = deviceOrModule->secondaryKey;
        device->generationId = deviceOrModule->generationId;
        device->eTag = deviceOrModule->eTag;
        device->connectionState = deviceOrModule->connectionState;
        device->connectionStateUpdatedTime = deviceOrModule->connectionStateUpdatedTime;
        device->status = deviceOrModule->status;
        device->statusReason = deviceOrModule->statusReason;
        device->statusUpdatedTime = deviceOrModule->statusUpdatedTime;
        device->lastActivityTime = deviceOrModule->lastActivityTime;
        device->cloudToDeviceMessageCount = deviceOrModule->cloudToDeviceMessageCount;
        device->isManaged = deviceOrModule->isManaged;
        device->configuration = deviceOrModule->configuration;
        device->deviceProperties = deviceOrModule->deviceProperties;
        device->serviceProperties = deviceOrModule->serviceProperties;
        device->authMethod = deviceOrModule->authMethod;
    }
}

static void free_nonDevice_members_from_deviceOrModule(IOTHUB_DEVICE_OR_MODULE* deviceOrModule)
{
    if (deviceOrModule != NULL)
    {
        //free module exclusive fields
        free((void*)deviceOrModule->moduleId);
        deviceOrModule->moduleId = NULL;
        free((void*)deviceOrModule->managedBy);
        deviceOrModule->managedBy = NULL;
        //free device_ex exclusive fields (none currently require deallocation)
    }
}

static void move_deviceOrModule_members_to_deviceEx(IOTHUB_DEVICE_OR_MODULE* deviceOrModule, IOTHUB_DEVICE_EX* device)
{
    if ((deviceOrModule != NULL) && (device != NULL))
    {
        if (device->version >= IOTHUB_DEVICE_EX_VERSION_1)
        {
            device->deviceId = deviceOrModule->deviceId;
            device->primaryKey = deviceOrModule->primaryKey;
            device->secondaryKey = deviceOrModule->secondaryKey;
            device->generationId = deviceOrModule->generationId;
            device->eTag = deviceOrModule->eTag;
            device->connectionState = deviceOrModule->connectionState;
            device->connectionStateUpdatedTime = deviceOrModule->connectionStateUpdatedTime;
            device->status = deviceOrModule->status;
            device->statusReason = deviceOrModule->statusReason;
            device->statusUpdatedTime = deviceOrModule->statusUpdatedTime;
            device->lastActivityTime = deviceOrModule->lastActivityTime;
            device->cloudToDeviceMessageCount = deviceOrModule->cloudToDeviceMessageCount;
            device->isManaged = deviceOrModule->isManaged;
            device->configuration = deviceOrModule->configuration;
            device->deviceProperties = deviceOrModule->deviceProperties;
            device->serviceProperties = deviceOrModule->serviceProperties;
            device->authMethod = deviceOrModule->authMethod;
            device->iotEdge_capable = deviceOrModule->iotEdge_capable;
        }
    }
}

static void free_nonDeviceEx_members_from_deviceOrModule(IOTHUB_DEVICE_OR_MODULE* deviceOrModule)
{
    if (deviceOrModule != NULL)
    {
        //free module exclusive fields
        free((void*)deviceOrModule->moduleId);
        deviceOrModule->moduleId = NULL;
        free((void*)deviceOrModule->managedBy);
        deviceOrModule->managedBy = NULL;
    }
}

static void move_deviceOrModule_members_to_module(IOTHUB_DEVICE_OR_MODULE* deviceOrModule, IOTHUB_MODULE* module)
{
    if ((deviceOrModule != NULL) && (module != NULL))
    {
        if (module->version >= IOTHUB_MODULE_VERSION_1)
        {
            module->deviceId = deviceOrModule->deviceId;
            module->primaryKey = deviceOrModule->primaryKey;
            module->secondaryKey = deviceOrModule->secondaryKey;
            module->generationId = deviceOrModule->generationId;
            module->eTag = deviceOrModule->eTag;
            module->connectionState = deviceOrModule->connectionState;
            module->connectionStateUpdatedTime = deviceOrModule->connectionStateUpdatedTime;
            module->status = deviceOrModule->status;
            module->statusReason = deviceOrModule->statusReason;
            module->statusUpdatedTime = deviceOrModule->statusUpdatedTime;
            module->lastActivityTime = deviceOrModule->lastActivityTime;
            module->cloudToDeviceMessageCount = deviceOrModule->cloudToDeviceMessageCount;
            module->isManaged = deviceOrModule->isManaged;
            module->configuration = deviceOrModule->configuration;
            module->deviceProperties = deviceOrModule->deviceProperties;
            module->serviceProperties = deviceOrModule->serviceProperties;
            module->authMethod = deviceOrModule->authMethod;
            module->moduleId = deviceOrModule->moduleId;
            module->managedBy = deviceOrModule->managedBy;
        }
    }
}

static bool isAuthTypeAllowed(IOTHUB_REGISTRYMANAGER_AUTH_METHOD authMethod)
{
    bool result;
    switch (authMethod)
    {
        case IOTHUB_REGISTRYMANAGER_AUTH_SPK:
        case IOTHUB_REGISTRYMANAGER_AUTH_X509_THUMBPRINT:
        case IOTHUB_REGISTRYMANAGER_AUTH_X509_CERTIFICATE_AUTHORITY:
        case IOTHUB_REGISTRYMANAGER_AUTH_NONE:
            result = true;
            break;

        default:
            result = false;
            break;
    }

    return result;
}

static int strHasNoWhitespace(const char* s)
{
    while (*s)
    {
        if (isspace(*s) != 0)
            return 1;
        s++;
    }
    return 0;
}

static const char *getStatusStringForJson(IOTHUB_DEVICE_STATUS status)
{
    const char *statusForJson;

    if (IOTHUB_DEVICE_STATUS_DISABLED == status)
    {
        statusForJson = DEVICE_JSON_DEFAULT_VALUE_DISABLED;
    }
    else
    {
        statusForJson = DEVICE_JSON_DEFAULT_VALUE_ENABLED;
    }

    return statusForJson;
}

static const char *getAuthTypeStringForJson(IOTHUB_REGISTRYMANAGER_AUTH_METHOD authMethod)
{
    const char *authTypeForJson;

    if (IOTHUB_REGISTRYMANAGER_AUTH_SPK == authMethod)
    {
        authTypeForJson = DEVICE_JSON_KEY_DEVICE_AUTH_SAS;
    }
    else if (IOTHUB_REGISTRYMANAGER_AUTH_X509_THUMBPRINT == authMethod)
    {
        authTypeForJson = DEVICE_JSON_KEY_DEVICE_AUTH_SELF_SIGNED;
    }
    else if (IOTHUB_REGISTRYMANAGER_AUTH_X509_CERTIFICATE_AUTHORITY == authMethod)
    {
        authTypeForJson = DEVICE_JSON_KEY_DEVICE_AUTH_CERTIFICATE_AUTHORITY;
    }
    else if (IOTHUB_REGISTRYMANAGER_AUTH_NONE == authMethod)
    {
        authTypeForJson = DEVICE_JSON_KEY_DEVICE_AUTH_NONE;
    }
    else
    {
        LogError("Cannot map <%d> to auth type for JSON string", authMethod);
        authTypeForJson = NULL;
    }

    return authTypeForJson;
}

static BUFFER_HANDLE constructDeviceOrModuleJson(const IOTHUB_DEVICE_OR_MODULE* deviceOrModuleInfo)
{
    BUFFER_HANDLE result;

    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_010: [ IoTHubRegistryManager_CreateDevice shall create a flat "key1:value2,key2:value2..." JSON representation from the given deviceCreateInfo parameter using the following parson APIs: json_value_init_object, json_value_get_object, json_object_set_string, json_object_dotset_string ] */
    JSON_Value* root_value = NULL;
    JSON_Object* root_object = NULL;
    JSON_Status jsonStatus;

    const char *authTypeForJson;

    int iotEdge_capable = (deviceOrModuleInfo->iotEdge_capable == true) ? 1 : 0;

    if (deviceOrModuleInfo == NULL)
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_013: [ IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR if the JSON creation failed  ] */
        LogError("deviceOrModuleInfo cannot be null");
        result = NULL;
    }
    else if (deviceOrModuleInfo->deviceId == NULL)
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_013: [ IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR if the JSON creation failed  ] */
        LogError("Device id cannot be NULL");
        result = NULL;
    }
    if ((root_value = json_value_init_object()) == NULL)
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_013: [ IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR if the JSON creation failed  ] */
        LogError("json_value_init_object failed");
        result = NULL;
    }
    else if ((root_object = json_value_get_object(root_value)) == NULL)
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_013: [ IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR if the JSON creation failed  ] */
        LogError("json_value_get_object failed");
        result = NULL;
    }
    else if ((json_object_set_string(root_object, DEVICE_JSON_KEY_DEVICE_NAME, deviceOrModuleInfo->deviceId)) != JSONSuccess)
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_013: [ IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR if the JSON creation failed  ] */
        LogError("json_object_set_string failed for deviceId");
        result = NULL;
    }
    else if ((deviceOrModuleInfo->moduleId != NULL) && ((json_object_set_string(root_object, DEVICE_JSON_KEY_MODULE_NAME, deviceOrModuleInfo->moduleId)) != JSONSuccess))
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_013: [ IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR if the JSON creation failed  ] */
        LogError("json_object_set_string failed for moduleId");
        result = NULL;
    }
    else if ((deviceOrModuleInfo->managedBy != NULL) && ((json_object_set_string(root_object, DEVICE_JSON_KEY_MANAGED_BY, deviceOrModuleInfo->managedBy)) != JSONSuccess))
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_013: [ IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_ERROR_CREATING_JSON if the JSON creation failed  ] */
        LogError("json_object_set_string failed for managedBy");
        result = NULL;
    }
    else if (json_object_dotset_string(root_object, DEVICE_JSON_KEY_DEVICE_STATUS, getStatusStringForJson(deviceOrModuleInfo->status)) != JSONSuccess)
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_013: [ IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR if the JSON creation failed  ] */
        LogError("json_object_dotset_string failed for status");
        result = NULL;
    }
    /*Codes_SRS_IOTHUBREGISTRYMANAGER_31_001: [** IoTHubRegistryManager_CreateDevice shall set 'type' to "sas"/"selfSigned"/"certificateAuthority" based on deviceOrModuleInfo->authMethod IOTHUB_REGISTRYMANAGER_AUTH_SPK/IOTHUB_REGISTRYMANAGER_AUTH_X509_THUMBPRINT/IOTHUB_REGISTRYMANAGER_AUTH_X509_CERTIFICATE_AUTHORITY **]** */
    /*Codes_SRS_IOTHUBREGISTRYMANAGER_31_002: [** IoTHubRegistryManager_UpdateDevice shall set 'type' to "sas"/"selfSigned"/"certificateAuthority" based on deviceOrModuleInfo->authMethod IOTHUB_REGISTRYMANAGER_AUTH_SPK/IOTHUB_REGISTRYMANAGER_AUTH_X509_THUMBPRINT/IOTHUB_REGISTRYMANAGER_AUTH_X509_CERTIFICATE_AUTHORITY **]** */
    else if ((NULL == (authTypeForJson = getAuthTypeStringForJson(deviceOrModuleInfo->authMethod))) || ((json_object_dotset_string(root_object, DEVICE_JSON_KEY_DEVICE_AUTH_TYPE, authTypeForJson)) != JSONSuccess))
    {
        LogError("json_object_dotset_string failed for authType");
        result = NULL;
    }
    //
    // Static function here.  We make the assumption that the auth method has been validated by the caller of this function.
    //
    /*Codes_SRS_IOTHUBREGISTRYMANAGER_06_002: [ IoTHubRegistryManager_CreateDevice shall, if deviceCreateInfo->authMethod is equal to "IOTHUB_REGISTRYMANAGER_AUTH_SPK", set "authorization.symmetricKey.primaryKey" to deviceCreateInfo->primaryKey and "authorization.symmetricKey.secondaryKey" to deviceCreateInfo->secondaryKey ] */
    /*Codes_SRS_IOTHUBREGISTRYMANAGER_06_004: [ IoTHubRegistryManager_UpdateDevice shall, if deviceUpdate->authMethod is equal to "IOTHUB_REGISTRYMANAGER_AUTH_SPK", set "authorization.symmetricKey.primaryKey" to deviceCreateInfo->primaryKey and "authorization.symmetricKey.secondaryKey" to deviceCreateInfo->secondaryKey ] */
    else if ((deviceOrModuleInfo->authMethod == IOTHUB_REGISTRYMANAGER_AUTH_SPK) && ((json_object_dotset_string(root_object, DEVICE_JSON_KEY_DEVICE_PRIMARY_KEY, deviceOrModuleInfo->primaryKey)) != JSONSuccess))
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_013: [ IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR if the JSON creation failed  ] */
        LogError("json_object_dotset_string failed for primarykey");
        result = NULL;
    }
    /*Codes_SRS_IOTHUBREGISTRYMANAGER_06_002: [ IoTHubRegistryManager_CreateDevice shall, if deviceCreateInfo->authMethod is equal to "IOTHUB_REGISTRYMANAGER_AUTH_SPK", set "authorization.symmetricKey.primaryKey" to deviceCreateInfo->primaryKey and "authorization.symmetricKey.secondaryKey" to deviceCreateInfo->secondaryKey ] */
    /*Codes_SRS_IOTHUBREGISTRYMANAGER_06_004: [ IoTHubRegistryManager_UpdateDevice shall, if deviceUpdate->authMethod is equal to "IOTHUB_REGISTRYMANAGER_AUTH_SPK", set "authorization.symmetricKey.primaryKey" to deviceCreateInfo->primaryKey and "authorization.symmetricKey.secondaryKey" to deviceCreateInfo->secondaryKey ] */
    else if ((deviceOrModuleInfo->authMethod == IOTHUB_REGISTRYMANAGER_AUTH_SPK) && ((json_object_dotset_string(root_object, DEVICE_JSON_KEY_DEVICE_SECONDARY_KEY, deviceOrModuleInfo->secondaryKey)) != JSONSuccess))
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_013: [ IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR if the JSON creation failed  ] */
        LogError("json_object_dotset_string failed for secondaryKey");
        result = NULL;
    }
    /*Codes_SRS_IOTHUBREGISTRYMANAGER_06_001: [ IoTHubRegistryManager_CreateDevice shall, if deviceCreateInfo->authMethod is equal to "IOTHUB_REGISTRYMANAGER_AUTH_X509_THUMBPRINT", set "authorization.x509Thumbprint.primaryThumbprint" to deviceCreateInfo->primaryKey and "authorization.x509Thumbprint.secondaryThumbprint" to deviceCreateInfo->secondaryKey ] */
    /*Codes_SRS_IOTHUBREGISTRYMANAGER_06_003: [ IoTHubRegistryManager_UpdateDevice shall, if deviceUpdate->authMethod is equal to "IOTHUB_REGISTRYMANAGER_AUTH_X509_THUMBPRINT", set "authorization.x509Thumbprint.primaryThumbprint" to deviceCreateInfo->primaryKey and "authorization.x509Thumbprint.secondaryThumbprint" to deviceCreateInfo->secondaryKey ] */
    else if ((deviceOrModuleInfo->authMethod == IOTHUB_REGISTRYMANAGER_AUTH_X509_THUMBPRINT) && ((json_object_dotset_string(root_object, DEVICE_JSON_KEY_DEVICE_PRIMARY_THUMBPRINT, deviceOrModuleInfo->primaryKey)) != JSONSuccess))
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_013: [ IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR if the JSON creation failed  ] */
        LogError("json_object_dotset_string failed for primaryThumbprint");
        result = NULL;
    }
    /*Codes_SRS_IOTHUBREGISTRYMANAGER_06_001: [ IoTHubRegistryManager_CreateDevice shall, if deviceCreateInfo->authMethod is equal to "IOTHUB_REGISTRYMANAGER_AUTH_X509_THUMBPRINT", set "authorization.x509Thumbprint.primaryThumbprint" to deviceCreateInfo->primaryKey and "authorization.x509Thumbprint.secondaryThumbprint" to deviceCreateInfo->secondaryKey ] */
    /*Codes_SRS_IOTHUBREGISTRYMANAGER_06_003: [ IoTHubRegistryManager_UpdateDevice shall, if deviceUpdate->authMethod is equal to "IOTHUB_REGISTRYMANAGER_AUTH_X509_THUMBPRINT", set "authorization.x509Thumbprint.primaryThumbprint" to deviceCreateInfo->primaryKey and "authorization.x509Thumbprint.secondaryThumbprint" to deviceCreateInfo->secondaryKey ] */
    else if ((deviceOrModuleInfo->authMethod == IOTHUB_REGISTRYMANAGER_AUTH_X509_THUMBPRINT) && ((json_object_dotset_string(root_object, DEVICE_JSON_KEY_DEVICE_SECONDARY_THUMBPRINT, deviceOrModuleInfo->secondaryKey)) != JSONSuccess))
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_013: [ IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR if the JSON creation failed  ] */
        LogError("json_object_dotset_string failed for secondaryThumbprint");
        result = NULL;
    }
    else if ((deviceOrModuleInfo->moduleId == NULL) && (json_object_dotset_boolean(root_object, DEVICE_JSON_KEY_CAPABILITIES_IOTEDGE, iotEdge_capable)) != JSONSuccess)
    {
        LogError("json_object_dotset_string failed for iotEdge capable");
        result = NULL;
    }
    else
    {
        char* serialized_string;
        if ((serialized_string = json_serialize_to_string(root_value)) == NULL)
        {
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_013: [ IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR if the JSON creation failed  ] */
            LogError("json_serialize_to_string failed");
            result = NULL;
        }
        else
        {
            if ((result = BUFFER_create((const unsigned char*)serialized_string, strlen(serialized_string))) == NULL)
            {
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_013: [ IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR if the JSON creation failed  ] */
                LogError("Buffer_Create failed");
                result = NULL;
            }
            json_free_serialized_string(serialized_string);
        }
    }

    if ((jsonStatus = json_object_clear(root_object)) != JSONSuccess)
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_013: [ IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR if the JSON creation failed  ] */
        LogError("json_object_clear failed");
        BUFFER_delete(result);
        result = NULL;
    }
    if(root_value != NULL)
        json_value_free(root_value);

    return result;
}

static IOTHUB_REGISTRYMANAGER_RESULT parseDeviceOrModuleJsonObject(JSON_Object* root_object, IOTHUB_DEVICE_OR_MODULE* deviceOrModuleInfo)
{
    IOTHUB_REGISTRYMANAGER_RESULT result;

    const char* deviceId = json_object_get_string(root_object, DEVICE_JSON_KEY_DEVICE_NAME);
    const char* moduleId = json_object_get_string(root_object, DEVICE_JSON_KEY_MODULE_NAME);
    const char* managedBy = json_object_get_string(root_object, DEVICE_JSON_KEY_MANAGED_BY);
    const char* primaryKey = NULL;
    const char* secondaryKey = NULL;
    const char* authType = json_object_dotget_string(root_object, DEVICE_JSON_KEY_DEVICE_AUTH_TYPE);
    const char* generationId = json_object_get_string(root_object, DEVICE_JSON_KEY_DEVICE_GENERATION_ID);
    const char* eTag = json_object_get_string(root_object, DEVICE_JSON_KEY_DEVICE_ETAG);
    const char* connectionState = json_object_get_string(root_object, DEVICE_JSON_KEY_DEVICE_CONNECTIONSTATE);
    const char* connectionStateUpdatedTime = json_object_get_string(root_object, DEVICE_JSON_KEY_DEVICE_CONNECTIONSTATEUPDATEDTIME);
    const char* status = json_object_get_string(root_object, DEVICE_JSON_KEY_DEVICE_STATUS);
    const char* statusReason = json_object_get_string(root_object, DEVICE_JSON_KEY_DEVICE_STATUSREASON);
    const char* statusUpdatedTime = json_object_get_string(root_object, DEVICE_JSON_KEY_DEVICE_STATUSUPDATEDTIME);
    const char* lastActivityTime = json_object_get_string(root_object, DEVICE_JSON_KEY_DEVICE_LASTACTIVITYTIME);
    const char* cloudToDeviceMessageCount = json_object_get_string(root_object, DEVICE_JSON_KEY_DEVICE_CLOUDTODEVICEMESSAGECOUNT);
    const char* isManaged = json_object_get_string(root_object, DEVICE_JSON_KEY_DEVICE_ISMANAGED);
    const char* configuration = json_object_get_string(root_object, DEVICE_JSON_KEY_DEVICE_CONFIGURATION);
    const char* deviceProperties = json_object_get_string(root_object, DEVICE_JSON_KEY_DEVICE_DEVICEROPERTIES);
    const char* serviceProperties = json_object_get_string(root_object, DEVICE_JSON_KEY_DEVICE_SERVICEPROPERTIES);
    int iotEdge_capable = json_object_dotget_boolean(root_object, DEVICE_JSON_KEY_CAPABILITIES_IOTEDGE);

    if (NULL != authType)
    {
        if (0 == strcmp(authType, DEVICE_JSON_KEY_DEVICE_AUTH_SAS))
        {
            primaryKey = (char*)json_object_dotget_string(root_object, DEVICE_JSON_KEY_DEVICE_PRIMARY_KEY);
            secondaryKey = (char*)json_object_dotget_string(root_object, DEVICE_JSON_KEY_DEVICE_SECONDARY_KEY);
            deviceOrModuleInfo->authMethod = IOTHUB_REGISTRYMANAGER_AUTH_SPK;
        }
        else if (0 == strcmp(authType, DEVICE_JSON_KEY_DEVICE_AUTH_SELF_SIGNED))
        {
            primaryKey = (char*)json_object_dotget_string(root_object, DEVICE_JSON_KEY_DEVICE_PRIMARY_THUMBPRINT);
            secondaryKey = (char*)json_object_dotget_string(root_object, DEVICE_JSON_KEY_DEVICE_SECONDARY_THUMBPRINT);
            deviceOrModuleInfo->authMethod = IOTHUB_REGISTRYMANAGER_AUTH_X509_THUMBPRINT;
        }
        else if (0 == strcmp(authType, DEVICE_JSON_KEY_DEVICE_AUTH_CERTIFICATE_AUTHORITY))
        {
            deviceOrModuleInfo->authMethod = IOTHUB_REGISTRYMANAGER_AUTH_X509_CERTIFICATE_AUTHORITY;
        }
        else if (0 == strcmp(authType, DEVICE_JSON_KEY_DEVICE_AUTH_NONE))
        {
            deviceOrModuleInfo->authMethod = IOTHUB_REGISTRYMANAGER_AUTH_NONE;
        }
        else
        {
            deviceOrModuleInfo->authMethod = IOTHUB_REGISTRYMANAGER_AUTH_UNKNOWN;
        }
    }
    else
    {
        deviceOrModuleInfo->authMethod = IOTHUB_REGISTRYMANAGER_AUTH_UNKNOWN;
    }

    if ((deviceId != NULL) && (mallocAndStrcpy_s((char**)&(deviceOrModuleInfo->deviceId), deviceId) != 0))
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_023: [ If the JSON parsing failed, IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_035: [ If the JSON parsing failed, IoTHubRegistryManager_GetDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
        LogError("mallocAndStrcpy_s failed for deviceId");
        result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
    }
    else if ((moduleId != NULL) && (mallocAndStrcpy_s((char**)&(deviceOrModuleInfo->moduleId), moduleId) != 0))
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_023: [ If the JSON parsing failed, IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_035: [ If the JSON parsing failed, IoTHubRegistryManager_GetDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
        LogError("mallocAndStrcpy_s failed for moduleId");
        result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
    }
    else if ((primaryKey != NULL) && (mallocAndStrcpy_s((char**)&deviceOrModuleInfo->primaryKey, primaryKey) != 0))
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_023: [ If the JSON parsing failed, IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_035: [ If the JSON parsing failed, IoTHubRegistryManager_GetDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
        LogError("mallocAndStrcpy_s failed for primaryKey");
        result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
    }
    else if ((secondaryKey != NULL) && (mallocAndStrcpy_s((char**)&deviceOrModuleInfo->secondaryKey, secondaryKey) != 0))
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_023: [ If the JSON parsing failed, IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_035: [ If the JSON parsing failed, IoTHubRegistryManager_GetDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
        LogError("mallocAndStrcpy_s failed for secondaryKey");
        result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
    }
    else if ((generationId != NULL) && (mallocAndStrcpy_s((char**)&deviceOrModuleInfo->generationId, generationId) != 0))
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_023: [ If the JSON parsing failed, IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_035: [ If the JSON parsing failed, IoTHubRegistryManager_GetDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
        LogError("mallocAndStrcpy_s failed for generationId");
        result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
    }
    else if ((eTag != NULL) && (mallocAndStrcpy_s((char**)&deviceOrModuleInfo->eTag, eTag) != 0))
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_023: [ If the JSON parsing failed, IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_035: [ If the JSON parsing failed, IoTHubRegistryManager_GetDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
        LogError("mallocAndStrcpy_s failed for eTag");
        result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
    }
    else if ((connectionStateUpdatedTime != NULL) && (mallocAndStrcpy_s((char**)&deviceOrModuleInfo->connectionStateUpdatedTime, connectionStateUpdatedTime) != 0))
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_023: [ If the JSON parsing failed, IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_035: [ If the JSON parsing failed, IoTHubRegistryManager_GetDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
        LogError("mallocAndStrcpy_s failed for connectionStateUpdatedTime");
        result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
    }
    else if ((statusReason != NULL) && (mallocAndStrcpy_s((char**)&deviceOrModuleInfo->statusReason, statusReason) != 0))
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_023: [ If the JSON parsing failed, IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_035: [ If the JSON parsing failed, IoTHubRegistryManager_GetDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
        LogError("mallocAndStrcpy_s failed for statusReason");
        result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
    }
    else if ((statusUpdatedTime != NULL) && (mallocAndStrcpy_s((char**)&deviceOrModuleInfo->statusUpdatedTime, statusUpdatedTime) != 0))
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_023: [ If the JSON parsing failed, IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_035: [ If the JSON parsing failed, IoTHubRegistryManager_GetDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
        LogError("mallocAndStrcpy_s failed for statusUpdatedTime");
        result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
    }
    else if ((lastActivityTime != NULL) && (mallocAndStrcpy_s((char**)&deviceOrModuleInfo->lastActivityTime, lastActivityTime) != 0))
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_023: [ If the JSON parsing failed, IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_035: [ If the JSON parsing failed, IoTHubRegistryManager_GetDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
        LogError("mallocAndStrcpy_s failed for lastActivityTime");
        result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
    }
    else if ((configuration != NULL) && (mallocAndStrcpy_s((char**)&deviceOrModuleInfo->configuration, configuration) != 0))
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_023: [ If the JSON parsing failed, IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_035: [ If the JSON parsing failed, IoTHubRegistryManager_GetDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
        LogError("mallocAndStrcpy_s failed for configuration");
        result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
    }
    else if ((deviceProperties != NULL) && (mallocAndStrcpy_s((char**)&deviceOrModuleInfo->deviceProperties, deviceProperties) != 0))
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_023: [ If the JSON parsing failed, IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_035: [ If the JSON parsing failed, IoTHubRegistryManager_GetDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
        LogError("mallocAndStrcpy_s failed for deviceProperties");
        result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
    }
    else if ((serviceProperties != NULL) && (mallocAndStrcpy_s((char**)&deviceOrModuleInfo->serviceProperties, serviceProperties) != 0))
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_023: [ If the JSON parsing failed, IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_035: [ If the JSON parsing failed, IoTHubRegistryManager_GetDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
        LogError("mallocAndStrcpy_s failed for serviceProperties");
        result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
    }
    else if ((managedBy != NULL) && (mallocAndStrcpy_s((char**)&(deviceOrModuleInfo->managedBy), managedBy) != 0))
    {
        LogError("mallocAndStrcpy_s failed for managedBy");
        result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
    }
    else
    {
        if ((connectionState != NULL) && (strcmp(connectionState, DEVICE_JSON_DEFAULT_VALUE_CONNECTED) == 0))
        {
            deviceOrModuleInfo->connectionState = IOTHUB_DEVICE_CONNECTION_STATE_CONNECTED;
        }
        if ((status != NULL) && (strcmp(status, DEVICE_JSON_DEFAULT_VALUE_ENABLED) == 0))
        {
            deviceOrModuleInfo->status = IOTHUB_DEVICE_STATUS_ENABLED;
        }
        if (cloudToDeviceMessageCount != NULL)
        {
            deviceOrModuleInfo->cloudToDeviceMessageCount = atoi(cloudToDeviceMessageCount);
        }
        if ((isManaged != NULL) && (strcmp(isManaged, DEVICE_JSON_DEFAULT_VALUE_TRUE) == 0))
        {
            deviceOrModuleInfo->isManaged = true;
        }
        if ((iotEdge_capable == -1) || (iotEdge_capable == 0))
        {
            deviceOrModuleInfo->iotEdge_capable = false;
        }
        else
        {
            deviceOrModuleInfo->iotEdge_capable = true;
        }
        result = IOTHUB_REGISTRYMANAGER_OK;
    }

    return result;
}

static IOTHUB_REGISTRYMANAGER_RESULT parseDeviceOrModuleJson(BUFFER_HANDLE jsonBuffer, IOTHUB_DEVICE_OR_MODULE* deviceOrModuleInfo)
{
    IOTHUB_REGISTRYMANAGER_RESULT result;

    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_024: [ If the deviceOrModuleInfo out parameter is not NULL IoTHubRegistryManager_CreateDevice shall save the received deviceOrModuleInfo to the out parameter and return IOTHUB_REGISTRYMANAGER_OK ] */
    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_033: [ IoTHubRegistryManager_GetDevice shall verify the received HTTP status code and if it is less or equal than 300 then try to parse the response JSON to deviceOrModuleInfo for the following properties: deviceId, primaryKey, secondaryKey, generationId, eTag, connectionState, connectionstateUpdatedTime, status, statusReason, statusUpdatedTime, lastActivityTime, cloudToDeviceMessageCount ] */
    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_034: [ If any of the property field above missing from the JSON the property value will not be populated ] */
    if (jsonBuffer == NULL)
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_023: [ If the JSON parsing failed, IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_035: [ If the JSON parsing failed, IoTHubRegistryManager_GetDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
        LogError("jsonBuffer cannot be NULL");
        result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
    }
    else if (deviceOrModuleInfo == NULL)
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_023: [ If the JSON parsing failed, IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_035: [ If the JSON parsing failed, IoTHubRegistryManager_GetDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
        LogError("deviceOrModuleInfo cannot be NULL");
        result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
    }
    else
    {
        const char* bufferStr = NULL;
        JSON_Value* root_value = NULL;
        JSON_Object* root_object = NULL;
        JSON_Status jsonStatus;

        if ((bufferStr = (const char*)BUFFER_u_char(jsonBuffer)) == NULL)
        {
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_023: [ If the JSON parsing failed, IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_035: [ If the JSON parsing failed, IoTHubRegistryManager_GetDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
            LogError("BUFFER_u_char failed");
            result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
        }
        else if ((root_value = json_parse_string(bufferStr)) == NULL)
        {
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_023: [ If the JSON parsing failed, IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_035: [ If the JSON parsing failed, IoTHubRegistryManager_GetDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
            LogError("json_parse_string failed");
            result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
        }
        else if ((root_object = json_value_get_object(root_value)) == NULL)
        {
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_023: [ If the JSON parsing failed, IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_035: [ If the JSON parsing failed, IoTHubRegistryManager_GetDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
            LogError("json_value_get_object failed");
            result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
        }
        else
        {
            result = parseDeviceOrModuleJsonObject(root_object, deviceOrModuleInfo);
        }

        if ((jsonStatus = json_object_clear(root_object)) != JSONSuccess)
        {
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_023: [ If the JSON parsing failed, IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_035: [ If the JSON parsing failed, IoTHubRegistryManager_GetDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
            LogError("json_object_clear failed");
            result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
        }

        json_value_free(root_value);

        if (result != IOTHUB_REGISTRYMANAGER_OK)
        {
            free_deviceOrModule_members(deviceOrModuleInfo);
        }
    }
    return result;
}

static int addDeviceOrModuleToLinkedListAsDevice(IOTHUB_DEVICE_OR_MODULE* iothubDeviceOrModule, SINGLYLINKEDLIST_HANDLE deviceOrModuleList)
{
    int result;
    IOTHUB_DEVICE* device = NULL;

    if ((device = (IOTHUB_DEVICE*)malloc(sizeof(IOTHUB_DEVICE))) == NULL)
    {
        LogError("Malloc failed for device");
        result = IOTHUB_REGISTRYMANAGER_ERROR;
    }
    else
    {
        //Convert to a Device struct
        move_deviceOrModule_members_to_device(iothubDeviceOrModule, device);

        if ((singlylinkedlist_add(deviceOrModuleList, device)) == NULL)
        {
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_072: [** If populating the deviceList parameter fails IoTHubRegistryManager_GetDeviceList shall return IOTHUB_REGISTRYMANAGER_ERROR **] */
            LogError("singlylinkedlist_add failed");
            free(device);   //only free structure. Because members are still referenced by iothubDeviceOrModule we will free them after
            result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
        }
        else
        {
            result = IOTHUB_REGISTRYMANAGER_OK;
        }
    }

    return result;
}

static int addDeviceOrModuleToLinkedListAsModule(IOTHUB_DEVICE_OR_MODULE* iothubDeviceOrModule, SINGLYLINKEDLIST_HANDLE deviceOrModuleList, int version)
{
    int result;
    IOTHUB_MODULE* module = NULL;

    if ((module = (IOTHUB_MODULE*)malloc(sizeof(IOTHUB_MODULE))) == NULL)
    {
        LogError("Malloc failed for module");
        result = IOTHUB_REGISTRYMANAGER_ERROR;
    }
    else
    {
        //Convert to a Module struct
        module->version = version;
        move_deviceOrModule_members_to_module(iothubDeviceOrModule, module);

        if ((singlylinkedlist_add(deviceOrModuleList, module)) == NULL)
        {
            LogError("singlylinkedlist_add failed");
            free(module);   //only free structure. Because members are still referenced by iothubDeviceOrModule we will free them after
            result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
        }
        else
        {
            result = IOTHUB_REGISTRYMANAGER_OK;
        }
    }

    return result;
}

static IOTHUB_REGISTRYMANAGER_RESULT parseDeviceOrModuleListJson(BUFFER_HANDLE jsonBuffer, SINGLYLINKEDLIST_HANDLE deviceOrModuleList, IOTHUB_REGISTRYMANAGER_MODEL_TYPE struct_type, int struct_version)
{
    IOTHUB_REGISTRYMANAGER_RESULT result;

    const char* bufferStr = NULL;
    JSON_Value* root_value = NULL;
    JSON_Array* device_or_module_array = NULL;
    JSON_Status jsonStatus = JSONFailure;

    if (jsonBuffer == NULL)
    {
        LogError("jsonBuffer cannot be NULL");
        result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
    }
    else if (deviceOrModuleList == NULL)
    {
        LogError("deviceOrModuleList cannot be NULL");
        result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
    }
    else
    {
        if ((bufferStr = (const char*)BUFFER_u_char(jsonBuffer)) == NULL)
        {
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_072: [** If populating the deviceList parameter fails IoTHubRegistryManager_GetDeviceList shall return IOTHUB_REGISTRYMANAGER_ERROR **] */
            LogError("BUFFER_u_char failed");
            result = IOTHUB_REGISTRYMANAGER_ERROR;
        }
        else if ((root_value = json_parse_string(bufferStr)) == NULL)
        {
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_070: [** If any of the parson API fails, IoTHubRegistryManager_GetDeviceList shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR **] */
            LogError("json_parse_string failed");
            result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
        }
        else if ((device_or_module_array = json_value_get_array(root_value)) == NULL)
        {
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_070: [** If any of the parson API fails, IoTHubRegistryManager_GetDeviceList shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR **] */
            LogError("json_value_get_object failed");
            result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
        }
        else
        {
            result = IOTHUB_REGISTRYMANAGER_OK;

            size_t array_count = json_array_get_count(device_or_module_array);
            for (size_t i = 0; i < array_count; i++)
            {
                JSON_Object* device_or_module_object = NULL;
                IOTHUB_DEVICE_OR_MODULE iothubDeviceOrModule;

                if ((device_or_module_object = json_array_get_object(device_or_module_array, i)) == NULL)
                {
                    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_070: [** If any of the parson API fails, IoTHubRegistryManager_GetDeviceList shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR **] */
                    LogError("json_array_get_object failed");
                    result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
                }
                else
                {
                    initializeDeviceOrModuleInfoMembers(&iothubDeviceOrModule);

                    result = parseDeviceOrModuleJsonObject(device_or_module_object, &iothubDeviceOrModule);
                    if (IOTHUB_REGISTRYMANAGER_OK != result)
                    {
                        free_deviceOrModule_members(&iothubDeviceOrModule);
                    }
                    else
                    {
                        if (struct_type == IOTHUB_REGISTRYMANAGER_MODEL_TYPE_DEVICE)
                        {
                            result = addDeviceOrModuleToLinkedListAsDevice(&iothubDeviceOrModule, deviceOrModuleList);
                            if (result == IOTHUB_REGISTRYMANAGER_OK) //only free these if we pass, if we fail, will deal with below
                            {
                                free_nonDevice_members_from_deviceOrModule(&iothubDeviceOrModule);
                            }
                        }
                        else //IOTHUB_REGISTRYMANAGER_MODEL_TYPE_MODULE
                        {
                            result = addDeviceOrModuleToLinkedListAsModule(&iothubDeviceOrModule, deviceOrModuleList, struct_version);
                            //no nonModule members to free... yet
                        }

                        if (result != IOTHUB_REGISTRYMANAGER_OK)
                        {
                            free_deviceOrModule_members(&iothubDeviceOrModule);
                        }
                    }
                }

                if ((device_or_module_object != NULL) && ((jsonStatus = json_object_clear(device_or_module_object)) != JSONSuccess))
                {
                    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_070: [** If any of the parson API fails, IoTHubRegistryManager_GetDeviceList shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR **] */
                    LogError("json_object_clear failed");
                    result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
                }
                if (result != IOTHUB_REGISTRYMANAGER_OK)
                {
                    break;
                }
            }
        }
    }
    if (device_or_module_array != NULL)
    {
        if ((jsonStatus = json_array_clear(device_or_module_array)) != JSONSuccess)
        {
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_070: [** If any of the parson API fails, IoTHubRegistryManager_GetDeviceList shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR **] */
            LogError("json_array_clear failed");
            result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
        }
    }

    if (root_value != NULL)
    {
        json_value_free(root_value);
    }

    if (result != IOTHUB_REGISTRYMANAGER_OK)
    {
        if (deviceOrModuleList != NULL)
        {
            LIST_ITEM_HANDLE itemHandle = singlylinkedlist_get_head_item(deviceOrModuleList);
            while (itemHandle != NULL)
            {
                const void* curr_item = singlylinkedlist_item_get_value(itemHandle);
                if (struct_type == IOTHUB_REGISTRYMANAGER_MODEL_TYPE_DEVICE)
                {
                    IOTHUB_DEVICE* deviceInfo = (IOTHUB_DEVICE*)curr_item;
                    free_device_members(deviceInfo);
                    free(deviceInfo);
                }
                else //IOTHUB_REGISTRYMANAGER_MODEL_TYPE_MODULE
                {
                    IOTHUB_MODULE* moduleInfo = (IOTHUB_MODULE*)curr_item;
                    IoTHubRegistryManager_FreeModuleMembers(moduleInfo);
                    free(moduleInfo);
                }

                LIST_ITEM_HANDLE lastHandle = itemHandle;
                itemHandle = singlylinkedlist_get_next_item(itemHandle);
                singlylinkedlist_remove(deviceOrModuleList, lastHandle);
            }
        }
    }
    return result;
}

static IOTHUB_REGISTRYMANAGER_RESULT parseStatisticsJson(BUFFER_HANDLE jsonBuffer, IOTHUB_REGISTRY_STATISTICS* registryStatistics)
{
    IOTHUB_REGISTRYMANAGER_RESULT result;

    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_083: [ IoTHubRegistryManager_GetStatistics shall save the registry statistics to the out value and return IOTHUB_REGISTRYMANAGER_OK ] */
    if (jsonBuffer == NULL)
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_082: [ If the parsing failed, IoTHubRegistryManager_GetStatistics shall return IOTHUB_REGISTRYMANAGER_ERROR ] */
        LogError("jsonBuffer cannot be NULL");
        result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
    }
    else if (registryStatistics == NULL)
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_082: [ If the parsing failed, IoTHubRegistryManager_GetStatistics shall return IOTHUB_REGISTRYMANAGER_ERROR ] */
        LogError("registryStatistics cannot be NULL");
        result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
    }
    else
    {
        const char* bufferStr = NULL;
        JSON_Value* root_value = NULL;
        JSON_Object* root_object = NULL;
        JSON_Status jsonStatus;

        if ((bufferStr = (const char*)BUFFER_u_char(jsonBuffer)) == NULL)
        {
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_082: [ If the parsing failed, IoTHubRegistryManager_GetStatistics shall return IOTHUB_REGISTRYMANAGER_ERROR ] */
            LogError("BUFFER_u_char failed");
            result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
        }
        else if ((root_value = json_parse_string(bufferStr)) == NULL)
        {
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_082: [ If the parsing failed, IoTHubRegistryManager_GetStatistics shall return IOTHUB_REGISTRYMANAGER_ERROR ] */
            LogError("json_parse_string failed");
            result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
        }
        else if ((root_object = json_value_get_object(root_value)) == NULL)
        {
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_082: [ If the parsing failed, IoTHubRegistryManager_GetStatistics shall return IOTHUB_REGISTRYMANAGER_ERROR ] */
            LogError("json_value_get_object failed");
            result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
        }
        else
        {
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_083: [ IoTHubRegistryManager_GetStatistics shall save the registry statistics to the out value and return IOTHUB_REGISTRYMANAGER_OK ] */
            registryStatistics->totalDeviceCount = (size_t)json_object_get_number(root_object, DEVICE_JSON_KEY_TOTAL_DEVICECOUNT);
            registryStatistics->enabledDeviceCount = (size_t)json_object_get_number(root_object, DEVICE_JSON_KEY_ENABLED_DEVICECCOUNT);
            registryStatistics->disabledDeviceCount = (size_t)json_object_get_number(root_object, DEVICE_JSON_KEY_DISABLED_DEVICECOUNT);

            result = IOTHUB_REGISTRYMANAGER_OK;
        }

        if ((jsonStatus = json_object_clear(root_object)) != JSONSuccess)
        {
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_082: [ If the parsing failed, IoTHubRegistryManager_GetStatistics shall return IOTHUB_REGISTRYMANAGER_ERROR ] */
            LogError("json_object_clear failed");
            result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
        }
        json_value_free(root_value);
    }
    return result;
}

static IOTHUB_REGISTRYMANAGER_RESULT createRelativePath(IOTHUB_REQUEST_MODE iotHubRequestMode, const char* deviceName, const char* moduleId, size_t numberOfDevices, char* relativePath)
{
    IOTHUB_REGISTRYMANAGER_RESULT result;

    if ((iotHubRequestMode == IOTHUB_REQUEST_GET_DEVICE_LIST) && (deviceName == NULL))
    {
        if ((numberOfDevices <= 0) || (numberOfDevices > IOTHUB_DEVICES_MAX_REQUEST))
        {
            numberOfDevices = IOTHUB_DEVICES_MAX_REQUEST;
        }

        char numberStr[256];
        result = IOTHUB_REGISTRYMANAGER_ERROR;
        if (snprintf(numberStr, 256, "%lu", (unsigned long)numberOfDevices) > 0)
        {
            if (snprintf(relativePath, 256, RELATIVE_PATH_FMT_LIST, numberStr, URL_API_VERSION) > 0)
            {
                result = IOTHUB_REGISTRYMANAGER_OK;
            }
            else
            {
                result = IOTHUB_REGISTRYMANAGER_ERROR;
            }
        }
        else
        {
            result = IOTHUB_REGISTRYMANAGER_ERROR;
        }
    }
    else if ((iotHubRequestMode == IOTHUB_REQUEST_GET_DEVICE_LIST) && (deviceName != NULL))
    {
        if (snprintf(relativePath, 256, RELATIVE_PATH_FMT_MODULE_LIST, deviceName, URL_API_VERSION))
        {
            result = IOTHUB_REGISTRYMANAGER_OK;
        }
        else
        {
            result = IOTHUB_REGISTRYMANAGER_ERROR;
        }
    }
    else if (iotHubRequestMode == IOTHUB_REQUEST_GET_STATISTICS)
    {
        if (snprintf(relativePath, 256, RELATIVE_PATH_FMT_STAT, URL_API_VERSION) > 0)
        {
            result = IOTHUB_REGISTRYMANAGER_OK;
        }
        else
        {
            result = IOTHUB_REGISTRYMANAGER_ERROR;
        }
    }
    else
    {
        if (moduleId != NULL)
        {
            result = (snprintf(relativePath, 256, RELATIVE_PATH_MODULE_FMT_CRUD, deviceName, moduleId, URL_API_VERSION)) > 0 ? IOTHUB_REGISTRYMANAGER_OK : IOTHUB_REGISTRYMANAGER_ERROR;
        }
        else
        {
            result = (snprintf(relativePath, 256, RELATIVE_PATH_FMT_CRUD, deviceName, URL_API_VERSION) > 0) ? IOTHUB_REGISTRYMANAGER_OK : IOTHUB_REGISTRYMANAGER_ERROR;
        }
    }

    return result;
}

static HTTP_HEADERS_HANDLE createHttpHeader(IOTHUB_REQUEST_MODE iotHubRequestMode)
{
    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_015: [ IoTHubRegistryManager_CreateDevice shall create an HTTP PUT request using the following HTTP headers: authorization=sasToken,Request-Id=1001,Accept=application/json,Content-Type=application/json,charset=utf-8 ] */
    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_027: [ IoTHubRegistryManager_GetDevice shall add the following headers to the created HTTP GET request: authorization=sasToken,Request-Id=1001,Accept=application/json,Content-Type=application/json,charset=utf-8 ] */
    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_054: [ IoTHubRegistryManager_DeleteDevice shall add the following headers to the created HTTP GET request : authorization = sasToken, Request - Id = 1001, Accept = application / json, Content - Type = application / json, charset = utf - 8 ] */
    HTTP_HEADERS_HANDLE httpHeader;

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
    else if (HTTPHeaders_AddHeaderNameValuePair(httpHeader, HTTP_HEADER_KEY_REQUEST_ID, HTTP_HEADER_VAL_REQUEST_ID) != HTTP_HEADERS_OK)
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

    if ((iotHubRequestMode == IOTHUB_REQUEST_DELETE) || (iotHubRequestMode == IOTHUB_REQUEST_UPDATE))
    {
        if (HTTPHeaders_AddHeaderNameValuePair(httpHeader, HTTP_HEADER_KEY_IFMATCH, HTTP_HEADER_VAL_IFMATCH) != HTTP_HEADERS_OK)
        {
            LogError("HTTPHeaders_AddHeaderNameValuePair failed for If-Match header");
            HTTPHeaders_Free(httpHeader);
            httpHeader = NULL;
        }

    }
    return httpHeader;
}

static STRING_HANDLE createUriPath(IOTHUB_REGISTRYMANAGER_HANDLE registryManagerHandle)
{
    if (registryManagerHandle->deviceId != NULL)
    {
        return STRING_construct_sprintf("%s%%2Fdevices%%2F%s", registryManagerHandle->hostname, registryManagerHandle->deviceId);
    }
    else
    {
        return STRING_construct(registryManagerHandle->hostname);
    }
}

static IOTHUB_REGISTRYMANAGER_RESULT sendHttpRequestCRUD(IOTHUB_REGISTRYMANAGER_HANDLE registryManagerHandle, IOTHUB_REQUEST_MODE iotHubRequestMode, const char* deviceName, const char* moduleId, BUFFER_HANDLE deviceJsonBuffer, size_t numberOfDevices, BUFFER_HANDLE responseBuffer)
{
    IOTHUB_REGISTRYMANAGER_RESULT result;

    STRING_HANDLE uriResource = NULL;
    STRING_HANDLE accessKey = NULL;
    STRING_HANDLE keyName = NULL;
    HTTPAPIEX_SAS_HANDLE httpExApiSasHandle = NULL;
    HTTPAPIEX_HANDLE httpExApiHandle = NULL;
    HTTP_HEADERS_HANDLE httpHeader = NULL;

    if ((uriResource = createUriPath(registryManagerHandle)) == NULL)
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_099: [ If any of the call fails during the HTTP creation IoTHubRegistryManager_CreateDevice shall fail and return IOTHUB_REGISTRYMANAGER_ERROR ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_103: [ If any of the call fails during the HTTP creation IoTHubRegistryManager_UpdateDevice shall fail and return IOTHUB_REGISTRYMANAGER_ERROR ] */
        LogError("STRING_construct failed for uriResource");
        result = IOTHUB_REGISTRYMANAGER_ERROR;
    }
    else if ((accessKey = STRING_construct(registryManagerHandle->sharedAccessKey)) == NULL)
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_099: [ If any of the call fails during the HTTP creation IoTHubRegistryManager_CreateDevice shall fail and return IOTHUB_REGISTRYMANAGER_ERROR ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_103: [ If any of the call fails during the HTTP creation IoTHubRegistryManager_UpdateDevice shall fail and return IOTHUB_REGISTRYMANAGER_ERROR ] */
        LogError("STRING_construct failed for accessKey");
        result = IOTHUB_REGISTRYMANAGER_ERROR;
    }
    else if ((registryManagerHandle->keyName != NULL) && ((keyName = STRING_construct(registryManagerHandle->keyName)) == NULL))
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_099: [ If any of the call fails during the HTTP creation IoTHubRegistryManager_CreateDevice shall fail and return IOTHUB_REGISTRYMANAGER_ERROR ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_103: [ If any of the call fails during the HTTP creation IoTHubRegistryManager_UpdateDevice shall fail and return IOTHUB_REGISTRYMANAGER_ERROR ] */
        LogError("STRING_construct failed for keyName");
        result = IOTHUB_REGISTRYMANAGER_ERROR;
    }
    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_015: [ IoTHubRegistryManager_CreateDevice shall create an HTTP PUT request using the following HTTP headers: authorization=sasToken,Request-Id=1001,Accept=application/json,Content-Type=application/json,charset=utf-8 ] */
    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_027: [ IoTHubRegistryManager_GetDevice shall add the following headers to the created HTTP GET request: authorization=sasToken,Request-Id=1001,Accept=application/json,Content-Type=application/json,charset=utf-8 ] */
    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_043: [ IoTHubRegistryManager_UpdateDevice shall create an HTTP PUT request using the created JSON ] */
    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_044: [ IoTHubRegistryManager_UpdateDevice shall create an HTTP PUT request using the createdfollowing HTTP headers : authorization = sasToken, Request - Id = 1001, Accept = application / json, Content - Type = application / json, charset = utf - 8 ] */
    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_054: [ IoTHubRegistryManager_DeleteDevice shall add the following headers to the created HTTP GET request : authorization=sasToken, Request-Id=1001, Accept=application/json, Content-Type=application/json, charset=utf-8 ] */
    else if ((httpHeader = createHttpHeader(iotHubRequestMode)) == NULL)
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_019: [ If any of the HTTPAPI call fails IoTHubRegistryManager_CreateDevice shall fail and return IOTHUB_REGISTRYMANAGER_HTTPAPI_ERROR ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_104: [ If any of the HTTPAPI call fails IoTHubRegistryManager_UpdateDevice shall fail and return IOTHUB_REGISTRYMANAGER_HTTPAPI_ERROR ] */
        LogError("HttpHeader creation failed");
        result = IOTHUB_REGISTRYMANAGER_HTTPAPI_ERROR;
    }
    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_016: [ IoTHubRegistryManager_CreateDevice shall create an HTTPAPIEX_SAS_HANDLE handle by calling HTTPAPIEX_SAS_Create ] */
    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_028: [ IoTHubRegistryManager_GetDevice shall create an HTTPAPIEX_SAS_HANDLE handle by calling HTTPAPIEX_SAS_Create ] */
    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_045: [ IoTHubRegistryManager_UpdateDevice shall create an HTTPAPIEX_SAS_HANDLE handle by calling HTTPAPIEX_SAS_Create ] */
    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_055: [ IoTHubRegistryManager_DeleteDevice shall create an HTTPAPIEX_SAS_HANDLE handle by calling HTTPAPIEX_SAS_Create ] */
    else if ((httpExApiSasHandle = HTTPAPIEX_SAS_Create(accessKey, uriResource, keyName)) == NULL)
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_019: [ If any of the HTTPAPI call fails IoTHubRegistryManager_CreateDevice shall fail and return IOTHUB_REGISTRYMANAGER_HTTPAPI_ERROR ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_104: [ If any of the HTTPAPI call fails IoTHubRegistryManager_UpdateDevice shall fail and return IOTHUB_REGISTRYMANAGER_HTTPAPI_ERROR ] */
        LogError("HTTPAPIEX_SAS_Create failed");
        result = IOTHUB_REGISTRYMANAGER_HTTPAPI_ERROR;
    }
    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_017: [ IoTHubRegistryManager_CreateDevice shall create an HTTPAPIEX_HANDLE handle by calling HTTPAPIEX_Create ] */
    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_029: [ IoTHubRegistryManager_GetDevice shall create an HTTPAPIEX_HANDLE handle by calling HTTPAPIEX_Create ] */
    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_046: [ IoTHubRegistryManager_UpdateDevice shall create an HTTPAPIEX_HANDLE handle by calling HTTPAPIEX_Create ] */
    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_056: [ IoTHubRegistryManager_DeleteDevice shall create an HTTPAPIEX_HANDLE handle by calling HTTPAPIEX_Create ] */
    else if ((httpExApiHandle = HTTPAPIEX_Create(registryManagerHandle->hostname)) == NULL)
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_019: [ If any of the HTTPAPI call fails IoTHubRegistryManager_CreateDevice shall fail and return IOTHUB_REGISTRYMANAGER_HTTPAPI_ERROR ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_104: [ If any of the HTTPAPI call fails IoTHubRegistryManager_UpdateDevice shall fail and return IOTHUB_REGISTRYMANAGER_HTTPAPI_ERROR ] */
        LogError("HTTPAPIEX_Create failed");
        result = IOTHUB_REGISTRYMANAGER_HTTPAPI_ERROR;
    }
    else
    {
        HTTPAPI_REQUEST_TYPE httpApiRequestType = HTTPAPI_REQUEST_GET;
        char relativePath[256];
        unsigned int statusCode;
        unsigned char is_error = 0;

        if ((iotHubRequestMode == IOTHUB_REQUEST_CREATE) || (iotHubRequestMode == IOTHUB_REQUEST_UPDATE))
        {
            httpApiRequestType = HTTPAPI_REQUEST_PUT;
        }
        else if (iotHubRequestMode == IOTHUB_REQUEST_DELETE)
        {
            httpApiRequestType = HTTPAPI_REQUEST_DELETE;
        }
        else if ((iotHubRequestMode == IOTHUB_REQUEST_GET) || (iotHubRequestMode == IOTHUB_REQUEST_GET_DEVICE_LIST) || (iotHubRequestMode == IOTHUB_REQUEST_GET_STATISTICS))
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
            result = IOTHUB_REGISTRYMANAGER_HTTPAPI_ERROR;
        }
        else
        {
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_026: [ IoTHubRegistryManager_GetDevice shall create HTTP GET request URL using the given deviceId using the following format: url/devices/[deviceId]?api-version=2017-06-30  ] */
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_053: [ IoTHubRegistryManager_DeleteDevice shall create HTTP DELETE request URL using the given deviceId using the following format : url/devices/[deviceId]?api-version ] */
            if (createRelativePath(iotHubRequestMode, deviceName, moduleId, numberOfDevices, relativePath) != IOTHUB_REGISTRYMANAGER_OK)
            {
                LogError("Failure creating relative path");
                result = IOTHUB_REGISTRYMANAGER_ERROR;
            }
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_014: [ IoTHubRegistryManager_CreateDevice shall create an HTTP PUT request using the created JSON ] */
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_018: [ IoTHubRegistryManager_CreateDevice shall execute the HTTP PUT request by calling HTTPAPIEX_ExecuteRequest ] */
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_030: [ IoTHubRegistryManager_GetDevice shall execute the HTTP GET request by calling HTTPAPIEX_ExecuteRequest ] */
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_047: [ IoTHubRegistryManager_UpdateDevice shall execute the HTTP PUT request by calling HTTPAPIEX_ExecuteRequest ] */
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_057: [ IoTHubRegistryManager_DeleteDevice shall execute the HTTP DELETE request by calling HTTPAPIEX_ExecuteRequest ] */
            else if (HTTPAPIEX_SAS_ExecuteRequest(httpExApiSasHandle, httpExApiHandle, httpApiRequestType, relativePath, httpHeader, deviceJsonBuffer, &statusCode, NULL, responseBuffer) != HTTPAPIEX_OK)
            {
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_019: [ If any of the HTTPAPI call fails IoTHubRegistryManager_CreateDevice shall fail and return IOTHUB_REGISTRYMANAGER_HTTPAPI_ERROR ] */
                LogError("HTTPAPIEX_SAS_ExecuteRequest failed");
                result = IOTHUB_REGISTRYMANAGER_HTTPAPI_ERROR;
            }
            else
            {
                if (statusCode > 300)
                {
                    if ((iotHubRequestMode == IOTHUB_REQUEST_CREATE) && (statusCode == 409))
                    {
                        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_020: [ IoTHubRegistryManager_CreateDevice shall verify the received HTTP status code and if it is 409 then return IOTHUB_REGISTRYMANAGER_DEVICE_EXIST ] */
                        result = IOTHUB_REGISTRYMANAGER_DEVICE_EXIST;
                    }
                    else if ((iotHubRequestMode == IOTHUB_REQUEST_GET) && (statusCode == 404))
                    {
                        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_020: [ IoTHubRegistryManager_CreateDevice shall verify the received HTTP status code and if it is 404 then return IOTHUB_REGISTRYMANAGER_DEVICE_NOT_EXIST ] */
                        result = IOTHUB_REGISTRYMANAGER_DEVICE_NOT_EXIST;
                    }
                    else
                    {
                        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_021: [ IoTHubRegistryManager_CreateDevice shall verify the received HTTP status code and if it is greater than 300 then return IOTHUB_REGISTRYMANAGER_HTTP_STATUS_ERROR ] */
                        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_032: [ IoTHubRegistryManager_GetDevice shall verify the received HTTP status code and if it is greater than 300 then return IOTHUB_REGISTRYMANAGER_ERROR ] */
                        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_058: [ IoTHubRegistryManager_DeleteDevice shall verify the received HTTP status code and if it is greater than 300 then return IOTHUB_REGISTRYMANAGER_HTTP_STATUS_ERROR ] */
                        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_048: [ IoTHubRegistryManager_UpdateDevice shall verify the received HTTP status code and if it is greater than 300 then return IOTHUB_REGISTRYMANAGER_ERROR ] */
                        LogError("Http Failure status code %d.", statusCode);
                        result = IOTHUB_REGISTRYMANAGER_HTTP_STATUS_ERROR;
                    }
                }
                else
                {
                    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_022: [ IoTHubRegistryManager_CreateDevice shall verify the received HTTP status code and if it is less or equal than 300 then try to parse the response JSON to deviceInfo ] */
                    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_059: [ IoTHubRegistryManager_DeleteDevice shall verify the received HTTP status code and if it is less or equal than 300 then return IOTHUB_REGISTRYMANAGER_OK ] */
                    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_049: [ IoTHubRegistryManager_UpdateDevice shall verify the received HTTP status code and if it is less or equal than 300 then return IOTHUB_REGISTRYMANAGER_OK ] */
                    result = IOTHUB_REGISTRYMANAGER_OK;
                }
            }
        }
    }

    HTTPHeaders_Free(httpHeader);
    HTTPAPIEX_Destroy(httpExApiHandle);
    HTTPAPIEX_SAS_Destroy(httpExApiSasHandle);
    STRING_delete(keyName);
    STRING_delete(accessKey);
    STRING_delete(uriResource);
    return result;
}

static void free_registrymanager_handle(IOTHUB_REGISTRYMANAGER *registryManager)
{
    free(registryManager->hostname);
    free(registryManager->iothubName);
    free(registryManager->iothubSuffix);
    free(registryManager->sharedAccessKey);
    free(registryManager->deviceId);
    free(registryManager);
}

IOTHUB_REGISTRYMANAGER_HANDLE IoTHubRegistryManager_Create(IOTHUB_SERVICE_CLIENT_AUTH_HANDLE serviceClientHandle)
{
    IOTHUB_REGISTRYMANAGER_HANDLE result;

    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_001: [ If the serviceClientHandle input parameter is NULL IoTHubRegistryManager_Create shall return NULL ] */
    if (serviceClientHandle == NULL)
    {
        LogError("serviceClientHandle input parameter cannot be NULL");
        result = NULL;
    }
    else
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_084: [ If any member of the serviceClientHandle input parameter is NULL IoTHubRegistryManager_Create shall return NULL ] */
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
        else if ((serviceClientAuth->keyName == NULL) && (serviceClientAuth->deviceId == NULL))
        {
            LogError("authInfo->keyName and authInfo->deviceId input parameter cannot both be NULL");
            result = NULL;
        }
        else if (serviceClientAuth->sharedAccessKey == NULL)
        {
            LogError("authInfo->sharedAccessKey input parameter cannot be NULL");
            result = NULL;
        }
        else
        {
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_002: [ IoTHubRegistryManager_Create shall allocate memory for a new registry manager instance ] */
            result = malloc(sizeof(IOTHUB_REGISTRYMANAGER));
            if (result == NULL)
            {
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_003: [ If the allocation failed, IoTHubRegistryManager_Create shall return NULL ] */
                LogError("Malloc failed for IOTHUB_REGISTRYMANAGER");
            }
            else
            {
                memset(result, 0, sizeof(IOTHUB_REGISTRYMANAGER));

                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_004: [ If the allocation successful, IoTHubRegistryManager_Create shall create a IOTHUB_REGISTRYMANAGER_HANDLE from the given IOTHUB_REGISTRYMANAGER_AUTH_HANDLE and return with it ] */
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_085: [ IoTHubRegistryManager_Create shall allocate memory and copy hostName to result->hostName by calling mallocAndStrcpy_s. ] */
                if (mallocAndStrcpy_s(&result->hostname, serviceClientAuth->hostname) != 0)
                {
                    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_086: [ If the mallocAndStrcpy_s fails, IoTHubRegistryManager_Create shall do clean up and return NULL. ] */
                    LogError("mallocAndStrcpy_s failed for hostName");
                    free_registrymanager_handle(result);
                    result = NULL;
                }
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_087: [ IoTHubRegistryManager_Create shall allocate memory and copy iothubName to result->iothubName by calling mallocAndStrcpy_s. ] */
                else if (mallocAndStrcpy_s(&result->iothubName, serviceClientAuth->iothubName) != 0)
                {
                    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_088: [ If the mallocAndStrcpy_s fails, IoTHubRegistryManager_Create shall do clean up and return NULL. ] */
                    LogError("mallocAndStrcpy_s failed for iothubName");
                    free_registrymanager_handle(result);
                    result = NULL;
                }
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_089: [ IoTHubRegistryManager_Create shall allocate memory and copy iothubSuffix to result->iothubSuffix by calling mallocAndStrcpy_s. ] */
                else if (mallocAndStrcpy_s(&result->iothubSuffix, serviceClientAuth->iothubSuffix) != 0)
                {
                    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_090: [ If the mallocAndStrcpy_s fails, IoTHubRegistryManager_Create shall do clean up and return NULL. ] */
                    LogError("mallocAndStrcpy_s failed for iothubSuffix");
                    free_registrymanager_handle(result);
                    result = NULL;
                }
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_091: [ IoTHubRegistryManager_Create shall allocate memory and copy sharedAccessKey to result->sharedAccessKey by calling mallocAndStrcpy_s. ] */
                else if (mallocAndStrcpy_s(&result->sharedAccessKey, serviceClientAuth->sharedAccessKey) != 0)
                {
                    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_092: [ If the mallocAndStrcpy_s fails, IoTHubRegistryManager_Create shall do clean up and return NULL. ] */
                    LogError("mallocAndStrcpy_s failed for sharedAccessKey");
                    free_registrymanager_handle(result);
                    result = NULL;
                }
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_093: [ IoTHubRegistryManager_Create shall allocate memory and copy keyName to result->keyName by calling mallocAndStrcpy_s. ] */
                else if ((serviceClientAuth->keyName != NULL) && (mallocAndStrcpy_s(&result->keyName, serviceClientAuth->keyName) != 0))
                {
                    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_094: [ If the mallocAndStrcpy_s fails, IoTHubRegistryManager_Create shall do clean up and return NULL. ] */
                    LogError("mallocAndStrcpy_s failed for keyName");
                    free_registrymanager_handle(result);
                    result = NULL;
                }
                else if ((serviceClientAuth->deviceId != NULL) && (mallocAndStrcpy_s(&result->deviceId, serviceClientAuth->deviceId) != 0))
                {
                    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_094: [ If the mallocAndStrcpy_s fails, IoTHubRegistryManager_Create shall do clean up and return NULL. ] */
                    LogError("mallocAndStrcpy_s failed for deviceId");
                    free_registrymanager_handle(result);
                    result = NULL;
                }
            }
        }
    }
    return result;
}

void IoTHubRegistryManager_Destroy(IOTHUB_REGISTRYMANAGER_HANDLE registryManagerHandle)
{
    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_005: [ If the registryManagerHandle input parameter is NULL IoTHubRegistryManager_Destroy shall return ] */
    if (registryManagerHandle != NULL)
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_006 : [ If the registryManagerHandle input parameter is not NULL IoTHubRegistryManager_Destroy shall free the memory of it and return ] */
        IOTHUB_REGISTRYMANAGER* regManHandle = (IOTHUB_REGISTRYMANAGER*)registryManagerHandle;

        free(regManHandle->hostname);
        free(regManHandle->iothubName);
        free(regManHandle->iothubSuffix);
        free(regManHandle->sharedAccessKey);
        free(regManHandle->keyName);
        free(regManHandle->deviceId);
        free(regManHandle);
    }
}

static IOTHUB_REGISTRYMANAGER_RESULT IoTHubRegistryManager_CreateDeviceOrModule(IOTHUB_REGISTRYMANAGER_HANDLE registryManagerHandle, const IOTHUB_REGISTRY_DEVICE_OR_MODULE_CREATE* deviceOrModuleCreateInfo, IOTHUB_DEVICE_OR_MODULE* deviceOrModuleInfo)
{
    IOTHUB_REGISTRYMANAGER_RESULT result;

    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_007: [ IoTHubRegistryManager_CreateDevice shall verify the input parameters and if any of them are NULL then return IOTHUB_REGISTRYMANAGER_INVALID_ARG ] */
    if ((registryManagerHandle == NULL) || (deviceOrModuleCreateInfo == NULL) || (deviceOrModuleInfo == NULL))
    {
        LogError("Input parameter cannot be NULL");
        result = IOTHUB_REGISTRYMANAGER_INVALID_ARG;
    }
    else
    {
        initializeDeviceOrModuleInfoMembers(deviceOrModuleInfo);

        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_008: [ IoTHubRegistryManager_CreateDevice shall verify the deviceOrModuleCreateInfo->deviceId input parameter and if it is NULL then return IOTHUB_REGISTRYMANAGER_INVALID_ARG ] */
        if (deviceOrModuleCreateInfo->deviceId == NULL)
        {
            LogError("deviceId cannot be NULL");
            result = IOTHUB_REGISTRYMANAGER_INVALID_ARG;
        }
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_009: [ IoTHubRegistryManager_CreateDevice shall verify the deviceOrModuleCreateInfo->deviceId input parameter and if it contains space(s) then return IOTHUB_REGISTRYMANAGER_INVALID_ARG ] */
        else if ((strHasNoWhitespace(deviceOrModuleCreateInfo->deviceId)) != 0)
        {
            LogError("deviceId cannot contain spaces");
            result = IOTHUB_REGISTRYMANAGER_INVALID_ARG;
        }
        else if (isAuthTypeAllowed(deviceOrModuleCreateInfo->authMethod) == false)
        {
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_06_006: [ IoTHubRegistryManager_CreateDevice shall cleanup and return IOTHUB_REGISTRYMANAGER_INVALID_ARG if deviceUpdate->authMethod is not "IOTHUB_REGISTRYMANAGER_AUTH_SPK" or "IOTHUB_REGISTRYMANAGER_AUTH_X509_THUMBPRINT" ] */
            LogError("Invalid authorization type specified");
            result = IOTHUB_REGISTRYMANAGER_INVALID_ARG;
        }
        else
        {
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_095: [ IoTHubRegistryManager_CreateDevice shall allocate memory for device info structure by calling malloc ] */
            IOTHUB_DEVICE_OR_MODULE* tempDeviceOrModuleInfo;
            if ((tempDeviceOrModuleInfo = malloc(sizeof(IOTHUB_DEVICE_OR_MODULE))) == NULL)
            {
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_096 : [ If the malloc fails, IoTHubRegistryManager_Create shall do clean up and return IOTHUB_REGISTRYMANAGER_ERROR. ] */
                LogError("Malloc failed for tempDeviceOrModuleInfo");
                result = IOTHUB_REGISTRYMANAGER_ERROR;
            }
            else
            {
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_011: [ IoTHubRegistryManager_CreateDevice shall set the "deviceId" value to the deviceOrModuleCreateInfo->deviceId ] */
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_012: [ IoTHubRegistryManager_CreateDevice shall set the "symmetricKey" value to deviceOrModuleCreateInfo->primaryKey and deviceOrModuleCreateInfo->secondaryKey ] */
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_117: [ IoTHubRegistryManager_CreateDevice shall set the "status" value to the IOTHUB_DEVICE_STATUS_ENABLED ] */
                memset(tempDeviceOrModuleInfo, 0, sizeof(*tempDeviceOrModuleInfo));
                tempDeviceOrModuleInfo->deviceId = deviceOrModuleCreateInfo->deviceId;
                tempDeviceOrModuleInfo->moduleId = deviceOrModuleCreateInfo->moduleId;
                tempDeviceOrModuleInfo->primaryKey = deviceOrModuleCreateInfo->primaryKey;
                tempDeviceOrModuleInfo->secondaryKey = deviceOrModuleCreateInfo->secondaryKey;
                tempDeviceOrModuleInfo->authMethod = deviceOrModuleCreateInfo->authMethod;
                tempDeviceOrModuleInfo->status = IOTHUB_DEVICE_STATUS_ENABLED;
                tempDeviceOrModuleInfo->iotEdge_capable = deviceOrModuleCreateInfo->iotEdge_capable;
                tempDeviceOrModuleInfo->managedBy = deviceOrModuleCreateInfo->managedBy;

                BUFFER_HANDLE deviceJsonBuffer = NULL;
                BUFFER_HANDLE responseBuffer = NULL;

                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_010: [ IoTHubRegistryManager_CreateDevice shall create a flat "key1:value2,key2:value2..." JSON representation from the given deviceOrModuleCreateInfo parameter using the following parson APIs: json_value_init_object, json_value_get_object, json_object_set_string, json_object_dotset_string ] */
                if ((deviceJsonBuffer = constructDeviceOrModuleJson(tempDeviceOrModuleInfo)) == NULL)
                {
                    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_013: [ IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR if the JSON creation failed  ] */
                    LogError("Json creation failed");
                    result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
                }
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_097: [ IoTHubRegistryManager_CreateDevice shall allocate memory for response buffer by calling BUFFER_new ] */
                else if ((responseBuffer = BUFFER_new()) == NULL)
                {
                    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_098 : [ If the BUFFER_new fails, IoTHubRegistryManager_CreateDevice shall do clean up and return IOTHUB_REGISTRYMANAGER_ERROR. ] */
                    LogError("BUFFER_new failed for responseBuffer");
                    result = IOTHUB_REGISTRYMANAGER_ERROR;
                }
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_014: [ IoTHubRegistryManager_CreateDevice shall create an HTTP PUT request using the created JSON ] */
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_015: [ IoTHubRegistryManager_CreateDevice shall create an HTTP PUT request using the following HTTP headers: authorization=sasToken,Request-Id=1001,Accept=application/json,Content-Type=application/json,charset=utf-8 ] */
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_016: [ IoTHubRegistryManager_CreateDevice shall create an HTTPAPIEX_SAS_HANDLE handle by calling HTTPAPIEX_SAS_Create ] */
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_017: [ IoTHubRegistryManager_CreateDevice shall create an HTTPAPIEX_HANDLE handle by calling HTTPAPIEX_Create ] */
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_018: [ IoTHubRegistryManager_CreateDevice shall execute the HTTP PUT request by calling HTTPAPIEX_ExecuteRequest ] */
                else if ((result = sendHttpRequestCRUD(registryManagerHandle, IOTHUB_REQUEST_CREATE, deviceOrModuleCreateInfo->deviceId, deviceOrModuleCreateInfo->moduleId, deviceJsonBuffer, 0, responseBuffer)) == IOTHUB_REGISTRYMANAGER_ERROR)
                {
                    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_019: [ If any of the HTTPAPI call fails IoTHubRegistryManager_CreateDevice shall fail and return IOTHUB_REGISTRYMANAGER_HTTPAPI_ERROR ] */
                    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_099: [ If any of the call fails during the HTTP creation IoTHubRegistryManager_CreateDevice shall fail and return IOTHUB_REGISTRYMANAGER_ERROR ] */
                    LogError("Failure sending HTTP request for create device");
                }
                else if (result == IOTHUB_REGISTRYMANAGER_OK)
                {
                    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_022: [ IoTHubRegistryManager_CreateDevice shall verify the received HTTP status code and if it is less or equal than 300 then try to parse the response JSON to deviceOrModuleInfo ] */
                    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_023: [ If the JSON parsing failed, IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
                    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_024: [ If the deviceOrModuleInfo out parameter is not NULL IoTHubRegistryManager_CreateDevice shall save the received deviceOrModuleInfo to the out parameter and return IOTHUB_REGISTRYMANAGER_OK ] */
                    result = parseDeviceOrModuleJson(responseBuffer, deviceOrModuleInfo);
                }
                else
                {
                    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_020: [ IoTHubRegistryManager_CreateDevice shall verify the received HTTP status code and if it is 409 then return IOTHUB_REGISTRYMANAGER_DEVICE_EXIST ] */
                }

                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_100: [ IoTHubRegistryManager_CreateDevice shall do clean up before return ] */
                if (responseBuffer != NULL)
                {
                    BUFFER_delete(responseBuffer);
                }
                if (deviceJsonBuffer != NULL)
                {
                    BUFFER_delete(deviceJsonBuffer);
                }
            }
            free(tempDeviceOrModuleInfo);
        }
    }
    return result;
}

IOTHUB_REGISTRYMANAGER_RESULT IoTHubRegistryManager_CreateDevice(IOTHUB_REGISTRYMANAGER_HANDLE registryManagerHandle, const IOTHUB_REGISTRY_DEVICE_CREATE* deviceCreateInfo, IOTHUB_DEVICE* deviceInfo)
{
    IOTHUB_REGISTRYMANAGER_RESULT result;

    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_007: [ IoTHubRegistryManager_CreateDevice shall verify the input parameters and if any of them are NULL then return IOTHUB_REGISTRYMANAGER_INVALID_ARG ] */
    if ((registryManagerHandle == NULL) || (deviceCreateInfo == NULL) || (deviceInfo == NULL))
    {
        LogError("Input parameter cannot be NULL");
        result = IOTHUB_REGISTRYMANAGER_INVALID_ARG;
    }
    else
    {
        IOTHUB_REGISTRY_DEVICE_OR_MODULE_CREATE deviceOrModuleCreateInfo;
        IOTHUB_DEVICE_OR_MODULE deviceOrModuleInfo;

        memset(&deviceOrModuleInfo, 0, sizeof(deviceOrModuleInfo));
        memset(&deviceOrModuleCreateInfo, 0, sizeof(deviceOrModuleCreateInfo));
        deviceOrModuleCreateInfo.type = IOTHUB_REGISTRYMANAGER_MODEL_TYPE_DEVICE;
        deviceOrModuleCreateInfo.deviceId = deviceCreateInfo->deviceId;
        deviceOrModuleCreateInfo.primaryKey = deviceCreateInfo->primaryKey;
        deviceOrModuleCreateInfo.secondaryKey = deviceCreateInfo->secondaryKey;
        deviceOrModuleCreateInfo.authMethod = deviceCreateInfo->authMethod;
        deviceOrModuleCreateInfo.iotEdge_capable = false; //IOTHUB_DEVICE does not have this field, so set it to disabled

        result = IoTHubRegistryManager_CreateDeviceOrModule(registryManagerHandle, &deviceOrModuleCreateInfo, &deviceOrModuleInfo);
        if (result == IOTHUB_REGISTRYMANAGER_OK)
        {
            move_deviceOrModule_members_to_device(&deviceOrModuleInfo, deviceInfo);
            free_nonDevice_members_from_deviceOrModule(&deviceOrModuleInfo);
        }
    }

    return result;
}

IOTHUB_REGISTRYMANAGER_RESULT IoTHubRegistryManager_CreateDevice_Ex(IOTHUB_REGISTRYMANAGER_HANDLE registryManagerHandle, const IOTHUB_REGISTRY_DEVICE_CREATE_EX* deviceCreateInfo, IOTHUB_DEVICE_EX* deviceInfo)
{
    IOTHUB_REGISTRYMANAGER_RESULT result;

    if ((registryManagerHandle == NULL) || (deviceCreateInfo == NULL) || (deviceInfo == NULL))
    {
        LogError("Input parameter cannot be NULL");
        result = IOTHUB_REGISTRYMANAGER_INVALID_ARG;
    }
    else if ((deviceCreateInfo->version < IOTHUB_REGISTRY_DEVICE_CREATE_EX_VERSION_1) ||
            (deviceCreateInfo->version > IOTHUB_REGISTRY_DEVICE_CREATE_EX_VERSION_LATEST))
    {
        LogError("deviceCreateInfo must have a valid version");
        result = IOTHUB_REGISTRYMANAGER_INVALID_VERSION;
    }
    else if ((deviceInfo->version < IOTHUB_DEVICE_EX_VERSION_1) ||
            (deviceInfo->version > IOTHUB_DEVICE_EX_VERSION_LATEST))
    {
        LogError("deviceInfo must have a valid version");
        result = IOTHUB_REGISTRYMANAGER_INVALID_VERSION;
    }
    else
    {
        IOTHUB_REGISTRY_DEVICE_OR_MODULE_CREATE deviceOrModuleCreateInfo;
        IOTHUB_DEVICE_OR_MODULE deviceOrModuleInfo;

        memset(&deviceOrModuleInfo, 0, sizeof(deviceOrModuleInfo));
        memset(&deviceOrModuleCreateInfo, 0, sizeof(deviceOrModuleCreateInfo));
        deviceOrModuleCreateInfo.type = IOTHUB_REGISTRYMANAGER_MODEL_TYPE_DEVICE;

        if (deviceCreateInfo->version >= IOTHUB_REGISTRY_DEVICE_CREATE_EX_VERSION_1)
        {
            deviceOrModuleCreateInfo.deviceId = deviceCreateInfo->deviceId;
            deviceOrModuleCreateInfo.primaryKey = deviceCreateInfo->primaryKey;
            deviceOrModuleCreateInfo.secondaryKey = deviceCreateInfo->secondaryKey;
            deviceOrModuleCreateInfo.authMethod = deviceCreateInfo->authMethod;
            deviceOrModuleCreateInfo.iotEdge_capable = deviceCreateInfo->iotEdge_capable;
        }

        result = IoTHubRegistryManager_CreateDeviceOrModule(registryManagerHandle, &deviceOrModuleCreateInfo, &deviceOrModuleInfo);
        if (result == IOTHUB_REGISTRYMANAGER_OK)
        {
            move_deviceOrModule_members_to_deviceEx(&deviceOrModuleInfo, deviceInfo);
            free_nonDeviceEx_members_from_deviceOrModule(&deviceOrModuleInfo);
        }
    }

    return result;
}

IOTHUB_REGISTRYMANAGER_RESULT IoTHubRegistryManager_GetDeviceOrModule(IOTHUB_REGISTRYMANAGER_HANDLE registryManagerHandle, const char* deviceId, const char* moduleId, IOTHUB_DEVICE_OR_MODULE* deviceOrModuleInfo)
{
    IOTHUB_REGISTRYMANAGER_RESULT result;

    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_025: [ IoTHubRegistryManager_GetDevice shall verify the registryManagerHandle and deviceId input parameters and if any of them are NULL then return IOTHUB_REGISTRYMANAGER_INVALID_ARG ] */
    if ((registryManagerHandle == NULL) || (deviceId == NULL))
    {
        LogError("Input parameter cannot be NULL");
        result = IOTHUB_REGISTRYMANAGER_INVALID_ARG;
    }
    else
    {
        BUFFER_HANDLE responseBuffer;
        initializeDeviceOrModuleInfoMembers(deviceOrModuleInfo);

        if ((responseBuffer = BUFFER_new()) == NULL)
        {
            LogError("BUFFER_new failed for responseBuffer");
            result = IOTHUB_REGISTRYMANAGER_ERROR;
        }
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_026: [ IoTHubRegistryManager_GetDevice shall create HTTP GET request URL using the given deviceId using the following format: url/devices/[deviceId]?api-version=2017-06-30  ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_027: [ IoTHubRegistryManager_GetDevice shall add the following headers to the created HTTP GET request: authorization=sasToken,Request-Id=1001,Accept=application/json,Content-Type=application/json,charset=utf-8 ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_028: [ IoTHubRegistryManager_GetDevice shall create an HTTPAPIEX_SAS_HANDLE handle by calling HTTPAPIEX_SAS_Create ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_029: [ IoTHubRegistryManager_GetDevice shall create an HTTPAPIEX_HANDLE handle by calling HTTPAPIEX_Create ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_030: [ IoTHubRegistryManager_GetDevice shall execute the HTTP GET request by calling HTTPAPIEX_ExecuteRequest ] */
        else if ((result = sendHttpRequestCRUD(registryManagerHandle, IOTHUB_REQUEST_GET, deviceId, moduleId, NULL, 0, responseBuffer)) == IOTHUB_REGISTRYMANAGER_ERROR)
        {
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_031: [ If any of the HTTPAPI call fails IoTHubRegistryManager_GetDevice shall fail and return IOTHUB_REGISTRYMANAGER_ERROR ] */
            LogError("Failure sending HTTP request for create device");
        }
        else if (result == IOTHUB_REGISTRYMANAGER_OK)
        {
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_033: [ IoTHubRegistryManager_GetDevice shall verify the received HTTP status code and if it is less or equal than 300 then try to parse the response JSON to deviceInfo for the following properties: deviceId, primaryKey, secondaryKey, generationId, eTag, connectionState, connectionstateUpdatedTime, status, statusReason, statusUpdatedTime, lastActivityTime, cloudToDeviceMessageCount ] */
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_034: [ If any of the property field above missing from the JSON the property value will not be populated ] */
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_035: [ If the JSON parsing failed, IoTHubRegistryManager_GetDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_036: [ If the received JSON is empty, IoTHubRegistryManager_GetDevice shall return IOTHUB_REGISTRYMANAGER_DEVICE_NOT_EXIST ] */
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_037: [ If the deviceInfo out parameter if not NULL IoTHubRegistryManager_GetDevice shall save the received deviceInfo to the out parameter and return IOTHUB_REGISTRYMANAGER_OK ] */
            if ((result = parseDeviceOrModuleJson(responseBuffer, deviceOrModuleInfo)) == IOTHUB_REGISTRYMANAGER_OK)
            {
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_036: [ If the received JSON is empty, IoTHubRegistryManager_GetDevice shall return IOTHUB_REGISTRYMANAGER_DEVICE_NOT_EXIST ] */
                if (deviceOrModuleInfo->deviceId == NULL)
                {
                    free_deviceOrModule_members(deviceOrModuleInfo);
                    result = IOTHUB_REGISTRYMANAGER_DEVICE_NOT_EXIST;
                }
            }
        }

        BUFFER_delete(responseBuffer);
    }
    return result;

}

IOTHUB_REGISTRYMANAGER_RESULT IoTHubRegistryManager_GetDevice(IOTHUB_REGISTRYMANAGER_HANDLE registryManagerHandle, const char* deviceId, IOTHUB_DEVICE* deviceInfo)
{
    IOTHUB_REGISTRYMANAGER_RESULT result;

    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_007: [ IoTHubRegistryManager_CreateDevice shall verify the input parameters and if any of them are NULL then return IOTHUB_REGISTRYMANAGER_INVALID_ARG ] */
    if ((registryManagerHandle == NULL) || (deviceId == NULL) || (deviceInfo == NULL))
    {
        LogError("Input parameter cannot be NULL");
        result = IOTHUB_REGISTRYMANAGER_INVALID_ARG;
    }
    else
    {
        IOTHUB_DEVICE_OR_MODULE deviceOrModuleInfo;
        memset(&deviceOrModuleInfo, 0, sizeof(deviceOrModuleInfo));

        result = IoTHubRegistryManager_GetDeviceOrModule(registryManagerHandle, deviceId, NULL, &deviceOrModuleInfo);
        if (result == IOTHUB_REGISTRYMANAGER_OK)
        {
            move_deviceOrModule_members_to_device(&deviceOrModuleInfo, deviceInfo);
            free_nonDevice_members_from_deviceOrModule(&deviceOrModuleInfo);
        }
    }

    return result;
}

IOTHUB_REGISTRYMANAGER_RESULT IoTHubRegistryManager_GetDevice_Ex(IOTHUB_REGISTRYMANAGER_HANDLE registryManagerHandle, const char* deviceId, IOTHUB_DEVICE_EX* deviceInfo)
{
    IOTHUB_REGISTRYMANAGER_RESULT result;

    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_007: [ IoTHubRegistryManager_CreateDevice shall verify the input parameters and if any of them are NULL then return IOTHUB_REGISTRYMANAGER_INVALID_ARG ] */
    if ((registryManagerHandle == NULL) || (deviceId == NULL) || (deviceInfo == NULL))
    {
        LogError("Input parameter cannot be NULL");
        result = IOTHUB_REGISTRYMANAGER_INVALID_ARG;
    }
    else if ((deviceInfo->version < IOTHUB_DEVICE_EX_VERSION_1) ||
            (deviceInfo->version > IOTHUB_DEVICE_EX_VERSION_LATEST))
    {
        LogError("deviceInfo must have a valid version");
        result = IOTHUB_REGISTRYMANAGER_INVALID_VERSION;
    }
    else
    {
        IOTHUB_DEVICE_OR_MODULE deviceOrModuleInfo;
        memset(&deviceOrModuleInfo, 0, sizeof(deviceOrModuleInfo));

        result = IoTHubRegistryManager_GetDeviceOrModule(registryManagerHandle, deviceId, NULL, &deviceOrModuleInfo);
        if (result == IOTHUB_REGISTRYMANAGER_OK)
        {
            move_deviceOrModule_members_to_deviceEx(&deviceOrModuleInfo, deviceInfo);
            free_nonDeviceEx_members_from_deviceOrModule(&deviceOrModuleInfo);
        }
    }

    return result;
}

IOTHUB_REGISTRYMANAGER_RESULT IoTHubRegistryManager_UpdateDeviceOrModule(IOTHUB_REGISTRYMANAGER_HANDLE registryManagerHandle, IOTHUB_REGISTRY_DEVICE_OR_MODULE_UPDATE* deviceOrModuleUpdate)
{
    IOTHUB_REGISTRYMANAGER_RESULT result;

    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_038: [ IoTHubRegistryManager_UpdateDevice shall verify the input parameters and if any of them are NULL then return IOTHUB_REGISTRYMANAGER_INVALID_ARG ] */
    if ((registryManagerHandle == NULL) || (deviceOrModuleUpdate == NULL))
    {
        LogError("Input parameter cannot be NULL");
        result = IOTHUB_REGISTRYMANAGER_INVALID_ARG;
    }
    else
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_039: [ IoTHubRegistryManager_UpdateDevice shall verify the deviceCreateInfo->deviceId input parameter and if it is NULL then return IOTHUB_REGISTRYMANAGER_INVALID_ARG ] */
        if (deviceOrModuleUpdate->deviceId == NULL)
        {
            LogError("deviceId cannot be NULL");
            result = IOTHUB_REGISTRYMANAGER_INVALID_ARG;
        }
        else if (isAuthTypeAllowed(deviceOrModuleUpdate->authMethod) == false)
        {
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_06_005: [ IoTHubRegistryManager_UpdateDevice shall clean up and return IOTHUB_REGISTRYMANAGER_INVALID_ARG if deviceUpdate->authMethod is not "IOTHUB_REGISTRYMANAGER_AUTH_SPK" or "IOTHUB_REGISTRYMANAGER_AUTH_X509_THUMBPRINT" ] */
            LogError("Invalid authorization type specified");
            result = IOTHUB_REGISTRYMANAGER_INVALID_ARG;
        }
        else
        {
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_106: [ IoTHubRegistryManager_UpdateDevice shall allocate memory for device info structure by calling malloc ] */
            IOTHUB_DEVICE_OR_MODULE* tempDeviceOrModuleInfo;
            if ((tempDeviceOrModuleInfo = malloc(sizeof(IOTHUB_DEVICE_OR_MODULE))) == NULL)
            {
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_108: [ If the malloc fails, IoTHubRegistryManager_UpdateDevice shall do clean up and return NULL ] */
                LogError("Malloc failed for tempDeviceOrModuleInfo");
                result = IOTHUB_REGISTRYMANAGER_ERROR;
            }
            else
            {
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_118: [ IoTHubRegistryManager_CreateDevice shall set the "status" value to the deviceCreateInfo->status ] */
                (void)memset(tempDeviceOrModuleInfo, 0, sizeof(IOTHUB_DEVICE_OR_MODULE));
                tempDeviceOrModuleInfo->deviceId = deviceOrModuleUpdate->deviceId;
                tempDeviceOrModuleInfo->primaryKey = deviceOrModuleUpdate->primaryKey;
                tempDeviceOrModuleInfo->secondaryKey = deviceOrModuleUpdate->secondaryKey;
                tempDeviceOrModuleInfo->authMethod = deviceOrModuleUpdate->authMethod;
                tempDeviceOrModuleInfo->status = deviceOrModuleUpdate->status;
                tempDeviceOrModuleInfo->moduleId = deviceOrModuleUpdate->moduleId;
                tempDeviceOrModuleInfo->iotEdge_capable = deviceOrModuleUpdate->iotEdge_capable;
                tempDeviceOrModuleInfo->managedBy = deviceOrModuleUpdate->managedBy;

                BUFFER_HANDLE deviceJsonBuffer = NULL;
                BUFFER_HANDLE responseBuffer = NULL;

                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_041: [ IoTHubRegistryManager_UpdateDevice shall create a flat "key1:value2,key2:value2..." JSON representation from the given deviceCreateInfo parameter using the following parson APIs : json_value_init_object, json_value_get_object, json_object_set_string, json_object_dotset_string ] */
                if ((deviceJsonBuffer = constructDeviceOrModuleJson(tempDeviceOrModuleInfo)) == NULL)
                {
                    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_042: [ IoTHubRegistryManager_UpdateDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR if the JSON creation failed  ] */
                    LogError("Json creation failed");
                    result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
                }
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_101: [ IoTHubRegistryManager_UpdateDevice shall allocate memory for response buffer by calling BUFFER_new ] */
                else if ((responseBuffer = BUFFER_new()) == NULL)
                {
                    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_102: [ If the BUFFER_new fails, IoTHubRegistryManager_UpdateDevice shall do clean up and return IOTHUB_REGISTRYMANAGER_ERROR. ] */
                    LogError("BUFFER_new failed for responseBuffer");
                    result = IOTHUB_REGISTRYMANAGER_ERROR;
                }
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_043: [ IoTHubRegistryManager_UpdateDevice shall create an HTTP PUT request using the created JSON ] */
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_044: [ IoTHubRegistryManager_UpdateDevice shall create an HTTP PUT request using the createdfollowing HTTP headers : authorization = sasToken, Request - Id = 1001, Accept = application / json, Content - Type = application / json, charset = utf - 8 ] */
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_045: [ IoTHubRegistryManager_UpdateDevice shall create an HTTPAPIEX_SAS_HANDLE handle by calling HTTPAPIEX_SAS_Create ] */
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_046: [ IoTHubRegistryManager_UpdateDevice shall create an HTTPAPIEX_HANDLE handle by calling HTTPAPIEX_Create ] */
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_047: [ IoTHubRegistryManager_UpdateDevice shall execute the HTTP PUT request by calling HTTPAPIEX_ExecuteRequest ] */
                else if ((result = sendHttpRequestCRUD(registryManagerHandle, IOTHUB_REQUEST_UPDATE, deviceOrModuleUpdate->deviceId, deviceOrModuleUpdate->moduleId, deviceJsonBuffer, 0, responseBuffer)) == IOTHUB_REGISTRYMANAGER_ERROR)
                {
                    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_103: [ If any of the call fails during the HTTP creation IoTHubRegistryManager_UpdateDevice shall fail and return IOTHUB_REGISTRYMANAGER_ERROR ] */
                    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_104: [ If any of the HTTPAPI call fails IoTHubRegistryManager_UpdateDevice shall fail and return IOTHUB_REGISTRYMANAGER_HTTPAPI_ERROR ] */
                    LogError("Failure sending HTTP request for update device");
                }

                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_105: [ IoTHubRegistryManager_UpdateDevice shall do clean up before return ] */
                if (deviceJsonBuffer != NULL)
                {
                    BUFFER_delete(deviceJsonBuffer);
                }
                if (responseBuffer != NULL)
                {
                    BUFFER_delete(responseBuffer);
                }
            }
            free(tempDeviceOrModuleInfo);
        }
    }
    return result;

}

IOTHUB_REGISTRYMANAGER_RESULT IoTHubRegistryManager_UpdateDevice(IOTHUB_REGISTRYMANAGER_HANDLE registryManagerHandle, IOTHUB_REGISTRY_DEVICE_UPDATE* deviceUpdate)
{
    IOTHUB_REGISTRYMANAGER_RESULT result;

    if (registryManagerHandle == NULL || deviceUpdate == NULL)
    {
        LogError("Input parameter cannot be NULL");
        result = IOTHUB_REGISTRYMANAGER_INVALID_ARG;
    }
    else
    {
        IOTHUB_REGISTRY_DEVICE_OR_MODULE_UPDATE deviceOrModuleUpdate;
        memset(&deviceOrModuleUpdate, 0, sizeof(deviceOrModuleUpdate));

        //Convert to generic update struct
        deviceOrModuleUpdate.type = IOTHUB_REGISTRYMANAGER_MODEL_TYPE_DEVICE;
        deviceOrModuleUpdate.deviceId = deviceUpdate->deviceId;
        deviceOrModuleUpdate.primaryKey = deviceUpdate->primaryKey;
        deviceOrModuleUpdate.secondaryKey = deviceUpdate->secondaryKey;
        deviceOrModuleUpdate.status = deviceUpdate->status;
        deviceOrModuleUpdate.authMethod = deviceUpdate->authMethod;
        deviceOrModuleUpdate.iotEdge_capable = false;   //IOTHUB_REGISTRY_DEVICE_UPDATE does not have this field, so set false

        result = IoTHubRegistryManager_UpdateDeviceOrModule(registryManagerHandle, &deviceOrModuleUpdate);
    }

    return result;
}

IOTHUB_REGISTRYMANAGER_RESULT IoTHubRegistryManager_UpdateDevice_Ex(IOTHUB_REGISTRYMANAGER_HANDLE registryManagerHandle, IOTHUB_REGISTRY_DEVICE_UPDATE_EX* deviceUpdate)
{
    IOTHUB_REGISTRYMANAGER_RESULT result;

    if (registryManagerHandle == NULL || deviceUpdate == NULL)
    {
        LogError("Input parameter cannot be NULL");
        result = IOTHUB_REGISTRYMANAGER_INVALID_ARG;
    }
    else if (deviceUpdate->version < IOTHUB_REGISTRY_DEVICE_UPDATE_EX_VERSION_1 || deviceUpdate->version > IOTHUB_REGISTRY_DEVICE_UPDATE_EX_VERSION_LATEST)
    {
        LogError("deviceUpdate must have valid version");
        result = IOTHUB_REGISTRYMANAGER_INVALID_VERSION;
    }
    else
    {
        IOTHUB_REGISTRY_DEVICE_OR_MODULE_UPDATE deviceOrModuleUpdate;
        memset(&deviceOrModuleUpdate, 0, sizeof(deviceOrModuleUpdate));

        //Convert to generic update struct
        deviceOrModuleUpdate.type = IOTHUB_REGISTRYMANAGER_MODEL_TYPE_DEVICE;

        if (deviceUpdate->version >= IOTHUB_REGISTRY_DEVICE_UPDATE_EX_VERSION_1)
        {
            deviceOrModuleUpdate.deviceId = deviceUpdate->deviceId;
            deviceOrModuleUpdate.primaryKey = deviceUpdate->primaryKey;
            deviceOrModuleUpdate.secondaryKey = deviceUpdate->secondaryKey;
            deviceOrModuleUpdate.status = deviceUpdate->status;
            deviceOrModuleUpdate.authMethod = deviceUpdate->authMethod;
            deviceOrModuleUpdate.iotEdge_capable = deviceUpdate->iotEdge_capable;
        }

        result = IoTHubRegistryManager_UpdateDeviceOrModule(registryManagerHandle, &deviceOrModuleUpdate);
    }

    return result;
}

IOTHUB_REGISTRYMANAGER_RESULT IoTHubRegistryManager_DeleteDevice(IOTHUB_REGISTRYMANAGER_HANDLE registryManagerHandle, const char* deviceId)
{
    IOTHUB_REGISTRYMANAGER_RESULT result;

    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_052: [ IoTHubRegistryManager_DeleteDevice shall verify the input parameters and if any of them are NULL then return IOTHUB_REGISTRYMANAGER_INVALID_ARG ] */
    if ((registryManagerHandle == NULL) || (deviceId == NULL))
    {
        LogError("Input parameter cannot be NULL");
        result = IOTHUB_REGISTRYMANAGER_INVALID_ARG;
    }
    else
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_053: [ IoTHubRegistryManager_DeleteDevice shall create HTTP DELETE request URL using the given deviceId using the following format : url / devices / [deviceId] ? api - version  ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_054: [ IoTHubRegistryManager_DeleteDevice shall add the following headers to the created HTTP GET request : authorization = sasToken, Request - Id = 1001, Accept = application / json, Content - Type = application / json, charset = utf - 8 ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_055: [ IoTHubRegistryManager_DeleteDevice shall create an HTTPAPIEX_SAS_HANDLE handle by calling HTTPAPIEX_SAS_Create ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_056: [ IoTHubRegistryManager_DeleteDevice shall create an HTTPAPIEX_HANDLE handle by calling HTTPAPIEX_Create ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_057: [ IoTHubRegistryManager_DeleteDevice shall execute the HTTP DELETE request by calling HTTPAPIEX_ExecuteRequest ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_058: [ IoTHubRegistryManager_DeleteDevice shall verify the received HTTP status code and if it is greater than 300 then return IOTHUB_REGISTRYMANAGER_HTTP_STATUS_ERROR ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_059: [ IoTHubRegistryManager_DeleteDevice shall verify the received HTTP status code and if it is less or equal than 300 then return IOTHUB_REGISTRYMANAGER_OK ] */
        result = sendHttpRequestCRUD(registryManagerHandle, IOTHUB_REQUEST_DELETE, deviceId, NULL, NULL, 0, NULL);
    }
    return result;
}

IOTHUB_REGISTRYMANAGER_RESULT IoTHubRegistryManager_GetModuleOrDeviceList(IOTHUB_REGISTRYMANAGER_HANDLE registryManagerHandle, const char* deviceId, size_t numberOfDevices, SINGLYLINKEDLIST_HANDLE deviceOrModuleList, IOTHUB_REGISTRYMANAGER_MODEL_TYPE struct_type, int struct_version)
{
    IOTHUB_REGISTRYMANAGER_RESULT result;

    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_060: [ IoTHubRegistryManager_GetDeviceList shall verify the input parameters and if any of them are NULL then return IOTHUB_REGISTRYMANAGER_INVALID_ARG ] */
    if ((registryManagerHandle == NULL) || (deviceOrModuleList == NULL))
    {
        LogError("Input parameter cannot be NULL");
        result = IOTHUB_REGISTRYMANAGER_INVALID_ARG;
    }
    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_061: [ IoTHubRegistryManager_GetDeviceList shall verify if the numberOfDevices input parameter is between 1 and 1000 and if it is not then return IOTHUB_REGISTRYMANAGER_INVALID_ARG ] */
    else if ((numberOfDevices == 0) || (numberOfDevices > IOTHUB_DEVICES_MAX_REQUEST))
    {
        LogError("numberOfDevices has to be between 1 and 1000");
        result = IOTHUB_REGISTRYMANAGER_INVALID_ARG;
    }
    else if ((struct_type == IOTHUB_REGISTRYMANAGER_MODEL_TYPE_MODULE) && ((struct_version < IOTHUB_MODULE_VERSION_1) || (struct_version > IOTHUB_MODULE_VERSION_LATEST)))
    {
        LogError("Invalid module version");
        result = IOTHUB_REGISTRYMANAGER_INVALID_VERSION;
    }
    else
    {
        BUFFER_HANDLE responseBuffer = NULL;

        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_109: [ IoTHubRegistryManager_GetDeviceList shall allocate memory for response buffer by calling BUFFER_new ] */
        if ((responseBuffer = BUFFER_new()) == NULL)
        {
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_110: [ If the BUFFER_new fails, IoTHubRegistryManager_GetDeviceList shall do clean up and return NULL ] */
            LogError("BUFFER_new failed for responseBuffer");
            result = IOTHUB_REGISTRYMANAGER_ERROR;
        }
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_062: [ IoTHubRegistryManager_GetDeviceList shall create HTTP GET request for numberOfDevices using the follwoing format: url/devices/?top=[numberOfDevices]&api-version ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_063: [ IoTHubRegistryManager_GetDeviceList shall add the following headers to the created HTTP GET request: authorization=sasToken,Request-Id=1001,Accept=application/json,Content-Type=application/json,charset=utf-8 ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_064: [ IoTHubRegistryManager_GetDeviceList shall create an HTTPAPIEX_SAS_HANDLE handle by calling HTTPAPIEX_SAS_Create ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_065: [ IoTHubRegistryManager_GetDeviceList shall create an HTTPAPIEX_HANDLE handle by calling HTTPAPIEX_Create ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_066: [ IoTHubRegistryManager_GetDeviceList shall execute the HTTP GET request by calling HTTPAPIEX_ExecuteRequest ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_067: [ IoTHubRegistryManager_GetDeviceList shall verify the received HTTP status code and if it is greater than 300 then return IOTHUB_REGISTRYMANAGER_ERROR ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_068: [ IoTHubRegistryManager_GetDeviceList shall verify the received HTTP status code and if it is less or equal than 300 then try to parse the response JSON to deviceList ] */
        else if ((result = sendHttpRequestCRUD(registryManagerHandle, IOTHUB_REQUEST_GET_DEVICE_LIST, deviceId, NULL, NULL, numberOfDevices, responseBuffer)) == IOTHUB_REGISTRYMANAGER_ERROR)
        {
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_115: [ If any of the HTTPAPI call fails IoTHubRegistryManager_GetDeviceList shall fail and return IOTHUB_REGISTRYMANAGER_ERROR ] */
            LogError("Failure sending HTTP request for get device list");
        }
        else if (result == IOTHUB_REGISTRYMANAGER_OK)
        {
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_069: [ IoTHubRegistryManager_GetDeviceList shall use the following parson APIs to parse the response JSON: json_parse_string, json_value_get_object, json_object_get_string, json_object_dotget_string  ] */
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_070: [ If any of the parson API fails, IoTHubRegistryManager_GetDeviceList shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_071: [ IoTHubRegistryManager_GetDeviceList shall populate the deviceList parameter with structures of type "IOTHUB_DEVICE" ] */
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_072: [ If populating the deviceList parameter fails IoTHubRegistryManager_GetDeviceList shall return IOTHUB_REGISTRYMANAGER_ERROR ] */
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_073: [ If populating the deviceList parameter successful IoTHubRegistryManager_GetDeviceList shall return IOTHUB_REGISTRYMANAGER_OK ] */
            result = parseDeviceOrModuleListJson(responseBuffer, deviceOrModuleList, struct_type, struct_version);
        }

        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_111: [ IoTHubRegistryManager_GetDeviceList shall do clean up before return ] */
        if (responseBuffer != NULL)
        {
            BUFFER_delete(responseBuffer);
        }
    }
    return result;
}

/* DEPRECATED: IoTHubRegistryManager_GetDeviceList is deprecated and may be removed from a future release. */
IOTHUB_REGISTRYMANAGER_RESULT IoTHubRegistryManager_GetDeviceList(IOTHUB_REGISTRYMANAGER_HANDLE registryManagerHandle, size_t numberOfDevices, SINGLYLINKEDLIST_HANDLE deviceList)
{
    return IoTHubRegistryManager_GetModuleOrDeviceList(registryManagerHandle, NULL, numberOfDevices, deviceList, IOTHUB_REGISTRYMANAGER_MODEL_TYPE_DEVICE, 0);
}

IOTHUB_REGISTRYMANAGER_RESULT IoTHubRegistryManager_GetStatistics(IOTHUB_REGISTRYMANAGER_HANDLE registryManagerHandle, IOTHUB_REGISTRY_STATISTICS* registryStatistics)
{
    IOTHUB_REGISTRYMANAGER_RESULT result;

    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_074: [ IoTHubRegistryManager_GetStatistics shall verify the input parameters and if any of them are NULL then return IOTHUB_REGISTRYMANAGER_INVALID_ARG ] */
    if ((registryManagerHandle == NULL) || (registryStatistics == NULL))
    {
        LogError("Input parameter cannot be NULL");
        result = IOTHUB_REGISTRYMANAGER_INVALID_ARG;
    }
    else
    {
        BUFFER_HANDLE responseBuffer;

        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_112: [ IoTHubRegistryManager_GetStatistics shall allocate memory for response buffer by calling BUFFER_new ] */
        if ((responseBuffer = BUFFER_new()) == NULL)
        {
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_113: [ If the BUFFER_new fails, IoTHubRegistryManager_GetStatistics shall do clean up and return NULL ] */
            LogError("BUFFER_new failed for responseBuffer");
            result = IOTHUB_REGISTRYMANAGER_ERROR;
        }
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_075: [ IoTHubRegistryManager_GetStatistics shall create HTTP GET request for statistics using the following format: url/statistics/devices?api-version ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_076: [ IoTHubRegistryManager_GetStatistics shall add the following headers to the created HTTP GET request: authorization=sasToken,Request-Id=1001,Accept=application/json,Content-Type=application/json,charset=utf-8 ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_077: [ IoTHubRegistryManager_GetStatistics shall create an HTTPAPIEX_SAS_HANDLE handle by calling HTTPAPIEX_SAS_Create ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_078: [ IoTHubRegistryManager_GetStatistics shall create an HTTPAPIEX_HANDLE handle by calling HTTPAPIEX_Create ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_079: [ IoTHubRegistryManager_GetStatistics shall execute the HTTP GET request by calling HTTPAPIEX_ExecuteRequest ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_080: [ IoTHubRegistryManager_GetStatistics shall verify the received HTTP status code and if it is greater than 300 then return IOTHUB_REGISTRYMANAGER_ERROR ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_081: [ IoTHubRegistryManager_GetStatistics shall verify the received HTTP status code and if it is less or equal than 300 then use the following parson APIs to parse the response JSON to registry statistics structure: json_parse_string, json_value_get_object, json_object_get_string, json_object_dotget_string ] */
        else if ((result = sendHttpRequestCRUD(registryManagerHandle, IOTHUB_REQUEST_GET_STATISTICS, NULL, NULL, NULL, 0, responseBuffer)) == IOTHUB_REGISTRYMANAGER_ERROR)
        {
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_116: [ If any of the HTTPAPI call fails IoTHubRegistryManager_GetStatistics shall fail and return IOTHUB_REGISTRYMANAGER_ERROR ] */
            LogError("Failure sending HTTP request for get registry statistics");
        }
        else if (result == IOTHUB_REGISTRYMANAGER_OK)
        {
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_082: [ If the parsing failed, IoTHubRegistryManager_GetStatistics shall return IOTHUB_REGISTRYMANAGER_ERROR ] */
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_083: [ IoTHubRegistryManager_GetStatistics shall save the registry statistics to the out value and return IOTHUB_REGISTRYMANAGER_OK ] */
            result = parseStatisticsJson(responseBuffer, registryStatistics);
        }

        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_114: [ IoTHubRegistryManager_GetStatistics shall do clean up before return ] */
        if (responseBuffer != NULL)
        {
            BUFFER_delete(responseBuffer);
        }
    }
    return result;
}

IOTHUB_REGISTRYMANAGER_RESULT IoTHubRegistryManager_CreateModule(IOTHUB_REGISTRYMANAGER_HANDLE registryManagerHandle, const IOTHUB_REGISTRY_MODULE_CREATE* moduleCreate, IOTHUB_MODULE* module)
{
    IOTHUB_REGISTRYMANAGER_RESULT result;

    if ((registryManagerHandle == NULL) || (module == NULL))
    {
        LogError("Input parameter cannot be NULL");
        result = IOTHUB_REGISTRYMANAGER_INVALID_ARG;
    }
    else if ((moduleCreate == NULL) || (moduleCreate->moduleId == NULL))
    {
        LogError("moduleId cannot be NULL");
        result = IOTHUB_REGISTRYMANAGER_INVALID_ARG;
    }
    else if (moduleCreate->version < IOTHUB_REGISTRY_MODULE_CREATE_VERSION_1 || moduleCreate->version > IOTHUB_REGISTRY_MODULE_CREATE_VERSION_LATEST)
    {
        LogError("moduleCreate must have a valid version");
        result = IOTHUB_REGISTRYMANAGER_INVALID_VERSION;
    }
    else if (module->version < IOTHUB_MODULE_VERSION_1 || module->version > IOTHUB_MODULE_VERSION_LATEST)
    {
        LogError("module must have a valid version");
        result = IOTHUB_REGISTRYMANAGER_INVALID_VERSION;
    }
    else
    {
        IOTHUB_REGISTRY_DEVICE_OR_MODULE_CREATE deviceOrModuleCreateInfo;
        IOTHUB_DEVICE_OR_MODULE deviceOrModuleInfo;
        memset(&deviceOrModuleCreateInfo, 0, sizeof(deviceOrModuleCreateInfo));
        memset(&deviceOrModuleInfo, 0, sizeof(deviceOrModuleInfo));

        //Convert to generic create struct
        deviceOrModuleCreateInfo.type = IOTHUB_REGISTRYMANAGER_MODEL_TYPE_MODULE;
        if (moduleCreate->version >= IOTHUB_REGISTRY_MODULE_CREATE_VERSION_1)
        {
            deviceOrModuleCreateInfo.deviceId = moduleCreate->deviceId;
            deviceOrModuleCreateInfo.primaryKey = moduleCreate->primaryKey;
            deviceOrModuleCreateInfo.secondaryKey = moduleCreate->secondaryKey;
            deviceOrModuleCreateInfo.authMethod = moduleCreate->authMethod;
            deviceOrModuleCreateInfo.moduleId = moduleCreate->moduleId;
            deviceOrModuleCreateInfo.managedBy = moduleCreate->managedBy;
        }

        result = IoTHubRegistryManager_CreateDeviceOrModule(registryManagerHandle, &deviceOrModuleCreateInfo, &deviceOrModuleInfo);
        if (result == IOTHUB_REGISTRYMANAGER_OK)
        {
            move_deviceOrModule_members_to_module(&deviceOrModuleInfo, module);
        }
    }

    return result;
}

IOTHUB_REGISTRYMANAGER_RESULT IoTHubRegistryManager_GetModule(IOTHUB_REGISTRYMANAGER_HANDLE registryManagerHandle, const char* deviceId, const char* moduleId, IOTHUB_MODULE* module)
{
    IOTHUB_REGISTRYMANAGER_RESULT result;

    if ((registryManagerHandle == NULL) || (deviceId == NULL) || (moduleId == NULL) || (module == NULL))
    {
        LogError("Input parameter cannot be NULL");
        result = IOTHUB_REGISTRYMANAGER_INVALID_ARG;
    }
    else if (module->version < IOTHUB_MODULE_VERSION_1 || module->version > IOTHUB_MODULE_VERSION_LATEST)
    {
        LogError("module must have a valid version");
        result = IOTHUB_REGISTRYMANAGER_INVALID_VERSION;
    }
    else
    {
        IOTHUB_DEVICE_OR_MODULE deviceOrModuleInfo;

        result = IoTHubRegistryManager_GetDeviceOrModule(registryManagerHandle, deviceId, moduleId, &deviceOrModuleInfo);
        if (deviceOrModuleInfo.moduleId == NULL)
        {
            result = IOTHUB_REGISTRYMANAGER_DEVICE_NOT_EXIST;
        }
        else if (result == IOTHUB_REGISTRYMANAGER_OK)
        {
            move_deviceOrModule_members_to_module(&deviceOrModuleInfo, module);
        }
    }

    return result;
}

IOTHUB_REGISTRYMANAGER_RESULT IoTHubRegistryManager_UpdateModule(IOTHUB_REGISTRYMANAGER_HANDLE registryManagerHandle, IOTHUB_REGISTRY_MODULE_UPDATE* moduleUpdate)
{
    IOTHUB_REGISTRYMANAGER_RESULT result;

    if (registryManagerHandle == NULL)
    {
        LogError("Input parameter cannot be NULL");
        result = IOTHUB_REGISTRYMANAGER_INVALID_ARG;
    }
    else if ((moduleUpdate == NULL) || (moduleUpdate->moduleId == NULL))
    {
        LogError("moduleId cannot be NULL");
        result = IOTHUB_REGISTRYMANAGER_INVALID_ARG;
    }
    else if (moduleUpdate->version < IOTHUB_REGISTRY_MODULE_UPDATE_VERSION_1 || moduleUpdate->version > IOTHUB_REGISTRY_MODULE_UPDATE_VERSION_LATEST)
    {
        LogError("moduleUpdate must have valid version");
        result = IOTHUB_REGISTRYMANAGER_INVALID_VERSION;
    }
    else
    {
        IOTHUB_REGISTRY_DEVICE_OR_MODULE_UPDATE deviceOrModuleUpdate;
        memset(&deviceOrModuleUpdate, 0, sizeof(deviceOrModuleUpdate));

        //Convert to generic update struct
        deviceOrModuleUpdate.type = IOTHUB_REGISTRYMANAGER_MODEL_TYPE_MODULE;
        if (moduleUpdate-> version >= IOTHUB_REGISTRY_MODULE_UPDATE_VERSION_1)
        {
            deviceOrModuleUpdate.deviceId = moduleUpdate->deviceId;
            deviceOrModuleUpdate.primaryKey = moduleUpdate->primaryKey;
            deviceOrModuleUpdate.secondaryKey = moduleUpdate->secondaryKey;
            deviceOrModuleUpdate.status = moduleUpdate->status;
            deviceOrModuleUpdate.authMethod = moduleUpdate->authMethod;
            deviceOrModuleUpdate.moduleId = moduleUpdate->moduleId;
            deviceOrModuleUpdate.managedBy = moduleUpdate->managedBy;
        }

        result = IoTHubRegistryManager_UpdateDeviceOrModule(registryManagerHandle, &deviceOrModuleUpdate);
    }

    return result;
}

IOTHUB_REGISTRYMANAGER_RESULT IoTHubRegistryManager_DeleteModule(IOTHUB_REGISTRYMANAGER_HANDLE registryManagerHandle, const char* deviceId, const char* moduleId)
{
    IOTHUB_REGISTRYMANAGER_RESULT result;

    if ((registryManagerHandle == NULL) || (deviceId == NULL) || (moduleId == NULL))
    {
        LogError("Input parameter cannot be NULL");
        result = IOTHUB_REGISTRYMANAGER_INVALID_ARG;
    }
    else
    {
        result = sendHttpRequestCRUD(registryManagerHandle, IOTHUB_REQUEST_DELETE, deviceId, moduleId, NULL, 0, NULL);
    }
    return result;
}

IOTHUB_REGISTRYMANAGER_RESULT IoTHubRegistryManager_GetModuleList(IOTHUB_REGISTRYMANAGER_HANDLE registryManagerHandle, const char* deviceId, SINGLYLINKEDLIST_HANDLE moduleList, int module_version)
{
    return IoTHubRegistryManager_GetModuleOrDeviceList(registryManagerHandle, deviceId, IOTHUB_DEVICES_MAX_REQUEST, moduleList, IOTHUB_REGISTRYMANAGER_MODEL_TYPE_MODULE, module_version);
}

