/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

 #ifndef PKA_EC_EDW_GLOB_REGS_DEF_H
 #define PKA_EC_EDW_GLOB_REGS_DEF_H

 /*! Note: Don't change registers ID-s ! */

 /*! Define global PKA registers ID-s used in EC Edwards operations */

 /*--------------------------*/
 /* Common EC Edw. registers */
 /*--------------------------*/
 #define EDW_REG_N       PKA_REG_N  /* EC mod. */
 #define EDW_REG_NP      PKA_REG_NP /* EC Barr.tag */
 #define EDW_REG_T3      2 /* 4 temp regs. */
 #define EDW_REG_T4      3
 #define EDW_REG_T5      4
 #define EDW_REG_T6      5
 #define EDW_REG_N_4     6  /* from domain */
 #define EDW_REG_D2      7  /* from domain */

 /*---------------------------------------------*/
 /* EC Edw. registers for ScalarMultBase        */
 /* and Signature functions                     */
 /*---------------------------------------------*/
 #define EDW_REG_XS      8 /* out point s */
 #define EDW_REG_YS      9
 #define EDW_REG_ZS     10
 #define EDW_REG_TS     11
 #define EDW_REG_SG4    12 /* precalc. point g4 */
 #define EDW_REG_DG4    13
 #define EDW_REG_PG4    14
 #define EDW_REG_MPG4   15
 /* for special scal.Mult only */
 #define EDW_REG_SG8    16 /* precalc. point g8 */
 #define EDW_REG_DG8    17
 #define EDW_REG_PG8    18
 #define EDW_REG_MPG8   19
 #define EDW_REG_SG16   20 /* precalc. point g16 */
 #define EDW_REG_DG16   21
 #define EDW_REG_PG16   22
 #define EDW_REG_MPG16  23

/* Registers, used in EC EDW Sign and Verify */
 #define EDW_REG_EPH_PRIV  EDW_REG_T3
 #define EDW_REG_INTEGR    EDW_REG_T4
 #define EDW_REG_USER_PRIV EDW_REG_T5
 #define EDW_REG_SIGN      EDW_REG_T6
// #define EDW_REG_TMP       EDW_REG_N_4

  /*------------------------------*/
  /* EC Edw. common registers for */
  /*  ScalarMultBase functions    */
  /*------------------------------*/
#define EDW_REG_SG    EDW_REG_SG8  /* pont g */
#define EDW_REG_DG    EDW_REG_DG8
#define EDW_REG_PG    EDW_REG_PG8
#define EDW_REG_MPG   EDW_REG_MPG8
#define EDW_REG_SG2   EDW_REG_SG16 /* pont g2 */
#define EDW_REG_DG2   EDW_REG_DG16
#define EDW_REG_PG2   EDW_REG_PG16
#define EDW_REG_MPG2  EDW_REG_MPG16

/* for modified points g2, g4: x,y,z=1,t */
#define EDW_REG_XG2   EDW_REG_T3
#define EDW_REG_YG2   EDW_REG_T4
#define EDW_REG_TG2   EDW_REG_T5
#define EDW_REG_XG4   EDW_REG_T6
#define EDW_REG_YG4   28
#define EDW_REG_TG4   29

 /*-------------------------------------------------*/
 /* EC Edw. registers for TwoScalarMultAdd function */
 /*-------------------------------------------------*/
 /*aff. input point P (xp,yp)*/
 #define EDW_REG_XP      8
 #define EDW_REG_YP      9
 /*extended point P: sp,dp,pp*/
 #define EDW_REG_SP     10
 #define EDW_REG_DP     11
 #define EDW_REG_PP     12

 /*extended result point R (xr,yr,zr,tr)*/
 #define EDW_REG_XR     13
 #define EDW_REG_YR     14
 #define EDW_REG_ZR     15
 #define EDW_REG_TR     16
 /*modified form of base point G */
 #define EDW_REG_XG     17
 #define EDW_REG_YG     18
 #define EDW_REG_ZG     19
 #define EDW_REG_TG     20
 /*precalculated form of base point G */
 #define EDW_REG_SG1    21
 #define EDW_REG_DG1    22
 #define EDW_REG_PG1    23
 /* modified point PG=P+G*/
 #define EDW_REG_XPG    24 /*EDW_REG_ZG */
 #define EDW_REG_YPG    25 /*EDW_REG_TG*/
 #define EDW_REG_ZPG    26 /*EDW_REG_XG*/
 #define EDW_REG_TPG    27 /*EDW_REG_TG*/

 /*Registers, used in decompression */
 #define EDW_REG_T      28 /*temp reg.*/
 /* parameters from domain */
 #define EDW_REG_Q58    EDW_REG_XPG
 #define EDW_REG_SQRTM1 EDW_REG_YPG
 #define EDW_REG_D      EDW_REG_ZPG


// #define EC_EDW_PKA_REGS_USED 28 /* besides 2 PKA temp regs. 30,31 */
#endif

// I n, np, t3, t4, t5, t6, n_4, d2, q58, sqrtm1; // 10 regs
//I xg,yg,zg,tg, xg32,yg32,zg32,tg32; // extended coordinates of G and 32G
//I sg, dg, pg, mpg; I sg4, dg4, pg4, mpg4; I sg8, dg8, pg8, mpg8; I sg16, dg16, pg16, mpg16; // precalculated coordinates: y+x, y-x, 2xy, -2xy of G, 4G, 8G, 16G
//void hwsmulg(I xs, I ys, I k)

