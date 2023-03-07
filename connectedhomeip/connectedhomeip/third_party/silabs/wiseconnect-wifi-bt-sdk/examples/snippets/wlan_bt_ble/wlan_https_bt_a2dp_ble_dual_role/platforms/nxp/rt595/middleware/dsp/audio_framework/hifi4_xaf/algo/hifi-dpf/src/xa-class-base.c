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
 * xa-class-base.c
 *
 * Generic audio codec task implementation
 ******************************************************************************/

#define MODULE_TAG                      BASE

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "xf-dp.h"
#include "xa-class-base.h"

/*******************************************************************************
 * Tracing configuration
 ******************************************************************************/

TRACE_TAG(INIT, 1);
TRACE_TAG(WARNING, 1);
TRACE_TAG(SETUP, 1);
TRACE_TAG(EXEC, 1);

/*******************************************************************************
 * Internal functions definitions
 ******************************************************************************/

/* ...codec pre-initialization */
static XA_ERRORCODE xa_base_preinit(XACodecBase *base, UWORD32 core)
{
    WORD32      n;

    /* ...codec must be empty */
    XF_CHK_ERR(base->state == 0, XA_API_FATAL_INVALID_CMD);

    /* ...get API structure size */
    XA_API(base, XA_API_CMD_GET_API_SIZE, 0, &n);

    /* ...allocate memory for codec API structure (4-bytes aligned) */
    XMALLOC(&base->api, n, 4, core);

    /* ...set default config parameters */
    XA_API(base, XA_API_CMD_INIT, XA_CMD_TYPE_INIT_API_PRE_CONFIG_PARAMS, NULL);

    /* ...get memory info tables size */
    if (XA_API(base, XA_API_CMD_GET_MEMTABS_SIZE, 0, &n), n != 0)
    {
        /* ...allocate memory for tables (4-bytes aligned) */
        XMALLOC(&base->mem_tabs, n, 4, core);

        /* ...set pointer for process memory tables */
        XA_API(base, XA_API_CMD_SET_MEMTABS_PTR, 0, base->mem_tabs.addr);
    }
    
    TRACE(INIT, _b("Codec[%p] pre-initialization completed"), base);
    
    return XA_NO_ERROR;
}

/* ...post-initialization setup */
static XA_ERRORCODE xa_base_postinit(XACodecBase *base, UWORD32 core)
{
    WORD32  n, i;
    
    /* ...issue post-config command and determine the buffer requirements */
    XA_API(base, XA_API_CMD_INIT, XA_CMD_TYPE_INIT_API_POST_CONFIG_PARAMS, NULL);

    /* ...get number of memory tables required */
    XA_API(base, XA_API_CMD_GET_N_MEMTABS, 0, &n);

    /* ...set scratch buffer in advance (as codec not necessarily exposes it) */
    base->scratch = XF_CORE_DATA(core)->scratch;

    /* ...allocate memory buffers */
    for (i = 0; i < n; i++)
    {
        WORD32      size, align, type;

        TRACE(1, _b("i = %u (of %u)"), (UWORD32)i, (UWORD32)n);
        
        /* ...get memory type */
        XA_API(base, XA_API_CMD_GET_MEM_INFO_TYPE, i, &type);

        /* ...get memory size of i-th buffer */
        XA_API(base, XA_API_CMD_GET_MEM_INFO_SIZE, i, &size);

        /* ...get alignment */
        XA_API(base, XA_API_CMD_GET_MEM_INFO_ALIGNMENT, i, &align);

        /* ...process individual buffer */
        switch (type)
        {
        case XA_MEMTYPE_SCRATCH:
            /* ...scratch memory is shared among all codecs; check its validity */
            XF_CHK_ERR(size <= XF_CFG_CODEC_SCRATCHMEM_SIZE, XA_API_FATAL_MEM_ALLOC);

            /* ...make sure alignment is sane */
            XF_CHK_ERR((XF_CFG_CODEC_SCRATCHMEM_ALIGN & (align - 1)) == 0, XA_API_FATAL_MEM_ALIGN);

            /* ...set the scratch memory pointer */
            XA_API(base, XA_API_CMD_SET_MEM_PTR, i, base->scratch);

            TRACE(INIT, _b("Mem tab %d: sz=%d al=%d ty=%d Scratch memory (%p)"), i, size, align, type, base->scratch);

            break;

        case XA_MEMTYPE_PERSIST:
            /* ...allocate persistent memory */
            XMALLOC(&base->persist, size, align, core);

            /* ...and set the pointer instantly */
            XA_API(base, XA_API_CMD_SET_MEM_PTR, i, base->persist.addr);

            TRACE(INIT, _b("Mem tab %d: sz=%d al=%d ty=%d Persistent memory (%p)"), i, size, align, type, base->persist.addr);

            break;

        case XA_MEMTYPE_INPUT:
        case XA_MEMTYPE_OUTPUT:
            /* ...input/output buffer specification; pass to codec function */
            CODEC_API(base, memtab, i, type, size, align, core);

            break;

        default:
            /* ...unrecognized memory type */
            TRACE(ERROR, _x("Invalid memory type: [%d]=(%u, %u, %u)"), i, type, size, align);
            return XA_API_FATAL_INVALID_CMD_TYPE;
        }
    }

    TRACE(INIT, _b("Codec[%p] post-initialization completed (api:%p[%u])"), base, base->api.addr, base->api.size);

    return XA_NO_ERROR;
}

/*******************************************************************************
 * Commands processing
 ******************************************************************************/

/* ...SET-PARAM processing (enabled in all states) */
XA_ERRORCODE xa_base_set_param(XACodecBase *base, xf_message_t *m)
{
    xf_set_param_msg_t     *cmd = m->buffer;
    xf_set_param_item_t    *param = &cmd->item[0];
    WORD32                  n, i;

    /* ...calculate total amount of parameters */
    n = m->length / sizeof(*param);

    /* ...check the message length is sane */
    XF_CHK_ERR(m->length == XF_SET_PARAM_CMD_LEN(n), XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...apply all parameters; pass to codec-specific function */
    for (i = 0; i < n; i++)
    {
        TRACE(SETUP, _b("set-param[%p]: [%u]=%u"), base, param[i].id, param[i].value);

        if (base->setparam)
        {
            CODEC_API(base, setparam, param[i].id, &param[i].value);
        }
        else
        {
            XA_API(base, XA_API_CMD_SET_CONFIG_PARAM, param[i].id, &param[i].value);
        }
    }

    /* ...check if we need to do post-initialization */
    if ((base->state & XA_BASE_FLAG_POSTINIT) == 0)
    {
        /* ...do post-initialization step */
        XA_CHK(xa_base_postinit(base, XF_MSG_DST_CORE(m->id)));
        
        /* ...mark the codec static configuration is set */
        base->state ^= XA_BASE_FLAG_POSTINIT | XA_BASE_FLAG_RUNTIME_INIT;
    }
    
    /* ...complete message processing; output buffer is empty */
    xf_response_ok(m);

    return XA_NO_ERROR;
}

/* ...GET-PARAM message processing (enabled in all states) */
XA_ERRORCODE xa_base_get_param(XACodecBase *base, xf_message_t *m)
{
    xf_get_param_msg_t *cmd = m->buffer;
    UWORD32                *id = &cmd->c.id[0];
    UWORD32                *value = &cmd->r.value[0];
    UWORD32                 n, i;

    /* ...calculate amount of parameters */
    n = m->length / sizeof(*id);

    /* ...check input parameter length */
    XF_CHK_ERR(XF_GET_PARAM_CMD_LEN(n) == m->length, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...retrieve the collection of codec  parameters */
    for (i = 0; i < n; i++)
    {
        /* ...place the result into same location */
        if (base->getparam)
        {
            CODEC_API(base, getparam, id[i], &value[i]);
        }
        else
        {
            XA_API(base, XA_API_CMD_GET_CONFIG_PARAM, id[i], &value[i]);
        }
    }

    /* ...complete message specifying output buffer size */
    xf_response_data(m, XF_GET_PARAM_RSP_LEN(n));

    return XA_NO_ERROR;
}

/* ...SET-PARAM-EXT processing (enabled in all states) */
XA_ERRORCODE xa_base_set_param_ext(XACodecBase *base, xf_message_t *m)
{
    xf_ext_param_msg_t *cmd = m->buffer;
    UWORD16                 length = m->length;
    UWORD16                 remaining = (length + 3) & ~3;
    UWORD16                 i;
    
    for (i = 0; TRACE_CFG(SETUP) && i < remaining; i += 16)
    {
        TRACE(SETUP, _b("[%03x]: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X"),
              i, 
              ((UWORD8 *)m->buffer)[i + 0], ((UWORD8 *)m->buffer)[i + 1],
              ((UWORD8 *)m->buffer)[i + 2], ((UWORD8 *)m->buffer)[i + 3],
              ((UWORD8 *)m->buffer)[i + 4], ((UWORD8 *)m->buffer)[i + 5],
              ((UWORD8 *)m->buffer)[i + 6], ((UWORD8 *)m->buffer)[i + 7],
              ((UWORD8 *)m->buffer)[i + 8], ((UWORD8 *)m->buffer)[i + 9],
              ((UWORD8 *)m->buffer)[i + 10], ((UWORD8 *)m->buffer)[i + 11],
              ((UWORD8 *)m->buffer)[i + 12], ((UWORD8 *)m->buffer)[i + 13],
              ((UWORD8 *)m->buffer)[i + 14], ((UWORD8 *)m->buffer)[i + 15]);
    }    

    /* ...process all parameters encapsulated in buffer */
    while (remaining >= sizeof(*cmd))
    {
        UWORD16     id = cmd->desc.id;
        UWORD16     dlen = cmd->desc.length;
        UWORD16     dsize = (dlen + 3) & ~3;
        UWORD16     pad = dlen & 3;

        /* ...cut-off descriptor header */
        remaining -= sizeof(*cmd);

        TRACE(SETUP, _b("remaining:%u, desc_size:%u"), (UWORD32)remaining, (UWORD32)dsize);
        
        /* ...make sure length is sufficient */        
        XF_CHK_ERR(remaining >= dsize, XA_API_FATAL_INVALID_CMD_TYPE);
        
        /* ...pad remaining bytes with zeroes */
        (pad ? memset(cmd->data + dlen, 0, 4 - pad) : 0);

        TRACE(SETUP, _b("set-ext-param[%p]: [%u]:%u - [%02X:%02X:%02X:%02X:...]"), base, id, dsize, cmd->data[0], cmd->data[1], cmd->data[2], cmd->data[3]);

        /* ...apply parameter */
        XA_API(base, XA_API_CMD_SET_CONFIG_PARAM, id, cmd->data);

        /* ...move to next item (keep 4-bytes alignment for descriptor) */
        cmd = (xf_ext_param_msg_t *)(&cmd->data[0] + dsize), remaining -= dsize;
    }
    
    /* ...check the message is fully processed */
    XF_CHK_ERR(remaining == 0, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...complete message processing; output buffer is empty */
    //xf_response_ok(m);

    /* ...unfortunately, it looks like a bug of the library that updates the memory
     * and leaves it in a dirty state causing subsequent cache inconsistency - tbd
     */
    xf_response_data(m, length);

    return XA_NO_ERROR;
}

/* ...GET-PARAM-EXT message processing (enabled in all states) */
XA_ERRORCODE xa_base_get_param_ext(XACodecBase *base, xf_message_t *m)
{
    xf_ext_param_msg_t *cmd = m->buffer;
    UWORD32                 length = m->length;
    UWORD32                 remaining = (length + 3) & ~3;
    int                 i;

    for (i = 0; TRACE_CFG(SETUP) && i < remaining; i += 16)
    {
        TRACE(SETUP, _b("[%03x]: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X"),
              i, 
              ((UWORD8 *)m->buffer)[i + 0], ((UWORD8 *)m->buffer)[i + 1],
              ((UWORD8 *)m->buffer)[i + 2], ((UWORD8 *)m->buffer)[i + 3],
              ((UWORD8 *)m->buffer)[i + 4], ((UWORD8 *)m->buffer)[i + 5],
              ((UWORD8 *)m->buffer)[i + 6], ((UWORD8 *)m->buffer)[i + 7],
              ((UWORD8 *)m->buffer)[i + 8], ((UWORD8 *)m->buffer)[i + 9],
              ((UWORD8 *)m->buffer)[i + 10], ((UWORD8 *)m->buffer)[i + 11],
              ((UWORD8 *)m->buffer)[i + 12], ((UWORD8 *)m->buffer)[i + 13],
              ((UWORD8 *)m->buffer)[i + 14], ((UWORD8 *)m->buffer)[i + 15]);
    }
    
    /* ...process all parameters encapsulated in buffer */
    while (remaining >= sizeof(*cmd))
    {
        UWORD16     id = cmd->desc.id;
        UWORD16     len = cmd->desc.length;
        UWORD16     size = (len + 3) & ~3;
        UWORD8      pad = len & 3;

        /* ...cut-off command header */
        remaining -= sizeof(*cmd);

        /* ...make sure data buffer has sufficient length */
        XF_CHK_ERR(remaining >= size, XA_API_FATAL_INVALID_CMD_TYPE);
        
        /* ...retrieve parameter from buffer (care about alignment? - tbd) */
        XA_API(base, XA_API_CMD_GET_CONFIG_PARAM, id, cmd->data);

        /* ...pad remaininig bytes with zeroes */
        (pad ? memset(cmd->data + len, 0, 4 - pad) : 0);

        TRACE(SETUP, _b("get-ext-param[%p]: [%u]:%u - [%02X:%02X:%02X:%02X:...]"), base, id, size, cmd->data[0], cmd->data[1], cmd->data[2], cmd->data[3]);
        
        /* ...move to next item (alignment issues? - tbd) */
        cmd = (xf_ext_param_msg_t *)(&cmd->data[0] + size), remaining -= size;
    }
    
    /* ...check the message is fully processed */
    XF_CHK_ERR(remaining == 0, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...complete message processing; output buffer has the same length */
    xf_response_data(m, length);

    return XA_NO_ERROR;
}

/*******************************************************************************
 * Command/data processing functions
 ******************************************************************************/

/* ...generic codec data processing */
static XA_ERRORCODE xa_base_process(XACodecBase *base)
{
    XA_ERRORCODE    error;
    WORD32          done=0;

    /* ...clear internal scheduling flag */
    base->state &= ~XA_BASE_FLAG_SCHEDULE;
        
    /* ...codec-specific preprocessing (buffer maintenance) */
    if ((error = CODEC_API(base, preprocess)) != XA_NO_ERROR)
    {
        /* ...return non-fatal codec error */
        return error;
    }

    /* ...execution step */
    if (base->state & XA_BASE_FLAG_RUNTIME_INIT)
    {
        /* ...kick initialization process */
        XA_API(base, XA_API_CMD_INIT, XA_CMD_TYPE_INIT_PROCESS, NULL);

        /* ...check if initialization is completed */
        XA_API(base, XA_API_CMD_INIT, XA_CMD_TYPE_INIT_DONE_QUERY, &done);

        TRACE(EXEC, _b("Initialization result: %d"), done);

        /* ...switch to execution state if required */
        (done ? base->state ^= XA_BASE_FLAG_RUNTIME_INIT | XA_BASE_FLAG_EXECUTION : 0);
    }
    else if (base->state & XA_BASE_FLAG_EXECUTION)
    {
        TRACE(1, _b("do exec"));
        
        /* ...execute decoding process */
        XA_API(base, XA_API_CMD_EXECUTE, XA_CMD_TYPE_DO_EXECUTE, NULL);

        /* ...check for end-of-stream condition */
        XA_API(base, XA_API_CMD_EXECUTE, XA_CMD_TYPE_DONE_QUERY, &done);

        TRACE(EXEC, _b("Execution result: %d"), done);

        /* ...mark the output path is done to release all queued buffers */
        (done ? base->state ^= XA_BASE_FLAG_EXECUTION | XA_BASE_FLAG_COMPLETED : 0);
    }

    /* ...codec-specific buffer post-processing */
    return CODEC_API(base, postprocess, done);
}
                              
/* ...message-processing function (component entry point) */
static int xa_base_command(xf_component_t *component, xf_message_t *m)
{
    XACodecBase    *base = (XACodecBase *) component;
    UWORD32             cmd;
    
    /* ...invoke data-processing function if message is null */
    if (m == NULL)
    {
        XF_CHK_ERR(!XA_ERROR_SEVERITY(xa_base_process(base)), -EPIPE);
        return 0;
    }

    /* ...process the command */
    TRACE(EXEC, _b("[%p]:state[%X]:(%X, %d, %p)"), base, base->state, m->opcode, m->length, m->buffer);

    /* ...bail out if this is forced termination command (I do have a map; maybe I'd better have a hook? - tbd) */
    if ((cmd = XF_OPCODE_TYPE(m->opcode)) == XF_OPCODE_TYPE(XF_UNREGISTER))
    {
        TRACE(INIT, _b("force component[%p] termination"), base);
        return -1;    
    }
    
    /* ...check opcode is valid */
    XF_CHK_ERR(cmd < base->command_num, -EINVAL);

    /* ...and has a hook */
    XF_CHK_ERR(base->command[cmd] != NULL, -EINVAL);
    
    /* ...pass control to specific command */
    XF_CHK_ERR(!XA_ERROR_SEVERITY(base->command[cmd](base, m)), -EPIPE);

    /* ...execution completed successfully */
    return 0;
}

/*******************************************************************************
 * Base codec API
 ******************************************************************************/

/* ...data processing scheduling */
void xa_base_schedule(XACodecBase *base, UWORD32 dts)
{
    if ((base->state & XA_BASE_FLAG_SCHEDULE) == 0)
    {
        /* ...schedule component task execution */
        xf_component_schedule(&base->component, dts);

        /* ...and put scheduling flag */
        base->state ^= XA_BASE_FLAG_SCHEDULE;
    }
    else
    {
        TRACE(EXEC, _b("codec[%p] processing pending"), base);
    }
}

/* ...cancel data processing */
void xa_base_cancel(XACodecBase *base)
{
    if (base->state & XA_BASE_FLAG_SCHEDULE)
    {
        /* ...cancel scheduled codec task */
        xf_component_cancel(&base->component);

        /* ...and clear scheduling flag */
        base->state ^= XA_BASE_FLAG_SCHEDULE;

        TRACE(EXEC, _b("codec[%p] processing cancelled"), base);
    }
}

/* ...base codec destructor */
void xa_base_destroy(XACodecBase *base, UWORD32 size, UWORD32 core)
{
    /* ...deallocate all resources */
    xf_mm_free_buffer(&base->persist, core);
    xf_mm_free_buffer(&base->mem_tabs, core);
    xf_mm_free_buffer(&base->api, core);

    /* ...destroy codec structure (and task) itself */
    xf_mem_free(base, size, core, 0);

    TRACE(INIT, _b("codec[%p]:%u destroyed"), base, core);
}

/* ...generic codec initialization routine */
XACodecBase * xa_base_factory(UWORD32 core, UWORD32 size, xa_codec_func_t process)
{
    XACodecBase    *base;

    /* ...make sure the size is sane */
    XF_CHK_ERR(size >= sizeof(XACodecBase), NULL);
    
    /* ...allocate local memory for codec structure */
    XF_CHK_ERR(base = xf_mem_alloc(size, 0, core, 0), NULL);
 
    /* ...reset codec memory */
    memset(base, 0, size);

    /* ...set low-level codec API function */
    base->process = process;

    /* ...set message processing function */
    base->component.entry = xa_base_command;

    /* ...do basic initialization */
    if (xa_base_preinit(base, core) != XA_NO_ERROR)
    {
        /* ...initialization failed for some reason; do cleanup */
        xa_base_destroy(base, size, core);

        return NULL;
    }   

    /* ...initialization completed successfully */
    TRACE(INIT, _b("Codec[%p]:%u initialized"), base, core);
    
    return base;
}
