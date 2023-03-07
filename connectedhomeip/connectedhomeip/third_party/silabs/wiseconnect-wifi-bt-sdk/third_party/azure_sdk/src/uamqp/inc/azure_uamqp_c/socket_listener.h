// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef SOCKETLISTENER_H
#define SOCKETLISTENER_H

#include "azure_c_shared_utility/xio.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "umock_c/umock_c_prod.h"

    typedef struct SOCKET_LISTENER_INSTANCE_TAG* SOCKET_LISTENER_HANDLE;
    typedef void(*ON_SOCKET_ACCEPTED)(void* context, const IO_INTERFACE_DESCRIPTION* interface_description, void* io_parameters);

    MOCKABLE_FUNCTION(, SOCKET_LISTENER_HANDLE, socketlistener_create, int, port);
    MOCKABLE_FUNCTION(, void, socketlistener_destroy, SOCKET_LISTENER_HANDLE, socket_listener);
    MOCKABLE_FUNCTION(, int, socketlistener_start, SOCKET_LISTENER_HANDLE, socket_listener, ON_SOCKET_ACCEPTED, on_socket_accepted, void*, callback_context);
    MOCKABLE_FUNCTION(, int, socketlistener_stop, SOCKET_LISTENER_HANDLE, socket_listener);
    MOCKABLE_FUNCTION(, void, socketlistener_dowork, SOCKET_LISTENER_HANDLE, socket_listener);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SOCKETLISTENER_H */
