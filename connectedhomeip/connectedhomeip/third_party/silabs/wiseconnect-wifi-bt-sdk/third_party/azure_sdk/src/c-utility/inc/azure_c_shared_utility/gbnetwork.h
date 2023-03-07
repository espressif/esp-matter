// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef GBNETWORK_H
#define GBNETWORK_H

#ifdef __cplusplus
#include <cstdlib>
#include <cstdint>
#else
#include <stdlib.h>
#include <stdint.h>
#endif

#include "umock_c/umock_c_prod.h"

#ifdef WIN32
    #include <winsock2.h>
#else
    #include <sys/socket.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/* all translation units that need network measurement need to have GB_MEASURE_NETWORK_FOR_THIS defined */
/* GB_DEBUG_NETWORK is the switch that turns the measurement on/off, so that it is not on always */
#if defined(GB_DEBUG_NETWORK)

MOCKABLE_FUNCTION(, int, gbnetwork_init);
MOCKABLE_FUNCTION(, void, gbnetwork_deinit);

#ifdef WIN32
MOCKABLE_FUNCTION(, int, gbnetwork_send, SOCKET, sock, const char*, buf, int, len, int, flags);
#else
MOCKABLE_FUNCTION(, ssize_t, gbnetwork_send, int, sock, const void*, buf, size_t, len, int, flags);
#endif

#ifdef WIN32
MOCKABLE_FUNCTION(, int, gbnetwork_recv, SOCKET, sock, char*, buf, int, len, int, flags);
#else
MOCKABLE_FUNCTION(, ssize_t, gbnetwork_recv, int, sock, void*, buf, size_t, len, int, flags);
#endif

MOCKABLE_FUNCTION(, uint64_t, gbnetwork_getBytesSent);
MOCKABLE_FUNCTION(, uint64_t, gbnetwork_getNumSends);
MOCKABLE_FUNCTION(, uint64_t, gbnetwork_getBytesRecv);
MOCKABLE_FUNCTION(, uint64_t, gbnetwork_getNumRecv);
MOCKABLE_FUNCTION(, void, gbnetwork_resetMetrics);

/* if GB_MEASURE_NETWORK_FOR_THIS is defined then we want to redirect network send functions to gbnetwork_xxx functions */
#ifdef GB_MEASURE_NETWORK_FOR_THIS
#define send gbnetwork_send
#define recv gbnetwork_recv
#endif

#else /* GB_DEBUG_NETWORK */

#define gbnetwork_init() 0
#define gbnetwork_deinit() ((void)0)
#define gbnetwork_getBytesSent() 0
#define gbnetwork_getNumSends() 0

#define gbnetwork_getBytesRecv() 0
#define gbnetwork_getNumRecv() 0

#endif /* GB_DEBUG_NETWORK */

#ifdef __cplusplus
}
#endif

#endif /* GBNETWORK_H */
