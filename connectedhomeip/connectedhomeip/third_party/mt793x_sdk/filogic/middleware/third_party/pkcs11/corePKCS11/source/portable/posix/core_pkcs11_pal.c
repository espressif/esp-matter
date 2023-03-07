/*
 * corePKCS11 PAL for Linux V2.0.0
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
 * @file core_pkcs11_pal.c
 * @brief Linux file save and read implementation
 * for PKCS #11 based on mbedTLS with for software keys. This
 * file deviates from the FreeRTOS style standard for some function names and
 * data types in order to maintain compliance with the PKCS #11 standard.
 */
/*-----------------------------------------------------------*/

/* PKCS 11 includes. */
#include "core_pkcs11_config.h"
#include "core_pkcs11.h"

/* C runtime includes. */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @ingroup pkcs11_macros
 * @brief Macros for managing PKCS #11 objects in flash.
 *
 */
#define pkcs11palFILE_NAME_CLIENT_CERTIFICATE    "FreeRTOS_P11_Certificate.dat"       /**< The file name of the Certificate object. */
#define pkcs11palFILE_NAME_KEY                   "FreeRTOS_P11_Key.dat"               /**< The file name of the Key object. */
#define pkcs11palFILE_CODE_SIGN_PUBLIC_KEY       "FreeRTOS_P11_CodeSignKey.dat"       /**< The file name of the Code Sign Key object. */

/**
 * @ingroup pkcs11_enums
 * @brief Enums for managing PKCS #11 object types.
 *
 */
enum eObjectHandles
{
    eInvalidHandle = 0,       /**< According to PKCS #11 spec, 0 is never a valid object handle. */
    eAwsDevicePrivateKey = 1, /**< Private Key. */
    eAwsDevicePublicKey,      /**< Public Key. */
    eAwsDeviceCertificate,    /**< Certificate. */
    eAwsCodeSigningKey        /**< Code Signing Key. */
};

/*-----------------------------------------------------------*/

/**
 * @brief Checks to see if a file exists
 *
 * @param[in] pcFileName         The name of the file to check for existance.
 *
 * @returns CKR_OK if the file exists, CKR_OBJECT_HANDLE_INVALID if not.
 */
static CK_RV prvFileExists( const char * pcFileName )
{
    FILE * pxFile = NULL;
    CK_RV xReturn = CKR_OK;

    /* fopen returns NULL if the file does not exist. */
    pxFile = fopen( pcFileName, "r" );

    if( pxFile == NULL )
    {
        xReturn = CKR_OBJECT_HANDLE_INVALID;
        LogError( ( "Could not open %s for reading.", pcFileName ) );
    }
    else
    {
        ( void ) fclose( pxFile );
        LogDebug( ( "Found file %s and was able to open it for reading.", pcFileName ) );
    }

    return xReturn;
}

/**
 * @brief Checks to see if a file exists
 *
 * @param[in] pcLabel            The PKCS #11 label to convert to a file name
 * @param[out] pcFileName        The name of the file to check for existance.
 * @param[out] pHandle           The type of the PKCS #11 object.
 *
 */
static void prvLabelToFilenameHandle( const char * pcLabel,
                                      const char ** pcFileName,
                                      CK_OBJECT_HANDLE_PTR pHandle )
{
    if( ( pcLabel != NULL ) && ( pHandle != NULL ) && ( pcFileName != NULL ) )
    {
        if( 0 == strncmp( pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS,
                          pcLabel,
                          sizeof( pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS ) ) )
        {
            *pcFileName = pkcs11palFILE_NAME_CLIENT_CERTIFICATE;
            *pHandle = ( CK_OBJECT_HANDLE ) eAwsDeviceCertificate;
        }
        else if( 0 == strncmp( pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS,
                               pcLabel,
                               sizeof( pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS ) ) )
        {
            *pcFileName = pkcs11palFILE_NAME_KEY;
            *pHandle = ( CK_OBJECT_HANDLE ) eAwsDevicePrivateKey;
        }
        else if( 0 == strncmp( pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS,
                               pcLabel,
                               sizeof( pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS ) ) )
        {
            *pcFileName = pkcs11palFILE_NAME_KEY;
            *pHandle = ( CK_OBJECT_HANDLE ) eAwsDevicePublicKey;
        }
        else if( 0 == strncmp( pkcs11configLABEL_CODE_VERIFICATION_KEY,
                               pcLabel,
                               sizeof( pkcs11configLABEL_CODE_VERIFICATION_KEY ) ) )
        {
            *pcFileName = pkcs11palFILE_CODE_SIGN_PUBLIC_KEY;
            *pHandle = ( CK_OBJECT_HANDLE ) eAwsCodeSigningKey;
        }
        else
        {
            *pcFileName = NULL;
            *pHandle = ( CK_OBJECT_HANDLE ) eInvalidHandle;
        }

        LogDebug( ( "Converted %s to %s", pcLabel, *pcFileName ) );
    }
    else
    {
        LogError( ( "Could not convert label to filename. Received a NULL parameter." ) );
    }
}

/**
 * @brief Maps object handle to file name
 *
 * @param[in] pcLabel            The PKCS #11 label to convert to a file name
 * @param[out] pcFileName        The name of the file to check for existance.
 * @param[out] pHandle           The type of the PKCS #11 object.
 *
 */
static CK_RV prvHandleToFilename( CK_OBJECT_HANDLE xHandle,
                                  const char ** pcFileName,
                                  CK_BBOOL * pIsPrivate )
{
    CK_RV xReturn = CKR_OK;

    if( pcFileName != NULL )
    {
        switch( ( CK_OBJECT_HANDLE ) xHandle )
        {
            case eAwsDeviceCertificate:
                *pcFileName = pkcs11palFILE_NAME_CLIENT_CERTIFICATE;
                /* coverity[misra_c_2012_rule_10_5_violation] */
                *pIsPrivate = ( CK_BBOOL ) CK_FALSE;
                break;

            case eAwsDevicePrivateKey:
                *pcFileName = pkcs11palFILE_NAME_KEY;
                /* coverity[misra_c_2012_rule_10_5_violation] */
                *pIsPrivate = ( CK_BBOOL ) CK_TRUE;
                break;

            case eAwsDevicePublicKey:
                *pcFileName = pkcs11palFILE_NAME_KEY;
                /* coverity[misra_c_2012_rule_10_5_violation] */
                *pIsPrivate = ( CK_BBOOL ) CK_FALSE;
                break;

            case eAwsCodeSigningKey:
                *pcFileName = pkcs11palFILE_CODE_SIGN_PUBLIC_KEY;
                /* coverity[misra_c_2012_rule_10_5_violation] */
                *pIsPrivate = ( CK_BBOOL ) CK_FALSE;
                break;

            default:
                xReturn = CKR_KEY_HANDLE_INVALID;
                break;
        }
    }
    else
    {
        LogError( ( "Could not convert label to filename. Received a NULL parameter." ) );
    }

    return xReturn;
}

/**
 * @brief Reads object value from file system.
 *
 * @param[in] pcLabel            The PKCS #11 label to convert to a file name
 * @param[out] pcFileName        The name of the file to check for existance.
 * @param[out] pHandle           The type of the PKCS #11 object.
 *
 */
static CK_RV prvReadData( const char * pcFileName,
                          CK_BYTE_PTR * ppucData,
                          CK_ULONG_PTR pulDataSize )
{
    CK_RV xReturn = CKR_OK;
    FILE * pxFile = NULL;
    size_t lSize = 0;

    pxFile = fopen( pcFileName, "r" );

    if( NULL == pxFile )
    {
        LogError( ( "PKCS #11 PAL failed to get object value. "
                    "Could not open file named %s for reading.", pcFileName ) );
        xReturn = CKR_FUNCTION_FAILED;
    }
    else
    {
        ( void ) fseek( pxFile, 0, SEEK_END );
        lSize = ftell( pxFile );
        ( void ) fseek( pxFile, 0, SEEK_SET );

        if( lSize > 0UL )
        {
            *pulDataSize = lSize;
            *ppucData = malloc( *pulDataSize );

            if( NULL == *ppucData )
            {
                LogError( ( "Could not get object value. Malloc failed to allocate memory." ) );
                xReturn = CKR_HOST_MEMORY;
            }
        }
        else
        {
            LogError( ( "Could not get object value. Failed to determine object size." ) );
            xReturn = CKR_FUNCTION_FAILED;
        }
    }

    if( CKR_OK == xReturn )
    {
        lSize = 0;
        lSize = fread( *ppucData, sizeof( uint8_t ), *pulDataSize, pxFile );

        if( lSize != *pulDataSize )
        {
            LogError( ( "PKCS #11 PAL Failed to get object value. Expected to read %ld "
                        "from %s but received %ld", *pulDataSize, pcFileName, lSize ) );
            xReturn = CKR_FUNCTION_FAILED;
        }
    }

    if( NULL != pxFile )
    {
        ( void ) fclose( pxFile );
    }

    return xReturn;
}

/*-----------------------------------------------------------*/

CK_RV PKCS11_PAL_Initialize( void )
{
    return CKR_OK;
}

CK_OBJECT_HANDLE PKCS11_PAL_SaveObject( CK_ATTRIBUTE_PTR pxLabel,
                                        CK_BYTE_PTR pucData,
                                        CK_ULONG ulDataSize )
{
    FILE * pxFile = NULL;
    size_t ulBytesWritten;
    const char * pcFileName = NULL;
    CK_OBJECT_HANDLE xHandle = ( CK_OBJECT_HANDLE ) eInvalidHandle;

    if( ( pxLabel != NULL ) && ( pucData != NULL ) )
    {
        /* Converts a label to its respective filename and handle. */
        prvLabelToFilenameHandle( pxLabel->pValue,
                                  &pcFileName,
                                  &xHandle );
    }
    else
    {
        LogError( ( "Could not save object. Received invalid parameters." ) );
    }

    if( pcFileName != NULL )
    {
        /* Overwrite the file every time it is saved. */
        pxFile = fopen( pcFileName, "w" );

        if( NULL == pxFile )
        {
            LogError( ( "PKCS #11 PAL was unable to save object to file. "
                        "The PAL was unable to open a file with name %s in write mode.", pcFileName ) );
            xHandle = ( CK_OBJECT_HANDLE ) eInvalidHandle;
        }
        else
        {
            ulBytesWritten = fwrite( pucData, sizeof( uint8_t ), ulDataSize, pxFile );

            if( ulBytesWritten != ulDataSize )
            {
                LogError( ( "PKCS #11 PAL was unable to save object to file. "
                            "Expected to write %lu bytes, but wrote %lu bytes.", ulDataSize, ulBytesWritten ) );
                xHandle = ( CK_OBJECT_HANDLE ) eInvalidHandle;
            }
            else
            {
                LogDebug( ( "Successfully wrote %lu to %s", ulBytesWritten, pcFileName ) );
            }
        }

        if( NULL != pxFile )
        {
            ( void ) fclose( pxFile );
        }
    }
    else
    {
        LogError( ( "Could not save object. Unable to find the correct file." ) );
    }

    return xHandle;
}

/*-----------------------------------------------------------*/


CK_OBJECT_HANDLE PKCS11_PAL_FindObject( CK_BYTE_PTR pxLabel,
                                        CK_ULONG usLength )
{
    const char * pcFileName = NULL;
    CK_OBJECT_HANDLE xHandle = ( CK_OBJECT_HANDLE ) eInvalidHandle;

    ( void ) usLength;

    if( pxLabel != NULL )
    {
        prvLabelToFilenameHandle( ( const char * ) pxLabel,
                                  &pcFileName,
                                  &xHandle );

        if( CKR_OK != prvFileExists( pcFileName ) )
        {
            xHandle = ( CK_OBJECT_HANDLE ) eInvalidHandle;
        }
    }
    else
    {
        LogError( ( "Could not find object. Received a NULL label." ) );
    }

    return xHandle;
}
/*-----------------------------------------------------------*/

CK_RV PKCS11_PAL_GetObjectValue( CK_OBJECT_HANDLE xHandle,
                                 CK_BYTE_PTR * ppucData,
                                 CK_ULONG_PTR pulDataSize,
                                 CK_BBOOL * pIsPrivate )
{
    CK_RV xReturn = CKR_OK;
    const char * pcFileName = NULL;


    if( ( ppucData == NULL ) || ( pulDataSize == NULL ) || ( pIsPrivate == NULL ) )
    {
        xReturn = CKR_ARGUMENTS_BAD;
        LogError( ( "Could not get object value. Received a NULL argument." ) );
    }
    else
    {
        xReturn = prvHandleToFilename( xHandle, &pcFileName, pIsPrivate );
    }

    if( xReturn == CKR_OK )
    {
        xReturn = prvReadData( pcFileName, ppucData, pulDataSize );
    }

    return xReturn;
}

/*-----------------------------------------------------------*/

void PKCS11_PAL_GetObjectValueCleanup( CK_BYTE_PTR pucData,
                                       CK_ULONG ulDataSize )
{
    /* Unused parameters. */
    ( void ) ulDataSize;

    if( NULL != pucData )
    {
        free( pucData );
    }
}

/*-----------------------------------------------------------*/

CK_RV PKCS11_PAL_DestroyObject( CK_OBJECT_HANDLE xHandle )
{
    const char * pcFileName = NULL;
    CK_BBOOL xIsPrivate = CK_TRUE;
    CK_RV xResult = CKR_OBJECT_HANDLE_INVALID;
    int ret = 0;


    xResult = prvHandleToFilename( xHandle,
                                   &pcFileName,
                                   &xIsPrivate );

    if( ( xResult == CKR_OK ) && ( prvFileExists( pcFileName ) == CKR_OK ) )
    {
        ret = remove( pcFileName );

        if( ret != 0 )
        {
            xResult = CKR_FUNCTION_FAILED;
        }
    }

    return xResult;
}

/*-----------------------------------------------------------*/
