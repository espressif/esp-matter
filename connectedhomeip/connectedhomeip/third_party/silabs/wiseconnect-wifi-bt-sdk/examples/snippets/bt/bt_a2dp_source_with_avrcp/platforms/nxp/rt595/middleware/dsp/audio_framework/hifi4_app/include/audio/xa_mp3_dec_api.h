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


#ifndef __XA_MP3_DEC_API_H__
#define __XA_MP3_DEC_API_H__

/*****************************************************************************/
/* MP3 Decoder specific API definitions                                      */
/*****************************************************************************/

/* mp3_dec-specific configuration parameters */
enum xa_config_param_mp3_dec {
  XA_MP3DEC_CONFIG_PARAM_PCM_WDSZ     = 0,
  XA_MP3DEC_CONFIG_PARAM_SAMP_FREQ    = 1,
  XA_MP3DEC_CONFIG_PARAM_NUM_CHANNELS = 2,
  XA_MP3DEC_CONFIG_PARAM_BITRATE      = 3,
  XA_MP3DEC_CONFIG_PARAM_CRC_CHECK    = 4,
  XA_MP3DEC_CONFIG_PARAM_MCH_ENABLE   = 5,
  XA_MP3DEC_CONFIG_PARAM_MCH_STATUS   = 6,
  XA_MP3DEC_CONFIG_PARAM_LFE_PRESENT  = 7,
  XA_MP3DEC_CONFIG_PARAM_EXTN_PRESENT = 8,
  XA_MP3DEC_CONFIG_PARAM_CHAN_CONFIG  = 9,
  XA_MP3DEC_CONFIG_PARAM_CHAN_MAP     = 10,
  XA_MP3DEC_CONFIG_PARAM_NUM_XCHANS   = 11,
  XA_MP3DEC_CONFIG_PARAM_CHMODE_INFO  = 12
#ifdef ENABLE_SCF_CRC
  ,XA_MP3DEC_CONFIG_PARAM_DAB_MP2 = 13

#endif

  ,XA_MP3DEC_CONFIG_PARAM_ACTIVATE_VLC_REWIND = 14
  ,XA_MP3DEC_CONFIG_PARAM_ORIGINAL_OR_COPY    = 15
  ,XA_MP3DEC_CONFIG_PARAM_COPYRIGHT_FLAG      = 16
  ,XA_MP3DEC_CONFIG_PARAM_MCH_EXT_HDR_INFO    = 17
  ,XA_MP3DEC_CONFIG_PARAM_MCH_COPYRIGHT_ID_PTR    = 18
};

enum xa_mp3_dec_mch_status {
  XA_MP3DEC_MCH_STATUS_UNSUPPORTED    = 0,
  XA_MP3DEC_MCH_STATUS_DISABLED       = 1,
  XA_MP3DEC_MCH_STATUS_NOT_PRESENT    = 2,
  XA_MP3DEC_MCH_STATUS_PRESENT        = 3
};

/* commands */
#include "xa_apicmd_standards.h"

/* mp3_dec-specific commands */
/* (none) */

/* mp3_dec-specific command types */
/* (none) */

/* error codes */
#include "xa_error_standards.h"

#define XA_CODEC_MP3_DEC	1

/* mp3_dec-specific error codes */
/*****************************************************************************/
/* Class 1: Configuration Errors                                             */
/*****************************************************************************/
/* Nonfatal Errors */
enum xa_error_nonfatal_config_mp3_dec {
  XA_MP3DEC_CONFIG_NONFATAL_MP3_PCM_ADJUST_16 = XA_ERROR_CODE(xa_severity_nonfatal, xa_class_config, XA_CODEC_MP3_DEC, 0),
  XA_MP3DEC_CONFIG_NONFATAL_MP3_PCM_ADJUST_24 = XA_ERROR_CODE(xa_severity_nonfatal, xa_class_config, XA_CODEC_MP3_DEC, 1),
  XA_MP3DEC_CONFIG_NONFATAL_INVALID_GEN_STRM_POS = XA_ERROR_CODE(xa_severity_nonfatal, xa_class_config, XA_CODEC_MP3_DEC, 2),
  XA_MP3DEC_CONFIG_NONFATAL_MCH_NOT_SUPPORTED = XA_ERROR_CODE(xa_severity_nonfatal, xa_class_config, XA_CODEC_MP3_DEC, 3)
};

/* Fatal Errors */
enum xa_error_fatal_config_mp3_dec {
  XA_MP3DEC_CONFIG_FATAL_SAMP_FREQ    = XA_ERROR_CODE(xa_severity_fatal, xa_class_config, XA_CODEC_MP3_DEC, 0),
  XA_MP3DEC_CONFIG_FATAL_NUM_CHANNELS = XA_ERROR_CODE(xa_severity_fatal, xa_class_config, XA_CODEC_MP3_DEC, 1)
};

/*****************************************************************************/
/* Class 2: Execution Errors                                                 */
/*****************************************************************************/
/* Nonfatal Errors */
enum xa_error_nonfatal_execute_mp3_dec {
  XA_MP3DEC_EXECUTE_NONFATAL_NEED_MORE           = XA_ERROR_CODE(xa_severity_nonfatal, xa_class_execute, XA_CODEC_MP3_DEC, 0),
  XA_MP3DEC_EXECUTE_NONFATAL_CANNOT_REWIND       = XA_ERROR_CODE(xa_severity_nonfatal, xa_class_execute, XA_CODEC_MP3_DEC, 1),
  XA_MP3DEC_EXECUTE_NONFATAL_CHANGED_CHANNELS    = XA_ERROR_CODE(xa_severity_nonfatal, xa_class_execute, XA_CODEC_MP3_DEC, 2),
  XA_MP3DEC_EXECUTE_NONFATAL_CHANGED_LAYER       = XA_ERROR_CODE(xa_severity_nonfatal, xa_class_execute, XA_CODEC_MP3_DEC, 3),
  XA_MP3DEC_EXECUTE_NONFATAL_CHANGED_SAMP_FREQ   = XA_ERROR_CODE(xa_severity_nonfatal, xa_class_execute, XA_CODEC_MP3_DEC, 4),
  XA_MP3DEC_EXECUTE_NONFATAL_NEXT_SYNC_NOT_FOUND = XA_ERROR_CODE(xa_severity_nonfatal, xa_class_execute, XA_CODEC_MP3_DEC, 5),
  XA_MP3DEC_EXECUTE_NONFATAL_CRC_FAILED          = XA_ERROR_CODE(xa_severity_nonfatal, xa_class_execute, XA_CODEC_MP3_DEC, 6),
  XA_MP3DEC_EXECUTE_NONFATAL_NO_MAIN_AUDIO_INPUT = XA_ERROR_CODE(xa_severity_nonfatal, xa_class_execute, XA_CODEC_MP3_DEC, 7),
  XA_MP3DEC_EXECUTE_NONFATAL_MCH_CRC_ERROR        = XA_ERROR_CODE(xa_severity_nonfatal, xa_class_execute, XA_CODEC_MP3_DEC, 8),
  XA_MP3DEC_EXECUTE_NONFATAL_MCH_EXT_NOTFOUND        = XA_ERROR_CODE(xa_severity_nonfatal, xa_class_execute, XA_CODEC_MP3_DEC, 9)
  ,XA_MP3DEC_EXECUTE_NONFATAL_INVALID_BITRATE_MODE_COMB = XA_ERROR_CODE(xa_severity_nonfatal, xa_class_execute, XA_CODEC_MP3_DEC, 10)
#ifdef ENABLE_SCF_CRC
  ,XA_MP3DEC_EXECUTE_NONFATAL_SCF_CRC_FAILED1         = XA_ERROR_CODE(xa_severity_nonfatal, xa_class_execute, XA_CODEC_MP3_DEC, 17),
  XA_MP3DEC_EXECUTE_NONFATAL_SCF_CRC_FAILED2         = XA_ERROR_CODE(xa_severity_nonfatal, xa_class_execute, XA_CODEC_MP3_DEC, 18),
  XA_MP3DEC_EXECUTE_NONFATAL_SCF_CRC_FAILED3         = XA_ERROR_CODE(xa_severity_nonfatal, xa_class_execute, XA_CODEC_MP3_DEC, 19),
  XA_MP3DEC_EXECUTE_NONFATAL_SCF_CRC_FAILED4         = XA_ERROR_CODE(xa_severity_nonfatal, xa_class_execute, XA_CODEC_MP3_DEC, 20)
#endif
};

/* Fatal Errors */
enum xa_error_fatal_execute_mp3_dec {
  XA_MP3DEC_EXECUTE_FATAL_MPEG1_NOT_SUPPORTED = XA_ERROR_CODE(xa_severity_fatal, xa_class_execute, XA_CODEC_MP3_DEC, 0),	/* apparently unused */
  XA_MP3DEC_EXECUTE_FATAL_UNSUPPORTED_LAYER   = XA_ERROR_CODE(xa_severity_fatal, xa_class_execute, XA_CODEC_MP3_DEC, 1),
  XA_MP3DEC_EXECUTE_FATAL_OVERLOADED_IN_BUF   = XA_ERROR_CODE(xa_severity_fatal, xa_class_execute, XA_CODEC_MP3_DEC, 2),
  XA_MP3DEC_EXECUTE_FATAL_STREAM_ERROR        = XA_ERROR_CODE(xa_severity_fatal, xa_class_execute, XA_CODEC_MP3_DEC, 3)
};

#include "xa_type_def.h"

/* metadata information api-interface structure */
typedef struct _xa_mc_ext_hdr_info_t
{
    /* mc header decoded here*/
    WORD    ext_bit_stream_present;
    WORD    n_ad_bytes;
    WORD    center;
    WORD    surround;
    WORD    lfe;
    WORD    audio_mix;
    WORD    dematrix_procedure;
    WORD    no_of_multi_lingual_ch;
    WORD    multi_lingual_fs;
    WORD    multi_lingual_layer;
} xa_mch_ext_hdr_info_t;

#if defined(__cplusplus)
extern "C" {
#endif	/* __cplusplus */
xa_codec_func_t xa_mp3_dec;
#if defined(__cplusplus)
}
#endif	/* __cplusplus */
#endif /* __XA_MP3_DEC_API_H__ */
