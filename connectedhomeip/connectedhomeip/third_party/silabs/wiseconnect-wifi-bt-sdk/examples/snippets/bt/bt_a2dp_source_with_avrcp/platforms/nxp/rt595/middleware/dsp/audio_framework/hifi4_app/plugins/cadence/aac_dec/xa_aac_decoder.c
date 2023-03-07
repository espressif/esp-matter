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
 * xa-aac-decoder.c
 *
 * AAC decoder plugin - thin wrapper around AACDEC library
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

#define MODULE_TAG                      AACDEC

/*******************************************************************************
 * Includes
 ******************************************************************************/

//#include "xf-plugin.h"
#include "audio/xa-audio-decoder-api.h"
#include "xa_aac_dec_api.h"
#ifdef XAF_PROFILE
#include "xaf_clk_test.h"
extern clk_t aac_dec_cycles;
#endif
/*******************************************************************************
 * Override GET-CONFIG-PARAM function
 ******************************************************************************/

static inline XA_ERRORCODE xa_aac_get_config_param(xa_codec_handle_t handle, WORD32 i_idx, pVOID pv_value)
{
    /* ...translate "standard" parameter index into internal value */
    switch (i_idx)
    {
    case XA_CODEC_CONFIG_PARAM_CHANNELS:
        /* ...return number of output channels */
        i_idx = XA_AACDEC_CONFIG_PARAM_OUTNCHANS;
        break;

    case XA_CODEC_CONFIG_PARAM_SAMPLE_RATE:
        /* ...return output sampling frequency */
        i_idx = XA_AACDEC_CONFIG_PARAM_SAMP_FREQ;
        break;

    case XA_CODEC_CONFIG_PARAM_PCM_WIDTH:
        /* ...return sample bit-width */
        i_idx = XA_AACDEC_CONFIG_PARAM_PCM_WDSZ;
        break;
    }

    /* ...pass to library */
    return xa_aac_dec(handle, XA_API_CMD_GET_CONFIG_PARAM, i_idx, pv_value);

}

/*******************************************************************************
 * API entry point
 ******************************************************************************/

XA_ERRORCODE xa_aac_decoder(xa_codec_handle_t p_xa_module_obj, WORD32 i_cmd, WORD32 i_idx, pVOID pv_value)
{
    /* ...process common audio-decoder commands */
    XA_ERRORCODE ret;
#ifdef XAF_PROFILE
    clk_t comp_start, comp_stop;
#endif
    if (i_cmd == XA_API_CMD_GET_CONFIG_PARAM)
    {


        ret = xa_aac_get_config_param(p_xa_module_obj, i_idx, pv_value);
    }
    else
    {
#ifdef XAF_PROFILE
        comp_start = clk_read_start(CLK_SELN_THREAD);
#endif
        ret = xa_aac_dec(p_xa_module_obj, i_cmd, i_idx, pv_value);
#ifdef XAF_PROFILE
        comp_stop = clk_read_stop(CLK_SELN_THREAD);
		aac_dec_cycles += (int)(comp_stop - comp_start);

#endif

    }
	return ret;
}
