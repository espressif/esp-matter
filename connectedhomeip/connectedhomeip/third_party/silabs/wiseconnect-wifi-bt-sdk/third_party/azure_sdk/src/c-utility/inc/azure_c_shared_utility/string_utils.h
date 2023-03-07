// Copyright (C) Microsoft Corporation. All rights reserved.

#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#ifdef __cplusplus
#include <cstdarg>
#include <cinttypes>
#include <cstdio>
#include <cwchar>
#else
#include <stddef.h>
#include <stdarg.h>
#include <wchar.h>
#include <inttypes.h>
#include <stdio.h>
#endif

#include "windows.h"

#include "azure_macro_utils/macro_utils.h"

#include "umock_c/umock_c_prod.h"

#ifdef __cplusplus
extern "C" {
#endif

/*produces a string as if printed by printf*/
extern char* sprintf_char_function(const char* format, ...);

/*produces a string as if printed by printf (will also verify arguments)*/
#define sprintf_char(format, ...) (0?printf((format), __VA_ARGS__):0, sprintf_char_function((format), __VA_ARGS__))

/*produces a string as if printed by wprintf*/
extern wchar_t* sprintf_wchar_function(const wchar_t* format, ...);
#define sprintf_wchar(format, ...) (0?wprintf((format), __VA_ARGS__):0, sprintf_wchar_function((format), __VA_ARGS__))

MOCKABLE_INTERFACE(string_utils_printf,
    /*produces a string as if printed by vprintf*/
    FUNCTION(, char*, vsprintf_char, const char*, format, va_list, va),
    /*produces a string as if printed by vwprintf*/
    FUNCTION(, wchar_t*, vsprintf_wchar, const wchar_t*, format, va_list, va)
)

/*
below macros can be used with printf. example:
printf("PartitionId = %" GUID_FORMAT "\n", GUID_VALUES(fabricDeployedStatefulServiceReplicaQueryResultItem->PartitionId)); produces on the screen:
PartitionId=316132b8-96a0-4bc7-aecc-a16e7c5a6bf6
*/
#define GUID_FORMAT "8.8" PRIx32 "-%4.4" PRIx16 "-%4.4" PRIx16 "-%4.4" PRIx16 "-%12.12" PRIx64
#define GUID_VALUES(guid) (guid).Data1, (guid).Data2, (guid).Data3, ((guid).Data4[0]<<8) + (guid).Data4[1], ((uint64_t)((guid).Data4[2])<<40) + ((uint64_t)((guid).Data4[3])<<32) + (((uint64_t)(guid).Data4[4])<<24) + ((guid).Data4[5]<<16) + ((guid).Data4[6]<<8) + ((guid).Data4[7])

/*takes a FILETIME, returns a nice string representation of it*/
char* FILETIME_toAsciiArray(const FILETIME* fileTime);

MOCKABLE_INTERFACE(string_utils_convert,
    /*produces the wchar_t* string representation of source (which is assumed to be multibyte). Returns NULL on any failure.*/
    FUNCTION(, wchar_t*, mbs_to_wcs, const char*, source),

    /*produces the multibyte char* string representation of source. Returns NULL on any failure.*/
    FUNCTION(, char*, wcs_to_mbs, const wchar_t*, source)
)

#ifdef __cplusplus
}
#endif

#endif
