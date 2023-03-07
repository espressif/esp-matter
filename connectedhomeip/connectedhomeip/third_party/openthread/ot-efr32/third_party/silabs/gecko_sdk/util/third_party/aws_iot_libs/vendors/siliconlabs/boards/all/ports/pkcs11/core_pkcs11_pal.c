/*
 * FreeRTOS PKCS #11 PAL V1.0.0
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
 * @brief Device specific helpers for PKCS11 Interface.
 */

/* FreeRTOS Includes. */
#include "core_pkcs11.h"
#include "core_pkcs11_config.h"
#include "FreeRTOS.h"

/* C runtime includes. */
#include <stdio.h>
#include <string.h>

/* Silabs NVM3 driver */
#include "nvm3.h"
#include "nvm3_default.h"

#define NVM3_INVALID_KEY   0
#define NVM3_DEV_CERT_KEY  0x550
#define NVM3_PRIVATE_KEY   0x551
#define NVM3_PUBLIC_KEY    0x552
#define NVM3_SIGN_KEY      0x553

static uint32_t prvLabelToKey( char *pcLabel )
{
  if( 0 == strncmp( pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS,
                    pcLabel,
                    sizeof( pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS ) ) )
  {
    return NVM3_DEV_CERT_KEY;
  }
  else if( 0 == strncmp( pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS,
                         pcLabel,
                         sizeof( pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS ) ) )
  {
    return NVM3_PRIVATE_KEY;
  }
  else if( 0 == strncmp( pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS,
                         pcLabel,
                         sizeof( pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS ) ) )
  {
    return NVM3_PUBLIC_KEY;
  }
  else if( 0 == strncmp( pkcs11configLABEL_CODE_VERIFICATION_KEY,
                         pcLabel,
                         sizeof( pkcs11configLABEL_CODE_VERIFICATION_KEY ) ) )
  {
    return NVM3_SIGN_KEY;
  } else {
    return NVM3_INVALID_KEY;
  }
}

/**
 * @brief Initializes the PKCS #11 PAL.
 *
 * @return CKR_OK on success.
 * CKR_FUNCTION_FAILED on failure.
 */
CK_RV PKCS11_PAL_Initialize( void )
{
  /* nothing to do here */
  return CKR_OK;
}

/**
 * @brief Saves an object in non-volatile storage.
 *
 * Port-specific file write for cryptographic information.
 *
 * @param[in] pxLabel       Attribute containing label of the object to be stored.
 * @param[in] pucData       The object data to be saved.
 * @param[in] ulDataSize    Size (in bytes) of object data.
 *
 * @return The object handle if successful.
 * eInvalidHandle = 0 if unsuccessful.
 */
CK_OBJECT_HANDLE PKCS11_PAL_SaveObject( CK_ATTRIBUTE_PTR pxLabel,
                                        CK_BYTE_PTR pucData,
                                        CK_ULONG ulDataSize )
{
  Ecode_t xEcode = ECODE_NVM3_OK;
  uint32_t ulKey = 0;

  /* get a NVM3 key for pxLabel */
  ulKey = prvLabelToKey((char *) pxLabel->pValue);

  /* return a handle of 0 if we couldn't increment the counter */
  if (ulKey == NVM3_INVALID_KEY) {
    return 0;
  }

  /* store the data in NVM3 using NVM3 key */
  xEcode = nvm3_writeData(nvm3_defaultHandle, ulKey, pucData, ulDataSize);

  /* return a handle of 0 if we couldn't store the data */
  if (xEcode != ECODE_NVM3_OK) {
    return 0;
  }

  /* return the key as a handle */
  return ulKey;
}

/**
* @brief Translates a PKCS #11 label into an object handle.
*
* Port-specific object handle retrieval.
*
*
* @param[in] pxLabel         Pointer to the label of the object
*                           who's handle should be found.
* @param[in] usLength       The length of the label, in bytes.
*
* @return The object handle if operation was successful.
* Returns eInvalidHandle if unsuccessful.
*/
CK_OBJECT_HANDLE PKCS11_PAL_FindObject( CK_BYTE_PTR pxLabel,
                                        CK_ULONG usLength )
{
  Ecode_t xEcode = ECODE_NVM3_OK;
  uint32_t ulKey = 0;
  uint32_t ulObjType = 0;
  size_t xObjLen = 0;

  /* usLength not used here */
  (void) usLength;

  /* get a NVM3 key for pxLabel */
  ulKey = prvLabelToKey((char *) pxLabel);

  /* return a handle of 0 if we couldn't increment the counter */
  if (ulKey == NVM3_INVALID_KEY) {
    return 0;
  }

  /* Get infromation about the data object */
  xEcode = nvm3_getObjectInfo(nvm3_defaultHandle, ulKey, &ulObjType, &xObjLen);

  /* NVM3 failed to get object? */
  if (xEcode != ECODE_NVM3_OK) {
    return 0;
  }

  /* return the key */
  return ulKey;
}

/**
* @brief Gets the value of an object in storage, by handle.
*
* Port-specific file access for cryptographic information.
*
* This call dynamically allocates the buffer which object value
* data is copied into.  PKCS11_PAL_GetObjectValueCleanup()
* should be called after each use to free the dynamically allocated
* buffer.
*
* @sa PKCS11_PAL_GetObjectValue
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
CK_RV PKCS11_PAL_GetObjectValue( CK_OBJECT_HANDLE xHandle,
                                      CK_BYTE_PTR * ppucData,
                                      CK_ULONG_PTR pulDataSize,
                                      CK_BBOOL * pIsPrivate )
{
  Ecode_t xEcode = ECODE_NVM3_OK;
  uint32_t ulKey = 0;
  uint32_t ulObjType = 0;
  size_t xObjLen = 0;

  /* ulKey is the handle */
  ulKey = xHandle;

  /* Get infromation about the data object */
  xEcode = nvm3_getObjectInfo(nvm3_defaultHandle, ulKey, &ulObjType, &xObjLen);

  /* NVM3 failed to get object? */
  if (xEcode != ECODE_NVM3_OK) {
    return CKR_KEY_HANDLE_INVALID;
  }

  /* Allocate buffer to read the data in */
  *ppucData = pvPortMalloc(xObjLen);
  *pulDataSize = xObjLen;
  *pIsPrivate = ulKey == NVM3_PRIVATE_KEY ? CK_TRUE : CK_FALSE;

  /* Read data */
  xEcode = nvm3_readData(nvm3_defaultHandle, ulKey, *ppucData, *pulDataSize);

  /* NVM3 failed to read object? */
  if (xEcode != ECODE_NVM3_OK) {
    return CKR_FUNCTION_FAILED;
  }

  /* done */
  return CKR_OK;
}


/**
* @brief Cleanup after PKCS11_GetObjectValue().
*
* @param[in] pucData       The buffer to free.
*                          (*ppucData from PKCS11_PAL_GetObjectValue())
* @param[in] ulDataSize    The length of the buffer to free.
*                          (*pulDataSize from PKCS11_PAL_GetObjectValue())
*/
void PKCS11_PAL_GetObjectValueCleanup( CK_BYTE_PTR pucData,
                                       CK_ULONG ulDataSize )
{
  /* just free up the buffer */
  (void) ulDataSize;
  vPortFree(pucData);
}

/**
 * @brief Delete an object from NVM.
 *
 * @param[in] xHandle       Handle to a PKCS #11 object.
 */
CK_RV PKCS11_PAL_DestroyObject( CK_OBJECT_HANDLE xHandle )
{
  Ecode_t xEcode = ECODE_NVM3_OK;
  uint32_t ulKey = 0;

  /* ulKey is the handle */
  ulKey = xHandle;

  /* delete the object */
  xEcode = nvm3_deleteObject(nvm3_defaultHandle, ulKey);

  /* failed to delete label object? */
  if (xEcode != ECODE_NVM3_OK) {
    return CKR_KEY_HANDLE_INVALID;
  }

  /* done */
  return CKR_OK;
}
