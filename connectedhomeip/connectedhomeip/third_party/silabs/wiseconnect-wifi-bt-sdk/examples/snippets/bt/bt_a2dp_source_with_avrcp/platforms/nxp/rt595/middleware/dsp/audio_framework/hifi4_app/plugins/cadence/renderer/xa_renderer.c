/*
 * Copyright 2018 NXP
 * Copyright (c) 2015-2018 Cadence Design Systems, Inc.
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
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
 */

/*******************************************************************************
 * xa-renderer.c
 *
 * Hardware renderer implementation
 ******************************************************************************/

#define MODULE_TAG                      RENDERER

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <xtensa/xos.h>

#include "audio/xa-renderer-api.h"
#include "xf-debug.h"

#include "fsl_dma.h"
#include "fsl_i2s.h"
#include "fsl_i2s_dma.h"

/*******************************************************************************
 * Tracing configuration
 ******************************************************************************/
TRACE_TAG(INIT, 1);
TRACE_TAG(PROCESS, 1);
TRACE_TAG(UNDERRUN, 1);

/*******************************************************************************
 * Hardware parameters
 ******************************************************************************/
#ifdef CPU_MIMXRT595SFFOA_dsp
#define DMA_RENDERER DMA1
#define I2S_RENDERER I2S3
#define I2S_CLOCK_FREQ (24576000)
#define I2S_DMA_CHANNEL (7)
#else
#define DMA_RENDERER DMA1
#define I2S_RENDERER I2S1
#define I2S_CLOCK_FREQ (24576000)
#define I2S_DMA_CHANNEL (3)
#endif

/* ...size of DMA buffers, in samples.  allocated statically. */
#define BUFFER_SIZE 512U
/* ...fixed sample size of 16-bit / stereo */
#define SAMPLE_SIZE 4U
/* ...fixed frame size, in bytes */
#define FRAME_SIZE (BUFFER_SIZE * SAMPLE_SIZE)

typedef struct XARenderer
{
    /***************************************************************************
     * Internal stuff
     **************************************************************************/

    /* ...component state */
    UWORD32                     state;

    /* ...notification callback pointer */
    xa_renderer_cb_t           *cdata;

    /* ...input buffer pointer */
    void                       *input;

    /* ...pointer to DMA FIFO buffer */
    void                       *fifo_head;

    /* ...number of bytes consumed */
    UWORD32                     consumed;

    /* ...total number of frames submitted */
    UWORD32                     submitted;

    /* ...total number of frames rendered */
    UWORD32                     rendered;

    /***************************************************************************
     * Run-time data
     **************************************************************************/

    /* ...size of one frame, in samples */
    UWORD32                     frame_size;

    /* ...size of PCM sample (respects channels and PCM width) */
    UWORD32                     sample_size;

    /* ...number of channels */
    UWORD32                     channels;

    /* ...sample width */
    UWORD32                     pcm_width;

    /* ...current sampling rate */
    UWORD32                     rate;

    /* ...flag for detecting underrun..made to non zero over submit */
    UWORD32                     submit_flag;

    /* bytes produced*/
    UWORD64                     bytes_produced;

    /* DMA handle */
    dma_handle_t                i2sTxDmaHandle;
} XARenderer;

/*******************************************************************************
 * Operating flags
 ******************************************************************************/
#define XA_RENDERER_FLAG_PREINIT_DONE   (1 << 0)
#define XA_RENDERER_FLAG_POSTINIT_DONE  (1 << 1)
#define XA_RENDERER_FLAG_IDLE           (1 << 2)
#define XA_RENDERER_FLAG_RUNNING        (1 << 3)
#define XA_RENDERER_FLAG_PAUSED         (1 << 4)

/*******************************************************************************
 * Variables
 ******************************************************************************/

/* Transfer ping pong buffer and descriptor */
AT_NONCACHEABLE_SECTION_ALIGN(
    static uint8_t g_i2sTxbuffer[FRAME_SIZE * 2], 4
);

AT_NONCACHEABLE_SECTION_ALIGN(
    dma_descriptor_t s_dmaDescriptorPingpongI2S[2U], 16
);

/*******************************************************************************
 * Local functions
 ******************************************************************************/

/* ...start renderer operation */
static inline void xa_hw_renderer_start(XARenderer *d)
{
    TRACE(INIT, _b("xa_hw_renderer_start"));

    /* ...enable I2S DMA FIFO */
    I2S_RENDERER->FIFOCFG |= I2S_FIFOCFG_DMATX_MASK;

    DMA_EnableChannelInterrupts(DMA_RENDERER, I2S_DMA_CHANNEL);
    DMA_StartTransfer(&d->i2sTxDmaHandle);

    I2S_Enable(I2S_RENDERER);
}

/* ...close hardware renderer */
static inline void xa_hw_renderer_close(XARenderer *d)
{
    TRACE(INIT, _b("xa_hw_renderer_close"));

    DMA_DisableChannelInterrupts(DMA_RENDERER, I2S_DMA_CHANNEL);
    DMA_AbortTransfer(&d->i2sTxDmaHandle);

    /* ...wait until all transmitted data get out of FIFO */
    while ((I2S_RENDERER->FIFOSTAT & I2S_FIFOSTAT_TXEMPTY_MASK) == 0U) 
    {   
    }   

    /* ...disable I2S DMA after FIFO data is flushed */
    I2S_RENDERER->FIFOCFG &= (~I2S_FIFOCFG_DMATX_MASK);
    I2S_RENDERER->FIFOCFG |= I2S_FIFOCFG_EMPTYTX_MASK;

    I2S_Disable(I2S_RENDERER);
}

/* ...pause renderer operation */
static inline void xa_hw_renderer_pause(XARenderer *d)
{
    TRACE(INIT, _b("xa_hw_renderer_pause"));

    /* TODO: implement */
}

/* ...resume renderer operation */
static inline void xa_hw_renderer_resume(XARenderer *d)
{
    TRACE(INIT, _b("xa_hw_renderer_resume"));

    /* TODO: implement */
}

/* DMA transfer callback. */
static void TxRenderCallback(struct _dma_handle *handle, void *userData, bool transferDone, uint32_t intmode)
{
    XARenderer *d = (XARenderer*) userData;

    if (transferDone)
    {
        d->rendered++;

        if (d->rendered >= d->submitted)
        {
            TRACE(UNDERRUN, _b("xa_renderer DMA underrun"));

            /* ...reset rendered value to match submitted.
             * TODO: stop DMA transfers here to avoid sending stale data to I2S. */
            d->rendered = d->submitted;

            d->state ^= XA_RENDERER_FLAG_RUNNING | XA_RENDERER_FLAG_IDLE;
        }

        /* ...notify user input buffer consumption */
        d->cdata->cb(d->cdata, 0);
    }
}

/* Hardware-specific initialization
 * Setup I2S and DMA */
static void evk_hw_renderer_init(void* ptr)
{
    i2s_config_t s_TxConfig;
    dma_channel_config_t transferConfig;
    XARenderer *d = (XARenderer*) ptr;

    I2S_TxGetDefaultConfig(&s_TxConfig);
    /* Configure I2S divider based on the configured data format. */
    s_TxConfig.divider = (I2S_CLOCK_FREQ / d->rate / d->pcm_width / d->channels);

    I2S_TxInit(I2S_RENDERER, &s_TxConfig);

    DMA_SetChannelPriority(DMA_RENDERER, I2S_DMA_CHANNEL, kDMA_ChannelPriority3);
    DMA_CreateHandle(&d->i2sTxDmaHandle, DMA_RENDERER, I2S_DMA_CHANNEL);

    DMA_SetCallback(&d->i2sTxDmaHandle, TxRenderCallback, ptr);

    memset(&g_i2sTxbuffer[0], 0, FRAME_SIZE * 2);

    DMA_PrepareChannelTransfer(&transferConfig,
                               &g_i2sTxbuffer[0],
                               (void *)&I2S_RENDERER->FIFOWR,
                               DMA_CHANNEL_XFER(true, false, true, false, 4, kDMA_AddressInterleave1xWidth, kDMA_AddressInterleave0xWidth, FRAME_SIZE),
                               kDMA_MemoryToPeripheral,
                               NULL,
                               s_dmaDescriptorPingpongI2S);

    DMA_SubmitChannelTransfer(&d->i2sTxDmaHandle, &transferConfig);

    DMA_SetupDescriptor(s_dmaDescriptorPingpongI2S,
                        DMA_CHANNEL_XFER(true, false, false, true, 4U, kDMA_AddressInterleave1xWidth,
                        kDMA_AddressInterleave0xWidth, FRAME_SIZE),
                        &g_i2sTxbuffer[FRAME_SIZE],
                        (void *)&I2S_RENDERER->FIFOWR,
                        &s_dmaDescriptorPingpongI2S[1U]);

    DMA_SetupDescriptor(&s_dmaDescriptorPingpongI2S[1U],
                        DMA_CHANNEL_XFER(true, false, true, false, 4U, kDMA_AddressInterleave1xWidth, kDMA_AddressInterleave0xWidth, FRAME_SIZE),
                        &g_i2sTxbuffer[0],
                        (void *)&I2S_RENDERER->FIFOWR,
                        s_dmaDescriptorPingpongI2S);
}

static XA_ERRORCODE xa_hw_renderer_init(XARenderer *d)
{
    /* ...reset frame counters */
    d->rendered = 0;
    d->submitted = 0;
    d->consumed = 0;
    d->bytes_produced = 0;

    d->fifo_head = (void*) &g_i2sTxbuffer[0];

    /* Initialize hardware */
    evk_hw_renderer_init(d);

    return XA_NO_ERROR;
}

/* ...submit data (in samples) into internal renderer ring-buffer */
static inline UWORD32 xa_hw_renderer_submit(XARenderer *d, void *b, UWORD32 n)
{
    UWORD32 samples_write;
    UWORD32 buffer_available;
    UWORD32 offset;

    buffer_available = (BUFFER_SIZE * 2) - ((d->submitted - d->rendered) * BUFFER_SIZE);
    samples_write = (n > buffer_available ? buffer_available : n);

    if (samples_write > 0)
    {
        offset = (d->submitted % 2) * (d->frame_size * d->sample_size);
        memcpy((char*) d->fifo_head + offset, b, samples_write * d->sample_size);

        buffer_available -= samples_write;

        /* ...increment number of submitted frames */
        d->submitted++;
    }

    /* ...process buffer start-up if buffer is full */
    if ((d->state & XA_RENDERER_FLAG_IDLE) && (buffer_available == 0))
    {
        xa_hw_renderer_start(d);

        d->state ^= XA_RENDERER_FLAG_IDLE | XA_RENDERER_FLAG_RUNNING;
    }

    return samples_write;
}

/* ...state retrieval function */
static inline UWORD32 xa_hw_renderer_get_state(XARenderer *d)
{
    if (d->state & XA_RENDERER_FLAG_RUNNING)
        return XA_RENDERER_STATE_RUN;
    else if (d->state & XA_RENDERER_FLAG_PAUSED)
        return XA_RENDERER_STATE_PAUSE;
    else
        return XA_RENDERER_STATE_IDLE;
}

/* ...HW-renderer control function */
static inline XA_ERRORCODE xa_hw_renderer_control(XARenderer *d, UWORD32 state)
{
    switch (state)
    {
    case XA_RENDERER_STATE_RUN:
        /* ...renderer must be in paused state */
        XF_CHK_ERR(d->state & XA_RENDERER_FLAG_PAUSED, XA_RENDERER_EXEC_NONFATAL_STATE);

        /* ...resume renderer operation */
        xa_hw_renderer_resume(d);

        /* ...mark renderer is running */
        d->state ^= XA_RENDERER_FLAG_RUNNING | XA_RENDERER_FLAG_PAUSED;
        return XA_NO_ERROR;

    case XA_RENDERER_STATE_PAUSE:
        /* ...renderer must be in running state */
        XF_CHK_ERR(d->state & XA_RENDERER_FLAG_RUNNING, XA_RENDERER_EXEC_NONFATAL_STATE);
        /* ...pause renderer operation */
        xa_hw_renderer_pause(d);

        /* ...mark renderer is paused */
        d->state ^= XA_RENDERER_FLAG_RUNNING | XA_RENDERER_FLAG_PAUSED;
        return XA_NO_ERROR;

    case XA_RENDERER_STATE_IDLE:
        /* ...command is valid in any active state; stop renderer operation */
        xa_hw_renderer_close(d);

        /* ...reset renderer flags */
        d->state &= ~(XA_RENDERER_FLAG_RUNNING | XA_RENDERER_FLAG_PAUSED);
        return XA_NO_ERROR;

    default:
        /* ...unrecognized command */
        return XF_CHK_ERR(0, XA_API_FATAL_INVALID_CMD_TYPE);
    }
}

/*******************************************************************************
 * API command hooks
 ******************************************************************************/

/* ...standard codec initialization routine */
static XA_ERRORCODE xa_renderer_get_api_size(XARenderer *d, WORD32 i_idx, pVOID pv_value)
{
    /* ...check parameters are sane */
    XF_CHK_ERR(pv_value, XA_API_FATAL_INVALID_CMD_TYPE);
    /* ...retrieve API structure size */
    *(WORD32 *)pv_value = sizeof(*d);

    return XA_NO_ERROR;
}

/* ...standard codec initialization routine */
static XA_ERRORCODE xa_renderer_init(XARenderer *d, WORD32 i_idx, pVOID pv_value)
{
    /* ...sanity check - pointer must be valid */
    XF_CHK_ERR(d, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...process particular initialization type */
    switch (i_idx)
    {
    case XA_CMD_TYPE_INIT_API_PRE_CONFIG_PARAMS:
    {
        /* ...pre-configuration initialization; reset internal data */
        memset(d, 0, sizeof(*d));

        /* ...sample width and channel count is fixed */
        d->channels = 2;
        d->pcm_width = 16;
        /* ...default to 48 kHz sample rate, configurable */
        d->rate = 48000;

        /* ...support fixed sample size of 16-bit / stereo */
        d->sample_size = SAMPLE_SIZE;
        /* ...support fixed frame size based on internal static DMA buffers */
        d->frame_size = BUFFER_SIZE;

        /* ...and mark renderer has been created */
        d->state = XA_RENDERER_FLAG_PREINIT_DONE;

        return XA_NO_ERROR;
    }

    case XA_CMD_TYPE_INIT_API_POST_CONFIG_PARAMS:
    {
        /* ...post-configuration initialization (all parameters are set) */
        XF_CHK_ERR(d->state & XA_RENDERER_FLAG_PREINIT_DONE, XA_API_FATAL_INVALID_CMD_TYPE);

        XF_CHK_ERR(xa_hw_renderer_init(d) == 0, XA_RENDERER_CONFIG_FATAL_HW);

        /* ...mark post-initialization is complete */
        d->state |= XA_RENDERER_FLAG_POSTINIT_DONE;

        return XA_NO_ERROR;
    }

    case XA_CMD_TYPE_INIT_PROCESS:
    {
        /* ...kick run-time initialization process; make sure setup is complete */
        XF_CHK_ERR(d->state & XA_RENDERER_FLAG_POSTINIT_DONE, XA_API_FATAL_INVALID_CMD_TYPE);

        /* ...mark renderer is in idle state */
        d->state |= XA_RENDERER_FLAG_IDLE;

        /* ...mark renderer is ready to submit */
        d->submit_flag = 0;

        return XA_NO_ERROR;
    }

    case XA_CMD_TYPE_INIT_DONE_QUERY:
    {
        /* ...check if initialization is done; make sure pointer is sane */
        XF_CHK_ERR(pv_value, XA_API_FATAL_INVALID_CMD_TYPE);
        /* ...put current status */
        *(WORD32 *)pv_value = (d->state & XA_RENDERER_FLAG_IDLE ? 1 : 0);

        return XA_NO_ERROR;
    }

    default:
        /* ...unrecognized command type */
        return XF_CHK_ERR(0, XA_API_FATAL_INVALID_CMD_TYPE);
    }
}

/* ...set renderer configuration parameter */
static XA_ERRORCODE xa_renderer_set_config_param(XARenderer *d, WORD32 i_idx, pVOID pv_value)
{
    UWORD32     i_value;

    /* ...sanity check - pointers must be sane */
    XF_CHK_ERR(d && pv_value, XA_API_FATAL_INVALID_CMD_TYPE);
    /* ...pre-initialization must be completed */
    XF_CHK_ERR(d->state & XA_RENDERER_FLAG_PREINIT_DONE, XA_RENDERER_CONFIG_FATAL_STATE);

    /* ...process individual configuration parameter */
    switch (i_idx)
    {
    case XA_RENDERER_CONFIG_PARAM_PCM_WIDTH:
        /* ...command is valid only in configuration state */
        XF_CHK_ERR((d->state & XA_RENDERER_FLAG_POSTINIT_DONE) == 0, XA_RENDERER_CONFIG_FATAL_STATE);
        /* ...get requested PCM width */
        i_value = (UWORD32) *(WORD32 *)pv_value;
        /* ...check value is permitted (16 bits only) */
        XF_CHK_ERR(i_value == 16, XA_RENDERER_CONFIG_NONFATAL_RANGE);
        /* ...apply setting */
        d->pcm_width = i_value;

        return XA_NO_ERROR;

    case XA_RENDERER_CONFIG_PARAM_CHANNELS:
        /* ...command is valid only in configuration state */
        XF_CHK_ERR((d->state & XA_RENDERER_FLAG_POSTINIT_DONE) == 0, XA_RENDERER_CONFIG_FATAL_STATE);
        /* ...get requested channel number */
        i_value = (UWORD32) *(WORD32 *)pv_value;
        /* ...allow stereo only */
        XF_CHK_ERR(i_value == 2, XA_RENDERER_CONFIG_NONFATAL_RANGE);
        /* ...apply setting */
        d->channels = (UWORD32)i_value;

        return XA_NO_ERROR;

    case XA_RENDERER_CONFIG_PARAM_SAMPLE_RATE:
        /* ...command is valid only in configuration state */
        XF_CHK_ERR((d->state & XA_RENDERER_FLAG_POSTINIT_DONE) == 0, XA_RENDERER_CONFIG_FATAL_STATE);
        /* ...get requested sampling rate */
        i_value = (UWORD32) *(WORD32 *)pv_value;
        /* ...allow 44.1 and 48KHz only - tbd */
        XF_CHK_ERR(i_value == 44100 || i_value == 48000 || i_value == 16000, XA_RENDERER_CONFIG_NONFATAL_RANGE);
        /* ...apply setting */
        d->rate = (UWORD32)i_value;

        return XA_NO_ERROR;

    case XA_RENDERER_CONFIG_PARAM_FRAME_SIZE:
        /* ...command is valid only in configuration state */
        XF_CHK_ERR((d->state & XA_RENDERER_FLAG_POSTINIT_DONE) == 0, XA_RENDERER_CONFIG_FATAL_STATE);

        /* ...get requested frame size */
        i_value = (UWORD32) *(WORD32 *)pv_value;

        /* ...check it is equal to the only frame size we support */
        XF_CHK_ERR(i_value == BUFFER_SIZE, XA_RENDERER_CONFIG_NONFATAL_RANGE);

        return XA_NO_ERROR;

    case XA_RENDERER_CONFIG_PARAM_CB:
        /* ...set opaque callback data function */
        d->cdata = (xa_renderer_cb_t *)pv_value;

        return XA_NO_ERROR;

    case XA_RENDERER_CONFIG_PARAM_STATE:
        /* ...runtime state control parameter valid only in execution state */
        XF_CHK_ERR(d->state & XA_RENDERER_FLAG_POSTINIT_DONE, XA_RENDERER_CONFIG_FATAL_STATE);

        /* ...get requested state */
        i_value = (UWORD32) *(WORD32 *)pv_value;

        /* ...pass to state control hook */
        return xa_hw_renderer_control(d, i_value);

    default:
        /* ...unrecognized parameter */
        return XF_CHK_ERR(0, XA_API_FATAL_INVALID_CMD_TYPE);
    }
}

/* ...retrieve configuration parameter */
static XA_ERRORCODE xa_renderer_get_config_param(XARenderer *d, WORD32 i_idx, pVOID pv_value)
{
    /* ...sanity check - renderer must be initialized */
    XF_CHK_ERR(d && pv_value, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...make sure pre-initialization is completed */
    XF_CHK_ERR(d->state & XA_RENDERER_FLAG_PREINIT_DONE, XA_RENDERER_CONFIG_FATAL_STATE);

    /* ...process individual configuration parameter */
    switch (i_idx)
    {
    case XA_RENDERER_CONFIG_PARAM_PCM_WIDTH:
        /* ...return current PCM width */
        *(WORD32 *)pv_value = d->pcm_width;
        return XA_NO_ERROR;

    case XA_RENDERER_CONFIG_PARAM_CHANNELS:
        /* ...return current channel number */
        *(WORD32 *)pv_value = d->channels;
        return XA_NO_ERROR;

    case XA_RENDERER_CONFIG_PARAM_SAMPLE_RATE:
        /* ...return current sampling rate */
        *(WORD32 *)pv_value = d->rate;
        return XA_NO_ERROR;

    case XA_RENDERER_CONFIG_PARAM_FRAME_SIZE:
        /* ...return current audio frame length (in samples) */
        *(WORD32 *)pv_value = d->frame_size;
        return XA_NO_ERROR;

    case XA_RENDERER_CONFIG_PARAM_STATE:
        /* ...return current execution state */
        *(WORD32 *)pv_value = xa_hw_renderer_get_state(d);
        return XA_NO_ERROR;

    case XA_RENDERER_CONFIG_PARAM_BYTES_PRODUCED:
        /* ...return current execution state */
        *(UWORD64 *)pv_value = d->bytes_produced;
        return XA_NO_ERROR;

    default:
        /* ...unrecognized parameter */
        return XF_CHK_ERR(0, XA_API_FATAL_INVALID_CMD_TYPE);
    }
}

/* ...execution command */
static XA_ERRORCODE xa_renderer_execute(XARenderer *d, WORD32 i_idx, pVOID pv_value)
{
    /* ...sanity check - pointer must be valid */
    XF_CHK_ERR(d, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...renderer must be in running state */
    XF_CHK_ERR(d->state & (XA_RENDERER_FLAG_RUNNING | XA_RENDERER_FLAG_IDLE), XA_RENDERER_EXEC_FATAL_STATE);

    /* ...process individual command type */
    switch (i_idx)
    {
    case XA_CMD_TYPE_DO_EXECUTE:
        /* ...silently ignore; everything is done in "set_input" */
        return XA_NO_ERROR;

    case XA_CMD_TYPE_DONE_QUERY:
        /* ...always report "no" - tbd - is that needed at all? */
        XF_CHK_ERR(pv_value, XA_API_FATAL_INVALID_CMD_TYPE);
        *(WORD32 *)pv_value = 0;
        return XA_NO_ERROR;

    case XA_CMD_TYPE_DO_RUNTIME_INIT:
        /* ...silently ignore */
        return XA_NO_ERROR;

    default:
        /* ...unrecognized command */
        return XF_CHK_ERR(0, XA_API_FATAL_INVALID_CMD_TYPE);
    }
}

/* ...set number of input bytes */
static XA_ERRORCODE xa_renderer_set_input_bytes(XARenderer *d, WORD32 i_idx, pVOID pv_value)
{
    UWORD32     size;

    XF_CHK_ERR(d && pv_value, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...make sure it is an input port  */
    XF_CHK_ERR(i_idx == 0, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...renderer must be initialized */
    XF_CHK_ERR(d->state & XA_RENDERER_FLAG_POSTINIT_DONE, XA_RENDERER_EXEC_FATAL_STATE);

    /* ...input buffer pointer must be valid */
    XF_CHK_ERR(d->input, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...check buffer size is sane */
    XF_CHK_ERR((size = *(UWORD32 *)pv_value / d->sample_size) >= 0, XA_RENDERER_EXEC_FATAL_INPUT);

    /* ...make sure we have integral amount of samples */
    XF_CHK_ERR(size * d->sample_size == *(UWORD32 *)pv_value, XA_RENDERER_EXEC_FATAL_INPUT);

    /* ...track total number of bytes submitted */
    d->bytes_produced += (*(UWORD64 *)pv_value);

    /* ...submit chunk of data into ring buffer */
    d->consumed = d->sample_size * xa_hw_renderer_submit(d, d->input, size);

    TRACE(PROCESS, _b("xa_renderer_set_input_bytes[state: %u]: consumed: %u"), d->state, d->consumed);

    /* ...all is correct */
    return XA_NO_ERROR;
}

/* ...get number of consumed bytes */
static XA_ERRORCODE xa_renderer_get_curidx_input_buf(XARenderer *d, WORD32 i_idx, pVOID pv_value)
{
    /* ...sanity check - check parameters */
    XF_CHK_ERR(d && pv_value, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...input buffer index must be valid */
    XF_CHK_ERR(i_idx == 0, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...renderer must be in post-init state */
    XF_CHK_ERR(d->state & XA_RENDERER_FLAG_POSTINIT_DONE, XA_RENDERER_EXEC_FATAL_STATE);

    /* ...input buffer must exist */
    XF_CHK_ERR(d->input, XA_RENDERER_EXEC_FATAL_INPUT);

    TRACE(PROCESS, _b("xa_renderer_get_curidx_input_buf[%u]"), d->consumed);

    /* ...return number of bytes consumed */
    *(WORD32 *)pv_value = d->consumed;

    /* ...reset consumed value to zero */
    d->consumed = 0;

    return XA_NO_ERROR;
}

/*******************************************************************************
 * Memory information API
 ******************************************************************************/

/* ..get total amount of data for memory tables */
static XA_ERRORCODE xa_renderer_get_memtabs_size(XARenderer *d, WORD32 i_idx, pVOID pv_value)
{
    /* ...basic sanity checks */
    XF_CHK_ERR(d && pv_value, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...check renderer is pre-initialized */
    XF_CHK_ERR(d->state & XA_RENDERER_FLAG_PREINIT_DONE, XA_RENDERER_CONFIG_FATAL_STATE);

    /* ...we have all our tables inside API structure */
    *(WORD32 *)pv_value = 0;

    return XA_NO_ERROR;
}

/* ..set memory tables pointer */
static XA_ERRORCODE xa_renderer_set_memtabs_ptr(XARenderer *d, WORD32 i_idx, pVOID pv_value)
{
    /* ...basic sanity checks */
    XF_CHK_ERR(d && pv_value, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...check renderer is pre-initialized */
    XF_CHK_ERR(d->state & XA_RENDERER_FLAG_PREINIT_DONE, XA_RENDERER_CONFIG_FATAL_STATE);

    /* ...do not do anything; just return success - tbd */
    return XA_NO_ERROR;
}

/* ...return total amount of memory buffers */
static XA_ERRORCODE xa_renderer_get_n_memtabs(XARenderer *d, WORD32 i_idx, pVOID pv_value)
{
    /* ...basic sanity checks */
    XF_CHK_ERR(d && pv_value, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...we have 1 input buffer only */
    *(WORD32 *)pv_value = 1;

    return XA_NO_ERROR;
}

/* ...return memory buffer data */
static XA_ERRORCODE xa_renderer_get_mem_info_size(XARenderer *d, WORD32 i_idx, pVOID pv_value)
{
    UWORD32     i_value;

    /* ...basic sanity check */
    XF_CHK_ERR(d && pv_value, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...command valid only after post-initialization step */
    XF_CHK_ERR(d->state & XA_RENDERER_FLAG_POSTINIT_DONE, XA_RENDERER_CONFIG_FATAL_STATE);

    switch (i_idx)
    {
    case 0:
        /* ...input buffer specification; accept exact audio frame */
    	i_value = BUFFER_SIZE * SAMPLE_SIZE;
        break;

    default:
        /* ...invalid index */
        return XF_CHK_ERR(0, XA_API_FATAL_INVALID_CMD_TYPE);
    }

    /* ...return buffer size to caller */
    *(WORD32 *)pv_value = (WORD32) i_value;

    return XA_NO_ERROR;
}

/* ...return memory alignment data */
static XA_ERRORCODE xa_renderer_get_mem_info_alignment(XARenderer *d, WORD32 i_idx, pVOID pv_value)
{
    /* ...basic sanity check */
    XF_CHK_ERR(d && pv_value, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...command valid only after post-initialization step */
    XF_CHK_ERR(d->state & XA_RENDERER_FLAG_POSTINIT_DONE, XA_RENDERER_CONFIG_FATAL_STATE);

    /* ...all buffers are at least 4-bytes aligned */
    *(WORD32 *)pv_value = 4;

    return XA_NO_ERROR;
}

/* ...return memory type data */
static XA_ERRORCODE xa_renderer_get_mem_info_type(XARenderer *d, WORD32 i_idx, pVOID pv_value)
{
    /* ...basic sanity check */
    XF_CHK_ERR(d && pv_value, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...command valid only after post-initialization step */
    XF_CHK_ERR(d->state & XA_RENDERER_FLAG_POSTINIT_DONE, XA_RENDERER_CONFIG_FATAL_STATE);

    switch (i_idx)
    {
    case 0:
        /* ...input buffers */
        *(WORD32 *)pv_value = XA_MEMTYPE_INPUT;
        return XA_NO_ERROR;

    default:
        /* ...invalid index */
        return XF_CHK_ERR(0, XA_API_FATAL_INVALID_CMD_TYPE);
    }
}

/* ...set memory pointer */
static XA_ERRORCODE xa_renderer_set_mem_ptr(XARenderer *d, WORD32 i_idx, pVOID pv_value)
{
    /* ...basic sanity check */
    XF_CHK_ERR(d && pv_value, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...codec must be initialized */
    XF_CHK_ERR(d->state & XA_RENDERER_FLAG_POSTINIT_DONE, XA_API_FATAL_INVALID_CMD_TYPE);

    TRACE(PROCESS, _b("xa_renderer_set_mem_ptr[%u]: %p"), i_idx, pv_value);

    /* ...select memory buffer */
    switch (i_idx)
    {
    case 0:
        /* ...input buffer */
        d->input = pv_value;
        return XA_NO_ERROR;

    default:
        /* ...invalid index */
        return XF_CHK_ERR(0, XA_API_FATAL_INVALID_CMD_TYPE);
    }
}

/* ...set input over */
static XA_ERRORCODE xa_renderer_input_over(XARenderer *d, WORD32 i_idx, pVOID pv_value)
{
    return XA_NO_ERROR;
}

/*******************************************************************************
 * API command hooks
 ******************************************************************************/

static XA_ERRORCODE (* const xa_renderer_api[])(XARenderer *, WORD32, pVOID) =
{
    [XA_API_CMD_GET_API_SIZE]           = xa_renderer_get_api_size,
    [XA_API_CMD_INIT]                   = xa_renderer_init,
    [XA_API_CMD_SET_CONFIG_PARAM]       = xa_renderer_set_config_param,
    [XA_API_CMD_GET_CONFIG_PARAM]       = xa_renderer_get_config_param,
    [XA_API_CMD_EXECUTE]                = xa_renderer_execute,
    [XA_API_CMD_SET_INPUT_BYTES]        = xa_renderer_set_input_bytes,
    [XA_API_CMD_GET_CURIDX_INPUT_BUF]   = xa_renderer_get_curidx_input_buf,
    [XA_API_CMD_GET_MEMTABS_SIZE]       = xa_renderer_get_memtabs_size,
    [XA_API_CMD_SET_MEMTABS_PTR]        = xa_renderer_set_memtabs_ptr,
    [XA_API_CMD_GET_N_MEMTABS]          = xa_renderer_get_n_memtabs,
    [XA_API_CMD_GET_MEM_INFO_SIZE]      = xa_renderer_get_mem_info_size,
    [XA_API_CMD_GET_MEM_INFO_ALIGNMENT] = xa_renderer_get_mem_info_alignment,
    [XA_API_CMD_GET_MEM_INFO_TYPE]      = xa_renderer_get_mem_info_type,
    [XA_API_CMD_SET_MEM_PTR]            = xa_renderer_set_mem_ptr,
    [XA_API_CMD_INPUT_OVER]             = xa_renderer_input_over,
};

/* ...total numer of commands supported */
#define XA_RENDERER_API_COMMANDS_NUM   (sizeof(xa_renderer_api) / sizeof(xa_renderer_api[0]))

/*******************************************************************************
 * API entry point
 ******************************************************************************/

XA_ERRORCODE xa_renderer(xa_codec_handle_t p_xa_module_obj, WORD32 i_cmd, WORD32 i_idx, pVOID pv_value)
{
    XARenderer *renderer = (XARenderer *) p_xa_module_obj;

    /* ...check if command index is sane */
    XF_CHK_ERR(i_cmd < XA_RENDERER_API_COMMANDS_NUM, XA_API_FATAL_INVALID_CMD);

    /* ...see if command is defined */
    XF_CHK_ERR(xa_renderer_api[i_cmd], XA_API_FATAL_INVALID_CMD);

    /* ...execute requested command */
    return xa_renderer_api[i_cmd](renderer, i_idx, pv_value);
}
