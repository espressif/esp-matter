// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef WS_URL_H
#define WS_URL_H

#ifdef __cplusplus
#include <cstddef>
#else
#include <stddef.h>
#include <stdbool.h>
#endif

#include "umock_c/umock_c_prod.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct WS_URL_TAG* WS_URL_HANDLE;

MOCKABLE_FUNCTION(, WS_URL_HANDLE, ws_url_create, const char*, url);
MOCKABLE_FUNCTION(, int, ws_url_is_secure, WS_URL_HANDLE, url, bool*, is_secure);
MOCKABLE_FUNCTION(, int, ws_url_get_host, WS_URL_HANDLE, url, const char**, host, size_t*, length);
MOCKABLE_FUNCTION(, int, ws_url_get_port, WS_URL_HANDLE, url, size_t*, port);
MOCKABLE_FUNCTION(, int, ws_url_get_path, WS_URL_HANDLE, url, const char**, path, size_t*, length);
MOCKABLE_FUNCTION(, int, ws_url_get_query, WS_URL_HANDLE, url, const char**, query, size_t*, length);
MOCKABLE_FUNCTION(, void, ws_url_destroy, WS_URL_HANDLE, url);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // WS_URL_H