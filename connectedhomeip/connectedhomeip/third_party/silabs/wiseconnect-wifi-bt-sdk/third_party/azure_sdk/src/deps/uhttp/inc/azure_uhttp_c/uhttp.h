// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef UHTTP_H
#define UHTTP_H

#ifdef __cplusplus
extern "C" {
#include <cstddef>
#else
#include <stddef.h>
#include <stdbool.h>
#endif /* __cplusplus */

#include "azure_c_shared_utility/httpheaders.h"
#include "azure_macro_utils/macro_utils.h"
#include "azure_c_shared_utility/xio.h"
#include "umock_c/umock_c_prod.h"

typedef struct HTTP_CLIENT_HANDLE_DATA_TAG* HTTP_CLIENT_HANDLE;

#define HTTP_CLIENT_RESULT_VALUES       \
    HTTP_CLIENT_OK,                     \
    HTTP_CLIENT_INVALID_ARG,            \
    HTTP_CLIENT_ERROR,                  \
    HTTP_CLIENT_OPEN_FAILED,            \
    HTTP_CLIENT_SEND_FAILED,            \
    HTTP_CLIENT_ALREADY_INIT,           \
    HTTP_CLIENT_HTTP_HEADERS_FAILED,    \
    HTTP_CLIENT_INVALID_STATE           \


/** @brief Enumeration specifying the possible return values for the APIs in
*           this module.
*/
MU_DEFINE_ENUM(HTTP_CLIENT_RESULT, HTTP_CLIENT_RESULT_VALUES);

#define HTTP_CLIENT_REQUEST_TYPE_VALUES \
    HTTP_CLIENT_REQUEST_OPTIONS,        \
    HTTP_CLIENT_REQUEST_GET,            \
    HTTP_CLIENT_REQUEST_POST,           \
    HTTP_CLIENT_REQUEST_PUT,            \
    HTTP_CLIENT_REQUEST_DELETE,         \
    HTTP_CLIENT_REQUEST_PATCH

MU_DEFINE_ENUM(HTTP_CLIENT_REQUEST_TYPE, HTTP_CLIENT_REQUEST_TYPE_VALUES);

#define HTTP_CALLBACK_REASON_VALUES     \
    HTTP_CALLBACK_REASON_OK,            \
    HTTP_CALLBACK_REASON_OPEN_FAILED,   \
    HTTP_CALLBACK_REASON_SEND_FAILED,   \
    HTTP_CALLBACK_REASON_ERROR,         \
    HTTP_CALLBACK_REASON_PARSING_ERROR, \
    HTTP_CALLBACK_REASON_DESTROY,       \
    HTTP_CALLBACK_REASON_DISCONNECTED

MU_DEFINE_ENUM(HTTP_CALLBACK_REASON, HTTP_CALLBACK_REASON_VALUES);

typedef void(*ON_HTTP_OPEN_COMPLETE_CALLBACK)(void* callback_ctx, HTTP_CALLBACK_REASON open_result);
typedef void(*ON_HTTP_ERROR_CALLBACK)(void* callback_ctx, HTTP_CALLBACK_REASON error_result);
typedef void(*ON_HTTP_REQUEST_CALLBACK)(void* callback_ctx, HTTP_CALLBACK_REASON request_result, const unsigned char* content, size_t content_length, unsigned int status_code,
    HTTP_HEADERS_HANDLE response_headers);
typedef void(*ON_HTTP_CLOSED_CALLBACK)(void* callback_ctx);

MOCKABLE_FUNCTION(, HTTP_CLIENT_HANDLE, uhttp_client_create, const IO_INTERFACE_DESCRIPTION*, io_interface_desc, const void*, xio_param, ON_HTTP_ERROR_CALLBACK, on_http_error, void*, callback_ctx);
MOCKABLE_FUNCTION(, void, uhttp_client_destroy, HTTP_CLIENT_HANDLE, handle);
MOCKABLE_FUNCTION(, HTTP_CLIENT_RESULT, uhttp_client_open, HTTP_CLIENT_HANDLE, handle, const char*, host, int, port_num, ON_HTTP_OPEN_COMPLETE_CALLBACK, on_connect, void*, callback_ctx);
MOCKABLE_FUNCTION(, void, uhttp_client_close, HTTP_CLIENT_HANDLE, handle, ON_HTTP_CLOSED_CALLBACK, on_close_callback, void*, callback_ctx);
MOCKABLE_FUNCTION(, HTTP_CLIENT_RESULT, uhttp_client_execute_request, HTTP_CLIENT_HANDLE, handle, HTTP_CLIENT_REQUEST_TYPE, request_type, const char*, relative_path,
    HTTP_HEADERS_HANDLE, http_header_handle, const unsigned char*, content, size_t, content_length, ON_HTTP_REQUEST_CALLBACK, on_request_callback, void*, callback_ctx);

MOCKABLE_FUNCTION(, void, uhttp_client_dowork, HTTP_CLIENT_HANDLE, handle);

MOCKABLE_FUNCTION(, HTTP_CLIENT_RESULT, uhttp_client_set_trace, HTTP_CLIENT_HANDLE, handle, bool, trace_on, bool, trace_data);
MOCKABLE_FUNCTION(, HTTP_CLIENT_RESULT, uhttp_client_set_X509_cert, HTTP_CLIENT_HANDLE, handle, bool, ecc_type, const char*, certificate, const char*, private_key);
MOCKABLE_FUNCTION(, HTTP_CLIENT_RESULT, uhttp_client_set_trusted_cert, HTTP_CLIENT_HANDLE, handle, const char*, certificate);
MOCKABLE_FUNCTION(, const char*, uhttp_client_get_trusted_cert, HTTP_CLIENT_HANDLE, handle);
MOCKABLE_FUNCTION(, HTTP_CLIENT_RESULT, uhttp_client_set_option, HTTP_CLIENT_HANDLE, handle, const char*, optionName, const void*, value);
MOCKABLE_FUNCTION(, XIO_HANDLE, uhttp_client_get_underlying_xio, HTTP_CLIENT_HANDLE, handle);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* UHTTP_H */
