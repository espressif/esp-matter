/* 
 * Copyright (c) 2020, Texas Instruments Incorporated - http://www.ti.com
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
/*!
 * @file ti/sysbios/heaps/HeapNull.h
 * @brief Empty heap
 */

/*
 * ======== HeapNull.h ========
 */

#ifndef HeapNull__include
#define HeapNull__include

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/sysbios/knl/Queue.h>

#include <ti/sysbios/runtime/Error.h>
#include <ti/sysbios/runtime/IHeap.h>
#include <ti/sysbios/runtime/Memory.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
} HeapNull_Params;

typedef struct {
    IHeap_Object iheap;
} HeapNull_Struct, *HeapNull_Handle;

typedef HeapNull_Struct HeapNull_Object;

/*!
 * @brief cast handle to an IHeap_Handle for use by Memory_alloc, etc.
 *
 * @param handle heap handle
 *
 * @retval IHeap_Handle
 */
#define HeapNull_Handle_upCast(handle) ((IHeap_Handle)(handle))

extern void * HeapNull_alloc(HeapNull_Handle handle, size_t size, size_t align, Error_Block *eb);
extern void HeapNull_free(HeapNull_Handle handle, void * buf, size_t size);
extern bool HeapNull_isBlocking(HeapNull_Handle handle);
extern void HeapNull_getStats(HeapNull_Handle handle, Memory_Stats *statBuf);

/*!
 * @brief Create a `HeapNull` heap
 */
extern HeapNull_Handle HeapNull_create(const HeapNull_Params *params, Error_Block *eb);
extern HeapNull_Handle HeapNull_construct(HeapNull_Struct *obj, const HeapNull_Params *params);

extern void HeapNull_delete(HeapNull_Handle *handle);
extern void HeapNull_destruct(HeapNull_Struct *obj);

extern void HeapNull_init(void);

extern void HeapNull_Params_init(HeapNull_Params *prms);

static inline HeapNull_Handle HeapNull_handle(HeapNull_Struct *str)
{  
    return ((HeapNull_Handle)str);
}

static inline HeapNull_Struct * HeapNull_struct(HeapNull_Handle h)
{
    return ((HeapNull_Struct *)h);
}

#ifdef __cplusplus
}
#endif

#endif
