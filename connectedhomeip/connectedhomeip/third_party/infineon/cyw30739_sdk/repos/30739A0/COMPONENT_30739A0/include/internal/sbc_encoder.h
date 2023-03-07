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
**  File Name:   $RCSfile: sbc_encoder.h,v $
**
**  Description: This file contains constants and structures used by Encoder.
**
**  Revision :   $Id: sbc_encoder.h,v 1.28 2006/06/27 12:29:32 mjougit Exp $
**
**
******************************************************************************/

#ifndef SBC_ENCODER_H
#define SBC_ENCODER_H


#include "sbc_plc_bec_tgt.h"



/*DEFINES*/
#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE (!FALSE)
#endif


/* Set SBC_WB to TRUE to enable Wide Band Speech SBC Codec */
#ifndef SBC_WB
#define SBC_WB	TRUE
#endif

#ifndef SBC_MAX_NUM_FRAME
#define SBC_MAX_NUM_FRAME 1
#endif

/* Set SBC_NEW_API  to TRUE to enable New API requested by FW team. SBC_NO_PCM_CPY_OPTION should be set to TRUE  */
#ifndef SBC_NEW_API
#define SBC_NEW_API TRUE
#endif

#ifndef SBC_MAX_PACKET_LENGTH
#define SBC_MAX_PACKET_LENGTH 520
#endif

#ifndef SBC_DSP_OPT
#define SBC_DSP_OPT FALSE
#endif

/* Set SBC_USE_ARM_PRAGMA to TRUE to use "#pragma arm section zidata" */
#ifndef SBC_USE_ARM_PRAGMA
#define SBC_USE_ARM_PRAGMA FALSE
#endif

/* Set SBC_ARM_ASM_OPT to TRUE in case the target is an ARM */
/* this will replace all the 32 and 64 bit mult by in line assembly code */
#ifndef SBC_ARM_ASM_OPT
#define SBC_ARM_ASM_OPT FALSE
#endif

/* green hill compiler option -> Used to distinguish the syntax for inline assembly code*/
#ifndef SBC_GHS_COMPILER
#define SBC_GHS_COMPILER FALSE
#endif

/* ARM compiler option -> Used to distinguish the syntax for inline assembly code */
#ifndef SBC_ARM_COMPILER
#define SBC_ARM_COMPILER TRUE
#endif

/* Set SBC_IPAQ_OPT to TRUE in case the target is an ARM */
/* 32 and 64 bit mult will be performed using SINT64 ( usualy __int64 ) cast that usualy give optimal performance if supported */
#ifndef SBC_IPAQ_OPT
#define SBC_IPAQ_OPT TRUE
#endif

/* Debug only: set SBC_IS_64_MULT_IN_WINDOW_ACCU to TRUE to use 64 bit multiplication in the windowing */
/* -> not recomended, more MIPS for the same restitution.  */
#ifndef SBC_IS_64_MULT_IN_WINDOW_ACCU
#define SBC_IS_64_MULT_IN_WINDOW_ACCU  FALSE
#endif /*SBC_IS_64_MULT_IN_WINDOW_ACCU */

/* Set SBC_IS_64_MULT_IN_IDCT to TRUE to use 64 bits multiplication in the DCT of Matrixing */
/* -> more MIPS required for a better audio quality. comparasion with the SIG utilities shows a division by 10 of the RMS */
/* CAUTION: It only apply in the if SBC_FAST_DCT is set to TRUE */
#ifndef SBC_IS_64_MULT_IN_IDCT
#define SBC_IS_64_MULT_IN_IDCT  FALSE
#endif /*SBC_IS_64_MULT_IN_IDCT */

/* set SBC_IS_64_MULT_IN_QUANTIZER to TRUE to use 64 bits multiplication in the quantizer */
/* setting this flag to FALSE add whistling noise at 5.5 and 11 KHz usualy not perceptible by human's hears. */
#ifndef SBC_IS_64_MULT_IN_QUANTIZER
#define SBC_IS_64_MULT_IN_QUANTIZER  TRUE
#endif /*SBC_IS_64_MULT_IN_IDCT */

/* Debug only: set this flag to FALSE to disable fast DCT algorithm */
#ifndef SBC_FAST_DCT
#define SBC_FAST_DCT  TRUE
#endif /*SBC_FAST_DCT */

/* In case we do not use joint stereo mode the flag save some RAM and ROM in case it is set to FALSE */
#ifndef SBC_JOINT_STE_INCLUDED
#define SBC_JOINT_STE_INCLUDED TRUE
#endif


/* exclude/include 4 subband code */
#ifndef SBC_ENC_4_SUBD_INCLUDED
#define SBC_ENC_4_SUBD_INCLUDED TRUE
#endif


/* TRUE -> application should provide PCM buffer, FALSE PCM buffer reside in SBC_ENC_PARAMS */
#ifndef SBC_NO_PCM_CPY_OPTION
#define SBC_NO_PCM_CPY_OPTION TRUE
#endif

/* exclude/include stereo code */
#ifndef SBC_ENC_STEREO_INCLUDED
#define SBC_ENC_STEREO_INCLUDED TRUE
#endif


#define MINIMUM_ENC_VX_BUFFER_SIZE (8*10*2)
#ifndef ENC_VX_BUFFER_SIZE
#define ENC_VX_BUFFER_SIZE (MINIMUM_ENC_VX_BUFFER_SIZE + 64)
/*#define ENC_VX_BUFFER_SIZE MINIMUM_ENC_VX_BUFFER_SIZE + 1024*/
#endif

/* Defines the Encoder PCM Buff size */
#define SBC_ENCODER_BTW_PCM_BUFF 480


#define DCT_BUF_SIZE 16



#include "sbc_types.h"
#include "sbc_common.h"


/*constants used for index calculation*/
//#define SBC_BLK (SBC_MAX_NUM_OF_CHANNELS * SBC_MAX_NUM_OF_SUBBANDS)


#if (SBC_WB)
#if (SB_BUFFER_SIZE_WB >SB_BUFFER_SIZE_SIG)
#define SB_BUFFER_SIZE SB_BUFFER_SIZE_WB
#else
#define SB_BUFFER_SIZE SB_BUFFER_SIZE_SIG
#endif
#define SB_BUFFER_SIZE SB_BUFFER_SIZE_SIG
#endif




typedef struct SBC_ENC_PARAMS_TAG
{
// -----------------------------------------------------------------
    SINT16 samplingFreq;                         /* 16k, 32k, 44.1k or 48k*/
    SINT16 channelMode;                          /* mono, dual, streo or joint streo*/
    SINT16 numOfSubBands;                        /* 4 or 8 */
    SINT16 numOfChannels;
    SINT16 numOfBlocks;                          /* 4, 8, 12 or 16*/
    SINT16 allocationMethod;                     /* loudness or SNR*/
    SINT16 bitPool;                              /* 16*numOfSb for mono & dual
                                                       32*numOfSb for stereo & joint stereo */
    SINT16 scaleFactor[SBC_MAX_NUM_OF_CHANNELS*SBC_MAX_NUM_OF_SUBBANDS];
    SINT16  ScratchMemForBitAlloc[SBC_MAX_NUM_OF_CHANNELS*SBC_MAX_NUM_OF_SUBBANDS];
    SINT16 bits[SBC_MAX_NUM_OF_CHANNELS*SBC_MAX_NUM_OF_SUBBANDS];
    UINT8  *packet;

// Keep identical to SBC_COM_PARAMS_TAG
// -----------------------------------------------------------------

    SINT32 StaticMem[ DCT_BUF_SIZE + ENC_VX_BUFFER_SIZE/2 ];

    UINT16 u16BitRate;
    UINT8   u8NumPacketToEncode;                    /* number of sbc frame to encode. Default is 1 */
#if (SBC_JOINT_STE_INCLUDED == TRUE)
	SINT16 as16Join[SBC_MAX_NUM_OF_SUBBANDS];       /* 1 if JS, 0 otherwise*/
	SINT32	 s32LRDiff[SBC_MAX_NUM_OF_BLOCKS];
	SINT32	 s32LRSum[SBC_MAX_NUM_OF_BLOCKS];
#endif


    SINT16 *ps16NextPcmBuffer;
#if (SBC_NO_PCM_CPY_OPTION == TRUE)
    SINT16 *ps16PcmBuffer;
#else
    SINT16 as16PcmBuffer[SBC_MAX_NUM_FRAME*SBC_MAX_NUM_OF_BLOCKS * SBC_MAX_NUM_OF_CHANNELS * SBC_MAX_NUM_OF_SUBBANDS];
#endif


    SINT32  s32SbBuffer[SB_BUFFER_SIZE];

#if (SBC_ENC_BTW==TRUE)
	SINT16 pcm_buff[SBC_ENCODER_BTW_PCM_BUFF];
	SINT32 buff_len;
#endif


    UINT8  *pu8Packet;
    UINT16 FrameHeader;
    UINT16 u16PacketLength;
#if (SBC_WB)
    UINT16 sbc_mode;                                /*0: A2DP mode, 1: WideBand mode */
#endif

}SBC_ENC_PARAMS;


#ifdef __cplusplus
extern "C" {
#endif


#if 0
extern void SBC_Encoder(SBC_ENC_PARAMS *strEncParams);
extern SINT16 SBC_Enc_get_PCM_size (SBC_ENC_PARAMS *pstrEncParams);
extern void SbcAnalysisInit (SBC_ENC_PARAMS *pstrEncParams);
extern void SbcAnalysisFilter4(SBC_ENC_PARAMS *strEncParams);
extern void SbcAnalysisFilter8(SBC_ENC_PARAMS *strEncParams);
extern void SBC_FastIDCT8 (SINT32 *pInVect, SINT32 *pOutVect);
extern void SBC_FastIDCT4 (SINT32 *x0, SINT32 *pOutVect);
extern void EncPacking(SBC_ENC_PARAMS *strEncParams);
extern void EncQuantizer(SBC_ENC_PARAMS *);
#endif

extern SINT16 SBC_Encoder_Init(SBC_ENC_PARAMS *strEncParams);
extern UINT16 SBC_Encoder_encode(SBC_ENC_PARAMS *pstrEncParams, SINT16  * pcm_in, UINT8 * sbc_out, UINT16 len);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
