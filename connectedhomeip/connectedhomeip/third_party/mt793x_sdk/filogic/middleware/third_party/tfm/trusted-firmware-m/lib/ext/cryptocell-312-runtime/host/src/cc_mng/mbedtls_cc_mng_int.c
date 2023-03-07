/*
 * Copyright (c) 2001-2020, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


/************* Include Files ****************/
#include "mbedtls_cc_mng.h"
#include "mbedtls_cc_mng_error.h"
#include "mbedtls_cc_mng_int.h"
#include "cc_pal_types.h"
#include "cc_pal_mem.h"
#include "cc_otp_defs.h"
#include "dx_id_registers.h"
#include "dx_crys_kernel.h"
#include "driver_defs.h"
#include "cc_pal_abort.h"
#include "cc_pal_mutex.h"
#include "cc_util_pm.h"

/************* Auxiliary API's *************/
/* All Musca-S1 platform-dependent defines (DX_PLAT_MUSCA_S1) are due to the
 * fact that the S1 board's OTP is just an ordinary register which is volatile.
 * The MRAM is used instead, and this is what the changes reflect.
 */
int mbedtls_mng_otpWordRead(uint32_t otpAddress, uint32_t *pOtpWord)
{
    uint32_t regVal=0;

    /* check input variables */
    if (pOtpWord == NULL) {
        return CC_MNG_ILLEGAL_INPUT_PARAM_ERR;
    }

    /* check otp limits */
    CC_GET_OTP_LENGTH(regVal);
    if (otpAddress >= regVal) {
        return CC_MNG_ILLEGAL_INPUT_PARAM_ERR;
    }

    /* read OTP word */
#ifdef DX_PLAT_MUSCA_S1
    CC_READ_MRAM_WORD(otpAddress*sizeof(uint32_t), regVal);
#else
    CC_READ_OTP_WORD(otpAddress*sizeof(uint32_t), regVal);
#endif
    *pOtpWord = regVal;

    return CC_OK;
}

int mbedtls_mng_lcsGet(uint32_t *pLcs)
{
#if defined(DX_PLAT_MUSCA_S1) && defined(CRYPTO_HW_ACCELERATOR_OTP_ENABLED)
    uint32_t manufacturer_flag;
    uint32_t oem_flag;

    /* Read the MRAM (OTP) contents the CC312 uses to determine the lcs (for
     * more information see Arm CryptoCell-312 Software Integrators Manual
     * (r1p3), pgs. 3-36 - 3-38)
     */
    CC_READ_MRAM_WORD(CC_OTP_MANUFACTURE_FLAG_OFFSET * sizeof(uint32_t),
                      manufacturer_flag);

    CC_READ_MRAM_WORD(CC_OTP_OEM_FLAG_OFFSET * sizeof(uint32_t), oem_flag);

    if((manufacturer_flag == 0) && (oem_flag == 0))
        *pLcs = CC_MNG_LCS_CM;
    else if((manufacturer_flag != 0) && (oem_flag & 0xFF) == 0)
        *pLcs = CC_MNG_LCS_DM;
    else if((manufacturer_flag != 0) && (oem_flag & 0xFF) != 0)
        *pLcs = CC_MNG_LCS_SEC_ENABLED;

    return CC_OK;
#else
    uint32_t regVal = 0;

    /* check input variables */
    if (pLcs == NULL) {
        return CC_MNG_ILLEGAL_INPUT_PARAM_ERR;
    }

    /* poll NVM register to assure that the NVM boot is finished (and LCS and the keys are valid) */
    CC_WAIT_ON_NVM_IDLE_BIT();

    /* read LCS register */
    regVal = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, LCS_REG));

    regVal = CC_REG_FLD_GET(0, LCS_REG, LCS_REG, regVal);

    /* return the LCS value */
    *pLcs = regVal;
    return CC_OK;
#endif /* DX_PLAT_MUSCA_S1 && CRYPTO_HW_ACCELERATOR_OTP_ENABLED */
}

int mbedtls_mng_swVersionGet(mbedtls_mng_pubKeyType_t keyIndex, uint32_t *swVersion)
{
    uint32_t i, regVal = 0, tmpVal = 0;
    uint32_t versionBitCount = 0;
    uint32_t cntrWidth=0, cntrOffset=0, cntrSwVersion;
    CCError_t error = CC_OK;
    uint32_t lcs = 0, isHbkFull = 0;
    CCBool_t isNextWordZero = CC_FALSE;

    /* check swVersion pointrer */
    if (swVersion == NULL) {
        return CC_MNG_ILLEGAL_INPUT_PARAM_ERR;
    }

    /* clear version in case of error */
    *swVersion = 0;

    /* get lifecycle */
    error = mbedtls_mng_lcsGet(&lcs);
    if (error != CC_OK) {
        return error;
    }

    /* in case of RMA, return constant 0 */
    if (lcs == CC_MNG_LCS_RMA){
        return CC_OK;
    }

    /* get HBK configuration */
    CC_IS_HBK_FULL(isHbkFull, error);
    if (error != CC_OK)
        return error;

    /* check HBK configuration (in case of CM, get any counter needed) */
    switch(keyIndex){
    case CC_MNG_HASH_BOOT_KEY_256B:

        /* verify correct HBK configuration for DM & SE */
        if ((lcs != CC_MNG_LCS_CM) && (isHbkFull != CC_TRUE)) {
            return CC_MNG_ILLEGAL_OPERATION_ERR;
        }

        cntrWidth = CC_OTP_HBK_MIN_VERSION_SIZE_IN_WORDS;
        cntrOffset = CC_OTP_HBK_MIN_VERSION_OFFSET;
        break;

    case CC_MNG_HASH_BOOT_KEY_0_128B:

        /* verify correct HBK configuration for DM & SE */
        if ((lcs != CC_MNG_LCS_CM) && (isHbkFull == CC_TRUE)) {
            return CC_MNG_ILLEGAL_OPERATION_ERR;
        }

        cntrWidth = CC_OTP_HBK0_MIN_VERSION_SIZE_IN_WORDS;
        cntrOffset = CC_OTP_HBK0_MIN_VERSION_OFFSET;
        break;

    case CC_MNG_HASH_BOOT_KEY_1_128B:

        /* verify correct HBK configuration for DM & SE */
        if ((lcs != CC_MNG_LCS_CM) && (isHbkFull == CC_TRUE)) {
            return CC_MNG_ILLEGAL_OPERATION_ERR;
        }

        cntrWidth = CC_OTP_HBK1_MIN_VERSION_SIZE_IN_WORDS;
        cntrOffset = CC_OTP_HBK1_MIN_VERSION_OFFSET;
        break;
    default:
        return CC_MNG_ILLEGAL_INPUT_PARAM_ERR;
    }

    /* read the SW version from the OTP, and accumulate number of ones */
    cntrSwVersion = 0;
    for(i=0; i < cntrWidth; i++) {
        error = mbedtls_mng_otpWordRead(cntrOffset+i, &regVal);
        if (error != CC_OK)
            return error;

        CC_MNG_COUNT_ONE_BITS(regVal, versionBitCount);

        /* verify legality of 1's bits */
        if((isNextWordZero == CC_TRUE) && (regVal!=0)){
            return CC_MNG_ILLEGAL_SW_VERSION_ERR;
        }
        if(versionBitCount < CC_MNG_ALL_ONES_NUM_BITS){
            isNextWordZero = CC_TRUE;
            }

        /* convert versionBitCount to base-1 representation and compare to OTP word */
        if(versionBitCount != 0){
            tmpVal = CC_MNG_ALL_ONES_VALUE >> (CC_MNG_ALL_ONES_NUM_BITS - versionBitCount);
        } else{
            tmpVal = 0;
        }
        if (tmpVal != regVal) {
            /* return error in case of invalid base-1 value */
            return CC_MNG_ILLEGAL_SW_VERSION_ERR;
        }

        cntrSwVersion += versionBitCount;
    }

    *swVersion = cntrSwVersion;

    return CC_OK;
}

int mbedtls_mng_pubKeyHashGet(mbedtls_mng_pubKeyType_t keyIndex, uint32_t *hashedPubKey, uint32_t hashResultSizeWords)
{
    uint32_t i, address;
    uint32_t regVal = 0, cntZero = 0, zerosHash = 0;
    uint32_t oemFlag = 0, icvFlag = 0;
    CCError_t  error = CC_OK;
    uint32_t lcs = 0, isHbkFull = 0;

    /* check hash buffer pointer */
    if (hashedPubKey == NULL) {
        return CC_MNG_ILLEGAL_INPUT_PARAM_ERR;
    }

    /* get lifecycle */
    error = mbedtls_mng_lcsGet(&lcs);
    if (error != CC_OK) {
        return error;
    }

    /* in case of CM, return error */
    if (lcs == CC_MNG_LCS_CM) {
        return CC_MNG_HASH_NOT_PROGRAMMED_ERR;
    }

    /* get HBK configuration */
    CC_IS_HBK_FULL(isHbkFull, error);
    if (error != CC_OK) {
        return error;
    }

    /* read icv flags word */
    error = mbedtls_mng_otpWordRead(CC_OTP_MANUFACTURE_FLAG_OFFSET, &icvFlag);
    if (error != CC_OK) {
        return error;
    }

    /* read OEM programmer flags word */
    error = mbedtls_mng_otpWordRead(CC_OTP_OEM_FLAG_OFFSET, &oemFlag);
    if (error != CC_OK)
        return error;

    /* verify validity of key index, key size and mode of operation */
    switch (keyIndex) {
    case CC_MNG_HASH_BOOT_KEY_256B:
        /* key size should hold 256b */
        if (hashResultSizeWords != CC_MNG_256B_HASH_SIZE_IN_WORDS)
            return CC_MNG_ILLEGAL_INPUT_PARAM_ERR;
        /* otp shuld support full HBK */
        if (isHbkFull != CC_TRUE) {
            return CC_MNG_ILLEGAL_OPERATION_ERR;
        }
        /* Hbk0 zero count should be cleared */
        if (CC_REG_FLD_GET2(0, OTP_MANUFACTURE_FLAG, HBK0_ZERO_BITS, icvFlag) != 0) {
            return CC_MNG_ILLEGAL_OPERATION_ERR;
        }

        /* DM lcs is illegal for Hbk */
        if (lcs == CC_MNG_LCS_DM) {
            return CC_MNG_HASH_NOT_PROGRAMMED_ERR;
        }

        zerosHash = CC_REG_FLD_GET2(0, OTP_OEM_FLAG, HBK_ZERO_BITS, oemFlag);
        address = CC_OTP_HBK_OFFSET;
        break;

    case CC_MNG_HASH_BOOT_KEY_0_128B:
        /* key size should hold 128b */
        if (hashResultSizeWords != CC_MNG_128B_HASH_SIZE_IN_WORDS)
            return CC_MNG_ILLEGAL_INPUT_PARAM_ERR;
        /* otp shuld support 2 HBK's */
        if (isHbkFull == CC_TRUE) {
            return CC_MNG_ILLEGAL_OPERATION_ERR;
        }

        zerosHash = CC_REG_FLD_GET2(0, OTP_MANUFACTURE_FLAG, HBK0_ZERO_BITS, icvFlag);
        address = CC_OTP_HBK0_OFFSET;
        break;

    case CC_MNG_HASH_BOOT_KEY_1_128B:
        /* key size should hold 128b */
        if (hashResultSizeWords != CC_MNG_128B_HASH_SIZE_IN_WORDS)
            return CC_MNG_ILLEGAL_INPUT_PARAM_ERR;
        /* otp shuld support 2 HBK's */
        if (isHbkFull == CC_TRUE) {
            return CC_MNG_ILLEGAL_OPERATION_ERR;
        }
        /* DM lcs is illegal for Hbk1 */
        if (lcs == CC_MNG_LCS_DM)
            return CC_MNG_HASH_NOT_PROGRAMMED_ERR;

        zerosHash = CC_REG_FLD_GET2(0, OTP_OEM_FLAG, HBK1_ZERO_BITS, oemFlag);
        address = CC_OTP_HBK1_OFFSET;
        break;

    default:
        return CC_MNG_ILLEGAL_INPUT_PARAM_ERR;
    }

    /* read hash key from OTP */
    for (i = 0; i < hashResultSizeWords; i++) {
        error = mbedtls_mng_otpWordRead(address + i, &regVal);
        if (error != CC_OK) {
            goto _Err_GetPubKey;
        }
        *(hashedPubKey + i) = regVal;

        /* accumulate number of zeroes */
        CC_MNG_COUNT_ZEROES(regVal, cntZero);
    }

    /* verify number of "0" bits in the hash key */
    if (zerosHash == cntZero) {
        return CC_OK;
    } else {
        error = CC_MNG_HBK_ZERO_COUNT_ERR;
    }

    /* case of error, clean hash buffer */
_Err_GetPubKey:
    for (i = 0; i < hashResultSizeWords; i++) {
        *(hashedPubKey + i) = 0;
    }

    return error;
}






