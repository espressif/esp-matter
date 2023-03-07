/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _NVM_OTP_H
#define _NVM_OTP_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "cc_crypto_boot_defs.h"

/*------------------------------------
    DEFINES
-------------------------------------*/

/**
 * @brief The NVM_GetSwVersion function is a NVM interface function -
 *        The function retrieves the SW version from the SRAM/NVM.
 *    In case of OTP, we support up to 16 anti-rollback counters (taken from the certificate)
 *
 * @param[in] hwBaseAddress -  CryptoCell base address
 *
 * @param[in] counterId -  relevant only for OTP (valid values: 1,2)
 *
 * @param[out] swVersion   -  the minimum SW version
 *
 * @return CCError_t - On success the value CC_OK is returned, and on failure   -a value from NVM_error.h
 */
CCError_t NVM_GetSwVersion(unsigned long hwBaseAddress, CCSbPubKeyIndexType_t keyIndex, uint32_t* swVersion);


/**
 * @brief The run time secure boot should not support SW version update.
 *
 * @param[in] hwBaseAddress -  CryptoCell base address
 *
 * @param[in] counterId -  relevant only for OTP (valid values: 1,2)
 *
 * @param[out] swVersion   -  the minimum SW version
 *
 * @return CCError_t - always return CC_OK
 */

CCError_t NVM_SetSwVersion(unsigned long hwBaseAddress, CCSbPubKeyIndexType_t keyIndex, uint32_t swVersion);

#ifdef __cplusplus
}
#endif

#endif


