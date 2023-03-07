// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef IOTHUBTRANSPORTAMQP_AMQP_DEVICE_H
#define IOTHUBTRANSPORTAMQP_AMQP_DEVICE_H

#include "umock_c/umock_c_prod.h"
#include "azure_c_shared_utility/optionhandler.h"
#include "azure_uamqp_c/session.h"
#include "azure_uamqp_c/cbs.h"
#include "iothub_message.h"
#include "iothub_client_private.h"
#include "iothubtransport_amqp_device.h"

#ifdef __cplusplus
extern "C"
{
#endif

// @brief    name of option to apply the instance obtained using amqp_device_retrieve_options
static const char* DEVICE_OPTION_SAVED_OPTIONS = "saved_device_options";
static const char* DEVICE_OPTION_EVENT_SEND_TIMEOUT_SECS = "event_send_timeout_secs";
static const char* DEVICE_OPTION_CBS_REQUEST_TIMEOUT_SECS = "cbs_request_timeout_secs";
static const char* DEVICE_OPTION_SAS_TOKEN_REFRESH_TIME_SECS = "sas_token_refresh_time_secs";
static const char* DEVICE_OPTION_SAS_TOKEN_LIFETIME_SECS = "sas_token_lifetime_secs";

#define DEVICE_STATE_VALUES \
    DEVICE_STATE_STOPPED, \
    DEVICE_STATE_STOPPING, \
    DEVICE_STATE_STARTING, \
    DEVICE_STATE_STARTED, \
    DEVICE_STATE_ERROR_AUTH, \
    DEVICE_STATE_ERROR_AUTH_TIMEOUT, \
    DEVICE_STATE_ERROR_MSG

MU_DEFINE_ENUM_WITHOUT_INVALID(DEVICE_STATE, DEVICE_STATE_VALUES);

#define DEVICE_AUTH_MODE_VALUES \
    DEVICE_AUTH_MODE_CBS, \
    DEVICE_AUTH_MODE_X509

MU_DEFINE_ENUM_WITHOUT_INVALID(DEVICE_AUTH_MODE, DEVICE_AUTH_MODE_VALUES);

#define DEVICE_SEND_STATUS_VALUES \
    DEVICE_SEND_STATUS_IDLE, \
    DEVICE_SEND_STATUS_BUSY

MU_DEFINE_ENUM_WITHOUT_INVALID(DEVICE_SEND_STATUS, DEVICE_SEND_STATUS_VALUES);

#define D2C_EVENT_SEND_RESULT_VALUES \
    D2C_EVENT_SEND_COMPLETE_RESULT_OK, \
    D2C_EVENT_SEND_COMPLETE_RESULT_ERROR_CANNOT_PARSE, \
    D2C_EVENT_SEND_COMPLETE_RESULT_ERROR_FAIL_SENDING, \
    D2C_EVENT_SEND_COMPLETE_RESULT_ERROR_TIMEOUT, \
    D2C_EVENT_SEND_COMPLETE_RESULT_DEVICE_DESTROYED, \
    D2C_EVENT_SEND_COMPLETE_RESULT_ERROR_UNKNOWN

MU_DEFINE_ENUM_WITHOUT_INVALID(D2C_EVENT_SEND_RESULT, D2C_EVENT_SEND_RESULT_VALUES);

#define DEVICE_MESSAGE_DISPOSITION_RESULT_VALUES \
    DEVICE_MESSAGE_DISPOSITION_RESULT_NONE, \
    DEVICE_MESSAGE_DISPOSITION_RESULT_ACCEPTED, \
    DEVICE_MESSAGE_DISPOSITION_RESULT_REJECTED, \
    DEVICE_MESSAGE_DISPOSITION_RESULT_RELEASED

MU_DEFINE_ENUM_WITHOUT_INVALID(DEVICE_MESSAGE_DISPOSITION_RESULT, DEVICE_MESSAGE_DISPOSITION_RESULT_VALUES);

#define DEVICE_TWIN_UPDATE_RESULT_STRINGS \
    DEVICE_TWIN_UPDATE_RESULT_OK, \
    DEVICE_TWIN_UPDATE_RESULT_ERROR

MU_DEFINE_ENUM_WITHOUT_INVALID(DEVICE_TWIN_UPDATE_RESULT, DEVICE_TWIN_UPDATE_RESULT_STRINGS);

#define DEVICE_TWIN_UPDATE_TYPE_STRINGS \
    DEVICE_TWIN_UPDATE_TYPE_PARTIAL, \
    DEVICE_TWIN_UPDATE_TYPE_COMPLETE

MU_DEFINE_ENUM_WITHOUT_INVALID(DEVICE_TWIN_UPDATE_TYPE, DEVICE_TWIN_UPDATE_TYPE_STRINGS)

typedef struct DEVICE_MESSAGE_DISPOSITION_INFO_TAG
{
    unsigned long message_id;
    char* source;
} DEVICE_MESSAGE_DISPOSITION_INFO;

typedef void(*ON_DEVICE_STATE_CHANGED)(void* context, DEVICE_STATE previous_state, DEVICE_STATE new_state);
typedef DEVICE_MESSAGE_DISPOSITION_RESULT(*ON_DEVICE_C2D_MESSAGE_RECEIVED)(IOTHUB_MESSAGE_HANDLE message, DEVICE_MESSAGE_DISPOSITION_INFO* disposition_info, void* context);
typedef void(*ON_DEVICE_D2C_EVENT_SEND_COMPLETE)(IOTHUB_MESSAGE_LIST* message, D2C_EVENT_SEND_RESULT result, void* context);
typedef void(*DEVICE_SEND_TWIN_UPDATE_COMPLETE_CALLBACK)(DEVICE_TWIN_UPDATE_RESULT result, int status_code, void* context);
typedef void(*DEVICE_TWIN_UPDATE_RECEIVED_CALLBACK)(DEVICE_TWIN_UPDATE_TYPE update_type, const unsigned char* message, size_t length, void* context);

typedef struct DEVICE_CONFIG_TAG
{
    const char* device_id;
    const char* module_id;
    pfTransport_GetOption_Product_Info_Callback prod_info_cb;
    void* prod_info_ctx;
    char* iothub_host_fqdn;
    DEVICE_AUTH_MODE authentication_mode;
    ON_DEVICE_STATE_CHANGED on_state_changed_callback;
    void* on_state_changed_context;

    // Auth module used to generating handle authorization
    // with either SAS Token, x509 Certs, and Device SAS Token
    IOTHUB_AUTHORIZATION_HANDLE authorization_module;
} AMQP_DEVICE_CONFIG;

typedef struct AMQP_DEVICE_INSTANCE* AMQP_DEVICE_HANDLE;

MOCKABLE_FUNCTION(, AMQP_DEVICE_HANDLE, amqp_device_create, AMQP_DEVICE_CONFIG*, config);
MOCKABLE_FUNCTION(, void, amqp_device_destroy, AMQP_DEVICE_HANDLE, handle);
MOCKABLE_FUNCTION(, int, amqp_device_start_async, AMQP_DEVICE_HANDLE, handle, SESSION_HANDLE, session_handle, CBS_HANDLE, cbs_handle);
MOCKABLE_FUNCTION(, int, amqp_device_stop, AMQP_DEVICE_HANDLE, handle);
MOCKABLE_FUNCTION(, int, amqp_device_delayed_stop, AMQP_DEVICE_HANDLE, handle, size_t, delay_secs);
MOCKABLE_FUNCTION(, void, amqp_device_do_work, AMQP_DEVICE_HANDLE, handle);
MOCKABLE_FUNCTION(, int, amqp_device_send_event_async, AMQP_DEVICE_HANDLE, handle, IOTHUB_MESSAGE_LIST*, message, ON_DEVICE_D2C_EVENT_SEND_COMPLETE, on_device_d2c_event_send_complete_callback, void*, context);
MOCKABLE_FUNCTION(, int, amqp_device_send_twin_update_async, AMQP_DEVICE_HANDLE, handle, CONSTBUFFER_HANDLE, data, DEVICE_SEND_TWIN_UPDATE_COMPLETE_CALLBACK, on_send_twin_update_complete_callback, void*, context);
MOCKABLE_FUNCTION(, int, amqp_device_subscribe_for_twin_updates, AMQP_DEVICE_HANDLE, handle, DEVICE_TWIN_UPDATE_RECEIVED_CALLBACK, on_device_twin_update_received_callback, void*, context);
MOCKABLE_FUNCTION(, int, amqp_device_unsubscribe_for_twin_updates, AMQP_DEVICE_HANDLE, handle);
MOCKABLE_FUNCTION(, int, amqp_device_get_twin_async, AMQP_DEVICE_HANDLE, handle, DEVICE_TWIN_UPDATE_RECEIVED_CALLBACK, on_device_get_twin_completed_callback, void*, context);
MOCKABLE_FUNCTION(, int, amqp_device_get_send_status, AMQP_DEVICE_HANDLE, handle, DEVICE_SEND_STATUS*, send_status);
MOCKABLE_FUNCTION(, int, amqp_device_subscribe_message, AMQP_DEVICE_HANDLE, handle, ON_DEVICE_C2D_MESSAGE_RECEIVED, on_message_received_callback, void*, context);
MOCKABLE_FUNCTION(, int, amqp_device_unsubscribe_message, AMQP_DEVICE_HANDLE, handle);
MOCKABLE_FUNCTION(, int, amqp_device_send_message_disposition, AMQP_DEVICE_HANDLE, AMQP_DEVICE_HANDLE, DEVICE_MESSAGE_DISPOSITION_INFO*, disposition_info, DEVICE_MESSAGE_DISPOSITION_RESULT, disposition_result);
MOCKABLE_FUNCTION(, int, amqp_device_set_retry_policy, AMQP_DEVICE_HANDLE, handle, IOTHUB_CLIENT_RETRY_POLICY, policy, size_t, retry_timeout_limit_in_seconds);
MOCKABLE_FUNCTION(, int, amqp_device_set_option, AMQP_DEVICE_HANDLE, handle, const char*, name, void*, value);
MOCKABLE_FUNCTION(, OPTIONHANDLER_HANDLE, amqp_device_retrieve_options, AMQP_DEVICE_HANDLE, handle);


#ifdef __cplusplus
}
#endif

#endif // IOTHUBTRANSPORTAMQP_AMQP_DEVICE_H
