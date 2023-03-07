// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

/** @file iothub_service_client_auth.h
*    @brief     APIs that allow a user (usually a device) to create and destroy
*             a handle for use in Service client APIs.
*
*    @details APIs that allow a user (usually a device) to create
*             a handle for use in Service client APIs.
*             The create API parses the given connection string and
*             saves the IoTHub specific authentication info in the handle.
*             The destory API deallocate all the resources allocated in the handle.
*
*             User will store the handle and use it as input parameter in
*             consequent APIs. When the handle is not needed anymore user
*             responsbility to call destory to free all the resources.
*/

#ifndef IOTHUB_SERVICE_CLIENT_AUTH_H
#define IOTHUB_SERVICE_CLIENT_AUTH_H

#include "azure_macro_utils/macro_utils.h"
#include "umock_c/umock_c_prod.h"

#define IOTHUB_DEVICE_STATUS_VALUES       \
    IOTHUB_DEVICE_STATUS_ENABLED,         \
    IOTHUB_DEVICE_STATUS_DISABLED         \

MU_DEFINE_ENUM_WITHOUT_INVALID(IOTHUB_DEVICE_STATUS, IOTHUB_DEVICE_STATUS_VALUES);

#define IOTHUB_DEVICE_CONNECTION_STATE_VALUES         \
    IOTHUB_DEVICE_CONNECTION_STATE_CONNECTED,         \
    IOTHUB_DEVICE_CONNECTION_STATE_DISCONNECTED       \

MU_DEFINE_ENUM_WITHOUT_INVALID(IOTHUB_DEVICE_CONNECTION_STATE, IOTHUB_DEVICE_CONNECTION_STATE_VALUES);

#ifdef __cplusplus
extern "C"
{
#else
#endif

#include "azure_macro_utils/macro_utils.h"

/** @brief Structure to store IoTHub authentication information
*/
typedef struct IOTHUB_SERVICE_CLIENT_AUTH_TAG
{
    char* hostname;
    char* iothubName;
    char* iothubSuffix;
    char* sharedAccessKey;  //field can contain "SharedAccessSignature" if prefixed with "sas="; Otherwise, a "SharedAccessKey" is expected.
    char* keyName;
    char* deviceId;
} IOTHUB_SERVICE_CLIENT_AUTH;

/** @brief Handle to hide struct and use it in consequent APIs
*/
typedef struct IOTHUB_SERVICE_CLIENT_AUTH_TAG* IOTHUB_SERVICE_CLIENT_AUTH_HANDLE;

/**
* @brief    Creates a IoT Hub service client handle for use it
*             in consequent APIs.
*
* @param    connectionString    Pointer to a character string
*
*            Sample connection string:
*                <blockquote>
*                    <pre>HostName=[IoT Hub name goes here].[IoT Hub suffix goes here, e.g., private.azure-devices-int.net];SharedAccessKeyName=[Shared Access Key Name goes here];SharedAccessKey=[Shared Access key goes here];</pre>
*                </blockquote>
*
* @return    A non-NULL @c IOTHUB_SERVICE_CLIENT_AUTH_HANDLE value that is used when
*             invoking other functions for IoT Hub Service Client and @c NULL on failure.
*/
extern IOTHUB_SERVICE_CLIENT_AUTH_HANDLE IoTHubServiceClientAuth_CreateFromConnectionString(const char* connectionString);

/**
* @brief    Creates a IoT Hub service client handle for use it
*             in consequent APIs.
*
* @param    connectionString    Pointer to a character string
*
*            Sample connection string:
*                <blockquote>
*                    <pre>HostName=[IoT Hub name goes here].[IoT Hub suffix goes here, e.g., private.azure-devices-int.net];SharedAccessSignature=[Shared Access Signature goes here];</pre>
*                </blockquote>
*
* @return    A non-NULL @c IOTHUB_SERVICE_CLIENT_AUTH_HANDLE value that is used when
*             invoking other functions for IoT Hub Service Client and @c NULL on failure.
*/
extern IOTHUB_SERVICE_CLIENT_AUTH_HANDLE IoTHubServiceClientAuth_CreateFromSharedAccessSignature(const char* connectionString);

/**
* @brief    Disposes of resources allocated by the IoT Hub Service Client.
*
* @param    serviceClientHandle    The handle created by a call to the create function.
*/
extern void IoTHubServiceClientAuth_Destroy(IOTHUB_SERVICE_CLIENT_AUTH_HANDLE serviceClientHandle);

#ifdef __cplusplus
}
#endif

#endif // IOTHUB_SERVICE_CLIENT_AUTH_H
