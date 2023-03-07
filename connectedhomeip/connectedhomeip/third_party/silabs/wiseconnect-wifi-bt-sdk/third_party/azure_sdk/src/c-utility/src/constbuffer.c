// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <stdbool.h>
#include "azure_macro_utils/macro_utils.h"
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/refcount.h"

#include "azure_c_shared_utility/constbuffer.h"

#define CONSTBUFFER_TYPE_VALUES \
    CONSTBUFFER_TYPE_COPIED, \
    CONSTBUFFER_TYPE_MEMORY_MOVED, \
    CONSTBUFFER_TYPE_WITH_CUSTOM_FREE, \
    CONSTBUFFER_TYPE_FROM_OFFSET_AND_SIZE

MU_DEFINE_ENUM(CONSTBUFFER_TYPE, CONSTBUFFER_TYPE_VALUES)

typedef struct CONSTBUFFER_HANDLE_DATA_TAG
{
    CONSTBUFFER alias;
    COUNT_TYPE count;
    CONSTBUFFER_TYPE buffer_type;
    CONSTBUFFER_CUSTOM_FREE_FUNC custom_free_func;
    void* custom_free_func_context;
    CONSTBUFFER_HANDLE originalHandle; /*where the CONSTBUFFER_TYPE_FROM_OFFSET_AND_SIZE was build from*/
} CONSTBUFFER_HANDLE_DATA;

static CONSTBUFFER_HANDLE CONSTBUFFER_Create_Internal(const unsigned char* source, size_t size)
{
    CONSTBUFFER_HANDLE result;
    /*Codes_SRS_CONSTBUFFER_02_005: [The non-NULL handle returned by CONSTBUFFER_Create shall have its ref count set to "1".]*/
    /*Codes_SRS_CONSTBUFFER_02_010: [The non-NULL handle returned by CONSTBUFFER_CreateFromBuffer shall have its ref count set to "1".]*/
    result = (CONSTBUFFER_HANDLE)calloc(1, (sizeof(CONSTBUFFER_HANDLE_DATA) + size));
    if (result == NULL)
    {
        /*Codes_SRS_CONSTBUFFER_02_003: [If creating the copy fails then CONSTBUFFER_Create shall return NULL.]*/
        /*Codes_SRS_CONSTBUFFER_02_008: [If copying the content fails, then CONSTBUFFER_CreateFromBuffer shall fail and return NULL.] */
        LogError("unable to malloc");
        /*return as is*/
    }
    else
    {
        INIT_REF_VAR(result->count);

        /*Codes_SRS_CONSTBUFFER_02_002: [Otherwise, CONSTBUFFER_Create shall create a copy of the memory area pointed to by source having size bytes.]*/
        result->alias.size = size;
        if (size == 0)
        {
            result->alias.buffer = NULL;
        }
        else
        {
            unsigned char* temp = (unsigned char*)(result + 1);
            /*Codes_SRS_CONSTBUFFER_02_004: [Otherwise CONSTBUFFER_Create shall return a non-NULL handle.]*/
            /*Codes_SRS_CONSTBUFFER_02_007: [Otherwise, CONSTBUFFER_CreateFromBuffer shall copy the content of buffer.]*/
            /*Codes_SRS_CONSTBUFFER_02_009: [Otherwise, CONSTBUFFER_CreateFromBuffer shall return a non-NULL handle.]*/
            (void)memcpy(temp, source, size);
            result->alias.buffer = temp;
        }

        result->buffer_type = CONSTBUFFER_TYPE_COPIED;
    }
    return result;
}

IMPLEMENT_MOCKABLE_FUNCTION(, CONSTBUFFER_HANDLE, CONSTBUFFER_Create, const unsigned char*, source, size_t, size)
{
    CONSTBUFFER_HANDLE result;
    /*Codes_SRS_CONSTBUFFER_02_001: [If source is NULL and size is different than 0 then CONSTBUFFER_Create shall fail and return NULL.]*/
    if (
        (source == NULL) &&
        (size != 0)
        )
    {
        LogError("invalid arguments passes to CONSTBUFFER_Create");
        result = NULL;
    }
    else
    {
        result = CONSTBUFFER_Create_Internal(source, size);
    }
    return result;
}

/*this creates a new constbuffer from an existing BUFFER_HANDLE*/
IMPLEMENT_MOCKABLE_FUNCTION(, CONSTBUFFER_HANDLE, CONSTBUFFER_CreateFromBuffer, BUFFER_HANDLE, buffer)
{
    CONSTBUFFER_HANDLE result;
    /*Codes_SRS_CONSTBUFFER_02_006: [If buffer is NULL then CONSTBUFFER_CreateFromBuffer shall fail and return NULL.]*/
    if (buffer == NULL)
    {
        LogError("invalid arg passed to CONSTBUFFER_CreateFromBuffer");
        result = NULL;
    }
    else
    {
        size_t length = BUFFER_length(buffer);
        unsigned char* rawBuffer = BUFFER_u_char(buffer);
        result = CONSTBUFFER_Create_Internal(rawBuffer, length);
    }
    return result;
}

IMPLEMENT_MOCKABLE_FUNCTION(, CONSTBUFFER_HANDLE, CONSTBUFFER_CreateWithMoveMemory, unsigned char*, source, size_t, size)
{
    CONSTBUFFER_HANDLE result;

    /* Codes_SRS_CONSTBUFFER_01_001: [ If source is NULL and size is different than 0 then CONSTBUFFER_Create shall fail and return NULL. ]*/
    if ((source == NULL) && (size > 0))
    {
        LogError("Invalid arguments: unsigned char* source=%p, size_t size=%u", source, (unsigned int)size);
        result = NULL;
    }
    else
    {
        result = (CONSTBUFFER_HANDLE)calloc(1, sizeof(CONSTBUFFER_HANDLE_DATA));
        if (result == NULL)
        {
            /* Codes_SRS_CONSTBUFFER_01_005: [ If any error occurs, CONSTBUFFER_CreateWithMoveMemory shall fail and return NULL. ]*/
            LogError("malloc failed");
        }
        else
        {
            /* Codes_SRS_CONSTBUFFER_01_004: [ If source is non-NULL and size is 0, the source pointer shall be owned (and freed) by the newly created instance of const buffer. ]*/
            /* Codes_SRS_CONSTBUFFER_01_002: [ CONSTBUFFER_CreateWithMoveMemory shall store the source and size and return a non-NULL handle to the newly created const buffer. ]*/
            result->alias.buffer = source;
            result->alias.size = size;
            result->buffer_type = CONSTBUFFER_TYPE_MEMORY_MOVED;

            /* Codes_SRS_CONSTBUFFER_01_003: [ The non-NULL handle returned by CONSTBUFFER_CreateWithMoveMemory shall have its ref count set to "1". ]*/
            INIT_REF_VAR(result->count);
        }
    }

    return result;
}

IMPLEMENT_MOCKABLE_FUNCTION(, CONSTBUFFER_HANDLE, CONSTBUFFER_CreateWithCustomFree, const unsigned char*, source, size_t, size, CONSTBUFFER_CUSTOM_FREE_FUNC, customFreeFunc, void*, customFreeFuncContext)
{
    CONSTBUFFER_HANDLE result;

    /* Codes_SRS_CONSTBUFFER_01_014: [ customFreeFuncContext shall be allowed to be NULL. ]*/

    if (
        /* Codes_SRS_CONSTBUFFER_01_006: [ If source is NULL and size is different than 0 then CONSTBUFFER_CreateWithCustomFree shall fail and return NULL. ]*/
        ((source == NULL) && (size > 0)) ||
        /* Codes_SRS_CONSTBUFFER_01_013: [ If customFreeFunc is NULL, CONSTBUFFER_CreateWithCustomFree shall fail and return NULL. ]*/
        (customFreeFunc == NULL)
        )
    {
        LogError("Invalid arguments: unsigned char* source=%p, size_t size=%u, customFreeFunc=%p, customFreeFuncContext=%p",
            source, (unsigned int)size, customFreeFunc, customFreeFuncContext);
        result = NULL;
    }
    else
    {
        result = (CONSTBUFFER_HANDLE)calloc(1, sizeof(CONSTBUFFER_HANDLE_DATA));
        if (result == NULL)
        {
            /* Codes_SRS_CONSTBUFFER_01_011: [ If any error occurs, CONSTBUFFER_CreateWithMoveMemory shall fail and return NULL. ]*/
            LogError("malloc failed");
        }
        else
        {
            /* Codes_SRS_CONSTBUFFER_01_007: [ If source is non-NULL and size is 0, the source pointer shall be owned (and freed) by the newly created instance of const buffer. ]*/
            /* Codes_SRS_CONSTBUFFER_01_008: [ CONSTBUFFER_CreateWithCustomFree shall store the source and size and return a non-NULL handle to the newly created const buffer. ]*/
            result->alias.buffer = source;
            result->alias.size = size;
            result->buffer_type = CONSTBUFFER_TYPE_WITH_CUSTOM_FREE;

            /* Codes_SRS_CONSTBUFFER_01_009: [ CONSTBUFFER_CreateWithCustomFree shall store customFreeFunc and customFreeFuncContext in order to use them to free the memory when the CONST buffer resources are freed. ]*/
            result->custom_free_func = customFreeFunc;
            result->custom_free_func_context = customFreeFuncContext;

            /* Codes_SRS_CONSTBUFFER_01_010: [ The non-NULL handle returned by CONSTBUFFER_CreateWithCustomFree shall have its ref count set to 1. ]*/
            INIT_REF_VAR(result->count);
        }
    }

    return result;
}

IMPLEMENT_MOCKABLE_FUNCTION(, CONSTBUFFER_HANDLE, CONSTBUFFER_CreateFromOffsetAndSize, CONSTBUFFER_HANDLE, handle, size_t, offset, size_t, size)
{
    CONSTBUFFER_HANDLE result;

    if (
        /*Codes_SRS_CONSTBUFFER_02_025: [ If handle is NULL then CONSTBUFFER_CreateFromOffsetAndSize shall fail and return NULL. ]*/
        (handle == NULL) ||
        /*Codes_SRS_CONSTBUFFER_02_026: [ If offset is greater than or equal to handles's size then CONSTBUFFER_CreateFromOffsetAndSize shall fail and return NULL. ]*/
        (offset > handle->alias.size) ||
        /*Codes_SRS_CONSTBUFFER_02_032: [ If there are any failures then CONSTBUFFER_CreateFromOffsetAndSize shall fail and return NULL. ]*/
        (offset > SIZE_MAX - size) ||
        /*Codes_SRS_CONSTBUFFER_02_027: [ If offset + size exceed handles's size then CONSTBUFFER_CreateFromOffsetAndSize shall fail and return NULL. ]*/
        (offset + size > handle->alias.size)
        )
    {
        LogError("invalid arguments CONSTBUFFER_HANDLE handle=%p, size_t offset=%zu, size_t size=%zu",
            handle, offset, size);
        result = NULL;
    }
    else
    {
        /*Codes_SRS_CONSTBUFFER_02_028: [ CONSTBUFFER_CreateFromOffsetAndSize shall allocate memory for a new CONSTBUFFER_HANDLE's content. ]*/
        result = (CONSTBUFFER_HANDLE)calloc(1, sizeof(CONSTBUFFER_HANDLE_DATA));
        if (result == NULL)
        {
            /*Codes_SRS_CONSTBUFFER_02_032: [ If there are any failures then CONSTBUFFER_CreateFromOffsetAndSize shall fail and return NULL. ]*/
            LogError("failure in malloc(sizeof(CONSTBUFFER_HANDLE_DATA)=%zu)", sizeof(CONSTBUFFER_HANDLE_DATA));
            /*return as is*/
        }
        else
        {
            result->buffer_type = CONSTBUFFER_TYPE_FROM_OFFSET_AND_SIZE;
            result->alias.buffer = handle->alias.buffer+offset;
            result->alias.size = size;

            /*Codes_SRS_CONSTBUFFER_02_030: [ CONSTBUFFER_CreateFromOffsetAndSize shall increment the reference count of handle. ]*/
            INC_REF_VAR(handle->count);
            result->originalHandle = handle;

            /*Codes_SRS_CONSTBUFFER_02_029: [ CONSTBUFFER_CreateFromOffsetAndSize shall set the ref count of the newly created CONSTBUFFER_HANDLE to the initial value. ]*/
            INIT_REF_VAR(result->count);

            /*Codes_SRS_CONSTBUFFER_02_031: [ CONSTBUFFER_CreateFromOffsetAndSize shall succeed and return a non-NULL value. ]*/
        }
    }
    return result;
}

IMPLEMENT_MOCKABLE_FUNCTION(, void, CONSTBUFFER_IncRef, CONSTBUFFER_HANDLE, constbufferHandle)
{
    if (constbufferHandle == NULL)
    {
        /*Codes_SRS_CONSTBUFFER_02_013: [If constbufferHandle is NULL then CONSTBUFFER_IncRef shall return.]*/
        LogError("Invalid arguments: CONSTBUFFER_HANDLE constbufferHandle=%p", constbufferHandle);
    }
    else
    {
        /*Codes_SRS_CONSTBUFFER_02_014: [Otherwise, CONSTBUFFER_IncRef shall increment the reference count.]*/
        INC_REF_VAR(constbufferHandle->count);
    }
}

IMPLEMENT_MOCKABLE_FUNCTION(, const CONSTBUFFER*, CONSTBUFFER_GetContent, CONSTBUFFER_HANDLE, constbufferHandle)
{
    const CONSTBUFFER* result;
    if (constbufferHandle == NULL)
    {
        /*Codes_SRS_CONSTBUFFER_02_011: [If constbufferHandle is NULL then CONSTBUFFER_GetContent shall return NULL.]*/
        result = NULL;
        LogError("invalid arg");
    }
    else
    {
        /*Codes_SRS_CONSTBUFFER_02_012: [Otherwise, CONSTBUFFER_GetContent shall return a const CONSTBUFFER* that matches byte by byte the original bytes used to created the const buffer and has the same length.]*/
        result = &(constbufferHandle->alias);
    }
    return result;
}

static void CONSTBUFFER_DecRef_internal(CONSTBUFFER_HANDLE constbufferHandle)
{
    /*Codes_SRS_CONSTBUFFER_02_016: [Otherwise, CONSTBUFFER_DecRef shall decrement the refcount on the constbufferHandle handle.]*/
    if (DEC_REF_VAR(constbufferHandle->count) == DEC_RETURN_ZERO)
    {
        if (constbufferHandle->buffer_type == CONSTBUFFER_TYPE_MEMORY_MOVED)
        {
            free((void*)constbufferHandle->alias.buffer);
        }
        else if (constbufferHandle->buffer_type == CONSTBUFFER_TYPE_WITH_CUSTOM_FREE)
        {
            /* Codes_SRS_CONSTBUFFER_01_012: [ If the buffer was created by calling CONSTBUFFER_CreateWithCustomFree, the customFreeFunc function shall be called to free the memory, while passed customFreeFuncContext as argument. ]*/
            constbufferHandle->custom_free_func(constbufferHandle->custom_free_func_context);
        }
        /*Codes_SRS_CONSTBUFFER_02_024: [ If the constbufferHandle was created by calling CONSTBUFFER_CreateFromOffsetAndSize then CONSTBUFFER_DecRef shall decrement the ref count of the original handle passed to CONSTBUFFER_CreateFromOffsetAndSize. ]*/
        else if (constbufferHandle->buffer_type == CONSTBUFFER_TYPE_FROM_OFFSET_AND_SIZE)
        {
            CONSTBUFFER_DecRef_internal(constbufferHandle->originalHandle);
        }

        /*Codes_SRS_CONSTBUFFER_02_017: [If the refcount reaches zero, then CONSTBUFFER_DecRef shall deallocate all resources used by the CONSTBUFFER_HANDLE.]*/
        free(constbufferHandle);
    }
}

IMPLEMENT_MOCKABLE_FUNCTION(, void, CONSTBUFFER_DecRef, CONSTBUFFER_HANDLE, constbufferHandle)
{
    if (constbufferHandle == NULL)
    {
        /*Codes_SRS_CONSTBUFFER_02_015: [If constbufferHandle is NULL then CONSTBUFFER_DecRef shall do nothing.]*/
        LogError("Invalid arguments: CONSTBUFFER_HANDLE constbufferHandle=%p", constbufferHandle);
    }
    else
    {
        CONSTBUFFER_DecRef_internal(constbufferHandle);
    }
}


IMPLEMENT_MOCKABLE_FUNCTION(, bool, CONSTBUFFER_HANDLE_contain_same, CONSTBUFFER_HANDLE, left, CONSTBUFFER_HANDLE, right)
{
    bool result;
    if (left == NULL)
    {
        if (right == NULL)
        {
            /*Codes_SRS_CONSTBUFFER_02_018: [ If left is NULL and right is NULL then CONSTBUFFER_HANDLE_contain_same shall return true. ]*/
            result = true;
        }
        else
        {
            /*Codes_SRS_CONSTBUFFER_02_019: [ If left is NULL and right is not NULL then CONSTBUFFER_HANDLE_contain_same shall return false. ]*/
            result = false;
        }
    }
    else
    {
        if (right == NULL)
        {
            /*Codes_SRS_CONSTBUFFER_02_020: [ If left is not NULL and right is NULL then CONSTBUFFER_HANDLE_contain_same shall return false. ]*/
            result = false;
        }
        else
        {
            if (left->alias.size != right->alias.size)
            {
                /*Codes_SRS_CONSTBUFFER_02_021: [ If left's size is different than right's size then CONSTBUFFER_HANDLE_contain_same shall return false. ]*/
                result = false;
            }
            else
            {
                if (memcmp(left->alias.buffer, right->alias.buffer, left->alias.size) != 0)
                {
                    /*Codes_SRS_CONSTBUFFER_02_022: [ If left's buffer is contains different bytes than rights's buffer then CONSTBUFFER_HANDLE_contain_same shall return false. ]*/
                    result = false;
                }
                else
                {
                    /*Codes_SRS_CONSTBUFFER_02_023: [ CONSTBUFFER_HANDLE_contain_same shall return true. ]*/
                    result = true;
                }
            }
        }
    }
    return result;
}
