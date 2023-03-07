/*
 * Copyright (c) 2015-2020, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/
/*
 *  ======== MemAlloc.c ========
 */

#if MemAlloc_generateFunctions_D

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/sysbios/runtime/Memory.h>
#include <ti/sysbios/runtime/Error.h>
#include <ti/sysbios/runtime/Types.h>

#include <string.h>

/*
 * Header is a union to make sure that the size is a power of 2.
 */
typedef union MemAlloc_Header {
    struct {
        void *   actualBuf;
        size_t size;
    } header;
    uintptr_t pad[2];	/* 4 words on 28L, 8 bytes on most others */
} MemAlloc_Header;

/*
 *  ======== MemAlloc_alloc ========
 */
static void *MemAlloc_alloc(size_t size)
{
    size_t allocSize;
    MemAlloc_Header *packet;

    if (size == 0) {
        return (NULL);
    }

    allocSize = size + sizeof(MemAlloc_Header);

    /*
     * If size is very large and allocSize overflows, the result will be
     * smaller than size. In this case, don't try to allocate.
     */
    if (allocSize < size) {
        return (NULL);
    }

    packet = (MemAlloc_Header *)Memory_alloc(NULL, allocSize, 0, Error_IGNORE);

    if (packet == NULL) {
        return (NULL);
    }

    packet->header.actualBuf = (void *)packet;
    packet->header.size = allocSize;

    return (packet + 1);
}

/*
 *  ======== malloc ========
 */
void *malloc(size_t size)
{
    return (MemAlloc_alloc(size));
}

/*
 *  ======== memalign ========
 *  mirrors the memalign() function from the TI run-time library
 */
void *memalign(size_t alignment, size_t size)
{
    MemAlloc_Header             *packet;
    void                        *tmp;

    /* return NULL if size is 0, or alignment is not a power-of-2 */
    if (size == 0 || (alignment & (alignment - 1))) {
        return (NULL);
    }

    if (alignment < sizeof(MemAlloc_Header)) {
        alignment = sizeof(MemAlloc_Header);
    }

    /*
     * If size is very large and alignment + size overflows, the result will be
     * smaller than size. In this case, don't try to allocate.
     */
    if ((alignment + size) < size) {
        return (NULL);
    }

    /*
     * Allocate 'align + size' so that we have room for the 'packet'
     * and can return an aligned buffer.
     */
    tmp = Memory_alloc(NULL, alignment + size, alignment, Error_IGNORE);

    if (tmp == NULL) {
        return (NULL);
    }

    packet = (MemAlloc_Header *)((char *)tmp + alignment - sizeof(MemAlloc_Header));

    packet->header.actualBuf = tmp;
    packet->header.size = size + sizeof(MemAlloc_Header);

    return (packet + 1);
}

/*
 *  ======== calloc ========
 */
void *calloc(size_t nmemb, size_t size)
{
    size_t       nbytes;
    void *         retval;

    nbytes = nmemb * size;

    /* return NULL if there's an overflow */
    if (nmemb && size != (nbytes / nmemb)) {
        return (NULL);
    }

    retval = MemAlloc_alloc(nbytes);
    if (retval != NULL) {
        (void)memset(retval, (int)'\0', nbytes);
    }

    return (retval);
}

/*
 *  ======== free ========
 */
void free(void *ptr)
{
    MemAlloc_Header      *packet;

    if (ptr != NULL) {
        packet = ((MemAlloc_Header *)ptr) - 1;

        Memory_free(NULL, (void *)packet->header.actualBuf,
            (packet->header.size +
            ((char*)packet - (char*)packet->header.actualBuf)));
    }
}

/*
 *  ======== realloc ========
 */
void *realloc(void *ptr, size_t size)
{
    void *         retval;
    MemAlloc_Header    *packet;
    size_t       oldSize;

    if (ptr == NULL) {
        retval = malloc(size);
    }
    else if (size == 0) {
        free(ptr);
        retval = NULL;
    }
    else {
        packet = (MemAlloc_Header *)ptr - 1;
        retval = malloc(size);
        if (retval != NULL) {
            oldSize = packet->header.size - sizeof(MemAlloc_Header);
            (void)memcpy(retval, ptr, (size < oldSize) ? size : oldSize);
            free(ptr);
        }
    }

    return (retval);
}

/*
 *  ======== aligned_alloc ========
 */
void *aligned_alloc(size_t alignment, size_t size)
{
    void *retval;

    retval = memalign(alignment, size);

    return (retval);
}

#if defined(__GNUC__) && !defined(__ti__)

#include <reent.h>

/*
 *  ======== _malloc_r ========
 */
void *_malloc_r(struct _reent *rptr, size_t size)
{
    return malloc(size);
}

/*
 *  ======== _calloc_r ========
 */
void *_calloc_r(struct _reent *rptr, size_t nmemb, size_t size)
{
    return calloc(nmemb, size);
}

/*
 *  ======== _free_r ========
 */
void _free_r(struct _reent *rptr, void *ptr)
{
    free(ptr);
}

/*
 *  ======== _realloc_r ========
 */
void *_realloc_r(struct _reent *rptr, void *ptr, size_t size)
{
    return realloc(ptr, size);
}

#endif /* defined(__GNUC__) && !define(__ti__) */

#endif /* BIOS_generateFunctions_D */
