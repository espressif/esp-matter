// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

/** @file iothub_sc_version.h
*   @brief Functions for managing the service client SDK version
*/

#ifndef IOTHUB_SC_VERSION_H
#define IOTHUB_SC_VERSION_H

#define IOTHUB_SERVICE_CLIENT_TYPE_PREFIX "iothubserviceclient"
#define IOTHUB_SERVICE_CLIENT_BACKSLASH "/"

#define IOTHUB_SERVICE_CLIENT_VERSION "1.1.0"

#include "umock_c/umock_c_prod.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief   Returns a pointer to a null terminated string containing the
     *          current IoT Hub Service Client SDK version.
     *
     * @return  Pointer to a null terminated string containing the
     *          current IoT Hub Service Client SDK version.
     */
    MOCKABLE_FUNCTION(, const char*, IoTHubServiceClient_GetVersionString);

#ifdef __cplusplus
}
#endif

#endif // IOTHUB_SC_VERSION_H
