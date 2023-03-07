// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef IOTHUB_TRANSPORT_LL_PRIVATE_H
#define IOTHUB_TRANSPORT_LL_PRIVATE_H

#include "iothub_transport_ll.h"

union IOTHUB_IDENTITY_INFO_TAG;
typedef union IOTHUB_IDENTITY_INFO_TAG IOTHUB_IDENTITY_INFO;

#include "azure_c_shared_utility/doublylinkedlist.h"
#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/platform.h"
#include "internal/iothub_client_authorization.h"
#include "iothub_message.h"

struct MESSAGE_DISPOSITION_CONTEXT_TAG;
typedef struct MESSAGE_DISPOSITION_CONTEXT_TAG* MESSAGE_DISPOSITION_CONTEXT_HANDLE;
typedef struct MESSAGE_CALLBACK_INFO_TAG
{
    IOTHUB_MESSAGE_HANDLE messageHandle;
    MESSAGE_DISPOSITION_CONTEXT_HANDLE transportContext;
}MESSAGE_CALLBACK_INFO;

#include "iothub_client_ll.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef bool (*pfTransport_MessageCallbackFromInput)(MESSAGE_CALLBACK_INFO* messageData, void* ctx);
    typedef bool (*pfTransport_MessageCallback)(MESSAGE_CALLBACK_INFO* messageData, void* ctx);
    typedef void (*pfTransport_ConnectionStatusCallBack)(IOTHUB_CLIENT_CONNECTION_STATUS status, IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason, void* ctx);
    typedef void (*pfTransport_SendComplete_Callback)(PDLIST_ENTRY completed, IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* ctx);
    typedef const char* (*pfTransport_GetOption_Product_Info_Callback)(void* ctx);
    typedef void (*pfTransport_Twin_ReportedStateComplete_Callback)(uint32_t item_id, int status_code, void* ctx);
    typedef void (*pfTransport_Twin_RetrievePropertyComplete_Callback)(DEVICE_TWIN_UPDATE_STATE update_state, const unsigned char* payLoad, size_t size, void* ctx);
    typedef int (*pfTransport_DeviceMethod_Complete_Callback)(const char* method_name, const unsigned char* payLoad, size_t size, METHOD_HANDLE response_id, void* ctx);
    typedef const char* (*pfTransport_GetOption_Model_Id_Callback)(void* ctx);

    /** @brief    This struct captures device configuration. */
    typedef struct IOTHUB_DEVICE_CONFIG_TAG
    {
        /** @brief    A string that identifies the device. */
        const char* deviceId;

        /** @brief    The device key used to authenticate the device. */
        const char* deviceKey;

        /** @brief    The device SAS used to authenticate the device in place of using the device key. */
        const char* deviceSasToken;

        IOTHUB_AUTHORIZATION_HANDLE authorization_module; // with either SAS Token, x509 Certs, and Device SAS Token

        /** @brief  A string that identifies the module.  Optional. */
        const char* moduleId;
    } IOTHUB_DEVICE_CONFIG;

    typedef struct TRANSPORT_CALLBACKS_INFO_TAG
    {
        pfTransport_MessageCallbackFromInput msg_input_cb;
        pfTransport_MessageCallback msg_cb;
        pfTransport_ConnectionStatusCallBack connection_status_cb;
        pfTransport_SendComplete_Callback send_complete_cb;
        pfTransport_GetOption_Product_Info_Callback prod_info_cb;
        pfTransport_Twin_ReportedStateComplete_Callback twin_rpt_state_complete_cb;
        pfTransport_Twin_RetrievePropertyComplete_Callback twin_retrieve_prop_complete_cb;
        pfTransport_DeviceMethod_Complete_Callback method_complete_cb;
        pfTransport_GetOption_Model_Id_Callback get_model_id_cb;
    } TRANSPORT_CALLBACKS_INFO;

    typedef STRING_HANDLE (*pfIoTHubTransport_GetHostname)(TRANSPORT_LL_HANDLE handle);
    typedef IOTHUB_CLIENT_RESULT(*pfIoTHubTransport_SetOption)(TRANSPORT_LL_HANDLE handle, const char *optionName, const void* value);
    typedef TRANSPORT_LL_HANDLE(*pfIoTHubTransport_Create)(const IOTHUBTRANSPORT_CONFIG* config, TRANSPORT_CALLBACKS_INFO* cb_info, void* ctx);
    typedef void (*pfIoTHubTransport_Destroy)(TRANSPORT_LL_HANDLE handle);
    typedef IOTHUB_DEVICE_HANDLE(*pfIotHubTransport_Register)(TRANSPORT_LL_HANDLE handle, const IOTHUB_DEVICE_CONFIG* device, PDLIST_ENTRY waitingToSend);
    typedef void(*pfIotHubTransport_Unregister)(IOTHUB_DEVICE_HANDLE deviceHandle);
    typedef int (*pfIoTHubTransport_Subscribe)(IOTHUB_DEVICE_HANDLE handle);
    typedef void (*pfIoTHubTransport_Unsubscribe)(IOTHUB_DEVICE_HANDLE handle);
    typedef void (*pfIoTHubTransport_DoWork)(TRANSPORT_LL_HANDLE handle);
    typedef int(*pfIoTHubTransport_SetRetryPolicy)(TRANSPORT_LL_HANDLE handle, IOTHUB_CLIENT_RETRY_POLICY retryPolicy, size_t retryTimeoutLimitInSeconds);
    typedef IOTHUB_CLIENT_RESULT(*pfIoTHubTransport_GetSendStatus)(IOTHUB_DEVICE_HANDLE handle, IOTHUB_CLIENT_STATUS *iotHubClientStatus);
    typedef int (*pfIoTHubTransport_Subscribe_DeviceTwin)(IOTHUB_DEVICE_HANDLE handle);
    typedef void (*pfIoTHubTransport_Unsubscribe_DeviceTwin)(IOTHUB_DEVICE_HANDLE handle);
    typedef IOTHUB_CLIENT_RESULT(*pfIoTHubTransport_GetTwinAsync)(IOTHUB_DEVICE_HANDLE handle, IOTHUB_CLIENT_DEVICE_TWIN_CALLBACK completionCallback, void* callbackContext);
    typedef IOTHUB_CLIENT_RESULT(*pfIotHubTransport_SendMessageDisposition)(MESSAGE_CALLBACK_INFO* messageData, IOTHUBMESSAGE_DISPOSITION_RESULT disposition);
    typedef IOTHUB_PROCESS_ITEM_RESULT(*pfIoTHubTransport_ProcessItem)(TRANSPORT_LL_HANDLE handle, IOTHUB_IDENTITY_TYPE item_type, IOTHUB_IDENTITY_INFO* iothub_item);
    typedef int(*pfIoTHubTransport_Subscribe_DeviceMethod)(IOTHUB_DEVICE_HANDLE handle);
    typedef void(*pfIoTHubTransport_Unsubscribe_DeviceMethod)(IOTHUB_DEVICE_HANDLE handle);
    typedef int(*pfIoTHubTransport_DeviceMethod_Response)(IOTHUB_DEVICE_HANDLE handle, METHOD_HANDLE methodId, const unsigned char* response, size_t response_size, int status_response);
    typedef int(*pfIoTHubTransport_Subscribe_InputQueue)(IOTHUB_DEVICE_HANDLE handle);
    typedef void(*pfIoTHubTransport_Unsubscribe_InputQueue)(IOTHUB_DEVICE_HANDLE handle);
    typedef int(*pfIoTHubTransport_SetCallbackContext)(TRANSPORT_LL_HANDLE handle, void* ctx);
    typedef int(*pfIoTHubTransport_GetSupportedPlatformInfo)(TRANSPORT_LL_HANDLE handle, PLATFORM_INFO_OPTION* info);

#define TRANSPORT_PROVIDER_FIELDS                                                   \
pfIotHubTransport_SendMessageDisposition IoTHubTransport_SendMessageDisposition;    \
pfIoTHubTransport_Subscribe_DeviceMethod IoTHubTransport_Subscribe_DeviceMethod;    \
pfIoTHubTransport_Unsubscribe_DeviceMethod IoTHubTransport_Unsubscribe_DeviceMethod;\
pfIoTHubTransport_DeviceMethod_Response IoTHubTransport_DeviceMethod_Response;      \
pfIoTHubTransport_Subscribe_DeviceTwin IoTHubTransport_Subscribe_DeviceTwin;        \
pfIoTHubTransport_Unsubscribe_DeviceTwin IoTHubTransport_Unsubscribe_DeviceTwin;    \
pfIoTHubTransport_ProcessItem IoTHubTransport_ProcessItem;                          \
pfIoTHubTransport_GetHostname IoTHubTransport_GetHostname;                          \
pfIoTHubTransport_SetOption IoTHubTransport_SetOption;                              \
pfIoTHubTransport_Create IoTHubTransport_Create;                                    \
pfIoTHubTransport_Destroy IoTHubTransport_Destroy;                                  \
pfIotHubTransport_Register IoTHubTransport_Register;                                \
pfIotHubTransport_Unregister IoTHubTransport_Unregister;                            \
pfIoTHubTransport_Subscribe IoTHubTransport_Subscribe;                              \
pfIoTHubTransport_Unsubscribe IoTHubTransport_Unsubscribe;                          \
pfIoTHubTransport_DoWork IoTHubTransport_DoWork;                                    \
pfIoTHubTransport_SetRetryPolicy IoTHubTransport_SetRetryPolicy;                    \
pfIoTHubTransport_GetSendStatus IoTHubTransport_GetSendStatus;                      \
pfIoTHubTransport_Subscribe_InputQueue IoTHubTransport_Subscribe_InputQueue;        \
pfIoTHubTransport_Unsubscribe_InputQueue IoTHubTransport_Unsubscribe_InputQueue;    \
pfIoTHubTransport_SetCallbackContext IoTHubTransport_SetCallbackContext;            \
pfIoTHubTransport_GetTwinAsync IoTHubTransport_GetTwinAsync;                        \
pfIoTHubTransport_GetSupportedPlatformInfo IoTHubTransport_GetSupportedPlatformInfo     /*there's an intentional missing ; on this line*/

    struct TRANSPORT_PROVIDER_TAG
    {
        TRANSPORT_PROVIDER_FIELDS;
    };

    MOCKABLE_FUNCTION(, int, IoTHub_Transport_ValidateCallbacks, const TRANSPORT_CALLBACKS_INFO*, transport_cb);

#ifdef __cplusplus
}
#endif

#endif /* IOTHUB_TRANSPORT_LL_PRIVATE_H */
