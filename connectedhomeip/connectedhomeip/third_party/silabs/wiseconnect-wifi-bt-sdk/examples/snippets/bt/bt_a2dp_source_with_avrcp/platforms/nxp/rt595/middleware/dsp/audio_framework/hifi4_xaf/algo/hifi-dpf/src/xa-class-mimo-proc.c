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
 * xa-class-mimo-proc.c
 *
 * Generic mimo process class
 ******************************************************************************/

#define MODULE_TAG                      MIMO

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "xf-dp.h"
#include "xa-class-base.h"
#include "audio/xa-mimo-proc-api.h"

/*******************************************************************************
 * Tracing tags
 ******************************************************************************/

TRACE_TAG(INIT, 1);
TRACE_TAG(WARNING, 1);
TRACE_TAG(INFO, 1);
TRACE_TAG(INPUT, 1);
TRACE_TAG(OUTPUT, 1);

/*******************************************************************************
 * External data structures
 ******************************************************************************/
extern const int xf_io_ports[XAF_MAX_COMPTYPE][2];

/*******************************************************************************
 * Constant definitions
 ******************************************************************************/

/* ...maximum in ports for mimo class */
#define XA_MIMO_PROC_MAX_IN_PORTS       XF_CFG_MAX_IN_PORTS

/* ...maximum out ports for mimo class */
#define XA_MIMO_PROC_MAX_OUT_PORTS      XF_CFG_MAX_OUT_PORTS

/*******************************************************************************
 * Data structures
 ******************************************************************************/

/* ...mimo-proc - input data */
typedef struct XAInTrack
{
    /* ...input port data */
    xf_input_port_t     input;

    /* ...current presentation timestamp (in samples; local to a mimo-proc state) */
    //UWORD32             pts;

    /* ...total amount of decoded frames since last synchronization point */
    //UWORD32           decoded;

    /* ...input port index */
    WORD32              idx;

    /* ...total amount of consumed frames since last synchronization point */
    UWORD32             consumed;

}   XAInTrack;

/* ...mimo-proc - output data */
typedef struct XAOutTrack
{
    /* ...output port data */
    xf_output_port_t     output;

    /* ...current presentation timestamp (in samples; local to a mimo-proc state) */
    //UWORD32             pts;

    /* ...total amount of decoded frames since last synchronization point */
    //UWORD32           decoded;

    /* ...output port index */
    WORD32              idx;

    /* ...total amount of produced frames since last synchronization point */
    UWORD32             produced;

}   XAOutTrack;

/*******************************************************************************
 * Helpers
 ******************************************************************************/

static inline UWORD32 xa_in_track_test_flags(XAInTrack *in_track, UWORD32 flags)
{
    return (in_track->input.flags & flags);
}

static inline UWORD32 xa_in_track_set_flags(XAInTrack *in_track, UWORD32 flags)
{
    return (in_track->input.flags |= flags);
}

static inline UWORD32 xa_in_track_clear_flags(XAInTrack *in_track, UWORD32 flags)
{
    return (in_track->input.flags &= ~flags);
}

static inline UWORD32 xa_in_track_toggle_flags(XAInTrack *in_track, UWORD32 flags)
{
    return (in_track->input.flags ^= flags);
}

static inline UWORD32 xa_out_track_test_flags(XAOutTrack *out_track, UWORD32 flags)
{
    return (out_track->output.flags & flags);
}

static inline UWORD32 xa_out_track_set_flags(XAOutTrack *out_track, UWORD32 flags)
{
    return (out_track->output.flags |= flags);
}

static inline UWORD32 xa_out_track_clear_flags(XAOutTrack *out_track, UWORD32 flags)
{
    return (out_track->output.flags &= ~flags);
}

static inline UWORD32 xa_out_track_toggle_flags(XAOutTrack *out_track, UWORD32 flags)
{
    return (out_track->output.flags ^= flags);
}

/*******************************************************************************
 * Mimo-proc data definitions
 ******************************************************************************/

/* ...mimo-proc data */
typedef struct XAMimoProc
{
    /***************************************************************************
     * Control data
     **************************************************************************/

    /* ...generic audio codec data */
    XACodecBase         base;

    /* ...input ports or tracks */
    XAInTrack             in_track[XA_MIMO_PROC_MAX_IN_PORTS];
    
    /* ...output ports or tracks */
    XAOutTrack            out_track[XA_MIMO_PROC_MAX_OUT_PORTS];

    /***************************************************************************
     * Run-time configuration parameters
     **************************************************************************/

    /* ...sample size in bytes */
    UWORD32                     sample_size;

    /* ...audio sample duration */
    UWORD32                     factor;

    /* ...audio frame size in samples */
    //UWORD32                 frame_size;

    /* ...audio frame duration */
    //UWORD32                 frame_duration;
    
    /* ...number of input porst or tracks */
    UWORD32                 num_in_ports;
    
    /* ...number of input porst or tracks */
    UWORD32                 num_out_ports;

}   XAMimoProc;

/*******************************************************************************
 * Output Track state flags
 ******************************************************************************/

/* ...output track is idle (will autostart as soon as output buffer is received) */
#define XA_OUT_TRACK_FLAG_IDLE              __XF_OUTPUT_FLAG(1 << 0)

/* ...output track is active */
#define XA_OUT_TRACK_FLAG_ACTIVE            __XF_OUTPUT_FLAG(1 << 1)

/* ...output track is routed */
#define XA_OUT_TRACK_FLAG_ROUTED            __XF_OUTPUT_FLAG(1 << 2)

/* ...output track setup completed */
#define XA_OUT_TRACK_FLAG_OUTPUT_SETUP      __XF_OUTPUT_FLAG(1 << 3)

/* ...output track flushing */
#define XA_OUT_TRACK_FLAG_FLUSHING          __XF_OUTPUT_FLAG(1 << 4)

/* ...output track setup flushing completed */
#define XA_OUT_TRACK_FLAG_FLUSHING_DONE     __XF_OUTPUT_FLAG(1 << 5)

/*******************************************************************************
 * Input Track state flags
 ******************************************************************************/

/* ...input track is idle (will autostart as soon as input data received) */
#define XA_IN_TRACK_FLAG_IDLE              __XF_INPUT_FLAG(1 << 0)

/* ...input track is rendered */
#define XA_IN_TRACK_FLAG_ACTIVE            __XF_INPUT_FLAG(1 << 1)

/* ...input track is paused */
#define XA_IN_TRACK_FLAG_PAUSED            __XF_INPUT_FLAG(1 << 2)

/* ...input track input port is setup */
#define XA_IN_TRACK_FLAG_INPUT_SETUP       __XF_INPUT_FLAG(1 << 3)

/* ...input track has received data */
#define XA_IN_TRACK_FLAG_RECVD_DATA        __XF_INPUT_FLAG(1 << 4)

/*******************************************************************************
 * Helper functions
 ******************************************************************************/
/* ...Count the input tracks that have received data or are active*/
static inline UWORD32 xa_mimo_proc_check_active(XAMimoProc *mimo_proc)
{
    XAInTrack      *track;
    UWORD32        i;
    UWORD32        cnt = 0;
    
    for (track = &mimo_proc->in_track[i = 0]; i < mimo_proc->num_in_ports; i++, track++)
    {
        if (xa_in_track_test_flags(track, XA_IN_TRACK_FLAG_RECVD_DATA | XA_IN_TRACK_FLAG_ACTIVE))
            cnt++;
    }
    return cnt;
}

static inline UWORD32 xa_mimo_proc_input_port_ready(XAMimoProc *mimo_proc)
{
    XAInTrack      *track;
    UWORD32        i;
    
    for (track = &mimo_proc->in_track[i = 0]; i < mimo_proc->num_in_ports; i++, track++)
    {
        /* ...tbd - check only XA_IN_TRACK_FLAG_ACTIVE here? */
        if (xa_in_track_test_flags(track, XA_IN_TRACK_FLAG_RECVD_DATA | XA_IN_TRACK_FLAG_ACTIVE) && !xf_input_port_ready(&track->input))
        {
            return 0;
        }
    }
    return 1;
}

static inline UWORD32 xa_mimo_proc_output_port_ready(XAMimoProc *mimo_proc)
{
    XAOutTrack  *track;
    UWORD32     i;

    for (track = &mimo_proc->out_track[i = 0]; i < mimo_proc->num_out_ports; i++, track++)
    {
        /* ...TBD check if any output port is active (routed) */
        if (!xf_output_port_ready(&track->output))
            return 0;
    }
    return 1;
}

static inline UWORD32 xa_mimo_proc_output_port_flush_done(XAMimoProc *mimo_proc)
{
    XAOutTrack  *track;
    UWORD32     i;
    UWORD32        cnt = 0;

    for (track = &mimo_proc->out_track[i = 0]; i < mimo_proc->num_out_ports; i++, track++)
    {
        if (xa_out_track_test_flags(track, XA_OUT_TRACK_FLAG_FLUSHING_DONE))
            cnt++;
    }
    return (cnt == mimo_proc->num_out_ports);
}

/* ...prepare mimo_proc for steady operation */
static inline XA_ERRORCODE xa_mimo_proc_prepare_runtime(XAMimoProc *mimo_proc)
{
    XACodecBase    *base = (XACodecBase *) mimo_proc;
    xf_message_t   *m = xf_msg_dequeue(&mimo_proc->out_track[0].output.queue);
    xf_start_msg_t *msg = m->buffer;
    UWORD32             frame_size;
    UWORD32             factor;
    UWORD32             i;

    /* ...memset the start-msg buffer to zero */
    memset(msg, 0, sizeof(xf_start_msg_t));   
   
    /* ...query mimo_proc parameters */
    XA_API(base, XA_API_CMD_GET_CONFIG_PARAM, XA_MIMO_PROC_CONFIG_PARAM_SAMPLE_RATE, &msg->sample_rate);
    XA_API(base, XA_API_CMD_GET_CONFIG_PARAM, XA_MIMO_PROC_CONFIG_PARAM_CHANNELS, &msg->channels);
    XA_API(base, XA_API_CMD_GET_CONFIG_PARAM, XA_MIMO_PROC_CONFIG_PARAM_PCM_WIDTH, &msg->pcm_width);

    for (i=0; i<mimo_proc->num_in_ports; i++)
    {
        XA_API(base, XA_API_CMD_GET_MEM_INFO_SIZE, mimo_proc->in_track[i].idx, &msg->input_length[i]);
    }
    for (i=0; i<mimo_proc->num_out_ports; i++)
    {
        XA_API(base, XA_API_CMD_GET_MEM_INFO_SIZE, mimo_proc->out_track[i].idx, &msg->output_length[i]);
    }

    /* ...save sample size in bytes */
    mimo_proc->sample_size = msg->channels * (msg->pcm_width == 16 ? 2 : 4);

    /* ...calculate frame duration; get number of samples in the frame (don't like division here - tbd) */
    frame_size = msg->output_length[0] / mimo_proc->sample_size;

    /* ...it must be a multiple */
    XF_CHK_ERR(frame_size * mimo_proc->sample_size == msg->output_length[0], XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...calculate mimo-proc frame duration; get upsample factor */
    XF_CHK_ERR(factor = xf_timebase_factor(msg->sample_rate), XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...set frame duration factor (converts number of bytes into timebase units) */
    mimo_proc->factor = factor / mimo_proc->sample_size;
    
    TRACE(INIT, _b("ts-factor: %u (%u)"), mimo_proc->factor, factor);

    BUG(mimo_proc->factor * mimo_proc->sample_size != factor, _x("Freq mismatch: %u vs %u"), mimo_proc->factor * mimo_proc->sample_size, factor);

    /* ...pass response to caller */
    xf_response_data(m, sizeof(*msg));
   
    /* ...tbd - more checks here? */ 
    {
        UWORD32             i;
        for (i=1; i<mimo_proc->num_out_ports; i++)
        {
            m = xf_msg_dequeue(&mimo_proc->out_track[i].output.queue);

            if (m != NULL)
            {      
                /* ...pass response to caller */
                xf_response_data(m, sizeof(*msg));
            }
        }
    } 

    return XA_NO_ERROR;
}

/*******************************************************************************
 * Commands handlers
 ******************************************************************************/

/* ...EMPTY-THIS-BUFFER command processing */
static XA_ERRORCODE xa_mimo_proc_empty_this_buffer(XACodecBase *base, xf_message_t *m)
{
    XAMimoProc *mimo_proc = (XAMimoProc *) base;
    UWORD32    i          = XF_MSG_DST_PORT(m->id);
    XAInTrack  *track     = &mimo_proc->in_track[i];

    /* ...make sure the port is valid */
    XF_CHK_ERR(i < mimo_proc->num_in_ports, XA_API_FATAL_INVALID_CMD_TYPE);
    
    /* ...command is allowed only in "postinit" state */
    XF_CHK_ERR(base->state & XA_BASE_FLAG_POSTINIT, XA_API_FATAL_INVALID_CMD);

    TRACE(INPUT, _b("in_track-%u: received buffer [%p]:%u"), i, m->buffer, m->length);

    /* ...update received data for the track */
    if (m->length)
        xa_in_track_set_flags(track, XA_IN_TRACK_FLAG_RECVD_DATA);
    else
        xa_in_track_clear_flags(track, XA_IN_TRACK_FLAG_RECVD_DATA);
    
    /* ...place received message into track input port */
    if (xf_input_port_put(&track->input, m))
    {
        /* ...restart stream if it is in completed state */
        if (base->state & XA_BASE_FLAG_COMPLETED)
        {
            /* ...reset execution stage */
            base->state = XA_BASE_FLAG_POSTINIT | XA_BASE_FLAG_EXECUTION;

            /* ...reset execution runtime */
            XA_API(base, XA_API_CMD_EXECUTE, XA_CMD_TYPE_DO_RUNTIME_INIT, NULL);

            /* ...tbd - reset produced samples counter, required? */
        }

        /* ...process track autostart if needed */
        if (xa_in_track_test_flags(track, XA_IN_TRACK_FLAG_IDLE))
        {
            /* ...put track into active state */
            xa_in_track_toggle_flags(track, XA_IN_TRACK_FLAG_IDLE | XA_IN_TRACK_FLAG_ACTIVE);
            
            /* ...save track presentation timestamp */
            //track->pts = mimo_proc->pts;

            TRACE(INFO, _b("in_track-%u started"), i);
        }
        
        /* ...schedule data processing if all active (routed) output ports are available */
        if (xa_mimo_proc_output_port_ready(mimo_proc))
        {
            /* ...force data processing */
            xa_base_schedule(base, 0);
        }
    }
    
    return XA_NO_ERROR;
}

/* ...FILL-THIS-BUFFER command processing */
static XA_ERRORCODE xa_mimo_proc_fill_this_buffer(XACodecBase *base, xf_message_t *m)
{
    XAMimoProc *mimo_proc = (XAMimoProc *) base;
    UWORD32    i          = XF_MSG_DST_PORT(m->id) - mimo_proc->num_in_ports;
    XAOutTrack  *track    = &mimo_proc->out_track[i];
    
    /* ...make sure the port is valid */
    XF_CHK_ERR(i < mimo_proc->num_out_ports, XA_API_FATAL_INVALID_CMD_TYPE);
    
    /* ...command is allowed only in "postinit" state */
    XF_CHK_ERR(base->state & XA_BASE_FLAG_POSTINIT, XA_API_FATAL_INVALID_CMD);

    /* ...process runtime initialization explicitly */
    if (base->state & XA_BASE_FLAG_RUNTIME_INIT)
    {
        /* ...message must be zero-length */
        XF_CHK_ERR(m->length == 0, XA_MIMO_PROC_EXEC_FATAL_STATE);
    }
    else if (m == xf_output_port_control_msg(&track->output))
    {
        /* ...end-of-stream processing indication received; check the state */
        BUG((base->state & XA_BASE_FLAG_COMPLETED) == 0, _x("invalid state: %x"), base->state);

        /* ... mark flushing sequence is done */
        xf_output_port_flush_done(&track->output);

        /* ... mark flushing sequence is done locally also */
        xa_out_track_set_flags(track, XA_OUT_TRACK_FLAG_FLUSHING_DONE);
        
        TRACE(INFO, _b("out_track-%u flushed"), i);

        if (xa_mimo_proc_output_port_flush_done(mimo_proc))
        {
            XAInTrack *in_track;

            for (in_track = &mimo_proc->in_track[i = 0]; i < mimo_proc->num_in_ports; i++, in_track++)
            {
                /* ...input stream is over; return zero-length input back to caller */
                xf_input_port_purge(&in_track->input);
            }
        }

        return XA_NO_ERROR;
    }
    else if ((base->state & XA_BASE_FLAG_COMPLETED) && !xf_output_port_routed(&track->output))
    {
        /* ...return message arrived from application immediately */
        xf_response_ok(m);

        return XA_NO_ERROR;
    }
    else if (m->length != 0) /* ...EOS response */
    {
        /* ...adjust message length (may be shorter than original) */
        m->length = track->output.length;
    }

    if (xf_output_port_put(&track->output, m))
    {
        /* ...process track autostart if needed */
        if (xa_out_track_test_flags(track, XA_OUT_TRACK_FLAG_IDLE))
        {
            /* ...put track into active state */
            xa_out_track_toggle_flags(track, XA_OUT_TRACK_FLAG_IDLE | XA_OUT_TRACK_FLAG_ACTIVE);

            /* ...save track presentation timestamp */
            //track->pts = mimo_proc->pts;

            TRACE(INFO, _b("out_track-%u started"), i);
        }

        TRACE(OUTPUT, _b("received output buffer track-%u [%p]:%u"), i, m->buffer, m->length);

        /* ...put message into output port */
        if (xa_mimo_proc_output_port_ready(mimo_proc))
        {
            /* ...force data processing */
            xa_base_schedule(base, 0);
        }
    }

    return XA_NO_ERROR;
}

/* ...output port routing */
static XA_ERRORCODE xa_mimo_proc_port_route(XACodecBase *base, xf_message_t *m)
{
    XAMimoProc             *mimo_proc = (XAMimoProc *) base;
    xf_route_port_msg_t    *cmd = m->buffer;
    UWORD32                 src = XF_MSG_DST(m->id);
    UWORD32                 dst = cmd->dst;
    UWORD32                 i   = XF_MSG_DST_PORT(m->id) - mimo_proc->num_in_ports;
    xf_output_port_t       *port = &mimo_proc->out_track[i].output;
    
    /* ...command is allowed only in "postinit" state */
    XF_CHK_ERR(base->state & XA_BASE_FLAG_POSTINIT, XA_API_FATAL_INVALID_CMD);

    /* ...make sure sane output port is addressed */
    XF_CHK_ERR(i < mimo_proc->num_out_ports, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...make sure port is not routed yet */
    XF_CHK_ERR(!xf_output_port_routed(port), XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...route output port - allocate queue */
    XF_CHK_ERR(xf_output_port_route(port, __XF_MSG_ID(dst, src), cmd->alloc_number, cmd->alloc_size, cmd->alloc_align) == 0, XA_API_FATAL_MEM_ALLOC);

    /* ...set routed flag */
    xa_out_track_set_flags(&mimo_proc->out_track[i], XA_OUT_TRACK_FLAG_ROUTED);

    /* ...schedule processing instantly - tbd - check if we have anything pending on input */
    /* ...TBD - do we need to check if other output ports are ready? */
    xa_base_schedule(base, 0);
    
    /* ...pass success result to caller */
    xf_response_ok(m);
    
    return XA_NO_ERROR;
}

/* ...port unroute command */
static XA_ERRORCODE xa_mimo_proc_port_unroute(XACodecBase *base, xf_message_t *m)
{
    XAMimoProc         *mimo_proc = (XAMimoProc *) base;
    UWORD32             i = XF_MSG_DST_PORT(m->id) - mimo_proc->num_in_ports;
    xf_output_port_t   *port = &mimo_proc->out_track[i].output;
    
    /* ...command is allowed only in "postinit" state */
    XF_CHK_ERR(base->state & XA_BASE_FLAG_POSTINIT, XA_API_FATAL_INVALID_CMD);

    /* ...make sure sane output port is addressed */
    XF_CHK_ERR(i < mimo_proc->num_out_ports, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...cancel any pending processing */
    xa_base_cancel(base);

    /* ...clear output-port-setup condition */
    xa_out_track_clear_flags(&mimo_proc->out_track[i], XA_OUT_TRACK_FLAG_OUTPUT_SETUP);

    /* ...pass flush command down the graph */
    if (xf_output_port_flush(port, XF_FLUSH))
    {
        TRACE(INFO, _b("port is idle; instantly unroute"));

        /* ...flushing sequence is not needed; command may be satisfied instantly */
        xf_output_port_unroute(port);

        /* ...pass response to the proxy */
        xf_response_ok(m);
    }
    else
    {
        TRACE(INFO, _b("port is busy; propagate unroute command"));

        /* ...flushing sequence is started; save flow-control message */
        xf_output_port_unroute_start(port, m);
    }

    return XA_NO_ERROR;
}

/* ...FLUSH command processing */
static XA_ERRORCODE xa_mimo_proc_flush(XACodecBase *base, xf_message_t *m)
{
    XAMimoProc *mimo_proc = (XAMimoProc *) base;
    UWORD32     i = XF_MSG_DST_PORT(m->id);

    /* ...command is allowed only in "postinit" state */
    XF_CHK_ERR(base->state & XA_BASE_FLAG_POSTINIT, XA_API_FATAL_INVALID_CMD);

    /* ...make sure the buffer is empty */
    XF_CHK_ERR(m->length == 0, XA_API_FATAL_INVALID_CMD_TYPE);

    TRACE(1, _b("flush command received"));

    /* ...if flush command is addressed to input port */
    if (i < mimo_proc->num_in_ports)
    {
        XAInTrack  *in_track = &mimo_proc->in_track[i];

        /* ...input port flushing; check the track state is valid */
        if (xa_in_track_test_flags(in_track, XA_IN_TRACK_FLAG_ACTIVE | XA_IN_TRACK_FLAG_PAUSED))
        {
            /* ...purge input port */
            xf_input_port_purge(&in_track->input);

            /* ...force clearing of ACTIVE and INPUT_SETUP condition */
            xa_in_track_clear_flags(in_track, XA_IN_TRACK_FLAG_ACTIVE | XA_IN_TRACK_FLAG_PAUSED | XA_IN_TRACK_FLAG_INPUT_SETUP);

            /* ...and enter into idle state */
            xa_in_track_set_flags(in_track, XA_IN_TRACK_FLAG_IDLE);

            /* ...other tracks may be waiting for this track, so force data processing */
            if (xa_mimo_proc_output_port_ready(mimo_proc))
            {
                xa_base_schedule(base, 0);
            }

            TRACE(INFO, _b("mimo_proc[%p]::in_track[%u] flushed"), mimo_proc, i);
        }
        
        /* ...complete message instantly (no propagation to output port) */
        xf_response(m);
    }
    else 
    {
        XAOutTrack  *out_track; 

        i = XF_MSG_DST_PORT(m->id) - mimo_proc->num_in_ports;
        out_track = &mimo_proc->out_track[i];
        
        /* ...make sure the port is valid */
        XF_CHK_ERR(i < mimo_proc->num_out_ports, XA_API_FATAL_INVALID_CMD_TYPE);

        if (xf_output_port_unrouting(&out_track->output))
        {
            /* ...flushing during port unrouting; complete unroute sequence */
            xf_output_port_unroute_done(&out_track->output);

            TRACE(INFO, _b("port is unrouted"));
        }
        else
        {
            /* ...output port flush command/response; check if the port is routed */
            if (!xf_output_port_routed(&out_track->output))
            {
                /* ...complete all queued messages */
                xf_output_port_flush(&out_track->output, XF_FLUSH);

                /* ...and pass response to flushing command */
                xf_response(m);
            }
            else
            {            
                /* ...response to flushing command received */
                BUG(m != xf_output_port_control_msg(&out_track->output), _x("invalid message: %p"), m);

                /* ...mark flushing sequence is completed */
                xf_output_port_flush_done(&out_track->output);

                /* ... mark flushing sequence is done locally also */
                xa_out_track_set_flags(out_track, XA_OUT_TRACK_FLAG_FLUSHING_DONE);

            }

            /* ...complete original flow-control command */
            if (xa_mimo_proc_output_port_flush_done(mimo_proc))
            {
                XAInTrack *in_track;

                for (in_track = &mimo_proc->in_track[i = 0]; i < mimo_proc->num_in_ports; i++, in_track++)
                {
                    /* ...input stream is over; return zero-length input back to caller */
                    xf_input_port_purge(&in_track->input);
                }
            }

            /* ...clear output-setup condition */
            xa_out_track_clear_flags(out_track, XA_OUT_TRACK_FLAG_OUTPUT_SETUP);
        }
    }

    return XA_NO_ERROR;
}

/*******************************************************************************
 * Codec API implementation
 ******************************************************************************/

/* ...buffers handling */
static XA_ERRORCODE xa_mimo_proc_memtab(XACodecBase *base, WORD32 idx, WORD32 type, WORD32 size, WORD32 align, UWORD32 core)
{
    XAMimoProc    *mimo_proc = (XAMimoProc *)base;
    UWORD32        i;
    
    if (type == XA_MEMTYPE_INPUT)
    {
        XAInTrack    *track;

        for (track = &mimo_proc->in_track[i = 0]; i < XA_MIMO_PROC_MAX_IN_PORTS; i++, track++)
        {
            /* ...get next available track */
            if (!xa_in_track_test_flags(track, XA_IN_TRACK_FLAG_IDLE))
                break;
        }

        /* ...input buffer allocation; check track number is sane */
        XF_CHK_ERR(i < mimo_proc->num_in_ports, XA_API_FATAL_INVALID_CMD_TYPE);

        /* ...create input port for a track */
        XF_CHK_ERR(xf_input_port_init(&track->input, size, align, core) == 0, XA_API_FATAL_MEM_ALLOC);

        /* ...set input port buffer */
        XA_API(base, XA_API_CMD_SET_MEM_PTR, idx, track->input.buffer);

        /* ...store idx in port state */
        track->idx = idx;
    
        /* ...put track into idle state (will start as soon as we receive data) */
        xa_in_track_set_flags(track, XA_IN_TRACK_FLAG_IDLE);
        
        TRACE(INIT, _b("mimo_proc[%p]::in_track[%u] input port created - size=%u"), mimo_proc, i, size);
    }
    else
    {
        XAOutTrack    *track;

        /* ...output buffer allocation */
        XF_CHK_ERR(type == XA_MEMTYPE_OUTPUT, XA_API_FATAL_INVALID_CMD_TYPE);

        for (track = &mimo_proc->out_track[i = 0]; i < XA_MIMO_PROC_MAX_OUT_PORTS; i++, track++)
        {
            /* ...get next available track */
            if (!xa_out_track_test_flags(track, XA_OUT_TRACK_FLAG_IDLE))
                break;
        }

        /* ...check output track number is sane */
        XF_CHK_ERR(i < mimo_proc->num_out_ports, XA_API_FATAL_INVALID_CMD_TYPE);

        /* ...create output port for a track */
        XF_CHK_ERR(xf_output_port_init(&track->output, size) == 0, XA_API_FATAL_MEM_ALLOC);

        /* ...store idx in port state */
        track->idx = idx;

        /* ...put track into idle state (will start as soon as we receive output buffer) */
        xa_out_track_set_flags(track, XA_OUT_TRACK_FLAG_IDLE);
        
        TRACE(INIT, _b("mimo_proc[%p]::out_track[%u] output port created - size=%u"), mimo_proc, i, size);
    }

    return XA_NO_ERROR;
}

/* ...preprocessing function */
static XA_ERRORCODE xa_mimo_proc_preprocess(XACodecBase *base)
{
    XAMimoProc     *mimo_proc = (XAMimoProc *) base;
    XAInTrack      *in_track;
    XAOutTrack     *out_track;
    UWORD8          i;
    XA_ERRORCODE    e = XA_MIMO_PROC_EXEC_NONFATAL_NO_DATA;

    /* ...setup output buffer pointers */
    for (out_track = &mimo_proc->out_track[i = 0]; i < mimo_proc->num_out_ports; i++, out_track++)
    {
        /* ...skip idle output ports */
        if (!xa_out_track_test_flags(out_track, XA_OUT_TRACK_FLAG_ACTIVE))  continue;

        /* ...prepare output buffer */
        if (!xa_out_track_test_flags(out_track, XA_OUT_TRACK_FLAG_OUTPUT_SETUP))
        {
            void   *output;

            /* ...set output buffer pointer */
            if (base->state & XA_BASE_FLAG_RUNTIME_INIT)
            {
                /* ...no actual data processing during initialization */
                return XA_NO_ERROR;
            }
            else if ((output = xf_output_port_data(&out_track->output)) == NULL)
            {
                /* ...no output buffer available */
                return e;
            }

            /* ...set output buffer pointer */
            XA_API(base, XA_API_CMD_SET_MEM_PTR, out_track->idx, output);

            /* ...mark output port is setup */
            xa_out_track_set_flags(out_track, XA_OUT_TRACK_FLAG_OUTPUT_SETUP);
        }
    }
   
    /* ...setup input buffer pointers and length */
    for (in_track = &mimo_proc->in_track[i = 0]; i < mimo_proc->num_in_ports; i++, in_track++)
    {
        /* ...skip tracks that are not active */
        if (!xa_in_track_test_flags(in_track, XA_IN_TRACK_FLAG_ACTIVE))  continue;

        /* ...set temporary mimo_proc request */
        e = XA_NO_ERROR;

        /* ...skip the tracks that has been setup already */
        if (xa_in_track_test_flags(in_track, XA_IN_TRACK_FLAG_INPUT_SETUP))  continue;

        {
            UWORD32     filled;
#if 0            
            /* ...take actual data from input port (mimo_proc is always using internal buffer) */
            if (xf_input_port_done(&in_track->input) || xf_input_port_fill(&in_track->input))
            {
                /* ...retrieve number of bytes available */
                filled = xf_input_port_level(&in_track->input);
            }
            else
            {
                /* ...failed to prefill input buffer - no sufficient data yet */
                return XA_MIMO_PROC_EXEC_NONFATAL_NO_DATA;
            }
#else
            if (!xf_input_port_done(&in_track->input))
            {
                /* ...take actual data from input port (mimo_proc is always using internal buffer) */
                xf_input_port_fill(&in_track->input);
            }

            /* ...retrieve number of bytes available */
            filled = xf_input_port_level(&in_track->input);
           
            /* ...allow partially filled inputs to components */ 
            if (!xf_input_port_done(&in_track->input) && !filled)
            {
                return XA_MIMO_PROC_EXEC_NONFATAL_NO_DATA;
            }

#endif

            /* ...check if input stream is over */
            if (xf_input_port_done(&in_track->input))
            {
                /* ...pass input-over command to the codec to indicate the final buffer */
                XA_API(base, XA_API_CMD_INPUT_OVER, in_track->idx, NULL);

                TRACE(INFO, _b("mimo_proc[%p]:in_track[%u] signal input-over (filled: %u)"), mimo_proc, i, filled);
            }

            /* ...set total number of bytes we have in buffer */
            XA_API(base, XA_API_CMD_SET_INPUT_BYTES, in_track->idx, &filled);

            /* ...actual data is to be played */
            TRACE(INPUT, _b("in_track-%u: filled %u bytes"), i, filled);
        }
        
        /* ...mark the track input is setup */
        xa_in_track_set_flags(in_track, XA_IN_TRACK_FLAG_INPUT_SETUP);
    }

    /* ...do mimo_proc operation only when all active tracks are setup */
    return e;
}

/* ...postprocessing function */
static XA_ERRORCODE xa_mimo_proc_postprocess(XACodecBase *base, int done)
{
    XAMimoProc     *mimo_proc = (XAMimoProc *) base;
    XAInTrack      *in_track;
    XAOutTrack     *out_track;
    UWORD32         produced = 0;
    UWORD32         consumed = 0;
    UWORD32         output_produced = 0;
    UWORD8          i;

    /* ...input ports maintenance; process all tracks */
    for (in_track = &mimo_proc->in_track[i = 0]; i < mimo_proc->num_in_ports; i++, in_track++)
    {
        /* ...skip the tracks that are not runing */
        if (!xa_in_track_test_flags(in_track, XA_IN_TRACK_FLAG_ACTIVE))  continue;

        /* ...get total amount of consumed bytes */
        XA_API(base, XA_API_CMD_GET_CURIDX_INPUT_BUF, in_track->idx, &consumed);

        TRACE(INPUT, _b("in_track-%u::postprocess(c=%u)"), i, consumed);

        if (consumed)
        {
            /* ...consume that amount from input port */
            xf_input_port_consume(&in_track->input, consumed);
            
            /* ...clear input setup flag */
            xa_in_track_clear_flags(in_track, XA_IN_TRACK_FLAG_INPUT_SETUP);
        }
        /* ...check if port is not completely filled - insufficient data */
        else if (in_track->input.length != xf_input_port_level(&in_track->input))
        {
            /* ...clear input setup flag */
            xa_in_track_clear_flags(in_track, XA_IN_TRACK_FLAG_INPUT_SETUP);
        }

        /* ...check if input port is done */
        if (xf_input_port_done(&in_track->input) && !xf_input_port_level(&in_track->input))
        {
            /* ...switch to idle state */
            xa_in_track_toggle_flags(in_track, XA_IN_TRACK_FLAG_ACTIVE | XA_IN_TRACK_FLAG_IDLE);

            TRACE(INFO, _b("mimo_proc[%p]::in_track[%u] completed"), mimo_proc, i);

            /* ...check if this is not last active input port */
            if (xa_mimo_proc_check_active(mimo_proc))
            {
                /* ...not last active input port - no need to wait for output flush, purge instantly */
                xf_input_port_purge(&in_track->input);
            }
        }
    }

    /* ...output ports maintenance; process all tracks */
    for (out_track = &mimo_proc->out_track[i = 0]; i < mimo_proc->num_out_ports; i++, out_track++)
    {
        /* ...skip idle output ports */
        if (!xa_out_track_test_flags(out_track, XA_OUT_TRACK_FLAG_ACTIVE))  continue;

        /* ...check if we have produced anything, only if runtime is initialized */
        (mimo_proc->sample_size ? XA_API(base, XA_API_CMD_GET_OUTPUT_BYTES, out_track->idx, &produced) : 0);

        TRACE(OUTPUT, _b("mimo_proc[%p]::out_track[%u] postprocess(p=%u, done=%u)"), mimo_proc, i, produced, done);

        if (produced)
        {
            /* ...all ports need not produce same length - tbd */
            if (output_produced) 
            {
                /* ...make sure we have produced same length on all output ports */
                BUG((produced != output_produced), _x("Invalid output lengths across ports: %u != %u"), produced, output_produced);
            }
            else 
            {
                output_produced = produced;
            }

            /* ...push data from output port */
            xf_output_port_produce(&out_track->output, produced);

            /* ...clear output-setup condition */
            xa_out_track_clear_flags(out_track, XA_OUT_TRACK_FLAG_OUTPUT_SETUP);
        }    
    }

    /* ...process execution stage transitions */
    if (done)
    {
        if (base->state & XA_BASE_FLAG_RUNTIME_INIT)
        {
            /* ...failed to initialize runtime (can't be? - tbd)*/
            BUG(1, _x("breakpoint"));
        }
        else if (base->state & XA_BASE_FLAG_EXECUTION)
        {
            /* ...enter into execution state; initialize runtime */
            return XA_CHK(xa_mimo_proc_prepare_runtime(mimo_proc));
        }
        else
        {

            for (out_track = &mimo_proc->out_track[i = 0]; i < mimo_proc->num_out_ports; i++, out_track++)
            {
                /* ...output stream is over; propagate condition to sink port */
                if (xf_output_port_flush(&out_track->output, XF_FILL_THIS_BUFFER))
                {
                    /* ...flushing sequence is not needed; complete pending zero-length input */
                    xa_out_track_set_flags(out_track, XA_OUT_TRACK_FLAG_FLUSHING_DONE); 

                    /* ...no propagation to output port */
                    TRACE(INFO, _b("mimo_proc[%p]:out_track[%u] playback completed"), mimo_proc, i);
                }
                else
                {
                    /* ...flushing sequence is started; wait until flow-control message returns */
                    xa_out_track_set_flags(out_track, XA_OUT_TRACK_FLAG_FLUSHING); 
                    TRACE(INFO, _b("mimo_proc[%p]:out_track[%u] propagate end-of-stream condition"), mimo_proc, i);
                }

            }
            
            if (xa_mimo_proc_output_port_flush_done(mimo_proc))
            {
                for (in_track = &mimo_proc->in_track[i = 0]; i < mimo_proc->num_in_ports; i++, in_track++)
                {
                    /* ...input stream is over; return zero-length input back to caller */
                    xf_input_port_purge(&in_track->input);
                }
            }
        }
    }

    /* ...reschedule data processing if there is a pending output message */
    if (xa_mimo_proc_input_port_ready(mimo_proc) && xa_mimo_proc_output_port_ready(mimo_proc))
    {
        /* ...schedule execution with respect to urgency */
        xa_base_schedule(base, output_produced * mimo_proc->factor);
    }

    return XA_NO_ERROR;
}

/*******************************************************************************
 * Command-processing function
 ******************************************************************************/

/* ...command hooks */
static XA_ERRORCODE (* const xa_mimo_proc_cmd[])(XACodecBase *, xf_message_t *) =
{
    /* ...set-parameter - actually, same as in generic case */
    [XF_OPCODE_TYPE(XF_SET_PARAM)] = xa_base_set_param,
    [XF_OPCODE_TYPE(XF_GET_PARAM)] = xa_base_get_param,

    /* ...output port routing/unrouting */
    [XF_OPCODE_TYPE(XF_ROUTE)] = xa_mimo_proc_port_route,
    [XF_OPCODE_TYPE(XF_UNROUTE)] = xa_mimo_proc_port_unroute,

    /* ...input/output buffers processing */
    [XF_OPCODE_TYPE(XF_EMPTY_THIS_BUFFER)] = xa_mimo_proc_empty_this_buffer,
    [XF_OPCODE_TYPE(XF_FILL_THIS_BUFFER)] = xa_mimo_proc_fill_this_buffer,
    [XF_OPCODE_TYPE(XF_FLUSH)] = xa_mimo_proc_flush,

};

/* ...total number of commands supported */
#define XA_MIMO_PROC_CMD_NUM        (sizeof(xa_mimo_proc_cmd) / sizeof(xa_mimo_proc_cmd[0]))

/*******************************************************************************
 * Entry points
 ******************************************************************************/

/* ...mimo_proc termination-state command processor */
static int xa_mimo_proc_terminate(xf_component_t *component, xf_message_t *m)
{
    XAMimoProc  *mimo_proc = (XAMimoProc *) component;
    UWORD32      opcode = m->opcode;
    UWORD32      i = XF_MSG_DST_PORT(m->id) - mimo_proc->num_in_ports;
    XAOutTrack  *track = &mimo_proc->out_track[i];
    
    /* ...make sure the port is valid */
    XF_CHK_ERR(i < mimo_proc->num_out_ports, XA_API_FATAL_INVALID_CMD_TYPE);
    
    if (m == xf_output_port_control_msg(&track->output))
    {
        /* ...output port flushing complete; mark port is idle and terminate */
        xf_output_port_flush_done(&track->output);
        return -1;
    }
    else if (opcode == XF_FILL_THIS_BUFFER && xf_output_port_routed(&track->output))
    {
        /* ...output buffer returned by the sink component; ignore and keep waiting */
        TRACE(OUTPUT, _b("collect output buffer"));
        return 0;
    }
    else if (opcode == XF_UNREGISTER)
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

/* ...mimo_proc class destructor */
static int xa_mimo_proc_destroy(xf_component_t *component, xf_message_t *m)
{
    XAMimoProc *mimo_proc = (XAMimoProc *) component;
    UWORD32     core = xf_component_core(component);
    UWORD32     i;
    
    /* ...destroy all input ports */
    for (i = 0; i < XA_MIMO_PROC_MAX_IN_PORTS; i++)
    {
        xf_input_port_destroy(&mimo_proc->in_track[i].input, core);
    }

    /* ...destroy all output ports */
    for (i = 0; i < XA_MIMO_PROC_MAX_OUT_PORTS; i++)
    {
        xf_output_port_destroy(&mimo_proc->out_track[i].output, core);
    }

    /* ...destroy base object */
    xa_base_destroy(&mimo_proc->base, XF_MM(sizeof(*mimo_proc)), core);

    TRACE(INIT, _b("mimo_proc[%p] destroyed"), mimo_proc);

    return 0;
}

/* ...mimo_proc class first-stage destructor */
static int xa_mimo_proc_cleanup(xf_component_t *component, xf_message_t *m)
{
    XAMimoProc    *mimo_proc = (XAMimoProc *) component;
    UWORD32        i;
    XAOutTrack    *out_track;

    /* ...complete message with error result code */
    xf_response_err(m);
    
    /* ...cancel internal scheduling message if needed */
    xa_base_cancel(&mimo_proc->base);    
    
    /* ...purge all input ports (specify "unregister"? - don't know yet - tbd) */
    for (i = 0; i < mimo_proc->num_in_ports; i++)
    {
        xf_input_port_purge(&mimo_proc->in_track[i].input);
    }

    /* ...flush all output ports */
    for (i = 0; i < mimo_proc->num_out_ports; i++)
    {
        out_track = &mimo_proc->out_track[i];

        if (xf_output_port_flush(&out_track->output, XF_FLUSH))
        {
            /* ... mark flushing sequence is done */
            xf_output_port_flush_done(&out_track->output);

            /* ... mark flushing sequence is done */
            xa_out_track_set_flags(out_track, XA_OUT_TRACK_FLAG_FLUSHING_DONE); 
        }
        else
        {
            /* ...flushing sequence is started; wait until flow-control message returns */
            xa_out_track_set_flags(out_track, XA_OUT_TRACK_FLAG_FLUSHING); 
            TRACE(INFO, _b("mimo_proc[%p]:out_track[%u] cleanup - propagate end-of-stream condition"), mimo_proc, i);
        }
    }

    if (xa_mimo_proc_output_port_flush_done(mimo_proc))
    {
        /* ...flushing sequence is complete; destroy mimo_proc */
        return xa_mimo_proc_destroy(component, NULL);
    }
    else
    {
        /* ...wait until output port is cleaned; adjust component hooks */
        component->entry = xa_mimo_proc_terminate;
        component->exit = xa_mimo_proc_destroy;
        
        TRACE(INIT, _b("mimo_proc[%p] cleanup sequence started"), mimo_proc);

        /* ...indicate that second stage is required */
        return 1;
    }
}

/* ...mimo_proc class factory */
xf_component_t * xa_mimo_proc_factory(UWORD32 core, xa_codec_func_t process, xaf_comp_type comp_type)
{
    XAMimoProc    *mimo_proc;

    /* ...construct generic audio component */
    XF_CHK_ERR(mimo_proc = (XAMimoProc *)xa_base_factory(core, XF_MM(sizeof(*mimo_proc)), process), NULL);

    /* ...set generic codec API */
    mimo_proc->base.memtab = xa_mimo_proc_memtab;
    mimo_proc->base.preprocess = xa_mimo_proc_preprocess;
    mimo_proc->base.postprocess = xa_mimo_proc_postprocess;

    /* ...set message-processing table */
    mimo_proc->base.command = xa_mimo_proc_cmd;
    mimo_proc->base.command_num = XA_MIMO_PROC_CMD_NUM;

    /* ...set component destructor hook */
    mimo_proc->base.component.exit = xa_mimo_proc_cleanup;

    /* ...set component type */
    mimo_proc->base.comp_type = comp_type;

    /* ...set num IO ports */
    mimo_proc->num_in_ports  = xf_io_ports[comp_type][0];
    mimo_proc->num_out_ports = xf_io_ports[comp_type][1];
    
    TRACE(INIT, _b("MimoProc[%p] with %d input and %d output ports is created"), mimo_proc, mimo_proc->num_in_ports, mimo_proc->num_out_ports);

    /* ...return handle to component */
    return (xf_component_t *) mimo_proc;
}

