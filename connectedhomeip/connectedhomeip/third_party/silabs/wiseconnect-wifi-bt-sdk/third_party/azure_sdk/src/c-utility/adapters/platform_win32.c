// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "azure_c_shared_utility/platform.h"
#include "azure_c_shared_utility/optimize_size.h"
#include "azure_c_shared_utility/xio.h"
#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/httpapiex.h"
#include "winsock2.h"
#include "minwindef.h"
#include "winnt.h"

#ifdef USE_OPENSSL
#include "azure_c_shared_utility/tlsio_openssl.h"
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

#include "azure_c_shared_utility/tlsio_schannel.h"

int platform_init(void)
{
    int result;

    WSADATA wsaData;
    int error_code = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (error_code != 0)
    {
        LogError("WSAStartup failed: 0x%x", error_code);
        result = MU_FAILURE;
    }
    else
    {
        result = 0;
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
    }
    return result;
}

const IO_INTERFACE_DESCRIPTION* platform_get_default_tlsio(void)
{
#ifdef USE_OPENSSL
    return tlsio_openssl_get_interface_description();
#elif USE_CYCLONESSL
    return tlsio_cyclonessl_get_interface_description();
#elif USE_WOLFSSL
    return tlsio_wolfssl_get_interface_description();
#elif USE_BEARSSL
    return tlsio_bearssl_get_interface_description();
#elif USE_MBEDTLS
    return tlsio_mbedtls_get_interface_description();
#else
    return tlsio_schannel_get_interface_description();
#endif
}

static char* get_win_sqm_info(void)
{
    char* result;
    LONG openKey;
    DWORD options = 0;
    HKEY openResult;
    HKEY hKey = HKEY_LOCAL_MACHINE;
    LPCSTR subKey = "Software\\Microsoft\\SQMClient";
    LPCSTR lpValue = "MachineId";
    DWORD dwFlags = RRF_RT_ANY;

    LONG getRegValue = ERROR_INVALID_HANDLE;
    DWORD dataType;
    DWORD size = GUID_LENGTH;
    PVOID pvData;

    // SQM values are guids in the system, we will allocate enough space to hold that
    if ((result = (char*)malloc(GUID_LENGTH)) == NULL)
    {
        LogError("Failure allocating sqm info");
    }
    else if ((openKey = RegOpenKeyExA(hKey, subKey, options, KEY_READ, &openResult)) != ERROR_SUCCESS)
    {
        LogError("Failure opening registry key: %d:%s", GetLastError(), subKey);
        free(result);
        result = NULL;
    }
    else
    {
        pvData = result;
        if ((getRegValue = RegGetValueA(openResult, NULL, lpValue, dwFlags, &dataType, pvData, &size)) != ERROR_SUCCESS)
        {
            // Failed to read value, so try opening the 64-bit reg key
            // in case this is an x86 binary being run on Windows x64
            if ((openKey = RegOpenKeyExA(hKey, subKey, options, KEY_READ | KEY_WOW64_64KEY, &openResult)) != ERROR_SUCCESS)
            {
                LogError("Failure opening registry sub key: %d:%s", GetLastError(), subKey);
                free(result);
                result = NULL;
            }
            else if ((getRegValue = RegGetValueA(openResult, NULL, lpValue, dwFlags, &dataType, pvData, &size)) != ERROR_SUCCESS)
            {
                LogError("Failure opening registry sub key: %d:%s", GetLastError(), subKey);
                free(result);
                result = NULL;
            }
        }

        if (getRegValue != ERROR_SUCCESS)
        {
            LogError("Failure retrieving SQM info Error value: %d", GetLastError());
            free(result);
            result = NULL;
        }
        RegCloseKey(openResult);
    }
    return result;
}

STRING_HANDLE platform_get_platform_info(PLATFORM_INFO_OPTION options)
{
    // Expected format: "(<runtime name>; <operating system name>; <platform>)"
    STRING_HANDLE result;
    SYSTEM_INFO sys_info;
    OSVERSIONINFO osvi;
    char *arch;
    GetSystemInfo(&sys_info);

    switch (sys_info.wProcessorArchitecture)
    {
        case PROCESSOR_ARCHITECTURE_AMD64:
            arch = "x64";
            break;

        case PROCESSOR_ARCHITECTURE_ARM:
            arch = "ARM";
            break;

        case PROCESSOR_ARCHITECTURE_IA64:
            arch = "IA64";
            break;

        case PROCESSOR_ARCHITECTURE_INTEL:
            arch = "x32";
            break;

        default:
            arch = "UNKNOWN";
            break;
    }

    result = NULL;
    memset(&osvi, 0, sizeof(osvi));
    osvi.dwOSVersionInfoSize = sizeof(osvi);
#pragma warning(disable:4996)
    if (GetVersionEx(&osvi))
    {
        DWORD product_type;
        if (GetProductInfo(osvi.dwMajorVersion, osvi.dwMinorVersion, 0, 0, &product_type))
        {
            result = STRING_construct_sprintf("(native; WindowsProduct:0x%08x %d.%d; %s", product_type, osvi.dwMajorVersion, osvi.dwMinorVersion, arch);
        }
    }

    if (result == NULL)
    {
        DWORD dwVersion = GetVersion();
        result = STRING_construct_sprintf("(native; WindowsProduct:Windows NT %d.%d; %s", LOBYTE(LOWORD(dwVersion)), HIBYTE(LOWORD(dwVersion)), arch);
    }
#pragma warning(default:4996)

    if (result == NULL)
    {
        LogError("STRING_construct_sprintf failed");
    }
    else if (options & PLATFORM_INFO_OPTION_RETRIEVE_SQM)
    {
        // Failure here should continue
        char* sqm_info = get_win_sqm_info();
        if (sqm_info != NULL)
        {
            if (STRING_sprintf(result, "; %s)", sqm_info) != 0)
            {
                LogError("failure concat file");
            }
            free(sqm_info);
        }
        else
        {
            if (STRING_concat(result, ")") != 0)
            {
                LogError("failure concat file");
            }
        }
    }
    else
    {
        if (STRING_concat(result, ")") != 0)
        {
            LogError("failure concat file");
        }
    }

    return result;
}

void platform_deinit(void)
{
    (void)WSACleanup();

#ifndef DONT_USE_UPLOADTOBLOB
    HTTPAPIEX_Deinit();
#endif /* DONT_USE_UPLOADTOBLOB */

#ifdef USE_OPENSSL
    tlsio_openssl_deinit();
#endif
}
