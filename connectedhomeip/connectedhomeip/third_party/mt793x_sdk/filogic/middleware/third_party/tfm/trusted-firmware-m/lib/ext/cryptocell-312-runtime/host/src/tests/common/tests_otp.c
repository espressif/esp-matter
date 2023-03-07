/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "tests_otp.h"
#include <stdio.h>
#include <string.h>

#include "cc_pal_types.h"
#include "cc_regs.h"
#include "cc_otp_defs.h"

#include "board_configs.h"
#include "test_pal_log.h"
#include "test_pal_time.h"

#include "hw_access.h"

uint32_t Test_SetLcsOtpBuff(uint32_t *otpBuf, uint32_t lcsState)
{
    //if RMA => set ICV_RMA and OEM_RMA bits
    //if CM => man_flag[31:0]=0 && oem_flag[7:0] = 0
    //if DM => oem_flag[7:0] = 0
    // otherwise SE

    uint32_t hwWord = 0;

    switch (lcsState) {
        case TESTS_LCS_RMA:
            hwWord = otpBuf[CC_OTP_OEM_FLAG_OFFSET];
            CC_REG_FLD_SET2(0, OTP_OEM_FLAG, ICV_RMA_MODE, hwWord, 1);
            CC_REG_FLD_SET2(0, OTP_OEM_FLAG, OEM_RMA_MODE, hwWord, 1);
            otpBuf[CC_OTP_OEM_FLAG_OFFSET] = hwWord;
            break;
        case TESTS_LCS_CM:
            otpBuf[CC_OTP_MANUFACTURE_FLAG_OFFSET] = 0;
            /* fall-through */
            // to clean oem_flag[7:0] bits as for DM
        case TESTS_LCS_DM:
            hwWord = otpBuf[CC_OTP_OEM_FLAG_OFFSET];
            CC_REG_FLD_SET2(0, OTP_OEM_FLAG, HBK_ZERO_BITS, hwWord, 0);
            otpBuf[CC_OTP_OEM_FLAG_OFFSET] = hwWord;
            break;

        case TESTS_LCS_SEC_DISABLED:
        case TESTS_LCS_SEC_ENABLED:
            break;
        default:
            TEST_PRINTF_ERROR("ERROR: ilegal lcs params\n");
            return TEST_INVALID_PARAM_ERR;
    }
    return TEST_OK;
}


uint32_t Test_SetNotInUseOtpBuff(uint32_t *otpBuf, uint32_t keyType, uint32_t value)
{
    uint32_t hwWord;

    switch (keyType){
        // TBD HBK
        case TEST_KCP_KEY:
            hwWord = otpBuf[CC_OTP_OEM_FLAG_OFFSET];
            CC_REG_FLD_SET2(0, OTP_OEM_FLAG, KCP_NOT_IN_USE, hwWord, value);
            otpBuf[CC_OTP_OEM_FLAG_OFFSET] = hwWord;
            break;
        case TEST_KCE_KEY:
            hwWord = otpBuf[CC_OTP_OEM_FLAG_OFFSET];
            CC_REG_FLD_SET2(0, OTP_OEM_FLAG, KCE_NOT_IN_USE, hwWord, value);
            otpBuf[CC_OTP_OEM_FLAG_OFFSET] = hwWord;
            break;
        case TEST_KPICV_KEY:
            hwWord = otpBuf[CC_OTP_MANUFACTURE_FLAG_OFFSET];
            CC_REG_FLD_SET2(0, OTP_MANUFACTURE_FLAG, KPICV_NOT_IN_USE, hwWord, value);
            otpBuf[CC_OTP_MANUFACTURE_FLAG_OFFSET] = hwWord;
            break;
        case TEST_KCEICV_KEY:
            hwWord = otpBuf[CC_OTP_MANUFACTURE_FLAG_OFFSET];
            CC_REG_FLD_SET2(0, OTP_MANUFACTURE_FLAG, KCEICV_NOT_IN_USE, hwWord, value);
            otpBuf[CC_OTP_MANUFACTURE_FLAG_OFFSET] = hwWord;
            break;
        case TEST_HUK_KEY:
        default:
            TEST_PRINTF_ERROR("ERROR: can't set notInUse bit for this type of key\n");
            return TEST_INVALID_PARAM_ERR;

    }
    return TEST_OK;
}

uint32_t Test_SetZeroBitsOtpBuff(uint32_t *otpBuf, uint32_t keyType, uint32_t value)
{
    uint32_t hwWord;

    // TBD HBK??
    switch (keyType){
        case TEST_KCP_KEY:
            hwWord = otpBuf[CC_OTP_OEM_FLAG_OFFSET];
            CC_REG_FLD_SET2(0, OTP_OEM_FLAG, KCP_ZERO_BITS, hwWord, value);
            otpBuf[CC_OTP_OEM_FLAG_OFFSET] = hwWord;
            break;
        case TEST_KCE_KEY:
            hwWord = otpBuf[CC_OTP_OEM_FLAG_OFFSET];
            CC_REG_FLD_SET2(0, OTP_OEM_FLAG, KCE_ZERO_BITS, hwWord, value);
            otpBuf[CC_OTP_OEM_FLAG_OFFSET] = hwWord;
            break;
        case TEST_KPICV_KEY:
            hwWord = otpBuf[CC_OTP_MANUFACTURE_FLAG_OFFSET];
            CC_REG_FLD_SET2(0, OTP_MANUFACTURE_FLAG, KPICV_ZERO_BITS, hwWord, value);
            otpBuf[CC_OTP_MANUFACTURE_FLAG_OFFSET] = hwWord;
            break;
        case TEST_KCEICV_KEY:
            hwWord = otpBuf[CC_OTP_MANUFACTURE_FLAG_OFFSET];
            CC_REG_FLD_SET2(0, OTP_MANUFACTURE_FLAG, KCEICV_ZERO_BITS, hwWord, value);
            otpBuf[CC_OTP_MANUFACTURE_FLAG_OFFSET] = hwWord;
            break;
        case TEST_HUK_KEY:
            hwWord = otpBuf[CC_OTP_MANUFACTURE_FLAG_OFFSET];
            CC_REG_FLD_SET2(0, OTP_MANUFACTURE_FLAG, HUK_ZERO_BITS, hwWord, value);
            otpBuf[CC_OTP_MANUFACTURE_FLAG_OFFSET] = hwWord;
            break;
        default:
            TEST_PRINTF_ERROR("ERROR: can't set notInUse bit for this type of key\n");
            return TEST_INVALID_PARAM_ERR;

    }
    return TEST_OK;
}

uint32_t Test_SetKeyOtpBuff(uint32_t *otpBuf, uint32_t *keyBuff, uint32_t keyType)
{
    uint32_t zeroCount = 0;
    uint32_t keySizeInWords = 0;
    uint32_t keyOffsetInOtp = 0;
    uint32_t rc = TEST_OK;

    if ((NULL == otpBuf) || (NULL == keyBuff))
    {
        TEST_PRINTF_ERROR("ilegal params\n");
        return TEST_INVALID_PARAM_ERR;
    }

    // TBD HBK??
    switch (keyType)
    {
        case TEST_HUK_KEY:
            keySizeInWords = CC_OTP_HUK_SIZE_IN_WORDS;
            keyOffsetInOtp = CC_OTP_HUK_OFFSET;
            break;
        case TEST_KCP_KEY:
            keySizeInWords = CC_OTP_KCP_SIZE_IN_WORDS;
            keyOffsetInOtp = CC_OTP_KCP_OFFSET;
            break;
        case TEST_KCE_KEY:
            keySizeInWords = CC_OTP_KCE_SIZE_IN_WORDS;
            keyOffsetInOtp = CC_OTP_KCE_OFFSET;
            break;
        case TEST_KPICV_KEY:
            keySizeInWords = CC_OTP_KPICV_SIZE_IN_WORDS;
            keyOffsetInOtp = CC_OTP_KPICV_OFFSET;
            break;
        case TEST_KCEICV_KEY:
            keySizeInWords = CC_OTP_KCEICV_SIZE_IN_WORDS;
            keyOffsetInOtp = CC_OTP_KCEICV_OFFSET;
            break;
        default:
            TEST_PRINTF_ERROR("ERROR: key type is not supported\n")
            ;
            return TEST_INVALID_PARAM_ERR;

    }

    memcpy((uint8_t *) (otpBuf + keyOffsetInOtp), (uint8_t *) keyBuff, keySizeInWords * sizeof(uint32_t));

    TEST_CALC_BUFF_ZEROS(keyBuff, keySizeInWords, zeroCount);

    if (keyType != TEST_HUK_KEY)
    {
        rc = Test_SetNotInUseOtpBuff(otpBuf, keyType, 0);
    }

    rc = rc & Test_SetZeroBitsOtpBuff(otpBuf, keyType, zeroCount);

    return rc;
}

/**
 * This function gets hw key and its size from otp buffer to user buffer
 *
 */
uint32_t Test_GetKeyOtpBuff(uint32_t *otpBuf, uint32_t keyType, uint32_t *keySizeInWords, uint32_t *keyBuff )
{
    uint32_t keyOffsetInOtp = 0;
    uint32_t rc = TEST_OK;

    if ((NULL == otpBuf) || (NULL == keyBuff) || (NULL == keySizeInWords)) {
        TEST_PRINTF_ERROR(" ilegal params\n");
        return TEST_INVALID_PARAM_ERR;
    }

    // TBD HBK??
    switch (keyType){
        case TEST_HUK_KEY:
            *keySizeInWords = CC_OTP_HUK_SIZE_IN_WORDS;
            keyOffsetInOtp = CC_OTP_HUK_OFFSET;
            break;
        case TEST_KCP_KEY:
            *keySizeInWords = CC_OTP_KCP_SIZE_IN_WORDS;
            keyOffsetInOtp = CC_OTP_KCP_OFFSET;
            break;
        case TEST_KCE_KEY:
            *keySizeInWords = CC_OTP_KCE_SIZE_IN_WORDS;
            keyOffsetInOtp = CC_OTP_KCE_OFFSET;
            break;
        case TEST_KPICV_KEY:
            *keySizeInWords = CC_OTP_KPICV_SIZE_IN_WORDS;
            keyOffsetInOtp = CC_OTP_KPICV_OFFSET;
            break;
        case TEST_KCEICV_KEY:
            *keySizeInWords = CC_OTP_KCEICV_SIZE_IN_WORDS;
            keyOffsetInOtp = CC_OTP_KCEICV_OFFSET;
            break;
        default:
            *keySizeInWords = 0;
            TEST_PRINTF_ERROR("ERROR: key type is not supported\n");
            return TEST_INVALID_PARAM_ERR;

    }
    memcpy((uint8_t *)keyBuff, (uint8_t *)(otpBuf + keyOffsetInOtp), (*keySizeInWords)*sizeof(uint32_t));

    return rc;
}


uint32_t Test_SetHbkInOtpBuff(uint32_t *otp, uint8_t *hbkBuff, OtpHbkTypes_t type)
{
    uint32_t  i = 0;
    uint32_t  zeroCount = 0;
    int otpStartOffset = (type == TEST_OTP_HBK1_TYPE)?CC_OTP_HBK1_OFFSET:CC_OTP_HBK0_OFFSET;
    int hbkStartOffset = (type == TEST_OTP_HBK1_TYPE)?CC_OTP_HBK0_SIZE_IN_WORDS:0;
    uint32_t hbkWordSize;


    if ((NULL == otp) || (NULL == hbkBuff))
    {
        TEST_PRINTF_ERROR("testSetHbkInOtpBuff ilegal params\n");
        return TEST_INVALID_PARAM_ERR;
    }

    if ((type == TEST_OTP_HBK0_TYPE) || (type == TEST_OTP_HBK1_TYPE))
    {
        hbkWordSize = CC_OTP_HBK0_SIZE_IN_WORDS;
    }
    else if ((type == TEST_OTP_HBK_256_TYPE) || (type == (TEST_OTP_HBK0_TYPE | TEST_OTP_HBK1_TYPE)))
    {
        hbkWordSize = CC_OTP_HBK0_SIZE_IN_WORDS + CC_OTP_HBK1_SIZE_IN_WORDS;
    }
    else
    {
        TEST_PRINTF_ERROR("Illegal type %d\n", type);
        return TEST_INVALID_PARAM_ERR;
    }

    //clear OEM HBK and its zero count
    memset(&otp[CC_OTP_HBK1_OFFSET], 0, CC_OTP_HBK1_SIZE_IN_WORDS);
    otp[CC_OTP_OEM_FLAG_OFFSET] &= ~(0xFF);

    if (type != TEST_OTP_HBK1_TYPE)
    {
        //clear ICV HBK zero count and clear HBK0 usage
        memset(&otp[CC_OTP_HBK0_OFFSET], 0, CC_OTP_HBK0_SIZE_IN_WORDS);
        otp[CC_OTP_MANUFACTURE_FLAG_OFFSET] &= ~(0xFF000000);

        otp[CC_OTP_MANUFACTURE_FLAG_OFFSET] |= (1U << CC_OTP_MANUFACTURE_FLAG_HBK0_NOT_IN_USE_BIT_SHIFT);
    }

    for (i = 0; i < hbkWordSize; i++)
    {
        TEST_CONVERT_BYTE_ARR_TO_WORD(&hbkBuff[(hbkStartOffset + i) * sizeof(uint32_t)], otp[otpStartOffset + i]);
    }

    if (type & TEST_OTP_HBK0_TYPE)
    {
        TEST_CALC_BUFF_ZEROS(&otp[CC_OTP_HBK0_OFFSET], CC_OTP_HBK0_SIZE_IN_WORDS, zeroCount);
        otp[CC_OTP_MANUFACTURE_FLAG_OFFSET] &= ~(0xFF000000);
        otp[CC_OTP_MANUFACTURE_FLAG_OFFSET] |= (zeroCount) << CC_OTP_MANUFACTURE_FLAG_HBK0_ZERO_BITS_BIT_SHIFT;
    }

    if (type & TEST_OTP_HBK1_TYPE)
    {
        TEST_CALC_BUFF_ZEROS(&otp[CC_OTP_HBK1_OFFSET], CC_OTP_HBK1_SIZE_IN_WORDS, zeroCount);
        otp[CC_OTP_OEM_FLAG_OFFSET] &= ~(0xFF);
        otp[CC_OTP_OEM_FLAG_OFFSET] |= (zeroCount) << CC_OTP_OEM_FLAG_HBK1_ZERO_BITS_BIT_SHIFT;
    }

    if (type == TEST_OTP_HBK_256_TYPE)
    {
        TEST_CALC_BUFF_ZEROS(&otp[CC_OTP_HBK_OFFSET], hbkWordSize, zeroCount);
        otp[CC_OTP_OEM_FLAG_OFFSET] &= ~(0xFF);
        otp[CC_OTP_OEM_FLAG_OFFSET] |= (zeroCount) << CC_OTP_OEM_FLAG_HBK1_ZERO_BITS_BIT_SHIFT;
    }

    return TEST_OK;
}

uint32_t Test_SetSwVerInOtpBuff(uint32_t *otp, uint32_t offset, uint32_t max_size)
{
    uint32_t i;

    for (i=0; i<max_size ; i++)
    {
        if (otp[offset + i] < 0xFFFFFFFF)
        {
            memset(&otp[offset+i+1], 0x0, (max_size-i-1)* sizeof(uint32_t));
            break;
        }
    }

    return TEST_OK;
}


void Test_SetDefaultOtp(uint32_t* otpValues, uint32_t otpSize, uint32_t lcsState)
{

    uint32_t huk[]    = {0xCAFE1111, 0xCAFE1222, 0xCAFE1333, 0xCAFE1444, 0xCAFE1555, 0xCAFE1666, 0xCAFE1777, 0xCAFE1888};
    uint32_t kpicv[]  = {0xCAFE2111, 0xCAFE2222, 0xCAFE2333, 0xCAFE2444};
    uint32_t kceicv[] = {0xCAFE3111, 0xCAFE3222, 0xCAFE3333, 0xCAFE3444};
    uint32_t kcp[]    = {0xCAFE4111, 0xCAFE4222, 0xCAFE4333, 0xCAFE4444};
    uint32_t kce[]    = {0xCAFE5111, 0xCAFE5222, 0xCAFE5333, 0xCAFE5444};
    uint32_t hbk[]    = {0xCAFE6111, 0xCAFE6222, 0xCAFE6333, 0xCAFE6444, 0xCAFE6555, 0xCAFE6666, 0xCAFE6777, 0xCAFE6888};

    memset(otpValues, 0, otpSize);

    Test_SetHbkInOtpBuff     (otpValues, (unsigned char*)hbk, TEST_OTP_HBK_256_TYPE);

    Test_SetKeyOtpBuff   (otpValues, huk, TEST_HUK_KEY);

    Test_SetKeyOtpBuff   (otpValues, kcp, TEST_KCP_KEY);

    Test_SetKeyOtpBuff   (otpValues, kce, TEST_KCE_KEY);

    Test_SetKeyOtpBuff   (otpValues, kceicv, TEST_KCEICV_KEY);

    Test_SetKeyOtpBuff   (otpValues, kpicv, TEST_KPICV_KEY);

    Test_SetLcsOtpBuff       (otpValues, lcsState);
}


uint32_t Test_BurnOtp(uint32_t  *otpBuf, uint32_t  nextLcs)
{

    CCError_t error = 0;
    uint32_t i = 0;

    /* Clean OTP */
    for (i = 0; i < TEST_OTP_SIZE_IN_WORDS; i++) {
        WRITE_OTP(i, 0);
        Test_PalDelay(1000);
    }

    /* Perform SW reset to reach CM LCS */
    Test_HalPerformPowerOnReset();
    Test_PalDelay(1000);

    /* Copy new OTP buffer */
    for (i = 0; i < TEST_OTP_SIZE_IN_WORDS; i++) {
        //                TEST_PRINTF("writing Otp [0x%X] 0x%X\n", i, otpBuf[i]);
        WRITE_OTP(i, otpBuf[i]);
        Test_PalDelay(1000);
    }

    /*  Perform SW reset after writing to OTP new values */
    Test_HalPerformPowerOnReset();

    /* verify LCS */
    error = Test_HalCheckLcs(nextLcs);
    if (error == 0) {
        TEST_PRINTF(" OTP burn succeeded with new LCS = 0x%02x \n", nextLcs);
    } else {
        TEST_PRINTF_ERROR("Error: Failed to burn OTP!!\n");
    }

    return error;
}

/**
 * This function burns otp image through "back door", performs reset, and check if system booted to required lcs
 *
 */
uint32_t Test_GetKeySizeInWordsOtp(uint32_t keyType, uint32_t* keySizeInWords)
{
    // TBD HBK??
    switch (keyType){
        case TEST_HUK_KEY:
            *keySizeInWords = CC_OTP_HUK_SIZE_IN_WORDS;
            break;
        case TEST_KCP_KEY:
            *keySizeInWords = CC_OTP_KCP_SIZE_IN_WORDS;
            break;
        case TEST_KCE_KEY:
            *keySizeInWords = CC_OTP_KCE_SIZE_IN_WORDS;
            break;
        case TEST_KPICV_KEY:
            *keySizeInWords = CC_OTP_KPICV_SIZE_IN_WORDS;
            break;
        case TEST_KCEICV_KEY:
            *keySizeInWords = CC_OTP_KCEICV_SIZE_IN_WORDS;
            break;
        default:
            *keySizeInWords = 0;
            TEST_PRINTF_ERROR("ERROR: key type %d is not supported\n", keyType);
            return TEST_INVALID_PARAM_ERR;

    }
    return 0;
}
