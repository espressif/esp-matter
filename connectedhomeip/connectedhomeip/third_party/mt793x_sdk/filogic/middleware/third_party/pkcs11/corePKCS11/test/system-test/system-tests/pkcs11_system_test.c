/*
 * AWS IoT Device SDK for Embedded C 202012.01
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

/* Standard includes. */
#include <stdlib.h>
#include <string.h>

#ifdef MTK_OS_CLI_ENABLE
#include "os_cli.h"
#endif /* MTK_OS_CLI_ENABLE */

/* PKCS includes. */
#include "core_pki_utils.h"
#include "core_pkcs11_config.h"
#include "core_pkcs11.h"
#include "core_test_pkcs11_config.h"

/* Test includes. */
#include "unity.h"

/* mbedTLS includes. */
#include "mbedtls/sha256.h"
#include "mbedtls/pk.h"
#include "mbedtls/pk_internal.h"
#include "mbedtls/oid.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy_poll.h"
#include "mbedtls/x509_crt.h"

/* Length parameters for importing RSA-2048 private keys. */
#define MODULUS_LENGTH              pkcs11RSA_2048_MODULUS_BITS / 8
#define E_LENGTH                    3
#define D_LENGTH                    pkcs11RSA_2048_MODULUS_BITS / 8
#define PRIME_1_LENGTH              128
#define PRIME_2_LENGTH              128
#define EXPONENT_1_LENGTH           128
#define EXPONENT_2_LENGTH           128
#define COEFFICIENT_LENGTH          128
#define PUB_EXP_LENGTH              3

#define CERTIFICATE_VALUE_LENGTH    949
#define pkcstestRAND_BUFFER_SIZE    10
#define EC_PARAMS_LENGTH            10
#define EC_D_LENGTH                 32

/* See core_pkcs11_mbedtls.c for length explanation. */
#define pkcs11EC_POINT_LENGTH       ( ( 32UL * 2UL ) + 1UL + 1UL + 1UL )

typedef struct RsaParams_t
{
    CK_BYTE modulus[ MODULUS_LENGTH + 1 ];
    CK_BYTE e[ E_LENGTH + 1 ];
    CK_BYTE d[ D_LENGTH + 1 ];
    CK_BYTE prime1[ PRIME_1_LENGTH + 1 ];
    CK_BYTE prime2[ PRIME_2_LENGTH + 1 ];
    CK_BYTE exponent1[ EXPONENT_1_LENGTH + 1 ];
    CK_BYTE exponent2[ EXPONENT_2_LENGTH + 1 ];
    CK_BYTE coefficient[ COEFFICIENT_LENGTH + 1 ];
} RsaParams_t;

/* PKCS #11 Globals.
 * These are used to reduce setup and tear down calls, and to
 * prevent memory leaks in the case of TEST_PROTECT() actions being triggered. */
CK_SESSION_HANDLE globalSession = 0;
CK_FUNCTION_LIST_PTR globalFunctionList = NULL_PTR;
CK_SLOT_ID globalSlotId = 0;
CK_MECHANISM_TYPE globalMechanismType = 0;


/* PKCS #11 Global Data Containers. */
CK_BYTE rsaHashedMessage[ pkcs11SHA256_DIGEST_LENGTH ] = { 0 };
CK_BYTE ecdsaSignature[ pkcs11RSA_2048_SIGNATURE_LENGTH ] = { 0x00 };
CK_BYTE ecdsaHashedMessage[ pkcs11SHA256_DIGEST_LENGTH ] = { 0xab };

/*-----------------------------------------------------------*/

/* Import the specified ECDSA private key into storage. */
static CK_RV provisionPrivateECKey( CK_SESSION_HANDLE session,
                                    uint8_t * label,
                                    CK_OBJECT_HANDLE_PTR objectHandle,
                                    mbedtls_pk_context * mbedPkContext );

/* Import the specified RSA private key into storage. */
static CK_RV provisionPrivateRSAKey( CK_SESSION_HANDLE session,
                                     uint8_t * label,
                                     CK_OBJECT_HANDLE_PTR objectHandle,
                                     mbedtls_pk_context * mbedPkContext );

/* Import the specified private key into storage. */
static CK_RV provisionPrivateKey( CK_SESSION_HANDLE session,
                                  uint8_t * privateKey,
                                  size_t privateKeyLength,
                                  uint8_t * label,
                                  CK_OBJECT_HANDLE_PTR objectHandle );

/* Import the specified public key into storage. */
static CK_RV provisionPublicKey( CK_SESSION_HANDLE session,
                                 uint8_t * keyPtr,
                                 size_t keyLength,
                                 CK_KEY_TYPE publicKeyType,
                                 uint8_t * publicKeyLabel,
                                 CK_OBJECT_HANDLE_PTR publicKeyHandlePtr );

static CK_RV generateKeyPairEC( CK_SESSION_HANDLE session,
                                uint8_t * privateKeyLabel,
                                uint8_t * publicKeyLabel,
                                CK_OBJECT_HANDLE_PTR privateKeyHandlePtr,
                                CK_OBJECT_HANDLE_PTR publicKeyHandlePtr );

/*-----------------------------------------------------------*/

/* Import the specified X.509 client certificate into storage. */
static CK_RV provisionCertificate( CK_SESSION_HANDLE session,
                                   uint8_t * certificate,
                                   size_t certificateLength,
                                   uint8_t * label,
                                   CK_OBJECT_HANDLE_PTR objectHandle );

/*-----------------------------------------------------------*/

/* Delete the specified crypto object from storage. */
static CK_RV destroyProvidedObjects( CK_SESSION_HANDLE session,
                                     CK_BYTE_PTR * pkcsLabelsPtr,
                                     CK_OBJECT_CLASS * class,
                                     CK_ULONG count );
/*-----------------------------------------------------------*/

/* ============================   UNITY FIXTURES ============================ */
/* Called before each test method. */
void setUp()
{
    CK_RV result;

    result = C_GetFunctionList( &globalFunctionList );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "Failed to get PKCS #11 function list." );

    result = xInitializePKCS11();

    if( result != CKR_CRYPTOKI_ALREADY_INITIALIZED )
    {
        TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "Failed to initialize PKCS #11 module." );
    }

    result = xInitializePkcs11Session( &globalSession );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "Failed to open PKCS #11 session." );
}

/* Called after each test method. */
void tearDown()
{
    CK_RV result;

    result = globalFunctionList->C_CloseSession( globalSession );

    if( result != CKR_CRYPTOKI_NOT_INITIALIZED )
    {
        TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "Failed to close session." );

        result = globalFunctionList->C_Finalize( NULL );
        TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "Failed to finalize session." );
    }
}

/* ========================== Test Cases ============================ */

/* Delete well-known crypto objects from storage. */

static CK_RV destroyTestCredentials( void )
{
    CK_RV result = CKR_OK;
    CK_RV destroyResult = CKR_OK;
    CK_OBJECT_HANDLE object = CK_INVALID_HANDLE;
    CK_ULONG labelCount = 0;

    CK_BYTE * pkcsLabels[] =
    {
        ( CK_BYTE * ) pkcs11testLABEL_DEVICE_CERTIFICATE_FOR_TLS,
        ( CK_BYTE * ) pkcs11testLABEL_DEVICE_PRIVATE_KEY_FOR_TLS,
        ( CK_BYTE * ) pkcs11testLABEL_DEVICE_PUBLIC_KEY_FOR_TLS,
        #if ( pkcs11configJITP_CODEVERIFY_ROOT_CERT_SUPPORTED == 1 )
            ( CK_BYTE * ) pkcs11testLABEL_CODE_VERIFICATION_KEY,
            ( CK_BYTE * ) pkcs11testLABEL_JITP_CERTIFICATE,
            ( CK_BYTE * ) pkcs11testLABEL_ROOT_CERTIFICATE
        #endif
    };
    CK_OBJECT_CLASS class[] =
    {
        CKO_CERTIFICATE,
        CKO_PRIVATE_KEY,
        CKO_PUBLIC_KEY,
        #if ( pkcs11configJITP_CODEVERIFY_ROOT_CERT_SUPPORTED == 1 )
            CKO_PUBLIC_KEY,
            CKO_CERTIFICATE,
            CKO_CERTIFICATE
        #endif
    };

    destroyResult = destroyProvidedObjects( globalSession,
                                            pkcsLabels,
                                            class,
                                            sizeof( class ) / sizeof( CK_OBJECT_CLASS ) );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "Failed to destroy credentials." );

    for( labelCount = 0;
         labelCount < sizeof( class ) / sizeof( CK_OBJECT_CLASS );
         labelCount++ )
    {
        result = xFindObjectWithLabelAndClass( globalSession,
                                               ( char * ) pkcsLabels[ labelCount ],
                                               strlen( ( char * ) pkcsLabels[ labelCount ] ),
                                               class[ labelCount ],
                                               &object );
        TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "Found an object after deleting it.\r\n" );
        TEST_ASSERT_EQUAL_MESSAGE( CK_INVALID_HANDLE, object, "Object found after it was destroyed.\r\n" );
    }

    return destroyResult;
}

/* Assumes that device is already provisioned at time of calling. */
static void findObjectTest( CK_OBJECT_HANDLE_PTR privateKeyHandlePtr,
                            CK_OBJECT_HANDLE_PTR pcertificateHandle,
                            CK_OBJECT_HANDLE_PTR publicKeyHandlePtr )
{
    CK_RV result;
    CK_OBJECT_HANDLE testObjectHandle = CK_INVALID_HANDLE;

    /* Happy Path - Find a previously created object. */
    result = xFindObjectWithLabelAndClass( globalSession,
                                           pkcs11testLABEL_DEVICE_PRIVATE_KEY_FOR_TLS,
                                           sizeof( pkcs11testLABEL_DEVICE_PRIVATE_KEY_FOR_TLS ),
                                           CKO_PRIVATE_KEY,
                                           privateKeyHandlePtr );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "Failed to find private key after closing and reopening a session." );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( CK_INVALID_HANDLE, *privateKeyHandlePtr, "Invalid object handle found for  private key." );

    /* TODO: Add the code sign key and root ca. */
    result = xFindObjectWithLabelAndClass( globalSession,
                                           pkcs11testLABEL_DEVICE_PUBLIC_KEY_FOR_TLS,
                                           sizeof( pkcs11testLABEL_DEVICE_PUBLIC_KEY_FOR_TLS ),
                                           CKO_PUBLIC_KEY,
                                           publicKeyHandlePtr );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "Failed to find public key after closing and reopening a session." );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( CK_INVALID_HANDLE, *publicKeyHandlePtr, "Invalid object handle found for public key key." );


    result = xFindObjectWithLabelAndClass( globalSession,
                                           pkcs11testLABEL_DEVICE_CERTIFICATE_FOR_TLS,
                                           sizeof( pkcs11testLABEL_DEVICE_CERTIFICATE_FOR_TLS ),
                                           CKO_CERTIFICATE,
                                           pcertificateHandle );

    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "Failed to find certificate after closing and reopening a session." );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( CK_INVALID_HANDLE, *pcertificateHandle, "Invalid object handle found for certificate." );

    /* Try to find an object that has never been created. */
    result = xFindObjectWithLabelAndClass( globalSession,
                                           ( char * ) "This label doesn't exist",
                                           sizeof( "This label doesn't exist" ),
                                           CKO_PUBLIC_KEY,
                                           &testObjectHandle );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "Incorrect error code finding object that doesn't exist" );
    TEST_ASSERT_EQUAL_MESSAGE( CK_INVALID_HANDLE, testObjectHandle, "Incorrect error code finding object that doesn't exist" );
}


/*-----------------------------------------------------------*/

void test_Get_Function_List( void )
{
    CK_FUNCTION_LIST_PTR functionList = NULL;

    TEST_ASSERT_EQUAL( CKR_ARGUMENTS_BAD, C_GetFunctionList( NULL ) );

    TEST_ASSERT_EQUAL( CKR_OK, C_GetFunctionList( &functionList ) );

    /* Ensure that functionList was changed by C_GetFunctionList. */
    TEST_ASSERT_NOT_EQUAL( NULL, functionList );
}

void test_Initialize_Finalize( void )
{
    CK_FUNCTION_LIST_PTR functionList = NULL;
    CK_RV result;

    result = C_GetFunctionList( &functionList );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "Failed to get function list." );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( NULL, functionList, "Invalid function list pointer." );

    /* Set up will initialize, get it back to a known state. */
    result = functionList->C_Finalize( NULL );

    result = xInitializePKCS11();
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "Failed to initialize PKCS #11 module." );

    if( TEST_PROTECT() )
    {
        /* Call initialize a second time.  Since this call may be made many times,
         * it is important that PKCS #11 implementations be tolerant of multiple calls. */
        result = xInitializePKCS11();
        TEST_ASSERT_EQUAL_MESSAGE( CKR_CRYPTOKI_ALREADY_INITIALIZED, result, "Second PKCS #11 module initialization. " );

        /* C_Finalize should fail if pReserved isn't NULL. */
        result = functionList->C_Finalize( ( CK_VOID_PTR ) 0x1234 );
        TEST_ASSERT_EQUAL_MESSAGE( CKR_ARGUMENTS_BAD, result, "Negative Test: Finalize with invalid argument" );
    }

    result = functionList->C_Finalize( NULL );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "Finalize failed" );
}

void test_GetSlotList( void )
{
    CK_RV result;
    CK_SLOT_ID * slotIdPtr = NULL;
    CK_ULONG slotCount = 0;
    CK_ULONG extraSlotCount = 0;

    if( TEST_PROTECT() )
    {
        /* The Happy Path. */

        /* When a NULL slot pointer is passed in,
         *  the number of slots should be updated. */
        result = globalFunctionList->C_GetSlotList( CK_TRUE, NULL, &slotCount );
        TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "Failed to get slot count" );
        TEST_ASSERT_GREATER_THAN_MESSAGE( 0, slotCount, "Slot count incorrectly updated" );

        /* Allocate memory to receive the list of slots, plus one extra. */
        slotIdPtr = PKCS11_MALLOC( sizeof( CK_SLOT_ID ) * ( slotCount + 1 ) );
        TEST_ASSERT_NOT_EQUAL_MESSAGE( NULL, slotIdPtr, "Failed malloc memory for slot list" );

        /* Call C_GetSlotList again to receive all slots with tokens present. */
        result = globalFunctionList->C_GetSlotList( CK_TRUE, slotIdPtr, &slotCount );
        TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "Failed to get slot count" );

        /* Note: Being able to use the slot to open a session will be  tested
         * in the C_OpenSession tests. */

        /* Off the happy path. */
        extraSlotCount = slotCount + 1;

        /* Make sure that number of slots returned is updated when extra buffer room exists. */
        result = globalFunctionList->C_GetSlotList( CK_TRUE, slotIdPtr, &extraSlotCount );
        TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "Failed to get slot list" );
        TEST_ASSERT_EQUAL_MESSAGE( slotCount, extraSlotCount, "Failed to update the number of slots" );

        /* Claim that the buffer to receive slots is too small. */
        slotCount = 0;
        result = globalFunctionList->C_GetSlotList( CK_TRUE, slotIdPtr, &slotCount );
        TEST_ASSERT_EQUAL_MESSAGE( CKR_BUFFER_TOO_SMALL, result, "Negative Test: Improper handling of too-small slot buffer" );
    }

    if( slotIdPtr != NULL )
    {
        PKCS11_FREE( slotIdPtr );
    }

    result = globalFunctionList->C_Finalize( NULL );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "Finalize failed" );
}

void test_OpenSession_CloseSession( void )
{
    CK_SLOT_ID_PTR slotIdPtr = NULL;
    CK_SLOT_ID slotId = 0;
    CK_ULONG slotCount = 0;
    CK_SESSION_HANDLE session = 0;
    CK_BBOOL sessionOpen = CK_FALSE;
    CK_RV result = CKR_OK;

    if( TEST_PROTECT() )
    {
        result = xGetSlotList( &slotIdPtr,
                               &slotCount );
        TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "Failed to get slot list" );

        slotId = slotIdPtr[ pkcs11testSLOT_NUMBER ];
        PKCS11_FREE( slotIdPtr ); /* xGetSlotList allocates memory. */
        TEST_ASSERT_GREATER_THAN( 0, slotCount );

        result = globalFunctionList->C_OpenSession( slotId,
                                                    CKF_SERIAL_SESSION, /* This flag is mandatory for PKCS #11 legacy reasons. */
                                                    NULL,               /* Application defined pointer. */
                                                    NULL,               /* Callback function. */
                                                    &session );
        TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "Failed to open session" );
        sessionOpen = CK_TRUE;
    }

    if( sessionOpen )
    {
        result = globalFunctionList->C_CloseSession( session );
        TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "Failed to close session." );
    }

    globalFunctionList->C_Finalize( NULL );


    /* Negative tests */

    /* Try to open a session without having initialized the module. */
    result = globalFunctionList->C_OpenSession( slotId,
                                                CKF_SERIAL_SESSION, /* This flag is mandatory for PKCS #11 legacy reasons. */
                                                NULL,               /* Application defined pointer. */
                                                NULL,               /* Callback function. */
                                                &session );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_CRYPTOKI_NOT_INITIALIZED, result, "Negative Test: Opened a session before initializing module." );
}

static CK_BYTE bitInput896[] = "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu";

static CK_BYTE sha256HashOfBitInput896[] =
{
    0xcf, 0x5b, 0x16, 0xa7, 0x78, 0xaf, 0x83, 0x80, 0x03, 0x6c, 0xe5, 0x9e, 0x7b, 0x04, 0x92, 0x37,
    0x0b, 0x24, 0x9b, 0x11, 0xe8, 0xf0, 0x7a, 0x51, 0xaf, 0xac, 0x45, 0x03, 0x7a, 0xfe, 0xe9, 0xd1
};

void test_Digest( void )
{
    CK_RV result = 0;

    CK_MECHANISM digestMechanism;

    CK_BYTE digestResult[ pkcs11SHA256_DIGEST_LENGTH ] = { 0 };
    CK_ULONG digestLength = 0;


    /* Hash with SHA256 */
    digestMechanism.mechanism = CKM_SHA256;

    result = globalFunctionList->C_DigestInit( globalSession, &digestMechanism );
    TEST_ASSERT_EQUAL( CKR_OK, result );

    /* Subtract one because this hash was performed on the characters without the null terminator. */
    result = globalFunctionList->C_DigestUpdate( globalSession, bitInput896, sizeof( bitInput896 ) - 1 );
    TEST_ASSERT_EQUAL( CKR_OK, result );

    /* Call C_DigestFinal on a NULL buffer to get the buffer length required. */
    result = globalFunctionList->C_DigestFinal( globalSession, NULL, &digestLength );
    TEST_ASSERT_EQUAL( CKR_OK, result );
    TEST_ASSERT_EQUAL( pkcs11SHA256_DIGEST_LENGTH, digestLength );

    result = globalFunctionList->C_DigestFinal( globalSession, digestResult, &digestLength );
    TEST_ASSERT_EQUAL( CKR_OK, result );
    TEST_ASSERT_EQUAL_INT8_ARRAY( sha256HashOfBitInput896, digestResult, pkcs11SHA256_DIGEST_LENGTH );
}

void test_Digest_ErrorConditions( void )
{
    CK_RV result = 0;
    CK_MECHANISM digestMechanism;
    CK_BYTE digestResult[ pkcs11SHA256_DIGEST_LENGTH ] = { 0 };
    CK_ULONG digestLength = 0;

    /* Make sure that no NULL pointers in functions to be called in this test. */
    TEST_ASSERT_NOT_EQUAL( NULL, globalFunctionList->C_DigestInit );
    TEST_ASSERT_NOT_EQUAL( NULL, globalFunctionList->C_DigestUpdate );
    TEST_ASSERT_NOT_EQUAL( NULL, globalFunctionList->C_DigestFinal );

    /* Invalid hash mechanism. */
    digestMechanism.mechanism = 0x253; /*253 doesn't correspond to anything. */ /*CKM_MD5; */

    result = globalFunctionList->C_DigestInit( globalSession, &digestMechanism );
    TEST_ASSERT_EQUAL( CKR_MECHANISM_INVALID, result );

    /* Null Session. */
    digestMechanism.mechanism = CKM_SHA256;
    result = globalFunctionList->C_DigestInit( ( CK_SESSION_HANDLE ) NULL, &digestMechanism );
    TEST_ASSERT_EQUAL( CKR_SESSION_HANDLE_INVALID, result );

    /* Make sure that digest update fails if DigestInit did not succeed. */
    result = globalFunctionList->C_DigestUpdate( globalSession, bitInput896, sizeof( bitInput896 ) - 1 );
    TEST_ASSERT_EQUAL( CKR_OPERATION_NOT_INITIALIZED, result );

    /* Initialize the session properly. */
    result = globalFunctionList->C_DigestInit( globalSession, &digestMechanism );
    TEST_ASSERT_EQUAL( CKR_OK, result );

    /* Try to update digest with a NULL session handle. */
    result = globalFunctionList->C_DigestUpdate( ( CK_SESSION_HANDLE ) NULL, bitInput896, sizeof( bitInput896 ) - 1 );
    TEST_ASSERT_EQUAL( CKR_SESSION_HANDLE_INVALID, result );

    /* DigestUpdate correctly.  Note that digest is not terminated because we didn't tell the session handle last time. */
    result = globalFunctionList->C_DigestUpdate( globalSession, bitInput896, sizeof( bitInput896 ) - 1 );
    TEST_ASSERT_EQUAL( CKR_OK, result );

    /* Call C_DigestFinal on a buffer that is too small. */
    digestLength = pkcs11SHA256_DIGEST_LENGTH - 1;
    result = globalFunctionList->C_DigestFinal( globalSession, digestResult, &digestLength );
    TEST_ASSERT_EQUAL( CKR_BUFFER_TOO_SMALL, result );

    /* Call C_DigestFinal on a NULL session handle. */
    digestLength = pkcs11SHA256_DIGEST_LENGTH;
    result = globalFunctionList->C_DigestFinal( ( CK_SESSION_HANDLE ) NULL, digestResult, &digestLength );
    TEST_ASSERT_EQUAL( CKR_SESSION_HANDLE_INVALID, result );

    /* Call C_DigestFinal on a proper buffer size. Note that Digest is not terminated if error is "buffer too small" or if session handle wasn't present. */
    digestLength = pkcs11SHA256_DIGEST_LENGTH;
    result = globalFunctionList->C_DigestFinal( globalSession, digestResult, &digestLength );
    TEST_ASSERT_EQUAL( CKR_OK, result );
    TEST_ASSERT_EQUAL_INT8_ARRAY( sha256HashOfBitInput896, digestResult, pkcs11SHA256_DIGEST_LENGTH );

    /* Call C_DigestUpdate after the digest operation has been completed. */
    result = globalFunctionList->C_DigestUpdate( globalSession, bitInput896, sizeof( bitInput896 ) - 1 );
    TEST_ASSERT_EQUAL( CKR_OPERATION_NOT_INITIALIZED, result );
}

void test_GenerateRandom( void )
{
    CK_RV result = 0;
    uint32_t sameSession = 0;
    uint32_t differentSessions = 0;
    int i;

    CK_BYTE buf1[ pkcstestRAND_BUFFER_SIZE ];
    CK_BYTE buf2[ pkcstestRAND_BUFFER_SIZE ];
    CK_BYTE buf3[ pkcstestRAND_BUFFER_SIZE ];

    /* Generate random bytes twice. */
    if( CKR_OK == result )
    {
        result = globalFunctionList->C_GenerateRandom( globalSession, buf1, pkcstestRAND_BUFFER_SIZE );
    }

    if( CKR_OK == result )
    {
        result = globalFunctionList->C_GenerateRandom( globalSession, buf2, pkcstestRAND_BUFFER_SIZE );
    }

    if( CKR_OK == result )
    {
        /* Close the session and PKCS #11 module */
        if( NULL != globalFunctionList )
        {
            ( void ) globalFunctionList->C_CloseSession( globalSession );
        }
    }

    /* Re-open PKCS #11 session. */
    result = xInitializePkcs11Session( &globalSession );

    if( CKR_OK == result )
    {
        result = globalFunctionList->C_GenerateRandom( globalSession, buf3, pkcstestRAND_BUFFER_SIZE );
    }

    /* Check that the result is good. */
    TEST_ASSERT_EQUAL_INT32( CKR_OK, result );

    /* Check that the random bytes generated within session
     * and between initializations of PKCS module are not the same. */
    for( i = 0; i < pkcstestRAND_BUFFER_SIZE; i++ )
    {
        if( buf1[ i ] == buf2[ i ] )
        {
            sameSession++;
        }

        if( buf1[ i ] == buf3[ i ] )
        {
            differentSessions++;
        }
    }

    if( ( sameSession > 1 ) || ( differentSessions > 1 ) )
    {
        LogDebug( ( "First Random Bytes: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X",
                    buf1[ 0 ], buf1[ 1 ], buf1[ 2 ], buf1[ 3 ], buf1[ 4 ],
                    buf1[ 5 ], buf1[ 6 ], buf1[ 7 ], buf1[ 8 ], buf1[ 9 ] ) );

        LogDebug( ( "Second Set of Random Bytes: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X",
                    buf2[ 0 ], buf2[ 1 ], buf2[ 2 ], buf2[ 3 ], buf2[ 4 ],
                    buf2[ 5 ], buf2[ 6 ], buf2[ 7 ], buf2[ 8 ], buf2[ 9 ] ) );

        LogDebug( ( "Third Set of Random Bytes:  %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X",
                    buf3[ 0 ], buf3[ 1 ], buf3[ 2 ], buf3[ 3 ], buf3[ 4 ],
                    buf3[ 5 ], buf3[ 6 ], buf3[ 7 ], buf3[ 8 ], buf3[ 9 ] ) );
    }

    TEST_ASSERT_LESS_THAN( 2, sameSession );
    TEST_ASSERT_LESS_THAN( 2, differentSessions );
}

/* Valid RSA private key. */
static const char validRSAPrivateKey[] =
    "-----BEGIN RSA PRIVATE KEY-----\n"
    "MIIEpAIBAAKCAQEAsIqRecRxLz3PZXzZOHF7jMlB25tfv2LDGR7nGTJiey5zxd7o\n"
    "swihe7+26yx8medpNvX1ym9jphty+9IR053k1WGnQQ4aaDeJonqn7V50Vesw6zFx\n"
    "/x8LMdXFoBAkRXIL8WS5YKafC87KPnye8A0piVWUFy7+IEEaK3hQEJTzB6LC/N10\n"
    "0XL5ykLCa4xJBOqlIvbDvJ+bKty1EBA3sStlTNuXi3nBWZbXwCB2A+ddjijFf5+g\n"
    "Ujinr7h6e2uQeipWyiIw9NKWbvq8AG1Mj4XBoFL9wP2YTf2SQAgAzx0ySPNrIYOz\n"
    "BNl1YZ4lIW5sJLATES9+Z8nHi7yRDLw6x/kcVQIDAQABAoIBADd+h3ZIeu/HtT8I\n"
    "vNuSSK0bwpj+wV1O9VcbMLfp760bEAd+J5XHu8NDo4NPi6dxZ9CABpBo7WEUtdNU\n"
    "2Ie11W4B8WpwvXpPIvOxLMJf85/ie5EjDNuObZ1vvlyvVkeCLyDlcaRhHBPBIC/+\n"
    "SpPY/1qNTSzwd6+55zkM69YajD60tFv8WuCsgkAteCoDjcqwDcowyAy4pILhOYaW\n"
    "1x+0ZPMUqwtld+06ct/yqBPB8C9IH7ZIeJr5e58R9SxytbuTwTN4jceOoeD5MBbG\n"
    "A+A0WxGdQ8N+kwWkz77qDbZfP4G8wNxeUXobnfqfDGqb0O5zeEaU7EI+mlEQH58Z\n"
    "B1edj6ECgYEA3rldciCQc4t2qYKiZSffnl7Pg7L+ojzH/Eam4Dlk27+DqOo70MnX\n"
    "LVWUWkLOMQ27dRSBQsUDUaqVRZLkcFKc6C8k2cIpPBMpA9WdZVd9kFawZ8lJ7jko\n"
    "qTbJxnDxvhdHrZRvLRjEenbdNXdAGy2EuqvThUJgPEybLAWg6sE3LB0CgYEAyurT\n"
    "14h4BGEGBpl2KevoPQ4PPS+IoDXMS29mtfcascVkYcxxW1v8wOQVyD4VrsRMErck\n"
    "ZMpu2evd+aQSPSrAod/sQ20C+wCCA7ipBlhAUeuS/FpqFIZWkHzZnVccp8O3nOFO\n"
    "KNeAmw4udq8PyjVVouey/6F386itJdxWt/d8i5kCgYA3Aru045wqHck6RvzLVVTj\n"
    "LfG9Sqmf8rlGc0DmYuapbB0dzHTnteLC3L9ep997uDOT0HO4xSZztllWLNjlcVI1\n"
    "+ub0LgO3Rdg8jTdp/3kQ/IhnqgzrnQyQ9upRbDYZSHC4y8/F6LcmtFMg0Ipx7AU7\n"
    "ghMld+aDHjy5W86KDR0OdQKBgQCAZoPSONqo+rQTbPwmns6AA+uErhVoO2KgwUdf\n"
    "EZPktaFFeVapltWjQTC/WvnhcvkoRpdS5/2pC+WUWEvqRKlMRSN9rvdZ2QJsVGcw\n"
    "Spu4urZx1MyXXEJef4I8W6kYR3JiZPdORL9uXlTsaO425/Tednr/4y7CEhQuhvSg\n"
    "yIwY0QKBgQC2NtKDOwcgFykKRYqtHuo6VpSeLmgm1DjlcAuaGJsblX7C07ZH8Tjm\n"
    "IHQb01oThNEa4tC0vO3518PkQwvyi/TWGHm9SLYdXvpVnBwkk5yRioKPgPmrs4Xi\n"
    "ERIYrvveGGtQ3vSknLWUJ/0BgmuYj5U6aJBZPv8COM2eKIbTQbtQaQ==\n"
    "-----END RSA PRIVATE KEY-----\n";

static const char validRSAPublicKey[] =
    "-----BEGIN RSA PUBLIC KEY-----\n"
    "MIIBCgKCAQEAsIqRecRxLz3PZXzZOHF7jMlB25tfv2LDGR7nGTJiey5zxd7oswih\n"
    "e7+26yx8medpNvX1ym9jphty+9IR053k1WGnQQ4aaDeJonqn7V50Vesw6zFx/x8L\n"
    "MdXFoBAkRXIL8WS5YKafC87KPnye8A0piVWUFy7+IEEaK3hQEJTzB6LC/N100XL5\n"
    "ykLCa4xJBOqlIvbDvJ+bKty1EBA3sStlTNuXi3nBWZbXwCB2A+ddjijFf5+gUjin\n"
    "r7h6e2uQeipWyiIw9NKWbvq8AG1Mj4XBoFL9wP2YTf2SQAgAzx0ySPNrIYOzBNl1\n"
    "YZ4lIW5sJLATES9+Z8nHi7yRDLw6x/kcVQIDAQAB\n"
    "-----END RSA PUBLIC KEY-----\n";

/* Valid RSA certificate. */
static const char validRSACertificate[] =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIDsTCCApmgAwIBAgIJALg4YJlPspxyMA0GCSqGSIb3DQEBCwUAMG8xCzAJBgNV\n"
    "BAYTAlVTMQswCQYDVQQIDAJXQTEQMA4GA1UEBwwHU2VhdHRsZTENMAsGA1UECgwE\n"
    "QW16bjEMMAoGA1UECwwDSW9UMQ0wCwYDVQQDDARUZXN0MRUwEwYJKoZIhvcNAQkB\n"
    "FgZub2JvZHkwHhcNMTgwNjExMTk0NjM2WhcNMjEwMzMxMTk0NjM2WjBvMQswCQYD\n"
    "VQQGEwJVUzELMAkGA1UECAwCV0ExEDAOBgNVBAcMB1NlYXR0bGUxDTALBgNVBAoM\n"
    "BEFtem4xDDAKBgNVBAsMA0lvVDENMAsGA1UEAwwEVGVzdDEVMBMGCSqGSIb3DQEJ\n"
    "ARYGbm9ib2R5MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAsIqRecRx\n"
    "Lz3PZXzZOHF7jMlB25tfv2LDGR7nGTJiey5zxd7oswihe7+26yx8medpNvX1ym9j\n"
    "phty+9IR053k1WGnQQ4aaDeJonqn7V50Vesw6zFx/x8LMdXFoBAkRXIL8WS5YKaf\n"
    "C87KPnye8A0piVWUFy7+IEEaK3hQEJTzB6LC/N100XL5ykLCa4xJBOqlIvbDvJ+b\n"
    "Kty1EBA3sStlTNuXi3nBWZbXwCB2A+ddjijFf5+gUjinr7h6e2uQeipWyiIw9NKW\n"
    "bvq8AG1Mj4XBoFL9wP2YTf2SQAgAzx0ySPNrIYOzBNl1YZ4lIW5sJLATES9+Z8nH\n"
    "i7yRDLw6x/kcVQIDAQABo1AwTjAdBgNVHQ4EFgQUHc4PjEL0CaxZ+1D/4VdeDjxt\n"
    "JO8wHwYDVR0jBBgwFoAUHc4PjEL0CaxZ+1D/4VdeDjxtJO8wDAYDVR0TBAUwAwEB\n"
    "/zANBgkqhkiG9w0BAQsFAAOCAQEAi1/okTpQuPcaQEBgepccZ/Lt/gEQNdGcbsYQ\n"
    "3aEABNVYL8dYOW9r/8l074zD+vi9iSli/yYmwRFD0baN1FRWUqkVEIQ+3yfivOW9\n"
    "R282NuQvEULgERC2KN7vm0vO+DF7ay58qm4PaAGHdQco1LaHKkljMPLHF841facG\n"
    "M9KVtzFveOQKkWvb4VgOyfn7aCnEogGlWt1S0d12pBRwYjJgKrVQaGs6IiGFVtm8\n"
    "JRLZrLL3sfgsN7L1xu//JUoTOkgxdKuYRmPuUdV2hw/VYDzcnKj7/DMXNDvgl3s7\n"
    "5GC4F+8LFLzRrZJWs18FMLaCE+zJChw/oeSt+RS0JZDFn+uX9Q==\n"
    "-----END CERTIFICATE-----\n";


static void provisionRsaTestCredentials( CK_OBJECT_HANDLE_PTR privateKeyHandlePtr,
                                         CK_OBJECT_HANDLE_PTR ppublicKeyHandle,
                                         CK_OBJECT_HANDLE_PTR pcertificateHandle )
{
    CK_RV result;

    /* Create a private key. */
    result = provisionPrivateKey( globalSession,
                                  ( uint8_t * ) validRSAPrivateKey,
                                  sizeof( validRSAPrivateKey ),
                                  ( uint8_t * ) pkcs11testLABEL_DEVICE_PRIVATE_KEY_FOR_TLS,
                                  privateKeyHandlePtr );

    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "Failed to create RSA private key." );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( CK_INVALID_HANDLE, *privateKeyHandlePtr, "Invalid object handle returned for RSA private key." );

    /* Create a public key. */
    result = provisionPublicKey( globalSession,
                                 ( uint8_t * ) validRSAPublicKey,
                                 sizeof( validRSAPublicKey ),
                                 CKK_RSA,
                                 ( uint8_t * ) pkcs11testLABEL_DEVICE_PUBLIC_KEY_FOR_TLS,
                                 ppublicKeyHandle );

    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "Failed to create RSA public key." );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( CK_INVALID_HANDLE, *ppublicKeyHandle, "Invalid object handle returned for RSA public key." );

    /* Create a certificate. */
    result = provisionCertificate( globalSession,
                                   ( uint8_t * ) validRSACertificate,
                                   sizeof( validRSACertificate ),
                                   ( uint8_t * ) pkcs11testLABEL_DEVICE_CERTIFICATE_FOR_TLS,
                                   pcertificateHandle );

    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "Failed to create RSA certificate." );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( CK_INVALID_HANDLE, *pcertificateHandle, "Invalid object handle returned for RSA certificate." );
}

void test_CreateObject_RSA( void )
{
    CK_RV result;
    CK_OBJECT_HANDLE privateKeyHandle = CK_INVALID_HANDLE;
    CK_OBJECT_HANDLE certificateHandle = CK_INVALID_HANDLE;
    CK_OBJECT_HANDLE publicKeyHandle = CK_INVALID_HANDLE;

    result = destroyTestCredentials();
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "Failed to destroy credentials in test setup." );

    provisionRsaTestCredentials( &privateKeyHandle, &publicKeyHandle, &certificateHandle );
}

void test_FindObject_RSA( void )
{
    CK_OBJECT_HANDLE privateKeyHandle = CK_INVALID_HANDLE;
    CK_OBJECT_HANDLE certificateHandle = CK_INVALID_HANDLE;
    CK_OBJECT_HANDLE publicKeyHandle = CK_INVALID_HANDLE;

    findObjectTest( &privateKeyHandle, &certificateHandle, &publicKeyHandle );
}

void test_GetAttributeValue_RSA( void )
{
    CK_RV result;
    CK_ATTRIBUTE template;
    CK_OBJECT_HANDLE certificateHandle = CK_INVALID_HANDLE;
    CK_OBJECT_HANDLE privateKeyHandle = CK_INVALID_HANDLE;


    CK_BYTE certificateValue[ CERTIFICATE_VALUE_LENGTH ];
    CK_BYTE keyComponent[ ( pkcs11RSA_2048_MODULUS_BITS / 8 ) + 1 ] = { 0 };

    result = xFindObjectWithLabelAndClass( globalSession,
                                           pkcs11testLABEL_DEVICE_PRIVATE_KEY_FOR_TLS,
                                           sizeof( pkcs11testLABEL_DEVICE_PRIVATE_KEY_FOR_TLS ),
                                           CKO_PRIVATE_KEY,
                                           &privateKeyHandle );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "Failed to find RSA private key." );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( CK_INVALID_HANDLE, privateKeyHandle, "Invalid object handle found for RSA private key." );

    result = xFindObjectWithLabelAndClass( globalSession,
                                           pkcs11testLABEL_DEVICE_CERTIFICATE_FOR_TLS,
                                           sizeof( pkcs11testLABEL_DEVICE_CERTIFICATE_FOR_TLS ),
                                           CKO_CERTIFICATE,
                                           &certificateHandle );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "Failed to find RSA certificate." );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( CK_INVALID_HANDLE, certificateHandle, "Invalid object handle found for RSA certificate." );

    /* TODO: Add RSA key component GetAttributeValue checks. */
    /* Get the certificate value. */
    template.type = CKA_VALUE;
    template.pValue = NULL;
    template.ulValueLen = 0;
    result = globalFunctionList->C_GetAttributeValue( globalSession, certificateHandle, &template, 1 );
    TEST_ASSERT_EQUAL_MESSAGE( CERTIFICATE_VALUE_LENGTH, template.ulValueLen, "GetAttributeValue returned incorrect length of RSA certificate value" );

    template.pValue = certificateValue;
    result = globalFunctionList->C_GetAttributeValue( globalSession, certificateHandle, &template, 1 );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "Failed to get RSA certificate value" );
    TEST_ASSERT_EQUAL_MESSAGE( CERTIFICATE_VALUE_LENGTH, template.ulValueLen, "GetAttributeValue returned incorrect length of RSA certificate value" );
    /* TODO: Check byte array */

    /* Check that the private key cannot be retrieved. */
    template.type = CKA_PRIVATE_EXPONENT;
    template.pValue = keyComponent;
    template.ulValueLen = sizeof( keyComponent );
    result = globalFunctionList->C_GetAttributeValue( globalSession, privateKeyHandle, &template, 1 );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_ATTRIBUTE_SENSITIVE, result, "Incorrect error code retrieved when trying to obtain private key." );
    TEST_ASSERT_EACH_EQUAL_INT8_MESSAGE( 0, keyComponent, sizeof( keyComponent ), "Private key bytes returned when they should not be." );
}


void test_Sign_RSA( void )
{
    CK_RV result;
    CK_OBJECT_HANDLE privateKeyHandle;
    CK_OBJECT_HANDLE publicKeyHandle;
    CK_OBJECT_HANDLE certificateHandle;
    CK_MECHANISM mechanism;
    CK_BYTE hashedMessage[ pkcs11SHA256_DIGEST_LENGTH ] = { 0 };
    CK_BYTE signature[ pkcs11RSA_2048_SIGNATURE_LENGTH ] = { 0 };
    CK_ULONG signatureLength;
    CK_BYTE hashPlusOid[ pkcs11RSA_SIGNATURE_INPUT_LENGTH ];

    provisionRsaTestCredentials( &privateKeyHandle, &publicKeyHandle, &certificateHandle );

    result = vAppendSHA256AlgorithmIdentifierSequence( hashedMessage, hashPlusOid );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "Failed to append hash algorithm to RSA signature material." );

    /* The RSA X.509 mechanism assumes a pre-hashed input. */
    mechanism.mechanism = CKM_RSA_PKCS;
    mechanism.pParameter = NULL;
    mechanism.ulParameterLen = 0;
    result = globalFunctionList->C_SignInit( globalSession, &mechanism, privateKeyHandle );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "Failed to SignInit RSA." );

    signatureLength = sizeof( signature );
    result = globalFunctionList->C_Sign( globalSession, hashPlusOid, sizeof( hashPlusOid ), signature, &signatureLength );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "Failed to RSA Sign." );
    TEST_ASSERT_EQUAL_MESSAGE( pkcs11RSA_2048_SIGNATURE_LENGTH, signatureLength, "RSA Sign returned an unexpected signature length." );

    result = globalFunctionList->C_SignInit( globalSession, &mechanism, privateKeyHandle );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "Failed to SignInit RSA." );

    result = globalFunctionList->C_Sign( globalSession, hashPlusOid, sizeof( hashPlusOid ), NULL, &signatureLength );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "Failed to RSA Sign." );
    TEST_ASSERT_EQUAL_MESSAGE( pkcs11RSA_2048_SIGNATURE_LENGTH, signatureLength, "RSA Sign should return needed signature buffer length when pucSignature is NULL." );

    /* Verify the signature with mbedTLS */
    mbedtls_pk_context mbedPkContext;
    int mbedTLSResult;

    mbedtls_pk_init( &mbedPkContext );

    if( TEST_PROTECT() )
    {
        mbedTLSResult = mbedtls_pk_parse_key( ( mbedtls_pk_context * ) &mbedPkContext,
                                              ( const unsigned char * ) validRSAPrivateKey,
                                              sizeof( validRSAPrivateKey ),
                                              NULL,
                                              0 );

        mbedTLSResult = mbedtls_rsa_pkcs1_verify( mbedPkContext.pk_ctx, NULL, NULL, MBEDTLS_RSA_PUBLIC, MBEDTLS_MD_SHA256, 32, hashedMessage, signature );
        TEST_ASSERT_EQUAL_MESSAGE( 0, mbedTLSResult, "mbedTLS failed to parse valid RSA key (verification)" );
    }

    mbedtls_pk_free( &mbedPkContext );
}


void test_DestroyObject_RSA( void )
{
    CK_RV result = CKR_OK;

    result = destroyTestCredentials();
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "Failed to destroy RSA credentials." );
}


/* Valid ECDSA private key. */
static const char validECDSAPrivateKey[] =
    "-----BEGIN EC PRIVATE KEY-----\n"
    "MHcCAQEEIACZbHljxOFuBeEKRcMijfbVcDzBxa8M4T5jElsElFQ5oAoGCCqGSM49\n"
    "AwEHoUQDQgAEzghp+QstUhOmzKBGEL7uBjsaBbyaNTMLXKLSW78+bdoP9bKTOrqi\n"
    "Kk9GzFk9ChthHFsx+T7UFithbYWtRf0Zww==\n"
    "-----END EC PRIVATE KEY-----";

/* Valid ECDSA public key. */
static const char validECDSAPublicKey[] =
    "-----BEGIN PUBLIC KEY-----\n"
    "MFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEzghp+QstUhOmzKBGEL7uBjsaBbya\n"
    "NTMLXKLSW78+bdoP9bKTOrqiKk9GzFk9ChthHFsx+T7UFithbYWtRf0Zww==\n"
    "-----END PUBLIC KEY-----";

/* Valid ECDSA certificate. */
static const char validECDSACertificate[] =
    "-----BEGIN CERTIFICATE-----\n"
    "MIICbjCCAhQCCQDqQDa2NeYOhTAKBggqhkjOPQQDAjCBvjELMAkGA1UEBhMCVVMx\n"
    "EzARBgNVBAgMCldhc2hpbmd0b24xEDAOBgNVBAcMB1NlYXR0bGUxGDAWBgNVBAoM\n"
    "D0FtYXpvbiBGcmVlUlRPUzEhMB8GA1UECwwYUEtDUyAjMTEgVGVzdCBDcmVkZW50\n"
    "aWFsMSgwJgYDVQQDDB9ET05UX1VTRV9USElTX0tFWV9JTl9BX1JFQUxfQVBQMSEw\n"
    "HwYJKoZIhvcNAQkBFhJub2JvZHlAbm93aGVyZS5jb20wHhcNMTkwNTI5MjE1NjAw\n"
    "WhcNMjkwNTI2MjE1NjAwWjCBvjELMAkGA1UEBhMCVVMxEzARBgNVBAgMCldhc2hp\n"
    "bmd0b24xEDAOBgNVBAcMB1NlYXR0bGUxGDAWBgNVBAoMD0FtYXpvbiBGcmVlUlRP\n"
    "UzEhMB8GA1UECwwYUEtDUyAjMTEgVGVzdCBDcmVkZW50aWFsMSgwJgYDVQQDDB9E\n"
    "T05UX1VTRV9USElTX0tFWV9JTl9BX1JFQUxfQVBQMSEwHwYJKoZIhvcNAQkBFhJu\n"
    "b2JvZHlAbm93aGVyZS5jb20wWTATBgcqhkjOPQIBBggqhkjOPQMBBwNCAATOCGn5\n"
    "Cy1SE6bMoEYQvu4GOxoFvJo1MwtcotJbvz5t2g/1spM6uqIqT0bMWT0KG2EcWzH5\n"
    "PtQWK2Ftha1F/RnDMAoGCCqGSM49BAMCA0gAMEUCIQCs1n3p+fOZxjZT+fnm3MQf\n"
    "IhxppLKnUggV42SAMpSneQIgdufH9clHZgrd9HVpRlIumy3sIMNEu9fzC9XZsSu8\n"
    "yQ8=\n"
    "-----END CERTIFICATE-----";

static void provisionCredentialsWithKeyImport( CK_OBJECT_HANDLE_PTR privateKeyHandlePtr,
                                               CK_OBJECT_HANDLE_PTR pcertificateHandle,
                                               CK_OBJECT_HANDLE_PTR publicKeyHandlePtr )
{
    CK_RV result;


    result = destroyTestCredentials();
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "Failed to destroy credentials in test setup." );

    result = provisionPublicKey( globalSession,
                                 ( uint8_t * ) validECDSAPublicKey,
                                 sizeof( validECDSAPublicKey ),
                                 CKK_EC,
                                 ( uint8_t * ) pkcs11testLABEL_DEVICE_PUBLIC_KEY_FOR_TLS,
                                 publicKeyHandlePtr );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "Failed to create EC public key." );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( 0, *publicKeyHandlePtr, "Invalid object handle returned for EC public key." );

    result = provisionPrivateKey( globalSession,
                                  ( uint8_t * ) validECDSAPrivateKey,
                                  sizeof( validECDSAPrivateKey ),
                                  ( uint8_t * ) pkcs11testLABEL_DEVICE_PRIVATE_KEY_FOR_TLS,
                                  privateKeyHandlePtr );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "Failed to create EC private key." );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( 0, *privateKeyHandlePtr, "Invalid object handle returned for EC private key." );

    result = provisionCertificate( globalSession,
                                   ( uint8_t * ) validECDSACertificate,
                                   sizeof( validECDSACertificate ),
                                   ( uint8_t * ) pkcs11testLABEL_DEVICE_CERTIFICATE_FOR_TLS,
                                   pcertificateHandle );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "Failed to create EC certificate." );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( 0, *pcertificateHandle, "Invalid object handle returned for EC certificate." );
}

void test_DestoryObject_EC( void )
{
    CK_RV result;

    result = destroyTestCredentials();
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK,
                               result,
                               "Failed to destroy credentials in test setup." );
}

void test_CreateObject_EC( void )
{
    CK_OBJECT_HANDLE privateKeyHandle;
    CK_OBJECT_HANDLE certificateHandle;
    CK_OBJECT_HANDLE publicKeyHandle;

    #if ( pkcs11configJITP_CODEVERIFY_ROOT_CERT_SUPPORTED == 1 )
        CK_RV result;
        CK_OBJECT_HANDLE rootCertificateHandle;
        CK_OBJECT_HANDLE codeSignPublicKeyHandle;
        CK_OBJECT_HANDLE JITPCertificateHandle;
    #endif /* if ( pkcs11configJITP_CODEVERIFY_ROOT_CERT_SUPPORTED == 1 ) */

    /* Ignore result as this might fail if the credentials did not exist. */
    destroyTestCredentials();

    provisionCredentialsWithKeyImport( &privateKeyHandle,
                                       &certificateHandle,
                                       &publicKeyHandle );

    #if ( pkcs11configJITP_CODEVERIFY_ROOT_CERT_SUPPORTED == 1 )
        result = provisionCertificate( globalSession,
                                       ( uint8_t * ) tlsATS1_ROOT_CERTIFICATE_PEM,
                                       tlsATS1_ROOT_CERTIFICATE_LENGTH,
                                       pkcs11configLABEL_ROOT_CERTIFICATE,
                                       &rootCertificateHandle );
        TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "Failed to create root EC certificate." );
        TEST_ASSERT_NOT_EQUAL_MESSAGE( CK_INVALID_HANDLE, rootCertificateHandle, "Invalid object handle returned for EC root certificate." );

        result = provisionCertificate( globalSession,
                                       ( uint8_t * ) tlsATS1_ROOT_CERTIFICATE_PEM,
                                       tlsATS1_ROOT_CERTIFICATE_LENGTH,
                                       pkcs11configLABEL_JITP_CERTIFICATE,
                                       &JITPCertificateHandle );
        TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "Failed to create JITP EC certificate." );
        TEST_ASSERT_NOT_EQUAL_MESSAGE( CK_INVALID_HANDLE, JITPCertificateHandle, "Invalid object handle returned for EC JITP certificate." );

        result = provisionPublicKey( globalSession,
                                     ( uint8_t * ) validECDSAPublicKey,
                                     sizeof( validECDSAPrivateKey ),
                                     CKK_EC,
                                     pkcs11configLABEL_CODE_VERIFICATION_KEY,
                                     &codeSignPublicKeyHandle );
        TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "Failed to create EC code sign public key." );
        TEST_ASSERT_NOT_EQUAL_MESSAGE( CK_INVALID_HANDLE, codeSignPublicKeyHandle, "Invalid object handle returned for EC code sign public key." );
    #endif /* if ( pkcs11configJITP_CODEVERIFY_ROOT_CERT_SUPPORTED == 1 ) */
}

void test_Sign_EC( void )
{
    CK_RV result = CKR_OK;
    CK_OBJECT_HANDLE privateKeyHandle = CK_INVALID_HANDLE;
    CK_OBJECT_HANDLE publicKeyHandle = CK_INVALID_HANDLE;
    CK_OBJECT_HANDLE certificateHandle = CK_INVALID_HANDLE;

    /* Note that ECDSA operations on a signature of all 0's is not permitted. */
    CK_BYTE hashedMessage[ pkcs11SHA256_DIGEST_LENGTH ] = { 0xab };
    CK_MECHANISM mechanism;
    CK_BYTE signature[ pkcs11RSA_2048_SIGNATURE_LENGTH ] = { 0 };
    CK_ULONG signatureLength;
    int mbedTLSResult;

    /* Find objects that were previously created. This test case should be run if
     * there are objects that exists under known labels. This test case is not
     * responsible for creating the objects used for signing. */
    findObjectTest( &privateKeyHandle, &certificateHandle, &publicKeyHandle );

    mechanism.mechanism = CKM_ECDSA;
    mechanism.pParameter = NULL;
    mechanism.ulParameterLen = 0;
    result = globalFunctionList->C_SignInit( globalSession, &mechanism, privateKeyHandle );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "Failed to SignInit ECDSA." );

    signatureLength = sizeof( signature );
    result = globalFunctionList->C_Sign( globalSession, hashedMessage, pkcs11SHA256_DIGEST_LENGTH, signature, &signatureLength );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "Failed to ECDSA Sign." );
    TEST_ASSERT_EQUAL_MESSAGE( pkcs11ECDSA_P256_SIGNATURE_LENGTH, signatureLength, "ECDSA Sign returned an unexpected ECDSA Signature length." );

    result = globalFunctionList->C_SignInit( globalSession, &mechanism, privateKeyHandle );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "Failed to SignInit ECDSA." );

    result = globalFunctionList->C_Sign( globalSession, hashedMessage, pkcs11SHA256_DIGEST_LENGTH, NULL, &signatureLength );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "Failed to ECDSA Sign." );
    TEST_ASSERT_EQUAL_MESSAGE( pkcs11ECDSA_P256_SIGNATURE_LENGTH, signatureLength, "ECDSA Sign should return needed signature buffer length when pucSignature is NULL." );

    /* Now extract the EC public key point so we can reconstruct it in mbed TLS. */
    mbedtls_pk_context ecdsaContext;
    mbedtls_pk_context * ecdsaContextPtr = &ecdsaContext;
    CK_ATTRIBUTE pubKeyQuery = { CKA_EC_POINT, NULL, 0 };
    CK_BYTE * publicKeyPtr = NULL;

    mbedtls_pk_init( ecdsaContextPtr );

    /* Reconstruct public key from EC Params. */
    mbedtls_ecp_keypair * keyPair;

    keyPair = PKCS11_MALLOC( sizeof( mbedtls_ecp_keypair ) );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( NULL, keyPair, "Failed to allocate memory for the mbed TLS context." );

    /* Initialize the info. */
    ecdsaContextPtr->pk_info = &mbedtls_eckey_info;
    mbedtls_ecp_keypair_init( keyPair );
    mbedtls_ecp_group_init( &keyPair->grp );

    /* Might want to make the ECP group configurable in the future. */
    mbedTLSResult = mbedtls_ecp_group_load( &keyPair->grp,
                                            MBEDTLS_ECP_DP_SECP256R1 );
    TEST_ASSERT_EQUAL_MESSAGE( 0, mbedTLSResult, "Failed to load EC group." );

    /* Initialize the context. */
    ecdsaContextPtr->pk_ctx = keyPair;

    /* Get EC point from PKCS #11 stack. */
    result = globalFunctionList->C_GetAttributeValue( globalSession, publicKeyHandle, &pubKeyQuery, 1 );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "Failed to query for public key length" );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( 0, pubKeyQuery.ulValueLen, "The size of the public key was an unexpected value." );

    publicKeyPtr = PKCS11_MALLOC( pubKeyQuery.ulValueLen );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( NULL, publicKeyPtr, "Failed to allocate space for public key." );

    pubKeyQuery.pValue = publicKeyPtr;
    result = globalFunctionList->C_GetAttributeValue( globalSession, publicKeyHandle, &pubKeyQuery, 1 );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "Failed to query for public key length" );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( 0, pubKeyQuery.ulValueLen, "The size of the public key was an unexpected value." );

    /* Strip the ANS.1 Encoding of type and length. Otherwise mbed TLS won't be
     * able to parse the binary EC point. */
    mbedTLSResult = mbedtls_ecp_point_read_binary( &keyPair->grp,
                                                   &keyPair->Q,
                                                   ( uint8_t * ) ( pubKeyQuery.pValue ) + 2,
                                                   pubKeyQuery.ulValueLen - 2 );
    TEST_ASSERT_EQUAL_MESSAGE( 0, mbedTLSResult, "mbedTLS failed to read binary point." );

    if( TEST_PROTECT() )
    {
        mbedtls_ecp_keypair * ecdsaContextPtr = ( mbedtls_ecp_keypair * ) ecdsaContext.pk_ctx;
        /* An ECDSA signature is comprised of 2 components - R & S. */
        mbedtls_mpi R;
        mbedtls_mpi S;
        mbedtls_mpi_init( &R );
        mbedtls_mpi_init( &S );

        mbedTLSResult = mbedtls_mpi_read_binary( &R, &signature[ 0 ], 32 );
        TEST_ASSERT_EQUAL_MESSAGE( 0, mbedTLSResult, "mbedTLS failed to read R binary in ECDSA signature." );

        mbedTLSResult = mbedtls_mpi_read_binary( &S, &signature[ 32 ], 32 );
        TEST_ASSERT_EQUAL_MESSAGE( 0, mbedTLSResult, "mbedTLS failed to read S binary in ECDSA signature." );

        mbedTLSResult = mbedtls_ecdsa_verify( &ecdsaContextPtr->grp, hashedMessage, sizeof( hashedMessage ), &ecdsaContextPtr->Q, &R, &S );
        TEST_ASSERT_EQUAL_MESSAGE( 0, mbedTLSResult, "mbedTLS failed to verify signature." );

        mbedtls_mpi_free( &R );
        mbedtls_mpi_free( &S );
    }

    PKCS11_FREE( publicKeyPtr );
    mbedtls_pk_free( &ecdsaContext );
}

void test_Verify_EC( void )
{
    CK_RV result;
    CK_OBJECT_HANDLE privateKeyHandle;
    CK_OBJECT_HANDLE publicKeyHandle;
    CK_OBJECT_HANDLE certificateHandle;
    CK_MECHANISM mechanism;
    CK_BYTE hashedMessage[ pkcs11SHA256_DIGEST_LENGTH ] = { 0xbe };
    CK_BYTE signature[ pkcs11ECDSA_P256_SIGNATURE_LENGTH + 10 ] = { 0 };
    CK_BYTE signaturePKCS[ 64 ] = { 0 };
    size_t signatureLength = pkcs11ECDSA_P256_SIGNATURE_LENGTH;

    /* TODO: Consider switching this out for a C_GenerateRandom dependent function for ports not implementing mbedTLS. */

    /* Find objects that were previously created. This test case should be run if
     * there are objects that exists under known labels. This test case is not
     * responsible for creating the objects used for signing. */
    findObjectTest( &privateKeyHandle, &certificateHandle, &publicKeyHandle );

    /* Sign data w/ PKCS. */
    mechanism.mechanism = CKM_ECDSA;
    mechanism.pParameter = NULL;
    mechanism.ulParameterLen = 0;
    result = globalFunctionList->C_SignInit( globalSession, &mechanism, privateKeyHandle );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "SignInit failed." );
    result = globalFunctionList->C_Sign( globalSession, hashedMessage, sizeof( hashedMessage ), signature, ( CK_ULONG * ) &signatureLength );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "Sign failed." );

    result = globalFunctionList->C_VerifyInit( globalSession, &mechanism, publicKeyHandle );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "VerifyInit failed." );

    result = globalFunctionList->C_Verify( globalSession, hashedMessage, pkcs11SHA256_DIGEST_LENGTH, signature, sizeof( signaturePKCS ) );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "Verify failed." );

    #if ( pkcs11testIMPORT_PRIVATE_KEY_SUPPORT == 1 )
        mbedtls_pk_context pkCtx;
        mbedtls_entropy_context entropyCtx;
        mbedtls_ctr_drbg_context drbgCtx;
        int mbedResult;

        /* Initialize the private key. */
        mbedtls_pk_init( &pkCtx );
        mbedResult = mbedtls_pk_parse_key( &pkCtx,
                                           ( const unsigned char * ) validECDSAPrivateKey,
                                           sizeof( validECDSAPrivateKey ),
                                           NULL,
                                           0 );
        TEST_ASSERT_EQUAL_MESSAGE( 0, mbedResult, "Failed to parse valid ECDSA key." );
        /* Initialize the RNG. */
        mbedtls_entropy_init( &entropyCtx );
        mbedtls_ctr_drbg_init( &drbgCtx );
        mbedResult = mbedtls_ctr_drbg_seed( &drbgCtx, mbedtls_entropy_func, &entropyCtx, NULL, 0 );
        TEST_ASSERT_EQUAL_MESSAGE( 0, mbedResult, "Failed to initialize DRBG" );

        mbedResult = mbedtls_pk_sign( &pkCtx, MBEDTLS_MD_SHA256, hashedMessage, sizeof( hashedMessage ), signature, &signatureLength, mbedtls_ctr_drbg_random, &drbgCtx );
        TEST_ASSERT_EQUAL_MESSAGE( 0, mbedResult, "Failed to perform ECDSA signature." );

        mbedtls_pk_free( &pkCtx );
        mbedtls_ctr_drbg_free( &drbgCtx );
        mbedtls_entropy_free( &entropyCtx );

        /* Reconstruct the signature in PKCS #11 format. */
        mbedResult = PKI_mbedTLSSignatureToPkcs11Signature( signaturePKCS,
                                                            signature );
        TEST_ASSERT_EQUAL_MESSAGE( 0, mbedResult, "Null buffers." );

        /* Verify with PKCS #11. */
        mechanism.mechanism = CKM_ECDSA;
        mechanism.pParameter = NULL;
        mechanism.ulParameterLen = 0;
        result = globalFunctionList->C_VerifyInit( globalSession, &mechanism, publicKeyHandle );
        TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "VerifyInit failed." );

        result = globalFunctionList->C_Verify( globalSession, hashedMessage, pkcs11SHA256_DIGEST_LENGTH, signaturePKCS, sizeof( signaturePKCS ) );
        TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "Verify failed." );
    #endif /* if ( pkcs11testIMPORT_PRIVATE_KEY_SUPPORT == 1 ) */
    /* Modify signature value and make sure verification fails. */
}



void test_FindObject_EC( void )
{
    CK_OBJECT_HANDLE privateKeyHandle = CK_INVALID_HANDLE;
    CK_OBJECT_HANDLE certificateHandle = CK_INVALID_HANDLE;
    CK_OBJECT_HANDLE publicKeyHandle = CK_INVALID_HANDLE;

    findObjectTest( &privateKeyHandle, &certificateHandle, &publicKeyHandle );
}

extern int convert_pem_to_der( const unsigned char * pucInput,
                               size_t xLen,
                               unsigned char * pucOutput,
                               size_t * pxOlen );

void test_GetAttributeValue_EC( void )
{
    CK_RV result;
    CK_OBJECT_HANDLE privateKey = CK_INVALID_HANDLE;
    CK_OBJECT_HANDLE publicKey = CK_INVALID_HANDLE;
    CK_OBJECT_HANDLE certificate = CK_INVALID_HANDLE;
    CK_ATTRIBUTE template;
    CK_KEY_TYPE keyType = 0;
    uint8_t p256Oid[] = pkcs11DER_ENCODED_OID_P256;
    CK_BYTE ecParams[ 10 ] = { 0 };
    CK_OBJECT_CLASS class;
    CK_BYTE ecPointExpected[] =
    {
        0x04, 0x41, 0x04, 0xce, 0x08, 0x69, 0xf9, 0x0b, 0x2d, 0x52, 0x13, 0xa6, 0xcc, 0xa0, 0x46, 0x10,
        0xbe, 0xee, 0x06, 0x3b, 0x1a, 0x05, 0xbc, 0x9a, 0x35, 0x33, 0x0b, 0x5c, 0xa2, 0xd2, 0x5b, 0xbf,
        0x3e, 0x6d, 0xda, 0x0f, 0xf5, 0xb2, 0x93, 0x3a, 0xba, 0xa2, 0x2a, 0x4f, 0x46, 0xcc, 0x59, 0x3d,
        0x0a, 0x1b, 0x61, 0x1c, 0x5b, 0x31, 0xf9, 0x3e, 0xd4, 0x16, 0x2b, 0x61, 0x6d, 0x85, 0xad, 0x45,
        0xfd, 0x19, 0xc3
    };
    CK_BYTE certificateValueExpected[ 626 ];
    CK_BYTE certificateValue[ 626 ];
    CK_BYTE ecPoint[ sizeof( ecPointExpected ) ] = { 0 };
    size_t length = sizeof( certificateValueExpected );
    int conversion;

    conversion = convert_pem_to_der( ( const unsigned char * ) validECDSACertificate,
                                     sizeof( validECDSACertificate ),
                                     certificateValueExpected,
                                     &length );

    if( conversion != 0 )
    {
        LogError( ( "Failed to convert the EC certificate from PEM to DER. Error code %d.", conversion ) );
    }

    findObjectTest( &privateKey, &certificate, &publicKey );

    /* The PKCS #11 standard expects that calling GetAttributeValue with a null pointer to the value
     * will yield a success with the value length updated to the size of the buffer needed to contain
     * the attribute.
     *
     * All tests start by querying the attribute length, and followed by a query of the attribute value. */

    /***** Private Key Checks. *****/

    /* Check object class. */
    template.type = CKA_CLASS;
    template.pValue = NULL;
    template.ulValueLen = 0;

    result = globalFunctionList->C_GetAttributeValue( globalSession, privateKey, &template, 1 );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "GetAttributeValue for length of private EC key class failed." );
    TEST_ASSERT_EQUAL_MESSAGE( sizeof( CK_OBJECT_CLASS ), template.ulValueLen, "Incorrect object class length returned from GetAttributeValue." );

    template.pValue = &class;
    result = globalFunctionList->C_GetAttributeValue( globalSession, privateKey, &template, 1 );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "GetAttributeValue for private EC key class failed." );
    TEST_ASSERT_EQUAL_MESSAGE( CKO_PRIVATE_KEY, class, "Incorrect object class returned from GetAttributeValue." );

    /* Key type. */
    template.type = CKA_KEY_TYPE;
    template.pValue = NULL;
    template.ulValueLen = 0;
    result = globalFunctionList->C_GetAttributeValue( globalSession, privateKey, &template, 1 );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "GetAttributeValue for length of EC key type failed." );
    TEST_ASSERT_EQUAL_MESSAGE( sizeof( CK_KEY_TYPE ), template.ulValueLen, "Incorrect key type length provided." );

    template.pValue = &keyType;
    result = globalFunctionList->C_GetAttributeValue( globalSession, privateKey, &template, 1 );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "GetAttributeValue for EC key type failed." );
    TEST_ASSERT_EQUAL_MESSAGE( CKK_EC, keyType, "Incorrect key type returned." );

    /* Check EC Params. */
    template.type = CKA_EC_PARAMS;
    template.pValue = NULL;
    template.ulValueLen = 0;
    result = globalFunctionList->C_GetAttributeValue( globalSession, privateKey, &template, 1 );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "GetAttributeValue for length of EC params type failed." );
    TEST_ASSERT_EQUAL_MESSAGE( sizeof( p256Oid ), template.ulValueLen, "Incorrect EC params length provided." );

    template.pValue = ecParams;
    result = globalFunctionList->C_GetAttributeValue( globalSession, privateKey, &template, 1 );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "GetAttributeValue for EC params failed." );
    TEST_ASSERT_EQUAL_INT8_ARRAY_MESSAGE( p256Oid, ecParams, sizeof( p256Oid ), "Incorrect ECParameters returned from GetAttributeValue" );

    /******* Public Key ********/
    /* Object class. */
    template.type = CKA_CLASS;
    template.pValue = NULL;
    template.ulValueLen = 0;
    result = globalFunctionList->C_GetAttributeValue( globalSession, publicKey, &template, 1 );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "GetAttributeValue for length of public EC key class failed." );
    TEST_ASSERT_EQUAL_MESSAGE( sizeof( CK_OBJECT_CLASS ), template.ulValueLen, "Incorrect object class length returned from GetAttributeValue." );

    template.pValue = &class;
    result = globalFunctionList->C_GetAttributeValue( globalSession, publicKey, &template, 1 );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "GetAttributeValue for public EC key class failed." );
    TEST_ASSERT_EQUAL_MESSAGE( CKO_PUBLIC_KEY, class, "Incorrect object class returned from GetAttributeValue." );

    /* Elliptic Curve Parameters (the OID of the curve). At this time only P256 curves are supported. */
    template.type = CKA_EC_PARAMS;
    template.pValue = NULL;
    template.ulValueLen = 0;
    result = globalFunctionList->C_GetAttributeValue( globalSession, publicKey, &template, 1 );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "GetAttributeValue for length of public key EC Params failed." );
    TEST_ASSERT_EQUAL_MESSAGE( sizeof( p256Oid ), template.ulValueLen, "Incorrect EC params length provided." );

    memset( ecParams, 0x0, sizeof( p256Oid ) );
    template.pValue = ecParams;
    result = globalFunctionList->C_GetAttributeValue( globalSession, publicKey, &template, 1 );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "GetAttributeValue for EC params failed." );
    TEST_ASSERT_EQUAL_INT8_ARRAY_MESSAGE( p256Oid, ecParams, sizeof( p256Oid ), "Incorrect ECParameters returned from GetAttributeValue" );

    /* Elliptic curve point. */
    template.type = CKA_EC_POINT;
    template.pValue = NULL;
    template.ulValueLen = 0;
    result = globalFunctionList->C_GetAttributeValue( globalSession, publicKey, &template, 1 );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "GetAttributeValue for length of public key EC point failed." );
    TEST_ASSERT_EQUAL_MESSAGE( sizeof( ecPointExpected ), template.ulValueLen, "Incorrect EC point length provided." );

    template.pValue = ecPoint;
    result = globalFunctionList->C_GetAttributeValue( globalSession, publicKey, &template, 1 );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "GetAttributeValue for EC point failed." );
    #if pkcs11testIMPORT_PRIVATE_KEY_SUPPORT == 1

        /* The EC point can only be known for a public key that was previously created
         * therefore this check is only done for implementations that support importing
         * a private key, as the credentials that are on the device are all known.
         */
        TEST_ASSERT_EQUAL_INT8_ARRAY_MESSAGE( ecPointExpected, ecPoint, sizeof( ecPointExpected ), "Incorrect EC Point returned from GetAttributeValue" );
    #endif

    /****** Certificate check. *******/
    /* Object class. */
    template.type = CKA_CLASS;
    template.pValue = NULL;
    template.ulValueLen = 0;
    result = globalFunctionList->C_GetAttributeValue( globalSession, certificate, &template, 1 );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "GetAttributeValue for length of EC certificate class failed." );
    #if ( pkcs11testPREPROVISIONED_SUPPORT != 1 )
        TEST_ASSERT_EQUAL_MESSAGE( sizeof( CK_OBJECT_CLASS ), template.ulValueLen, "Incorrect object class length returned from GetAttributeValue." );
    #endif

    template.pValue = &class;
    result = globalFunctionList->C_GetAttributeValue( globalSession, certificate, &template, 1 );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "GetAttributeValue for EC certificate class failed." );
    TEST_ASSERT_EQUAL_MESSAGE( CKO_CERTIFICATE, class, "Incorrect object class returned from GetAttributeValue." );

    /* Certificate value (the DER encoded certificate). */
    template.type = CKA_VALUE;
    template.pValue = NULL;
    template.ulValueLen = 0;
    result = globalFunctionList->C_GetAttributeValue( globalSession, certificate, &template, 1 );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "GetAttributeValue for length of certificate value failed." );
    #if ( pkcs11testPREPROVISIONED_SUPPORT != 1 )
        TEST_ASSERT_EQUAL_MESSAGE( sizeof( certificateValueExpected ), template.ulValueLen, "Incorrect certificate value length" );
    #endif

    template.pValue = certificateValue;
    result = globalFunctionList->C_GetAttributeValue( globalSession, certificate, &template, 1 );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "GetAttributeValue for certificate value failed." );
    #if ( pkcs11testPREPROVISIONED_SUPPORT != 1 )
        TEST_ASSERT_EQUAL_INT8_ARRAY_MESSAGE( certificateValueExpected, certificateValue, sizeof( certificateValueExpected ), "Incorrect certificate value returned." );
    #endif
}

/*
 * 1. Generates an Elliptic Curve P256 key pair
 * 2. Calls GetAttributeValue to check generated key & that private key is not extractable.
 * 3. Constructs the public key using values from GetAttributeValue calls
 * 4. Uses private key to perform a sign operation
 * 5. Verifies the signature using mbedTLS library and reconstructed public key
 * 6. Verifies the signature using the public key just created.
 * 7. Finds the public and private key using FindObject calls
 */
void test_GenerateKeyPair_EC( void )
{
    CK_RV result;
    CK_OBJECT_HANDLE privateKeyHandle = CK_INVALID_HANDLE;
    CK_OBJECT_HANDLE publicKeyHandle = CK_INVALID_HANDLE;
    CK_OBJECT_HANDLE certificateHandle = CK_INVALID_HANDLE;

    CK_BYTE ecPoint[ pkcs11EC_POINT_LENGTH ] = { 0 };
    CK_BYTE privateKeyBuffer[ 32 ] = { 0 };
    CK_KEY_TYPE keyType;
    CK_ATTRIBUTE template;
    CK_OBJECT_CLASS class;

    /* mbedTLS structures for verification. */
    uint8_t ecp256r1Oid[] = pkcs11DER_ENCODED_OID_P256; /*"\x06\x08" MBEDTLS_OID_EC_GRP_SECP256R1; */
    CK_BYTE ecParams[ sizeof( ecp256r1Oid ) ] = { 0 };

    result = generateKeyPairEC( globalSession,
                                ( uint8_t * ) pkcs11testLABEL_DEVICE_PRIVATE_KEY_FOR_TLS,
                                ( uint8_t * ) pkcs11testLABEL_DEVICE_PUBLIC_KEY_FOR_TLS,
                                &privateKeyHandle,
                                &publicKeyHandle );

    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "Generating EC key pair failed." );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( CK_INVALID_HANDLE, privateKeyHandle, "Invalid private key handle generated by GenerateKeyPair" );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( CK_INVALID_HANDLE, publicKeyHandle, "Invalid public key handle generated by GenerateKeyPair" );

    /* We will try to provision the certificate as well, as it is needed for the tests that are not responsible for creating objects. */
    result = provisionCertificate( globalSession,
                                   ( uint8_t * ) validECDSACertificate,
                                   sizeof( validECDSACertificate ),
                                   ( uint8_t * ) pkcs11testLABEL_DEVICE_CERTIFICATE_FOR_TLS,
                                   &certificateHandle );

    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "Failed to create EC certificate." );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( CK_INVALID_HANDLE, certificateHandle, "Invalid object handle returned for EC certificate." );

    /* Call GetAttributeValue to retrieve information about the keypair stored. */
    /* Check that correct object class retrieved. */
    template.type = CKA_CLASS;
    template.pValue = NULL;
    template.ulValueLen = 0;
    result = globalFunctionList->C_GetAttributeValue( globalSession, publicKeyHandle, &template, 1 );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "GetAttributeValue for length of public EC key class failed." );
    TEST_ASSERT_EQUAL_MESSAGE( sizeof( CK_OBJECT_CLASS ), template.ulValueLen, "Incorrect object class length returned from GetAttributeValue." );

    template.pValue = &class;
    result = globalFunctionList->C_GetAttributeValue( globalSession, privateKeyHandle, &template, 1 );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "GetAttributeValue for private EC key class failed." );
    TEST_ASSERT_EQUAL_MESSAGE( CKO_PRIVATE_KEY, class, "Incorrect object class returned from GetAttributeValue." );

    template.pValue = &class;
    result = globalFunctionList->C_GetAttributeValue( globalSession, publicKeyHandle, &template, 1 );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "GetAttributeValue for public EC key class failed." );
    TEST_ASSERT_EQUAL_MESSAGE( CKO_PUBLIC_KEY, class, "Incorrect object class returned from GetAttributeValue." );

    /* Check that both keys are stored as EC Keys. */
    template.type = CKA_KEY_TYPE;
    template.pValue = &keyType;
    template.ulValueLen = sizeof( CK_KEY_TYPE );
    result = globalFunctionList->C_GetAttributeValue( globalSession, privateKeyHandle, &template, 1 );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "Error getting attribute value of EC key type." );
    TEST_ASSERT_EQUAL_MESSAGE( sizeof( CK_KEY_TYPE ), template.ulValueLen, "Length of key type incorrect in GetAttributeValue" );
    TEST_ASSERT_EQUAL_MESSAGE( CKK_EC, keyType, "Incorrect key type for private key" );

    result = globalFunctionList->C_GetAttributeValue( globalSession, publicKeyHandle, &template, 1 );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "Error getting attribute value of EC key type." );
    TEST_ASSERT_EQUAL_MESSAGE( sizeof( CK_KEY_TYPE ), template.ulValueLen, "Length of key type incorrect in GetAttributeValue" );
    TEST_ASSERT_EQUAL_MESSAGE( CKK_EC, keyType, "Incorrect key type for public key" );

    /* Check that correct curve retrieved for private key. */
    template.type = CKA_EC_PARAMS;
    template.pValue = ecParams;
    template.ulValueLen = sizeof( ecParams );
    result = globalFunctionList->C_GetAttributeValue( globalSession, privateKeyHandle, &template, 1 );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "Error getting attribute value of EC Parameters." );
    TEST_ASSERT_EQUAL_MESSAGE( sizeof( ecp256r1Oid ), template.ulValueLen, "Length of ECParameters identifier incorrect in GetAttributeValue" );
    TEST_ASSERT_EQUAL_INT8_ARRAY_MESSAGE( ecp256r1Oid, ecParams, template.ulValueLen, "EcParameters did not match P256 OID." );

    /* Check that the private key cannot be retrieved. */
    template.type = CKA_VALUE;
    template.pValue = privateKeyBuffer;
    template.ulValueLen = sizeof( privateKeyBuffer );
    result = globalFunctionList->C_GetAttributeValue( globalSession, privateKeyHandle, &template, 1 );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_ATTRIBUTE_SENSITIVE, result, "Wrong error code retrieving private key" );
    TEST_ASSERT_EACH_EQUAL_INT8_MESSAGE( 0, privateKeyBuffer, sizeof( privateKeyBuffer ), "Private key bytes returned when they should not be" );

    /* Check that public key point can be retrieved for public key. */
    template.type = CKA_EC_POINT;
    template.pValue = ecPoint;
    template.ulValueLen = sizeof( ecPoint );
    result = globalFunctionList->C_GetAttributeValue( globalSession, publicKeyHandle, &template, 1 );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, result, "Failed to retrieve EC Point." );
}

/*-----------------------------------------------------------*/

/* Import the specified ECDSA private key into storage. */
static CK_RV provisionPrivateECKey( CK_SESSION_HANDLE session,
                                    uint8_t * label,
                                    CK_OBJECT_HANDLE_PTR objectHandle,
                                    mbedtls_pk_context * mbedPkContext )
{
    CK_RV result = CKR_OK;
    CK_FUNCTION_LIST_PTR functionList = NULL;
    CK_BYTE * DPtr;               /* Private value D. */
    CK_BYTE * ecParamsPtr = NULL; /* DER-encoding of an ANSI X9.62 Parameters value */
    int mbedResult = 0;
    CK_BBOOL trueObject = CK_TRUE;
    CK_KEY_TYPE privateKeyType = CKK_EC;
    CK_OBJECT_CLASS privateKeyClass = CKO_PRIVATE_KEY;
    mbedtls_ecp_keypair * keyPair = ( mbedtls_ecp_keypair * ) mbedPkContext->pk_ctx;

    result = C_GetFunctionList( &functionList );

    DPtr = PKCS11_MALLOC( EC_D_LENGTH );

    if( ( DPtr == NULL ) )
    {
        result = CKR_HOST_MEMORY;
    }

    if( result == CKR_OK )
    {
        mbedResult = mbedtls_mpi_write_binary( &( keyPair->d ), DPtr, EC_D_LENGTH );

        if( mbedResult != 0 )
        {
            LogError( ( "Failed to parse EC private key components. \r\n" ) );
            result = CKR_ATTRIBUTE_VALUE_INVALID;
        }
    }

    if( result == CKR_OK )
    {
        if( keyPair->grp.id == MBEDTLS_ECP_DP_SECP256R1 )
        {
            ecParamsPtr = ( CK_BYTE * ) ( "\x06\x08" MBEDTLS_OID_EC_GRP_SECP256R1 );
        }
        else
        {
            result = CKR_FUNCTION_NOT_SUPPORTED;//CKR_CURVE_NOT_SUPPORTED;
        }
    }

    if( result == CKR_OK )
    {
        CK_ATTRIBUTE privateKeyTemplate[] =
        {
            { CKA_CLASS,     NULL /* &privateKeyClass*/, sizeof( CK_OBJECT_CLASS )                     },
            { CKA_KEY_TYPE,  NULL /* &privateKeyType*/,  sizeof( CK_KEY_TYPE )                         },
            { CKA_LABEL,     label,                      ( CK_ULONG ) strlen( ( const char * ) label ) },
            { CKA_TOKEN,     NULL /* &trueObject*/,      sizeof( CK_BBOOL )                            },
            { CKA_SIGN,      NULL /* &trueObject*/,      sizeof( CK_BBOOL )                            },
            { CKA_EC_PARAMS, NULL /* ecParamsPtr*/,      EC_PARAMS_LENGTH                              },
            { CKA_VALUE,     NULL /* DPtr*/,             EC_D_LENGTH                                   }
        };

        /* Aggregate initializers must not use the address of an automatic variable. */
        /* See MSVC Compiler Warning C4221 */
        privateKeyTemplate[ 0 ].pValue = &privateKeyClass;
        privateKeyTemplate[ 1 ].pValue = &privateKeyType;
        privateKeyTemplate[ 3 ].pValue = &trueObject;
        privateKeyTemplate[ 4 ].pValue = &trueObject;
        privateKeyTemplate[ 5 ].pValue = ecParamsPtr;
        privateKeyTemplate[ 6 ].pValue = DPtr;

        result = functionList->C_CreateObject( session,
                                               ( CK_ATTRIBUTE_PTR ) &privateKeyTemplate,
                                               sizeof( privateKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                                               objectHandle );
    }

    if( DPtr != NULL )
    {
        PKCS11_FREE( DPtr );
    }

    return result;
}

/*-----------------------------------------------------------*/

/* Import the specified RSA private key into storage. */
static CK_RV provisionPrivateRSAKey( CK_SESSION_HANDLE session,
                                     uint8_t * label,
                                     CK_OBJECT_HANDLE_PTR objectHandle,
                                     mbedtls_pk_context * mbedPkContext )
{
    CK_RV result = CKR_OK;
    CK_FUNCTION_LIST_PTR functionList = NULL;
    int mbedResult = 0;
    CK_KEY_TYPE privateKeyType = CKK_RSA;
    mbedtls_rsa_context * rsaContext = mbedPkContext->pk_ctx;
    CK_OBJECT_CLASS privateKeyClass = CKO_PRIVATE_KEY;
    RsaParams_t * rsaParams = NULL;
    CK_BBOOL trueObject = CK_TRUE;

    result = C_GetFunctionList( &functionList );

    rsaParams = PKCS11_MALLOC( sizeof( RsaParams_t ) );

    if( rsaParams == NULL )
    {
        result = CKR_HOST_MEMORY;
    }

    if( result == CKR_OK )
    {
        memset( rsaParams, 0, sizeof( RsaParams_t ) );

        mbedResult = mbedtls_rsa_export_raw( rsaContext,
                                             rsaParams->modulus, MODULUS_LENGTH + 1,
                                             rsaParams->prime1, PRIME_1_LENGTH + 1,
                                             rsaParams->prime2, PRIME_2_LENGTH + 1,
                                             rsaParams->d, D_LENGTH + 1,
                                             rsaParams->e, E_LENGTH + 1 );

        if( mbedResult != 0 )
        {
            LogError( ( "Failed to parse RSA private key components. \r\n" ) );
            result = CKR_ATTRIBUTE_VALUE_INVALID;
        }

        /* Export Exponent 1, Exponent 2, Coefficient. */
        mbedResult |= mbedtls_mpi_write_binary( ( mbedtls_mpi const * ) &rsaContext->DP, rsaParams->exponent1, EXPONENT_1_LENGTH + 1 );
        mbedResult |= mbedtls_mpi_write_binary( ( mbedtls_mpi const * ) &rsaContext->DQ, rsaParams->exponent2, EXPONENT_2_LENGTH + 1 );
        mbedResult |= mbedtls_mpi_write_binary( ( mbedtls_mpi const * ) &rsaContext->QP, rsaParams->coefficient, COEFFICIENT_LENGTH + 1 );

        if( mbedResult != 0 )
        {
            LogError( ( "Failed to parse RSA private key Chinese Remainder Theorem variables. \r\n" ) );
            result = CKR_ATTRIBUTE_VALUE_INVALID;
        }
    }

    if( result == CKR_OK )
    {
        /* When importing the fields, the pointer is incremented by 1
         * to remove the leading 0 padding (if it existed) and the original field length is used */


        CK_ATTRIBUTE privateKeyTemplate[] =
        {
            { CKA_CLASS,            NULL /* &privateKeyClass */, sizeof( CK_OBJECT_CLASS )                     },
            { CKA_KEY_TYPE,         NULL /* &privateKeyType */,  sizeof( CK_KEY_TYPE )                         },
            { CKA_LABEL,            label,                       ( CK_ULONG ) strlen( ( const char * ) label ) },
            { CKA_TOKEN,            NULL /* &trueObject */,      sizeof( CK_BBOOL )                            },
            { CKA_SIGN,             NULL /* &trueObject */,      sizeof( CK_BBOOL )                            },
            { CKA_MODULUS,          rsaParams->modulus + 1,      MODULUS_LENGTH                                },
            { CKA_PRIVATE_EXPONENT, rsaParams->d + 1,            D_LENGTH                                      },
            { CKA_PUBLIC_EXPONENT,  rsaParams->e + 1,            E_LENGTH                                      },
            { CKA_PRIME_1,          rsaParams->prime1 + 1,       PRIME_1_LENGTH                                },
            { CKA_PRIME_2,          rsaParams->prime2 + 1,       PRIME_2_LENGTH                                },
            { CKA_EXPONENT_1,       rsaParams->exponent1 + 1,    EXPONENT_1_LENGTH                             },
            { CKA_EXPONENT_2,       rsaParams->exponent2 + 1,    EXPONENT_2_LENGTH                             },
            { CKA_COEFFICIENT,      rsaParams->coefficient + 1,  COEFFICIENT_LENGTH                            }
        };

        /* Aggregate initializers must not use the address of an automatic variable. */
        /* See MSVC Compiler Warning C4221 */
        privateKeyTemplate[ 0 ].pValue = &privateKeyClass;
        privateKeyTemplate[ 1 ].pValue = &privateKeyType;
        privateKeyTemplate[ 3 ].pValue = &trueObject;
        privateKeyTemplate[ 4 ].pValue = &trueObject;

        result = functionList->C_CreateObject( session,
                                               ( CK_ATTRIBUTE_PTR ) &privateKeyTemplate,
                                               sizeof( privateKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                                               objectHandle );
    }

    if( NULL != rsaParams )
    {
        PKCS11_FREE( rsaParams );
    }

    return result;
}

/*-----------------------------------------------------------*/

/* Import the specified private key into storage. */
static CK_RV provisionPrivateKey( CK_SESSION_HANDLE session,
                                  uint8_t * privateKey,
                                  size_t privateKeyLength,
                                  uint8_t * label,
                                  CK_OBJECT_HANDLE_PTR objectHandle )
{
    CK_RV result = CKR_OK;
    mbedtls_pk_type_t mbedKeyType = MBEDTLS_PK_NONE;
    int mbedResult = 0;
    mbedtls_pk_context mbedPkContext = { 0 };

    mbedtls_pk_init( &mbedPkContext );
    mbedResult = mbedtls_pk_parse_key( &mbedPkContext, privateKey, privateKeyLength, NULL, 0 );

    if( mbedResult != 0 )
    {
        LogError( ( "Unable to parse private key.\r\n" ) );
        result = CKR_ARGUMENTS_BAD;
    }

    /* Determine whether the key to be imported is RSA or EC. */
    if( result == CKR_OK )
    {
        mbedKeyType = mbedtls_pk_get_type( &mbedPkContext );

        if( mbedKeyType == MBEDTLS_PK_RSA )
        {
            result = provisionPrivateRSAKey( session,
                                             label,
                                             objectHandle,
                                             &mbedPkContext );
        }
        else if( ( mbedKeyType == MBEDTLS_PK_ECDSA ) || ( mbedKeyType == MBEDTLS_PK_ECKEY ) || ( mbedKeyType == MBEDTLS_PK_ECKEY_DH ) )
        {
            result = provisionPrivateECKey( session,
                                            label,
                                            objectHandle,
                                            &mbedPkContext );
        }
        else
        {
            LogError( ( "Invalid private key type provided.  RSA-2048 and EC P-256 keys are supported.\r\n" ) );
            result = CKR_ARGUMENTS_BAD;
        }
    }

    mbedtls_pk_free( &mbedPkContext );

    return result;
}

/*-----------------------------------------------------------*/

/* Import the specified public key into storage. */
static CK_RV provisionPublicKey( CK_SESSION_HANDLE session,
                                 uint8_t * keyPtr,
                                 size_t keyLength,
                                 CK_KEY_TYPE publicKeyType,
                                 uint8_t * publicKeyLabel,
                                 CK_OBJECT_HANDLE_PTR publicKeyHandlePtr )
{
    CK_RV result;
    CK_BBOOL trueObject = CK_TRUE;
    CK_FUNCTION_LIST_PTR functionList;
    CK_OBJECT_CLASS class = CKO_PUBLIC_KEY;
    int mbedResult = 0;
    mbedtls_pk_context mbedPkContext = { 0 };

    result = C_GetFunctionList( &functionList );

    mbedtls_pk_init( &mbedPkContext );

    /* Try parsing the private key using mbedtls_pk_parse_key. */
    mbedResult = mbedtls_pk_parse_key( &mbedPkContext, keyPtr, keyLength, NULL, 0 );

    /* If mbedtls_pk_parse_key didn't work, maybe the private key is not included in the input passed in.
     * Try to parse just the public key. */
    if( mbedResult != 0 )
    {
        mbedResult = mbedtls_pk_parse_public_key( &mbedPkContext, keyPtr, keyLength );
    }

    if( mbedResult != 0 )
    {
        LogError( ( "Failed to parse the public key. \r\n" ) );
        result = CKR_ARGUMENTS_BAD;
    }

    if( ( result == CKR_OK ) && ( publicKeyType == CKK_RSA ) )
    {
        CK_BYTE publicExponent[] = { 0x01, 0x00, 0x01 };
        CK_BYTE modulus[ MODULUS_LENGTH + 1 ] = { 0 };

        mbedResult = mbedtls_rsa_export_raw( ( mbedtls_rsa_context * ) mbedPkContext.pk_ctx,
                                             ( unsigned char * ) &modulus, MODULUS_LENGTH + 1,
                                             NULL, 0,
                                             NULL, 0,
                                             NULL, 0,
                                             NULL, 0 );
        CK_ATTRIBUTE publicKeyTemplate[] =
        {
            { CKA_CLASS,           NULL /* &class */,         sizeof( CK_OBJECT_CLASS )                 },
            { CKA_KEY_TYPE,        NULL /* &publicKeyType */, sizeof( CK_KEY_TYPE )                     },
            { CKA_TOKEN,           NULL /* &trueObject */,    sizeof( trueObject )                      },
            { CKA_MODULUS,         NULL /* &modulus + 1 */,   MODULUS_LENGTH                            },          /* Extra byte allocated at beginning for 0 padding. */
            { CKA_VERIFY,          NULL /* &trueObject */,    sizeof( trueObject )                      },
            { CKA_PUBLIC_EXPONENT, NULL /* publicExponent */, sizeof( publicExponent )                  },
            { CKA_LABEL,           publicKeyLabel,            strlen( ( const char * ) publicKeyLabel ) }
        };

        /* Aggregate initializers must not use the address of an automatic variable. */
        /* See MSVC Compiler Warning C4221 */
        publicKeyTemplate[ 0 ].pValue = &class;
        publicKeyTemplate[ 1 ].pValue = &publicKeyType;
        publicKeyTemplate[ 2 ].pValue = &trueObject;
        publicKeyTemplate[ 3 ].pValue = (void *)&modulus[1];
        publicKeyTemplate[ 4 ].pValue = &trueObject;
        publicKeyTemplate[ 5 ].pValue = publicExponent;

        result = functionList->C_CreateObject( session,
                                               ( CK_ATTRIBUTE_PTR ) publicKeyTemplate,
                                               sizeof( publicKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                                               publicKeyHandlePtr );
    }
    else if( ( result == CKR_OK ) && ( publicKeyType == CKK_EC ) )
    {
        CK_BYTE ecParams[] = pkcs11DER_ENCODED_OID_P256;
        size_t length;
        CK_BYTE ecPoint[ pkcs11EC_POINT_LENGTH ] = { 0 };

        mbedtls_ecdsa_context * ecdsaContextPtr = ( mbedtls_ecdsa_context * ) mbedPkContext.pk_ctx;

        /* DER encoded EC point. Leave 2 bytes for the tag and length. */
        mbedResult = mbedtls_ecp_point_write_binary( &ecdsaContextPtr->grp,
                                                     &ecdsaContextPtr->Q,
                                                     MBEDTLS_ECP_PF_UNCOMPRESSED,
                                                     &length,
                                                     ecPoint + 2,
                                                     sizeof( ecPoint ) - 2 );
        ecPoint[ 0 ] = 0x04; /* Octet string. */
        ecPoint[ 1 ] = ( CK_BYTE ) length;

        CK_ATTRIBUTE publicKeyTemplate[] =
        {
            { CKA_CLASS,     NULL /* &class */,         sizeof( class )                           },
            { CKA_KEY_TYPE,  NULL /* &publicKeyType */, sizeof( publicKeyType )                   },
            { CKA_TOKEN,     NULL /* &trueObject */,    sizeof( trueObject )                      },
            { CKA_VERIFY,    NULL /* &trueObject */,    sizeof( trueObject )                      },
            { CKA_EC_PARAMS, NULL /* ecParams */,       sizeof( ecParams )                        },
            { CKA_EC_POINT,  NULL /* ecPoint */,        length + 2                                },
            { CKA_LABEL,     publicKeyLabel,            strlen( ( const char * ) publicKeyLabel ) }
        };

        /* Aggregate initializers must not use the address of an automatic variable. */
        /* See MSVC Compiler Warning C4221 */
        publicKeyTemplate[ 0 ].pValue = &class;
        publicKeyTemplate[ 1 ].pValue = &publicKeyType;
        publicKeyTemplate[ 2 ].pValue = &trueObject;
        publicKeyTemplate[ 3 ].pValue = &trueObject;
        publicKeyTemplate[ 4 ].pValue = ecParams;
        publicKeyTemplate[ 5 ].pValue = ecPoint;

        result = functionList->C_CreateObject( session,
                                               ( CK_ATTRIBUTE_PTR ) publicKeyTemplate,
                                               sizeof( publicKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                                               publicKeyHandlePtr );
    }
    else
    {
        result = CKR_ATTRIBUTE_VALUE_INVALID;
        LogError( ( "Invalid key type. Supported options are CKK_RSA and CKK_EC" ) );
    }

    mbedtls_pk_free( &mbedPkContext );

    return result;
}

/*-----------------------------------------------------------*/

/* Generate a new ECDSA key pair using curve P256. */
static CK_RV generateKeyPairEC( CK_SESSION_HANDLE session,
                                uint8_t * privateKeyLabel,
                                uint8_t * publicKeyLabel,
                                CK_OBJECT_HANDLE_PTR privateKeyHandlePtr,
                                CK_OBJECT_HANDLE_PTR publicKeyHandlePtr )
{
    CK_RV result;
    CK_MECHANISM mechanism =
    {
        CKM_EC_KEY_PAIR_GEN, NULL_PTR, 0
    };
    CK_FUNCTION_LIST_PTR functionList;
    CK_BYTE ecParams[] = pkcs11DER_ENCODED_OID_P256; /* prime256v1 */
    CK_KEY_TYPE keyType = CKK_EC;

    CK_BBOOL trueObject = CK_TRUE;
    CK_ATTRIBUTE publicKeyTemplate[] =
    {
        { CKA_KEY_TYPE,  NULL /* &keyType */,    sizeof( keyType )                         },
        { CKA_VERIFY,    NULL /* &trueObject */, sizeof( trueObject )                      },
        { CKA_EC_PARAMS, NULL /* ecParams */,    sizeof( ecParams )                        },
        { CKA_LABEL,     publicKeyLabel,         strlen( ( const char * ) publicKeyLabel ) }
    };

    /* Aggregate initializers must not use the address of an automatic variable. */
    /* See MSVC Compiler Warning C4221 */
    publicKeyTemplate[ 0 ].pValue = &keyType;
    publicKeyTemplate[ 1 ].pValue = &trueObject;
    publicKeyTemplate[ 2 ].pValue = &ecParams;

    CK_ATTRIBUTE privateKeyTemplate[] =
    {
        { CKA_KEY_TYPE, NULL /* &keyType */,    sizeof( keyType )                          },
        { CKA_TOKEN,    NULL /* &trueObject */, sizeof( trueObject )                       },
        { CKA_PRIVATE,  NULL /* &trueObject */, sizeof( trueObject )                       },
        { CKA_SIGN,     NULL /* &trueObject */, sizeof( trueObject )                       },
        { CKA_LABEL,    privateKeyLabel,        strlen( ( const char * ) privateKeyLabel ) }
    };

    /* Aggregate initializers must not use the address of an automatic variable. */
    /* See MSVC Compiler Warning C4221 */
    privateKeyTemplate[ 0 ].pValue = &keyType;
    privateKeyTemplate[ 1 ].pValue = &trueObject;
    privateKeyTemplate[ 2 ].pValue = &trueObject;
    privateKeyTemplate[ 3 ].pValue = &trueObject;

    result = C_GetFunctionList( &functionList );

    result = functionList->C_GenerateKeyPair( session,
                                              &mechanism,
                                              publicKeyTemplate,
                                              sizeof( publicKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                                              privateKeyTemplate, sizeof( privateKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                                              publicKeyHandlePtr,
                                              privateKeyHandlePtr );

    return result;
}

/*-----------------------------------------------------------*/

/* Import the specified X.509 client certificate into storage. */
static CK_RV provisionCertificate( CK_SESSION_HANDLE session,
                                   uint8_t * certificate,
                                   size_t certificateLength,
                                   uint8_t * label,
                                   CK_OBJECT_HANDLE_PTR objectHandle )
{
    PKCS11_CertificateTemplate_t certificateTemplate;
    CK_OBJECT_CLASS certificateClass = CKO_CERTIFICATE;
    CK_CERTIFICATE_TYPE certificateType = CKC_X_509;
    CK_FUNCTION_LIST_PTR functionList;
    CK_RV result;
    uint8_t * derObject = NULL;
    int32_t conversion = 0;
    size_t derLen = 0;
    CK_BBOOL tokenStorage = CK_TRUE;

    /* TODO: Subject is a required attribute.
     * Currently, this field is not used by FreeRTOS ports,
     * this should be updated so that subject matches proper
     * format for future ports. */
    CK_BYTE subject[] = "TestSubject";

    /* Initialize the client certificate template. */
    certificateTemplate.xObjectClass.type = CKA_CLASS;
    certificateTemplate.xObjectClass.pValue = &certificateClass;
    certificateTemplate.xObjectClass.ulValueLen = sizeof( certificateClass );
    certificateTemplate.xSubject.type = CKA_SUBJECT;
    certificateTemplate.xSubject.pValue = subject;
    certificateTemplate.xSubject.ulValueLen = strlen( ( const char * ) subject );
    certificateTemplate.xValue.type = CKA_VALUE;
    certificateTemplate.xValue.pValue = ( CK_VOID_PTR ) certificate;
    certificateTemplate.xValue.ulValueLen = ( CK_ULONG ) certificateLength;
    certificateTemplate.xLabel.type = CKA_LABEL;
    certificateTemplate.xLabel.pValue = ( CK_VOID_PTR ) label;
    certificateTemplate.xLabel.ulValueLen = strlen( ( const char * ) label );
    certificateTemplate.xCertificateType.type = CKA_CERTIFICATE_TYPE;
    certificateTemplate.xCertificateType.pValue = &certificateType;
    certificateTemplate.xCertificateType.ulValueLen = sizeof( CK_CERTIFICATE_TYPE );
    certificateTemplate.xTokenObject.type = CKA_TOKEN;
    certificateTemplate.xTokenObject.pValue = &tokenStorage;
    certificateTemplate.xTokenObject.ulValueLen = sizeof( tokenStorage );

    result = C_GetFunctionList( &functionList );

    /* Test for a valid certificate: 0x2d is '-', as in ----- BEGIN CERTIFICATE. */
    if( ( certificate == NULL ) || ( certificate[ 0 ] != 0x2d ) )
    {
        result = CKR_ATTRIBUTE_VALUE_INVALID;
    }

    if( result == CKR_OK )
    {
        /* Convert the certificate to DER format if it was in PEM. The DER key
         * should be about 3/4 the size of the PEM key, so mallocing the PEM key
         * size is sufficient. */
        derObject = PKCS11_MALLOC( certificateTemplate.xValue.ulValueLen );
        derLen = certificateTemplate.xValue.ulValueLen;

        if( derObject != NULL )
        {
            conversion = convert_pem_to_der( certificateTemplate.xValue.pValue,
                                             certificateTemplate.xValue.ulValueLen,
                                             derObject,
                                             &derLen );

            if( 0 != conversion )
            {
                result = CKR_ARGUMENTS_BAD;
            }
        }
        else
        {
            result = CKR_HOST_MEMORY;
        }
    }

    if( result == CKR_OK )
    {
        /* Set the template pointers to refer to the DER converted objects. */
        certificateTemplate.xValue.pValue = derObject;
        certificateTemplate.xValue.ulValueLen = derLen;
    }

    /* Best effort clean-up of the existing object, if it exists. */
    if( result == CKR_OK )
    {
        destroyProvidedObjects( session,
                                &label,
                                &certificateClass,
                                1 );
    }

    /* Create an object using the encoded client certificate. */
    if( result == CKR_OK )
    {
        LogError( ( "Write certificate...\r\n" ) );

        result = functionList->C_CreateObject( session,
                                               ( CK_ATTRIBUTE_PTR ) &certificateTemplate,
                                               sizeof( certificateTemplate ) / sizeof( CK_ATTRIBUTE ),
                                               objectHandle );
    }

    if( derObject != NULL )
    {
        PKCS11_FREE( derObject );
    }

    return result;
}

/*-----------------------------------------------------------*/

/* Delete the specified crypto object from storage. */
static CK_RV destroyProvidedObjects( CK_SESSION_HANDLE session,
                                     CK_BYTE_PTR * pkcsLabelsPtr,
                                     CK_OBJECT_CLASS * class,
                                     CK_ULONG count )
{
    CK_RV result;
    CK_FUNCTION_LIST_PTR functionList;
    CK_OBJECT_HANDLE objectHandle;
    CK_BYTE * labelPtr;
    CK_ULONG index = 0;

    result = C_GetFunctionList( &functionList );

    for( index = 0; index < count; index++ )
    {
        labelPtr = pkcsLabelsPtr[ index ];

        result = xFindObjectWithLabelAndClass( session,
                                               ( char * ) labelPtr,
                                               strlen( ( char * ) labelPtr ),
                                               class[ index ],
                                               &objectHandle );

        while( ( result == CKR_OK ) && ( objectHandle != CK_INVALID_HANDLE ) )
        {
            result = functionList->C_DestroyObject( session, objectHandle );

            /* PKCS #11 allows a module to maintain multiple objects with the same
             * label and type. The intent of this loop is to try to delete all of them.
             * However, to avoid getting stuck, we won't try to find another object
             * of the same label/type if the previous delete failed. */
            if( result == CKR_OK )
            {
                result = xFindObjectWithLabelAndClass( session,
                                                       ( char * ) labelPtr,
                                                       strlen( ( char * ) labelPtr ),
                                                       class[ index ],
                                                       &objectHandle );
            }
            else
            {
                break;
            }
        }

        if( result == CKR_FUNCTION_NOT_SUPPORTED )
        {
            break;
        }
    }

    return result;
}

/*-----------------------------------------------------------*/
static uint8_t _pkcs11_test(uint8_t len, char *param[])
{
    UNITY_BEGIN();
    RUN_TEST(test_Get_Function_List);
    RUN_TEST(test_Initialize_Finalize);
    RUN_TEST(test_GetSlotList);
    RUN_TEST(test_OpenSession_CloseSession);
    RUN_TEST(test_Digest);
    RUN_TEST(test_Digest_ErrorConditions);
    RUN_TEST(test_GenerateRandom);
    RUN_TEST(test_CreateObject_RSA);
    RUN_TEST(test_FindObject_RSA);
    RUN_TEST(test_GetAttributeValue_RSA);
    RUN_TEST(test_DestroyObject_RSA);
    RUN_TEST(test_Sign_RSA);
    RUN_TEST(test_DestroyObject_RSA);
    RUN_TEST(test_CreateObject_EC);
    RUN_TEST(test_Sign_EC);
    RUN_TEST(test_Verify_EC);
    RUN_TEST(test_FindObject_EC);
    RUN_TEST(test_GetAttributeValue_EC);
    RUN_TEST(test_DestoryObject_EC);
    RUN_TEST(test_GenerateKeyPair_EC);
    RUN_TEST(test_DestoryObject_EC);
    UNITY_END();
    return 0;
}

#ifdef MTK_OS_CLI_ENABLE
cmd_t pkcs11_cli[] = {
    { "test",   "test",      _pkcs11_test       },
    { NULL }
};
#endif /* MTK_OS_CLI_ENABLE */

