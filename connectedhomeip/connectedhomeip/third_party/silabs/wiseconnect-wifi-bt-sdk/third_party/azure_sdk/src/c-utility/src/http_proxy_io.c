// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <limits.h>
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/xio.h"
#include "azure_c_shared_utility/socketio.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/http_proxy_io.h"
#include "azure_c_shared_utility/azure_base64.h"

static const char* const OPTION_UNDERLYING_IO_OPTIONS = "underlying_io_options";

typedef enum HTTP_PROXY_IO_STATE_TAG
{
    HTTP_PROXY_IO_STATE_CLOSED,
    HTTP_PROXY_IO_STATE_OPENING_UNDERLYING_IO,
    HTTP_PROXY_IO_STATE_WAITING_FOR_CONNECT_RESPONSE,
    HTTP_PROXY_IO_STATE_OPEN,
    HTTP_PROXY_IO_STATE_CLOSING,
    HTTP_PROXY_IO_STATE_ERROR
} HTTP_PROXY_IO_STATE;

typedef struct HTTP_PROXY_IO_INSTANCE_TAG
{
    HTTP_PROXY_IO_STATE http_proxy_io_state;
    ON_BYTES_RECEIVED on_bytes_received;
    void* on_bytes_received_context;
    ON_IO_ERROR on_io_error;
    void* on_io_error_context;
    ON_IO_OPEN_COMPLETE on_io_open_complete;
    void* on_io_open_complete_context;
    ON_IO_CLOSE_COMPLETE on_io_close_complete;
    void* on_io_close_complete_context;
    char* hostname;
    int port;
    char* proxy_hostname;
    int proxy_port;
    char* username;
    char* password;
    XIO_HANDLE underlying_io;
    unsigned char* receive_buffer;
    size_t receive_buffer_size;
} HTTP_PROXY_IO_INSTANCE;

static CONCRETE_IO_HANDLE http_proxy_io_create(void* io_create_parameters)
{
    HTTP_PROXY_IO_INSTANCE* result;

    if (io_create_parameters == NULL)
    {
        /* Codes_SRS_HTTP_PROXY_IO_01_002: [ If io_create_parameters is NULL, http_proxy_io_create shall fail and return NULL. ]*/
        result = NULL;
        LogError("NULL io_create_parameters.");
    }
    else
    {
        /* Codes_SRS_HTTP_PROXY_IO_01_003: [ io_create_parameters shall be used as an HTTP_PROXY_IO_CONFIG*. ]*/
        HTTP_PROXY_IO_CONFIG* http_proxy_io_config = (HTTP_PROXY_IO_CONFIG*)io_create_parameters;
        if ((http_proxy_io_config->hostname == NULL) ||
            (http_proxy_io_config->proxy_hostname == NULL))
        {
            /* Codes_SRS_HTTP_PROXY_IO_01_004: [ If the hostname or proxy_hostname member is NULL, then http_proxy_io_create shall fail and return NULL. ]*/
            result = NULL;
            LogError("Bad arguments: hostname = %p, proxy_hostname = %p",
                http_proxy_io_config->hostname, http_proxy_io_config->proxy_hostname);
        }
        /* Codes_SRS_HTTP_PROXY_IO_01_095: [ If one of the fields username and password is non-NULL, then the other has to be also non-NULL, otherwise http_proxy_io_create shall fail and return NULL. ]*/
        else if (((http_proxy_io_config->username == NULL) && (http_proxy_io_config->password != NULL)) ||
            ((http_proxy_io_config->username != NULL) && (http_proxy_io_config->password == NULL)))
        {
            result = NULL;
            LogError("Bad arguments: username = %p, password = %p",
                http_proxy_io_config->username, http_proxy_io_config->password);
        }
        else
        {
            /* Codes_SRS_HTTP_PROXY_IO_01_001: [ http_proxy_io_create shall create a new instance of the HTTP proxy IO. ]*/
            result = (HTTP_PROXY_IO_INSTANCE*)calloc(1, sizeof(HTTP_PROXY_IO_INSTANCE));
            if (result == NULL)
            {
                /* Codes_SRS_HTTP_PROXY_IO_01_051: [ If allocating memory for the new instance fails, http_proxy_io_create shall fail and return NULL. ]*/
                LogError("Failed allocating HTTP proxy IO instance.");
            }
            else
            {
                /* Codes_SRS_HTTP_PROXY_IO_01_005: [ http_proxy_io_create shall copy the hostname, port, username and password values for later use when the actual CONNECT is performed. ]*/
                /* Codes_SRS_HTTP_PROXY_IO_01_006: [ hostname and proxy_hostname, username and password shall be copied by calling mallocAndStrcpy_s. ]*/
                if (mallocAndStrcpy_s(&result->hostname, http_proxy_io_config->hostname) != 0)
                {
                    /* Codes_SRS_HTTP_PROXY_IO_01_007: [ If mallocAndStrcpy_s fails then http_proxy_io_create shall fail and return NULL. ]*/
                    LogError("Failed to copy the hostname.");
                    /* Codes_SRS_HTTP_PROXY_IO_01_008: [ When http_proxy_io_create fails, all allocated resources up to that point shall be freed. ]*/
                    free(result);
                    result = NULL;
                }
                else
                {
                    /* Codes_SRS_HTTP_PROXY_IO_01_006: [ hostname and proxy_hostname, username and password shall be copied by calling mallocAndStrcpy_s. ]*/
                    if (mallocAndStrcpy_s(&result->proxy_hostname, http_proxy_io_config->proxy_hostname) != 0)
                    {
                        /* Codes_SRS_HTTP_PROXY_IO_01_007: [ If mallocAndStrcpy_s fails then http_proxy_io_create shall fail and return NULL. ]*/
                        LogError("Failed to copy the proxy_hostname.");
                        /* Codes_SRS_HTTP_PROXY_IO_01_008: [ When http_proxy_io_create fails, all allocated resources up to that point shall be freed. ]*/
                        free(result->hostname);
                        free(result);
                        result = NULL;
                    }
                    else
                    {
                        result->username = NULL;
                        result->password = NULL;

                        /* Codes_SRS_HTTP_PROXY_IO_01_006: [ hostname and proxy_hostname, username and password shall be copied by calling mallocAndStrcpy_s. ]*/
                        /* Codes_SRS_HTTP_PROXY_IO_01_094: [ username and password shall be optional. ]*/
                        if ((http_proxy_io_config->username != NULL) && (mallocAndStrcpy_s(&result->username, http_proxy_io_config->username) != 0))
                        {
                            /* Codes_SRS_HTTP_PROXY_IO_01_007: [ If mallocAndStrcpy_s fails then http_proxy_io_create shall fail and return NULL. ]*/
                            LogError("Failed to copy the username.");
                            /* Codes_SRS_HTTP_PROXY_IO_01_008: [ When http_proxy_io_create fails, all allocated resources up to that point shall be freed. ]*/
                            free(result->proxy_hostname);
                            free(result->hostname);
                            free(result);
                            result = NULL;
                        }
                        else
                        {
                            /* Codes_SRS_HTTP_PROXY_IO_01_006: [ hostname and proxy_hostname, username and password shall be copied by calling mallocAndStrcpy_s. ]*/
                            /* Codes_SRS_HTTP_PROXY_IO_01_094: [ username and password shall be optional. ]*/
                            if ((http_proxy_io_config->password != NULL) && (mallocAndStrcpy_s(&result->password, http_proxy_io_config->password) != 0))
                            {
                                /* Codes_SRS_HTTP_PROXY_IO_01_007: [ If mallocAndStrcpy_s fails then http_proxy_io_create shall fail and return NULL. ]*/
                                LogError("Failed to copy the passowrd.");
                                /* Codes_SRS_HTTP_PROXY_IO_01_008: [ When http_proxy_io_create fails, all allocated resources up to that point shall be freed. ]*/
                                free(result->username);
                                free(result->proxy_hostname);
                                free(result->hostname);
                                free(result);
                                result = NULL;
                            }
                            else
                            {
                                /* Codes_SRS_HTTP_PROXY_IO_01_010: [ - io_interface_description shall be set to the result of socketio_get_interface_description. ]*/
                                const IO_INTERFACE_DESCRIPTION* underlying_io_interface = socketio_get_interface_description();
                                if (underlying_io_interface == NULL)
                                {
                                    /* Codes_SRS_HTTP_PROXY_IO_01_050: [ If socketio_get_interface_description fails, http_proxy_io_create shall fail and return NULL. ]*/
                                    LogError("Unable to get the socket IO interface description.");
                                    /* Codes_SRS_HTTP_PROXY_IO_01_008: [ When http_proxy_io_create fails, all allocated resources up to that point shall be freed. ]*/
                                    free(result->password);
                                    free(result->username);
                                    free(result->proxy_hostname);
                                    free(result->hostname);
                                    free(result);
                                    result = NULL;
                                }
                                else
                                {
                                    SOCKETIO_CONFIG socket_io_config;

                                    /* Codes_SRS_HTTP_PROXY_IO_01_011: [ - xio_create_parameters shall be set to a SOCKETIO_CONFIG* where hostname is set to the proxy_hostname member of io_create_parameters and port is set to the proxy_port member of io_create_parameters. ]*/
                                    socket_io_config.hostname = http_proxy_io_config->proxy_hostname;
                                    socket_io_config.port = http_proxy_io_config->proxy_port;
                                    socket_io_config.accepted_socket = NULL;

                                    /* Codes_SRS_HTTP_PROXY_IO_01_009: [ http_proxy_io_create shall create a new socket IO by calling xio_create with the arguments: ]*/
                                    result->underlying_io = xio_create(underlying_io_interface, &socket_io_config);
                                    if (result->underlying_io == NULL)
                                    {
                                        /* Codes_SRS_HTTP_PROXY_IO_01_012: [ If xio_create fails, http_proxy_io_create shall fail and return NULL. ]*/
                                        LogError("Unable to create the underlying IO.");
                                        /* Codes_SRS_HTTP_PROXY_IO_01_008: [ When http_proxy_io_create fails, all allocated resources up to that point shall be freed. ]*/
                                        free(result->password);
                                        free(result->username);
                                        free(result->proxy_hostname);
                                        free(result->hostname);
                                        free(result);
                                        result = NULL;
                                    }
                                    else
                                    {
                                        result->port = http_proxy_io_config->port;
                                        result->proxy_port = http_proxy_io_config->proxy_port;
                                        result->receive_buffer = NULL;
                                        result->receive_buffer_size = 0;
                                        result->http_proxy_io_state = HTTP_PROXY_IO_STATE_CLOSED;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return result;
}

static void http_proxy_io_destroy(CONCRETE_IO_HANDLE http_proxy_io)
{
    if (http_proxy_io == NULL)
    {
        /* Codes_SRS_HTTP_PROXY_IO_01_014: [ If http_proxy_io is NULL, http_proxy_io_destroy shall do nothing. ]*/
        LogError("NULL http_proxy_io.");
    }
    else
    {
        HTTP_PROXY_IO_INSTANCE* http_proxy_io_instance = (HTTP_PROXY_IO_INSTANCE*)http_proxy_io;

        /* Codes_SRS_HTTP_PROXY_IO_01_013: [ http_proxy_io_destroy shall free the HTTP proxy IO instance indicated by http_proxy_io. ]*/
        if (http_proxy_io_instance->receive_buffer != NULL)
        {
            free(http_proxy_io_instance->receive_buffer);
        }

        /* Codes_SRS_HTTP_PROXY_IO_01_016: [ http_proxy_io_destroy shall destroy the underlying IO created in http_proxy_io_create by calling xio_destroy. ]*/
        xio_destroy(http_proxy_io_instance->underlying_io);
        free(http_proxy_io_instance->hostname);
        free(http_proxy_io_instance->proxy_hostname);
        free(http_proxy_io_instance->username);
        free(http_proxy_io_instance->password);
        free(http_proxy_io_instance);
    }
}

static void indicate_open_complete_error_and_close(HTTP_PROXY_IO_INSTANCE* http_proxy_io_instance)
{
    http_proxy_io_instance->http_proxy_io_state = HTTP_PROXY_IO_STATE_CLOSED;
    (void)xio_close(http_proxy_io_instance->underlying_io, NULL, NULL);
    http_proxy_io_instance->on_io_open_complete(http_proxy_io_instance->on_io_open_complete_context, IO_OPEN_ERROR);
}

// This callback usage needs to be either verified and commented or integrated into
// the state machine.
static void unchecked_on_send_complete(void* context, IO_SEND_RESULT send_result)
{
    (void)context;
    (void)send_result;
}

static void on_underlying_io_open_complete(void* context, IO_OPEN_RESULT open_result)
{
    if (context == NULL)
    {
        /* Codes_SRS_HTTP_PROXY_IO_01_081: [ on_underlying_io_open_complete called with NULL context shall do nothing. ]*/
        LogError("NULL context in on_underlying_io_open_complete");
    }
    else
    {
        HTTP_PROXY_IO_INSTANCE* http_proxy_io_instance = (HTTP_PROXY_IO_INSTANCE*)context;
        switch (http_proxy_io_instance->http_proxy_io_state)
        {
        default:
            LogError("on_underlying_io_open_complete called in an unexpected state.");
            break;

        case HTTP_PROXY_IO_STATE_CLOSING:
        case HTTP_PROXY_IO_STATE_OPEN:
            /* Codes_SRS_HTTP_PROXY_IO_01_077: [ When on_underlying_io_open_complete is called in after OPEN has completed, the on_io_error callback shall be triggered passing the on_io_error_context argument as context. ]*/
            http_proxy_io_instance->on_io_error(http_proxy_io_instance->on_io_error_context);
            break;

        case HTTP_PROXY_IO_STATE_WAITING_FOR_CONNECT_RESPONSE:
            /* Codes_SRS_HTTP_PROXY_IO_01_076: [ When on_underlying_io_open_complete is called while waiting for the CONNECT reply, the on_open_complete callback shall be triggered with IO_OPEN_ERROR, passing also the on_open_complete_context argument as context. ]*/
            LogError("Open complete called again by underlying IO.");
            indicate_open_complete_error_and_close(http_proxy_io_instance);
            break;

        case HTTP_PROXY_IO_STATE_OPENING_UNDERLYING_IO:
            switch (open_result)
            {
            default:
            case IO_OPEN_ERROR:
                /* Codes_SRS_HTTP_PROXY_IO_01_078: [ When on_underlying_io_open_complete is called with IO_OPEN_ERROR, the on_open_complete callback shall be triggered with IO_OPEN_ERROR, passing also the on_open_complete_context argument as context. ]*/
                LogError("Underlying IO open failed");
                indicate_open_complete_error_and_close(http_proxy_io_instance);
                break;

            case IO_OPEN_CANCELLED:
                /* Codes_SRS_HTTP_PROXY_IO_01_079: [ When on_underlying_io_open_complete is called with IO_OPEN_CANCELLED, the on_open_complete callback shall be triggered with IO_OPEN_CANCELLED, passing also the on_open_complete_context argument as context. ]*/
                LogError("Underlying IO open failed");
                http_proxy_io_instance->http_proxy_io_state = HTTP_PROXY_IO_STATE_CLOSED;
                (void)xio_close(http_proxy_io_instance->underlying_io, NULL, NULL);
                http_proxy_io_instance->on_io_open_complete(http_proxy_io_instance->on_io_open_complete_context, IO_OPEN_CANCELLED);
                break;

            case IO_OPEN_OK:
            {
                STRING_HANDLE encoded_auth_string;

                /* Codes_SRS_HTTP_PROXY_IO_01_057: [ When on_underlying_io_open_complete is called, the http_proxy_io shall send the CONNECT request constructed per RFC 2817: ]*/
                http_proxy_io_instance->http_proxy_io_state = HTTP_PROXY_IO_STATE_WAITING_FOR_CONNECT_RESPONSE;

                if (http_proxy_io_instance->username != NULL)
                {
                    char* plain_auth_string_bytes;

                    /* Codes_SRS_HTTP_PROXY_IO_01_060: [ - The value of Proxy-Authorization shall be the constructed according to RFC 2617. ]*/
                    int plain_auth_string_length = (int)(strlen(http_proxy_io_instance->username)+1);
                    if (http_proxy_io_instance->password != NULL)
                    {
                        plain_auth_string_length += (int)strlen(http_proxy_io_instance->password);
                    }

                    if (plain_auth_string_length < 0)
                    {
                        /* Codes_SRS_HTTP_PROXY_IO_01_062: [ If any failure is encountered while constructing the request, the on_open_complete callback shall be triggered with IO_OPEN_ERROR, passing also the on_open_complete_context argument as context. ]*/
                        encoded_auth_string = NULL;
                        indicate_open_complete_error_and_close(http_proxy_io_instance);
                    }
                    else
                    {
                        plain_auth_string_bytes = (char*)malloc(plain_auth_string_length + 1);
                        if (plain_auth_string_bytes == NULL)
                        {
                            /* Codes_SRS_HTTP_PROXY_IO_01_062: [ If any failure is encountered while constructing the request, the on_open_complete callback shall be triggered with IO_OPEN_ERROR, passing also the on_open_complete_context argument as context. ]*/
                            encoded_auth_string = NULL;
                            indicate_open_complete_error_and_close(http_proxy_io_instance);
                        }
                        else
                        {
                            /* Codes_SRS_HTTP_PROXY_IO_01_091: [ To receive authorization, the client sends the userid and password, separated by a single colon (":") character, within a base64 [7] encoded string in the credentials. ]*/
                            /* Codes_SRS_HTTP_PROXY_IO_01_092: [ A client MAY preemptively send the corresponding Authorization header with requests for resources in that space without receipt of another challenge from the server. ]*/
                            /* Codes_SRS_HTTP_PROXY_IO_01_093: [ Userids might be case sensitive. ]*/
                            if (sprintf(plain_auth_string_bytes, "%s:%s", http_proxy_io_instance->username, (http_proxy_io_instance->password == NULL) ? "" : http_proxy_io_instance->password) < 0)
                            {
                                /* Codes_SRS_HTTP_PROXY_IO_01_062: [ If any failure is encountered while constructing the request, the on_open_complete callback shall be triggered with IO_OPEN_ERROR, passing also the on_open_complete_context argument as context. ]*/
                                encoded_auth_string = NULL;
                                indicate_open_complete_error_and_close(http_proxy_io_instance);
                            }
                            else
                            {
                                /* Codes_SRS_HTTP_PROXY_IO_01_061: [ Encoding to Base64 shall be done by calling Azure_Base64_Encode_Bytes. ]*/
                                encoded_auth_string = Azure_Base64_Encode_Bytes((const unsigned char*)plain_auth_string_bytes, plain_auth_string_length);
                                if (encoded_auth_string == NULL)
                                {
                                    /* Codes_SRS_HTTP_PROXY_IO_01_062: [ If any failure is encountered while constructing the request, the on_open_complete callback shall be triggered with IO_OPEN_ERROR, passing also the on_open_complete_context argument as context. ]*/
                                    LogError("Cannot Base64 encode auth string");
                                    indicate_open_complete_error_and_close(http_proxy_io_instance);
                                }
                            }

                            free(plain_auth_string_bytes);
                        }
                    }
                }
                else
                {
                    encoded_auth_string = NULL;
                }

                if (http_proxy_io_instance->hostname == NULL ||
                    (http_proxy_io_instance->username != NULL && encoded_auth_string == NULL))
                {
                    LogError("Cannot create authorization header");
                }
                else
                {
                    int connect_request_length;
                    const char* auth_string_payload;
                    /* Codes_SRS_HTTP_PROXY_IO_01_075: [ The Request-URI portion of the Request-Line is always an 'authority' as defined by URI Generic Syntax [2], which is to say the host name and port number destination of the requested connection separated by a colon: ]*/
                    const char request_format[] = "CONNECT %s:%d HTTP/1.1\r\nHost:%s:%d%s%s\r\n\r\n";
                    const char proxy_basic[] = "\r\nProxy-authorization: Basic ";
                    if (http_proxy_io_instance->username != NULL)
                    {
                        auth_string_payload = STRING_c_str(encoded_auth_string);
                    }
                    else
                    {
                        auth_string_payload = "";
                    }

                    /* Codes_SRS_HTTP_PROXY_IO_01_059: [ - If username and password have been specified in the arguments passed to http_proxy_io_create, then the header Proxy-Authorization shall be added to the request. ]*/

                    connect_request_length = (int)(strlen(request_format)+(strlen(http_proxy_io_instance->hostname)*2)+strlen(auth_string_payload)+10);
                    if (http_proxy_io_instance->username != NULL)
                    {
                        connect_request_length += (int)strlen(proxy_basic);
                    }

                    if (connect_request_length < 0)
                    {
                        /* Codes_SRS_HTTP_PROXY_IO_01_062: [ If any failure is encountered while constructing the request, the on_open_complete callback shall be triggered with IO_OPEN_ERROR, passing also the on_open_complete_context argument as context. ]*/
                        LogError("Cannot encode the CONNECT request");
                        indicate_open_complete_error_and_close(http_proxy_io_instance);
                    }
                    else
                    {
                        char* connect_request = (char*)malloc(connect_request_length + 1);
                        if (connect_request == NULL)
                        {
                            /* Codes_SRS_HTTP_PROXY_IO_01_062: [ If any failure is encountered while constructing the request, the on_open_complete callback shall be triggered with IO_OPEN_ERROR, passing also the on_open_complete_context argument as context. ]*/
                            LogError("Cannot allocate memory for CONNECT request");
                            indicate_open_complete_error_and_close(http_proxy_io_instance);
                        }
                        else
                        {
                            /* Codes_SRS_HTTP_PROXY_IO_01_059: [ - If username and password have been specified in the arguments passed to http_proxy_io_create, then the header Proxy-Authorization shall be added to the request. ]*/
                            connect_request_length = sprintf(connect_request, request_format,
                                http_proxy_io_instance->hostname,
                                http_proxy_io_instance->port,
                                http_proxy_io_instance->hostname,
                                http_proxy_io_instance->port,
                                (http_proxy_io_instance->username != NULL) ? proxy_basic : "",
                                auth_string_payload);

                            if (connect_request_length < 0)
                            {
                                /* Codes_SRS_HTTP_PROXY_IO_01_062: [ If any failure is encountered while constructing the request, the on_open_complete callback shall be triggered with IO_OPEN_ERROR, passing also the on_open_complete_context argument as context. ]*/
                                LogError("Cannot encode the CONNECT request");
                                indicate_open_complete_error_and_close(http_proxy_io_instance);
                            }
                            else
                            {
                                /* Codes_SRS_HTTP_PROXY_IO_01_063: [ The request shall be sent by calling xio_send and passing NULL as on_send_complete callback. ]*/
                                if (xio_send(http_proxy_io_instance->underlying_io, connect_request, connect_request_length, unchecked_on_send_complete, NULL) != 0)
                                {
                                    /* Codes_SRS_HTTP_PROXY_IO_01_064: [ If xio_send fails, the on_open_complete callback shall be triggered with IO_OPEN_ERROR, passing also the on_open_complete_context argument as context. ]*/
                                    LogError("Could not send CONNECT request");
                                    indicate_open_complete_error_and_close(http_proxy_io_instance);
                                }
                            }

                            free(connect_request);
                        }
                    }
                }

                if (encoded_auth_string != NULL)
                {
                    STRING_delete(encoded_auth_string);
                }

                break;
            }
            }

            break;
        }
    }
}

static void on_underlying_io_error(void* context)
{
    if (context == NULL)
    {
        /* Codes_SRS_HTTP_PROXY_IO_01_088: [ on_underlying_io_error called with NULL context shall do nothing. ]*/
        LogError("NULL context in on_underlying_io_error");
    }
    else
    {
        HTTP_PROXY_IO_INSTANCE* http_proxy_io_instance = (HTTP_PROXY_IO_INSTANCE*)context;

        switch (http_proxy_io_instance->http_proxy_io_state)
        {
        default:
            LogError("on_underlying_io_error in invalid state");
            break;

        case HTTP_PROXY_IO_STATE_OPENING_UNDERLYING_IO:
        case HTTP_PROXY_IO_STATE_WAITING_FOR_CONNECT_RESPONSE:
            /* Codes_SRS_HTTP_PROXY_IO_01_087: [ If the on_underlying_io_error callback is called while OPENING, the on_open_complete callback shall be triggered with IO_OPEN_ERROR, passing also the on_open_complete_context argument as context. ]*/
            indicate_open_complete_error_and_close(http_proxy_io_instance);
            break;

        case HTTP_PROXY_IO_STATE_OPEN:
            /* Codes_SRS_HTTP_PROXY_IO_01_089: [ If the on_underlying_io_error callback is called while the IO is OPEN, the on_io_error callback shall be called with the on_io_error_context argument as context. ]*/
            http_proxy_io_instance->http_proxy_io_state = HTTP_PROXY_IO_STATE_ERROR;
            http_proxy_io_instance->on_io_error(http_proxy_io_instance->on_io_error_context);
            break;
        }
    }
}

static void on_underlying_io_close_complete(void* context)
{
    if (context == NULL)
    {
        /* Cdoes_SRS_HTTP_PROXY_IO_01_084: [ on_underlying_io_close_complete called with NULL context shall do nothing. ]*/
        LogError("NULL context in on_underlying_io_open_complete");
    }
    else
    {
        HTTP_PROXY_IO_INSTANCE* http_proxy_io_instance = (HTTP_PROXY_IO_INSTANCE*)context;

        switch (http_proxy_io_instance->http_proxy_io_state)
        {
        default:
            LogError("on_underlying_io_close_complete called in an invalid state");
            break;

        case HTTP_PROXY_IO_STATE_CLOSING:
            http_proxy_io_instance->http_proxy_io_state = HTTP_PROXY_IO_STATE_CLOSED;

            /* Codes_SRS_HTTP_PROXY_IO_01_086: [ If the on_io_close_complete callback passed to http_proxy_io_close was NULL, no callback shall be triggered. ]*/
            if (http_proxy_io_instance->on_io_close_complete != NULL)
            {
                /* Codes_SRS_HTTP_PROXY_IO_01_083: [ on_underlying_io_close_complete while CLOSING shall call the on_io_close_complete callback, passing to it the on_io_close_complete_context as context argument. ]*/
                http_proxy_io_instance->on_io_close_complete(http_proxy_io_instance->on_io_close_complete_context);
            }

            break;
        }
    }
}

/*the following function does the same as sscanf(pos2, "%d", &sec)*/
/*this function only exists because some of platforms do not have sscanf. */
static int ParseStringToDecimal(const char *src, int* dst)
{
    int result;
    char* next;

    (*dst) = (int)strtol(src, &next, 0);
    if ((src == next) || ((((*dst) == INT_MAX) || ((*dst) == INT_MIN)) && (errno != 0)))
    {
        result = __LINE__;
    }
    else
    {
        result = 0;
    }

    return result;
}

/*the following function does the same as sscanf(buf, "HTTP/%*d.%*d %d %*[^\r\n]", &ret) */
/*this function only exists because some of platforms do not have sscanf. This is not a full implementation; it only works with well-defined HTTP response. */
static int ParseHttpResponse(const char* src, int* dst)
{
    int result;
    static const char HTTPPrefix[] = "HTTP/";
    bool fail;
    const char* runPrefix;

    if ((src == NULL) || (dst == NULL))
    {
        result = __LINE__;
    }
    else
    {
        fail = false;
        runPrefix = HTTPPrefix;

        while ((*runPrefix) != '\0')
        {
            if ((*runPrefix) != (*src))
            {
                fail = true;
                break;
            }
            src++;
            runPrefix++;
        }

        if (!fail)
        {
            while ((*src) != '.')
            {
                if ((*src) == '\0')
                {
                    fail = true;
                    break;
                }
                src++;
            }
        }

        if (!fail)
        {
            while ((*src) != ' ')
            {
                if ((*src) == '\0')
                {
                    fail = true;
                    break;
                }
                src++;
            }
        }

        if (fail)
        {
            result = __LINE__;
        }
        else
        {
            if (ParseStringToDecimal(src, dst) != 0)
            {
                result = __LINE__;
            }
            else
            {
                result = 0;
            }
        }
    }

    return result;
}

static void on_underlying_io_bytes_received(void* context, const unsigned char* buffer, size_t size)
{
    if (context == NULL)
    {
        /* Codes_SRS_HTTP_PROXY_IO_01_082: [ on_underlying_io_bytes_received called with NULL context shall do nothing. ]*/
        LogError("NULL context in on_underlying_io_bytes_received");
    }
    else
    {
        HTTP_PROXY_IO_INSTANCE* http_proxy_io_instance = (HTTP_PROXY_IO_INSTANCE*)context;

        switch (http_proxy_io_instance->http_proxy_io_state)
        {
        default:
        case HTTP_PROXY_IO_STATE_CLOSING:
            LogError("Bytes received in invalid state");
            break;

        case HTTP_PROXY_IO_STATE_OPENING_UNDERLYING_IO:
            /* Codes_SRS_HTTP_PROXY_IO_01_080: [ If on_underlying_io_bytes_received is called while the underlying IO is being opened, the on_open_complete callback shall be triggered with IO_OPEN_ERROR, passing also the on_open_complete_context argument as context. ]*/
            LogError("Bytes received while opening underlying IO");
            indicate_open_complete_error_and_close(http_proxy_io_instance);
            break;

        case HTTP_PROXY_IO_STATE_WAITING_FOR_CONNECT_RESPONSE:
        {
            /* Codes_SRS_HTTP_PROXY_IO_01_065: [ When bytes are received and the response to the CONNECT request was not yet received, the bytes shall be accumulated until a double new-line is detected. ]*/
            unsigned char* new_receive_buffer = (unsigned char*)realloc(http_proxy_io_instance->receive_buffer, http_proxy_io_instance->receive_buffer_size + size + 1);
            if (new_receive_buffer == NULL)
            {
                /* Codes_SRS_HTTP_PROXY_IO_01_067: [ If allocating memory for the buffered bytes fails, the on_open_complete callback shall be triggered with IO_OPEN_ERROR, passing also the on_open_complete_context argument as context. ]*/
                LogError("Cannot allocate memory for received data");
                indicate_open_complete_error_and_close(http_proxy_io_instance);
            }
            else
            {
                http_proxy_io_instance->receive_buffer = new_receive_buffer;
                memcpy(http_proxy_io_instance->receive_buffer + http_proxy_io_instance->receive_buffer_size, buffer, size);
                http_proxy_io_instance->receive_buffer_size += size;
            }

            if (http_proxy_io_instance->receive_buffer_size >= 4)
            {
                const char* request_end_ptr;

                http_proxy_io_instance->receive_buffer[http_proxy_io_instance->receive_buffer_size] = 0;

                /* Codes_SRS_HTTP_PROXY_IO_01_066: [ When a double new-line is detected the response shall be parsed in order to extract the status code. ]*/
                if ((http_proxy_io_instance->receive_buffer_size >= 4) &&
                    ((request_end_ptr = strstr((const char*)http_proxy_io_instance->receive_buffer, "\r\n\r\n")) != NULL))
                {
                    int status_code;

                    /* This part should really be done with the HTTPAPI, but that has to be done as a separate step
                    as the HTTPAPI has to expose somehow the underlying IO and currently this would be a too big of a change. */

                    if (ParseHttpResponse((const char*)http_proxy_io_instance->receive_buffer, &status_code) != 0)
                    {
                        /* Codes_SRS_HTTP_PROXY_IO_01_068: [ If parsing the CONNECT response fails, the on_open_complete callback shall be triggered with IO_OPEN_ERROR, passing also the on_open_complete_context argument as context. ]*/
                        LogError("Cannot decode HTTP response");
                        indicate_open_complete_error_and_close(http_proxy_io_instance);
                    }
                    /* Codes_SRS_HTTP_PROXY_IO_01_069: [ Any successful (2xx) response to a CONNECT request indicates that the proxy has established a connection to the requested host and port, and has switched to tunneling the current connection to that server connection. ]*/
                    /* Codes_SRS_HTTP_PROXY_IO_01_090: [ Any successful (2xx) response to a CONNECT request indicates that the proxy has established a connection to the requested host and port, and has switched to tunneling the current connection to that server connection. ]*/
                    else if ((status_code < 200) || (status_code > 299))
                    {
                        /* Codes_SRS_HTTP_PROXY_IO_01_071: [ If the status code is not successful, the on_open_complete callback shall be triggered with IO_OPEN_ERROR, passing also the on_open_complete_context argument as context. ]*/
                        LogError("Bad status (%d) received in CONNECT response", status_code);
                        indicate_open_complete_error_and_close(http_proxy_io_instance);
                    }
                    else
                    {
                        size_t length_remaining = http_proxy_io_instance->receive_buffer + http_proxy_io_instance->receive_buffer_size - ((const unsigned char *)request_end_ptr + 4);

                        /* Codes_SRS_HTTP_PROXY_IO_01_073: [ Once a success status code was parsed, the IO shall be OPEN. ]*/
                        http_proxy_io_instance->http_proxy_io_state = HTTP_PROXY_IO_STATE_OPEN;
                        /* Codes_SRS_HTTP_PROXY_IO_01_070: [ When a success status code is parsed, the on_open_complete callback shall be triggered with IO_OPEN_OK, passing also the on_open_complete_context argument as context. ]*/
                        http_proxy_io_instance->on_io_open_complete(http_proxy_io_instance->on_io_open_complete_context, IO_OPEN_OK);

                        if (length_remaining > 0)
                        {
                            /* Codes_SRS_HTTP_PROXY_IO_01_072: [ Any bytes that are extra (not consumed by the CONNECT response), shall be indicated as received by calling the on_bytes_received callback and passing the on_bytes_received_context as context argument. ]*/
                            http_proxy_io_instance->on_bytes_received(http_proxy_io_instance->on_bytes_received_context, (const unsigned char*)request_end_ptr + 4, length_remaining);
                        }
                    }
                }
            }
            break;
        }
        case HTTP_PROXY_IO_STATE_OPEN:
            /* Codes_SRS_HTTP_PROXY_IO_01_074: [ If on_underlying_io_bytes_received is called while OPEN, all bytes shall be indicated as received by calling the on_bytes_received callback and passing the on_bytes_received_context as context argument. ]*/
            http_proxy_io_instance->on_bytes_received(http_proxy_io_instance->on_bytes_received_context, buffer, size);
            break;
        }
    }
}

static int http_proxy_io_open(CONCRETE_IO_HANDLE http_proxy_io, ON_IO_OPEN_COMPLETE on_io_open_complete, void* on_io_open_complete_context, ON_BYTES_RECEIVED on_bytes_received, void* on_bytes_received_context, ON_IO_ERROR on_io_error, void* on_io_error_context)
{
    int result;

    /* Codes_SRS_HTTP_PROXY_IO_01_051: [ The arguments on_io_open_complete_context, on_bytes_received_context and on_io_error_context shall be allowed to be NULL. ]*/
    /* Codes_SRS_HTTP_PROXY_IO_01_018: [ If any of the arguments http_proxy_io, on_io_open_complete, on_bytes_received or on_io_error are NULL then http_proxy_io_open shall return a non-zero value. ]*/
    if ((http_proxy_io == NULL) ||
        (on_io_open_complete == NULL) ||
        (on_bytes_received == NULL) ||
        (on_io_error == NULL))
    {
        LogError("Bad arguments: http_proxy_io = %p, on_io_open_complete = %p, on_bytes_received = %p, on_io_error_context = %p.",
            http_proxy_io,
            on_io_open_complete,
            on_bytes_received,
            on_io_error);
        result = __LINE__;
    }
    else
    {
        HTTP_PROXY_IO_INSTANCE* http_proxy_io_instance = (HTTP_PROXY_IO_INSTANCE*)http_proxy_io;

        if (http_proxy_io_instance->http_proxy_io_state != HTTP_PROXY_IO_STATE_CLOSED)
        {
            LogError("Invalid tlsio_state. Expected state is HTTP_PROXY_IO_STATE_CLOSED.");
            result = __LINE__;
        }
        else
        {
            http_proxy_io_instance->on_bytes_received = on_bytes_received;
            http_proxy_io_instance->on_bytes_received_context = on_bytes_received_context;

            http_proxy_io_instance->on_io_error = on_io_error;
            http_proxy_io_instance->on_io_error_context = on_io_error_context;

            http_proxy_io_instance->on_io_open_complete = on_io_open_complete;
            http_proxy_io_instance->on_io_open_complete_context = on_io_open_complete_context;

            http_proxy_io_instance->http_proxy_io_state = HTTP_PROXY_IO_STATE_OPENING_UNDERLYING_IO;

            /* Codes_SRS_HTTP_PROXY_IO_01_019: [ http_proxy_io_open shall open the underlying IO by calling xio_open on the underlying IO handle created in http_proxy_io_create, while passing to it the callbacks on_underlying_io_open_complete, on_underlying_io_bytes_received and on_underlying_io_error. ]*/
            if (xio_open(http_proxy_io_instance->underlying_io, on_underlying_io_open_complete, http_proxy_io_instance, on_underlying_io_bytes_received, http_proxy_io_instance, on_underlying_io_error, http_proxy_io_instance) != 0)
            {
                /* Codes_SRS_HTTP_PROXY_IO_01_020: [ If xio_open fails, then http_proxy_io_open shall return a non-zero value. ]*/
                http_proxy_io_instance->http_proxy_io_state = HTTP_PROXY_IO_STATE_CLOSED;
                LogError("Cannot open the underlying IO.");
                result = __LINE__;
            }
            else
            {
                /* Codes_SRS_HTTP_PROXY_IO_01_017: [ http_proxy_io_open shall open the HTTP proxy IO and on success it shall return 0. ]*/
                result = 0;
            }
        }
    }

    return result;
}

static int http_proxy_io_close(CONCRETE_IO_HANDLE http_proxy_io, ON_IO_CLOSE_COMPLETE on_io_close_complete, void* on_io_close_complete_context)
{
    int result = 0;

    /* Codes_SRS_HTTP_PROXY_IO_01_052: [ on_io_close_complete_context shall be allowed to be NULL. ]*/
    /* Codes_SRS_HTTP_PROXY_IO_01_028: [ on_io_close_complete shall be allowed to be NULL. ]*/
    if (http_proxy_io == NULL)
    {
        /* Codes_SRS_HTTP_PROXY_IO_01_023: [ If the argument http_proxy_io is NULL, http_proxy_io_close shall fail and return a non-zero value. ]*/
        result = __LINE__;
        LogError("NULL http_proxy_io.");
    }
    else
    {
        HTTP_PROXY_IO_INSTANCE* http_proxy_io_instance = (HTTP_PROXY_IO_INSTANCE*)http_proxy_io;

        /* Codes_SRS_HTTP_PROXY_IO_01_027: [ If http_proxy_io_close is called when not open, http_proxy_io_close shall fail and return a non-zero value. ]*/
        if ((http_proxy_io_instance->http_proxy_io_state == HTTP_PROXY_IO_STATE_CLOSED) ||
            /* Codes_SRS_HTTP_PROXY_IO_01_054: [ http_proxy_io_close while OPENING shall fail and return a non-zero value. ]*/
            (http_proxy_io_instance->http_proxy_io_state == HTTP_PROXY_IO_STATE_CLOSING))
        {
            result = __LINE__;
            LogError("Invalid tlsio_state. Expected state is HTTP_PROXY_IO_STATE_OPEN.");
        }
        else if ((http_proxy_io_instance->http_proxy_io_state == HTTP_PROXY_IO_STATE_OPENING_UNDERLYING_IO) ||
            (http_proxy_io_instance->http_proxy_io_state == HTTP_PROXY_IO_STATE_WAITING_FOR_CONNECT_RESPONSE))
        {
            /* Codes_SRS_HTTP_PROXY_IO_01_053: [ http_proxy_io_close while OPENING shall trigger the on_io_open_complete callback with IO_OPEN_CANCELLED. ]*/
            http_proxy_io_instance->http_proxy_io_state = HTTP_PROXY_IO_STATE_CLOSED;
            (void)xio_close(http_proxy_io_instance->underlying_io, NULL, NULL);
            http_proxy_io_instance->on_io_open_complete(http_proxy_io_instance->on_io_open_complete_context, IO_OPEN_CANCELLED);

            /* Codes_SRS_HTTP_PROXY_IO_01_022: [ http_proxy_io_close shall close the HTTP proxy IO and on success it shall return 0. ]*/
            result = 0;
        }
        else
        {
            HTTP_PROXY_IO_STATE previous_state = http_proxy_io_instance->http_proxy_io_state;

            http_proxy_io_instance->http_proxy_io_state = HTTP_PROXY_IO_STATE_CLOSING;

            /* Codes_SRS_HTTP_PROXY_IO_01_026: [ The on_io_close_complete and on_io_close_complete_context arguments shall be saved for later use. ]*/
            http_proxy_io_instance->on_io_close_complete = on_io_close_complete;
            http_proxy_io_instance->on_io_close_complete_context = on_io_close_complete_context;

            /* Codes_SRS_HTTP_PROXY_IO_01_024: [ http_proxy_io_close shall close the underlying IO by calling xio_close on the IO handle create in http_proxy_io_create, while passing to it the on_underlying_io_close_complete callback. ]*/
            if (xio_close(http_proxy_io_instance->underlying_io, on_underlying_io_close_complete, http_proxy_io_instance) != 0)
            {
                /* Codes_SRS_HTTP_PROXY_IO_01_025: [ If xio_close fails, http_proxy_io_close shall fail and return a non-zero value. ]*/
                result = __LINE__;
                http_proxy_io_instance->http_proxy_io_state = previous_state;
                LogError("Cannot close underlying IO.");
            }
            else
            {
                /* Codes_SRS_HTTP_PROXY_IO_01_022: [ http_proxy_io_close shall close the HTTP proxy IO and on success it shall return 0. ]*/
                result = 0;
            }
        }
    }

    return result;
}

static int http_proxy_io_send(CONCRETE_IO_HANDLE http_proxy_io, const void* buffer, size_t size, ON_SEND_COMPLETE on_send_complete, void* on_send_complete_context)
{
    int result;

    /* Codes_SRS_HTTP_PROXY_IO_01_032: [ on_send_complete shall be allowed to be NULL. ]*/
    /* Codes_SRS_HTTP_PROXY_IO_01_030: [ If any of the arguments http_proxy_io or buffer is NULL, http_proxy_io_send shall fail and return a non-zero value. ]*/
    if ((http_proxy_io == NULL) ||
        (buffer == NULL) ||
        /* Codes_SRS_HTTP_PROXY_IO_01_031: [ If size is 0, http_proxy_io_send shall fail and return a non-zero value. ]*/
        (size == 0))
    {
        result = __LINE__;
        LogError("Bad arguments: http_proxy_io = %p, buffer = %p.",
            http_proxy_io, buffer);
    }
    else
    {
        HTTP_PROXY_IO_INSTANCE* http_proxy_io_instance = (HTTP_PROXY_IO_INSTANCE*)http_proxy_io;

        /* Codes_SRS_HTTP_PROXY_IO_01_034: [ If http_proxy_io_send is called when the IO is not open, http_proxy_io_send shall fail and return a non-zero value. ]*/
        /* Codes_SRS_HTTP_PROXY_IO_01_035: [ If the IO is in an error state (an error was reported through the on_io_error callback), http_proxy_io_send shall fail and return a non-zero value. ]*/
        if (http_proxy_io_instance->http_proxy_io_state != HTTP_PROXY_IO_STATE_OPEN)
        {
            result = __LINE__;
            LogError("Invalid HTTP proxy IO state. Expected state is HTTP_PROXY_IO_STATE_OPEN.");
        }
        else
        {
            /* Codes_SRS_HTTP_PROXY_IO_01_033: [ http_proxy_io_send shall send the bytes by calling xio_send on the underlying IO created in http_proxy_io_create and passing buffer and size as arguments. ]*/
            if (xio_send(http_proxy_io_instance->underlying_io, buffer, size, on_send_complete, on_send_complete_context) != 0)
            {
                /* Codes_SRS_HTTP_PROXY_IO_01_055: [ If xio_send fails, http_proxy_io_send shall fail and return a non-zero value. ]*/
                result = __LINE__;
                LogError("Underlying xio_send failed.");
            }
            else
            {
                /* Codes_SRS_HTTP_PROXY_IO_01_029: [ http_proxy_io_send shall send the size bytes pointed to by buffer and on success it shall return 0. ]*/
                result = 0;
            }
        }
    }

    return result;
}

static void http_proxy_io_dowork(CONCRETE_IO_HANDLE http_proxy_io)
{
    if (http_proxy_io == NULL)
    {
        /* Codes_SRS_HTTP_PROXY_IO_01_038: [ If the http_proxy_io argument is NULL, http_proxy_io_dowork shall do nothing. ]*/
        LogError("NULL http_proxy_io.");
    }
    else
    {
        HTTP_PROXY_IO_INSTANCE* http_proxy_io_instance = (HTTP_PROXY_IO_INSTANCE*)http_proxy_io;

        if (http_proxy_io_instance->http_proxy_io_state != HTTP_PROXY_IO_STATE_CLOSED)
        {
            /* Codes_SRS_HTTP_PROXY_IO_01_037: [ http_proxy_io_dowork shall call xio_dowork on the underlying IO created in http_proxy_io_create. ]*/
            xio_dowork(http_proxy_io_instance->underlying_io);
        }
    }
}

static int http_proxy_io_set_option(CONCRETE_IO_HANDLE http_proxy_io, const char* option_name, const void* value)
{
    int result;

    if ((http_proxy_io == NULL) || (option_name == NULL))
    {
        /* Codes_SRS_HTTP_PROXY_IO_01_040: [ If any of the arguments http_proxy_io or option_name is NULL, http_proxy_io_set_option shall return a non-zero value. ]*/
        LogError("Bad arguments: http_proxy_io = %p, option_name = %p",
            http_proxy_io, option_name);
        result = MU_FAILURE;
    }
    else
    {
        HTTP_PROXY_IO_INSTANCE* http_proxy_io_instance = (HTTP_PROXY_IO_INSTANCE*)http_proxy_io;

        /* Codes_SRS_HTTP_PROXY_IO_01_045: [ None. ]*/

        if (strcmp(option_name, OPTION_UNDERLYING_IO_OPTIONS) == 0)
        {
            if (OptionHandler_FeedOptions((OPTIONHANDLER_HANDLE)value, (void*)http_proxy_io_instance->underlying_io) != OPTIONHANDLER_OK)
            {
                LogError("failed feeding options to underlying I/O instance");
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }
        }
        /* Codes_SRS_HTTP_PROXY_IO_01_043: [ If the option_name argument indicates an option that is not handled by http_proxy_io_set_option, then xio_setoption shall be called on the underlying IO created in http_proxy_io_create, passing the option name and value to it. ]*/
        /* Codes_SRS_HTTP_PROXY_IO_01_056: [ The value argument shall be allowed to be NULL. ]*/
        else if (xio_setoption(http_proxy_io_instance->underlying_io, option_name, value) != 0)
        {
            /* Codes_SRS_HTTP_PROXY_IO_01_044: [ if xio_setoption fails, http_proxy_io_set_option shall return a non-zero value. ]*/
            LogError("Unrecognized option %s", option_name);
            result = MU_FAILURE;
        }
        else
        {
            /* Codes_SRS_HTTP_PROXY_IO_01_042: [ If the option was handled by http_proxy_io_set_option or the underlying IO, then http_proxy_io_set_option shall return 0. ]*/
            result = 0;
        }
    }

    return result;
}

/*this function will clone an option given by name and value*/
static void* http_proxy_io_clone_option(const char* name, const void* value)
{
    void* result;
    if (
        (name == NULL) || (value == NULL)
        )
    {
        LogError("invalid parameter detected: name=%p, value=%p", name, value);
        result = NULL;
    }
    else
    {
        if (strcmp(name, OPTION_UNDERLYING_IO_OPTIONS) == 0)
        {
            result = (void*)value;
        }
        else
        {
            LogError("not handled option : %s", name);
            result = NULL;
        }
    }

    return result;
}

/*this function destroys an option previously created*/
static void http_proxy_io_destroy_option(const char* name, const void* value)
{
    /*since all options for this layer are actually string copies., disposing of one is just calling free*/
    if (
        (name == NULL) || (value == NULL)
        )
    {
        LogError("invalid parameter detected: const char* name=%p, const void* value=%p", name, value);
    }
    else
    {
        if (strcmp(name, OPTION_UNDERLYING_IO_OPTIONS) == 0)
        {
            OptionHandler_Destroy((OPTIONHANDLER_HANDLE)value);
        }
        else
        {
            LogError("not handled option : %s", name);
        }
    }
}

static OPTIONHANDLER_HANDLE http_proxy_io_retrieve_options(CONCRETE_IO_HANDLE http_proxy_io)
{
    OPTIONHANDLER_HANDLE result;

    if (http_proxy_io == NULL)
    {
        /* Codes_SRS_HTTP_PROXY_IO_01_047: [ If the parameter http_proxy_io is NULL then http_proxy_io_retrieve_options shall fail and return NULL. ]*/
        LogError("invalid parameter detected: CONCRETE_IO_HANDLE handle=%p", http_proxy_io);
        result = NULL;
    }
    else
    {
        HTTP_PROXY_IO_INSTANCE* http_proxy_io_instance = (HTTP_PROXY_IO_INSTANCE*)http_proxy_io;

        result = OptionHandler_Create(http_proxy_io_clone_option, http_proxy_io_destroy_option, http_proxy_io_set_option);

        if (result == NULL)
        {
            LogError("OptionHandler_Create failed");
        }
        else
        {
            OPTIONHANDLER_HANDLE underlying_io_options;

            /* Codes_SRS_HTTP_PROXY_IO_01_046: [ http_proxy_io_retrieve_options shall return an OPTIONHANDLER_HANDLE obtained by calling xio_retrieveoptions on the underlying IO created in http_proxy_io_create. ]*/
            if ((underlying_io_options = xio_retrieveoptions(http_proxy_io_instance->underlying_io)) == NULL ||
                OptionHandler_AddOption(result, OPTION_UNDERLYING_IO_OPTIONS, underlying_io_options) != OPTIONHANDLER_OK)
            {
                /* Codes_SRS_HTTP_PROXY_IO_01_048: [ If xio_retrieveoptions fails, http_proxy_io_retrieve_options shall return NULL. ]*/
                LogError("unable to save underlying_io options");
                OptionHandler_Destroy(underlying_io_options);
                OptionHandler_Destroy(result);
                result = NULL;
            }
            else
            {
                // All is fine.
            }
        }
    }
    return result;
}

static const IO_INTERFACE_DESCRIPTION http_proxy_io_interface_description =
{
    http_proxy_io_retrieve_options,
    http_proxy_io_create,
    http_proxy_io_destroy,
    http_proxy_io_open,
    http_proxy_io_close,
    http_proxy_io_send,
    http_proxy_io_dowork,
    http_proxy_io_set_option
};

const IO_INTERFACE_DESCRIPTION* http_proxy_io_get_interface_description(void)
{
    /* Codes_SRS_HTTP_PROXY_IO_01_049: [ http_proxy_io_get_interface_description shall return a pointer to an IO_INTERFACE_DESCRIPTION structure that contains pointers to the functions: http_proxy_io_retrieve_options, http_proxy_io_retrieve_create, http_proxy_io_destroy, http_proxy_io_open, http_proxy_io_close, http_proxy_io_send and http_proxy_io_dowork. ]*/
    return &http_proxy_io_interface_description;
}
