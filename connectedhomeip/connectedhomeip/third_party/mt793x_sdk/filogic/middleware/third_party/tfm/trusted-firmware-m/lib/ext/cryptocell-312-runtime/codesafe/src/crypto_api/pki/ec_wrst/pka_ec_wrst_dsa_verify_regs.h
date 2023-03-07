/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef PKA_ECDSA_VERIFY_REGS_DEF_H
#define PKA_ECDSA_VERIFY_REGS_DEF_H

/* Definition of ECDSA Verify specific registers */
#define EC_VERIFY_REG_R     PKA_REG_N
#define EC_VERIFY_REG_NR    PKA_REG_NP
#define EC_VERIFY_REG_F     2
#define EC_VERIFY_REG_D     3
#define EC_VERIFY_REG_H     4
#define EC_VERIFY_REG_TMP   5
#define EC_VERIFY_REG_XPQ 14
#define EC_VERIFY_REG_YPQ 15
#define EC_VERIFY_REG_ZR  16
#define EC_VERIFY_REG_TR  17
#define EC_VERIFY_REG_H1   18
#define EC_VERIFY_REG_H2   19
#define EC_VERIFY_REG_P_GX  20
#define EC_VERIFY_REG_P_GY  21
#define EC_VERIFY_REG_P_WX  22
#define EC_VERIFY_REG_P_WY  23
#define EC_VERIFY_REG_P_RX  24
#define EC_VERIFY_REG_P_RY  25
#define EC_VERIFY_REG_TMP_N  26
#define EC_VERIFY_REG_TMP_NP 27
#define EC_VERIFY_REG_C    28
#endif
