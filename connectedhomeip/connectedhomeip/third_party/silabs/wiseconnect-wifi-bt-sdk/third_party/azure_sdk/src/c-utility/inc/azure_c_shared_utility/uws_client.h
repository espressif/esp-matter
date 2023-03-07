// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef UWS_CLIENT_H
#define UWS_CLIENT_H

#ifdef __cplusplus
#include <cstddef>
#include <cstdint>
#else
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#endif

#include "xio.h"
#include "umock_c/umock_c_prod.h"
#include "azure_c_shared_utility/optionhandler.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UWS_CLIENT_INSTANCE_TAG* UWS_CLIENT_HANDLE;

#define WS_SEND_FRAME_RESULT_VALUES \
    WS_SEND_FRAME_OK, \
    WS_SEND_FRAME_ERROR, \
    WS_SEND_FRAME_CANCELLED

MU_DEFINE_ENUM(WS_SEND_FRAME_RESULT, WS_SEND_FRAME_RESULT_VALUES);

#define WS_OPEN_RESULT_VALUES \
    WS_OPEN_OK, \
    WS_OPEN_ERROR_UNDERLYING_IO_OPEN_FAILED, \
    WS_OPEN_ERROR_UNDERLYING_IO_OPEN_CANCELLED, \
    WS_OPEN_ERROR_NOT_ENOUGH_MEMORY, \
    WS_OPEN_ERROR_CANNOT_CONSTRUCT_UPGRADE_REQUEST, \
    WS_OPEN_ERROR_CANNOT_SEND_UPGRADE_REQUEST, \
    WS_OPEN_ERROR_MULTIPLE_UNDERLYING_IO_OPEN_EVENTS, \
    WS_OPEN_ERROR_CONSTRUCTING_UPGRADE_REQUEST, \
    WS_OPEN_ERROR_INVALID_BYTES_RECEIVED_ARGUMENTS, \
    WS_OPEN_ERROR_BYTES_RECEIVED_BEFORE_UNDERLYING_OPEN, \
    WS_OPEN_CANCELLED, \
    WS_OPEN_ERROR_UNDERLYING_IO_ERROR, \
    WS_OPEN_ERROR_BAD_UPGRADE_RESPONSE, \
    WS_OPEN_ERROR_BAD_RESPONSE_STATUS, \
    WS_OPEN_ERROR_BASE64_ENCODE_FAILED

MU_DEFINE_ENUM(WS_OPEN_RESULT, WS_OPEN_RESULT_VALUES);

#define WS_ERROR_VALUES \
    WS_ERROR_NOT_ENOUGH_MEMORY, \
    WS_ERROR_BAD_FRAME_RECEIVED, \
    WS_ERROR_CANNOT_REMOVE_SENT_ITEM_FROM_LIST, \
    WS_ERROR_UNDERLYING_IO_ERROR, \
    WS_ERROR_CANNOT_CLOSE_UNDERLYING_IO

MU_DEFINE_ENUM(WS_ERROR, WS_ERROR_VALUES);

#define WS_FRAME_TYPE_UNKNOWN       0x00
#define WS_FRAME_TYPE_TEXT          0x01
#define WS_FRAME_TYPE_BINARY        0x02

/* Codes_SRS_UWS_CLIENT_01_324: [ 1000 indicates a normal closure, meaning that the purpose for which the connection was established has been fulfilled. ]*/
/* Codes_SRS_UWS_CLIENT_01_325: [ 1001 indicates that an endpoint is "going away", such as a server going down or a browser having navigated away from a page. ]*/
/* Codes_SRS_UWS_CLIENT_01_326: [ 1002 indicates that an endpoint is terminating the connection due to a protocol error. ]*/
/* Codes_SRS_UWS_CLIENT_01_327: [ 1003 indicates that an endpoint is terminating the connection because it has received a type of data it cannot accept (e.g., an endpoint that understands only text data MAY send this if it receives a binary message). ]*/
/* Codes_SRS_UWS_CLIENT_01_328: [ Reserved.  The specific meaning might be defined in the future. ]*/
/* Codes_SRS_UWS_CLIENT_01_329: [ 1005 is a reserved value and MUST NOT be set as a status code in a Close control frame by an endpoint. ]*/
/* Codes_SRS_UWS_CLIENT_01_330: [ 1006 is a reserved value and MUST NOT be set as a status code in a Close control frame by an endpoint. ]*/
/* Codes_SRS_UWS_CLIENT_01_331: [ 1007 indicates that an endpoint is terminating the connection because it has received data within a message that was not consistent with the type of the message (e.g., non-UTF-8 [RFC3629] data within a text message). ]*/
/* Codes_SRS_UWS_CLIENT_01_332: [ 1008 indicates that an endpoint is terminating the connection because it has received a message that violates its policy. ]*/
/* Codes_SRS_UWS_CLIENT_01_333: [ 1009 indicates that an endpoint is terminating the connection because it has received a message that is too big for it to process. ]*/
/* Codes_SRS_UWS_CLIENT_01_334: [ 1010 indicates that an endpoint (client) is terminating the connection because it has expected the server to negotiate one or more extension, but the server didn't return them in the response message of the WebSocket handshake. ]*/
/* Codes_SRS_UWS_CLIENT_01_336: [ 1011 indicates that a server is terminating the connection because it encountered an unexpected condition that prevented it from fulfilling the request. ]*/
/* Codes_SRS_UWS_CLIENT_01_337: [ 1015 is a reserved value and MUST NOT be set as a status code in a Close control frame by an endpoint. ]*/
/*  */
#define CLOSE_NORMAL                        1000
#define CLOSE_GOING_AWAY                    1001
#define CLOSE_PROTOCOL_ERROR                1002
#define CLOSE_CANNOT_ACCEPT_DATA_TYPE       1003
#define CLOSE_UNDEFINED_1004                1004
#define CLOSE_RESERVED_1005                 1005
#define CLOSE_RESERVED_1006                 1006
#define CLOSE_INCONSISTENT_DATA_IN_MESSAGE  1007
#define CLOSE_POLICY_VIOLATION              1008
#define CLOSE_MESSAGE_TOO_BIG               1009
#define CLOSE_UNSUPPORTED_EXTENSION_LIST    1010
#define CLOSE_UNEXPECTED_CONDITION          1011
#define CLOSE_RESERVED_1015                 1015

typedef void(*ON_WS_FRAME_RECEIVED)(void* context, unsigned char frame_type, const unsigned char* buffer, size_t size);
typedef void(*ON_WS_SEND_FRAME_COMPLETE)(void* context, WS_SEND_FRAME_RESULT ws_send_frame_result);
typedef void(*ON_WS_OPEN_COMPLETE)(void* context, WS_OPEN_RESULT ws_open_result);
typedef void(*ON_WS_CLOSE_COMPLETE)(void* context);
typedef void(*ON_WS_PEER_CLOSED)(void* context, uint16_t* close_code, const unsigned char* extra_data, size_t extra_data_length);
typedef void(*ON_WS_ERROR)(void* context, WS_ERROR error_code);

typedef struct WS_PROTOCOL_TAG
{
    const char* protocol;
} WS_PROTOCOL;

MOCKABLE_FUNCTION(, UWS_CLIENT_HANDLE, uws_client_create, const char*, hostname, unsigned int, port, const char*, resource_name, bool, use_ssl, const WS_PROTOCOL*, protocols, size_t, protocol_count);
MOCKABLE_FUNCTION(, UWS_CLIENT_HANDLE, uws_client_create_with_io, const IO_INTERFACE_DESCRIPTION*, io_interface, void*, io_create_parameters, const char*, hostname, unsigned int, port, const char*, resource_name, const WS_PROTOCOL*, protocols, size_t, protocol_count)
MOCKABLE_FUNCTION(, void, uws_client_destroy, UWS_CLIENT_HANDLE, uws_client);
MOCKABLE_FUNCTION(, int, uws_client_open_async, UWS_CLIENT_HANDLE, uws_client, ON_WS_OPEN_COMPLETE, on_ws_open_complete, void*, on_ws_open_complete_context, ON_WS_FRAME_RECEIVED, on_ws_frame_received, void*, on_ws_frame_received_context, ON_WS_PEER_CLOSED, on_ws_peer_closed, void*, on_ws_peer_closed_context, ON_WS_ERROR, on_ws_error, void*, on_ws_error_context);
MOCKABLE_FUNCTION(, int, uws_client_close_async, UWS_CLIENT_HANDLE, uws_client, ON_WS_CLOSE_COMPLETE, on_ws_close_complete, void*, on_ws_close_complete_context);
MOCKABLE_FUNCTION(, int, uws_client_close_handshake_async, UWS_CLIENT_HANDLE, uws_client, uint16_t, close_code, const char*, close_reason, ON_WS_CLOSE_COMPLETE, on_ws_close_complete, void*, on_ws_close_complete_context);
MOCKABLE_FUNCTION(, int, uws_client_send_frame_async, UWS_CLIENT_HANDLE, uws_client, unsigned char, frame_type, const unsigned char*, buffer, size_t, size, bool, is_final, ON_WS_SEND_FRAME_COMPLETE, on_ws_send_frame_complete, void*, callback_context);
MOCKABLE_FUNCTION(, void, uws_client_dowork, UWS_CLIENT_HANDLE, uws_client);
MOCKABLE_FUNCTION(, int, uws_client_set_request_header, UWS_CLIENT_HANDLE, uws_client, const char*, name, const char*, value);
MOCKABLE_FUNCTION(, int, uws_client_set_option, UWS_CLIENT_HANDLE, uws_client, const char*, option_name, const void*, value);
MOCKABLE_FUNCTION(, OPTIONHANDLER_HANDLE, uws_client_retrieve_options, UWS_CLIENT_HANDLE, uws_client);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* UWS_CLIENT_H */
