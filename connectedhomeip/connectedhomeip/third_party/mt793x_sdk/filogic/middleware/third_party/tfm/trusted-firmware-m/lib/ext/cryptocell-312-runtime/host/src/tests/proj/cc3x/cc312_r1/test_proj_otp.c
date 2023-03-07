/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "cc_otp_defs.h"
#include "cc_regs.h"
#include "dx_crys_kernel.h"
#include "cc_pal_types.h"

#include "test_proj_otp.h"
#include "test_proj_defs.h"
#include "test_proj_otp.h"

#include "test_pal_log.h"
#include "test_pal_time.h"

void Test_ProjWriteOtpWord(uint32_t offsetInWords, uint32_t value)
{
    TEST_WRITE_OTP_BY_ENV(offsetInWords, value);
}

void Test_ProjWriteOtpField(uint32_t offsetInWords, uint32_t sizeWords, uint32_t *buff)
{
    uint32_t i = 0;

    for (i = 0; i < sizeWords; i++) {
        Test_ProjWriteOtpWord(offsetInWords + i, buff[i]);
        Test_PalDelay(1000);
    }
}

uint32_t Test_ProjReadOtpWord(uint32_t offsetInWords)
{
    uint32_t read_value = 0;

    read_value = TEST_READ_OTP_BY_ENV(offsetInWords);

    return read_value;
}

uint32_t Test_ProjSetZeroBitsOtpBuff(uint32_t *otpBuf, uint32_t keyType, uint32_t value)
{
    uint32_t hwWord;

    switch (keyType) {
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
            TEST_PRINTF_ERROR("ERROR: can't set zero bits for this type of key\n")
            ;
            return TEST_INVALID_PARAM_ERR;
    }
    return TEST_OK;
}

uint32_t Test_ProjSetNotInUseOtpBuff(uint32_t *otpBuf, uint32_t keyType, uint32_t value)
{
    uint32_t hwWord;

    switch (keyType) {
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
            TEST_PRINTF_ERROR("ERROR: can't set notInUse bit for this type of key\n")
            ;
            return TEST_INVALID_PARAM_ERR;

    }
    return TEST_OK;
}

uint32_t Test_ProjSetKeyOtpBuff(uint32_t *otpBuf, uint32_t *keyBuff, uint32_t keyType)
{
    uint32_t zeroCount = 0;
    uint32_t keySizeInWords = 0;
    uint32_t keyOffsetInOtp = 0;
    uint32_t rc = TEST_OK;

    switch (keyType) {
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
            TEST_PRINTF_ERROR("ERROR: key type is not supported %d\n", keyType);
            return TEST_INVALID_PARAM_ERR;

    }

    if (keyBuff != NULL) {
        memcpy((uint8_t *) (otpBuf + keyOffsetInOtp),
               (uint8_t *) keyBuff,
               keySizeInWords * sizeof(uint32_t));
    }

    TEST_CALC_BUFF_ZEROS((otpBuf + keyOffsetInOtp), keySizeInWords, zeroCount);

    if (keyType != TEST_HUK_KEY) {
        rc = Test_ProjSetNotInUseOtpBuff(otpBuf, keyType, 0);
    }

    rc = rc & Test_ProjSetZeroBitsOtpBuff(otpBuf, keyType, zeroCount);
    return rc;
}

uint32_t Test_ProjBurnOtpPlain(uint32_t *otpBuf, uint32_t nextLcs, OtpChipState_t chipIndication)
{

    uint32_t error = 0;
    uint32_t i = 0;

    CC_UNUSED_PARAM(chipIndication);

    /* Clean OTP */
    for (i = 0; i < TEST_OTP_SIZE_IN_WORDS; i++) {
        Test_ProjWriteOtpWord(i, 0);
        Test_PalDelay(1000);
    }
    /* Perform SW reset to reach CM LCS */
    Test_ProjPerformPowerOnReset();
    Test_PalDelay(1000);

    /* Copy new OTP buffer */
    for (i = 0; i < TEST_OTP_SIZE_IN_WORDS; i++) {
        TEST_PRINTF("writing Otp [0x%X] 0x%X", i, otpBuf[i]);
        Test_ProjWriteOtpWord(i, otpBuf[i]);
        Test_PalDelay(1000);
    }

    /*  Perform SW reset after writing to OTP new values */
    Test_ProjPerformPowerOnReset();

    /* verify LCS */
    error = Test_ProjCheckLcs(nextLcs);
    if (error == 0) {
        TEST_PRINTF(" OTP burn succeeded with new LCS = 0x%02x \n", nextLcs);
    } else {
        TEST_PRINTF_ERROR("Error: Failed to burn OTP!!\n");
    }

    return error;
}

uint32_t Test_ProjBurnOtp(uint32_t *otpBuf, uint32_t nextLcs, OtpChipState_t chipIndication)
{

    uint32_t error = 0;
    uint32_t i = 0;

    if (nextLcs != TEST_PROJ_LCS_CM) {
        for (i = TEST_HUK_KEY; i <= TEST_KCEICV_KEY; i++) {
            /* rtl key is not a part of OTP image => skip it */
            if (i == TEST_RTL_KEY) {
                continue;
            };
            /* set the the key to OTP buffer*/
            Test_ProjSetKeyOtpBuff(otpBuf, NULL, i);
        }
    }

    error = Test_ProjBurnOtpPlain(otpBuf, nextLcs, chipIndication);

    return error;
}

uint32_t Test_ProjBurnCmOtp(OtpChipState_t chipIndication)
{

    uint32_t error = 0;
    uint32_t i = 0;

    CC_UNUSED_PARAM(chipIndication);

    /* Clean OTP */
    for (i = 0; i < TEST_OTP_SIZE_IN_WORDS; i++) {
        TEST_WRITE_OTP_BY_ENV(i, 0);
        Test_PalDelay(1000);
    }

    /*  Perform SW reset after writing to OTP new values */
    Test_ProjPerformPowerOnReset();

    /* verify LCS */
    error = Test_ProjCheckLcs(TEST_PROJ_LCS_CM);
    if (error == 0) {
        TEST_PRINTF(" OTP burn succeeded with TEST_PROJ_LCS_DM\n");
    } else {
        TEST_PRINTF_ERROR("Error: Failed to burn OTP!!\n");
    }

    return error;
}

uint32_t Test_ProjBurnDmOtp(uint32_t *otpBuf, OtpChipState_t chipIndication)
{

    uint32_t error = 0;
    uint32_t i = 0;

    CC_UNUSED_PARAM(chipIndication);

    /* Clean OTP */
    for (i = 0; i < TEST_OTP_SIZE_IN_WORDS; i++) {
        TEST_WRITE_OTP_BY_ENV(i, 0);
        Test_PalDelay(1000);
    }

    /* Perform SW reset to reach CM LCS */
    Test_ProjPerformPowerOnReset();
    Test_PalDelay(1000);

    /* Prepare OTP */
    Test_ProjSetKeyOtpBuff(otpBuf, NULL, TEST_HUK_KEY);
    Test_ProjSetKeyOtpBuff(otpBuf, NULL, TEST_KPICV_KEY);
    Test_ProjSetKeyOtpBuff(otpBuf, NULL, TEST_KCEICV_KEY);

    /* Copy new OTP buffer */
    Test_ProjWriteOtpField(CC_OTP_HUK_OFFSET, CC_OTP_HUK_SIZE_IN_WORDS, &otpBuf[CC_OTP_HUK_OFFSET]);
    Test_ProjWriteOtpField(CC_OTP_KPICV_OFFSET,
                           CC_OTP_KPICV_SIZE_IN_WORDS,
                           &otpBuf[CC_OTP_KPICV_OFFSET]);
    Test_ProjWriteOtpField(CC_OTP_KCEICV_OFFSET,
                           CC_OTP_KCEICV_SIZE_IN_WORDS,
                           &otpBuf[CC_OTP_KCEICV_OFFSET]);
    Test_ProjWriteOtpField(CC_OTP_MANUFACTURE_FLAG_OFFSET,
                           1,
                           &otpBuf[CC_OTP_MANUFACTURE_FLAG_OFFSET]);
    Test_ProjWriteOtpField(CC_OTP_HBK0_OFFSET,
                           CC_OTP_HBK0_SIZE_IN_WORDS,
                           &otpBuf[CC_OTP_HBK0_OFFSET]);
    Test_ProjWriteOtpField(CC_OTP_HBK0_MIN_VERSION_OFFSET,
                           CC_OTP_HBK0_MIN_VERSION_SIZE_IN_WORDS,
                           &otpBuf[CC_OTP_HBK0_MIN_VERSION_OFFSET]);
    Test_ProjWriteOtpField(CC_OTP_ICV_GENERAL_PURPOSE_FLAG_OFFSET,
                           CC_OTP_ICV_GENERAL_PURPOSE_FLAG_SIZE_IN_WORDS,
                           &otpBuf[CC_OTP_ICV_GENERAL_PURPOSE_FLAG_OFFSET]);
    Test_ProjWriteOtpField(CC_OTP_DCU_OFFSET, CC_OTP_DCU_SIZE_IN_WORDS, &otpBuf[CC_OTP_DCU_OFFSET]);

    /*  Perform SW reset after writing to OTP new values */
    Test_ProjPerformPowerOnReset();

    /* verify LCS */
    error = Test_ProjCheckLcs(TEST_PROJ_LCS_DM);
    if (error == 0) {
        TEST_PRINTF(" OTP burn succeeded with TEST_PROJ_LCS_DM\n");
    } else {
        TEST_PRINTF_ERROR("Error: Failed to burn OTP!!\n");
    }

    return error;
}

uint32_t Test_ProjSetKdrInOtpBuff(uint32_t *otp, uint8_t *kdrBuff)
{
    int i = 0;
    int zeroCount = 0;
    int kdrZeroSize = ((0x1 << CC_OTP_MANUFACTURE_FLAG_HUK_ZERO_BITS_BIT_SIZE) - 1);
    int kdrZeroMask = (kdrZeroSize << CC_OTP_MANUFACTURE_FLAG_HUK_ZERO_BITS_BIT_SHIFT);

    if ((NULL == otp) || (NULL == kdrBuff)) {
        TEST_PRINTF_ERROR(" ilegal params\n");
        return TEST_INVALID_PARAM_ERR;
    }
    for (i = 0; i < CC_OTP_HUK_SIZE_IN_WORDS; i++) {
        TEST_CONVERT_BYTE_ARR_TO_WORD(&kdrBuff[i * sizeof(uint32_t)], otp[CC_OTP_START_OFFSET+i])
    }

    TEST_CALC_BUFF_ZEROS(&otp[CC_OTP_START_OFFSET], CC_OTP_HUK_SIZE_IN_WORDS, zeroCount);
    otp[CC_OTP_MANUFACTURE_FLAG_OFFSET] &= ~(kdrZeroMask);
    otp[CC_OTP_MANUFACTURE_FLAG_OFFSET] |= (zeroCount)
                    << CC_OTP_MANUFACTURE_FLAG_HUK_ZERO_BITS_BIT_SHIFT;
    return TEST_OK;

}

uint32_t Test_ProjSetHbkInOtpBuff(uint32_t *otp,
                                  uint8_t *hbkBuff,
                                  OtpHbkTypes_t type,
                                  uint8_t isFullHbk)
{
    uint32_t i = 0;
    uint32_t zeroCount = 0;
    uint32_t otpStartOffset =
                    (type == TEST_OTP_HBK1_TYPE) ? CC_OTP_HBK1_OFFSET : CC_OTP_HBK0_OFFSET;
    uint32_t hbkStartOffset = (type == TEST_OTP_HBK1_TYPE) ? CC_OTP_HBK0_SIZE_IN_WORDS : 0;
    uint32_t hbkWordSize;
    uint32_t hwWord = 0;

    if ((NULL == otp) || (NULL == hbkBuff)) {
        TEST_PRINTF_ERROR("ilegal params\n");
        return TEST_INVALID_PARAM_ERR;
    }

    TEST_PRINTF_ERROR("type %d\n", type);

    if ((type == TEST_OTP_HBK0_TYPE) || (type == TEST_OTP_HBK1_TYPE)) {
        hbkWordSize = CC_OTP_HBK0_SIZE_IN_WORDS;
    } else if (type == TEST_OTP_HBK_256_TYPE) {
        hbkWordSize = CC_OTP_HBK_SIZE_IN_WORDS;
    } else {
        TEST_PRINTF_ERROR("ilegal type %d\n", type);
        return TEST_INVALID_PARAM_ERR;
    }

    /* clear OTP HBK value */
    memset(&otp[otpStartOffset], 0, hbkWordSize * sizeof(uint32_t));

    if (isFullHbk == 1) {
        //clear ICV HBK zero count & clear HBK0 usage
        otp[CC_OTP_MANUFACTURE_FLAG_OFFSET] &= ~(0x7F000000);
        hwWord = otp[CC_OTP_MANUFACTURE_FLAG_OFFSET];
        CC_REG_FLD_SET2(0, OTP_MANUFACTURE_FLAG, HBK0_NOT_IN_USE, hwWord, 1);
    }

    TEST_PRINTF_ERROR("writing hbk otpStartOffset %d, hbkWordSize %d\n",
                      otpStartOffset,
                      hbkWordSize);

    for (i = 0; i < hbkWordSize; i++) {
        TEST_CONVERT_BYTE_ARR_TO_WORD(&hbkBuff[(hbkStartOffset + i) * sizeof(uint32_t)],
                                      otp[otpStartOffset + i]);
    }

    TEST_CALC_BUFF_ZEROS(&otp[otpStartOffset], hbkWordSize, zeroCount);

    if (type == TEST_OTP_HBK0_TYPE) {
        otp[CC_OTP_MANUFACTURE_FLAG_OFFSET] &= ~(0xFF000000);
        otp[CC_OTP_MANUFACTURE_FLAG_OFFSET] |= (zeroCount)
                        << CC_OTP_MANUFACTURE_FLAG_HBK0_ZERO_BITS_BIT_SHIFT;
        TEST_PRINTF_ERROR("zeros for HBK0 %d\n", zeroCount);
    } else {
        otp[CC_OTP_OEM_FLAG_OFFSET] &= ~(0xFF);
        otp[CC_OTP_OEM_FLAG_OFFSET] |= (zeroCount) << CC_OTP_OEM_FLAG_HBK1_ZERO_BITS_BIT_SHIFT;
    }

    TEST_PRINTF_ERROR("otp[0x%lx] 0x%x, otp[0x%lx] 0x%x\n",
                      CC_OTP_OEM_FLAG_OFFSET,
                      otp[CC_OTP_OEM_FLAG_OFFSET],
                      CC_OTP_MANUFACTURE_FLAG_OFFSET,
                      otp[CC_OTP_MANUFACTURE_FLAG_OFFSET]);

    return TEST_OK;
}

uint32_t Test_ProjSetKpicvInOtpBuff(uint32_t *otpBuf, uint8_t *kpicvBuff)
{
    int zeroCount = 0;
    uint32_t hwWord = 0;

    if ((NULL == otpBuf) || (NULL == kpicvBuff)) {
        TEST_PRINTF_ERROR(" ilegal params\n");
        return TEST_INVALID_PARAM_ERR;
    }
    memcpy((uint8_t *) (otpBuf + CC_OTP_KPICV_OFFSET),
           (uint8_t *) kpicvBuff,
           CC_OTP_KPICV_SIZE_IN_WORDS * sizeof(uint32_t));

    TEST_CALC_BUFF_ZEROS(&otpBuf[CC_OTP_KPICV_OFFSET], CC_OTP_KPICV_SIZE_IN_WORDS, zeroCount);
    hwWord = otpBuf[CC_OTP_MANUFACTURE_FLAG_OFFSET];
    CC_REG_FLD_SET2(0, OTP_MANUFACTURE_FLAG, KPICV_ZERO_BITS, hwWord, zeroCount);
    CC_REG_FLD_SET2(0, OTP_MANUFACTURE_FLAG, KPICV_NOT_IN_USE, hwWord, 0);
    otpBuf[CC_OTP_MANUFACTURE_FLAG_OFFSET] = hwWord;
    return TEST_OK;
}

uint32_t Test_ProjSetKceicvOtpBuff(uint32_t *otpBuf, uint8_t *kceicvBuff)
{
    int zeroCount = 0;
    uint32_t hwWord = 0;

    if ((NULL == otpBuf) || (NULL == kceicvBuff)) {
        TEST_PRINTF_ERROR(" ilegal params\n");
        return TEST_INVALID_PARAM_ERR;
    }
    memcpy((uint8_t *) (otpBuf + CC_OTP_KCEICV_OFFSET),
           (uint8_t *) kceicvBuff,
           CC_OTP_KCEICV_SIZE_IN_WORDS * sizeof(uint32_t));

    TEST_CALC_BUFF_ZEROS(&otpBuf[CC_OTP_KCEICV_OFFSET], CC_OTP_KCEICV_SIZE_IN_WORDS, zeroCount);
    hwWord = otpBuf[CC_OTP_MANUFACTURE_FLAG_OFFSET];
    CC_REG_FLD_SET2(0, OTP_MANUFACTURE_FLAG, KCEICV_ZERO_BITS, hwWord, zeroCount);
    CC_REG_FLD_SET2(0, OTP_MANUFACTURE_FLAG, KCEICV_NOT_IN_USE, hwWord, 0);
    otpBuf[CC_OTP_MANUFACTURE_FLAG_OFFSET] = hwWord;
    return TEST_OK;
}

uint32_t Test_ProjSetKcpOtpBuff(uint32_t *otpBuf, uint8_t *kcpBuff)
{
    int zeroCount = 0;
    uint32_t hwWord = 0;

    if ((NULL == otpBuf) || (NULL == kcpBuff)) {
        TEST_PRINTF_ERROR(" ilegal params\n");
        return TEST_INVALID_PARAM_ERR;
    }
    memcpy((uint8_t *) (otpBuf + CC_OTP_KCP_OFFSET),
           (uint8_t *) kcpBuff,
           CC_OTP_KCP_SIZE_IN_WORDS * sizeof(uint32_t));

    TEST_CALC_BUFF_ZEROS(&otpBuf[CC_OTP_KCP_OFFSET], CC_OTP_KCP_SIZE_IN_WORDS, zeroCount);
    hwWord = otpBuf[CC_OTP_OEM_FLAG_OFFSET];
    CC_REG_FLD_SET2(0, OTP_OEM_FLAG, KCP_ZERO_BITS, hwWord, zeroCount);
    CC_REG_FLD_SET2(0, OTP_OEM_FLAG, KCP_NOT_IN_USE, hwWord, 0);
    otpBuf[CC_OTP_OEM_FLAG_OFFSET] = hwWord;
    return TEST_OK;
}

uint32_t Test_ProjSetKceOtpBuff(uint32_t *otpBuf, uint8_t *kceBuff)
{
    int zeroCount = 0;
    uint32_t hwWord = 0;

    if ((NULL == otpBuf) || (NULL == kceBuff)) {
        TEST_PRINTF_ERROR(" ilegal params\n");
        return TEST_INVALID_PARAM_ERR;
    }
    memcpy((uint8_t *) (otpBuf + CC_OTP_KCE_OFFSET),
           (uint8_t *) kceBuff,
           CC_OTP_KCE_SIZE_IN_WORDS * sizeof(uint32_t));

    TEST_CALC_BUFF_ZEROS(&otpBuf[CC_OTP_KCE_OFFSET], CC_OTP_KCE_SIZE_IN_WORDS, zeroCount);
    hwWord = otpBuf[CC_OTP_OEM_FLAG_OFFSET];
    CC_REG_FLD_SET2(0, OTP_OEM_FLAG, KCE_ZERO_BITS, hwWord, zeroCount);
    CC_REG_FLD_SET2(0, OTP_OEM_FLAG, KCE_NOT_IN_USE, hwWord, 0);
    otpBuf[CC_OTP_OEM_FLAG_OFFSET] = hwWord;
    return TEST_OK;
}

uint32_t Test_ProjGetKeySizeInWordsOtp(uint32_t keyType, uint32_t* keySizeInWords)
{
    // TBD HBK??
    switch (keyType) {
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
            TEST_PRINTF_ERROR("ERROR: key type %d is not supported\n", keyType)
            ;
            return TEST_INVALID_PARAM_ERR;

    }
    return 0;
}

/**
 * This function gets hw key and its size from otp buffer to user buffer
 *
 */
uint32_t Test_ProjGetKeyOtpBuff(uint32_t *otpBuf,
                                uint32_t keyType,
                                uint32_t *keySizeInWords,
                                uint32_t *keyBuff)
{
    uint32_t keyOffsetInOtp = 0;
    uint32_t rc = TEST_OK;

    if ((NULL == otpBuf) || (NULL == keyBuff) || (NULL == keySizeInWords)) {
        TEST_PRINTF_ERROR(" ilegal params\n");
        return TEST_INVALID_PARAM_ERR;
    }

    // TBD HBK??
    switch (keyType) {
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
            TEST_PRINTF_ERROR("ERROR: key type is not supported\n")
            ;
            return TEST_INVALID_PARAM_ERR;

    }
    memcpy((uint8_t *) keyBuff,
           (uint8_t *) (otpBuf + keyOffsetInOtp),
           (*keySizeInWords) * sizeof(uint32_t));

    return rc;
}

uint32_t Test_ProjSetSwVerInOtpBuff(uint32_t *otp, uint32_t offset, uint32_t max_size)
{
    uint32_t i;

    for (i = 0; i < max_size; i++) {
        if (otp[offset + i] < 0xFFFFFFFF) {
            memset(&otp[offset + i + 1], 0x0, (max_size - i - 1) * sizeof(uint32_t));
            break;
        }
    }

    return TEST_OK;
}

