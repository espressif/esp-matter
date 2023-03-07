// Copyright (C) Microsoft Corporation. All rights reserved.

#include "windows.h"

#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/xlogging.h"

#include "azure_c_shared_utility/timer.h"

typedef struct TIMER_HANDLE_DATA_TAG
{
    LARGE_INTEGER freq;
    LARGE_INTEGER startTime;
}TIMER_HANDLE_DATA;

TIMER_HANDLE timer_create(void)
{
    TIMER_HANDLE result;
    result = malloc(sizeof(TIMER_HANDLE_DATA));
    if (result == NULL)
    {
        LogError("failure in malloc");
        /*return as is*/
    }
    else
    {
        (void)QueryPerformanceFrequency(&(result->freq)); /* from MSDN:  On systems that run Windows XP or later, the function will always succeed and will thus never return zero.*/
                                                          /*return as is*/
        (void)QueryPerformanceCounter(&(result->startTime)); /*from MSDN:  On systems that run Windows XP or later, the function will always succeed and will thus never return zero.*/
    }
    return result;
}

void timer_start(TIMER_HANDLE timer)
{
    if (timer == NULL)
    {
        LogError("invalid arg TIMER_HANDLE timer=%p", timer);
    }
    else
    {
        (void)QueryPerformanceCounter(&(timer->startTime)); /*from MSDN:  On systems that run Windows XP or later, the function will always succeed and will thus never return zero.*/
    }
}

double timer_get_elapsed(TIMER_HANDLE timer)
{
    double result;
    if (timer == NULL)
    {
        LogError("invalid arg TIMER_HANDLE timer=%p", timer);
        result = -1.0;
    }
    else
    {
        LARGE_INTEGER stopTime;
        (void)QueryPerformanceCounter(&stopTime);
        result = ((double)(stopTime.QuadPart - timer->startTime.QuadPart) / (double)timer->freq.QuadPart);
    }
    return result;
}

double timer_get_elapsed_ms(TIMER_HANDLE timer)
{
    double result = timer_get_elapsed(timer);
    return result < 0 ? result : result * 1000;
}

void timer_destroy(TIMER_HANDLE timer)
{
    if (timer == NULL)
    {
        LogError("invalid arg TIMER_HANDLE timer=%p", timer);
    }
    else
    {
        free(timer);
    }
}
