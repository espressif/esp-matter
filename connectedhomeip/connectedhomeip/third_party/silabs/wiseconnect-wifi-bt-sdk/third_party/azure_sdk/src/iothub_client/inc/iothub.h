// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef IOTHUB_H
#define IOTHUB_H

#include "umock_c/umock_c_prod.h"

#ifdef __cplusplus
extern "C"
{
#else
#endif
    /**
    * @brief    IoTHubClient_Init Initializes the IoTHub Client System.
    *
    * @return   int zero upon success, any other value upon failure.
    */
    MOCKABLE_FUNCTION(, int, IoTHub_Init);

    /**
    * @brief    IoTHubClient_Deinit Frees resources initialized in the IoTHubClient_Init function call.
    *
    */
    MOCKABLE_FUNCTION(, void, IoTHub_Deinit);

#ifdef __cplusplus
}
#endif

#endif /* IOTHUB_H */
