// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "azure_c_shared_utility/xlogging.h"

/*Codes_SRS_SNTP_LWIP_30_001: [ The ntp_lwip shall implement the methods defined in sntp.h. ]*/
#include "sntp.h"
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/agenttime.h"
#include "sntp_os.h"


/*Codes_SRS_SNTP_LWIP_30_002: [ The serverName parameter shall be an NTP server URL which shall not be validated. ]*/
/*Codes_SRS_SNTP_LWIP_30_003: [ The SNTP_SetServerName shall set the NTP server to be used by ntp_lwip and return 0 to indicate success.]*/
//
// SNTP_SetServerName must be called before SNTP_Init.
// The character array pointed to by serverName parameter must persist
// between calls to SNTP_SetServerName and SNTP_Deinit because the
// char* is stored and no copy of the string is made.
//
// SNTP_SetServerName is a wrapper for the lwIP call sntp_setservername
// and defers parameter validation to the lwIP library.
//
// Future implementations of this adapter may allow multiple calls to
// SNTP_SetServerName in order to support multiple servers.
//
int SNTP_SetServerName(const char* serverName)
{
    // Future implementations could easily allow multiple calls to SNTP_SetServerName
    // by incrementing the index supplied to sntp_setservername
    sntp_setservername(0, (char*)serverName);
    return 0;
}

/*Codes_SRS_SNTP_LWIP_30_004: [ SNTP_Init shall initialize the SNTP client, contact the NTP server to set system time, then return 0 to indicate success (lwIP has no failure path). ]*/
int SNTP_Init()
{
    LogInfo("Initializing SNTP");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_init();
    time_t ts = 0;
    // Before 1980 is uninitialized
    while (ts < 10 * 365 * 24 * 3600)
    {
        ThreadAPI_Sleep(1000);
        ts = get_time(NULL);

    }
    LogInfo("SNTP initialization complete");
    return 0;
}

/*Codes_SRS_SNTP_LWIP_30_005: [ SNTP_Denit shall deinitialize the SNTP client. ]*/
void SNTP_Deinit()
{
    sntp_stop();
}
