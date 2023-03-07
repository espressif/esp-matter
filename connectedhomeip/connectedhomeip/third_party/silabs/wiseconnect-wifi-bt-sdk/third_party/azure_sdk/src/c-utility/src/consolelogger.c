// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/consolelogger.h"

#if (defined(_MSC_VER))
#include "windows.h"

/*returns a string as if printed by vprintf*/
static char* vprintf_alloc(const char* format, va_list va)
{
    char* result;
    int neededSize = vsnprintf(NULL, 0, format, va);
    if (neededSize < 0)
    {
        result = NULL;
    }
    else
    {
        result = (char*)malloc(neededSize + 1);
        if (result == NULL)
        {
            /*return as is*/
        }
        else
        {
            if (vsnprintf(result, neededSize + 1, format, va) != neededSize)
            {
                free(result);
                result = NULL;
            }
        }
    }
    return result;
}

/*returns a string as if printed by printf*/
static char* printf_alloc(const char* format, ...)
{
    char* result;
    va_list va;
    va_start(va, format);
    result = vprintf_alloc(format, va);
    va_end(va);
    return result;
}

/*returns NULL if it fails*/
static char* lastErrorToString(DWORD lastError)
{
    char* result;
    if (lastError == 0)
    {
        result = printf_alloc(""); /*no error should appear*/
        if (result == NULL)
        {
            (void)printf("failure in printf_alloc");
        }
        else
        {
            /*return as is*/
        }
    }
    else
    {
        char temp[MESSAGE_BUFFER_SIZE];
        if (FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, lastError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), temp, MESSAGE_BUFFER_SIZE, NULL) == 0)
        {
            result = printf_alloc("GetLastError()=0X%x", lastError);
            if (result == NULL)
            {
                (void)printf("failure in printf_alloc\n");
                /*return as is*/
            }
            else
            {
                /*return as is*/
            }
        }
        else
        {
            /*eliminate the \r or \n from the string*/
            /*one replace of each is enough*/
            char* whereAreThey;
            if ((whereAreThey = strchr(temp, '\r')) != NULL)
            {
                *whereAreThey = '\0';
            }
            if ((whereAreThey = strchr(temp, '\n')) != NULL)
            {
                *whereAreThey = '\0';
            }

            result = printf_alloc("GetLastError()==0X%x (%s)", lastError, temp);

            if (result == NULL)
            {
                (void)printf("failure in printf_alloc\n");
                /*return as is*/
            }
            else
            {
                /*return as is*/
            }
        }
    }
    return result;
}
/*this function will use 1x printf (in the happy case) .*/
/*more than 1x printf / function call can mean intermingled LogErrors in a multithreaded env*/
/*the function will also attempt to produce some human readable strings for GetLastError*/
void consolelogger_log_with_GetLastError(const char* file, const char* func, int line, const char* format, ...)
{
    DWORD lastError;
    char* lastErrorAsString;
    int lastErrorAsString_should_be_freed;
    time_t t;
    int systemMessage_should_be_freed;
    char* systemMessage;
    int userMessage_should_be_freed;
    char* userMessage;

    va_list args;
    va_start(args, format);

    /*this is what this case will do:
    1. snip the last error
    2. create a string with what that last error means
    3. printf the system message (__FILE__, __LINE__ etc) + the last error + whatever the user wanted
    */
    /*1. snip the last error*/
    lastError = GetLastError();

    /*2. create a string with what that last error means*/
    lastErrorAsString = lastErrorToString(lastError);
    if (lastErrorAsString == NULL)
    {
        (void)printf("failure in lastErrorToString");
        lastErrorAsString = "";
        lastErrorAsString_should_be_freed = 0;
    }
    else
    {
        lastErrorAsString_should_be_freed = 1;
    }

    t = time(NULL);
    systemMessage = printf_alloc("Error: Time:%.24s File:%s Func:%s Line:%d %s", ctime(&t), file, func, line, lastErrorAsString);

    if (systemMessage == NULL)
    {
        systemMessage = "";
        (void)printf("Error: [FAILED] Time:%.24s File : %s Func : %s Line : %d %s", ctime(&t), file, func, line, lastErrorAsString);
        systemMessage_should_be_freed = 0;
    }
    else
    {
        systemMessage_should_be_freed = 1;
    }

    userMessage = vprintf_alloc(format, args);
    if (userMessage == NULL)
    {
        (void)printf("[FAILED] ");
        (void)vprintf(format, args);
        (void)printf("\n");
        userMessage_should_be_freed = 0;
    }
    else
    {
        /*3. printf the system message(__FILE__, __LINE__ etc) + the last error + whatever the user wanted*/
        (void)printf("%s %s\n", systemMessage, userMessage);
        userMessage_should_be_freed = 1;
    }

    if (userMessage_should_be_freed == 1)
    {
        free(userMessage);
    }

    if (systemMessage_should_be_freed == 1)
    {
        free(systemMessage);
    }

    if (lastErrorAsString_should_be_freed == 1)
    {
        free(lastErrorAsString);
    }
    va_end(args);
}
#endif

#if defined(__GNUC__)
__attribute__ ((format (printf, 6, 7)))
#endif
void consolelogger_log(LOG_CATEGORY log_category, const char* file, const char* func, int line, unsigned int options, const char* format, ...)
{
    time_t t;
    va_list args;
    va_start(args, format);

    t = time(NULL);

    switch (log_category)
    {
    case AZ_LOG_INFO:
        (void)printf("Info: ");
        break;
    case AZ_LOG_ERROR:
        (void)printf("Error: Time:%.24s File:%s Func:%s Line:%d ", ctime(&t), file, func, line);
        break;
    default:
        break;
    }

    (void)vprintf(format, args);
    va_end(args);

    (void)log_category;
    if (options & LOG_LINE)
    {
        (void)printf("\r\n");
    }
}

