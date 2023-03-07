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
 * xa-audio-decoder-api.h
 *
 * Generic audio decoder API
 ******************************************************************************/

#ifndef __XA_ADEC_API_H__
#define __XA_ADEC_API_H__

/* ...includes */
#include "xa_type_def.h"
#include "xa_error_standards.h"
#include "xa_apicmd_standards.h"
#include "xa_memory_standards.h"

/* ...generic audio-decoder configuration parameters */
enum xa_config_param_codec {
    XA_CODEC_CONFIG_PARAM_CHANNELS       = 0x10000 + 0,
    XA_CODEC_CONFIG_PARAM_SAMPLE_RATE    = 0x10000 + 1,
    XA_CODEC_CONFIG_PARAM_PCM_WIDTH      = 0x10000 + 2,
    XA_CODEC_CONFIG_PARAM_PRODUCED       = 0x10000 + 3   
};

/* ...ports indices */
enum xa_codec_ports {
    XA_CODEC_INPUT_PORT  = 0,
    XA_CODEC_OUTPUT_PORT = 1
};

/* ...non-fatal execution errors */
enum
{
    XA_CODEC_EXEC_NO_DATA = XA_ERROR_CODE(xa_severity_nonfatal, xa_class_execute, XA_CODEC_GENERIC, 0)
};

#endif
