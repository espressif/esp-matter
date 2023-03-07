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
 * xa-pcm-gain.c
 *
 * Sample pcm gain plugin
 ******************************************************************************/

#define MODULE_TAG                      PCM_GAIN

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdint.h>
#include <string.h>

/* ...debugging facility */
#include "xf-debug.h"
#include "audio/xa_pcm_gain_api.h"

#ifdef XAF_PROFILE
#include "xaf_clk_test.h"
extern clk_t pcm_gain_cycles;
#endif

/*******************************************************************************
 * Tracing configuration
 ******************************************************************************/

TRACE_TAG(INIT, 1);
TRACE_TAG(PROCESS, 1);

/*******************************************************************************
 * Internal functions definitions
 ******************************************************************************/

/* ...API structure */
typedef struct XAPcmGain
{
    /* ...pcm gain module state */
    UWORD32                 state;

    /* ...number of channels (channel mask?) */
    UWORD32                 channels;

    /* ...PCM sample width */
    UWORD32                 pcm_width;

    /* ...sampling rate */
    UWORD32                 sample_rate;

    /* ...number of bytes in input/output buffer */
    UWORD32                 buffer_size;

    /* ...input buffer */
    void               *input;

    /* ...output buffer */
    void               *output;

    /* ...scratch buffer pointer */
    void               *scratch;

    /* ...number of available bytes in the input buffer */
    UWORD32                 input_avail;

    /* ...number of bytes consumed from input buffer */
    UWORD32                 consumed;

    /* ...number of produced bytes */
    UWORD32                 produced;

    /* ...gain index */
    UWORD32                 gain_idx;

}   XAPcmGain;


/****************************************************************************
 * Gain multiplier table
 ******************************************************************************/
/*

0db = 1

-6db = 0.50118723362727228500155418688495
-12db = 0.25118864315095801110850320677993
-18db = 0.12589254117941672104239541063958

6db = 1.9952623149688796013524553967395
12db = 3.9810717055349725077025230508775
18db = 7.9432823472428150206591828283639
*/

WORD16 pcm_gains_dB[7] = {   0,   -6,  -12, -18,    6,    12,    18};    // in dB
WORD16 pcm_gains[7]    = {4096, 2053, 1029, 516, 8173, 16306, 32536};    // Q12 format

#define MAX_16BIT (32767)
#define MIN_16BIT (-32768)
#define MAX_32BIT (2147483647)
#define MIN_32BIT (-2147483648)

/*******************************************************************************
 * Pcm gain state flags
 ******************************************************************************/

#define XA_PCM_GAIN_FLAG_PREINIT_DONE      (1 << 0)
#define XA_PCM_GAIN_FLAG_POSTINIT_DONE     (1 << 1)
#define XA_PCM_GAIN_FLAG_RUNNING           (1 << 2)
#define XA_PCM_GAIN_FLAG_OUTPUT            (1 << 3)
#define XA_PCM_GAIN_FLAG_COMPLETE          (1 << 4)

/*******************************************************************************
 * DSP functions
 ******************************************************************************/

/* ...pcm gain component pre-initialization (default parameters) */
static inline void xa_pcm_gain_preinit(XAPcmGain *d)
{

    /* ...pre-configuration initialization; reset internal data */
    memset(d, 0, sizeof(*d));

    /* ...set default parameters */
    d->channels = 1;
    d->pcm_width = 16;
    d->sample_rate = 48000;
}

/* ...apply gain to 16-bit PCM stream */
static XA_ERRORCODE xa_pcm_gain_do_execute_16bit(XAPcmGain *d)
{
    WORD32     i, k, nSize;
    WORD16    *pIn = (WORD16 *) d->input;
    WORD16    *pOut = (WORD16 *) d->output;
    UWORD32     filled = d->input_avail;
    WORD16     input;
    WORD16     gain = pcm_gains[d->gain_idx];
    WORD32     product;



    nSize = filled >> 1;    //size of each sample is 2 bytes
    k = (WORD32)(d->buffer_size - filled);

    /* ...check I/O buffer */
    XF_CHK_ERR(d->input, XA_PCM_GAIN_EXEC_FATAL_INPUT);
    XF_CHK_ERR(d->output, XA_PCM_GAIN_EXEC_FATAL_INPUT);

    (k > 0 ? memset((void *)pIn + filled, 0x00, k) : 0);

    /* ...Processing loop */
    for (i = 0; i < nSize; i++)
    {
        input = *pIn++;
        product = (WORD32)input*gain;
        product = product >> 12;

        if(product > MAX_16BIT)
            product = MAX_16BIT;
        else if(product < MIN_16BIT)
            product = MIN_16BIT;

        *pOut++ = (WORD16)product;
    }

    /* ...save total number of consumed bytes */
    d->consumed = (UWORD32)((void *)pIn - d->input);

    /* ...save total number of produced bytes */
    d->produced = (UWORD32)((void *)pOut - d->output);

    /* ...put flag saying we have output buffer */
    d->state |= XA_PCM_GAIN_FLAG_OUTPUT;

    TRACE(PROCESS, _b("produced: %u bytes (%u samples)"), d->produced, nSize);

    /* ...return success result code */
    return XA_NO_ERROR;
}

/* ...apply gain to 32-bit PCM stream */
static XA_ERRORCODE xa_pcm_gain_do_execute_32bit(XAPcmGain *d)
{
    WORD32     i, k, nSize;
    WORD32    *pIn = (WORD32 *) d->input;
    WORD32    *pOut = (WORD32 *) d->output;
    UWORD32     filled = d->input_avail;
    WORD32     input;
    WORD16     gain = pcm_gains[d->gain_idx];
    WORD64     product;

    nSize = filled >> 2;    //size of each sample is 4 bytes
    k = (WORD32)(d->buffer_size - filled);

    /* ...check I/O buffer */
    XF_CHK_ERR(d->input, XA_PCM_GAIN_EXEC_FATAL_INPUT);
    XF_CHK_ERR(d->output, XA_PCM_GAIN_EXEC_FATAL_INPUT);

    /* ...Processing loop */
    for (i = 0; i < nSize; i++)
    {
        input = *pIn++;
        product = input;
        product <<=8;

        //Gain factor has been 12 bit left shift (Q12)
        product = product*gain;
        // Result should right shift 12 bits first, and then left shift 8 bits to keep 24 high bits significant
        product = product >> 12;

        *pOut++ = (WORD32)product;
    }

    /* ...save total number of consumed bytes */
    d->consumed = (UWORD32)((void *)pIn - d->input);

    /* ...save total number of produced bytes */
    d->produced = (UWORD32)((void *)pOut - d->output);

    /* ...put flag saying we have output buffer */
    d->state |= XA_PCM_GAIN_FLAG_OUTPUT;

    TRACE(PROCESS, _b("produced: %u bytes (%u samples)"), d->produced, nSize);

    /* ...return success result code */
    return XA_NO_ERROR;
}

/* ...runtime reset */
static XA_ERRORCODE xa_pcm_gain_do_runtime_init(XAPcmGain *d)
{
    /* ...no special processing is needed here */
    return XA_NO_ERROR;
}

/*******************************************************************************
 * Commands processing
 ******************************************************************************/

/* ...codec API size query */
static XA_ERRORCODE xa_pcm_gain_get_api_size(XAPcmGain *d, WORD32 i_idx, pVOID pv_value)
{
    /* ...check parameters are sane */
    XF_CHK_ERR(pv_value, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...retrieve API structure size */
    *(WORD32 *)pv_value = sizeof(*d);

    return XA_NO_ERROR;
}

/* ...standard codec initialization routine */
static XA_ERRORCODE xa_pcm_gain_init(XAPcmGain *d, WORD32 i_idx, pVOID pv_value)
{
    /* ...sanity check - pcm gain component must be valid */
    XF_CHK_ERR(d, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...process particular initialization type */
    switch (i_idx)
    {
    case XA_CMD_TYPE_INIT_API_PRE_CONFIG_PARAMS:
    {
        /* ...pre-configuration initialization; reset internal data */
        xa_pcm_gain_preinit(d);

        /* ...and mark pcm gain component has been created */
        d->state = XA_PCM_GAIN_FLAG_PREINIT_DONE;

        return XA_NO_ERROR;
    }

    case XA_CMD_TYPE_INIT_API_POST_CONFIG_PARAMS:
    {
        /* ...post-configuration initialization (all parameters are set) */
        XF_CHK_ERR(d->state & XA_PCM_GAIN_FLAG_PREINIT_DONE, XA_API_FATAL_INVALID_CMD_TYPE);

        /* ...mark post-initialization is complete */
        d->state |= XA_PCM_GAIN_FLAG_POSTINIT_DONE;

        return XA_NO_ERROR;
    }

    case XA_CMD_TYPE_INIT_PROCESS:
    {
        /* ...kick run-time initialization process; make sure pcm gain component is setup */
        XF_CHK_ERR(d->state & XA_PCM_GAIN_FLAG_POSTINIT_DONE, XA_API_FATAL_INVALID_CMD_TYPE);

        /* ...enter into execution stage */
        d->state |= XA_PCM_GAIN_FLAG_RUNNING;

        return XA_NO_ERROR;
    }

    case XA_CMD_TYPE_INIT_DONE_QUERY:
    {
        /* ...check if initialization is done; make sure pointer is sane */
        XF_CHK_ERR(pv_value, XA_API_FATAL_INVALID_CMD_TYPE);

        /* ...put current status */
        *(WORD32 *)pv_value = (d->state & XA_PCM_GAIN_FLAG_RUNNING ? 1 : 0);

        return XA_NO_ERROR;
    }

    default:
        /* ...unrecognised command type */
        TRACE(ERROR, _x("Unrecognised command type: %X"), i_idx);
        return XA_API_FATAL_INVALID_CMD_TYPE;
    }
}

/* ...set pcm gain component configuration parameter */
static XA_ERRORCODE xa_pcm_gain_set_config_param(XAPcmGain *d, WORD32 i_idx, pVOID pv_value)
{
    UWORD32     i_value;

    /* ...sanity check - pcm gain component pointer must be sane */
    XF_CHK_ERR(d && pv_value, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...pre-initialization must be completed, pcm gain component must be idle */
    XF_CHK_ERR(d->state & XA_PCM_GAIN_FLAG_PREINIT_DONE, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...get parameter value  */
    i_value = (UWORD32) *(WORD32 *)pv_value;

    /* ...process individual configuration parameter */
    switch (i_idx & 0xF)
    {
    case XA_PCM_GAIN_CONFIG_PARAM_PCM_WIDTH:
        /* ...check value is permitted (16 bits or 32 bits) */
        XF_CHK_ERR((i_value == 16) || (i_value == 32), XA_PCM_GAIN_CONFIG_NONFATAL_RANGE);
        d->pcm_width = (UWORD32)i_value;
        return XA_NO_ERROR;

    case XA_PCM_GAIN_CONFIG_PARAM_CHANNELS:
        /* ...allow mono/stereo only */
        XF_CHK_ERR(((i_value == 1)||(i_value == 2)), XA_PCM_GAIN_CONFIG_NONFATAL_RANGE);
        d->channels = (UWORD32)i_value;
        return XA_NO_ERROR;

    case XA_PCM_GAIN_CONFIG_PARAM_SAMPLE_RATE:
        {
            /* ...set pcm gain component sample rate */
            switch ((UWORD32)i_value)
            {
                case 4000:
                case 8000:
                case 11025:
                case 12000:
                case 16000:
                case 22050:
                case 24000:
                case 32000:
                case 44100:
                case 48000:
                case 64000:
                case 88200:
                case 96000:
                case 128000:
                case 176400:
                case 192000:
                    d->sample_rate = (UWORD32)i_value;
                    break;
                default:
                    XF_CHK_ERR(0, XA_PCM_GAIN_CONFIG_NONFATAL_RANGE);
            }
            return XA_NO_ERROR;
        }

    case XA_PCM_GAIN_CONFIG_PARAM_GAIN_FACTOR:
        /* ...set pcm gain component gain index */
        XF_CHK_ERR(((i_value >= 0)&&(i_value <= 6)), XA_PCM_GAIN_CONFIG_NONFATAL_RANGE);
        d->gain_idx = (UWORD32)i_value;
        return XA_NO_ERROR;

    case XA_PCM_GAIN_CONFIG_PARAM_FRAME_SIZE:
        /* ...set pcm gain component buffer size */
        d->buffer_size = (UWORD32)i_value;
        return XA_NO_ERROR;

    default:
        TRACE(ERROR, _x("Invalid parameter: %X"), i_idx);
        return XA_API_FATAL_INVALID_CMD_TYPE;
    }
}

/* ...retrieve configuration parameter */
static XA_ERRORCODE xa_pcm_gain_get_config_param(XAPcmGain *d, WORD32 i_idx, pVOID pv_value)
{
    /* ...sanity check - pcm gain component must be initialized */
    XF_CHK_ERR(d && pv_value, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...make sure pre-initialization is completed */
    XF_CHK_ERR(d->state & XA_PCM_GAIN_FLAG_PREINIT_DONE, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...process individual configuration parameter */
    switch (i_idx & 0xF)
    {
    case XA_PCM_GAIN_CONFIG_PARAM_SAMPLE_RATE:
        /* ...return sample rate */
        *(WORD32 *)pv_value = d->sample_rate;
        return XA_NO_ERROR;

    case XA_PCM_GAIN_CONFIG_PARAM_PCM_WIDTH:
        /* ...return current PCM width */
        *(WORD32 *)pv_value = d->pcm_width;
        return XA_NO_ERROR;

    case XA_PCM_GAIN_CONFIG_PARAM_CHANNELS:
        /* ...return current channel number */
        *(WORD32 *)pv_value = d->channels;
        return XA_NO_ERROR;

    case XA_PCM_GAIN_CONFIG_PARAM_PRODUCED:
        /* ...return no.of produced bytes */
        *(WORD32 *)pv_value = d->produced;
        return XA_NO_ERROR;

    case XA_PCM_GAIN_CONFIG_PARAM_FRAME_SIZE:
        /* ...return pcm gain component buffer size */
        *(WORD32 *)pv_value = d->buffer_size;
        return XA_NO_ERROR;

    case XA_PCM_GAIN_CONFIG_PARAM_GAIN_FACTOR:
        /* ...return no.of produced bytes */
        *(WORD32 *)pv_value = pcm_gains_dB[d->gain_idx];
        return XA_NO_ERROR;

    default:
        TRACE(ERROR, _x("Invalid parameter: %X"), i_idx);
        return XA_API_FATAL_INVALID_CMD_TYPE;
    }
}

/* ...execution command */
static XA_ERRORCODE xa_pcm_gain_execute(XAPcmGain *d, WORD32 i_idx, pVOID pv_value)
{
    XA_ERRORCODE ret;
#ifdef XAF_PROFILE
    clk_t comp_start, comp_stop;
#endif

    /* ...sanity check - pcm gain component must be valid */
    XF_CHK_ERR(d, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...pcm gain component must be in running state */
    XF_CHK_ERR(d->state & XA_PCM_GAIN_FLAG_RUNNING, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...process individual command type */
    switch (i_idx)
    {
    case XA_CMD_TYPE_DO_EXECUTE:
        /* ...perform mixing of the channels */
#ifdef XAF_PROFILE
        comp_start = clk_read_start(CLK_SELN_THREAD);
#endif
        if (d->pcm_width == 16)
        	ret = xa_pcm_gain_do_execute_16bit(d);
        else if (d->pcm_width == 32)
			ret = xa_pcm_gain_do_execute_32bit(d);
#ifdef XAF_PROFILE
        comp_stop = clk_read_stop(CLK_SELN_THREAD);
        pcm_gain_cycles += clk_diff(comp_stop, comp_start);
#endif
        return ret;

    case XA_CMD_TYPE_DONE_QUERY:
        /* ...check if processing is complete */
        XF_CHK_ERR(pv_value, XA_API_FATAL_INVALID_CMD_TYPE);
        *(WORD32 *)pv_value = (d->state & XA_PCM_GAIN_FLAG_COMPLETE ? 1 : 0);
        return XA_NO_ERROR;

    case XA_CMD_TYPE_DO_RUNTIME_INIT:
        /* ...reset pcm gain component operation */
        return xa_pcm_gain_do_runtime_init(d);

    default:
        /* ...unrecognised command */
        TRACE(ERROR, _x("Invalid index: %X"), i_idx);
        return XA_API_FATAL_INVALID_CMD_TYPE;
    }
}

/* ...set number of input bytes */
static XA_ERRORCODE xa_pcm_gain_set_input_bytes(XAPcmGain *d, WORD32 i_idx, pVOID pv_value)
{
    UWORD32     size;

    /* ...sanity check - check parameters */
    XF_CHK_ERR(d && pv_value, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...track index must be valid */
    XF_CHK_ERR(i_idx == 0, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...pcm gain component must be initialized */
    XF_CHK_ERR(d->state & XA_PCM_GAIN_FLAG_POSTINIT_DONE, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...input buffer must exist */
    XF_CHK_ERR(d->input, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...input frame length should not be zero (in bytes) */
    XF_CHK_ERR((size = (UWORD32)*(WORD32 *)pv_value) >= 0, XA_PCM_GAIN_EXEC_NONFATAL_INPUT);

    /* ...all is correct; set input buffer length in bytes */
    d->input_avail = size;

    return XA_NO_ERROR;
}

/* ...get number of output bytes */
static XA_ERRORCODE xa_pcm_gain_get_output_bytes(XAPcmGain *d, WORD32 i_idx, pVOID pv_value)
{
    /* ...sanity check - check parameters */
    XF_CHK_ERR(d && pv_value, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...track index must be zero */
    XF_CHK_ERR(i_idx == 1, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...pcm gain component must be running */
    XF_CHK_ERR(d->state & XA_PCM_GAIN_FLAG_RUNNING, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...output buffer must exist */
    XF_CHK_ERR(d->output, XA_PCM_GAIN_EXEC_NONFATAL_OUTPUT);

    /* ...return number of produced bytes */
    *(WORD32 *)pv_value = ((d->state & XA_PCM_GAIN_FLAG_OUTPUT) ? d->produced : 0);

    return XA_NO_ERROR;
}

/* ...get number of consumed bytes */
static XA_ERRORCODE xa_pcm_gain_get_curidx_input_buf(XAPcmGain *d, WORD32 i_idx, pVOID pv_value)
{
    /* ...sanity check - check parameters */
    XF_CHK_ERR(d && pv_value, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...track index must be valid */
    XF_CHK_ERR(i_idx == 0, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...pcm gain component must be running */
    XF_CHK_ERR(d->state & XA_PCM_GAIN_FLAG_RUNNING, XA_PCM_GAIN_EXEC_FATAL_STATE);

    /* ...input buffer must exist */
    XF_CHK_ERR(d->input, XA_PCM_GAIN_EXEC_FATAL_INPUT);

    /* ...return number of bytes consumed (always consume fixed-length chunk) */
    *(WORD32 *)pv_value = d->consumed;

    /* ...and reset internally managed number of consumed bytes */
    d->consumed = 0;

    return XA_NO_ERROR;
}

/* ...end-of-stream processing */
static XA_ERRORCODE xa_pcm_gain_input_over(XAPcmGain *d, WORD32 i_idx, pVOID pv_value)
{
    /* ...sanity check */
    XF_CHK_ERR(d, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...put end-of-stream flag */
    d->state |= XA_PCM_GAIN_FLAG_COMPLETE;

    TRACE(PROCESS, _b("Input-over-condition signalled"));

    return XA_NO_ERROR;
}

/*******************************************************************************
 * Memory information API
 ******************************************************************************/
/* ..get total amount of data for memory tables */
static XA_ERRORCODE xa_pcm_gain_get_memtabs_size(XAPcmGain *d, WORD32 i_idx, pVOID pv_value)
{
    /* ...basic sanity checks */
    XF_CHK_ERR(d && pv_value, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...check pcm gain component is pre-initialized */
    XF_CHK_ERR(d->state & XA_PCM_GAIN_FLAG_PREINIT_DONE, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...we have all our tables inside API structure - good? tbd */
    *(WORD32 *)pv_value = 0;

    return XA_NO_ERROR;
}

/* ..set memory tables pointer */
static XA_ERRORCODE xa_pcm_gain_set_memtabs_ptr(XAPcmGain *d, WORD32 i_idx, pVOID pv_value)
{
    /* ...basic sanity checks */
    XF_CHK_ERR(d && pv_value, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...check pcm gain component is pre-initialized */
    XF_CHK_ERR(d->state & XA_PCM_GAIN_FLAG_PREINIT_DONE, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...do not do anything; just return success - tbd */
    return XA_NO_ERROR;
}

/* ...return total amount of memory buffers */
static XA_ERRORCODE xa_pcm_gain_get_n_memtabs(XAPcmGain *d, WORD32 i_idx, pVOID pv_value)
{
    /* ...basic sanity checks */
    XF_CHK_ERR(d && pv_value, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...we have 1 input buffer, 1 output buffer and 1 scratch buffer*/
    *(WORD32 *)pv_value = 3;

    return XA_NO_ERROR;
}

/* ...return memory buffer data */
static XA_ERRORCODE xa_pcm_gain_get_mem_info_size(XAPcmGain *d, WORD32 i_idx, pVOID pv_value)
{
    /* ...basic sanity check */
    XF_CHK_ERR(d && pv_value, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...return frame buffer minimal size only after post-initialization is done */
    XF_CHK_ERR(d->state & XA_PCM_GAIN_FLAG_POSTINIT_DONE, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...all buffers are of the same length */
    *(WORD32 *)pv_value = (WORD32) d->buffer_size;

    return XA_NO_ERROR;
}

/* ...return memory alignment data */
static XA_ERRORCODE xa_pcm_gain_get_mem_info_alignment(XAPcmGain *d, WORD32 i_idx, pVOID pv_value)
{
    /* ...basic sanity check */
    XF_CHK_ERR(d && pv_value, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...return frame buffer minimal size only after post-initialization is done */
    XF_CHK_ERR(d->state & XA_PCM_GAIN_FLAG_POSTINIT_DONE, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...all buffers are 4-bytes aligned */
    *(WORD32 *)pv_value = 4;

    return XA_NO_ERROR;
}

/* ...return memory type data */
static XA_ERRORCODE xa_pcm_gain_get_mem_info_type(XAPcmGain *d, WORD32 i_idx, pVOID pv_value)
{
    /* ...basic sanity check */
    XF_CHK_ERR(d && pv_value, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...return frame buffer minimal size only after post-initialization is done */
    XF_CHK_ERR(d->state & XA_PCM_GAIN_FLAG_POSTINIT_DONE, XA_API_FATAL_INVALID_CMD_TYPE);

    switch (i_idx)
    {
    case 0:
        /* ...input buffer */
        *(WORD32 *)pv_value = XA_MEMTYPE_INPUT;
        return XA_NO_ERROR;

    case 1:
        /* ...output buffer */
        *(WORD32 *)pv_value = XA_MEMTYPE_OUTPUT;
        return XA_NO_ERROR;

    case 2:
        /* ...scratch buffer */
        *(WORD32 *)pv_value = XA_MEMTYPE_SCRATCH;
        return XA_NO_ERROR;

    default:
        /* ...invalid index */
        return XF_CHK_ERR(0, XA_API_FATAL_INVALID_CMD_TYPE);
    }
}

/* ...set memory pointer */
static XA_ERRORCODE xa_pcm_gain_set_mem_ptr(XAPcmGain *d, WORD32 i_idx, pVOID pv_value)
{
    /* ...basic sanity check */
    XF_CHK_ERR(d && pv_value, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...codec must be initialized */
    XF_CHK_ERR(d->state & XA_PCM_GAIN_FLAG_POSTINIT_DONE, XA_API_FATAL_INVALID_CMD_TYPE);

    /* ...select memory buffer */
    switch (i_idx)
    {
    case 0:
        /* ...input buffers */
        d->input = pv_value;
        return XA_NO_ERROR;

    case 1:
        /* ...output buffer */
        d->output = pv_value;
        return XA_NO_ERROR;

    case 2:
        /* ...scratch buffer */
        d->scratch = pv_value;
        return XA_NO_ERROR;

    default:
        /* ...invalid index */
        return XF_CHK_ERR(0, XA_API_FATAL_INVALID_CMD_TYPE);
    }
}


/*******************************************************************************
 * API command hooks
 ******************************************************************************/

static XA_ERRORCODE (* const xa_pcm_gain_api[])(XAPcmGain *, WORD32, pVOID) =
{
    [XA_API_CMD_GET_API_SIZE]           = xa_pcm_gain_get_api_size,

    [XA_API_CMD_INIT]                   = xa_pcm_gain_init,
    [XA_API_CMD_SET_CONFIG_PARAM]       = xa_pcm_gain_set_config_param,
    [XA_API_CMD_GET_CONFIG_PARAM]       = xa_pcm_gain_get_config_param,

    [XA_API_CMD_EXECUTE]                = xa_pcm_gain_execute,
    [XA_API_CMD_SET_INPUT_BYTES]        = xa_pcm_gain_set_input_bytes,
    [XA_API_CMD_GET_OUTPUT_BYTES]       = xa_pcm_gain_get_output_bytes,
    [XA_API_CMD_GET_CURIDX_INPUT_BUF]   = xa_pcm_gain_get_curidx_input_buf,
    [XA_API_CMD_INPUT_OVER]             = xa_pcm_gain_input_over,

    [XA_API_CMD_GET_MEMTABS_SIZE]       = xa_pcm_gain_get_memtabs_size,
    [XA_API_CMD_SET_MEMTABS_PTR]        = xa_pcm_gain_set_memtabs_ptr,
    [XA_API_CMD_GET_N_MEMTABS]          = xa_pcm_gain_get_n_memtabs,
    [XA_API_CMD_GET_MEM_INFO_SIZE]      = xa_pcm_gain_get_mem_info_size,
    [XA_API_CMD_GET_MEM_INFO_ALIGNMENT] = xa_pcm_gain_get_mem_info_alignment,
    [XA_API_CMD_GET_MEM_INFO_TYPE]      = xa_pcm_gain_get_mem_info_type,
    [XA_API_CMD_SET_MEM_PTR]            = xa_pcm_gain_set_mem_ptr,
};

/* ...total number of commands supported */
#define XA_PCM_GAIN_API_COMMANDS_NUM   (sizeof(xa_pcm_gain_api) / sizeof(xa_pcm_gain_api[0]))

/*******************************************************************************
 * API entry point
 ******************************************************************************/

XA_ERRORCODE xa_pcm_gain(xa_codec_handle_t p_xa_module_obj, WORD32 i_cmd, WORD32 i_idx, pVOID pv_value)
{
    XAPcmGain *d = (XAPcmGain *) p_xa_module_obj;

    /* ...check if command index is sane */
    XF_CHK_ERR(i_cmd < XA_PCM_GAIN_API_COMMANDS_NUM, XA_API_FATAL_INVALID_CMD);

    /* ...see if command is defined */
    XF_CHK_ERR(xa_pcm_gain_api[i_cmd], XA_API_FATAL_INVALID_CMD);

    /* ...execute requested command */
    return xa_pcm_gain_api[i_cmd](d, i_idx, pv_value);
}
