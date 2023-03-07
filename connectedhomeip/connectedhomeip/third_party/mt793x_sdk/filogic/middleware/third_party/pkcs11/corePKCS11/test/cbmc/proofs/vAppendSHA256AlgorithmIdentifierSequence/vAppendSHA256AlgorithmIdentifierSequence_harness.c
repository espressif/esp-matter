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
 * @file vAppendSHA256AlgorithmIdentifierSequence_harness.c
 * @brief Implements the proof harness for vAppendSHA256AlgorithmIdentifierSequence function.
 */
#include <stddef.h>
#include "core_pkcs11.h"


void harness()
{
    uint8_t * pucHash;
    const uint8_t * pucOid;
    uint32_t ulHashLen;
    uint32_t ulOidLen;

    __CPROVER_assume( ulHashLen >= 32 && ulOidLen >= 51 );
    pucHash = malloc( ulHashLen * sizeof( uint8_t ) );
    pucOid = malloc( ulOidLen * sizeof( uint8_t ) );

    vAppendSHA256AlgorithmIdentifierSequence( pucHash, pucOid );
}
