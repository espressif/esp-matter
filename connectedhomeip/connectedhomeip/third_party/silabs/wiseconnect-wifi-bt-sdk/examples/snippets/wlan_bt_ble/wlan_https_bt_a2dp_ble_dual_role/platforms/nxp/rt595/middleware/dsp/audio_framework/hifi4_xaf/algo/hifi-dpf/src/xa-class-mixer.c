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
 * xa-class-mixer.c
 *
 * Generic mixer component class
 ******************************************************************************/

#define MODULE_TAG                      MIXER

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "xf-dp.h"
#include "xa-class-base.h"
#include "audio/xa-mixer-api.h"

/*******************************************************************************
 * Tracing tags
 ******************************************************************************/

TRACE_TAG(INIT, 1);
TRACE_TAG(WARNING, 1);
TRACE_TAG(INFO, 1);
TRACE_TAG(INPUT, 1);
TRACE_TAG(OUTPUT, 1);

/*******************************************************************************
 * Data structures
 ******************************************************************************/

/* ...mixed source - input data */
typedef struct XATrack
{
    /* ...input port data */
    xf_input_port_t     input;

    /* ...current presentation timestamp (in samples; local to a mixer state) */
    UWORD32                 pts;

    /* ...total amount of decoded frames since last synchronization point */
    UWORD32                 decoded;

    /* ...total amount of rendered frames (consumed) since last synchronization point */
    UWORD32                 rendered;

}   XATrack;

/*******************************************************************************
 * Helpers
 ******************************************************************************/

static inline UWORD32 xa_track_test_flags(XATrack *track, UWORD32 flags)
{
    return (track->input.flags & flags);
}

static inline UWORD32 xa_track_set_flags(XATrack *track, UWORD32 flags)
{
    return (track->input.flags |= flags);
}

static inline UWORD32 xa_track_clear_flags(XATrack *track, UWORD32 flags)
{
    return (track->input.flags &= ~flags);
}

static inline UWORD32 xa_track_toggle_flags(XATrack *track, UWORD32 flags)
{
    return (track->input.flags ^= flags);
}

/*******************************************************************************
 * Mixer data definitions
 ******************************************************************************/

/* ...mixer data */
typedef struct XAMixer
{
    /***************************************************************************
     * Control data
     **************************************************************************/

    /* ...generic audio codec data */
    XACodecBase         base;

    /* ...input tracks */
    XATrack             track[XA_MIXER_MAX_TRACK_NUMBER];
    
    /* ...output port */
    xf_output_port_t    output;

    /***************************************************************************
     * Run-time configuration parameters
     **************************************************************************/

    /* ...audio frame size in samples */
    UWORD32                 frame_size;

    /* ...audio frame duration */
    UWORD32                 frame_duration;
    
    /* ...presentation timestamp (in samples; local mixer scope) */
    UWORD32                 pts;
    
}   XAMixer;

/*******************************************************************************
 * Mixer flags
 ******************************************************************************/

/* ...output port setup completed */
#define XA_MIXER_FLAG_OUTPUT_SETUP      __XA_BASE_FLAG(1 << 0)

/*******************************************************************************
 * Track state flags
 ******************************************************************************/

/* ...track is idle (will autostart as soon as input data received) */
#define XA_TRACK_FLAG_IDLE              __XF_INPUT_FLAG(1 << 0)

/* ...track is rendered */
#define XA_TRACK_FLAG_ACTIVE            __XF_INPUT_FLAG(1 << 1)

/* ...track is paused */
#define XA_TRACK_FLAG_PAUSED            __XF_INPUT_FLAG(1 << 2)

/* ...track input port is setup */
#define XA_TRACK_FLAG_INPUT_SETUP       __XF_INPUT_FLAG(1 << 3)

/* ...track has received data */
#define XA_TRACK_FLAG_RECVD_DATA        __XF_INPUT_FLAG(1 << 4)

/*******************************************************************************
 * Helper functions
 ******************************************************************************/
/* ...Count the tracks that have received data or are active*/
static inline UWORD32 xa_mixer_check_active(XAMixer *mixer)
{
    XATrack        *track;
    UWORD32            i;
    UWORD32            cnt = 0;
    
    for (track = &mixer->track[i = 0]; i < XA_MIXER_MAX_TRACK_NUMBER; i++, track++)
    {
        if (xa_track_test_flags(track, XA_TRACK_FLAG_RECVD_DATA | XA_TRACK_FLAG_ACTIVE))
            cnt++;
    }
    return cnt;
}

/* ...prepare mixer for steady operation */
static inline XA_ERRORCODE xa_mixer_prepare_runtime(XAMixer *mixer)
{
    XACodecBase    *base = (XACodecBase *) mixer;
    xf_message_t   *m = xf_msg_dequeue(&mixer->output.queue);
    xf_start_msg_t *msg = m->buffer;
    UWORD32             frame_size;
    UWORD32             factor;
    
    /* ...query mixer parameters */
    XA_API(base, XA_API_CMD_GET_CONFIG_PARAM, XA_MIXER_CONFIG_PARAM_SAMPLE_RATE, &msg->sample_rate);
    XA_API(base, XA_API_CMD_GET_CONFIG_PARAM, XA_MIXER_CONFIG_PARAM_CHANNELS, &msg->channels);
    XA_API(base, XA_API_CMD_GET_CONFIG_PARAM, XA_MIXER_CONFIG_PARAM_PCM_WIDTH, &msg->pcm_width);
    XA_API(base, XA_API_CMD_GET_MEM_INFO_SIZE, 0, &msg->input_length[0]);
    XA_API(base, XA_API_CMD_GET_MEM_INFO_SIZE, XA_MIXER_MAX_TRACK_NUMBER, &msg->output_length[0]);
    XA_API(base, XA_API_CMD_GET_CONFIG_PARAM, XA_MIXER_CONFIG_PARAM_FRAME_SIZE, &frame_size);

    /* ...calculate mixer frame duration; get upsample factor */
    XF_CHK_ERR(factor = xf_timebase_factor(msg->sample_rate), XA_MIXER_CONFIG_FATAL_RANGE);

    /* ...set mixer frame duration */
    mixer->frame_duration = frame_size * factor;
    
    /* ...pass response to caller */
    xf_response_data(m, sizeof(*msg));

    return XA_NO_ERROR;
}

/*******************************************************************************
 * Commands handlers
 ******************************************************************************/

/* ...EMPTY-THIS-BUFFER command processing */
static XA_ERRORCODE xa_mixer_empty_this_buffer(XACodecBase *base, xf_message_t *m)
{
    XAMixer    *mixer = (XAMixer *) base;
    UWORD32         i = XF_MSG_DST_PORT(m->id);
    XATrack    *track = &mixer->track[i];

    /* ...make sure the port is valid */
    XF_CHK_ERR(i < XA_MIXER_MAX_TRACK_NUMBER, XA_API_FATAL_INVALID_CMD_TYPE);
    
    /* ...command is allowed only in "postinit" state */
    XF_CHK_ERR(base->state & XA_BASE_FLAG_POSTINIT, XA_API_FATAL_INVALID_CMD);

    TRACE(INPUT, _b("track-%u: received buffer [%p]:%u"), i, m->buffer, m->length);

    /* ...update received data for the track */
    if (m->length)
        xa_track_set_flags(track, XA_TRACK_FLAG_RECVD_DATA);
    else
        xa_track_clear_flags(track, XA_TRACK_FLAG_RECVD_DATA);
    
    /* ...place received message into track input port */
    if (xf_input_port_put(&track->input, m))
    {
        /* ...process track autostart if needed */
        if (xa_track_test_flags(track, XA_TRACK_FLAG_IDLE))
        {
            /* ...put track into active state */
            xa_track_toggle_flags(track, XA_TRACK_FLAG_IDLE | XA_TRACK_FLAG_ACTIVE);
            
            /* ...save track presentation timestamp */
            track->pts = mixer->pts;

            TRACE(INFO, _b("track-%u started (pts=%08x)"), i, track->pts);
        }
        
        /* ...schedule data processing if there is output port available */
        if (xf_output_port_ready(&mixer->output))
        {
            /* ...force data processing */
            xa_base_schedule(base, 0);
        }
    }
    
    return XA_NO_ERROR;
}

/* ...FILL-THIS-BUFFER command processing */
static XA_ERRORCODE xa_mixer_fill_this_buffer(XACodecBase *base, xf_message_t *m)
{
    XAMixer    *mixer = (XAMixer *) base;
    UWORD32         i = XF_MSG_DST_PORT(m->id);
    
    /* ...make sure the port is valid */
    XF_CHK_ERR(i == XA_MIXER_MAX_TRACK_NUMBER, XA_API_FATAL_INVALID_CMD_TYPE);
    
    /* ...command is allowed only in "postinit" state */
    XF_CHK_ERR(base->state & XA_BASE_FLAG_POSTINIT, XA_API_FATAL_INVALID_CMD);

    /* ...process runtime initialization explicitly */
    if (base->state & XA_BASE_FLAG_RUNTIME_INIT)
    {
        /* ...message must be zero-length */
        XF_CHK_ERR(m->length == 0, XA_MIXER_EXEC_FATAL_STATE);
    }
    else if (m->length != 0) /* ...EOS response */
    {
        /* ...message must have exactly expected size (there is no ordered abortion) */
        XF_CHK_ERR(m->length == mixer->output.length, XA_MIXER_EXEC_FATAL_STATE);
    }

    TRACE(OUTPUT, _b("received output buffer [%p]:%u"), m->buffer, m->length);
    
    /* ...put message into output port */
    if (xf_output_port_put(&mixer->output, m))
    {
        /* ...force data processing */
        xa_base_schedule(base, 0);
    }

    return XA_NO_ERROR;
}

/* ...output port routing */
static XA_ERRORCODE xa_mixer_port_route(XACodecBase *base, xf_message_t *m)
{
    XAMixer                *mixer = (XAMixer *) base;
    xf_route_port_msg_t    *cmd = m->buffer;
    xf_output_port_t       *port = &mixer->output;
    UWORD32                     src = XF_MSG_DST(m->id);
    UWORD32                     dst = cmd->dst;
    
    /* ...command is allowed only in "postinit" state */
    XF_CHK_ERR(base->state & XA_BASE_FLAG_POSTINIT, XA_API_FATAL_INVALID_CMD);

    /* ...make sure output port is addressed */
    XF_CHK_ERR(XF_MSG_DST_PORT(m->id) == XA_MIXER_MAX_TRACK_NUMBER, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...make sure port is not routed yet */
    XF_CHK_ERR(!xf_output_port_routed(port), XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...route output port - allocate queue */
    XF_CHK_ERR(xf_output_port_route(port, __XF_MSG_ID(dst, src), cmd->alloc_number, cmd->alloc_size, cmd->alloc_align) == 0, XA_API_FATAL_MEM_ALLOC);

    /* ...schedule processing instantly - tbd - check if we have anything pending on input */
    xa_base_schedule(base, 0);
    
    /* ...pass success result to caller */
    xf_response_ok(m);
    
    return XA_NO_ERROR;
}

/* ...port unroute command */
static XA_ERRORCODE xa_mixer_port_unroute(XACodecBase *base, xf_message_t *m)
{
    XAMixer            *mixer = (XAMixer *) base;
    xf_output_port_t   *port = &mixer->output;
    
    /* ...command is allowed only in "postinit" state */
    XF_CHK_ERR(base->state & XA_BASE_FLAG_POSTINIT, XA_API_FATAL_INVALID_CMD);

    /* ...make sure output port is addressed */
    XF_CHK_ERR(XF_MSG_DST_PORT(m->id) == XA_MIXER_MAX_TRACK_NUMBER, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...cancel any pending processing */
    xa_base_cancel(base);

    /* ...clear output-port-setup condition */
    base->state &= ~XA_MIXER_FLAG_OUTPUT_SETUP;

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

/* ...PAUSE message processing */
static XA_ERRORCODE xa_mixer_pause(XACodecBase *base, xf_message_t *m)
{
    XAMixer    *mixer = (XAMixer *) base;
    UWORD32         i = XF_MSG_DST_PORT(m->id);
    XATrack    *track = &mixer->track[i];
    
    /* ...make sure the buffer is empty */
    XF_CHK_ERR(m->length == 0, XA_API_FATAL_INVALID_CMD_TYPE);
    
    /* ...check destination port is valid */
    XF_CHK_ERR(i < XA_MIXER_MAX_TRACK_NUMBER, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...check for actual track flags */
    if (xa_track_test_flags(track, XA_TRACK_FLAG_ACTIVE))
    {
        /* ...switch to paused state */
        xa_track_toggle_flags(track, XA_TRACK_FLAG_ACTIVE | XA_TRACK_FLAG_PAUSED);

        /* ...other tracks may be waiting for this one, so force data processing */
        if (xf_output_port_ready(&mixer->output))
        {
            xa_base_schedule(base, 0);
        }

        TRACE(INFO, _b("mixer[%p]::track[%u] paused"), mixer, i);
    }
    else
    {
        /* ...track is in idle state and pausing here means suspending */
        TRACE(INFO, _b("mixer[%p]::track[%u] is not active"), mixer, i);
    }

    /* ...complete message immediately */
    xf_response(m);
    
    return XA_NO_ERROR;
}

/* ...RESUME command processing */
static XA_ERRORCODE xa_mixer_resume(XACodecBase *base, xf_message_t *m)
{
    XAMixer    *mixer = (XAMixer *) base;
    UWORD32         i = XF_MSG_DST_PORT(m->id);
    XATrack    *track = &mixer->track[i];
    
    /* ...make sure the buffer is empty */
    XF_CHK_ERR(m->length == 0, XA_API_FATAL_INVALID_CMD_TYPE);
    
    /* ...check destination port is valid */
    XF_CHK_ERR(i < XA_MIXER_MAX_TRACK_NUMBER, XA_API_FATAL_INVALID_CMD_TYPE);
    
    /* ...check for actual track state */
    if (xa_track_test_flags(track, XA_TRACK_FLAG_PAUSED))
    {
        /* ...switch track to active state */
        xa_track_toggle_flags(track, XA_TRACK_FLAG_ACTIVE | XA_TRACK_FLAG_PAUSED);

        /* ...reset track presentation timestamp - tbd */
        track->pts = mixer->pts;

        /* ...force data processing if there is an output buffer */
        if (xf_output_port_ready(&mixer->output))
        {
            xa_base_schedule(base, 0);
        }

        TRACE(INFO, _b("mixer[%p]::track[%u] resumed"), mixer, i);
    }
    else
    {
        /* ...track is in idle state; do nothing */
        TRACE(INFO, _b("mixer[%p]::track[%u] is not paused"), mixer, i);
    }
    
    /* ...complete message */
    xf_response(m);
    
    return XA_NO_ERROR;
}

/* ...FLUSH command processing */
static XA_ERRORCODE xa_mixer_flush(XACodecBase *base, xf_message_t *m)
{
    XAMixer    *mixer = (XAMixer *) base;
    UWORD32         i = XF_MSG_DST_PORT(m->id);
    XATrack    *track = &mixer->track[i];

    /* ...make sure the buffer is empty */
    XF_CHK_ERR(m->length == 0, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...check destination port index */
    if (i == XA_MIXER_MAX_TRACK_NUMBER)
    {
        /* ...flushing response received; that is a port unrouting sequence */
        XF_CHK_ERR(xf_output_port_unrouting(&mixer->output), XA_API_FATAL_INVALID_CMD_TYPE);
        
        /* ...complete unroute sequence */
        xf_output_port_unroute_done(&mixer->output);

        TRACE(INFO, _b("port is unrouted"));

        return XA_NO_ERROR;
    }

    /* ...check destination port index is valid */
    XF_CHK_ERR(i < XA_MIXER_MAX_TRACK_NUMBER, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...input port flushing; check the track state is valid */
    if (xa_track_test_flags(track, XA_TRACK_FLAG_ACTIVE | XA_TRACK_FLAG_PAUSED))
    {
        /* ...purge input port */
        xf_input_port_purge(&track->input);

        /* ...force clearing of ACTIVE and INPUT_SETUP condition */
        xa_track_clear_flags(track, XA_TRACK_FLAG_ACTIVE | XA_TRACK_FLAG_PAUSED | XA_TRACK_FLAG_INPUT_SETUP);

        /* ...and enter into idle state */
        xa_track_set_flags(track, XA_TRACK_FLAG_IDLE);

        /* ...other tracks may be waiting for this track, so force data processing */
        if (xf_output_port_ready(&mixer->output))
        {
            xa_base_schedule(base, 0);
        }

        TRACE(INFO, _b("mixer[%p]::track[%u] flushed"), mixer, i);
    }

    /* ...complete message instantly (no propagation to output port) */
    xf_response(m);

    return XA_NO_ERROR;
}

/*******************************************************************************
 * Codec API implementation
 ******************************************************************************/

/* ...buffers handling */
static XA_ERRORCODE xa_mixer_memtab(XACodecBase *base, WORD32 idx, WORD32 type, WORD32 size, WORD32 align, UWORD32 core)
{
    XAMixer    *mixer = (XAMixer *)base;
    
    if (type == XA_MEMTYPE_INPUT)
    {
        XATrack    *track = &mixer->track[idx];

        /* ...input buffer allocation; check track number is sane */
        XF_CHK_ERR(idx < XA_MIXER_MAX_TRACK_NUMBER, XA_API_FATAL_INVALID_CMD_TYPE);

        /* ...create input port for a track */
        XF_CHK_ERR(xf_input_port_init(&track->input, size, align, core) == 0, XA_API_FATAL_MEM_ALLOC);

        /* ...set input port buffer */
        XA_API(base, XA_API_CMD_SET_MEM_PTR, idx, track->input.buffer);

        /* ...put track into idle state (will start as soon as we receive data) */
        xa_track_set_flags(track, XA_TRACK_FLAG_IDLE);
        
        TRACE(INIT, _b("mixer[%p]::track[%u] input port created - size=%u"), mixer, idx, size);
    }
    else
    {
        /* ...output buffer allocation */
        XF_CHK_ERR(type == XA_MEMTYPE_OUTPUT, XA_API_FATAL_INVALID_CMD_TYPE);

        /* ...check port number is what we expect */
        XF_CHK_ERR(idx == XA_MIXER_MAX_TRACK_NUMBER, XA_API_FATAL_INVALID_CMD_TYPE);

        /* ...set mixer frame-size (in samples - for timestamp tracking) */
        XA_API(base, XA_API_CMD_GET_CONFIG_PARAM, XA_MIXER_CONFIG_PARAM_FRAME_SIZE, &mixer->frame_size);
        
        /* ...create output port for a track */
        XF_CHK_ERR(xf_output_port_init(&mixer->output, size) == 0, XA_API_FATAL_MEM_ALLOC);
        
        TRACE(INIT, _b("mixer[%p] output port created; size=%u"), mixer, size);
    }

    return XA_NO_ERROR;
}

/* ...preprocessing function */
static XA_ERRORCODE xa_mixer_preprocess(XACodecBase *base)
{
    XAMixer        *mixer = (XAMixer *) base;
    XATrack        *track;
    UWORD8              i;
    XA_ERRORCODE    e = XA_MIXER_EXEC_NONFATAL_NO_DATA;

    /* ...prepare output buffer */
    if (!(base->state & XA_MIXER_FLAG_OUTPUT_SETUP))
    {
        void   *output;

        /* ...set output buffer pointer */
        if (base->state & XA_BASE_FLAG_RUNTIME_INIT)
        {
            /* ...no actual data processing during initialization */
            return XA_NO_ERROR;
        }
        else if ((output = xf_output_port_data(&mixer->output)) == NULL)
        {
            /* ...no output buffer available */
            return e;
        }
            
        /* ...set output buffer pointer */
        XA_API(base, XA_API_CMD_SET_MEM_PTR, XA_MIXER_MAX_TRACK_NUMBER, output);

        /* ...mark output port is setup */
        base->state ^= XA_MIXER_FLAG_OUTPUT_SETUP;
    }
    
    /* ...check EOS */
    if (!xa_mixer_check_active(mixer))
    {
        /* ...push EOS to output port */
        xf_output_port_produce(&mixer->output, 0);
        TRACE(INFO, _b("mixer[%p]::EOS generated"), mixer);
    }

    /* ...setup input buffer pointers and length */
    for (track = &mixer->track[i = 0]; i < XA_MIXER_MAX_TRACK_NUMBER; i++, track++)
    {
        /* ...skip tracks that are not played */
        if (!xa_track_test_flags(track, XA_TRACK_FLAG_ACTIVE))  continue;

        /* ...set temporary mixing request */
        e = XA_NO_ERROR;

        /* ...skip the tracks that has been setup already */
        if (xa_track_test_flags(track, XA_TRACK_FLAG_INPUT_SETUP))  continue;

        /* ...found active track that hasn't been setup yet */
        TRACE(INPUT, _b("track-%u: ts=%08x vs mts=%08x"), i, track->pts, mixer->pts);

        /* ...if track presentation timestamp is in the future, do nothing yet really */
        if (!xf_time_after(track->pts, mixer->pts))
        {
            UWORD32     filled;
            
            /* ...take actual data from input port (mixer is always using internal buffer) */
            if (!xf_input_port_fill(&track->input))
            {
                /* ...failed to prefill input buffer - no sufficient data yet */
                return XA_MIXER_EXEC_NONFATAL_NO_DATA;
            }
            else
            {
                /* ...retrieve number of bytes available */
                filled = xf_input_port_level(&track->input);
            }

            /* ...set total number of bytes we have in buffer */
            XA_API(base, XA_API_CMD_SET_INPUT_BYTES, i, &filled);

            /* ...actual data is to be played */
            TRACE(INPUT, _b("track-%u: filled %u bytes"), i, filled);
        }
        
        /* ...mark the track input is setup (emit silence or actual data) */
        xa_track_set_flags(track, XA_TRACK_FLAG_INPUT_SETUP);
    }

    /* ...do mixing operation only when all active tracks are setup */
    return e;
}

/* ...postprocessing function */
static XA_ERRORCODE xa_mixer_postprocess(XACodecBase *base, int done)
{
    XAMixer        *mixer = (XAMixer *) base;
    XATrack        *track;
    UWORD32             produced;
    UWORD32             consumed;
    UWORD8              i;

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
            return XA_CHK(xa_mixer_prepare_runtime(mixer));
        }
        else
        {
            /* ...mixer operation is over (can't be? - tbd) */
            BUG(1, _x("breakpoint"));
        }
    }

    /* ...input ports maintenance; process all tracks */
    for (track = &mixer->track[i = 0]; i < XA_MIXER_MAX_TRACK_NUMBER; i++, track++)
    {
        /* ...skip the tracks that are not runing */
        if (!xa_track_test_flags(track, XA_TRACK_FLAG_ACTIVE))  continue;

        /* ...clear input setup flag */
        xa_track_clear_flags(track, XA_TRACK_FLAG_INPUT_SETUP);
        
        /* ...advance track presentation timestamp */
        track->pts += mixer->frame_size;
        
        /* ...get total amount of consumed bytes */
        XA_API(base, XA_API_CMD_GET_CURIDX_INPUT_BUF, i, &consumed);

        TRACE(INPUT, _b("track-%u::postprocess(c=%u, ts=%08x)"), i, consumed, track->pts);

        /* ...consume that amount from input port (may be zero) */
        xf_input_port_consume(&track->input, consumed);
        
        /* ...check if input port is done */
        if (xf_input_port_done(&track->input))
        {
            /* ...input stream is over; return zero-length input back to caller */
            xf_input_port_purge(&track->input);

            /* ...switch to idle state */
            xa_track_toggle_flags(track, XA_TRACK_FLAG_ACTIVE | XA_TRACK_FLAG_IDLE);

            TRACE(INFO, _b("mixer[%p]::track[%u] completed"), mixer, i);
        }
    }

    /* ...check if we have produced anything */
    XA_API(base, XA_API_CMD_GET_OUTPUT_BYTES, XA_MIXER_MAX_TRACK_NUMBER, &produced);

    TRACE(OUTPUT, _b("mixer[%p]::postprocess(p=%u, ts=%08x, done=%u)"), mixer, produced, mixer->pts, done);

    /* ...output port maintenance */
    if (produced)
    {
        /* ...make sure we have produced exactly single frame */
        BUG(produced != mixer->output.length, _x("Invalid length: %u != %u"), produced, mixer->output.length);
        
        /* ...steady mixing process; advance mixer presentation timestamp */
        mixer->pts += mixer->frame_size;

        /* ...push data from output port */
        xf_output_port_produce(&mixer->output, produced);

        /* ...clear output-setup condition */
        base->state &= ~XA_MIXER_FLAG_OUTPUT_SETUP;
    }    
    
    /* ...reschedule data processing if there is a pending output message */
    if (xf_output_port_ready(&mixer->output))
    {
        /* ...schedule execution with respect to urgency */
        xa_base_schedule(base, (produced ? mixer->frame_duration : 0));
    }

    return XA_NO_ERROR;
}

/*******************************************************************************
 * Command-processing function
 ******************************************************************************/

/* ...command hooks */
static XA_ERRORCODE (* const xa_mixer_cmd[])(XACodecBase *, xf_message_t *) =
{
    /* ...set-parameter - actually, same as in generic case */
    [XF_OPCODE_TYPE(XF_SET_PARAM)] = xa_base_set_param,
    [XF_OPCODE_TYPE(XF_GET_PARAM)] = xa_base_get_param,

    /* ...output port routing/unrouting */
    [XF_OPCODE_TYPE(XF_ROUTE)] = xa_mixer_port_route,
    [XF_OPCODE_TYPE(XF_UNROUTE)] = xa_mixer_port_unroute,

    /* ...input/output buffers processing */
    [XF_OPCODE_TYPE(XF_EMPTY_THIS_BUFFER)] = xa_mixer_empty_this_buffer,
    [XF_OPCODE_TYPE(XF_FILL_THIS_BUFFER)] = xa_mixer_fill_this_buffer,
    [XF_OPCODE_TYPE(XF_FLUSH)] = xa_mixer_flush,

    /* ...track control */
    [XF_OPCODE_TYPE(XF_PAUSE)] = xa_mixer_pause,
    [XF_OPCODE_TYPE(XF_RESUME)] = xa_mixer_resume,
};

/* ...total number of commands supported */
#define XA_MIXER_CMD_NUM        (sizeof(xa_mixer_cmd) / sizeof(xa_mixer_cmd[0]))

/*******************************************************************************
 * Entry points
 ******************************************************************************/

/* ...mixer termination-state command processor */
static int xa_mixer_terminate(xf_component_t *component, xf_message_t *m)
{
    XAMixer    *mixer = (XAMixer *) component;
    UWORD32         opcode = m->opcode;
    
    if (m == xf_output_port_control_msg(&mixer->output))
    {
        /* ...output port flushing complete; mark port is idle and terminate */
        xf_output_port_flush_done(&mixer->output);
        return -1;
    }
    else if (opcode == XF_FILL_THIS_BUFFER && xf_output_port_routed(&mixer->output))
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

/* ...mixer class destructor */
static int xa_mixer_destroy(xf_component_t *component, xf_message_t *m)
{
    XAMixer    *mixer = (XAMixer *) component;
    UWORD32         core = xf_component_core(component);
    UWORD32         i;
    
    /* ...destroy all inputs */
    for (i = 0; i < XA_MIXER_MAX_TRACK_NUMBER; i++)
    {
        xf_input_port_destroy(&mixer->track[i].input, core);
    }

    /* ...destroy output port */
    xf_output_port_destroy(&mixer->output, core);

    /* ...destroy base object */
    xa_base_destroy(&mixer->base, XF_MM(sizeof(*mixer)), core);

    TRACE(INIT, _b("mixer[%p] destroyed"), mixer);

    return 0;
}

/* ...mixer class first-stage destructor */
static int xa_mixer_cleanup(xf_component_t *component, xf_message_t *m)
{
    XAMixer    *mixer = (XAMixer *) component;
    UWORD32         i;

    /* ...complete message with error result code */
    xf_response_err(m);
    
    /* ...cancel internal scheduling message if needed */
    xa_base_cancel(&mixer->base);    
    
    /* ...purge all input ports (specify "unregister"? - don't know yet - tbd) */
    for (i = 0; i < XA_MIXER_MAX_TRACK_NUMBER; i++)
    {
        xf_input_port_purge(&mixer->track[i].input);
    }

    /* ...flush output port */
    if (xf_output_port_flush(&mixer->output, XF_FLUSH))
    {
        /* ...flushing sequence is not needed; destroy mixer */
        return xa_mixer_destroy(component, NULL);
    }
    else
    {
        /* ...wait until output port is cleaned; adjust component hooks */
        component->entry = xa_mixer_terminate;
        component->exit = xa_mixer_destroy;
        
        TRACE(INIT, _b("mixer[%p] cleanup sequence started"), mixer);

        /* ...indicate that second stage is required */
        return 1;
    }
}

/* ...mixer class factory */
xf_component_t * xa_mixer_factory(UWORD32 core, xa_codec_func_t process, xaf_comp_type comp_type)
{
    XAMixer    *mixer;

    /* ...construct generic audio component */
    XF_CHK_ERR(mixer = (XAMixer *)xa_base_factory(core, XF_MM(sizeof(*mixer)), process), NULL);

    /* ...set generic codec API */
    mixer->base.memtab = xa_mixer_memtab;
    mixer->base.preprocess = xa_mixer_preprocess;
    mixer->base.postprocess = xa_mixer_postprocess;

    /* ...set message-processing table */
    mixer->base.command = xa_mixer_cmd;
    mixer->base.command_num = XA_MIXER_CMD_NUM;

    /* ...set component destructor hook */
    mixer->base.component.exit = xa_mixer_cleanup;

    /* ...set component type */
    mixer->base.comp_type = comp_type;
    
    TRACE(INIT, _b("Mixer[%p] created"), mixer);

    /* ...return handle to component */
    return (xf_component_t *) mixer;
}
