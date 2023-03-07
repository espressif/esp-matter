// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <stddef.h>
#include "azure_c_shared_utility/optimize_size.h"
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/strings.h"
#include "azure_uamqp_c/link.h"
#include "azure_uamqp_c/messaging.h"
#include "azure_uamqp_c/message_receiver.h"
#include "azure_uamqp_c/message_sender.h"
#include "azure_uamqp_c/amqp_definitions_application_properties.h"

#include "internal/iothubtransportamqp_methods.h"
#include "internal/iothub_internal_consts.h"

typedef enum SUBSCRIBE_STATE_TAG
{
    SUBSCRIBE_STATE_NOT_SUBSCRIBED,
    SUBSCRIBE_STATE_SUBSCRIBED
} SUBSCRIBE_STATE;

typedef struct IOTHUBTRANSPORT_AMQP_METHODS_TAG
{
    char* device_id;
    char* module_id;
    char* hostname;
    LINK_HANDLE receiver_link;
    LINK_HANDLE sender_link;
    MESSAGE_RECEIVER_HANDLE message_receiver;
    MESSAGE_SENDER_HANDLE message_sender;
    ON_METHOD_REQUEST_RECEIVED on_method_request_received;
    void* on_method_request_received_context;
    ON_METHODS_ERROR on_methods_error;
    void* on_methods_error_context;
    ON_METHODS_UNSUBSCRIBED on_methods_unsubscribed;
    void* on_methods_unsubscribed_context;
    SUBSCRIBE_STATE subscribe_state;
    IOTHUBTRANSPORT_AMQP_METHOD_HANDLE* method_request_handles;
    size_t method_request_handle_count;
    bool receiver_link_disconnected;
    bool sender_link_disconnected;
} IOTHUBTRANSPORT_AMQP_METHODS;

typedef enum MESSAGE_OUTCOME_TAG
{
    MESSAGE_OUTCOME_ACCEPTED,
    MESSAGE_OUTCOME_REJECTED,
    MESSAGE_OUTCOME_RELEASED
} MESSAGE_OUTCOME;

typedef struct IOTHUBTRANSPORT_AMQP_METHOD_TAG
{
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE iothubtransport_amqp_methods_handle;
    uuid correlation_id;
} IOTHUBTRANSPORT_AMQP_METHOD;

static void remove_tracked_handle(IOTHUBTRANSPORT_AMQP_METHODS* amqp_methods_handle, IOTHUBTRANSPORT_AMQP_METHOD_HANDLE method_request_handle)
{
    size_t i;

    for (i = 0; i < amqp_methods_handle->method_request_handle_count; i++)
    {
        if (amqp_methods_handle->method_request_handles[i] == method_request_handle)
        {
            if (amqp_methods_handle->method_request_handle_count - i > 1)
            {
                (void)memmove(&amqp_methods_handle->method_request_handles[i], &amqp_methods_handle->method_request_handles[i + 1],
                    (amqp_methods_handle->method_request_handle_count - i - 1) * sizeof(IOTHUBTRANSPORT_AMQP_METHOD_HANDLE));
            }

            amqp_methods_handle->method_request_handle_count--;
            i--;
        }
    }

    if (amqp_methods_handle->method_request_handle_count == 0)
    {
        free(amqp_methods_handle->method_request_handles);
        amqp_methods_handle->method_request_handles = NULL;
    }
    else
    {
        IOTHUBTRANSPORT_AMQP_METHOD_HANDLE* new_handles = (IOTHUBTRANSPORT_AMQP_METHOD_HANDLE*)realloc(amqp_methods_handle->method_request_handles, amqp_methods_handle->method_request_handle_count * sizeof(IOTHUBTRANSPORT_AMQP_METHOD_HANDLE));
        if (new_handles != NULL)
        {
            amqp_methods_handle->method_request_handles = new_handles;
        }
    }
}

IOTHUBTRANSPORT_AMQP_METHODS_HANDLE iothubtransportamqp_methods_create(const char* hostname, const char* device_id, const char* module_id)
{
    IOTHUBTRANSPORT_AMQP_METHODS* result;

    if ((hostname == NULL) ||
        (device_id == NULL))
    {
        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_002: [ If any argument is NULL, `iothubtransportamqp_methods_create` shall return NULL. ]*/
        result = NULL;
        LogError("Bad arguments: hostname=%p, device_id=%p", hostname, device_id);
    }
    else
    {
        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_001: [ `iothubtransportamqp_methods_create` shall instantiate a new handler for C2D methods over AMQP for device `device_id` and on success return a non-NULL handle to it. ]*/
        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_003: [ `iothubtransportamqp_methods_create` shall allocate memory for the new instance. ]*/
        result = malloc(sizeof(IOTHUBTRANSPORT_AMQP_METHODS));
        if (result == NULL)
        {
            /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_004: [ If allocating memory fails, `iothubtransportamqp_methods_create` shall return NULL. ]*/
            LogError("Cannot allocate memory for AMQP C2D methods handle");
        }
        else
        {
            memset(result, 0, sizeof(IOTHUBTRANSPORT_AMQP_METHODS));

            /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_115: [ `iothubtransportamqp_methods_create` shall save the device id for later use by using `mallocAndStrcpy_s`. ]*/
            if (mallocAndStrcpy_s(&result->device_id, device_id) != 0)
            {
                /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_116: [ If `mallocAndStrcpy_s` fails, `iothubtransportamqp_methods_create` shall return NULL. ]*/
                LogError("Cannot copy device_id");
                free(result);
                result = NULL;
            }
            else if ((module_id != NULL) && (mallocAndStrcpy_s(&result->module_id, module_id) != 0))
            {
                /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_116: [ If `mallocAndStrcpy_s` fails, `iothubtransportamqp_methods_create` shall return NULL. ]*/
                LogError("Cannot copy device_id");
                free(result->device_id);
                free(result);
                result = NULL;
            }
            else
            {
                /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_139: [ `iothubtransportamqp_methods_create` shall save the `hostname` for later use by using `mallocAndStrcpy_s`. ]*/
                if (mallocAndStrcpy_s(&result->hostname, hostname) != 0)
                {
                    /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_116: [ If `mallocAndStrcpy_s` fails, `iothubtransportamqp_methods_create` shall return NULL. ]*/
                    LogError("Cannot copy hostname");
                    free(result->device_id);
                    free(result->module_id);
                    free(result);
                    result = NULL;
                }
                else
                {
                    result->subscribe_state = SUBSCRIBE_STATE_NOT_SUBSCRIBED;
                    result->method_request_handles = NULL;
                    result->method_request_handle_count = 0;
                    result->receiver_link_disconnected = false;
                    result->sender_link_disconnected = false;
                }
            }
        }
    }

    return result;
}

void iothubtransportamqp_methods_destroy(IOTHUBTRANSPORT_AMQP_METHODS_HANDLE iothubtransport_amqp_methods_handle)
{
    /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_006: [ If `iothubtransport_amqp_methods_handle` is NULL, `iothubtransportamqp_methods_destroy` shall do nothing. ]*/
    if (iothubtransport_amqp_methods_handle == NULL)
    {
        LogError("NULL handle");
    }
    else
    {
        size_t i;

        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_007: [ If the instance pointed to by `iothubtransport_amqp_methods_handle` is subscribed to receive C2D methods, `iothubtransportamqp_methods_destroy` shall free all resources allocated by the subscribe. ]*/
        if (iothubtransport_amqp_methods_handle->subscribe_state == SUBSCRIBE_STATE_SUBSCRIBED)
        {
            iothubtransportamqp_methods_unsubscribe(iothubtransport_amqp_methods_handle);
        }

        for (i = 0; i < iothubtransport_amqp_methods_handle->method_request_handle_count; i++)
        {
            free(iothubtransport_amqp_methods_handle->method_request_handles[i]);
        }

        if (iothubtransport_amqp_methods_handle->method_request_handles != NULL)
        {
            free(iothubtransport_amqp_methods_handle->method_request_handles);
        }

        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_005: [ `iothubtransportamqp_methods_destroy` shall free all resources allocated by `iothubtransportamqp_methods_create` for the handle `iothubtransport_amqp_methods_handle`. ]*/
        free(iothubtransport_amqp_methods_handle->hostname);
        free(iothubtransport_amqp_methods_handle->device_id);
        free(iothubtransport_amqp_methods_handle->module_id);
        free(iothubtransport_amqp_methods_handle);
    }
}

static void call_methods_unsubscribed_if_needed(IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle)
{
    if (amqp_methods_handle->receiver_link_disconnected && amqp_methods_handle->sender_link_disconnected)
    {
        amqp_methods_handle->receiver_link_disconnected = false;
        amqp_methods_handle->sender_link_disconnected = false;
        amqp_methods_handle->on_methods_unsubscribed(amqp_methods_handle->on_methods_unsubscribed_context);
    }
}

static void on_message_receiver_state_changed(const void* context, MESSAGE_RECEIVER_STATE new_state, MESSAGE_RECEIVER_STATE previous_state)
{
    /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_119: [ When `on_message_receiver_state_changed` is called with the `new_state` being `MESSAGE_RECEIVER_STATE_ERROR`, an error shall be indicated by calling the `on_methods_error` callback passed to `iothubtransportamqp_methods_subscribe`. ]*/
    /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_126: [ For the other state changes, on_message_receiver_state_changed shall do nothing. ]*/
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle = (IOTHUBTRANSPORT_AMQP_METHODS_HANDLE)context;
    if ((new_state != previous_state) &&
        (new_state == MESSAGE_RECEIVER_STATE_ERROR))
    {
        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_120: [ When an error is indicated by calling the `on_methods_error`, it shall be called with the context being the `on_methods_error_context` argument passed to `iothubtransportamqp_methods_subscribe`. ]*/
        amqp_methods_handle->on_methods_error(amqp_methods_handle->on_methods_error_context);
    }
    else
    {
        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_12_001: [ When `on_message_receiver_state_changed` is called with the `new_state` being `MESSAGE_RECEIVER_STATE_IDLE` and `previous_state` being `MESSAGE_RECEIVER_STATE_OPEN`and the sender link is already diconnected `on_message_receiver_state_changed` calls to `on_methods_unsubscribed`. ]*/
        if ((new_state == MESSAGE_RECEIVER_STATE_IDLE) && (previous_state == MESSAGE_RECEIVER_STATE_OPEN))
        {
            amqp_methods_handle->receiver_link_disconnected = true;
        }
        call_methods_unsubscribed_if_needed(amqp_methods_handle);
    }
}

static void on_message_sender_state_changed(void* context, MESSAGE_SENDER_STATE new_state, MESSAGE_SENDER_STATE previous_state)
{
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle = (IOTHUBTRANSPORT_AMQP_METHODS_HANDLE)context;
    /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_059: [ When `on_message_sender_state_changed` is called with the `new_state` being `MESSAGE_SENDER_STATE_ERROR`, an error shall be indicated by calling the `on_methods_error` callback passed to `iothubtransportamqp_methods_subscribe`. ]*/
    /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_127: [ For the other state changes, on_message_sender_state_changed shall do nothing. ]*/
    if ((new_state != previous_state) &&
        (new_state == MESSAGE_SENDER_STATE_ERROR))
    {
        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_120: [ When an error is indicated by calling the `on_methods_error`, it shall be called with the context being the `on_methods_error_context` argument passed to `iothubtransportamqp_methods_subscribe`. ]*/
        amqp_methods_handle->on_methods_error(amqp_methods_handle->on_methods_error_context);
    }
    else
    {
        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_12_002: [ When `on_message_sender_state_changed` is called with the `new_state` being `MESSAGE_SENDER_STATE_IDLE` and `previous_state` being `MESSAGE_SENDER_STATE_OPEN`and the receiver link is already diconnected `on_message_sender_state_changed` calls to `on_methods_unsubscribed`. ]*/
        if ((new_state == MESSAGE_SENDER_STATE_IDLE) && (previous_state == MESSAGE_SENDER_STATE_OPEN))
        {
            amqp_methods_handle->sender_link_disconnected = true;
        }
        call_methods_unsubscribed_if_needed(amqp_methods_handle);
    }
}

static void on_message_send_complete(void* context, MESSAGE_SEND_RESULT send_result, AMQP_VALUE delivery_state)
{
    (void)delivery_state;
    /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_084: [ Otherwise no action shall be taken. ]*/
    if (send_result == MESSAGE_SEND_ERROR)
    {
        IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle = (IOTHUBTRANSPORT_AMQP_METHODS_HANDLE)context;
        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_083: [ If `send_result` is `MESSAGE_SEND_ERROR` then an error shall be indicated by calling the `on_methods_error` callback passed to `iothubtransportamqp_methods_subscribe`. ]*/
        amqp_methods_handle->on_methods_error(amqp_methods_handle->on_methods_error_context);
    }
}

static AMQP_VALUE on_message_received(const void* context, MESSAGE_HANDLE message)
{
    PROPERTIES_HANDLE properties;
    /* VS believes this is not initialized, so have to set it to the worse case here */
    AMQP_VALUE result = NULL;
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle = (IOTHUBTRANSPORT_AMQP_METHODS_HANDLE)context;
    MESSAGE_OUTCOME message_outcome;

    if (message == NULL)
    {
        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_041: [ If `message` is NULL, the RELEASED outcome shall be returned and an error shall be indicated. ]*/
        LogError("NULL message");
        message_outcome = MESSAGE_OUTCOME_RELEASED;
    }
    else
    {
        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_043: [ When `on_message_received` is called (to indicate a new message being received over the receiver link), the message shall be processed as below: ]*/
        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_044: [ - The message properties shall be obtained by calling `message_get_properties`. ]*/
        if (message_get_properties(message, &properties) != 0)
        {
            /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_045: [ If `message_get_properties` fails, the REJECTED outcome with `amqp:decode-error` shall be returned. ]*/
            LogError("Cannot retrieve message properties");
            message_outcome = MESSAGE_OUTCOME_REJECTED;
            result = messaging_delivery_rejected("amqp:decode-error", "Cannot retrieve message properties");
        }
        else
        {
            AMQP_VALUE correlation_id;

            /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_046: [ - The correlation id shall be obtained by calling `properties_get_correlation_id` on the message properties. ]*/
            if (properties_get_correlation_id(properties, &correlation_id) != 0)
            {
                /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_047: [ If `properties_get_correlation_id` fails the REJECTED outcome with `amqp:decode-error` shall be returned. ]*/
                LogError("Cannot retrieve correlation id");
                message_outcome = MESSAGE_OUTCOME_REJECTED;
                result = messaging_delivery_rejected("amqp:decode-error", "Cannot retrieve correlation id");
            }
            else
            {
                /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_112: [ Memory shall be allocated for the `IOTHUBTRANSPORT_AMQP_METHOD_HANDLE` to hold the correlation-id, so that it can be used in the `iothubtransportamqp_methods_respond` function. ]*/
                IOTHUBTRANSPORT_AMQP_METHOD* method_handle = (IOTHUBTRANSPORT_AMQP_METHOD*)malloc(sizeof(IOTHUBTRANSPORT_AMQP_METHOD));
                if (method_handle == NULL)
                {
                    /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_130: [ If allocating memory for the `IOTHUBTRANSPORT_AMQP_METHOD_HANDLE` handle fails, the RELEASED outcome shall be returned and an error shall be indicated. ]*/
                    LogError("Cannot allocate method handle");
                    message_outcome = MESSAGE_OUTCOME_RELEASED;
                }
                else
                {
                    IOTHUBTRANSPORT_AMQP_METHOD_HANDLE* new_handles;
                    /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_113: [ All `IOTHUBTRANSPORT_AMQP_METHOD_HANDLE` handles shall be tracked in an array of handles that shall be resized accordingly when a methopd handle is added to it. ]*/
                    new_handles = (IOTHUBTRANSPORT_AMQP_METHOD_HANDLE*)realloc(amqp_methods_handle->method_request_handles, (amqp_methods_handle->method_request_handle_count + 1) * sizeof(IOTHUBTRANSPORT_AMQP_METHOD_HANDLE));
                    if (new_handles == NULL)
                    {
                        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_138: [ If resizing the tracked method handles array fails, the RELEASED outcome shall be returned and an error shall be indicated. ]*/
                        free(method_handle);
                        LogError("Cannot grow method handles array");
                        message_outcome = MESSAGE_OUTCOME_RELEASED;
                    }
                    else
                    {
                        amqp_methods_handle->method_request_handles = new_handles;

                        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_121: [ The uuid value for the correlation ID shall be obtained by calling `amqpvalue_get_uuid`. ]*/
                        if (amqpvalue_get_uuid(correlation_id, &method_handle->correlation_id) != 0)
                        {
                            /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_122: [ If `amqpvalue_get_uuid` fails the REJECTED outcome with `amqp:decode-error` shall be returned. ]*/
                            free(method_handle);
                            LogError("Cannot get uuid value for correlation-id");
                            message_outcome = MESSAGE_OUTCOME_REJECTED;
                            result = messaging_delivery_rejected("amqp:decode-error", "Cannot get uuid value for correlation-id");
                        }
                        else
                        {
                            BINARY_DATA binary_data;

                            /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_048: [ - The message payload shall be obtained by calling `message_get_body_amqp_data_in_place` with the index argument being 0. ]*/
                            if (message_get_body_amqp_data_in_place(message, 0, &binary_data) != 0)
                            {
                                /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_049: [ If `message_get_body_amqp_data_in_place` fails the REJECTED outcome with `amqp:decode-error` shall be returned. ]*/
                                free(method_handle);
                                LogError("Cannot get method request message payload");
                                message_outcome = MESSAGE_OUTCOME_REJECTED;
                                result = messaging_delivery_rejected("amqp:decode-error", "Cannot get method request message payload");
                            }
                            else
                            {
                                AMQP_VALUE application_properties;

                                /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_099: [ The application properties for the received message shall be obtained by calling `message_get_application_properties`. ]*/
                                if (message_get_application_properties(message, &application_properties) != 0)
                                {
                                    /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_133: [ If `message_get_application_properties` fails the REJECTED outcome with `amqp:decode-error` shall be returned. ]*/
                                    LogError("Cannot get application properties");
                                    free(method_handle);
                                    message_outcome = MESSAGE_OUTCOME_REJECTED;
                                    result = messaging_delivery_rejected("amqp:decode-error", "Cannot get application properties");
                                }
                                else
                                {
                                    /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_123: [ The AMQP map shall be retrieve from the application properties by calling `amqpvalue_get_inplace_described_value`. ]*/
                                    AMQP_VALUE amqp_properties_map = amqpvalue_get_inplace_described_value(application_properties);
                                    if (amqp_properties_map == NULL)
                                    {
                                        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_134: [ If `amqpvalue_get_inplace_described_value` fails the RELEASED outcome with `amqp:decode-error` shall be returned. ]*/
                                        LogError("Cannot get application properties map");
                                        free(method_handle);
                                        message_outcome = MESSAGE_OUTCOME_RELEASED;
                                    }
                                    else
                                    {
                                        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_100: [ A property key `IoThub-methodname` shall be created by calling `amqpvalue_create_string`. ]*/
                                        AMQP_VALUE property_key = amqpvalue_create_string("IoThub-methodname");
                                        if (property_key == NULL)
                                        {
                                            /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_132: [ If `amqpvalue_create_string` fails the RELEASED outcome shall be returned. ]*/
                                            LogError("Cannot create the property key for method name");
                                            free(method_handle);
                                            message_outcome = MESSAGE_OUTCOME_RELEASED;
                                        }
                                        else
                                        {
                                            /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_101: [ The method name property value shall be found in the map by calling `amqpvalue_get_map_value`. ]*/
                                            AMQP_VALUE property_value = amqpvalue_get_map_value(amqp_properties_map, property_key);
                                            if (property_value == NULL)
                                            {
                                                /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_103: [ If `amqpvalue_get_map_value` fails the REJECTED outcome with `amqp:decode-error` shall be returned. ]*/
                                                LogError("Cannot find the IoThub-methodname property in the properties map");
                                                free(method_handle);
                                                message_outcome = MESSAGE_OUTCOME_REJECTED;
                                                result = messaging_delivery_rejected("amqp:decode-error", "Cannot find the IoThub-methodname property in the properties map");
                                            }
                                            else
                                            {
                                                const char* method_name;

                                                /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_102: [ The string contained by the property value shall be obtained by calling `amqpvalue_get_string`. ]*/
                                                if (amqpvalue_get_string(property_value, &method_name) != 0)
                                                {
                                                    /*Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_131: [ If `amqpvalue_get_string` fails the REJECTED outcome with `amqp:decode-error` shall be returned. ]*/
                                                    LogError("Cannot read the method name from the property value");
                                                    free(method_handle);
                                                    message_outcome = MESSAGE_OUTCOME_REJECTED;
                                                    result = messaging_delivery_rejected("amqp:decode-error", "Cannot read the method name from the property value");
                                                }
                                                else
                                                {
                                                    /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_056: [ On success the `on_message_received` callback shall return a newly constructed delivery state obtained by calling `messaging_delivery_accepted`. ]*/
                                                    result = messaging_delivery_accepted();
                                                    if (result == NULL)
                                                    {
                                                        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_057: [ If `messaging_delivery_accepted` fails the RELEASED outcome with `amqp:decode-error` shall be returned. ]*/
                                                        LogError("Cannot allocate memory for delivery state");
                                                        free(method_handle);
                                                        message_outcome = MESSAGE_OUTCOME_RELEASED;
                                                    }
                                                    else
                                                    {
                                                        method_handle->iothubtransport_amqp_methods_handle = amqp_methods_handle;

                                                        /* set the method request handle in the handle array */
                                                        amqp_methods_handle->method_request_handles[amqp_methods_handle->method_request_handle_count] = method_handle;
                                                        amqp_methods_handle->method_request_handle_count++;

                                                        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_050: [ The binary message payload shall be indicated by calling the `on_method_request_received` callback passed to `iothubtransportamqp_methods_subscribe` with the arguments: ]*/
                                                        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_051: [ - `context` shall be set to the `on_method_request_received_context` argument passed to `iothubtransportamqp_methods_subscribe`. ]*/
                                                        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_098: [ - `method_name` shall be set to the application property value for `IoThub-methodname`. ]*/
                                                        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_052: [ - `request` shall be set to the payload bytes obtained by calling `message_get_body_amqp_data_in_place`. ]*/
                                                        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_053: [ - `request_size` shall be set to the payload size obtained by calling `message_get_body_amqp_data_in_place`. ]*/
                                                        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_054: [ - `method_handle` shall be set to a newly created `IOTHUBTRANSPORT_AMQP_METHOD_HANDLE` that can be passed later as an argument to `iothubtransportamqp_methods_respond`. ]*/
                                                        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_109: [ `iothubtransportamqp_methods_respond` shall be allowed to be called from the callback `on_method_request_received`. ]*/
                                                        if (amqp_methods_handle->on_method_request_received(amqp_methods_handle->on_method_request_received_context, method_name, binary_data.bytes, binary_data.length, method_handle) != 0)
                                                        {
                                                            /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_147: [ If `on_method_request_received` fails, the REJECTED outcome shall be returned with `amqp:internal-error`. ]*/
                                                            LogError("Cannot execute the callback with the given data");
                                                            amqpvalue_destroy(result);
                                                            free(method_handle);
                                                            amqp_methods_handle->method_request_handle_count--;
                                                            message_outcome = MESSAGE_OUTCOME_REJECTED;
                                                            result = messaging_delivery_rejected("amqp:internal-error", "Cannot execute the callback with the given data");
                                                        }
                                                        else
                                                        {
                                                            message_outcome = MESSAGE_OUTCOME_ACCEPTED;
                                                        }
                                                    }
                                                }

                                                amqpvalue_destroy(property_value);
                                            }

                                            amqpvalue_destroy(property_key);
                                        }
                                    }

                                    application_properties_destroy(application_properties);
                                }
                            }
                        }
                    }
                }
            }

            properties_destroy(properties);
        }
    }

    switch (message_outcome)
    {
    default:
        break;

    case MESSAGE_OUTCOME_RELEASED:
        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_129: [ The released outcome shall be created by calling `messaging_delivery_released`. ]*/
        result = messaging_delivery_released();

        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_128: [ When the RELEASED outcome is returned, an error shall be indicated by calling the `on_methods_error` callback passed to `iothubtransportamqp_methods_subscribe`. ]*/
        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_042: [ When an error is indicated by calling the `on_methods_error`, it shall be called with the context being the `on_methods_error_context` argument passed to `iothubtransportamqp_methods_subscribe`. ]*/
        amqp_methods_handle->on_methods_error(amqp_methods_handle->on_methods_error_context);
        break;

    case MESSAGE_OUTCOME_REJECTED:
    case MESSAGE_OUTCOME_ACCEPTED:
        /* all is well */
        break;
    }

    return result;
}

STRING_HANDLE create_correlation_id(IOTHUBTRANSPORT_AMQP_METHODS_HANDLE iothubtransport_amqp_methods_handle)
{
    if (iothubtransport_amqp_methods_handle->module_id != NULL)
    {
        return STRING_construct_sprintf("%s/%s", iothubtransport_amqp_methods_handle->device_id, iothubtransport_amqp_methods_handle->module_id);
    }
    else
    {
        return STRING_construct(iothubtransport_amqp_methods_handle->device_id);
    }
}

STRING_HANDLE create_peer_endpoint_name(IOTHUBTRANSPORT_AMQP_METHODS_HANDLE iothubtransport_amqp_methods_handle)
{
    if (iothubtransport_amqp_methods_handle->module_id != NULL)
    {
        return STRING_construct_sprintf("amqps://%s/devices/%s/modules/%s/methods/devicebound", iothubtransport_amqp_methods_handle->hostname, iothubtransport_amqp_methods_handle->device_id, iothubtransport_amqp_methods_handle->module_id);
    }
    else
    {
        return STRING_construct_sprintf("amqps://%s/devices/%s/methods/devicebound", iothubtransport_amqp_methods_handle->hostname, iothubtransport_amqp_methods_handle->device_id);
    }
}

STRING_HANDLE create_requests_link_name(IOTHUBTRANSPORT_AMQP_METHODS_HANDLE iothubtransport_amqp_methods_handle)
{
    if (iothubtransport_amqp_methods_handle->module_id != NULL)
    {
        return STRING_construct_sprintf("methods_requests_link-%s/%s", iothubtransport_amqp_methods_handle->device_id, iothubtransport_amqp_methods_handle->module_id);
    }
    else
    {
        return STRING_construct_sprintf("methods_requests_link-%s", iothubtransport_amqp_methods_handle->device_id);
    }
}

STRING_HANDLE create_responses_link_name(IOTHUBTRANSPORT_AMQP_METHODS_HANDLE iothubtransport_amqp_methods_handle)
{
    if (iothubtransport_amqp_methods_handle->module_id != NULL)
    {
        return STRING_construct_sprintf("methods_responses_link-%s/%s", iothubtransport_amqp_methods_handle->device_id, iothubtransport_amqp_methods_handle->module_id);
    }
    else
    {
        return STRING_construct_sprintf("methods_responses_link-%s", iothubtransport_amqp_methods_handle->device_id);
    }
}



static int set_link_attach_properties(IOTHUBTRANSPORT_AMQP_METHODS_HANDLE iothubtransport_amqp_methods_handle)
{
    int result = 0;
    fields link_attach_properties;

    /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_140: [ A link attach properties map shall be created by calling `amqpvalue_create_map`. ]*/
    link_attach_properties = amqpvalue_create_map();
    if (link_attach_properties == NULL)
    {
        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_145: [ If any call for creating or setting the link attach properties fails `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. ]*/
        LogError("Cannot create the map for link attach properties");
        result = MU_FAILURE;
    }
    else
    {
        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_141: [ A property key which shall be a symbol named `com.microsoft:channel-correlation-id` shall be created by calling `amqp_create_symbol`. ]*/
        AMQP_VALUE channel_correlation_id_property_key = amqpvalue_create_symbol("com.microsoft:channel-correlation-id");
        if (channel_correlation_id_property_key == NULL)
        {
            /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_145: [ If any call for creating or setting the link attach properties fails `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. ]*/
            LogError("Cannot create the channel correlation id property key for the link attach properties");
            result = MU_FAILURE;
        }
        else
        {
            /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_142: [ A property value of type string that shall contain the device id (and "/" + module id if module is present) shall be created by calling `amqpvalue_create_string`. ]*/
            STRING_HANDLE correlation_id = NULL;
            AMQP_VALUE channel_correlation_id_property_value = NULL;

            if ((correlation_id = create_correlation_id(iothubtransport_amqp_methods_handle)) == NULL)
            {
                /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_145: [ If any call for creating or setting the link attach properties fails `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. ]*/
                LogError("Cannot create the channel correlation id string for the link attach properties");
                result = MU_FAILURE;
            }
            else if ((channel_correlation_id_property_value = amqpvalue_create_string(STRING_c_str(correlation_id))) == NULL)
            {
                /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_145: [ If any call for creating or setting the link attach properties fails `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. ]*/
                LogError("Cannot create the channel correlation id property key for the link attach properties");
                result = MU_FAILURE;
            }
            else
            {
                /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_143: [ The `com.microsoft:channel-correlation-id` shall be added to the link attach properties by calling `amqpvalue_set_map_value`. ]*/
                if (amqpvalue_set_map_value(link_attach_properties, channel_correlation_id_property_key, channel_correlation_id_property_value) != 0)
                {
                    /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_145: [ If any call for creating or setting the link attach properties fails `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. ]*/
                    LogError("Cannot set the property for channel correlation on the link attach properties");
                    result = MU_FAILURE;
                }
                else
                {
                    /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_150: [ A property key which shall be a symbol named `com.microsoft:api-version` shall be created by calling `amqp_create_symbol`. ]*/
                    AMQP_VALUE api_version_property_key = amqpvalue_create_symbol("com.microsoft:api-version");
                    if (api_version_property_key == NULL)
                    {
                        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_145: [ If any call for creating or setting the link attach properties fails `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. ]*/
                        LogError("Cannot create the API version property key for the link attach properties");
                        result = MU_FAILURE;
                    }
                    else
                    {
                        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_151: [ A property value of type string that shall contain the `Current API version` shall be created by calling `amqpvalue_create_string`. ]*/
                        AMQP_VALUE api_version_property_value = amqpvalue_create_string(IOTHUB_API_VERSION);
                        if (api_version_property_value == NULL)
                        {
                            /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_145: [ If any call for creating or setting the link attach properties fails `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. ]*/
                            LogError("Cannot create the API version property value for the link attach properties");
                            result = MU_FAILURE;
                        }
                        else
                        {
                            /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_152: [ The `com.microsoft:api-version` shall be added to the link attach properties by calling `amqpvalue_set_map_value`. ]*/
                            if (amqpvalue_set_map_value(link_attach_properties, api_version_property_key, api_version_property_value) != 0)
                            {
                                /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_145: [ If any call for creating or setting the link attach properties fails `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. ]*/
                                LogError("Cannot set the property for API version on the link attach properties");
                                result = MU_FAILURE;
                            }
                            /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_144: [ The link attach properties shall be set on the receiver and sender link by calling `link_set_attach_properties`. ]*/
                            else if (link_set_attach_properties(iothubtransport_amqp_methods_handle->sender_link, link_attach_properties) != 0)
                            {
                                /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_145: [ If any call for creating or setting the link attach properties fails `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. ]*/
                                LogError("Cannot set the link attach properties on the sender link");
                                result = MU_FAILURE;
                            }
                            else if (link_set_attach_properties(iothubtransport_amqp_methods_handle->receiver_link, link_attach_properties) != 0)
                            {
                                /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_145: [ If any call for creating or setting the link attach properties fails `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. ]*/
                                LogError("Cannot set the link attach properties on the receiver link");
                                result = MU_FAILURE;
                            }
                            else
                            {
                                result = 0;
                            }

                            /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_146: [ The link attach properties and all associated values shall be freed by calling `amqpvalue_destroy` after setting the link attach properties. ]*/
                            amqpvalue_destroy(api_version_property_value);
                        }

                        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_146: [ The link attach properties and all associated values shall be freed by calling `amqpvalue_destroy` after setting the link attach properties. ]*/
                        amqpvalue_destroy(api_version_property_key);
                    }
                }

                /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_146: [ The link attach properties and all associated values shall be freed by calling `amqpvalue_destroy` after setting the link attach properties. ]*/
                amqpvalue_destroy(channel_correlation_id_property_value);
            }
            STRING_delete(correlation_id);

            /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_146: [ The link attach properties and all associated values shall be freed by calling `amqpvalue_destroy` after setting the link attach properties. ]*/
            amqpvalue_destroy(channel_correlation_id_property_key);
        }

        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_146: [ The link attach properties and all associated values shall be freed by calling `amqpvalue_destroy` after setting the link attach properties. ]*/
        amqpvalue_destroy(link_attach_properties);
    }

    return result;
}

int iothubtransportamqp_methods_subscribe(IOTHUBTRANSPORT_AMQP_METHODS_HANDLE iothubtransport_amqp_methods_handle,
    SESSION_HANDLE session_handle, ON_METHODS_ERROR on_methods_error, void* on_methods_error_context,
    ON_METHOD_REQUEST_RECEIVED on_method_request_received, void* on_method_request_received_context,
    ON_METHODS_UNSUBSCRIBED on_methods_unsubscribed, void* on_methods_unsubscribed_context)
{
    int result;

    /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_009: [ If any of the argument `iothubtransport_amqp_methods_handle`, `session_handle`, `on_methods_error`, `on_method_request_received`, `on_methods_unsubscribed` is NULL, `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. ]*/
    if ((iothubtransport_amqp_methods_handle == NULL) ||
        (session_handle == NULL) ||
        (on_methods_error == NULL) ||
        (on_method_request_received == NULL) ||
        (on_methods_unsubscribed == NULL))
    {
        LogError("Invalid arguments: iothubtransport_amqp_methods_handle=%p, session_handle=%p, on_methods_error=%p, on_method_request_received=%p, on_methods_unsubscribed=%p",
            iothubtransport_amqp_methods_handle, session_handle, on_methods_error, on_method_request_received, on_methods_unsubscribed);
        result = MU_FAILURE;
    }
    /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_137: [ `iothubtransportamqp_methods_subscribe` after another succesfull `iothubtransportamqp_methods_subscribe` without any unsubscribe shall return a non-zero value without performing any subscribe action. ]*/
    else if (iothubtransport_amqp_methods_handle->subscribe_state != SUBSCRIBE_STATE_NOT_SUBSCRIBED)
    {
        LogError("Already subscribed");
        result = MU_FAILURE;
    }
    else
    {
        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_015: [ The address string used to create the source shall be of the form `/devices/{device id}` + (`/modules/{module  id}` if modules are present) + `/methods/devicebound`. ]*/
        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_016: [ The string shall be created by using `STRING_construct_sprintf`. ]*/
        STRING_HANDLE peer_endpoint_string = create_peer_endpoint_name(iothubtransport_amqp_methods_handle);
        if (peer_endpoint_string == NULL)
        {
            /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_018: [ If `STRING_construct_sprintf` fails `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. ]*/
            result = MU_FAILURE;
        }
        else
        {
            iothubtransport_amqp_methods_handle->on_method_request_received = on_method_request_received;
            iothubtransport_amqp_methods_handle->on_method_request_received_context = on_method_request_received_context;
            iothubtransport_amqp_methods_handle->on_methods_error = on_methods_error;
            iothubtransport_amqp_methods_handle->on_methods_error_context = on_methods_error_context;
            iothubtransport_amqp_methods_handle->on_methods_unsubscribed = on_methods_unsubscribed;
            iothubtransport_amqp_methods_handle->on_methods_unsubscribed_context = on_methods_unsubscribed_context;

            /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_014: [ - `source` shall be the a source value created by calling `messaging_create_source`. ]*/
            AMQP_VALUE receiver_source = messaging_create_source(STRING_c_str(peer_endpoint_string));
            if (receiver_source == NULL)
            {
                /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_019: [ If creating the target or source values fails `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. ]*/
                LogError("Cannot create receiver source");
                result = MU_FAILURE;
            }
            else
            {
                /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_017: [ - `target` shall be the a target value created by calling `messaging_create_target`. ]*/
                /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_117: [ The address string used to create the target shall be `requests`. ]*/
                AMQP_VALUE receiver_target = messaging_create_target("requests");
                if (receiver_target == NULL)
                {
                    /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_019: [ If creating the target or source values fails `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. ]*/
                    LogError("Cannot create receiver target");
                    result = MU_FAILURE;
                }
                else
                {
                    /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_012: [ - `name` shall be in the format `methods_requests_link-{device_id}` (+ `/{module-id}` if module id is present). ]*/
                    STRING_HANDLE requests_link_name = create_requests_link_name(iothubtransport_amqp_methods_handle);
                    if (requests_link_name == NULL)
                    {
                        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_153: [ If constructing the requests link name fails, `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. ]*/
                        LogError("Cannot create methods requests link name.");
                        result = MU_FAILURE;
                    }
                    else
                    {
                        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_010: [ `iothubtransportamqp_methods_subscribe` shall create a receiver link by calling `link_create` with the following arguments: ]*/
                        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_011: [ - `session_handle` shall be the session_handle argument passed to iothubtransportamqp_methods_subscribe ]*/
                        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_013: [ - `role` shall be role_receiver. ]*/
                        iothubtransport_amqp_methods_handle->receiver_link = link_create(session_handle, STRING_c_str(requests_link_name), role_receiver, receiver_source, receiver_target);
                        if (iothubtransport_amqp_methods_handle->receiver_link == NULL)
                        {
                            /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_020: [ If creating the receiver link fails `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. ]*/
                            LogError("Cannot create receiver link");
                            result = MU_FAILURE;
                        }
                        else
                        {
                            /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_025: [ - `source` shall be the a source value created by calling `messaging_create_source`. ]*/
                            /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_026: [ The address string used to create the target shall be `responses`. ]*/
                            AMQP_VALUE sender_source = messaging_create_source("responses");
                            if (sender_source == NULL)
                            {
                                /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_031: [ If creating the target or source values fails `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. ]*/
                                LogError("Cannot create sender source");
                                result = MU_FAILURE;
                            }
                            else
                            {
                                /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_027: [ - `target` shall be the a target value created by calling `messaging_create_target`. ]*/
                                AMQP_VALUE sender_target = messaging_create_target(STRING_c_str(peer_endpoint_string));
                                if (sender_target == NULL)
                                {
                                    /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_031: [ If creating the target or source values fails `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. ]*/
                                    LogError("Cannot create sender target");
                                    result = MU_FAILURE;
                                }
                                else
                                {
                                    /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_023: [ - `name` shall be format `methods_responses_link-{device_id}` (+ `/{module-id}` if module id is present). ]*/
                                    STRING_HANDLE responses_link_name = create_responses_link_name(iothubtransport_amqp_methods_handle);
                                    if (responses_link_name == NULL)
                                    {
                                        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_154: [ If constructing the responses link name fails, `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. ]*/
                                        LogError("Cannot create methods responses link name.");
                                        result = MU_FAILURE;
                                    }
                                    else
                                    {
                                        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_021: [ `iothubtransportamqp_methods_subscribe` shall create a sender link by calling `link_create` with the following arguments: ]*/
                                        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_022: [ - `session_handle` shall be the session_handle argument passed to iothubtransportamqp_methods_subscribe ]*/
                                        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_024: [ - `role` shall be role_sender. ]*/
                                        iothubtransport_amqp_methods_handle->sender_link = link_create(session_handle, STRING_c_str(responses_link_name), role_sender, sender_source, sender_target);
                                        if (iothubtransport_amqp_methods_handle->sender_link == NULL)
                                        {
                                            /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_032: [ If creating the receiver link fails `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. ]*/
                                            LogError("Cannot create sender link");
                                            result = MU_FAILURE;
                                        }
                                        else
                                        {
                                            if (set_link_attach_properties(iothubtransport_amqp_methods_handle) != 0)
                                            {
                                                result = MU_FAILURE;
                                            }
                                            else
                                            {
                                                /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_033: [ `iothubtransportamqp_methods_subscribe` shall create a message receiver associated with the receiver link by calling `messagereceiver_create` and passing the receiver link handle to it. ]*/
                                                /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_118: [ An `on_message_receiver_state_changed` callback together with its context shall be passed to `messagereceiver_create`. ]*/
                                                iothubtransport_amqp_methods_handle->message_receiver = messagereceiver_create(iothubtransport_amqp_methods_handle->receiver_link, on_message_receiver_state_changed, iothubtransport_amqp_methods_handle);
                                                if (iothubtransport_amqp_methods_handle->message_receiver == NULL)
                                                {
                                                    /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_034: [ If `messagereceiver_create` fails, `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. ]*/
                                                    LogError("Cannot create message receiver");
                                                    result = MU_FAILURE;
                                                }
                                                else
                                                {
                                                    /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_035: [ `iothubtransportamqp_methods_subscribe` shall create a message sender associated with the sender link by calling `messagesender_create` and passing the sender link handle to it. ]*/
                                                    /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_106: [ An `on_message_sender_state_changed` callback together with its context shall be passed to `messagesender_create`. ]*/
                                                    iothubtransport_amqp_methods_handle->message_sender = messagesender_create(iothubtransport_amqp_methods_handle->sender_link, on_message_sender_state_changed, iothubtransport_amqp_methods_handle);
                                                    if (iothubtransport_amqp_methods_handle->message_sender == NULL)
                                                    {
                                                        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_036: [ If `messagesender_create` fails, `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. ]*/
                                                        LogError("Cannot create message sender");
                                                        result = MU_FAILURE;
                                                    }
                                                    else
                                                    {
                                                        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_037: [ `iothubtransportamqp_methods_subscribe` shall open the message sender by calling `messagesender_open`. ]*/
                                                        if (messagesender_open(iothubtransport_amqp_methods_handle->message_sender) != 0)
                                                        {
                                                            /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_038: [ If `messagesender_open` fails, `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. ]*/
                                                            LogError("Cannot open the message sender");
                                                            result = MU_FAILURE;
                                                        }
                                                        else
                                                        {
                                                            /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_039: [ `iothubtransportamqp_methods_subscribe` shall open the message sender by calling `messagereceiver_open`. ]*/
                                                            /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_104: [ An `on_message_received` callback together with its context shall be passed to `messagereceiver_open`. ]*/
                                                            if (messagereceiver_open(iothubtransport_amqp_methods_handle->message_receiver, on_message_received, iothubtransport_amqp_methods_handle) != 0)
                                                            {
                                                                /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_040: [ If `messagereceiver_open` fails, `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. ]*/
                                                                LogError("Cannot open the message receiver");
                                                                result = MU_FAILURE;
                                                            }
                                                            else
                                                            {
                                                                iothubtransport_amqp_methods_handle->subscribe_state = SUBSCRIBE_STATE_SUBSCRIBED;

                                                                /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_008: [ On success it shall return 0. ]*/
                                                                result = 0;
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }

                                        STRING_delete(responses_link_name);
                                    }

                                    amqpvalue_destroy(sender_target);
                                }

                                amqpvalue_destroy(sender_source);
                            }
                        }

                        STRING_delete(requests_link_name);
                    }

                    amqpvalue_destroy(receiver_target);
                }

                amqpvalue_destroy(receiver_source);
            }

            /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_105: [ The string created in order to hold the source and target addresses shall be freed by calling `STRING_delete`. ]*/
            STRING_delete(peer_endpoint_string);
        }
    }

    return result;
}

void iothubtransportamqp_methods_unsubscribe(IOTHUBTRANSPORT_AMQP_METHODS_HANDLE iothubtransport_amqp_methods_handle)
{
    /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_072: [ If the argument `iothubtransport_amqp_methods_handle` is NULL, `iothubtransportamqp_methods_unsubscribe` shall do nothing. ]*/
    if (iothubtransport_amqp_methods_handle == NULL)
    {
        LogError("NULL handle");
    }
    else
    {
        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_135: [ If subscribe was not called yet, `iothubtransportamqp_methods_unsubscribe` shall do nothing. ]*/
        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_136: [ `iothubtransportamqp_methods_unsubscribe` after `iothubtransportamqp_methods_unsubscribe` shall do nothing. ]*/
        if (iothubtransport_amqp_methods_handle->subscribe_state != SUBSCRIBE_STATE_SUBSCRIBED)
        {
            LogError("unsubscribe called while not subscribed");
        }
        else
        {
            /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_073: [ Otherwise `iothubtransportamqp_methods_unsubscribe` shall free all resources allocated in `iothubtransportamqp_methods_subscribe`: ]*/
            /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_075: [ - It shall free the message receiver by calling `messagereceiver_destroy'. ]*/
            messagereceiver_destroy(iothubtransport_amqp_methods_handle->message_receiver);
            iothubtransport_amqp_methods_handle->message_receiver = NULL;
            /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_074: [ - It shall free the message sender by calling `messagesender_destroy'. ]*/
            messagesender_destroy(iothubtransport_amqp_methods_handle->message_sender);
            iothubtransport_amqp_methods_handle->message_sender = NULL;
            /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_076: [ - It shall free the sender link by calling `link_destroy'. ]*/
            link_destroy(iothubtransport_amqp_methods_handle->sender_link);
            iothubtransport_amqp_methods_handle->sender_link = NULL;
            /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_077: [ - It shall free the receiver link by calling `link_destroy'. ]*/
            link_destroy(iothubtransport_amqp_methods_handle->receiver_link);
            iothubtransport_amqp_methods_handle->receiver_link = NULL;

            iothubtransport_amqp_methods_handle->subscribe_state = SUBSCRIBE_STATE_NOT_SUBSCRIBED;
        }
    }
}

int iothubtransportamqp_methods_respond(IOTHUBTRANSPORT_AMQP_METHOD_HANDLE method_handle, const unsigned char* response, size_t response_size, int status_code)
{
    int result;

    /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_107: [ If the argument `method_handle` is NULL, `iothubtransportamqp_methods_respond` shall fail and return a non-zero value. ]*/
    if (method_handle == NULL)
    {
        LogError("NULL method handle");
        result = MU_FAILURE;
    }
    /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_108: [ If `response_size` is greater than zero and `response` is NULL, `iothubtransportamqp_methods_respond` shall fail and return a non-zero value. ]*/
    else if ((response == NULL) && (response_size > 0))
    {
        LogError("NULL response buffer with > 0 response payload size");
        result = MU_FAILURE;
    }
    else
    {
        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_061: [ - A new uAMQP message shall be created by calling `message_create`. ]*/
        MESSAGE_HANDLE message = message_create();
        if (message == NULL)
        {
            /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_062: [ If the `message_create` call fails, `iothubtransportamqp_methods_respond` shall fail and return a non-zero value. ]*/
            LogError("Cannot create message");
            result = MU_FAILURE;
        }
        else
        {
            /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_063: [ - A new properties handle shall be created by calling `properties_create`. ]*/
            PROPERTIES_HANDLE properties = properties_create();
            if (properties == NULL)
            {
                /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_064: [ If the `properties_create call` fails, `iothubtransportamqp_methods_respond` shall fail and return a non-zero value. ]*/
                LogError("Cannot create properties");
                result = MU_FAILURE;
            }
            else
            {
                /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_124: [ - An AMQP value holding the correlation id associated with the `method_handle` handle shall be created by calling `amqpvalue_create_uuid`. ]*/
                AMQP_VALUE correlation_id = amqpvalue_create_uuid(method_handle->correlation_id);
                if (correlation_id == NULL)
                {
                    /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_125: [ If `amqpvalue_create_uuid` fails, `iothubtransportamqp_methods_respond` shall fail and return a non-zero value. ]*/
                    LogError("Cannot create correlation_id value");
                    result = MU_FAILURE;
                }
                else
                {
                    /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_065: [ - The correlation id on the message properties shall be set by calling `properties_set_correlation_id` and passing as argument the already create correlation ID AMQP value. ]*/
                    if (properties_set_correlation_id(properties, correlation_id) != 0)
                    {
                        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_066: [ If the `properties_set_correlation_id` call fails, `iothubtransportamqp_methods_respond` shall fail and return a non-zero value. ]*/
                        LogError("Cannot set correlation_id on the properties");
                        result = MU_FAILURE;
                    }
                    /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_148: [ The properties shall be set on the message by calling `message_set_properties`. ]*/
                    else if (message_set_properties(message, properties) != 0)
                    {
                        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_149: [ If `message_set_properties` fails, `iothubtransportamqp_methods_respond` shall fail and return a non-zero value. ]*/
                        LogError("Cannot set properties on the response message");
                        result = MU_FAILURE;
                    }
                    else
                    {
                        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_090: [ An AMQP map shall be created to hold the application properties for the response by calling `amqpvalue_create_map`. ]*/
                        AMQP_VALUE application_properties_map = amqpvalue_create_map();
                        if (application_properties_map == NULL)
                        {
                            /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_096: [ If any of the calls `amqpvalue_create_string`, `amqpvalue_create_int`, `amqpvalue_create_map`, `amqpvalue_set_map_value` or `message_set_application_properties` fails `iothubtransportamqp_methods_respond` shall fail and return a non-zero value. ]*/
                            LogError("Cannot create map for application properties");
                            result = MU_FAILURE;
                        }
                        else
                        {
                            /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_091: [ A property key `IoThub-status` shall be created by calling `amqpvalue_create_string`. ]*/
                            AMQP_VALUE property_key_status = amqpvalue_create_string("IoThub-status");
                            if (property_key_status == NULL)
                            {
                                /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_096: [ If any of the calls `amqpvalue_create_string`, `amqpvalue_create_int`, `amqpvalue_create_map`, `amqpvalue_set_map_value` or `message_set_application_properties` fails `iothubtransportamqp_methods_respond` shall fail and return a non-zero value. ]*/
                                LogError("Cannot create the property key for the status property");
                                result = MU_FAILURE;
                            }
                            else
                            {
                                /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_097: [ A property value of type int shall be created from the `status_code` argument by calling `amqpvalue_create_int`. ] ]*/
                                AMQP_VALUE property_value_status = amqpvalue_create_int(status_code);
                                if (property_value_status == NULL)
                                {
                                    /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_096: [ If any of the calls `amqpvalue_create_string`, `amqpvalue_create_int`, `amqpvalue_create_map`, `amqpvalue_set_map_value` or `message_set_application_properties` fails `iothubtransportamqp_methods_respond` shall fail and return a non-zero value. ]*/
                                    LogError("Cannot create the status code property value for the application properties map");
                                    result = MU_FAILURE;
                                }
                                else
                                {
                                    /* Cdoes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_093: [ A new entry shall be added in the application properties map by calling `amqpvalue_set_map_value` and passing the key and value that were previously created. ]*/
                                    if (amqpvalue_set_map_value(application_properties_map, property_key_status, property_value_status) != 0)
                                    {
                                        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_096: [ If any of the calls `amqpvalue_create_string`, `amqpvalue_create_int`, `amqpvalue_create_map`, `amqpvalue_set_map_value` or `message_set_application_properties` fails `iothubtransportamqp_methods_respond` shall fail and return a non-zero value. ]*/
                                        LogError("Cannot add the status property to the application properties");
                                        result = MU_FAILURE;
                                    }
                                    else
                                    {
                                        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_094: [ The application properties map shall be set on the response message by calling `message_set_application_properties`. ]*/
                                        if (message_set_application_properties(message, application_properties_map) != 0)
                                        {
                                            /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_096: [ If any of the calls `amqpvalue_create_string`, `amqpvalue_create_int`, `amqpvalue_create_map`, `amqpvalue_set_map_value` or `message_set_application_properties` fails `iothubtransportamqp_methods_respond` shall fail and return a non-zero value. ]*/
                                            LogError("Cannot set the application properties on the message");
                                            result = MU_FAILURE;
                                        }
                                        else
                                        {
                                            BINARY_DATA binary_data;

                                            /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_079: [ The field `bytes` of the `binary_data` argument shall be set to the `response` argument value. ]*/
                                            /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_080: [ The field `length` of the `binary_data` argument shall be set to the `response_size` argument value. ]*/
                                            binary_data.bytes = response;
                                            binary_data.length = response_size;

                                            /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_078: [ The binary payload for the response shall be set by calling `message_add_body_amqp_data` for the newly created message handle. ]*/
                                            if (message_add_body_amqp_data(message, binary_data) != 0)
                                            {
                                                /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_081: [ If the `message_add_body_amqp_data` call fails, `iothubtransportamqp_methods_respond` shall fail and return a non-zero value. ]*/
                                                LogError("Cannot set the response payload on the reponse message");
                                                result = MU_FAILURE;
                                            }
                                            else
                                            {
                                                /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_067: [ The message shall be handed over to the message_sender by calling `messagesender_send` and passing as arguments: ]*/
                                                /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_068: [ - The response message handle. ]*/
                                                /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_069: [ - A send callback and its context for the `on_message_send_complete` callback. ]*/
                                                if (messagesender_send_async(method_handle->iothubtransport_amqp_methods_handle->message_sender, message, on_message_send_complete, method_handle->iothubtransport_amqp_methods_handle, 0) == NULL)
                                                {
                                                    /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_071: [ If the `messagesender_send` call fails, `iothubtransportamqp_methods_respond` shall fail and return a non-zero value. ]*/
                                                    LogError("Cannot send response message");
                                                    result = MU_FAILURE;
                                                }
                                                else
                                                {
                                                    /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_114: [ The handle `method_handle` shall be removed from the array used to track the method handles. ]*/
                                                    remove_tracked_handle(method_handle->iothubtransport_amqp_methods_handle, method_handle);

                                                    /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_111: [ The handle `method_handle` shall be freed (have no meaning) after `iothubtransportamqp_methods_respond` has been executed. ]*/
                                                    free(method_handle);

                                                    /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_060: [ `iothubtransportamqp_methods_respond` shall construct a response message and on success it shall return 0. ]*/
                                                    result = 0;
                                                }
                                            }
                                        }
                                    }

                                    /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_095: [ The application property map and all intermediate values shall be freed after being passed to `message_set_application_properties`. ]*/
                                    amqpvalue_destroy(property_value_status);
                                }

                                /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_095: [ The application property map and all intermediate values shall be freed after being passed to `message_set_application_properties`. ]*/
                                amqpvalue_destroy(property_key_status);
                            }

                            /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_095: [ The application property map and all intermediate values shall be freed after being passed to `message_set_application_properties`. ]*/
                            amqpvalue_destroy(application_properties_map);
                        }
                    }

                    /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_095: [ The application property map and all intermediate values shall be freed after being passed to `message_set_application_properties`. ]*/
                    amqpvalue_destroy(correlation_id);
                }

                /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_095: [ The application property map and all intermediate values shall be freed after being passed to `message_set_application_properties`. ]*/
                properties_destroy(properties);
            }

            /* Codes_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_095: [ The application property map and all intermediate values shall be freed after being passed to `message_set_application_properties`. ]*/
            message_destroy(message);
        }
    }

    return result;
}