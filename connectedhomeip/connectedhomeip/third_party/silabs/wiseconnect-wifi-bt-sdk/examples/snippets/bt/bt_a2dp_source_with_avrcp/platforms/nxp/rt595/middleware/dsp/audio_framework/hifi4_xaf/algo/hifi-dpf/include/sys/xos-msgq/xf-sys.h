/*******************************************************************************
* Copyright (c) 2015-2019 Cadence Design Systems, Inc.
* 
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to use this Software with Cadence processor cores only and 
* not with any other processors and platforms, subject to
* the following conditions:
* 
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

******************************************************************************/
/*******************************************************************************
 * xf-sys.h
 *
 * Definitions for Xtensa SHMEM configuration
 *******************************************************************************/

#ifndef __XF_H
#error "xf-sys.h mustn't be included directly"
#endif

/*******************************************************************************
 * Standard system includes
 ******************************************************************************/

/* ...from directory ./board-BOARDNAME */
#include "xf-board.h"

/*******************************************************************************
 * Global abstractions
 ******************************************************************************/

/* ...prevent instructions reordering */
#define barrier()                           \
    __asm__ __volatile__("": : : "memory")

/* ...memory barrier */
#define XF_PROXY_BARRIER()                  \
    __asm__ __volatile__("memw": : : "memory")

/* ...memory invalidation */
#define XF_PROXY_INVALIDATE(buf, length)    \
    ({ if ((length)) { xthal_dcache_region_invalidate((buf), (length)); barrier(); } buf; })

/* ...memory flushing */
#define XF_PROXY_FLUSH(buf, length)         \
    ({ if ((length)) { barrier(); xthal_dcache_region_writeback((buf), (length)); XF_PROXY_BARRIER(); } buf; })

/*******************************************************************************
 * Core-specific data accessor
 ******************************************************************************/

/* ...per-core execution data */
extern xf_dsp_t *xf_g_dsp;

/* ...local memory accessor */
#define XF_CORE_DATA(core)      (&(xf_g_dsp->xf_core_data[core]))

/*******************************************************************************
 * Inter-processor communication and shared memory interface definition
 ******************************************************************************/

#include "xf-dp_ipc.h"
