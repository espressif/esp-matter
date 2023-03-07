// Copyright (C) Microsoft Corporation. All rights reserved.

#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>

#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/optimize_size.h"

#include "azure_c_shared_utility/constbuffer.h"
#include "azure_c_shared_utility/constbuffer_array.h"
#include "azure_c_shared_utility/refcount.h"

typedef struct CONSTBUFFER_ARRAY_HANDLE_DATA_TAG
{
    uint32_t nBuffers;
    bool created_with_moved_memory;
    CONSTBUFFER_HANDLE* buffers;
#ifdef _MSC_VER
    /*warning C4200: nonstandard extension used: zero-sized array in struct/union : looks very standard in C99 and it is called flexible array. Documentation-wise is a flexible array, but called "unsized" in Microsoft's docs*/ /*https://msdn.microsoft.com/en-us/library/b6fae073.aspx*/
#pragma warning(disable:4200)
#endif
    CONSTBUFFER_HANDLE buffers_memory[];
} CONSTBUFFER_ARRAY_HANDLE_DATA;

DEFINE_REFCOUNT_TYPE(CONSTBUFFER_ARRAY_HANDLE_DATA);

IMPLEMENT_MOCKABLE_FUNCTION(, CONSTBUFFER_ARRAY_HANDLE, constbuffer_array_create, const CONSTBUFFER_HANDLE*, buffers, uint32_t, buffer_count)
{
    CONSTBUFFER_ARRAY_HANDLE result;

    if (
        /* Codes_SRS_CONSTBUFFER_ARRAY_01_012: [ If buffers is NULL and buffer_count is not 0, constbuffer_array_create shall fail and return NULL. ]*/
        (buffers == NULL) && (buffer_count != 0)
        )
    {
        LogError("Invalid arguments: const CONSTBUFFER_HANDLE* buffers=%p, uint32_t buffer_count=%" PRIu32,
            buffers, buffer_count);
    }
    else
    {
        /* Codes_SRS_CONSTBUFFER_ARRAY_01_009: [ constbuffer_array_create shall allocate memory for a new CONSTBUFFER_ARRAY_HANDLE that can hold buffer_count buffers. ]*/
        result = REFCOUNT_TYPE_CREATE_WITH_EXTRA_SIZE(CONSTBUFFER_ARRAY_HANDLE_DATA, buffer_count * sizeof(CONSTBUFFER_HANDLE));
        if (result == NULL)
        {
            /* Codes_SRS_CONSTBUFFER_ARRAY_01_014: [ If any error occurs, constbuffer_array_create shall fail and return NULL. ]*/
            LogError("failure in allocating const buffer array");
        }
        else
        {
            uint32_t i;

            result->buffers = result->buffers_memory;
            result->nBuffers = buffer_count;
            result->created_with_moved_memory = false;

            for (i = 0; i < buffer_count; i++)
            {
                /* Codes_SRS_CONSTBUFFER_ARRAY_01_010: [ constbuffer_array_create shall clone the buffers in buffers and store them. ]*/
                CONSTBUFFER_IncRef(buffers[i]);
                result->buffers[i] = buffers[i];
            }

            /* Codes_SRS_CONSTBUFFER_ARRAY_01_011: [ On success constbuffer_array_create shall return a non-NULL handle. ]*/
            goto all_ok;
        }
    }

    result = NULL;

all_ok:
    return result;
}

IMPLEMENT_MOCKABLE_FUNCTION(, CONSTBUFFER_ARRAY_HANDLE, constbuffer_array_create_empty)
{
    CONSTBUFFER_ARRAY_HANDLE result;

    /*Codes_SRS_CONSTBUFFER_ARRAY_02_004: [ constbuffer_array_create_empty shall allocate memory for a new CONSTBUFFER_ARRAY_HANDLE. ]*/
    result = REFCOUNT_TYPE_CREATE(CONSTBUFFER_ARRAY_HANDLE_DATA); /*explicit 0*/
    if (result == NULL)
    {
        /*Codes_SRS_CONSTBUFFER_ARRAY_02_001: [ If are any failure is encountered, constbuffer_array_create_empty shall fail and return NULL. ]*/
        LogError("failure allocating const buffer array");
        /*return as is*/
    }
    else
    {
        /*Codes_SRS_CONSTBUFFER_ARRAY_02_041: [ constbuffer_array_create_empty shall succeed and return a non-NULL value. ]*/
        result->created_with_moved_memory = false;
        result->nBuffers = 0;
        result->buffers = result->buffers_memory;
    }
    return result;
}

IMPLEMENT_MOCKABLE_FUNCTION(, CONSTBUFFER_ARRAY_HANDLE, constbuffer_array_create_with_move_buffers, CONSTBUFFER_HANDLE*, buffers, uint32_t, buffer_count)
{
    CONSTBUFFER_ARRAY_HANDLE result;

    /* Codes_SRS_CONSTBUFFER_ARRAY_01_028: [ If buffers is NULL and buffer_count is not 0, constbuffer_array_create_with_move_buffers shall fail and return NULL. ]*/
    if (buffers == NULL)
    {
        LogError("Invalid arguments: CONSTBUFFER_HANDLE* buffers=%p, uint32_t buffer_count=%" PRIu32,
            buffers, buffer_count);
        result = NULL;
    }
    else
    {
        result = REFCOUNT_TYPE_CREATE(CONSTBUFFER_ARRAY_HANDLE_DATA); /*explicit 0*/
        if (result == NULL)
        {
            /* Codes_SRS_CONSTBUFFER_ARRAY_01_030: [ If any error occurs, constbuffer_array_create_with_move_buffers shall fail and return NULL. ]*/
            LogError("failure allocating const buffer array");
            /*return as is*/
        }
        else
        {
            result->created_with_moved_memory = true;
            result->buffers = buffers;
            result->nBuffers = buffer_count;
        }
    }

    return result;
}

IMPLEMENT_MOCKABLE_FUNCTION(, CONSTBUFFER_ARRAY_HANDLE, constbuffer_array_create_from_array_array, const CONSTBUFFER_ARRAY_HANDLE*, buffer_arrays, uint32_t, buffer_array_count)
{
    CONSTBUFFER_ARRAY_HANDLE result;

    if (
        /*Codes_SRS_CONSTBUFFER_ARRAY_42_009: [ If buffer_arrays is NULL and buffer_array_count is not 0 then constbuffer_array_create_from_array_array shall fail and return NULL. ]*/
        (buffer_arrays == NULL && buffer_array_count != 0)
        )
    {
        LogError("invalid arguments: const CONSTBUFFER_ARRAY_HANDLE* buffer_arrays=%p, uint32_t buffer_array_count=%" PRIu32, buffer_arrays, buffer_array_count);
    }
    else
    {
        /*Codes_SRS_CONSTBUFFER_ARRAY_42_001: [ If buffer_arrays is NULL or buffer_array_count is 0 then constbuffer_array_create_from_array_array shall create a new, empty CONSTBUFFER_ARRAY_HANDLE. ]*/
        if (buffer_arrays == NULL || buffer_array_count == 0)
        {
            result = constbuffer_array_create_empty();

            if (result == NULL)
            {
                LogError("constbuffer_array_create_empty failed");
            }
            else
            {
                goto allOk;
            }
        }
        else
        {
            uint32_t total_buffer_count = 0;
            uint32_t i;
            for (i = 0; i < buffer_array_count; ++i)
            {
                if (buffer_arrays[i] == NULL)
                {
                    /*Codes_SRS_CONSTBUFFER_ARRAY_42_002: [ If any const buffer array in buffer_arrays is NULL then constbuffer_array_create_from_array_array shall fail and return NULL. ]*/
                    LogError("Invalid arguments: NULL buffer array %" PRIu32, i);
                    break;
                }
                else
                {
                    // Overflow check
                    total_buffer_count += buffer_arrays[i]->nBuffers;
                    if (total_buffer_count < buffer_arrays[i]->nBuffers)
                    {
                        LogError("Array size overflow while checking index %" PRIu32, i);
                        break;
                    }
                }
            }

            if (i < buffer_array_count)
            {
                // Failed in loop, fall through to cleanup
            }
            else
            {
                /*Codes_SRS_CONSTBUFFER_ARRAY_42_003: [ constbuffer_array_create_from_array_array shall allocate memory to hold all of the CONSTBUFFER_HANDLES from buffer_arrays. ]*/
                result = REFCOUNT_TYPE_CREATE_WITH_EXTRA_SIZE(CONSTBUFFER_ARRAY_HANDLE_DATA, (total_buffer_count) * sizeof(CONSTBUFFER_HANDLE));
                if (result == NULL)
                {
                    /*Codes_SRS_CONSTBUFFER_ARRAY_42_008: [ If there are any failures then constbuffer_array_create_from_array_array shall fail and return NULL. ]*/
                    LogError("failure in malloc");
                }
                else
                {
                    uint32_t dest_idx;
                    uint32_t array_idx;
                    uint32_t source_idx;

                    result->nBuffers = total_buffer_count;
                    result->created_with_moved_memory = false;
                    result->buffers = result->buffers_memory;

                    for (dest_idx = 0, array_idx = 0; array_idx < buffer_array_count; ++array_idx)
                    {
                        for (source_idx = 0; source_idx < buffer_arrays[array_idx]->nBuffers; ++source_idx, ++dest_idx)
                        {
                            /*Codes_SRS_CONSTBUFFER_ARRAY_42_004: [ constbuffer_array_create_from_array_array shall copy all of the CONSTBUFFER_HANDLES from each const buffer array in buffer_arrays to the newly constructed array by calling CONSTBUFFER_IncRef. ]*/
                            CONSTBUFFER_IncRef(buffer_arrays[array_idx]->buffers[source_idx]);
                            result->buffers[dest_idx] = buffer_arrays[array_idx]->buffers[source_idx];
                        }
                    }

                    /*Codes_SRS_CONSTBUFFER_ARRAY_42_007: [ constbuffer_array_create_from_array_array shall succeed and return a non-NULL value. ]*/
                    goto allOk;
                }
            }
        }
    }
    result = NULL;
allOk:;
    return result;
}

IMPLEMENT_MOCKABLE_FUNCTION(, CONSTBUFFER_ARRAY_HANDLE, constbuffer_array_add_front, CONSTBUFFER_ARRAY_HANDLE, constbuffer_array_handle, CONSTBUFFER_HANDLE, constbuffer_handle)
{
    CONSTBUFFER_ARRAY_HANDLE result;
    if (
        /*Codes_SRS_CONSTBUFFER_ARRAY_02_006: [ If constbuffer_array_handle is NULL then constbuffer_array_add_front shall fail and return NULL ]*/
        (constbuffer_array_handle == NULL) ||
        /*Codes_SRS_CONSTBUFFER_ARRAY_02_007: [ If constbuffer_handle is NULL then constbuffer_array_add_front shall fail and return NULL ]*/
        (constbuffer_handle == NULL)
        )
    {
        LogError("invalid arguments CONSTBUFFER_ARRAY_HANDLE constbuffer_array_handle=%p, CONSTBUFFER_HANDLE constbuffer_handle=%p", constbuffer_array_handle, constbuffer_handle);
    }
    else
    {
        /*Codes_SRS_CONSTBUFFER_ARRAY_02_042: [ constbuffer_array_add_front shall allocate enough memory to hold all of constbuffer_array_handle existing CONSTBUFFER_HANDLE and constbuffer_handle. ]*/
        result = REFCOUNT_TYPE_CREATE_WITH_EXTRA_SIZE(CONSTBUFFER_ARRAY_HANDLE_DATA, (constbuffer_array_handle->nBuffers + 1) * sizeof(CONSTBUFFER_HANDLE));
        if (result == NULL)
        {
            /*Codes_SRS_CONSTBUFFER_ARRAY_02_011: [ If there any failures constbuffer_array_add_front shall fail and return NULL. ]*/
            LogError("failure in malloc");
            /*return as is*/
        }
        else
        {
            uint32_t i;

            /*Codes_SRS_CONSTBUFFER_ARRAY_02_043: [ constbuffer_array_add_front shall copy constbuffer_handle and all of constbuffer_array_handle existing CONSTBUFFER_HANDLE. ]*/
            /*Codes_SRS_CONSTBUFFER_ARRAY_02_044: [ constbuffer_array_add_front shall inc_ref all the CONSTBUFFER_HANDLE it had copied. ]*/
            result->nBuffers = constbuffer_array_handle->nBuffers + 1;
            result->created_with_moved_memory = false;
            result->buffers = result->buffers_memory;
            CONSTBUFFER_IncRef(constbuffer_handle);
            result->buffers_memory[0] = constbuffer_handle;
            for (i = 1; i < result->nBuffers; i++)
            {
                CONSTBUFFER_IncRef(constbuffer_array_handle->buffers[i - 1]);
                result->buffers[i] = constbuffer_array_handle->buffers[i - 1];
            }

            /*Codes_SRS_CONSTBUFFER_ARRAY_02_010: [ constbuffer_array_add_front shall succeed and return a non-NULL value. ]*/
            goto allOk;
        }
    }
    /*Codes_SRS_CONSTBUFFER_ARRAY_02_011: [ If there any failures constbuffer_array_add_front shall fail and return NULL. ]*/
    result = NULL;
allOk:;
    return result;
}

IMPLEMENT_MOCKABLE_FUNCTION(, CONSTBUFFER_ARRAY_HANDLE, constbuffer_array_remove_front, CONSTBUFFER_ARRAY_HANDLE, constbuffer_array_handle, CONSTBUFFER_HANDLE*, constbuffer_handle)
{
    CONSTBUFFER_ARRAY_HANDLE result;
    if (
        /*Codes_SRS_CONSTBUFFER_ARRAY_02_012: [ If constbuffer_array_handle is NULL then constbuffer_array_remove_front shall fail and return NULL. ]*/
        (constbuffer_array_handle == NULL) ||
        /*Codes_SRS_CONSTBUFFER_ARRAY_02_045: [ If constbuffer_handle is NULL then constbuffer_array_remove_front shall fail and return NULL. ]*/
        (constbuffer_handle == NULL)
        )
    {
        /*Codes_SRS_CONSTBUFFER_ARRAY_02_036: [ If there are any failures then constbuffer_array_remove_front shall fail and return NULL. ]*/
        LogError("invalid arguments CONSTBUFFER_ARRAY_HANDLE constbuffer_array_handle=%p, CONSTBUFFER_HANDLE* constbuffer_handle=%p", constbuffer_array_handle, constbuffer_handle);
    }
    else
    {
        /*Codes_SRS_CONSTBUFFER_ARRAY_02_002: [ constbuffer_array_remove_front shall fail when called on a newly constructed CONSTBUFFER_ARRAY_HANDLE. ]*/
        /*Codes_SRS_CONSTBUFFER_ARRAY_02_013: [ If there is no front CONSTBUFFER_HANDLE then constbuffer_array_remove_front shall fail and return NULL. ]*/
        if (constbuffer_array_handle->nBuffers == 0)
        {
            /*Codes_SRS_CONSTBUFFER_ARRAY_02_036: [ If there are any failures then constbuffer_array_remove_front shall fail and return NULL. ]*/
            LogError("cannot remove from that which does not have");
        }
        else
        {
            /*Codes_SRS_CONSTBUFFER_ARRAY_02_046: [ constbuffer_array_remove_front shall allocate memory to hold all of constbuffer_array_handle CONSTBUFFER_HANDLEs except the front one. ]*/
            result = REFCOUNT_TYPE_CREATE_WITH_EXTRA_SIZE(CONSTBUFFER_ARRAY_HANDLE_DATA, (constbuffer_array_handle->nBuffers - 1) * sizeof(CONSTBUFFER_HANDLE));
            if (result == NULL)
            {
                /*Codes_SRS_CONSTBUFFER_ARRAY_02_036: [ If there are any failures then constbuffer_array_remove_front shall fail and return NULL. ]*/
                LogError("failure in malloc");
                /*return as is*/
            }
            else
            {
                uint32_t i;

                /* Codes_SRS_CONSTBUFFER_ARRAY_01_001: [ constbuffer_array_remove_front shall inc_ref the removed buffer. ]*/
                CONSTBUFFER_IncRef(constbuffer_array_handle->buffers[0]);
                result->nBuffers = constbuffer_array_handle->nBuffers - 1;
                result->created_with_moved_memory = false;
                result->buffers = result->buffers_memory;

                /*Codes_SRS_CONSTBUFFER_ARRAY_02_047: [ constbuffer_array_remove_front shall copy all of constbuffer_array_handle CONSTBUFFER_HANDLEs except the front one. ]*/
                /*Codes_SRS_CONSTBUFFER_ARRAY_02_048: [ constbuffer_array_remove_front shall inc_ref all the copied CONSTBUFFER_HANDLEs. ]*/
                for (i = 1; i < constbuffer_array_handle->nBuffers; i++)
                {
                    CONSTBUFFER_IncRef(constbuffer_array_handle->buffers[i]);
                    result->buffers[i - 1] = constbuffer_array_handle->buffers[i];
                }

                /*Codes_SRS_CONSTBUFFER_ARRAY_02_049: [ constbuffer_array_remove_front shall succeed, write in constbuffer_handle the front handle and return a non-NULL value. ]*/
                *constbuffer_handle = constbuffer_array_handle->buffers[0];
                goto allOk;
            }
        }
    }
    /*Codes_SRS_CONSTBUFFER_ARRAY_02_036: [ If there are any failures then constbuffer_array_remove_front shall fail and return NULL. ]*/
    result = NULL;
allOk:;
    return result;
}

IMPLEMENT_MOCKABLE_FUNCTION(, int, constbuffer_array_get_buffer_count, CONSTBUFFER_ARRAY_HANDLE, constbuffer_array_handle, uint32_t*, buffer_count)
{
    int result;

    if (
        /* Codes_SRS_CONSTBUFFER_ARRAY_01_003: [ If constbuffer_array_handle is NULL, constbuffer_array_get_buffer_count shall fail and return a non-zero value. ]*/
        (constbuffer_array_handle == NULL) ||
        /* Codes_SRS_CONSTBUFFER_ARRAY_01_004: [ If buffer_count is NULL, constbuffer_array_get_buffer_count shall fail and return a non-zero value. ]*/
        (buffer_count == NULL)
        )
    {
        LogError("Invalid arguments: CONSTBUFFER_ARRAY_HANDLE constbuffer_array_handle=%p, uint32_t* buffer_count=%p",
            constbuffer_array_handle, buffer_count);
        result = MU_FAILURE;
    }
    else
    {
        /* Codes_SRS_CONSTBUFFER_ARRAY_01_002: [ On success, constbuffer_array_get_buffer_count shall return 0 and write the buffer count in buffer_count. ]*/
        *buffer_count = constbuffer_array_handle->nBuffers;

        result = 0;
    }

    return result;
}

IMPLEMENT_MOCKABLE_FUNCTION(, CONSTBUFFER_HANDLE, constbuffer_array_get_buffer, CONSTBUFFER_ARRAY_HANDLE, constbuffer_array_handle, uint32_t, buffer_index)
{
    CONSTBUFFER_HANDLE result;

    if (
        /* Codes_SRS_CONSTBUFFER_ARRAY_01_007: [ If constbuffer_array_handle is NULL, constbuffer_array_get_buffer shall fail and return NULL. ]*/
        (constbuffer_array_handle == NULL) ||
        /* Codes_SRS_CONSTBUFFER_ARRAY_01_008: [ If buffer_index is greater or equal to the number of buffers in the array, constbuffer_array_get_buffer shall fail and return NULL. ]*/
        (buffer_index >= constbuffer_array_handle->nBuffers)
        )
    {
        LogError("Invalid arguments: CONSTBUFFER_ARRAY_HANDLE constbuffer_array_handle=%p, uint32_t buffer_index=%" PRIu32,
            constbuffer_array_handle, buffer_index);
    }
    else
    {
        /* Codes_SRS_CONSTBUFFER_ARRAY_01_006: [ The returned handle shall have its reference count incremented. ]*/
        CONSTBUFFER_IncRef(constbuffer_array_handle->buffers[buffer_index]);
        result = constbuffer_array_handle->buffers[buffer_index];

        /* Codes_SRS_CONSTBUFFER_ARRAY_01_005: [ On success, constbuffer_array_get_buffer shall return a non-NULL handle to the buffer_index-th const buffer in the array. ]*/
        goto all_ok;
    }

    result = NULL;

all_ok:
    return result;
}

IMPLEMENT_MOCKABLE_FUNCTION(, const CONSTBUFFER*, constbuffer_array_get_buffer_content, CONSTBUFFER_ARRAY_HANDLE, constbuffer_array_handle, uint32_t, buffer_index)
{
    const CONSTBUFFER* result;
    if (
        /* Codes_SRS_CONSTBUFFER_ARRAY_01_023: [ If constbuffer_array_handle is NULL, constbuffer_array_get_buffer_content shall fail and return NULL. ]*/
        (constbuffer_array_handle == NULL) ||
        /* Codes_SRS_CONSTBUFFER_ARRAY_01_024: [ If buffer_index is greater or equal to the number of buffers in the array, constbuffer_array_get_buffer_content shall fail and return NULL. ]*/
        (buffer_index >= constbuffer_array_handle->nBuffers)
        )
    {
        LogError("Invalid arguments: CONSTBUFFER_ARRAY_HANDLE constbuffer_array_handle=%p, uint32_t buffer_index=%" PRIu32,
            constbuffer_array_handle, buffer_index);
        result = NULL;
    }
    else
    {
        /* Codes_SRS_CONSTBUFFER_ARRAY_01_025: [ Otherwise constbuffer_array_get_buffer_content shall call CONSTBUFFER_GetContent for the buffer_index-th buffer and return its result. ]*/
        result = CONSTBUFFER_GetContent(constbuffer_array_handle->buffers[buffer_index]);
    }

    return result;
}

IMPLEMENT_MOCKABLE_FUNCTION(, void, constbuffer_array_inc_ref, CONSTBUFFER_ARRAY_HANDLE, constbuffer_array_handle)
{
    if (constbuffer_array_handle == NULL)
    {
        /* Codes_SRS_CONSTBUFFER_ARRAY_01_017: [ If constbuffer_array_handle is NULL then constbuffer_array_inc_ref shall return. ]*/
        LogError("invalid argument CONSTBUFFER_ARRAY_HANDLE constbuffer_array_handle=%p", constbuffer_array_handle);
    }
    else
    {
        /* Codes_SRS_CONSTBUFFER_ARRAY_01_018: [ Otherwise constbuffer_array_inc_ref shall increment the reference count for constbuffer_array_handle. ]*/
        INC_REF(CONSTBUFFER_ARRAY_HANDLE_DATA, constbuffer_array_handle);
    }
}

IMPLEMENT_MOCKABLE_FUNCTION(, void, constbuffer_array_dec_ref, CONSTBUFFER_ARRAY_HANDLE, constbuffer_array_handle)
{
    if (constbuffer_array_handle == NULL)
    {
        /*Codes_SRS_CONSTBUFFER_ARRAY_02_039: [ If constbuffer_array_handle is NULL then constbuffer_array_dec_ref shall return. ]*/
        LogError("invalid argument CONSTBUFFER_ARRAY_HANDLE constbuffer_array_handle=%p", constbuffer_array_handle);
    }
    else
    {
        /* Codes_SRS_CONSTBUFFER_ARRAY_01_016: [ Otherwise constbuffer_array_dec_ref shall decrement the reference count for constbuffer_array_handle. ]*/
        if (DEC_REF(CONSTBUFFER_ARRAY_HANDLE_DATA, constbuffer_array_handle) == DEC_RETURN_ZERO)
        {
            uint32_t i;

            /*Codes_SRS_CONSTBUFFER_ARRAY_02_038: [ If the reference count reaches 0, constbuffer_array_dec_ref shall free all used resources. ]*/
            for (i = 0; i < constbuffer_array_handle->nBuffers; i++)
            {
                CONSTBUFFER_DecRef(constbuffer_array_handle->buffers[i]);
            }

            if (constbuffer_array_handle->created_with_moved_memory)
            {
                free(constbuffer_array_handle->buffers);
            }

            REFCOUNT_TYPE_DESTROY(CONSTBUFFER_ARRAY_HANDLE_DATA, constbuffer_array_handle);
        }
    }
}

IMPLEMENT_MOCKABLE_FUNCTION(, int, constbuffer_array_get_all_buffers_size, CONSTBUFFER_ARRAY_HANDLE, constbuffer_array_handle, uint32_t*, all_buffers_size)
{
    int result;

    if (
        /* Codes_SRS_CONSTBUFFER_ARRAY_01_019: [ If constbuffer_array_handle is NULL, constbuffer_array_get_all_buffers_size shall fail and return a non-zero value. ]*/
        (constbuffer_array_handle == NULL) ||
        /* Codes_SRS_CONSTBUFFER_ARRAY_01_020: [ If all_buffers_size is NULL, constbuffer_array_get_all_buffers_size shall fail and return a non-zero value. ]*/
        (all_buffers_size == NULL)
        )
    {
        LogError("CONSTBUFFER_ARRAY_HANDLE constbuffer_array_handle=%p, uint32_t* all_buffers_size=%p",
            constbuffer_array_handle, all_buffers_size);
        result = MU_FAILURE;
    }
    else
    {
        uint32_t i;
        uint32_t total_size = 0;

        for (i = 0; i < constbuffer_array_handle->nBuffers; i++)
        {
            const CONSTBUFFER* content = CONSTBUFFER_GetContent(constbuffer_array_handle->buffers[i]);
            if (content != NULL)
            {
#if SIZE_MAX > UINT32_MAX
                if (content->size > UINT32_MAX)
                {
                    break;
                }
#endif
                if (total_size + (uint32_t)content->size < total_size)
                {
                    break;
                }

                total_size += (uint32_t)content->size;
            }
        }

        if (i < constbuffer_array_handle->nBuffers)
        {
            /* Codes_SRS_CONSTBUFFER_ARRAY_01_021: [ If summing up the sizes results in an uint32_t overflow, shall fail and return a non-zero value. ]*/
            LogError("Overflow in computing all buffers size");
            result = MU_FAILURE;
        }
        else
        {
            /* Codes_SRS_CONSTBUFFER_ARRAY_01_022: [ Otherwise constbuffer_array_get_all_buffers_size shall write in all_buffers_size the total size of all buffers in the array and return 0. ]*/
            *all_buffers_size = total_size;
            result = 0;
        }
    }

    return result;
}

IMPLEMENT_MOCKABLE_FUNCTION(, const CONSTBUFFER_HANDLE*, constbuffer_array_get_const_buffer_handle_array, CONSTBUFFER_ARRAY_HANDLE, constbuffer_array_handle)
{
    const CONSTBUFFER_HANDLE* result;

    /* Codes_SRS_CONSTBUFFER_ARRAY_01_026: [ If constbuffer_array_handle is NULL, constbuffer_array_get_const_buffer_handle_array shall fail and return NULL. ]*/
    if (constbuffer_array_handle == NULL)
    {
        LogError("CONSTBUFFER_ARRAY_HANDLE constbuffer_array_handle=%p", constbuffer_array_handle);
        result = NULL;
    }
    else
    {
        /* Codes_SRS_CONSTBUFFER_ARRAY_01_027: [ Otherwise constbuffer_array_get_const_buffer_handle_array shall return the array of const buffer handles backing the const buffer array. ]*/
        result = constbuffer_array_handle->buffers;
    }

    return result;
}


IMPLEMENT_MOCKABLE_FUNCTION(, bool, CONSTBUFFER_ARRAY_HANDLE_contain_same, CONSTBUFFER_ARRAY_HANDLE, left, CONSTBUFFER_ARRAY_HANDLE, right)
{
    bool result;
    if (left == NULL)
    {
        if (right == NULL)
        {
            /*Codes_SRS_CONSTBUFFER_ARRAY_02_050: [ If left is NULL and right is NULL then CONSTBUFFER_ARRAY_HANDLE_contain_same shall return true. ]*/
            result = true;
        }
        else
        {
            /*Codes_SRS_CONSTBUFFER_ARRAY_02_051: [ If left is NULL and right is not NULL then CONSTBUFFER_ARRAY_HANDLE_contain_same shall return false. ]*/
            result = false;
        }
    }
    else
    {
        if (right == NULL)
        {
            /*Codes_SRS_CONSTBUFFER_ARRAY_02_052: [ If left is not NULL and right is NULL then CONSTBUFFER_ARRAY_HANDLE_contain_same shall return false. ]*/
            result = false;
        }
        else
        {
            if (left->nBuffers != right->nBuffers)
            {
                /*Codes_SRS_CONSTBUFFER_ARRAY_02_053: [ If the number of CONSTBUFFER_HANDLEs in left is different then the number of CONSTBUFFER_HANDLEs in right then CONSTBUFFER_ARRAY_HANDLE_contain_same shall return false. ]*/
                result = false;
            }
            else
            {
                uint32_t i;
                for (i = 0; i < left->nBuffers; i++)
                {
                    /*Codes_SRS_CONSTBUFFER_ARRAY_02_054: [ If left and right CONSTBUFFER_HANDLEs at same index are different (as indicated by CONSTBUFFER_HANDLE_contain_same call) then CONSTBUFFER_ARRAY_HANDLE_contain_same shall return false. ]*/
                    if (!CONSTBUFFER_HANDLE_contain_same(left->buffers[i], right->buffers[i]))
                    {
                        break;
                    }
                }

                if (i == left->nBuffers)
                {
                    /*Codes_SRS_CONSTBUFFER_ARRAY_02_055: [ CONSTBUFFER_ARRAY_HANDLE_contain_same shall return true. ]*/
                    result = true;
                }
                else
                {
                    /*Codes_SRS_CONSTBUFFER_ARRAY_02_054: [ If left and right CONSTBUFFER_HANDLEs at same index are different (as indicated by CONSTBUFFER_HANDLE_contain_same call) then CONSTBUFFER_ARRAY_HANDLE_contain_same shall return false. ]*/
                    result = false;
                }
            }
        }
    }
    return result;
}
