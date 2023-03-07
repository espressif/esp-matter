// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef IOTHUB_TRANSPORT_LL_H
#define IOTHUB_TRANSPORT_LL_H

#ifdef __cplusplus
#include <cstddef>
#include <cstdint>
extern "C"
{
#else
#include <stddef.h>
#include <stdint.h>
#endif

#include "umock_c/umock_c_prod.h"

typedef void* TRANSPORT_LL_HANDLE;
typedef void* IOTHUB_DEVICE_HANDLE;

struct TRANSPORT_HANDLE_DATA_TAG;
typedef struct TRANSPORT_HANDLE_DATA_TAG* TRANSPORT_HANDLE;

typedef void* METHOD_HANDLE;

struct TRANSPORT_PROVIDER_TAG;
typedef struct TRANSPORT_PROVIDER_TAG TRANSPORT_PROVIDER;

typedef const TRANSPORT_PROVIDER*(*IOTHUB_CLIENT_TRANSPORT_PROVIDER)(void);

MOCKABLE_FUNCTION(, TRANSPORT_HANDLE, IoTHubTransport_Create, IOTHUB_CLIENT_TRANSPORT_PROVIDER, protocol, const char*, iotHubName, const char*, iotHubSuffix);
MOCKABLE_FUNCTION(, void, IoTHubTransport_Destroy, TRANSPORT_HANDLE, transportHandle);
MOCKABLE_FUNCTION(, TRANSPORT_LL_HANDLE, IoTHubTransport_GetLLTransport, TRANSPORT_HANDLE, transportHandle);

#ifdef __cplusplus
}
#endif

#endif /* IOTHUB_TRANSPORT_LL_H */

