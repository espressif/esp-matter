// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

/** @file   iothub_client_diagnostic.h
*    @brief  The @c diagnostic is a component that helps to add predefined diagnostic
            properties to message for end to end diagnostic purpose
*/

#ifndef IOTHUB_CLIENT_DIAGNOSTIC_H
#define IOTHUB_CLIENT_DIAGNOSTIC_H

#include "umock_c/umock_c_prod.h"

#include "iothub_message.h"
#include <stdint.h>

#ifdef __cplusplus
#include <cstddef>
extern "C" {
#else
#include <stddef.h>
#endif

/** @brief diagnostic related setting */
typedef struct IOTHUB_DIAGNOSTIC_SETTING_DATA_TAG
{
    uint32_t diagSamplingPercentage;
    uint32_t currentMessageNumber;
} IOTHUB_DIAGNOSTIC_SETTING_DATA;

/**
    * @brief    Adds diagnostic information to message if:
    *           a. diagSetting->diagSamplingPercentage > 0 and
    *           b. the number of current message matches sample rule specified by diagSetting->diagSamplingPercentage
    *
    * @param    diagSetting        Pointer to an @c IOTHUB_DIAGNOSTIC_SETTING_DATA structure
    *
    * @param    messageHandle    message handle
    *
    * @return    0 upon success
    */
MOCKABLE_FUNCTION(, int, IoTHubClient_Diagnostic_AddIfNecessary, IOTHUB_DIAGNOSTIC_SETTING_DATA *, diagSetting, IOTHUB_MESSAGE_HANDLE, messageHandle);

#ifdef __cplusplus
}
#endif

#endif /* IOTHUB_CLIENT_DIAGNOSTIC_H */
