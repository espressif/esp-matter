/*
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 * Copyright (c) 2019-2020 Arm Limited. All Rights Reserved.
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
 */

/*
 * This file is derivative of amazon-freertos\vendors\st\boards\stm32l475_discovery\
 * ports\pkcs11\iot_pkcs11_pal.c(amazon-freertos commit 74875b1d2)
 */

#ifndef __IOT_PKCS11_PSA_OBJECT_MANAGEMENT_H__
#define __IOT_PKCS11_PSA_OBJECT_MANAGEMENT_H__

#include <string.h>
#include <stdbool.h>

/* PKCS#11 includes. */
#include "core_pkcs11_config.h"
#include "core_pkcs11.h"

/* PSA includes. */
#include "psa/crypto.h"
#include "psa/protected_storage.h"

/* mbedTLS includes. */
#include "mbedtls/pk.h"

/*
 * Define the key ID of the device keys which will be saved as
 * persistent keys in TF-M.
 */
#ifndef PSA_DEVICE_PRIVATE_KEY_ID
#define PSA_DEVICE_PRIVATE_KEY_ID     ( ( psa_key_id_t )0x01 )
#endif

#ifndef PSA_DEVICE_PUBLIC_KEY_ID
#define PSA_DEVICE_PUBLIC_KEY_ID      ( ( psa_key_id_t )0x10 )
#endif

#ifndef PSA_CODE_VERIFICATION_KEY_ID
#define PSA_CODE_VERIFICATION_KEY_ID  ( ( psa_key_id_t )0x11 )
#endif

/* UID that indicates the device certificate data in PSA protected storage service. */
#ifndef PSA_DEVICE_CERTIFICATE_UID
#define PSA_DEVICE_CERTIFICATE_UID    ( ( psa_storage_uid_t )5 )
#endif

/* UID that indicates the jitp certificate data in PSA protected storage service. */
#ifndef PSA_JITP_CERTIFICATE_UID
#define PSA_JITP_CERTIFICATE_UID      ( ( psa_storage_uid_t )6 )
#endif

/* UID that indicates the root certificate data in PSA protected storage service. */
#ifndef PSA_ROOT_CERTIFICATE_UID
#define PSA_ROOT_CERTIFICATE_UID      ( ( psa_storage_uid_t )7 )
#endif

/**
 * @brief The oject handle field is N/A/
 */
#define pkcs11OBJECT_HANDLE_NA        ( 0x0000 )

/**
 * The max length(in byte) of the privateKey field of the ECPrivateKey format
 * defined by RFC 5915.
 */
#define EC_PRIVATE_KEY_MAX_LENGTH     ( pkcs11EC_POINT_LENGTH )

typedef enum eObjectHandles
{
    eInvalidHandle = 0, /* From PKCS #11 spec: 0 is never a valid object handle.*/
    eAwsDevicePrivateKey = 1,
    eAwsDevicePublicKey = 2,
    eAwsDeviceCertificate = 3,
    eAwsCodeVerifyingKey = 4,
    eAwsJitpCertificate = 5,
    eAwsRootCertificate = 6
}P11ObjectHandles_t;

/**
 * @brief Structure for certificates/keys storage.
 */
typedef struct
{
    psa_key_handle_t uxDevicePrivateKey;  /* Device private key handle that returned by PSA crypto service. */
    psa_key_handle_t uxDevicePublicKey;  /* Device public key handle that returned by PSA crypto service. */
    psa_key_handle_t uxCodeVerifyKey;  /* Key handle of the key that used by over-the-air update code to verify an incoming signed image. */
    BaseType_t xDeviceCertificateMark;
    BaseType_t xDevicePrivateKeyMark;
    BaseType_t xDevicePublicKeyMark;
    BaseType_t xCodeVerifyKeyMark;
    BaseType_t xJitpCertificateMark;
    BaseType_t xRootCertificateMark;
} P11KeyConfig_t;

/**
* @brief Writes a file to local storage.
*
* Port-specific file write for crytographic information.
*
* @param[in] pxClass       Class of the object to be saved.
* @param[in] pxLabel       Label of the object to be saved.
* @param[in] pucData       Data buffer to be written to file
* @param[in] ulDataSize    Size (in bytes) of data to be saved.
* @param[in] pvContext     Context of the key.
*
* @return The file handle of the object that was stored.
*/
CK_OBJECT_HANDLE PKCS11PSASaveObject( CK_ATTRIBUTE_PTR pxClass,
    CK_ATTRIBUTE_PTR pxLabel,
    uint8_t * pucData,
    uint32_t ulDataSize,
    mbedtls_pk_context *pvContext );

/**
* @brief Gets the value of an object in storage, by handle.
*
* Port-specific file access for cryptographic information.
*
* This call dynamically allocates the buffer which object value
* data is copied into.  PKCS11PSAGetObjectValueCleanup()
* should be called after each use to free the dynamically allocated
* buffer.
*
* @sa PKCS11PSAGetObjectValueCleanup
*
* @param[in] pcFileName    The name of the file to be read.
* @param[out] ppucData     Pointer to buffer for file data.
* @param[out] pulDataSize  Size (in bytes) of data located in file.
* @param[out] pIsPrivate   Boolean indicating if value is private (CK_TRUE)
*                          or exportable (CK_FALSE)
*
* @return CKR_OK if operation was successful.  CKR_KEY_HANDLE_INVALID if
* no such object handle was found, CKR_DEVICE_MEMORY if memory for
* buffer could not be allocated, CKR_FUNCTION_FAILED for device driver
* error.
*/
CK_RV PKCS11PSAGetObjectValue( CK_OBJECT_HANDLE xHandle,
    uint8_t * ppucData,
    size_t * pulDataSize,
    CK_BBOOL * pIsPrivate );

/**
* @brief Cleanup after PKCS11PSAGetObjectValue().
*
* @param[in] pucData       The buffer to free.
*                          (*ppucData from PKCS11PSAGetObjectValue())
* @param[in] ulDataSize    The length of the buffer to free.
*                          (*pulDataSize from PKCS11PSAGetObjectValue())
*/
void PKCS11PSAGetObjectValueCleanup( uint8_t * pucData, uint32_t ulDataSize );

/**
* @brief Import an object into P11KeyConfig.
*
* @param[in] pLableValue        The lable of the object.
*
* @param[in] ulLableLen         The length of the object label.
*
* @param[in] uxKeyHandle        The key handle.
*
*/
void PKCS11PSAContextImportObject( CK_VOID_PTR pLableValue,
                                   CK_LONG  ulLableLen,
                                   psa_key_handle_t uxKeyHandle );

/**
* @brief Close or remove an object from PSA and update P11KeyConfig accordingly.
*
* @param[in] pLableValue        The lable of the object.
*
* @param[in] ulLableLen         The length of the object label.
*
* @param[in] permanent          Destroy the key or just close the key.
*
*/
CK_RV PKCS11PSARemoveObject(uint8_t * pcLable, size_t xLabelLength, bool permanent);

/**
* @brief Get the PSA key handle by the object label.
*
* @param[in] pLableValue        The lable of the object.
*
* @param[in] ulLableLen         The length of the object label.
*
* @param[in] uxKeyHandle        The returned key handle.
*
*/
CK_RV PKCS11PSAGetKeyHandle( uint8_t * pcLable, size_t xLabelLength, psa_key_handle_t * uxKeyHandle );

#endif /* __IOT_PKCS11_PSA_OBJECT_MANAGEMENT_H__ */
