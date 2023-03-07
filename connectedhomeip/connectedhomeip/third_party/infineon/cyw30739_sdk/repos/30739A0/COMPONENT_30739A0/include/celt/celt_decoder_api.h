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
**  File Name:   celt_decoder_api.h
**
**  Description: This file has function declarations and structs for integration
**
**  Revision :
**               2015/11/26  hochangw
**
******************************************************************************/

#ifndef CELT_DECODER_API_H
#define CELT_DECODER_API_H

#include "celt_scratch.h"


/******************************************************************************
 *
 *  struct CELT_DEC_PARAMS
 *
 *****************************************************************************/

//struct CELT_DEC_PARAMS_TAG
typedef struct
{
    UINT32 celt_dec_scratch_buf[OPUS_CELT_DEC_SCRATCH_SIZE_UINT32];   /* decoder scratch buffer */

//#if defined (USE_AUDIOPLC)
    UINT32 aplc_struct[OPUS_CELT_PLC_STRUCT_SIZE_BYTE/4];  //struct CELT_AUDIOPLC_STRUCT aplc;   /* LCPLC for CELT */
//#endif
    INT32 sampling_rate;                /* 8k, 16k, 24k or 48k */
    INT16 channels;                     /* mono or streo */
    UINT16 pkt_len;                     /* Input packet length (bytes) */
    INT16 *pcmBuffer;                   /* Pointer of output buffer */
    UINT8 *packet;                      /* Pointer of input buffer */

    UINT16 frame_status;                /* Frame status: 0:GOOD, 1:BAD(lost)  */
    INT32  frame_size;                  /* PCM samples per frame per channel, needed for PLC init*/
    INT16  enable_plc;                  /* Enable PLC: 1:enable, 0:disable */
    /* Below variables are used inside CELT */
    void  *dec_handler;                 /* OpusDecoder */

}CELT_DEC_PARAMS;
//typedef struct CELT_DEC_PARAMS_TAG CELT_DEC_PARAMS


/******************************************************************************
 *
 *  Function: CELT_Decoder
 *
 *  Abstract:    Read CELT data and if enough data will run the CEL Decoder and return the number of decoded samples
 *
 *  Param:
 *           * ptrDecParams  :   Pointer to CELT_DEC_PARAMS structure
 *
 *  Return:  Number of PCM samples
 *
 *******************************************************************************/
 extern INT32 CELT_Decoder(CELT_DEC_PARAMS *ptr_dec_param);


 /******************************************************************************
 *
 *  Function: CELT_Decoder_Init
 *
 *  Abstract: InitCELT Decoder
 *
 *  Param:
 *           * ptr_dec_param  : Pointer to CELT_DEC_PARAMS structure
 *
 *  Return:  0 if success
 *
 *******************************************************************************/
 extern INT32 CELT_Decoder_Init(CELT_DEC_PARAMS *ptr_dec_param);



 /******************************************************************************
 *
 *  Function: CELT_Decoder_get_mem_size
 *
 *  Abstract:
 *
 *  Param:
 *           None
 *
 *  Return:  Needed memory size for CELT decoder
 *
 *******************************************************************************/
 extern UINT32 CELT_Decoder_get_mem_size(void);



#endif
