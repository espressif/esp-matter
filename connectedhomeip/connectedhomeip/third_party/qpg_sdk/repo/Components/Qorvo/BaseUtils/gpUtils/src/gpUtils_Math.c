/*
 * Copyright (c) 2008-2010, 2012-2016, GreenPeak Technologies
 * Copyright (c) 2017, 2019, Qorvo Inc
 *
 *
 * This file contains the stack features of the Utils component.
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

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#define GP_COMPONENT_ID GP_COMPONENT_ID_UTILS

#include "gpUtils.h"
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
 *                    External Data Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

static UInt64 Utils_binomial_coefficient(UInt8 n, UInt8 k);

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

// We should be able to avoid early overflow by using the multipicative formula instead
// https://en.wikipedia.org/wiki/binomial_coefficient#Multiplicative_formula
UInt64 Utils_binomial_coefficient(UInt8 n, UInt8 k)
{
    return gpUtils_factorial(n) / (gpUtils_factorial(k)*gpUtils_factorial(n-k));
}

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

UInt64 gpUtils_Pow(UInt32 base, UInt16 exp)
{
    UInt64 result = 1;
    switch(exp)
    {
        case 0: result = 1;     break;
        case 1: result = base;  break;
        default:
        {
            UInt16 i;
            result = base;
            for(i = 1; (i< exp); i++)
            {
                result *= base;
            }
        }
        break;
    }

    return result;
}

UInt8 gpUtils_Log2(UInt64 n)
{
    UInt8 count;
    for (count = 0; n>1; count++)
    {
        n >>= 1;
    }
    return count;
}

UInt64 gpUtils_factorial(UInt8 n)
{
    UInt64 prod = 1;
    UInt8 x;
    for (x=1; x<=n; x++)
    {
        prod *= x;
    }
    return prod;
}

UInt64 gpUtils_pow_fractional(UInt8 base_unit, UInt8 exp_int, UInt8 denominator)
{
   UInt64 sum = 0;
   UInt8 base_int_part = base_unit / denominator;
   UInt8 base_fractional_part = base_unit % denominator;

   UInt8 i;
   for (i=0; i<exp_int; ++i)
   {
       if(gpUtils_Pow(denominator,exp_int-i) == 0)
       {

           break;
       }
       sum += Utils_binomial_coefficient(exp_int, i) * gpUtils_Pow(base_int_part,i) * ((UInt64)gpUtils_Pow(base_fractional_part,exp_int-i)/(UInt64)gpUtils_Pow(denominator,exp_int-i));
   }
   return sum;
}

