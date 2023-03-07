// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef IOTHUBTRANSPORT_AMQP_TELEMETRY_MESSENGER
#define IOTHUBTRANSPORT_AMQP_TELEMETRY_MESSENGER

#include "umock_c/umock_c_prod.h"
#include "azure_c_shared_utility/optionhandler.h"
#include "azure_uamqp_c/session.h"

#include "azure_uamqp_c/amqp_definitions_sequence_no.h"
#include "azure_uamqp_c/amqp_definitions_delivery_number.h"

#include "iothub_client_private.h"

#ifdef __cplusplus
extern "C"
{
#endif


static const char* TELEMETRY_MESSENGER_OPTION_EVENT_SEND_TIMEOUT_SECS = "telemetry_event_send_timeout_secs";
static const char* TELEMETRY_MESSENGER_OPTION_SAVED_OPTIONS = "saved_telemetry_messenger_options";

typedef struct TELEMETRY_MESSENGER_INSTANCE* TELEMETRY_MESSENGER_HANDLE;

typedef enum TELEMETRY_MESSENGER_SEND_STATUS_TAG
{
    TELEMETRY_MESSENGER_SEND_STATUS_IDLE,
    TELEMETRY_MESSENGER_SEND_STATUS_BUSY
} TELEMETRY_MESSENGER_SEND_STATUS;

typedef enum TELEMETRY_MESSENGER_EVENT_SEND_COMPLETE_RESULT_TAG
{
    TELEMETRY_MESSENGER_EVENT_SEND_COMPLETE_RESULT_OK,
    TELEMETRY_MESSENGER_EVENT_SEND_COMPLETE_RESULT_ERROR_CANNOT_PARSE,
    TELEMETRY_MESSENGER_EVENT_SEND_COMPLETE_RESULT_ERROR_FAIL_SENDING,
    TELEMETRY_MESSENGER_EVENT_SEND_COMPLETE_RESULT_ERROR_TIMEOUT,
    TELEMETRY_MESSENGER_EVENT_SEND_COMPLETE_RESULT_MESSENGER_DESTROYED
} TELEMETRY_MESSENGER_EVENT_SEND_COMPLETE_RESULT;

typedef enum TELEMETRY_MESSENGER_DISPOSITION_RESULT_TAG
{
    TELEMETRY_MESSENGER_DISPOSITION_RESULT_NONE,
    TELEMETRY_MESSENGER_DISPOSITION_RESULT_ACCEPTED,
    TELEMETRY_MESSENGER_DISPOSITION_RESULT_REJECTED,
    TELEMETRY_MESSENGER_DISPOSITION_RESULT_RELEASED
} TELEMETRY_MESSENGER_DISPOSITION_RESULT;

typedef enum TELEMETRY_MESSENGER_STATE_TAG
{
    TELEMETRY_MESSENGER_STATE_STARTING,
    TELEMETRY_MESSENGER_STATE_STARTED,
    TELEMETRY_MESSENGER_STATE_STOPPING,
    TELEMETRY_MESSENGER_STATE_STOPPED,
    TELEMETRY_MESSENGER_STATE_ERROR
} TELEMETRY_MESSENGER_STATE;

typedef struct TELEMETRY_MESSENGER_MESSAGE_DISPOSITION_INFO_TAG
{
    delivery_number message_id;
    char* source;
} TELEMETRY_MESSENGER_MESSAGE_DISPOSITION_INFO;

typedef void(*ON_TELEMETRY_MESSENGER_EVENT_SEND_COMPLETE)(IOTHUB_MESSAGE_LIST* iothub_message_list, TELEMETRY_MESSENGER_EVENT_SEND_COMPLETE_RESULT messenger_event_send_complete_result, void* context);
typedef void(*ON_TELEMETRY_MESSENGER_STATE_CHANGED_CALLBACK)(void* context, TELEMETRY_MESSENGER_STATE previous_state, TELEMETRY_MESSENGER_STATE new_state);
typedef TELEMETRY_MESSENGER_DISPOSITION_RESULT(*ON_TELEMETRY_MESSENGER_MESSAGE_RECEIVED)(IOTHUB_MESSAGE_HANDLE message, TELEMETRY_MESSENGER_MESSAGE_DISPOSITION_INFO* disposition_info, void* context);

typedef struct TELEMETRY_MESSENGER_CONFIG_TAG
{
    const char* device_id;
    const char* module_id;
    char* iothub_host_fqdn;
    ON_TELEMETRY_MESSENGER_STATE_CHANGED_CALLBACK on_state_changed_callback;
    void* on_state_changed_context;
} TELEMETRY_MESSENGER_CONFIG;

#define AMQP_BATCHING_RESERVE_SIZE              (1024)

MOCKABLE_FUNCTION(, TELEMETRY_MESSENGER_HANDLE, telemetry_messenger_create, const TELEMETRY_MESSENGER_CONFIG*, messenger_config, pfTransport_GetOption_Product_Info_Callback, prod_info_cb, void*, prod_info_ctx);
MOCKABLE_FUNCTION(, int, telemetry_messenger_send_async, TELEMETRY_MESSENGER_HANDLE, messenger_handle, IOTHUB_MESSAGE_LIST*, message, ON_TELEMETRY_MESSENGER_EVENT_SEND_COMPLETE, on_messenger_event_send_complete_callback, void*, context);
MOCKABLE_FUNCTION(, int, telemetry_messenger_subscribe_for_messages, TELEMETRY_MESSENGER_HANDLE, messenger_handle, ON_TELEMETRY_MESSENGER_MESSAGE_RECEIVED, on_message_received_callback, void*, context);
MOCKABLE_FUNCTION(, int, telemetry_messenger_unsubscribe_for_messages, TELEMETRY_MESSENGER_HANDLE, messenger_handle);
MOCKABLE_FUNCTION(, int, telemetry_messenger_send_message_disposition, TELEMETRY_MESSENGER_HANDLE, messenger_handle, TELEMETRY_MESSENGER_MESSAGE_DISPOSITION_INFO*, disposition_info, TELEMETRY_MESSENGER_DISPOSITION_RESULT, disposition_result);
MOCKABLE_FUNCTION(, int, telemetry_messenger_get_send_status, TELEMETRY_MESSENGER_HANDLE, messenger_handle, TELEMETRY_MESSENGER_SEND_STATUS*, send_status);
MOCKABLE_FUNCTION(, int, telemetry_messenger_start, TELEMETRY_MESSENGER_HANDLE, messenger_handle, SESSION_HANDLE, session_handle);
MOCKABLE_FUNCTION(, int, telemetry_messenger_stop, TELEMETRY_MESSENGER_HANDLE, messenger_handle);
MOCKABLE_FUNCTION(, void, telemetry_messenger_do_work, TELEMETRY_MESSENGER_HANDLE, messenger_handle);
MOCKABLE_FUNCTION(, void, telemetry_messenger_destroy, TELEMETRY_MESSENGER_HANDLE, messenger_handle);
MOCKABLE_FUNCTION(, int, telemetry_messenger_set_option, TELEMETRY_MESSENGER_HANDLE, messenger_handle, const char*, name, void*, value);
MOCKABLE_FUNCTION(, OPTIONHANDLER_HANDLE, telemetry_messenger_retrieve_options, TELEMETRY_MESSENGER_HANDLE, messenger_handle);


#ifdef __cplusplus
}
#endif

#endif /*IOTHUBTRANSPORT_AMQP_TELEMETRY_MESSENGER*/
