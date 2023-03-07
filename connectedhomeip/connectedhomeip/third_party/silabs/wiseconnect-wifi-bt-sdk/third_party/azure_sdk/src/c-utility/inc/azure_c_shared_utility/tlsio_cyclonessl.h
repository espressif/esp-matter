// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef TLSIO_CYCLONESSL_H
#define TLSIO_CYCLONESSL_H

#include "azure_c_shared_utility/xio.h"
#include "umock_c/umock_c_prod.h"

#ifdef __cplusplus
extern "C" {
#include <cstddef>
#else
#include <stddef.h>
#endif /* __cplusplus */

MOCKABLE_FUNCTION(, const IO_INTERFACE_DESCRIPTION*, tlsio_cyclonessl_get_interface_description);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* TLSIO_CYCLONESSL_H */
