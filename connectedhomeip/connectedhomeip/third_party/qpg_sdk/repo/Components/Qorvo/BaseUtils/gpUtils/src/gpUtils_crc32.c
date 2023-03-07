/*
 * Copyright (c) 2011, 2013-2016, GreenPeak Technologies
 * Copyright (c) 2017-2018, Qorvo Inc
 *
 * gpUtils_crc.c
 *
 * This file contains the crc features of the Utils component.
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
#include "gpLog.h"

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

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

#define _crc32_init(crc) /*No init of crc HW */
static INLINE UInt32 _crc32_update(UInt32 crc, UInt8 a)
{
    UIntLoop j;

    crc= crc ^ a;
    for (j=0; j<8; j++)
    {
        if (crc & 1)
            crc = (crc>>1) ^ 0xEDB88320 ;
        else
            crc = crc >>1 ;
    }

    return crc;
}

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void gpUtils_CalculatePartialCrc32 (UInt32* pCrcValue, UInt8* pData, UInt32 length)
{
    _crc32_init(*pCrcValue);
    while(length--)
    {
        *pCrcValue = _crc32_update(*pCrcValue, *pData);
        pData++;
    }
}

UInt32 gpUtils_CalculateCrc32(UInt8* pData , UInt32 length)
{
    UInt32 crc_value = GP_UTILS_CRC32_INITIAL_REMAINDER;
    gpUtils_CalculatePartialCrc32(&crc_value, pData, length);

    return (crc_value ^ GP_UTILS_CRC32_FINAL_XOR_VALUE);
}

void gpUtils_UpdateCrc32(UInt32* pCRCValue, UInt8 byte)
{
    gpUtils_CalculatePartialCrc32(pCRCValue, &byte, 1);
}

