/*
 * corePKCS v2.0.0
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
 */

/**
 * @file core_pki_utils_stubs.c
 * @brief Stubs to mock calls to core PKCS #11 utils.
 */
#include "core_pki_utils.h"

int8_t PKI_mbedTLSSignatureToPkcs11Signature( uint8_t * pxSignaturePKCS,
                                              const uint8_t * pxMbedSignature )
{
    __CPROVER_assert( __CPROVER_OBJECT_SIZE( pxSignaturePKCS ) >= 64, "Signature was an incorrect size." );
    __CPROVER_assert( __CPROVER_OBJECT_SIZE( pxMbedSignature ) >= 72, "mbed return buffer was an incorrect size." );
    return nondet_bool() ? 0 : -1;
}
