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
 * xa-pcm-split-api.h
 *
 * pcm-split component API
 ******************************************************************************/

#ifndef __XA_PCM_SPLIT_API_H__
#define __XA_PCM_SPLIT_API_H__

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "xa_memory_standards.h"

/* ...generic commands */
#include "xa_apicmd_standards.h"

/* ...generic error codes */
#include "xa_error_standards.h"

/* ...common types */
#include "xa_type_def.h"

/*******************************************************************************
 * Constants definitions
 ******************************************************************************/

/* ...pcm_split-specific configuration parameters */
enum xa_config_param_pcm_split {
    XA_PCM_SPLIT_CONFIG_PARAM_CHANNELS          = 0,
    XA_PCM_SPLIT_CONFIG_PARAM_SAMPLE_RATE       = 1,
    XA_PCM_SPLIT_CONFIG_PARAM_PCM_WIDTH         = 2,
    XA_PCM_SPLIT_CONFIG_PARAM_PRODUCED          = 3,
};

/* ...component identifier (informative) */
#define XA_MIMO_PROC_PCM_SPLIT                  0x23

/*******************************************************************************
 * Class 1: Configuration Errors
 ******************************************************************************/

enum xa_error_nonfatal_config_pcm_split {
    XA_PCM_SPLIT_CONFIG_NONFATAL_RANGE  = XA_ERROR_CODE(xa_severity_nonfatal, xa_class_config, XA_MIMO_PROC_PCM_SPLIT, 0),
};

enum xa_error_fatal_config_pcm_split {
    XA_PCM_SPLIT_CONFIG_FATAL_RANGE     = XA_ERROR_CODE(xa_severity_fatal, xa_class_config, XA_MIMO_PROC_PCM_SPLIT, 0),
};

/*******************************************************************************
 * Class 2: Execution Class Errors
 ******************************************************************************/

enum xa_error_nonfatal_execute_pcm_split {
    XA_PCM_SPLIT_EXEC_NONFATAL_NO_DATA  = XA_ERROR_CODE(xa_severity_nonfatal, xa_class_execute, XA_MIMO_PROC_PCM_SPLIT, 0),
};

enum xa_error_fatal_execute_pcm_split {
    XA_PCM_SPLIT_EXEC_FATAL_STATE       = XA_ERROR_CODE(xa_severity_fatal, xa_class_execute, XA_MIMO_PROC_PCM_SPLIT, 0),
};

#endif /* __XA_PCM_SPLIT_API_H__ */
