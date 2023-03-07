/*
 * Copyright (c) 2008-2016, GreenPeak Technologies
 * Copyright (c) 2017-2019, Qorvo Inc
 *
 * gpUtils.h
 *
 * This file contains the API of the Utils component.  This component implements the
 * following features:
 *
 *        - Link Lists (LL)
 *        - Array Lists (AL)
 *        - Circular buffer (CircB)
 *        - Stack control (stack)
 *        - CRC calculation (crc)
 *        - CRC32 calculation (crc32)
 *
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 * Alternatively, this software may be distributed under the terms of the
 * modified BSD License or the 3-clause BSD License as published by the Free
 * Software Foundation @ https://directory.fsf.org/wiki/License:BSD-3-Clause
 *
 * $Header$
 * $Change$
 * $DateTime$
 *
 */


#ifndef _GPUTILS_MATH_H_
#define _GPUTILS_MATH_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/


/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/


/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

/**
 *
 *  This function calculates the power of base^exp (3^9 => 19,683).
 *
 *  @param base         base value
 *  @param exp          exponential value
 *
 *  @note This function is limited to a UInt64.
*/
UInt64 gpUtils_Pow(UInt32 base, UInt16 exp);

/**
 *
 *  This function calculates floor of Log2 of n floor(2^x => n)
 *
 *  @param n            input value
 *
 *  @note This function is limited to a UInt64.
*/
UInt8 gpUtils_Log2(UInt64 n);

/**
 *
 *  This function calculates factorial of n
 *
 *  @param n            input value
 *
 *  @note in math n! example 4! 4*3*2*1 = 24
*/
UInt64 gpUtils_factorial(UInt8 n);


/**
 *
 *  This function is an alternative of powf
 *
 *  @param n            input value
 *
 *  @note Alternative to powf, powd Basically instead of float/double we caculate with ints of size 1/denominator
 *  So base == base_unit/denominator
 *  Since pow(base_unit/denominator, exp) == pow(base_unit,exp)/pow(denominator,exp) we could simply
 *  calculate using the gpUtils_pow function. However, as the numbers get large quickly
 *  (e.g. pow(23,exp)/pow(10,exp)) this quickly leads to overflow.
 *  To keep the numbers small we use the binomial theorem to split the base into it's
 *  whole and fractional constituents (e.g. 2.3  = 2 + 3/10) which
 *  reduces the largest number we have to calculate from pow(23,exp) to pow(10,exp)
 *  This enables us to calculate much larger numbers.
*/
UInt64 gpUtils_pow_fractional(UInt8 base_unit, UInt8 exp_int, UInt8 denominator);

#endif    // _GPUTILS_MATH_H_

