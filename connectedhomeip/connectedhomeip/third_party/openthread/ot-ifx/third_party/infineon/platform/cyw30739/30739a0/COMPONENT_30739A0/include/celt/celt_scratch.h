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
#ifndef CELT_SCRATCH_H
#define CELT_SCRATCH_H

#include "brcm_fw_types.h"

typedef struct
{
    UINT32 *buf_base;
    UINT16 buf_offset;
    UINT16 buf_size;
}tCELT_SCRATCH_BUFFER_STATE;


#define OPUS_CELT_DEC_ST_SIZE   2*9300         //byte
#define OPUS_CELT_DEC_SCRATCH_SIZE_UINT32  (OPUS_CELT_DEC_ST_SIZE/4) + 4500 + ((3504*2*2)/4)    //4500 for decoder, ((3504*2*2)/4) for plc
                                                                                                //3504: xqbuflenmax, 2:INT16=2bytes, 2:channels, /4:UINT32=4bytes
#define OPUS_CELT_DEC_SCRATCH_SIZE_BYTE    OPUS_CELT_DEC_SCRATCH_SIZE_UINT32*4

#define OPUS_CELT_PLC_STRUCT_SIZE_BYTE     580  //sizeof(struct CELT_AUDIOPLC_STRUCT)=569

#define OPUS_CELT_ENC_ST_SIZE   3000
#define OPUS_CELT_ENC_SCRATCH_SIZE_UINT32   9000
#define OPUS_CELT_ENC_SCRATCH_SIZE_BYTE     OPUS_CELT_ENC_SCRATCH_SIZE_UINT32*4



#if !defined (CELT_INTEGRATE)
extern UINT32 g_celt_dec_scratch_buf[OPUS_CELT_DEC_SCRATCH_SIZE_UINT32];

extern UINT32 g_celt_enc_scratch_buf[OPUS_CELT_ENC_SCRATCH_SIZE_UINT32];
extern UINT32 g_opus_celt_enc_st_buf[OPUS_CELT_ENC_ST_SIZE];
#endif


extern tCELT_SCRATCH_BUFFER_STATE g_celt_enc_scratch_buf_st;
extern tCELT_SCRATCH_BUFFER_STATE g_celt_dec_scratch_buf_st;

#endif
