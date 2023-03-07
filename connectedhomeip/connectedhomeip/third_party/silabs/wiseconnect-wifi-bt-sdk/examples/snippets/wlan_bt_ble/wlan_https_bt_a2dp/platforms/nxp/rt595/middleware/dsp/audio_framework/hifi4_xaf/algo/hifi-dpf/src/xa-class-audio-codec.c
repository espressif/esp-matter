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
 * xa-class-audio-codec.c
 *
 * Generic audio codec task implementation
 ******************************************************************************/

#define MODULE_TAG                      CODEC

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "xf-dp.h"
#include "xa-class-base.h"
#include "audio/xa-audio-decoder-api.h"

/*******************************************************************************
 * Tracing configuration
 ******************************************************************************/

TRACE_TAG(INIT, 1);
TRACE_TAG(WARNING, 1);
TRACE_TAG(INFO, 1);
TRACE_TAG(INPUT, 1);
TRACE_TAG(OUTPUT, 1);
TRACE_TAG(DECODE, 1);

/*******************************************************************************
 * Internal functions definitions
 ******************************************************************************/

typedef struct XAAudioCodec
{
    /***************************************************************************
     * Control data
     **************************************************************************/

    /* ...generic audio codec data */
    XACodecBase             base;

    /* ...input port data */
    xf_input_port_t         input;

    /* ...output port data */
    xf_output_port_t        output;

    /* ...input port index */
    WORD32                  in_idx;

    /* ...output port index */
    WORD32                  out_idx;

    /***************************************************************************
     * Run-time configuration parameters
     **************************************************************************/

    /* ...sample size in bytes */
    UWORD32                     sample_size;

    /* ...audio sample duration */
    UWORD32                     factor;

    /* ...total number of produced audio frames since last reset */
    UWORD32                     produced;

    UWORD32                     consumed;

}   XAAudioCodec;

/*******************************************************************************
 * Auxiliary codec execution flags
 ******************************************************************************/

/* ...input port setup condition */
#define XA_CODEC_FLAG_INPUT_SETUP       __XA_BASE_FLAG(1 << 0)

/* ...output port setup condition */
#define XA_CODEC_FLAG_OUTPUT_SETUP      __XA_BASE_FLAG(1 << 1)

/*******************************************************************************
 * Data processing scheduling
 ******************************************************************************/

/* ...prepare codec for steady operation (tbd - don't absolutely like it) */
static inline XA_ERRORCODE xa_codec_prepare_runtime(XAAudioCodec *codec)
{
    XACodecBase    *base = (XACodecBase *)codec;
    xf_message_t   *m = xf_msg_queue_head(&codec->output.queue);
    xf_start_msg_t *msg = m->buffer;
    UWORD32             frame_size;
    UWORD32             factor;

    /* ...fill-in buffer parameters */
    XA_API(base, XA_API_CMD_GET_CONFIG_PARAM, XA_CODEC_CONFIG_PARAM_SAMPLE_RATE, &msg->sample_rate);
    XA_API(base, XA_API_CMD_GET_CONFIG_PARAM, XA_CODEC_CONFIG_PARAM_CHANNELS, &msg->channels);
    XA_API(base, XA_API_CMD_GET_CONFIG_PARAM, XA_CODEC_CONFIG_PARAM_PCM_WIDTH, &msg->pcm_width);
    XA_API(base, XA_API_CMD_GET_MEM_INFO_SIZE, codec->in_idx, &msg->input_length[0]);
    XA_API(base, XA_API_CMD_GET_MEM_INFO_SIZE, codec->out_idx, &msg->output_length[0]);

    TRACE(INIT, _b("codec[%p]::runtime init: f=%u, c=%u, w=%u, i=%u, o=%u"), codec, msg->sample_rate, msg->channels, msg->pcm_width, msg->input_length[0], msg->output_length[0]);

    /* ...reallocate input port buffer as needed - tbd */
    BUG(msg->input_length[0] > codec->input.length, _x("Input buffer reallocation required: %u to %u"), codec->input.length, msg->input_length[0]);

    /* ...save sample size in bytes */
    codec->sample_size = msg->channels * (msg->pcm_width == 16 ? 2 : 4);

    /* ...calculate frame duration; get number of samples in the frame (don't like division here - tbd) */
    frame_size = msg->output_length[0] / codec->sample_size;

    /* ...it must be a multiple */
    XF_CHK_ERR(frame_size * codec->sample_size == msg->output_length[0], XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...retrieve upsampling factor for given sample rate */
    XF_CHK_ERR(factor = xf_timebase_factor(msg->sample_rate), XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...set frame duration factor (converts number of bytes into timebase units) */
    codec->factor = factor / codec->sample_size;

    TRACE(INIT, _b("ts-factor: %u (%u)"), codec->factor, factor);

    BUG(codec->factor * codec->sample_size != factor, _x("Freq mismatch: %u vs %u"), codec->factor * codec->sample_size, factor);

    /* ...pass response to caller (push out of output port) */
    xf_output_port_produce(&codec->output, sizeof(*msg));

    /* ...codec runtime initialization is completed */
    TRACE(INIT, _b("codec[%p] runtime initialized: i=%u, o=%u"), codec, msg->input_length[0], msg->output_length[0]);

    return XA_NO_ERROR;
}

/*******************************************************************************
 * Commands processing
 ******************************************************************************/

/* ...EMPTY-THIS-BUFFER command processing */
static XA_ERRORCODE xa_codec_empty_this_buffer(XACodecBase *base, xf_message_t *m)
{
    XAAudioCodec   *codec = (XAAudioCodec *) base;

    /* ...make sure the port is sane */
    XF_CHK_ERR(XF_MSG_DST_PORT(m->id) == 0, XA_API_FATAL_INVALID_CMD);

    /* ...command is allowed only in post-init state */
    XF_CHK_ERR(base->state & XA_BASE_FLAG_POSTINIT, XA_API_FATAL_INVALID_CMD);

    /* ...put message into input queue */
    if (xf_input_port_put(&codec->input, m))
    {
        /* ...restart stream if it is in completed state */
        if (base->state & XA_BASE_FLAG_COMPLETED)
        {
            /* ...reset execution stage */
            base->state = XA_BASE_FLAG_POSTINIT | XA_BASE_FLAG_EXECUTION;

            /* ...reset execution runtime */
            XA_API(base, XA_API_CMD_EXECUTE, XA_CMD_TYPE_DO_RUNTIME_INIT, NULL);

            /* ...reset produced samples counter */
            codec->produced = 0;
        }

        /* ...codec must be in one of these states */
        XF_CHK_ERR(base->state & (XA_BASE_FLAG_RUNTIME_INIT | XA_BASE_FLAG_EXECUTION), XA_API_FATAL_INVALID_CMD);
        
        /* ...schedule data processing if output is ready */
        if (xf_output_port_ready(&codec->output))
        {
            xa_base_schedule(base, 0);
        }
    }

    TRACE(INPUT, _b("Received buffer [%p]:%u"), m->buffer, m->length);

    return XA_NO_ERROR;
}

/* ...FILL-THIS-BUFFER command processing */
static XA_ERRORCODE xa_codec_fill_this_buffer(XACodecBase *base, xf_message_t *m)
{
    XAAudioCodec   *codec = (XAAudioCodec *) base;

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
    else if (m == xf_output_port_control_msg(&codec->output))
    {
        /* ...end-of-stream processing indication received; check the state */
        BUG((base->state & XA_BASE_FLAG_COMPLETED) == 0, _x("invalid state: %x"), base->state);

        /* ... mark flushing sequence is done */
        xf_output_port_flush_done(&codec->output);

        /* ...complete pending zero-length input buffer */
        xf_input_port_purge(&codec->input);

        TRACE(INFO, _b("codec[%p] playback completed"), codec);

        /* ...playback is over */
        return XA_NO_ERROR;
    }
    else if ((base->state & XA_BASE_FLAG_COMPLETED) && !xf_output_port_routed(&codec->output))
    {
        /* ...return message arrived from application immediately */
        xf_response_ok(m);

        return XA_NO_ERROR;
    }
    else
    {
        TRACE(OUTPUT, _b("Received output buffer [%p]:%u"), m->buffer, m->length);

        /* ...adjust message length (may be shorter than original) */
        m->length = codec->output.length;
    }

    /* ...place message into output port */
    if (xf_output_port_put(&codec->output, m) && xf_input_port_ready(&codec->input))
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
static XA_ERRORCODE xa_codec_port_route(XACodecBase *base, xf_message_t *m)
{
    XAAudioCodec           *codec = (XAAudioCodec *) base;
    xf_route_port_msg_t    *cmd = m->buffer;
    xf_output_port_t       *port = &codec->output;
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

    /* ...schedule processing instantly */
    xa_base_schedule(base, 0);
    
    /* ...pass success result to caller */
    xf_response_ok(m);
    
    return XA_NO_ERROR;
}

/* ...port unroute command */
static XA_ERRORCODE xa_codec_port_unroute(XACodecBase *base, xf_message_t *m)
{
    XAAudioCodec           *codec = (XAAudioCodec *) base;
    
    /* ...command is allowed only in "postinit" state */
    XF_CHK_ERR(base->state & XA_BASE_FLAG_POSTINIT, XA_API_FATAL_INVALID_CMD);

    /* ...make sure output port is addressed */
    XF_CHK_ERR(XF_MSG_DST_PORT(m->id) == 1, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...cancel any pending processing */
    xa_base_cancel(base);

    /* ...clear output-port-setup condition */
    base->state &= ~XA_CODEC_FLAG_OUTPUT_SETUP;

    /* ...pass flush command down the graph */
    if (xf_output_port_flush(&codec->output, XF_FLUSH))
    {
        TRACE(INFO, _b("port is idle; instantly unroute"));

        /* ...flushing sequence is not needed; command may be satisfied instantly */
        xf_output_port_unroute(&codec->output);

        /* ...pass response to the proxy */
        xf_response_ok(m);
    }
    else
    {
        TRACE(INFO, _b("port is busy; propagate unroute command"));

        /* ...flushing sequence is started; save flow-control message */
        xf_output_port_unroute_start(&codec->output, m);
    }

    return XA_NO_ERROR;
}

/* ...FLUSH command processing */
static XA_ERRORCODE xa_codec_flush(XACodecBase *base, xf_message_t *m)
{
    XAAudioCodec   *codec = (XAAudioCodec *) base;

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

        /* ...input port flushing; purge content of input buffer */
        xf_input_port_purge(&codec->input);

        /* ...clear input-ready condition */
        base->state &= ~XA_CODEC_FLAG_INPUT_SETUP;

        /* ...reset execution runtime */
        XA_API(base, XA_API_CMD_EXECUTE, XA_CMD_TYPE_DO_RUNTIME_INIT, NULL);

        /* ...reset produced samples counter */
        codec->produced = 0;

        /* ...propagate flushing command to output port */
        if (xf_output_port_flush(&codec->output, XF_FLUSH))
        {
            /* ...flushing sequence is not needed; satisfy command instantly */
            xf_response(m);
        }
        else
        {
            /* ...flushing sequence is started; save flow-control message at input port */
            xf_input_port_control_save(&codec->input, m);
        }
    }
    else if (xf_output_port_unrouting(&codec->output))
    {
        /* ...flushing during port unrouting; complete unroute sequence */
        xf_output_port_unroute_done(&codec->output);

        TRACE(INFO, _b("port is unrouted"));
    }
    else
    {
        /* ...output port flush command/response; check if the port is routed */
        if (!xf_output_port_routed(&codec->output))
        {
            /* ...complete all queued messages */
            xf_output_port_flush(&codec->output, XF_FLUSH);

            /* ...and pass response to flushing command */
            xf_response(m);
        }
        else
        {            
            /* ...response to flushing command received */
            BUG(m != xf_output_port_control_msg(&codec->output), _x("invalid message: %p"), m);

            /* ...mark flushing sequence is completed */
            xf_output_port_flush_done(&codec->output);

            /* ...complete original flow-control command */
            xf_input_port_purge_done(&codec->input);
        }

        /* ...clear output-setup condition */
        base->state &= ~XA_CODEC_FLAG_OUTPUT_SETUP;
    }

    return XA_NO_ERROR;
}

/*******************************************************************************
 * Generic codec API
 ******************************************************************************/

/* ...memory buffer handling */
static XA_ERRORCODE xa_codec_memtab(XACodecBase *base, WORD32 idx, WORD32 type, WORD32 size, WORD32 align, UWORD32 core)
{
    XAAudioCodec   *codec = (XAAudioCodec *) base;

    if (type == XA_MEMTYPE_INPUT)
    {
        /* ...input port specification; allocate internal buffer */
        XF_CHK_ERR(xf_input_port_init(&codec->input, size, align, core) == 0, XA_API_FATAL_MEM_ALLOC);

        /* ...save input port index */
        codec->in_idx = idx;

        /* ...set input buffer pointer as needed */
        (size ? XA_API(base, XA_API_CMD_SET_MEM_PTR, idx, codec->input.buffer) : 0);

        (size ? TRACE(1, _x("set input ptr: %p"), codec->input.buffer) : 0);
    }
    else
    {
        /* ...output buffer specification */
        XF_CHK_ERR(type == XA_MEMTYPE_OUTPUT, XA_API_FATAL_INVALID_CMD_TYPE);

        /* ...initialize output port queue (no allocation here yet) */
        XF_CHK_ERR(xf_output_port_init(&codec->output, size) == 0, XA_API_FATAL_MEM_ALLOC);

        /* ...save output port index */
        codec->out_idx = idx;
    }

    return XA_NO_ERROR;
}

/* ...prepare input/output buffers */
static XA_ERRORCODE xa_codec_preprocess(XACodecBase *base)
{
    XAAudioCodec   *codec = (XAAudioCodec *) base;

    /* ...prepare output buffer if needed */
    if (!(base->state & XA_CODEC_FLAG_OUTPUT_SETUP))
    {
        void   *output;

        /* ...get output buffer from port, if possible */
        if (base->state & XA_BASE_FLAG_RUNTIME_INIT)
        {
            /* ...run-time is not initialized yet; use scratch buffer */
            output = base->scratch;
        }
        else if ((output = xf_output_port_data(&codec->output)) == NULL)
        {
            /* ...no output buffer available */
            return XA_CODEC_EXEC_NO_DATA;
        }

        /* ...set the output buffer pointer */
        XA_API(base, XA_API_CMD_SET_MEM_PTR, codec->out_idx, output);

        TRACE(1, _x("set output ptr: %p"), output);
        
        /* ...mark output port is setup */
        base->state ^= XA_CODEC_FLAG_OUTPUT_SETUP;
    }

    /* ...prepare input data if needed */
    if (!(base->state & XA_CODEC_FLAG_INPUT_SETUP))
    {
        void   *input;
        UWORD32     filled;

        /* ...fill input buffer */
        if (xf_input_port_bypass(&codec->input))
        {
            /* ...use input buffer directly; check if there is data available */
            if ((input = xf_input_port_data(&codec->input)) != NULL)
            {
                /* ...set input data buffer pointer */
                XA_API(base, XA_API_CMD_SET_MEM_PTR, codec->in_idx, input);

                /* ...retrieve number of input bytes */
                filled = xf_input_port_length(&codec->input);
            }
            else if (!xf_input_port_done(&codec->input))
            {
                /* ...return non-fatal indication to prevent further processing */
                return XA_CODEC_EXEC_NO_DATA;
            }
            else
            {
                /* ...mark we have no data in current buffer */
                filled = 0;
            }
        }
        else
        {
            /* ...port is in non-bypass mode; try to fill internal buffer */
            if (xf_input_port_done(&codec->input) || xf_input_port_fill(&codec->input))
            {
                /* ...retrieve number of bytes in input buffer (not really - tbd) */
                filled = xf_input_port_level(&codec->input);
            }
            else
            {
                /* ...return non-fatal indication to prevent further processing */
                return XA_CODEC_EXEC_NO_DATA;
            }
        }

        /* ...check if input stream is over */
        if (xf_input_port_done(&codec->input))
        {
            /* ...pass input-over command to the codec to indicate the final buffer */
            XA_API(base, XA_API_CMD_INPUT_OVER, codec->in_idx, NULL);

            TRACE(INFO, _b("codec[%p]: signal input-over (filled: %u)"), codec, filled);
        }

        TRACE(INPUT, _b("input-buffer fill-level: %u bytes"), filled);

        /* ...specify number of bytes available in the input buffer */
        XA_API(base, XA_API_CMD_SET_INPUT_BYTES, codec->in_idx, &filled);

        /* ...mark input port is setup */
        base->state ^= XA_CODEC_FLAG_INPUT_SETUP;
    }

    return XA_NO_ERROR;
}

/* ...post-processing operation; input/output ports maintenance */
static XA_ERRORCODE xa_codec_postprocess(XACodecBase *base, int done)
{
    XAAudioCodec   *codec = (XAAudioCodec *) base;
    WORD32          consumed = 0;
    WORD32          produced = 0;
    xaf_comp_type    comp_type = base->comp_type;

    /* ...get number of consumed / produced bytes */
    XA_API(base, XA_API_CMD_GET_CURIDX_INPUT_BUF, codec->in_idx, &consumed);

    /* ...get number of produced bytes only if runtime is initialized (sample size is known) */
    (codec->sample_size ? XA_API(base, XA_API_CMD_GET_OUTPUT_BYTES, codec->out_idx, &produced) : 0);

    TRACE(DECODE, _b("codec[%p]::postprocess(c=%u, p=%u, d=%u)"), codec, consumed, produced, done);

    /* ...input buffer maintenance; check if we consumed anything */
    if (consumed)
    {
        if(comp_type == XAF_ENCODER)
        {
            codec->consumed += consumed / codec->sample_size;
        }
        /* ...consume specified number of bytes from input port */
        xf_input_port_consume(&codec->input, consumed);

        /* ...clear input-setup flag */
        base->state ^= XA_CODEC_FLAG_INPUT_SETUP;
    }

    /* ...output buffer maintenance; check if we have produced anything */
    if (produced)
    {
        /* ...increment total number of produced samples (really don't like division here - tbd) */
        codec->produced += produced / codec->sample_size;

        /* ...immediately complete output buffer (don't wait until it gets filled) */
        xf_output_port_produce(&codec->output, produced);

        /* ...clear output port setup flag */
        base->state ^= XA_CODEC_FLAG_OUTPUT_SETUP;
    }

    /* ...process execution stage transition */
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
            XA_CHK(xa_codec_prepare_runtime(codec));

            /* ...clear output port setup flag as we were using scratch buffer;
             * technically, no need to repeat setup of input buffer, but some codecs require
             * it as well
             */
            base->state &= ~(XA_CODEC_FLAG_INPUT_SETUP | XA_CODEC_FLAG_OUTPUT_SETUP);
        }
        else
        {
            /* ...output stream is over; propagate condition to sink port */
            if (xf_output_port_flush(&codec->output, XF_FILL_THIS_BUFFER))
            {
                /* ...flushing sequence is not needed; complete pending zero-length input */
                xf_input_port_purge(&codec->input);
                
                /* ...no propagation to output port */
                TRACE(INFO, _b("codec[%p] playback completed"), codec);
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

    /* ...reschedule processing if needed */
    if (xf_input_port_ready(&codec->input) && xf_output_port_ready(&codec->output))
    {
        /* ...schedule data processing with respect to its urgency */
        if(comp_type == XAF_ENCODER)
        {
            xa_base_schedule(base, consumed * codec->factor);
        }
        else
        {
            xa_base_schedule(base, produced * codec->factor);
        }
    }

    return XA_NO_ERROR;
}

/* ...configuration parameter retrieval */
static XA_ERRORCODE xa_codec_getparam(XACodecBase *base, WORD32 id, pVOID value)
{
    XAAudioCodec   *codec = (XAAudioCodec *) base;

    if (id == XA_CODEC_CONFIG_PARAM_PRODUCED)
    {
        /* ...retrieve number of produced samples since last reset */
        *(UWORD32 *)value = codec->produced;

        return XA_NO_ERROR;
    }
    else
    {
        /* ...pass command to underlying codec plugin */
        return XA_API(base, XA_API_CMD_GET_CONFIG_PARAM, id, value);
    }
}

/*******************************************************************************
 * Component entry point
 ******************************************************************************/

/* ...command hooks */
static XA_ERRORCODE (* const xa_codec_cmd[])(XACodecBase *, xf_message_t *) =
{
    [XF_OPCODE_TYPE(XF_SET_PARAM)] = xa_base_set_param,
    [XF_OPCODE_TYPE(XF_GET_PARAM)] = xa_base_get_param,
    [XF_OPCODE_TYPE(XF_ROUTE)] = xa_codec_port_route,
    [XF_OPCODE_TYPE(XF_UNROUTE)] = xa_codec_port_unroute,
    [XF_OPCODE_TYPE(XF_EMPTY_THIS_BUFFER)] = xa_codec_empty_this_buffer,
    [XF_OPCODE_TYPE(XF_FILL_THIS_BUFFER)] = xa_codec_fill_this_buffer,
    [XF_OPCODE_TYPE(XF_FLUSH)] = xa_codec_flush,
    [XF_OPCODE_TYPE(XF_SET_PARAM_EXT)] = xa_base_set_param_ext,
    [XF_OPCODE_TYPE(XF_GET_PARAM_EXT)] = xa_base_get_param_ext,
};

/* ...total number of commands supported */
#define XA_CODEC_CMD_NUM        (sizeof(xa_codec_cmd) / sizeof(xa_codec_cmd[0]))

/* ...command processor for termination state (only for routed port case) */
static int xa_audio_codec_terminate(xf_component_t *component, xf_message_t *m)
{
    XAAudioCodec   *codec = (XAAudioCodec *) component;
    UWORD32             opcode = m->opcode;

    /* ...check if we received output port control message */
    if (m == xf_output_port_control_msg(&codec->output))
    {
        /* ...output port flushing complete; mark port is idle and terminate */
        xf_output_port_flush_done(&codec->output);
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
static int xa_audio_codec_destroy(xf_component_t *component, xf_message_t *m)
{
    XAAudioCodec   *codec = (XAAudioCodec *) component;
    UWORD32             core = xf_component_core(component);

    /* ...destroy input port */
    xf_input_port_destroy(&codec->input, core);

    /* ...destroy output port */
    xf_output_port_destroy(&codec->output, core);

    /* ...deallocate all resources */
    xa_base_destroy(&codec->base, XF_MM(sizeof(*codec)), core);

    TRACE(INIT, _b("audio-codec[%p@%u] destroyed"), codec, core);

    /* ...indicate the client has been destroyed */
    return 0;
}

/* ...audio codec destructor - first stage (ports unrouting) */
static int xa_audio_codec_cleanup(xf_component_t *component, xf_message_t *m)
{
    XAAudioCodec *codec = (XAAudioCodec *) component;

    /* ...complete message with error response */
    xf_response_err(m);

    /* ...cancel internal scheduling message if needed */
    xa_base_cancel(&codec->base);

    /* ...purge input port (returns OK? pretty strange at this point - tbd) */
    xf_input_port_purge(&codec->input);

    /* ...propagate unregister command to connected component */
    if (xf_output_port_flush(&codec->output, XF_FLUSH))
    {
        /* ...flushing sequence is not needed; destroy audio codec */
        return xa_audio_codec_destroy(component, NULL);
    }
    else
    {
        /* ...wait until output port is cleaned; adjust component hooks */
        component->entry = xa_audio_codec_terminate;
        component->exit = xa_audio_codec_destroy;

        TRACE(INIT, _b("codec[%p] cleanup sequence started"), codec);

        /* ...indicate that second stage is required */
        return 1;
    }
}

/*******************************************************************************
 * Audio codec component factory
 ******************************************************************************/

xf_component_t * xa_audio_codec_factory(UWORD32 core, xa_codec_func_t process, xaf_comp_type comp_type)
{
    XAAudioCodec   *codec;

    /* ...allocate local memory for codec structure */
    XF_CHK_ERR(codec = (XAAudioCodec *) xa_base_factory(core, XF_MM(sizeof(*codec)), process), NULL);

    /* ...set base codec API methods */
    codec->base.memtab = xa_codec_memtab;
    codec->base.preprocess = xa_codec_preprocess;
    codec->base.postprocess = xa_codec_postprocess;
    codec->base.getparam = xa_codec_getparam;

    /* ...set message commands processing table */
    codec->base.command = xa_codec_cmd;
    codec->base.command_num = XA_CODEC_CMD_NUM;

    /* ...set component destructor hook */
    codec->base.component.exit = xa_audio_codec_cleanup;

    /* ...set component type */
    codec->base.comp_type = comp_type;

    TRACE(INIT, _b("Codec[%p] initialized"), codec);

    return (xf_component_t *) codec;
}
