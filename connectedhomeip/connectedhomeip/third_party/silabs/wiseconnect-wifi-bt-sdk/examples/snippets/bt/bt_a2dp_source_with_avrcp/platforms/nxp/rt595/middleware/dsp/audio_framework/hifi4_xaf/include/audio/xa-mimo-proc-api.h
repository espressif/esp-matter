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
 * xa-mimo-proc-api.h
 *
 * Generic mimo process API
 ******************************************************************************/

#ifndef __XA_MIMO_PROC_API_H__
#define __XA_MIMO_PROC_API_H__

/* ...includes */
#include "xa_type_def.h"
#include "xa_error_standards.h"
#include "xa_apicmd_standards.h"
#include "xa_memory_standards.h"


/* ...generic mimo-proc configuration parameters */
enum xa_config_param_mimo_proc {
    XA_MIMO_PROC_CONFIG_PARAM_CHANNELS       = 0x10000 + 0,
    XA_MIMO_PROC_CONFIG_PARAM_SAMPLE_RATE    = 0x10000 + 1,
    XA_MIMO_PROC_CONFIG_PARAM_PCM_WIDTH      = 0x10000 + 2,
    XA_MIMO_PROC_CONFIG_PARAM_PRODUCED       = 0x10000 + 3   
};

/* ...generic mimo-proc fatal config error */
enum
{
    XA_MIMO_PROC_CONFIG_FATAL_RANGE = XA_ERROR_CODE(xa_severity_fatal, xa_class_config, XA_CODEC_GENERIC, 0)
};

/* ...generic mimo-proc non-fatal execution errors */
enum
{
    XA_MIMO_PROC_EXEC_NONFATAL_NO_DATA = XA_ERROR_CODE(xa_severity_nonfatal, xa_class_execute, XA_CODEC_GENERIC, 0)
};

/* ...generic mimo-proc fatal execution error */
enum
{
    XA_MIMO_PROC_EXEC_FATAL_STATE = XA_ERROR_CODE(xa_severity_fatal, xa_class_execute, XA_CODEC_GENERIC, 0)
};
#endif
