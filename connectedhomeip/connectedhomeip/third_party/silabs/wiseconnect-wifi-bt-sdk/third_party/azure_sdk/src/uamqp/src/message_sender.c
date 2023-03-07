// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>
#include "azure_macro_utils/macro_utils.h"
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/tickcounter.h"
#include "azure_uamqp_c/link.h"
#include "azure_uamqp_c/message.h"
#include "azure_uamqp_c/message_sender.h"
#include "azure_uamqp_c/amqpvalue_to_string.h"
#include "azure_uamqp_c/async_operation.h"
#include "azure_uamqp_c/amqp_definitions.h"

typedef enum MESSAGE_SEND_STATE_TAG
{
    MESSAGE_SEND_STATE_NOT_SENT,
    MESSAGE_SEND_STATE_PENDING
} MESSAGE_SEND_STATE;

typedef enum SEND_ONE_MESSAGE_RESULT_TAG
{
    SEND_ONE_MESSAGE_OK,
    SEND_ONE_MESSAGE_ERROR,
    SEND_ONE_MESSAGE_BUSY
} SEND_ONE_MESSAGE_RESULT;

typedef struct MESSAGE_WITH_CALLBACK_TAG
{
    MESSAGE_HANDLE message;
    ON_MESSAGE_SEND_COMPLETE on_message_send_complete;
    void* context;
    MESSAGE_SENDER_HANDLE message_sender;
    MESSAGE_SEND_STATE message_send_state;
    tickcounter_ms_t timeout;
} MESSAGE_WITH_CALLBACK;

DEFINE_ASYNC_OPERATION_CONTEXT(MESSAGE_WITH_CALLBACK);

typedef struct MESSAGE_SENDER_INSTANCE_TAG
{
    LINK_HANDLE link;
    size_t message_count;
    ASYNC_OPERATION_HANDLE* messages;
    MESSAGE_SENDER_STATE message_sender_state;
    ON_MESSAGE_SENDER_STATE_CHANGED on_message_sender_state_changed;
    void* on_message_sender_state_changed_context;
    unsigned int is_trace_on : 1;
} MESSAGE_SENDER_INSTANCE;

static void remove_pending_message_by_index(MESSAGE_SENDER_HANDLE message_sender, size_t index)
{
    ASYNC_OPERATION_HANDLE* new_messages;
    MESSAGE_WITH_CALLBACK* message_with_callback = GET_ASYNC_OPERATION_CONTEXT(MESSAGE_WITH_CALLBACK, message_sender->messages[index]);

    if (message_with_callback->message != NULL)
    {
        message_destroy(message_with_callback->message);
        message_with_callback->message = NULL;
    }

    async_operation_destroy(message_sender->messages[index]);

    if (message_sender->message_count - index > 1)
    {
        (void)memmove(&message_sender->messages[index], &message_sender->messages[index + 1], sizeof(ASYNC_OPERATION_HANDLE) * (message_sender->message_count - index - 1));
    }

    message_sender->message_count--;

    if (message_sender->message_count > 0)
    {
        new_messages = (ASYNC_OPERATION_HANDLE*)realloc(message_sender->messages, sizeof(ASYNC_OPERATION_HANDLE) * (message_sender->message_count));
        if (new_messages != NULL)
        {
            message_sender->messages = new_messages;
        }
    }
    else
    {
        free(message_sender->messages);
        message_sender->messages = NULL;
    }
}

static void remove_pending_message(MESSAGE_SENDER_INSTANCE* message_sender, ASYNC_OPERATION_HANDLE pending_send)
{
    size_t i;

    for (i = 0; i < message_sender->message_count; i++)
    {
        if (message_sender->messages[i] == pending_send)
        {
            remove_pending_message_by_index(message_sender, i);
            break;
        }
    }
}

static void on_delivery_settled(void* context, delivery_number delivery_no, LINK_DELIVERY_SETTLE_REASON reason, AMQP_VALUE delivery_state)
{
    ASYNC_OPERATION_HANDLE pending_send = (ASYNC_OPERATION_HANDLE)context;
    MESSAGE_WITH_CALLBACK* message_with_callback = GET_ASYNC_OPERATION_CONTEXT(MESSAGE_WITH_CALLBACK, pending_send);
    MESSAGE_SENDER_INSTANCE* message_sender = (MESSAGE_SENDER_INSTANCE*)message_with_callback->message_sender;
    (void)delivery_no;

    if (message_with_callback != NULL && 
        message_with_callback->on_message_send_complete != NULL)
    {
        switch (reason)
        {
        case LINK_DELIVERY_SETTLE_REASON_DISPOSITION_RECEIVED:
            if (delivery_state == NULL)
            {
                LogError("delivery state not provided");
            }
            else
            {
                AMQP_VALUE descriptor = amqpvalue_get_inplace_descriptor(delivery_state);
                AMQP_VALUE described = amqpvalue_get_inplace_described_value(delivery_state);

                if (descriptor == NULL)
                {
                    LogError("Error getting descriptor for delivery state");
                }
                else if (is_accepted_type_by_descriptor(descriptor))
                {
                    message_with_callback->on_message_send_complete(message_with_callback->context, MESSAGE_SEND_OK, described);
                }
                else
                {
                    message_with_callback->on_message_send_complete(message_with_callback->context, MESSAGE_SEND_ERROR, described);
                }
            }

            break;
        case LINK_DELIVERY_SETTLE_REASON_SETTLED:
            message_with_callback->on_message_send_complete(message_with_callback->context, MESSAGE_SEND_OK, NULL);
            break;
        case LINK_DELIVERY_SETTLE_REASON_TIMEOUT:
            message_with_callback->on_message_send_complete(message_with_callback->context, MESSAGE_SEND_TIMEOUT, NULL);
            break;
        case LINK_DELIVERY_SETTLE_REASON_NOT_DELIVERED:
        default:
            message_with_callback->on_message_send_complete(message_with_callback->context, MESSAGE_SEND_ERROR, NULL);
            break;
        }
    }

    remove_pending_message(message_sender, pending_send);
}

static int encode_bytes(void* context, const unsigned char* bytes, size_t length)
{
    PAYLOAD* payload = (PAYLOAD*)context;
    (void)memcpy((unsigned char*)payload->bytes + payload->length, bytes, length);
    payload->length += length;
    return 0;
}

static void log_message_chunk(MESSAGE_SENDER_INSTANCE* message_sender, const char* name, AMQP_VALUE value)
{
#ifdef NO_LOGGING
    (void)message_sender;
    (void)name;
    (void)value;
#else
    if (xlogging_get_log_function() != NULL && message_sender->is_trace_on == 1)
    {
        char* value_as_string = NULL;
        LOG(AZ_LOG_TRACE, 0, "%s", MU_P_OR_NULL(name));
        LOG(AZ_LOG_TRACE, 0, "%s", ((value_as_string = amqpvalue_to_string(value)), MU_P_OR_NULL(value_as_string)));
        if (value_as_string != NULL)
        {
            free(value_as_string);
        }
    }
#endif
}

static SEND_ONE_MESSAGE_RESULT send_one_message(MESSAGE_SENDER_INSTANCE* message_sender, ASYNC_OPERATION_HANDLE pending_send, MESSAGE_HANDLE message)
{
    SEND_ONE_MESSAGE_RESULT result;

    size_t encoded_size;
    size_t total_encoded_size = 0;
    MESSAGE_BODY_TYPE message_body_type;
    message_format message_format;

    if ((message_get_body_type(message, &message_body_type) != 0) ||
        (message_get_message_format(message, &message_format) != 0))
    {
        LogError("Failure getting message body type and/or message format");
        result = SEND_ONE_MESSAGE_ERROR;
    }
    else
    {
        // header
        HEADER_HANDLE header = NULL;
        AMQP_VALUE header_amqp_value = NULL;
        PROPERTIES_HANDLE properties = NULL;
        AMQP_VALUE properties_amqp_value = NULL;
        AMQP_VALUE application_properties = NULL;
        AMQP_VALUE application_properties_value = NULL;
        AMQP_VALUE body_amqp_value = NULL;
        size_t body_data_count = 0;
        AMQP_VALUE msg_annotations = NULL;
        bool is_error = false;

        // message header
        if ((message_get_header(message, &header) == 0) &&
            (header != NULL))
        {
            header_amqp_value = amqpvalue_create_header(header);
            if (header_amqp_value == NULL)
            {
                LogError("Cannot create header AMQP value");
                is_error = true;
            }
            else
            {
                if (amqpvalue_get_encoded_size(header_amqp_value, &encoded_size) != 0)
                {
                    LogError("Cannot obtain header encoded size");
                    is_error = true;
                }
                else
                {
                    total_encoded_size += encoded_size;
                }
            }

        }

        // message annotations
        if ((!is_error) &&
            (message_get_message_annotations(message, &msg_annotations) == 0) &&
            (msg_annotations != NULL))
        {
            if (amqpvalue_get_encoded_size(msg_annotations, &encoded_size) != 0)
            {
                LogError("Cannot obtain message annotations encoded size");
                is_error = true;
            }
            else
            {
                total_encoded_size += encoded_size;
            }
        }

        // properties
        if ((!is_error) &&
            (message_get_properties(message, &properties) == 0) &&
            (properties != NULL))
        {
            properties_amqp_value = amqpvalue_create_properties(properties);
            if (properties_amqp_value == NULL)
            {
                LogError("Cannot create message properties AMQP value");
                is_error = true;
            }
            else
            {
                if (amqpvalue_get_encoded_size(properties_amqp_value, &encoded_size) != 0)
                {
                    LogError("Cannot obtain message properties encoded size");
                    is_error = true;
                }
                else
                {
                    total_encoded_size += encoded_size;
                }
            }
        }

        // application properties
        if ((!is_error) &&
            (message_get_application_properties(message, &application_properties) == 0) &&
            (application_properties != NULL))
        {
            application_properties_value = amqpvalue_create_application_properties(application_properties);
            if (application_properties_value == NULL)
            {
                LogError("Cannot create application properties AMQP value");
                is_error = true;
            }
            else
            {
                if (amqpvalue_get_encoded_size(application_properties_value, &encoded_size) != 0)
                {
                    LogError("Cannot obtain application properties encoded size");
                    is_error = true;
                }
                else
                {
                    total_encoded_size += encoded_size;
                }
            }
        }

        if (is_error)
        {
            result = SEND_ONE_MESSAGE_ERROR;
        }
        else
        {
            result = SEND_ONE_MESSAGE_OK;

            // body - amqp data
            switch (message_body_type)
            {
            default:
                LogError("Unknown body type");
                result = SEND_ONE_MESSAGE_ERROR;
                break;

            case MESSAGE_BODY_TYPE_VALUE:
            {
                AMQP_VALUE message_body_amqp_value;
                if (message_get_body_amqp_value_in_place(message, &message_body_amqp_value) != 0)
                {
                    LogError("Cannot obtain AMQP value from body");
                    result = SEND_ONE_MESSAGE_ERROR;
                }
                else
                {
                    body_amqp_value = amqpvalue_create_amqp_value(message_body_amqp_value);
                    if (body_amqp_value == NULL)
                    {
                        LogError("Cannot create body AMQP value");
                        result = SEND_ONE_MESSAGE_ERROR;
                    }
                    else
                    {
                        if (amqpvalue_get_encoded_size(body_amqp_value, &encoded_size) != 0)
                        {
                            LogError("Cannot get body AMQP value encoded size");
                            result = SEND_ONE_MESSAGE_ERROR;
                        }
                        else
                        {
                            total_encoded_size += encoded_size;
                        }
                    }
                }

                break;
            }

            case MESSAGE_BODY_TYPE_DATA:
            {
                BINARY_DATA binary_data;
                size_t i;

                if (message_get_body_amqp_data_count(message, &body_data_count) != 0)
                {
                    LogError("Cannot get body AMQP data count");
                    result = SEND_ONE_MESSAGE_ERROR;
                }
                else
                {
                    if (body_data_count == 0)
                    {
                        LogError("Body data count is zero");
                        result = SEND_ONE_MESSAGE_ERROR;
                    }
                    else
                    {
                        for (i = 0; i < body_data_count; i++)
                        {
                            if (message_get_body_amqp_data_in_place(message, i, &binary_data) != 0)
                            {
                                LogError("Cannot get body AMQP data %u", (unsigned int)i);
                                result = SEND_ONE_MESSAGE_ERROR;
                            }
                            else
                            {
                                AMQP_VALUE body_amqp_data;
                                amqp_binary binary_value;
                                binary_value.bytes = binary_data.bytes;
                                binary_value.length = (uint32_t)binary_data.length;
                                body_amqp_data = amqpvalue_create_data(binary_value);
                                if (body_amqp_data == NULL)
                                {
                                    LogError("Cannot create body AMQP data");
                                    result = SEND_ONE_MESSAGE_ERROR;
                                }
                                else
                                {
                                    if (amqpvalue_get_encoded_size(body_amqp_data, &encoded_size) != 0)
                                    {
                                        LogError("Cannot get body AMQP data encoded size");
                                        result = SEND_ONE_MESSAGE_ERROR;
                                    }
                                    else
                                    {
                                        total_encoded_size += encoded_size;
                                    }

                                    amqpvalue_destroy(body_amqp_data);
                                }
                            }
                        }
                    }
                }
                break;
            }
            }

            if (result == 0)
            {
                void* data_bytes = malloc(total_encoded_size);
                PAYLOAD payload;
                payload.bytes = (const unsigned char*)data_bytes;
                payload.length = 0;
                result = SEND_ONE_MESSAGE_OK;

                if (header != NULL)
                {
                    if (amqpvalue_encode(header_amqp_value, encode_bytes, &payload) != 0)
                    {
                        LogError("Cannot encode header value");
                        result = SEND_ONE_MESSAGE_ERROR;
                    }

                    log_message_chunk(message_sender, "Header:", header_amqp_value);
                }

                if ((result == SEND_ONE_MESSAGE_OK) && (msg_annotations != NULL))
                {
                    if (amqpvalue_encode(msg_annotations, encode_bytes, &payload) != 0)
                    {
                        LogError("Cannot encode message annotations value");
                        result = SEND_ONE_MESSAGE_ERROR;
                    }

                    log_message_chunk(message_sender, "Message Annotations:", msg_annotations);
                }

                if ((result == SEND_ONE_MESSAGE_OK) && (properties != NULL))
                {
                    if (amqpvalue_encode(properties_amqp_value, encode_bytes, &payload) != 0)
                    {
                        LogError("Cannot encode message properties value");
                        result = SEND_ONE_MESSAGE_ERROR;
                    }

                    log_message_chunk(message_sender, "Properties:", properties_amqp_value);
                }

                if ((result == SEND_ONE_MESSAGE_OK) && (application_properties != NULL))
                {
                    if (amqpvalue_encode(application_properties_value, encode_bytes, &payload) != 0)
                    {
                        LogError("Cannot encode application properties value");
                        result = SEND_ONE_MESSAGE_ERROR;
                    }

                    log_message_chunk(message_sender, "Application properties:", application_properties_value);
                }

                if (result == SEND_ONE_MESSAGE_OK)
                {
                    switch (message_body_type)
                    {
                    default:
                        LogError("Unknown message type");
                        result = SEND_ONE_MESSAGE_ERROR;
                        break;

                    case MESSAGE_BODY_TYPE_VALUE:
                    {
                        if (amqpvalue_encode(body_amqp_value, encode_bytes, &payload) != 0)
                        {
                            LogError("Cannot encode body AMQP value");
                            result = SEND_ONE_MESSAGE_ERROR;
                        }

                        log_message_chunk(message_sender, "Body - amqp value:", body_amqp_value);
                        break;
                    }
                    case MESSAGE_BODY_TYPE_DATA:
                    {
                        BINARY_DATA binary_data;
                        size_t i;

                        for (i = 0; i < body_data_count; i++)
                        {
                            if (message_get_body_amqp_data_in_place(message, i, &binary_data) != 0)
                            {
                                LogError("Cannot get AMQP data %u", (unsigned int)i);
                                result = SEND_ONE_MESSAGE_ERROR;
                            }
                            else
                            {
                                AMQP_VALUE body_amqp_data;
                                amqp_binary binary_value;
                                binary_value.bytes = binary_data.bytes;
                                binary_value.length = (uint32_t)binary_data.length;
                                body_amqp_data = amqpvalue_create_data(binary_value);
                                if (body_amqp_data == NULL)
                                {
                                    LogError("Cannot create body AMQP data %u", (unsigned int)i);
                                    result = SEND_ONE_MESSAGE_ERROR;
                                }
                                else
                                {
                                    if (amqpvalue_encode(body_amqp_data, encode_bytes, &payload) != 0)
                                    {
                                        LogError("Cannot encode body AMQP data %u", (unsigned int)i);
                                        result = SEND_ONE_MESSAGE_ERROR;
                                        break;
                                    }

                                    amqpvalue_destroy(body_amqp_data);
                                }
                            }
                        }
                        break;
                    }
                    }
                }

                if (result == SEND_ONE_MESSAGE_OK)
                {
                    ASYNC_OPERATION_HANDLE transfer_async_operation;
                    LINK_TRANSFER_RESULT link_transfer_error;
                    MESSAGE_WITH_CALLBACK* message_with_callback = GET_ASYNC_OPERATION_CONTEXT(MESSAGE_WITH_CALLBACK, pending_send);
                    message_with_callback->message_send_state = MESSAGE_SEND_STATE_PENDING;

                    transfer_async_operation = link_transfer_async(message_sender->link, message_format, &payload, 1, on_delivery_settled, pending_send, &link_transfer_error, message_with_callback->timeout);
                    if (transfer_async_operation == NULL)
                    {
                        if (link_transfer_error == LINK_TRANSFER_BUSY)
                        {
                            message_with_callback->message_send_state = MESSAGE_SEND_STATE_NOT_SENT;
                            result = SEND_ONE_MESSAGE_BUSY;
                        }
                        else
                        {
                            LogError("Error in link transfer");
                            result = SEND_ONE_MESSAGE_ERROR;
                        }
                    }
                    else
                    {
                        result = SEND_ONE_MESSAGE_OK;
                    }
                }

                free(data_bytes);

                if (body_amqp_value != NULL)
                {
                    amqpvalue_destroy(body_amqp_value);
                }
            }
        }

        if (header != NULL)
        {
            header_destroy(header);
        }

        if (header_amqp_value != NULL)
        {
            amqpvalue_destroy(header_amqp_value);
        }

        if (msg_annotations != NULL)
        {
            annotations_destroy(msg_annotations);
        }

        if (application_properties != NULL)
        {
            amqpvalue_destroy(application_properties);
        }

        if (application_properties_value != NULL)
        {
            amqpvalue_destroy(application_properties_value);
        }

        if (properties_amqp_value != NULL)
        {
            amqpvalue_destroy(properties_amqp_value);
        }

        if (properties != NULL)
        {
            properties_destroy(properties);
        }
    }

    return result;
}

static void send_all_pending_messages(MESSAGE_SENDER_HANDLE message_sender)
{
    size_t i;

    for (i = 0; i < message_sender->message_count; i++)
    {
        MESSAGE_WITH_CALLBACK* message_with_callback = GET_ASYNC_OPERATION_CONTEXT(MESSAGE_WITH_CALLBACK, message_sender->messages[i]);
        if (message_with_callback->message_send_state == MESSAGE_SEND_STATE_NOT_SENT)
        {
            switch (send_one_message(message_sender, message_sender->messages[i], message_with_callback->message))
            {
            default:
                LogError("Invalid send one message result");
                break;

            case SEND_ONE_MESSAGE_ERROR:
            {
                ON_MESSAGE_SEND_COMPLETE on_message_send_complete = message_with_callback->on_message_send_complete;
                void* context = message_with_callback->context;
                remove_pending_message_by_index(message_sender, i);

                if (on_message_send_complete != NULL)
                {
                    on_message_send_complete(context, MESSAGE_SEND_ERROR, NULL);
                }

                i = message_sender->message_count;
                break;
            }
            case SEND_ONE_MESSAGE_BUSY:
                i = message_sender->message_count + 1;
                break;

            case SEND_ONE_MESSAGE_OK:
                break;
            }

            i--;
        }
    }
}

static void set_message_sender_state(MESSAGE_SENDER_INSTANCE* message_sender, MESSAGE_SENDER_STATE new_state)
{
    MESSAGE_SENDER_STATE previous_state = message_sender->message_sender_state;
    message_sender->message_sender_state = new_state;
    if (message_sender->on_message_sender_state_changed != NULL)
    {
        message_sender->on_message_sender_state_changed(message_sender->on_message_sender_state_changed_context, new_state, previous_state);
    }
}

static void indicate_all_messages_as_error(MESSAGE_SENDER_INSTANCE* message_sender)
{
    size_t i;

    for (i = 0; i < message_sender->message_count; i++)
    {
        MESSAGE_WITH_CALLBACK* message_with_callback = GET_ASYNC_OPERATION_CONTEXT(MESSAGE_WITH_CALLBACK, message_sender->messages[i]);
        if (message_with_callback->on_message_send_complete != NULL)
        {
            message_with_callback->on_message_send_complete(message_with_callback->context, MESSAGE_SEND_ERROR, NULL);
        }

        if (message_with_callback->message != NULL)
        {
            message_destroy(message_with_callback->message);
        }
        async_operation_destroy(message_sender->messages[i]);
    }

    if (message_sender->messages != NULL)
    {
        message_sender->message_count = 0;

        free(message_sender->messages);
        message_sender->messages = NULL;
    }
}

static void on_link_state_changed(void* context, LINK_STATE new_link_state, LINK_STATE previous_link_state)
{
    MESSAGE_SENDER_INSTANCE* message_sender = (MESSAGE_SENDER_INSTANCE*)context;
    (void)previous_link_state;

    switch (new_link_state)
    {
    default:
        break;

    case LINK_STATE_ATTACHED:
        if (message_sender->message_sender_state == MESSAGE_SENDER_STATE_OPENING)
        {
            set_message_sender_state(message_sender, MESSAGE_SENDER_STATE_OPEN);
        }
        break;
    case LINK_STATE_DETACHED:
        if ((message_sender->message_sender_state == MESSAGE_SENDER_STATE_OPEN) ||
            (message_sender->message_sender_state == MESSAGE_SENDER_STATE_CLOSING))
        {
            /* switch to closing so that no more requests should be accepted */
            indicate_all_messages_as_error(message_sender);
            set_message_sender_state(message_sender, MESSAGE_SENDER_STATE_IDLE);
        }
        else if (message_sender->message_sender_state != MESSAGE_SENDER_STATE_IDLE)
        {
            /* Any other transition must be an error */
            set_message_sender_state(message_sender, MESSAGE_SENDER_STATE_ERROR);
        }
        break;
    case LINK_STATE_ERROR:
        if (message_sender->message_sender_state != MESSAGE_SENDER_STATE_ERROR)
        {
            indicate_all_messages_as_error(message_sender);
            set_message_sender_state(message_sender, MESSAGE_SENDER_STATE_ERROR);
        }
        break;
    }
}

static void on_link_flow_on(void* context)
{
    MESSAGE_SENDER_HANDLE message_sender = (MESSAGE_SENDER_INSTANCE*)context;
    send_all_pending_messages(message_sender);
}

MESSAGE_SENDER_HANDLE messagesender_create(LINK_HANDLE link, ON_MESSAGE_SENDER_STATE_CHANGED on_message_sender_state_changed, void* context)
{
    MESSAGE_SENDER_INSTANCE* message_sender = (MESSAGE_SENDER_INSTANCE*)calloc(1, sizeof(MESSAGE_SENDER_INSTANCE));
    if (message_sender == NULL)
    {
        LogError("Failed allocating message sender");
    }
    else
    {
        message_sender->messages = NULL;
        message_sender->message_count = 0;
        message_sender->link = link;
        message_sender->on_message_sender_state_changed = on_message_sender_state_changed;
        message_sender->on_message_sender_state_changed_context = context;
        message_sender->message_sender_state = MESSAGE_SENDER_STATE_IDLE;
        message_sender->is_trace_on = 0;
    }

    return message_sender;
}

void messagesender_destroy(MESSAGE_SENDER_HANDLE message_sender)
{
    if (message_sender == NULL)
    {
        LogError("NULL message_sender");
    }
    else
    {
        (void)messagesender_close(message_sender);

        free(message_sender);
    }
}

int messagesender_open(MESSAGE_SENDER_HANDLE message_sender)
{
    int result;

    if (message_sender == NULL)
    {
        LogError("NULL message_sender");
        result = MU_FAILURE;
    }
    else
    {
        if (message_sender->message_sender_state == MESSAGE_SENDER_STATE_IDLE)
        {
            set_message_sender_state(message_sender, MESSAGE_SENDER_STATE_OPENING);
            if (link_attach(message_sender->link, NULL, on_link_state_changed, on_link_flow_on, message_sender) != 0)
            {
                LogError("attach link failed");
                result = MU_FAILURE;
                set_message_sender_state(message_sender, MESSAGE_SENDER_STATE_ERROR);
            }
            else
            {
                result = 0;
            }
        }
        else
        {
            result = 0;
        }
    }

    return result;
}

int messagesender_close(MESSAGE_SENDER_HANDLE message_sender)
{
    int result;

    if (message_sender == NULL)
    {
        LogError("NULL message_sender");
        result = MU_FAILURE;
    }
    else
    {
        indicate_all_messages_as_error(message_sender);

        if ((message_sender->message_sender_state == MESSAGE_SENDER_STATE_OPENING) ||
            (message_sender->message_sender_state == MESSAGE_SENDER_STATE_OPEN))
        {
            set_message_sender_state(message_sender, MESSAGE_SENDER_STATE_CLOSING);
            if (link_detach(message_sender->link, true, NULL, NULL, NULL) != 0)
            {
                LogError("Detaching link failed");
                result = MU_FAILURE;
                set_message_sender_state(message_sender, MESSAGE_SENDER_STATE_ERROR);
            }
            else
            {
                result = 0;
            }
        }
        else
        {
            result = 0;
        }
    }

    return result;
}

static void messagesender_send_cancel_handler(ASYNC_OPERATION_HANDLE send_operation)
{
    MESSAGE_WITH_CALLBACK* message_with_callback = GET_ASYNC_OPERATION_CONTEXT(MESSAGE_WITH_CALLBACK, send_operation);
    if (message_with_callback->on_message_send_complete != NULL)
    {
        message_with_callback->on_message_send_complete(message_with_callback->context, MESSAGE_SEND_CANCELLED, NULL);
    }

    remove_pending_message(message_with_callback->message_sender, send_operation);
}

ASYNC_OPERATION_HANDLE messagesender_send_async(MESSAGE_SENDER_HANDLE message_sender, MESSAGE_HANDLE message, ON_MESSAGE_SEND_COMPLETE on_message_send_complete, void* callback_context, tickcounter_ms_t timeout)
{
    ASYNC_OPERATION_HANDLE result;

    if ((message_sender == NULL) ||
        (message == NULL))
    {
        LogError("Bad parameters: message_sender=%p, message=%p, on_message_send_complete=%p, callback_context=%p, timeout=%" PRIu64, message_sender, message, on_message_send_complete, callback_context, (uint64_t)timeout);
        result = NULL;
    }
    else
    {
        if (message_sender->message_sender_state == MESSAGE_SENDER_STATE_ERROR)
        {
            LogError("Message sender in ERROR state");
            result = NULL;
        }
        else
        {
            result = CREATE_ASYNC_OPERATION(MESSAGE_WITH_CALLBACK, messagesender_send_cancel_handler);
            if (result == NULL)
            {
                LogError("Failed allocating context for send");
            }
            else
            {
                MESSAGE_WITH_CALLBACK* message_with_callback = GET_ASYNC_OPERATION_CONTEXT(MESSAGE_WITH_CALLBACK, result);
                ASYNC_OPERATION_HANDLE* new_messages = (ASYNC_OPERATION_HANDLE*)realloc(message_sender->messages, sizeof(ASYNC_OPERATION_HANDLE) * (message_sender->message_count + 1));
                if (new_messages == NULL)
                {
                    LogError("Failed allocating memory for pending sends");
                    async_operation_destroy(result);
                    result = NULL;
                }
                else
                {
                    message_with_callback->timeout = timeout;
                    message_sender->messages = new_messages;
                    if (message_sender->message_sender_state != MESSAGE_SENDER_STATE_OPEN)
                    {
                        message_with_callback->message = message_clone(message);
                        if (message_with_callback->message == NULL)
                        {
                            LogError("Cannot clone message for placing it in the pending sends list");
                            async_operation_destroy(result);
                            result = NULL;
                        }

                        message_with_callback->message_send_state = MESSAGE_SEND_STATE_NOT_SENT;
                    }
                    else
                    {
                        message_with_callback->message = NULL;
                        message_with_callback->message_send_state = MESSAGE_SEND_STATE_PENDING;
                    }

                    if (result != NULL)
                    {
                        message_with_callback->on_message_send_complete = on_message_send_complete;
                        message_with_callback->context = callback_context;
                        message_with_callback->message_sender = message_sender;

                        message_sender->messages[message_sender->message_count] = result;
                        message_sender->message_count++;

                        if (message_sender->message_sender_state == MESSAGE_SENDER_STATE_OPEN)
                        {
                            switch (send_one_message(message_sender, result, message))
                            {
                            default:
                            case SEND_ONE_MESSAGE_ERROR:
                                LogError("Error sending message");
                                remove_pending_message_by_index(message_sender, message_sender->message_count - 1);
                                result = NULL;
                                break;

                            case SEND_ONE_MESSAGE_BUSY:
                                message_with_callback->message = message_clone(message);
                                if (message_with_callback->message == NULL)
                                {
                                    LogError("Error cloning message for placing it in the pending sends list");
                                    async_operation_destroy(result);
                                    result = NULL;
                                }
                                else
                                {
                                    message_with_callback->message_send_state = MESSAGE_SEND_STATE_NOT_SENT;
                                }
                                break;

                            case SEND_ONE_MESSAGE_OK:
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    return result;
}

void messagesender_set_trace(MESSAGE_SENDER_HANDLE message_sender, bool traceOn)
{
    if (message_sender == NULL)
    {
        LogError("NULL message_sender");
    }
    else
    {
        message_sender->is_trace_on = traceOn ? 1 : 0;
    }
}
