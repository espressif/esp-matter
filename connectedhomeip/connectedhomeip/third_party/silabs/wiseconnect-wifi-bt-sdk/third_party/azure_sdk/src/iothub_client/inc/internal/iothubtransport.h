// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef IOTHUB_TRANSPORT_H
#define IOTHUB_TRANSPORT_H

#include "azure_c_shared_utility/lock.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "iothub_transport_ll.h"
#include "iothub_client_core.h"
#include "internal/iothub_client_private.h"
#include "internal/iothub_transport_ll_private.h"
#include "internal/iothub_client_authorization.h"

#ifndef IOTHUB_CLIENT_CORE_INSTANCE_TYPE
typedef struct IOTHUB_CLIENT_CORE_INSTANCE_TAG* IOTHUB_CLIENT_CORE_HANDLE;
#define IOTHUB_CLIENT_CORE_INSTANCE_TYPE
#endif // IOTHUB_CLIENT_CORE_INSTANCE

#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif

#include "umock_c/umock_c_prod.h"

    /** @brief  This struct captures IoTHub transport configuration. */
    struct IOTHUBTRANSPORT_CONFIG_TAG
    {
        const IOTHUB_CLIENT_CONFIG* upperConfig;
        PDLIST_ENTRY waitingToSend;
        IOTHUB_AUTHORIZATION_HANDLE auth_module_handle;
        const char* moduleId;
    };

    typedef void(*IOTHUB_CLIENT_MULTIPLEXED_DO_WORK)(void* iotHubClientInstance);

    MOCKABLE_FUNCTION(, LOCK_HANDLE, IoTHubTransport_GetLock, TRANSPORT_HANDLE, transportHandle);
    MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubTransport_StartWorkerThread, TRANSPORT_HANDLE, transportHandle, IOTHUB_CLIENT_CORE_HANDLE, clientHandle, IOTHUB_CLIENT_MULTIPLEXED_DO_WORK, muxDoWork);
    MOCKABLE_FUNCTION(, bool, IoTHubTransport_SignalEndWorkerThread, TRANSPORT_HANDLE, transportHandle, IOTHUB_CLIENT_CORE_HANDLE, clientHandle);
    MOCKABLE_FUNCTION(, void, IoTHubTransport_JoinWorkerThread, TRANSPORT_HANDLE, transportHandle, IOTHUB_CLIENT_CORE_HANDLE, clientHandle);

#ifdef __cplusplus
}
#endif

#endif /* IOTHUB_TRANSPORT_H */
