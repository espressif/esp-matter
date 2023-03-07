// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef HTTP_PROXY_IO_H
#define HTTP_PROXY_IO_H

#include "azure_c_shared_utility/xio.h"
#include "umock_c/umock_c_prod.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct HTTP_PROXY_IO_CONFIG_TAG
{
    const char* hostname;
    int port;
    const char* proxy_hostname;
    int proxy_port;
    const char* username;
    const char* password;
} HTTP_PROXY_IO_CONFIG;

MOCKABLE_FUNCTION(, const IO_INTERFACE_DESCRIPTION*, http_proxy_io_get_interface_description);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HTTP_PROXY_IO_H */
