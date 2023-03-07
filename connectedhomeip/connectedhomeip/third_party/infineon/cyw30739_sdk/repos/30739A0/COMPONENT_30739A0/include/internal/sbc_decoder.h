/*
 * Copyright 2016-2021, Cypress Semiconductor Corporation (an Infineon company) or
 * an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
 *
 * This software, including source code, documentation and related
 * materials ("Software") is owned by Cypress Semiconductor Corporation
 * or one of its affiliates ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products.  Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */

/******************************************************************************
**
**  File Name:   sbc_plc_bec_tgt.h
**
**  Description: This file defines the SBC Decoder interface
**
**
******************************************************************************/

#ifndef SBC_DECODER_H
#define SBC_DECODER_H



#include "sbc_plc_bec_tgt.h"



#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE (!FALSE)
#endif


/* Set SET_SYNC_ON_ESCO_FAIL to TRUE to enable overwriting the sync field */
/* in the case that the esco CRC fails */
#ifndef SBC_SET_SYNC_ON_ESCO_FAIL
#define SBC_SET_SYNC_ON_ESCO_FAIL TRUE
#endif

/* Set SBC_SET_RESERVED_ON_ESCO_FAIL to TRUE to track the reserved bytes and   */
/* over-write the values with prior received values if it is found that the    */
/* received values never change for correctly received frames (eSCO CRC passes */
#ifndef SBC_SET_RESERVED_ON_ESCO_FAIL
#define SBC_SET_RESERVED_ON_ESCO_FAIL TRUE
#endif

/* Set SBC_PLC to TRUE to enable WB-PLC for 16kHz SBC speech stream */
#ifndef SBC_PLC
#define SBC_PLC TRUE
#endif

/* Set SBC_BEC_1_2 to TRUE to enable BEC-1 and BEC-2 */
#ifndef SBC_BEC_1_2
#define SBC_BEC_1_2 TRUE
#endif

/* Set SBC_HEADER_BEC to TRUE to enable SBC Header BEC (aka BEC-0) */
#ifndef SBC_HEADER_BEC
#define SBC_HEADER_BEC TRUE
#endif

/* Set SBC_NEW_API  to TRUE to enable New API requested by FW team. */
//#ifndef SBC_NEW_API
#define SBC_NEW_API TRUE
//#endif

/* Set SBC_WB to TRUE to enable Wide Band Speech SBC Codec */
#ifndef SBC_WB
#define SBC_WB    TRUE
#endif


/* this  SBC_ARM_OPT flag should not be change, it woudl degrate the quality bellow the sig accptance criteria */
#ifndef SBC_ARM_OPT
#define SBC_ARM_OPT FALSE
#endif

/* this  SBC_ARM_INLINE_ASM it to used only if SBC_ARM_OPT == TRUE. please refer to the above comment */
#ifndef SBC_ARM_INLINE_ASM
#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE (!FALSE)
#endif
#define SBC_ARM_INLINE_ASM FALSE
#endif

//#ifndef SBC_OPTIMIZATION
#define SBC_OPTIMIZATION TRUE
//#endif

/* 32 and 64 bit mult will be performed using SINT64 ( usualy __int64 ) cast that usualy give optimal performance if supported */
#ifndef SBC_IPAQ_OPT
#define SBC_IPAQ_OPT TRUE
#endif

/* TRUE to perform the dequantification 32x32, 16x32 operation in 64 bit, false to perform those in 32 bit */
#ifndef SBC_IS_64_BITS_IN_DEQUANTIZER
#define SBC_IS_64_BITS_IN_DEQUANTIZER TRUE
#endif

/* TRUE to perform the windowing MAC 32x32, 16x32 operation in 64 bit, false to perform those in 32 bit */
#ifndef SBC_IS_64_BITS_IN_WINDOWING
#define SBC_IS_64_BITS_IN_WINDOWING FALSE
#endif

/* TRUE to hold the VX vector in 16 bit, FALSE for 32 bit */
#ifndef SBC_VECTOR_VX_ON_16BITS
#define SBC_VECTOR_VX_ON_16BITS FALSE
#endif

/* TRUE to perform the DCT 32x32, 16x32 operation in 64 bit, false to perform those in 32 bit */
#ifndef SBC_IS_64_MULT_IN_DCT
#define SBC_IS_64_MULT_IN_DCT TRUE
#endif

/* 32 and 64 bit mult will be performed using SINT64 ( long long ) cast that usualy give optimal performance if supported */
#ifndef SBC_FOR_EMBEDDED_LINUX
#define SBC_FOR_EMBEDDED_LINUX FALSE
#endif

#ifndef SBC_GKI_BUFFERBASED
#define SBC_GKI_BUFFERBASED FALSE
#endif /* SBC_GKI_BUFFERBASED */

#ifndef SBC_MINIMUM_BUFFER_COPIES
#define SBC_MINIMUM_BUFFER_COPIES FALSE
#endif

/* this SBC_DEC_SANITY_CHECK verify that we have enough byte to decode before starting the decoding */
/* this is sometime necessary when the applicaiton is not able to provide the data require when they call sbc decoder */
#ifndef SBC_DEC_SANITY_CHECK
#define SBC_DEC_SANITY_CHECK FALSE
#endif

/* exclude/include 4 subband code */
#ifndef SBC_DEC_4_SUBD_INCLUDED
#define SBC_DEC_4_SUBD_INCLUDED TRUE
#endif
/* exclude/include 8 subband code */
#ifndef SBC_DEC_8_SUBD_INCLUDED
#define SBC_DEC_8_SUBD_INCLUDED TRUE
#endif
/* exclude/include stereo code */
#ifndef SBC_DEC_STEREO_INCLUDED
#define SBC_DEC_STEREO_INCLUDED TRUE
#endif
/* exclude/include MONO A2DP code */
#ifndef SBC_DEC_MONO_A2DP_INCLUDED
#define SBC_DEC_MONO_A2DP_INCLUDED TRUE
#endif
/* exclude/include ATTENUATION code */
/* Enable this flag to have the SBC output attenuation*/
#ifndef SBC_ATTENUATION_INCLUDED
#define SBC_ATTENUATION_INCLUDED TRUE
#endif
/* exclude/include MIXING code */
/* Enable this flag to have the SBC output Mixed with Narrow Band (8kHz) PCM input*/
#ifndef SBC_MIXING_INCLUDED
#define SBC_MIXING_INCLUDED TRUE
#endif

/* exclude/include Pointer verification code */
/* Enable this flag check pointer ( more MHz )*/
#ifndef SBC_CHECK_POINTER
#define SBC_CHECK_POINTER TRUE
#endif


/* exclude/include Stereo 32 kHz downsampling code */
/* Enable this flag to have the SBC Stereo 32kHz always dowsampled to 16kHz.  This is mainly for 20748 where DAC does not support 32 kHz Frequency sampling */
#ifndef SBC_STEREO_32K_DOWNSAMPLED_16K
#define SBC_STEREO_32K_DOWNSAMPLED_16K FALSE
#endif

/* exclude/include Sample Rate Converter for 2074 A2 only */
/* Enable this flag to have the SBC PCM output dowsampled to 16kHz.  This is only for for 2074 A2 where only DAC does support Frequency sampling up to 16 kHz only */
#ifndef SBC_SAMPLE_RATE_CONVERTER_16K
#define SBC_SAMPLE_RATE_CONVERTER_16K FALSE
#endif

/* exclude/include FIR Filter Convolution */
/* Enable this flag to have the FIR Filter convolution function,. This is used in 20748 to combine  two set of FIR Coefficients */
#ifndef SBC_CONVOLUTION_FILTER_INCLUDED
#define SBC_CONVOLUTION_FILTER_INCLUDED FALSE
#endif

/* Enable this flag to support WBS for BTW */
#ifndef SBC_DEC_BTW
#define SBC_DEC_BTW FALSE
#endif

#include "sbc_types.h"
#include "sbc_common.h"

#if (SBC_WB)
#if (SB_BUFFER_SIZE_WB >SB_BUFFER_SIZE_SIG)
#define SB_BUFFER_SIZE SB_BUFFER_SIZE_WB
#else
#define SB_BUFFER_SIZE SB_BUFFER_SIZE_SIG
#endif
#define SB_BUFFER_SIZE SB_BUFFER_SIZE_SIG
#endif

/* up/down sampling ratio for Mono A2DP */
#define SBC_sf32000_DS 2
#define SBC_sf44100_DS 11
#define SBC_sf44100_US 4
#define SBC_sf48000_DS 3

/* Mono A2DP */
#define MONO_A2DP_FILTER_LEN 96

#define PCM_BUF 0
#define SBC_BUF 0


/* NB Mixing with A2DP */
#define NB_UP_SAMPLING_FILTER_LEN_48K 32
#define NB_UP_SAMPLING_FILTER_LEN_44K 96
#define NB_UP_SAMPLING_FILTER_LEN_32K 32
#define NB_UP_SAMPLING_FILTER_LEN_16K 32
#define NB_UP_SAMPLING_MAX_FILTER_LEN NB_UP_SAMPLING_FILTER_LEN_44K
#define NB_UP_SAMPLING_FILTER_LEN NB_UP_SAMPLING_MAX_FILTER_LEN
#define NB_UP_SAMPLING_NB_SAMPLE_BUF 128    // 16 ms of NB PCM Buffering
#define NB_UP_SAMPLING_BUF_LEN (NB_UP_SAMPLING_FILTER_LEN+NB_UP_SAMPLING_NB_SAMPLE_BUF) // 16 ms of NB PCM Buffering
#define NB_UP_SAMPLING_START_LEVEL (NB_UP_SAMPLING_NB_SAMPLE_BUF>>1)    // Start upsampling and mixing when NB PCM buffer level is half filled
#define NB_UP_SAMPLED_BUF_LEN (NB_UP_SAMPLING_NB_SAMPLE_BUF*6)    // PCM Buffering for 48 kHz

/* Upsampling filters  */
typedef struct SBC_FILTER_TAG
{
    const SINT16 * h;
    SINT16 filter_len;
    SINT16 up_ratio;
    SINT16 dw_ratio;
}SBC_FILTER_PARAM;

/* sbc_status bit field */
#define SBC_STATUS_RESET_REQ                (1<<0)        // To indicated that SBC will be resetted on next good packet (Init procedure)
//#define SBC_STATUS_WAIT_FOR_FIRST_EV3    (1<<1)        // To indicate we are waiting for the first EV3 packet of a new SBC packet
//#define SBC_STATUS_EV3_LOST                (1<<2)        // To indicate that at least 1 EV3 packet has been lost
#define SBC_STATUS_RESERVED_PASS            (1<<3)        // To indicate that reserved bytes are not changing on remote side
#define SBC_STATUS_MIXING_ENABLED        (1<<4)        // To indicate that NB Mixing is enabled (set when NB PCM buffer level is high enouh to start upsampling and mixing)
#define SBC_STATUS_FORCE_MONO            (1<<5)        // For Mono Mode in Stereo A2DP (low MHz mode).
#define SBC_STATUS_SUSPEND_WB_PLC_BEC    (1<<6)        // To suspend WB PLC and BEC.
#define SBC_STATUS_SUSPEND2_WB_PLC_BEC    (1<<7)        // To suspend WB PLC and BEC (Second entry).
#define SBC_STATUS_PLC_CALLED                (1<<8)        // To indicated PLC was called on last SBC_Decoder call (for Fw purpose).
#define SBC_STATUS_SYNC_NOT_FOUND        (1<<9)        // To indicate that SBC Sync has not been found

#define SBC_STATUS_SUSPEND_MASK            (SBC_STATUS_SUSPEND_WB_PLC_BEC | SBC_STATUS_SUSPEND2_WB_PLC_BEC ) // Suspend WB PLC Mask


/* sbc_option bit field */
#define SBC_OPTION_PLC_MODE_MASK                    (0x07)    // SBC PLC MODE
//#define SBC_OPTION_DYNAMIC_UUI                    (1<<3)    // For SBC WB: Enable Dynamic UUI for variable bit-rate
#define SBC_OPTION_ESCO_CRC_ERR_AS_LOST             (1<<4)    // For PLC: Process eSCO CRC error frame as Lost frame. Default case is process eSCO CRC Error as Good SBC frame
#define SBC_OPTION_DISABLE_HEADER_BEC1             (1<<5)    // For Header BEC1:  Default setting is enabled
#define SBC_OPTION_DISABLE_BEC1_2                 (1<<6)    // For BEC1 and BEC2: Disable BEC1 and BEC2. Default setting is enabled
#define SBC_OPTION_DISABLE_BEC2_BURST_PROTECT     (1<<7)    // For BEC2: Disable Burst Error Protection if short on MHz. Default is enabled
#define SBC_OPTION_DISABLE_SYNC_OVERWRITE         (1<<8)    // For WideBand: Overwrite SBC Sync byte, since the SBC frame synchronizatio is done by H2 Header. Default is enabled
#define SBC_OPTION_DISABLE_RESERVED_OVERWRITE      (1<<9)    // For WideBand: Overwrite Reserved 16 bits, since thsi field is not supposed to be used. Default is enabled
//#define SBC_OPTION_ENABLE_EV3                         (1<<10)    // For WideBand: 1: Enabled EV3 Packet Type. 0: Default Packet Type (2EV3)
#define SBC_OPTION_DISABLE_SAMPLE_RATE_CONV        (1<<11)    // For Mono-A2DP: 1: Disable 16 kHz Sample Rate Converter for Mono A2DP. Default setting is Enabled
#define SBC_OPTION_ENABLE_ST_32K_SRC                (1<<12)    // Enable Sample Rate Converter for all A2DP streams @ 32 kHz. Will be downsampled to 16 kHz


/* For SBC_OPTION_PLC_MODE */
enum
{
    SBC_PLC_DISABLED,
    SBC_PLC_ENABLED,
    SBC_PLC_TBD2,
    SBC_PLC_TBD3,
    SBC_PLC_TBD4,
    SBC_PLC_TBD5,
    SBC_PLC_TBD6,
    SBC_PLC_TBD7
};

/* For SBC PLC WB frame_status */
enum
{
    SBC_GOOD_ESCO_CRC,
    SBC_BAD_ESCO_CRC,
    SBC_LOST
};

#define SBC_FRAME_TYPE_MASK 0x3

#define SBC_3_FRAME_TYPE_MASK   ((SBC_FRAME_TYPE_MASK<<16) | (SBC_FRAME_TYPE_MASK<<8) |SBC_FRAME_TYPE_MASK)

#define SBC_1_AND_2_FRAME_TYPE_MASK   ( (SBC_FRAME_TYPE_MASK<<8) | SBC_FRAME_TYPE_MASK )
#define SBC_1_AND_3_FRAME_TYPE_MASK   ( (SBC_FRAME_TYPE_MASK<<16) | SBC_FRAME_TYPE_MASK)
#define SBC_2_AND_3_FRAME_TYPE_MASK   ( (SBC_FRAME_TYPE_MASK<<16) | (SBC_FRAME_TYPE_MASK<<8) )



/* For sbc_att */
enum
{
    SBC_ATT_OFF = 0,        // Attenuation is disabled
    SBC_ATT_6DB,
    SBC_ATT_12DB,
    SBC_ATT_18DB,            // Attenuation of 18 dB
    SBC_ATT_24DB,
    SBC_ATT_30DB,
    SBC_ATT_36DB,
    SBC_ATT_INF,            // Mute
    NB_SBC_ATT_STATE
};



#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE (!FALSE)
#endif


#define MSBC_LEN                  57
#define SBC_2EV3_LEN            60    // 2EV3 packet length

#define SBC_BUF_LEN         (3*SBC_2EV3_LEN) // SBC Buffer length

#ifndef SBC_MAX_PACKET_LENGTH
#define SBC_MAX_PACKET_LENGTH 520
#endif

#define NB_STATUS_FLAG_BLOCK 4            // status_flag for SBC buffer

/*constants used for index calculation*/
#define CHL    SBC_MAX_NUM_OF_SUBBANDS
#define BLK    (SBC_MAX_NUM_OF_CHANNELS*CHL)

#define DEC_VX_MINIMUM_BUFFER_SIZE (SBC_MAX_NUM_OF_SUBBANDS*SBC_MAX_NUM_OF_CHANNELS*20)




/* Memory allocation  */
#if (SBC_PLC)
#include "typedef.h"
#include "lcplc_wb.h"
#endif

#ifndef DEC_VX_BUFFER_SIZE
#define DEC_VX_BUFFER_SIZE (DEC_VX_MINIMUM_BUFFER_SIZE+(20*SBC_SUB_BANDS_8))
#endif


#if (SBC_VECTOR_VX_ON_16BITS==TRUE)
#define SBC_STATIC_MEM_SIZE (sizeof(SINT32)*(DEC_VX_BUFFER_SIZE/2 + (SBC_BUF)))    // in Bytes
#else
#define SBC_STATIC_MEM_SIZE (sizeof(SINT32)*(DEC_VX_BUFFER_SIZE+ (SBC_BUF)))
#endif

#if (SBC_ARM_OPT==TRUE)
#define SBC_SCRATCH_MEM_SIZE (4*(240+128+PCM_BUF))    // in Bytes
#else
#define SBC_SCRATCH_MEM_SIZE (4*(240+256+PCM_BUF))
#endif

#if ((SBC_PLC == TRUE) && (LC_PLC_SCRATCH_MEM_SIZE > SBC_SCRATCH_MEM_SIZE) )
#error PLC Scratch Memory will exceed SBC Scratch Memory !!
#endif



typedef struct SBC_DEC_STATUS_FLAG_TAG
{
    SINT16 status_flag;
    SINT16 len;
} SBC_DEC_STATUS_FLAG_PARAMS;




typedef struct SBC_DEC_PARAMS_TAG
{
    SINT16 samplingFreq;            /*16k, 32k, 44.1k or 48k*/
    SINT16 channelMode;            /*mono, dual, streo or joint streo*/
    SINT16 numOfSubBands;        /*4 or 8*/
    SINT16 numOfChannels;
    SINT16 numOfBlocks;            /*4, 8, 12 or 16*/
    SINT16 allocationMethod;        /*loudness or SNR*/
    SINT16 bitPool;                /* 16*numOfSb for mono & dual; 32*numOfSb for stereo & joint stereo */
    SINT16 scaleFactor[SBC_MAX_NUM_OF_CHANNELS*SBC_MAX_NUM_OF_SUBBANDS];
    SINT16 ScratchMemForBitAlloc[SBC_MAX_NUM_OF_CHANNELS*SBC_MAX_NUM_OF_SUBBANDS];
    SINT16 bits[SBC_MAX_NUM_OF_CHANNELS*SBC_MAX_NUM_OF_SUBBANDS];
    UINT8  *packet;

    SINT32 *s32ScratchMem;
    SINT32 *s32StaticMem;
    SINT16 *pcmBuffer;
#if (SBC_ARM_OPT==TRUE)
    SINT16 *sbBuffer;
#else
    SINT32 *sbBuffer;
#endif
#if (SBC_ARM_OPT==FALSE && SBC_IPAQ_OPT==FALSE && SBC_OPTIMIZATION==FALSE)
    SINT32 *scartchMemForFilter;
#endif
#if (SBC_VECTOR_VX_ON_16BITS==TRUE)
    SINT16 *VX;
#else
    SINT32 *VX;
#endif
    SINT32 ShiftCounter[2];
    SINT32 DecMaxShiftCounter;

    UINT8 join[SBC_MAX_NUM_OF_SUBBANDS];       /*0 if not joint stereo*/
    UINT16 u16PrevPacLen;
    UINT16 frame_len;                /* frame length. When multiple frames are provided, it still refers to one frame length */
    UINT16 nb_frame;                /* Number of frames provided by Fw */
    UINT16 u16ReaminingBytes;
    UINT16 sbc_status;                /* For internal state machine */
    UINT16 sbc_option;
#if SBC_ATTENUATION_INCLUDED
    UINT16 sbc_att;                /* SBC output attenuation level */
    UINT16 sbc_curr_att;             /* SBC output current attenuation. Used for Fade-in/out */
#endif
#if (SBC_WB)
    UINT16 sbc_mode;                /*0: A2DP mode, 1: WideBand mode, 2: Mono-A2DP mode */
#endif
#if SBC_MIXING_INCLUDED

/* nb_buf */
/*-------------------------------------- ---------------------------------------------------
|      NB_UP_SAMPLING_FILTER_LEN             |                  NB_UP_SAMPLING_NB_SAMPLE_BUF        |
  --------------------------------------- ---------------------------------------------------*/
    SINT16 nb_buf[NB_UP_SAMPLING_BUF_LEN];    /* Narrow Band  buffer */
    UINT16 nb_len;                            /* number of Samples in Narrow Band  buffer */
/* up_buf */
/*----------------------------------------------------------
|       NB_UP_SAMPLED_BUF_LEN                                |
  ----------------------------------------------------------*/
    SINT16 up_buf[NB_UP_SAMPLED_BUF_LEN];    /* Up sampled buffer */
    UINT16 up_len;                            /* number of Samples in Up sampled buffer */
#endif
#if SBC_DEC_MONO_A2DP_INCLUDED /* filter memory */
#if ( SBC_SAMPLE_RATE_CONVERTER_16K==TRUE)
    SINT16 ds_filter[MONO_A2DP_FILTER_LEN];
#endif
    UINT8 ds_idx;                /* Last downsampling index*/
#endif
#if (SBC_PLC)
    struct LCPLC_State * plc_state;    /* PLC struct */
    UINT8 frame_status[SBC_MAX_NUM_OF_ESCO_FRAME];            /* Frame status: GOOD_ESCO_CRC, BAD_ESCO_CRC, LOST for the different SBC frame*/
    UINT16 good_pkt_len;                                        /* SBC Packet Len of good frame */
    SINT16 plc_tuning[5];                                        /* PLC tuning parametsr that can be updated between SBC_Decoder_decode_Init and first SBC_Decoder_decode call */
#if SBC_SET_RESERVED_ON_ESCO_FAIL
    UINT8   reserved[2];
#endif
#endif
    UINT8 sbc_buf[2][SBC_BUF_LEN];        /* Internal SBC buffer. Keep only 2 different frames for BEC1. BEC2 is run on the fly and lead to one single frame (GOOD or LOST) */
    SINT32 rd_buf;                        /* Internal SBC buffer read pointer */
    SINT32 wr_buf;                        /* Internal SBC buffer write pointer */
    SBC_DEC_STATUS_FLAG_PARAMS status_flag[NB_STATUS_FLAG_BLOCK];

}SBC_DEC_PARAMS;

/*FUNCTION DECLARATIONS*/
extern const SINT16 DCTcoeff4[];
extern const SINT16 DCTcoeff8[];


#ifdef __cplusplus
extern "C" {
#endif


void SbcSynthesisFilter(SBC_DEC_PARAMS *);


extern SINT16 SBC_Decoder_decode_Init(SBC_DEC_PARAMS *strDecParams);
extern SINT16 SBC_Decoder_decoder(SBC_DEC_PARAMS *strDecParams, UINT8 * sbc_in, UINT32 len, SINT16 * pcm_out);

extern SINT16 SBC_Decoder(SBC_DEC_PARAMS *strDecParams, UINT8 * sbc_in, SINT16 * pcm_out, UINT32 len);
extern SINT16 SBC_Decoder_PLC_Init(SBC_DEC_PARAMS *strDecParams);
extern SINT16 SBC_Decoder_ReInit(SBC_DEC_PARAMS *strDecParams, unsigned char * sbc_in, unsigned int len);
extern void SBC_DecoderZIR(SBC_DEC_PARAMS *strDecParams, short * pcm_out);
extern SINT16 SBC_PLC_called(SBC_DEC_PARAMS *strDecParams);
extern UINT8 * SBC_Decoder_get_pt(SBC_DEC_PARAMS *strDecParams, SINT32 nb_bytes);
extern SINT16 SBC_Decoder_get_delay(SBC_DEC_PARAMS *strDecParams, UINT32 * delay_samples, UINT32 * delay_ms);

extern void SbcSynthesisFilter(SBC_DEC_PARAMS *strDecParams);
extern SINT16 DecUnPacking(SBC_DEC_PARAMS *strDecParams);
extern void SbcSynthesisFilter4sb(SBC_DEC_PARAMS *strDecParams);
extern void SbcSynthesisFilter8sb(SBC_DEC_PARAMS *strDecParams);
extern void Mult64(SINT32 s32In1, SINT32 s32In2, SINT32 *s32OutLow, SINT32 *s32OutHi);

#if ( SBC_DEC_MONO_A2DP_INCLUDED ==TRUE)
extern SINT16 SBC_sample_rate_conv(SBC_DEC_PARAMS *strDecParams);
#endif

#if SBC_ATTENUATION_INCLUDED
UINT16 SBC_att_shift(SBC_DEC_PARAMS *strDecParams);
#endif

#if SBC_MIXING_INCLUDED
extern void SBC_mixing_update_nb_buff(SBC_DEC_PARAMS *strDecParams, UINT16 nb_samples);
extern void SBC_handle_upsampling(SBC_DEC_PARAMS *strDecParams);
extern SINT16 * SBC_mixing_get_pt(SBC_DEC_PARAMS *strDecParams, SINT32 nb_samples);
extern void SBC_mixing_reset_nb_buffer(SBC_DEC_PARAMS *strDecParams);
extern SINT32 SBC_upsampling(SBC_DEC_PARAMS *strDecParams, SINT16 * pcm_in, SINT16 * pcm_out, SINT32 frame_size);
extern void SBC_mixing_reset_up_buffer(SBC_DEC_PARAMS *strDecParams);
extern SINT16 SBC_mixing_fill_up_buffer(SBC_DEC_PARAMS *strDecParams, SINT16 * pcm_in, SINT32 nb_samples);
extern SINT16 SBC_mixing_get_up_buffer_size (SBC_DEC_PARAMS *strDecParams);
extern SINT16 * SBC_mixing_get_up_pt(SBC_DEC_PARAMS *strDecParams, SINT32 nb_samples);
extern void SBC_mixing_update_up_buff(SBC_DEC_PARAMS *strDecParams, UINT16 nb_samples);
#endif

SINT16 DecUnPacking(SBC_DEC_PARAMS *strDecParams);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
