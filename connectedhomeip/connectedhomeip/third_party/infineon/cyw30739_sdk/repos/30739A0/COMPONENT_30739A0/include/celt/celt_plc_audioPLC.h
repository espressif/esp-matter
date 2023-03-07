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
/* $Id: audioPLC.h 1.6 2011/03/22 21:38:41 rzopf Exp $ */
/* $Log: audioPLC.h $
 * Revision 1.6  2011/03/22 21:38:41  rzopf
 * code cleanup.
 *
 * Revision 1.5  2011/03/16 17:35:07  rzopf
 * classifier complete for stereo.
 *
 * Revision 1.4  2011/03/11 20:26:48  rzopf
 * stereo support
 *
 * Revision 1.3  2011/02/23 21:51:13  rzopf
 * *** empty log message ***
 *
 * Revision 1.2  2011/02/11 22:01:41  rzopf
 * *** empty log message ***
 *
 * Revision 1.1  2011/01/14 01:33:21  rzopf
 * Initial Version.
 *
 * */

#ifndef CELT_PLC_AUDIOPLC_H
#define CELT_PLC_AUDIOPLC_H

#define CELT_GOOD_FRAME   0
#define CELT_BAD_FRAME    1

#define  MAX_CHAN       2           /* maximum number of supported channels - stereo support */
#define  MAX_SF         48000       /* maximum sampling frequency - 48kHz                    */
#define  DISCRIM_FS     0.02        /* Frame size (in sec) for speech/music discriminator    */
#define  DISCRIM_FS_MS  20          /* Frame size (in msec) for speech/music discriminator   */
#define  DISCRIM_WL     50          /* window length for energy buffer with mler algorithm   */
#define  MAX_FRSZ       1024        /* maximum frame size (in samples) for supported codecs  */
#define  ESHIFT         6           /* 20ms frame size, subsampled to 4kHz gives 80 samples per Energiy computation */

#define DELAY_FOR_PLC   240

struct CELT_SMD_STRUCT
{
   INT16 nbuf;
   INT16 buflen;
   INT16 mler;
   long  Ebuf[DISCRIM_WL];
};



struct CELT_AUDIOPLC_STRUCT
{
//#if USE_BEC
//   struct   GCL_STRUCT gcl_state;
//#endif
   struct   celt_plc_LCPLC_State plc_state[MAX_CHAN];  /* 2 for stereo support */
   struct   CELT_SMD_STRUCT smd_state[MAX_CHAN];
   INT16    SoftClass[MAX_CHAN];
   INT16    fpp;
   INT16    frsz;
   INT16    cfecount;
   INT16    nchan;
   INT16    enable_plc;
};



/*************************************************************************************
 * Function celt_plc_init_audioPLC()
 *
 * Purpose:
 *
 * Inputs:
 *
 *
 * Outputs
 *
 * Return   -
 *
 *************************************************************************************/
void celt_plc_init_audioPLC(struct CELT_AUDIOPLC_STRUCT *aplc, INT16 frsz,
                   INT16 fpp, INT16 nchan);

/*************************************************************************************
 * Function celt_plc_SpeechMusicDiscriminator()
 *
 * Purpose: Compute an indication of the degree that the input is either music (0)
 *          or speech (1).
 *
 * Inputs:
 *          inbuf - pointer to the input signal.  Length is given by aplc->frsz
 *          smd   - pointer to the speech music discriminator memory
 *          aplc  - pointer to the audio plc memory
 *
 * Outputs: none
 *
 * Return:  INT16 - soft decision => 0 = music, 32767 = speech.
 *
 *************************************************************************************/
INT16 celt_plc_SpeechMusicDiscriminator(INT16 *inbuf, struct CELT_SMD_STRUCT *smd, struct CELT_AUDIOPLC_STRUCT *aplc);

#endif /* CELT_PLC_AUDIOPLC_H */
