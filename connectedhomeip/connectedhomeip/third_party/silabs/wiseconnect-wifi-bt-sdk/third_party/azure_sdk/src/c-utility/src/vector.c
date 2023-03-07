// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/vector.h"
#include "azure_c_shared_utility/optimize_size.h"
#include "azure_c_shared_utility/xlogging.h"

#include "azure_c_shared_utility/vector_types_internal.h"

IMPLEMENT_MOCKABLE_FUNCTION(, VECTOR_HANDLE, VECTOR_create, size_t, elementSize)
{
    VECTOR_HANDLE result;

    /* Codes_SRS_VECTOR_10_002: [VECTOR_create shall fail and return NULL if elementsize is 0.] */
    if (elementSize == 0)
    {
        LogError("invalid elementSize(%zd).", elementSize);
        result = NULL;
    }
    else
    {
        result = (VECTOR*)malloc(sizeof(VECTOR));
        /* Codes_SRS_VECTOR_10_002 : [VECTOR_create shall fail and return NULL if malloc fails.] */
        if (result == NULL)
        {
            LogError("malloc failed.");
        }
        else
        {
            /* Codes_SRS_VECTOR_10_001: [VECTOR_create shall allocate a VECTOR_HANDLE that will contain an empty vector.The size of each element is given with the parameter elementSize.] */
            result->storage = NULL;
            result->count = 0;
            result->elementSize = elementSize;
        }
    }
    return result;
}

IMPLEMENT_MOCKABLE_FUNCTION(, void, VECTOR_destroy, VECTOR_HANDLE, handle)
{
    /* Codes_SRS_VECTOR_10_009: [VECTOR_destroy shall return if the given handle is NULL.] */
    if (handle == NULL)
    {
        LogError("invalid argument handle(NULL).");
    }
    else
    {
        /* Codes_SRS_VECTOR_10_008: [VECTOR_destroy shall free the given handle and its internal storage.] */
        free(handle->storage);
        free(handle);
    }
}

IMPLEMENT_MOCKABLE_FUNCTION(, VECTOR_HANDLE, VECTOR_move, VECTOR_HANDLE, handle)
{
    VECTOR_HANDLE result;
    if (handle == NULL)
    {
        /* Codes_SRS_VECTOR_10_005: [VECTOR_move shall fail and return NULL if the given handle is NULL.] */
        LogError("invalid argument - handle(NULL).");
        result = NULL;
    }
    else
    {
        result = (VECTOR*)malloc(sizeof(VECTOR));
        if (result == NULL)
        {
            /* Codes_SRS_VECTOR_10_006: [VECTOR_move shall fail and return NULL if malloc fails.] */
            LogError("malloc failed.");
        }
        else
        {
            /* Codes_SRS_VECTOR_10_004: [VECTOR_move shall allocate a VECTOR_HANDLE and move the data to it from the given handle.] */
            result->count = handle->count;
            result->elementSize = handle->elementSize;
            result->storage = handle->storage;

            handle->storage = NULL;
            handle->count = 0;
        }
    }
    return result;
}

/* insertion */

IMPLEMENT_MOCKABLE_FUNCTION(, int, VECTOR_push_back, VECTOR_HANDLE, handle, const void*, elements, size_t, numElements)
{
    int result;
    if (handle == NULL || elements == NULL || numElements == 0)
    {
       /* Codes_SRS_VECTOR_10_011: [VECTOR_push_back shall fail and return non-zero if handle is NULL.] */
       /* Codes_SRS_VECTOR_10_034: [VECTOR_push_back shall fail and return non-zero if elements is NULL.] */
       /* Codes_SRS_VECTOR_10_035: [VECTOR_push_back shall fail and return non-zero if numElements is 0.] */
        LogError("invalid argument - handle(%p), elements(%p), numElements(%zd).", handle, elements, numElements);
        result = MU_FAILURE;
    }
    else
    {
        size_t curSize = handle->elementSize * handle->count;
        size_t appendSize = handle->elementSize * numElements;

        void* temp = realloc(handle->storage, curSize + appendSize);
        if (temp == NULL)
        {
           /* Codes_SRS_VECTOR_10_012: [VECTOR_push_back shall fail and return non-zero if memory allocation fails.] */
            LogError("realloc failed.");
            result = MU_FAILURE;
        }
        else
        {
            /* Codes_SRS_VECTOR_10_013: [VECTOR_push_back shall append the given elements and return 0 indicating success.] */
            (void)memcpy((unsigned char*)temp + curSize, elements, appendSize);
            handle->storage = temp;
            handle->count += numElements;
            result = 0;
        }
    }
    return result;
}

/* removal */

IMPLEMENT_MOCKABLE_FUNCTION(, void, VECTOR_erase, VECTOR_HANDLE, handle, void*, elements, size_t, numElements)
{
    if (handle == NULL || elements == NULL || numElements == 0)
    {
        /* Codes_SRS_VECTOR_10_015: [VECTOR_erase shall return if handle is NULL.] */
        /* Codes_SRS_VECTOR_10_038: [VECTOR_erase shall return if elements is NULL.] */
        /* Codes_SRS_VECTOR_10_039: [VECTOR_erase shall return if numElements is 0.] */
        LogError("invalid argument - handle(%p), elements(%p), numElements(%zd).", handle, elements, numElements);
    }
    else
    {
        if (elements < handle->storage)
        {
            /* Codes_SRS_VECTOR_10_040: [VECTOR_erase shall return if elements is out of bound.] */
            LogError("invalid argument elements(%p) is not a member of this object.", elements);
        }
        else
        {
            ptrdiff_t diff = ((unsigned char*)elements) - ((unsigned char*)handle->storage);
            if ((diff % handle->elementSize) != 0)
            {
                /* Codes_SRS_VECTOR_10_041: [VECTOR_erase shall return if elements is misaligned.] */
                LogError("invalid argument - elements(%p) is misaligned", elements);
            }
            else
            {
                /* Compute the arguments needed for memmove. */
                unsigned char* src = (unsigned char*)elements + (handle->elementSize * numElements);
                unsigned char* srcEnd = (unsigned char*)handle->storage + (handle->elementSize * handle->count);
                if (src > srcEnd)
                {
                    /* Codes_SRS_VECTOR_10_040: [VECTOR_erase shall return if elements is out of bound.] */
                    LogError("invalid argument - numElements(%zd) is out of bound.", numElements);
                }
                else
                {
                    /* Codes_SRS_VECTOR_10_014: [VECTOR_erase shall remove the 'numElements' starting at 'elements' and reduce its internal storage.] */
                    handle->count -= numElements;
                    if (handle->count == 0)
                    {
                        free(handle->storage);
                        handle->storage = NULL;
                    }
                    else
                    {
                        void* tmp;
                        (void)memmove(elements, src, srcEnd - src);
                        tmp = realloc(handle->storage, (handle->elementSize * handle->count));
                        if (tmp == NULL)
                        {
                            LogInfo("realloc failed. Keeping original internal storage pointer.");
                        }
                        else
                        {
                            handle->storage = tmp;
                        }
                    }
                }
            }
        }
    }
}

IMPLEMENT_MOCKABLE_FUNCTION(, void, VECTOR_clear, VECTOR_HANDLE, handle)
{
    /* Codes_SRS_VECTOR_10_017: [VECTOR_clear shall if the object is NULL or empty.] */
    if (handle == NULL)
    {
        LogError("invalid argument handle(NULL).");
    }
    else
    {
        /* Codes_SRS_VECTOR_10_016: [VECTOR_clear shall remove all elements from the object and release internal storage.] */
        free(handle->storage);
        handle->storage = NULL;
        handle->count = 0;
    }
}

/* access */

IMPLEMENT_MOCKABLE_FUNCTION(, void*, VECTOR_element, VECTOR_HANDLE, handle, size_t, index)
{
    void* result;
    if (handle == NULL)
    {
        /* Codes_SRS_VECTOR_10_019: [VECTOR_element shall fail and return NULL if handle is NULL.] */
        LogError("invalid argument handle(NULL).");
        result = NULL;
    }
    else
    {
        if (index >= handle->count)
        {
            /* Codes_SRS_VECTOR_10_020: [VECTOR_element shall fail and return NULL if the given index is out of range.] */
            LogError("invalid argument - index(%zd); should be >= 0 and < %zd.", index, handle->count);
            result = NULL;
        }
        else
        {
            /* Codes_SRS_VECTOR_10_018: [VECTOR_element shall return the element at the given index.] */
            result = (unsigned char*)handle->storage + (handle->elementSize * index);
        }
    }
    return result;
}

IMPLEMENT_MOCKABLE_FUNCTION(, void*, VECTOR_front, VECTOR_HANDLE, handle)
{
    void* result;
    if (handle == NULL)
    {
        /* Codes_SRS_VECTOR_10_022: [VECTOR_front shall fail and return NULL if handle is NULL.] */
        LogError("invalid argument handle (NULL).");
        result = NULL;
    }
    else
    {
        if (handle->count == 0)
        {
            /* Codes_SRS_VECTOR_10_028: [VECTOR_front shall return NULL if the vector is empty.] */
            LogError("vector is empty.");
            result = NULL;
        }
        else
        {
            /* Codes_SRS_VECTOR_10_021: [VECTOR_front shall return a pointer to the element at index 0.] */
            result = handle->storage;
        }
    }
    return result;
}

IMPLEMENT_MOCKABLE_FUNCTION(, void*, VECTOR_back, VECTOR_HANDLE, handle)
{
    void* result;
    if (handle == NULL)
    {
        /* Codes_SRS_VECTOR_10_024: [VECTOR_back shall fail and return NULL if handle is NULL.] */
        LogError("invalid argument handle (NULL).");
        result = NULL;
    }
    else
    {
        if (handle->count == 0)
        {
            /* Codes_SRS_VECTOR_10_029: [VECTOR_back shall return NULL if the vector is empty.] */
            LogError("vector is empty.");
            result = NULL;
        }
        else
        {
            /* Codes_SRS_VECTOR_10_023: [VECTOR_front shall return the last element of the vector.] */
            result = (unsigned char*)handle->storage + (handle->elementSize * (handle->count - 1));
        }
    }
    return result;
}

IMPLEMENT_MOCKABLE_FUNCTION(, void*, VECTOR_find_if, VECTOR_HANDLE, handle, PREDICATE_FUNCTION, pred, const void*, value)
{
    void* result;
    if (handle == NULL || pred == NULL)
    {
        /* Codes_SRS_VECTOR_10_030: [VECTOR_find_if shall fail and return NULL if handle is NULL.] */
        /* Codes_SRS_VECTOR_10_036: [VECTOR_find_if shall fail and return NULL if pred is NULL.] */
        LogError("invalid argument - handle(%p), pred(%p)", handle, pred);
        result = NULL;
    }
    else
    {
        size_t i;
        for (i = 0; i < handle->count; ++i)
        {
            if (true == pred((unsigned char*)handle->storage + (handle->elementSize * i), value))
            {
                /* Codes_SRS_VECTOR_10_031: [VECTOR_find_if shall return the first element in the vector that matches pred.] */
                break;
            }
        }

        if (i == handle->count)
        {
            /* Codes_SRS_VECTOR_10_032: [VECTOR_find_if shall return NULL if no matching element is found.] */
            result = NULL;
        }
        else
        {
            /* Codes_SRS_VECTOR_10_031: [VECTOR_find_if shall return the first element in the vector that matches pred.]*/
            result = (unsigned char*)handle->storage + (handle->elementSize * i);
        }
    }
    return result;
}

/* capacity */

IMPLEMENT_MOCKABLE_FUNCTION(, size_t, VECTOR_size, VECTOR_HANDLE, handle)
{
    size_t result;
    if (handle == NULL)
    {
        /* Codes_SRS_VECTOR_10_026: [**VECTOR_size shall return 0 if the given handle is NULL.] */
        LogError("invalid argument handle(NULL).");
        result = 0;
    }
    else
    {
        /* Codes_SRS_VECTOR_10_025: [VECTOR_size shall return the number of elements stored with the given handle.] */
        result = handle->count;
    }
    return result;
}
