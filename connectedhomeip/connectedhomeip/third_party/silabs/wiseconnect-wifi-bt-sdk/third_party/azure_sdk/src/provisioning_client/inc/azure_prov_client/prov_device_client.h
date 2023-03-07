// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

/** @file prov_device_client.h
*    @brief Extends the Prov_Device_LL module with additional features.
*
*    @details Prov_Device is a module that extends the Prov_Device_LL
*             module with 2 features:
*                - scheduling the work for the IoTHubCLient from a
*                  thread, so that the user does not need to create their
*                  own thread
*                - thread-safe APIs
*/

#ifndef PROV_DEVICE_CLIENT_H
#define PROV_DEVICE_CLIENT_H

#ifndef PROV_DEVICE_CLIENT_INSTANCE_TYPE
typedef struct PROV_DEVICE_INSTANCE_TAG* PROV_DEVICE_HANDLE;
#define PROV_DEVICE_CLIENT_INSTANCE_TYPE
#endif // PROV_DEVICE_CLIENT_INSTANCE_TYPE

#include <stddef.h>
#include <stdint.h>
#include "prov_device_ll_client.h"
#include "umock_c/umock_c_prod.h"
#include "azure_macro_utils/macro_utils.h"
#include "azure_c_shared_utility/const_defines.h"
#include "azure_prov_client/prov_transport.h"

#ifdef __cplusplus
extern "C"
{
#endif

static STATIC_VAR_UNUSED const char* const PROV_OPTION_DO_WORK_FREQUENCY_IN_MS = "do_work_freq_ms";

MOCKABLE_FUNCTION(, PROV_DEVICE_HANDLE, Prov_Device_Create, const char*, uri, const char*, scope_id, PROV_DEVICE_TRANSPORT_PROVIDER_FUNCTION, protocol);
MOCKABLE_FUNCTION(, void, Prov_Device_Destroy, PROV_DEVICE_HANDLE, prov_device_handle);
MOCKABLE_FUNCTION(, PROV_DEVICE_RESULT, Prov_Device_Register_Device, PROV_DEVICE_HANDLE, prov_device_handle, PROV_DEVICE_CLIENT_REGISTER_DEVICE_CALLBACK, register_callback, void*, user_context, PROV_DEVICE_CLIENT_REGISTER_STATUS_CALLBACK, register_status_callback, void*, status_user_context);
MOCKABLE_FUNCTION(, PROV_DEVICE_RESULT, Prov_Device_SetOption, PROV_DEVICE_HANDLE, prov_device_handle, const char*, optionName, const void*, value);
MOCKABLE_FUNCTION(, const char*, Prov_Device_GetVersionString);

/**
* @brief    Sets the Provisioning Data that is sent to the Provisioning service
*
* @param    handle          The handle created by a call to the create function.
* @param    json            The json payload field that is sent to the service.  Setting json to NULL will unset the value previously set
*
* @return PROV_DEVICE_RESULT_OK upon success or an error code upon failure
*/
MOCKABLE_FUNCTION(, PROV_DEVICE_RESULT, Prov_Device_Set_Provisioning_Payload, PROV_DEVICE_HANDLE, handle, const char*, json);

/**
* @brief    Retrieves the Provisioning Data that is sent from the Provisioning service
*
* @param    handle          The handle created by a call to the create function.
*
* @return The data that was specified by the service
*/
MOCKABLE_FUNCTION(, const char*, Prov_Device_Get_Provisioning_Payload, PROV_DEVICE_HANDLE, handle);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // PROV_DEVICE_CLIENT_H
