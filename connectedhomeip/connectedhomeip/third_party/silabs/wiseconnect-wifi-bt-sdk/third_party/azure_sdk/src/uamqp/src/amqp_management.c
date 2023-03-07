// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "azure_macro_utils/macro_utils.h"
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/singlylinkedlist.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_uamqp_c/amqp_management.h"
#include "azure_uamqp_c/link.h"
#include "azure_uamqp_c/message_sender.h"
#include "azure_uamqp_c/message_receiver.h"
#include "azure_uamqp_c/messaging.h"
#include "azure_uamqp_c/amqp_definitions.h"

static const char sender_suffix[] = "-sender";
static const char receiver_suffix[] = "-receiver";

#define COUNT_CHARS(str) (sizeof(str) / sizeof((str)[0]) - 1)

typedef struct OPERATION_MESSAGE_INSTANCE_TAG
{
    ON_AMQP_MANAGEMENT_EXECUTE_OPERATION_COMPLETE on_execute_operation_complete;
    void* callback_context;
    uint64_t message_id;
    AMQP_MANAGEMENT_HANDLE amqp_management;
} OPERATION_MESSAGE_INSTANCE;

typedef enum AMQP_MANAGEMENT_STATE_TAG
{
    AMQP_MANAGEMENT_STATE_IDLE,
    AMQP_MANAGEMENT_STATE_OPENING,
    AMQP_MANAGEMENT_STATE_CLOSING,
    AMQP_MANAGEMENT_STATE_OPEN,
    AMQP_MANAGEMENT_STATE_ERROR
} AMQP_MANAGEMENT_STATE;

typedef struct AMQP_MANAGEMENT_INSTANCE_TAG
{
    LINK_HANDLE sender_link;
    LINK_HANDLE receiver_link;
    MESSAGE_SENDER_HANDLE message_sender;
    MESSAGE_RECEIVER_HANDLE message_receiver;
    SINGLYLINKEDLIST_HANDLE pending_operations;
    uint64_t next_message_id;
    ON_AMQP_MANAGEMENT_OPEN_COMPLETE on_amqp_management_open_complete;
    void* on_amqp_management_open_complete_context;
    ON_AMQP_MANAGEMENT_ERROR on_amqp_management_error;
    void* on_amqp_management_error_context;
    AMQP_MANAGEMENT_STATE amqp_management_state;
    char* status_code_key_name;
    char* status_description_key_name;
    unsigned int sender_connected : 1;
    unsigned int receiver_connected : 1;
} AMQP_MANAGEMENT_INSTANCE;

static AMQP_VALUE on_message_received(const void* context, MESSAGE_HANDLE message)
{
    AMQP_VALUE result;

    if (context == NULL)
    {
        /* Codes_SRS_AMQP_MANAGEMENT_01_108: [ When `on_message_received` is called with a NULL context, it shall do nothing. ]*/
        LogError("NULL context in on_message_received");
        result = NULL;
    }
    else
    {
        AMQP_MANAGEMENT_HANDLE amqp_management = (AMQP_MANAGEMENT_HANDLE)context;
        AMQP_VALUE application_properties;

        /* Codes_SRS_AMQP_MANAGEMENT_01_109: [ `on_message_received` shall obtain the application properties from the message by calling `message_get_application_properties`. ]*/
        if (message_get_application_properties(message, &application_properties) != 0)
        {
            /* Codes_SRS_AMQP_MANAGEMENT_01_113: [ If obtaining the application properties or message properties fails, an error shall be indicated by calling `on_amqp_management_error` and passing the `on_amqp_management_error_context` to it. ]*/
            LogError("Could not retrieve application properties");
            amqp_management->on_amqp_management_error(amqp_management->on_amqp_management_error_context);
            /* Codes_SRS_AMQP_MANAGEMENT_01_136: [ When `on_message_received` fails due to errors in parsing the response message `on_message_received` shall call `messaging_delivery_rejected` and return the created delivery AMQP value. ]*/
            result = messaging_delivery_rejected("amqp:internal-error", "Could not get application properties on AMQP management response.");
        }
        else
        {
            PROPERTIES_HANDLE response_properties;

            /* Codes_SRS_AMQP_MANAGEMENT_01_110: [ `on_message_received` shall obtain the message properties from the message by calling `message_get_properties`. ]*/
            if (message_get_properties(message, &response_properties) != 0)
            {
                /* Codes_SRS_AMQP_MANAGEMENT_01_113: [ If obtaining the application properties or message properties fails, an error shall be indicated by calling `on_amqp_management_error` and passing the `on_amqp_management_error_context` to it. ]*/
                LogError("Could not retrieve message properties");
                amqp_management->on_amqp_management_error(amqp_management->on_amqp_management_error_context);
                /* Codes_SRS_AMQP_MANAGEMENT_01_136: [ When `on_message_received` fails due to errors in parsing the response message `on_message_received` shall call `messaging_delivery_rejected` and return the created delivery AMQP value. ]*/
                result = messaging_delivery_rejected("amqp:internal-error", "Could not get message properties on AMQP management response.");
            }
            else
            {
                AMQP_VALUE key;
                AMQP_VALUE value;
                AMQP_VALUE desc_key;
                AMQP_VALUE desc_value;
                AMQP_VALUE map;
                AMQP_VALUE correlation_id_value;
                uint64_t correlation_id;

                /* Codes_SRS_AMQP_MANAGEMENT_01_111: [ `on_message_received` shall obtain the correlation Id from the message properties by using `properties_get_correlation_id`. ]*/
                if (properties_get_correlation_id(response_properties, &correlation_id_value) != 0)
                {
                    /* Codes_SRS_AMQP_MANAGEMENT_01_114: [ If obtaining the correlation Id fails, an error shall be indicated by calling `on_amqp_management_error` and passing the `on_amqp_management_error_context` to it. ] */
                    LogError("Could not retrieve correlation Id");
                    amqp_management->on_amqp_management_error(amqp_management->on_amqp_management_error_context);
                    /* Codes_SRS_AMQP_MANAGEMENT_01_136: [ When `on_message_received` fails due to errors in parsing the response message `on_message_received` shall call `messaging_delivery_rejected` and return the created delivery AMQP value. ]*/
                    result = messaging_delivery_rejected("amqp:internal-error", "Could not get correlation Id from AMQP management response.");
                }
                else
                {
                    if (amqpvalue_get_ulong(correlation_id_value, &correlation_id) != 0)
                    {
                        /* Codes_SRS_AMQP_MANAGEMENT_01_132: [ If any functions manipulating AMQP values, application properties, etc., fail, an error shall be indicated to the consumer by calling the `on_amqp_management_error` and passing the `on_amqp_management_error_context` to it. ]*/
                        LogError("Could not retrieve correlation Id ulong value");
                        amqp_management->on_amqp_management_error(amqp_management->on_amqp_management_error_context);
                        /* Codes_SRS_AMQP_MANAGEMENT_01_136: [ When `on_message_received` fails due to errors in parsing the response message `on_message_received` shall call `messaging_delivery_rejected` and return the created delivery AMQP value. ]*/
                        result = messaging_delivery_rejected("amqp:internal-error", "Could not get correlation Id from AMQP management response.");
                    }
                    else
                    {
                        /* Codes_SRS_AMQP_MANAGEMENT_01_119: [ `on_message_received` shall obtain the application properties map by calling `amqpvalue_get_inplace_described_value`. ]*/
                        /* Codes_SRS_AMQP_MANAGEMENT_01_070: [ Response messages have the following application-properties: ]*/
                        map = amqpvalue_get_inplace_described_value(application_properties);
                        if (map == NULL)
                        {
                            /* Codes_SRS_AMQP_MANAGEMENT_01_132: [ If any functions manipulating AMQP values, application properties, etc., fail, an error shall be indicated to the consumer by calling the `on_amqp_management_error` and passing the `on_amqp_management_error_context` to it. ]*/
                            LogError("Could not retrieve application property map");
                            amqp_management->on_amqp_management_error(amqp_management->on_amqp_management_error_context);
                            /* Codes_SRS_AMQP_MANAGEMENT_01_136: [ When `on_message_received` fails due to errors in parsing the response message `on_message_received` shall call `messaging_delivery_rejected` and return the created delivery AMQP value. ]*/
                            result = messaging_delivery_rejected("amqp:internal-error", "Could not get application property map from the application properties in the AMQP management response.");
                        }
                        else
                        {
                            /* Codes_SRS_AMQP_MANAGEMENT_01_120: [ An AMQP value used to lookup the status code shall be created by calling `amqpvalue_create_string` with the status code key name (`statusCode`) as argument. ]*/
                            /* Codes_SRS_AMQP_MANAGEMENT_01_071: [ statusCode integer Yes HTTP response code [RFC2616] ]*/
                            key = amqpvalue_create_string(amqp_management->status_code_key_name);
                            if (key == NULL)
                            {
                                /* Codes_SRS_AMQP_MANAGEMENT_01_132: [ If any functions manipulating AMQP values, application properties, etc., fail, an error shall be indicated to the consumer by calling the `on_amqp_management_error` and passing the `on_amqp_management_error_context` to it. ]*/
                                LogError("Could not create status-code amqp value");
                                amqp_management->on_amqp_management_error(amqp_management->on_amqp_management_error_context);
                                /* Codes_SRS_AMQP_MANAGEMENT_01_135: [ When an error occurs in creating AMQP values (for status code, etc.) `on_message_received` shall call `messaging_delivery_released` and return the created delivery AMQP value. ]*/
                                result = messaging_delivery_released();
                            }
                            else
                            {
                                /* Codes_SRS_AMQP_MANAGEMENT_01_121: [ The status code shall be looked up in the application properties by using `amqpvalue_get_map_value`. ]*/
                                value = amqpvalue_get_map_value(map, key);
                                if (value == NULL)
                                {
                                    /* Codes_SRS_AMQP_MANAGEMENT_01_122: [ If status code is not found an error shall be indicated to the consumer by calling the `on_amqp_management_error` and passing the `on_amqp_management_error_context` to it. ]*/
                                    LogError("Could not retrieve status code from application properties");
                                    amqp_management->on_amqp_management_error(amqp_management->on_amqp_management_error_context);
                                    /* Codes_SRS_AMQP_MANAGEMENT_01_136: [ When `on_message_received` fails due to errors in parsing the response message `on_message_received` shall call `messaging_delivery_rejected` and return the created delivery AMQP value. ]*/
                                    result = messaging_delivery_rejected("amqp:internal-error", "Could not retrieve status code from the application properties in the AMQP management response.");
                                }
                                else
                                {
                                    int32_t status_code;
                                    /* Codes_SRS_AMQP_MANAGEMENT_01_133: [ The status code value shall be extracted from the value found in the map by using `amqpvalue_get_int`. ]*/
                                    if (amqpvalue_get_int(value, &status_code) != 0)
                                    {
                                        /* Codes_SRS_AMQP_MANAGEMENT_01_132: [ If any functions manipulating AMQP values, application properties, etc., fail, an error shall be indicated to the consumer by calling the `on_amqp_management_error` and passing the `on_amqp_management_error_context` to it. ]*/
                                        LogError("Could not retrieve status code int value");
                                        amqp_management->on_amqp_management_error(amqp_management->on_amqp_management_error_context);
                                        /* Codes_SRS_AMQP_MANAGEMENT_01_136: [ When `on_message_received` fails due to errors in parsing the response message `on_message_received` shall call `messaging_delivery_rejected` and return the created delivery AMQP value. ]*/
                                        result = messaging_delivery_rejected("amqp:internal-error", "Could not retrieve status code value from the application properties in the AMQP management response.");
                                    }
                                    else
                                    {
                                        /* Codes_SRS_AMQP_MANAGEMENT_01_123: [ An AMQP value used to lookup the status description shall be created by calling `amqpvalue_create_string` with the status description key name (`statusDescription`) as argument. ]*/
                                        /* Codes_SRS_AMQP_MANAGEMENT_01_072: [ statusDescription string No Description of the status. ]*/
                                        desc_key = amqpvalue_create_string(amqp_management->status_description_key_name);
                                        if (desc_key == NULL)
                                        {
                                            /* Codes_SRS_AMQP_MANAGEMENT_01_132: [ If any functions manipulating AMQP values, application properties, etc., fail, an error shall be indicated to the consumer by calling the `on_amqp_management_error` and passing the `on_amqp_management_error_context` to it. ]*/
                                            LogError("Could not create status-description amqp value");
                                            amqp_management->on_amqp_management_error(amqp_management->on_amqp_management_error_context);
                                            /* Codes_SRS_AMQP_MANAGEMENT_01_135: [ When an error occurs in creating AMQP values (for status code, etc.) `on_message_received` shall call `messaging_delivery_released` and return the created delivery AMQP value. ]*/
                                            result = messaging_delivery_released();
                                        }
                                        else
                                        {
                                            const char* status_description = NULL;
                                            LIST_ITEM_HANDLE list_item_handle;
                                            bool found = false;
                                            bool is_error = false;

                                            /* Codes_SRS_AMQP_MANAGEMENT_01_124: [ The status description shall be looked up in the application properties by using `amqpvalue_get_map_value`. ]*/
                                            desc_value = amqpvalue_get_map_value(map, desc_key);
                                            if (desc_value != NULL)
                                            {
                                                /* Codes_SRS_AMQP_MANAGEMENT_01_134: [ The status description value shall be extracted from the value found in the map by using `amqpvalue_get_string`. ]*/
                                                if (amqpvalue_get_string(desc_value, &status_description) != 0)
                                                {
                                                    /* Codes_SRS_AMQP_MANAGEMENT_01_125: [ If status description is not found, NULL shall be passed to the user callback as `status_description` argument. ]*/
                                                    status_description = NULL;
                                                }
                                            }
                                            else
                                            {
                                                /* Codes_SRS_AMQP_MANAGEMENT_01_125: [ If status description is not found, NULL shall be passed to the user callback as `status_description` argument. ]*/
                                                status_description = NULL;
                                            }

                                            list_item_handle = singlylinkedlist_get_head_item(amqp_management->pending_operations);
                                            while (list_item_handle != NULL)
                                            {
                                                /* Codes_SRS_AMQP_MANAGEMENT_01_116: [ Each pending operation item value shall be obtained by calling `singlylinkedlist_item_get_value`. ]*/
                                                OPERATION_MESSAGE_INSTANCE* operation_message = (OPERATION_MESSAGE_INSTANCE*)singlylinkedlist_item_get_value(list_item_handle);
                                                if (operation_message == NULL)
                                                {
                                                    /* Codes_SRS_AMQP_MANAGEMENT_01_117: [ If iterating through the pending operations list fails, an error shall be indicated by calling `on_amqp_management_error` and passing the `on_amqp_management_error_context` to it. ]*/
                                                    LogError("Could not create status-description amqp value");
                                                    amqp_management->on_amqp_management_error(amqp_management->on_amqp_management_error_context);
                                                    /* Codes_SRS_AMQP_MANAGEMENT_01_135: [ When an error occurs in creating AMQP values (for status code, etc.) `on_message_received` shall call `messaging_delivery_released` and return the created delivery AMQP value. ]*/
                                                    result = messaging_delivery_released();
                                                    break;
                                                }
                                                else
                                                {
                                                    AMQP_MANAGEMENT_EXECUTE_OPERATION_RESULT execute_operation_result;

                                                    /* Codes_SRS_AMQP_MANAGEMENT_01_112: [ `on_message_received` shall check if the correlation Id matches the stored message Id of any pending operation. ]*/
                                                    /* Codes_SRS_AMQP_MANAGEMENT_01_068: [ The correlation-id of the response message MUST be the correlation-id from the request message (if present) ]*/
                                                    /* Codes_SRS_AMQP_MANAGEMENT_01_069: [ else the message-id from the request message. ]*/
                                                    if (correlation_id == operation_message->message_id)
                                                    {
                                                        /* Codes_SRS_AMQP_MANAGEMENT_01_074: [ Successful operations MUST result in a statusCode in the 2xx range as defined in Section 10.2 of [RFC2616]. ]*/
                                                        if ((status_code < 200) || (status_code > 299))
                                                        {
                                                            /* Codes_SRS_AMQP_MANAGEMENT_01_128: [ If the status indicates that the operation failed, the result callback argument shall be `AMQP_MANAGEMENT_EXECUTE_OPERATION_FAILED_BAD_STATUS`. ]*/
                                                            /* Codes_SRS_AMQP_MANAGEMENT_01_075: [ Unsuccessful operations MUST NOT result in a statusCode in the 2xx range as defined in Section 10.2 of [RFC2616]. ]*/
                                                            execute_operation_result = AMQP_MANAGEMENT_EXECUTE_OPERATION_FAILED_BAD_STATUS;
                                                        }
                                                        else
                                                        {
                                                            /* Codes_SRS_AMQP_MANAGEMENT_01_127: [ If the operation succeeded the result callback argument shall be `AMQP_MANAGEMENT_EXECUTE_OPERATION_OK`. ]*/
                                                            execute_operation_result = AMQP_MANAGEMENT_EXECUTE_OPERATION_OK;
                                                        }

                                                        /* Codes_SRS_AMQP_MANAGEMENT_01_126: [ If a corresponding correlation Id is found in the pending operations list, the callback associated with the pending operation shall be called. ]*/
                                                        /* Codes_SRS_AMQP_MANAGEMENT_01_166: [ The `message` shall be passed as argument to the callback. ]*/
                                                        operation_message->on_execute_operation_complete(operation_message->callback_context, execute_operation_result, status_code, status_description, message);

                                                        free(operation_message);

                                                        /* Codes_SRS_AMQP_MANAGEMENT_01_129: [ After calling the callback, the pending operation shall be removed from the pending operations list by calling `singlylinkedlist_remove`. ]*/
                                                        if (singlylinkedlist_remove(amqp_management->pending_operations, list_item_handle) != 0)
                                                        {
                                                            LogError("Cannot remove pending operation");
                                                            is_error = true;
                                                            break;
                                                        }
                                                        else
                                                        {
                                                            found = true;
                                                        }

                                                        break;
                                                    }
                                                }

                                                /* Codes_SRS_AMQP_MANAGEMENT_01_115: [ Iterating through the pending operations shall be done by using `singlylinkedlist_get_head_item` and `singlylinkedlist_get_next_item` until the enm of the pending operations singly linked list is reached. ]*/
                                                /* Codes_SRS_AMQP_MANAGEMENT_01_117: [ If iterating through the pending operations list fails, an error shall be indicated by calling `on_amqp_management_error` and passing the `on_amqp_management_error_context` to it. ]*/
                                                list_item_handle = singlylinkedlist_get_next_item(list_item_handle);
                                            }

                                            if (is_error)
                                            {
                                                /* Codes_SRS_AMQP_MANAGEMENT_01_117: [ If iterating through the pending operations list fails, an error shall be indicated by calling `on_amqp_management_error` and passing the `on_amqp_management_error_context` to it. ]*/
                                                amqp_management->on_amqp_management_error(amqp_management->on_amqp_management_error_context);
                                                /* Codes_SRS_AMQP_MANAGEMENT_01_135: [ When an error occurs in creating AMQP values (for status code, etc.) `on_message_received` shall call `messaging_delivery_released` and return the created delivery AMQP value. ]*/
                                                result = messaging_delivery_released();
                                            }
                                            else
                                            {
                                                if (!found)
                                                {
                                                    /* Codes_SRS_AMQP_MANAGEMENT_01_118: [ If no pending operation is found matching the correlation Id, an error shall be indicated by calling `on_amqp_management_error` and passing the `on_amqp_management_error_context` to it. ]*/
                                                    LogError("Could not match AMQP management response to request");
                                                    amqp_management->on_amqp_management_error(amqp_management->on_amqp_management_error_context);
                                                    /* Codes_SRS_AMQP_MANAGEMENT_01_135: [ When an error occurs in creating AMQP values (for status code, etc.) `on_message_received` shall call `messaging_delivery_released` and return the created delivery AMQP value. ]*/
                                                    result = messaging_delivery_rejected("amqp:internal-error", "Could not match AMQP management response to request");
                                                }
                                                else
                                                {
                                                    /* Codes_SRS_AMQP_MANAGEMENT_01_130: [ The `on_message_received` shall call `messaging_delivery_accepted` and return the created delivery AMQP value. ]*/
                                                    result = messaging_delivery_accepted();
                                                }
                                            }

                                            if (desc_value != NULL)
                                            {
                                                /* Codes_SRS_AMQP_MANAGEMENT_01_131: [ All temporary values like AMQP values used as keys shall be freed before exiting the callback. ]*/
                                                amqpvalue_destroy(desc_value);
                                            }

                                            /* Codes_SRS_AMQP_MANAGEMENT_01_131: [ All temporary values like AMQP values used as keys shall be freed before exiting the callback. ]*/
                                            amqpvalue_destroy(desc_key);
                                        }
                                    }

                                    /* Codes_SRS_AMQP_MANAGEMENT_01_131: [ All temporary values like AMQP values used as keys shall be freed before exiting the callback. ]*/
                                    amqpvalue_destroy(value);
                                }

                                /* Codes_SRS_AMQP_MANAGEMENT_01_131: [ All temporary values like AMQP values used as keys shall be freed before exiting the callback. ]*/
                                amqpvalue_destroy(key);
                            }
                        }
                    }
                }

                /* Codes_SRS_AMQP_MANAGEMENT_01_131: [ All temporary values like AMQP values used as keys shall be freed before exiting the callback. ]*/
                properties_destroy(response_properties);
            }

            /* Codes_SRS_AMQP_MANAGEMENT_01_131: [ All temporary values like AMQP values used as keys shall be freed before exiting the callback. ]*/
            application_properties_destroy(application_properties);
        }
    }

    return result;
}

static void on_message_send_complete(void* context, MESSAGE_SEND_RESULT send_result, AMQP_VALUE delivery_state)
{
    (void)delivery_state;

    if (context == NULL)
    {
        /* Codes_SRS_AMQP_MANAGEMENT_01_167: [ When `on_message_send_complete` is called with a NULL context it shall return. ]*/
        LogError("NULL context");
    }
    else
    {
        if (send_result == MESSAGE_SEND_OK)
        {
            /* Codes_SRS_AMQP_MANAGEMENT_01_170: [ If `send_result` is `MESSAGE_SEND_OK`, `on_message_send_complete` shall return. ]*/
        }
        else
        {
            /* Codes_SRS_AMQP_MANAGEMENT_01_172: [ If `send_result` is different then `MESSAGE_SEND_OK`: ]*/
            /* Codes_SRS_AMQP_MANAGEMENT_01_168: [ - `context` shall be used as a LIST_ITEM_HANDLE containing the pending operation. ]*/
            LIST_ITEM_HANDLE pending_operation_list_item_handle = (LIST_ITEM_HANDLE)context;

            /* Codes_SRS_AMQP_MANAGEMENT_01_169: [ - `on_message_send_complete` shall obtain the pending operation by calling `singlylinkedlist_item_get_value`. ]*/
            OPERATION_MESSAGE_INSTANCE* pending_operation_message = (OPERATION_MESSAGE_INSTANCE*)singlylinkedlist_item_get_value(pending_operation_list_item_handle);
            AMQP_MANAGEMENT_HANDLE amqp_management = pending_operation_message->amqp_management;

            /* Codes_SRS_AMQP_MANAGEMENT_01_171: [ - `on_message_send_complete` shall removed the pending operation from the pending operations list. ]*/
            if (singlylinkedlist_remove(amqp_management->pending_operations, pending_operation_list_item_handle) != 0)
            {
                /* Tests_SRS_AMQP_MANAGEMENT_01_174: [ If any error occurs in removing the pending operation from the list `on_amqp_management_error` callback shall be invoked while passing the `on_amqp_management_error_context` as argument. ]*/
                amqp_management->on_amqp_management_error(amqp_management->on_amqp_management_error_context);
                LogError("Cannot remove pending operation");
            }
            else
            {
                /* Codes_SRS_AMQP_MANAGEMENT_01_173: [ - The callback associated with the pending operation shall be called with `AMQP_MANAGEMENT_EXECUTE_OPERATION_ERROR`. ]*/
                pending_operation_message->on_execute_operation_complete(pending_operation_message->callback_context, AMQP_MANAGEMENT_EXECUTE_OPERATION_ERROR, 0, NULL, NULL);
                free(pending_operation_message);
            }
        }
    }
}

static void on_message_sender_state_changed(void* context, MESSAGE_SENDER_STATE new_state, MESSAGE_SENDER_STATE previous_state)
{
    if (context == NULL)
    {
        /* Codes_SRS_AMQP_MANAGEMENT_01_137: [ When `on_message_sender_state_changed` is called with NULL `context`, it shall do nothing. ]*/
        LogError("on_message_sender_state_changed called with NULL context");
    }
    else
    {
        /* Codes_SRS_AMQP_MANAGEMENT_01_138: [ When `on_message_sender_state_changed` is called and the `new_state` is different than `previous_state`, the following actions shall be taken: ]*/
        /* Codes_SRS_AMQP_MANAGEMENT_01_148: [ When no state change is detected, `on_message_sender_state_changed` shall do nothing. ]*/
        if (new_state != previous_state)
        {
            AMQP_MANAGEMENT_INSTANCE* amqp_management_instance = (AMQP_MANAGEMENT_INSTANCE*)context;
            switch (amqp_management_instance->amqp_management_state)
            {
            default:
                break;

            /* Codes_SRS_AMQP_MANAGEMENT_01_139: [ For the current state of AMQP management being `OPENING`: ]*/
            case AMQP_MANAGEMENT_STATE_OPENING:
            {
                switch (new_state)
                {
                case MESSAGE_SENDER_STATE_OPENING:
                    /* Codes_SRS_AMQP_MANAGEMENT_01_165: [ - If `new_state` is `MESSAGE_SENDER_STATE_OPEING` the transition shall be ignored. ]*/
                    break;

                default:
                /* Codes_SRS_AMQP_MANAGEMENT_01_140: [ - If `new_state` is `MESSAGE_SENDER_STATE_IDLE`, `MESSAGE_SENDER_STATE_CLOSING` or `MESSAGE_SENDER_STATE_ERROR`, the `on_amqp_management_open_complete` callback shall be called with `AMQP_MANAGEMENT_OPEN_ERROR`, while also passing the context passed in `amqp_management_open_async`. ]*/
                case MESSAGE_SENDER_STATE_IDLE:
                case MESSAGE_SENDER_STATE_CLOSING:
                case MESSAGE_SENDER_STATE_ERROR:
                    amqp_management_instance->amqp_management_state = AMQP_MANAGEMENT_STATE_IDLE;
                    amqp_management_instance->on_amqp_management_open_complete(amqp_management_instance->on_amqp_management_open_complete_context, AMQP_MANAGEMENT_OPEN_ERROR);
                    break;

                case MESSAGE_SENDER_STATE_OPEN:
                    amqp_management_instance->sender_connected = 1;
                    /* Codes_SRS_AMQP_MANAGEMENT_01_142: [ - If `new_state` is `MESSAGE_SENDER_STATE_OPEN` and the message receiver did not yet indicate its state as `MESSAGE_RECEIVER_STATE_OPEN`, the `on_amqp_management_open_complete` callback shall not be called.]*/
                    if (amqp_management_instance->receiver_connected != 0)
                    {
                        /* Codes_SRS_AMQP_MANAGEMENT_01_141: [ - If `new_state` is `MESSAGE_SENDER_STATE_OPEN` and the message receiver already indicated its state as `MESSAGE_RECEIVER_STATE_OPEN`, the `on_amqp_management_open_complete` callback shall be called with `AMQP_MANAGEMENT_OPEN_OK`, while also passing the context passed in `amqp_management_open_async`. ]*/
                        amqp_management_instance->amqp_management_state = AMQP_MANAGEMENT_STATE_OPEN;
                        amqp_management_instance->on_amqp_management_open_complete(amqp_management_instance->on_amqp_management_open_complete_context, AMQP_MANAGEMENT_OPEN_OK);
                    }
                    break;
                }
                break;
            }
            /* Codes_SRS_AMQP_MANAGEMENT_01_144: [ For the current state of AMQP management being `OPEN`: ]*/
            case AMQP_MANAGEMENT_STATE_OPEN:
            {
                switch (new_state)
                {
                default:
                /* Codes_SRS_AMQP_MANAGEMENT_01_143: [ - If `new_state` is `MESSAGE_SENDER_STATE_IDLE`, `MESSAGE_SENDER_STATE_OPENING`, `MESSAGE_SENDER_STATE_CLOSING` or `MESSAGE_SENDER_STATE_ERROR` the `on_amqp_management_error` callback shall be invoked while passing the `on_amqp_management_error_context` as argument. ]*/
                case MESSAGE_SENDER_STATE_IDLE:
                case MESSAGE_SENDER_STATE_CLOSING:
                case MESSAGE_SENDER_STATE_ERROR:
                    amqp_management_instance->amqp_management_state = AMQP_MANAGEMENT_STATE_ERROR;
                    amqp_management_instance->on_amqp_management_error(amqp_management_instance->on_amqp_management_error_context);
                    break;

                case MESSAGE_SENDER_STATE_OPEN:
                    /* Codes_SRS_AMQP_MANAGEMENT_01_145: [ - If `new_state` is `MESSAGE_SENDER_STATE_OPEN`, `on_message_sender_state_changed` shall do nothing. ]*/
                    break;
                }
                break;
            }
            /* Codes_SRS_AMQP_MANAGEMENT_09_001: [ For the current state of AMQP management being `CLOSING`: ]*/
            case AMQP_MANAGEMENT_STATE_CLOSING:
            {
                switch (new_state)
                {
                default:
                    /* Codes_SRS_AMQP_MANAGEMENT_09_002: [ - If `new_state` is `MESSAGE_SENDER_STATE_OPEN`, `MESSAGE_SENDER_STATE_OPENING`, `MESSAGE_SENDER_STATE_ERROR` the `on_amqp_management_error` callback shall be invoked while passing the `on_amqp_management_error_context` as argument. ]*/
                case MESSAGE_SENDER_STATE_OPEN:
                case MESSAGE_SENDER_STATE_OPENING:
                case MESSAGE_SENDER_STATE_ERROR:
                    amqp_management_instance->amqp_management_state = AMQP_MANAGEMENT_STATE_ERROR;
                    amqp_management_instance->on_amqp_management_error(amqp_management_instance->on_amqp_management_error_context);
                    break;
                case MESSAGE_SENDER_STATE_IDLE:
                case MESSAGE_SENDER_STATE_CLOSING:
                    /* Codes_SRS_AMQP_MANAGEMENT_09_003: [ - If `new_state` is `MESSAGE_SENDER_STATE_CLOSING` or `MESSAGE_SENDER_STATE_IDLE`, `on_message_sender_state_changed` shall do nothing. ]*/
                    break;
                }
                break;
            }
            /* Codes_SRS_AMQP_MANAGEMENT_01_146: [ For the current state of AMQP management being `ERROR`: ]*/
            case AMQP_MANAGEMENT_STATE_ERROR:
                /* Codes_SRS_AMQP_MANAGEMENT_01_147: [ - All state transitions shall be ignored. ]*/
                break;
            }
        }
    }
}

static void on_message_receiver_state_changed(const void* context, MESSAGE_RECEIVER_STATE new_state, MESSAGE_RECEIVER_STATE previous_state)
{
    if (context == NULL)
    {
        /* Codes_SRS_AMQP_MANAGEMENT_01_149: [ When `on_message_receiver_state_changed` is called with NULL `context`, it shall do nothing. ]*/
        LogError("on_message_receiver_state_changed called with NULL context");
    }
    else
    {
        /* Codes_SRS_AMQP_MANAGEMENT_01_150: [ When `on_message_receiver_state_changed` is called and the `new_state` is different than `previous_state`, the following actions shall be taken: ]*/
        /* Codes_SRS_AMQP_MANAGEMENT_01_160: [ When no state change is detected, `on_message_receiver_state_changed` shall do nothing. ]*/
        if (new_state != previous_state)
        {
            AMQP_MANAGEMENT_INSTANCE* amqp_management_instance = (AMQP_MANAGEMENT_INSTANCE*)context;
            switch (amqp_management_instance->amqp_management_state)
            {
            default:
                break;

            /* Codes_SRS_AMQP_MANAGEMENT_01_151: [ For the current state of AMQP management being `OPENING`: ]*/
            case AMQP_MANAGEMENT_STATE_OPENING:
            {
                switch (new_state)
                {
                case MESSAGE_RECEIVER_STATE_OPENING:
                    /* Codes_SRS_AMQP_MANAGEMENT_01_164: [ - If `new_state` is `MESSAGE_RECEIVER_STATE_OPEING` the transition shall be ignored. ]*/
                    break;

                default:
                /* Codes_SRS_AMQP_MANAGEMENT_01_152: [ - If `new_state` is `MESSAGE_RECEIVER_STATE_IDLE`, `MESSAGE_RECEIVER_STATE_CLOSING` or `MESSAGE_RECEIVER_STATE_ERROR`, the `on_amqp_management_open_complete` callback shall be called with `AMQP_MANAGEMENT_OPEN_ERROR`, while also passing the context passed in `amqp_management_open_async`. ]*/
                case MESSAGE_RECEIVER_STATE_IDLE:
                case MESSAGE_RECEIVER_STATE_CLOSING:
                case MESSAGE_RECEIVER_STATE_ERROR:
                    amqp_management_instance->amqp_management_state = AMQP_MANAGEMENT_STATE_IDLE;
                    amqp_management_instance->on_amqp_management_open_complete(amqp_management_instance->on_amqp_management_open_complete_context, AMQP_MANAGEMENT_OPEN_ERROR);
                    break;

                case MESSAGE_RECEIVER_STATE_OPEN:
                    amqp_management_instance->receiver_connected = 1;
                    /* Codes_SRS_AMQP_MANAGEMENT_01_154: [ - If `new_state` is `MESSAGE_RECEIVER_STATE_OPEN` and the message sender did not yet indicate its state as `MESSAGE_RECEIVER_STATE_OPEN`, the `on_amqp_management_open_complete` callback shall not be called. ]*/
                    if (amqp_management_instance->sender_connected != 0)
                    {
                        /* Codes_SRS_AMQP_MANAGEMENT_01_153: [ - If `new_state` is `MESSAGE_RECEIVER_STATE_OPEN` and the message sender already indicated its state as `MESSAGE_RECEIVER_STATE_OPEN`, the `on_amqp_management_open_complete` callback shall be called with `AMQP_MANAGEMENT_OPEN_OK`, while also passing the context passed in `amqp_management_open_async`. ]*/
                        amqp_management_instance->amqp_management_state = AMQP_MANAGEMENT_STATE_OPEN;
                        amqp_management_instance->on_amqp_management_open_complete(amqp_management_instance->on_amqp_management_open_complete_context, AMQP_MANAGEMENT_OPEN_OK);
                    }
                    break;
                }
                break;
            }
            /* Codes_SRS_AMQP_MANAGEMENT_01_155: [ For the current state of AMQP management being `OPEN`: ]*/
            case AMQP_MANAGEMENT_STATE_OPEN:
            {
                switch (new_state)
                {
                default:
                /* Codes_SRS_AMQP_MANAGEMENT_01_156: [ - If `new_state` is `MESSAGE_RECEIVER_STATE_IDLE`, `MESSAGE_RECEIVER_STATE_OPENING`, `MESSAGE_RECEIVER_STATE_CLOSING` or `MESSAGE_RECEIVER_STATE_ERROR` the `on_amqp_management_error` callback shall be invoked while passing the `on_amqp_management_error_context` as argument. ]*/
                case MESSAGE_RECEIVER_STATE_IDLE:
                case MESSAGE_RECEIVER_STATE_CLOSING:
                case MESSAGE_RECEIVER_STATE_ERROR:
                    amqp_management_instance->amqp_management_state = AMQP_MANAGEMENT_STATE_ERROR;
                    amqp_management_instance->on_amqp_management_error(amqp_management_instance->on_amqp_management_error_context);
                    break;

                case MESSAGE_RECEIVER_STATE_OPEN:
                    /* Codes_SRS_AMQP_MANAGEMENT_01_157: [ - If `new_state` is `MESSAGE_RECEIVER_STATE_OPEN`, `on_message_receiver_state_changed` shall do nothing. ]*/
                    break;
                }
                break;
            }
            /* Codes_SRS_AMQP_MANAGEMENT_01_158: [ For the current state of AMQP management being `ERROR`: ]*/
            case AMQP_MANAGEMENT_STATE_ERROR:
                /* Codes_SRS_AMQP_MANAGEMENT_01_159: [ - All state transitions shall be ignored. ]*/
                break;
            }
        }
    }
}

static int set_message_id(MESSAGE_HANDLE message, uint64_t next_message_id)
{
    int result;
    PROPERTIES_HANDLE properties;

    /* Codes_SRS_AMQP_MANAGEMENT_01_094: [ In order to set the message Id on the message, the properties shall be obtained by calling `message_get_properties`. ]*/
    if (message_get_properties(message, &properties) != 0)
    {
        /* Codes_SRS_AMQP_MANAGEMENT_01_098: [ If any API fails while setting the message Id, `amqp_management_execute_operation_async` shall fail and return a non-zero value. ]*/
        LogError("Could not retrieve message properties");
        result = MU_FAILURE;
    }
    else
    {
        /* Codes_SRS_AMQP_MANAGEMENT_01_099: [ If the properties were not set on the message, a new properties instance shall be created by calling `properties_create`. ]*/
        if (properties == NULL)
        {
            properties = properties_create();
        }

        if (properties == NULL)
        {
            /* Codes_SRS_AMQP_MANAGEMENT_01_098: [ If any API fails while setting the message Id, `amqp_management_execute_operation_async` shall fail and return a non-zero value. ]*/
            LogError("Could not create message properties");
            result = MU_FAILURE;
        }
        else
        {
            /* Codes_SRS_AMQP_MANAGEMENT_01_095: [ A message Id with the next ulong value to be used shall be created by calling `amqpvalue_create_message_id_ulong`. ]*/
            AMQP_VALUE message_id = amqpvalue_create_message_id_ulong(next_message_id);
            if (message_id == NULL)
            {
                /* Codes_SRS_AMQP_MANAGEMENT_01_098: [ If any API fails while setting the message Id, `amqp_management_execute_operation_async` shall fail and return a non-zero value. ]*/
                LogError("Could not create message id value");
                result = MU_FAILURE;
            }
            else
            {
                /* Codes_SRS_AMQP_MANAGEMENT_01_096: [ The message Id value shall be set on the properties by calling `properties_set_message_id`. ]*/
                if (properties_set_message_id(properties, message_id) != 0)
                {
                    /* Codes_SRS_AMQP_MANAGEMENT_01_098: [ If any API fails while setting the message Id, `amqp_management_execute_operation_async` shall fail and return a non-zero value. ]*/
                    LogError("Could not set message Id on the properties");
                    result = MU_FAILURE;
                }
                /* Codes_SRS_AMQP_MANAGEMENT_01_097: [ The properties thus modified to contain the message Id shall be set on the message by calling `message_set_properties`. ]*/
                else if (message_set_properties(message, properties) != 0)
                {
                    /* Codes_SRS_AMQP_MANAGEMENT_01_098: [ If any API fails while setting the message Id, `amqp_management_execute_operation_async` shall fail and return a non-zero value. ]*/
                    LogError("Could not set message properties");
                    result = MU_FAILURE;
                }
                else
                {
                    result = 0;
                }

                amqpvalue_destroy(message_id);
            }

            /* Codes_SRS_AMQP_MANAGEMENT_01_100: [ After setting the properties, the properties instance shall be freed by `properties_destroy`. ]*/
            properties_destroy(properties);
        }
    }

    return result;
}

static int add_string_key_value_pair_to_map(AMQP_VALUE map, const char* key, const char* value)
{
    int result;

    /* Codes_SRS_AMQP_MANAGEMENT_01_084: [ For each of the arguments `operation`, `type` and `locales` an AMQP value of type string shall be created by calling `amqpvalue_create_string` in order to be used as key in the application properties map. ]*/
    AMQP_VALUE key_value = amqpvalue_create_string(key);
    if (key_value == NULL)
    {
        /* Codes_SRS_AMQP_MANAGEMENT_01_090: [ If any APIs used to create and set the application properties on the message fails, `amqp_management_execute_operation_async` shall fail and return a non-zero value. ]*/
        LogError("Could not create key value for %s", key);
        result = MU_FAILURE;
    }
    else
    {
        /* Codes_SRS_AMQP_MANAGEMENT_01_085: [ For each of the arguments `operation`, `type` and `locales` an AMQP value of type string containing the argument value shall be created by calling `amqpvalue_create_string` in order to be used as value in the application properties map. ]*/
        AMQP_VALUE value_value = amqpvalue_create_string(value);
        if (value_value == NULL)
        {
            /* Codes_SRS_AMQP_MANAGEMENT_01_090: [ If any APIs used to create and set the application properties on the message fails, `amqp_management_execute_operation_async` shall fail and return a non-zero value. ]*/
            LogError("Could not create value for key %s", key);
            result = MU_FAILURE;
        }
        else
        {
            /* Codes_SRS_AMQP_MANAGEMENT_01_086: [ The key/value pairs for `operation`, `type` and `locales` shall be added to the application properties map by calling `amqpvalue_set_map_value`. ]*/
            if (amqpvalue_set_map_value(map, key_value, value_value) != 0)
            {
                /* Codes_SRS_AMQP_MANAGEMENT_01_090: [ If any APIs used to create and set the application properties on the message fails, `amqp_management_execute_operation_async` shall fail and return a non-zero value. ]*/
                LogError("Could not set the value in the map for key %s", key);
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(value_value);
        }

        amqpvalue_destroy(key_value);
    }

    return result;
}

static int internal_set_status_code_key_name(AMQP_MANAGEMENT_HANDLE amqp_management, const char* status_code_key_name)
{
    int result;
    char* copied_status_code_key_name;
    if (mallocAndStrcpy_s(&copied_status_code_key_name, status_code_key_name) != 0)
    {
        LogError("Cannot copy status code key name");
        result = MU_FAILURE;
    }
    else
    {
        if (amqp_management->status_code_key_name != NULL)
        {
            free(amqp_management->status_code_key_name);
        }

        amqp_management->status_code_key_name = copied_status_code_key_name;
        result = 0;
    }

    return result;
}

static int internal_set_status_description_key_name(AMQP_MANAGEMENT_HANDLE amqp_management, const char* status_description_key_name)
{
    int result;
    char* copied_status_description_key_name;
    if (mallocAndStrcpy_s(&copied_status_description_key_name, status_description_key_name) != 0)
    {
        LogError("Cannot copy status description key name");
        result = MU_FAILURE;
    }
    else
    {
        if (amqp_management->status_description_key_name != NULL)
        {
            free(amqp_management->status_description_key_name);
        }

        amqp_management->status_description_key_name = copied_status_description_key_name;
        result = 0;
    }

    return result;
}

AMQP_MANAGEMENT_HANDLE amqp_management_create(SESSION_HANDLE session, const char* management_node)
{
    AMQP_MANAGEMENT_INSTANCE* amqp_management;

    if ((session == NULL) ||
        (management_node == NULL))
    {
        /* Codes_SRS_AMQP_MANAGEMENT_01_002: [ If `session` or `management_node` is NULL then `amqp_management_create` shall fail and return NULL. ]*/
        LogError("Bad arguments: session = %p, management_node = %p", session, management_node);
        amqp_management = NULL;
    }
    else if (strlen(management_node) == 0)
    {
        /* Codes_SRS_AMQP_MANAGEMENT_01_030: [ If `management_node` is an empty string, then `amqp_management_create` shall fail and return NULL. ]*/
        LogError("Empty string management node");
        amqp_management = NULL;
    }
    else
    {
        /* Codes_SRS_AMQP_MANAGEMENT_01_001: [ `amqp_management_create` shall create a new CBS instance and on success return a non-NULL handle to it. ]*/
        amqp_management = (AMQP_MANAGEMENT_INSTANCE*)calloc(1, sizeof(AMQP_MANAGEMENT_INSTANCE));
        if (amqp_management == NULL)
        {
            /* Codes_SRS_AMQP_MANAGEMENT_01_005: [ If allocating memory for the new handle fails, `amqp_management_create` shall fail and return NULL. ]*/
            LogError("Cannot allocate memory for AMQP management handle");
        }
        else
        {
            amqp_management->sender_connected = 0;
            amqp_management->receiver_connected = 0;
            amqp_management->on_amqp_management_open_complete = NULL;
            amqp_management->on_amqp_management_open_complete_context = NULL;
            amqp_management->on_amqp_management_error = NULL;
            amqp_management->on_amqp_management_error_context = NULL;
            amqp_management->amqp_management_state = AMQP_MANAGEMENT_STATE_IDLE;
            amqp_management->status_code_key_name = NULL;
            amqp_management->status_description_key_name = NULL;

            /* Codes_SRS_AMQP_MANAGEMENT_01_003: [ `amqp_management_create` shall create a singly linked list for pending operations by calling `singlylinkedlist_create`. ]*/
            amqp_management->pending_operations = singlylinkedlist_create();
            if (amqp_management->pending_operations == NULL)
            {
                /* Codes_SRS_AMQP_MANAGEMENT_01_004: [ If `singlylinkedlist_create` fails, `amqp_management_create` shall fail and return NULL. ]*/
                LogError("Cannot create pending operations list");
            }
            else
            {
                /* Codes_SRS_AMQP_MANAGEMENT_01_181: [ `amqp_management_create` shall set the status code key name to be used for parsing the status code to `statusCode`. ]*/
                if (internal_set_status_code_key_name(amqp_management, "statusCode") != 0)
                {
                    LogError("Cannot set status code key name");
                }
                else
                {
                    /* Codes_SRS_AMQP_MANAGEMENT_01_182: [ `amqp_management_create` shall set the status description key name to be used for parsing the status description to `statusDescription`. ]*/
                    if (internal_set_status_description_key_name(amqp_management, "statusDescription") != 0)
                    {
                        LogError("Cannot set status description key name");
                    }
                    else
                    {
                        /* Codes_SRS_AMQP_MANAGEMENT_01_010: [ The `source` argument shall be a value created by calling `messaging_create_source` with `management_node` as argument. ]*/
                        AMQP_VALUE source = messaging_create_source(management_node);
                        if (source == NULL)
                        {
                            /* Codes_SRS_AMQP_MANAGEMENT_01_012: [ If `messaging_create_source` fails then `amqp_management_create` shall fail and return NULL. ]*/
                            LogError("Failed creating source AMQP value");
                        }
                        else
                        {
                            /* Codes_SRS_AMQP_MANAGEMENT_01_011: [ The `target` argument shall be a value created by calling `messaging_create_target` with `management_node` as argument. ]*/
                            AMQP_VALUE target = messaging_create_target(management_node);
                            if (target == NULL)
                            {
                                /* Codes_SRS_AMQP_MANAGEMENT_01_013: [ If `messaging_create_target` fails then `amqp_management_create` shall fail and return NULL. ]*/
                                LogError("Failed creating target AMQP value");
                            }
                            else
                            {
                                size_t management_node_length = strlen(management_node);

                                char* sender_link_name = (char*)malloc(management_node_length + COUNT_CHARS(sender_suffix) + 1);
                                if (sender_link_name == NULL)
                                {
                                    /* Codes_SRS_AMQP_MANAGEMENT_01_033: [ If any other error occurs `amqp_management_create` shall fail and return NULL. ]*/
                                    LogError("Failed allocating memory for sender link name");
                                }
                                else
                                {
                                    char* receiver_link_name;

                                    (void)memcpy(sender_link_name, management_node, management_node_length);
                                    (void)memcpy(sender_link_name + management_node_length, sender_suffix, COUNT_CHARS(sender_suffix) + 1);

                                    receiver_link_name = (char*)malloc(management_node_length + COUNT_CHARS(receiver_suffix) + 1);
                                    if (receiver_link_name == NULL)
                                    {
                                        /* Codes_SRS_AMQP_MANAGEMENT_01_033: [ If any other error occurs `amqp_management_create` shall fail and return NULL. ]*/
                                        LogError("Failed allocating memory for receiver link name");
                                    }
                                    else
                                    {
                                        (void)memcpy(receiver_link_name, management_node, management_node_length);
                                        (void)memcpy(receiver_link_name + management_node_length, receiver_suffix, COUNT_CHARS(receiver_suffix) + 1);

                                        /* Codes_SRS_AMQP_MANAGEMENT_01_006: [ `amqp_management_create` shall create a sender link by calling `link_create`. ]*/
                                        /* Codes_SRS_AMQP_MANAGEMENT_01_007: [ The `session` argument shall be set to `session`. ]*/
                                        /* Codes_SRS_AMQP_MANAGEMENT_01_008: [ The `name` argument shall be constructed by concatenating the `management_node` value with `-sender`. ]*/
                                        /* Codes_SRS_AMQP_MANAGEMENT_01_009: [ The `role` argument shall be `role_sender`. ]*/
                                        /* Codes_SRS_AMQP_MANAGEMENT_01_019: [ The `source` argument shall be the value created by calling `messaging_create_source`. ]*/
                                        /* Codes_SRS_AMQP_MANAGEMENT_01_020: [ The `target` argument shall be the value created by calling `messaging_create_target`. ]*/
                                        amqp_management->sender_link = link_create(session, sender_link_name, role_sender, source, target);
                                        if (amqp_management->sender_link == NULL)
                                        {
                                            /* Codes_SRS_AMQP_MANAGEMENT_01_014: [ If `link_create` fails when creating the sender link then `amqp_management_create` shall fail and return NULL. ]*/
                                            LogError("Failed creating sender link");
                                        }
                                        else
                                        {
                                            /* Codes_SRS_AMQP_MANAGEMENT_01_015: [ `amqp_management_create` shall create a receiver link by calling `link_create`. ]*/
                                            /* Codes_SRS_AMQP_MANAGEMENT_01_016: [ The `session` argument shall be set to `session`. ]*/
                                            /* Codes_SRS_AMQP_MANAGEMENT_01_017: [ The `name` argument shall be constructed by concatenating the `management_node` value with `-receiver`. ]*/
                                            /* Codes_SRS_AMQP_MANAGEMENT_01_018: [ The `role` argument shall be `role_receiver`. ]*/
                                            /* Codes_SRS_AMQP_MANAGEMENT_01_019: [ The `source` argument shall be the value created by calling `messaging_create_source`. ]*/
                                            /* Codes_SRS_AMQP_MANAGEMENT_01_020: [ The `target` argument shall be the value created by calling `messaging_create_target`. ]*/
                                            amqp_management->receiver_link = link_create(session, receiver_link_name, role_receiver, source, target);
                                            if (amqp_management->receiver_link == NULL)
                                            {
                                                /* Codes_SRS_AMQP_MANAGEMENT_01_021: [ If `link_create` fails when creating the receiver link then `amqp_management_create` shall fail and return NULL. ]*/
                                                LogError("Failed creating receiver link");
                                            }
                                            else
                                            {
                                                /* Codes_SRS_AMQP_MANAGEMENT_01_022: [ `amqp_management_create` shall create a message sender by calling `messagesender_create` and passing to it the sender link handle. ]*/
                                                amqp_management->message_sender = messagesender_create(amqp_management->sender_link, on_message_sender_state_changed, amqp_management);
                                                if (amqp_management->message_sender == NULL)
                                                {
                                                    /* Codes_SRS_AMQP_MANAGEMENT_01_031: [ If `messagesender_create` fails then `amqp_management_create` shall fail and return NULL. ]*/
                                                    LogError("Failed creating message sender");
                                                }
                                                else
                                                {
                                                    /* Codes_SRS_AMQP_MANAGEMENT_01_023: [ `amqp_management_create` shall create a message receiver by calling `messagereceiver_create` and passing to it the receiver link handle. ]*/
                                                    amqp_management->message_receiver = messagereceiver_create(amqp_management->receiver_link, on_message_receiver_state_changed, amqp_management);
                                                    if (amqp_management->message_receiver == NULL)
                                                    {
                                                        /* Codes_SRS_AMQP_MANAGEMENT_01_032: [ If `messagereceiver_create` fails then `amqp_management_create` shall fail and return NULL. ]*/
                                                        LogError("Failed creating message receiver");
                                                        link_destroy(amqp_management->receiver_link);
                                                    }
                                                    else
                                                    {
                                                        free(receiver_link_name);
                                                        free(sender_link_name);
                                                        amqpvalue_destroy(target);
                                                        amqpvalue_destroy(source);

                                                        /* Codes_SRS_AMQP_MANAGEMENT_01_106: [ The message Id set on the message properties shall start at 0. ]*/
                                                        amqp_management->next_message_id = 0;

                                                        goto all_ok;
                                                    }

                                                    messagesender_destroy(amqp_management->message_sender);
                                                }

                                                link_destroy(amqp_management->receiver_link);
                                            }

                                            link_destroy(amqp_management->sender_link);
                                        }

                                        free(receiver_link_name);
                                    }

                                    free(sender_link_name);
                                }

                                amqpvalue_destroy(target);
                            }

                            amqpvalue_destroy(source);
                        }

                        free(amqp_management->status_description_key_name);
                    }

                    free(amqp_management->status_code_key_name);
                }

                singlylinkedlist_destroy(amqp_management->pending_operations);
            }

            free(amqp_management);
            amqp_management = NULL;
        }
    }

all_ok:
    return amqp_management;
}

void amqp_management_destroy(AMQP_MANAGEMENT_HANDLE amqp_management)
{
    if (amqp_management == NULL)
    {
        /* Codes_SRS_AMQP_MANAGEMENT_01_025: [ If `amqp_management` is NULL, `amqp_management_destroy` shall do nothing. ]*/
        LogError("NULL amqp_management");
    }
    else
    {
        /* Codes_SRS_AMQP_MANAGEMENT_01_024: [ `amqp_management_destroy` shall free all the resources allocated by `amqp_management_create`. ]*/
        if (amqp_management->amqp_management_state != AMQP_MANAGEMENT_STATE_IDLE)
        {
            (void)amqp_management_close(amqp_management);
        }

        /* Codes_SRS_AMQP_MANAGEMENT_01_028: [ `amqp_management_destroy` shall free the message sender by calling `messagesender_destroy`. ]*/
        messagesender_destroy(amqp_management->message_sender);
        /* Codes_SRS_AMQP_MANAGEMENT_01_029: [ `amqp_management_destroy` shall free the message receiver by calling `messagereceiver_destroy`. ]*/
        messagereceiver_destroy(amqp_management->message_receiver);
        /* Codes_SRS_AMQP_MANAGEMENT_01_027: [ `amqp_management_destroy` shall free the sender and receiver links by calling `link_destroy`. ]*/
        link_destroy(amqp_management->sender_link);
        link_destroy(amqp_management->receiver_link);
        free(amqp_management->status_code_key_name);
        free(amqp_management->status_description_key_name);
        /* Codes_SRS_AMQP_MANAGEMENT_01_026: [ `amqp_management_destroy` shall free the singly linked list by calling `singlylinkedlist_destroy`. ]*/
        singlylinkedlist_destroy(amqp_management->pending_operations);
        free(amqp_management);
    }
}

int amqp_management_open_async(AMQP_MANAGEMENT_HANDLE amqp_management, ON_AMQP_MANAGEMENT_OPEN_COMPLETE on_amqp_management_open_complete, void* on_amqp_management_open_complete_context, ON_AMQP_MANAGEMENT_ERROR on_amqp_management_error, void* on_amqp_management_error_context)
{
    int result;

    /* Codes_SRS_AMQP_MANAGEMENT_01_044: [ `on_amqp_management_open_complete_context` and `on_amqp_management_error_context` shall be allowed to be NULL. ]*/
    if ((amqp_management == NULL) ||
        (on_amqp_management_open_complete == NULL) ||
        (on_amqp_management_error == NULL))
    {
        /* Codes_SRS_AMQP_MANAGEMENT_01_038: [ If `amqp_management`, `on_amqp_management_open_complete` or `on_amqp_management_error` is NULL, `amqp_management_open_async` shall fail and return a non-zero value. ]*/
        LogError("Bad arguments: amqp_management = %p, on_amqp_management_open_complete = %p, on_amqp_management_error = %p",
            amqp_management,
            on_amqp_management_open_complete,
            on_amqp_management_error);
        result = MU_FAILURE;
    }
    else if (amqp_management->amqp_management_state != AMQP_MANAGEMENT_STATE_IDLE)
    {
        /* Codes_SRS_AMQP_MANAGEMENT_01_043: [ If the AMQP management instance is already OPEN or OPENING, `amqp_management_open_async` shall fail and return a non-zero value. ]*/
        LogError("AMQP management instance already OPEN");
        result = MU_FAILURE;
    }
    else
    {
        /* Codes_SRS_AMQP_MANAGEMENT_01_036: [ `amqp_management_open_async` shall start opening the AMQP management instance and save the callbacks so that they can be called when opening is complete. ]*/
        amqp_management->on_amqp_management_open_complete = on_amqp_management_open_complete;
        amqp_management->on_amqp_management_open_complete_context = on_amqp_management_open_complete_context;
        amqp_management->on_amqp_management_error = on_amqp_management_error;
        amqp_management->on_amqp_management_error_context = on_amqp_management_error_context;
        amqp_management->amqp_management_state = AMQP_MANAGEMENT_STATE_OPENING;

        /* Codes_SRS_AMQP_MANAGEMENT_01_040: [ `amqp_management_open_async` shall open the message receiver by calling `messagereceiver_open`. ]*/
        if (messagereceiver_open(amqp_management->message_receiver, on_message_received, amqp_management) != 0)
        {
            /* Codes_SRS_AMQP_MANAGEMENT_01_042: [ If `messagereceiver_open` fails, `amqp_management_open_async` shall fail and return a non-zero value. ]*/
            LogError("Failed opening message receiver");
            amqp_management->amqp_management_state = AMQP_MANAGEMENT_STATE_IDLE;
            result = MU_FAILURE;
        }
        else
        {
            /* Codes_SRS_AMQP_MANAGEMENT_01_039: [ `amqp_management_open_async` shall open the message sender by calling `messagesender_open`. ]*/
            if (messagesender_open(amqp_management->message_sender) != 0)
            {
                /* Codes_SRS_AMQP_MANAGEMENT_01_041: [ If `messagesender_open` fails, `amqp_management_open_async` shall fail and return a non-zero value. ]*/
                LogError("Failed opening message sender");
                amqp_management->amqp_management_state = AMQP_MANAGEMENT_STATE_IDLE;
                (void)messagereceiver_close(amqp_management->message_receiver);
                result = MU_FAILURE;
            }
            else
            {
                /* Codes_SRS_AMQP_MANAGEMENT_01_037: [ On success it shall return 0. ]*/
                result = 0;
            }
        }
    }

    return result;
}

int amqp_management_close(AMQP_MANAGEMENT_HANDLE amqp_management)
{
    int result;

    if (amqp_management == NULL)
    {
        /* Codes_SRS_AMQP_MANAGEMENT_01_047: [ If `amqp_management` is NULL, `amqp_management_close` shall fail and return a non-zero value. ]*/
        LogError("NULL amqp_management");
        result = MU_FAILURE;
    }
    else if (amqp_management->amqp_management_state == AMQP_MANAGEMENT_STATE_IDLE)
    {
        /* Codes_SRS_AMQP_MANAGEMENT_01_049: [ `amqp_management_close` on an AMQP management instance that is not OPEN, shall fail and return a non-zero value. ]*/
        LogError("AMQP management instance not open");
        result = MU_FAILURE;
    }
    else
    {
        AMQP_MANAGEMENT_STATE previous_state = amqp_management->amqp_management_state;

        amqp_management->amqp_management_state = AMQP_MANAGEMENT_STATE_CLOSING;

        if (previous_state == AMQP_MANAGEMENT_STATE_OPENING)
        {
            /* Codes_SRS_AMQP_MANAGEMENT_01_048: [ `amqp_management_close` on an AMQP management instance that is OPENING shall trigger the `on_amqp_management_open_complete` callback with `AMQP_MANAGEMENT_OPEN_CANCELLED`, while also passing the context passed in `amqp_management_open_async`. ]*/
            amqp_management->on_amqp_management_open_complete(amqp_management->on_amqp_management_open_complete_context, AMQP_MANAGEMENT_OPEN_CANCELLED);
        }


        /* Codes_SRS_AMQP_MANAGEMENT_01_045: [ `amqp_management_close` shall close the AMQP management instance. ]*/
        /* Codes_SRS_AMQP_MANAGEMENT_01_050: [ `amqp_management_close` shall close the message sender by calling `messagesender_close`. ]*/
        if (messagesender_close(amqp_management->message_sender) != 0)
        {
            /* Codes_SRS_AMQP_MANAGEMENT_01_052: [ If `messagesender_close` fails, `amqp_management_close` shall fail and return a non-zero value. ]*/
            LogError("messagesender_close failed");
            result = MU_FAILURE;
        }
        /* Codes_SRS_AMQP_MANAGEMENT_01_051: [ `amqp_management_close` shall close the message receiver by calling `messagereceiver_close`. ]*/
        else if (messagereceiver_close(amqp_management->message_receiver) != 0)
        {
            /* Codes_SRS_AMQP_MANAGEMENT_01_053: [ If `messagereceiver_close` fails, `amqp_management_close` shall fail and return a non-zero value. ]*/
            LogError("messagereceiver_close failed");
            result = MU_FAILURE;
        }
        else
        {
            LIST_ITEM_HANDLE list_item_handle = singlylinkedlist_get_head_item(amqp_management->pending_operations);
            while (list_item_handle != NULL)
            {
                OPERATION_MESSAGE_INSTANCE* operation_message = (OPERATION_MESSAGE_INSTANCE*)singlylinkedlist_item_get_value(list_item_handle);
                if (operation_message == NULL)
                {
                    LogError("Cannot obtain pending operation");
                }
                else
                {
                    /* Codes_SRS_AMQP_MANAGEMENT_01_054: [ All pending operations shall be indicated complete with the code `AMQP_MANAGEMENT_EXECUTE_OPERATION_INSTANCE_CLOSED`. ]*/
                    operation_message->on_execute_operation_complete(operation_message->callback_context, AMQP_MANAGEMENT_EXECUTE_OPERATION_INSTANCE_CLOSED, 0, NULL, NULL);
                    free(operation_message);
                }

                if (singlylinkedlist_remove(amqp_management->pending_operations, list_item_handle) != 0)
                {
                    LogError("Cannot remove item");
                }

                list_item_handle = singlylinkedlist_get_head_item(amqp_management->pending_operations);
            }

            amqp_management->amqp_management_state = AMQP_MANAGEMENT_STATE_IDLE;

            /* Codes_SRS_AMQP_MANAGEMENT_01_046: [ On success it shall return 0. ]*/
            result = 0;
        }
    }

    return result;
}

int amqp_management_execute_operation_async(AMQP_MANAGEMENT_HANDLE amqp_management, const char* operation, const char* type, const char* locales, MESSAGE_HANDLE message, ON_AMQP_MANAGEMENT_EXECUTE_OPERATION_COMPLETE on_execute_operation_complete, void* on_execute_operation_complete_context)
{
    int result;

    if ((amqp_management == NULL) ||
        (operation == NULL) ||
        (type == NULL) ||
        (on_execute_operation_complete == NULL))
    {
        /* Codes_SRS_AMQP_MANAGEMENT_01_057: [ If `amqp_management`, `operation`, `type` or `on_execute_operation_complete` is NULL, `amqp_management_execute_operation_async` shall fail and return a non-zero value. ]*/
        LogError("Bad arguments: amqp_management = %p, operation = %p, type = %p",
            amqp_management, operation, type);
        result = MU_FAILURE;
    }
    /* Codes_SRS_AMQP_MANAGEMENT_01_081: [ If `amqp_management_execute_operation_async` is called when not OPEN, it shall fail and return a non-zero value. ]*/
    else if ((amqp_management->amqp_management_state == AMQP_MANAGEMENT_STATE_IDLE) ||
        /* Codes_SRS_AMQP_MANAGEMENT_01_104: [ If `amqp_management_execute_operation_async` is called when the AMQP management is in error, it shall fail and return a non-zero value. ]*/
        (amqp_management->amqp_management_state == AMQP_MANAGEMENT_STATE_ERROR))
    {
        LogError("amqp_management_execute_operation_async called while not open or in error");
        result = MU_FAILURE;
    }
    else
    {
        AMQP_VALUE application_properties;
        MESSAGE_HANDLE cloned_message;

        if (message == NULL)
        {
            /* Codes_SRS_AMQP_MANAGEMENT_01_102: [ If `message` is NULL, a new message shall be created by calling `message_create`. ]*/
            cloned_message = message_create();
        }
        else
        {
            /* Codes_SRS_AMQP_MANAGEMENT_01_103: [ Otherwise the existing message shall be cloned by using `message_clone` before being modified accordingly and used for the pending operation. ]*/
            cloned_message = message_clone(message);
            if (cloned_message == NULL)
            {
                LogError("Could not clone message");
            }
        }

        if (cloned_message == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            /* Codes_SRS_AMQP_MANAGEMENT_01_055: [ `amqp_management_execute_operation_async` shall start an AMQP management operation. ]*/
            /* Codes_SRS_AMQP_MANAGEMENT_01_082: [ `amqp_management_execute_operation_async` shall obtain the application properties from the message by calling `message_get_application_properties`. ]*/
            if (message_get_application_properties(cloned_message, &application_properties) != 0)
            {
                LogError("Could not get application properties");
                result = MU_FAILURE;
            }
            else
            {
                if (application_properties == NULL)
                {
                    /* Codes_SRS_AMQP_MANAGEMENT_01_083: [ If no application properties were set on the message, a new application properties instance shall be created by calling `amqpvalue_create_map`; ]*/
                    application_properties = amqpvalue_create_map();
                    if (application_properties == NULL)
                    {
                        LogError("Could not create application properties");
                    }
                }

                if (application_properties == NULL)
                {
                    result = MU_FAILURE;
                }
                else
                {
                    /* Codes_SRS_AMQP_MANAGEMENT_01_084: [ For each of the arguments `operation`, `type` and `locales` an AMQP value of type string shall be created by calling `amqpvalue_create_string` in order to be used as key in the application properties map. ]*/
                    /* Codes_SRS_AMQP_MANAGEMENT_01_085: [ For each of the arguments `operation`, `type` and `locales` an AMQP value of type string containing the argument value shall be created by calling `amqpvalue_create_string` in order to be used as value in the application properties map. ]*/
                    /* Codes_SRS_AMQP_MANAGEMENT_01_058: [ Request messages have the following application-properties: ]*/
                    /* Codes_SRS_AMQP_MANAGEMENT_01_059: [ operation string Yes The management operation to be performed. ] */
                    if ((add_string_key_value_pair_to_map(application_properties, "operation", operation) != 0) ||
                        /* Codes_SRS_AMQP_MANAGEMENT_01_061: [ type string Yes The Manageable Entity Type of the Manageable Entity to be managed. ]*/
                        (add_string_key_value_pair_to_map(application_properties, "type", type) != 0) ||
                        /* Codes_SRS_AMQP_MANAGEMENT_01_093: [ If `locales` is NULL, no key/value pair shall be added for it in the application properties map. ]*/
                        /* Codes_SRS_AMQP_MANAGEMENT_01_063: [ locales string No A list of locales that the sending peer permits for incoming informational text in response messages. ]*/
                        ((locales != NULL) && (add_string_key_value_pair_to_map(application_properties, "locales", locales) != 0)))
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        /* Codes_SRS_AMQP_MANAGEMENT_01_087: [ The application properties obtained after adding the key/value pairs shall be set on the message by calling `message_set_application_properties`. ]*/
                        if (message_set_application_properties(cloned_message, application_properties) != 0)
                        {
                            /* Codes_SRS_AMQP_MANAGEMENT_01_090: [ If any APIs used to create and set the application properties on the message fails, `amqp_management_execute_operation_async` shall fail and return a non-zero value. ]*/
                            LogError("Could not set application properties");
                            result = MU_FAILURE;
                        }
                        else if (set_message_id(cloned_message, amqp_management->next_message_id) != 0)
                        {
                            result = MU_FAILURE;
                        }
                        else
                        {
                            OPERATION_MESSAGE_INSTANCE* pending_operation_message = (OPERATION_MESSAGE_INSTANCE*)calloc(1, sizeof(OPERATION_MESSAGE_INSTANCE));
                            if (pending_operation_message == NULL)
                            {
                                result = MU_FAILURE;
                            }
                            else
                            {
                                LIST_ITEM_HANDLE added_item;
                                pending_operation_message->callback_context = on_execute_operation_complete_context;
                                pending_operation_message->on_execute_operation_complete = on_execute_operation_complete;
                                pending_operation_message->message_id = amqp_management->next_message_id;
                                pending_operation_message->amqp_management = amqp_management;

                                /* Codes_SRS_AMQP_MANAGEMENT_01_091: [ Once the request message has been sent, an entry shall be stored in the pending operations list by calling `singlylinkedlist_add`. ]*/
                                added_item = singlylinkedlist_add(amqp_management->pending_operations, pending_operation_message);
                                if (added_item == NULL)
                                {
                                    /* Codes_SRS_AMQP_MANAGEMENT_01_092: [ If `singlylinkedlist_add` fails then `amqp_management_execute_operation_async` shall fail and return a non-zero value. ]*/
                                    LogError("Could not add the operation to the pending operations list.");
                                    free(pending_operation_message);
                                    result = MU_FAILURE;
                                }
                                else
                                {
                                    /* Codes_SRS_AMQP_MANAGEMENT_01_088: [ `amqp_management_execute_operation_async` shall send the message by calling `messagesender_send_async`. ]*/
                                    /* Codes_SRS_AMQP_MANAGEMENT_01_166: [ The `on_message_send_complete` callback shall be passed to the `messagesender_send_async` call. ]*/
                                    if (messagesender_send_async(amqp_management->message_sender, cloned_message, on_message_send_complete, added_item, 0) == NULL)
                                    {
                                        /* Codes_SRS_AMQP_MANAGEMENT_01_089: [ If `messagesender_send_async` fails, `amqp_management_execute_operation_async` shall fail and return a non-zero value. ]*/
                                        LogError("Could not send request message");
                                        (void)singlylinkedlist_remove(amqp_management->pending_operations, added_item);
                                        free(pending_operation_message);
                                        result = MU_FAILURE;
                                    }
                                    else
                                    {
                                        /* Codes_SRS_AMQP_MANAGEMENT_01_107: [ The message Id set on the message properties shall be incremented with each operation. ]*/
                                        amqp_management->next_message_id++;

                                        /* Codes_SRS_AMQP_MANAGEMENT_01_056: [ On success it shall return 0. ]*/
                                        result = 0;
                                    }
                                }
                            }
                        }
                    }

                    /* Codes_SRS_AMQP_MANAGEMENT_01_101: [ After setting the application properties, the application properties instance shall be freed by `amqpvalue_destroy`. ]*/
                    amqpvalue_destroy(application_properties);
                }
            }

            message_destroy(cloned_message);
        }
    }
    return result;
}

void amqp_management_set_trace(AMQP_MANAGEMENT_HANDLE amqp_management, bool trace_on)
{
    if (amqp_management == NULL)
    {
        /* Codes_SRS_AMQP_MANAGEMENT_01_163: [ If `amqp_management` is NULL, `amqp_management_set_trace` shal do nothing. ]*/
        LogError("NULL amqp_management");
    }
    else
    {
        /* Codes_SRS_AMQP_MANAGEMENT_01_161: [ `amqp_management_set_trace` shall call `messagesender_set_trace` to enable/disable tracing on the message sender. ]*/
        messagesender_set_trace(amqp_management->message_sender, trace_on);
        /* Codes_SRS_AMQP_MANAGEMENT_01_162: [ `amqp_management_set_trace` shall call `messagereceiver_set_trace` to enable/disable tracing on the message receiver. ]*/
        messagereceiver_set_trace(amqp_management->message_receiver, trace_on);
    }
}

int amqp_management_set_override_status_code_key_name(AMQP_MANAGEMENT_HANDLE amqp_management, const char* override_status_code_key_name)
{
    int result;

    /* Codes_SRS_AMQP_MANAGEMENT_01_171: [ If `amqp_management` is NULL, `amqp_management_set_override_status_code_key_name` shall fail and return a non-zero value. ]*/
    if ((amqp_management == NULL) ||
        /* Codes_SRS_AMQP_MANAGEMENT_01_172: [ If `override_status_code_key_name` is NULL, `amqp_management_set_override_status_code_key_name` shall fail and return a non-zero value. ]*/
        (override_status_code_key_name == NULL))
    {
        LogError("Bad arguments: amqp_management = %p, override_status_code_key_name = %s",
            amqp_management, MU_P_OR_NULL(override_status_code_key_name));
        result = MU_FAILURE;
    }
    else
    {
        /* Codes_SRS_AMQP_MANAGEMENT_01_167: [ `amqp_management_set_override_status_code_key_name` shall set the status code key name used to parse the status code from the reply messages to `override_status_code_key_name`. ]*/
        /* Codes_SRS_AMQP_MANAGEMENT_01_168: [ `amqp_management_set_override_status_code_key_name` shall copy the `override_status_code_key_name` string. ]*/
        /* Codes_SRS_AMQP_MANAGEMENT_01_169: [ `amqp_management_set_override_status_code_key_name` shall free any string previously used for the status code key name. ]*/
        if (internal_set_status_code_key_name(amqp_management, override_status_code_key_name) != 0)
        {
            /* Codes_SRS_AMQP_MANAGEMENT_01_173: [ If any error occurs in copying the `override_status_code_key_name` string, `amqp_management_set_override_status_code_key_name` shall fail and return a non-zero value. ]*/
            LogError("Cannot set status code key name");
            result = MU_FAILURE;
        }
        else
        {
            /* Codes_SRS_AMQP_MANAGEMENT_01_170: [ On success, `amqp_management_set_override_status_code_key_name` shall return 0. ]*/
            result = 0;
        }
    }

    return result;
}

int amqp_management_set_override_status_description_key_name(AMQP_MANAGEMENT_HANDLE amqp_management, const char* override_status_description_key_name)
{
    int result;

    /* Codes_SRS_AMQP_MANAGEMENT_01_178: [ If `amqp_management` is NULL, `amqp_management_set_override_status_description_key_name` shall fail and return a non-zero value. ]*/
    if ((amqp_management == NULL) ||
        /* Tests_SRS_AMQP_MANAGEMENT_01_179: [ If `override_status_description_key_name` is NULL, `amqp_management_set_override_status_description_key_name` shall fail and return a non-zero value. ]*/
        (override_status_description_key_name == NULL))
    {
        LogError("Bad arguments: amqp_management = %p, override_status_description_key_name = %s",
            amqp_management, MU_P_OR_NULL(override_status_description_key_name));
        result = MU_FAILURE;
    }
    else
    {
        /* Codes_SRS_AMQP_MANAGEMENT_01_174: [ `amqp_management_set_override_status_description_key_name` shall set the status description key name used to parse the status description from the reply messages to `over ride_status_description_key_name`.]*/
        /* Codes_SRS_AMQP_MANAGEMENT_01_175: [ `amqp_management_set_override_status_description_key_name` shall copy the `override_status_description_key_name` string. ]*/
        /* Codes_SRS_AMQP_MANAGEMENT_01_176: [ `amqp_management_set_override_status_description_key_name` shall free any string previously used for the status description key name. ]*/
        /* Codes_SRS_AMQP_MANAGEMENT_01_177: [ On success, `amqp_management_set_override_status_description_key_name` shall return 0. ]*/
        if (internal_set_status_description_key_name(amqp_management, override_status_description_key_name) != 0)
        {
            /* Codes_SRS_AMQP_MANAGEMENT_01_180: [ If any error occurs in copying the `override_status_description_key_name` string, `amqp_management_set_override_status_description_key_name` shall fail and return a non-zero value. ]*/
            LogError("Cannot set status description key name");
            result = MU_FAILURE;
        }
        else
        {
            result = 0;
        }
    }

    return result;
}
