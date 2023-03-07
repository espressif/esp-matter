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
 * @file pkcs11_interface_stubs.c
 * @brief Stubs to mock calls to PKCS #11.
 */

#include <stddef.h>

#include "core_pkcs11.h"
#include "pkcs11.h"


CK_DECLARE_FUNCTION( CK_RV, C_GetSlotList )( CK_BBOOL tokenPresent,
                                             CK_SLOT_ID_PTR pSlotList,
                                             CK_ULONG_PTR pulCount )
{
    int32_t ulCount = nondet_int32();

    /* Most slot lists are less than 10, as it represents an individual HSM. corePKCS11 only
     * has 1 slot (This is allowed and many implementations do this. */
    __CPROVER_assume( ulCount > 0 );
    __CPROVER_assert( pulCount != NULL, "The count pointer can never be NULL." );

    CK_SLOT_ID * pxSlot = malloc( ulCount );

    __CPROVER_assume( pxSlot != NULL );

    if( pSlotList != NULL )
    {
        *pSlotList = pxSlot;
    }

    *pulCount = ulCount;

    return CKR_OK;
}

CK_DECLARE_FUNCTION( CK_RV, C_GetFunctionList )( CK_FUNCTION_LIST_PTR_PTR ppFunctionList )
{
    CK_RV xResult;
    static CK_FUNCTION_LIST prvP11FunctionList =
    {
        { CRYPTOKI_VERSION_MAJOR, CRYPTOKI_VERSION_MINOR },
        nondet_bool() ? C_Initialize : NULL,
        nondet_bool() ? C_Finalize : NULL,
        nondet_bool() ? C_GetInfo : NULL,
        nondet_bool() ? C_GetFunctionList : NULL,
        nondet_bool() ? C_GetSlotList : NULL,
        NULL, /*C_GetSlotInfo*/
        nondet_bool() ? C_GetTokenInfo : NULL,
        NULL, /*C_GetMechanismList*/

        NULL, /*C_InitPIN*/
        nondet_bool() ? C_InitToken : NULL,
        NULL, /*C_InitPIN*/
        NULL, /*C_SetPIN*/
        nondet_bool() ? C_OpenSession : NULL,
        NULL,
        NULL, /*C_CloseAllSessions*/
        NULL, /*C_GetSessionInfo*/
        NULL, /*C_GetOperationState*/
        NULL, /*C_SetOperationState*/
        nondet_bool() ? C_Login : NULL,
        NULL, /*C_Logout*/
        nondet_bool() ? C_CreateObject : NULL,
        NULL, /*C_CopyObject*/
        nondet_bool() ? C_DestroyObject : NULL,
        NULL, /*C_GetObjectSize*/
        nondet_bool() ? C_GetAttributeValue : NULL,
        NULL, /*C_SetAttributeValue*/
        nondet_bool() ? C_FindObjectsInit : NULL,
        nondet_bool() ? C_FindObjects : NULL,
        nondet_bool() ? C_FindObjectsFinal : NULL,
        NULL, /*C_EncryptInit*/
        NULL, /*C_Encrypt*/
        NULL, /*C_EncryptUpdate*/
        NULL, /*C_EncryptFinal*/
        NULL, /*C_DecryptInit*/
        NULL, /*C_Decrypt*/
        NULL, /*C_DecryptUpdate*/
        NULL, /*C_DecryptFinal*/
        nondet_bool() ? C_FindObjectsFinal : NULL,
        NULL, /*C_Digest*/
        nondet_bool() ? C_DigestUpdate : NULL,
        NULL, /* C_DigestKey*/
        nondet_bool() ? C_DigestFinal : NULL,
        nondet_bool() ? C_SignInit : NULL,
        nondet_bool() ? C_Sign : NULL,
        NULL, /*C_SignUpdate*/
        NULL, /*C_SignFinal*/
        NULL, /*C_SignRecoverInit*/
        NULL, /*C_SignRecover*/
        nondet_bool() ? C_VerifyInit : NULL,
        nondet_bool() ? C_Verify : NULL,
        NULL, /*C_VerifyUpdate*/
        NULL, /*C_VerifyFinal*/
        NULL, /*C_VerifyRecoverInit*/
        NULL, /*C_VerifyRecover*/
        NULL, /*C_DigestEncryptUpdate*/
        NULL, /*C_DecryptDigestUpdate*/
        NULL, /*C_SignEncryptUpdate*/
        NULL, /*C_DecryptVerifyUpdate*/
        NULL, /*C_GenerateKey*/
        nondet_bool() ? C_GenerateKeyPair : NULL,
        NULL, /*C_WrapKey*/
        NULL, /*C_UnwrapKey*/
        NULL, /*C_DeriveKey*/
        NULL, /*C_SeedRandom*/
        nondet_bool() ? C_GenerateRandom : NULL,
        NULL, /*C_GetFunctionStatus*/
        NULL, /*C_CancelFunction*/
        NULL  /*C_WaitForSlotEvent*/
    };

    if( xResult == CKR_OK )
    {
        *ppFunctionList = &prvP11FunctionList;
    }

    return xResult;
}
