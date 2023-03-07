// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <limits.h>
#include "azure_c_shared_utility/optimize_size.h"
#include "azure_c_shared_utility/agenttime.h"
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/singlylinkedlist.h"
#include "azure_c_shared_utility/doublylinkedlist.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/platform.h"
#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/urlencode.h"
#include "azure_c_shared_utility/tlsio.h"
#include "azure_c_shared_utility/optionhandler.h"
#include "azure_c_shared_utility/shared_util_options.h"
#include "azure_macro_utils/macro_utils.h"

#include "azure_uamqp_c/cbs.h"
#include "azure_uamqp_c/amqp_definitions.h"
#include "azure_uamqp_c/session.h"
#include "azure_uamqp_c/message.h"
#include "azure_uamqp_c/messaging.h"

#include "iothub_client_options.h"
#include "internal/iothub_client_private.h"
#include "internal/iothubtransportamqp_methods.h"
#include "internal/iothub_client_retry_control.h"
#include "internal/iothubtransport_amqp_common.h"
#include "internal/iothubtransport_amqp_connection.h"
#include "internal/iothubtransport_amqp_device.h"
#include "internal/iothubtransport.h"
#include "iothub_client_version.h"
#include "internal/iothub_transport_ll_private.h"


#define RESULT_OK                                 0
#define INDEFINITE_TIME                           ((time_t)(-1))
#define DEFAULT_CBS_REQUEST_TIMEOUT_SECS          30
#define DEFAULT_DEVICE_STATE_CHANGE_TIMEOUT_SECS  60
#define DEFAULT_EVENT_SEND_TIMEOUT_SECS           300
#define MAX_NUMBER_OF_DEVICE_FAILURES             5
#define DEFAULT_SERVICE_KEEP_ALIVE_FREQ_SECS      240
#define DEFAULT_REMOTE_IDLE_PING_RATIO            0.50
#define DEFAULT_RETRY_POLICY                      IOTHUB_CLIENT_RETRY_EXPONENTIAL_BACKOFF_WITH_JITTER
// DEFAULT_MAX_RETRY_TIME_IN_SECS = 0 means infinite retry.
#define DEFAULT_MAX_RETRY_TIME_IN_SECS            0
#define MAX_SERVICE_KEEP_ALIVE_RATIO              0.9
#define DEFAULT_DEVICE_STOP_DELAY                 10

// ---------- Data Definitions ---------- //

typedef enum AMQP_TRANSPORT_AUTHENTICATION_MODE_TAG
{
    AMQP_TRANSPORT_AUTHENTICATION_MODE_NOT_SET,
    AMQP_TRANSPORT_AUTHENTICATION_MODE_CBS,
    AMQP_TRANSPORT_AUTHENTICATION_MODE_X509
} AMQP_TRANSPORT_AUTHENTICATION_MODE;

/*
Definition of transport states:

AMQP_TRANSPORT_STATE_NOT_CONNECTED:                    Initial state when the transport is created.
AMQP_TRANSPORT_STATE_CONNECTING:                       First connection ever.
AMQP_TRANSPORT_STATE_CONNECTED:                        Transition from AMQP_TRANSPORT_STATE_CONNECTING or AMQP_TRANSPORT_STATE_RECONNECTING.
AMQP_TRANSPORT_STATE_RECONNECTION_REQUIRED:            When a failure occurred and the transport identifies a reconnection is needed.
AMQP_TRANSPORT_STATE_READY_FOR_RECONNECTION:           Transition from AMQP_TRANSPORT_STATE_RECONNECTION_REQUIRED after all prep is done (transient instances are destroyed, devices are stopped).
AMQP_TRANSPORT_STATE_RECONNECTING:                     Transition from AMQP_TRANSPORT_STATE_READY_FOR_RECONNECTION.
AMQP_TRANSPORT_STATE_NOT_CONNECTED_NO_MORE_RETRIES:    State reached if the maximum number/length of reconnections has been reached.
AMQP_TRANSPORT_STATE_BEING_DESTROYED:                  State set if IoTHubTransport_AMQP_Common_Destroy function is invoked.
*/

#define AMQP_TRANSPORT_STATE_STRINGS                    \
    AMQP_TRANSPORT_STATE_NOT_CONNECTED,                 \
    AMQP_TRANSPORT_STATE_CONNECTING,                    \
    AMQP_TRANSPORT_STATE_CONNECTED,                     \
    AMQP_TRANSPORT_STATE_RECONNECTION_REQUIRED,         \
    AMQP_TRANSPORT_STATE_READY_FOR_RECONNECTION,        \
    AMQP_TRANSPORT_STATE_RECONNECTING,                  \
    AMQP_TRANSPORT_STATE_NOT_CONNECTED_NO_MORE_RETRIES, \
    AMQP_TRANSPORT_STATE_BEING_DESTROYED

// Suppress unused function warning for AMQP_TRANSPORT_STATEstrings
#ifdef __APPLE__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"
#endif
MU_DEFINE_LOCAL_ENUM(AMQP_TRANSPORT_STATE, AMQP_TRANSPORT_STATE_STRINGS);
#ifdef __APPLE__
#pragma clang diagnostic pop
#endif

typedef struct AMQP_TRANSPORT_INSTANCE_TAG
{
    STRING_HANDLE iothub_host_fqdn;                                     // FQDN of the IoT Hub.
    XIO_HANDLE tls_io;                                                  // TSL I/O transport.
    AMQP_GET_IO_TRANSPORT underlying_io_transport_provider;             // Pointer to the function that creates the TLS I/O (internal use only).
    AMQP_CONNECTION_HANDLE amqp_connection;                             // Base amqp connection with service.
    AMQP_CONNECTION_STATE amqp_connection_state;                        // Current state of the amqp_connection.
    AMQP_TRANSPORT_AUTHENTICATION_MODE preferred_authentication_mode;   // Used to avoid registered devices using different authentication modes.
    SINGLYLINKEDLIST_HANDLE registered_devices;                         // List of devices currently registered in this transport.
    bool is_trace_on;                                                   // Turns logging on and off.
    OPTIONHANDLER_HANDLE saved_tls_options;                             // Here are the options from the xio layer if any is saved.
    AMQP_TRANSPORT_STATE state;                                         // Current state of the transport.
    RETRY_CONTROL_HANDLE connection_retry_control;                      // Controls when the re-connection attempt should occur.
    size_t svc2cl_keep_alive_timeout_secs;                       // Service to device keep alive frequency
    double cl2svc_keep_alive_send_ratio;                                    // Client to service keep alive frequency

    char* http_proxy_hostname;
    int http_proxy_port;
    char* http_proxy_username;
    char* http_proxy_password;

    size_t option_cbs_request_timeout_secs;                             // Device-specific option.
    size_t option_send_event_timeout_secs;                              // Device-specific option.

                                                                        // Auth module used to generating handle authorization
    IOTHUB_AUTHORIZATION_HANDLE authorization_module;                   // with either SAS Token, x509 Certs, and Device SAS Token

    TRANSPORT_CALLBACKS_INFO transport_callbacks;
    void* transport_ctx;
} AMQP_TRANSPORT_INSTANCE;

typedef struct AMQP_TRANSPORT_DEVICE_INSTANCE_TAG
{
    STRING_HANDLE device_id;                                            // Identity of the device.
    AMQP_DEVICE_HANDLE device_handle;                                   // Logic unit that performs authentication, messaging, etc.
    AMQP_TRANSPORT_INSTANCE* transport_instance;                        // Saved reference to the transport the device is registered on.
    PDLIST_ENTRY waiting_to_send;                                       // List of events waiting to be sent to the iot hub (i.e., haven't been processed by the transport yet).
    DEVICE_STATE device_state;                                          // Current state of the device_handle instance.
    size_t number_of_previous_failures;                                 // Number of times the device has failed in sequence; this value is reset to 0 if device succeeds to authenticate, send and/or recv messages.
    size_t number_of_send_event_complete_failures;                      // Number of times on_event_send_complete was called in row with an error.
    time_t time_of_last_state_change;                                   // Time the device_handle last changed state; used to track timeouts of amqp_device_start_async and amqp_device_stop.
    unsigned int max_state_change_timeout_secs;                         // Maximum number of seconds allowed for device_handle to complete start and stop state changes.
    // the methods portion
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE methods_handle;                 // Handle to instance of module that deals with device methods for AMQP.
    // is subscription for methods needed?
    bool subscribe_methods_needed;                                       // Indicates if should subscribe for device methods.
    // is the transport subscribed for methods?
    bool subscribed_for_methods;                                         // Indicates if device is subscribed for device methods.

    TRANSPORT_CALLBACKS_INFO transport_callbacks;
    void* transport_ctx;
} AMQP_TRANSPORT_DEVICE_INSTANCE;

typedef struct MESSAGE_DISPOSITION_CONTEXT_TAG
{
    AMQP_TRANSPORT_DEVICE_INSTANCE* device_state;
    char* link_name;
    delivery_number message_id;
} MESSAGE_DISPOSITION_CONTEXT;

typedef struct AMQP_TRANSPORT_DEVICE_TWIN_CONTEXT_TAG
{
    uint32_t item_id;
    TRANSPORT_CALLBACKS_INFO transport_callbacks;
    void* transport_ctx;
} AMQP_TRANSPORT_DEVICE_TWIN_CONTEXT;

typedef struct AMQP_TRANSPORT_GET_TWIN_CONTEXT_TAG
{
    IOTHUB_CLIENT_DEVICE_TWIN_CALLBACK on_get_twin_completed_callback;
    void* user_context;
} AMQP_TRANSPORT_GET_TWIN_CONTEXT;

// ---------- General Helpers ---------- //

static void free_proxy_data(AMQP_TRANSPORT_INSTANCE* amqp_transport_instance)
{
    if (amqp_transport_instance->http_proxy_hostname != NULL)
    {
        free(amqp_transport_instance->http_proxy_hostname);
        amqp_transport_instance->http_proxy_hostname = NULL;
    }

    if (amqp_transport_instance->http_proxy_username != NULL)
    {
        free(amqp_transport_instance->http_proxy_username);
        amqp_transport_instance->http_proxy_username = NULL;
    }

    if (amqp_transport_instance->http_proxy_password != NULL)
    {
        free(amqp_transport_instance->http_proxy_password);
        amqp_transport_instance->http_proxy_password = NULL;
    }
}

static STRING_HANDLE get_target_iothub_fqdn(const IOTHUBTRANSPORT_CONFIG* config)
{
    STRING_HANDLE fqdn;

    if (config->upperConfig->protocolGatewayHostName == NULL)
    {
        if ((fqdn = STRING_construct_sprintf("%s.%s", config->upperConfig->iotHubName, config->upperConfig->iotHubSuffix)) == NULL)
        {
            LogError("Failed to copy iotHubName and iotHubSuffix (STRING_construct_sprintf failed)");
        }
    }
    else if ((fqdn = STRING_construct(config->upperConfig->protocolGatewayHostName)) == NULL)
    {
        LogError("Failed to copy protocolGatewayHostName (STRING_construct failed)");
    }

    return fqdn;
}

static void update_state(AMQP_TRANSPORT_INSTANCE* transport_instance, AMQP_TRANSPORT_STATE new_state)
{
    transport_instance->state = new_state;
}

static void reset_retry_control(AMQP_TRANSPORT_DEVICE_INSTANCE* registered_device)
{
    retry_control_reset(registered_device->transport_instance->connection_retry_control);
}

// ---------- Register/Unregister Helpers ---------- //

static void internal_destroy_amqp_device_instance(AMQP_TRANSPORT_DEVICE_INSTANCE *trdev_inst)
{
    if (trdev_inst->methods_handle != NULL)
    {
        iothubtransportamqp_methods_destroy(trdev_inst->methods_handle);
    }

    if (trdev_inst->device_handle != NULL)
    {
        amqp_device_destroy(trdev_inst->device_handle);
    }

    if (trdev_inst->device_id != NULL)
    {
        STRING_delete(trdev_inst->device_id);
    }

    free(trdev_inst);
}

static DEVICE_AUTH_MODE get_authentication_mode(const IOTHUB_DEVICE_CONFIG* device)
{
    DEVICE_AUTH_MODE result;

    if (device->deviceKey != NULL || device->deviceSasToken != NULL)
    {
        result = DEVICE_AUTH_MODE_CBS;
    }
    else if (IoTHubClient_Auth_Get_Credential_Type(device->authorization_module) == IOTHUB_CREDENTIAL_TYPE_DEVICE_AUTH)
    {
        result = DEVICE_AUTH_MODE_CBS;
    }
    else
    {
        result = DEVICE_AUTH_MODE_X509;
    }
    return result;
}

static void raise_connection_status_callback_retry_expired(const void* item, const void* action_context, bool* continue_processing)
{
    (void)action_context;

    AMQP_TRANSPORT_DEVICE_INSTANCE* registered_device = (AMQP_TRANSPORT_DEVICE_INSTANCE*)item;

    registered_device->transport_callbacks.connection_status_cb(IOTHUB_CLIENT_CONNECTION_UNAUTHENTICATED, IOTHUB_CLIENT_CONNECTION_RETRY_EXPIRED, registered_device->transport_ctx);

    *continue_processing = true;
}

// @brief
//     Saves the new state, if it is different than the previous one.
static void on_device_state_changed_callback(void* context, DEVICE_STATE previous_state, DEVICE_STATE new_state)
{
    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_061: [If `new_state` is the same as `previous_state`, on_device_state_changed_callback shall return]
    if (context != NULL && new_state != previous_state)
    {
        AMQP_TRANSPORT_DEVICE_INSTANCE* registered_device = (AMQP_TRANSPORT_DEVICE_INSTANCE*)context;
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_062: [If `new_state` shall be saved into the `registered_device` instance]
        registered_device->device_state = new_state;
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_063: [If `registered_device->time_of_last_state_change` shall be set using get_time()]
        registered_device->time_of_last_state_change = get_time(NULL);

        if (new_state == DEVICE_STATE_STARTED)
        {
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_127: [If `new_state` is DEVICE_STATE_STARTED, retry_control_reset() shall be invoked passing `instance->connection_retry_control`]
            reset_retry_control(registered_device);

            // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_120: [If `new_state` is DEVICE_STATE_STARTED, IoTHubClientCore_LL_ConnectionStatusCallBack shall be invoked with IOTHUB_CLIENT_CONNECTION_AUTHENTICATED and IOTHUB_CLIENT_CONNECTION_OK]
            registered_device->transport_callbacks.connection_status_cb(IOTHUB_CLIENT_CONNECTION_AUTHENTICATED, IOTHUB_CLIENT_CONNECTION_OK, registered_device->transport_ctx);
        }
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_121: [If `new_state` is DEVICE_STATE_STOPPED, IoTHubClientCore_LL_ConnectionStatusCallBack shall be invoked with IOTHUB_CLIENT_CONNECTION_UNAUTHENTICATED and IOTHUB_CLIENT_CONNECTION_OK]
        else if (new_state == DEVICE_STATE_STOPPED)
        {
            if (registered_device->transport_instance->state == AMQP_TRANSPORT_STATE_CONNECTED ||
                registered_device->transport_instance->state == AMQP_TRANSPORT_STATE_BEING_DESTROYED)
            {
                registered_device->transport_callbacks.connection_status_cb(IOTHUB_CLIENT_CONNECTION_UNAUTHENTICATED, IOTHUB_CLIENT_CONNECTION_OK, registered_device->transport_ctx);
            }
        }
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_122: [If `new_state` is DEVICE_STATE_ERROR_AUTH, IoTHubClientCore_LL_ConnectionStatusCallBack shall be invoked with IOTHUB_CLIENT_CONNECTION_UNAUTHENTICATED and IOTHUB_CLIENT_CONNECTION_BAD_CREDENTIAL]
        else if (new_state == DEVICE_STATE_ERROR_AUTH)
        {
            registered_device->transport_callbacks.connection_status_cb(IOTHUB_CLIENT_CONNECTION_UNAUTHENTICATED, IOTHUB_CLIENT_CONNECTION_BAD_CREDENTIAL, registered_device->transport_ctx);
        }
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_123: [If `new_state` is DEVICE_STATE_ERROR_AUTH_TIMEOUT or DEVICE_STATE_ERROR_MSG, IoTHubClientCore_LL_ConnectionStatusCallBack shall be invoked with IOTHUB_CLIENT_CONNECTION_UNAUTHENTICATED and IOTHUB_CLIENT_CONNECTION_COMMUNICATION_ERROR]
        else if (new_state == DEVICE_STATE_ERROR_AUTH_TIMEOUT || new_state == DEVICE_STATE_ERROR_MSG)
        {
            registered_device->transport_callbacks.connection_status_cb(IOTHUB_CLIENT_CONNECTION_UNAUTHENTICATED, IOTHUB_CLIENT_CONNECTION_COMMUNICATION_ERROR, registered_device->transport_ctx);
        }
    }
}

// @brief    Auxiliary function to be used to find a device in the registered_devices list.
// @returns  true if the device ids match, false otherwise.
static bool find_device_by_id_callback(LIST_ITEM_HANDLE list_item, const void* match_context)
{
    bool result;

    if (match_context == NULL)
    {
        result = false;
    }
    else
    {
        AMQP_TRANSPORT_DEVICE_INSTANCE* device_instance = (AMQP_TRANSPORT_DEVICE_INSTANCE*)singlylinkedlist_item_get_value(list_item);

        if (device_instance == NULL ||
            device_instance->device_id == NULL ||
            STRING_c_str(device_instance->device_id) != match_context)
        {
            result = false;
        }
        else
        {
            result = true;
        }
    }

    return result;
}

// @brief       Verifies if a device is already registered within the transport that owns the list of registered devices.
// @remarks     Returns the correspoding LIST_ITEM_HANDLE in registered_devices, if found.
// @returns     true if the device is already in the list, false otherwise.
static bool is_device_registered_ex(SINGLYLINKEDLIST_HANDLE registered_devices, const char* device_id, LIST_ITEM_HANDLE *list_item)
{
    return ((*list_item = singlylinkedlist_find(registered_devices, find_device_by_id_callback, device_id)) != NULL ? 1 : 0);
}

// @brief       Verifies if a device is already registered within the transport that owns the list of registered devices.
// @returns     true if the device is already in the list, false otherwise.
static bool is_device_registered(AMQP_TRANSPORT_DEVICE_INSTANCE* amqp_device_instance)
{
    if (amqp_device_instance == NULL)
    {
        LogError("AMQP_TRANSPORT_DEVICE_INSTANCE is NULL");
        return false;
    }
    else
    {
        LIST_ITEM_HANDLE list_item;
        const char* device_id = STRING_c_str(amqp_device_instance->device_id);
        return is_device_registered_ex(amqp_device_instance->transport_instance->registered_devices, device_id, &list_item);
    }
}

static size_t get_number_of_registered_devices(AMQP_TRANSPORT_INSTANCE* transport)
{
    size_t result = 0;

    LIST_ITEM_HANDLE list_item = singlylinkedlist_get_head_item(transport->registered_devices);

    while (list_item != NULL)
    {
        result++;
        list_item = singlylinkedlist_get_next_item(list_item);
    }

    return result;
}


// ---------- Callbacks ---------- //

static MESSAGE_CALLBACK_INFO* MESSAGE_CALLBACK_INFO_Create(IOTHUB_MESSAGE_HANDLE message, DEVICE_MESSAGE_DISPOSITION_INFO* disposition_info, AMQP_TRANSPORT_DEVICE_INSTANCE* device_state)
{
    MESSAGE_CALLBACK_INFO* result;

    if ((result = (MESSAGE_CALLBACK_INFO*)malloc(sizeof(MESSAGE_CALLBACK_INFO))) == NULL)
    {
        LogError("Failed creating MESSAGE_CALLBACK_INFO (malloc failed)");
    }
    else
    {
        memset(result, 0, sizeof(MESSAGE_CALLBACK_INFO));
        MESSAGE_DISPOSITION_CONTEXT* tc;

        if ((tc = (MESSAGE_DISPOSITION_CONTEXT*)malloc(sizeof(MESSAGE_DISPOSITION_CONTEXT))) == NULL)
        {
            LogError("Failed creating MESSAGE_DISPOSITION_CONTEXT (malloc failed)");
            free(result);
            result = NULL;
        }
        else
        {
            memset(tc, 0, sizeof(MESSAGE_DISPOSITION_CONTEXT));
            if (mallocAndStrcpy_s(&(tc->link_name), disposition_info->source) == 0)
            {
                tc->device_state = device_state;
                tc->message_id = (delivery_number)disposition_info->message_id;

                result->messageHandle = message;
                result->transportContext = tc;
            }
            else
            {
                LogError("Failed creating MESSAGE_CALLBACK_INFO (mallocAndStrcyp_s failed)");
                free(tc);
                free(result);
                result = NULL;
            }
        }
    }

    return result;
}

static void MESSAGE_CALLBACK_INFO_Destroy(MESSAGE_CALLBACK_INFO* message_callback_info)
{
    if (message_callback_info->transportContext != NULL)
    {
        free(message_callback_info->transportContext->link_name);
        free(message_callback_info->transportContext);
    }
    free(message_callback_info);
}

static DEVICE_MESSAGE_DISPOSITION_RESULT get_device_disposition_result_from(IOTHUBMESSAGE_DISPOSITION_RESULT iothubclient_disposition_result)
{
    DEVICE_MESSAGE_DISPOSITION_RESULT device_disposition_result;

    if (iothubclient_disposition_result == IOTHUBMESSAGE_ACCEPTED)
    {
        device_disposition_result = DEVICE_MESSAGE_DISPOSITION_RESULT_ACCEPTED;
    }
    else if (iothubclient_disposition_result == IOTHUBMESSAGE_ABANDONED)
    {
        device_disposition_result = DEVICE_MESSAGE_DISPOSITION_RESULT_RELEASED;
    }
    else if (iothubclient_disposition_result == IOTHUBMESSAGE_REJECTED)
    {
        device_disposition_result = DEVICE_MESSAGE_DISPOSITION_RESULT_REJECTED;
    }
    else
    {
        LogError("Failed getting corresponding DEVICE_MESSAGE_DISPOSITION_RESULT for IOTHUBMESSAGE_DISPOSITION_RESULT (%d is not supported)", iothubclient_disposition_result);
        device_disposition_result = DEVICE_MESSAGE_DISPOSITION_RESULT_RELEASED;
    }

    return device_disposition_result;
}

static DEVICE_MESSAGE_DISPOSITION_RESULT on_message_received(IOTHUB_MESSAGE_HANDLE message, DEVICE_MESSAGE_DISPOSITION_INFO* disposition_info, void* context)
{
    AMQP_TRANSPORT_DEVICE_INSTANCE* amqp_device_instance = (AMQP_TRANSPORT_DEVICE_INSTANCE*)context;
    DEVICE_MESSAGE_DISPOSITION_RESULT device_disposition_result;
    MESSAGE_CALLBACK_INFO* message_data;

    if ((message_data = MESSAGE_CALLBACK_INFO_Create(message, disposition_info, amqp_device_instance)) == NULL)
    {
        LogError("Failed processing message received (failed to assemble callback info)");
        device_disposition_result = DEVICE_MESSAGE_DISPOSITION_RESULT_RELEASED;
    }
    else
    {
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_089: [IoTHubClientCore_LL_MessageCallback() shall be invoked passing the client and the incoming message handles as parameters]
        if (amqp_device_instance->transport_callbacks.msg_cb(message_data, amqp_device_instance->transport_ctx) != true)
        {
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_090: [If IoTHubClientCore_LL_MessageCallback() fails, on_message_received_callback shall return DEVICE_MESSAGE_DISPOSITION_RESULT_RELEASED]
            LogError("Failed processing message received (IoTHubClientCore_LL_MessageCallback failed)");
            IoTHubMessage_Destroy(message);
            MESSAGE_CALLBACK_INFO_Destroy(message_data);
            device_disposition_result = DEVICE_MESSAGE_DISPOSITION_RESULT_RELEASED;
        }
        else
        {
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_091: [If IoTHubClientCore_LL_MessageCallback() succeeds, on_message_received_callback shall return DEVICE_MESSAGE_DISPOSITION_RESULT_NONE]
            device_disposition_result = DEVICE_MESSAGE_DISPOSITION_RESULT_NONE;
        }
    }

    return device_disposition_result;
}

static void on_methods_error(void* context)
{
    /* Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_030: [ `on_methods_error` shall do nothing. ]*/
    (void)context;
}

static void on_methods_unsubscribed(void* context)
{
    /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_12_001: [ `on_methods_unsubscribed` calls iothubtransportamqp_methods_unsubscribe. ]*/
    AMQP_TRANSPORT_DEVICE_INSTANCE* device_state = (AMQP_TRANSPORT_DEVICE_INSTANCE*)context;

    iothubtransportamqp_methods_unsubscribe(device_state->methods_handle);
    device_state->subscribed_for_methods = false;
}

static int on_method_request_received(void* context, const char* method_name, const unsigned char* request, size_t request_size, IOTHUBTRANSPORT_AMQP_METHOD_HANDLE method_handle)
{
    int result;
    AMQP_TRANSPORT_DEVICE_INSTANCE* device_state = (AMQP_TRANSPORT_DEVICE_INSTANCE*)context;

    /* Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_017: [ `on_methods_request_received` shall call the `IoTHubClientCore_LL_DeviceMethodComplete` passing the method name, request buffer and size and the newly created BUFFER handle. ]*/
    /* Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_022: [ The status code shall be the return value of the call to `IoTHubClientCore_LL_DeviceMethodComplete`. ]*/
    if (device_state->transport_callbacks.method_complete_cb(method_name, request, request_size, (void*)method_handle, device_state->transport_ctx) != 0)
    {
        LogError("Failure: IoTHubClientCore_LL_DeviceMethodComplete");
        result = MU_FAILURE;
    }
    else
    {
        result = 0;
    }
    return result;
}

static int subscribe_methods(AMQP_TRANSPORT_DEVICE_INSTANCE* deviceState)
{
    int result;

    if (deviceState->subscribed_for_methods)
    {
        result = 0;
    }
    else
    {
        SESSION_HANDLE session_handle;

        if ((amqp_connection_get_session_handle(deviceState->transport_instance->amqp_connection, &session_handle)) != RESULT_OK)
        {
            LogError("Device '%s' failed subscribing for methods (failed getting session handle)", STRING_c_str(deviceState->device_id));
            result = MU_FAILURE;
        }
        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_024: [ If the device authentication status is AUTHENTICATION_STATUS_OK and `IoTHubTransport_AMQP_Common_Subscribe_DeviceMethod` was called to register for methods, `IoTHubTransport_AMQP_Common_DoWork` shall call `iothubtransportamqp_methods_subscribe`. ]*/
        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_027: [ The current session handle shall be passed to `iothubtransportamqp_methods_subscribe`. ]*/
        else if (iothubtransportamqp_methods_subscribe(deviceState->methods_handle, session_handle, on_methods_error, deviceState, on_method_request_received, deviceState, on_methods_unsubscribed, deviceState) != 0)
        {
            LogError("Cannot subscribe for methods");
            result = MU_FAILURE;
        }
        else
        {
            deviceState->subscribed_for_methods = true;
            result = 0;
        }
    }

    return result;
}

static void on_device_send_twin_update_complete_callback(DEVICE_TWIN_UPDATE_RESULT result, int status_code, void* context)
{
    (void)result;

    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_151: [If `context` is NULL, the callback shall return]
    if (context == NULL)
    {
        LogError("Invalid argument (context is NULL)");
    }
    else
    {
        AMQP_TRANSPORT_DEVICE_TWIN_CONTEXT* dev_twin_ctx = (AMQP_TRANSPORT_DEVICE_TWIN_CONTEXT*)context;

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_152: [`IoTHubClientCore_LL_ReportedStateComplete` shall be invoked passing `status_code` and `context` details]
        dev_twin_ctx->transport_callbacks.twin_rpt_state_complete_cb(dev_twin_ctx->item_id, status_code, dev_twin_ctx->transport_ctx);

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_153: [The memory allocated for `context` shall be released]
        free(dev_twin_ctx);
    }
}

static void on_device_twin_update_received_callback(DEVICE_TWIN_UPDATE_TYPE update_type, const unsigned char* message, size_t length, void* context)
{
    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_137: [If `context` is NULL, the callback shall return.]
    if (context == NULL)
    {
        LogError("Invalid argument (context is NULL)");
    }
    else
    {
        AMQP_TRANSPORT_DEVICE_INSTANCE* registered_device = (AMQP_TRANSPORT_DEVICE_INSTANCE*)context;

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_138: [If `update_type` is DEVICE_TWIN_UPDATE_TYPE_PARTIAL IoTHubClientCore_LL_RetrievePropertyComplete shall be invoked passing `context` as handle, `DEVICE_TWIN_UPDATE_PARTIAL`, `payload` and `size`.]
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_139: [If `update_type` is DEVICE_TWIN_UPDATE_TYPE_COMPLETE IoTHubClientCore_LL_RetrievePropertyComplete shall be invoked passing `context` as handle, `DEVICE_TWIN_UPDATE_COMPLETE`, `payload` and `size`.]
        registered_device->transport_instance->transport_callbacks.twin_retrieve_prop_complete_cb((update_type == DEVICE_TWIN_UPDATE_TYPE_COMPLETE ? DEVICE_TWIN_UPDATE_COMPLETE : DEVICE_TWIN_UPDATE_PARTIAL),
                message, length, registered_device->transport_instance->transport_ctx);
    }
}

static void on_device_get_twin_completed_callback(DEVICE_TWIN_UPDATE_TYPE update_type, const unsigned char* message, size_t length, void* context)
{
    (void)update_type;

    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_158: [ If `message` or `context` are NULL, the callback shall do nothing and return. ]
    if (message == NULL || context == NULL)
    {
        LogError("Invalid argument (message=%p, context=%p)", message, context);
    }
    else
    {
        AMQP_TRANSPORT_GET_TWIN_CONTEXT* getTwinCtx = (AMQP_TRANSPORT_GET_TWIN_CONTEXT*)context;

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_159: [ `message` and `length` shall be passed to the callback `completionCallback` provided in IoTHubTransport_AMQP_Common_GetTwinAsync. ]
        getTwinCtx->on_get_twin_completed_callback(DEVICE_TWIN_UPDATE_COMPLETE, message, length, getTwinCtx->user_context);

        free(getTwinCtx);
    }
}


// ---------- Underlying TLS I/O Helpers ---------- //

// @brief
//     Retrieves the options of the current underlying TLS I/O instance and saves in the transport instance.
// @remarks
//     This is used when the new underlying I/O transport (TLS I/O, or WebSockets, etc) needs to be recreated,
//     and the options previously set must persist.
//
//     If no TLS I/O instance was created yet, results in failure.
// @returns
//     0 if succeeds, non-zero otherwise.
static int save_underlying_io_transport_options(AMQP_TRANSPORT_INSTANCE* transport_instance)
{
    int result;

    if (transport_instance->tls_io == NULL)
    {
        LogError("failed saving underlying I/O transport options (tls_io instance is NULL)");
        result = MU_FAILURE;
    }
    else
    {
        OPTIONHANDLER_HANDLE fresh_options;

        if ((fresh_options = xio_retrieveoptions(transport_instance->tls_io)) == NULL)
        {
            LogError("failed saving underlying I/O transport options (tls_io instance is NULL)");
            result = MU_FAILURE;
        }
        else
        {
            OPTIONHANDLER_HANDLE previous_options = transport_instance->saved_tls_options;
            transport_instance->saved_tls_options = fresh_options;

            if (previous_options != NULL)
            {
                OptionHandler_Destroy(previous_options);
            }

            result = RESULT_OK;
        }
    }

    return result;
}

static void destroy_underlying_io_transport_options(AMQP_TRANSPORT_INSTANCE* transport_instance)
{
    if (transport_instance->saved_tls_options != NULL)
    {
        OptionHandler_Destroy(transport_instance->saved_tls_options);
        transport_instance->saved_tls_options = NULL;
    }
}

// @brief
//     Applies TLS I/O options if previously saved to a new TLS I/O instance.
// @returns
//     0 if succeeds, non-zero otherwise.
static int restore_underlying_io_transport_options(AMQP_TRANSPORT_INSTANCE* transport_instance, XIO_HANDLE xio_handle)
{
    int result;

    if (transport_instance->saved_tls_options == NULL)
    {
        result = RESULT_OK;
    }
    else
    {
        if (OptionHandler_FeedOptions(transport_instance->saved_tls_options, xio_handle) != OPTIONHANDLER_OK)
        {
            LogError("Failed feeding existing options to new TLS instance.");
            result = MU_FAILURE;
        }
        else
        {
            result = RESULT_OK;
        }
    }

    return result;
}

// @brief    Destroys the XIO_HANDLE obtained with underlying_io_transport_provider(), saving its options beforehand.
static void destroy_underlying_io_transport(AMQP_TRANSPORT_INSTANCE* transport_instance)
{
    if (transport_instance->tls_io != NULL)
    {
        xio_destroy(transport_instance->tls_io);
        transport_instance->tls_io = NULL;
    }
}

// @brief    Invokes underlying_io_transport_provider() and retrieves a new XIO_HANDLE to use for I/O (TLS, or websockets, or w/e is supported).
// @param    xio_handle: if successfull, set with the new XIO_HANDLE acquired; not changed otherwise.
// @returns  0 if successfull, non-zero otherwise.
static int get_new_underlying_io_transport(AMQP_TRANSPORT_INSTANCE* transport_instance, XIO_HANDLE *xio_handle)
{
    int result;
    AMQP_TRANSPORT_PROXY_OPTIONS amqp_transport_proxy_options;

    /* Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_041: [ If the `proxy_data` option has been set, the proxy options shall be filled in the argument `amqp_transport_proxy_options` when calling the function `underlying_io_transport_provider()` to obtain the underlying IO handle. ]*/
    amqp_transport_proxy_options.host_address = transport_instance->http_proxy_hostname;
    amqp_transport_proxy_options.port = transport_instance->http_proxy_port;
    amqp_transport_proxy_options.username = transport_instance->http_proxy_username;
    amqp_transport_proxy_options.password = transport_instance->http_proxy_password;

    /* Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_042: [ If no `proxy_data` option has been set, NULL shall be passed as the argument `amqp_transport_proxy_options` when calling the function `underlying_io_transport_provider()`. ]*/
    if ((*xio_handle = transport_instance->underlying_io_transport_provider(STRING_c_str(transport_instance->iothub_host_fqdn), amqp_transport_proxy_options.host_address == NULL ? NULL : &amqp_transport_proxy_options)) == NULL)
    {
        LogError("Failed to obtain a TLS I/O transport layer (underlying_io_transport_provider() failed)");
        result = MU_FAILURE;
    }
    else
    {
        // If this is the HSM x509 ECC certificate
        if (transport_instance->authorization_module != NULL && IoTHubClient_Auth_Get_Credential_Type(transport_instance->authorization_module) == IOTHUB_CREDENTIAL_TYPE_X509_ECC)
        {
            // Set the xio_handle
            if (IoTHubClient_Auth_Set_xio_Certificate(transport_instance->authorization_module, *xio_handle) != 0)
            {
                LogError("Unable to create the lower level TLS layer.");
                result = MU_FAILURE;
            }
            else
            {
                result = RESULT_OK;
            }
        }
        else
        {
            result = RESULT_OK;
        }

        if (restore_underlying_io_transport_options(transport_instance, *xio_handle) != RESULT_OK)
        {
            /*pessimistically hope TLS will fail, be recreated and options re-given*/
            LogError("Failed to apply options previous saved to new underlying I/O transport instance.");
        }
    }

    return result;
}


// ---------- AMQP connection establishment/tear-down, connectry retry ---------- //

static void on_amqp_connection_state_changed(const void* context, AMQP_CONNECTION_STATE previous_state, AMQP_CONNECTION_STATE new_state)
{
    if (context != NULL && new_state != previous_state)
    {
        AMQP_TRANSPORT_INSTANCE* transport_instance = (AMQP_TRANSPORT_INSTANCE*)context;

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_059: [`new_state` shall be saved in to the transport instance]
        transport_instance->amqp_connection_state = new_state;

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_060: [If `new_state` is AMQP_CONNECTION_STATE_ERROR, the connection shall be flagged as faulty (so the connection retry logic can be triggered)]
        if (new_state == AMQP_CONNECTION_STATE_ERROR)
        {
            LogError("Transport received an ERROR from the amqp_connection (state changed %s -> %s); it will be flagged for connection retry.", MU_ENUM_TO_STRING(AMQP_CONNECTION_STATE, previous_state), MU_ENUM_TO_STRING(AMQP_CONNECTION_STATE, new_state));
            transport_instance->transport_callbacks.connection_status_cb(IOTHUB_CLIENT_CONNECTION_UNAUTHENTICATED, IOTHUB_CLIENT_CONNECTION_NO_NETWORK, transport_instance->transport_ctx);
            update_state(transport_instance, AMQP_TRANSPORT_STATE_RECONNECTION_REQUIRED);
        }
        else if (new_state == AMQP_CONNECTION_STATE_OPENED)
        {
            update_state(transport_instance, AMQP_TRANSPORT_STATE_CONNECTED);
        }
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_115: [If the AMQP connection is closed by the service side, the connection retry logic shall be triggered]
        else if (new_state == AMQP_CONNECTION_STATE_CLOSED && previous_state == AMQP_CONNECTION_STATE_OPENED && transport_instance->state != AMQP_TRANSPORT_STATE_BEING_DESTROYED)
        {
            LogError("amqp_connection was closed unexpectedly; connection retry will be triggered.");

            update_state(transport_instance, AMQP_TRANSPORT_STATE_RECONNECTION_REQUIRED);
        }
    }
}

static int establish_amqp_connection(AMQP_TRANSPORT_INSTANCE* transport_instance)
{
    int result;

    if (transport_instance->preferred_authentication_mode == AMQP_TRANSPORT_AUTHENTICATION_MODE_NOT_SET)
    {
        LogError("Failed establishing connection (transport doesn't have a preferred authentication mode set; unexpected!).");
        result = MU_FAILURE;
    }
    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_023: [If `instance->tls_io` is NULL, it shall be set invoking instance->underlying_io_transport_provider()]
    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_025: [When `instance->tls_io` is created, it shall be set with `instance->saved_tls_options` using OptionHandler_FeedOptions()]
    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_111: [If OptionHandler_FeedOptions() fails, it shall be ignored]
    else if (transport_instance->tls_io == NULL &&
        get_new_underlying_io_transport(transport_instance, &transport_instance->tls_io) != RESULT_OK)
    {
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_024: [If instance->underlying_io_transport_provider() fails, IoTHubTransport_AMQP_Common_DoWork shall fail and return]
        LogError("Failed establishing connection (failed to obtain a TLS I/O transport layer).");
        result = MU_FAILURE;
    }
    else
    {
        AMQP_CONNECTION_CONFIG amqp_connection_config;
        amqp_connection_config.iothub_host_fqdn = STRING_c_str(transport_instance->iothub_host_fqdn);
        amqp_connection_config.underlying_io_transport = transport_instance->tls_io;
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_029: [`instance->is_trace_on` shall be set into `AMQP_CONNECTION_CONFIG->is_trace_on`]
        amqp_connection_config.is_trace_on = transport_instance->is_trace_on;
        amqp_connection_config.on_state_changed_callback = on_amqp_connection_state_changed;
        amqp_connection_config.on_state_changed_context = transport_instance;
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_12_003: [AMQP connection will be configured using the `svc2cl_keep_alive_timeout_secs` value from SetOption ]
        amqp_connection_config.svc2cl_keep_alive_timeout_secs = transport_instance->svc2cl_keep_alive_timeout_secs;
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_99_001: [AMQP connection will be configured using the `remote_idle_timeout_ratio` value from SetOption ]
        amqp_connection_config.cl2svc_keep_alive_send_ratio = transport_instance->cl2svc_keep_alive_send_ratio;

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_027: [If `transport->preferred_authentication_method` is CBS, AMQP_CONNECTION_CONFIG shall be set with `create_sasl_io` = true and `create_cbs_connection` = true]
        if (transport_instance->preferred_authentication_mode == AMQP_TRANSPORT_AUTHENTICATION_MODE_CBS)
        {
            amqp_connection_config.create_sasl_io = true;
            amqp_connection_config.create_cbs_connection = true;
        }
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_028: [If `transport->preferred_credential_method` is X509, AMQP_CONNECTION_CONFIG shall be set with `create_sasl_io` = false and `create_cbs_connection` = false]
        else if (transport_instance->preferred_authentication_mode == AMQP_TRANSPORT_AUTHENTICATION_MODE_X509)
        {
            amqp_connection_config.create_sasl_io = false;
            amqp_connection_config.create_cbs_connection = false;
        }
        // If new AMQP_TRANSPORT_AUTHENTICATION_MODE values are added, they need to be covered here.

        transport_instance->amqp_connection_state = AMQP_CONNECTION_STATE_CLOSED;

        if (transport_instance->state == AMQP_TRANSPORT_STATE_READY_FOR_RECONNECTION)
        {
            update_state(transport_instance, AMQP_TRANSPORT_STATE_RECONNECTING);
        }
        else
        {
            update_state(transport_instance, AMQP_TRANSPORT_STATE_CONNECTING);
        }

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_026: [If `transport->connection` is NULL, it shall be created using amqp_connection_create()]
        if ((transport_instance->amqp_connection = amqp_connection_create(&amqp_connection_config)) == NULL)
        {
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_030: [If amqp_connection_create() fails, IoTHubTransport_AMQP_Common_DoWork shall fail and return]
            LogError("Failed establishing connection (failed to create the amqp_connection instance).");
            result = MU_FAILURE;
        }
        else
        {
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_110: [If amqp_connection_create() succeeds, IoTHubTransport_AMQP_Common_DoWork shall proceed to invoke amqp_connection_do_work]
            result = RESULT_OK;
        }
    }

    return result;
}

static void prepare_device_for_connection_retry(AMQP_TRANSPORT_DEVICE_INSTANCE* registered_device)
{
    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_032: [ Each `instance->registered_devices` shall unsubscribe from receiving C2D method requests by calling `iothubtransportamqp_methods_unsubscribe`]
    iothubtransportamqp_methods_unsubscribe(registered_device->methods_handle);
    registered_device->subscribed_for_methods = 0;

    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_031: [amqp_device_stop() shall be invoked on all `instance->registered_devices` that are not already stopped]
    if (registered_device->device_state != DEVICE_STATE_STOPPED)
    {
        if (amqp_device_stop(registered_device->device_handle) != RESULT_OK)
        {
            LogError("Failed preparing device '%s' for connection retry (amqp_device_stop failed)", STRING_c_str(registered_device->device_id));
        }
    }

    registered_device->number_of_previous_failures = 0;
    registered_device->number_of_send_event_complete_failures = 0;
}

static void prepare_for_connection_retry(AMQP_TRANSPORT_INSTANCE* transport_instance)
{
    LogInfo("Preparing transport for re-connection");

    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_034: [`instance->tls_io` options shall be saved on `instance->saved_tls_options` using xio_retrieveoptions()]
    if (save_underlying_io_transport_options(transport_instance) != RESULT_OK)
    {
        LogError("Failed saving TLS I/O options while preparing for connection retry; failure will be ignored");
    }

    LIST_ITEM_HANDLE list_item = singlylinkedlist_get_head_item(transport_instance->registered_devices);

    while (list_item != NULL)
    {
        AMQP_TRANSPORT_DEVICE_INSTANCE* registered_device = (AMQP_TRANSPORT_DEVICE_INSTANCE*)singlylinkedlist_item_get_value(list_item);

        if (registered_device == NULL)
        {
            LogError("Failed preparing device for connection retry (singlylinkedlist_item_get_value failed)");
        }
        else
        {
            prepare_device_for_connection_retry(registered_device);
        }

        list_item = singlylinkedlist_get_next_item(list_item);
    }

    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_033: [`instance->connection` shall be destroyed using amqp_connection_destroy()]
    amqp_connection_destroy(transport_instance->amqp_connection);
    transport_instance->amqp_connection = NULL;
    transport_instance->amqp_connection_state = AMQP_CONNECTION_STATE_CLOSED;

    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_035: [`instance->tls_io` shall be destroyed using xio_destroy()]
    destroy_underlying_io_transport(transport_instance);

    update_state(transport_instance, AMQP_TRANSPORT_STATE_READY_FOR_RECONNECTION);
}


// @brief    Verifies if the crendentials used by the device match the requirements and authentication mode currently supported by the transport.
// @returns  true if credentials are good, false otherwise.
static bool is_device_credential_acceptable(const IOTHUB_DEVICE_CONFIG* device_config, AMQP_TRANSPORT_AUTHENTICATION_MODE preferred_authentication_mode)
{
    bool result;

    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_03_003: [IoTHubTransport_AMQP_Common_Register shall return NULL if both deviceKey and deviceSasToken are not NULL.]
    if ((device_config->deviceSasToken != NULL) && (device_config->deviceKey != NULL))
    {
        LogError("Credential of device '%s' is not acceptable (must provide EITHER deviceSasToken OR deviceKey)", device_config->deviceId);
        result = false;
    }
    else if (preferred_authentication_mode == AMQP_TRANSPORT_AUTHENTICATION_MODE_NOT_SET)
    {
        result = true;
    }
    else if (preferred_authentication_mode == AMQP_TRANSPORT_AUTHENTICATION_MODE_X509 && (device_config->deviceKey != NULL || device_config->deviceSasToken != NULL))
    {
        LogError("Credential of device '%s' is not acceptable (transport is using X509 certificate authentication, but device config contains deviceKey or sasToken)", device_config->deviceId);
        result = false;
    }
    else if (preferred_authentication_mode != AMQP_TRANSPORT_AUTHENTICATION_MODE_X509 && (device_config->deviceKey == NULL && device_config->deviceSasToken == NULL))
    {
        LogError("Credential of device '%s' is not acceptable (transport is using CBS authentication, but device config does not contain deviceKey nor sasToken)", device_config->deviceId);
        result = false;
    }
    else
    {
        result = true;
    }

    return result;
}



//---------- DoWork Helpers ----------//

static IOTHUB_MESSAGE_LIST* get_next_event_to_send(AMQP_TRANSPORT_DEVICE_INSTANCE* registered_device)
{
    IOTHUB_MESSAGE_LIST* message;

    if (!DList_IsListEmpty(registered_device->waiting_to_send))
    {
        PDLIST_ENTRY list_entry = registered_device->waiting_to_send->Flink;
        message = containingRecord(list_entry, IOTHUB_MESSAGE_LIST, entry);
        (void)DList_RemoveEntryList(list_entry);
    }
    else
    {
        message = NULL;
    }

    return message;
}

// @brief    "Parses" the D2C_EVENT_SEND_RESULT (from iothubtransport_amqp_device module) into a IOTHUB_CLIENT_CONFIRMATION_RESULT.
static IOTHUB_CLIENT_CONFIRMATION_RESULT get_iothub_client_confirmation_result_from(D2C_EVENT_SEND_RESULT result)
{
    IOTHUB_CLIENT_CONFIRMATION_RESULT iothub_send_result;

    switch (result)
    {
        case D2C_EVENT_SEND_COMPLETE_RESULT_OK:
            iothub_send_result = IOTHUB_CLIENT_CONFIRMATION_OK;
            break;
        case D2C_EVENT_SEND_COMPLETE_RESULT_ERROR_CANNOT_PARSE:
        case D2C_EVENT_SEND_COMPLETE_RESULT_ERROR_FAIL_SENDING:
            iothub_send_result = IOTHUB_CLIENT_CONFIRMATION_ERROR;
            break;
        case D2C_EVENT_SEND_COMPLETE_RESULT_ERROR_TIMEOUT:
            iothub_send_result = IOTHUB_CLIENT_CONFIRMATION_MESSAGE_TIMEOUT;
            break;
        case D2C_EVENT_SEND_COMPLETE_RESULT_DEVICE_DESTROYED:
            iothub_send_result = IOTHUB_CLIENT_CONFIRMATION_BECAUSE_DESTROY;
            break;
        case D2C_EVENT_SEND_COMPLETE_RESULT_ERROR_UNKNOWN:
        default:
            iothub_send_result = IOTHUB_CLIENT_CONFIRMATION_ERROR;
            break;
    }

    return iothub_send_result;
}

// @brief
//     Callback function for amqp_device_send_event_async.
static void on_event_send_complete(IOTHUB_MESSAGE_LIST* message, D2C_EVENT_SEND_RESULT result, void* context)
{
    AMQP_TRANSPORT_DEVICE_INSTANCE* registered_device = (AMQP_TRANSPORT_DEVICE_INSTANCE*)context;

    if (result != D2C_EVENT_SEND_COMPLETE_RESULT_OK && result != D2C_EVENT_SEND_COMPLETE_RESULT_DEVICE_DESTROYED)
    {
        registered_device->number_of_send_event_complete_failures++;
    }
    else
    {
        registered_device->number_of_send_event_complete_failures = 0;
    }

    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_056: [If `message->callback` is not NULL, it shall invoked with the `iothub_send_result`]
    if (message->callback != NULL)
    {
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_050: [If result is D2C_EVENT_SEND_COMPLETE_RESULT_OK, `iothub_send_result` shall be set using IOTHUB_CLIENT_CONFIRMATION_OK]
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_051: [If result is D2C_EVENT_SEND_COMPLETE_RESULT_ERROR_CANNOT_PARSE, `iothub_send_result` shall be set using IOTHUB_CLIENT_CONFIRMATION_ERROR]
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_052: [If result is D2C_EVENT_SEND_COMPLETE_RESULT_ERROR_FAIL_SENDING, `iothub_send_result` shall be set using IOTHUB_CLIENT_CONFIRMATION_ERROR]
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_053: [If result is D2C_EVENT_SEND_COMPLETE_RESULT_ERROR_TIMEOUT, `iothub_send_result` shall be set using IOTHUB_CLIENT_CONFIRMATION_MESSAGE_TIMEOUT]
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_054: [If result is D2C_EVENT_SEND_COMPLETE_RESULT_DEVICE_DESTROYED, `iothub_send_result` shall be set using IOTHUB_CLIENT_CONFIRMATION_BECAUSE_DESTROY]
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_055: [If result is D2C_EVENT_SEND_COMPLETE_RESULT_ERROR_UNKNOWN, `iothub_send_result` shall be set using IOTHUB_CLIENT_CONFIRMATION_ERROR]
        IOTHUB_CLIENT_CONFIRMATION_RESULT iothub_send_result = get_iothub_client_confirmation_result_from(result);

        message->callback(iothub_send_result, message->context);
    }

    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_057: [`message->messageHandle` shall be destroyed using IoTHubMessage_Destroy]
    IoTHubMessage_Destroy(message->messageHandle);
    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_058: [`message` shall be destroyed using free]
    free(message);
}

// @brief
//     Gets events from wait to send list and sends to service in the order they were added.
// @returns
//     0 if all events could be sent to the next layer successfully, non-zero otherwise.
static int send_pending_events(AMQP_TRANSPORT_DEVICE_INSTANCE* device_state)
{
    int result;
    IOTHUB_MESSAGE_LIST* message;

    result = RESULT_OK;

    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_047: [If the registered device is started, each event on `registered_device->wait_to_send_list` shall be removed from the list and sent using amqp_device_send_event_async()]
    while ((message = get_next_event_to_send(device_state)) != NULL)
    {
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_048: [amqp_device_send_event_async() shall be invoked passing `on_event_send_complete`]
        if (amqp_device_send_event_async(device_state->device_handle, message, on_event_send_complete, device_state) != RESULT_OK)
        {
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_049: [If amqp_device_send_event_async() fails, `on_event_send_complete` shall be invoked passing EVENT_SEND_COMPLETE_RESULT_ERROR_FAIL_SENDING and return]
            LogError("Device '%s' failed to send message (amqp_device_send_event_async failed)", STRING_c_str(device_state->device_id));
            result = MU_FAILURE;

            on_event_send_complete(message, D2C_EVENT_SEND_COMPLETE_RESULT_ERROR_FAIL_SENDING, device_state);
            break;
        }
    }

    return result;
}

// @brief
//     Auxiliary function for the public DoWork API, performing DoWork activities (authenticate, messaging) for a specific device.
// @requires
//     The transport to have a valid instance of AMQP_CONNECTION (from which to obtain SESSION_HANDLE and CBS_HANDLE)
// @returns
//     0 if no errors occur, non-zero otherwise.
static int IoTHubTransport_AMQP_Common_Device_DoWork(AMQP_TRANSPORT_DEVICE_INSTANCE* registered_device)
{
    int result;

    if (registered_device->device_state != DEVICE_STATE_STARTED)
    {
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_036: [If the device state is DEVICE_STATE_STOPPED, it shall be started]
        if (registered_device->device_state == DEVICE_STATE_STOPPED)
        {
            SESSION_HANDLE session_handle;
            CBS_HANDLE cbs_handle = NULL;

            // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_039: [amqp_connection_get_session_handle() shall be invoked on `instance->connection`]
            if (amqp_connection_get_session_handle(registered_device->transport_instance->amqp_connection, &session_handle) != RESULT_OK)
            {
                // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_040: [If amqp_connection_get_session_handle() fails, IoTHubTransport_AMQP_Common_DoWork shall fail and return]
                LogError("Failed performing DoWork for device '%s' (failed to get the amqp_connection session_handle)", STRING_c_str(registered_device->device_id));
                result = MU_FAILURE;
            }
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_037: [If transport is using CBS authentication, amqp_connection_get_cbs_handle() shall be invoked on `instance->connection`]
            else if (registered_device->transport_instance->preferred_authentication_mode == AMQP_TRANSPORT_AUTHENTICATION_MODE_CBS &&
                amqp_connection_get_cbs_handle(registered_device->transport_instance->amqp_connection, &cbs_handle) != RESULT_OK)
            {
                // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_038: [If amqp_connection_get_cbs_handle() fails, IoTHubTransport_AMQP_Common_DoWork shall fail and return]
                LogError("Failed performing DoWork for device '%s' (failed to get the amqp_connection cbs_handle)", STRING_c_str(registered_device->device_id));
                result = MU_FAILURE;
            }
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_041: [The device handle shall be started using amqp_device_start_async()]
            else if (amqp_device_start_async(registered_device->device_handle, session_handle, cbs_handle) != RESULT_OK)
            {
                // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_042: [If amqp_device_start_async() fails, IoTHubTransport_AMQP_Common_DoWork shall fail and skip to the next registered device]
                LogError("Failed performing DoWork for device '%s' (failed to start device)", STRING_c_str(registered_device->device_id));
                result = MU_FAILURE;
            }
            else
            {
                result = RESULT_OK;
            }
        }
        else if (registered_device->device_state == DEVICE_STATE_STARTING ||
                 registered_device->device_state == DEVICE_STATE_STOPPING)
        {
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_043: [If the device handle is in state DEVICE_STATE_STARTING or DEVICE_STATE_STOPPING, it shall be checked for state change timeout]
            bool is_timed_out;
            if (is_timeout_reached(registered_device->time_of_last_state_change, registered_device->max_state_change_timeout_secs, &is_timed_out) != RESULT_OK)
            {
                LogError("Failed performing DoWork for device '%s' (failed tracking timeout of device %d state)", STRING_c_str(registered_device->device_id), registered_device->device_state);
                registered_device->device_state = DEVICE_STATE_ERROR_AUTH; // if time could not be calculated, the worst must be assumed.
                result = MU_FAILURE;
            }
            else if (is_timed_out)
            {
                // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_044: [If the device times out in state DEVICE_STATE_STARTING or DEVICE_STATE_STOPPING, the registered device shall be marked with failure]
                LogError("Failed performing DoWork for device '%s' (device failed to start or stop within expected timeout)", STRING_c_str(registered_device->device_id));
                registered_device->device_state = DEVICE_STATE_ERROR_AUTH; // this will cause device to be stopped bellow on the next call to this function.
                result = MU_FAILURE;
            }
            else
            {
                result = RESULT_OK;
            }
        }
        else // i.e., DEVICE_STATE_ERROR_AUTH || DEVICE_STATE_ERROR_AUTH_TIMEOUT || DEVICE_STATE_ERROR_MSG
        {
            registered_device->number_of_previous_failures++;

            // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_046: [If the device has failed for MAX_NUMBER_OF_DEVICE_FAILURES in a row, it shall trigger a connection retry on the transport]
            if (registered_device->number_of_previous_failures >= MAX_NUMBER_OF_DEVICE_FAILURES)
            {
                LogError("Failed performing DoWork for device '%s' (device reported state %d; number of previous failures: %lu)",
                    STRING_c_str(registered_device->device_id), (int)registered_device->device_state, (unsigned long)registered_device->number_of_previous_failures);
                result = MU_FAILURE;
            }
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_045: [If the registered device has a failure, it shall be stopped using amqp_device_stop()]
            else if (amqp_device_delayed_stop(registered_device->device_handle, DEFAULT_DEVICE_STOP_DELAY) != RESULT_OK)
            {
                LogError("Failed to stop reset device '%s' (amqp_device_stop failed)", STRING_c_str(registered_device->device_id));
                result = MU_FAILURE;
            }
            else
            {
                result = RESULT_OK;
            }
        }
    }
    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_031: [ Once the device is authenticated, `iothubtransportamqp_methods_subscribe` shall be invoked (subsequent DoWork calls shall not call it if already subscribed). ]
    else if (registered_device->subscribe_methods_needed &&
        !registered_device->subscribed_for_methods &&
        subscribe_methods(registered_device) != RESULT_OK)
    {
        LogError("Failed performing DoWork for device '%s' (failed registering for device methods)", STRING_c_str(registered_device->device_id));
        registered_device->number_of_previous_failures++;
        result = MU_FAILURE;
    }
    else
    {
        if (send_pending_events(registered_device) != RESULT_OK)
        {
            LogError("Failed performing DoWork for device '%s' (failed sending pending events)", STRING_c_str(registered_device->device_id));
            registered_device->number_of_previous_failures++;
            result = MU_FAILURE;
        }
        else
        {
            registered_device->number_of_previous_failures = 0;
            result = RESULT_OK;
        }
    }

    // No harm in invoking this as API will simply exit if the state is not "started".
    amqp_device_do_work(registered_device->device_handle);

    return result;
}


//---------- SetOption-ish Helpers ----------//

// @brief
//     Gets all the device-specific options and replicates them into this new registered device.
// @returns
//     0 if the function succeeds, non-zero otherwise.
static int replicate_device_options_to(AMQP_TRANSPORT_DEVICE_INSTANCE* dev_instance, DEVICE_AUTH_MODE auth_mode)
{
    int result;

    if (amqp_device_set_option(
        dev_instance->device_handle,
        DEVICE_OPTION_EVENT_SEND_TIMEOUT_SECS,
        &dev_instance->transport_instance->option_send_event_timeout_secs) != RESULT_OK)
    {
        LogError("Failed to apply option DEVICE_OPTION_EVENT_SEND_TIMEOUT_SECS to device '%s' (amqp_device_set_option failed)", STRING_c_str(dev_instance->device_id));
        result = MU_FAILURE;
    }
    else if (auth_mode == DEVICE_AUTH_MODE_CBS)
    {
        if (amqp_device_set_option(
            dev_instance->device_handle,
            DEVICE_OPTION_CBS_REQUEST_TIMEOUT_SECS,
            &dev_instance->transport_instance->option_cbs_request_timeout_secs) != RESULT_OK)
        {
            LogError("Failed to apply option DEVICE_OPTION_CBS_REQUEST_TIMEOUT_SECS to device '%s' (amqp_device_set_option failed)", STRING_c_str(dev_instance->device_id));
            result = MU_FAILURE;
        }
        else
        {
            result = RESULT_OK;
        }
    }
    else
    {
        result = RESULT_OK;
    }

    return result;
}

// @brief
//     Translates from the option names supported by iothubtransport_amqp_common to the ones supported by iothubtransport_amqp_device.
static const char* get_device_option_name_from(const char* iothubclient_option_name)
{
    const char* device_option_name;

    if (strcmp(OPTION_CBS_REQUEST_TIMEOUT, iothubclient_option_name) == 0)
    {
        device_option_name = DEVICE_OPTION_CBS_REQUEST_TIMEOUT_SECS;
    }
    else if (strcmp(OPTION_EVENT_SEND_TIMEOUT_SECS, iothubclient_option_name) == 0)
    {
        device_option_name = DEVICE_OPTION_EVENT_SEND_TIMEOUT_SECS;
    }
    else
    {
        device_option_name = NULL;
    }

    return device_option_name;
}

// @brief
//     Auxiliary function invoked by IoTHubTransport_AMQP_Common_SetOption to set an option on every registered device.
// @returns
//     0 if it succeeds, non-zero otherwise.
static int IoTHubTransport_AMQP_Common_Device_SetOption(TRANSPORT_LL_HANDLE handle, const char* option, void* value)
{
    int result;
    const char* device_option;

    if ((device_option = get_device_option_name_from(option)) == NULL)
    {
        LogError("failed setting option '%s' to registered device (could not match name to options supported by device)", option);
        result = MU_FAILURE;
    }
    else
    {
        AMQP_TRANSPORT_INSTANCE* instance = (AMQP_TRANSPORT_INSTANCE*)handle;
        result = RESULT_OK;

        LIST_ITEM_HANDLE list_item = singlylinkedlist_get_head_item(instance->registered_devices);

        while (list_item != NULL)
        {
            AMQP_TRANSPORT_DEVICE_INSTANCE* registered_device;

            if ((registered_device = (AMQP_TRANSPORT_DEVICE_INSTANCE*)singlylinkedlist_item_get_value(list_item)) == NULL)
            {
                LogError("failed setting option '%s' to registered device (singlylinkedlist_item_get_value failed)", option);
                result = MU_FAILURE;
                break;
            }
            else if (amqp_device_set_option(registered_device->device_handle, device_option, value) != RESULT_OK)
            {
                LogError("failed setting option '%s' to registered device '%s' (amqp_device_set_option failed)",
                    option, STRING_c_str(registered_device->device_id));
                result = MU_FAILURE;
                break;
            }

            list_item = singlylinkedlist_get_next_item(list_item);
        }
    }

    return result;
}

static void internal_destroy_instance(AMQP_TRANSPORT_INSTANCE* instance)
{
    if (instance != NULL)
    {
        update_state(instance, AMQP_TRANSPORT_STATE_BEING_DESTROYED);

        if (instance->registered_devices != NULL)
        {
            LIST_ITEM_HANDLE list_item = singlylinkedlist_get_head_item(instance->registered_devices);

            while (list_item != NULL)
            {
                AMQP_TRANSPORT_DEVICE_INSTANCE* registered_device = (AMQP_TRANSPORT_DEVICE_INSTANCE*)singlylinkedlist_item_get_value(list_item);
                list_item = singlylinkedlist_get_next_item(list_item);
                IoTHubTransport_AMQP_Common_Unregister(registered_device);
            }

            singlylinkedlist_destroy(instance->registered_devices);
        }

        if (instance->amqp_connection != NULL)
        {
            amqp_connection_destroy(instance->amqp_connection);
        }

        destroy_underlying_io_transport(instance);
        destroy_underlying_io_transport_options(instance);
        retry_control_destroy(instance->connection_retry_control);

        STRING_delete(instance->iothub_host_fqdn);

        /* SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_043: [ `IoTHubTransport_AMQP_Common_Destroy` shall free the stored proxy options. ]*/
        free_proxy_data(instance);

        free(instance);
    }
}

// ---------- SendMessageDisposition helpers ---------- //

static DEVICE_MESSAGE_DISPOSITION_INFO* create_device_message_disposition_info_from(MESSAGE_CALLBACK_INFO* message_data)
{
    DEVICE_MESSAGE_DISPOSITION_INFO* result;

    if ((result = (DEVICE_MESSAGE_DISPOSITION_INFO*)malloc(sizeof(DEVICE_MESSAGE_DISPOSITION_INFO))) == NULL)
    {
        LogError("Failed creating DEVICE_MESSAGE_DISPOSITION_INFO (malloc failed)");
    }
    else if (mallocAndStrcpy_s(&result->source, message_data->transportContext->link_name) != RESULT_OK)
    {
        LogError("Failed creating DEVICE_MESSAGE_DISPOSITION_INFO (mallocAndStrcpy_s failed)");
        free(result);
        result = NULL;
    }
    else
    {
        result->message_id = message_data->transportContext->message_id;
    }

    return result;
}

static void destroy_device_message_disposition_info(DEVICE_MESSAGE_DISPOSITION_INFO* device_message_disposition_info)
{
    free(device_message_disposition_info->source);
    free(device_message_disposition_info);
}


// ---------- API functions ---------- //

TRANSPORT_LL_HANDLE IoTHubTransport_AMQP_Common_Create(const IOTHUBTRANSPORT_CONFIG* config, AMQP_GET_IO_TRANSPORT get_io_transport, TRANSPORT_CALLBACKS_INFO* cb_info, void* ctx)
{
    TRANSPORT_LL_HANDLE result;

    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_001: [If `config` or `config->upperConfig` or `get_io_transport` are NULL then IoTHubTransport_AMQP_Common_Create shall fail and return NULL.]
    if (config == NULL || config->upperConfig == NULL || get_io_transport == NULL || cb_info == NULL)
    {
        LogError("IoTHub AMQP client transport null configuration parameter (config=%p, get_io_transport=%p, cb_info=%p).", config, get_io_transport, cb_info);
        result = NULL;
    }
    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_002: [IoTHubTransport_AMQP_Common_Create shall fail and return NULL if `config->upperConfig->protocol` is NULL]
    else if (config->upperConfig->protocol == NULL)
    {
        LogError("Failed to create the AMQP transport common instance (NULL parameter received: protocol)");
        result = NULL;
    }
    else if (IoTHub_Transport_ValidateCallbacks(cb_info) != 0)
    {
        LogError("failure checking transport callbacks");
        result = NULL;
    }
    else
    {
        AMQP_TRANSPORT_INSTANCE* instance;

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_003: [Memory shall be allocated for the transport's internal state structure (`instance`)]
        if ((instance = (AMQP_TRANSPORT_INSTANCE*)malloc(sizeof(AMQP_TRANSPORT_INSTANCE))) == NULL)
        {
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_004: [If malloc() fails, IoTHubTransport_AMQP_Common_Create shall fail and return NULL]
            LogError("Could not allocate AMQP transport state (malloc failed)");
            result = NULL;
        }
        else
        {
            memset(instance, 0, sizeof(AMQP_TRANSPORT_INSTANCE));
            instance->amqp_connection_state = AMQP_CONNECTION_STATE_CLOSED;
            instance->preferred_authentication_mode = AMQP_TRANSPORT_AUTHENTICATION_MODE_NOT_SET;
            instance->state = AMQP_TRANSPORT_STATE_NOT_CONNECTED;
            instance->authorization_module = config->auth_module_handle;

            // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_124: [`instance->connection_retry_control` shall be set using retry_control_create(), passing defaults EXPONENTIAL_BACKOFF_WITH_JITTER and 0]
            if ((instance->connection_retry_control = retry_control_create(DEFAULT_RETRY_POLICY, DEFAULT_MAX_RETRY_TIME_IN_SECS)) == NULL)
            {
                // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_125: [If retry_control_create() fails, IoTHubTransport_AMQP_Common_Create shall fail and return NULL]
                LogError("Failed to create the connection retry control.");
                result = NULL;
            }
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_005: [If `config->upperConfig->protocolGatewayHostName` is NULL, `instance->iothub_target_fqdn` shall be set as `config->upperConfig->iotHubName` + "." + `config->upperConfig->iotHubSuffix`]
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_006: [If `config->upperConfig->protocolGatewayHostName` is not NULL, `instance->iothub_target_fqdn` shall be set with a copy of it]
            else if ((instance->iothub_host_fqdn = get_target_iothub_fqdn(config)) == NULL)
            {
                // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_007: [If `instance->iothub_target_fqdn` fails to be set, IoTHubTransport_AMQP_Common_Create shall fail and return NULL]
                LogError("Failed to obtain the iothub target fqdn.");
                result = NULL;
            }
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_008: [`instance->registered_devices` shall be set using singlylinkedlist_create()]
            else if ((instance->registered_devices = singlylinkedlist_create()) == NULL)
            {
                // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_009: [If singlylinkedlist_create() fails, IoTHubTransport_AMQP_Common_Create shall fail and return NULL]
                LogError("Failed to initialize the internal list of registered devices (singlylinkedlist_create failed)");
                result = NULL;
            }
            else
            {
                // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_010: [`get_io_transport` shall be saved on `instance->underlying_io_transport_provider`]
                instance->underlying_io_transport_provider = get_io_transport;
                instance->is_trace_on = false;
                instance->option_cbs_request_timeout_secs = DEFAULT_CBS_REQUEST_TIMEOUT_SECS;
                instance->option_send_event_timeout_secs = DEFAULT_EVENT_SEND_TIMEOUT_SECS;
                // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_12_002: [The connection idle timeout parameter default value shall be set to 240000 milliseconds using connection_set_idle_timeout()]
                instance->svc2cl_keep_alive_timeout_secs = DEFAULT_SERVICE_KEEP_ALIVE_FREQ_SECS;
                // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_99_001: [The remote idle timeout ratio shall be set to 0.5 using connection_set_remote_idle_timeout_empty_frame_send_ratio()]
                instance->cl2svc_keep_alive_send_ratio = DEFAULT_REMOTE_IDLE_PING_RATIO;

                instance->transport_ctx = ctx;
                instance->transport_callbacks.msg_input_cb = cb_info->msg_input_cb;
                instance->transport_callbacks.msg_cb = cb_info->msg_cb;
                instance->transport_callbacks.connection_status_cb = cb_info->connection_status_cb;
                instance->transport_callbacks.send_complete_cb = cb_info->send_complete_cb;
                instance->transport_callbacks.prod_info_cb = cb_info->prod_info_cb;
                instance->transport_callbacks.twin_rpt_state_complete_cb = cb_info->twin_rpt_state_complete_cb;
                instance->transport_callbacks.twin_retrieve_prop_complete_cb = cb_info->twin_retrieve_prop_complete_cb;
                instance->transport_callbacks.method_complete_cb = cb_info->method_complete_cb;

                // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_012: [If IoTHubTransport_AMQP_Common_Create succeeds it shall return a pointer to `instance`.]
                result = (TRANSPORT_LL_HANDLE)instance;
            }

            if (result == NULL)
            {
                // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_011: [If IoTHubTransport_AMQP_Common_Create fails it shall free any memory it allocated]
                internal_destroy_instance(instance);
            }
        }
    }

    return result;
}

IOTHUB_PROCESS_ITEM_RESULT IoTHubTransport_AMQP_Common_ProcessItem(TRANSPORT_LL_HANDLE handle, IOTHUB_IDENTITY_TYPE item_type, IOTHUB_IDENTITY_INFO* iothub_item)
{
    IOTHUB_PROCESS_ITEM_RESULT result;

    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_144: [If `handle` or `iothub_item` are NULL, `IoTHubTransport_AMQP_Common_ProcessItem` shall fail and return IOTHUB_PROCESS_ERROR.]
    if (handle == NULL || iothub_item == NULL)
    {
        LogError("Invalid argument (handle=%p, iothub_item=%p)", handle, iothub_item);
        result = IOTHUB_PROCESS_ERROR;
    }
    else
    {
        if (item_type == IOTHUB_TYPE_DEVICE_TWIN)
        {
            AMQP_TRANSPORT_DEVICE_TWIN_CONTEXT* dev_twin_ctx;

            if ((dev_twin_ctx = (AMQP_TRANSPORT_DEVICE_TWIN_CONTEXT*)malloc(sizeof(AMQP_TRANSPORT_DEVICE_TWIN_CONTEXT))) == NULL)
            {
                LogError("Failed allocating context for TWIN message");
                result = IOTHUB_PROCESS_ERROR;
            }
            else
            {
                AMQP_TRANSPORT_INSTANCE* transport_instance = (AMQP_TRANSPORT_INSTANCE*)handle;

                AMQP_TRANSPORT_DEVICE_INSTANCE* registered_device = (AMQP_TRANSPORT_DEVICE_INSTANCE*)iothub_item->device_twin->device_handle;

                dev_twin_ctx->transport_callbacks = transport_instance->transport_callbacks;
                dev_twin_ctx->transport_ctx = transport_instance->transport_ctx;
                dev_twin_ctx->item_id = iothub_item->device_twin->item_id;

                // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_146: [amqp_device_send_twin_update_async() shall be invoked passing `iothub_item->device_twin->report_data_handle` and `on_device_send_twin_update_complete_callback`]
                if (amqp_device_send_twin_update_async(
                    registered_device->device_handle,
                    iothub_item->device_twin->report_data_handle,
                    on_device_send_twin_update_complete_callback, (void*)dev_twin_ctx) != RESULT_OK)
                {
                    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_147: [If amqp_device_send_twin_update_async() fails, `IoTHubTransport_AMQP_Common_ProcessItem` shall fail and return IOTHUB_PROCESS_ERROR.]
                    LogError("Failed sending TWIN update");
                    free(dev_twin_ctx);
                    result = IOTHUB_PROCESS_ERROR;
                }
                else
                {
                    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_150: [If no errors occur, `IoTHubTransport_AMQP_Common_ProcessItem` shall return IOTHUB_PROCESS_OK.]
                    result = IOTHUB_PROCESS_OK;
                }
            }
        }
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_145: [If `item_type` is not IOTHUB_TYPE_DEVICE_TWIN, `IoTHubTransport_AMQP_Common_ProcessItem` shall fail and return IOTHUB_PROCESS_ERROR.]
        else
        {
            LogError("Item type not supported (%d)", item_type);
            result = IOTHUB_PROCESS_ERROR;
        }
    }

    return result;
}

void IoTHubTransport_AMQP_Common_DoWork(TRANSPORT_LL_HANDLE handle)
{
    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_016: [If `handle` is NULL, IoTHubTransport_AMQP_Common_DoWork shall return without doing any work]
    if (handle == NULL)
    {
        LogError("IoTHubClient DoWork failed: transport handle parameter is NULL.");
    }
    else
    {
        AMQP_TRANSPORT_INSTANCE* transport_instance = (AMQP_TRANSPORT_INSTANCE*)handle;
        LIST_ITEM_HANDLE list_item;

        if (transport_instance->state == AMQP_TRANSPORT_STATE_NOT_CONNECTED_NO_MORE_RETRIES)
        {
            // Nothing to be done.
        }
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_017: [If `instance->state` is `RECONNECTION_REQUIRED`, IoTHubTransport_AMQP_Common_DoWork shall attempt to trigger the connection-retry logic and return]
        else if (transport_instance->state == AMQP_TRANSPORT_STATE_RECONNECTION_REQUIRED)
        {
            RETRY_ACTION retry_action;

            // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_126: [The connection retry shall be attempted only if retry_control_should_retry() returns RETRY_ACTION_NOW, or if it fails]
            if (retry_control_should_retry(transport_instance->connection_retry_control, &retry_action) != RESULT_OK)
            {
                LogError("retry_control_should_retry() failed; assuming immediate connection retry for safety.");
                retry_action = RETRY_ACTION_RETRY_NOW;
            }

            if (retry_action == RETRY_ACTION_RETRY_NOW)
            {
                prepare_for_connection_retry(transport_instance);
            }
            else if (retry_action == RETRY_ACTION_STOP_RETRYING)
            {
                update_state(transport_instance, AMQP_TRANSPORT_STATE_NOT_CONNECTED_NO_MORE_RETRIES);

                (void)singlylinkedlist_foreach(transport_instance->registered_devices, raise_connection_status_callback_retry_expired, NULL);
            }
        }
        else
        {
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_018: [If there are no devices registered on the transport, IoTHubTransport_AMQP_Common_DoWork shall skip do_work for devices]
            if ((list_item = singlylinkedlist_get_head_item(transport_instance->registered_devices)) != NULL)
            {
                // We need to check if there are devices, otherwise the amqp_connection won't be able to be created since
                // there is not a preferred authentication mode set yet on the transport.

                // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_019: [If `instance->amqp_connection` is NULL, it shall be established]
                // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_12_003: [AMQP connection will be configured using the `svc2cl_keep_alive_timeout_secs` value from SetOption ]
                if (transport_instance->amqp_connection == NULL && establish_amqp_connection(transport_instance) != RESULT_OK)
                {
                    LogError("AMQP transport failed to establish connection with service.");

                    update_state(transport_instance, AMQP_TRANSPORT_STATE_RECONNECTION_REQUIRED);
                }
                // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_020: [If the amqp_connection is OPENED, the transport shall iterate through each registered device and perform a device-specific do_work on each]
                else if (transport_instance->amqp_connection_state == AMQP_CONNECTION_STATE_OPENED)
                {
                    size_t number_of_devices = 0;
                    size_t number_of_faulty_devices = 0;

                    while (list_item != NULL)
                    {
                        AMQP_TRANSPORT_DEVICE_INSTANCE* registered_device;

                        if ((registered_device = (AMQP_TRANSPORT_DEVICE_INSTANCE*)singlylinkedlist_item_get_value(list_item)) == NULL)
                        {
                            LogError("Transport had an unexpected failure during DoWork (failed to fetch a registered_devices list item value)");
                        }
                        else if (registered_device->number_of_send_event_complete_failures >= DEVICE_FAILURE_COUNT_RECONNECTION_THRESHOLD)
                        {
                            number_of_faulty_devices++;
                        }
                        else if (IoTHubTransport_AMQP_Common_Device_DoWork(registered_device) != RESULT_OK)
                        {
                            // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_021: [If DoWork fails for the registered device for more than DEVICE_FAILURE_COUNT_RECONNECTION_THRESHOLD, connection retry shall be triggered]
                            if (registered_device->number_of_previous_failures >= DEVICE_FAILURE_COUNT_RECONNECTION_THRESHOLD)
                            {
                                number_of_faulty_devices++;
                            }
                        }

                        list_item = singlylinkedlist_get_next_item(list_item);
                        number_of_devices++;
                    }

                    if (number_of_faulty_devices > 0 &&
                        ((float)number_of_faulty_devices/(float)number_of_devices) >= DEVICE_MULTIPLEXING_FAULTY_DEVICE_RATIO_RECONNECTION_THRESHOLD)
                    {
                        LogError("Reconnection required. %zd of %zd registered devices are failing.", number_of_faulty_devices, number_of_devices);

                        update_state(transport_instance, AMQP_TRANSPORT_STATE_RECONNECTION_REQUIRED);
                    }
                }
            }

            // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_022: [If `instance->amqp_connection` is not NULL, amqp_connection_do_work shall be invoked]
            if (transport_instance->amqp_connection != NULL)
            {
                amqp_connection_do_work(transport_instance->amqp_connection);
            }
        }
    }
}

int IoTHubTransport_AMQP_Common_Subscribe(IOTHUB_DEVICE_HANDLE handle)
{
    int result;

    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_084: [If `handle` is NULL, IoTHubTransport_AMQP_Common_Subscribe shall return a non-zero result]
    if (handle == NULL)
    {
        LogError("Invalid handle to IoTHubClient AMQP transport device handle.");
        result = MU_FAILURE;
    }
    else
    {
        AMQP_TRANSPORT_DEVICE_INSTANCE* amqp_device_instance = (AMQP_TRANSPORT_DEVICE_INSTANCE*)handle;

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_085: [If `amqp_device_instance` is not registered, IoTHubTransport_AMQP_Common_Subscribe shall return a non-zero result]
        if (!is_device_registered(amqp_device_instance))
        {
            LogError("Device '%s' failed subscribing to cloud-to-device messages (device is not registered)", STRING_c_str(amqp_device_instance->device_id));
            result = MU_FAILURE;
        }
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_086: [amqp_device_subscribe_message() shall be invoked passing `on_message_received_callback`]
        else if (amqp_device_subscribe_message(amqp_device_instance->device_handle, on_message_received, amqp_device_instance) != RESULT_OK)
        {
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_087: [If amqp_device_subscribe_message() fails, IoTHubTransport_AMQP_Common_Subscribe shall return a non-zero result]
            LogError("Device '%s' failed subscribing to cloud-to-device messages (amqp_device_subscribe_message failed)", STRING_c_str(amqp_device_instance->device_id));
            result = MU_FAILURE;
        }
        else
        {
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_088: [If no failures occur, IoTHubTransport_AMQP_Common_Subscribe shall return 0]
            result = RESULT_OK;
        }
    }

    return result;
}

void IoTHubTransport_AMQP_Common_Unsubscribe(IOTHUB_DEVICE_HANDLE handle)
{
    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_093: [If `handle` is NULL, IoTHubTransport_AMQP_Common_Subscribe shall return]
    if (handle == NULL)
    {
        LogError("Invalid handle to IoTHubClient AMQP transport device handle.");
    }
    else
    {
        AMQP_TRANSPORT_DEVICE_INSTANCE* amqp_device_instance = (AMQP_TRANSPORT_DEVICE_INSTANCE*)handle;

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_094: [If `amqp_device_instance` is not registered, IoTHubTransport_AMQP_Common_Subscribe shall return]
        if (!is_device_registered(amqp_device_instance))
        {
            LogError("Device '%s' failed unsubscribing to cloud-to-device messages (device is not registered)", STRING_c_str(amqp_device_instance->device_id));
        }
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_095: [amqp_device_unsubscribe_message() shall be invoked passing `amqp_device_instance->device_handle`]
        else if (amqp_device_unsubscribe_message(amqp_device_instance->device_handle) != RESULT_OK)
        {
            LogError("Device '%s' failed unsubscribing to cloud-to-device messages (amqp_device_unsubscribe_message failed)", STRING_c_str(amqp_device_instance->device_id));
        }
    }
}

int IoTHubTransport_AMQP_Common_Subscribe_DeviceTwin(IOTHUB_DEVICE_HANDLE handle)
{
    int result;

    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_131: [If `handle` is NULL, `IoTHubTransport_AMQP_Common_Subscribe_DeviceTwin` shall fail and return non-zero.]
    if (handle == NULL)
    {
        LogError("Invalid argument (handle is NULL");
        result = MU_FAILURE;
    }
    else
    {
        AMQP_TRANSPORT_INSTANCE* transport = (AMQP_TRANSPORT_INSTANCE*)handle;

        if (get_number_of_registered_devices(transport) != 1)
        {
            LogError("Device Twin not supported on device multiplexing scenario");
            result = MU_FAILURE;
        }
        else
        {
            LIST_ITEM_HANDLE list_item = singlylinkedlist_get_head_item(transport->registered_devices);

            // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_136: [If no errors occur, `IoTHubTransport_AMQP_Common_Subscribe_DeviceTwin` shall return zero.]
            result = RESULT_OK;

            while (list_item != NULL)
            {
                AMQP_TRANSPORT_DEVICE_INSTANCE* registered_device;

                if ((registered_device = (AMQP_TRANSPORT_DEVICE_INSTANCE*)singlylinkedlist_item_get_value(list_item)) == NULL)
                {
                    LogError("Failed retrieving registered device information");
                    result = MU_FAILURE;
                    break;
                }
                // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_134: [amqp_device_subscribe_for_twin_updates() shall be invoked for the registered device, passing `on_device_twin_update_received_callback`]
                else if (amqp_device_subscribe_for_twin_updates(registered_device->device_handle, on_device_twin_update_received_callback, (void*)registered_device) != RESULT_OK)
                {
                    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_135: [If amqp_device_subscribe_for_twin_updates() fails, `IoTHubTransport_AMQP_Common_Subscribe_DeviceTwin` shall fail and return non-zero.]
                    LogError("Failed subscribing for device Twin updates");
                    result = MU_FAILURE;
                    break;
                }

                list_item = singlylinkedlist_get_next_item(list_item);
            }
        }
    }

    return result;
}

void IoTHubTransport_AMQP_Common_Unsubscribe_DeviceTwin(IOTHUB_DEVICE_HANDLE handle)
{
    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_140: [If `handle` is NULL, `IoTHubTransport_AMQP_Common_Unsubscribe_DeviceTwin` shall return.]
    if (handle == NULL)
    {
        LogError("Invalid argument (handle is NULL");
    }
    else
    {
        AMQP_TRANSPORT_INSTANCE* transport = (AMQP_TRANSPORT_INSTANCE*)handle;

        if (get_number_of_registered_devices(transport) != 1)
        {
            LogError("Device Twin not supported on device multiplexing scenario");
        }
        else
        {
            LIST_ITEM_HANDLE list_item = singlylinkedlist_get_head_item(transport->registered_devices);

            while (list_item != NULL)
            {
                AMQP_TRANSPORT_DEVICE_INSTANCE* registered_device;

                if ((registered_device = (AMQP_TRANSPORT_DEVICE_INSTANCE*)singlylinkedlist_item_get_value(list_item)) == NULL)
                {
                    LogError("Failed retrieving registered device information");
                    break;
                }
                // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_142: [amqp_device_unsubscribe_for_twin_updates() shall be invoked for the registered device]
                else if (amqp_device_unsubscribe_for_twin_updates(registered_device->device_handle) != RESULT_OK)
                {
                    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_143: [If `amqp_device_unsubscribe_for_twin_updates` fails, the error shall be ignored]
                    LogError("Failed unsubscribing for device Twin updates");
                    break;
                }

                list_item = singlylinkedlist_get_next_item(list_item);
            }
        }
    }
}

IOTHUB_CLIENT_RESULT IoTHubTransport_AMQP_Common_GetTwinAsync(IOTHUB_DEVICE_HANDLE handle, IOTHUB_CLIENT_DEVICE_TWIN_CALLBACK completionCallback, void* callbackContext)
{
    (void)callbackContext;

    IOTHUB_CLIENT_RESULT result;

    if (handle == NULL || completionCallback == NULL)
    {
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_154: [ If `handle` or `completionCallback` are NULL, `IoTHubTransport_AMQP_Common_GetTwinAsync` shall fail and return IOTHUB_CLIENT_INVALID_ARG ]
        LogError("Invalid argument (handle=%p, completionCallback=%p)", handle, completionCallback);
        result = IOTHUB_CLIENT_INVALID_ARG;
    }
    else
    {
        AMQP_TRANSPORT_DEVICE_INSTANCE* registered_device = (AMQP_TRANSPORT_DEVICE_INSTANCE*)handle;

        if (get_number_of_registered_devices(registered_device->transport_instance) != 1)
        {
            LogError("Device Twin not supported on device multiplexing scenario");
            result = IOTHUB_CLIENT_ERROR;
        }
        else
        {
            AMQP_TRANSPORT_GET_TWIN_CONTEXT* getTwinCtx;

            if ((getTwinCtx = malloc(sizeof(AMQP_TRANSPORT_GET_TWIN_CONTEXT))) == NULL)
            {
                LogError("Failed creating context for get twin");
                result = IOTHUB_CLIENT_ERROR;
            }
            else
            {
                getTwinCtx->on_get_twin_completed_callback = completionCallback;
                getTwinCtx->user_context = callbackContext;

                // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_155: [ amqp_device_get_twin_async() shall be invoked for the registered device, passing `on_device_get_twin_completed_callback`]
                if (amqp_device_get_twin_async(registered_device->device_handle, on_device_get_twin_completed_callback, (void*)getTwinCtx) != RESULT_OK)
                {
                    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_156: [ If amqp_device_get_twin_async() fails, `IoTHubTransport_AMQP_Common_GetTwinAsync` shall fail and return IOTHUB_CLIENT_ERROR ]
                    LogError("Failed subscribing for device Twin updates");
                    free(getTwinCtx);
                    result = IOTHUB_CLIENT_ERROR;
                }
                else
                {
                    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_157: [ If no errors occur, `IoTHubTransport_AMQP_Common_GetTwinAsync` shall return IOTHUB_CLIENT_OK ]
                    result = IOTHUB_CLIENT_OK;
                }
            }
        }
    }

    return result;
}

int IoTHubTransport_AMQP_Common_Subscribe_DeviceMethod(IOTHUB_DEVICE_HANDLE handle)
{
    int result;

    if (handle == NULL)
    {
        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_004: [ If `handle` is NULL, `IoTHubTransport_AMQP_Common_Subscribe_DeviceMethod` shall fail and return a non-zero value. ] */
        LogError("NULL handle");
        result = MU_FAILURE;
    }
    else
    {
        AMQP_TRANSPORT_DEVICE_INSTANCE* device_state = (AMQP_TRANSPORT_DEVICE_INSTANCE*)handle;
        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_026: [ `IoTHubTransport_AMQP_Common_Subscribe_DeviceMethod` shall remember that a subscribe is to be performed in the next call to DoWork and on success it shall return 0. ]*/
        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_005: [ If the transport is already subscribed to receive C2D method requests, `IoTHubTransport_AMQP_Common_Subscribe_DeviceMethod` shall perform no additional action and return 0. ]*/
        device_state->subscribe_methods_needed = true;
        device_state->subscribed_for_methods = false;
        result = 0;
    }

    return result;
}

void IoTHubTransport_AMQP_Common_Unsubscribe_DeviceMethod(IOTHUB_DEVICE_HANDLE handle)
{
    if (handle == NULL)
    {
        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_006: [ If `handle` is NULL, `IoTHubTransport_AMQP_Common_Unsubscribe_DeviceMethod` shall do nothing. ]*/
        LogError("NULL handle");
    }
    else
    {
        AMQP_TRANSPORT_DEVICE_INSTANCE* device_state = (AMQP_TRANSPORT_DEVICE_INSTANCE*)handle;

        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_008: [ If the transport is not subscribed to receive C2D method requests then `IoTHubTransport_AMQP_Common_Unsubscribe_DeviceMethod` shall do nothing. ]*/
        if (device_state->subscribe_methods_needed)
        {
            /* Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_007: [ `IoTHubTransport_AMQP_Common_Unsubscribe_DeviceMethod` shall unsubscribe from receiving C2D method requests by calling `iothubtransportamqp_methods_unsubscribe`. ]*/
            device_state->subscribed_for_methods = false;
            device_state->subscribe_methods_needed = false;
            iothubtransportamqp_methods_unsubscribe(device_state->methods_handle);
        }
    }
}

int IoTHubTransport_AMQP_Common_DeviceMethod_Response(IOTHUB_DEVICE_HANDLE handle, METHOD_HANDLE methodId, const unsigned char* response, size_t response_size, int status_response)
{
    (void)response;
    (void)response_size;
    (void)status_response;
    (void)methodId;
    int result;
    AMQP_TRANSPORT_DEVICE_INSTANCE* device_state = (AMQP_TRANSPORT_DEVICE_INSTANCE*)handle;
    if (device_state != NULL)
    {
        IOTHUBTRANSPORT_AMQP_METHOD_HANDLE saved_handle = (IOTHUBTRANSPORT_AMQP_METHOD_HANDLE)methodId;
        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_019: [ `IoTHubTransport_AMQP_Common_DeviceMethod_Response` shall call `iothubtransportamqp_methods_respond` passing to it the `method_handle` argument, the response bytes, response size and the status code. ]*/
        if (iothubtransportamqp_methods_respond(saved_handle, response, response_size, status_response) != 0)
        {
            /* Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_029: [ If `iothubtransportamqp_methods_respond` fails, `on_methods_request_received` shall return a non-zero value. ]*/
            LogError("iothubtransportamqp_methods_respond failed");
            result = MU_FAILURE;
        }
        else
        {
            result = 0;
        }
    }
    else
    {
        result = MU_FAILURE;
    }
    return result;
}

IOTHUB_CLIENT_RESULT IoTHubTransport_AMQP_Common_GetSendStatus(IOTHUB_DEVICE_HANDLE handle, IOTHUB_CLIENT_STATUS *iotHubClientStatus)
{
    IOTHUB_CLIENT_RESULT result;

    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_096: [If `handle` or `iotHubClientStatus` are NULL, IoTHubTransport_AMQP_Common_GetSendStatus shall return IOTHUB_CLIENT_INVALID_ARG]
    if (handle == NULL || iotHubClientStatus == NULL)
    {
        result = IOTHUB_CLIENT_INVALID_ARG;
        LogError("Failed retrieving the device send status (either handle (%p) or iotHubClientStatus (%p) are NULL)", handle, iotHubClientStatus);
    }
    else
    {
        AMQP_TRANSPORT_DEVICE_INSTANCE* amqp_device_state = (AMQP_TRANSPORT_DEVICE_INSTANCE*)handle;

        DEVICE_SEND_STATUS device_send_status;

        if (!DList_IsListEmpty(amqp_device_state->waiting_to_send))
        {
            *iotHubClientStatus = IOTHUB_CLIENT_SEND_STATUS_BUSY;
            result = IOTHUB_CLIENT_OK;
        }
        else
        {
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_097: [IoTHubTransport_AMQP_Common_GetSendStatus shall invoke amqp_device_get_send_status()]
            if (amqp_device_get_send_status(amqp_device_state->device_handle, &device_send_status) != RESULT_OK)
            {
                // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_098: [If amqp_device_get_send_status() fails, IoTHubTransport_AMQP_Common_GetSendStatus shall return IOTHUB_CLIENT_ERROR]
                LogError("Failed retrieving the device send status (amqp_device_get_send_status failed)");
                result = IOTHUB_CLIENT_ERROR;
            }
            else
            {
                // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_099: [If amqp_device_get_send_status() returns DEVICE_SEND_STATUS_BUSY, IoTHubTransport_AMQP_Common_GetSendStatus shall return IOTHUB_CLIENT_OK and status IOTHUB_CLIENT_SEND_STATUS_BUSY]
                if (device_send_status == DEVICE_SEND_STATUS_BUSY)
                {
                    *iotHubClientStatus = IOTHUB_CLIENT_SEND_STATUS_BUSY;
                }
                // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_100: [If amqp_device_get_send_status() returns DEVICE_SEND_STATUS_IDLE, IoTHubTransport_AMQP_Common_GetSendStatus shall return IOTHUB_CLIENT_OK and status IOTHUB_CLIENT_SEND_STATUS_IDLE]
                else // DEVICE_SEND_STATUS_IDLE
                {
                    *iotHubClientStatus = IOTHUB_CLIENT_SEND_STATUS_IDLE;
                }

                // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_109: [If no failures occur, IoTHubTransport_AMQP_Common_GetSendStatus shall return IOTHUB_CLIENT_OK]
                result = IOTHUB_CLIENT_OK;
            }
        }
    }

    return result;
}

IOTHUB_CLIENT_RESULT IoTHubTransport_AMQP_Common_SetOption(TRANSPORT_LL_HANDLE handle, const char* option, const void* value)
{
    IOTHUB_CLIENT_RESULT result;

    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_101: [If `handle`, `option` or `value` are NULL then IoTHubTransport_AMQP_Common_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.]
    if ((handle == NULL) || (option == NULL) || (value == NULL))
    {
        LogError("Invalid parameter (NULL) passed to AMQP transport SetOption (handle=%p, options=%p, value=%p)", handle, option, value);
        result = IOTHUB_CLIENT_INVALID_ARG;
    }
    else
    {
        AMQP_TRANSPORT_INSTANCE* transport_instance = (AMQP_TRANSPORT_INSTANCE*)handle;
        bool is_device_specific_option;

        if (strcmp(OPTION_CBS_REQUEST_TIMEOUT, option) == 0)
        {
            is_device_specific_option = true;
            transport_instance->option_cbs_request_timeout_secs = *(size_t*)value;
        }
        else if (strcmp(OPTION_EVENT_SEND_TIMEOUT_SECS, option) == 0)
        {
            is_device_specific_option = true;
            transport_instance->option_send_event_timeout_secs = *(size_t*)value;
        }
        else
        {
            is_device_specific_option = false;
        }

        if (is_device_specific_option)
        {
            if (IoTHubTransport_AMQP_Common_Device_SetOption(handle, option, (void*)value) != RESULT_OK)
            {
                // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_103: [If amqp_device_set_option() fails, IoTHubTransport_AMQP_Common_SetOption shall return IOTHUB_CLIENT_ERROR]
                LogError("transport failed setting option '%s' (failed setting option on one or more registered devices)", option);
                result = IOTHUB_CLIENT_ERROR;
            }
            else
            {
                result = IOTHUB_CLIENT_OK;
            }
        }
        else if (strcmp(OPTION_RETRY_INTERVAL_SEC, option) == 0)
        {
            if (retry_control_set_option(transport_instance->connection_retry_control, RETRY_CONTROL_OPTION_INITIAL_WAIT_TIME_IN_SECS, value) != 0)
            {
                LogError("Failure setting retry interval option");
                result = IOTHUB_CLIENT_ERROR;
            }
            else
            {
                result = IOTHUB_CLIENT_OK;
            }
        }
        else if (strcmp(OPTION_RETRY_MAX_DELAY_SECS, option) == 0)
        {
            if (retry_control_set_option(transport_instance->connection_retry_control, RETRY_CONTROL_OPTION_MAX_DELAY_IN_SECS, value) != 0)
            {
                LogError("Failure setting retry max delay option");
                result = IOTHUB_CLIENT_ERROR;
            }
            else
            {
                result = IOTHUB_CLIENT_OK;
            }
        }
        else if ((strcmp(OPTION_SERVICE_SIDE_KEEP_ALIVE_FREQ_SECS, option) == 0) || (strcmp(OPTION_C2D_KEEP_ALIVE_FREQ_SECS, option) == 0))
        {
            transport_instance->svc2cl_keep_alive_timeout_secs = *(size_t*)value;
            result = IOTHUB_CLIENT_OK;
        }
        else if (strcmp(OPTION_REMOTE_IDLE_TIMEOUT_RATIO, option) == 0)
        {

            if ((*(double*)value <= 0.0) || (*(double*)value >= MAX_SERVICE_KEEP_ALIVE_RATIO))
            {
                LogError("Invalid remote idle ratio %lf", *(double*) value);
                result = IOTHUB_CLIENT_INVALID_ARG;
            }
            else
            {
                transport_instance->cl2svc_keep_alive_send_ratio = *(double*)value; // override the default and set the user configured remote idle ratio value
                result = IOTHUB_CLIENT_OK;
            }

        }
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_104: [If `option` is `logtrace`, `value` shall be saved and applied to `instance->connection` using amqp_connection_set_logging()]
        else if (strcmp(OPTION_LOG_TRACE, option) == 0)
        {
            transport_instance->is_trace_on = *((bool*)value);

            if (transport_instance->amqp_connection != NULL &&
                amqp_connection_set_logging(transport_instance->amqp_connection, transport_instance->is_trace_on) != RESULT_OK)
            {
                LogError("transport failed setting option '%s' (amqp_connection_set_logging failed)", option);
                result = IOTHUB_CLIENT_ERROR;
            }
            else
            {
                result = IOTHUB_CLIENT_OK;
            }
        }
        else if (strcmp(OPTION_HTTP_PROXY, option) == 0)
        {
            /* Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_032: [ If `option` is `proxy_data`, `value` shall be used as an `HTTP_PROXY_OPTIONS*`. ]*/
            HTTP_PROXY_OPTIONS* proxy_options = (HTTP_PROXY_OPTIONS*)value;

            if (transport_instance->tls_io != NULL)
            {
                /* Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_038: [ If the underlying IO has already been created, then `IoTHubTransport_AMQP_Common_SetOption` shall fail and return `IOTHUB_CLIENT_ERROR`. ]*/
                LogError("Cannot set proxy option once the underlying IO is created");
                result = IOTHUB_CLIENT_ERROR;
            }
            else if (proxy_options->host_address == NULL)
            {
                /* Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_034: [ If `host_address` is NULL, `IoTHubTransport_AMQP_Common_SetOption` shall fail and return `IOTHUB_CLIENT_INVALID_ARG`. ]*/
                LogError("NULL host_address in proxy options");
                result = IOTHUB_CLIENT_INVALID_ARG;
            }
            /* Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_037: [ If only one of `username` and `password` is NULL, `IoTHubTransport_AMQP_Common_SetOption` shall fail and return `IOTHUB_CLIENT_INVALID_ARG`. ]*/
            else if (((proxy_options->username == NULL) || (proxy_options->password == NULL)) &&
                (proxy_options->username != proxy_options->password))
            {
                LogError("Only one of username and password for proxy settings was NULL");
                result = IOTHUB_CLIENT_INVALID_ARG;
            }
            else
            {
                char* copied_proxy_hostname = NULL;
                char* copied_proxy_username = NULL;
                char* copied_proxy_password = NULL;

                /* Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_033: [ The fields `host_address`, `port`, `username` and `password` shall be saved for later used (needed when creating the underlying IO to be used by the transport). ]*/
                transport_instance->http_proxy_port = proxy_options->port;
                if (mallocAndStrcpy_s(&copied_proxy_hostname, proxy_options->host_address) != 0)
                {
                    /* Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_035: [ If copying `host_address`, `username` or `password` fails, `IoTHubTransport_AMQP_Common_SetOption` shall fail and return `IOTHUB_CLIENT_ERROR`. ]*/
                    LogError("Cannot copy HTTP proxy hostname");
                    result = IOTHUB_CLIENT_ERROR;
                }
                /* Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_036: [ `username` and `password` shall be allowed to be NULL. ]*/
                else if ((proxy_options->username != NULL) && (mallocAndStrcpy_s(&copied_proxy_username, proxy_options->username) != 0))
                {
                    /* Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_035: [ If copying `host_address`, `username` or `password` fails, `IoTHubTransport_AMQP_Common_SetOption` shall fail and return `IOTHUB_CLIENT_ERROR`. ]*/
                    free(copied_proxy_hostname);
                    LogError("Cannot copy HTTP proxy username");
                    result = IOTHUB_CLIENT_ERROR;
                }
                /* Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_036: [ `username` and `password` shall be allowed to be NULL. ]*/
                else if ((proxy_options->password != NULL) && (mallocAndStrcpy_s(&copied_proxy_password, proxy_options->password) != 0))
                {
                    /* Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_035: [ If copying `host_address`, `username` or `password` fails, `IoTHubTransport_AMQP_Common_SetOption` shall fail and return `IOTHUB_CLIENT_ERROR`. ]*/
                    if (copied_proxy_username != NULL)
                    {
                        free(copied_proxy_username);
                    }
                    free(copied_proxy_hostname);
                    LogError("Cannot copy HTTP proxy password");
                    result = IOTHUB_CLIENT_ERROR;
                }
                else
                {
                    /* Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_040: [ When setting the proxy options succeeds any previously saved proxy options shall be freed. ]*/
                    free_proxy_data(transport_instance);

                    transport_instance->http_proxy_hostname = copied_proxy_hostname;
                    transport_instance->http_proxy_username = copied_proxy_username;
                    transport_instance->http_proxy_password = copied_proxy_password;

                    /* Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_039: [ If setting the `proxy_data` option succeeds, `IoTHubTransport_AMQP_Common_SetOption` shall return `IOTHUB_CLIENT_OK` ]*/
                    result = IOTHUB_CLIENT_OK;
                }
            }
        }
        else
        {
            result = IOTHUB_CLIENT_OK;

            // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_02_007: [ If `option` is `x509certificate` and the transport preferred authentication method is not x509 then IoTHubTransport_AMQP_Common_SetOption shall return IOTHUB_CLIENT_INVALID_ARG. ]
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_02_008: [ If `option` is `x509privatekey` and the transport preferred authentication method is not x509 then IoTHubTransport_AMQP_Common_SetOption shall return IOTHUB_CLIENT_INVALID_ARG. ]
            if (strcmp(OPTION_X509_CERT, option) == 0 || strcmp(OPTION_X509_PRIVATE_KEY, option) == 0)
            {
                if (transport_instance->preferred_authentication_mode == AMQP_TRANSPORT_AUTHENTICATION_MODE_NOT_SET)
                {
                    transport_instance->preferred_authentication_mode = AMQP_TRANSPORT_AUTHENTICATION_MODE_X509;
                }
                else if (transport_instance->preferred_authentication_mode != AMQP_TRANSPORT_AUTHENTICATION_MODE_X509)
                {
                    LogError("transport failed setting option '%s' (preferred authentication method is not x509)", option);
                    result = IOTHUB_CLIENT_INVALID_ARG;
                }
                else
                {
                    IoTHubClient_Auth_Set_x509_Type(transport_instance->authorization_module, true);
                }
            }

            // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_105: [If `option` does not match one of the options handled by this module, it shall be passed to `instance->tls_io` using xio_setoption()]
            if (result != IOTHUB_CLIENT_INVALID_ARG)
            {
                // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_106: [If `instance->tls_io` is NULL, it shall be set invoking instance->underlying_io_transport_provider()]
                if (transport_instance->tls_io == NULL &&
                    get_new_underlying_io_transport(transport_instance, &transport_instance->tls_io) != RESULT_OK)
                {
                    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_107: [If instance->underlying_io_transport_provider() fails, IoTHubTransport_AMQP_Common_SetOption shall fail and return IOTHUB_CLIENT_ERROR]
                    LogError("transport failed setting option '%s' (failed to obtain a TLS I/O transport).", option);
                    result = IOTHUB_CLIENT_ERROR;
                }
                // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_108: [When `instance->tls_io` is created, IoTHubTransport_AMQP_Common_SetOption shall apply `instance->saved_tls_options` with OptionHandler_FeedOptions()]
                else if (xio_setoption(transport_instance->tls_io, option, value) != RESULT_OK)
                {
                    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_03_001: [If xio_setoption fails, IoTHubTransport_AMQP_Common_SetOption shall return IOTHUB_CLIENT_ERROR.]
                    LogError("transport failed setting option '%s' (xio_setoption failed)", option);
                    result = IOTHUB_CLIENT_ERROR;
                }
                else
                {
                    if (save_underlying_io_transport_options(transport_instance) != RESULT_OK)
                    {
                        LogError("IoTHubTransport_AMQP_Common_SetOption failed to save underlying I/O options; failure will be ignored");
                    }

                    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_03_001: [If no failures occur, IoTHubTransport_AMQP_Common_SetOption shall return IOTHUB_CLIENT_OK.]
                    result = IOTHUB_CLIENT_OK;
                }
            }
        }
    }

    return result;
}

IOTHUB_DEVICE_HANDLE IoTHubTransport_AMQP_Common_Register(TRANSPORT_LL_HANDLE handle, const IOTHUB_DEVICE_CONFIG* device, PDLIST_ENTRY waitingToSend)
{
    IOTHUB_DEVICE_HANDLE result;

    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_17_005: [If `handle`, `device` or `waitingToSend` is NULL, IoTHubTransport_AMQP_Common_Register shall return NULL]
    if ((handle == NULL) || (device == NULL) || (waitingToSend == NULL))
    {
        LogError("invalid parameter TRANSPORT_LL_HANDLE handle=%p, const IOTHUB_DEVICE_CONFIG* device=%p, PDLIST_ENTRY waiting_to_send=%p",
            handle, device, waitingToSend);
        result = NULL;
    }
    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_03_002: [IoTHubTransport_AMQP_Common_Register shall return NULL if `device->deviceId` is NULL.]
    else if (device->deviceId == NULL)
    {
        LogError("Transport failed to register device (device_id provided is NULL)");
        result = NULL;
    }
    else
    {
        LIST_ITEM_HANDLE list_item;
        AMQP_TRANSPORT_INSTANCE* transport_instance = (AMQP_TRANSPORT_INSTANCE*)handle;

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_064: [If the device is already registered, IoTHubTransport_AMQP_Common_Register shall fail and return NULL.]
        if (is_device_registered_ex(transport_instance->registered_devices, device->deviceId, &list_item))
        {
            LogError("IoTHubTransport_AMQP_Common_Register failed (device '%s' already registered on this transport instance)", device->deviceId);
            result = NULL;
        }
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_065: [IoTHubTransport_AMQP_Common_Register shall fail and return NULL if the device is not using an authentication mode compatible with the currently used by the transport.]
        else if (!is_device_credential_acceptable(device, transport_instance->preferred_authentication_mode))
        {
            LogError("Transport failed to register device '%s' (device credential was not accepted)", device->deviceId);
            result = NULL;
        }
        else
        {
            AMQP_TRANSPORT_DEVICE_INSTANCE* amqp_device_instance;

            // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_066: [IoTHubTransport_AMQP_Common_Register shall allocate an instance of AMQP_TRANSPORT_DEVICE_INSTANCE to store the state of the new registered device.]
            if ((amqp_device_instance = (AMQP_TRANSPORT_DEVICE_INSTANCE*)malloc(sizeof(AMQP_TRANSPORT_DEVICE_INSTANCE))) == NULL)
            {
                // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_067: [If malloc fails, IoTHubTransport_AMQP_Common_Register shall fail and return NULL.]
                LogError("Transport failed to register device '%s' (failed to create the device state instance; malloc failed)", device->deviceId);
                result = NULL;
            }
            else
            {
                memset(amqp_device_instance, 0, sizeof(AMQP_TRANSPORT_DEVICE_INSTANCE));

                // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_068: [IoTHubTransport_AMQP_Common_Register shall save the handle references to the IoTHubClient, transport, waitingToSend list on `amqp_device_instance`.]
                amqp_device_instance->transport_instance = transport_instance;
                amqp_device_instance->waiting_to_send = waitingToSend;
                amqp_device_instance->device_state = DEVICE_STATE_STOPPED;
                amqp_device_instance->max_state_change_timeout_secs = DEFAULT_DEVICE_STATE_CHANGE_TIMEOUT_SECS;
                amqp_device_instance->subscribe_methods_needed = false;
                amqp_device_instance->subscribed_for_methods = false;
                amqp_device_instance->transport_ctx = transport_instance->transport_ctx;
                amqp_device_instance->transport_callbacks = transport_instance->transport_callbacks;

                // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_069: [A copy of `config->deviceId` shall be saved into `device_state->device_id`]
                if ((amqp_device_instance->device_id = STRING_construct(device->deviceId)) == NULL)
                {
                    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_070: [If STRING_construct() fails, IoTHubTransport_AMQP_Common_Register shall fail and return NULL]
                    LogError("Transport failed to register device '%s' (failed to copy the deviceId)", device->deviceId);
                    result = NULL;
                }
                else
                {
                    AMQP_DEVICE_CONFIG device_config;
                    (void)memset(&device_config, 0, sizeof(AMQP_DEVICE_CONFIG));
                    device_config.iothub_host_fqdn = (char*)STRING_c_str(transport_instance->iothub_host_fqdn);
                    device_config.authorization_module = device->authorization_module;

                    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_072: [The configuration for amqp_device_create shall be set according to the authentication preferred by IOTHUB_DEVICE_CONFIG]
                    device_config.authentication_mode = get_authentication_mode(device);
                    device_config.on_state_changed_callback = on_device_state_changed_callback;
                    device_config.on_state_changed_context = amqp_device_instance;
                    device_config.prod_info_cb = transport_instance->transport_callbacks.prod_info_cb;
                    device_config.prod_info_ctx = transport_instance->transport_ctx;

                    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_071: [`amqp_device_instance->device_handle` shall be set using amqp_device_create()]
                    if ((amqp_device_instance->device_handle = amqp_device_create(&device_config)) == NULL)
                    {
                        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_073: [If amqp_device_create() fails, IoTHubTransport_AMQP_Common_Register shall fail and return NULL]
                        LogError("Transport failed to register device '%s' (failed to create the DEVICE_HANDLE instance)", device->deviceId);
                        result = NULL;
                    }
                    else
                    {
                        bool is_first_device_being_registered = (singlylinkedlist_get_head_item(transport_instance->registered_devices) == NULL);

                        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_010: [ `IoTHubTransport_AMQP_Common_Register` shall create a new iothubtransportamqp_methods instance by calling `iothubtransportamqp_methods_create` while passing to it the the fully qualified domain name, the device Id, and optional module Id. ]*/
                        amqp_device_instance->methods_handle = iothubtransportamqp_methods_create(STRING_c_str(transport_instance->iothub_host_fqdn), device->deviceId, device->moduleId);
                        if (amqp_device_instance->methods_handle == NULL)
                        {
                            /* Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_011: [ If `iothubtransportamqp_methods_create` fails, `IoTHubTransport_AMQP_Common_Create` shall fail and return NULL. ]*/
                            LogError("Transport failed to register device '%s' (Cannot create the methods module)", device->deviceId);
                            result = NULL;
                        }
                        else
                        {
                            if (replicate_device_options_to(amqp_device_instance, device_config.authentication_mode) != RESULT_OK)
                            {
                                LogError("Transport failed to register device '%s' (failed to replicate options)", device->deviceId);
                                result = NULL;
                            }
                            // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_074: [IoTHubTransport_AMQP_Common_Register shall add the `amqp_device_instance` to `instance->registered_devices`]
                            else if (singlylinkedlist_add(transport_instance->registered_devices, amqp_device_instance) == NULL)
                            {
                                // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_075: [If it fails to add `amqp_device_instance`, IoTHubTransport_AMQP_Common_Register shall fail and return NULL]
                                LogError("Transport failed to register device '%s' (singlylinkedlist_add failed)", device->deviceId);
                                result = NULL;
                            }
                            else
                            {
                                // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_076: [If the device is the first being registered on the transport, IoTHubTransport_AMQP_Common_Register shall save its authentication mode as the transport preferred authentication mode]
                                if (transport_instance->preferred_authentication_mode == AMQP_TRANSPORT_AUTHENTICATION_MODE_NOT_SET &&
                                    is_first_device_being_registered)
                                {
                                    if (device_config.authentication_mode == DEVICE_AUTH_MODE_CBS)
                                    {
                                        transport_instance->preferred_authentication_mode = AMQP_TRANSPORT_AUTHENTICATION_MODE_CBS;
                                    }
                                    else
                                    {
                                        transport_instance->preferred_authentication_mode = AMQP_TRANSPORT_AUTHENTICATION_MODE_X509;
                                    }
                                }

                                // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_078: [IoTHubTransport_AMQP_Common_Register shall return a handle to `amqp_device_instance` as a IOTHUB_DEVICE_HANDLE]
                                result = (IOTHUB_DEVICE_HANDLE)amqp_device_instance;
                            }
                        }
                    }
                }

                if (result == NULL)
                {
                    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_077: [If IoTHubTransport_AMQP_Common_Register fails, it shall free all memory it allocated]
                    internal_destroy_amqp_device_instance(amqp_device_instance);
                }
            }
        }
    }

    return result;
}

void IoTHubTransport_AMQP_Common_Unregister(IOTHUB_DEVICE_HANDLE deviceHandle)
{
    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_079: [if `deviceHandle` provided is NULL, IoTHubTransport_AMQP_Common_Unregister shall return.]
    if (deviceHandle == NULL)
    {
        LogError("Failed to unregister device (deviceHandle is NULL).");
    }
    else
    {
        AMQP_TRANSPORT_DEVICE_INSTANCE* registered_device = (AMQP_TRANSPORT_DEVICE_INSTANCE*)deviceHandle;
        const char* device_id;
        LIST_ITEM_HANDLE list_item;

        if ((device_id = STRING_c_str(registered_device->device_id)) == NULL)
        {
            LogError("Failed to unregister device (failed to get device id char ptr)");
        }
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_080: [if `deviceHandle` has a NULL reference to its transport instance, IoTHubTransport_AMQP_Common_Unregister shall return.]
        else if (registered_device->transport_instance == NULL)
        {
            LogError("Failed to unregister device '%s' (deviceHandle does not have a transport state associated to).", device_id);
        }
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_081: [If the device is not registered with this transport, IoTHubTransport_AMQP_Common_Unregister shall return]
        else if (!is_device_registered_ex(registered_device->transport_instance->registered_devices, device_id, &list_item))
        {
            LogError("Failed to unregister device '%s' (device is not registered within this transport).", device_id);
        }
        else
        {
            // Removing it first so the race hazzard is reduced between this function and DoWork. Best would be to use locks.
            if (singlylinkedlist_remove(registered_device->transport_instance->registered_devices, list_item) != RESULT_OK)
            {
                LogError("Failed to unregister device '%s' (singlylinkedlist_remove failed).", device_id);
            }
            else
            {
                // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_012: [IoTHubTransport_AMQP_Common_Unregister shall destroy the C2D methods handler by calling iothubtransportamqp_methods_destroy]
                // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_083: [IoTHubTransport_AMQP_Common_Unregister shall free all the memory allocated for the `device_instance`]
                internal_destroy_amqp_device_instance(registered_device);
            }
        }
    }
}

void IoTHubTransport_AMQP_Common_Destroy(TRANSPORT_LL_HANDLE handle)
{
    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_013: [If `handle` is NULL, IoTHubTransport_AMQP_Common_Destroy shall return immediatelly]
    if (handle == NULL)
    {
        LogError("Failed to destroy AMQP transport instance (handle is NULL)");
    }
    else
    {
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_014: [IoTHubTransport_AMQP_Common_Destroy shall invoke IoTHubTransport_AMQP_Common_Unregister on each of its registered devices.]
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_015: [All members of `instance` (including tls_io) shall be destroyed and its memory released]
        internal_destroy_instance((AMQP_TRANSPORT_INSTANCE*)handle);
    }
}

int IoTHubTransport_AMQP_Common_SetRetryPolicy(TRANSPORT_LL_HANDLE handle, IOTHUB_CLIENT_RETRY_POLICY retryPolicy, size_t retryTimeoutLimitInSeconds)
{
    int result;

    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_128: [If `handle` is NULL, `IoTHubTransport_AMQP_Common_SetRetryPolicy` shall fail and return non-zero.]
    if (handle == NULL)
    {
        LogError("Cannot set retry policy (transport handle is NULL)");
        result = MU_FAILURE;
    }
    else
    {
        RETRY_CONTROL_HANDLE new_retry_control;

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_129: [`transport_instance->connection_retry_control` shall be set using retry_control_create(), passing `retryPolicy` and `retryTimeoutLimitInSeconds`.]
        if ((new_retry_control = retry_control_create(retryPolicy, (unsigned int)retryTimeoutLimitInSeconds)) == NULL)
        {
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_130: [If retry_control_create() fails, `IoTHubTransport_AMQP_Common_SetRetryPolicy` shall fail and return non-zero.]
            LogError("Cannot set retry policy (retry_control_create failed)");
            result = MU_FAILURE;
        }
        else
        {
            AMQP_TRANSPORT_INSTANCE* transport_instance = (AMQP_TRANSPORT_INSTANCE*)handle;
            RETRY_CONTROL_HANDLE previous_retry_control = transport_instance->connection_retry_control;

            transport_instance->connection_retry_control = new_retry_control;

            retry_control_destroy(previous_retry_control);

            if (transport_instance->state == AMQP_TRANSPORT_STATE_NOT_CONNECTED_NO_MORE_RETRIES)
            {
                transport_instance->state = AMQP_TRANSPORT_STATE_RECONNECTION_REQUIRED;
            }

            // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_128: [If no errors occur, `IoTHubTransport_AMQP_Common_SetRetryPolicy` shall return zero.]
            result = RESULT_OK;
        }
    }

    return result;
}

STRING_HANDLE IoTHubTransport_AMQP_Common_GetHostname(TRANSPORT_LL_HANDLE handle)
{
    STRING_HANDLE result;

    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_02_001: [If `handle` is NULL, `IoTHubTransport_AMQP_Common_GetHostname` shall return NULL.]
    if (handle == NULL)
    {
        LogError("Cannot provide the target host name (transport handle is NULL).");

        result = NULL;
    }
    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_02_002: [IoTHubTransport_AMQP_Common_GetHostname shall return a copy of `instance->iothub_target_fqdn`.]
    else if ((result = STRING_clone(((AMQP_TRANSPORT_INSTANCE*)(handle))->iothub_host_fqdn)) == NULL)
    {
        LogError("Cannot provide the target host name (STRING_clone failed).");
    }

    return result;
}

IOTHUB_CLIENT_RESULT IoTHubTransport_AMQP_Common_SendMessageDisposition(MESSAGE_CALLBACK_INFO* message_data, IOTHUBMESSAGE_DISPOSITION_RESULT disposition)
{
    IOTHUB_CLIENT_RESULT result;
    if (message_data == NULL)
    {
        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_10_001: [If messageData is NULL, IoTHubTransport_AMQP_Common_SendMessageDisposition shall fail and return IOTHUB_CLIENT_INVALID_ARG.] */
        LogError("Failed sending message disposition (message_data is NULL)");
        result = IOTHUB_CLIENT_INVALID_ARG;
    }
    else
    {
        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_10_002: [If any of the messageData fields are NULL, IoTHubTransport_AMQP_Common_SendMessageDisposition shall fail and return IOTHUB_CLIENT_INVALID_ARG.] */
        if (message_data->messageHandle == NULL || message_data->transportContext == NULL)
        {
            LogError("Failed sending message disposition (message_data->messageHandle (%p) or message_data->transportContext (%p) are NULL)", message_data->messageHandle, message_data->transportContext);
            result = IOTHUB_CLIENT_INVALID_ARG;
        }
        else
        {
            DEVICE_MESSAGE_DISPOSITION_INFO* device_message_disposition_info;

            /* Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_10_004: [IoTHubTransport_AMQP_Common_SendMessageDisposition shall convert the given IOTHUBMESSAGE_DISPOSITION_RESULT to the equivalent AMQP_VALUE and will return the result of calling messagereceiver_send_message_disposition. ] */
            DEVICE_MESSAGE_DISPOSITION_RESULT device_disposition_result = get_device_disposition_result_from(disposition);

            // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_112: [A DEVICE_MESSAGE_DISPOSITION_INFO instance shall be created with a copy of the `link_name` and `message_id` contained in `message_data`]
            if ((device_message_disposition_info = create_device_message_disposition_info_from(message_data)) == NULL)
            {
                // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_113: [If the DEVICE_MESSAGE_DISPOSITION_INFO fails to be created, `IoTHubTransport_AMQP_Common_SendMessageDisposition()` shall fail and return IOTHUB_CLIENT_ERROR]
                LogError("Device '%s' failed sending message disposition (failed creating DEVICE_MESSAGE_DISPOSITION_RESULT)", STRING_c_str(message_data->transportContext->device_state->device_id));
                result = IOTHUB_CLIENT_ERROR;
            }
            else
            {
                /* Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_10_003: [IoTHubTransport_AMQP_Common_SendMessageDisposition shall fail and return IOTHUB_CLIENT_ERROR if the POST message fails, otherwise return IOTHUB_CLIENT_OK.] */
                if (amqp_device_send_message_disposition(message_data->transportContext->device_state->device_handle, device_message_disposition_info, device_disposition_result) != RESULT_OK)
                {
                    LogError("Device '%s' failed sending message disposition (amqp_device_send_message_disposition failed)", STRING_c_str(message_data->transportContext->device_state->device_id));
                    result = IOTHUB_CLIENT_ERROR;
                }
                else
                {
                    IoTHubMessage_Destroy(message_data->messageHandle);
                    result = IOTHUB_CLIENT_OK;
                }

                // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_114: [`IoTHubTransport_AMQP_Common_SendMessageDisposition()` shall destroy the DEVICE_MESSAGE_DISPOSITION_INFO instance]
                destroy_device_message_disposition_info(device_message_disposition_info);
            }
        }

        MESSAGE_CALLBACK_INFO_Destroy(message_data);
    }

    return result;
}

int IoTHubTransport_AMQP_SetCallbackContext(TRANSPORT_LL_HANDLE handle, void* ctx)
{
    int result;
    if (handle == NULL)
    {
        LogError("Invalid parameter specified handle: %p", handle);
        result = MU_FAILURE;
    }
    else
    {
        AMQP_TRANSPORT_INSTANCE* instance = (AMQP_TRANSPORT_INSTANCE*)handle;
        instance->transport_ctx = ctx;
        result = 0;
    }
    return result;
}

int IoTHubTransport_AMQP_Common_GetSupportedPlatformInfo(TRANSPORT_LL_HANDLE handle, PLATFORM_INFO_OPTION* info)
{
    int result;

    if (handle == NULL || info == NULL)
    {
        LogError("Invalid parameter specified (handle: %p, info: %p)", handle, info);
        result = MU_FAILURE;
    }
    else
    {
        *info = PLATFORM_INFO_OPTION_RETRIEVE_SQM;
        result = 0;
    }

    return result;
}