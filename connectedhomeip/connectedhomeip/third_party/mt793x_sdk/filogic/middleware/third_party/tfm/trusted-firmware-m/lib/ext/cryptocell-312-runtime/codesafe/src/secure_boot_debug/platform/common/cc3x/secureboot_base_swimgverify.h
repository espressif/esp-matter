/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */



#ifndef _SECURE_BOOT_BASE_SWIMGVERIFY_H
#define _SECURE_BOOT_BASE_SWIMGVERIFY_H


#ifdef __cplusplus
extern "C"
{
#endif


#include "secureboot_defs.h"
#include "secureboot_stage_defs.h"

/*----------------------------
      PUBLIC FUNCTIONS
-----------------------------------*/

/**
 * @brief This function load the SW component to RAM, calculates HASH on it and compares the
 *        result with the given HASH (taken from the certificate).
 *        This function calculates the HASH simultaneously to reading data from the Flash.
 *
 *
 * @param[in] preHashflashRead_func - User's Flash read function used to read data from the flash to memory location.
 *        this is the first function used (before the hash)
 * @param[in] preHashUserContext - User's context for the usage of preHashflashRead_func
 * @param[in] hwBaseAddress - base address for the ARM TrustZone CryptoCell HW engines
 * @param[in] isLoadFromFlash - should image be copied from flash with user callback
 * @param[in] isVerifyImage - should image be verified with hash (and Aes if needed)
 * @param[in] cryptoMode - crypto mode type: 0 = AES to Hash; 1 = AES and Hash
 * @param[in] keyType - code encryption type definition
 * @param[in] AESIv - AES IV buffer
 * @param[in] pSwRecSignedData - a pointer to the s/w record signed data: hash, load address, max image size, code encode flag
 * @param[in] pSwRecNoneSignedData - a pointer to the s/w record non-signed data: storage address, actual image size
 * @param[in] workspace_ptr - temporary buffer to load the SW components to (SW components without
 *            loading address).
 * @param[in] workspaceSize - the temporary buffer size in bytes, minimal allowed size is
 *            CC_DOUBLE_BUFFER_MAX_SIZE_IN_BYTES
 *
 * @return CCError_t - On success the value CC_OK is returned,
 *         on failure - a value from BootImagesVerifier_error.h
 */

CCError_t CCSbImageLoadAndVerify(CCSbFlashReadFunc preHashflashRead_func,
                       void *preHashUserContext,
                       unsigned long hwBaseAddress,
                       uint8_t isLoadFromFlash,
                       uint8_t isVerifyImage,
                       bsvCryptoMode_t cryptoMode,
                       CCBsvKeyType_t  keyType,
                       AES_Iv_t AESIv,
                       uint8_t *pSwRecSignedData,
                       uint32_t *pSwRecNoneSignedData,
                       uint32_t *workspace_ptr,
                       uint32_t workspaceSize);

/*!
 * @brief verify NV counter extension against OTP
 *
 * @param[in] hwBaseAddress - hw registers base address
 * @param[in] pCertNvCounter    - NV counter fields read from certificate
 * @param[in] certPkgInfo   - certPkgInfo - certificate data structure
 *
 * @return uint32_t         - On success: the value CC_OK is returned,
 *                    On failure: a value from bsv_error.h
 */
CCError_t CCSbVerifyNvCounter(unsigned long hwBaseAddress, uint32_t swVersion, CCSbCertInfo_t *certPkgInfo);

/*!
 * @brief Set NV counter to the OTP (if needed)
 *
 * @param[in] hwBaseAddress - hw registers base address
 * @param[in] certPkgInfo   - certificate data structure
 *
 * @return uint32_t         - On success: the value CC_OK is returned,
 *                    On failure: a value from bsv_error.h
 */
CCError_t CCSbSetNvCounter(unsigned long hwBaseAddress, CCSbCertInfo_t *certPkgInfo);


#ifdef __cplusplus
}
#endif

#endif


