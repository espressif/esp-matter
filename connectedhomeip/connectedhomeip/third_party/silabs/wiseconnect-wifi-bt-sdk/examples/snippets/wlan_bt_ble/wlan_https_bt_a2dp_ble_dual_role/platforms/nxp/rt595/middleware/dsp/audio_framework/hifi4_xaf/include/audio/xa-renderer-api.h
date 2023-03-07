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
 * xa-renderer-api.h
 *
 * Renderer component API
 *
 * Copyright (c) 20131 by Tensilica Inc.  ALL RIGHTS RESERVED.
 *
 * These coded instructions, statements, and computer programs are the
 * copyrighted works and confidential proprietary information of
 * Tensilica Inc. and its licensors, and are licensed to the recipient
 * under the terms of a separate license agreement.  They may be
 * adapted and modified by bona fide purchasers under the terms of the
 * separate license agreement for internal use, but no adapted or
 * modified version may be disclosed or distributed to third parties
 * in any manner, medium, or form, in whole or in part, without the
 * prior written consent of Tensilica Inc.
 ******************************************************************************/

#ifndef __XA_RENDERER_API_H__
#define __XA_RENDERER_API_H__

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "xa_type_def.h"
#include "xa_error_standards.h"
#include "xa_apicmd_standards.h"
#include "xa_memory_standards.h"

/*******************************************************************************
 * Constants definitions
 ******************************************************************************/

/* ...renderer-specific configuration parameters */
enum xa_config_param_renderer {
    XA_RENDERER_CONFIG_PARAM_CB             = 0,
    XA_RENDERER_CONFIG_PARAM_STATE          = 1,
    XA_RENDERER_CONFIG_PARAM_PCM_WIDTH      = 2,
    XA_RENDERER_CONFIG_PARAM_CHANNELS       = 3,
    XA_RENDERER_CONFIG_PARAM_SAMPLE_RATE    = 4,
    XA_RENDERER_CONFIG_PARAM_FRAME_SIZE     = 5,
    XA_RENDERER_CONFIG_PARAM_BYTES_PRODUCED = 6,
    XA_RENDERER_CONFIG_PARAM_NUM            = 7
};

/* ...XA_RENDERER_CONFIG_PARAM_CB: compound parameters data structure */
typedef struct xa_renderer_cb_s {
    /* ...input buffer completion callback */
    void      (*cb)(struct xa_renderer_cb_s *, WORD32 idx);

}   xa_renderer_cb_t;
    

/* ...renderer states  */
enum xa_randerer_state {
    XA_RENDERER_STATE_IDLE  = 0,
    XA_RENDERER_STATE_RUN   = 1,
    XA_RENDERER_STATE_PAUSE = 2
};
    
/* ...component identifier (informative) */
#define XA_CODEC_RENDERER               6

/*******************************************************************************
 * Class 0: API Errors
 ******************************************************************************/

#define XA_RENDERER_API_NONFATAL(e)     \
    XA_ERROR_CODE(xa_severity_nonfatal, xa_class_api, XA_CODEC_RENDERER, (e))

#define XA_RENDERER_API_FATAL(e)        \
    XA_ERROR_CODE(xa_severity_fatal, xa_class_api, XA_CODEC_RENDERER, (e))

enum xa_error_nonfatal_api_renderer {
    XA_RENDERER_API_NONFATAL_MAX = XA_RENDERER_API_NONFATAL(0)
};

enum xa_error_fatal_api_renderer {
    XA_RENDERER_API_FATAL_MAX = XA_RENDERER_API_FATAL(0)
};

/*******************************************************************************
 * Class 1: Configuration Errors
 ******************************************************************************/

#define XA_RENDERER_CONFIG_NONFATAL(e)  \
    XA_ERROR_CODE(xa_severity_nonfatal, xa_class_config, XA_CODEC_RENDERER, (e))

#define XA_RENDERER_CONFIG_FATAL(e)     \
    XA_ERROR_CODE(xa_severity_fatal, xa_class_config, XA_CODEC_RENDERER, (e))

enum xa_error_nonfatal_config_renderer {
    XA_RENDERER_CONFIG_NONFATAL_RANGE   = XA_RENDERER_CONFIG_NONFATAL(0),
    XA_RENDERER_CONFIG_NONFATAL_STATE   = XA_RENDERER_CONFIG_NONFATAL(1),
    XA_RENDERER_CONFIG_NONFATAL_MAX     = XA_RENDERER_CONFIG_NONFATAL(2)
};

enum xa_error_fatal_config_renderer {
    XA_RENDERER_CONFIG_FATAL_HW         = XA_RENDERER_CONFIG_FATAL(0),
    XA_RENDERER_CONFIG_FATAL_STATE      = XA_RENDERER_CONFIG_FATAL(1),
    XA_RENDERER_CONFIG_FATAL_RANGE      = XA_RENDERER_CONFIG_FATAL(2),
    XA_RENDERER_CONFIG_FATAL_MAX        = XA_RENDERER_CONFIG_FATAL(3)
};

/*******************************************************************************
 * Class 2: Execution Class Errors
 ******************************************************************************/

#define XA_RENDERER_EXEC_NONFATAL(e)    \
    XA_ERROR_CODE(xa_severity_nonfatal, xa_class_execute, XA_CODEC_RENDERER, (e))

#define XA_RENDERER_EXEC_FATAL(e)       \
    XA_ERROR_CODE(xa_severity_fatal, xa_class_execute, XA_CODEC_RENDERER, (e))

enum xa_error_nonfatal_execute_renderer {
    XA_RENDERER_EXEC_NONFATAL_STATE     = XA_RENDERER_EXEC_NONFATAL(0),
    XA_RENDERER_EXEC_NONFATAL_INPUT     = XA_RENDERER_EXEC_NONFATAL(1),
    XA_RENDERER_EXEC_NONFATAL_MAX       = XA_RENDERER_EXEC_NONFATAL(2)
};

enum xa_error_fatal_execute_renderer {
    XA_RENDERER_EXEC_FATAL_HW           = XA_RENDERER_EXEC_FATAL(0),
    XA_RENDERER_EXEC_FATAL_STATE        = XA_RENDERER_EXEC_FATAL(1),
    XA_RENDERER_EXEC_FATAL_INPUT        = XA_RENDERER_EXEC_FATAL(2),
    XA_RENDERER_EXEC_FATAL_MAX          = XA_RENDERER_EXEC_FATAL(3)
};

/*******************************************************************************
 * API function definition (tbd)
 ******************************************************************************/

#if 0

#if defined(__cplusplus)
extern "C" {
#endif  /* __cplusplus */
#define DLL_SHARED
DLL_SHARED xa_codec_func_t xa_renderer;
#if defined(__cplusplus)
}
#endif  /* __cplusplus */
#endif
#endif /* __XA_RENDERER_API_H__ */
