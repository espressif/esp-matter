// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// MBED seems to not implement size_t properly within their cstddef.h header, and builds will fail if stddef.h is not included by the projects.
#include <stddef.h> 
#include "azure_c_shared_utility/platform.h"
#include "EthernetInterface.h"
#include "NTPClient.h"
#include "azure_c_shared_utility/optimize_size.h"
#include "azure_c_shared_utility/xio.h"
#include "azure_c_shared_utility/tlsio_wolfssl.h"

int setupRealTime(void)
{
    int result;

    if (EthernetInterface::connect())
    {
        result = MU_FAILURE;
    }
    else
    {
        NTPClient ntp;
        if (ntp.setTime("0.pool.ntp.org") != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            result = 0;
        }
        EthernetInterface::disconnect();
    }

    return result;
}

int platform_init(void)
{
    int result;

    if (EthernetInterface::init())
    {
        result = MU_FAILURE;
    }
    else if (setupRealTime() != 0)
    {
        result = MU_FAILURE;
    } 
    else if (EthernetInterface::connect())
    {
        result = MU_FAILURE;
    }
    else
    {
        result = 0;
    }

    return result;
}

const IO_INTERFACE_DESCRIPTION* platform_get_default_tlsio(void)
{
    return tlsio_wolfssl_get_interface_description();
}

STRING_HANDLE platform_get_platform_info(PLATFORM_INFO_OPTION options)
{
    // No applicable options, so ignoring parameter
    (void)options;

    // Expected format: "(<runtime name>; <operating system name>; <platform>)"

    return STRING_construct("(native; mbed; undefined)");
}

void platform_deinit(void)
{
    EthernetInterface::disconnect();
}
