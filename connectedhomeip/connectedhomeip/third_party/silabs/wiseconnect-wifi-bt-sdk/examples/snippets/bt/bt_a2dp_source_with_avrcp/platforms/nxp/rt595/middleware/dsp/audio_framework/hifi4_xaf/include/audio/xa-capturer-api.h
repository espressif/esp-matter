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
 * xa-capturer-api.h
 *
 * Capturer component API
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

#ifndef __XA_CAPTURER_API_H__
#define __XA_CAPTURER_API_H__

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

/* ...capturer-specific configuration parameters */
enum xa_config_param_capturer {
    XA_CAPTURER_CONFIG_PARAM_CB             = 0,
    XA_CAPTURER_CONFIG_PARAM_STATE          = 1,
    XA_CAPTURER_CONFIG_PARAM_PCM_WIDTH      = 2,
    XA_CAPTURER_CONFIG_PARAM_CHANNELS       = 3,
    XA_CAPTURER_CONFIG_PARAM_SAMPLE_RATE    = 4,
    XA_CAPTURER_CONFIG_PARAM_FRAME_SIZE     = 5,
    XA_CAPTURER_CONFIG_PARAM_BYTES_PRODUCED  = 6,
    XA_CAPTURER_CONFIG_PARAM_SAMPLE_END      = 7,
    XA_CAPTURER_CONFIG_PARAM_INTERLEAVE      = 8,
    XA_CAPTURER_CONFIG_PARAM_NUM            = 9
};

/* ...XA_CAPTURER_CONFIG_PARAM_CB: compound parameters data structure */
typedef struct xa_capturer_cb_s {
    /* ...input buffer completion callback */
    void      (*cb)(struct xa_capturer_cb_s *, WORD32 idx);

}   xa_capturer_cb_t;


/* ...capturer states  */
enum xa_capturer_state {
    XA_CAPTURER_STATE_IDLE  = 0,
    XA_CAPTURER_STATE_RUN   = 1,
    XA_CAPTURER_STATE_PAUSE = 2
};

/* ...component identifier (informative) */
#define XA_CODEC_CAPTURER               7

/*******************************************************************************
 * Class 0: API Errors
 ******************************************************************************/

#define XA_CAPTURER_API_NONFATAL(e)     \
    XA_ERROR_CODE(xa_severity_nonfatal, xa_class_api, XA_CODEC_CAPTURER, (e))

#define XA_CAPTURER_API_FATAL(e)        \
    XA_ERROR_CODE(xa_severity_fatal, xa_class_api, XA_CODEC_CAPTURER, (e))

enum xa_error_nonfatal_api_capturer {
    XA_CAPTURER_API_NONFATAL_MAX = XA_CAPTURER_API_NONFATAL(0)
};

enum xa_error_fatal_api_capturer {
    XA_CAPTURER_API_FATAL_MAX = XA_CAPTURER_API_FATAL(0)
};

/*******************************************************************************
 * Class 1: Configuration Errors
 ******************************************************************************/

#define XA_CAPTURER_CONFIG_NONFATAL(e)  \
    XA_ERROR_CODE(xa_severity_nonfatal, xa_class_config, XA_CODEC_CAPTURER, (e))

#define XA_CAPTURER_CONFIG_FATAL(e)     \
    XA_ERROR_CODE(xa_severity_fatal, xa_class_config, XA_CODEC_CAPTURER, (e))

enum xa_error_nonfatal_config_capturer {
    XA_CAPTURER_CONFIG_NONFATAL_RANGE   = XA_CAPTURER_CONFIG_NONFATAL(0),
    XA_CAPTURER_CONFIG_NONFATAL_STATE   = XA_CAPTURER_CONFIG_NONFATAL(1),
    XA_CAPTURER_CONFIG_NONFATAL_MAX     = XA_CAPTURER_CONFIG_NONFATAL(2)
};

enum xa_error_fatal_config_capturer {
    XA_CAPTURER_CONFIG_FATAL_HW         = XA_CAPTURER_CONFIG_FATAL(0),
    XA_CAPTURER_CONFIG_FATAL_STATE      = XA_CAPTURER_CONFIG_FATAL(1),
    XA_CAPTURER_CONFIG_FATAL_RANGE      = XA_CAPTURER_CONFIG_FATAL(2),
    XA_CAPTURER_CONFIG_FATAL_MAX        = XA_CAPTURER_CONFIG_FATAL(3)
};

/*******************************************************************************
 * Class 2: Execution Class Errors
 ******************************************************************************/

#define XA_CAPTURER_EXEC_NONFATAL(e)    \
    XA_ERROR_CODE(xa_severity_nonfatal, xa_class_execute, XA_CODEC_CAPTURER, (e))

#define XA_CAPTURER_EXEC_FATAL(e)       \
    XA_ERROR_CODE(xa_severity_fatal, xa_class_execute, XA_CODEC_CAPTURER, (e))

enum xa_error_nonfatal_execute_capturer {
    XA_CAPTURER_EXEC_NONFATAL_STATE     = XA_CAPTURER_EXEC_NONFATAL(0),
    XA_CAPTURER_EXEC_NONFATAL_INPUT     = XA_CAPTURER_EXEC_NONFATAL(1),
    XA_CAPTURER_EXEC_NONFATAL_MAX       = XA_CAPTURER_EXEC_NONFATAL(2),
    XA_CAPTURER_EXEC_NONFATAL_NO_DATA   = XA_CAPTURER_EXEC_NONFATAL(3)
};

enum xa_error_fatal_execute_capturer {
    XA_CAPTURER_EXEC_FATAL_HW           = XA_CAPTURER_EXEC_FATAL(0),
    XA_CAPTURER_EXEC_FATAL_STATE        = XA_CAPTURER_EXEC_FATAL(1),
    XA_CAPTURER_EXEC_FATAL_INPUT        = XA_CAPTURER_EXEC_FATAL(2),
    XA_CAPTURER_EXEC_FATAL_MAX          = XA_CAPTURER_EXEC_FATAL(3)
};

/*******************************************************************************
 * API function definition (tbd)
 ******************************************************************************/

#if defined(USE_DLL) && defined(_WIN32)
#define DLL_SHARED __declspec(dllimport)
#elif defined (_WINDLL)
#define DLL_SHARED __declspec(dllexport)
#else
#define DLL_SHARED
#endif

#if defined(__cplusplus)
extern "C" {
#endif  /* __cplusplus */
DLL_SHARED xa_codec_func_t xa_capturer;
#if defined(__cplusplus)
}
#endif  /* __cplusplus */

#endif /* __XA_CAPTURER_API_H__ */
