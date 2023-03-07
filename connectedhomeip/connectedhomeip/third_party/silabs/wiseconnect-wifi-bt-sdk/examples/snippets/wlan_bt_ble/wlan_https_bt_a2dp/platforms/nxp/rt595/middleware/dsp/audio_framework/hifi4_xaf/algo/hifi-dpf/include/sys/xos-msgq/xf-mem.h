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
 * xf-mem.h
 *
 * System-specific memory allocator
 *******************************************************************************/

#ifndef __XF_H
#error "xf-mem.h mustn't be included directly"
#endif

/*******************************************************************************
 * System specific memory pools
 ******************************************************************************/

#if XF_CFG_CORES_NUM > 1
/* ...shared memory pool for communication within DSP-cluster */
extern xf_mm_pool_t     xf_dsp_shmem_pool;
#endif

/*******************************************************************************
 * Platform-specific SHMEM allocation registering functions
 ******************************************************************************/

/* ...register shmem allocation address */
static inline void xf_shmem_alloc_addref(UWORD32 core, xf_message_t *m)
{
}

/* ...unregister shmem allocation address */
static inline void xf_shmem_alloc_rmref(UWORD32 core, xf_message_t *m)
{
}

/*******************************************************************************
 * API functions
 ******************************************************************************/

/* ...allocate aligned memory on particular core specifying if it is shared */
static inline void * xf_mem_alloc(UWORD32 size, UWORD32 align, UWORD32 core, UWORD32 shared)
{
#if XF_CFG_CORES_NUM > 1    
    if (shared)
    {
        /* ...if memory is shared, core is dropped */
        return xf_mm_alloc(&xf_dsp_shmem_pool, size);
    }
#endif
    
    /* ...select local memory pool basing on core specification */
    return xf_mm_alloc(&XF_CORE_DATA(core)->local_pool, size);
}

/* ...release allocated memory */
static inline void xf_mem_free(void *p, UWORD32 size, UWORD32 core, UWORD32 shared)
{
#if XF_CFG_CORES_NUM > 1    
    if (shared)
    {
        /* ...if memory is shared, core is dropped */
        xf_mm_free(&xf_dsp_shmem_pool, p, size);
        return;
    }
#endif
    
    /* ...select proper pool basing on core specification */
    xf_mm_free(&XF_CORE_DATA(core)->local_pool, p, size);
}

/* ...allocate AP-DSP shared memory */
static inline int xf_shmem_alloc(UWORD32 core, xf_message_t *m)
{
    xf_mm_pool_t   *pool = &XF_CORE_DATA(core)->shared_pool;

    /* ...length is always cache-line aligned */    
    if ((m->buffer = xf_mm_alloc(pool, XF_ALIGNED(m->length))) != NULL)
    {
        /* ...register allocation address */
        xf_shmem_alloc_addref(core, m);

        return 0;
    }
    else
    {
        return -ENOMEM;
    }
}

/* ...free AP-DSP shared memory */
static inline void xf_shmem_free(UWORD32 core, xf_message_t *m)
{
    xf_mm_pool_t   *pool = &XF_CORE_DATA(core)->shared_pool;

    /* ...length is always cache-line aligned */
    xf_mm_free(pool, m->buffer, XF_ALIGNED(m->length));

    /* ...unregister allocation address */
    xf_shmem_alloc_rmref(core, m);
}

/*******************************************************************************
 * Scratch memory management
 ******************************************************************************/

static inline void * xf_scratch_mem_init(UWORD32 core)
{
    /* ...allocate scratch memory from local DSP memory */
    return xf_mem_alloc(XF_CFG_CODEC_SCRATCHMEM_SIZE, XF_CFG_CODEC_SCRATCHMEM_ALIGN, core, 0);
}

/*******************************************************************************
 * Helpers - hmm; they are platform-independent - tbd
 ******************************************************************************/

/* ...allocate local buffer */
static inline int xf_mm_alloc_buffer(UWORD32 size, UWORD32 align, UWORD32 core, xf_mm_buffer_t *b)
{
    /* ...allocate memory from proper local pool */
    if ((size = XF_MM(size)) != 0)
        XF_CHK_ERR(b->addr = xf_mem_alloc(size, align, core, 0), -ENOMEM);
    else
        b->addr = NULL;

    /* ...save address */
    b->size = size;
    
    return 0;
}

/* ...free local buffer */
static inline void  xf_mm_free_buffer(xf_mm_buffer_t *b, UWORD32 core)
{
    if (b->addr)
    {
        xf_mem_free(b->addr, b->size, core, 0);
    }
}
