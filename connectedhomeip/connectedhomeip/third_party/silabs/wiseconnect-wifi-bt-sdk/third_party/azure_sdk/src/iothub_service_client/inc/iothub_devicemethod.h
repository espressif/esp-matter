// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// This file is under development and it is subject to change

#ifndef IOTHUB_DEVICETMETHOD_H
#define IOTHUB_DEVICETMETHOD_H

#ifdef __cplusplus
extern "C"
{
#else
#endif

#include "iothub_service_client_auth.h"

#include "umock_c/umock_c_prod.h"

#define IOTHUB_DEVICE_METHOD_RESULT_VALUES     \
    IOTHUB_DEVICE_METHOD_OK,                   \
    IOTHUB_DEVICE_METHOD_INVALID_ARG,          \
    IOTHUB_DEVICE_METHOD_ERROR,                \
    IOTHUB_DEVICE_METHOD_HTTPAPI_ERROR         \

MU_DEFINE_ENUM_WITHOUT_INVALID(IOTHUB_DEVICE_METHOD_RESULT, IOTHUB_DEVICE_METHOD_RESULT_VALUES);

/** @brief Handle to hide struct and use it in consequent APIs
*/
typedef struct IOTHUB_SERVICE_CLIENT_DEVICE_METHOD_TAG* IOTHUB_SERVICE_CLIENT_DEVICE_METHOD_HANDLE;

/** @brief    Creates a IoT Hub Service Client DeviceMethod handle for use it in consequent APIs.
*
* @param    serviceClientHandle    Service client handle.
*
* @return    A non-NULL @c IOTHUB_SERVICE_CLIENT_DEVICE_METHOD_HANDLE value that is used when
*             invoking other functions for IoT Hub DeviceMethod and @c NULL on failure.
*/
MOCKABLE_FUNCTION(, IOTHUB_SERVICE_CLIENT_DEVICE_METHOD_HANDLE, IoTHubDeviceMethod_Create, IOTHUB_SERVICE_CLIENT_AUTH_HANDLE, serviceClientHandle);

/** @brief    Disposes of resources allocated by the IoT Hub IoTHubDeviceMethod_Create.
*
* @param    serviceClientDeviceMethodHandle    The handle created by a call to the create function.
*/
MOCKABLE_FUNCTION(, void,  IoTHubDeviceMethod_Destroy, IOTHUB_SERVICE_CLIENT_DEVICE_METHOD_HANDLE, serviceClientDeviceMethodHandle);

/** @brief    Call a method on device with a given payload.
*
* @param    serviceClientDeviceMethodHandle    The handle created by a call to the create function.
* @param    deviceId                        The device name (id) to call a method on.
* @param    methodName                      The method name to call.
* @param    methodPayload                   The message payload to send.
* @param    response                        Output buffer for response payload.
* @param    timeout                         Time before IoTHubDeviceMethod_InvokeModule times out.
*
* @warning  The timeout parameter is ignored.  See https://github.com/Azure/azure-iot-sdk-c/issues/1378.
*           The timeout used will be the default for IoT Hub.
*
* @param    responseStatus                  Response status code from invocation
* @param    responsePayload                 Output buffer for response payload.
* @param    responsePayloadSize             String length of responsePayload.
*
* @return    An IOTHUB_DEVICE_METHOD_RESULT containing the return status.
*/
MOCKABLE_FUNCTION(, IOTHUB_DEVICE_METHOD_RESULT, IoTHubDeviceMethod_Invoke, IOTHUB_SERVICE_CLIENT_DEVICE_METHOD_HANDLE, serviceClientDeviceMethodHandle, const char*, deviceId, const char*, methodName, const char*, methodPayload, unsigned int, timeout, int*, responseStatus, unsigned char**, responsePayload, size_t*, responsePayloadSize);

/** @brief    Call a method on device and a module with a given payload.
*
* @param    serviceClientDeviceMethodHandle    The handle created by a call to the create function.
* @param    deviceId                        The device name (id) to call a method on.
* @param    moduleId                        The module name (id) to call a method on.
* @param    methodName                      The method name to call.
* @param    methodPayload                   The message payload to send.
*
* @warning  The timeout parameter is ignored.  See https://github.com/Azure/azure-iot-sdk-c/issues/1378.
*           The timeout used will be the default for IoT Hub.
*
* @param    timeout                         Time before IoTHubDeviceMethod_InvokeModule times out.
* @param    responseStatus                  Response status code from invocation.
* @param    responsePayload                 Output buffer for response payload.
* @param    responsePayloadSize             String length of responsePayload.
*
* @return    An IOTHUB_DEVICE_METHOD_RESULT containing the return status.
*/
MOCKABLE_FUNCTION(, IOTHUB_DEVICE_METHOD_RESULT, IoTHubDeviceMethod_InvokeModule, IOTHUB_SERVICE_CLIENT_DEVICE_METHOD_HANDLE, serviceClientDeviceMethodHandle, const char*, deviceId, const char*, moduleId, const char*, methodName, const char*, methodPayload, unsigned int, timeout, int*, responseStatus, unsigned char**, responsePayload, size_t*, responsePayloadSize);


#ifdef __cplusplus
}
#endif

#endif // IOTHUB_DEVICETMETHOD_H
