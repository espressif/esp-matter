// Copyright (C) Microsoft Corporation. All rights reserved.

/*poor man's string routines*/
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#include "windows.h"

#include "azure_macro_utils/macro_utils.h"
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/xlogging.h"

#include "azure_c_shared_utility/string_utils.h"

IMPLEMENT_MOCKABLE_FUNCTION(, char*, vsprintf_char, const char*, format, va_list, va)
{
    char* result;
    int neededSize = vsnprintf(NULL, 0, format, va);
    if (neededSize < 0)
    {
        LogError("failure in vsnprintf");
        result = NULL;
    }
    else
    {
        result = (char*)malloc((neededSize + 1) * sizeof(char));
        if (result == NULL)
        {
            LogError("failure in malloc");
            /*return as is*/
        }
        else
        {
            if (vsnprintf(result, neededSize + 1, format, va) != neededSize)
            {
                LogError("inconsistent vsnprintf behavior");
                free(result);
                result = NULL;
            }
        }
    }
    return result;
}

IMPLEMENT_MOCKABLE_FUNCTION(, wchar_t*, vsprintf_wchar, const wchar_t*, format, va_list, va)
{
    wchar_t* result;
    int neededSize = vswprintf(NULL, 0, format, va);
    if (neededSize < 0)
    {
        LogError("failure in swprintf");
        result = NULL;
    }
    else
    {
        result = (wchar_t*)malloc((neededSize + 1)*sizeof(wchar_t));
        if (result == NULL)
        {
            LogError("failure in malloc");
            /*return as is*/
        }
        else
        {
            if (vswprintf(result, neededSize + 1, format, va) != neededSize)
            {
                LogError("inconsistent vswprintf behavior");
                free(result);
                result = NULL;
            }
        }
    }
    return result;
}

/*returns a char* that is as if printed by printf*/
/*needs to be free'd after usage*/
char* sprintf_char_function(const char* format, ...)
{
    char* result;
    va_list va;
    va_start(va, format);
    result = vsprintf_char(format, va);
    va_end(va);
    return result;
}

wchar_t* sprintf_wchar_function(const wchar_t* format, ...)
{
    wchar_t* result;
    va_list va;
    va_start(va, format);
    result = vsprintf_wchar(format, va);
    va_end(va);
    return result;
}

/*takes a FILETIME, returns a nice string representation of it*/
char* FILETIME_toAsciiArray(const FILETIME* fileTime)
{
    char* result;
    if (fileTime == NULL)
    {
        LogError("invalid argument const FILETIME* fileTime=%p", fileTime);
        result = NULL;
    }
    else
    {
        FILETIME localFileTime;
        if (FileTimeToLocalFileTime(fileTime, &localFileTime) == 0)
        {
            LogLastError("failure in FileTimeToLocalFileTime");
            result = NULL;
        }
        else
        {
            SYSTEMTIME systemTime;
            if (FileTimeToSystemTime(&localFileTime, &systemTime) == 0)
            {
                LogLastError("failure in FileTimeToLocalFileTime");
                result = NULL;
            }
            else
            {
                char localDate[255];
                if (GetDateFormat(LOCALE_USER_DEFAULT, DATE_LONGDATE, &systemTime, NULL, localDate, sizeof(localDate)/sizeof(localDate[0])) == 0)
                {
                    LogLastError("failure in GetDateFormat");
                    result = NULL;
                }
                else
                {
                    char localTime[255];
                    if (GetTimeFormat(LOCALE_USER_DEFAULT, 0, &systemTime, NULL, localTime, sizeof(localTime)/sizeof(localTime[0])) == 0)
                    {
                        LogLastError("failure in GetTimeFormat");
                        result = NULL;
                    }
                    else
                    {
                        result = sprintf_char("%s %s", localDate, localTime);
                        /*return as is*/
                    }
                }
            }
        }
    }
    return result;
}

IMPLEMENT_MOCKABLE_FUNCTION(, wchar_t*, mbs_to_wcs, const char*, source)
{
    wchar_t *result;
    if (source == NULL)
    {
        LogError("invalid argument const char* source=%s", MU_P_OR_NULL(source));
        result = NULL;
    }
    else
    {
        const char* sameAsSource = source;
        /*assuming source is a multibyte character string*/
        mbstate_t state = { 0 };/*initial state!*/
        size_t nwc = mbsrtowcs(NULL, &sameAsSource, 0, &state); /*note 350 from C standard seems to indicate that NULL is a valid pointer to pass here*/
        if (nwc == (size_t)(-1))
        {
            LogError("failure to get the length of the string %s in multibyte characters", strerror(errno));
            result = NULL;
        }
        else
        {
            result = (wchar_t*)malloc(sizeof(wchar_t)*(nwc+1));
            if (result == NULL)
            {
                LogError("failure in malloc");
                /*return as is*/
            }
            else
            {
                size_t nwc2 = mbsrtowcs(result, &sameAsSource, nwc+1, &state);
                if (nwc2 != nwc)
                {
                    LogError("unexpected inconsistency in mbsrtowcs");
                }
                else
                {
                    /*all is fine*/
                    goto allOk;
                }
                free(result);
                result = NULL;
            }
allOk:;
        }
    }
    return result;
}

IMPLEMENT_MOCKABLE_FUNCTION(, char*, wcs_to_mbs, const wchar_t*, source)
{
    char *result;
    if (source == NULL)
    {
        LogError("invalid argument const wchar_t* source=%ls", MU_WP_OR_NULL(source));
        result = NULL;
    }
    else
    {
        const wchar_t* sameAsSource = source;
        mbstate_t state = { 0 };/*initial state!*/
        size_t nc = wcsrtombs(NULL, &sameAsSource, 0, &state);
        if (nc == (size_t)(-1))
        {
            LogError("failure to get the length of the string %s in characters", strerror(errno));
            result = NULL;
        }
        else
        {
            result = (char*)malloc(sizeof(char)*(nc + 1));
            if (result == NULL)
            {
                LogError("failure in malloc");
                /*return as is*/
            }
            else
            {
                size_t nc2 = wcsrtombs(result, &sameAsSource, nc + 1, &state);
                if (nc2 != nc)
                {
                    LogError("unexpected inconsistency in wcsrtombs");
                }
                else
                {
                    /*all is fine*/
                    goto allOk;
                }
                free(result);
                result = NULL;
            }
        allOk:;
        }
    }
    return result;
}
