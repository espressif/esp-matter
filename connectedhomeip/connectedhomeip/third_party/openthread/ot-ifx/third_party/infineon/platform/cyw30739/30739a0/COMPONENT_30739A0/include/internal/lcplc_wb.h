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
**  File Name:   lcplc_wb.h
**
**  Description: This file contains common defines / structure and structures used by
**  Packet Loss Concealment WideBand
**
**
******************************************************************************/

#ifndef LCPLC_H
#define LCPLC_H

#ifndef NULL
#define NULL     0
#endif

// Added define for LCPLC_Config structure

/* DEFINE BASIC PLC PARAMETERS */

#define  DOLA_NO_HW_RE_ENCOD    5
#define  DOLA_HW_RE_ENCOD          0

#define  SF_8          8          /* % input Sampling Frequency (in kHz)                         */
#define  SF_16        16          /* % input Sampling Frequency (in kHz)                         */

#define  DECF_8        (4*SF_8/8)    /* % DECimation Factor for coarse pitch period search */
#define  DECF_16        (4*SF_16/8)    /* % DECimation Factor for coarse pitch period search */

#define  NSUB_8        1
#define  NSUB_16      2

#define  MIDPP_8       (82*SF_8/8)   /* % MIDdle point of the pitch period range */
#define  HPPR_8        (58*SF_8/8)   /* % Half the Pitch Period Range */
#define  PWSZ_8        15*SF_8       /* % Pitch analysis Window SiZe for 8 kHz lowband (15 ms)   */
#define  MAXRFWSZ_8    10*SF_8       /* % MAXimum ReFinement Window Size                         */

#define  MIDPP_16       (82*SF_16/8)   /* % MIDdle point of the pitch period range */
#define  HPPR_16        (58*SF_16/8)   /* % Half the Pitch Period Range */
#define  PWSZ_16        15*SF_16       /* % Pitch analysis Window SiZe for 8 kHz lowband (15 ms) */
#define  MAXRFWSZ_16    10*SF_16       /* % MAXimum ReFinement Window Size                       */

#define  SMWSZ_8        22//25          /* % pitch Sub-Multiple search Window SiZe (fixed) */
#define  SFWSZ_8        18

#define  SMWSZ_16       42          /* % pitch Sub-Multiple search Window SiZe (fixed) */
#define  SFWSZ_16       35

#define  RPSR_8        (3*SF_8/8)    /* % Refined Pitch Search Range (# of samples in delta pitch) */
#define  RPSR_16        (3*SF_16/8)    /* % Refined Pitch Search Range (# of samples in delta pitch) */

#define  SMPSR       2           /* % Sub-Multiple Pitch Search Range (# of samples in delta pitch) */

#define  SMDTH       4915        /* % 1.15 Sum of Magnitude Difference THreshold for pitch submultiples */

#define  OLAL_8        (30*SF_8/8)   /* % OverLap Add Length                                     */
#define  OLALF_8       (20*SF_8/8)

#define  OLAL_16        (30*SF_16/8)   /* % OverLap Add Length                                   */
#define  OLALF_16       (20*SF_16/8)

#define  MIDPP_8       (82*SF_8/8)   /* % MIDdle point of the pitch period range */
#define  HPPR_8        (58*SF_8/8)   /* % Half the Pitch Period Range */
#define  PWSZ_8        15*SF_8       /* % Pitch analysis Window SiZe for 8 kHz lowband (15 ms)   */
#define  MAXRFWSZ_8    10*SF_8       /* % MAXimum ReFinement Window Size   */

#define  MIDPP_16       (82*SF_16/8)   /* % MIDdle point of the pitch period range */
#define  HPPR_16        (58*SF_16/8)   /* % Half the Pitch Period Range */
#define  PWSZ_16        15*SF_16       /* % Pitch analysis Window SiZe for 8 kHz lowband (15 ms) */
#define  MAXRFWSZ_16    10*SF_16       /* % MAXimum ReFinement Window Size       */

#define  MAXPP_8       (MIDPP_8+HPPR_8+RPSR_8) /* % MAXimum Pitch Period (in # of 8 kHz samples) */
#define  MINPP_8       (MIDPP_8-HPPR_8)      /* % MINimum Pitch Period (in # of 8 kHz samples)   */
#define  XQOFF_8       (MAXPP_8+PWSZ_8)      /* % xq() offset before current subframe            */


#define  MAXPP_16       (MIDPP_16+HPPR_16+RPSR_16) /* % MAXimum Pitch Period (in # of 8 kHz samples) */
#define  MINPP_16       (MIDPP_16-HPPR_16)      /* % MINimum Pitch Period (in # of 8 kHz samples)    */
#define  XQOFF_16       (MAXPP_16+PWSZ_16)      /* % xq() offset before current subframe             */

#define  RING_LEN      (15*(8>>2))      /* Ring Len */


//

/* DEFINE TUNING PLC PARAMETERS */
#define PLC_DEF_PLRATT_MAX     4096    // plratt_max        : Maximum attenuation once the PLR reaches PLRATTEND. Q15 format. Default value is 4096 (0.125)
#define PLC_DEF_PLRATT_START     14746    // plrattstrt           : Attenuation begins at this PLR % Q15 format. Default value is 14746 (0.45)
#define PLC_DEF_PLRATT_END        22938    // plrattend           : Attenuation reaches its maximum once the PLR % hits this rate (CGS Variable) Q15 format. Default value is 22938 (0.70)
#define PLC_DEF_GATT_TIME_ST      30        // gatt_time_st      : Gain ATTenuation STarting time in ms (into erasure) integer. Default value is 30
#define PLC_DEF_GATT_F            32234    // ggattf               : Gain ATTenuation Factor (per sample)  Q15 format. Default value is
#define PLC_DEF_GATT_F_WB        32500    // ggattf for WB     : Gain ATTenuation Factor (per sample)  Q15 format. Default value is


/*--------------------------------------------------------------------------------------------------
  #define AFFECTING MEMORY USAGE
  ==============================

  The #defines in this section are used for memory allocation.  They should be set according to the
  vector requirements of the supported configurations.
  --------------------------------------------------------------------------------------------------*/

/* LEFMAX = max(midpp+hppr+rpsr+pwsz+frsz) for all supported configurations */
/*  cvsd 30  sample frame = 293 */
/*  esbc 120 sample frame = 646 */ /* + 40 for 160 frame size */
#define  LEFMAX         (646+40)           /* % Length to the End of current Frame of xq() */

/* OLALMAX = max(olal, olalf) for all supported configurations */
#define  OLALMAX        60

/* DOMAX = max(dola) for all supported configurations */
#define  DOMAX           38             /* max delay OLA */

#define  MAX8KFRSZ       90             /* Maximum frsz length for 8k config  */

/*-----------------------------------------------------------------------------------------------*/
/*  END OF #define AFFECTING MEMORY USAGE                                                        */
/*-----------------------------------------------------------------------------------------------*/


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
#if ARM_MATH==1
#define MPY_32_16(x32, y16) (Word32)(((SINT64)(x32)*(SINT64)(y16))>>15)
#else
#define EXTRACT_HI(x) (x>>16)
#define EXTRACT_LO(x) ((x-((x>>16)<<16))>>1)
#define MPY_32_16(x32, y16) (((EXTRACT_HI(x32)*((Word32)y16))+((EXTRACT_LO(x32)*((Word32)y16))>>15))<<1)
#endif



typedef struct
{
   short    plratt_max;    /* Maximum attenuation once the PLR reaches PLRATTEND (State variable used for CGS           */
   short    plrattstrt;    /* Attenuation begins at this PLR %, with the attenuation ramping down from 1 (CGS Variable) */
   short    plrattend;     /* Attenuation reaches its maximum once the PLR % hits this rate (CGS Variable)              */
   short    gatt_time_st;        /* % Gain ATTenuation STarting time in ms (into erasure)               */
   short    gattf;         /* % Gain ATTenuation Factor (per sample)                         */
} PLC_TUNING_CFG;


/* The original struct LCPLC_config structure is splitted into 2 different structure */
/* LCPLC_config1: It contains all the run-time settings which depend on user settings and BT Link */
/* LCPLC_config2: It contains all internal PLC parameters only depending on Frequency sampling */

struct LCPLC_config1        /* Sampling frequency, codec, or customer dependent configs */
{
   short    sf;            /* Sampling Frequency                                             */
   short    frsz;          /* Frame Size (samples)                                           */
   short    dola;          /* Delayed OverLap Add length (samples)                           */
   short    reencsw;       /* Re-Encoding in software is required (1) or done in h/w (0)     */
   short    nsub;          /* Number of Subframes                                            */
   short    subfrsz;       /* Subframe Size (samples)                                        */
   short    plratt_max;    /* Maximum attenuation once the PLR reaches PLRATTEND (State variable used for CGS           */
   short    plrattstrt;    /* Attenuation begins at this PLR %, with the attenuation ramping down from 1 (CGS Variable) */
   short    plrattend;     /* Attenuation reaches its maximum once the PLR % hits this rate (CGS Variable)              */
   short    gatt_time_st;        /* % Gain ATTenuation STarting time in ms (into erasure)               */
   short    gattf;         /* % Gain ATTenuation Factor (per sample)                         */
   long     attslp;        /* Adaptive Attenuation Curve implemented as slope+y-intercept. ((1.0-PLRATT_MIN)/(PLRATT_STRT-PLRATT_END)) */
   long     attyinter;     /* Adaptive Attenuation Curve implemented as slope+y-intercept. (1.0-PLRATT_SLP*PLRATT_STRT)                */
   short    gattfu;        /* Gain Recovery Factor    */
};

struct LCPLC_config2        /* Internal PLC parameters only depending on Frequency sampling */
{
   short    decf;          /* DECimation Factor for coarse pitch period search               */
   short    midpp;         /* MIDdle point of the pitch period range                         */
   short    hppr;          /* Half the Pitch Period Range                                    */
   short    pwsz;          /* Pitch analysis Window SiZe for 8 kHz lowband (15 ms)           */
   short    smwsz;         /* pitch Sub-Multiple search Window SiZe (fixed)                  */
   short    sfwsz;         /* Scale Factor Window Size                                       */
   short    rpsr;          /* Refined Pitch Search Range (# of samples in delta pitch)       */
   short    olal;          /* OverLap Add Length                                             */
   short    olalf;         /* OverLap Add Length First good frame                            */
   const short    *ola;          /* pointer to be set to OLA table                            */
   const short    *olaf;         /* pointer to be set to OLA table for use in first good frame */
   short    minpp;         /* MINimum Pitch Period (in # of 8 kHz samples) = (MIDPP-HPPR)    */
   short    olalmax;       /* maximum of olal, olalf                                         */
   short    stwsz;         /* Short-Term predictive analysis Window SiZe                     */
};


struct LCPLC_config        /* Sampling frequency, codec, or customer dependent configs */
{
   short    sf;            /* Sampling Frequency                                             */
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
   const short    *ola;          /* pointer to be set to OLA table                           */
   const short    *olaf;         /* pointer to be set to OLA table for use in first good frame */
   short    minpp;         /* MINimum Pitch Period (in # of 8 kHz samples) = (MIDPP-HPPR)    */
   short    olalmax;       /* maximum of olal, olalf                                         */
   short    stwsz;         /* Short-Term predictive analysis Window SiZe                     */
};

/* LCPLC State Information */
struct LCPLC_State
{
   Word16 *ScratchMem;                 /* Allocated memory */
   Word16 cfecount;                    /* continuous frame erasure count                     */
   Word16 pp;                          /* pitch period                                       */
   Word16 opp;                         /* orig pp                                            */
   Word16 sf;                          /* scaling factor for pwe                             */
   Word16 xq[LEFMAX+OLALMAX+DOMAX];    /* signal buffer                                      */
   Word16 gc;                          /* Gain Correction                                    */
   Word16 gaf;                         /* gain attenuation factor                            */
   Word16 natt;                        /* Number of consecutively lost samples before atten. */
   Word16 Class;                       /* Speech frame classification                        */
   unsigned long idum;                 /* Random number generator seed memory                */
   Word16 plr;                         /* Packet Loss Rate                                   */
   Word32 plratt;                      /* Packet Loss Rate based ATTenuation factor          */
   Word16   * CodecRingBuf;            /*  Ring buffer */
   short    ssc;
   short    ss;                        // step size estimate;
   short    yhat;                      // cvsd acc estimate;
   short    ss_1;                      // step size estimate from last frame;
   short    yhat_1;                    // cvsd acc estimate from last frame;
   short    last_BFI;                  // Bad Frame Indicator from the last frame
   short    BEC;                       // Bit Error Concealment Detection Status (1= biterror, 0=no error)
   short    cvsd_step_comp_en;                       // Added to field to disable CVSD Step compensation (last minute update before tape-out !)
   struct LCPLC_config1 config1;         /* Constant config setttings                         */
   const struct LCPLC_config2 *config2;  /* Constant config setttings                         */
};



/* Prototypes */
#ifdef __cplusplus
extern "C" {
#endif


void LC_PLC_erasure_wb(struct LCPLC_State  *plc_state, Word16 *out, Word16 *CodecRingBuf, Word16 CodecRingLen);
void LC_PLC_wb(struct LCPLC_State *plc_state, Word16 *in, Word16 *out);
void LC_PLC_Indicate_BEC(struct LCPLC_State *plc_state, Word16 BitError);
void Update_LC_PLC_wb(struct LCPLC_State *plc_state, Word16 plratt_max, Word16 plrattstrt, Word16 plrattend, Word16 gatt_time_st, Word16 gattf);
void Reset_LC_PLC_wb(struct LCPLC_State *plc_state);
Word16 Init_LC_PLC_wb(struct LCPLC_State *plc_state, Word16 freq_s, Word16 frame_size, Word16 hw_re_encode);
Word16 LC_PLC_get_pl_rate(struct LCPLC_State *plc_state);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LCPLC_H */
