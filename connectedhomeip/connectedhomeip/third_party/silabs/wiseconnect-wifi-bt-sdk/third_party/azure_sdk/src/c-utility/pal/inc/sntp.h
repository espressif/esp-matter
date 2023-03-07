// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

/** @file threadapi.h
 *    @brief     This module implements support for creating new threads,
 *             terminating threads and sleeping threads.
 */

#ifndef AZURE_IOT_SNTP_H
#define AZURE_IOT_SNTP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "azure_macro_utils/macro_utils.h"
#include "umock_c/umock_c_prod.h"


/**
 * @brief    Set the url for the ntp server to be used. Must be called before
 *          SNTP_Init.
 *
 * @param   serverName    The url of the ntp server to be used. The char array
 *          passed in must remain valid between the SNTP_SetServerName and
 *          the SNTP_Deinit calls.
 *
 * @return    @c 0 if the API call is successful or an error
 *             code in case it fails.
 */
MOCKABLE_FUNCTION(, int, SNTP_SetServerName, const char*, serverName);

/**
 * @brief    Performs platform-specific sntp initialization, then loops until
 *             system time has been set from the ntp server.
 *
 *
 * @return    @c 0 if the API call is successful or an error
 *             code in case it fails.
 */
MOCKABLE_FUNCTION(, int, SNTP_Init);

/**
 * @brief    This function is called by a thread when the thread exits.
 */
MOCKABLE_FUNCTION(, void, SNTP_Deinit);


#ifdef __cplusplus
}
#endif

#endif /* AZURE_IOT_SNTP_H */
