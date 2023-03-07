// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/buffer_.h"
#include "azure_c_shared_utility/optimize_size.h"
#include "azure_c_shared_utility/xlogging.h"

typedef struct BUFFER_TAG
{
    unsigned char* buffer;
    size_t size;
} BUFFER;

/* Codes_SRS_BUFFER_07_001: [BUFFER_new shall allocate a BUFFER_HANDLE that will contain a NULL unsigned char*.] */
BUFFER_HANDLE BUFFER_new(void)
{
    BUFFER* temp = (BUFFER*)calloc(1, sizeof(BUFFER));
    /* Codes_SRS_BUFFER_07_002: [BUFFER_new shall return NULL on any error that occurs.] */
    if (temp != NULL)
    {
        temp->buffer = NULL;
        temp->size = 0;
    }
    return (BUFFER_HANDLE)temp;
}

static int BUFFER_safemalloc(BUFFER* handleptr, size_t size)
{
    int result;
    size_t sizetomalloc = size;
    if (size == 0)
    {
        sizetomalloc = 1;
    }
    handleptr->buffer = (unsigned char*)malloc(sizetomalloc);
    if (handleptr->buffer == NULL)
    {
        /*Codes_SRS_BUFFER_02_003: [If allocating memory fails, then BUFFER_create shall return NULL.]*/
        LogError("Failure allocating data");
        result = MU_FAILURE;
    }
    else
    {
        // we still consider the real buffer size is 0
        handleptr->size = size;
        result = 0;
    }
    return result;
}

BUFFER_HANDLE BUFFER_create(const unsigned char* source, size_t size)
{
    BUFFER* result;
    /*Codes_SRS_BUFFER_02_001: [If source is NULL then BUFFER_create shall return NULL.]*/
    if (source == NULL)
    {
        LogError("invalid parameter source: %p", source);
        result = NULL;
    }
    else
    {
        /*Codes_SRS_BUFFER_02_002: [Otherwise, BUFFER_create shall allocate memory to hold size bytes and shall copy from source size bytes into the newly allocated memory.] */
        result = (BUFFER*)calloc(1, sizeof(BUFFER));
        if (result == NULL)
        {
            /*Codes_SRS_BUFFER_02_003: [If allocating memory fails, then BUFFER_create shall return NULL.] */
            /*fallthrough*/
            LogError("Failure allocating BUFFER structure");
        }
        else
        {
            /* Codes_SRS_BUFFER_02_005: [If size parameter is 0 then 1 byte of memory shall be allocated yet size of the buffer shall be set to 0.]*/
            if (BUFFER_safemalloc(result, size) != 0)
            {
                LogError("unable to BUFFER_safemalloc ");
                free(result);
                result = NULL;
            }
            else
            {
                /*Codes_SRS_BUFFER_02_004: [Otherwise, BUFFER_create shall return a non-NULL handle.] */
                (void)memcpy(result->buffer, source, size);
            }
        }
    }
    return (BUFFER_HANDLE)result;
}

// Codes_SRS_BUFFER_07_029: [ BUFFER_create_with_size shall create a BUFFER_HANDLE with a pre allocated underlying buffer size.]
BUFFER_HANDLE BUFFER_create_with_size(size_t buff_size)
{
    BUFFER* result;
    result = (BUFFER*)calloc(1, sizeof(BUFFER));
    if (result != NULL)
    {
        if (buff_size == 0)
        {
            // Codes_SRS_BUFFER_07_030: [ If buff_size is 0 BUFFER_create_with_size shall create a valid non-NULL handle of zero size. ]
            result->size = 0;
            result->buffer = NULL;
        }
        else
        {
            // Codes_SRS_BUFFER_07_031: [ BUFFER_create_with_size shall allocate a buffer of buff_size. ]
            result->size = buff_size;
            if ((result->buffer = (unsigned char*)malloc(result->size)) == NULL)
            {
                // Codes_SRS_BUFFER_07_032: [ If allocating memory fails, then BUFFER_create_with_size shall return NULL. ]
                LogError("unable to allocate buffer");
                free(result);
                result = NULL;
            }
        }
    }
    else
    {
        // Codes_SRS_BUFFER_07_032: [ If allocating memory fails, then BUFFER_create_with_size shall return NULL. ]
        LogError("unable to allocate BUFFER");
    }
    // Codes_SRS_BUFFER_07_033: [ Otherwise, BUFFER_create_with_size shall return a non-NULL handle. ]
    return (BUFFER_HANDLE)result;
}

/* Codes_SRS_BUFFER_07_003: [BUFFER_delete shall delete the data associated with the BUFFER_HANDLE along with the Buffer.] */
void BUFFER_delete(BUFFER_HANDLE handle)
{
    /* Codes_SRS_BUFFER_07_004: [BUFFER_delete shall not delete any BUFFER_HANDLE that is NULL.] */
    if (handle != NULL)
    {
        BUFFER* b = (BUFFER*)handle;
        if (b->buffer != NULL)
        {
            /* Codes_SRS_BUFFER_07_003: [BUFFER_delete shall delete the data associated with the BUFFER_HANDLE along with the Buffer.] */
            free(b->buffer);
        }
        free(b);
    }
}

/*return 0 if the buffer was copied*/
/*else return different than zero*/
/* Codes_SRS_BUFFER_07_008: [BUFFER_build allocates size_t bytes, copies the unsigned char* into the buffer and returns zero on success.] */
int BUFFER_build(BUFFER_HANDLE handle, const unsigned char* source, size_t size)
{
    int result;
    if (handle == NULL)
    {
        /* Codes_SRS_BUFFER_07_009: [BUFFER_build shall return nonzero if handle is NULL ] */
        result = MU_FAILURE;
    }
    /* Codes_SRS_BUFFER_01_002: [The size argument can be zero, in which case the underlying buffer held by the buffer instance shall be freed.] */
    else if (size == 0)
    {
        /* Codes_SRS_BUFFER_01_003: [If size is zero, source can be NULL.] */
        BUFFER* b = (BUFFER*)handle;
        free(b->buffer);
        b->buffer = NULL;
        b->size = 0;

        result = 0;
    }
    else
    {
        if (source == NULL)
        {
            /* Codes_SRS_BUFFER_01_001: [If size is positive and source is NULL, BUFFER_build shall return nonzero] */
            result = MU_FAILURE;
        }
        else
        {
            BUFFER* b = (BUFFER*)handle;
            /* Codes_SRS_BUFFER_07_011: [BUFFER_build shall overwrite previous contents if the buffer has been previously allocated.] */
            unsigned char* newBuffer = (unsigned char*)realloc(b->buffer, size);
            if (newBuffer == NULL)
            {
                /* Codes_SRS_BUFFER_07_010: [BUFFER_build shall return nonzero if any error is encountered.] */
                LogError("Failure reallocating buffer");
                result = MU_FAILURE;
            }
            else
            {
                b->buffer = newBuffer;
                b->size = size;
                /* Codes_SRS_BUFFER_01_002: [The size argument can be zero, in which case nothing shall be copied from source.] */
                (void)memcpy(b->buffer, source, size);

                result = 0;
            }
        }
    }

    return result;
}

int BUFFER_append_build(BUFFER_HANDLE handle, const unsigned char* source, size_t size)
{
    int result;
    if (handle == NULL || source == NULL || size == 0)
    {
        /* Codes_SRS_BUFFER_07_029: [ BUFFER_append_build shall return nonzero if handle or source are NULL or if size is 0. ] */
        LogError("BUFFER_append_build failed invalid parameter handle: %p, source: %p, size: %lu", handle, source, (unsigned long)size);
        result = MU_FAILURE;
    }
    else
    {
        if (handle->buffer == NULL)
        {
            /* Codes_SRS_BUFFER_07_030: [ if handle->buffer is NULL BUFFER_append_build shall allocate the a buffer of size bytes... ] */
            if (BUFFER_safemalloc(handle, size) != 0)
            {
                /* Codes_SRS_BUFFER_07_035: [ If any error is encountered BUFFER_append_build shall return a non-null value. ] */
                LogError("Failure with BUFFER_safemalloc");
                result = MU_FAILURE;
            }
            else
            {
                /* Codes_SRS_BUFFER_07_031: [ ... and copy the contents of source to handle->buffer. ] */
                (void)memcpy(handle->buffer, source, size);
                /* Codes_SRS_BUFFER_07_034: [ On success BUFFER_append_build shall return 0 ] */
                result = 0;
            }
        }
        else
        {
            /* Codes_SRS_BUFFER_07_032: [ if handle->buffer is not NULL BUFFER_append_build shall realloc the buffer to be the handle->size + size ] */
            unsigned char* temp = (unsigned char*)realloc(handle->buffer, handle->size + size);
            if (temp == NULL)
            {
                /* Codes_SRS_BUFFER_07_035: [ If any error is encountered BUFFER_append_build shall return a non-null value. ] */
                LogError("Failure reallocating temporary buffer");
                result = MU_FAILURE;
            }
            else
            {
                /* Codes_SRS_BUFFER_07_033: [ ... and copy the contents of source to the end of the buffer. ] */
                handle->buffer = temp;
                // Append the BUFFER
                (void)memcpy(&handle->buffer[handle->size], source, size);
                handle->size += size;
                /* Codes_SRS_BUFFER_07_034: [ On success BUFFER_append_build shall return 0 ] */
                result = 0;
            }
        }
    }
    return result;
}

/*return 0 if the buffer was pre-build(that is, had its space allocated)*/
/*else return different than zero*/
/* Codes_SRS_BUFFER_07_005: [BUFFER_pre_build allocates size_t bytes of BUFFER_HANDLE and returns zero on success.] */
int BUFFER_pre_build(BUFFER_HANDLE handle, size_t size)
{
    int result;
    if (handle == NULL)
    {
        /* Codes_SRS_BUFFER_07_006: [If handle is NULL or size is 0 then BUFFER_pre_build shall return a nonzero value.] */
        result = MU_FAILURE;
    }
    else if (size == 0)
    {
        /* Codes_SRS_BUFFER_07_006: [If handle is NULL or size is 0 then BUFFER_pre_build shall return a nonzero value.] */
        result = MU_FAILURE;
    }
    else
    {
        BUFFER* b = (BUFFER*)handle;
        if (b->buffer != NULL)
        {
            /* Codes_SRS_BUFFER_07_007: [BUFFER_pre_build shall return nonzero if the buffer has been previously allocated and is not NULL.] */
            LogError("Failure buffer data is NULL");
            result = MU_FAILURE;
        }
        else
        {
            if ((b->buffer = (unsigned char*)malloc(size)) == NULL)
            {
                /* Codes_SRS_BUFFER_07_013: [BUFFER_pre_build shall return nonzero if any error is encountered.] */
                LogError("Failure allocating buffer");
                result = MU_FAILURE;
            }
            else
            {
                b->size = size;
                result = 0;
            }
        }
    }
    return result;
}

/* Codes_SRS_BUFFER_07_019: [BUFFER_content shall return the data contained within the BUFFER_HANDLE.] */
int BUFFER_content(BUFFER_HANDLE handle, const unsigned char** content)
{
    int result;
    if ((handle == NULL) || (content == NULL))
    {
        /* Codes_SRS_BUFFER_07_020: [If the handle and/or content*is NULL BUFFER_content shall return nonzero.] */
        result = MU_FAILURE;
    }
    else
    {
        BUFFER* b = (BUFFER*)handle;
        *content = b->buffer;
        result = 0;
    }
    return result;
}

/*return 0 if everything went ok and whatever was built in the buffer was unbuilt*/
/* Codes_SRS_BUFFER_07_012: [BUFFER_unbuild shall clear the underlying unsigned char* data associated with the BUFFER_HANDLE this will return zero on success.] */
extern int BUFFER_unbuild(BUFFER_HANDLE handle)
{
    int result;
    if (handle == NULL)
    {
        /* Codes_SRS_BUFFER_07_014: [BUFFER_unbuild shall return a nonzero value if BUFFER_HANDLE is NULL.] */
        LogError("Failure: handle is invalid.");
        result = MU_FAILURE;
    }
    else
    {
        BUFFER* b = (BUFFER*)handle;
        if (b->buffer != NULL)
        {
            free(b->buffer);
            b->buffer = NULL;
            b->size = 0;
        
        }

        /* Codes_SRS_BUFFER_07_015: [BUFFER_unbuild shall always return success if the unsigned char* referenced by BUFFER_HANDLE is NULL.] */
        result = 0;
    }
    return result;
}

/* Codes_SRS_BUFFER_07_016: [BUFFER_enlarge shall increase the size of the unsigned char* referenced by BUFFER_HANDLE.] */
int BUFFER_enlarge(BUFFER_HANDLE handle, size_t enlargeSize)
{
    int result;
    if (handle == NULL)
    {
        /* Codes_SRS_BUFFER_07_017: [BUFFER_enlarge shall return a nonzero result if any parameters are NULL or zero.] */
        LogError("Failure: handle is invalid.");
        result = MU_FAILURE;
    }
    else if (enlargeSize == 0)
    {
        /* Codes_SRS_BUFFER_07_017: [BUFFER_enlarge shall return a nonzero result if any parameters are NULL or zero.] */
        LogError("Failure: enlargeSize size is 0.");
        result = MU_FAILURE;
    }
    else
    {
        BUFFER* b = (BUFFER*)handle;
        unsigned char* temp = (unsigned char*)realloc(b->buffer, b->size + enlargeSize);
        if (temp == NULL)
        {
            /* Codes_SRS_BUFFER_07_018: [BUFFER_enlarge shall return a nonzero result if any error is encountered.] */
            LogError("Failure: allocating temp buffer.");
            result = MU_FAILURE;
        }
        else
        {
            b->buffer = temp;
            b->size += enlargeSize;
            result = 0;
        }
    }
    return result;
}

int BUFFER_shrink(BUFFER_HANDLE handle, size_t decreaseSize, bool fromEnd)
{
    int result;
    if (handle == NULL)
    {
        /* Codes_SRS_BUFFER_07_036: [ if handle is NULL, BUFFER_shrink shall return a non-null value ]*/
        LogError("Failure: handle is invalid.");
        result = MU_FAILURE;
    }
    else if (decreaseSize == 0)
    {
        /* Codes_SRS_BUFFER_07_037: [ If decreaseSize is equal zero, BUFFER_shrink shall return a non-null value ] */
        LogError("Failure: decrease size is 0.");
        result = MU_FAILURE;
    }
    else if (decreaseSize > handle->size)
    {
        /* Codes_SRS_BUFFER_07_038: [ If decreaseSize is less than the size of the buffer, BUFFER_shrink shall return a non-null value ] */
        LogError("Failure: decrease size is less than buffer size.");
        result = MU_FAILURE;
    }
    else
    {
        /* Codes_SRS_BUFFER_07_039: [ BUFFER_shrink shall allocate a temporary buffer of existing buffer size minus decreaseSize. ] */
        size_t alloc_size = handle->size - decreaseSize;
        if (alloc_size == 0)
        {
            /* Codes_SRS_BUFFER_07_043: [ If the decreaseSize is equal the buffer size , BUFFER_shrink shall deallocate the buffer and set the size to zero. ] */
            free(handle->buffer);
            handle->buffer = NULL;
            handle->size = 0;
            result = 0;
        }
        else
        {
            unsigned char* tmp = malloc(alloc_size);
            if (tmp == NULL)
            {
                /* Codes_SRS_BUFFER_07_042: [ If a failure is encountered, BUFFER_shrink shall return a non-null value ] */
                LogError("Failure: allocating temp buffer.");
                result = MU_FAILURE;
            }
            else
            {
                if (fromEnd)
                {
                    /* Codes_SRS_BUFFER_07_040: [ if the fromEnd variable is true, BUFFER_shrink shall remove the end of the buffer of size decreaseSize. ] */
                    memcpy(tmp, handle->buffer, alloc_size);
                    free(handle->buffer);
                    handle->buffer = tmp;
                    handle->size = alloc_size;
                    result = 0;
                }
                else
                {
                    /* Codes_SRS_BUFFER_07_041: [ if the fromEnd variable is false, BUFFER_shrink shall remove the beginning of the buffer of size decreaseSize. ] */
                    memcpy(tmp, handle->buffer + decreaseSize, alloc_size);
                    free(handle->buffer);
                    handle->buffer = tmp;
                    handle->size = alloc_size;
                    result = 0;
                }
            }
        }
    }
    return result;
}

/* Codes_SRS_BUFFER_07_021: [BUFFER_size shall place the size of the associated buffer in the size variable and return zero on success.] */
int BUFFER_size(BUFFER_HANDLE handle, size_t* size)
{
    int result;
    if ((handle == NULL) || (size == NULL))
    {
        /* Codes_SRS_BUFFER_07_022: [BUFFER_size shall return a nonzero value for any error that is encountered.] */
        result = MU_FAILURE;
    }
    else
    {
        BUFFER* b = (BUFFER*)handle;
        *size = b->size;
        result = 0;
    }
    return result;
}

/* Codes_SRS_BUFFER_07_024: [BUFFER_append concatenates b2 onto b1 without modifying b2 and shall return zero on success.] */
int BUFFER_append(BUFFER_HANDLE handle1, BUFFER_HANDLE handle2)
{
    int result;
    if ( (handle1 == NULL) || (handle2 == NULL) || (handle1 == handle2) )
    {
        /* Codes_SRS_BUFFER_07_023: [BUFFER_append shall return a nonzero upon any error that is encountered.] */
        result = MU_FAILURE;
    }
    else
    {
        BUFFER* b1 = (BUFFER*)handle1;
        BUFFER* b2 = (BUFFER*)handle2;
        if (b1->buffer == NULL)
        {
            /* Codes_SRS_BUFFER_07_023: [BUFFER_append shall return a nonzero upon any error that is encountered.] */
            result = MU_FAILURE;
        }
        else if (b2->buffer == NULL)
        {
            /* Codes_SRS_BUFFER_07_023: [BUFFER_append shall return a nonzero upon any error that is encountered.] */
            result = MU_FAILURE;
        }
        else
        {
            if (b2->size ==0)
            {
                // b2->size = 0, whatever b1->size is, do nothing
                result = 0;
            }
            else
            {
                // b2->size != 0, whatever b1->size is
                unsigned char* temp = (unsigned char*)realloc(b1->buffer, b1->size + b2->size);
                if (temp == NULL)
                {
                    /* Codes_SRS_BUFFER_07_023: [BUFFER_append shall return a nonzero upon any error that is encountered.] */
                    LogError("Failure: allocating temp buffer.");
                    result = MU_FAILURE;
                }
                else
                {
                    /* Codes_SRS_BUFFER_07_024: [BUFFER_append concatenates b2 onto b1 without modifying b2 and shall return zero on success.]*/
                    b1->buffer = temp;
                    // Append the BUFFER
                    (void)memcpy(&b1->buffer[b1->size], b2->buffer, b2->size);
                    b1->size += b2->size;
                    result = 0;
                }
            }
        }
    }
    return result;
}

int BUFFER_prepend(BUFFER_HANDLE handle1, BUFFER_HANDLE handle2)
{
    int result;
    if ((handle1 == NULL) || (handle2 == NULL) || (handle1 == handle2))
    {
        /* Codes_SRS_BUFFER_01_005: [ BUFFER_prepend shall return a non-zero upon value any error that is encountered. ]*/
        result = MU_FAILURE;
    }
    else
    {
        BUFFER* b1 = (BUFFER*)handle1;
        BUFFER* b2 = (BUFFER*)handle2;
        if (b1->buffer == NULL)
        {
            /* Codes_SRS_BUFFER_01_005: [ BUFFER_prepend shall return a non-zero upon value any error that is encountered. ]*/
            result = MU_FAILURE;
        }
        else if (b2->buffer == NULL)
        {
            /* Codes_SRS_BUFFER_01_005: [ BUFFER_prepend shall return a non-zero upon value any error that is encountered. ]*/
            result = MU_FAILURE;
        }
        else
        {
            //put b2 ahead of b1: [b2][b1], return b1
            if (b2->size == 0)
            {
                // do nothing
                result = 0;
            }
            else if (b1->size + b2->size < b2->size)
            {
                LogError("Failure: size_t overflow.");
                result = MU_FAILURE;
            }
            else
            {
                // b2->size != 0
                unsigned char* temp = (unsigned char*)malloc(b1->size + b2->size);
                if (temp == NULL)
                {
                    /* Codes_SRS_BUFFER_01_005: [ BUFFER_prepend shall return a non-zero upon value any error that is encountered. ]*/
                    LogError("Failure: allocating temp buffer.");
                    result = MU_FAILURE;
                }
                else
                {
                    /* Codes_SRS_BUFFER_01_004: [ BUFFER_prepend concatenates handle1 onto handle2 without modifying handle1 and shall return zero on success. ]*/
                    // Append the BUFFER
                    (void)memcpy(temp, b2->buffer, b2->size);
                    // start from b1->size to append b1
                    (void)memcpy(&temp[b2->size], b1->buffer, b1->size);
                    free(b1->buffer);
                    b1->buffer = temp;
                    b1->size += b2->size;
                    result = 0;
                }
            }
        }
    }
    return result;
}

int BUFFER_fill(BUFFER_HANDLE handle, unsigned char fill_char)
{
    int result;
    if (handle == NULL)
    {
        /* Codes_SRS_BUFFER_07_002: [ If handle is NULL BUFFER_fill shall return a non-zero value. ] */
        LogError("Invalid parameter specified, handle == NULL.");
        result = MU_FAILURE;
    }
    else
    {
        size_t index;
        /* Codes_SRS_BUFFER_07_001: [ BUFFER_fill shall fill the supplied BUFFER_HANDLE with the supplied fill character. ] */
        BUFFER* buffer_data = (BUFFER*)handle;
        for (index = 0; index < buffer_data->size; index++)
        {
            buffer_data->buffer[index] = fill_char;
        }
        result = 0;
    }
    return result;
}


/* Codes_SRS_BUFFER_07_025: [BUFFER_u_char shall return a pointer to the underlying unsigned char*.] */
unsigned char* BUFFER_u_char(BUFFER_HANDLE handle)
{
    BUFFER* handleData = (BUFFER*)handle;
    unsigned char* result;
    if (handle == NULL || handleData->size == 0)
    {
        /* Codes_SRS_BUFFER_07_026: [BUFFER_u_char shall return NULL for any error that is encountered.] */
        /* Codes_SRS_BUFFER_07_029: [BUFFER_u_char shall return NULL if underlying buffer size is zero.] */
        result = NULL;
    }
    else
    {
        result = handleData->buffer;
    }
    return result;
}

/* Codes_SRS_BUFFER_07_027: [BUFFER_length shall return the size of the underlying buffer.] */
size_t BUFFER_length(BUFFER_HANDLE handle)
{
    size_t result;
    if (handle == NULL)
    {
        /* Codes_SRS_BUFFER_07_028: [BUFFER_length shall return zero for any error that is encountered.] */
        result = 0;
    }
    else
    {
        BUFFER* b = (BUFFER*)handle;
        result = b->size;
    }
    return result;
}

BUFFER_HANDLE BUFFER_clone(BUFFER_HANDLE handle)
{
    BUFFER_HANDLE result;
    if (handle == NULL)
    {
        result = NULL;
    }
    else
    {
        BUFFER* suppliedBuff = (BUFFER*)handle;
        BUFFER* b = (BUFFER*)calloc(1, sizeof(BUFFER));
        if (b != NULL)
        {
            if (BUFFER_safemalloc(b, suppliedBuff->size) != 0)
            {
                free(b);
                LogError("Failure: allocating temp buffer.");
                result = NULL;
            }
            else
            {
                (void)memcpy(b->buffer, suppliedBuff->buffer, suppliedBuff->size);
                b->size = suppliedBuff->size;
                result = (BUFFER_HANDLE)b;
            }
        }
        else
        {
            result = NULL;
        }
    }
    return result;
}
