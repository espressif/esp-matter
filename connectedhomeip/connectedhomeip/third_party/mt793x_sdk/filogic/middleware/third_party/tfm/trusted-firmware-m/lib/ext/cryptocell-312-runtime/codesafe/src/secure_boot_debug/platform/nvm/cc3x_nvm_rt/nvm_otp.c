/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


/************* Include Files ****************/

#include "secureboot_basetypes.h"
#include "secureboot_error.h"
#include "nvm_otp.h"
#include "secureboot_general_hwdefs.h"
#include "rsa_bsv.h"
#include "bootimagesverifier_error.h"
#include "mbedtls_cc_mng_error.h"

#include "bsv_defs.h"
#include "bsv_error.h"

#include "secureboot_stage_defs.h"


/************************ Defines ******************************/

/************************ Enums ******************************/

/************************ Typedefs ******************************/

/************************ Global Data ******************************/

/************************ Public functions ******************************/

/************************ Private functions ******************************/


/**
 * @brief This function reads the LCS from the SRAM/NVM
 *
 * @param[in] hwBaseAddress  -  cryptocell base address
 *
 * @param[in/out] lcs_ptr  - pointer to memory to store the LCS
 *
 * @return CCError_t - On success the value CC_OK is returned, and on failure   -a value from NVM_error.h
 */
CCError_t NVM_GetLCS(unsigned long hwBaseAddress, uint32_t *lcs_ptr)
{
  CCError_t error = CC_OK;

  /* Get LCS from register */
  error = CC_BsvLcsGet(hwBaseAddress, lcs_ptr);

  return error;
}


/**
 * @brief The NVM_ReadHASHPubKey function is a NVM interface function -
 *        The function retrieves the HASH of the device Public key from the SRAM/NVM
 *
 *
 * @param[in] hwBaseAddress -  cryptocell base address
 *
 * @param[in] keyIndex -  Index of HASH in the OTP
 *
 * @param[out] PubKeyHASH   -  the public key HASH.
 *
 * @param[in] hashSizeInWords -  hash size (valid values: 4W, 8W)
 *
 * @return CCError_t - On success the value CC_OK is returned, and on failure   -a value from NVM_error.h
 */
CCError_t NVM_ReadHASHPubKey(unsigned long hwBaseAddress, CCSbPubKeyIndexType_t keyIndex, CCHashResult_t PubKeyHASH, uint32_t hashSizeInWords)
{
    CCError_t error = CC_OK;
    uint32_t i;
    uint32_t lcs;

    /* Check input variables */
    if (PubKeyHASH == NULL)
        return CC_BOOT_IMG_VERIFIER_INV_INPUT_PARAM;

    /* Get LCS from register */
    error = CC_BsvLcsGet(hwBaseAddress, &lcs);
    if (error != CC_OK) {
        return error;
    }

    if ( (lcs == CC_BSV_CHIP_MANUFACTURE_LCS) ||
         (lcs == CC_BSV_RMA_LCS) ){
        return CC_BOOT_IMG_VERIFIER_SKIP_PUBLIC_KEY_VERIFY;
    }

    error = CC_BsvPubKeyHashGet(hwBaseAddress, keyIndex, PubKeyHASH, hashSizeInWords);
    /* Secure Boot should skip verification of the Certificate key against OTP memory when public key hash is not programmed yet (in CM or DM). */
    if (error == CC_MNG_HASH_NOT_PROGRAMMED_ERR){
        return CC_BOOT_IMG_VERIFIER_SKIP_PUBLIC_KEY_VERIFY;
    }

    if (error == CC_OK){
        /* All key and digest fields are stored in OTP in little-endian format */
        for (i=0; i < hashSizeInWords; i++) {
            PubKeyHASH[i] = UTIL_REVERT_UINT32_BYTES( PubKeyHASH[i] );
        }
    }

    return error;
}


/**
 * @brief The NVM_GetSwVersion function is a NVM interface function -
 *        The function retrieves the SW version from the SRAM/NVM.
 *        In case of OTP, we support up to 16 anti-rollback counters (taken from the certificate)
 *
 * @param[in] hwBaseAddress -  cryptocell base address
 *
 * @param[in] keyIndex -  relevant only for OTP (valid values: 1,2)
 *
 * @param[out] swVersion   -  the minimum SW version
 *
 * @return CCError_t - On success the value CC_OK is returned, and on failure   -a value from NVM_error.h
 */
CCError_t NVM_GetSwVersion(unsigned long hwBaseAddress, CCSbPubKeyIndexType_t keyIndex, uint32_t* swVersion)
{
    uint32_t swVersionNum = 0;
    CCError_t error = CC_OK;

    /* Check input variables */
    if (swVersion == NULL)
        return CC_BOOT_IMG_VERIFIER_INV_INPUT_PARAM;

    /* get FW minimum version according to counter ID */
    error = CC_BsvSwVersionGet(hwBaseAddress, keyIndex, &swVersionNum);
    if (error != CC_OK) {
        return error;
    }

    *swVersion = swVersionNum;
    return CC_OK;
}

CCError_t NVM_SetSwVersion(unsigned long hwBaseAddress, CCSbPubKeyIndexType_t keyIndex, uint32_t swVersion)
{
    CC_UNUSED_PARAM(hwBaseAddress);
    CC_UNUSED_PARAM(keyIndex);
    CC_UNUSED_PARAM(swVersion);

    return CC_OK;
}
