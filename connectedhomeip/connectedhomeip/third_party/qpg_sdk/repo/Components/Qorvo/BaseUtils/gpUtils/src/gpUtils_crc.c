/*
 * Copyright (c) 2008-2013, 2015-2016, GreenPeak Technologies
 * Copyright (c) 2017-2019, Qorvo Inc
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
#include "gpAssert.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#define GPUTILS_CRC8_POLYNOMIAL 0x139 // http://checksumcrc.blogspot.com/2012/01/crc-for-protecting-single-value.html
#define GPUTILS_CRC8_PADDING 0xff

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

//CRC-16-IBM - x16 + x15 + x2 + 1
//#define GP_UTILS_DIVERSITY_CRC_TABLE
static INLINE UInt16 _crc16_update(UInt16 crc, UInt8 a)
{
    int i;

    crc ^= a;
    for (i = 0; i < 8; ++i)
    {
        if (crc & 1)
            crc = (crc >> 1) ^ 0xA001;
        else
            crc = (crc >> 1);
    }

    return crc;
}

static INLINE UInt16 _crc16_kermit_update(UInt16 crc, UInt8 a)
{
    UInt8 i;
    UInt16 tmp;
    UInt16 c;

    tmp = 0;
    c = crc ^ a;
    for (i = 0; i < 8; i++)
    {
        if ((tmp ^ c) & 0x0001)
        {
            tmp = (tmp >> 1) ^ 0x8408;
        }
        else
        {
            tmp = tmp >> 1;
        }

        c = c >> 1;
    }
    crc = (crc >> 8) ^ tmp;

    return crc;
}

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void gpUtils_CalculatePartialCrc(UInt16* pCrcValue, UInt8* pData, UInt16 length)
{
    while(length--)
    {
        *pCrcValue = _crc16_update(*pCrcValue, *pData);
        pData++;
    }
}

UInt16 gpUtils_CalculateCrc(UInt8* pData , UInt16 length)
{
    UInt16 crc_value = 0;
    gpUtils_CalculatePartialCrc(&crc_value, pData, length);

    return crc_value;
}

void gpUtils_UpdateCrc(UInt16* pCRCValue, UInt8 byte)
{
    gpUtils_CalculatePartialCrc(pCRCValue, &byte, 1);
}

void gpUtils_CalculatePartialKermitCrc(UInt16* pCrcValue, UInt8* pData, UInt16 length)
{
    while (length--)
    {
        *pCrcValue = _crc16_kermit_update(*pCrcValue, *pData++);
    }
}

UInt16 gpUtils_CalculateKermitCrc(UInt8 *pData, UInt16 length)
{
    UInt16 crc_value = 0;
    gpUtils_CalculatePartialKermitCrc(&crc_value, pData, length);

    return crc_value;
}

UInt8 gpUtils_CalculateCrc8(UInt8 data)
{
    UInt16 crc = (data << 8) | GPUTILS_CRC8_PADDING;
    UInt16 divisor = GPUTILS_CRC8_POLYNOMIAL << 7;
    UInt16 i;

    for(i = 15 ; i >= 8 ; i--)
    {
        if(crc & (1<<i))
        {
            crc ^= divisor;
        }
        divisor = divisor >> 1;
    }

    return crc & 0xff;
}
