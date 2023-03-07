/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef PKA_POINT_COMPRESS_REGS_DEF_H
#define PKA_POINT_COMPRESS_REGS_DEF_H

/*stack*/
#define PKA_REG_X     2
#define PKA_REG_Y     3
#define PKA_REG_EC_A  4
#define PKA_REG_EC_B  5

/*Square root*/
/*in*/
#define PKA_REG_Y1    PKA_REG_Y    //zQ
#define PKA_REG_Y2    PKA_REG_EC_A //zN
/*stack*/
#define PKA_REG_T     6   //zT
#define PKA_REG_Z     7   //zZ
#define PKA_REG_EX    8   //zEx
#define PKA_REG_YT    9   //zYt

/* Jacobi symbol */
/*in*/
#define PKA_REG_A    10   //za
#define PKA_REG_B    11   //zb
/*stack*/
#define PKA_REG_C    12   //zc

#endif
