// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef TLSIO_WOLFSSL_H
#define TLSIO_WOLFSSL_H

#include "azure_c_shared_utility/xio.h"
#include "azure_c_shared_utility/xlogging.h"
#include "umock_c/umock_c_prod.h"
#include "azure_c_shared_utility/const_defines.h"

#ifdef __cplusplus
extern "C" {
#include <cstddef>
#else
#include <stddef.h>
#endif /* __cplusplus */

extern const char* const OPTION_WOLFSSL_SET_DEVICE_ID;

MOCKABLE_FUNCTION(, CONCRETE_IO_HANDLE, tlsio_wolfssl_create, void*, io_create_parameters);
MOCKABLE_FUNCTION(, void, tlsio_wolfssl_destroy, CONCRETE_IO_HANDLE, tls_io);
MOCKABLE_FUNCTION(, int, tlsio_wolfssl_open, CONCRETE_IO_HANDLE, tls_io, ON_IO_OPEN_COMPLETE, on_io_open_complete, void*, on_io_open_complete_context, ON_BYTES_RECEIVED, on_bytes_received, void*, on_bytes_received_context, ON_IO_ERROR, on_io_error, void*, on_io_error_context);
MOCKABLE_FUNCTION(, int, tlsio_wolfssl_close, CONCRETE_IO_HANDLE, tls_io, ON_IO_CLOSE_COMPLETE, on_io_close_complete, void*, callback_context);
MOCKABLE_FUNCTION(, int, tlsio_wolfssl_send, CONCRETE_IO_HANDLE, tls_io, const void*, buffer, size_t, size, ON_SEND_COMPLETE, on_send_complete, void*, callback_context);
MOCKABLE_FUNCTION(, void, tlsio_wolfssl_dowork, CONCRETE_IO_HANDLE, tls_io);
MOCKABLE_FUNCTION(, int, tlsio_wolfssl_setoption, CONCRETE_IO_HANDLE, tls_io, const char*, optionName, const void*, value);

MOCKABLE_FUNCTION(, const IO_INTERFACE_DESCRIPTION*, tlsio_wolfssl_get_interface_description);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* TLSIO_WOLFSSL_H */
