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
**  File Name:   sbc_common.h
**
**  Description: This file contains common defines / structure and structures used by Encoder and
**               Decoder
**
**
******************************************************************************/

#ifndef SBC_COMMON_H
#define SBC_COMMON_H

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE (!FALSE)
#endif

#define SBC_LIBRARY_VERSION        138    //    1.38

#define SBC_FAST_CRC TRUE

#define SBC_MAX_NUM_OF_SUBBANDS    8
#define SBC_MAX_NUM_OF_CHANNELS    2
#define SBC_MAX_NUM_OF_BLOCKS    16

#define SBC_LOUDNESS    0
#define SBC_SNR 1

#define SUB_BANDS_8 8
#define SUB_BANDS_4 4

#define SBC_SUB_BANDS_8    SUB_BANDS_8
#define SBC_SUB_BANDS_4    SUB_BANDS_4

#define SBC_sf16000 0
#define SBC_sf32000 1
#define SBC_sf44100 2
#define SBC_sf48000 3

#define SBC_MONO    0
#define SBC_DUAL    1
#define SBC_STEREO  2
#define SBC_JOINT_STEREO    3

#define SBC_BLOCK_0 4
#define SBC_BLOCK_1 8
#define SBC_BLOCK_2 12
#define SBC_BLOCK_3 16

#define SBC_NULL    0

/* Maximum number of FIR EQ */
#define SBC_UTIL_MAX_FIR_EQ    128


/*DEFINES*/
#define SBC_CRC_FAILURE -1
#define SBC_SYNC_FAILURE -2
#define SBC_EMPTY_FAILURE -3
#define SBC_MEM_FAILURE -4
//#define SBC_BAD_WB_UUI -5
#define SBC_PLC_FAILURE -6
#define SBC_INIT_FAILURE -7
#define SBC_WAIT_FOR_LAST_FRAME -8
//#define SBC_WAIT_FOR_NEXT_EV3 -9
#define SBC_MODE_NOT_SUPPORTED -10
#define SBC_BAD_SBC_FRAME_STATUS -11            /* nb_frame is not inline with frame_status */
#define SBC_WAIT_FOR_MORE_SBC_DATA -12        /* Not enough SBC Data to run SBC Decoder */
#define SBC_TOO_MUCH_SBC_DATA -13                /* SBC buffer is full. Host need to call SBC_Dec_get_PCM to free some space */
#define SBC_FAILURE 0
#define SBC_SUCCESS 1

#define SBC_NULL 0


#define SBC_HDR_SYNC        0x9C
#define SBC_WBS_HDR_SYNC    0xAD



/* For SBC WB */
enum
{
    SBC_MODE_A2DP,        // Original SBC for A2DP
    SBC_MODE_WB,            // SBC for WideBand
    SBC_DEC_MODE_MONO_A2DP    // Mono A2DP Decoder: PCM output is always Mono / Fs 16 kHz
};

/* Header with synchronisation word and sequence number
H2 contains both the synchronisation word and the sequence number.
The two octet header shall contain a 12 bit synchronisation word and a 2 bit sequence number (SN0, SN1).
The latter is protected by a simple repetition code (both bits are duplicated).  Hence, each pair of bits in the sequence number shall be always 00 or 11. */

#define SBC_H2_HDR_LEN            2        // H2 header is 2 bytes
#define SBC_H2_BYTE_0            0x01    // First H2 byte is always equal to 1
#define SBC_H2_BYTE_1            0x08    // Second H2 byte: First 4 bits
#define SBC_H2_BYTE_1_MASK    0x0F    // H2 byte Mask : First 4 bits

#define SBC_PADDING_BYTE_LEN    1        // Padding of 1 byte after mSBC frame

#define SBC_MAX_NUM_OF_BLOCKS_WB   15
#define SBC_MAX_NUM_OF_RETRANS    2
#define SBC_MAX_NUM_OF_ESCO_FRAME (1+SBC_MAX_NUM_OF_RETRANS)  /* Assume 1 SBC frame + N retransmission */
#define SBC_HEADER_SIZE 4 /* sync byte, ch mode/num subb/num blk byte, bit pool byte, and crc */

#ifndef SBC_MAX_PACKET_LENGTH
#define SBC_MAX_PACKET_LENGTH 520
#endif

#define SBC_WB_MAX_PCM_SAMPLES 120    // 2EV3 case, with 8 sb and 15 blk = 120 samples
#define SBC_WB_MAX_PACKET_LEN 57        // 2EV3 case, with 8 sb and 15 blk = 57 bytes

#define SBC_WB_MAX_PACKET_LEN_RND32  ((SBC_WB_MAX_PACKET_LEN & ~3)+4)



/* Compute optimum buffer size */
#define SB_BUFFER_SIZE_WB (1 * SBC_MAX_NUM_OF_SUBBANDS * SBC_MAX_NUM_OF_BLOCKS_WB)
#define SB_BUFFER_SIZE_SIG (SBC_MAX_NUM_OF_CHANNELS * SBC_MAX_NUM_OF_SUBBANDS * SBC_BLOCK_3)

/* Max Nb of byte to compute CRC */
#define SBC_MAX_BYTE_FOR_CRC        (4+(4*SBC_MAX_NUM_OF_SUBBANDS*SBC_MAX_NUM_OF_CHANNELS/8)+1)




/* In SBC WideBand, the SBC parameters are defined according to UUI */
typedef struct SBC_WB_UUI_TAG
{
    SINT16 NumOfSubBands;
    SINT16 NumOfBlock;
    SINT16 Bitpool;
}SBC_WB_UUI_PARAM;



typedef struct SBC_COM_PARAMS_TAG
{
    SINT16 samplingFreq;                         /* 16k, 32k, 44.1k or 48k*/
    SINT16 channelMode;                          /* mono, dual, stereo or joint stereo*/
    SINT16 numOfSubBands;                        /* 4 or 8 */
    SINT16 numOfChannels;
    SINT16 numOfBlocks;                          /* 4, 8, 12 or 16*/
    SINT16 allocationMethod;                     /* loudness or SNR*/
    SINT16 bitPool;                              /* 16*numOfSb for mono & dual;
                                                       32*numOfSb for stereo & joint stereo */
    SINT16 scaleFactor[SBC_MAX_NUM_OF_CHANNELS*SBC_MAX_NUM_OF_SUBBANDS];
    SINT16  ScratchMemForBitAlloc[SBC_MAX_NUM_OF_CHANNELS*SBC_MAX_NUM_OF_SUBBANDS];
    SINT16 bits[SBC_MAX_NUM_OF_CHANNELS*SBC_MAX_NUM_OF_SUBBANDS];
    UINT8  *packet;
} SBC_COM_PARAMS;


#ifdef __cplusplus
extern "C" {
#endif


extern void sbc_com_bit_alloc_mono(SBC_COM_PARAMS *CodecParams);
#if (SBC_DEC_STEREO_INCLUDED == TRUE || SBC_ENC_STEREO_INCLUDED ==TRUE)
extern void sbc_com_bit_alloc_ste(SBC_COM_PARAMS *CodecParams);
#endif
extern UINT8 SBC_com_crc(SBC_COM_PARAMS *pstrCodecParams, UINT32 sbc_header_mode);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
