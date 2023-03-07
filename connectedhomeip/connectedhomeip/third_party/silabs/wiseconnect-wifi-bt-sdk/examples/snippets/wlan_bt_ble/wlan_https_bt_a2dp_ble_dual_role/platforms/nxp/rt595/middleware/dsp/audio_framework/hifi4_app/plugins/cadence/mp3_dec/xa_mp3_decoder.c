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
 * xa-mp3-decoder.c
 *
 * MP3 decoder plugin - thin wrapper around MP3DEC library
 ******************************************************************************/

#define MODULE_TAG                      MP3DEC

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "xa_mp3_dec_api.h"
#include "audio/xa-audio-decoder-api.h"


#ifdef XAF_PROFILE
#include "xaf_clk_test.h"
extern clk_t dec_cycles;
#endif

/*******************************************************************************
 * Override GET-CONFIG-PARAM function
 ******************************************************************************/

static inline XA_ERRORCODE xa_mp3_get_config_param(xa_codec_handle_t handle, WORD32 i_idx, pVOID pv_value)
{
    /* ...translate "standard" parameter index into internal value */
    switch (i_idx)
    {
    case XA_CODEC_CONFIG_PARAM_CHANNELS:
        /* ...return number of output channels */
        i_idx = XA_MP3DEC_CONFIG_PARAM_NUM_CHANNELS;
        break;

    case XA_CODEC_CONFIG_PARAM_SAMPLE_RATE:
        /* ...return output sampling frequency */
        i_idx = XA_MP3DEC_CONFIG_PARAM_SAMP_FREQ;
        break;

    case XA_CODEC_CONFIG_PARAM_PCM_WIDTH:
        /* ...return sample bit-width */
        i_idx = XA_MP3DEC_CONFIG_PARAM_PCM_WDSZ;
        break;
    }

    /* ...pass to library */
    return xa_mp3_dec(handle, XA_API_CMD_GET_CONFIG_PARAM, i_idx, pv_value);
}

/*******************************************************************************
 * API entry point
 ******************************************************************************/

XA_ERRORCODE xa_mp3_decoder(xa_codec_handle_t p_xa_module_obj, WORD32 i_cmd, WORD32 i_idx, pVOID pv_value)
{
    XA_ERRORCODE ret;
#ifdef XAF_PROFILE
    clk_t comp_start, comp_stop;
#endif

    /* ...process common audio-decoder commands */
    if (i_cmd == XA_API_CMD_GET_CONFIG_PARAM)
    {
        return xa_mp3_get_config_param(p_xa_module_obj, i_idx, pv_value);
    }
    else
    {
#ifdef XAF_PROFILE
        comp_start = clk_read_start(CLK_SELN_THREAD);
#endif
        ret = xa_mp3_dec(p_xa_module_obj, i_cmd, i_idx, pv_value);

#ifdef XAF_PROFILE
        comp_stop = clk_read_stop(CLK_SELN_THREAD);
        dec_cycles += clk_diff(comp_stop, comp_start);
#endif
        return ret;
    }
}

