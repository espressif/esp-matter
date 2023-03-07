// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <ctype.h>

#include "umock_c/umock_c_prod.h"
#include "azure_c_shared_utility/gballoc.h"

#include <string.h>
#include "azure_uhttp_c/uhttp.h"
#include "azure_c_shared_utility/httpheaders.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/buffer_.h"
#include "azure_c_shared_utility/singlylinkedlist.h"
#include "azure_c_shared_utility/shared_util_options.h"
#include "azure_c_shared_utility/optimize_size.h"

#define MAX_HOSTNAME        64
#define TIME_MAX_BUFFER     16
#define HTTP_CRLF_LEN       2
#define HTTP_END_TOKEN_LEN  4
#define MAX_CONTENT_LENGTH  16

static const char* HTTP_REQUEST_LINE_FMT = "%s %s HTTP/1.1\r\n";
static const char* HTTP_HOST = "Host";
// The following header names MUST be lowercase as they are used for HTTP response header comparison:
static const char* HTTP_CONTENT_LEN = "content-length";
static const char* HTTP_TRANSFER_ENCODING = "transfer-encoding";
static const char* HTTP_CRLF_VALUE = "\r\n";

typedef enum RESPONSE_MESSAGE_STATE_TAG
{
    state_initial,
    state_opening,
    state_open,
    state_process_status_line,
    state_process_headers,
    state_process_body,
    state_process_chunked_body,

    state_send_user_callback,
    state_parse_complete,

    state_closing,
    state_closed,
    state_error
} RESPONSE_MESSAGE_STATE;

typedef struct HTTP_RECV_DATA_TAG
{
    ON_HTTP_REQUEST_CALLBACK on_request_callback;
    void* user_ctx;
    int status_code;
    RESPONSE_MESSAGE_STATE recv_state;
    HTTP_HEADERS_HANDLE resp_header;
    BUFFER_HANDLE msg_body;
    size_t total_body_len;
    BUFFER_HANDLE accrual_buff;
    bool chunked_reply;
} HTTP_RECV_DATA;

typedef struct HTTP_CLIENT_HANDLE_DATA_TAG
{
    XIO_HANDLE xio_handle;
    ON_HTTP_OPEN_COMPLETE_CALLBACK on_connect;
    void* connect_user_ctx;
    ON_HTTP_ERROR_CALLBACK on_error;
    void* error_user_ctx;
    ON_HTTP_CLOSED_CALLBACK on_close_callback;
    void* close_user_ctx;
    HTTP_RECV_DATA recv_msg;
    bool chunk_request;
    bool trace_on;
    bool trace_body;
    char* host_name;
    int port_num;
    SINGLYLINKEDLIST_HANDLE data_list;
    bool cert_type_ecc;
    char* x509_cert;
    char* x509_pk;
    char* certificate;
    int connected;
} HTTP_CLIENT_HANDLE_DATA;

typedef struct HTTP_SEND_DATA_TAG
{
    HTTP_CLIENT_REQUEST_TYPE request_type;
    STRING_HANDLE relative_path;
    STRING_HANDLE header_line;
    BUFFER_HANDLE content;
} HTTP_SEND_DATA;

static void send_complete_callback(void* context, IO_SEND_RESULT send_result)
{
    (void)context;(void)send_result;
}

static int initialize_received_data(HTTP_CLIENT_HANDLE_DATA* http_data)
{
    int result = 0;

    // Initialize data if necessary
    if (http_data->recv_msg.resp_header == NULL)
    {
        http_data->recv_msg.resp_header = HTTPHeaders_Alloc();
        if (http_data->recv_msg.resp_header == NULL)
        {
            /* Codes_SRS_UHTTP_07_048: [ If any error is encountered on_bytes_received shall set the state to error. ] */
            LogError("Failure creating Http header.");
            result = MU_FAILURE;
        }
    }
    if (result == 0 && http_data->recv_msg.accrual_buff == NULL)
    {
        http_data->recv_msg.accrual_buff = BUFFER_new();
        if (http_data->recv_msg.accrual_buff == NULL)
        {
            /* Codes_SRS_UHTTP_07_048: [ If any error is encountered on_bytes_received shall set the state to error. ] */
            LogError("Failure creating accrual buffer.");
            result = MU_FAILURE;
        }
    }
    http_data->recv_msg.chunked_reply = false;
    return result;
}

static int process_status_code_line(const unsigned char* buffer, size_t len, size_t* position, int* statusLen)
{
    int result = MU_FAILURE;
    size_t index;
    int spaceFound = 0;
    const char* initSpace = NULL;
    char status_code[4] = { 0 };

    for (index = 0; index < len; index++)
    {
        if (buffer[index] == ' ')
        {
            if (spaceFound == 1)
            {
                (void)memcpy(status_code, initSpace, 3);
                status_code[3] = '\0';
            }
            else
            {
                initSpace = (const char*)buffer + index + 1;
            }
            spaceFound++;
        }
        else if (buffer[index] == '\n')
        {
            *statusLen = (int)atol(status_code);
            if (index < len)
            {
                *position = index + 1;
            }
            else
            {
                *position = index;
            }
            result = 0;
            break;
        }
    }
    return result;
}

static int process_header_line(const unsigned char* buffer, size_t len, size_t* position, HTTP_HEADERS_HANDLE resp_header, size_t* contentLen, bool* isChunked)
{
    int result = MU_FAILURE;
    size_t index;
    const unsigned char* targetPos = buffer;
    bool crlfEncounted = false;
    bool colonEncountered = false;
    char* headerKey = NULL;
    bool continueProcessing = true;

    for (index = 0; index < len && continueProcessing; index++)
    {
        if (buffer[index] == ':' && !colonEncountered)
        {
            colonEncountered = true;
            size_t keyLen = (&buffer[index]) - targetPos;

            if (keyLen == 0)
            {
                LogError("Invalid header name with zero length.");
                result = MU_FAILURE;
                continueProcessing = false;
            }
            else
            {
                if (headerKey != NULL)
                {
                    free(headerKey);
                    headerKey = NULL;
                }
                headerKey = (char*)malloc(keyLen + 1);
                if (headerKey == NULL)
                {
                    result = MU_FAILURE;
                    continueProcessing = false;
                }
                else
                {
                    memcpy(headerKey, targetPos, keyLen);
                    headerKey[keyLen] = '\0';

                    // Convert to lower case
                    for (size_t inner = 0; inner < keyLen; inner++)
                    {
                        headerKey[inner] = (char)tolower(headerKey[inner]);
                    }

                    targetPos = buffer+index+1;
                    crlfEncounted = false;
                }
            }
        }
        else if (buffer[index] == '\r')
        {
            if (headerKey != NULL)
            {
                // Remove leading spaces
                while (*targetPos == 32) { targetPos++; }

                size_t valueLen = (&buffer[index])-targetPos;
                char* headerValue = (char*)malloc(valueLen+1);
                if (headerValue == NULL)
                {
                    result = MU_FAILURE;
                    continueProcessing = false;
                }
                else
                {
                    memcpy(headerValue, targetPos, valueLen);
                    headerValue[valueLen] = '\0';

                    if (HTTPHeaders_AddHeaderNameValuePair(resp_header, headerKey, headerValue) != HTTP_HEADERS_OK)
                    {
                        result = MU_FAILURE;
                        continueProcessing = false;
                    }
                    else
                    {
                        if (strcmp(headerKey, HTTP_CONTENT_LEN) == 0)
                        {
                            *isChunked = false;
                            *contentLen = atol(headerValue);
                        }
                        else if (strcmp(headerKey, HTTP_TRANSFER_ENCODING) == 0)
                        {
                            *isChunked = true;
                            *contentLen = 0;
                        }
                        if (index < len)
                        {
                            *position = index;
                        }
                        else
                        {
                            *position = index-1;
                        }
                    }
                }
                free(headerKey);
                headerKey = NULL;
                free(headerValue);
            }
        }
        else if (buffer[index] == '\n')
        {
            if (crlfEncounted)
            {
                if (index < len)
                {
                    *position = index+1;
                }
                else
                {
                    *position = index;
                }
                result = 0;
                break;
            }
            else
            {
                colonEncountered = false;
                crlfEncounted = true;
                targetPos = buffer+index+1;
            }
        }
        else
        {
            crlfEncounted = false;
        }
    }
    if (headerKey != NULL)
    {
        free(headerKey);
    }
    return result;
}

static int write_text_line(HTTP_CLIENT_HANDLE_DATA* http_data, const char* text_line)
{
    int result;
    if (xio_send(http_data->xio_handle, text_line, strlen(text_line), send_complete_callback, NULL) != 0)
    {
        LogError("Failure calling xio_send.");
        result = MU_FAILURE;
    }
    else
    {
        result = 0;
        if (http_data->trace_on)
        {
            LOG(AZ_LOG_TRACE, LOG_LINE, "%s", text_line);
        }
    }
    return result;
}

static int write_data_line(HTTP_CLIENT_HANDLE_DATA* http_data, const unsigned char* data_line, size_t length)
{
    int result;
    if (xio_send(http_data->xio_handle, data_line, length, send_complete_callback, NULL) != 0)
    {
        LogError("Failure calling xio_send.");
        result = MU_FAILURE;
    }
    else
    {
        result = 0;
        if (http_data->trace_on)
        {
            if (length > 0)
            {
                if (http_data->trace_body)
                {
                    LOG(AZ_LOG_TRACE, LOG_LINE, "len: %d\r\n%.*s\r\n", (int)length, (int)length, data_line);
                }
                else
                {
                    LOG(AZ_LOG_TRACE, LOG_LINE, "<data> len: %d\r\n", (int)length);
                }
            }
            else
            {
                LOG(AZ_LOG_TRACE, LOG_LINE, "len: %d\r\n", (int)length);
            }
        }
    }
    return result;
}

static int convert_char_to_hex(const unsigned char* hexText, size_t len)
{
    int result = 0;
    for (size_t index = 0; index < len; index++)
    {
        if (hexText[index] == ';')
        {
            break;
        }
        else
        {
            int accumulator = 0;
            if (hexText[index] >= 48 && hexText[index] <= 57)
            {
                accumulator = hexText[index] - 48;
            }
            else if (hexText[index] >= 65 && hexText[index] <= 70)
            {
                accumulator = hexText[index] - 55;
            }
            else if (hexText[index] >= 97 && hexText[index] <= 102)
            {
                accumulator = hexText[index] - 87;
            }
            if (index > 0)
            {
                result = result << 4;
            }
            result += accumulator;
        }
    }
    return result;
}

static int setup_init_recv_msg(HTTP_RECV_DATA* recv_msg)
{
    int result;
    recv_msg->status_code = 0;
    recv_msg->recv_state = state_initial;
    recv_msg->total_body_len = 0;
    if (recv_msg->resp_header != NULL)
    {
        HTTPHeaders_Free(recv_msg->resp_header);
    }
    if (recv_msg->msg_body != NULL)
    {
        BUFFER_delete(recv_msg->msg_body);
    }
    if ((recv_msg->resp_header = HTTPHeaders_Alloc()) == NULL)
    {
        /* Codes_SRS_UHTTP_07_017: [If any failure encountered http_client_execute_request shall return HTTP_CLIENT_ERROR] */
        LogError("Failure allocating http http_data items");
        result = HTTP_CLIENT_ERROR;
    }
    else if ((recv_msg->msg_body = BUFFER_new()) == NULL)
    {
        /* Codes_SRS_UHTTP_07_017: [If any failure encountered http_client_execute_request shall return HTTP_CLIENT_ERROR] */
        LogError("Failure allocating http data items");
        HTTPHeaders_Free(recv_msg->resp_header);
        recv_msg->resp_header = NULL;
        result = HTTP_CLIENT_ERROR;
    }
    else
    {
        result = 0;
    }
    return result;
}

static void on_bytes_received(void* context, const unsigned char* buffer, size_t len)
{
    HTTP_CLIENT_HANDLE_DATA* http_data = (HTTP_CLIENT_HANDLE_DATA*)context;

    if (http_data != NULL && buffer != NULL && len > 0 && http_data->recv_msg.recv_state != state_error)
    {
        if (http_data->recv_msg.recv_state == state_parse_complete)
        {
            // The callback is getting called during a new send.
            setup_init_recv_msg(&http_data->recv_msg);

        }

        if (http_data->recv_msg.recv_state == state_initial || http_data->recv_msg.recv_state == state_open)
        {
            if (initialize_received_data(http_data) != 0)
            {
                http_data->recv_msg.recv_state = state_error;
            }
            else
            {
                http_data->recv_msg.recv_state = state_process_status_line;
            }
        }

        // Put the data in the buffer
        if (BUFFER_append_build(http_data->recv_msg.accrual_buff, buffer, len) != 0)
        {
            /* Codes_SRS_UHTTP_07_048: [ If any error is encountered on_bytes_received shall set the state to error. ] */
            LogError("Failure appending bytes to buffer.");
            http_data->recv_msg.recv_state = state_error;
        }

        if (http_data->recv_msg.recv_state == state_process_status_line)
        {
            size_t index = 0;
            const unsigned char* stored_bytes = BUFFER_u_char(http_data->recv_msg.accrual_buff);
            size_t stored_len = BUFFER_length(http_data->recv_msg.accrual_buff);

            int lineComplete = process_status_code_line(stored_bytes, stored_len, &index, &http_data->recv_msg.status_code);
            if (lineComplete == 0 && http_data->recv_msg.status_code > 0)
            {
                if (BUFFER_shrink(http_data->recv_msg.accrual_buff, index, false) != 0)
                {
                    LogError("Failure appending bytes to buffer.");
                    http_data->recv_msg.recv_state = state_error;
                }
                else
                {
                    http_data->recv_msg.recv_state = state_process_headers;
                }
            }
        }

        if (http_data->recv_msg.recv_state == state_process_headers)
        {
            size_t index = 0;
            const unsigned char* stored_bytes = BUFFER_u_char(http_data->recv_msg.accrual_buff);
            size_t stored_len = BUFFER_length(http_data->recv_msg.accrual_buff);

            int headerComplete = process_header_line(stored_bytes, stored_len, &index, http_data->recv_msg.resp_header, &http_data->recv_msg.total_body_len, &http_data->recv_msg.chunked_reply);
            if (headerComplete == 0)
            {
                if (http_data->recv_msg.total_body_len == 0)
                {
                    if (http_data->recv_msg.chunked_reply)
                    {

                        /* Codes_SRS_UHTTP_07_054: [ If the http header does not include a content length then it indicates a chunk response. ] */
                        http_data->recv_msg.recv_state = state_process_chunked_body;
                    }
                    else
                    {
                        // Content len is 0 so we are finished with the body
                        http_data->recv_msg.recv_state = state_send_user_callback;
                    }
                }
                else
                {
                    http_data->recv_msg.recv_state = state_process_body;
                }
            }
            if (index > 0)
            {
                if (BUFFER_shrink(http_data->recv_msg.accrual_buff, index, false) != 0)
                {
                    LogError("Failure appending bytes to buffer.");
                    http_data->recv_msg.recv_state = state_error;
                }
            }
        }

        if (http_data->recv_msg.recv_state == state_process_body)
        {
            if (http_data->recv_msg.total_body_len != 0)
            {
                size_t stored_len = BUFFER_length(http_data->recv_msg.accrual_buff);

                if ((http_data->recv_msg.total_body_len == stored_len) || (http_data->recv_msg.total_body_len == (stored_len - HTTP_END_TOKEN_LEN)))
                {
                    if (http_data->recv_msg.msg_body != NULL)
                    {
                        BUFFER_delete(http_data->recv_msg.msg_body);
                    }
                    if ((http_data->recv_msg.msg_body = BUFFER_clone(http_data->recv_msg.accrual_buff)) == NULL)
                    {
                        LogError("Failure cloning BUFFER.");
                        http_data->recv_msg.recv_state = state_error;
                    }
                    else
                    {
                        http_data->recv_msg.recv_state = state_send_user_callback;
                    }
                }
                else if (stored_len > http_data->recv_msg.total_body_len)
                {
                    LogError("Failure bytes encountered is greater then body length.");
                    http_data->recv_msg.recv_state = state_error;
                }
            }
        }

        if (http_data->recv_msg.recv_state == state_process_chunked_body)
        {
            const unsigned char* iterator = BUFFER_u_char(http_data->recv_msg.accrual_buff);
            const unsigned char* initial_pos = iterator;
            const unsigned char* begin = iterator;
            const unsigned char* end = iterator;
            size_t accural_len = BUFFER_length(http_data->recv_msg.accrual_buff);

            /* Codes_SRS_UHTTP_07_059: [ on_bytes_received shall loop throught the stored data to find the /r/n separator. ] */
            while (iterator < (initial_pos + accural_len))
            {
                if (*iterator == '\r')
                {
                    // Don't need anything
                    end = iterator;
                    iterator++;
                }
                else if (*iterator == '\n')
                {
                    size_t data_length = 0;

                    /* Codes_SRS_UHTTP_07_055: [ on_bytes_received shall convert the hexs length supplied in the response to the data length of the chunked data. ] */
                    size_t hex_len = end - begin;
                    data_length = convert_char_to_hex(begin, hex_len);
                    if (data_length == 0)
                    {
                        if (accural_len - (iterator - initial_pos + 1) <= HTTP_END_TOKEN_LEN)
                        {
                            http_data->recv_msg.recv_state = state_send_user_callback;
                        }
                        else
                        {
                            // Need to continue parsing
                            http_data->recv_msg.recv_state = state_process_headers;
                        }
                        break;
                    }
                    else if ((data_length + HTTP_CRLF_LEN) < accural_len - (iterator - initial_pos))
                    {
                        /* Codes_SRS_UHTTP_07_056: [ After the response chunk is parsed it shall be placed in a BUFFER_HANDLE. ] */
                        iterator += 1;
                        if (BUFFER_append_build(http_data->recv_msg.msg_body, iterator, data_length) != 0)
                        {
                            /* Codes_SRS_UHTTP_07_048: [ If any error is encountered on_bytes_received shall set the stop processing the request. ] */
                            LogError("Failure building buffer for chunked data.");
                            http_data->recv_msg.recv_state = state_error;
                        }
                        else
                        {
                            /* Codes_SRS_UHTTP_07_060: [ if the data_length specified in the chunk is beyond the amount of data recieved, the parsing shall end and wait for more data. ] */
                            if (iterator + (data_length + HTTP_CRLF_LEN) > initial_pos + accural_len)
                            {
                                LogError("Invalid length specified.");
                                http_data->recv_msg.recv_state = state_error;
                                break;
                            }
                            else if (iterator + (data_length + HTTP_CRLF_LEN) == initial_pos + accural_len)
                            {
                                if (BUFFER_shrink(http_data->recv_msg.accrual_buff, accural_len, false) != 0)
                                {
                                    LogError("Failure shrinking accural buffer.");
                                    http_data->recv_msg.recv_state = state_error;
                                }
                                break;
                            }
                            else
                            {
                                // Move the iterator beyond the data we read and the /r/n
                                iterator += (data_length + HTTP_CRLF_LEN);
                            }

                            /* Codes_SRS_UHTTP_07_058: [ Once a chunk size value of 0 is encountered on_bytes_received shall call the on_request_callback with the http message ] */
                            if (*iterator == '0' && (accural_len - (iterator - initial_pos + 1) <= HTTP_END_TOKEN_LEN))
                            {
                                if (accural_len - (iterator - initial_pos + 1) <= HTTP_END_TOKEN_LEN)
                                {
                                    http_data->recv_msg.recv_state = state_send_user_callback;
                                }
                                else
                                {
                                    // Need to continue parsing
                                    http_data->recv_msg.recv_state = state_process_headers;
                                }
                                break;
                            }
                            else
                            {
                                size_t shrink_len = iterator - initial_pos;
                                if (shrink_len > 0)
                                {
                                    if (BUFFER_shrink(http_data->recv_msg.accrual_buff, shrink_len, false) != 0)
                                    {
                                        LogError("Failure shrinking accrual buffer.");
                                        http_data->recv_msg.recv_state = state_error;
                                    }
                                    else
                                    {
                                        accural_len = BUFFER_length(http_data->recv_msg.accrual_buff);
                                        initial_pos = iterator = BUFFER_u_char(http_data->recv_msg.accrual_buff);
                                    }
                                }
                            }
                        }
                        begin = end = iterator;
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    end = iterator;
                    iterator++;
                }
            }
        }

        if (http_data->recv_msg.recv_state == state_send_user_callback || http_data->recv_msg.recv_state == state_error)
        {
            const unsigned char* reply_data = NULL;
            size_t reply_len = 0;
            HTTP_CALLBACK_REASON http_reason = HTTP_CALLBACK_REASON_OK;
            if (http_data->recv_msg.msg_body != NULL)
            {
                reply_data = BUFFER_u_char(http_data->recv_msg.msg_body);
                reply_len = BUFFER_length(http_data->recv_msg.msg_body);
            }
            if (http_data->recv_msg.recv_state == state_error)
            {
                http_reason = HTTP_CALLBACK_REASON_PARSING_ERROR;
            }
            if (http_data->trace_on)
            {
                LOG(AZ_LOG_TRACE, LOG_LINE, "\r\nHTTP Status: %d\r\n", http_data->recv_msg.status_code);

                // Loop through headers
                size_t count;
                if (HTTPHeaders_GetHeaderCount(http_data->recv_msg.resp_header, &count) == 0)
                {
                    for (size_t index = 0; index < count; index++)
                    {
                        char* header;
                        if (HTTPHeaders_GetHeader(http_data->recv_msg.resp_header, index, &header) == HTTP_HEADERS_OK)
                        {
                            LOG(AZ_LOG_TRACE, LOG_LINE, "%s", header);
                            free(header);
                        }
                    }
                }
                if (http_data->trace_body && reply_len > 0)
                {
                    LOG(AZ_LOG_TRACE, LOG_LINE, "\r\n%.*s\r\n", (int)reply_len, reply_data);
                }
            }
            http_data->recv_msg.on_request_callback(http_data->recv_msg.user_ctx, http_reason, reply_data, reply_len, http_data->recv_msg.status_code, http_data->recv_msg.resp_header);
            http_data->recv_msg.recv_state = state_parse_complete;
        }

        if (http_data->recv_msg.recv_state == state_parse_complete)
        {
            HTTPHeaders_Free(http_data->recv_msg.resp_header);
            http_data->recv_msg.resp_header = NULL;
            BUFFER_delete(http_data->recv_msg.msg_body);
            http_data->recv_msg.msg_body = NULL;
            BUFFER_delete(http_data->recv_msg.accrual_buff);
            http_data->recv_msg.accrual_buff = NULL;
        }
    }
}

static void on_xio_close_complete(void* context)
{
    if (context != NULL)
    {
        /* Codes_SRS_UHTTP_07_045: [ If on_close_callback is not NULL, on_close_callback shall be called once the underlying xio is closed. ] */
        HTTP_CLIENT_HANDLE_DATA* http_data = (HTTP_CLIENT_HANDLE_DATA*)context;
        if (http_data->on_close_callback)
        {
            http_data->on_close_callback(http_data->close_user_ctx);
        }
        http_data->recv_msg.recv_state = state_closed;
        http_data->connected = 0;
    }
}

static void on_xio_open_complete(void* context, IO_OPEN_RESULT open_result)
{
    /* Codes_SRS_UHTTP_07_049: [ If not NULL uhttp_client_open shall call the on_connect callback with the callback_ctx, once the underlying xio's open is complete. ] */
    if (context != NULL)
    {
        HTTP_CLIENT_HANDLE_DATA* http_data = (HTTP_CLIENT_HANDLE_DATA*)context;
        if (open_result == IO_OPEN_OK)
        {
            /* Codes_SRS_UHTTP_07_042: [ If the underlying socket opens successfully the on_connect callback shall be call with HTTP_CALLBACK_REASON_OK... ] */
            if (http_data->on_connect != NULL)
            {
                http_data->on_connect(http_data->connect_user_ctx, HTTP_CALLBACK_REASON_OK);
            }
            http_data->recv_msg.recv_state = state_open;
            http_data->connected = 1;
        }
        else
        {
            /* Codes_SRS_UHTTP_07_043: [ Otherwise on_connect callback shall be call with HTTP_CLIENT_OPEN_REQUEST_FAILED. ] */
            if (http_data->on_connect != NULL)
            {
                http_data->on_connect(http_data->connect_user_ctx, HTTP_CALLBACK_REASON_OPEN_FAILED);
            }
        }
    }
    else
    {
        LogError("Context on_xio_open_complete is NULL");
    }
}

static void on_io_error(void* context)
{
    /* Codes_SRS_UHTTP_07_050: [ if context is NULL on_io_error shall do nothing. ] */
    if (context != NULL)
    {
        HTTP_CLIENT_HANDLE_DATA* http_data = (HTTP_CLIENT_HANDLE_DATA*)context;
        /* Codes_SRS_UHTTP_07_051: [ if on_error callback is not NULL, on_io_error shall call on_error callback. ] */
        if (http_data->on_error)
        {
            http_data->on_error(http_data->error_user_ctx, HTTP_CALLBACK_REASON_ERROR);
        }
        http_data->connected = 0;
    }
    else
    {
        LogError("Context on_io_error is NULL");
    }
}

static int construct_http_headers(HTTP_HEADERS_HANDLE http_header, size_t content_len, STRING_HANDLE buffData, bool chunk_data, const char* hostname, int port_num)
{
    (void)chunk_data;
    int result = 0;
    size_t headerCnt = 0;
    if ( (http_header != NULL) && HTTPHeaders_GetHeaderCount(http_header, &headerCnt) != HTTP_HEADERS_OK)
    {
        LogError("Failed in HTTPHeaders_GetHeaderCount");
        result = MU_FAILURE;
    }
    else
    {
        bool hostname_found = false;
        for (size_t index = 0; index < headerCnt && result == 0; index++)
        {
            char* header;
            if (HTTPHeaders_GetHeader(http_header, index, &header) != HTTP_HEADERS_OK)
            {
                result = MU_FAILURE;
                LogError("Failed in HTTPHeaders_GetHeader");
            }
            else
            {
                size_t dataLen = strlen(header)+2;
                char* sendData = malloc(dataLen+1);
                if (sendData == NULL)
                {
                    result = MU_FAILURE;
                    LogError("Failed in allocating header data");
                }
                else
                {
                    if (strcmp(header, HTTP_HOST) == 0)
                    {
                        hostname_found = true;
                    }

                    if (snprintf(sendData, dataLen+1, "%s\r\n", header) <= 0)
                    {
                        result = MU_FAILURE;
                        LogError("Failed in constructing header data");
                    }
                    else
                    {
                        if (STRING_concat(buffData, sendData) != 0)
                        {
                            result = MU_FAILURE;
                            LogError("Failed in building header data");
                        }
                    }
                    free(sendData);
                }
                free(header);
            }
        }
        if (!hostname_found)
        {
            // calculate the size of the host header
            size_t host_len = strlen(HTTP_HOST) + strlen(hostname) + MAX_CONTENT_LENGTH + 2;
            char* host_header = malloc(host_len + 1);
            if (host_header == NULL)
            {
                LogError("Failed allocating host header");
                result = MU_FAILURE;
            }
            else
            {
                if (snprintf(host_header, host_len + 1, "%s: %s:%d\r\n", HTTP_HOST, hostname, port_num) <= 0)
                {
                    LogError("Failed constructing host header");
                    result = MU_FAILURE;
                }
                else if (STRING_concat(buffData, host_header) != 0)
                {
                    LogError("Failed adding the host header to the http item");
                    result = MU_FAILURE;
                }
                free(host_header);
            }
        }

        if (result == 0)
        {
            /* Codes_SRS_UHTTP_07_015: [uhttp_client_execute_request shall add the Content-Length to the request if the contentLength is > 0] */
            size_t fmtLen = strlen(HTTP_CONTENT_LEN) + HTTP_CRLF_LEN + 8;
            char* content = malloc(fmtLen+1);
            if (content == NULL)
            {
                LogError("Failed allocating chunk header");
                result = MU_FAILURE;
            }
            else
            {
                /* Codes_SRS_UHTTP_07_015: [on_bytes_received shall add the Content-Length http header item to the request.] */
                if (sprintf(content, "%s: %u%s", HTTP_CONTENT_LEN, (unsigned int)content_len, HTTP_CRLF_VALUE) <= 0)
                {
                    result = MU_FAILURE;
                    LogError("Failed allocating content len header data");
                }
                else
                {
                    if (STRING_concat(buffData, content) != 0)
                    {
                        result = MU_FAILURE;
                        LogError("Failed building content len header data");
                    }
                }
                free(content);
            }

            if (STRING_concat(buffData, "\r\n") != 0)
            {
                result = MU_FAILURE;
                LogError("Failed sending header finalization data");
            }
        }
    }
    return result;
}

static STRING_HANDLE construct_http_data(HTTP_CLIENT_REQUEST_TYPE request_type, const char* relative_path, STRING_HANDLE http_line)
{
    STRING_HANDLE result;

    const char* method = (request_type == HTTP_CLIENT_REQUEST_GET) ? "GET"
        : (request_type == HTTP_CLIENT_REQUEST_OPTIONS) ? "OPTIONS"
        : (request_type == HTTP_CLIENT_REQUEST_POST) ? "POST"
        : (request_type == HTTP_CLIENT_REQUEST_PUT) ? "PUT"
        : (request_type == HTTP_CLIENT_REQUEST_DELETE) ? "DELETE"
        : (request_type == HTTP_CLIENT_REQUEST_PATCH) ? "PATCH"
        : NULL;
    /* Codes_SRS_UHTTP_07_014: [If the request_type is not a valid request http_client_execute_request shall return HTTP_CLIENT_ERROR] */
    if (method == NULL)
    {
        LogError("Invalid request method %s specified", method);
        result = NULL;
    }
    else
    {
        size_t buffLen = strlen(HTTP_REQUEST_LINE_FMT) + strlen(method) + strlen(relative_path);
        char* request = malloc(buffLen+1);
        if (request == NULL)
        {
            result = NULL;
            LogError("Failure allocating Request data");
        }
        else
        {
            if (snprintf(request, buffLen + 1, HTTP_REQUEST_LINE_FMT, method, relative_path) <= 0)
            {
                result = NULL;
                LogError("Failure writing request buffer");
            }
            else
            {
                result = STRING_construct(request);
                if (result == NULL)
                {
                    LogError("Failure creating buffer object");
                }
                else if (STRING_concat_with_STRING(result, http_line) != 0)
                {
                    STRING_delete(result);
                    result = NULL;
                    LogError("Failure writing request buffers");
                }
            }
            free(request);
        }
    }
    return result;
}

static int send_http_data(HTTP_CLIENT_HANDLE_DATA* http_data, HTTP_CLIENT_REQUEST_TYPE request_type, const char* relative_path,
    STRING_HANDLE http_line)
{
    int result;
    STRING_HANDLE transmit_data = construct_http_data(request_type, relative_path, http_line);
    if (transmit_data == NULL)
    {
        LogError("Failure constructing http data");
        result = MU_FAILURE;
    }
    else
    {
        /* Tests_SRS_UHTTP_07_016: [http_client_execute_request shall transmit the http headers data through a call to xio_send;]*/
        if (write_text_line(http_data, STRING_c_str(transmit_data) ) != 0)
        {
            result = MU_FAILURE;
            LogError("Failure writing request buffer");
        }
        else
        {
            result = 0;
        }
        STRING_delete(transmit_data);
    }
    return result;
}

HTTP_CLIENT_HANDLE uhttp_client_create(const IO_INTERFACE_DESCRIPTION* io_interface_desc, const void* xio_param, ON_HTTP_ERROR_CALLBACK on_http_error, void* callback_ctx)
{
    HTTP_CLIENT_HANDLE_DATA* result;
    /* Codes_SRS_UHTTP_07_002: [If io_interface_desc is NULL, uhttp_client_create shall return NULL.] */
    if (io_interface_desc == NULL)
    {
        LogError("Invalid Parameter io_interface_desc is NULL");
        result = NULL;
    }
    else
    {
        result = malloc(sizeof(HTTP_CLIENT_HANDLE_DATA));
        if (result == NULL)
        {
            /* Codes_SRS_UHTTP_07_003: [If uhttp_client_create encounters any error then it shall return NULL] */
            LogError("Failure allocating http_client_handle");
        }
        else
        {
            memset(result, 0, sizeof(HTTP_CLIENT_HANDLE_DATA) );
            if ((result->data_list = singlylinkedlist_create() ) == NULL)
            {
                /* Codes_SRS_UHTTP_07_003: [If uhttp_client_create encounters any error then it shall return NULL] */
                LogError("Failure allocating data list");
                free(result);
                result = NULL;
            }
            else if ((result->xio_handle = xio_create(io_interface_desc, xio_param) ) == NULL)
            {
                /* Codes_SRS_UHTTP_07_044: [ if a failure is encountered on xio_open uhttp_client_open shall return HTTP_CLIENT_OPEN_REQUEST_FAILED. ] */
                LogError("xio create failed");
                singlylinkedlist_destroy(result->data_list);
                free(result);
                result = NULL;
            }
            else
            {
                /* Codes_SRS_UHTTP_07_001: [uhttp_client_create shall return an initialize the http client handle.] */
                result->on_error = on_http_error;
                result->error_user_ctx = callback_ctx;
                result->recv_msg.recv_state = state_initial;
                result->chunk_request = false;
                result->trace_on = false;
            }
        }
    }
    return (HTTP_CLIENT_HANDLE)result;
}

void uhttp_client_destroy(HTTP_CLIENT_HANDLE handle)
{
    /* Codes_SRS_UHTTP_07_004: [ If handle is NULL then uhttp_client_destroy shall do nothing ] */
    if (handle != NULL)
    {
        /* Codes_SRS_UHTTP_07_005: [uhttp_client_destroy shall free any resource that is allocated in this translation unit] */
        if(handle->host_name != NULL)
        {
            free(handle->host_name);
            handle->host_name = NULL;
        }
        singlylinkedlist_destroy(handle->data_list);
        xio_destroy(handle->xio_handle);
        free(handle->certificate);
        free(handle->x509_pk);
        free(handle->x509_cert);
        free(handle);
    }
}

HTTP_CLIENT_RESULT uhttp_client_open(HTTP_CLIENT_HANDLE handle, const char* host, int port_num, ON_HTTP_OPEN_COMPLETE_CALLBACK on_connect, void* callback_ctx)
{
    HTTP_CLIENT_RESULT result;
    if (handle == NULL || host == NULL)
    {
        /* Codes_SRS_UHTTP_07_006: [If handle, io_interface_desc or host is NULL then `uhttp_client_open` shall return HTTP_CLIENT_INVALID_ARG] */
        LogError("Invalid handle value");
        result = HTTP_CLIENT_INVALID_ARG;
    }
    else
    {
        HTTP_CLIENT_HANDLE_DATA* http_data = (HTTP_CLIENT_HANDLE_DATA*)handle;

        if ((http_data->recv_msg.recv_state != state_initial) &&
            (http_data->recv_msg.recv_state != state_error) &&
            (http_data->recv_msg.recv_state != state_closed))
        {
            LogError("Unable to open previously open client.");
            result = HTTP_CLIENT_INVALID_STATE;
        }
        else
        {
            if (http_data->host_name != NULL)
            {
                free(http_data->host_name);
                handle->host_name = NULL;
            }

            if (mallocAndStrcpy_s(&http_data->host_name, host) != 0)
            {
                LogError("copying hostname has failed");
                result = HTTP_CLIENT_ERROR;
            }
            /* Codes_SRS_UHTTP_07_007: [http_client_connect shall attempt to open the xio_handle. ] */
            else
            {
                result = HTTP_CLIENT_OK;
                http_data->recv_msg.recv_state = state_opening;
                http_data->on_connect = on_connect;
                http_data->connect_user_ctx = callback_ctx;
                http_data->port_num = port_num;

                if ((http_data->x509_cert != NULL) && (http_data->x509_pk != NULL))
                {
                    if ((xio_setoption(http_data->xio_handle, SU_OPTION_X509_CERT, http_data->x509_cert) != 0) ||
                        (xio_setoption(http_data->xio_handle, SU_OPTION_X509_PRIVATE_KEY, http_data->x509_pk) != 0))
                    {
                        LogError("Failed setting x509 certificate");
                        result = HTTP_CLIENT_ERROR;
                        free(http_data->host_name);
                        http_data->host_name = NULL;
                        http_data->on_connect = NULL;
                        http_data->connect_user_ctx = NULL;
                        http_data->port_num = 0;
                    }
                }

                if ((result == HTTP_CLIENT_OK) && (http_data->certificate != NULL))
                {
                    if (xio_setoption(http_data->xio_handle, OPTION_TRUSTED_CERT, http_data->certificate) != 0)
                    {
                        LogError("Failed setting Trusted certificate");
                        result = HTTP_CLIENT_ERROR;
                        free(http_data->host_name);
                        http_data->host_name = NULL;
                        http_data->on_connect = NULL;
                        http_data->connect_user_ctx = NULL;
                        http_data->port_num = 0;
                    }
                }

                if (result == HTTP_CLIENT_OK)
                {
                    if (xio_open(http_data->xio_handle, on_xio_open_complete, http_data, on_bytes_received, http_data, on_io_error, http_data) != 0)
                    {
                        /* Codes_SRS_UHTTP_07_044: [ if a failure is encountered on xio_open uhttp_client_open shall return HTTP_CLIENT_OPEN_REQUEST_FAILED. ] */
                        LogError("opening xio failed");
                        free(http_data->host_name);
                        http_data->host_name = NULL;
                        http_data->on_connect = NULL;
                        http_data->connect_user_ctx = NULL;
                        http_data->port_num = 0;
                        http_data->recv_msg.recv_state = state_error;

                        result = HTTP_CLIENT_OPEN_FAILED;
                    }
                    else
                    {
                        /* Codes_SRS_UHTTP_07_008: [If http_client_connect succeeds then it shall return HTTP_CLIENT_OK] */
                        result = HTTP_CLIENT_OK;
                    }
                }
            }
        }
    }
    return result;
}

void uhttp_client_close(HTTP_CLIENT_HANDLE handle, ON_HTTP_CLOSED_CALLBACK on_close_callback, void* callback_ctx)
{
    HTTP_CLIENT_HANDLE_DATA* http_data = (HTTP_CLIENT_HANDLE_DATA*)handle;
    /* Codes_SRS_UHTTP_07_009: [If handle is NULL then http_client_close shall do nothing] */
    /* Codes_SRS_UHTTP_07_049: [ If the state has been previously set to state_closed, uhttp_client_close shall do nothing. ] */
    if (http_data != NULL && http_data->recv_msg.recv_state != state_closed && http_data->recv_msg.recv_state != state_closing)
    {
        http_data->on_close_callback = on_close_callback;
        http_data->close_user_ctx = callback_ctx;
        /* Codes_SRS_UHTTP_07_010: [If the xio_handle is NOT NULL http_client_close shall call xio_close to close the handle] */
        (void)xio_close(http_data->xio_handle, on_xio_close_complete, http_data);

        LIST_ITEM_HANDLE pending_list_item;
        while ((pending_list_item = singlylinkedlist_get_head_item(http_data->data_list)) != NULL)
        {
            HTTP_SEND_DATA* send_data = (HTTP_SEND_DATA*)singlylinkedlist_item_get_value(pending_list_item);
            if (send_data != NULL)
            {
                STRING_delete(send_data->relative_path);
                BUFFER_delete(send_data->content);
                STRING_delete(send_data->header_line);
                free(send_data);
            }
            singlylinkedlist_remove(http_data->data_list, pending_list_item);
        }

        http_data->recv_msg.status_code = 0;
        http_data->recv_msg.recv_state = state_closed;
        http_data->recv_msg.total_body_len = 0;
        free(http_data->host_name);
        http_data->host_name = NULL;

        /* Codes_SRS_UHTTP_07_011: [http_client_close shall free any HTTPHeader object that has not been freed] */
        if (http_data->recv_msg.resp_header != NULL)
        {
            HTTPHeaders_Free(http_data->recv_msg.resp_header);
            http_data->recv_msg.resp_header = NULL;
        }
        if (http_data->recv_msg.msg_body != NULL)
        {
            BUFFER_delete(http_data->recv_msg.msg_body);
            http_data->recv_msg.msg_body = NULL;
        }
    }
}

HTTP_CLIENT_RESULT uhttp_client_execute_request(HTTP_CLIENT_HANDLE handle, HTTP_CLIENT_REQUEST_TYPE request_type, const char* relative_path,
    HTTP_HEADERS_HANDLE http_header_handle, const unsigned char* content, size_t content_len, ON_HTTP_REQUEST_CALLBACK on_request_callback, void* callback_ctx)
{
    HTTP_CLIENT_RESULT result;
    LIST_ITEM_HANDLE list_item;

    /* Codes_SRS_UHTTP_07_012: [If handle, relativePath, or httpHeadersHandle is NULL then http_client_execute_request shall return HTTP_CLIENT_INVALID_ARG] */
    if (handle == NULL || on_request_callback == NULL ||
        (content != NULL && content_len == 0) || (content == NULL && content_len != 0) )
    {
        result = HTTP_CLIENT_INVALID_ARG;
        LogError("Invalid parameter sent to execute_request");
    }
    else
    {
        HTTP_CLIENT_HANDLE_DATA* http_data = (HTTP_CLIENT_HANDLE_DATA*)handle;

        http_data->recv_msg.on_request_callback = on_request_callback;
        http_data->recv_msg.user_ctx = callback_ctx;
        if (setup_init_recv_msg(&http_data->recv_msg) != 0)
        {
            /* Codes_SRS_UHTTP_07_017: [If any failure encountered http_client_execute_request shall return HTTP_CLIENT_ERROR] */
            LogError("Failure allocating http http_data items");
            result = HTTP_CLIENT_ERROR;
        }
        else
        {
            HTTP_SEND_DATA* send_data = (HTTP_SEND_DATA*)malloc(sizeof(HTTP_SEND_DATA));
            if (send_data == NULL)
            {
                LogError("Failure allocating http data items");
                BUFFER_delete(http_data->recv_msg.msg_body);
                http_data->recv_msg.msg_body = NULL;
                HTTPHeaders_Free(http_data->recv_msg.resp_header);
                http_data->recv_msg.resp_header = NULL;
                result = HTTP_CLIENT_ERROR;
            }
            else
            {
                memset(send_data, 0, sizeof(HTTP_SEND_DATA));
                /* Codes_SRS_UHTTP_07_041: [HTTP_CLIENT_REQUEST_TYPE shall support all request types specified under section 9.1.2 in the spec.] */
                send_data->request_type = request_type;
                if ( (content_len > 0) && (send_data->content = BUFFER_create(content, content_len)) == NULL)
                {
                    LogError("Failure allocating content buffer");
                    result = HTTP_CLIENT_ERROR;
                    BUFFER_delete(http_data->recv_msg.msg_body);
                    http_data->recv_msg.msg_body = NULL;
                    HTTPHeaders_Free(http_data->recv_msg.resp_header);
                    http_data->recv_msg.resp_header = NULL;
                    free(send_data);
                }
                else if ((send_data->header_line = STRING_new()) == NULL)
                {
                    LogError("Failure allocating content buffer");
                    result = HTTP_CLIENT_ERROR;
                    BUFFER_delete(send_data->content);
                    BUFFER_delete(http_data->recv_msg.msg_body);
                    http_data->recv_msg.msg_body = NULL;
                    HTTPHeaders_Free(http_data->recv_msg.resp_header);
                    http_data->recv_msg.resp_header = NULL;
                    free(send_data);
                }
                else if (construct_http_headers(http_header_handle, content_len, send_data->header_line, false, http_data->host_name, http_data->port_num))
                {
                    LogError("Failure allocating content buffer");
                    result = HTTP_CLIENT_ERROR;
                    BUFFER_delete(send_data->content);
                    STRING_delete(send_data->header_line);
                    BUFFER_delete(http_data->recv_msg.msg_body);
                    http_data->recv_msg.msg_body = NULL;
                    HTTPHeaders_Free(http_data->recv_msg.resp_header);
                    http_data->recv_msg.resp_header = NULL;
                    free(send_data);
                }
                else if ((list_item = singlylinkedlist_add(http_data->data_list, send_data)) == NULL)
                {
                    STRING_delete(send_data->header_line);
                    BUFFER_delete(send_data->content);
                    LogError("Failure adding send data to list");
                    result = HTTP_CLIENT_ERROR;
                    BUFFER_delete(http_data->recv_msg.msg_body);
                    http_data->recv_msg.msg_body = NULL;
                    HTTPHeaders_Free(http_data->recv_msg.resp_header);
                    http_data->recv_msg.resp_header = NULL;
                    free(send_data);
                }
                else
                {
                    if (relative_path != NULL)
                    {
                        if ((send_data->relative_path = STRING_construct(relative_path)) == NULL)
                        {
                            (void)singlylinkedlist_remove(http_data->data_list, list_item);
                            STRING_delete(send_data->header_line);
                            BUFFER_delete(send_data->content);
                            LogError("Failure allocating relative path buffer");
                            BUFFER_delete(http_data->recv_msg.msg_body);
                            http_data->recv_msg.msg_body = NULL;
                            HTTPHeaders_Free(http_data->recv_msg.resp_header);
                            http_data->recv_msg.resp_header = NULL;
                            free(send_data);
                            result = HTTP_CLIENT_ERROR;
                        }
                        else
                        {
                            /* Codes_SRS_UHTTP_07_018: [upon success http_client_execute_request shall return HTTP_CLIENT_OK.] */
                            result = HTTP_CLIENT_OK;
                        }
                    }
                    else
                    {
                        if ((send_data->relative_path = STRING_construct("/")) == NULL)
                        {
                            (void)singlylinkedlist_remove(http_data->data_list, list_item);
                            STRING_delete(send_data->header_line);
                            BUFFER_delete(send_data->content);
                            LogError("Failure allocating relative path buffer");
                            BUFFER_delete(http_data->recv_msg.msg_body);
                            http_data->recv_msg.msg_body = NULL;
                            HTTPHeaders_Free(http_data->recv_msg.resp_header);
                            http_data->recv_msg.resp_header = NULL;
                            free(send_data);
                            result = HTTP_CLIENT_ERROR;
                        }
                        else
                        {
                            /* Codes_SRS_UHTTP_07_018: [upon success http_client_execute_request shall return HTTP_CLIENT_OK.] */
                            result = HTTP_CLIENT_OK;
                        }
                    }
                }
            }
        }
    }
    return result;
}

void uhttp_client_dowork(HTTP_CLIENT_HANDLE handle)
{
    if (handle != NULL)
    {
        /* Codes_SRS_UHTTP_07_037: [http_client_dowork shall call the underlying xio_dowork function. ] */
        HTTP_CLIENT_HANDLE_DATA* http_data = (HTTP_CLIENT_HANDLE_DATA*)handle;
        xio_dowork(http_data->xio_handle);

        // Wait till I'm connected
        if (handle->connected == 1)
        {
            LIST_ITEM_HANDLE pending_list_item;
            /* Codes_SRS_UHTTP_07_016: [http_client_dowork shall iterate through the queued Data using the xio interface to send the http request in the following ways...] */
            while ((pending_list_item = singlylinkedlist_get_head_item(http_data->data_list)) != NULL)
            {
                HTTP_SEND_DATA* send_data = (HTTP_SEND_DATA*)singlylinkedlist_item_get_value(pending_list_item);
                if (send_data != NULL)
                {
                    size_t content_len = BUFFER_length(send_data->content);
                    /* Codes_SRS_UHTTP_07_052: [uhttp_client_dowork shall call xio_send to transmits the header information... ] */
                    if (send_http_data(http_data, send_data->request_type, STRING_c_str(send_data->relative_path), send_data->header_line) != 0)
                    {
                        LogError("Failure writing content buffer");
                        if (http_data->on_error)
                        {
                            http_data->on_error(http_data->error_user_ctx, HTTP_CALLBACK_REASON_SEND_FAILED);
                        }
                    }
                    else if (content_len > 0)
                    {
                        /* Codes_SRS_UHTTP_07_053: [ Then uhttp_client_dowork shall use xio_send to transmit the content of the http request. ] */
                        if (write_data_line(http_data, BUFFER_u_char(send_data->content), content_len) != 0)
                        {
                            LogError("Failure writing content buffer");
                            if (http_data->on_error)
                            {
                                http_data->on_error(http_data->error_user_ctx, HTTP_CALLBACK_REASON_SEND_FAILED);
                            }
                        }
                    }

                    /* Codes_SRS_UHTTP_07_046: [ http_client_dowork shall free resouces queued to send to the http endpoint. ] */
                    STRING_delete(send_data->relative_path);
                    BUFFER_delete(send_data->content);
                    STRING_delete(send_data->header_line);
                    free(send_data);
                }
                (void)singlylinkedlist_remove(http_data->data_list, pending_list_item);
            }
        }
    }
}

HTTP_CLIENT_RESULT uhttp_client_set_trace(HTTP_CLIENT_HANDLE handle, bool trace_on, bool trace_data)
{
    HTTP_CLIENT_RESULT result;
    if (handle == NULL)
    {
        /* Codes_SRS_UHTTP_07_038: [If handle is NULL then http_client_set_trace shall return HTTP_CLIENT_INVALID_ARG] */
        result = HTTP_CLIENT_INVALID_ARG;
        LogError("invalid parameter (NULL) passed to http_client_set_trace");
    }
    else
    {
        /* Codes_SRS_UHTTP_07_039: [http_client_set_trace shall set the HTTP tracing to the trace_on variable.] */
        handle->trace_on = trace_on;
        handle->trace_body = trace_data;
        /* Codes_SRS_UHTTP_07_040: [if http_client_set_trace finishes successfully then it shall return HTTP_CLIENT_OK.] */
        result = HTTP_CLIENT_OK;
    }
    return result;
}

HTTP_CLIENT_RESULT uhttp_client_set_X509_cert(HTTP_CLIENT_HANDLE handle, bool ecc_type, const char* certificate, const char* private_key)
{
    HTTP_CLIENT_RESULT result;
    if (handle == NULL || certificate == NULL || private_key == NULL)
    {
        /* Codes_SRS_UHTTP_07_038: [If handle is NULL then http_client_set_trace shall return HTTP_CLIENT_INVALID_ARG] */
        result = HTTP_CLIENT_INVALID_ARG;
        LogError("invalid parameter handle: %p certificate: %p private_key: %p", handle, certificate, private_key);
    }
    else if (handle->recv_msg.recv_state != state_initial)
    {
        result = HTTP_CLIENT_INVALID_STATE;
        LogError("You must set the X509 certificates before opening the connection");
    }
    else
    {
        handle->cert_type_ecc = ecc_type;
        if (mallocAndStrcpy_s(&handle->x509_cert, certificate) != 0)
        {
            result = HTTP_CLIENT_ERROR;
            LogError("failure allocating certificate");
        }
        else if (mallocAndStrcpy_s(&handle->x509_pk, private_key) != 0)
        {
            free(handle->x509_cert);
            handle->x509_cert = NULL;
            result = HTTP_CLIENT_ERROR;
            LogError("failure allocating private key");
        }
        else
        {
            result = HTTP_CLIENT_OK;
        }
    }
    return result;
}

HTTP_CLIENT_RESULT uhttp_client_set_trusted_cert(HTTP_CLIENT_HANDLE handle, const char* certificate)
{
    HTTP_CLIENT_RESULT result;
    if (handle == NULL || certificate == NULL)
    {
        /* Codes_SRS_UHTTP_07_038: [If handle is NULL then http_client_set_trace shall return HTTP_CLIENT_INVALID_ARG] */
        result = HTTP_CLIENT_INVALID_ARG;
        LogError("invalid parameter handle: %p certificate: %p", handle, certificate);
    }
    else if (handle->recv_msg.recv_state != state_initial)
    {
        result = HTTP_CLIENT_INVALID_STATE;
        LogError("You must set the certificates before opening the connection");
    }
    else
    {
        if (mallocAndStrcpy_s(&handle->certificate, certificate) != 0)
        {
            result = HTTP_CLIENT_ERROR;
            LogError("failure allocating certificate");
        }
        else
        {
            result = HTTP_CLIENT_OK;
        }
    }
    return result;
}

const char* uhttp_client_get_trusted_cert(HTTP_CLIENT_HANDLE handle)
{
    const char* result;
    if (handle == NULL)
    {
        result = NULL;
        LogError("invalid parameter NULL handle");
    }
    else
    {
        result = handle->certificate;
    }
    return result;
}

HTTP_CLIENT_RESULT uhttp_client_set_option(HTTP_CLIENT_HANDLE handle, const char* optionName, const void* value)
{
    HTTP_CLIENT_RESULT result;
    if (handle == NULL)
    {
        /* Codes_SRS_UHTTP_07_038: [If handle is NULL then http_client_set_trace shall return HTTP_CLIENT_INVALID_ARG] */
        result = HTTP_CLIENT_INVALID_ARG;
        LogError("invalid parameter handle: %p", handle);
    }
    else
    {
        int setoption_result = xio_setoption(handle->xio_handle, optionName, value);
        if (setoption_result != 0)
        {
            LogError("xio_setoption fails, returns %d", setoption_result);
            result = HTTP_CLIENT_ERROR;
        }
        else
        {
            result = HTTP_CLIENT_OK;
        }

    }

    return result;
}

XIO_HANDLE uhttp_client_get_underlying_xio(HTTP_CLIENT_HANDLE handle)
{
    XIO_HANDLE result;
    if (handle == NULL)
    {
        LogError("invalid parameter handle: %p", handle);
        result = NULL;
    }
    else
    {
        result = handle->xio_handle;
    }
    return result;
}
