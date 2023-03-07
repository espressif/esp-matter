// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "iothub_sc_version.h"

const char* IoTHubServiceClient_GetVersionString(void)
{
    /*Codes_SRS_IOTHUBSERVICECLIENT_12_040: [IoTHubServiceClient_GetVersionString shall return a pointer to a constant string which indicates the version of IoTHubServiceClient API.]*/
    return IOTHUB_SERVICE_CLIENT_VERSION;
}
