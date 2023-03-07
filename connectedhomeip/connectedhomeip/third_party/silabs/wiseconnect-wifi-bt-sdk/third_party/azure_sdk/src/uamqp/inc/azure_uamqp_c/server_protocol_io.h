// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef SERVER_PROTOCOL_IO_H
#define SERVER_PROTOCOL_IO_H

#ifdef __cplusplus
extern "C" {
#include "cstdint"
#else
#include "stdint.h"
#endif /* __cplusplus */

#include "azure_c_shared_utility/xio.h"

typedef struct SERVER_PROTOCOL_IO_CONFIG_TAG
{
    XIO_HANDLE underlying_io;
    ON_BYTES_RECEIVED* on_bytes_received;
    void** on_bytes_received_context;
} SERVER_PROTOCOL_IO_CONFIG;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SERVER_PROTOCOL_IO_H */
