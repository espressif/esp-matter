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
 * xa-class-renderer.c
 *
 * Generic audio renderer component class
 *
 * Copyright (c) 2013 Tensilica Inc. ALL RIGHTS RESERVED.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 ******************************************************************************/
 #define MODULE_TAG                      RENDERER


/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "xf-dp.h"
#include "xa-class-base.h"
#include "audio/xa-renderer-api.h"

/*******************************************************************************
 * Tracing tags
 ******************************************************************************/

TRACE_TAG(INIT, 1);
TRACE_TAG(WARNING, 1);
TRACE_TAG(INFO, 1);
TRACE_TAG(INPUT, 1);

/*******************************************************************************
 * Data structures
 ******************************************************************************/

/* ...renderer data */
typedef struct XARenderer
{
    /***************************************************************************
     * Control data
     **************************************************************************/

    /* ...generic audio codec data */
    XACodecBase         base;

    /* ...input port */
    xf_input_port_t     input;

    /* ...buffer completion hook */
    xa_renderer_cb_t    cdata;

    /* ...output ready message */
    xf_message_t        msg;

    /***************************************************************************
     * Run-time configuration parameters
     **************************************************************************/

    /* ...time conversion factor (input byte "duration" in timebase units) */
    UWORD32                 factor;

    /* ...internal message scheduling flag (shared with interrupt) */
    UWORD32                 schedule;

}   XARenderer;

/*******************************************************************************
 * Renderee flags
 ******************************************************************************/

/* ...rendering is performed */
#define XA_RENDERER_FLAG_RUNNING        __XA_BASE_FLAG(1 << 0)

/* ...renderer is idle and produces silence */
#define XA_RENDERER_FLAG_SILENCE        __XA_BASE_FLAG(1 << 1)

/* ...ouput data is ready */
#define XA_RENDERER_FLAG_OUTPUT_READY   __XA_BASE_FLAG(1 << 2)

/*******************************************************************************
 * Internal helpers
 ******************************************************************************/

/* ...prepare renderer for steady operation */
static inline XA_ERRORCODE xa_renderer_prepare_runtime(XARenderer *renderer)
{
    XACodecBase    *base = (XACodecBase *) renderer;
    UWORD32             sample_rate;
    UWORD32             channels;
    UWORD32             pcm_width;
    UWORD32             sample_size;
    UWORD32             factor;

    /* ...query renderer parameters */
    XA_API(base, XA_API_CMD_GET_CONFIG_PARAM, XA_RENDERER_CONFIG_PARAM_SAMPLE_RATE, &sample_rate);
    XA_API(base, XA_API_CMD_GET_CONFIG_PARAM, XA_RENDERER_CONFIG_PARAM_CHANNELS, &channels);
    XA_API(base, XA_API_CMD_GET_CONFIG_PARAM, XA_RENDERER_CONFIG_PARAM_PCM_WIDTH, &pcm_width);

    /* ...calculate sample size */
    sample_size = channels * (pcm_width == 16 ? 2 : 4);

    /* ...calculate output audio frame duration; get upsample factor */
    XF_CHK_ERR(factor = xf_timebase_factor(sample_rate), XA_RENDERER_CONFIG_FATAL_RANGE);

    /* ...set renderer timestamp factor (converts input bytes into timebase units) */
    renderer->factor = factor / sample_size;

    /* ...it must be a multiple */
    XF_CHK_ERR(renderer->factor * sample_size == factor, XA_RENDERER_CONFIG_FATAL_RANGE);

    TRACE(INFO, _b("renderer runtime initialized: s=%u, c=%u, w=%u"), sample_rate, channels, pcm_width);

    return XA_NO_ERROR;
}

/*******************************************************************************
 * Commands handlers
 ******************************************************************************/

/* ...EMPTY-THIS-BUFFER command processing */
static XA_ERRORCODE xa_renderer_empty_this_buffer(XACodecBase *base, xf_message_t *m)
{
    XARenderer *renderer = (XARenderer *) base;

    /* ...make sure the port is valid (what about multi-channel renderer?) */
    XF_CHK_ERR(XF_MSG_DST_PORT(m->id) == 0, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...command is allowed only in "postinit" state */
    XF_CHK_ERR(base->state & XA_BASE_FLAG_POSTINIT, XA_API_FATAL_INVALID_CMD);

    TRACE(INPUT, _b("received buffer [%p]:%u"), m->buffer, m->length);

    /* ...put message into input port */
    if (xf_input_port_put(&renderer->input, m) && (base->state & XA_RENDERER_FLAG_OUTPUT_READY))
    {
        /* ...force data processing instantly */
        xa_base_schedule(base, 0);
    }

    return XA_NO_ERROR;
}

/* ...FILL-THIS-BUFFER command processing */
static XA_ERRORCODE xa_renderer_fill_this_buffer(XACodecBase *base, xf_message_t *m)
{
    XARenderer *renderer = (XARenderer *) base;
    UWORD32             consumed;

    TRACE(INFO, _b("received fill this buffer [%p]:%u"), m->buffer, m->length);

    /* ...make sure message is our internal one */
    XF_CHK_ERR(m == &renderer->msg, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...atomically clear callback message scheduling flag */
    xf_atomic_clear(&renderer->schedule, 1);

    /* ...check if output port flag was not set */
    if ((base->state & XA_RENDERER_FLAG_OUTPUT_READY) == 0)
    {
        /* ...indicate ouput is ready */
        base->state ^= XA_RENDERER_FLAG_OUTPUT_READY;

        /* ...check if we have pending input */
        if (xf_input_port_ready(&renderer->input))
        {
            /* ...force data processing instantly */
            xa_base_schedule(base, 0);
        }
    }

    XA_API(base, XA_API_CMD_GET_CURIDX_INPUT_BUF, 0, &consumed);

    /* ...input buffer maintenance; consume that amount from input port and check for end-of-stream condition */
    if (consumed)
    {
        /* ...consume bytes from input buffer */
        xf_input_port_consume(&renderer->input, consumed);

      
    }
    else
    {

        /* ...failed to put anything; clear OUTPUT-READY condition */
        base->state &= ~XA_RENDERER_FLAG_OUTPUT_READY;
    }


    return XA_NO_ERROR;
}

/* ...FLUSH command processing */
static XA_ERRORCODE xa_renderer_flush(XACodecBase *base, xf_message_t *m)
{
    XARenderer *renderer = (XARenderer *) base;

    /* ...command is allowed only in "execution" state - not necessarily - tbd*/
    XF_CHK_ERR(base->state & XA_BASE_FLAG_EXECUTION, XA_API_FATAL_INVALID_CMD);

    /* ...ensure input parameter length is zero */
    XF_CHK_ERR(m->length == 0, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...flush command must be addressed to input port */
    XF_CHK_ERR(XF_MSG_DST_PORT(m->id) == 0, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...cancel data processing if needed */
    xa_base_cancel(base);

    /* ...input port flushing; purge content of input buffer */
    xf_input_port_purge(&renderer->input);

    /* ...pass response to caller */
    xf_response(m);

    return XA_NO_ERROR;
}

/*******************************************************************************
 * Completion callback - shall be a separate IRQ-safe code
 ******************************************************************************/

/* ...this code runs from interrupt handler; we need to protect data somehow */
static void xa_renderer_callback(xa_renderer_cb_t *cdata, WORD32 i_idx)
{
    XARenderer     *renderer = container_of(cdata, XARenderer, cdata);

    /* ...schedule component execution if needed */
    if (xf_atomic_test_and_set(&renderer->schedule, 1))
    {
        /* ...pass fake fill-this-buffer command */
        xf_msg_schedule_isr(&renderer->msg);
    }
}

/*******************************************************************************
 * Codec API implementation
 ******************************************************************************/

/* ...buffers handling */
static XA_ERRORCODE xa_renderer_memtab(XACodecBase *base, WORD32 idx, WORD32 type, WORD32 size, WORD32 align, UWORD32 core)
{
    XARenderer *renderer = (XARenderer *)base;

    /* ...only "input" buffers are supported */
    XF_CHK_ERR(type == XA_MEMTYPE_INPUT, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...input buffer allocation; make sure input port index is sane */
    XF_CHK_ERR(idx == 0, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...create input port for a track */
    XF_CHK_ERR(xf_input_port_init(&renderer->input, size, align, core) == 0, XA_API_FATAL_MEM_ALLOC);

    /* ...well, we want to use buffers without copying them into interim buffer */
    TRACE(INIT, _b("renderer input port created - size=%u"), size);

    /* ...set input port buffer if needed */
    (size ? XA_API(base, XA_API_CMD_SET_MEM_PTR, idx, renderer->input.buffer) : 0);

    /* ...set internal scheduling message */
    renderer->msg.id = __XF_MSG_ID(0, ((xf_component_t *)renderer)->id);
    renderer->msg.opcode = XF_FILL_THIS_BUFFER;
    renderer->msg.length = 0;
    renderer->msg.buffer = NULL;

    /* ...mark renderer output buffer is ready */
    base->state |= XA_RENDERER_FLAG_OUTPUT_READY;

    return XA_NO_ERROR;
}

/* ...preprocessing function */
static XA_ERRORCODE xa_renderer_preprocess(XACodecBase *base)
{
    XARenderer     *renderer = (XARenderer *) base;
    UWORD32             filled;

    /* ...check current execution stage */
    if (base->state & XA_BASE_FLAG_RUNTIME_INIT)
    {
        /* ...no special processing in runtime initialization stage */
        return XA_NO_ERROR;
    }

    /* ...submit input buffer to the renderer */
    if (xf_input_port_bypass(&renderer->input))
    {
        void   *input;

        /* ...in-place buffers used */
        if ((input = xf_input_port_data(&renderer->input)) != NULL)
        {
            /* ...set input buffer pointer */
            XA_API(base, XA_API_CMD_SET_MEM_PTR, 0, input);

            /* ..retrieve number of bytes */
            filled = xf_input_port_length(&renderer->input);
        }
        else if (!xf_input_port_done(&renderer->input))
        {
            /* ...no input data available; do nothing */
            return XA_RENDERER_EXEC_NONFATAL_INPUT;
        }
        else
        {
            /* ...input port is done; buffer is empty */
            filled = 0;
        }
    }
    else
    {
        /* ...port is in non-bypass mode; try to fill internal buffer */
        if (!xf_input_port_fill(&renderer->input))
        {
            /* ...insufficient input data */
            return XA_RENDERER_EXEC_NONFATAL_INPUT;
        }
        else
        {
            /* ...retrieve number of bytes put in buffer */
            filled = xf_input_port_level(&renderer->input);
        }
    }

    /* ...set total number of bytes we have in buffer */
    XA_API(base, XA_API_CMD_SET_INPUT_BYTES, 0, &filled);

    /* ...check if input stream is over */
    if (xf_input_port_done(&renderer->input))
    {
        /* ...pass input-over command to plugin */
        XA_API(base, XA_API_CMD_INPUT_OVER, 0, NULL);

        /* ...input stream is over; complete pending zero-length message */
        xf_input_port_purge(&renderer->input);

        TRACE(INFO, _b("renderer operation is over"));
    }

    return XA_NO_ERROR;
}

/* ...postprocessing function */
static XA_ERRORCODE xa_renderer_postprocess(XACodecBase *base, int done)
{
    XARenderer     *renderer = (XARenderer *) base;
    UWORD32             consumed;
    UWORD32             i = 0;

    if (done)
    {
        if (base->state & XA_BASE_FLAG_RUNTIME_INIT)
        {
            /* ...processing is done while in runtime initialization state (can't be - tbd) */
            BUG(1, _x("breakpoint"));
        }
        else if (base->state & XA_BASE_FLAG_EXECUTION)
        {
            /* ...runtime initialization is done */
            XA_CHK(xa_renderer_prepare_runtime(renderer));

            /* ...reschedule execution instantly (both input- and output-ready conditions are set) */
            xa_base_schedule(base, 0);

            return XA_NO_ERROR;
        }
        else
        {
            /* ...renderer operation is completed (can't be - tbd) */
            BUG(1, _x("breakpoint"));
        }
    }

    /* ...get total amount of consumed bytes */
    XA_API(base, XA_API_CMD_GET_CURIDX_INPUT_BUF, i, &consumed);

    /* ...input buffer maintenance; consume that amount from input port and check for end-of-stream condition */
    if (consumed)
    {
        /* ...consume bytes from input buffer */
        xf_input_port_consume(&renderer->input, consumed);

        /* ...reschedule execution if we have pending input */
        if (xf_input_port_ready(&renderer->input))
        {
            /* ...schedule execution with respect to urgency  */
            xa_base_schedule(base, consumed * renderer->factor);
        }
    }
    else
    {
        /* ...failed to put anything; clear OUTPUT-READY condition */
        base->state &= ~XA_RENDERER_FLAG_OUTPUT_READY;
    }

    return XA_NO_ERROR;
}

/*******************************************************************************
 * Command-processing function
 ******************************************************************************/

/* ...command hooks */
static XA_ERRORCODE (* const xa_renderer_cmd[])(XACodecBase *, xf_message_t *) =
{
    /* ...set-parameter - actually, same as in generic case */
    [XF_OPCODE_TYPE(XF_SET_PARAM)] = xa_base_set_param,
    [XF_OPCODE_TYPE(XF_GET_PARAM)] = xa_base_get_param,

    /* ...input buffers processing */
    [XF_OPCODE_TYPE(XF_EMPTY_THIS_BUFFER)] = xa_renderer_empty_this_buffer,
    [XF_OPCODE_TYPE(XF_FILL_THIS_BUFFER)] = xa_renderer_fill_this_buffer,
    [XF_OPCODE_TYPE(XF_FLUSH)]  = xa_renderer_flush,
};

/* ...total number of commands supported */
#define XA_RENDERER_CMD_NUM         (sizeof(xa_renderer_cmd) / sizeof(xa_renderer_cmd[0]))

/*******************************************************************************
 * Entry points
 ******************************************************************************/

/* ...renderer termination-state command processor */
static int xa_renderer_terminate(xf_component_t *component, xf_message_t *m)
{
    XARenderer *renderer = (XARenderer *) component;

    /* ...check if we received internal message */
    if (m == &renderer->msg)
    {
        /* ...callback execution completed; complete operation */
        return -1;
    }
    else if (m->opcode == XF_UNREGISTER)
    {
        /* ...ignore subsequent unregister command/response */
        return 0;
    }
    else
    {
        /* ...everything else is responded with generic failure */
        xf_response_err(m);
        return 0;
    }
}

/* ...renderer class destructor */
static int xa_renderer_destroy(xf_component_t *component, xf_message_t *m)
{
    XARenderer *renderer = (XARenderer *) component;
    UWORD32         core = xf_component_core(component);

    /* ...destroy input port */
    xf_input_port_destroy(&renderer->input, core);

    /* ...destroy base object */
    xa_base_destroy(&renderer->base, XF_MM(sizeof(*renderer)), core);

    TRACE(INIT, _b("renderer[%p] destroyed"), renderer);

    /* ...indicate the component is destroyed */
    return 0;
}

/* ...renderer class first-stage destructor */
static int xa_renderer_cleanup(xf_component_t *component, xf_message_t *m)
{
    XARenderer     *renderer = (XARenderer *) component;
    XACodecBase    *base = (XACodecBase *) renderer;
    UWORD32             state = XA_RENDERER_STATE_IDLE;

    /* ...complete message with error result code */
    xf_response_err(m);

    /* ...cancel component task execution if needed */
    xa_base_cancel(base);

    /* ...stop hardware renderer if it's running */
    XA_API_NORET(base, XA_API_CMD_SET_CONFIG_PARAM, XA_RENDERER_CONFIG_PARAM_STATE, &state);

    /* ...purge input port */
    xf_input_port_purge(&renderer->input);

    /* ...check if we have internal message scheduled */
    if (xf_atomic_test_and_clear(&renderer->schedule, 1))
    {
        /* ...wait until callback message is returned */
        component->entry = xa_renderer_terminate;
        component->exit = xa_renderer_destroy;

        TRACE(INIT, _b("renderer[%p] cleanup sequence started"), renderer);

        /* ...indicate that second stage is required */
        return 1;
    }
    else
    {
        /* ...callback is not scheduled; destroy renderer */
        return xa_renderer_destroy(component, NULL);
    }
}

/* ...renderer class factory */
xf_component_t * xa_renderer_factory(UWORD32 core, xa_codec_func_t process,xaf_comp_type comp_type)
{
    XARenderer *renderer;

    /* ...construct generic audio component */
    XF_CHK_ERR(renderer = (XARenderer *)xa_base_factory(core, XF_MM(sizeof(*renderer)), process), NULL);

    /* ...set generic codec API */
    renderer->base.memtab = xa_renderer_memtab;
    renderer->base.preprocess = xa_renderer_preprocess;
    renderer->base.postprocess = xa_renderer_postprocess;

    /* ...set message-processing table */
    renderer->base.command = xa_renderer_cmd;
    renderer->base.command_num = XA_RENDERER_CMD_NUM;

    /* ...set component destructor hook */
    renderer->base.component.exit = xa_renderer_cleanup;

    /* ...set notification callback data */
    renderer->cdata.cb = xa_renderer_callback;
    renderer->base.comp_type = comp_type;
    /* ...pass buffer completion callback to the component */
    XA_API_NORET(&renderer->base, XA_API_CMD_SET_CONFIG_PARAM, XA_RENDERER_CONFIG_PARAM_CB, &renderer->cdata);

    TRACE(INIT, _b("Renderer[%p] created"), renderer);

    /* ...return handle to component */
    return (xf_component_t *) renderer;
}


