// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/platform.h"

#include <stdlib.h>
#include <unistd.h>
#include <sys/utsname.h>

#include "tlsio_appleios.h"

int platform_init(void)
{
    return 0;
}

const IO_INTERFACE_DESCRIPTION* platform_get_default_tlsio(void)
{
    return tlsio_appleios_get_interface_description();
}

STRING_HANDLE platform_get_platform_info(PLATFORM_INFO_OPTION options)
{
    // No applicable options, so ignoring parameter
    (void)options;

    STRING_HANDLE result;
    struct utsname nnn;

    if (uname(&nnn) == 0)
    {
        result = STRING_construct_sprintf("(%s; %s)", nnn.sysname, nnn.machine);

        if (result == NULL)
        {
            LogInfo("ERROR: Failed to create machine info string");
        }
    }
    else
    {
        LogInfo("WARNING: failed to find machine info.");
        result = STRING_construct("iOS");

        if (result == NULL)
        {
            LogInfo("ERROR: Failed to create machine info string");
        }
    }

    return result;
}

void platform_deinit(void)
{
}
