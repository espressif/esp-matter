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
 * @file C_GetSlotList_harness.c
 * @brief Implements the proof harness for C_GetSlotList function.
 */

#include <stddef.h>
#include "core_pkcs11.h"

void harness()
{
    CK_BBOOL xToken;
    CK_ULONG * pulSlotSize = malloc( sizeof( CK_ULONG ) );
    CK_SLOT_ID_PTR pxSlot;
    CK_RV xResult;

    if( pulSlotSize != NULL )
    {
        /* Multiplication overflow protection for the harness. */
        __CPROVER_assume( sizeof( CK_SLOT_ID ) == *pulSlotSize / sizeof( CK_SLOT_ID ) );
        pxSlot = malloc( sizeof( CK_SLOT_ID ) * *pulSlotSize );
    }

    /* Check case for uninitialized stack. */
    ( void ) C_GetSlotList( xToken, pxSlot, pulSlotSize );

    /* Respect the API contract. PKCS #11 MUST be initialized before getting a slot. */
    xResult = C_Initialize( NULL );
    __CPROVER_assume( xResult == CKR_OK );

    ( void ) C_GetSlotList( xToken, pxSlot, pulSlotSize );
}
