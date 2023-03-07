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
 * xa-capturer.c
 *
 * Hardware capturer implementation
 ******************************************************************************/

#define MODULE_TAG                      CAPTURER

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <xtensa/xos.h>

#include "audio/xa-capturer-api.h"
#include "xf-debug.h"

#include "fsl_dma.h"
#include "fsl_dmic.h"
#include "fsl_dmic_dma.h"

/*******************************************************************************
 * Tracing configuration
 ******************************************************************************/
TRACE_TAG(INIT, 1);
TRACE_TAG(PROCESS, 1);
TRACE_TAG(UNDERRUN, 1);

/*******************************************************************************
 * Hardware parameters
 ******************************************************************************/
#define DMA_CAPTURER DMA1

#define BUFFER_LENGTH 128U
#define BUFFER_COUNT 2U
#define DMIC_CH_COUNT 8U
#define DMAREQ_DMIC_CH0 16U
#define FIFO_DEPTH 15U

#ifdef CPU_MIMXRT595SFFOA_dsp
#define REF_MIC0 0U
#define REF_MIC1 1U
#define REF_MIC2 2U
#define REF_MIC3 3U
#define REF_MIC4 4U
#define REF_MIC5 5U
#define REF_MIC6 6U
#define REF_MIC7 7U
#else
#define REF_MIC0 6U
#define REF_MIC1 7U
#define REF_MIC2 5U
#define REF_MIC3 4U
#define REF_MIC4 3U
#define REF_MIC5 2U
#define REF_MIC6 1U
#define REF_MIC7 0U
#endif

typedef struct _circular_buf_t
{
    uint16_t (*buffer)[BUFFER_LENGTH];
    uint8_t pingpong;
    bool full;
} circular_buf_t;

typedef struct XACapturer
{
    /***************************************************************************
     * Internal stuff
     **************************************************************************/

    /* ...component state */
    UWORD32                     state;

    /* ...notification callback pointer */
    xa_capturer_cb_t           *cdata;

    /* ...input buffer pointer */
    void                       *output;

    /* ...number of samples produced */
    UWORD32                     produced;

    /***************************************************************************
     * Run-time data
     **************************************************************************/

    /* ...size of PCM sample (respects channels and PCM width) */
    UWORD32                     sample_size;

    /* ...number of channels */
    UWORD32                     channels;

    /* ...interleave */
    UWORD32                     interleave;

    /* ...sample width */
    UWORD32                     pcm_width;

    /* ...current sampling rate */
    UWORD32                     rate;

    UWORD32                     over_flow_flag;

    /* total bytes produced by the component*/
    UWORD64                     tot_bytes_produced;

    /* total bytes to be produced*/
    UWORD64                     bytes_end;

    circular_buf_t              circular_buf_h;
    WORD32                      newDataAvailable;

    /* ...DMIC dma handle for 8 channel */
    dmic_dma_handle_t s_dmicDmaHandle[FSL_FEATURE_DMIC_CHANNEL_NUM];
    dma_handle_t s_dmaHandle[FSL_FEATURE_DMIC_CHANNEL_NUM];
} XACapturer;

/*******************************************************************************
 * Operating flags
 ******************************************************************************/
#define XA_CAPTURER_FLAG_PREINIT_DONE   (1 << 0)
#define XA_CAPTURER_FLAG_POSTINIT_DONE  (1 << 1)
#define XA_CAPTURER_FLAG_IDLE           (1 << 2)
#define XA_CAPTURER_FLAG_RUNNING        (1 << 3)
#define XA_CAPTURER_FLAG_PAUSED         (1 << 4)

/*******************************************************************************
* Variables
******************************************************************************/
/* Receive ping pong buffer and descriptor */
AT_NONCACHEABLE_SECTION_ALIGN(
    static uint16_t g_rxBuffer[DMIC_CH_COUNT * BUFFER_COUNT][BUFFER_LENGTH], 4
);

AT_NONCACHEABLE_SECTION_ALIGN(
    dma_descriptor_t s_dmaDescriptorPingpong[FSL_FEATURE_DMIC_CHANNEL_NUM * 2], 16
);

static dmic_transfer_t s_receiveXfer[FSL_FEATURE_DMIC_CHANNEL_NUM * 2] = {
    /* transfer configurations for channel0 */
    {
        .data = &g_rxBuffer[0][0],
        .dataWidth = sizeof(uint16_t),
        .dataSize = sizeof(uint16_t) * BUFFER_LENGTH,
        .dataAddrInterleaveSize = kDMA_AddressInterleave1xWidth,
        .linkTransfer = &s_receiveXfer[1],
    },

    {
        .data = &g_rxBuffer[1][0],
        .dataWidth = sizeof(uint16_t),
        .dataSize = sizeof(uint16_t) * BUFFER_LENGTH,
        .dataAddrInterleaveSize = kDMA_AddressInterleave1xWidth,
        .linkTransfer = &s_receiveXfer[0],
    },

    /* transfer configurations for channel1 */
    {
        .data = &g_rxBuffer[2][0],
        .dataWidth = sizeof(uint16_t),
        .dataSize = sizeof(uint16_t) * BUFFER_LENGTH,
        .dataAddrInterleaveSize = kDMA_AddressInterleave1xWidth,
        .linkTransfer = &s_receiveXfer[3],
    },

    {
        .data = &g_rxBuffer[3][0],
        .dataWidth = sizeof(uint16_t),
        .dataSize = sizeof(uint16_t) * BUFFER_LENGTH,
        .dataAddrInterleaveSize = kDMA_AddressInterleave1xWidth,
        .linkTransfer = &s_receiveXfer[2],
    },

    /* transfer configurations for channel2 */
    {
        .data = &g_rxBuffer[4][0],
        .dataWidth = sizeof(uint16_t),
        .dataSize = sizeof(uint16_t) * BUFFER_LENGTH,
        .dataAddrInterleaveSize = kDMA_AddressInterleave1xWidth,
        .linkTransfer = &s_receiveXfer[5],
    },

    {
        .data = &g_rxBuffer[5][0],
        .dataWidth = sizeof(uint16_t),
        .dataSize = sizeof(uint16_t) * BUFFER_LENGTH,
        .dataAddrInterleaveSize = kDMA_AddressInterleave1xWidth,
        .linkTransfer = &s_receiveXfer[4],
    },

    /* transfer configurations for channel3 */
    {
        .data = &g_rxBuffer[6][0],
        .dataWidth = sizeof(uint16_t),
        .dataSize = sizeof(uint16_t) * BUFFER_LENGTH,
        .dataAddrInterleaveSize = kDMA_AddressInterleave1xWidth,
        .linkTransfer = &s_receiveXfer[7],
    },

    {
        .data = &g_rxBuffer[7][0],
        .dataWidth = sizeof(uint16_t),
        .dataSize = sizeof(uint16_t) * BUFFER_LENGTH,
        .dataAddrInterleaveSize = kDMA_AddressInterleave1xWidth,
        .linkTransfer = &s_receiveXfer[6],
    },

    /* transfer configurations for channel4 */
    {
        .data = &g_rxBuffer[8][0],
        .dataWidth = sizeof(uint16_t),
        .dataSize = sizeof(uint16_t) * BUFFER_LENGTH,
        .dataAddrInterleaveSize = kDMA_AddressInterleave1xWidth,
        .linkTransfer = &s_receiveXfer[9],
    },

    {
        .data = &g_rxBuffer[9][0],
        .dataWidth = sizeof(uint16_t),
        .dataSize = sizeof(uint16_t) * BUFFER_LENGTH,
        .dataAddrInterleaveSize = kDMA_AddressInterleave1xWidth,
        .linkTransfer = &s_receiveXfer[8],
    },

    /* transfer configurations for channel5 */
    {
        .data = &g_rxBuffer[10][0],
        .dataWidth = sizeof(uint16_t),
        .dataSize = sizeof(uint16_t) * BUFFER_LENGTH,
        .dataAddrInterleaveSize = kDMA_AddressInterleave1xWidth,
        .linkTransfer = &s_receiveXfer[11],
    },

    {
        .data = &g_rxBuffer[11][0],
        .dataWidth = sizeof(uint16_t),
        .dataSize = sizeof(uint16_t) * BUFFER_LENGTH,
        .dataAddrInterleaveSize = kDMA_AddressInterleave1xWidth,
        .linkTransfer = &s_receiveXfer[10],
    },

    /* transfer configurations for channel6 */
    {
        .data = &g_rxBuffer[12][0],
        .dataWidth = sizeof(uint16_t),
        .dataSize = sizeof(uint16_t) * BUFFER_LENGTH,
        .dataAddrInterleaveSize = kDMA_AddressInterleave1xWidth,
        .linkTransfer = &s_receiveXfer[13],
    },

    {
        .data = &g_rxBuffer[13][0],
        .dataWidth = sizeof(uint16_t),
        .dataSize = sizeof(uint16_t) * BUFFER_LENGTH,
        .dataAddrInterleaveSize = kDMA_AddressInterleave1xWidth,
        .linkTransfer = &s_receiveXfer[12],
    },

    /* transfer configurations for channel7 */
    {
        .data = &g_rxBuffer[14][0],
        .dataWidth = sizeof(uint16_t),
        .dataSize = sizeof(uint16_t) * BUFFER_LENGTH,
        .dataAddrInterleaveSize = kDMA_AddressInterleave1xWidth,
        .linkTransfer = &s_receiveXfer[15],
    },

    {
        .data = &g_rxBuffer[15][0],
        .dataWidth = sizeof(uint16_t),
        .dataSize = sizeof(uint16_t) * BUFFER_LENGTH,
        .dataAddrInterleaveSize = kDMA_AddressInterleave1xWidth,
        .linkTransfer = &s_receiveXfer[14],
    },
};

static dmic_channel_config_t s_dmicChannelConfig = {
    .divhfclk = kDMIC_PdmDiv1,
    .osr = 32U,
    .gainshft = 4U,
    .preac2coef = kDMIC_CompValueZero,
    .preac4coef = kDMIC_CompValueZero,
    .dc_cut_level = kDMIC_DcCut155,
    .post_dc_gain_reduce = 4U,
    .saturate16bit = 1U,
    .sample_rate = kDMIC_PhyFullSpeed,
};

/*******************************************************************************
 * Local functions
 ******************************************************************************/

void DMIC_Callback(DMIC_Type *base, dmic_dma_handle_t *handle, status_t status, void *userData)
{
    XACapturer *capturer = (XACapturer*) userData;
    uint32_t i;

    if (status == kStatus_DMIC_Idle)
    {
        for (i = 0; i < FSL_FEATURE_DMIC_CHANNEL_NUM; i++)
        {
            capturer->newDataAvailable |= (1 << i);			
        }

        if (capturer->over_flow_flag == 0)
        {
            capturer->over_flow_flag = 1;
            capturer->cdata->cb(capturer->cdata, 0);
        }
    }
}

/* Hardware-specific initialization
 * Start DMIC DMA transfers */
static void evk_dmic_dma_start(XACapturer *d)
{
    uint32_t i;

    DMIC_EnableChannnel(DMIC0, DMIC_CHANEN_EN_CH0_MASK | DMIC_CHANEN_EN_CH1_MASK | DMIC_CHANEN_EN_CH2_MASK | DMIC_CHANEN_EN_CH3_MASK | DMIC_CHANEN_EN_CH4_MASK | DMIC_CHANEN_EN_CH5_MASK | DMIC_CHANEN_EN_CH6_MASK | DMIC_CHANEN_EN_CH7_MASK);

    for (i = 0; i < FSL_FEATURE_DMIC_CHANNEL_NUM; i++)
    {
        DMIC_TransferReceiveDMA(DMIC0, &d->s_dmicDmaHandle[i], &s_receiveXfer[i*2], kDMIC_Channel0 + i);
    }
}

/* Hardware-specific initialization
 * Setup DMIC and DMA */
static void evk_dmic_dma_config(void* ptr)
{
    dmic_channel_config_t dmic_channel_cfg;
    uint32_t i;

    XACapturer *capturer = (XACapturer*) ptr;

    RESET_PeripheralReset(kDMAC0_RST_SHIFT_RSTn);

    DMIC_Init(DMIC0);
#ifndef FSL_FEATURE_DMIC_HAS_NO_IOCFG
    DMIC_SetIOCFG(DMIC0, kDMIC_PdmStereo | kDMIC_PdmStereo2 | kDMIC_PdmStereo4 | kDMIC_PdmStereo6);
#endif
    DMIC_Use2fs(DMIC0, false);

    for (i = 0; i < FSL_FEATURE_DMIC_CHANNEL_NUM; i++)
    {
        DMIC_EnableChannelDma(DMIC0, (dmic_channel_t)(kDMIC_Channel0 + i), true);
    }

    for (i = 0; i < FSL_FEATURE_DMIC_CHANNEL_NUM; i+=2)
    {
        DMIC_ConfigChannel(DMIC0, (dmic_channel_t)(kDMIC_Channel0 + i), kDMIC_Left, &s_dmicChannelConfig);
        DMIC_FifoChannel(DMIC0, kDMIC_Channel0 + i, FIFO_DEPTH, true, true);
        DMIC_ConfigChannel(DMIC0, (dmic_channel_t)(kDMIC_Channel0 + i + 1), kDMIC_Right, &s_dmicChannelConfig);
        DMIC_FifoChannel(DMIC0, kDMIC_Channel0 + i + 1, FIFO_DEPTH, true, true);
    }

    for (i = 0; i < FSL_FEATURE_DMIC_CHANNEL_NUM; i++)
    {
        DMA_EnableChannel(DMA_CAPTURER, DMAREQ_DMIC_CH0 + i);
        DMA_SetChannelPriority(DMA_CAPTURER, DMAREQ_DMIC_CH0 + i, kDMA_ChannelPriority0);
        /* Request dma channels from DMA manager. */
        DMA_CreateHandle(&capturer->s_dmaHandle[i], DMA_CAPTURER, DMAREQ_DMIC_CH0 + i);
        if (i != 7)
        {
            DMA_DisableChannelInterrupts(DMA_CAPTURER, DMAREQ_DMIC_CH0 + i);
            DMIC_TransferCreateHandleDMA(DMIC0, &capturer->s_dmicDmaHandle[i], NULL, NULL, &capturer->s_dmaHandle[i]);
        }
        else
        {
            DMIC_TransferCreateHandleDMA(DMIC0, &capturer->s_dmicDmaHandle[i], DMIC_Callback, capturer, &capturer->s_dmaHandle[i]);
        }

        DMIC_InstallDMADescriptorMemory(&capturer->s_dmicDmaHandle[i], &s_dmaDescriptorPingpong[i * 2U], 2U);
    }
}

static void evk_hw_capturer_init(void* ptr)
{
    XACapturer *d = (XACapturer*) ptr;

    d->circular_buf_h.buffer = g_rxBuffer;
    d->circular_buf_h.pingpong = 0;
    d->circular_buf_h.full = false;

    evk_dmic_dma_config(ptr);
}

/*******************************************************************************
 * Codec access functions
 ******************************************************************************/
static inline void xa_hw_capturer_close(XACapturer *d)
{
    /* TODO: implement proper close device. */
}

/* ...submit data (in samples) into internal capturer ring-buffer */

/*******************************************************************************
 * API command hooks
 ******************************************************************************/
/* ...standard codec initialization routine */
static XA_ERRORCODE xa_capturer_get_api_size(XACapturer *d, WORD32 i_idx, pVOID pv_value)
{
    /* ...check parameters are sane */
    XF_CHK_ERR(pv_value, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...retrieve API structure size */
    *(WORD32 *)pv_value = sizeof(*d);

    return XA_NO_ERROR;
}

static XA_ERRORCODE xa_hw_capturer_init (XACapturer *d)
{
    d->produced = 0;
    d->tot_bytes_produced = 0;
    d->over_flow_flag = 0;

    evk_hw_capturer_init(d);

    return XA_NO_ERROR;
}

/* ...standard codec initialization routine */
static XA_ERRORCODE xa_capturer_init(XACapturer *d, WORD32 i_idx, pVOID pv_value)
{
    int status;
    /* ...sanity check - pointer must be valid */
    XF_CHK_ERR(d, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...process particular initialization type */
    switch (i_idx)
    {
    case XA_CMD_TYPE_INIT_API_PRE_CONFIG_PARAMS:
    {
        /* ...pre-configuration initialization; reset internal data */
        memset(d, 0, sizeof(*d));

        /* ...set default capturer parameters - 16-bit stereo @ 16KHz */
        d->sample_size = 2;
        d->channels = 2;
        d->pcm_width = 16;
        d->interleave = 0;
        d->rate = 16000;

        /* ...and mark capturer has been created */
        d->state = XA_CAPTURER_FLAG_PREINIT_DONE;

        return XA_NO_ERROR;
    }

    case XA_CMD_TYPE_INIT_API_POST_CONFIG_PARAMS:
    {
        /* ...post-configuration initialization (all parameters are set) */

        XF_CHK_ERR(d->state & XA_CAPTURER_FLAG_PREINIT_DONE, XA_API_FATAL_INVALID_CMD_TYPE);

        XF_CHK_ERR(xa_hw_capturer_init(d) == 0, XA_CAPTURER_CONFIG_FATAL_HW);

        /* ...mark post-initialization is complete */
        d->state |= XA_CAPTURER_FLAG_POSTINIT_DONE;

        return XA_NO_ERROR;
    }

    case XA_CMD_TYPE_INIT_PROCESS:
    {
        /* ...kick run-time initialization process; make sure setup is complete */
        XF_CHK_ERR(d->state & XA_CAPTURER_FLAG_POSTINIT_DONE, XA_API_FATAL_INVALID_CMD_TYPE);
        /* ...mark capturer is in idle state */
        d->state |= XA_CAPTURER_FLAG_IDLE;

        evk_dmic_dma_start(d);

        return XA_NO_ERROR;
    }

    case XA_CMD_TYPE_INIT_DONE_QUERY:
    {
        /* ...check if initialization is done; make sure pointer is sane */
        XF_CHK_ERR(pv_value, XA_API_FATAL_INVALID_CMD_TYPE);

        /* ...put current status */
        *(WORD32 *)pv_value = (d->state & XA_CAPTURER_FLAG_IDLE ? 1 : 0);

        d->state ^= XA_CAPTURER_FLAG_IDLE | XA_CAPTURER_FLAG_RUNNING;

        return XA_NO_ERROR;
    }

    default:
        /* ...unrecognized command type */
        return XF_CHK_ERR(0, XA_API_FATAL_INVALID_CMD_TYPE);
    }
}

/* ...HW-capturer control function */
static inline XA_ERRORCODE xa_hw_capturer_control(XACapturer *d, UWORD32 state)
{
    switch (state)
    {
    case XA_CAPTURER_STATE_RUN:
        /* ...capturer must be in paused state */
        XF_CHK_ERR(d->state & XA_CAPTURER_FLAG_PAUSED, XA_CAPTURER_EXEC_NONFATAL_STATE);

        /* ...mark capturer is running */
        d->state ^= XA_CAPTURER_FLAG_RUNNING | XA_CAPTURER_FLAG_PAUSED;

        return XA_NO_ERROR;

    case XA_CAPTURER_STATE_PAUSE:
        /* ...capturer must be in running state */
        XF_CHK_ERR(d->state & XA_CAPTURER_FLAG_RUNNING, XA_CAPTURER_EXEC_NONFATAL_STATE);

        /* ...pause capturer operation */
        /* TODO: implement pause functionality */

        /* ...mark capturer is paused */
        d->state ^= XA_CAPTURER_FLAG_RUNNING | XA_CAPTURER_FLAG_PAUSED;

        return XA_NO_ERROR;

    case XA_CAPTURER_STATE_IDLE:
        /* ...command is valid in any active state; stop capturer operation */
        xa_hw_capturer_close(d);

        /* ...reset capturer flags */
        d->state &= ~(XA_CAPTURER_FLAG_RUNNING | XA_CAPTURER_FLAG_PAUSED);

        return XA_NO_ERROR;

    default:
        /* ...unrecognized command */
        return XF_CHK_ERR(0, XA_API_FATAL_INVALID_CMD_TYPE);
    }
}

/* ...set capturer configuration parameter */
static XA_ERRORCODE xa_capturer_set_config_param(XACapturer *d, WORD32 i_idx, pVOID pv_value)
{
    UWORD32     i_value;

    /* ...sanity check - pointers must be sane */
    XF_CHK_ERR(d && pv_value, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...pre-initialization must be completed */
    XF_CHK_ERR(d->state & XA_CAPTURER_FLAG_PREINIT_DONE, XA_CAPTURER_CONFIG_FATAL_STATE);

    /* ...process individual configuration parameter */
    switch (i_idx)
    {
    case XA_CAPTURER_CONFIG_PARAM_PCM_WIDTH:
        /* ...command is valid only in configuration state */
        XF_CHK_ERR((d->state & XA_CAPTURER_FLAG_POSTINIT_DONE) == 0, XA_CAPTURER_CONFIG_FATAL_STATE);

        /* ...get requested PCM width */
        i_value = (UWORD32) *(WORD32 *)pv_value;

        /* ...check value is permitted (16 bits only) */
        XF_CHK_ERR(i_value == 16, XA_CAPTURER_CONFIG_NONFATAL_RANGE);

        /* ...apply setting */
        d->pcm_width = i_value;

        return XA_NO_ERROR;

    case XA_CAPTURER_CONFIG_PARAM_CHANNELS:
        /* ...command is valid only in configuration state */
        XF_CHK_ERR((d->state & XA_CAPTURER_FLAG_POSTINIT_DONE) == 0, XA_CAPTURER_CONFIG_FATAL_STATE);

        /* ...get requested channel number */
        i_value = (UWORD32) *(WORD32 *)pv_value;

        /* ...allow stereo only */
        XF_CHK_ERR((i_value >= 1 && i_value <= 8), XA_CAPTURER_CONFIG_NONFATAL_RANGE);

        /* ...apply setting */
        d->channels = (UWORD32)i_value;

        return XA_NO_ERROR;

    case XA_CAPTURER_CONFIG_PARAM_INTERLEAVE:
        /* ...command is valid only in configuration state */
        XF_CHK_ERR((d->state & XA_CAPTURER_FLAG_POSTINIT_DONE) == 0, XA_CAPTURER_CONFIG_FATAL_STATE);

        i_value = (UWORD32) *(WORD32 *)pv_value;

        XF_CHK_ERR((i_value >= 0 && i_value <= 1), XA_CAPTURER_CONFIG_NONFATAL_RANGE);

        /* ...apply setting */
        d->interleave = (UWORD32)i_value;

        return XA_NO_ERROR;

    case XA_CAPTURER_CONFIG_PARAM_SAMPLE_RATE:
        /* ...command is valid only in configuration state */
        XF_CHK_ERR((d->state & XA_CAPTURER_FLAG_POSTINIT_DONE) == 0, XA_CAPTURER_CONFIG_FATAL_STATE);

        /* ...get requested sampling rate */
        i_value = (UWORD32) *(WORD32 *)pv_value;

        /* ...allow 16 / 44.1 / 48 KHz */
        XF_CHK_ERR(i_value == 44100 || i_value == 48000 || i_value == 16000, XA_CAPTURER_CONFIG_NONFATAL_RANGE);

        /* ...apply setting */
        d->rate = (UWORD32)i_value;

        return XA_NO_ERROR;

    case XA_CAPTURER_CONFIG_PARAM_FRAME_SIZE:
        /* ...command is valid only in configuration state */
        XF_CHK_ERR((d->state & XA_CAPTURER_FLAG_POSTINIT_DONE) == 0, XA_CAPTURER_CONFIG_FATAL_STATE);

        /* ...get requested frame size */
        i_value = (UWORD32) *(WORD32 *)pv_value;

        /* ...check it is equal to the only frame size we support */
        XF_CHK_ERR(i_value == BUFFER_LENGTH, XA_CAPTURER_CONFIG_NONFATAL_RANGE);

        return XA_NO_ERROR;

    case XA_CAPTURER_CONFIG_PARAM_SAMPLE_END:
        /* ...command is valid only in configuration state */
        XF_CHK_ERR((d->state & XA_CAPTURER_FLAG_POSTINIT_DONE) == 0, XA_CAPTURER_CONFIG_FATAL_STATE);

        /* ...get requested frame size */
        d->bytes_end = ((UWORD64) *(UWORD64 *)pv_value)* (d->sample_size) * (d->channels) ;

        return XA_NO_ERROR;

    case XA_CAPTURER_CONFIG_PARAM_CB:
        /* ...set opaque callback data function */
        d->cdata = (xa_capturer_cb_t *)pv_value;

        return XA_NO_ERROR;

    case XA_CAPTURER_CONFIG_PARAM_STATE:
        /* ...runtime state control parameter valid only in execution state */
        XF_CHK_ERR(d->state & XA_CAPTURER_FLAG_POSTINIT_DONE, XA_CAPTURER_CONFIG_FATAL_STATE);

        /* ...get requested state */
        i_value = (UWORD32) *(WORD32 *)pv_value;

        /* ...pass to state control hook */
        return xa_hw_capturer_control(d, i_value);

    default:
        /* ...unrecognized parameter */
        return XF_CHK_ERR(0, XA_API_FATAL_INVALID_CMD_TYPE);
    }
}

/* ...state retrieval function */
static inline UWORD32 xa_hw_capturer_get_state(XACapturer *d)
{
    if (d->state & XA_CAPTURER_FLAG_RUNNING)
        return XA_CAPTURER_STATE_RUN;
    else if (d->state & XA_CAPTURER_FLAG_PAUSED)
        return XA_CAPTURER_STATE_PAUSE;
    else
        return XA_CAPTURER_STATE_IDLE;
}

/* ...retrieve configuration parameter */
static XA_ERRORCODE xa_capturer_get_config_param(XACapturer *d, WORD32 i_idx, pVOID pv_value)
{
    /* ...sanity check - capturer must be initialized */
    XF_CHK_ERR(d && pv_value, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...make sure pre-initialization is completed */
    XF_CHK_ERR(d->state & XA_CAPTURER_FLAG_PREINIT_DONE, XA_CAPTURER_CONFIG_FATAL_STATE);

    /* ...process individual configuration parameter */
    switch (i_idx)
    {
    case XA_CAPTURER_CONFIG_PARAM_PCM_WIDTH:
        /* ...return current PCM width */
        *(WORD32 *)pv_value = d->pcm_width;
        return XA_NO_ERROR;

    case XA_CAPTURER_CONFIG_PARAM_CHANNELS:
        /* ...return current channel number */
        *(WORD32 *)pv_value = d->channels;
        return XA_NO_ERROR;

    case XA_CAPTURER_CONFIG_PARAM_INTERLEAVE:
        /* ...return current channel interleave status */
        *(WORD32 *)pv_value = d->interleave;
        return XA_NO_ERROR;

    case XA_CAPTURER_CONFIG_PARAM_SAMPLE_RATE:
        /* ...return current sampling rate */
        *(WORD32 *)pv_value = d->rate;
        return XA_NO_ERROR;

    case XA_CAPTURER_CONFIG_PARAM_FRAME_SIZE:
        /* ...return current audio frame length (in samples) */
        *(WORD32 *)pv_value = BUFFER_LENGTH;
        return XA_NO_ERROR;

    case XA_CAPTURER_CONFIG_PARAM_STATE:
        /* ...return current execution state */
        *(WORD32 *)pv_value = xa_hw_capturer_get_state(d);
        return XA_NO_ERROR;

    case XA_CAPTURER_CONFIG_PARAM_BYTES_PRODUCED:
        /* ...return current execution state */
        *(WORD32 *)pv_value = d->tot_bytes_produced;
        return XA_NO_ERROR;

    default:
        /* ...unrecognized parameter */
        return XF_CHK_ERR(0, XA_API_FATAL_INVALID_CMD_TYPE);
    }
}

/* ...execution command */
static XA_ERRORCODE xa_capturer_execute(XACapturer *d, WORD32 i_idx, pVOID pv_value)
{
    /* ...sanity check - pointer must be valid */
    XF_CHK_ERR(d, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...capturer must be in running state */
    XF_CHK_ERR(d->state & (XA_CAPTURER_FLAG_RUNNING | XA_CAPTURER_FLAG_IDLE), XA_CAPTURER_EXEC_FATAL_STATE);

    /* ...process individual command type */
    switch (i_idx)
    {
    case XA_CMD_TYPE_DO_EXECUTE:
        /* ...silently ignore; everything is done in "set_input" */
        return XA_NO_ERROR;

    case XA_CMD_TYPE_DONE_QUERY:
        /* ...always report "no" - tbd - is that needed at all? */
        XF_CHK_ERR(pv_value, XA_API_FATAL_INVALID_CMD_TYPE);

        if ((d->produced == 0))
        {
            *(WORD32 *)pv_value = 1;
        }
        else
        {
            *(WORD32 *)pv_value = 0;
        }

        return XA_NO_ERROR;

    case XA_CMD_TYPE_DO_RUNTIME_INIT:
        /* ...silently ignore */
    	return XA_NO_ERROR;

    default:
        /* ...unrecognized command */
        return XF_CHK_ERR(0, XA_API_FATAL_INVALID_CMD_TYPE);
    }
}

/* ...get number of produced bytes */
static XA_ERRORCODE xa_capturer_get_output_bytes(XACapturer *d, WORD32 i_idx, pVOID pv_value)
{
    /* ...sanity check - check parameters */
    XF_CHK_ERR(d && pv_value, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...input buffer index must be valid */
    XF_CHK_ERR(i_idx == 0, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...capturer must be in post-init state */
    XF_CHK_ERR(d->state & XA_CAPTURER_FLAG_POSTINIT_DONE, XA_CAPTURER_EXEC_FATAL_STATE);

    /* ...input buffer must exist */
    XF_CHK_ERR(d->output, XA_CAPTURER_EXEC_FATAL_INPUT);

    /* ...return number of bytes produced */
    *(WORD32 *)pv_value = d->produced;

    d->produced = 0;

    return XA_NO_ERROR;
}

/*******************************************************************************
 * Memory information API
 ******************************************************************************/

/* ..get total amount of data for memory tables */
static XA_ERRORCODE xa_capturer_get_memtabs_size(XACapturer *d, WORD32 i_idx, pVOID pv_value)
{
    /* ...basic sanity checks */
    XF_CHK_ERR(d && pv_value, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...check capturer is pre-initialized */
    XF_CHK_ERR(d->state & XA_CAPTURER_FLAG_PREINIT_DONE, XA_CAPTURER_CONFIG_FATAL_STATE);

    /* ...we have all our tables inside API structure */
    *(WORD32 *)pv_value = 0;

    return XA_NO_ERROR;
}

/* ..set memory tables pointer */
static XA_ERRORCODE xa_capturer_set_memtabs_ptr(XACapturer *d, WORD32 i_idx, pVOID pv_value)
{
    /* ...basic sanity checks */
    XF_CHK_ERR(d && pv_value, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...check capturer is pre-initialized */
    XF_CHK_ERR(d->state & XA_CAPTURER_FLAG_PREINIT_DONE, XA_CAPTURER_CONFIG_FATAL_STATE);

    /* ...do not do anything; just return success - tbd */
    return XA_NO_ERROR;
}

/* ...return total amount of memory buffers */
static XA_ERRORCODE xa_capturer_get_n_memtabs(XACapturer *d, WORD32 i_idx, pVOID pv_value)
{
    /* ...basic sanity checks */
    XF_CHK_ERR(d && pv_value, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...we have 1 input buffer only */
    *(WORD32 *)pv_value = 1;

    return XA_NO_ERROR;
}

/* ...return memory buffer data */
static XA_ERRORCODE xa_capturer_get_mem_info_size(XACapturer *d, WORD32 i_idx, pVOID pv_value)
{
    UWORD32     i_value;

    /* ...basic sanity check */
    XF_CHK_ERR(d && pv_value, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...command valid only after post-initialization step */
    XF_CHK_ERR(d->state & XA_CAPTURER_FLAG_POSTINIT_DONE, XA_CAPTURER_CONFIG_FATAL_STATE);

    switch (i_idx)
    {
    case 0:
        /* ...input buffer specification; accept exact audio frame */
        i_value = BUFFER_LENGTH * d->channels * 2;
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
static XA_ERRORCODE xa_capturer_get_mem_info_alignment(XACapturer *d, WORD32 i_idx, pVOID pv_value)
{
    /* ...basic sanity check */
    XF_CHK_ERR(d && pv_value, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...command valid only after post-initialization step */
    XF_CHK_ERR(d->state & XA_CAPTURER_FLAG_POSTINIT_DONE, XA_CAPTURER_CONFIG_FATAL_STATE);

    /* ...all buffers are at least 4-bytes aligned */
    *(WORD32 *)pv_value = 4;

    return XA_NO_ERROR;
}

/* ...return memory type data */
static XA_ERRORCODE xa_capturer_get_mem_info_type(XACapturer *d, WORD32 i_idx, pVOID pv_value)
{
    /* ...basic sanity check */
    XF_CHK_ERR(d && pv_value, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...command valid only after post-initialization step */
    XF_CHK_ERR(d->state & XA_CAPTURER_FLAG_POSTINIT_DONE, XA_CAPTURER_CONFIG_FATAL_STATE);

    switch (i_idx)
    {
    case 0:
        /* ...input buffers */
        *(WORD32 *)pv_value = XA_MEMTYPE_OUTPUT;
        return XA_NO_ERROR;

    default:
        /* ...invalid index */
        return XF_CHK_ERR(0, XA_API_FATAL_INVALID_CMD_TYPE);
    }
}

static inline UWORD32 xa_hw_capturer_read_FIFO(XACapturer *d)
{
    WORD16* out_buffer_ptr = NULL;
    UWORD32 i;

    d->over_flow_flag = 0;
    out_buffer_ptr = d->output;

    if (d->channels == 1)
    {
        for (i = 0 ; i < BUFFER_LENGTH ;i++)
        {
            *((unsigned short*) out_buffer_ptr) = d->circular_buf_h.buffer[REF_MIC0 * 2 + d->circular_buf_h.pingpong][i];
            out_buffer_ptr++;
        }

        d->circular_buf_h.full = false;
        d->circular_buf_h.pingpong ^= 1;

        return (BUFFER_LENGTH << 1);
    }
    else if (d->channels == 2)
    {
        if (d->interleave == 1)
        {
            for (i = 0; i < BUFFER_LENGTH; i++)
            {
                *((unsigned short*) out_buffer_ptr) = d->circular_buf_h.buffer[REF_MIC0 * 2 + d->circular_buf_h.pingpong][i];
                out_buffer_ptr++;
                *((unsigned short*) out_buffer_ptr) = d->circular_buf_h.buffer[REF_MIC1 * 2 + d->circular_buf_h.pingpong][i];
                out_buffer_ptr++;
            }
        }
        else
        {
            for (i = 0; i < BUFFER_LENGTH; i++)
            {
                *((unsigned short*) out_buffer_ptr) = d->circular_buf_h.buffer[REF_MIC0 * 2 + d->circular_buf_h.pingpong][i];
                out_buffer_ptr++;
            }
            for (i = 0; i < BUFFER_LENGTH; i++)
            {
                *((unsigned short*) out_buffer_ptr) = d->circular_buf_h.buffer[REF_MIC1 * 2 + d->circular_buf_h.pingpong][i];
                out_buffer_ptr++;
            }
        }

        d->circular_buf_h.full = false;
        d->circular_buf_h.pingpong ^= 1;

        return (BUFFER_LENGTH << 2);
    }
    else if (d->channels == 3)
    {
        if (d->interleave == 1)
        {
            for (i = 0; i < BUFFER_LENGTH; i++)
            {
                *((unsigned short*) out_buffer_ptr) = d->circular_buf_h.buffer[REF_MIC0 * 2 + d->circular_buf_h.pingpong][i];
                out_buffer_ptr++;
                *((unsigned short*) out_buffer_ptr) = d->circular_buf_h.buffer[REF_MIC1 * 2 + d->circular_buf_h.pingpong][i];
                out_buffer_ptr++;
                *((unsigned short*) out_buffer_ptr) = d->circular_buf_h.buffer[REF_MIC2 * 2 + d->circular_buf_h.pingpong][i];
                out_buffer_ptr++;
            }
        }
        else
        {
            for (i = 0; i < BUFFER_LENGTH; i++)
            {
                *((unsigned short*) out_buffer_ptr) = d->circular_buf_h.buffer[REF_MIC0 * 2 + d->circular_buf_h.pingpong][i];
                out_buffer_ptr++;
            }
            for (i = 0; i < BUFFER_LENGTH; i++)
            {
                *((unsigned short*) out_buffer_ptr) = d->circular_buf_h.buffer[REF_MIC1 * 2 + d->circular_buf_h.pingpong][i];
                out_buffer_ptr++;
            }
            for (i = 0; i < BUFFER_LENGTH; i++)
            {
                *((unsigned short*) out_buffer_ptr) = d->circular_buf_h.buffer[REF_MIC2 * 2 + d->circular_buf_h.pingpong][i];
                out_buffer_ptr++;
            }
        }

        d->circular_buf_h.full = false;
        d->circular_buf_h.pingpong ^= 1;

        return (BUFFER_LENGTH * d->channels * 2);
    }
    else if (d->channels == 8)
    {
        if (d->interleave == 1)
        {
            for (i = 0; i < BUFFER_LENGTH; i++)
            {
                *((unsigned short*) out_buffer_ptr) = d->circular_buf_h.buffer[REF_MIC0 * 2 + d->circular_buf_h.pingpong][i];
                out_buffer_ptr++;
                *((unsigned short*) out_buffer_ptr) = d->circular_buf_h.buffer[REF_MIC1 * 2 + d->circular_buf_h.pingpong][i];
                out_buffer_ptr++;
                *((unsigned short*) out_buffer_ptr) = d->circular_buf_h.buffer[REF_MIC2 * 2 + d->circular_buf_h.pingpong][i];
                out_buffer_ptr++;
                *((unsigned short*) out_buffer_ptr) = d->circular_buf_h.buffer[REF_MIC3 * 2 + d->circular_buf_h.pingpong][i];
                out_buffer_ptr++;
                *((unsigned short*) out_buffer_ptr) = d->circular_buf_h.buffer[REF_MIC4 * 2 + d->circular_buf_h.pingpong][i];
                out_buffer_ptr++;
                *((unsigned short*) out_buffer_ptr) = d->circular_buf_h.buffer[REF_MIC5 * 2 + d->circular_buf_h.pingpong][i];
                out_buffer_ptr++;
                *((unsigned short*) out_buffer_ptr) = d->circular_buf_h.buffer[REF_MIC6 * 2 + d->circular_buf_h.pingpong][i];
                out_buffer_ptr++;
                *((unsigned short*) out_buffer_ptr) = d->circular_buf_h.buffer[REF_MIC7 * 2 + d->circular_buf_h.pingpong][i];
                out_buffer_ptr++;
            }
        }
        else
        {
            for (i = 0; i < BUFFER_LENGTH; i++)
            {
                *((unsigned short*) out_buffer_ptr) = d->circular_buf_h.buffer[REF_MIC0 * 2 + d->circular_buf_h.pingpong][i];
                out_buffer_ptr++;
            }
            for (i = 0; i < BUFFER_LENGTH; i++)
            {
                *((unsigned short*) out_buffer_ptr) = d->circular_buf_h.buffer[REF_MIC1 * 2 + d->circular_buf_h.pingpong][i];
                out_buffer_ptr++;
            }
            for (i = 0; i < BUFFER_LENGTH; i++)
            {
                *((unsigned short*) out_buffer_ptr) = d->circular_buf_h.buffer[REF_MIC2 * 2 + d->circular_buf_h.pingpong][i];
                out_buffer_ptr++;
            }
            for (i = 0; i < BUFFER_LENGTH; i++)
            {
                *((unsigned short*) out_buffer_ptr) = d->circular_buf_h.buffer[REF_MIC3 * 2 + d->circular_buf_h.pingpong][i];
                out_buffer_ptr++;
            }
            for (i = 0; i < BUFFER_LENGTH; i++)
            {
                *((unsigned short*) out_buffer_ptr) = d->circular_buf_h.buffer[REF_MIC4 * 2 + d->circular_buf_h.pingpong][i];
                out_buffer_ptr++;
            }
            for (i = 0; i < BUFFER_LENGTH; i++)
            {
                *((unsigned short*) out_buffer_ptr) = d->circular_buf_h.buffer[REF_MIC5 * 2 + d->circular_buf_h.pingpong][i];
                out_buffer_ptr++;
            }
            for (i = 0; i < BUFFER_LENGTH; i++)
            {
                *((unsigned short*) out_buffer_ptr) = d->circular_buf_h.buffer[REF_MIC6 * 2 + d->circular_buf_h.pingpong][i];
                out_buffer_ptr++;
            }
            for (i = 0; i < BUFFER_LENGTH; i++)
            {
                *((unsigned short*) out_buffer_ptr) = d->circular_buf_h.buffer[REF_MIC7 * 2 + d->circular_buf_h.pingpong][i];
                out_buffer_ptr++;
            }
        }

        d->circular_buf_h.full = false;
        d->circular_buf_h.pingpong ^= 1;

        return (BUFFER_LENGTH * d->channels * 2);
    }

    /* Should not get here - error in number of DMIC channels. */
    return 0;
}

/* ...set memory pointer */
static XA_ERRORCODE xa_capturer_set_mem_ptr(XACapturer *d, WORD32 i_idx, pVOID pv_value)
{
    WORD32 bytes_read = 0;
    static UWORD32 frame_cnt = 0;
    /* ...basic sanity check */
    XF_CHK_ERR(d && pv_value, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...codec must be initialized */
    XF_CHK_ERR(d->state & XA_CAPTURER_FLAG_POSTINIT_DONE, XA_API_FATAL_INVALID_CMD_TYPE);

    TRACE(INIT, _b("xa_capturer_set_mem_ptr[%u]: %p"), i_idx, pv_value);

    /* ...select memory buffer */
    switch (i_idx)
    {
    case 0:
        /* ...input buffer */
        d->output = pv_value;

        if ((d->newDataAvailable & (0xFF << 0)) == (0xFF << 0))
        {
            bytes_read = xa_hw_capturer_read_FIFO(d);

            d->produced = bytes_read;
            d->tot_bytes_produced = d->tot_bytes_produced + d->produced;

            d->newDataAvailable &= ~(0xFF << 0);
        }

        return XA_NO_ERROR;

    default:
        /* ...invalid index */
        return XF_CHK_ERR(0, XA_API_FATAL_INVALID_CMD_TYPE);
    }
}

/*******************************************************************************
 * API command hooks
 ******************************************************************************/
static XA_ERRORCODE (* const xa_capturer_api[])(XACapturer *, WORD32, pVOID) =
{
    [XA_API_CMD_GET_API_SIZE]           = xa_capturer_get_api_size,
    [XA_API_CMD_INIT]                   = xa_capturer_init,
    [XA_API_CMD_SET_CONFIG_PARAM]       = xa_capturer_set_config_param,
    [XA_API_CMD_GET_CONFIG_PARAM]       = xa_capturer_get_config_param,
    [XA_API_CMD_EXECUTE]                = xa_capturer_execute,
    [XA_API_CMD_GET_OUTPUT_BYTES]       = xa_capturer_get_output_bytes,
    [XA_API_CMD_GET_MEMTABS_SIZE]       = xa_capturer_get_memtabs_size,
    [XA_API_CMD_SET_MEMTABS_PTR]        = xa_capturer_set_memtabs_ptr,
    [XA_API_CMD_GET_N_MEMTABS]          = xa_capturer_get_n_memtabs,
    [XA_API_CMD_GET_MEM_INFO_SIZE]      = xa_capturer_get_mem_info_size,
    [XA_API_CMD_GET_MEM_INFO_ALIGNMENT] = xa_capturer_get_mem_info_alignment,
    [XA_API_CMD_GET_MEM_INFO_TYPE]      = xa_capturer_get_mem_info_type,
    [XA_API_CMD_SET_MEM_PTR]            = xa_capturer_set_mem_ptr,

};

/* ...total numer of commands supported */
#define XA_CAPTURER_API_COMMANDS_NUM   (sizeof(xa_capturer_api) / sizeof(xa_capturer_api[0]))

/*******************************************************************************
 * API entry point
 ******************************************************************************/

XA_ERRORCODE xa_capturer(xa_codec_handle_t p_xa_module_obj, WORD32 i_cmd, WORD32 i_idx, pVOID pv_value)
{
    XACapturer *capturer = (XACapturer *) p_xa_module_obj;

    /* ...check if command index is sane */
    XF_CHK_ERR(i_cmd < XA_CAPTURER_API_COMMANDS_NUM, XA_API_FATAL_INVALID_CMD);

    /* ...see if command is defined */
    XF_CHK_ERR(xa_capturer_api[i_cmd], XA_API_FATAL_INVALID_CMD);

    /* ...execute requested command */
    return xa_capturer_api[i_cmd](capturer, i_idx, pv_value);
}
