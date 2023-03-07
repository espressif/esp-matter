// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef SASLCLIENTIO_H
#define SASLCLIENTIO_H

#ifdef __cplusplus
extern "C" {
#include <cstddef>
#else
#include <stddef.h>
#endif /* __cplusplus */

#include <stdbool.h>

#include "azure_c_shared_utility/xio.h"
#include "azure_uamqp_c/sasl_mechanism.h"

#include "umock_c/umock_c_prod.h"

typedef struct SASLCLIENTIO_CONFIG_TAG
{
    XIO_HANDLE underlying_io;
    SASL_MECHANISM_HANDLE sasl_mechanism;
} SASLCLIENTIO_CONFIG;

MOCKABLE_FUNCTION(, const IO_INTERFACE_DESCRIPTION*, saslclientio_get_interface_description);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SASLCLIENTIO_H */
