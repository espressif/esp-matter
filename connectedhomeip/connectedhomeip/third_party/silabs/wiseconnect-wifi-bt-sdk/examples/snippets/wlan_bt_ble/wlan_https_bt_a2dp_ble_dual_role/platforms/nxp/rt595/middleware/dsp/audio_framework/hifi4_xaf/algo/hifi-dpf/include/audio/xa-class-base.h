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
 * xa-class-base.h
 *
 * Generic Xtensa Audio codecs interfaces
 ******************************************************************************/

#ifndef __XA_CLASS_BASE_H
#define __XA_CLASS_BASE_H

/*******************************************************************************
 * Includes
 ******************************************************************************/

/* ...audio-specific API */
#include "xaf-api.h"
#include "xa_type_def.h"
#include "xa_error_standards.h"
#include "xa_apicmd_standards.h"
#include "xa_memory_standards.h"

/*******************************************************************************
 * Generic codec structure
 ******************************************************************************/

typedef struct XACodecBase XACodecBase;

/* ...memory buffer initialization */
typedef XA_ERRORCODE  (*xa_codec_memtab_f)(XACodecBase *codec, WORD32 i, WORD32 type, WORD32 size, WORD32 align, UWORD32 core);

/* ...preprocessing operation */
typedef XA_ERRORCODE  (*xa_codec_preprocess_f)(XACodecBase *);

/* ...postprocessing operation */
typedef XA_ERRORCODE  (*xa_codec_postprocess_f)(XACodecBase *, int);

/* ...parameter setting function */
typedef XA_ERRORCODE  (*xa_codec_setparam_f)(XACodecBase *, WORD32, pVOID p);

/* ...parameter retrival function */
typedef XA_ERRORCODE  (*xa_codec_getparam_f)(XACodecBase *, WORD32, pVOID p);

/*******************************************************************************
 * Codec instance structure
 ******************************************************************************/

struct XACodecBase
{
    /***************************************************************************
     * Control data
     **************************************************************************/

    /* ...generic component handle */
    xf_component_t          component;

    /* ...codec API entry point (function) */
    xa_codec_func_t        *process;

    /* ...codec API handle, passed to *process */
    xf_mm_buffer_t          api;

    /* ...memory table buffer */
    xf_mm_buffer_t          mem_tabs;

    /* ...persistent memory buffer */
    xf_mm_buffer_t          persist;

    /* ...scratch memory pointer */
    void                   *scratch;

    /* ...codec control state */
    UWORD32                     state;

    /***************************************************************************
     * Codec-specific methods
     **************************************************************************/

    /* ...memory buffer initialization */
    xa_codec_memtab_f       memtab;
 
    /* ...preprocessing function */
    xa_codec_preprocess_f   preprocess;

    /* ...postprocessing function */
    xa_codec_postprocess_f  postprocess;

    /* ...configuration parameter setting function */
    xa_codec_setparam_f     setparam;

    /* ...configuration parameter retrieval function */
    xa_codec_getparam_f     getparam;
    
    /* ...command-processing table */
    XA_ERRORCODE (* const * command)(XACodecBase *, xf_message_t *);
    
    /* ...command-processing table size */
    UWORD32                     command_num;

    /* ...component type */
    xaf_comp_type            comp_type;
};

/*******************************************************************************
 * Base codec execution flags
 ******************************************************************************/

/* ...codec static initialization completed */
#define XA_BASE_FLAG_POSTINIT           (1 << 0)

/* ...codec runtime initialization sequence */
#define XA_BASE_FLAG_RUNTIME_INIT       (1 << 1)

/* ...codec steady execution state */
#define XA_BASE_FLAG_EXECUTION          (1 << 2)

/* ...execution stage completed */
#define XA_BASE_FLAG_COMPLETED          (1 << 3)

/* ...data processing scheduling flag */
#define XA_BASE_FLAG_SCHEDULE           (1 << 4)

/* ...base codec flags accessor */
#define __XA_BASE_FLAGS(flags)          ((flags) & ((1 << 5) - 1))

/* ...custom execution flag */
#define __XA_BASE_FLAG(f)               ((f) << 5)

/*******************************************************************************
 * Local macros definitions
 ******************************************************************************/

/* ...audio-framework API function execution */
#define XA_CHK(cond)                            \
({                                              \
    XA_ERRORCODE  __e = (cond);                 \
    if (__e != XA_NO_ERROR)                     \
    {                                           \
        if (XA_ERROR_SEVERITY(__e))             \
        {                                       \
            TRACE(ERROR, _x("error: %X"), __e); \
            return __e;                         \
        }                                       \
        TRACE(WARNING, _x("warning: %X"), __e); \
    }                                           \
    __e;                                        \
})

/* ...low-level codec API function execution */
#define XA_API(codec, cmd, idx, pv)                                                         \
({                                                                                          \
    XA_ERRORCODE  __e;                                                                      \
    __e = (codec)->process((xa_codec_handle_t)(codec)->api.addr, (cmd), (idx), (pv));       \
    if (__e != XA_NO_ERROR)                                                                 \
    {                                                                                       \
        if (XA_ERROR_SEVERITY(__e))                                                         \
        {                                                                                   \
            TRACE(ERROR, _x("[%p]:(%d, %d, %p): %X"), (codec), (cmd), (idx), (pv), __e);    \
            return __e;                                                                     \
        }                                                                                   \
        TRACE(WARNING, _x("%X"), __e);                                                      \
    }                                                                                       \
    __e;                                                                                    \
})

#define XA_API_NORET(codec, cmd, idx, pv)                                                   \
({                                                                                          \
    XA_ERRORCODE  __e;                                                                      \
    __e = (codec)->process((xa_codec_handle_t)(codec)->api.addr, (cmd), (idx), (pv));       \
    if (__e != XA_NO_ERROR)                                                                 \
    {                                                                                       \
        if (XA_ERROR_SEVERITY(__e))                                                         \
        {                                                                                   \
            TRACE(ERROR, _x("[%p]:(%d, %d, %p): %X"), (codec), (cmd), (idx), (pv), __e);    \
        }                                                                                   \
        TRACE(WARNING, _x("%X"), __e);                                                      \
    }                                                                                       \
    __e;                                                                                    \
})

/* ...codec hook invocation */
#define CODEC_API(codec, func, ...)                                 \
({                                                                  \
    XA_ERRORCODE    __e = (codec)->func((codec), ##__VA_ARGS__);    \
                                                                    \
    if (__e != XA_NO_ERROR)                                         \
    {                                                               \
        if (XA_ERROR_SEVERITY(__e))                                 \
        {                                                           \
            /* ...actual error is reported by the codec */          \
            TRACE(ERROR, _x("[%p]: " #func ": %X"), (codec), __e);  \
            return __e;                                             \
        }                                                           \
                                                                    \
        TRACE(WARNING, _x("warning: %X"), __e);                     \
    }                                                               \
    __e;                                                            \
})

/* ...allocate local memory on specific core */
#define XMALLOC(p, size, align, core)                                           \
do                                                                              \
{                                                                               \
    if (xf_mm_alloc_buffer((size), (align), (core), (p)) != 0)                  \
    {                                                                           \
        TRACE(ERROR, _x("Failed to allocate %d bytes of memory"), (size));      \
        return XA_API_FATAL_MEM_ALLOC;                                          \
    }                                                                           \
                                                                                \
    if (((UWORD32)((p)->addr) & ((align) - 1)) != 0)                                \
    {                                                                           \
        TRACE(ERROR, _x("Invalid %d-algnment: %p"), (align), (p)->addr);        \
        return XA_API_FATAL_MEM_ALIGN;                                          \
    }                                                                           \
}                                                                               \
while (0)

/*******************************************************************************
 * Public API
 ******************************************************************************/

/* ...SET-PARAM processing */
extern XA_ERRORCODE xa_base_set_param(XACodecBase *base, xf_message_t *m);

/* ...GET-PARAM-EXT message processing */
extern XA_ERRORCODE xa_base_set_param_ext(XACodecBase *base, xf_message_t *m);

/* ...GET-PARAM message processing */
extern XA_ERRORCODE xa_base_get_param(XACodecBase *base, xf_message_t *m);

/* ...GET-PARAM-EXT message processing */
extern XA_ERRORCODE xa_base_get_param_ext(XACodecBase *base, xf_message_t *m);

/* ...data processing scheduling */
extern void xa_base_schedule(XACodecBase *base, UWORD32 dts);

/* ...cancel internal scheduling message */
extern void xa_base_cancel(XACodecBase *base);

/* ...base codec factory */
extern XACodecBase * xa_base_factory(UWORD32 core, UWORD32 size, xa_codec_func_t process);

/* ...base codec destructor */
extern void xa_base_destroy(XACodecBase *base, UWORD32 size, UWORD32 core);

#endif  /* __XA_CLASS_BASE_H */
