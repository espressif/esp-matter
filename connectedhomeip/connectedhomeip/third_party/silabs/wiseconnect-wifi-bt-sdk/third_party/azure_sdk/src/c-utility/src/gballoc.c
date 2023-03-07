// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <stdint.h>
#include "azure_c_shared_utility/lock.h"
#include "azure_c_shared_utility/optimize_size.h"
#include "azure_c_shared_utility/xlogging.h"

#ifndef GB_USE_CUSTOM_HEAP

#ifndef SIZE_MAX
#define SIZE_MAX ((size_t)~(size_t)0)
#endif

typedef struct ALLOCATION_TAG
{
    size_t size;
    void* ptr;
    void* next;
} ALLOCATION;

typedef enum GBALLOC_STATE_TAG
{
    GBALLOC_STATE_INIT,
    GBALLOC_STATE_NOT_INIT
} GBALLOC_STATE;

static ALLOCATION* head = NULL;
static size_t totalSize = 0;
static size_t maxSize = 0;
static size_t g_allocations = 0;
static GBALLOC_STATE gballocState = GBALLOC_STATE_NOT_INIT;

static LOCK_HANDLE gballocThreadSafeLock = NULL;

int gballoc_init(void)
{
    int result;

    if (gballocState != GBALLOC_STATE_NOT_INIT)
    {
        /* Codes_SRS_GBALLOC_01_025: [Init after Init shall fail and return a non-zero value.] */
        result = MU_FAILURE;
    }
    /* Codes_SRS_GBALLOC_01_026: [gballoc_Init shall create a lock handle that will be used to make the other gballoc APIs thread-safe.] */
    else if ((gballocThreadSafeLock = Lock_Init()) == NULL)
    {
        /* Codes_SRS_GBALLOC_01_027: [If the Lock creation fails, gballoc_init shall return a non-zero value.]*/
        result = MU_FAILURE;
    }
    else
    {
        gballocState = GBALLOC_STATE_INIT;

        /* Codes_ SRS_GBALLOC_01_002: [Upon initialization the total memory used and maximum total memory used tracked by the module shall be set to 0.] */
        totalSize = 0;
        maxSize = 0;
        g_allocations = 0;

        /* Codes_SRS_GBALLOC_01_024: [gballoc_init shall initialize the gballoc module and return 0 upon success.] */
        result = 0;
    }

    return result;
}

void gballoc_deinit(void)
{
    if (gballocState == GBALLOC_STATE_INIT)
    {
        /* Codes_SRS_GBALLOC_01_028: [gballoc_deinit shall free all resources allocated by gballoc_init.] */
        (void)Lock_Deinit(gballocThreadSafeLock);
    }

    gballocState = GBALLOC_STATE_NOT_INIT;
}

void* gballoc_malloc(size_t size)
{
    void* result;

    if (gballocState != GBALLOC_STATE_INIT)
    {
        /* Codes_SRS_GBALLOC_01_039: [If gballoc was not initialized gballoc_malloc shall simply call malloc without any memory tracking being performed.] */
        result = malloc(size);
    }
    /* Codes_SRS_GBALLOC_01_030: [gballoc_malloc shall ensure thread safety by using the lock created by gballoc_Init.] */
    else if (LOCK_OK != Lock(gballocThreadSafeLock))
    {
        /* Codes_SRS_GBALLOC_01_048: [If acquiring the lock fails, gballoc_malloc shall return NULL.] */
        LogError("Failed to get the Lock.");
        result = NULL;
    }
    else
    {
        ALLOCATION* allocation = (ALLOCATION*)calloc(1, sizeof(ALLOCATION));
        if (allocation == NULL)
        {
            result = NULL;
        }
        else
        {
            /* Codes_SRS_GBALLOC_01_003: [gb_malloc shall call the C99 malloc function and return its result.] */
            result = malloc(size);
            if (result == NULL)
            {
                /* Codes_SRS_GBALLOC_01_012: [When the underlying malloc call fails, gballoc_malloc shall return NULL and size should not be counted towards total memory used.] */
                free(allocation);
            }
            else
            {
                /* Codes_SRS_GBALLOC_01_004: [If the underlying malloc call is successful, gb_malloc shall increment the total memory used with the amount indicated by size.] */
                allocation->ptr = result;
                allocation->size = size;
                allocation->next = head;
                head = allocation;

                g_allocations++;
                totalSize += size;
                /* Codes_SRS_GBALLOC_01_011: [The maximum total memory used shall be the maximum of the total memory used at any point.] */
                if (maxSize < totalSize)
                {
                    maxSize = totalSize;
                }
            }
        }

        (void)Unlock(gballocThreadSafeLock);
    }

    return result;
}

void* gballoc_calloc(size_t nmemb, size_t size)
{
    void* result;

    if (gballocState != GBALLOC_STATE_INIT)
    {
        /* Codes_SRS_GBALLOC_01_040: [If gballoc was not initialized gballoc_calloc shall simply call calloc without any memory tracking being performed.] */
        result = calloc(nmemb, size);
    }
    /* Codes_SRS_GBALLOC_01_031: [gballoc_calloc shall ensure thread safety by using the lock created by gballoc_Init]  */
    else if (LOCK_OK != Lock(gballocThreadSafeLock))
    {
        /* Codes_SRS_GBALLOC_01_046: [If acquiring the lock fails, gballoc_calloc shall return NULL.] */
        LogError("Failed to get the Lock.");
        result = NULL;
    }
    else
    {
        ALLOCATION* allocation = (ALLOCATION*)calloc(1, sizeof(ALLOCATION));
        if (allocation == NULL)
        {
            result = NULL;
        }
        else
        {
            /* Codes_SRS_GBALLOC_01_020: [gballoc_calloc shall call the C99 calloc function and return its result.] */
            result = calloc(nmemb, size);
            if (result == NULL)
            {
                /* Codes_SRS_GBALLOC_01_022: [When the underlying calloc call fails, gballoc_calloc shall return NULL and size should not be counted towards total memory used.] */
                free(allocation);
            }
            else
            {
                /* Codes_SRS_GBALLOC_01_021: [If the underlying calloc call is successful, gballoc_calloc shall increment the total memory used with nmemb*size.] */
                allocation->ptr = result;
                allocation->size = nmemb * size;
                allocation->next = head;
                head = allocation;
                g_allocations++;

                totalSize += allocation->size;
                /* Codes_SRS_GBALLOC_01_011: [The maximum total memory used shall be the maximum of the total memory used at any point.] */
                if (maxSize < totalSize)
                {
                    maxSize = totalSize;
                }
            }
        }

        (void)Unlock(gballocThreadSafeLock);
    }

    return result;
}

void* gballoc_realloc(void* ptr, size_t size)
{
    ALLOCATION* curr;
    void* result;
    ALLOCATION* allocation = NULL;

    if (gballocState != GBALLOC_STATE_INIT)
    {
        /* Codes_SRS_GBALLOC_01_041: [If gballoc was not initialized gballoc_realloc shall shall simply call realloc without any memory tracking being performed.] */
        result = realloc(ptr, size);
    }
    /* Codes_SRS_GBALLOC_01_032: [gballoc_realloc shall ensure thread safety by using the lock created by gballoc_Init.] */
    else if (LOCK_OK != Lock(gballocThreadSafeLock))
    {
        /* Codes_SRS_GBALLOC_01_047: [If acquiring the lock fails, gballoc_realloc shall return NULL.] */
        LogError("Failed to get the Lock.");
        result = NULL;
    }
    else
    {
        if (ptr == NULL)
        {
            /* Codes_SRS_GBALLOC_01_017: [When ptr is NULL, gballoc_realloc shall call the underlying realloc with ptr being NULL and the realloc result shall be tracked by gballoc.] */
            allocation = (ALLOCATION*)calloc(1, sizeof(ALLOCATION));
        }
        else
        {
            curr = head;
            while (curr != NULL)
            {
                if (curr->ptr == ptr)
                {
                    allocation = curr;
                    break;
                }
                else
                {
                    curr = (ALLOCATION*)curr->next;
                }
            }
        }

        if (allocation == NULL)
        {
            /* Codes_SRS_GBALLOC_01_015: [When allocating memory used for tracking by gballoc_realloc fails, gballoc_realloc shall return NULL and no change should be made to the counted total memory usage.] */
            /* Codes_SRS_GBALLOC_01_016: [When the ptr pointer cannot be found in the pointers tracked by gballoc, gballoc_realloc shall return NULL and the underlying realloc shall not be called.] */
            result = NULL;
        }
        else
        {
            result = realloc(ptr, size);
            if (result == NULL)
            {
                /* Codes_SRS_GBALLOC_01_014: [When the underlying realloc call fails, gballoc_realloc shall return NULL and no change should be made to the counted total memory usage.] */
                if (ptr == NULL)
                {
                    free(allocation);
                }
            }
            else
            {
                if (ptr != NULL)
                {
                    /* Codes_SRS_GBALLOC_01_006: [If the underlying realloc call is successful, gballoc_realloc shall look up the size associated with the pointer ptr and decrease the total memory used with that size.] */
                    allocation->ptr = result;
                    totalSize -= allocation->size;
                    allocation->size = size;
                }
                else
                {
                    /* add block */
                    allocation->ptr = result;
                    allocation->size = size;
                    allocation->next = head;
                    head = allocation;
                }

                /* Codes_SRS_GBALLOC_01_007: [If realloc is successful, gballoc_realloc shall also increment the total memory used value tracked by this module.] */
                totalSize += size;
                g_allocations++;

                /* Codes_SRS_GBALLOC_01_011: [The maximum total memory used shall be the maximum of the total memory used at any point.] */
                if (maxSize < totalSize)
                {
                    maxSize = totalSize;
                }
            }
        }

        (void)Unlock(gballocThreadSafeLock);
    }

    return result;
}

void gballoc_free(void* ptr)
{
    ALLOCATION* curr = head;
    ALLOCATION* prev = NULL;

    if (gballocState != GBALLOC_STATE_INIT)
    {
        /* Codes_SRS_GBALLOC_01_042: [If gballoc was not initialized gballoc_free shall shall simply call free.] */
        free(ptr);
    }
    /* Codes_SRS_GBALLOC_01_033: [gballoc_free shall ensure thread safety by using the lock created by gballoc_Init.] */
    else if (LOCK_OK != Lock(gballocThreadSafeLock))
    {
        /* Codes_SRS_GBALLOC_01_049: [If acquiring the lock fails, gballoc_free shall do nothing.] */
        LogError("Failed to get the Lock.");
    }
    else
    {
        /* Codes_SRS_GBALLOC_01_009: [gballoc_free shall also look up the size associated with the ptr pointer and decrease the total memory used with the associated size amount.] */
        while (curr != NULL)
        {
            if (curr->ptr == ptr)
            {
                /* Codes_SRS_GBALLOC_01_008: [gballoc_free shall call the C99 free function.] */
                free(ptr);
                totalSize -= curr->size;
                if (prev != NULL)
                {
                    prev->next = curr->next;
                }
                else
                {
                    head = (ALLOCATION*)curr->next;
                }

                free(curr);
                break;
            }

            prev = curr;
            curr = (ALLOCATION*)curr->next;
        }

        if ((curr == NULL) && (ptr != NULL))
        {
            /* Codes_SRS_GBALLOC_01_019: [When the ptr pointer cannot be found in the pointers tracked by gballoc, gballoc_free shall not free any memory.] */

            /* could not find the allocation */
            LogError("Could not free allocation for address %p (not found)", ptr);
        }
        (void)Unlock(gballocThreadSafeLock);
    }
}

size_t gballoc_getMaximumMemoryUsed(void)
{
    size_t result;

    /* Codes_SRS_GBALLOC_01_038: [If gballoc was not initialized gballoc_getMaximumMemoryUsed shall return MAX_INT_SIZE.] */
    if (gballocState != GBALLOC_STATE_INIT)
    {
        LogError("gballoc is not initialized.");
        result = SIZE_MAX;
    }
    /* Codes_SRS_GBALLOC_01_034: [gballoc_getMaximumMemoryUsed shall ensure thread safety by using the lock created by gballoc_Init.]  */
    else if (LOCK_OK != Lock(gballocThreadSafeLock))
    {
        /* Codes_SRS_GBALLOC_01_050: [If the lock cannot be acquired, gballoc_getMaximumMemoryUsed shall return SIZE_MAX.]  */
        LogError("Failed to get the Lock.");
        result = SIZE_MAX;
    }
    else
    {
        /* Codes_SRS_GBALLOC_01_010: [gballoc_getMaximumMemoryUsed shall return the maximum amount of total memory used recorded since the module initialization.] */
        result = maxSize;
        (void)Unlock(gballocThreadSafeLock);
    }

    return result;
}

size_t gballoc_getCurrentMemoryUsed(void)
{
    size_t result;

    /* Codes_SRS_GBALLOC_01_044: [If gballoc was not initialized gballoc_getCurrentMemoryUsed shall return SIZE_MAX.] */
    if (gballocState != GBALLOC_STATE_INIT)
    {
        LogError("gballoc is not initialized.");
        result = SIZE_MAX;
    }
    /* Codes_SRS_GBALLOC_01_036: [gballoc_getCurrentMemoryUsed shall ensure thread safety by using the lock created by gballoc_Init.]*/
    else if (LOCK_OK != Lock(gballocThreadSafeLock))
    {
        /* Codes_SRS_GBALLOC_01_051: [If the lock cannot be acquired, gballoc_getCurrentMemoryUsed shall return SIZE_MAX.] */
        LogError("Failed to get the Lock.");
        result = SIZE_MAX;
    }
    else
    {
        /*Codes_SRS_GBALLOC_02_001: [gballoc_getCurrentMemoryUsed shall return the currently used memory size.] */
        result = totalSize;
        (void)Unlock(gballocThreadSafeLock);
    }

    return result;
}

size_t gballoc_getAllocationCount(void)
{
    size_t result;

    /* Codes_SRS_GBALLOC_07_001: [ If gballoc was not initialized gballoc_getAllocationCount shall return 0. ] */
    if (gballocState != GBALLOC_STATE_INIT)
    {
        LogError("gballoc is not initialized.");
        result = 0;
    }
    /* Codes_SRS_GBALLOC_07_002: [ gballoc_getAllocationCount shall ensure thread safety by using the lock created by gballoc_Init ] */
    else if (LOCK_OK != Lock(gballocThreadSafeLock))
    {
        /* Codes_SRS_GBALLOC_07_003: [ If the lock cannot be acquired, gballoc_getAllocationCount shall return 0. ] */
        LogError("Failed to get the Lock.");
        result = 0;
    }
    else
    {
        /* Codes_SRS_GBALLOC_07_004: [ gballoc_getAllocationCount shall return the currently number of allocations. ] */
        result = g_allocations;
        (void)Unlock(gballocThreadSafeLock);
    }

    return result;
}

void gballoc_resetMetrics()
{
    /* Codes_SRS_GBALLOC_07_005: [ If gballoc was not initialized gballoc_reset Metrics shall do nothing.] */
    if (gballocState != GBALLOC_STATE_INIT)
    {
        LogError("gballoc is not initialized.");
    }
    /* Codes_SRS_GBALLOC_07_006: [ gballoc_resetMetrics shall ensure thread safety by using the lock created by gballoc_Init ]*/
    else if (LOCK_OK != Lock(gballocThreadSafeLock))
    {
        /* Codes_SRS_GBALLOC_07_007: [ If the lock cannot be acquired, gballoc_reset Metrics shall do nothing.] */
        LogError("Failed to get the Lock.");
    }
    else
    {
        /* Codes_SRS_GBALLOC_07_008: [ gballoc_resetMetrics shall reset the total allocation size, max allocation size and number of allocation to zero. ] */
        totalSize = 0;
        maxSize = 0;
        g_allocations = 0;
        (void)Unlock(gballocThreadSafeLock);
    }
}

#endif // GB_USE_CUSTOM_HEAP
