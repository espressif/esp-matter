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
 * @file C_DestroyObject_harness.c
 * @brief Implements the proof harness for C_DestroyObject function.
 */
#include "mbedtls/ecp.h"
#include "mbedtls/oid.h"
#include "mbedtls/sha256.h"
#include "mbedtls/pk.h"
#include "core_pkcs11_config.h"
#include "core_pkcs11.h"

/* Internal struct for corePKCS11 mbed TLS implementation, but we don't really care what it contains
 * in this proof.
 *
 * It is just copied over from "core_pkcs11_mbedtls.c" so the structure is correct.
 */
typedef struct P11Session
{
    CK_ULONG ulState;
    CK_BBOOL xOpened;
    CK_MECHANISM_TYPE xOperationDigestMechanism;
    CK_BYTE * pxFindObjectLabel;
    CK_ULONG xFindObjectLabelLen;
    CK_MECHANISM_TYPE xOperationVerifyMechanism;
    mbedtls_threading_mutex_t xVerifyMutex;
    CK_OBJECT_HANDLE xVerifyKeyHandle;
    mbedtls_pk_context xVerifyKey;
    CK_MECHANISM_TYPE xOperationSignMechanism;
    mbedtls_threading_mutex_t xSignMutex;
    CK_OBJECT_HANDLE xSignKeyHandle;
    mbedtls_pk_context xSignKey;
    mbedtls_sha256_context xSHA256Context;
} P11Session_t;

CK_RV __CPROVER_file_local_core_pkcs11_mbedtls_c_prvCheckValidSessionAndModule( const P11Session_t * pxSession )
{
    CK_RV xResult;

    __CPROVER_assert( pxSession != NULL, "pxSession was NULL." );
    return xResult;
}

void __CPROVER_file_local_core_pkcs11_mbedtls_c_prvFindObjectInListByHandle( CK_OBJECT_HANDLE xAppHandle,
                                                                             CK_OBJECT_HANDLE_PTR pxPalHandle,
                                                                             CK_BYTE_PTR * ppcLabel,
                                                                             CK_ULONG_PTR pxLabelLength )
{
    CK_OBJECT_HANDLE handle;
    CK_ULONG xLen;

    __CPROVER_assert( pxPalHandle != NULL, "ppcLabel was NULL." );
    __CPROVER_assert( ppcLabel != NULL, "ppcLabel was NULL." );
    __CPROVER_assert( pxLabelLength != NULL, "ppcLabel was NULL." );

    __CPROVER_assume( handle < MAX_OBJECT_NUM );
    *pxPalHandle = handle;

    *ppcLabel = malloc( xLen );
    *pxLabelLength = xLen;
}

void harness()
{
    CK_SESSION_HANDLE hSession;
    CK_OBJECT_HANDLE hObject;
    CK_RV xResult;

    /* We need to populate the PKCS module with mutexes. Rather than stubbing out
     * some critical paths, we can just initialize the module.
     */
    xResult = C_Initialize( NULL );
    __CPROVER_assume( xResult == CKR_OK );

    __CPROVER_assume( ( hSession > CK_INVALID_HANDLE ) &&
                      ( hSession <= pkcs11configMAX_SESSIONS ) );
    ( void ) C_DestroyObject( hSession, hObject );
}
