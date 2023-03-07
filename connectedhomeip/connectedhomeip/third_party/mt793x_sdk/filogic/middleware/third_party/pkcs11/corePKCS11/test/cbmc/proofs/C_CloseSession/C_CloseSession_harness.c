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
 * @file C_CloseSession_harness.c
 * @brief Implements the proof harness for C_CloseSession function.
 */

#include <stddef.h>
#include "core_pkcs11.h"
#include "core_pkcs11_config.h"

void harness()
{
    CK_RV xResult;
    CK_FLAGS xFlags;
    CK_SESSION_HANDLE * pxSession = malloc( sizeof( CK_SESSION_HANDLE ) );

    /* Cover the case when the corePKCS11 module is not already initialized. */
    if( pxSession != NULL )
    {
        ( void ) C_CloseSession( *pxSession );
    }

    xResult = C_Initialize( NULL );
    __CPROVER_assume( xResult == CKR_OK );

    xResult = C_OpenSession( 0, xFlags, NULL, 0, pxSession );

    if( xResult == CKR_OK )
    {
        __CPROVER_assert( *pxSession > CK_INVALID_HANDLE && *pxSession <= pkcs11configMAX_SESSIONS, "For the C_OpenSession result to "
                                                                                                    "be CKR_OK, we expect the session handle to be a valid value." );
    }

    if( pxSession != NULL )
    {
        ( void ) C_CloseSession( *pxSession );
    }
}
