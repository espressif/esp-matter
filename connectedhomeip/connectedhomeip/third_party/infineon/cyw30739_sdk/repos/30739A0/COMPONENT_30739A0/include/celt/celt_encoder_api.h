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
**  File Name:   celt_encoder_api.h
**
**  Description: This file has function declarations and structs for integration
**
**  Revision :
**               2015/11/26  hochangw
**
******************************************************************************/

#ifndef CELT_ENCODER_API_H
#define CELT_ENCODER_API_H

#include "celt_scratch.h"

/******************************************************************************
 *
 *  struct CELT_ENC_PARAMS
 *
 *****************************************************************************/

//struct CELT_ENC_PARAMS_TAG
typedef struct
{
    UINT32 celt_enc_scratch_buf[OPUS_CELT_ENC_SCRATCH_SIZE_UINT32];   /* decoder scratch buffer */
    UINT32 opus_celt_enc_st_buf[OPUS_CELT_ENC_ST_SIZE];

    INT32 sampling_rate;        /* 8k, 16k, 24k or 48k */
    INT16 channels;             /* 1 or 2 */
    INT32 bitrate;              /* bits per second */
    INT16 complexity;           /* 0 - 10 */
    INT16 use_vbr;              /* variable bit rate, 0:cbr, 1:vbr */
    INT16 use_cvbr;             /* constrained vbr, 0: enable, 1:disable */
    INT32 frame_size;           /* PCM samples of input per channel */
    INT16 *pcmBuffer;           /* Encoder input buffer */
    UINT8 *packet;              /* Encoder output buffer */

    /* Below variables are used inside CELT */
    void  *enc_handler;         /* OpusEncoder */

}CELT_ENC_PARAMS;
//typedef struct CELT_ENC_PARAMS_TAG CELT_ENC_PARAMS


/******************************************************************************
 *
 *  Function: CELT_Encoder
 *
 *  Abstract:    Read PCM samples, run the CEL Encoder and return the number of encoded samples
 *
 *  Param:
 *           * ptrEncParams  :   Pointer to CELT_ENC_PARAMS structure
 *
 *  Return:  Packet length (bytes)
 *
 *******************************************************************************/
 extern INT32 CELT_Encoder(CELT_ENC_PARAMS *ptr_enc_param);


 /******************************************************************************
 *
 *  Function: CELT_Encoder_Init
 *
 *  Abstract: InitCELT Encoder
 *
 *  Param:
 *           * ptr_enc_param  : Pointer to CELT_ENC_PARAMS structure
 *
 *  Return:  0 if success
 *
 *******************************************************************************/
 extern INT32 CELT_Encoder_Init(CELT_ENC_PARAMS *ptr_enc_param);


 /******************************************************************************
 *
 *  Function: CELT_Encoder_get_mem_size
 *
 *  Abstract:
 *
 *  Param:
 *           None
 *
 *  Return:  Needed memory size for CELT encoder
 *
 *******************************************************************************/
 extern UINT32 CELT_Encoder_get_mem_size(void);


/******************************************************************************
*
*  Function: CELT_Encoder_get_frame_len
*
*  Abstract:
*
*  Param:   ptr_enc_param->bitrate, ptr_enc_param->sampling_rate, ptr_enc_param->frame_size
*
*
*  Return:  Needed buffer size per frame for CELT encoder output
*
*******************************************************************************/
 extern UINT32 CELT_Encoder_get_frame_len(CELT_ENC_PARAMS *ptr_enc_param);


#endif
