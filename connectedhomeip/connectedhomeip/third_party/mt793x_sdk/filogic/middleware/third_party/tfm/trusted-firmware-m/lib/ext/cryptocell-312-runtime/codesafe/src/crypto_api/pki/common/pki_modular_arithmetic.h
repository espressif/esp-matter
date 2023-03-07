/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef _PKI_MODULAR_ARITHMETIC_H
#define _PKI_MODULAR_ARITHMETIC_H

#include "cc_pal_types.h"

#ifdef __cplusplus
extern "C"
{
#endif



int32_t PkiCalcJacobiSymbol(void);
bool PkiCalcModSquareRoot(void);



#ifdef __cplusplus
}
#endif

#endif
