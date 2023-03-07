/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

 #ifndef PKA_EC_MONT_GLOB_REGS_DEF_H
 #define PKA_EC_MONT_GLOB_REGS_DEF_H

 /*! Note: Don't change registers ID-s ! */

 /*! Define global PKA registers ID-s used in EC Montgomry operations */
 /* global regs. */
 #define EC_MONT_REG_N     PKA_REG_N  /* EC mod. */
 #define EC_MONT_REG_NP    PKA_REG_NP  /* EC Barr.tag */
 #define EC_MONT_REG_T     2
 #define EC_MONT_REG_T1    3
 #define EC_MONT_REG_T2    4
 #define EC_MONT_REG_N4    5  /* 4*mod */
 #define EC_MONT_REG_A24   6  /* ec parameter (A+2)/4 */
 /*! scalarmult in/out and local regs. */
 #define EC_MONT_REG_RES   7  /* result point */
 #define EC_MONT_REG_X1    8  /* inputt point */
 #define EC_MONT_REG_X2    9
 #define EC_MONT_REG_Z2   10
 #define EC_MONT_REG_X3   11
 #define EC_MONT_REG_Z3   12

 #define EC_MONT_PKA_REGS_USED 13 /* beside 2 PKA temp regs. */
#endif
