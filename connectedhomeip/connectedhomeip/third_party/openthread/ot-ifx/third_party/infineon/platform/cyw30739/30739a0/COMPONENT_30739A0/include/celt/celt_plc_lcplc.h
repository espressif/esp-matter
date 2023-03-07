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
/* $Id: lcplc.h 1.40 2011/03/23 21:08:19 rzopf Exp $ */
/* $Log: lcplc.h $
 * Revision 1.40  2011/03/23 21:08:19  rzopf
 * convert xq to long
 *
 * Revision 1.39  2011/03/23 20:50:35  rzopf
 * *** empty log message ***
 *
 * Revision 1.38  2011/03/22 23:04:13  rzopf
 * further speed up/ clean up.
 *
 * Revision 1.37  2011/03/22 16:48:07  rzopf
 * malloc of xqbuf, but not working
 *
 * Revision 1.36  2011/03/22 15:20:30  rzopf
 * cleanup.
 *
 * Revision 1.35  2011/03/10 18:21:48  rzopf
 * AAC 48kHz functionality
 *
 * Revision 1.34  2011/02/25 17:16:29  rzopf
 * 32kHz SBC
 *
 * Revision 1.33  2011/02/24 17:49:11  rzopf
 * 16kHz SBC FR working.
 *
 * Revision 1.32  2011/02/23 18:16:52  rzopf
 * *** empty log message ***
 *
 * Revision 1.31  2011/02/11 21:55:08  rzopf
 * restructured LC-PLC for improved sub-frame flexibility.
 *
 * Revision 1.30  2011/02/07 21:40:39  rzopf
 * modified for 48kHz aac
 *
 * Revision 1.29  2011/01/14 01:31:28  rzopf
 * support of high sampling rates (ie. 48kHz).
 *
 * Revision 1.28  2010/11/09 18:15:55  rzopf
 * added cvsd memory compensation for when no re-encoding h/w is available.
 *
 * Revision 1.27  2010/01/07 22:51:04  rzopf
 * *** empty log message ***
 *
 * Revision 1.26  2010/01/06 18:02:29  rzopf
 * Cleanup
 *
 * Revision 1.25  2009/12/23 23:29:54  rzopf
 * *** empty log message ***
 *
 * Revision 1.24  2009/12/18 20:28:07  rzopf
 * 8kHz version basically in place now.  16kHz untested.
 *
 * Revision 1.23  2009/12/17 21:13:03  rzopf
 * intermediate version
 *
 * Revision 1.22  2009/12/16 18:16:23  rzopf
 * intermediate version - making code run-time switchable between nb/wb
 *
 * Revision 1.21  2009/12/15 00:51:05  rzopf
 * add Packet Loss Rate attenuation.
 *
 * Revision 1.20  2009/10/24 01:58:27  rzopf
 * fixed point speech classifier.
 *
 * Revision 1.19  2009/10/24 01:06:28  rzopf
 * configurable length of zirr and do.
 *
 * Revision 1.18  2009/10/23 23:08:56  rzopf
 * configurable frame size
 *
 * Revision 1.17  2009/05/04 18:56:52  rzopf
 * Added noise-mixing switch, but the noise mixing code is not working
 * properly and should not be enabled.
 *
 * Revision 1.16  2009/04/30 19:12:00  rzopf
 * *** empty log message ***
 *
 * Revision 1.15  2009/04/27 21:25:04  rzopf
 * *** empty log message ***
 *
 * Revision 1.14  2009/04/27 17:28:36  rzopf
 * *** empty log message ***
 *
 * Revision 1.13  2009/04/23 20:35:17  rzopf
 * *** empty log message ***
 *
 * Revision 1.12  2009/01/27 16:49:46  rzopf
 * added a state variable to store the original pitch
 *
 * Revision 1.11  2009/01/27 02:41:19  rzopf
 * ZIR signal.
 *
 * Revision 1.10  2009/01/27 00:52:52  rzopf
 * *** empty log message ***
 *
 * Revision 1.9  2009/01/26 23:55:49  rzopf
 * Changes to integrate the PLC with SBC.
 *
 * Revision 1.8  2008/07/28 22:02:50  rzopf
 * 16kHz
 *
 * Revision 1.7  2008/07/16 16:16:33  rzopf
 * added state variable gaf for gain attenuation factor.
 *
 * Revision 1.6  2007/11/20 21:16:51  rzopf
 * Includes the #defines for controlling the new "mini-plc" version
 * now implemented in lcplc.c.  To enable this new version, set
 * #define MINI_PLC=1.
 *
 * Revision 1.5  2007/09/13 18:37:47  rzopf
 * added celt_plc_MPY_32_16 macro for ARM.
 *
 * Revision 1.4  2007/07/26 23:08:29  rzopf
 * added gain compensation and delayed OLA code.
 *
 * Revision 1.3  2007/07/24 19:59:44  rzopf
 * updating to latest version.
 *
 * Revision 1.2  2007/06/04 21:32:52  rzopf
 * Changed frame size from 40 to 30.
 *
 * Revision 1.1  2007/06/04 19:54:32  rzopf
 * Initial Version.
 *
*/
#ifndef CELT_PLC_LCPLC_H
#define CELT_PLC_LCPLC_H

#include "celt_scratch.h"

/* PLC Method */
#define  FR_      0       /* Frame Repeat */
#define  LCPLC_   1       /* Pitch Extrapolation */

#define  SPEECHPLC      0
#define  MUSICPLC       1
#define  SPEECHMUSICPLC 2

#define MAX_CODEC_CONFIGS 2

/*--------------------------------------------------------------------------------------------------
  #define AFFECTING MEMORY USAGE
  ==============================

  The #defines in this section are used for memory allocation.  They should be set according to the
  vector requirements of the supported configurations.
  --------------------------------------------------------------------------------------------------*/

/* LEFMAX = max(midpp+hppr+rpsr+pwsz+frsz) for all supported configurations */
/*  cvsd 30  sample frame = 293 */
/*  esbc 120 sample frame = 646 */ /* + 40 for 160 frame size */
//#define  LEFMAX         5000 //(646+40)           /* % Length to the End of current Frame of xq()          */

/* OLALMAX = max(olal, olalf) for all supported configurations */
//#define  OLALMAX        1024 //60

/* DOMAX = max(dola) for all supported configurations */
//#define  DOMAX           512//38             /* max delay OLA */

#define  CELT_MAX8KFRSZ       60             /* Maximum frsz length for 8k config  */

#define  MAXRINGL        1024          /* maximum ringing length for filter ringing OLA */

//#define XQBUFLEN           (LEFMAX+OLALMAX+DOMAX)
/*--------------------------------------------------------------------------------------------------*/
/*  END OF #define AFFECTING MEMORY USAGE                                                           */
/*--------------------------------------------------------------------------------------------------*/


/* The #defines below do not change */
#define  SMDTH       4915        /* % 1.15 Sum of Magnitude Difference THreshold for pitch submultiples */
#define  STFO        8           /* % Short-Term Filter Order for ringing calculation           */
#define  LTRATT      24576       /* % 0.75 Long-Term Ringing ATTenuation factor from PWE scaling factor */
#define  GCINI       9830              /* % 0.3 Q15 Gain Correction INItial value */
#define  GCATT       32545             /* % 0.9932 Q15 Gain Correction ATTenuation factor */
#define  PLRATTB      ((short)(0.99*32768.0))    /* decay factor for signal attenuation at high loss rates */
#define  PLR_B        ((short)(0.975*32768.0))   /* Running Mean Beta Factor for Packet Loss Rate Estimation */
#define  PLR_B1       ((short)(32768l-PLR_B))    /* 1 - PLR_B */

/* CVSD Step Size Compensation */
#define  SSATT         0.9952
#define  FSSATT        32611

/* Macros */
//#if ARM_MATH==1
//#define celt_plc_MPY_32_16(x32, y16) (Word32)(((__int64)(x32)*(__int64)(y16))>>15)
//#else
#define celt_plc_EXTRACT_HI(x) (x>>16)
#define celt_plc_EXTRACT_LO(x) ((x-((x>>16)<<16))>>1)
#define celt_plc_MPY_32_16(x32, y16) (((celt_plc_EXTRACT_HI(x32)*((Word32)y16))+((celt_plc_EXTRACT_LO(x32)*((Word32)y16))>>15))<<1)
//#endif

struct celt_plc_LCPLC_config        /* Sampling frequency, codec, or customer dependent configs */
{
   short    sf;            /* Sampling Frequency                                             */
   short    sfratio;       /* Ratio of sampling frequency to 8kHz (sf/8)                     */
   short    frsz;          /* Frame Size (samples)                                           */
   short    dola;          /* Delayed OverLap Add length (samples)                           */
   short    reencsw;       /* Re-Encoding in software is required (1) or done in h/w (0)     */
   short    nsub;          /* Number of Subframes                                            */
   short    subfrsz;       /* Subframe Size (samples)                                        */
   short    plratt_max;    /* Maximum attenuation once the PLR reaches PLRATTEND (State variable used for CGS           */
   short    plrattstrt;    /* Attenuation begins at this PLR %, with the attenuation ramping down from 1 (CGS Variable) */
   short    plrattend;     /* Attenuation reaches its maximum once the PLR % hits this rate (CGS Variable)              */
   short    gattst;        /* % Gain ATTenuation STarting frame (into erasure)               */
   short    gattf;         /* % Gain ATTenuation Factor (per sample)                         */
   long     attslp;        /* Adaptive Attenuation Curve implemented as slope+y-intercept. ((1.0-PLRATT_MIN)/(PLRATT_STRT-PLRATT_END)) */
   long     attyinter;     /* Adaptive Attenuation Curve implemented as slope+y-intercept. (1.0-PLRATT_SLP*PLRATT_STRT)                */
   short    gattfu;        /* Gain Recovery Factor                                           */
   short    decf;          /* DECimation Factor for coarse pitch period search               */
   short    midpp;         /* MIDdle point of the pitch period range                         */
   short    hppr;          /* Half the Pitch Period Range                                    */
   short    pwsz;          /* Pitch analysis Window SiZe for 8 kHz lowband (15 ms)           */
   short    smwsz;         /* pitch Sub-Multiple search Window SiZe (fixed)                  */
   short    sfwsz;         /* Scale Factor Window Size                                       */
   short    rpsr;          /* Refined Pitch Search Range (# of samples in delta pitch)       */
   short    olal;          /* OverLap Add Length                                             */
   short    olalf;         /* OverLap Add Length First good frame                            */
   short    *ola;          /* pointer to be set to OLA table                                 */
   short    *olaf;         /* pointer to be set to OLA table for use in first good frame     */
   short    minpp;         /* MINimum Pitch Period (in # of 8 kHz samples) = (MIDPP-HPPR)    */
   short    stwsz;         /* Short-Term predictive analysis Window SiZe                     */
   short    maxpp2;        /* largest value that is a power of 2 and is less than the max pp (midpp+hppr) */
   short    usefiltring;   /* use filter ringing in first bad frame? 0=NO, 1=YES             */
   short    applyola2zsr;  /* apply the overlap-add window to the codec ZSR signal?  0/1     */
   short    *olauv;        /* ola window for unvoiced (uncorrelated) signals                 */
   short    *olauvf;       /* ola window for unvoiced (uncorrelated) signals in first good frame */
   short    folding;       /* window folding (ie. MDCT) present in the codec? (0=NO, 1=YES)  */
   short    *olafold;      /* overlap-add window for folding                                 */
   short    *zirolav;      /* overlap add window for zir/ringing in first bad frame - voiced */
   short    *zirolauv;     /* overlap add window for zir/ringing in first bad frame - unvoiced */
   short    zirwin;        /* 1= window is applied to incoming signal, 0 = no window applied */
   short    ringl;         /* length of ringing and zir windows in the case that zirwin=0 and ringing is used */
   short    maxzsrolal;    /* max zsrolal (including all possible first good frame windows */
   short    zirolastart;   /* the number of samples before zir ola begins                    */
   short    zirolal;       /* zir ola length                                                 */
   short    fprr;          /* final pitch refinement range                                   */
   short    fprwl;         /* final pitch refinement window length                           */
   short    fprdecf;       /* final pitch refinement decimation factor                       */
   short    delay;         /* number of samples to delay the output.  Use the delay for OLA in FGF in the case of no ZIR signal */
};

/* LCPLC State Information */
struct celt_plc_LCPLC_State
{
   Word16 cfecount;                    /* continuous frame erasure count                     */
   Word16 pp;                          /* pitch period                                       */
   Word16 opp;                         /* orig pp                                            */
   Word16 sf;                          /* scaling factor for pwe                             */
   long   *xq;                         /* signal buffer                                      */
   Word16 gc;                          /* Gain Correction                                    */
   Word16 gaf;                         /* gain attenuation factor                            */
   Word16 natt;                        /* Number of consecutively lost samples before atten. */
   Word16 Class;                       /* Speech frame classification                        */
   unsigned long idum;                 /* Random number generator seed memory                */
   Word16 plr;                         /* Packet Loss Rate                                   */
   Word32 plratt;                      /* Packet Loss Rate based ATTenuation factor          */
   short    ssc;
   short    ss;                        // step size estimate;
   short    yhat;                      // cvsd acc estimate;
   short    ss_1;                      // step size estimate from last frame;
   short    yhat_1;                    // cvsd acc estimate from last frame;
   short    last_BFI;                  // Bad Frame Indicator from the last frame
   short    BEC;                       // Bit Error Concealment Detection Status (1= biterror, 0=no error)
   short    fpp;                       /* frames per packet */
   struct   celt_plc_LCPLC_config *config[MAX_CODEC_CONFIGS];   /* LC-PLC pitch extrapolation (speech) configurations */
   struct   celt_plc_LCPLC_config *frconfig[MAX_CODEC_CONFIGS]; /* Frame Repeat Configurations */
   short    ConfigSetting;             /* SPEECHPLC, MUSICPLC, SPEECHMUSICPLC                */
   short    xqbuflen;                  /* length of xq buffer */
};



/* Prototypes */
void celt_plc_LC_PLC_erasure(struct celt_plc_LCPLC_State  *plc_state, Word16 *out, Word16 *CodecRingBuf,
                    Word16 zirl, Word16 PlcMethod, Word16 CodecConfig
#if defined (ENABLE_SCRATCH_BUF)
                    ,tCELT_SCRATCH_BUFFER_STATE *pst_celt_scrabuf_st
#endif
                    );
void celt_plc_LC_PLC(struct celt_plc_LCPLC_State *plc_state, Word16 *in, Word16 *out, Word16 nzsr, Word16 PlcMethod, Word16 CodecConfig);
void celt_plc_Init_LC_PLC(struct celt_plc_LCPLC_State *plc_state, struct celt_plc_LCPLC_config *config[MAX_CODEC_CONFIGS], struct celt_plc_LCPLC_config *frconfig[MAX_CODEC_CONFIGS], short fpp, short ConfigSetting
#if defined (ENABLE_SCRATCH_BUF)
                            ,tCELT_SCRATCH_BUFFER_STATE *pst_celt_scrabuf_st
#endif
                         );
void celt_plc_Free_LC_PLC(struct celt_plc_LCPLC_State *plc_state);

#endif /* CELT_PLC_LCPLC_H */
