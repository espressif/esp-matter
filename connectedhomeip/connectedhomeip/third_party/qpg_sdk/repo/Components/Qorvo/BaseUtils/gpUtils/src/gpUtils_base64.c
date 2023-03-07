/*
 * Copyright (c) 2017, 2019, Qorvo Inc
 *
 *   Helper functions to encode/decode Base64
 *
 *   This software is owned by Qorvo Inc
 *   and protected under applicable copyright laws.
 *   It is delivered under the terms of the license
 *   and is intended and supplied for use solely and
 *   exclusively with products manufactured by
 *   Qorvo Inc.
 *
 *
 *   THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 *   CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 *   IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 *   LIMITED TO, IMPLIED WARRANTIES OF
 *   MERCHANTABILITY AND FITNESS FOR A
 *   PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 *   QORVO INC. SHALL NOT, IN ANY
 *   CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 *   INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 *   FOR ANY REASON WHATSOEVER.
 *
 * Alternatively, this software may be distributed under the terms of the
 * modified BSD License or the 3-clause BSD License as published by the Free
 * Software Foundation @ https://directory.fsf.org/wiki/License:BSD-3-Clause
 *
 *   $Header$
 *   $Change$
 *   $DateTime$
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

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/


/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

/*!
 *  \brief   Encode binary data with the Base64 scheme
 *
 *  \param   in_buf   Pointer to the input buffer
 *  \param   in_len   Length of the input buffer
 *  \param   out_buf  Pointer to the output buffer
 *  \param   out_len  Length of the output buffer
 *
 *  \return  0, on success
 *           N, if out_len is less than N or any of the data pointers is NULL
 *           in_len, if N would overflow UInt32
 *
 *  \notes
 *         * N is the minimum size of the output buffer for the given input (string termination included)
 *         * the output buffer is to be allocated/freed by the caller
 */
UInt32 gpUtil_encodeBase64(const UInt8 *in_buf, const UInt32 in_len, char *out_buf, const UInt32 out_len)
{
    const char *encoding_table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    const UInt8 pad_len[] = { 0, 2, 1 };

    const UInt8 *in_end = in_buf + in_len;
    const UInt8 *in_p = in_buf;

    const UInt32 out_min_len = 4 * ((in_len + 2) / 3) + 1;
    const char *out_end = out_buf + out_min_len;
    char *out_p = out_buf;

    /* validate parameters */
    if (in_buf == NULL || out_buf == NULL || out_len < out_min_len || out_min_len < in_len)
    {
        /* failure, return in_len or out_min_len (N), as needed */
        return out_min_len < in_len ? in_len : out_min_len;
    }

    /* fill the whole output buffer with string terminators */
    MEMSET(out_buf, 0, out_len);

    /* data loop: consume three input bytes and produce 4 output bytes */
    while (in_p < in_end)
    {
        UInt32 n = ((UInt32) *in_p++) << 16;

        if (in_p < in_end)
        {
            n += ((UInt32) *in_p++) << 8;
        }

        if (in_p < in_end)
        {
            n += ((UInt32) *in_p++);
        }

        *out_p++ = encoding_table[(n >> 3 * 6) & 0x3f];
        *out_p++ = encoding_table[(n >> 2 * 6) & 0x3f];
        *out_p++ = encoding_table[(n >> 1 * 6) & 0x3f];
        *out_p++ = encoding_table[(n >> 0 * 6) & 0x3f];
    }

    /* fix-up padding if the input size is not 0 modulo 3 */
    out_p = out_buf + out_min_len - pad_len[in_len % 3] - 1;
    while (out_p < out_end - 1)
    {
        *out_p++ = '=';
    }

    /* success */
    return 0;
}
