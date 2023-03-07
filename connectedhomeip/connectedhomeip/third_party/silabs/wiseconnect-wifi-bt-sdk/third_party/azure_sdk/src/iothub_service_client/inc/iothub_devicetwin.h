// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// This file is under development and it is subject to change

#ifndef IOTHUB_DEVICETWIN_H
#define IOTHUB_DEVICETWIN_H

#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/singlylinkedlist.h"
#include "azure_c_shared_utility/map.h"
#include <time.h>
#include "iothub_service_client_auth.h"

#include "umock_c/umock_c_prod.h"

#ifdef __cplusplus
extern "C"
{
#else
#endif

#define IOTHUB_DEVICE_TWIN_RESULT_VALUES     \
    IOTHUB_DEVICE_TWIN_OK,                   \
    IOTHUB_DEVICE_TWIN_INVALID_ARG,          \
    IOTHUB_DEVICE_TWIN_ERROR,                \
    IOTHUB_DEVICE_TWIN_HTTPAPI_ERROR         \

MU_DEFINE_ENUM_WITHOUT_INVALID(IOTHUB_DEVICE_TWIN_RESULT, IOTHUB_DEVICE_TWIN_RESULT_VALUES);

/** @brief Handle to hide struct and use it in consequent APIs
*/
typedef struct IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_TAG* IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE;


/** @brief    Creates a IoT Hub Service Client DeviceTwin handle for use it in consequent APIs.
*
* @param    serviceClientHandle    Service client handle.
*
* @return    A non-NULL @c IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE value that is used when
*             invoking other functions for IoT Hub DeviceTwin and @c NULL on failure.
*/
MOCKABLE_FUNCTION(, IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE, IoTHubDeviceTwin_Create, IOTHUB_SERVICE_CLIENT_AUTH_HANDLE, serviceClientHandle);

/** @brief    Disposes of resources allocated by the IoT Hub IoTHubDeviceTwin_Create.
*
* @param    serviceClientDeviceTwinHandle    The handle created by a call to the create function.
*/
MOCKABLE_FUNCTION(, void,  IoTHubDeviceTwin_Destroy, IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE, serviceClientDeviceTwinHandle);

/** @brief    Retrieves the given device's twin info.
*
* @param    serviceClientDeviceTwinHandle    The handle created by a call to the create function.
* @param    deviceId                        The device name (id) to retrieve twin info for.
*
* @return    A non-NULL char* containing device twin info upon success or NULL upon failure.
*/
MOCKABLE_FUNCTION(, char*,  IoTHubDeviceTwin_GetTwin, IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE, serviceClientDeviceTwinHandle, const char*, deviceId);

/** @brief    Updates (partial update) the given device's twin info.
*
* @param    serviceClientDeviceTwinHandle    The handle created by a call to the create function.
* @param    deviceId                        The device name (id) to update the twin info for.
* @param    deviceTwinJson                  DeviceTwin JSon string containing the info (tags, desired properties) to update.
*                                           All well-known read-only members are ignored.
*                                           Properties provided with value of null are removed from twin's document.
*
* @return    A non-NULL char* containing updated device twin info upon success or NULL upon failure.
*/
MOCKABLE_FUNCTION(, char*,  IoTHubDeviceTwin_UpdateTwin, IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE, serviceClientDeviceTwinHandle, const char*, deviceId, const char*, deviceTwinJson);

/** @brief  Retrieves the given module's twin info.
*
* @param    serviceClientDeviceTwinHandle   The handle created by a call to the create function.
* @param    deviceId                        The device name (id) containing the module to retrieve the twin info for.
* @param    moduleId                        The module name (id) to retrieve twin info for.
*
* @return   A non-NULL char* containing module twin info upon success or NULL upon failure.
*/
MOCKABLE_FUNCTION(, char*,  IoTHubDeviceTwin_GetModuleTwin, IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE, serviceClientDeviceTwinHandle, const char*, deviceId, const char*, moduleId);

/** @brief  Updates (partial update) the given module's twin info.
*
* @param    serviceClientDeviceTwinHandle   The handle created by a call to the create function.
* @param    deviceId                        The device name (id) containing the module to update.
* @param    moduleId                        The module name (id) to update the twin info for.
* @param    moduleTwinJson                  ModuleTwin JSon string containing the info (tags, desired properties) to update.
*                                           All well-known read-only members are ignored.
*                                           Properties provided with value of null are removed from twin's document.
*
* @return   A non-NULL char* containing updated module twin info upon success or NULL upon failure.
*/
MOCKABLE_FUNCTION(, char*,  IoTHubDeviceTwin_UpdateModuleTwin, IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE, serviceClientDeviceTwinHandle, const char*, deviceId, const char*, moduleId, const char*, moduleTwinJson);

#ifdef __cplusplus
}
#endif

#endif // IOTHUB_DEVICETWIN_H
