/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef PKA_ECDSA_SIGN_REGS_DEF_H
#define PKA_ECDSA_SIGN_REGS_DEF_H

  /* PkaScalarMultSca regs */
  #define EC_SIGN_REG_X2  12
  #define EC_SIGN_REG_Y2  13
  #define EC_SIGN_REG_Z2  14
  #define EC_SIGN_REG_T2  15
  #define EC_SIGN_REG_X4  16
  #define EC_SIGN_REG_Y4  17
  #define EC_SIGN_REG_Z4  18
  #define EC_SIGN_REG_T4  19
  #define EC_SIGN_REG_XS  20
  #define EC_SIGN_REG_YS  21
  #define EC_SIGN_REG_ZS  22
  #define EC_SIGN_REG_TS  23
  #define EC_SIGN_REG_ZP  24
  #define EC_SIGN_REG_TP  25
  #define EC_SIGN_REG_ZR  26
  /* k, p[in/out] */
  #define EC_SIGN_REG_ORD 26 /*=EC_SIGN_REG_ZR, used for EC order*/
  #define EC_SIGN_REG_RK  27 /*scalar*/
  #define EC_SIGN_REG_XP  28 /*in/out*/
  #define EC_SIGN_REG_YP  29 /*in/out*/
#endif
