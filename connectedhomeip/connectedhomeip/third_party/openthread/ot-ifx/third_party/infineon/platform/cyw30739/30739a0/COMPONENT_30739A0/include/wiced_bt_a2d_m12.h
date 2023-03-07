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

/** @file
 *
 * MPEG-1, 2 Audio A2DP Application Programming Interface
 *
 */
/*****************************************************************************
**
**  Name:       wiced_bt_a2d_m12.h
**
**  Description:Interface to MPEG-1, 2 Audio
**
**
*****************************************************************************/
#pragma once

/*****************************************************************************
**  Constants
*****************************************************************************/
/* the length of the MPEG_1, 2 Audio Media Payload header. */
#define A2D_M12_MPL_HDR_LEN         4

/* the LOSC of MPEG_1, 2 Audio media codec capabilitiy */
#define A2D_M12_INFO_LEN          6

/* for Codec Specific Information Element */
#define A2D_M12_IE_LAYER_MSK        0xE0    /* b7-b5 layer */
#define A2D_M12_IE_LAYER1           0x80    /* b7: layer1 (mp1) */
#define A2D_M12_IE_LAYER2           0x40    /* b6: layer2 (mp2) */
#define A2D_M12_IE_LAYER3           0x20    /* b5: layer3 (mp3) */

#define A2D_M12_IE_CRC_MSK          0x10    /* b4: CRC */

#define A2D_M12_IE_CH_MD_MSK        0x0F    /* b3-b0 channel mode */
#define A2D_M12_IE_CH_MD_MONO       0x08    /* b3: mono */
#define A2D_M12_IE_CH_MD_DUAL       0x04    /* b2: dual */
#define A2D_M12_IE_CH_MD_STEREO     0x02    /* b1: stereo */
#define A2D_M12_IE_CH_MD_JOINT      0x01    /* b0: joint stereo */

#define A2D_M12_IE_MPF_MSK          0x40    /* b6: MPF */

#define A2D_M12_IE_SAMP_FREQ_MSK    0x3F    /* b5-b0 sampling frequency */
#define A2D_M12_IE_SAMP_FREQ_16     0x20    /* b5:16  kHz */
#define A2D_M12_IE_SAMP_FREQ_22     0x10    /* b4:22.05kHz */
#define A2D_M12_IE_SAMP_FREQ_24     0x08    /* b3:24  kHz */
#define A2D_M12_IE_SAMP_FREQ_32     0x04    /* b2:32  kHz */
#define A2D_M12_IE_SAMP_FREQ_44     0x02    /* b1:44.1kHz */
#define A2D_M12_IE_SAMP_FREQ_48     0x01    /* b0:48  kHz */

#define A2D_M12_IE_VBR_MSK          0x80    /* b7: VBR */

#define A2D_M12_IE_BITRATE_MSK      0x7FFF  /* b6-b0 of octect 2, all of octect3*/
#define A2D_M12_IE_BITRATE_0        0x0001  /* 0000 */
#define A2D_M12_IE_BITRATE_1        0x0002  /* 0001 */
#define A2D_M12_IE_BITRATE_2        0x0004  /* 0010 */
#define A2D_M12_IE_BITRATE_3        0x0008  /* 0011 */
#define A2D_M12_IE_BITRATE_4        0x0010  /* 0100 */
#define A2D_M12_IE_BITRATE_5        0x0020  /* 0101 */
#define A2D_M12_IE_BITRATE_6        0x0040  /* 0110 */
#define A2D_M12_IE_BITRATE_7        0x0080  /* 0111 */
#define A2D_M12_IE_BITRATE_8        0x0100  /* 1000 */
#define A2D_M12_IE_BITRATE_9        0x0200  /* 1001 */
#define A2D_M12_IE_BITRATE_10       0x0400  /* 1010 */
#define A2D_M12_IE_BITRATE_11       0x0800  /* 1011 */
#define A2D_M12_IE_BITRATE_12       0x1000  /* 1100 */
#define A2D_M12_IE_BITRATE_13       0x2000  /* 1101 */
#define A2D_M12_IE_BITRATE_14       0x4000  /* 1110 */

#define A2D_BLD_M12_PML_HDR(p_dst,frag_offset) {UINT16_TO_BE_STREAM(p_dst, 0); \
                                                UINT16_TO_BE_STREAM(p_dst, frag_offset); }

#define A2D_PARS_M12_PML_HDR(p_src,frag_offset) {BE_STREAM_TO_UINT16(frag_offset, p_src); \
                                                 BE_STREAM_TO_UINT16(frag_offset, p_src); }


/*****************************************************************************
**  Type Definitions
*****************************************************************************/

/* data type for the MPEG-1, 2 Audio Codec Information Element*/
typedef struct
{
    uint8_t       layer;      /* layers */
    wiced_bool_t  crc;        /* Support of CRC protection or not */
    uint8_t       ch_mode;    /* Channel mode */
    uint8_t       mpf;        /* 1, if MPF-2 is supported. 0, otherwise */
    uint8_t       samp_freq;  /* Sampling frequency */
    uint8_t       vbr;        /* Variable Bit Rate */
    uint16_t      bitrate;    /* Bit rate index */
} wiced_bt_a2d_m12_cie_t;

/*****************************************************************************
**  External Function Declarations
*****************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif
/******************************************************************************
**
** Function         wiced_bt_a2d_bld_m12info
**
** Description      This function is called by an application to build
**                  the MPEG-1, 2 Audio Media Codec Capabilities byte sequence
**                  beginning from the LOSC octet.
**                  Input Parameters:
**                      media_type:  Indicates Audio, or Multimedia.
**
**                      p_ie:  The MPEG-1, 2 Audio Codec Information Element
**                             information.
**
**                  Output Parameters:
**                      p_result:  the resulting codec info byte sequence.
**
** Returns          A2D_SUCCESS if function execution succeeded.
**                  Error status code, otherwise.
******************************************************************************/
wiced_bt_a2d_status_t wiced_bt_a2d_bld_m12info(uint8_t media_type, wiced_bt_a2d_m12_cie_t *p_ie,
                                               uint8_t *p_result);

/******************************************************************************
**
** Function         wiced_bt_a2d_pars_m12info
**
** Description      This function is called by an application to parse
**                  the MPEG-1, 2 Audio Media Codec Capabilities byte sequence
**                  beginning from the LOSC octet.
**                  Input Parameters:
**                      p_info:  the byte sequence to parse.
**
**                      for_caps:  TRUE, if the byte sequence is for get capabilities
**                                 response.
**
**                  Output Parameters:
**                      p_ie:  The MPEG-1, 2 Audio Codec Information Element
**                             information.
**
** Returns          A2D_SUCCESS if function execution succeeded.
**                  Error status code, otherwise.
******************************************************************************/
wiced_bt_a2d_status_t wiced_bt_a2d_pars_m12info(wiced_bt_a2d_m12_cie_t *p_ie, uint8_t *p_info,
                                               wiced_bool_t for_caps);

#ifdef __cplusplus
}
#endif
