// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stddef.h>
#include <stdio.h>
#include <inttypes.h>

#include "azure_macro_utils/macro_utils.h"
#include "ctest_windows.h"
#include "windows.h"
#include "psapi.h"

#define N_MAX_MODULES 100

#define TO_STRING_AND_COMPARE_FOR_TYPE(type, format) \
void MU_C2(type,_ToString)(char* string, size_t bufferSize, type val) \
{ \
    (void)snprintf(string, bufferSize, format, val); \
} \
int MU_C2(type,_Compare)(type left, type right) \
{ \
    return left != right; \
}

#ifdef VLD_OPT_REPORT_TO_STDOUT
#include "vld.h"
#endif

TO_STRING_AND_COMPARE_FOR_TYPE(ULONG, "%lu")
TO_STRING_AND_COMPARE_FOR_TYPE(ULONG64, "%" PRIu64)
TO_STRING_AND_COMPARE_FOR_TYPE(LONG, "%ld")
TO_STRING_AND_COMPARE_FOR_TYPE(LONG64, "%" PRId64)

void HRESULT_ToString(char* string, size_t bufferSize, HRESULT hr)
{
    /*see if the "system" can provide the code*/
    if (FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        hr,
        0, /*if you pass in zero, FormatMessage looks for a message for LANGIDs in the following order...*/
        (LPVOID)string, (DWORD)bufferSize, NULL) != 0)
    {
        /*success, SYSTEM was able to find the message*/
        /*return as is*/
        goto allok;
    }
    else
    {
        /*then maaaaaybe one of the other modules provides it*/

        HANDLE currentProcess = GetCurrentProcess();
        /*apparently this cannot fail and returns somewhat of a "pseudo handle"*/

        HMODULE hModules[N_MAX_MODULES];
        DWORD enumModulesUsedBytes;
        if (EnumProcessModules(currentProcess, hModules, sizeof(hModules), &enumModulesUsedBytes) == 0)
        {
            // no modules
        }
        else
        {
            size_t iModule;
            for (iModule = 0; iModule < (enumModulesUsedBytes / sizeof(HMODULE)); iModule++)
            {
                char fileName[MAX_PATH];
                if (GetModuleFileNameA(hModules[iModule], fileName, sizeof(fileName) / sizeof(fileName[0])) == 0)
                {
                    iModule = enumModulesUsedBytes / sizeof(HMODULE);
                    break;
                }
                else
                {
                    /*see if this module */
                    if (FormatMessageA(
                        FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_IGNORE_INSERTS,
                        hModules[iModule],
                        hr,
                        0,
                        (LPVOID)string, (DWORD)bufferSize, NULL) != 0)
                    {
                        break;
                    }
                    else
                    {
                        /*this module does not have it...*/
                    }
                }
            }

            if (iModule == (enumModulesUsedBytes / sizeof(HMODULE)))
            {
                // not found
            }
            else
            {
                goto allok;
            }
        }
    }

    // on error print the value as is
    (void)snprintf(string, bufferSize, "0x%08x", hr);

allok:;
}

int HRESULT_Compare(HRESULT left, HRESULT right)
{
    return left != right;
}
