// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include "umock_c/umock_c_prod.h"
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/xlogging.h"

#include "iothub_client_core.h"
#include "iothub_module_client.h"

IOTHUB_MODULE_CLIENT_HANDLE IoTHubModuleClient_CreateFromConnectionString(const char* connectionString, IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol)
{
    return (IOTHUB_MODULE_CLIENT_HANDLE)IoTHubClientCore_CreateFromConnectionString(connectionString, protocol);
}

void IoTHubModuleClient_Destroy(IOTHUB_MODULE_CLIENT_HANDLE iotHubModuleClientHandle)
{
    IoTHubClientCore_Destroy((IOTHUB_CLIENT_CORE_HANDLE)iotHubModuleClientHandle);
}

IOTHUB_CLIENT_RESULT IoTHubModuleClient_SendEventAsync(IOTHUB_MODULE_CLIENT_HANDLE iotHubModuleClientHandle, IOTHUB_MESSAGE_HANDLE eventMessageHandle, IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK eventConfirmationCallback, void* userContextCallback)
{
    return IoTHubClientCore_SendEventAsync((IOTHUB_CLIENT_CORE_HANDLE)iotHubModuleClientHandle, eventMessageHandle, eventConfirmationCallback, userContextCallback);
}

IOTHUB_CLIENT_RESULT IoTHubModuleClient_GetSendStatus(IOTHUB_MODULE_CLIENT_HANDLE iotHubModuleClientHandle, IOTHUB_CLIENT_STATUS *iotHubClientStatus)
{
    return IoTHubClientCore_GetSendStatus((IOTHUB_CLIENT_CORE_HANDLE)iotHubModuleClientHandle, iotHubClientStatus);
}

IOTHUB_CLIENT_RESULT IoTHubModuleClient_SetMessageCallback(IOTHUB_MODULE_CLIENT_HANDLE iotHubModuleClientHandle, IOTHUB_CLIENT_MESSAGE_CALLBACK_ASYNC messageCallback, void* userContextCallback)
{
    return IoTHubClientCore_SetInputMessageCallback((IOTHUB_CLIENT_CORE_HANDLE)iotHubModuleClientHandle, NULL, messageCallback, userContextCallback);}

IOTHUB_CLIENT_RESULT IoTHubModuleClient_SetConnectionStatusCallback(IOTHUB_MODULE_CLIENT_HANDLE iotHubModuleClientHandle, IOTHUB_CLIENT_CONNECTION_STATUS_CALLBACK connectionStatusCallback, void * userContextCallback)
{
    return IoTHubClientCore_SetConnectionStatusCallback((IOTHUB_CLIENT_CORE_HANDLE)iotHubModuleClientHandle, connectionStatusCallback, userContextCallback);
}

IOTHUB_CLIENT_RESULT IoTHubModuleClient_SetRetryPolicy(IOTHUB_MODULE_CLIENT_HANDLE iotHubModuleClientHandle, IOTHUB_CLIENT_RETRY_POLICY retryPolicy, size_t retryTimeoutLimitInSeconds)
{
    return IoTHubClientCore_SetRetryPolicy((IOTHUB_CLIENT_CORE_HANDLE)iotHubModuleClientHandle, retryPolicy, retryTimeoutLimitInSeconds);
}

IOTHUB_CLIENT_RESULT IoTHubModuleClient_GetRetryPolicy(IOTHUB_MODULE_CLIENT_HANDLE iotHubModuleClientHandle, IOTHUB_CLIENT_RETRY_POLICY* retryPolicy, size_t* retryTimeoutLimitInSeconds)
{
    return IoTHubClientCore_GetRetryPolicy((IOTHUB_CLIENT_CORE_HANDLE)iotHubModuleClientHandle, retryPolicy, retryTimeoutLimitInSeconds);
}

IOTHUB_CLIENT_RESULT IoTHubModuleClient_GetLastMessageReceiveTime(IOTHUB_MODULE_CLIENT_HANDLE iotHubModuleClientHandle, time_t* lastMessageReceiveTime)
{
    return IoTHubClientCore_GetLastMessageReceiveTime((IOTHUB_CLIENT_CORE_HANDLE)iotHubModuleClientHandle, lastMessageReceiveTime);
}

IOTHUB_CLIENT_RESULT IoTHubModuleClient_SetOption(IOTHUB_MODULE_CLIENT_HANDLE iotHubModuleClientHandle, const char* optionName, const void* value)
{
    return IoTHubClientCore_SetOption((IOTHUB_CLIENT_CORE_HANDLE)iotHubModuleClientHandle, optionName, value);
}

IOTHUB_CLIENT_RESULT IoTHubModuleClient_SetModuleTwinCallback(IOTHUB_MODULE_CLIENT_HANDLE iotHubModuleClientHandle, IOTHUB_CLIENT_DEVICE_TWIN_CALLBACK moduleTwinCallback, void* userContextCallback)
{
    return IoTHubClientCore_SetDeviceTwinCallback((IOTHUB_CLIENT_CORE_HANDLE)iotHubModuleClientHandle, moduleTwinCallback, userContextCallback);
}

IOTHUB_CLIENT_RESULT IoTHubModuleClient_SendReportedState(IOTHUB_MODULE_CLIENT_HANDLE iotHubModuleClientHandle, const unsigned char* reportedState, size_t size, IOTHUB_CLIENT_REPORTED_STATE_CALLBACK reportedStateCallback, void* userContextCallback)
{
    return IoTHubClientCore_SendReportedState((IOTHUB_CLIENT_CORE_HANDLE)iotHubModuleClientHandle, reportedState, size, reportedStateCallback, userContextCallback);
}

IOTHUB_CLIENT_RESULT IoTHubModuleClient_GetTwinAsync(IOTHUB_MODULE_CLIENT_HANDLE iotHubModuleClientHandle, IOTHUB_CLIENT_DEVICE_TWIN_CALLBACK moduleTwinCallback, void* userContextCallback)
{
    return IoTHubClientCore_GetTwinAsync((IOTHUB_CLIENT_CORE_HANDLE)iotHubModuleClientHandle, moduleTwinCallback, userContextCallback);
}

IOTHUB_CLIENT_RESULT IoTHubModuleClient_SetModuleMethodCallback(IOTHUB_MODULE_CLIENT_HANDLE iotHubClientHandle, IOTHUB_CLIENT_DEVICE_METHOD_CALLBACK_ASYNC methodCallback, void* userContextCallback)
{
    return IoTHubClientCore_SetDeviceMethodCallback((IOTHUB_CLIENT_CORE_HANDLE)iotHubClientHandle, (IOTHUB_CLIENT_DEVICE_METHOD_CALLBACK_ASYNC)methodCallback, userContextCallback);
}

IOTHUB_CLIENT_RESULT IoTHubModuleClient_SendEventToOutputAsync(IOTHUB_MODULE_CLIENT_HANDLE iotHubModuleClientHandle, IOTHUB_MESSAGE_HANDLE eventMessageHandle, const char* outputName, IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK eventConfirmationCallback, void* userContextCallback)
{
    return IoTHubClientCore_SendEventToOutputAsync((IOTHUB_CLIENT_CORE_HANDLE)iotHubModuleClientHandle, eventMessageHandle, outputName, eventConfirmationCallback, userContextCallback);
}

IOTHUB_CLIENT_RESULT IoTHubModuleClient_SetInputMessageCallback(IOTHUB_MODULE_CLIENT_HANDLE iotHubModuleClientHandle, const char* inputName, IOTHUB_CLIENT_MESSAGE_CALLBACK_ASYNC eventHandlerCallback, void* userContextCallback)
{
    return IoTHubClientCore_SetInputMessageCallback((IOTHUB_CLIENT_CORE_HANDLE)iotHubModuleClientHandle, inputName, eventHandlerCallback, userContextCallback);
}

#ifdef USE_EDGE_MODULES

IOTHUB_MODULE_CLIENT_HANDLE IoTHubModuleClient_CreateFromEnvironment(IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol)
{
    return IoTHubClientCore_CreateFromEnvironment(protocol);
}

IOTHUB_CLIENT_RESULT IoTHubModuleClient_DeviceMethodInvokeAsync(IOTHUB_MODULE_CLIENT_HANDLE iotHubModuleClientHandle, const char* deviceId, const char* methodName, const char* methodPayload, unsigned int timeout, IOTHUB_METHOD_INVOKE_CALLBACK methodInvokeCallback, void* context)
{
    IOTHUB_CLIENT_RESULT result;

    if ((iotHubModuleClientHandle == NULL) || (deviceId == NULL) || (methodName == NULL) || (methodPayload == NULL))
    {
        LogError("Argument cannot be NULL");
        result = IOTHUB_CLIENT_INVALID_ARG;
    }
    else
    {
        result = IoTHubClientCore_GenericMethodInvoke((IOTHUB_CLIENT_CORE_HANDLE)iotHubModuleClientHandle, deviceId, NULL, methodName, methodPayload, timeout, methodInvokeCallback, context);
    }
    return result;
}

IOTHUB_CLIENT_RESULT IoTHubModuleClient_ModuleMethodInvokeAsync(IOTHUB_MODULE_CLIENT_HANDLE iotHubModuleClientHandle, const char* deviceId, const char* moduleId, const char* methodName, const char* methodPayload, unsigned int timeout, IOTHUB_METHOD_INVOKE_CALLBACK methodInvokeCallback, void* context)
{
    IOTHUB_CLIENT_RESULT result;

    if ((iotHubModuleClientHandle == NULL) || (deviceId == NULL) || (moduleId == NULL) || (methodName == NULL) || (methodPayload == NULL))
    {
        LogError("Argument cannot be NULL");
        result = IOTHUB_CLIENT_INVALID_ARG;
    }
    else
    {
        result = IoTHubClientCore_GenericMethodInvoke((IOTHUB_CLIENT_CORE_HANDLE)iotHubModuleClientHandle, deviceId, moduleId, methodName, methodPayload, timeout, methodInvokeCallback, context);
    }
    return result;
}

#endif /*USE_EDGE_MODULES*/

