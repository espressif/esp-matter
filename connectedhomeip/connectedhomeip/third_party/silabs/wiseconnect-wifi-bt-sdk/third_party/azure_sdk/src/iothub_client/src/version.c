// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "iothub_client_version.h"

const char* IoTHubClient_GetVersionString(void)
{
    /*Codes_SRS_IOTHUBCLIENT_05_001: [IoTHubClient_GetVersionString shall return a pointer to a constant string which indicates the version of IoTHubClient API.]*/
    return IOTHUB_SDK_VERSION;
}
