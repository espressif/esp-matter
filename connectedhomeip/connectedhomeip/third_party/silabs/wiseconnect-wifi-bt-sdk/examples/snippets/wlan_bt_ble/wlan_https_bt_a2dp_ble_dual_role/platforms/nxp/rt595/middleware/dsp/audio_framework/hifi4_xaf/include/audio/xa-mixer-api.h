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
 * xa-mixer-api.h
 *
 * Mixer component API
 ******************************************************************************/

#ifndef __XA_MIXER_API_H__
#define __XA_MIXER_API_H__

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

/* ...mixer-specific configuration parameters */
enum xa_config_param_mixer {
    XA_MIXER_CONFIG_PARAM_INPUT_TRACKS      = 0,
    XA_MIXER_CONFIG_PARAM_PCM_WIDTH         = 1,
    XA_MIXER_CONFIG_PARAM_CHANNELS          = 2,
    XA_MIXER_CONFIG_PARAM_SAMPLE_RATE       = 4,
    XA_MIXER_CONFIG_PARAM_FRAME_SIZE        = 5,
    XA_MIXER_CONFIG_PARAM_BUFFER_SIZE       = 6,
    XA_MIXER_CONFIG_PARAM_VOLUME            = 7,
    XA_MIXER_CONFIG_PARAM_NUM               = 8
};

/* ...component identifier (informative) */
#define XA_CODEC_MIXER                  2

/* ...global limitation - maximal mixer track number */
#define XA_MIXER_MAX_TRACK_NUMBER       4

/* ...volume representation */
#define __XA_MIXER_VOLUME(v)            \
    ({ UWORD32  __v = (UWORD32)((v) * (1 << 12)); (__v > 0xFFFF ? __v = 0xFFFF : 0); (UWORD16)__v; })

/* ...mixer volume setting command encoding */
#define XA_MIXER_VOLUME(track, channel, volume) \
    (__XA_MIXER_VOLUME(volume) | ((track) << 16) | ((channel) << 20))

/*******************************************************************************
 * Class 0: API Errors
 ******************************************************************************/

#define XA_MIXER_API_NONFATAL(e)        \
    XA_ERROR_CODE(xa_severity_nonfatal, xa_class_api, XA_CODEC_MIXER, (e))

#define XA_MIXER_API_FATAL(e)           \
    XA_ERROR_CODE(xa_severity_fatal, xa_class_api, XA_CODEC_MIXER, (e))

enum xa_error_nonfatal_api_mixer {
    XA_MIXER_API_NONFATAL_MAX = XA_MIXER_API_NONFATAL(0)
};

enum xa_error_fatal_api_mixer {
    XA_MIXER_API_FATAL_MAX = XA_MIXER_API_FATAL(0)
};

/*******************************************************************************
 * Class 1: Configuration Errors
 ******************************************************************************/

#define XA_MIXER_CONFIG_NONFATAL(e)     \
    XA_ERROR_CODE(xa_severity_nonfatal, xa_class_config, XA_CODEC_MIXER, (e))

#define XA_MIXER_CONFIG_FATAL(e)        \
    XA_ERROR_CODE(xa_severity_fatal, xa_class_config, XA_CODEC_MIXER, (e))

enum xa_error_nonfatal_config_mixer {
    XA_MIXER_CONFIG_NONFATAL_RANGE  = XA_MIXER_CONFIG_NONFATAL(0),
    XA_MIXER_CONFIG_NONFATAL_STATE  = XA_MIXER_CONFIG_NONFATAL(1),
    XA_MIXER_CONFIG_NONFATAL_MAX    = XA_MIXER_CONFIG_NONFATAL(2)
};

enum xa_error_fatal_config_mixer {
    XA_MIXER_CONFIG_FATAL_RANGE     = XA_MIXER_CONFIG_FATAL(0),
    XA_MIXER_CONFIG_FATAL_TRACK_STATE = XA_MIXER_CONFIG_FATAL(0 + XA_MIXER_CONFIG_NONFATAL_MAX),
    XA_MIXER_CONFIG_FATAL_MAX       = XA_MIXER_CONFIG_FATAL(1)
};

/*******************************************************************************
 * Class 2: Execution Class Errors
 ******************************************************************************/

#define XA_MIXER_EXEC_NONFATAL(e)       \
    XA_ERROR_CODE(xa_severity_nonfatal, xa_class_execute, XA_CODEC_MIXER, (e))

#define XA_MIXER_EXEC_FATAL(e)          \
    XA_ERROR_CODE(xa_severity_fatal, xa_class_execute, XA_CODEC_MIXER, (e))

enum xa_error_nonfatal_execute_mixer {
    XA_MIXER_EXEC_NONFATAL_STATE    = XA_MIXER_EXEC_NONFATAL(0),
    XA_MIXER_EXEC_NONFATAL_NO_DATA  = XA_MIXER_EXEC_NONFATAL(1),
    XA_MIXER_EXEC_NONFATAL_INPUT    = XA_MIXER_EXEC_NONFATAL(2),
    XA_MIXER_EXEC_NONFATAL_OUTPUT   = XA_MIXER_EXEC_NONFATAL(3),
    XA_MIXER_EXEC_NONFATAL_MAX      = XA_MIXER_EXEC_NONFATAL(4)
};

enum xa_error_fatal_execute_mixer {
    XA_MIXER_EXEC_FATAL_STATE       = XA_MIXER_EXEC_FATAL(0),
    XA_MIXER_EXEC_FATAL_INPUT       = XA_MIXER_EXEC_FATAL(1),
    XA_MIXER_EXEC_FATAL_OUTPUT      = XA_MIXER_EXEC_FATAL(2),
    XA_MIXER_EXEC_FATAL_MAX         = XA_MIXER_EXEC_FATAL(3)
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
DLL_SHARED xa_codec_func_t xa_mixer;
#if defined(__cplusplus)
}
#endif  /* __cplusplus */

#endif /* __XA_MIXER_API_H__ */
