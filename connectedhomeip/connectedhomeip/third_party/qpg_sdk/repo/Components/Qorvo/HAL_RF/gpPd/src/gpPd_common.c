/*
 * Copyright (c) 2015-2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
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

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

INLINE static void Pd_AppendWithUpdate(gpPd_Loh_t *pPdLoh, UInt8 length, UInt8 const *pData)
{
    if (pPdLoh && length && pData)
    {
        if (length == 1)
        {
            Pd_WriteByte(pPdLoh->handle, pPdLoh->offset + pPdLoh->length, *pData);
        }
        else
        {
            Pd_WriteByteStream(pPdLoh->handle, pPdLoh->offset + pPdLoh->length, length, (UInt8*)pData);
        }
        pPdLoh->length += length;
    }
}
INLINE static void Pd_PrependWithUpdate(gpPd_Loh_t *pPdLoh, UInt8 length, UInt8 const *pData)
{
    if (pPdLoh && length && pData)
    {
        pPdLoh->offset -= length;
        if (length == 1)
        {
            Pd_WriteByte(pPdLoh->handle, pPdLoh->offset, *pData);
        }
        else
        {
            Pd_WriteByteStream(pPdLoh->handle, pPdLoh->offset, length, (UInt8*)pData);
        }
        pPdLoh->length += length;
    }
}

INLINE static void Pd_ReadWithUpdate(gpPd_Loh_t *pPdLoh, UInt8 length, UInt8 *pData)
{
    if (pPdLoh && length && pData)
    {
        if (length == 1)
        {
            *pData = Pd_ReadByte(pPdLoh->handle, pPdLoh->offset);
        }
        else
        {
            Pd_ReadByteStream(pPdLoh->handle, pPdLoh->offset, length, pData);
        }
        pPdLoh->offset += length;
        pPdLoh->length -= length;
    }
}
