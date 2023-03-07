// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "ctrs_sprintf.h"

static char* ctrs_vsprintf_char(const char* format, va_list va)
{
    char* result;
    int neededSize = vsnprintf(NULL, 0, format, va);
    if (neededSize < 0)
    {
        (void)printf("failure in vsnprintf\n");
        result = NULL;
    }
    else
    {
        result = malloc(neededSize + 1);
        if (result == NULL)
        {
            (void)printf("failure in malloc\n");
            /*return as is*/
        }
        else
        {
            if (vsnprintf(result, neededSize + 1, format, va) != neededSize)
            {
                (void)printf("inconsistent vsnprintf behavior\n");
                free(result);
                result = NULL;
            }
        }
    }
    return result;
}

/*returns a char* that is as if printed by printf*/
/*needs to be free'd after usage*/
char* ctrs_sprintf_char(const char* format, ...)
{
    char* result;
    va_list va;
    va_start(va, format);
    result = ctrs_vsprintf_char(format, va);
    va_end(va);
    return result;
}

void ctrs_sprintf_free(char* string)
{
    free(string);
}
