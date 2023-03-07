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

#ifndef __XA_AAC_DEC_API_H__
#define __XA_AAC_DEC_API_H__

/* aac_dec-specific configuration parameters */
enum xa_config_param_aac_dec {
  XA_AACDEC_CONFIG_PARAM_BDOWNSAMPLE          = 0,
  XA_AACDEC_CONFIG_PARAM_BBITSTREAMDOWNMIX    = 1,
  XA_AACDEC_CONFIG_PARAM_EXTERNALSAMPLINGRATE = 2,
  XA_AACDEC_CONFIG_PARAM_EXTERNALBSFORMAT     = 3,
  XA_AACDEC_CONFIG_PARAM_TO_STEREO            = 4,
  XA_AACDEC_CONFIG_PARAM_SAMP_FREQ            = 5,
  XA_AACDEC_CONFIG_PARAM_NUM_CHANNELS         = 6,
  XA_AACDEC_CONFIG_PARAM_PCM_WDSZ             = 7,
  XA_AACDEC_CONFIG_PARAM_SBR_TYPE             = 8,
  XA_AACDEC_CONFIG_PARAM_AAC_SAMPLERATE       = 9,
  XA_AACDEC_CONFIG_PARAM_DATA_RATE            = 10,
  XA_AACDEC_CONFIG_PARAM_OUTNCHANS            = 11,
  XA_AACDEC_CONFIG_PARAM_CHANROUTING          = 12,
  XA_AACDEC_CONFIG_PARAM_SBR_SIGNALING        = 13,
  XA_AACDEC_CONFIG_PARAM_CHANMAP              = 14,
  XA_AACDEC_CONFIG_PARAM_ACMOD                = 15,
  XA_AACDEC_CONFIG_PARAM_AAC_FORMAT           = 16,
  XA_AACDEC_CONFIG_PARAM_ZERO_UNUSED_CHANS    = 17,
  XA_AACDEC_CONFIG_PARAM_DECODELAYERS         = 18,
  XA_AACDEC_CONFIG_PARAM_EXTERNALCHCONFIG     = 19,
  XA_AACDEC_CONFIG_PARAM_RAW_AU_SIDEINFO      = 20,
  XA_AACDEC_CONFIG_PARAM_EXTERNALBITRATE      = 21,
  XA_AACDEC_CONFIG_PARAM_PAD_SIZE             = 22,
  XA_AACDEC_CONFIG_PARAM_PAD_PTR              = 23,
  XA_AACDEC_CONFIG_PARAM_MPEGSURR_PRESENT     = 24,   /* For DAB-plus only */
  XA_AACDEC_CONFIG_PARAM_METADATASTRUCT_PTR   = 25,   /* Only if Audio MetaData support is present for the library */
  XA_AACDEC_CONFIG_PARAM_ASCONFIGSTRUCT_PTR   = 26,   /* Only if Audio MetaData support is present for the library */
  XA_AACDEC_CONFIG_PARAM_LIMITBANDWIDTH       = 27,   /* ms10 ddt only */
  XA_AACDEC_CONFIG_PARAM_PCE_STATUS           = 28,   /* for Loas build only */
  XA_AACDEC_CONFIG_PARAM_DWNMIX_METADATA      = 29,   /* for Loas build only */
  XA_AACDEC_CONFIG_PARAM_MPEG_ID              = 30,
  XA_AACDEC_CONFIG_PARAM_DWNMIX_LEVEL_DVB     = 31     /* for Loas build only */
#ifdef APPLY_ISO_DRC
  /* DRC and PRL information as per ISO/IEC 14496.3 */
  /* PRL Parametbers */
  ,XA_AACDEC_CONFIG_PARAM_ENABLE_APPLY_PRL     = 32     /* for Loas build only */
  ,XA_AACDEC_CONFIG_PARAM_TARGET_LEVEL         = 33     /* for Loas build only */
  ,XA_AACDEC_CONFIG_PARAM_PROG_REF_LEVEL       = 34     /* for Loas build only */
  /* DRC Parametbers */
  ,XA_AACDEC_CONFIG_PARAM_ENABLE_APPLY_DRC     = 35     /* for Loas build only */
  ,XA_AACDEC_CONFIG_PARAM_DRC_COMPRESS_FAC     = 36     /* for Loas build only */
  ,XA_AACDEC_CONFIG_PARAM_DRC_BOOST_FAC        = 37     /* for Loas build only */
#endif /* APPLY_ISO_DRC */
  ,XA_AACDEC_CONFIG_PARAM_DRC_EXT_PRESENT      = 38     /* for dabplus and all aacplus builds (except loas) */
  ,XA_AACDEC_CONFIG_PARAM_ORIGINAL_OR_COPY     = 39     /* for ADTS and ADIF files only */
  ,XA_AACDEC_CONFIG_PARAM_COPYRIGHT_ID_PTR     = 40     /* for ADTS and ADIF files only */
  ,XA_AACDEC_CONFIG_PARAM_PARSED_DRC_INFO      = 41     /* for loas and all aacmch builds */
};

/* Types of channel modes (acmod) */
typedef enum {
  XA_AACDEC_CHANNELMODE_UNDEFINED = 0,
  XA_AACDEC_CHANNELMODE_MONO,
  XA_AACDEC_CHANNELMODE_PARAMETRIC_STEREO,
  XA_AACDEC_CHANNELMODE_DUAL_CHANNEL,
  XA_AACDEC_CHANNELMODE_STEREO,
  XA_AACDEC_CHANNELMODE_3_CHANNEL_FRONT,
  XA_AACDEC_CHANNELMODE_3_CHANNEL_SURR,
  XA_AACDEC_CHANNELMODE_4_CHANNEL_2SURR,
  XA_AACDEC_CHANNELMODE_4_CHANNEL_1SURR,
  XA_AACDEC_CHANNELMODE_5_CHANNEL,
  XA_AACDEC_CHANNELMODE_6_CHANNEL,
  XA_AACDEC_CHANNELMODE_7_CHANNEL,
  XA_AACDEC_CHANNELMODE_2_1_STEREO,
  XA_AACDEC_CHANNELMODE_3_1_CHANNEL_FRONT,
  XA_AACDEC_CHANNELMODE_3_1_CHANNEL_SURR,
  XA_AACDEC_CHANNELMODE_4_1_CHANNEL_2SURR,
  XA_AACDEC_CHANNELMODE_4_1_CHANNEL_1SURR,
  XA_AACDEC_CHANNELMODE_5_1_CHANNEL,
  XA_AACDEC_CHANNELMODE_6_1_CHANNEL,
  XA_AACDEC_CHANNELMODE_7_1_CHANNEL
} XA_AACDEC_CHANNELMODE;

/* Types of bitstreams */
typedef enum {
  /* The bitstream type has not (yet) been successfully determined. */
  XA_AACDEC_EBITSTREAM_TYPE_UNKNOWN = 0,
  /* ADIF is an unsynced, unframed format. Errors in the bitstream cannot always
     be detected, and when they occur, no further parsing is possible. Avoid ADIF at
     all costs. */
  XA_AACDEC_EBITSTREAM_TYPE_AAC_ADIF = 1,
  /* ADTS is a simple synced framing format similar to MPEG layer-3. */
  XA_AACDEC_EBITSTREAM_TYPE_AAC_ADTS = 2,
  /* LATM, with in-band config. This format cannot be detected by the library;
     it needs to be signaled explicitely. */
  XA_AACDEC_EBITSTREAM_TYPE_AAC_LATM = 3,
  /* LATM, with out of band config. This format cannot be detected by the library;
     it needs to be signaled explicitely. */
  XA_AACDEC_EBITSTREAM_TYPE_AAC_LATM_OUTOFBAND_CONFIG = 4,
  /* Low overhead audio stream. */
  XA_AACDEC_EBITSTREAM_TYPE_AAC_LOAS = 5,

  /* Raw bitstream. This format cannot be detected by the library;
     it needs to be signaled explicitly. */
  XA_AACDEC_EBITSTREAM_TYPE_AAC_RAW = 6,

  /* Raw bsac bitstream. it needs to be signaled explicitely. */
  XA_AACDEC_EBITSTREAM_TYPE_BSAC_RAW = 7,

  /* Raw DAB+ bitstream. It needs sideInfo for every frame for error recovery */
  XA_AACDEC_EBITSTREAM_TYPE_DABPLUS_RAW_SIDEINFO = 8,

  /* DAB+ audio superframe bitstream */
  XA_AACDEC_EBITSTREAM_TYPE_DABPLUS = 9

} XA_AACDEC_EBITSTREAM_TYPE;

/* commands */
#include "xa_apicmd_standards.h"

/* error codes */
#include "xa_error_standards.h"

#define XA_CODEC_AAC_DEC 3

/* aac_dec-specific error codes */
/*****************************************************************************/
/* Class 0: API Errors                                                       */
/*****************************************************************************/
/* Non Fatal Errors */
enum xa_error_nonfatal_api_aac_dec {
  XA_AACDEC_API_NONFATAL_CMD_TYPE_NOT_SUPPORTED = XA_ERROR_CODE(xa_severity_nonfatal, xa_class_api, XA_CODEC_AAC_DEC, 0)
};
/* Fatal Errors */
/* (none) */

/*****************************************************************************/
/* Class 1: Configuration Errors                                             */
/*****************************************************************************/
/* Nonfatal Errors */
enum xa_error_nonfatal_config_aac_dec {
  XA_AACDEC_CONFIG_NONFATAL_PARAMS_NOT_SET          = XA_ERROR_CODE(xa_severity_nonfatal, xa_class_config, XA_CODEC_AAC_DEC, 0),
  XA_AACDEC_CONFIG_NONFATAL_DATA_RATE_NOT_SET       = XA_ERROR_CODE(xa_severity_nonfatal, xa_class_config, XA_CODEC_AAC_DEC, 1),
  XA_AACDEC_CONFIG_NONFATAL_PARTIAL_CHANROUTING    = XA_ERROR_CODE(xa_severity_nonfatal, xa_class_config, XA_CODEC_AAC_DEC, 2)
  ,XA_AACDEC_CONFIG_NONFATAL_INVALID_GEN_STRM_POS   = XA_ERROR_CODE(xa_severity_nonfatal, xa_class_config, XA_CODEC_AAC_DEC, 3)
  ,XA_AACDEC_CONFIG_NONFATAL_CPID_NOT_PERSENT       = XA_ERROR_CODE(xa_severity_nonfatal, xa_class_config, XA_CODEC_AAC_DEC, 4)
#ifdef APPLY_ISO_DRC
  ,XA_AACDEC_CONFIG_NONFATAL_INVALID_PRL_PARAMS     = XA_ERROR_CODE(xa_severity_nonfatal, xa_class_config, XA_CODEC_AAC_DEC, 5)
  ,XA_AACDEC_CONFIG_NONFATAL_INVALID_DRC_PARAMS     = XA_ERROR_CODE(xa_severity_nonfatal, xa_class_config, XA_CODEC_AAC_DEC, 6)
#endif /* APPLY_ISO_DRC */
};
/* Fatal Errors */
enum xa_error_fatal_config_aac_dec {
  XA_AACDEC_CONFIG_FATAL_INVALID_BDOWNSAMPLE          = XA_ERROR_CODE(xa_severity_fatal, xa_class_config, XA_CODEC_AAC_DEC, 0),
  XA_AACDEC_CONFIG_FATAL_INVALID_BBITSTREAMDOWNMIX    = XA_ERROR_CODE(xa_severity_fatal, xa_class_config, XA_CODEC_AAC_DEC, 1),
  XA_AACDEC_CONFIG_FATAL_INVALID_EXTERNALSAMPLINGRATE = XA_ERROR_CODE(xa_severity_fatal, xa_class_config, XA_CODEC_AAC_DEC, 2),
  XA_AACDEC_CONFIG_FATAL_INVALID_EXTERNALBSFORMAT     = XA_ERROR_CODE(xa_severity_fatal, xa_class_config, XA_CODEC_AAC_DEC, 3),
  XA_AACDEC_CONFIG_FATAL_INVALID_TO_STEREO            = XA_ERROR_CODE(xa_severity_fatal, xa_class_config, XA_CODEC_AAC_DEC, 4),
  XA_AACDEC_CONFIG_FATAL_INVALID_OUTNCHANS            = XA_ERROR_CODE(xa_severity_fatal, xa_class_config, XA_CODEC_AAC_DEC, 5),
  XA_AACDEC_CONFIG_FATAL_INVALID_SBR_SIGNALING        = XA_ERROR_CODE(xa_severity_fatal, xa_class_config, XA_CODEC_AAC_DEC, 6),
  XA_AACDEC_CONFIG_FATAL_INVALID_CHANROUTING          = XA_ERROR_CODE(xa_severity_fatal, xa_class_config, XA_CODEC_AAC_DEC, 7),
  XA_AACDEC_CONFIG_FATAL_INVALID_PCM_WDSZ             = XA_ERROR_CODE(xa_severity_fatal, xa_class_config, XA_CODEC_AAC_DEC, 8),
  XA_AACDEC_CONFIG_FATAL_INVALID_ZERO_UNUSED_CHANS    = XA_ERROR_CODE(xa_severity_fatal, xa_class_config, XA_CODEC_AAC_DEC, 9),
  /* Code For Invalid Number of input channels */
  XA_AACDEC_CONFIG_FATAL_INVALID_EXTERNALCHCONFIG    = XA_ERROR_CODE(xa_severity_fatal, xa_class_config, XA_CODEC_AAC_DEC, 10),
  XA_AACDEC_CONFIG_FATAL_INVALID_DECODELAYERS        = XA_ERROR_CODE(xa_severity_fatal, xa_class_config, XA_CODEC_AAC_DEC, 11),
  XA_AACDEC_CONFIG_FATAL_INVALID_EXTERNALBITRATE     = XA_ERROR_CODE(xa_severity_fatal, xa_class_config, XA_CODEC_AAC_DEC, 12)
};
/*****************************************************************************/
/* Class 2: Execution Class Errors                                           */
/*****************************************************************************/
/* Nonfatal Errors */
enum xa_error_nonfatal_execute_aac_dec {
  XA_AACDEC_EXECUTE_NONFATAL_INSUFFICIENT_FRAME_DATA   = XA_ERROR_CODE(xa_severity_nonfatal, xa_class_execute, XA_CODEC_AAC_DEC, 0),
  XA_AACDEC_EXECUTE_NONFATAL_RUNTIME_INIT_RAMP_DOWN    = XA_ERROR_CODE(xa_severity_nonfatal, xa_class_execute, XA_CODEC_AAC_DEC, 1)
  ,XA_AACDEC_EXECUTE_NONFATAL_RAW_FRAME_PARSE_ERROR     = XA_ERROR_CODE(xa_severity_nonfatal, xa_class_execute, XA_CODEC_AAC_DEC, 2)
  ,XA_AACDEC_EXECUTE_NONFATAL_ADTS_HEADER_ERROR        = XA_ERROR_CODE(xa_severity_nonfatal, xa_class_execute, XA_CODEC_AAC_DEC, 3)
  ,XA_AACDEC_EXECUTE_NONFATAL_ADTS_HEADER_NOT_FOUND    = XA_ERROR_CODE(xa_severity_nonfatal, xa_class_execute, XA_CODEC_AAC_DEC, 4)
  ,XA_AACDEC_EXECUTE_NONFATAL_DABPLUS_HEADER_NOT_FOUND = XA_ERROR_CODE(xa_severity_nonfatal, xa_class_execute, XA_CODEC_AAC_DEC, 5)
  ,XA_AACDEC_EXECUTE_NONFATAL_LOAS_HEADER_ERROR         = XA_ERROR_CODE(xa_severity_nonfatal, xa_class_execute, XA_CODEC_AAC_DEC, 6)
  ,XA_AACDEC_EXECUTE_NONFATAL_STREAM_CHANGE             = XA_ERROR_CODE(xa_severity_nonfatal, xa_class_execute, XA_CODEC_AAC_DEC, 7)
};
/* Fatal Errors */
enum xa_error_fatal_execute_aac_dec {
  XA_AACDEC_EXECUTE_FATAL_PARSING_ERROR                  = XA_ERROR_CODE(xa_severity_fatal, xa_class_execute, XA_CODEC_AAC_DEC, 0),
  XA_AACDEC_EXECUTE_FATAL_RAW_FRAME_PARSE_ERROR          = XA_ERROR_CODE(xa_severity_fatal, xa_class_execute, XA_CODEC_AAC_DEC, 1),
  XA_AACDEC_EXECUTE_FATAL_BAD_INPUT_FAILURE              = XA_ERROR_CODE(xa_severity_fatal, xa_class_execute, XA_CODEC_AAC_DEC, 2),
  XA_AACDEC_EXECUTE_FATAL_UNSUPPORTED_FORMAT             = XA_ERROR_CODE(xa_severity_fatal, xa_class_execute, XA_CODEC_AAC_DEC, 3),
  XA_AACDEC_EXECUTE_FATAL_ERROR_IN_CHANROUTING           = XA_ERROR_CODE(xa_severity_fatal, xa_class_execute, XA_CODEC_AAC_DEC, 4),
  XA_AACDEC_EXECUTE_FATAL_ZERO_FRAME_LENGTH              = XA_ERROR_CODE(xa_severity_fatal, xa_class_execute, XA_CODEC_AAC_DEC, 5)
  ,XA_AACDEC_EXECUTE_FATAL_LOAS_HEADER_CHANGE            = XA_ERROR_CODE(xa_severity_fatal, xa_class_execute, XA_CODEC_AAC_DEC, 6)
};

#include "xa_type_def.h"

/* Relevant for loas build only */
/* PCE status in the bit-stream */
typedef enum {
  XA_AACDEC_PCE_NOT_FOUND_YET = 0,      /* No PCE found in the stream yet. */
  XA_AACDEC_PCE_NEW           = 1,      /* New PCE found in the current frame. */
  XA_AACDEC_PCE_USE_PREV      = 2       /* No PCE in current frame, using previous PCE. */
} xa_aac_dec_pce_status;

/* MetaData Structure */
typedef struct
{
  UWORD8 bMatrixMixdownIdxPresent;   /* Flag indicating if ucMatrixMixdownIndex & bPseudoSurroundEnable were present in PCE */
  UWORD8 ucMatrixMixdownIndex;       /* 2-bit value selecting the coefficient set for matrix downmix.
                                        Note, ucMatrixMixdownIndex is valid only if bMatrixMixdownIdxPresent = 1 */
  UWORD8 bPseudoSurroundEnable;      /* Flag indicating the possibility of mixdown for pseudo surround reproduction.
                                        Note, bPseudoSurroundEnable is valid only if bMatrixMixdownIdxPresent = 1 */
} xa_aac_dec_dwnmix_metadata_t;

/* Structure for downmix levels present in acnillary data (DSE) */
/*
Where
new_dvb_downmix_data:
     Flag for indicating the presence of new downmixing data
     in the current frame.
     0 - no "new" dvb downmixing data
     1 - dvb downmixing data available

mpeg_audio_type:
     2-bits value indicating mpeg audio type.
     0 - Reserved
     1,2 - MPEG1 and MPEG2 Audio data.
     3 - MPEG4 Audio data.
(Refer Section C.4.2.3 and C.5.2.2.1 of ETSI TS 101 154 V1.9.1 document )

dolby_surround_mode:
     2-bits value indicating dolby surround mode.
     0,3 - Reserved
     1 - 2-ch audio is not dolby surround encoded.
     2 - 2-ch audio is dolby surround encoded.
(Refer Section C.4.2.4 and C.5.2.2.2 of ETSI TS 101 154 V1.9.1 document )

center_mix_level_on:
     Flag for the presence of center_mix_level_value.
     0 or 1 are valid values(Refer to ETSI TS 101 154 V1.9.1)
center_mix_level_value;
     3-bit value for the downmix factor for mixing the center channel
     into the stereo output. Values refer to ETSI TS 101 154 V1.9.1
surround_mix_level_on:
     Flag for the presence of surround_mix_level_value
     0 or 1 are valid values(Refer to ETSI TS 101 154 V1.9.1)
surround_mix_level_value:
     3-bit value for the downmix factor for mixing the left and
     right surrond into the stereo output.
     Values refer to ETSI TS 101 154 V1.9.1
(Refer Section C.4.2.10 and C.5.2.4 of ETSI TS 101 154 V1.9.1 document for the above)

coarse_grain_timecode_on;
fine_grain_timecode_on;
     2 bit flags indicating whether the coarse or fine time codes are present or not.
coarse_grain_timecode_value;
fine_grain_timecode_value;
     14 bit values containing the coarse or fine grain_timecode values.
(Refer Section C.4.2.13 / C.4.2.14 and C.5.2.4 of ETSI TS 101 154 V1.9.1 document )

(Details about timecodes:
Resetting of corse_grain_timecode_value based on (coarse_grain_timecode_on == '10') shall NOT
be done by the library. Same shall be true true of fine_grain_time_code_value.

For MPEG4, if the status bit coarse_grain_timecode_status is 0, then both
coarse_grain_timecode_on;
coarse_grain_timecode_value;
shall be set to 0.
Same shall hold true for fine_grain_timecode values.)

*/

typedef struct {
  UWORD8 new_dvb_downmix_data;
  UWORD8 mpeg_audio_type;
  UWORD8 dolby_surround_mode;
  UWORD8 center_mix_level_on;
  UWORD8 center_mix_level_value;
  UWORD8 surround_mix_level_on;
  UWORD8 surround_mix_level_value;
  UWORD8 coarse_grain_timecode_on;
  UWORD coarse_grain_timecode_value;
  UWORD8 fine_grain_timecode_on;
  UWORD fine_grain_timecode_value;
} xa_aac_dec_dwnmix_level_dvb_info_t;

#define MAX_NUM_CHANNELS   8
#define MAX_NUM_DRC_BANDS    16

typedef struct {
  unsigned char drc_info_valid;
  unsigned char exclude_masks[MAX_NUM_CHANNELS];
  unsigned char drc_bands_present;
  unsigned char drc_interpolation_scheme;
  unsigned char drc_num_bands;
  unsigned char drc_band_incr;
  unsigned char drc_band_top[MAX_NUM_DRC_BANDS];
  unsigned char prog_ref_level_present;
  unsigned char prog_ref_level;
  char dyn_rng_dbx4[MAX_NUM_DRC_BANDS];
} xa_aac_dec_parsed_drc_info_t;


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
DLL_SHARED xa_codec_func_t xa_aac_dec;
DLL_SHARED xa_codec_func_t xa_bsac_dec;
DLL_SHARED xa_codec_func_t xa_dabplus_dec;
#if defined(__cplusplus)
}
#endif  /* __cplusplus */

#endif /* __XA_AAC_DEC_API_H__ */
