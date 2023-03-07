// Copyright (C) Microsoft Corporation. All rights reserved.

#include <inttypes.h>
#include <stdbool.h>

#include "windows.h"

#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/timer.h"
#include "azure_c_shared_utility/string_utils.h"

#include "azure_c_shared_utility/srw_lock.h"

/*
vocabulary:
blaBlaCount = values of counter for event blabla
blablaCounts = sum of individual blablaCountEnd-blablaCountBegin (always a sum of durations of Counts)
time = blablaCounts/freq
*/

#define TIME_BETWEEN_STATISTICS_LOG 600 /*in seconds, so every 10 minutes*/

typedef struct SRW_LOCK_HANDLE_DATA_TAG
{
    
    SRWLOCK lock;

    LARGE_INTEGER freq;

    volatile LONG64 nCalls_AcquireSRWLockExclusive; /*number of calls to AcquireSRWLockExclusive*/
    volatile LONG64 totalCounts_AcquireSRWLockExclusive; /*how many counts were spent taking the lock (that is, just before and after AcquireSRWLockExclusive call*/
    volatile LONG64 totalCounts_ReleaseSRWLockExclusive; /*how many counts were spent releasing the exclusive SRW that is, just before and after ReleaseSRWLockExclusive call*/
    volatile LONG64 lastCount_AcquireSRWLockExclusive; /*last time the lock was taken exclusively*/
    volatile LONG64 totalCountsBetween_AcquireSRWLockExclusive_and_ReleaseSRWLockExclusive; /*how much time the lock was taken in total in exclusive mode*/

    volatile LONG64 nCalls_AcquireSRWLockShared; /*number of calls to AcquireSRWLockShared*/
    volatile LONG64 totalCounts_AcquireSRWLockShared; /*how many counts were spent taking the lock (that is, just before and after AcquireSRWLockShared */
    volatile LONG64 totalCounts_ReleaseSRWLockShared; /*how many counts were spent releasing the shared SRW that is, just before and after ReleaseSRWLockShared call*/
    volatile LONG nSharedReaders; /*if lock_shared is granted, then there are 1 more readers... when readers gets to 0, the shared "locked" count can be updated*/
    volatile LONG64 firstCount_AcquireSRWLockShared; /*first time the lock was taken in shared mode*/
    volatile LONG64 totalCountsBetween_AcquireSRWLockShared_and_ReleaseSRWLockShared; /*how much time the lock was taken from the first granted shared access to the last released shared access*/

    LARGE_INTEGER handleCreateCount; /*when the handle was created*/

    TIMER_HANDLE timer;

    char* lockName;
    bool doStatistics;
    
}SRW_LOCK_HANDLE_DATA;


static void LogStatistics(SRW_LOCK_HANDLE handle, const char* reason)
{
    LARGE_INTEGER now;
    (void)QueryPerformanceCounter(&now);

    /*print statistics*/

LogInfo("srw_lock_statistics reason:%s SRW_LOCK_HANDLE handle %p lock_name=%s\n"
"freq=%" PRId64 ",\n"
"nCalls_AcquireSRWLockExclusive=%" PRId64 ",\n"
"totalCounts_AcquireSRWLockExclusive=%" PRId64 ",\n"
"totalCounts_ReleaseSRWLockExclusive=%" PRId64 ",\n"
"totalCountsBetween_AcquireSRWLockExclusive_and_ReleaseSRWLockExclusive=%" PRId64 ",\n"
"nCalls_AcquireSRWLockShared=%" PRId64 ",\n"
"totalCounts_AcquireSRWLockShared=%" PRId64 ",\n"
"totalCounts_ReleaseSRWLockShared=%" PRId64 ",\n"
"totalCountsBetween_AcquireSRWLockShared_and_ReleaseSRWLockShared=%" PRId64 ",\n"
"totalCountsBetween_Create_and_Now=%" PRId64 ",\n"
"totalTimeBetween_Create_and_Now[s]=%.2f, \n"
"totalTimeBetween_AcquireSRWLockExclusive_and_ReleaseSRWLockExclusive[s]=%.2f, \n"
"totalTimeBetween_AcquireSRWLockExclusive_and_ReleaseSRWLockExclusive[%%]=%.2f, \n"
"averageTimeBetween_AcquireSRWLockExclusive_and_ReleaseSRWLockExclusive[us]=%.2f, \n"
"totalTimeBetween_AcquireSRWLockShared_and_ReleaseSRWLockShared[s]=%.2f, \n"
"totalTimeBetween_AcquireSRWLockShared_and_ReleaseSRWLockShared[%%]=%.2f",

reason,
handle,
handle->lockName,
handle->freq.QuadPart,
InterlockedAdd64(&handle->nCalls_AcquireSRWLockExclusive, 0),
InterlockedAdd64(&handle->totalCounts_AcquireSRWLockExclusive, 0),
InterlockedAdd64(&handle->totalCounts_ReleaseSRWLockExclusive, 0),
InterlockedAdd64(&handle->totalCountsBetween_AcquireSRWLockExclusive_and_ReleaseSRWLockExclusive, 0),
InterlockedAdd64(&handle->nCalls_AcquireSRWLockShared, 0),
InterlockedAdd64(&handle->totalCounts_AcquireSRWLockShared, 0),
InterlockedAdd64(&handle->totalCounts_ReleaseSRWLockShared, 0),
InterlockedAdd64(&handle->totalCountsBetween_AcquireSRWLockShared_and_ReleaseSRWLockShared, 0),
/*"totalTimeBetween_Create_and_Now[s]*/
now.QuadPart - handle->handleCreateCount.QuadPart,
(double)(now.QuadPart -handle->handleCreateCount.QuadPart)/ (double)handle->freq.QuadPart,
(double)InterlockedAdd64(&handle->totalCountsBetween_AcquireSRWLockExclusive_and_ReleaseSRWLockExclusive, 0) / (double)handle->freq.QuadPart,
(double)InterlockedAdd64(&handle->totalCountsBetween_AcquireSRWLockExclusive_and_ReleaseSRWLockExclusive, 0) / (double)(now.QuadPart - handle->handleCreateCount.QuadPart) *100,
(InterlockedAdd64(&handle->nCalls_AcquireSRWLockExclusive, 0)>0?(double)InterlockedAdd64(&handle->totalCountsBetween_AcquireSRWLockExclusive_and_ReleaseSRWLockExclusive, 0)*1000000 / (double)(now.QuadPart - handle->handleCreateCount.QuadPart) / InterlockedAdd64(&handle->nCalls_AcquireSRWLockExclusive, 0):-1),
(double)InterlockedAdd64(&handle->totalCountsBetween_AcquireSRWLockShared_and_ReleaseSRWLockShared , 0) / (double)handle->freq.QuadPart,
(double)InterlockedAdd64(&handle->totalCountsBetween_AcquireSRWLockShared_and_ReleaseSRWLockShared, 0) / (double)(now.QuadPart - handle->handleCreateCount.QuadPart) * 100
);

}

SRW_LOCK_HANDLE srw_lock_create(bool do_statistics, const char* lock_name)
{
    SRW_LOCK_HANDLE result;
    /*Codes_SRS_SRW_LOCK_02_001: [ srw_lock_create shall allocate memory for SRW_LOCK_HANDLE. ]*/
    result = calloc(1, sizeof(SRW_LOCK_HANDLE_DATA));
    if (result == NULL)
    {
        /*return as is*/
        LogError("failure in malloc(sizeof(SRW_LOCK_HANDLE_DATA)=%zu)", sizeof(SRW_LOCK_HANDLE_DATA));
    }
    else
    {
        /*Codes_SRS_SRW_LOCK_02_023: [ If do_statistics is true then srw_lock_create shall copy lock_name. ]*/
        if (do_statistics &&
            ((result->lockName= sprintf_char("%s", MU_P_OR_NULL(lock_name))) == NULL)
        )
        {
            LogError("failure in sprintf_char(\"%%s\", lock_name=%s)", MU_P_OR_NULL(lock_name));
        }
        else
        {
            /*Codes_SRS_SRW_LOCK_02_024: [ If do_statistics is true then srw_lock_create shall create a new TIMER_HANDLE by calling timer_create. ]*/
            if(
                do_statistics &&
                ((result->timer = timer_create())==NULL)
                )
            {
                LogError("failure in timer_create()");
            }
            else
            {
                result->doStatistics = do_statistics;
                /*Codes_SRS_SRW_LOCK_02_015: [ srw_lock_create shall call InitializeSRWLock. ]*/

                InitializeSRWLock(&result->lock);

                (void)QueryPerformanceFrequency(&result->freq);

                (void)InterlockedExchange64(&result->nCalls_AcquireSRWLockExclusive, 0);
                (void)InterlockedExchange64(&result->nCalls_AcquireSRWLockShared, 0);

                (void)InterlockedExchange64(&result->totalCounts_AcquireSRWLockExclusive, 0);
                (void)InterlockedExchange64(&result->totalCounts_ReleaseSRWLockExclusive, 0);

                (void)InterlockedExchange64(&result->totalCounts_AcquireSRWLockShared, 0);
                (void)InterlockedExchange64(&result->totalCounts_ReleaseSRWLockShared, 0);

                (void)InterlockedExchange64(&result->totalCountsBetween_AcquireSRWLockExclusive_and_ReleaseSRWLockExclusive, 0);

                (void)InterlockedExchange(&result->nSharedReaders, 0);
                (void)InterlockedExchange64(&result->totalCountsBetween_AcquireSRWLockShared_and_ReleaseSRWLockShared, 0);


                (void)QueryPerformanceCounter(&result->handleCreateCount);

                if (do_statistics)
                {
                    LogInfo("srw_lock_create returns %p for lock_name=%s", result, MU_P_OR_NULL(lock_name));
                }
                /*Codes_SRS_SRW_LOCK_02_003: [ srw_lock_create shall succeed and return a non-NULL value. ]*/
                goto allOk;

                //timer_destroy(result->timer);
            }
            /*Codes_SRS_SRW_LOCK_02_004: [ If there are any failures then srw_lock_create shall fail and return NULL. ]*/
            do_statistics ? free(result->lockName) : (void)0;
        }
        free(result);
        result = NULL;
    }
allOk:;
    return result;
    
}

void srw_lock_acquire_exclusive(SRW_LOCK_HANDLE handle)
{
    /*Codes_SRS_SRW_LOCK_02_022: [ If handle is NULL then srw_lock_acquire_exclusive shall return. ]*/
    if (handle == NULL)
    {
        LogError("invalid argument SRW_LOCK_HANDLE handle=%p", handle);
    }
    else
    {
        if (!handle->doStatistics)
        {
            AcquireSRWLockExclusive(&handle->lock);
        }
        else
        {
            LARGE_INTEGER start, stop;
            (void)QueryPerformanceCounter(&start);
            /*Codes_SRS_SRW_LOCK_02_006: [ srw_lock_acquire_exclusive shall call AcquireSRWLockExclusive. ]*/
            AcquireSRWLockExclusive(&handle->lock);
            (void)QueryPerformanceCounter(&stop); /*measure acquire time*/

            (void)InterlockedIncrement64(&handle->nCalls_AcquireSRWLockExclusive);
            (void)InterlockedAdd64(&handle->totalCounts_AcquireSRWLockExclusive, stop.QuadPart - start.QuadPart);

            /*piggyback on the lock itself to print the statistics "until now" - note - does not include the current Lock call*/
            /*Codes_SRS_SRW_LOCK_02_025: [ If do_statistics is true and if the timer created has recorded more than TIME_BETWEEN_STATISTICS_LOG seconds then statistics will be logged and the timer shall be started again. ]*/
            if (timer_get_elapsed(handle->timer) >= TIME_BETWEEN_STATISTICS_LOG)
            {
                timer_start(handle->timer);
                LogStatistics(handle, "periodic logging almost every " MU_TOSTRING(TIME_BETWEEN_STATISTICS_LOG) " seconds");
            }

            (void)InterlockedExchange64(&handle->lastCount_AcquireSRWLockExclusive, stop.QuadPart);
        }
    }
}

void srw_lock_release_exclusive(SRW_LOCK_HANDLE handle)
{
    /*Codes_SRS_SRW_LOCK_02_009: [ If handle is NULL then srw_lock_release_exclusive shall return. ]*/
    if (handle == NULL)
    {
        LogError("invalid argument SRW_LOCK_HANDLE handle=%p", handle);
    }
    else
    {
        if (!handle->doStatistics)
        {
            ReleaseSRWLockExclusive(&handle->lock);
        }
        else
        {
            LARGE_INTEGER start, stop;

            LARGE_INTEGER lastAcquireCount_exclusive_copy;
            lastAcquireCount_exclusive_copy.QuadPart = InterlockedAdd64(&handle->lastCount_AcquireSRWLockExclusive, 0);

            (void)QueryPerformanceCounter(&start);
            /*Codes_SRS_SRW_LOCK_02_010: [ srw_lock_release_exclusive shall call ReleaseSRWLockExclusive. ]*/
            ReleaseSRWLockExclusive(&handle->lock);
            (void)QueryPerformanceCounter(&stop); /*measure release time*/

            (void)InterlockedAdd64(&handle->totalCounts_ReleaseSRWLockExclusive, (stop.QuadPart - start.QuadPart));
            (void)InterlockedAdd64(&handle->totalCountsBetween_AcquireSRWLockExclusive_and_ReleaseSRWLockExclusive, (stop.QuadPart - lastAcquireCount_exclusive_copy.QuadPart));
        }
    }
}

void srw_lock_acquire_shared(SRW_LOCK_HANDLE handle)
{
    /*Codes_SRS_SRW_LOCK_02_017: [ If handle is NULL then srw_lock_acquire_shared shall return. ]*/
    if (handle == NULL)
    {
        LogError("invalid argument SRW_LOCK_HANDLE handle=%p", handle);
    }
    else
    {
        if (!handle->doStatistics)
        {
            AcquireSRWLockShared(&handle->lock);
        }
        else
        {
            LARGE_INTEGER start, stop;

            (void)QueryPerformanceCounter(&start);
            /*Codes_SRS_SRW_LOCK_02_018: [ srw_lock_acquire_shared shall call AcquireSRWLockShared. ]*/
            AcquireSRWLockShared(&handle->lock);
            (void)QueryPerformanceCounter(&stop); /*measure acquire time*/

            /*there is one more shared reader*/
            /*is this the first reader?*/
            if (InterlockedIncrement(&handle->nSharedReaders) == 1)
            {
                (void)InterlockedExchange64(&handle->firstCount_AcquireSRWLockShared, stop.QuadPart);
            }

            (void)InterlockedIncrement64(&handle->nCalls_AcquireSRWLockShared);
            (void)InterlockedAdd64(&handle->totalCounts_AcquireSRWLockShared, stop.QuadPart - start.QuadPart);

            /*Codes_SRS_SRW_LOCK_02_026: [ If do_statistics is true and the timer created has recorded more than TIME_BETWEEN_STATISTICS_LOG seconds then statistics will be logged and the timer shall be started again. ]*/
            if (timer_get_elapsed(handle->timer) >= TIME_BETWEEN_STATISTICS_LOG)
            {
                timer_start(handle->timer);
                LogStatistics(handle, "periodic logging almost every " MU_TOSTRING(TIME_BETWEEN_STATISTICS_LOG) " seconds");
            }
        }
    }
}

void srw_lock_release_shared(SRW_LOCK_HANDLE handle)
{
    /*Codes_SRS_SRW_LOCK_02_020: [ If handle is NULL then srw_lock_release_shared shall return. ]*/
    if (handle == NULL)
    {
        LogError("invalid argument SRW_LOCK_HANDLE handle=%p", handle);
    }
    else
    {
        if (!handle->doStatistics)
        {
            ReleaseSRWLockShared(&handle->lock);
        }
        else
        {
            LARGE_INTEGER start, stop;

            LARGE_INTEGER firstCount_AcquireSRWLockShared_copy;
            firstCount_AcquireSRWLockShared_copy.QuadPart = InterlockedAdd64(&handle->firstCount_AcquireSRWLockShared, 0);

            (void)QueryPerformanceCounter(&start);
            /*Codes_SRS_SRW_LOCK_02_021: [ srw_lock_release_shared shall call ReleaseSRWLockShared. ]*/
            ReleaseSRWLockShared(&handle->lock);
            (void)QueryPerformanceCounter(&stop); /*measure release time*/

            if (InterlockedDecrement(&handle->nSharedReaders) == 0)
            {
                (void)InterlockedAdd64(&handle->totalCountsBetween_AcquireSRWLockShared_and_ReleaseSRWLockShared, (stop.QuadPart - firstCount_AcquireSRWLockShared_copy.QuadPart));
            }

            (void)InterlockedAdd64(&handle->totalCounts_ReleaseSRWLockShared, (stop.QuadPart - start.QuadPart));
        }
    }
}

void srw_lock_destroy(SRW_LOCK_HANDLE handle)
{
    /*Codes_SRS_SRW_LOCK_02_011: [ If handle is NULL then srw_lock_destroy shall return. ]*/
    if (handle == NULL)
    {
        LogError("invalid arguments SRW_LOCK_HANDLE handle=%p", handle);
    }
    else
    {
        if (handle->doStatistics)
        {
            LogStatistics(handle, "srw_lock_destroy was called");
            timer_destroy(handle->timer);
            free(handle->lockName);
        }

        /*Codes_SRS_SRW_LOCK_02_012: [ srw_lock_destroy shall free all used resources. ]*/
        /*there's no deinit for a SRW, since it is a static construct*/
        free(handle);
    }
}

