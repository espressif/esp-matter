// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "azure_c_shared_utility/platform.h"
#include "azure_c_shared_utility/xio.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/httpapiex.h"
#ifdef USE_OPENSSL
#include "azure_c_shared_utility/tlsio_openssl.h"
#else
const IO_INTERFACE_DESCRIPTION* tlsio_openssl_get_interface_description();
#endif
#if USE_CYCLONESSL
#include "azure_c_shared_utility/tlsio_cyclonessl.h"
#endif
#if USE_WOLFSSL
#include "azure_c_shared_utility/tlsio_wolfssl.h"
#endif
#if USE_MBEDTLS
#include "azure_c_shared_utility/tlsio_mbedtls.h"
#endif
#if USE_BEARSSL
#include "azure_c_shared_utility/tlsio_bearssl.h"
#endif

#include <stdlib.h>
#include <unistd.h>
#include <sys/utsname.h>

int platform_init(void)
{
    int result = 0;
#ifndef DONT_USE_UPLOADTOBLOB
    if (HTTPAPIEX_Init() == HTTPAPIEX_ERROR)
    {
        LogError("HTTP for upload to blob failed on initialization.");
        result = MU_FAILURE;
    }
#endif /* DONT_USE_UPLOADTOBLOB */
#ifdef USE_OPENSSL
    if (result == 0)
    {
        result = tlsio_openssl_init();
    }
#endif
    return result;
}

const IO_INTERFACE_DESCRIPTION* platform_get_default_tlsio(void)
{
#if USE_CYCLONESSL
    return tlsio_cyclonessl_get_interface_description();
#elif USE_WOLFSSL
    return tlsio_wolfssl_get_interface_description();
#elif USE_MBEDTLS
    return tlsio_mbedtls_get_interface_description();
#elif USE_BEARSSL
    return tlsio_bearssl_get_interface_description();
#else
    // Default to openssl
    return tlsio_openssl_get_interface_description();
#endif
}

STRING_HANDLE platform_get_platform_info(PLATFORM_INFO_OPTION options)
{
    // No applicable options, so ignoring parameter
    (void)options;

    // Expected format: "(<runtime name>; <operating system name>; <platform>)"

    STRING_HANDLE result;
    struct utsname nnn;

    if (uname(&nnn) == 0)
    {
        result = STRING_construct_sprintf("(native; %s; %s)", nnn.sysname, nnn.machine);
    }
    else
    {
        LogInfo("WARNING: failed to find machine info.");
        result = STRING_construct("(native; Linux; undefined)");
    }
    return result;
}

void platform_deinit(void)
{
#ifndef DONT_USE_UPLOADTOBLOB
    HTTPAPIEX_Deinit();
#endif /* DONT_USE_UPLOADTOBLOB */
#ifdef USE_OPENSSL
    tlsio_openssl_deinit();
#endif
}
