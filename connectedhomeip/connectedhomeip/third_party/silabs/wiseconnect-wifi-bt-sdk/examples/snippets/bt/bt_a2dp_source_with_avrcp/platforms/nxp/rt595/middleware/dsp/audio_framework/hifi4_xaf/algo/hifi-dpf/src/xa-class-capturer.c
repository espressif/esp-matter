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
 * xa-class-capturer.c
 *
 * Generic audio codec task implementation
 ******************************************************************************/

#define MODULE_TAG                      CAPTURER

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "xf-dp.h"
#include "xa-class-base.h"
#include "audio/xa-capturer-api.h"

/*******************************************************************************
 * Tracing configuration
 ******************************************************************************/

TRACE_TAG(INIT, 1);
TRACE_TAG(WARNING, 1);
TRACE_TAG(INFO, 1);
TRACE_TAG(INPUT, 1);
TRACE_TAG(OUTPUT, 1);
/*******************************************************************************
 * Internal functions definitions
 ******************************************************************************/

typedef struct XACapturer
{
    /***************************************************************************
     * Control data
     **************************************************************************/

    /* ...generic audio codec data */
    XACodecBase             base;


    /* ...output port data */
    xf_output_port_t        output;

     /* ...buffer completion hook */
    xa_capturer_cb_t    cdata;

    /* ...output ready message */
    xf_message_t        msg;


    /***************************************************************************
     * Run-time configuration parameters
     **************************************************************************/

    /* ...sample size in bytes */
    UWORD32                     sample_size;

    /* ...audio sample duration */
    UWORD32                     factor;
    /* ...internal message scheduling flag (shared with interrupt) */
    UWORD32                 schedule;

}   XACapturer;

/*******************************************************************************
 * Auxiliary codec execution flags
 ******************************************************************************/

/* ...input port setup condition */
#define XA_CAPTURER_FLAG_INPUT_READY        __XA_BASE_FLAG(1 << 0)

/*******************************************************************************
 * Data processing scheduling
 ******************************************************************************/

/* ...prepare codec for steady operation (tbd - don't absolutely like it) */
static inline XA_ERRORCODE xa_capturer_prepare_runtime(XACapturer *capturer)
{
    XACodecBase    *base = (XACodecBase *)capturer;
    xf_message_t   *m = xf_msg_queue_head(&capturer->output.queue);
    xf_start_msg_t *msg = m->buffer;
    UWORD32             frame_size;
    UWORD32             factor;

    /* ...fill-in buffer parameters */
    XA_API(base, XA_API_CMD_GET_CONFIG_PARAM, XA_CAPTURER_CONFIG_PARAM_SAMPLE_RATE, &msg->sample_rate);
    XA_API(base, XA_API_CMD_GET_CONFIG_PARAM, XA_CAPTURER_CONFIG_PARAM_CHANNELS, &msg->channels);
    XA_API(base, XA_API_CMD_GET_CONFIG_PARAM, XA_CAPTURER_CONFIG_PARAM_PCM_WIDTH, &msg->pcm_width);
    XA_API(base, XA_API_CMD_GET_MEM_INFO_SIZE, 0, &msg->output_length);
    base->state &= ~XA_CAPTURER_FLAG_INPUT_READY;
    TRACE(INIT, _b("codec[%p]::runtime init: f=%u, c=%u, w=%u, i=%u, o=%u"), capturer, msg->sample_rate, msg->channels, msg->pcm_width, msg->input_length, msg->output_length);

    /* ...save sample size in bytes */
    capturer->sample_size = msg->channels * (msg->pcm_width == 16 ? 2 : 4);

    /* ...retrieve upsampling factor for given sample rate */
    XF_CHK_ERR(factor = xf_timebase_factor(msg->sample_rate), XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...set frame duration factor (converts number of bytes into timebase units) */
    capturer->factor = factor / capturer->sample_size;

    TRACE(INIT, _b("ts-factor: %u (%u)"), capturer->factor, factor);

    BUG(capturer->factor * capturer->sample_size != factor, _x("Freq mismatch: %u vs %u"), capturer->factor * capturer->sample_size, factor);

    /* ...pass response to caller (push out of output port) */
    /*here the capturer would be sending the response back to the app*/
    xf_output_port_produce(&capturer->output, sizeof(*msg));

    /* ...codec runtime initialization is completed */
    TRACE(INIT, _b("codec[%p] runtime initialized: i=%u, o=%u"), capturer, msg->input_length, msg->output_length);

    return XA_NO_ERROR;
}


/*******************************************************************************
 * Commands processing
 ******************************************************************************/

/* ...EMPTY-THIS-BUFFER command processing */

static XA_ERRORCODE xa_capturer_empty_this_buffer(XACodecBase *base, xf_message_t *m)
{
    XACapturer   *capturer = (XACapturer *) base;
    UWORD32 produced = 0;

    /* ...make sure the port is sane */
    XF_CHK_ERR(XF_MSG_DST_PORT(m->id) == 0, XA_API_FATAL_INVALID_CMD);

    /* ...command is allowed only in post-init state */
    XF_CHK_ERR(base->state & XA_BASE_FLAG_POSTINIT, XA_API_FATAL_INVALID_CMD);

    /* ...put message into input queue */
    /* ...codec must be in one of these states */
    XF_CHK_ERR(base->state & (XA_BASE_FLAG_RUNTIME_INIT | XA_BASE_FLAG_EXECUTION), XA_API_FATAL_INVALID_CMD);

    xf_atomic_clear(&capturer->schedule, 1);
    /* ...schedule data processing if output is ready */

    if (xf_output_port_ready(&capturer->output))
    {
         xa_base_schedule(base, 0);
    }
    else
    {
        /*error case...in empty this buffer output should be ready*/
        return XA_CAPTURER_EXEC_FATAL_STATE;
    }


    TRACE(INPUT, _b("Received buffer [%p]:%u"), m->buffer, m->length);

    return XA_NO_ERROR;
}

/* ...FILL-THIS-BUFFER command processing */
static XA_ERRORCODE xa_capturer_fill_this_buffer(XACodecBase *base, xf_message_t *m)
{
    XACapturer   *capturer = (XACapturer *) base;

    /* ...make sure the port is sane */
    XF_CHK_ERR(XF_MSG_DST_PORT(m->id) == 1, XA_API_FATAL_INVALID_CMD);

    /* ...command is allowed only in postinit state */
    XF_CHK_ERR(base->state & XA_BASE_FLAG_POSTINIT, XA_API_FATAL_INVALID_CMD);

    /* ...special handling of zero-length buffer */
    if (base->state & XA_BASE_FLAG_RUNTIME_INIT)
    {
        /* ...message must be zero-length */
        BUG(m->length != 0, _x("Invalid message length: %u"), m->length);
    }
    else if (m == xf_output_port_control_msg(&capturer->output))
    {
        /* ...end-of-stream processing indication received; check the state */
        BUG((base->state & XA_BASE_FLAG_COMPLETED) == 0, _x("invalid state: %x"), base->state);

        /* ... mark flushing sequence is done */
        xf_output_port_flush_done(&capturer->output);

        /* ...complete pending zero-length input buffer */
       // xf_input_port_purge(&codec->input);

        TRACE(INFO, _b("codec[%p] playback completed"), capturer);

        /* ...playback is over */
        return XA_NO_ERROR;
    }
    else if ((base->state & XA_BASE_FLAG_COMPLETED) && !xf_output_port_routed(&capturer->output))
    {
        /* ...return message arrived from application immediately */
        xf_response_ok(m);

        return XA_NO_ERROR;
    }
    else
    {
        TRACE(OUTPUT, _b("Received output buffer [%p]:%u"), m->buffer, m->length);

        /* ...adjust message length (may be shorter than original) */
        m->length = capturer->output.length;
    }

    /* ...place message into output port */
    if (xf_output_port_put(&capturer->output, m) && (base->state & XA_CAPTURER_FLAG_INPUT_READY))
    {
        /* ...schedule data processing instantly */
        if (base->state & (XA_BASE_FLAG_RUNTIME_INIT | XA_BASE_FLAG_EXECUTION))
        {
             xa_base_schedule(base, 0);
        }
    }

    return XA_NO_ERROR;
}

/* ...output port routing */
static XA_ERRORCODE xa_capturer_port_route(XACodecBase *base, xf_message_t *m)
{
    XACapturer          *capturer = (XACapturer *) base;
    xf_route_port_msg_t    *cmd = m->buffer;
    xf_output_port_t       *port = &capturer->output;
    UWORD32                     src = XF_MSG_DST(m->id);
    UWORD32                     dst = cmd->dst;

    /* ...command is allowed only in "postinit" state */

    XF_CHK_ERR(base->state & XA_BASE_FLAG_POSTINIT, XA_API_FATAL_INVALID_CMD);

    /* ...make sure output port is addressed */
    XF_CHK_ERR(XF_MSG_DST_PORT(m->id) == 1, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...make sure port is not routed yet */
    XF_CHK_ERR(!xf_output_port_routed(port), XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...route output port - allocate queue */
    XF_CHK_ERR(xf_output_port_route(port, __XF_MSG_ID(dst, src), cmd->alloc_number, cmd->alloc_size, cmd->alloc_align) == 0, XA_API_FATAL_MEM_ALLOC);

    /* ...pass success result to caller */
    xf_response_ok(m);

    return XA_NO_ERROR;
}

/* ...port unroute command */
static XA_ERRORCODE xa_capturer_port_unroute(XACodecBase *base, xf_message_t *m)
{
    XACapturer         *capturer = (XACapturer *) base;

    /* ...command is allowed only in "postinit" state */
    XF_CHK_ERR(base->state & XA_BASE_FLAG_POSTINIT, XA_API_FATAL_INVALID_CMD);

    /* ...make sure output port is addressed */
    XF_CHK_ERR(XF_MSG_DST_PORT(m->id) == 1, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...cancel any pending processing */
    xa_base_cancel(base);

    /* ...clear output-port-setup condition */
    base->state &= ~XA_CAPTURER_FLAG_INPUT_READY;

    /* ...pass flush command down the graph */
    if (xf_output_port_flush(&capturer->output, XF_FLUSH))
    {
        TRACE(INFO, _b("port is idle; instantly unroute"));

        /* ...flushing sequence is not needed; command may be satisfied instantly */
        xf_output_port_unroute(&capturer->output);

        /* ...pass response to the proxy */
        xf_response_ok(m);
    }
    else
    {
        TRACE(INFO, _b("port is busy; propagate unroute command"));

        /* ...flushing sequence is started; save flow-control message */
        xf_output_port_unroute_start(&capturer->output, m);
    }

    return XA_NO_ERROR;
}
/* ...FLUSH command processing */
static XA_ERRORCODE xa_capturer_flush(XACodecBase *base, xf_message_t *m)
{
    XACapturer   *capturer = (XACapturer *) base;

    /* ...command is allowed only in "postinit" state */
    XF_CHK_ERR(base->state & XA_BASE_FLAG_POSTINIT, XA_API_FATAL_INVALID_CMD);

    /* ...ensure input parameter length is zero */
    XF_CHK_ERR(m->length == 0, XA_API_FATAL_INVALID_CMD_TYPE);

    TRACE(1, _b("flush command received"));

    /* ...flush command must be addressed to input port */
    if (XF_MSG_DST_PORT(m->id) == 0)
    {
        /* ...cancel data processing message if needed */
        xa_base_cancel(base);

        /* ...clear input-ready condition */
        base->state &= ~XA_CAPTURER_FLAG_INPUT_READY;

        /* ...reset execution runtime */
        XA_API(base, XA_API_CMD_EXECUTE, XA_CMD_TYPE_DO_RUNTIME_INIT, NULL);

        /* ...propagate flushing command to output port */
        if (xf_output_port_flush(&capturer->output, XF_FLUSH))
        {
            /* ...flushing sequence is not needed; satisfy command instantly */
            xf_response(m);
        }

    }
    else if (xf_output_port_unrouting(&capturer->output))
    {
        /* ...flushing during port unrouting; complete unroute sequence */
        xf_output_port_unroute_done(&capturer->output);

        TRACE(INFO, _b("port is unrouted"));
    }
    else
    {
        /* ...output port flush command/response; check if the port is routed */
        if (!xf_output_port_routed(&capturer->output))
        {
            /* ...complete all queued messages */
            xf_output_port_flush(&capturer->output, XF_FLUSH);

            /* ...and pass response to flushing command */
            xf_response(m);
        }
        else
        {
            /* ...response to flushing command received */
            BUG(m != xf_output_port_control_msg(&capturer->output), _x("invalid message: %p"), m);

            /* ...mark flushing sequence is completed */
            xf_output_port_flush_done(&capturer->output);

            /* ...complete original flow-control command */
           // xf_input_port_purge_done(&codec->input);
        }

        /* ...clear output-setup condition */
        base->state &= ~XA_CAPTURER_FLAG_INPUT_READY;
    }

    return XA_NO_ERROR;
}

/*******************************************************************************
 * Completion callback - shall be a separate IRQ-safe code
 ******************************************************************************/

/* ...this code runs from interrupt handler; we need to protect data somehow */
static void xa_capturer_callback(xa_capturer_cb_t *cdata, WORD32 i_idx)
{
    XACapturer     *capturer = container_of(cdata, XACapturer, cdata);

    /* ...schedule component execution if needed */
    if (xf_atomic_test_and_set(&capturer->schedule, 1))
    {
        /* ...pass fake fill-this-buffer command */
        xf_msg_schedule_isr(&capturer->msg);
    }
}

/*******************************************************************************
 * Generic codec API
 ******************************************************************************/

/* ...memory buffer handling */
static XA_ERRORCODE xa_capturer_memtab(XACodecBase *base, WORD32 idx, WORD32 type, WORD32 size, WORD32 align, UWORD32 core)
{
    XACapturer   *capturer = (XACapturer *) base;

    /* ...output buffer specification */
     XF_CHK_ERR(type == XA_MEMTYPE_OUTPUT, XA_API_FATAL_INVALID_CMD_TYPE);
    /* ...output buffer allocation; make sure output port index is sane */
    XF_CHK_ERR(idx == 0, XA_API_FATAL_INVALID_CMD_TYPE);
    /* ...initialize output port queue (no allocation here yet) */
    XF_CHK_ERR(xf_output_port_init(&capturer->output, size) == 0, XA_API_FATAL_MEM_ALLOC);
     /* ...save output port index */
    /* ...set internal scheduling message */
    capturer->msg.id = __XF_MSG_ID(0, ((xf_component_t *)capturer)->id);
    capturer->msg.opcode = XF_EMPTY_THIS_BUFFER;
    capturer->msg.length = 0;
    capturer->msg.buffer = NULL;

    /* ...mark renderer output buffer is ready */
    base->state |= XA_CAPTURER_FLAG_INPUT_READY;


    return XA_NO_ERROR;
}
/*extern int inter_cnt;*/

/* ...prepare output buffers */
static XA_ERRORCODE xa_capturer_preprocess(XACodecBase *base)
{
    XACapturer   *capturer = (XACapturer *) base;
    void   *output;

     /* ...get output buffer from port, if possible */
    if (base->state & XA_BASE_FLAG_RUNTIME_INIT)
    {
        return XA_NO_ERROR;
    }
    else if ((output = xf_output_port_data(&capturer->output)) == NULL)
    {
        /* ...no output buffer available */
        return XA_CAPTURER_EXEC_NONFATAL_NO_DATA;
    }
     XA_API(base, XA_API_CMD_SET_MEM_PTR, 0/*codec->out_idx*/, output);
    TRACE(1, _x("set output ptr: %p"), output);
    return XA_NO_ERROR;
}

/* ...post-processing operation; input/output ports maintenance */

static XA_ERRORCODE xa_capturer_postprocess(XACodecBase *base, int done)
{
    XACapturer   *capturer = (XACapturer *) base;
    WORD32          consumed = 0;
    WORD32          produced = 0;
    xaf_comp_type    comp_type = base->comp_type;

    if (done)
    {
        if (base->state & XA_BASE_FLAG_RUNTIME_INIT)
        {
            /* ...stream is completed while codec is in runtime initialization stage */
            BUG(1, _x("breakpoint"));
        }
        else if (base->state & XA_BASE_FLAG_EXECUTION)
        {
            /* ...runtime initialization done */
            XA_CHK(xa_capturer_prepare_runtime(capturer));
            return XA_NO_ERROR;
        }
        else
        {
            /* ...output stream is over; propagate condition to sink port */
            if (xf_output_port_flush(&capturer->output, XF_FILL_THIS_BUFFER))
            {
                /* ...flushing sequence is not needed; complete pending zero-length input */
                /* ...no propagation to output port */
                TRACE(INFO, _b("codec[%p] playback completed"), capturer);
            }
            else
            {
                /* ...flushing sequence is started; wait until flow-control message returns */
                TRACE(INFO, _b("propagate end-of-stream condition"));
            }
        }

        /* ...return early to prevent task rescheduling */
        return XA_NO_ERROR;
    }
    (capturer->sample_size ? XA_API(base, XA_API_CMD_GET_OUTPUT_BYTES, 0/*codec->out_idx*/, &produced) : 0);

    if (produced)
    {
        /* ...immediately complete output buffer (don't wait until it gets filled) */
        xf_output_port_produce(&capturer->output, produced);
    }

    return XA_NO_ERROR;
}

/*******************************************************************************
 * Component entry point
 ******************************************************************************/

/* ...command hooks */
static XA_ERRORCODE (* const xa_capturer_cmd[])(XACodecBase *, xf_message_t *) =
{
    [XF_OPCODE_TYPE(XF_SET_PARAM)] = xa_base_set_param,
    [XF_OPCODE_TYPE(XF_GET_PARAM)] = xa_base_get_param,
    [XF_OPCODE_TYPE(XF_ROUTE)] = xa_capturer_port_route,
    [XF_OPCODE_TYPE(XF_UNROUTE)] = xa_capturer_port_unroute,
    [XF_OPCODE_TYPE(XF_EMPTY_THIS_BUFFER)] = xa_capturer_empty_this_buffer,
    [XF_OPCODE_TYPE(XF_FILL_THIS_BUFFER)] = xa_capturer_fill_this_buffer,
    [XF_OPCODE_TYPE(XF_FLUSH)] = xa_capturer_flush,
    [XF_OPCODE_TYPE(XF_SET_PARAM_EXT)] = xa_base_set_param_ext,
    [XF_OPCODE_TYPE(XF_GET_PARAM_EXT)] = xa_base_get_param_ext,
};

/* ...total number of commands supported */
#define XA_CAPTURER_CMD_NUM        (sizeof(xa_capturer_cmd) / sizeof(xa_capturer_cmd[0]))

/* ...command processor for termination state (only for routed port case) */
static int xa_capturer_terminate(xf_component_t *component, xf_message_t *m)
{
    XACapturer   *capturer = (XACapturer *) component;
    UWORD32             opcode = m->opcode;

    /* ...check if we received output port control message */
    if (m == xf_output_port_control_msg(&capturer->output))
    {
        /* ...output port flushing complete; mark port is idle and terminate */
        xf_output_port_flush_done(&capturer->output);
        return -1;
    }
    else if (opcode == XF_FILL_THIS_BUFFER)
    {
        /* ...output buffer returned by the sink component; ignore and keep waiting */
        TRACE(OUTPUT, _b("collect output buffer"));
        return 0;
    }
    else if (opcode == XF_UNREGISTER)
    {
        /* ...ignore subsequent unregister command/response - tbd */
        return 0;
    }
    else
    {
        /* ...everything else is responded with generic failure */
        xf_response_err(m);
        return 0;
    }
}

/* ...audio codec destructor */
static int xa_capturer_destroy(xf_component_t *component, xf_message_t *m)
{
    XACapturer   *capturer = (XACapturer *) component;
    UWORD32             core = xf_component_core(component);

    /* ...destroy input port */
    //xf_input_port_destroy(&codec->input, core);

    /* ...destroy output port */
    xf_output_port_destroy(&capturer->output, core);

    /* ...deallocate all resources */
    xa_base_destroy(&capturer->base, XF_MM(sizeof(*capturer)), core);

    TRACE(INIT, _b("audio-codec[%p@%u] destroyed"), capturer, core);

    /* ...indicate the client has been destroyed */
    return 0;
}

/* ...audio codec destructor - first stage (ports unrouting) */
static int xa_capturer_cleanup(xf_component_t *component, xf_message_t *m)
{
    XACapturer *capturer = (XACapturer *) component;
    UWORD32             state = XA_CAPTURER_STATE_IDLE;
     XACodecBase    *base = (XACodecBase *) capturer;

    /* ...complete message with error response */
    xf_response_err(m);

    /* ...cancel internal scheduling message if needed */
    xa_base_cancel(&capturer->base);

    XA_API_NORET(base, XA_API_CMD_SET_CONFIG_PARAM, XA_CAPTURER_CONFIG_PARAM_STATE, &state);

    /* ...propagate unregister command to connected component */
    if (xf_output_port_flush(&capturer->output, XF_FLUSH))
    {
        /* ...flushing sequence is not needed; destroy audio codec */
        return xa_capturer_destroy(component, NULL);
    }
    else
    {
        /* ...wait until output port is cleaned; adjust component hooks */
        component->entry = xa_capturer_terminate;
        component->exit = xa_capturer_destroy;

        TRACE(INIT, _b("codec[%p] cleanup sequence started"), capturer);

        /* ...indicate that second stage is required */
        return 1;
    }
}

/*******************************************************************************
 * Capturer component factory
 ******************************************************************************/

xf_component_t * xa_capturer_factory(UWORD32 core, xa_codec_func_t process, xaf_comp_type comp_type)
{
    XACapturer   *capturer;

    /* ...allocate local memory for codec structure */
    XF_CHK_ERR(capturer = (XACapturer *) xa_base_factory(core, XF_MM(sizeof(*capturer)), process), NULL);

    /* ...set base codec API methods */
    capturer->base.memtab = xa_capturer_memtab;
    capturer->base.preprocess = xa_capturer_preprocess;
    capturer->base.postprocess = xa_capturer_postprocess;

    /* ...set message commands processing table */
    capturer->base.command = xa_capturer_cmd;
    capturer->base.command_num = XA_CAPTURER_CMD_NUM;

    /* ...set component destructor hook */
    capturer->base.component.exit = xa_capturer_cleanup;

    capturer->cdata.cb = xa_capturer_callback;
    /* ...set component type */
    capturer->base.comp_type = comp_type;
    XA_API_NORET(&capturer->base, XA_API_CMD_SET_CONFIG_PARAM, XA_CAPTURER_CONFIG_PARAM_CB, &capturer->cdata);
    TRACE(INIT, _b("Codec[%p] initialized"), capturer);

    return (xf_component_t *) capturer;
}
