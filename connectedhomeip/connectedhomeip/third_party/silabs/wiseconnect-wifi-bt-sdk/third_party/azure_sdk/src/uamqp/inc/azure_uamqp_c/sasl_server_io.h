// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef SASL_SERVER_IO_H
#define SASL_SERVER_IO_H

#ifdef __cplusplus
extern "C" {
#include <cstddef>
#else
#include <stddef.h>
#include <stdbool.h>
#endif /* __cplusplus */

#include "azure_c_shared_utility/xio.h"
#include "azure_uamqp_c/server_protocol_io.h"
#include "azure_uamqp_c/sasl_server_mechanism.h"

#include "umock_c/umock_c_prod.h"

typedef struct SASL_SERVER_IO_TAG
{
    SERVER_PROTOCOL_IO_CONFIG server_protocol_io;
    const SASL_SERVER_MECHANISM_HANDLE* sasl_server_mechanisms;
    size_t sasl_server_mechanism_count;
} SASL_SERVER_IO;

MOCKABLE_FUNCTION(, const IO_INTERFACE_DESCRIPTION*, sasl_server_io_get_interface_description);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SASL_SERVER_IO_H */
