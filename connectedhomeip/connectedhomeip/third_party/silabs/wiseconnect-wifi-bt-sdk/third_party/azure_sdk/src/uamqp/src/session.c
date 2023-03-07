// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <string.h>
#include "azure_macro_utils/macro_utils.h"
#include "azure_c_shared_utility/gballoc.h"
#include "azure_uamqp_c/session.h"
#include "azure_uamqp_c/connection.h"
#include "azure_uamqp_c/amqp_definitions.h"

typedef enum LINK_ENDPOINT_STATE_TAG
{
    LINK_ENDPOINT_STATE_NOT_ATTACHED,
    LINK_ENDPOINT_STATE_ATTACHED,
    LINK_ENDPOINT_STATE_DETACHING
} LINK_ENDPOINT_STATE;

typedef struct LINK_ENDPOINT_INSTANCE_TAG
{
    char* name;
    handle input_handle;
    handle output_handle;
    ON_ENDPOINT_FRAME_RECEIVED frame_received_callback;
    ON_SESSION_STATE_CHANGED on_session_state_changed;
    ON_SESSION_FLOW_ON on_session_flow_on;
    void* callback_context;
    SESSION_HANDLE session;
    LINK_ENDPOINT_STATE link_endpoint_state;
    ON_LINK_ENDPOINT_DESTROYED_CALLBACK on_link_endpoint_destroyed_callback;
    void* on_link_endpoint_destroyed_context;
} LINK_ENDPOINT_INSTANCE;

typedef struct SESSION_INSTANCE_TAG
{
    ON_ENDPOINT_FRAME_RECEIVED frame_received_callback;
    void* frame_received_callback_context;
    SESSION_STATE session_state;
    SESSION_STATE previous_session_state;
    CONNECTION_HANDLE connection;
    ENDPOINT_HANDLE endpoint;
    LINK_ENDPOINT_INSTANCE** link_endpoints;
    uint32_t link_endpoint_count;

    ON_LINK_ATTACHED on_link_attached;
    void* on_link_attached_callback_context;

    /* Codes_S_R_S_SESSION_01_016: [next-outgoing-id The next-outgoing-id is the transfer-id to assign to the next transfer frame.] */
    transfer_number next_outgoing_id;
    transfer_number next_incoming_id;
    uint32_t desired_incoming_window;
    uint32_t incoming_window;
    uint32_t outgoing_window;
    handle handle_max;
    uint32_t remote_incoming_window;
    uint32_t remote_outgoing_window;
    unsigned int is_underlying_connection_open : 1;
} SESSION_INSTANCE;

#define UNDERLYING_CONNECTION_NOT_OPEN 0
#define UNDERLYING_CONNECTION_OPEN 1

static void remove_link_endpoint(LINK_ENDPOINT_HANDLE link_endpoint)
{
    if (link_endpoint != NULL)
    {
        LINK_ENDPOINT_INSTANCE* endpoint_instance = (LINK_ENDPOINT_INSTANCE*)link_endpoint;
        SESSION_INSTANCE* session_instance = endpoint_instance->session;
        uint32_t i;

        for (i = 0; i < session_instance->link_endpoint_count; i++)
        {
            if (session_instance->link_endpoints[i] == link_endpoint)
            {
                break;
            }
        }

        if (i < session_instance->link_endpoint_count)
        {
            LINK_ENDPOINT_INSTANCE** new_endpoints;

            if (i < (session_instance->link_endpoint_count - 1))
            {
                (void)memmove(&session_instance->link_endpoints[i], &session_instance->link_endpoints[i + 1], (session_instance->link_endpoint_count - (uint32_t)i - 1) * sizeof(LINK_ENDPOINT_INSTANCE*));
            }

            session_instance->link_endpoint_count--;

            if (session_instance->link_endpoint_count == 0)
            {
                free(session_instance->link_endpoints);
                session_instance->link_endpoints = NULL;
            }
            else
            {
                new_endpoints = (LINK_ENDPOINT_INSTANCE**)realloc(session_instance->link_endpoints, sizeof(LINK_ENDPOINT_INSTANCE*) * session_instance->link_endpoint_count);
                if (new_endpoints != NULL)
                {
                    session_instance->link_endpoints = new_endpoints;
                }
            }
        }
    }
}

static void free_link_endpoint(LINK_ENDPOINT_HANDLE link_endpoint)
{
    // The link endpoint handle can be managed by both uamqp and the upper layer.
    // uamqp may destroy a link endpoint if a DETACH is received from the remote endpoint,
    // so in this case the upper layer must be notified so it does not attempt to destroy the link endpoint as well.
    // Ref counting would not suffice to address this situation as uamqp does not destroy link endpoints by itself 
    // every time, only when receiving a DETACH.
    if (link_endpoint->on_link_endpoint_destroyed_callback != NULL)
    {
        link_endpoint->on_link_endpoint_destroyed_callback(link_endpoint, link_endpoint->on_link_endpoint_destroyed_context);
    }

    if (link_endpoint->name != NULL)
    {
        free(link_endpoint->name);
    }

    free(link_endpoint);
}

static void session_set_state(SESSION_INSTANCE* session_instance, SESSION_STATE session_state)
{
    uint32_t i;

    session_instance->previous_session_state = session_instance->session_state;
    session_instance->session_state = session_state;

    for (i = 0; i < session_instance->link_endpoint_count; i++)
    {
        if (session_instance->link_endpoints[i]->on_session_state_changed != NULL)
        {
            if (session_instance->link_endpoints[i]->link_endpoint_state != LINK_ENDPOINT_STATE_DETACHING)
            {
                session_instance->link_endpoints[i]->on_session_state_changed(session_instance->link_endpoints[i]->callback_context, session_state, session_instance->previous_session_state);
            }
        }
    }
}

static int send_end_frame(SESSION_INSTANCE* session_instance, ERROR_HANDLE error_handle)
{
    int result;
    END_HANDLE end_performative;

    end_performative = end_create();
    if (end_performative == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        if ((error_handle != NULL) &&
            (end_set_error(end_performative, error_handle) != 0))
        {
            result = MU_FAILURE;
        }
        else
        {
            AMQP_VALUE end_performative_value = amqpvalue_create_end(end_performative);
            if (end_performative_value == NULL)
            {
                result = MU_FAILURE;
            }
            else
            {
                if (connection_encode_frame(session_instance->endpoint, end_performative_value, NULL, 0, NULL, NULL) != 0)
                {
                    result = MU_FAILURE;
                }
                else
                {
                    result = 0;
                }

                amqpvalue_destroy(end_performative_value);
            }
        }

        end_destroy(end_performative);
    }

    return result;
}

static void end_session_with_error(SESSION_INSTANCE* session_instance, const char* condition_value, const char* description)
{
    ERROR_HANDLE error_handle = error_create(condition_value);
    if (error_handle == NULL)
    {
        /* fatal error */
        session_set_state(session_instance, SESSION_STATE_DISCARDING);
        (void)connection_close(session_instance->connection, "amqp:internal-error", "Cannot allocate error handle to end session", NULL);
    }
    else
    {
        if ((error_set_description(error_handle, description) != 0) ||
            (send_end_frame(session_instance, error_handle) != 0))
        {
            /* fatal error */
            session_set_state(session_instance, SESSION_STATE_DISCARDING);
            (void)connection_close(session_instance->connection, "amqp:internal-error", "Cannot allocate error handle to end session", NULL);
        }
        else
        {
            session_set_state(session_instance, SESSION_STATE_DISCARDING);
        }

        error_destroy(error_handle);
    }
}

static int send_begin(SESSION_INSTANCE* session_instance)
{
    int result;
    BEGIN_HANDLE begin = begin_create(session_instance->next_outgoing_id, session_instance->incoming_window, session_instance->outgoing_window);

    if (begin == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint16_t remote_channel;
        if (begin_set_handle_max(begin, session_instance->handle_max) != 0)
        {
            result = MU_FAILURE;
        }
        else if ((session_instance->session_state == SESSION_STATE_BEGIN_RCVD) &&
            ((connection_endpoint_get_incoming_channel(session_instance->endpoint, &remote_channel) != 0) ||
            (begin_set_remote_channel(begin, remote_channel) != 0)))
        {
            result = MU_FAILURE;
        }
        else
        {
            AMQP_VALUE begin_performative_value = amqpvalue_create_begin(begin);
            if (begin_performative_value == NULL)
            {
                result = MU_FAILURE;
            }
            else
            {
                if (connection_encode_frame(session_instance->endpoint, begin_performative_value, NULL, 0, NULL, NULL) != 0)
                {
                    result = MU_FAILURE;
                }
                else
                {
                    result = 0;
                }

                amqpvalue_destroy(begin_performative_value);
            }
        }

        begin_destroy(begin);
    }

    return result;
}

static int send_flow(SESSION_INSTANCE* session)
{
    int result;
    if (session == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        FLOW_HANDLE flow = flow_create(session->incoming_window, session->next_outgoing_id, session->outgoing_window);

        if (flow == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (flow_set_next_incoming_id(flow, session->next_incoming_id) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE flow_performative_value = amqpvalue_create_flow(flow);
                if (flow_performative_value == NULL)
                {
                    result = MU_FAILURE;
                }
                else
                {
                    if (connection_encode_frame(session->endpoint, flow_performative_value, NULL, 0, NULL, NULL) != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }

                    amqpvalue_destroy(flow_performative_value);
                }
            }

            flow_destroy(flow);
        }
    }

    return result;
}

static LINK_ENDPOINT_INSTANCE* find_link_endpoint_by_name(SESSION_INSTANCE* session, const char* name)
{
    uint32_t i;
    LINK_ENDPOINT_INSTANCE* result;

    for (i = 0; i < session->link_endpoint_count; i++)
    {
        if (strcmp(session->link_endpoints[i]->name, name) == 0)
        {
            break;
        }
    }

    if (i == session->link_endpoint_count)
    {
        result = NULL;
    }
    else
    {
        result = session->link_endpoints[i];
    }

    return result;
}

static LINK_ENDPOINT_INSTANCE* find_link_endpoint_by_input_handle(SESSION_INSTANCE* session, handle input_handle)
{
    uint32_t i;
    LINK_ENDPOINT_INSTANCE* result;

    for (i = 0; i < session->link_endpoint_count; i++)
    {
        if (session->link_endpoints[i]->input_handle == input_handle)
        {
            break;
        }
    }

    if (i == session->link_endpoint_count)
    {
        result = NULL;
    }
    else
    {
        result = session->link_endpoints[i];
    }

    return result;
}

static void on_connection_state_changed(void* context, CONNECTION_STATE new_connection_state, CONNECTION_STATE previous_connection_state)
{
    SESSION_INSTANCE* session_instance = (SESSION_INSTANCE*)context;

    /* Codes_S_R_S_SESSION_01_060: [If the previous connection state is not OPENED and the new connection state is OPENED, the BEGIN frame shall be sent out and the state shall be switched to BEGIN_SENT.] */
    if ((new_connection_state == CONNECTION_STATE_OPENED) && (previous_connection_state != CONNECTION_STATE_OPENED) && (session_instance->session_state == SESSION_STATE_UNMAPPED))
    {
        if (send_begin(session_instance) == 0)
        {
            session_set_state(session_instance, SESSION_STATE_BEGIN_SENT);
        }
    }
    /* Codes_S_R_S_SESSION_01_061: [If the previous connection state is OPENED and the new connection state is not OPENED anymore, the state shall be switched to DISCARDING.] */
    else if ((new_connection_state == CONNECTION_STATE_CLOSE_RCVD) || (new_connection_state == CONNECTION_STATE_END))
    {
        session_set_state(session_instance, SESSION_STATE_DISCARDING);
    }
    /* Codes_S_R_S_SESSION_09_001: [If the new connection state is ERROR, the state shall be switched to ERROR.] */
    else if (new_connection_state == CONNECTION_STATE_ERROR)
    {
        session_set_state(session_instance, SESSION_STATE_ERROR);
    }
}

static void on_frame_received(void* context, AMQP_VALUE performative, uint32_t payload_size, const unsigned char* payload_bytes)
{
    SESSION_INSTANCE* session_instance = (SESSION_INSTANCE*)context;
    AMQP_VALUE descriptor = amqpvalue_get_inplace_descriptor(performative);

    if (is_begin_type_by_descriptor(descriptor))
    {
        BEGIN_HANDLE begin_handle;

        if (amqpvalue_get_begin(performative, &begin_handle) != 0)
        {
            connection_close(session_instance->connection, "amqp:decode-error", "Cannot decode BEGIN frame", NULL);
        }
        else
        {
            if ((begin_get_incoming_window(begin_handle, &session_instance->remote_incoming_window) != 0) ||
                (begin_get_next_outgoing_id(begin_handle, &session_instance->next_incoming_id) != 0))
            {
                /* error */
                begin_destroy(begin_handle);
                session_set_state(session_instance, SESSION_STATE_DISCARDING);
                connection_close(session_instance->connection, "amqp:decode-error", "Cannot get incoming windows and next outgoing id", NULL);
            }
            else
            {
                begin_destroy(begin_handle);

                if (session_instance->session_state == SESSION_STATE_BEGIN_SENT)
                {
                    session_set_state(session_instance, SESSION_STATE_MAPPED);
                }
                else if(session_instance->session_state == SESSION_STATE_UNMAPPED)
                {
                    session_set_state(session_instance, SESSION_STATE_BEGIN_RCVD);
                    if (send_begin(session_instance) != 0)
                    {
                        connection_close(session_instance->connection, "amqp:internal-error", "Failed sending BEGIN frame", NULL);
                        session_set_state(session_instance, SESSION_STATE_DISCARDING);
                    }
                    else
                    {
                        session_set_state(session_instance, SESSION_STATE_MAPPED);
                    }
                }
            }
        }
    }
    else if (is_attach_type_by_descriptor(descriptor))
    {
        const char* name = NULL;
        ATTACH_HANDLE attach_handle;

        if (amqpvalue_get_attach(performative, &attach_handle) != 0)
        {
            end_session_with_error(session_instance, "amqp:decode-error", "Cannot decode ATTACH frame");
        }
        else
        {
            role role;
            AMQP_VALUE source;
            AMQP_VALUE target;
            fields properties;

            if (attach_get_name(attach_handle, &name) != 0)
            {
                end_session_with_error(session_instance, "amqp:decode-error", "Cannot get link name from ATTACH frame");
            }
            else if (attach_get_role(attach_handle, &role) != 0)
            {
                end_session_with_error(session_instance, "amqp:decode-error", "Cannot get link role from ATTACH frame");
            }
            else
            {
                LINK_ENDPOINT_INSTANCE* link_endpoint;

                if (attach_get_source(attach_handle, &source) != 0)
                {
                    source = NULL;
                }
                if (attach_get_target(attach_handle, &target) != 0)
                {
                    target = NULL;
                }
                if (attach_get_properties(attach_handle, &properties) != 0)
                {
                    properties = NULL;
                }

                link_endpoint = find_link_endpoint_by_name(session_instance, name);
                if (link_endpoint == NULL)
                {
                    /* new link attach */
                    if (session_instance->on_link_attached != NULL)
                    {
                        LINK_ENDPOINT_HANDLE new_link_endpoint = session_create_link_endpoint(session_instance, name);
                        if (new_link_endpoint == NULL)
                        {
                            end_session_with_error(session_instance, "amqp:internal-error", "Cannot create link endpoint");
                        }
                        else if (attach_get_handle(attach_handle, &new_link_endpoint->input_handle) != 0)
                        {
                            end_session_with_error(session_instance, "amqp:decode-error", "Cannot get input handle from ATTACH frame");
                        }
                        else
                        {
                            new_link_endpoint->link_endpoint_state = LINK_ENDPOINT_STATE_ATTACHED;

                            if (!session_instance->on_link_attached(session_instance->on_link_attached_callback_context, new_link_endpoint, name, role, source, target, properties))
                            {
                                remove_link_endpoint(new_link_endpoint);
                                free_link_endpoint(new_link_endpoint);
                                new_link_endpoint = NULL;
                            }
                            else
                            {
                                if (new_link_endpoint->frame_received_callback != NULL)
                                {
                                    new_link_endpoint->frame_received_callback(new_link_endpoint->callback_context, performative, payload_size, payload_bytes);
                                }
                            }
                        }
                    }
                }
                else
                {
                    if (attach_get_handle(attach_handle, &link_endpoint->input_handle) != 0)
                    {
                        end_session_with_error(session_instance, "amqp:decode-error", "Cannot get input handle from ATTACH frame");
                    }
                    else
                    {
                        link_endpoint->link_endpoint_state = LINK_ENDPOINT_STATE_ATTACHED;

                        if (session_instance->on_link_attached != NULL)
                        {
                            if (!session_instance->on_link_attached(session_instance->on_link_attached_callback_context, link_endpoint, name, role, source, target, properties))
                            {
                                link_endpoint->link_endpoint_state = LINK_ENDPOINT_STATE_DETACHING;
                            }
                        }

                        link_endpoint->frame_received_callback(link_endpoint->callback_context, performative, payload_size, payload_bytes);
                    }
                }
            }

            attach_destroy(attach_handle);
        }
    }
    else if (is_detach_type_by_descriptor(descriptor))
    {
        DETACH_HANDLE detach_handle;

        if (amqpvalue_get_detach(performative, &detach_handle) != 0)
        {
            end_session_with_error(session_instance, "amqp:decode-error", "Cannot decode DETACH frame");
        }
        else
        {
            uint32_t remote_handle;
            if (detach_get_handle(detach_handle, &remote_handle) != 0)
            {
                end_session_with_error(session_instance, "amqp:decode-error", "Cannot get handle from DETACH frame");

                detach_destroy(detach_handle);
            }
            else
            {
                LINK_ENDPOINT_INSTANCE* link_endpoint;
                detach_destroy(detach_handle);

                link_endpoint = find_link_endpoint_by_input_handle(session_instance, remote_handle);
                if (link_endpoint == NULL)
                {
                    end_session_with_error(session_instance, "amqp:session:unattached-handle", "");
                }
                else
                {
                    if (link_endpoint->link_endpoint_state != LINK_ENDPOINT_STATE_DETACHING)
                    {
                        link_endpoint->link_endpoint_state = LINK_ENDPOINT_STATE_DETACHING;
                        link_endpoint->frame_received_callback(link_endpoint->callback_context, performative, payload_size, payload_bytes);
                    }
                    else
                    {
                        /* remove the link endpoint */
                        remove_link_endpoint(link_endpoint);
                        free_link_endpoint(link_endpoint);
                    }
                }
            }
        }
    }
    else if (is_flow_type_by_descriptor(descriptor))
    {
        FLOW_HANDLE flow_handle;

        if (amqpvalue_get_flow(performative, &flow_handle) != 0)
        {
            end_session_with_error(session_instance, "amqp:decode-error", "Cannot decode FLOW frame");
        }
        else
        {
            uint32_t remote_handle;
            transfer_number flow_next_incoming_id;
            uint32_t flow_incoming_window;

            if (flow_get_next_incoming_id(flow_handle, &flow_next_incoming_id) != 0)
            {
                /*
                If the next-incoming-id field of the flow frame is not set,
                then remote-incomingwindow is computed as follows:
                initial-outgoing-id(endpoint) + incoming-window(flow) - next-outgoing-id(endpoint)
                */
                flow_next_incoming_id = session_instance->next_outgoing_id;
            }

            if ((flow_get_next_outgoing_id(flow_handle, &session_instance->next_incoming_id) != 0) ||
                (flow_get_incoming_window(flow_handle, &flow_incoming_window) != 0))
            {
                flow_destroy(flow_handle);

                end_session_with_error(session_instance, "amqp:decode-error", "Cannot decode FLOW frame");
            }
            else
            {
                LINK_ENDPOINT_INSTANCE* link_endpoint_instance = NULL;
                size_t i;

                session_instance->remote_incoming_window = flow_next_incoming_id + flow_incoming_window - session_instance->next_outgoing_id;

                if (flow_get_handle(flow_handle, &remote_handle) == 0)
                {
                    link_endpoint_instance = find_link_endpoint_by_input_handle(session_instance, remote_handle);
                }

                flow_destroy(flow_handle);

                if (link_endpoint_instance != NULL)
                {
                    if (link_endpoint_instance->link_endpoint_state != LINK_ENDPOINT_STATE_DETACHING)
                    {
                        link_endpoint_instance->frame_received_callback(link_endpoint_instance->callback_context, performative, payload_size, payload_bytes);
                    }
                }

                i = 0;
                while ((session_instance->remote_incoming_window > 0) && (i < session_instance->link_endpoint_count))
                {
                    /* notify the caller that it can send here */
                    if (session_instance->link_endpoints[i]->link_endpoint_state != LINK_ENDPOINT_STATE_DETACHING &&
                        session_instance->link_endpoints[i]->on_session_flow_on != NULL)
                    {
                        session_instance->link_endpoints[i]->on_session_flow_on(session_instance->link_endpoints[i]->callback_context);
                    }

                    i++;
                }
            }
        }
    }
    else if (is_transfer_type_by_descriptor(descriptor))
    {
        TRANSFER_HANDLE transfer_handle;

        if (amqpvalue_get_transfer(performative, &transfer_handle) != 0)
        {
            end_session_with_error(session_instance, "amqp:decode-error", "Cannot decode TRANSFER frame");
        }
        else
        {
            uint32_t remote_handle;
            delivery_number delivery_id;

            transfer_get_delivery_id(transfer_handle, &delivery_id);
            if (transfer_get_handle(transfer_handle, &remote_handle) != 0)
            {
                transfer_destroy(transfer_handle);
                end_session_with_error(session_instance, "amqp:decode-error", "Cannot get handle from TRANSFER frame");
            }
            else
            {
                LINK_ENDPOINT_INSTANCE* link_endpoint;
                transfer_destroy(transfer_handle);

                session_instance->next_incoming_id++;
                session_instance->remote_outgoing_window--;
                session_instance->incoming_window--;

                link_endpoint = find_link_endpoint_by_input_handle(session_instance, remote_handle);
                if (link_endpoint == NULL)
                {
                    end_session_with_error(session_instance, "amqp:session:unattached-handle", "");
                }
                else
                {
                    if (link_endpoint->link_endpoint_state != LINK_ENDPOINT_STATE_DETACHING)
                    {
                        link_endpoint->frame_received_callback(link_endpoint->callback_context, performative, payload_size, payload_bytes);
                    }
                }

                if (session_instance->incoming_window == 0)
                {
                    session_instance->incoming_window = session_instance->desired_incoming_window;
                    send_flow(session_instance);
                }
            }
        }
    }
    else if (is_disposition_type_by_descriptor(descriptor))
    {
        uint32_t i;

        for (i = 0; i < session_instance->link_endpoint_count; i++)
        {
            LINK_ENDPOINT_INSTANCE* link_endpoint = session_instance->link_endpoints[i];
            if (link_endpoint->link_endpoint_state != LINK_ENDPOINT_STATE_DETACHING)
            {
                link_endpoint->frame_received_callback(link_endpoint->callback_context, performative, payload_size, payload_bytes);
            }
        }
    }
    else if (is_end_type_by_descriptor(descriptor))
    {
        END_HANDLE end_handle;

        if (amqpvalue_get_end(performative, &end_handle) != 0)
        {
            end_session_with_error(session_instance, "amqp:decode-error", "Cannot decode END frame");
        }
        else
        {
            end_destroy(end_handle);
            if ((session_instance->session_state != SESSION_STATE_END_RCVD) &&
                (session_instance->session_state != SESSION_STATE_DISCARDING))
            {
                session_set_state(session_instance, SESSION_STATE_END_RCVD);
                if (send_end_frame(session_instance, NULL) != 0)
                {
                    /* fatal error */
                    (void)connection_close(session_instance->connection, "amqp:internal-error", "Cannot send END frame.", NULL);
                }

                session_set_state(session_instance, SESSION_STATE_DISCARDING);
            }
        }
    }
}

SESSION_HANDLE session_create(CONNECTION_HANDLE connection, ON_LINK_ATTACHED on_link_attached, void* callback_context)
{
    SESSION_INSTANCE* result;

    if (connection == NULL)
    {
        /* Codes_S_R_S_SESSION_01_031: [If connection is NULL, session_create shall fail and return NULL.] */
        result = NULL;
    }
    else
    {
        /* Codes_S_R_S_SESSION_01_030: [session_create shall create a new session instance and return a non-NULL handle to it.] */
        result = (SESSION_INSTANCE*)calloc(1, sizeof(SESSION_INSTANCE));
        /* Codes_S_R_S_SESSION_01_042: [If allocating memory for the session fails, session_create shall fail and return NULL.] */
        if (result != NULL)
        {
            result->connection = connection;
            result->link_endpoints = NULL;
            result->link_endpoint_count = 0;
            result->handle_max = 4294967295u;

            /* Codes_S_R_S_SESSION_01_057: [The delivery ids shall be assigned starting at 0.] */
            /* Codes_S_R_S_SESSION_01_017: [The nextoutgoing-id MAY be initialized to an arbitrary value ] */
            result->next_outgoing_id = 0;

            result->desired_incoming_window = 1;
            result->incoming_window = 1;
            result->outgoing_window = 1;
            result->handle_max = 4294967295u;
            result->remote_incoming_window = 0;
            result->remote_outgoing_window = 0;
            result->previous_session_state = SESSION_STATE_UNMAPPED;
            result->is_underlying_connection_open = UNDERLYING_CONNECTION_NOT_OPEN;
            result->session_state = SESSION_STATE_UNMAPPED;
            result->on_link_attached = on_link_attached;
            result->on_link_attached_callback_context = callback_context;

            /* Codes_S_R_S_SESSION_01_032: [session_create shall create a new session endpoint by calling connection_create_endpoint.] */
            result->endpoint = connection_create_endpoint(connection);
            if (result->endpoint == NULL)
            {
                /* Codes_S_R_S_SESSION_01_033: [If connection_create_endpoint fails, session_create shall fail and return NULL.] */
                free(result);
                result = NULL;
            }
            else
            {
                session_set_state(result, SESSION_STATE_UNMAPPED);
            }
        }
    }

    return result;
}

SESSION_HANDLE session_create_from_endpoint(CONNECTION_HANDLE connection, ENDPOINT_HANDLE endpoint, ON_LINK_ATTACHED on_link_attached, void* callback_context)
{
    SESSION_INSTANCE* result;

    if (endpoint == NULL)
    {
        result = NULL;
    }
    else
    {
        result = (SESSION_INSTANCE*)calloc(1, sizeof(SESSION_INSTANCE));
        if (result != NULL)
        {
            result->connection = connection;
            result->link_endpoints = NULL;
            result->link_endpoint_count = 0;
            result->handle_max = 4294967295u;

            result->next_outgoing_id = 0;

            result->desired_incoming_window = 1;
            result->incoming_window = 1;
            result->outgoing_window = 1;
            result->handle_max = 4294967295u;
            result->remote_incoming_window = 0;
            result->remote_outgoing_window = 0;
            result->previous_session_state = SESSION_STATE_UNMAPPED;
            result->is_underlying_connection_open = UNDERLYING_CONNECTION_NOT_OPEN;
            result->session_state = SESSION_STATE_UNMAPPED;
            result->on_link_attached = on_link_attached;
            result->on_link_attached_callback_context = callback_context;

            result->endpoint = endpoint;
            session_set_state(result, SESSION_STATE_UNMAPPED);
        }
    }

    return result;
}

void session_destroy(SESSION_HANDLE session)
{
    /* Codes_S_R_S_SESSION_01_036: [If session is NULL, session_destroy shall do nothing.] */
    if (session != NULL)
    {
        SESSION_INSTANCE* session_instance = (SESSION_INSTANCE*)session;

        session_end(session, NULL, NULL);

        /* Codes_S_R_S_SESSION_01_034: [session_destroy shall free all resources allocated by session_create.] */
        /* Codes_S_R_S_SESSION_01_035: [The endpoint created in session_create shall be freed by calling connection_destroy_endpoint.] */
        connection_destroy_endpoint(session_instance->endpoint);
        if (session_instance->link_endpoints != NULL)
        {
            free(session_instance->link_endpoints);
        }

        free(session);
    }
}

int session_begin(SESSION_HANDLE session)
{
    int result;

    if (session == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        SESSION_INSTANCE* session_instance = (SESSION_INSTANCE*)session;

        if (connection_start_endpoint(session_instance->endpoint, on_frame_received, on_connection_state_changed, session_instance) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (!session_instance->is_underlying_connection_open)
            {
                if (connection_open(session_instance->connection) != 0)
                {
                    session_instance->is_underlying_connection_open = UNDERLYING_CONNECTION_NOT_OPEN;
                    result = MU_FAILURE;
                }
                else
                {
                    session_instance->is_underlying_connection_open = UNDERLYING_CONNECTION_OPEN;
                    result = 0;
                }
            }
            else
            {
                result = 0;
            }
        }
    }

    return result;
}

int session_end(SESSION_HANDLE session, const char* condition_value, const char* description)
{
    int result;

    if (session == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        SESSION_INSTANCE* session_instance = (SESSION_INSTANCE*)session;
        size_t i;

        if ((session_instance->session_state != SESSION_STATE_UNMAPPED) &&
            (session_instance->session_state != SESSION_STATE_DISCARDING))
        {
            ERROR_HANDLE error_handle = NULL;
            result = 0;

            if (condition_value != NULL)
            {
                error_handle = error_create(condition_value);
                if (error_handle == NULL)
                {
                    result = MU_FAILURE;
                }
                else
                {
                    if (error_set_description(error_handle, description) != 0)
                    {
                        result = MU_FAILURE;
                    }
                }
            }

            if (result == 0)
            {
                if (send_end_frame(session_instance, error_handle) != 0)
                {
                    result = MU_FAILURE;
                }
                else
                {
                    session_set_state(session_instance, SESSION_STATE_DISCARDING);
                    result = 0;
                }
            }

            if (error_handle != NULL)
            {
                error_destroy(error_handle);
            }
        }
        else
        {
            result = 0;
        }

        // all link endpoints are destroyed when the session end happens
        for (i = 0; i < session_instance->link_endpoint_count; i++)
        {
            free_link_endpoint(session_instance->link_endpoints[i]);
        }

        session_instance->link_endpoint_count = 0;
    }

    return result;
}

int session_set_incoming_window(SESSION_HANDLE session, uint32_t incoming_window)
{
    int result;

    if (session == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        SESSION_INSTANCE* session_instance = (SESSION_INSTANCE*)session;

        session_instance->desired_incoming_window = incoming_window;
        session_instance->incoming_window = incoming_window;

        result = 0;
    }

    return result;
}

int session_get_incoming_window(SESSION_HANDLE session, uint32_t* incoming_window)
{
    int result;

    if ((session == NULL) ||
        (incoming_window == NULL))
    {
        result = MU_FAILURE;
    }
    else
    {
        SESSION_INSTANCE* session_instance = (SESSION_INSTANCE*)session;

        *incoming_window = session_instance->incoming_window;

        result = 0;
    }

    return result;
}

int session_set_outgoing_window(SESSION_HANDLE session, uint32_t outgoing_window)
{
    int result;

    if (session == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        SESSION_INSTANCE* session_instance = (SESSION_INSTANCE*)session;

        session_instance->outgoing_window = outgoing_window;

        result = 0;
    }

    return result;
}

int session_get_outgoing_window(SESSION_HANDLE session, uint32_t* outgoing_window)
{
    int result;

    if ((session == NULL) ||
        (outgoing_window == NULL))
    {
        result = MU_FAILURE;
    }
    else
    {
        SESSION_INSTANCE* session_instance = (SESSION_INSTANCE*)session;

        *outgoing_window = session_instance->outgoing_window;

        result = 0;
    }

    return result;
}

int session_set_handle_max(SESSION_HANDLE session, handle handle_max)
{
    int result;

    if (session == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        SESSION_INSTANCE* session_instance = (SESSION_INSTANCE*)session;

        session_instance->handle_max = handle_max;

        result = 0;
    }

    return result;
}

int session_get_handle_max(SESSION_HANDLE session, handle* handle_max)
{
    int result;

    if ((session == NULL) ||
        (handle_max == NULL))
    {
        result = MU_FAILURE;
    }
    else
    {
        SESSION_INSTANCE* session_instance = (SESSION_INSTANCE*)session;

        *handle_max = session_instance->handle_max;

        result = 0;
    }

    return result;
}

LINK_ENDPOINT_HANDLE session_create_link_endpoint(SESSION_HANDLE session, const char* name)
{
    LINK_ENDPOINT_INSTANCE* result;

    /* Codes_S_R_S_SESSION_01_044: [If session, name or frame_received_callback is NULL, session_create_link_endpoint shall fail and return NULL.] */
    if ((session == NULL) ||
        (name == NULL))
    {
        result = NULL;
    }
    else
    {
        /* Codes_S_R_S_SESSION_01_043: [session_create_link_endpoint shall create a link endpoint associated with a given session and return a non-NULL handle to it.] */
        SESSION_INSTANCE* session_instance = (SESSION_INSTANCE*)session;

        result = (LINK_ENDPOINT_INSTANCE*)calloc(1, sizeof(LINK_ENDPOINT_INSTANCE));
        /* Codes_S_R_S_SESSION_01_045: [If allocating memory for the link endpoint fails, session_create_link_endpoint shall fail and return NULL.] */
        if (result != NULL)
        {
            /* Codes_S_R_S_SESSION_01_046: [An unused handle shall be assigned to the link endpoint.] */
            handle selected_handle = 0;
            size_t i;
            size_t name_length;

            for (i = 0; i < session_instance->link_endpoint_count; i++)
            {
                if (session_instance->link_endpoints[i]->output_handle > selected_handle)
                {
                    break;
                }

                selected_handle++;
            }

            result->on_session_state_changed = NULL;
            result->on_session_flow_on = NULL;
            result->frame_received_callback = NULL;
            result->callback_context = NULL;
            result->output_handle = selected_handle;
            result->input_handle = 0xFFFFFFFF;
            result->link_endpoint_state = LINK_ENDPOINT_STATE_NOT_ATTACHED;
            name_length = strlen(name);
            result->name = (char*)malloc(name_length + 1);
            result->on_link_endpoint_destroyed_callback = NULL;
            result->on_link_endpoint_destroyed_context = NULL;
            if (result->name == NULL)
            {
                /* Codes_S_R_S_SESSION_01_045: [If allocating memory for the link endpoint fails, session_create_link_endpoint shall fail and return NULL.] */
                free(result);
                result = NULL;
            }
            else
            {
                LINK_ENDPOINT_INSTANCE** new_link_endpoints;
                (void)memcpy(result->name, name, name_length + 1);
                result->session = session;

                new_link_endpoints = (LINK_ENDPOINT_INSTANCE**)realloc(session_instance->link_endpoints, sizeof(LINK_ENDPOINT_INSTANCE*) * (session_instance->link_endpoint_count + 1));
                if (new_link_endpoints == NULL)
                {
                    /* Codes_S_R_S_SESSION_01_045: [If allocating memory for the link endpoint fails, session_create_link_endpoint shall fail and return NULL.] */
                    free(result->name);
                    free(result);
                    result = NULL;
                }
                else
                {
                    session_instance->link_endpoints = new_link_endpoints;

                    if (session_instance->link_endpoint_count - selected_handle > 0)
                    {
                        (void)memmove(&session_instance->link_endpoints[selected_handle + 1], &session_instance->link_endpoints[selected_handle], (session_instance->link_endpoint_count - selected_handle) * sizeof(LINK_ENDPOINT_INSTANCE*));
                    }

                    session_instance->link_endpoints[selected_handle] = result;
                    session_instance->link_endpoint_count++;
                }
            }
        }
    }

    return result;
}

void session_set_link_endpoint_callback(LINK_ENDPOINT_HANDLE link_endpoint, ON_LINK_ENDPOINT_DESTROYED_CALLBACK on_link_endpoint_destroyed, void* context)
{
    if (link_endpoint != NULL)
    {
        link_endpoint->on_link_endpoint_destroyed_callback = on_link_endpoint_destroyed;
        link_endpoint->on_link_endpoint_destroyed_context = context;
    }
}

void session_destroy_link_endpoint(LINK_ENDPOINT_HANDLE link_endpoint)
{
    if (link_endpoint != NULL)
    {
        LINK_ENDPOINT_INSTANCE* endpoint_instance = (LINK_ENDPOINT_INSTANCE*)link_endpoint;

        if (endpoint_instance->link_endpoint_state == LINK_ENDPOINT_STATE_ATTACHED)
        {
            endpoint_instance->link_endpoint_state = LINK_ENDPOINT_STATE_DETACHING;
        }
        else
        {
            remove_link_endpoint(link_endpoint);
            free_link_endpoint(link_endpoint);
        }
    }
}

int session_start_link_endpoint(LINK_ENDPOINT_HANDLE link_endpoint, ON_ENDPOINT_FRAME_RECEIVED frame_received_callback, ON_SESSION_STATE_CHANGED on_session_state_changed, ON_SESSION_FLOW_ON on_session_flow_on, void* context)
{
    int result;

    if ((link_endpoint == NULL) ||
        (frame_received_callback == NULL))
    {
        result = MU_FAILURE;
    }
    else
    {
        link_endpoint->frame_received_callback = frame_received_callback;
        link_endpoint->on_session_state_changed = on_session_state_changed;
        link_endpoint->on_session_flow_on = on_session_flow_on;
        link_endpoint->callback_context = context;
        link_endpoint->link_endpoint_state = LINK_ENDPOINT_STATE_NOT_ATTACHED;

        if (link_endpoint->on_session_state_changed != NULL)
        {
            link_endpoint->on_session_state_changed(link_endpoint->callback_context, link_endpoint->session->session_state, link_endpoint->session->previous_session_state);
        }

        result = 0;
    }

    return result;
}

static int encode_frame(LINK_ENDPOINT_HANDLE link_endpoint, AMQP_VALUE performative, PAYLOAD* payloads, size_t payload_count)
{
    int result;

    if ((link_endpoint == NULL) ||
        (performative == NULL))
    {
        result = MU_FAILURE;
    }
    else
    {
        LINK_ENDPOINT_INSTANCE* link_endpoint_instance = (LINK_ENDPOINT_INSTANCE*)link_endpoint;
        SESSION_INSTANCE* session_instance = (SESSION_INSTANCE*)link_endpoint_instance->session;

        if (connection_encode_frame(session_instance->endpoint, performative, payloads, payload_count, NULL, NULL) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            result = 0;
        }
    }

    return result;
}

int session_send_flow(LINK_ENDPOINT_HANDLE link_endpoint, FLOW_HANDLE flow)
{
    int result;

    if ((link_endpoint == NULL) ||
        (flow == NULL))
    {
        result = MU_FAILURE;
    }
    else
    {
        LINK_ENDPOINT_INSTANCE* link_endpoint_instance = (LINK_ENDPOINT_INSTANCE*)link_endpoint;
        SESSION_INSTANCE* session_instance = (SESSION_INSTANCE*)link_endpoint_instance->session;

        result = 0;

        if (session_instance->session_state == SESSION_STATE_BEGIN_RCVD)
        {
            if (flow_set_next_incoming_id(flow, session_instance->next_incoming_id) != 0)
            {
                result = MU_FAILURE;
            }
        }

        if (result == 0)
        {
            if ((flow_set_next_incoming_id(flow, session_instance->next_incoming_id) != 0) ||
                (flow_set_incoming_window(flow, session_instance->incoming_window) != 0) ||
                (flow_set_next_outgoing_id(flow, session_instance->next_outgoing_id) != 0) ||
                (flow_set_outgoing_window(flow, session_instance->outgoing_window) != 0) ||
                (flow_set_handle(flow, link_endpoint_instance->output_handle) != 0))
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE flow_performative_value = amqpvalue_create_flow(flow);
                if (flow_performative_value == NULL)
                {
                    result = MU_FAILURE;
                }
                else
                {
                    if (encode_frame(link_endpoint, flow_performative_value, NULL, 0) != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }

                    amqpvalue_destroy(flow_performative_value);
                }
            }
        }
    }

    return result;
}

int session_send_attach(LINK_ENDPOINT_HANDLE link_endpoint, ATTACH_HANDLE attach)
{
    int result;

    if ((link_endpoint == NULL) ||
        (attach == NULL))
    {
        result = MU_FAILURE;
    }
    else
    {
        LINK_ENDPOINT_INSTANCE* link_endpoint_instance = (LINK_ENDPOINT_INSTANCE*)link_endpoint;

        if (attach_set_handle(attach, link_endpoint_instance->output_handle) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            AMQP_VALUE attach_performative_value = amqpvalue_create_attach(attach);
            if (attach_performative_value == NULL)
            {
                result = MU_FAILURE;
            }
            else
            {
                if (encode_frame(link_endpoint, attach_performative_value, NULL, 0) != 0)
                {
                    result = MU_FAILURE;
                }
                else
                {
                    result = 0;
                }

                amqpvalue_destroy(attach_performative_value);
            }
        }
    }

    return result;
}

int session_send_disposition(LINK_ENDPOINT_HANDLE link_endpoint, DISPOSITION_HANDLE disposition)
{
    int result;

    if ((link_endpoint == NULL) ||
        (disposition == NULL))
    {
        result = MU_FAILURE;
    }
    else
    {
        AMQP_VALUE disposition_performative_value = amqpvalue_create_disposition(disposition);
        if (disposition_performative_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (encode_frame(link_endpoint, disposition_performative_value, NULL, 0) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(disposition_performative_value);
        }
    }

    return result;
}

int session_send_detach(LINK_ENDPOINT_HANDLE link_endpoint, DETACH_HANDLE detach)
{
    int result;

    if ((link_endpoint == NULL) ||
        (detach == NULL))
    {
        result = MU_FAILURE;
    }
    else
    {
        LINK_ENDPOINT_INSTANCE* link_endpoint_instance = (LINK_ENDPOINT_INSTANCE*)link_endpoint;

        if (detach_set_handle(detach, link_endpoint_instance->output_handle) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            AMQP_VALUE detach_performative_value = amqpvalue_create_detach(detach);
            if (detach_performative_value == NULL)
            {
                result = MU_FAILURE;
            }
            else
            {
                if (encode_frame(link_endpoint, detach_performative_value, NULL, 0) != 0)
                {
                    result = MU_FAILURE;
                }
                else
                {
                    result = 0;
                }

                amqpvalue_destroy(detach_performative_value);
            }
        }
    }

    return result;
}

/* Codes_S_R_S_SESSION_01_051: [session_send_transfer shall send a transfer frame with the performative indicated in the transfer argument.] */
SESSION_SEND_TRANSFER_RESULT session_send_transfer(LINK_ENDPOINT_HANDLE link_endpoint, TRANSFER_HANDLE transfer, PAYLOAD* payloads, size_t payload_count, delivery_number* delivery_id, ON_SEND_COMPLETE on_send_complete, void* callback_context)
{
    SESSION_SEND_TRANSFER_RESULT result;

    /* Codes_S_R_S_SESSION_01_054: [If link_endpoint or transfer is NULL, session_send_transfer shall fail and return a non-zero value.] */
    if ((link_endpoint == NULL) ||
        (transfer == NULL))
    {
        result = SESSION_SEND_TRANSFER_ERROR;
    }
    else
    {
        LINK_ENDPOINT_INSTANCE* link_endpoint_instance = (LINK_ENDPOINT_INSTANCE*)link_endpoint;
        SESSION_INSTANCE* session_instance = (SESSION_INSTANCE*)link_endpoint_instance->session;

        /* Codes_S_R_S_SESSION_01_059: [When session_send_transfer is called while the session is not in the MAPPED state, session_send_transfer shall fail and return a non-zero value.] */
        if (session_instance->session_state != SESSION_STATE_MAPPED)
        {
            result = SESSION_SEND_TRANSFER_ERROR;
        }
        else
        {
            size_t payload_size = 0;
            size_t i;

            for (i = 0; i < payload_count; i++)
            {
                if ((payloads[i].length > UINT32_MAX) ||
                    (payload_size + payloads[i].length < payload_size))
                {
                    break;
                }

                payload_size += payloads[i].length;
            }

            if ((i < payload_count) ||
                (payload_size > UINT32_MAX))
            {
                result = SESSION_SEND_TRANSFER_ERROR;
            }
            else
            {
                if (session_instance->remote_incoming_window == 0)
                {
                    result = SESSION_SEND_TRANSFER_BUSY;
                }
                else
                {
                    /* Codes_S_R_S_SESSION_01_012: [The session endpoint assigns each outgoing transfer frame an implicit transfer-id from a session scoped sequence.] */
                    /* Codes_S_R_S_SESSION_01_027: [sending a transfer Upon sending a transfer, the sending endpoint will increment its next-outgoing-id] */
                    *delivery_id = session_instance->next_outgoing_id;
                    if ((transfer_set_handle(transfer, link_endpoint_instance->output_handle) != 0) ||
                        (transfer_set_delivery_id(transfer, *delivery_id) != 0) ||
                        (transfer_set_more(transfer, false) != 0))
                    {
                        /* Codes_S_R_S_SESSION_01_058: [When any other error occurs, session_send_transfer shall fail and return a non-zero value.] */
                        result = SESSION_SEND_TRANSFER_ERROR;
                    }
                    else
                    {
                        AMQP_VALUE transfer_value;

                        transfer_value = amqpvalue_create_transfer(transfer);
                        if (transfer_value == NULL)
                        {
                            /* Codes_S_R_S_SESSION_01_058: [When any other error occurs, session_send_transfer shall fail and return a non-zero value.] */
                            result = SESSION_SEND_TRANSFER_ERROR;
                        }
                        else
                        {
                            uint32_t available_frame_size;
                            size_t encoded_size;

                            if ((connection_get_remote_max_frame_size(session_instance->connection, &available_frame_size) != 0) ||
                                (amqpvalue_get_encoded_size(transfer_value, &encoded_size) != 0))
                            {
                                result = SESSION_SEND_TRANSFER_ERROR;
                            }
                            else
                            {
                                payload_size = 0;

                                for (i = 0; i < payload_count; i++)
                                {
                                    payload_size += payloads[i].length;
                                }

                                available_frame_size -= (uint32_t)encoded_size;
                                available_frame_size -= 8;

                                if (available_frame_size >= payload_size)
                                {
                                    /* Codes_S_R_S_SESSION_01_055: [The encoding of the frame shall be done by calling connection_encode_frame and passing as arguments: the connection handle associated with the session, the transfer performative and the payload chunks passed to session_send_transfer.] */
                                    if (connection_encode_frame(session_instance->endpoint, transfer_value, payloads, payload_count, on_send_complete, callback_context) != 0)
                                    {
                                        /* Codes_S_R_S_SESSION_01_056: [If connection_encode_frame fails then session_send_transfer shall fail and return a non-zero value.] */
                                        result = SESSION_SEND_TRANSFER_ERROR;
                                    }
                                    else
                                    {
                                        /* Codes_S_R_S_SESSION_01_018: [is incremented after each successive transfer according to RFC-1982 [RFC1982] serial number arithmetic.] */
                                        session_instance->next_outgoing_id++;
                                        session_instance->remote_incoming_window--;
                                        session_instance->outgoing_window--;

                                        /* Codes_S_R_S_SESSION_01_053: [On success, session_send_transfer shall return 0.] */
                                        result = SESSION_SEND_TRANSFER_OK;
                                    }
                                }
                                else
                                {
                                    size_t current_payload_index = 0;
                                    uint32_t current_payload_pos = 0;

                                    /* break it down into different deliveries */
                                    while (payload_size > 0)
                                    {
                                        uint32_t transfer_frame_payload_count = 0;
                                        uint32_t current_transfer_frame_payload_size = (uint32_t)payload_size;
                                        uint32_t byte_counter;
                                        size_t temp_current_payload_index = current_payload_index;
                                        uint32_t temp_current_payload_pos = current_payload_pos;
                                        AMQP_VALUE multi_transfer_amqp_value;
                                        PAYLOAD* transfer_frame_payloads;
                                        bool more;

                                        if (current_transfer_frame_payload_size > available_frame_size)
                                        {
                                            current_transfer_frame_payload_size = available_frame_size;
                                        }

                                        if (available_frame_size >= payload_size)
                                        {
                                            more = false;
                                        }
                                        else
                                        {
                                            more = true;
                                        }

                                        if (transfer_set_more(transfer, more) != 0)
                                        {
                                            break;
                                        }

                                        multi_transfer_amqp_value = amqpvalue_create_transfer(transfer);
                                        if (multi_transfer_amqp_value == NULL)
                                        {
                                            break;
                                        }

                                        byte_counter = current_transfer_frame_payload_size;
                                        while (byte_counter > 0)
                                        {
                                            if (payloads[temp_current_payload_index].length - temp_current_payload_pos >= byte_counter)
                                            {
                                                /* more data than we need */
                                                temp_current_payload_pos += byte_counter;
                                                byte_counter = 0;
                                            }
                                            else
                                            {
                                                byte_counter -= (uint32_t)payloads[temp_current_payload_index].length - temp_current_payload_pos;
                                                temp_current_payload_index++;
                                                temp_current_payload_pos = 0;
                                            }
                                        }

                                        transfer_frame_payload_count = (uint32_t)(temp_current_payload_index - current_payload_index + 1);
                                        transfer_frame_payloads = (PAYLOAD*)calloc(1, (transfer_frame_payload_count * sizeof(PAYLOAD)));
                                        if (transfer_frame_payloads == NULL)
                                        {
                                            amqpvalue_destroy(multi_transfer_amqp_value);
                                            break;
                                        }

                                        /* copy data */
                                        byte_counter = current_transfer_frame_payload_size;
                                        transfer_frame_payload_count = 0;

                                        while (byte_counter > 0)
                                        {
                                            if (payloads[current_payload_index].length - current_payload_pos > byte_counter)
                                            {
                                                /* more data than we need */
                                                transfer_frame_payloads[transfer_frame_payload_count].bytes = payloads[current_payload_index].bytes + current_payload_pos;
                                                transfer_frame_payloads[transfer_frame_payload_count].length = byte_counter;
                                                current_payload_pos += byte_counter;
                                                byte_counter = 0;
                                            }
                                            else
                                            {
                                                /* copy entire payload and move to the next */
                                                transfer_frame_payloads[transfer_frame_payload_count].bytes = payloads[current_payload_index].bytes + current_payload_pos;
                                                transfer_frame_payloads[transfer_frame_payload_count].length = payloads[current_payload_index].length - current_payload_pos;
                                                byte_counter -= (uint32_t)payloads[current_payload_index].length - current_payload_pos;
                                                current_payload_index++;
                                                current_payload_pos = 0;
                                            }

                                            transfer_frame_payload_count++;
                                        }

                                        if (connection_encode_frame(session_instance->endpoint, multi_transfer_amqp_value, transfer_frame_payloads, transfer_frame_payload_count,
                                            /* only fire the send complete calllback on the last frame of the multi frame transfer */
                                            more ? NULL : on_send_complete,
                                            callback_context) != 0)
                                        {
                                            free(transfer_frame_payloads);
                                            amqpvalue_destroy(multi_transfer_amqp_value);
                                            break;
                                        }

                                        free(transfer_frame_payloads);
                                        amqpvalue_destroy(multi_transfer_amqp_value);
                                        payload_size -= current_transfer_frame_payload_size;
                                    }

                                    if (payload_size > 0)
                                    {
                                        result = SESSION_SEND_TRANSFER_ERROR;
                                    }
                                    else
                                    {
                                        /* Codes_S_R_S_SESSION_01_018: [is incremented after each successive transfer according to RFC-1982 [RFC1982] serial number arithmetic.] */
                                        session_instance->next_outgoing_id++;
                                        session_instance->remote_incoming_window--;
                                        session_instance->outgoing_window--;

                                        result = SESSION_SEND_TRANSFER_OK;
                                    }
                                }
                            }

                            amqpvalue_destroy(transfer_value);
                        }
                    }
                }
            }
        }
    }

    return result;
}
