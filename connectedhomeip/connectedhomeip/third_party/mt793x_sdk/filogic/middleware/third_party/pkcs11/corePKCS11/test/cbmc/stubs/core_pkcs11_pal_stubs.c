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
 * @file core_pkcs11_pal_stubs.c
 * @brief Stubs to mock calls to core PKCS #11 PAL.
 */

#include <stddef.h>

#include "core_pkcs11.h"
#include "core_pkcs11_pal.h"

CK_RV PKCS11_PAL_Initialize( void )
{
    CK_RV xResult;

    return xResult;
}

CK_RV PKCS11_PAL_DestroyObject( CK_OBJECT_HANDLE xHandle )
{
    CK_RV xResult;

    __CPROVER_assert( xHandle != CK_INVALID_HANDLE,
                      "Pal destroy should not get an invalid handle." );

    return xResult;
}

CK_RV PKCS11_PAL_GetObjectValue( CK_OBJECT_HANDLE xHandle,
                                 CK_BYTE_PTR * ppucData,
                                 CK_ULONG_PTR pulDataSize,
                                 CK_BBOOL * pIsPrivate )
{
    /* Random Data, the implementation is just going to check that the memory is not zeroed. */
    static CK_BYTE dummyDummyData[ 12 ] = { 0xAB };

    __CPROVER_assert( ppucData != NULL, "ppucData was NULL." );
    __CPROVER_assert( pulDataSize != NULL, "pulDataSize was NULL." );
    __CPROVER_assert( pIsPrivate != NULL, "pIsPrivate was NULL." );
    *ppucData = &dummyDummyData;
    *pulDataSize = sizeof( dummyDummyData );
    nondet_bool() ? ( *pIsPrivate = CK_TRUE ) : ( *pIsPrivate = CK_FALSE );
    return nondet_bool() ? CKR_OK : CKR_FUNCTION_FAILED;
}
