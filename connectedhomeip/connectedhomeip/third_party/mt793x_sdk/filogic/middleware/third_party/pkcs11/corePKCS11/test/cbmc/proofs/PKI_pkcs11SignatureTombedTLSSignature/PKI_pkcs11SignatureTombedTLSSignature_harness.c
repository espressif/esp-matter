/*
 * corePKCS11 V3.0.1
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/**
 * @file PKI_pkcs11SignatureTombedTLSSignature_harness.c
 * @brief Implements the proof harness for PKI_pkcs11SignatureTombedTLSSignature function.
 */

#include "core_pki_utils.h"

#define mbedtlsEncodedBufferLen    72

void harness()
{
    int8_t ucReturn;
    uint8_t * pucSig;
    uint32_t ulSigLen;
    size_t ulReturnLen;

    __CPROVER_assume( ulSigLen >= mbedtlsEncodedBufferLen );
    pucSig = malloc( ulSigLen * sizeof( uint8_t ) );

    ucReturn = PKI_pkcs11SignatureTombedTLSSignature( pucSig, &ulReturnLen );

    if( ucReturn != -1 )
    {
        __CPROVER_assert( ulReturnLen <= mbedtlsEncodedBufferLen, "The signature was larger than expected." );
    }
}
