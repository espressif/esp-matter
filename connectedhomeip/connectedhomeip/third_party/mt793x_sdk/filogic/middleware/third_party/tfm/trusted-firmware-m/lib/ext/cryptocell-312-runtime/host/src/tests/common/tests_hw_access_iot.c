/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include "cc_pal_types.h"
#include "dx_env.h"
#include "dx_nvm.h"
#include "cc_regs.h"
#include "dx_host.h"
#include "tests_hw_access_iot.h"
#include "tests_phys_map.h"
#include "test_log.h"
#include "dx_crys_kernel.h"
#include "cc_otp_defs.h"



/******************************/
/*   function definitions     */
/*****************************/

unsigned int testCheckLcs(unsigned int  nextLcs)
{
    unsigned int regVal = 0;

    /* poll NVM register to be assure that the NVM boot is finished (and LCS and the keys are valid) */
    WAIT_NVM_IDLE(regVal);

    /* Read the LCS register */
    regVal = READ_REG(CC_REG_OFFSET(HOST_RGF, LCS_REG));
    regVal = CC_REG_FLD_GET(0, LCS_REG, LCS_REG, regVal);

    /* Verify lcs */
    if(regVal != nextLcs) {
                TEST_PRINTF_ERROR("actual LCS %d != expected LCS %d", regVal, nextLcs);
        return -1;
    }

    return 0;
}


unsigned int testCheckLcsAndError(unsigned int  nextLcs)
{
    unsigned int regVal = 0;

    /* poll NVM register to be assure that the NVM boot is finished (and LCS and the keys are valid) */
    WAIT_NVM_IDLE(regVal);

    /* Read the LCS register */
    regVal = READ_REG(CC_REG_OFFSET(HOST_RGF, LCS_REG));
    regVal = CC_REG_FLD_GET(0, LCS_REG, LCS_REG, regVal);

    /* Verify lcs */
    if(regVal != nextLcs) {
                TEST_PRINTF_ERROR("actual LCS %d != expected LCS %d", regVal, nextLcs);
        return -1;
    }

    if ((CC_REG_FLD_GET(0, LCS_REG, ERROR_KDR_ZERO_CNT, regVal) != 0) ||
            (CC_REG_FLD_GET(0, LCS_REG, ERROR_KPICV_ZERO_CNT, regVal) != 0) ||
            (CC_REG_FLD_GET(0, LCS_REG, ERROR_KCEICV_ZERO_CNT, regVal) != 0) ||
        (CC_REG_FLD_GET(0, LCS_REG, ERROR_PROV_ZERO_CNT, regVal) != 0) ||
            (CC_REG_FLD_GET(0, LCS_REG, ERROR_KCE_ZERO_CNT, regVal) != 0)) {
                TEST_PRINTF_ERROR("regVal 0x%x indicates error for LCS %d", regVal, nextLcs);
        return -1;
    }

    return 0;
}


unsigned int testGetLcs(unsigned int  *lcs)
{
    unsigned int regVal = 0;

    /* poll NVM register to be assure that the NVM boot is finished (and LCS and the keys are valid) */
    WAIT_NVM_IDLE(regVal);

    /* Read the LCS register */
    regVal = READ_REG(CC_REG_OFFSET(HOST_RGF, LCS_REG));
    regVal = CC_REG_FLD_GET(0, LCS_REG, LCS_REG, regVal);

    *lcs = regVal;

    return 0;
}


unsigned int testGetOtpSize(unsigned int * size)
{
    unsigned int regVal = 0;

    /* poll NVM register to be assure that the NVM boot is finished (and LCS and the keys are valid) */
    WAIT_NVM_IDLE(regVal);

    /* Read the LCS register */
    regVal = READ_REG(DX_OTP_ADDR_WIDTH_DEF_REG_OFFSET);
    regVal = CC_REG_FLD_GET(0, OTP_ADDR_WIDTH_DEF, VALUE, regVal);

    *size = regVal;

    return 0;
}


/* Global Reset of CC and AO and env regs */
void performPowerOnReset(void)
{
    WRITE_ENV(DX_ENV_CC_POR_N_ADDR_REG_OFFSET , 0x1UL);
    usleep(1000);

    /* poll NVM register to assure that the NVM boot is finished (and LCS and the keys are valid) */
    WAIT_NVM_IDLE(regVal);

#ifdef BIG__ENDIAN
    /* Set DMA endianess to big */
    WRITE_REG(CC_REG_OFFSET(HOST_RGF, HOST_ENDIAN) , 0xCCUL);
#else /* LITTLE__ENDIAN */
    WRITE_REG(CC_REG_OFFSET(HOST_RGF, HOST_ENDIAN) , 0x00UL);
#endif

    /* turn off the DFA since Cerberus doesn't support it */
    TURN_DFA_OFF();
    return;

}


/* Reset both CC and AO regs */
void performColdReset(void)
{
    WRITE_ENV(DX_ENV_CC_COLD_RST_REG_OFFSET , 0x1UL);
    usleep(1000);

#ifdef BIG__ENDIAN
    /* Set DMA endianess to big */
    WRITE_REG(CC_REG_OFFSET(HOST_RGF, HOST_ENDIAN) , 0xCCUL);
#else /* LITTLE__ENDIAN */
    WRITE_REG(CC_REG_OFFSET(HOST_RGF, HOST_ENDIAN) , 0x00UL);
#endif
    /* turn off the DFA since Cerberus doesn't support it */
    TURN_DFA_OFF();
    return;
}


/* Reset only CC regs */
void performWarmReset(void)
{
    WRITE_ENV(DX_ENV_CC_RST_N_REG_OFFSET , 0x1UL);
    usleep(1000);

#ifdef BIG__ENDIAN
    /* Set DMA endianess to big */
    WRITE_REG(CC_REG_OFFSET(HOST_RGF, HOST_ENDIAN) , 0xCCUL);
#else /* LITTLE__ENDIAN */
    WRITE_REG(CC_REG_OFFSET(HOST_RGF, HOST_ENDIAN) , 0x00UL);
#endif
    return;

}

unsigned int testBurnOtp(unsigned int  *otpBuf, unsigned int  nextLcs)
{

    CCError_t error = 0;
    unsigned int i = 0;

    /* Clean OTP */
    for (i = 0; i < TEST_OTP_SIZE_IN_WORDS; i++) {
        WRITE_OTP(i, 0);
        usleep(1000);
    }

    /* Perform SW reset to reach CM LCS */
    performPowerOnReset();
    usleep(1000);

    /* Copy new OTP buffer */
    for (i = 0; i < TEST_OTP_SIZE_IN_WORDS; i++) {
                TEST_PRINTF("writing Otp [0x%X] 0x%X", i, otpBuf[i]);
        WRITE_OTP(i, otpBuf[i]);
        usleep(1000);
    }

    /*  Perform SW reset after writing to OTP new values */
    performPowerOnReset();

    /* verify LCS */
    error = testCheckLcs(nextLcs);
    if (error == 0) {
        TEST_PRINTF(" OTP burn succeeded with new LCS = 0x%02x \n", nextLcs);
    } else {
        TEST_PRINTF_ERROR("Error: Failed to burn OTP!!\n");
    }

    return error;
}


uint32_t initPlatform(void)
{
    unsigned int rc = 0;

    rc = TestMapCCRegs();
    if (rc != 0) {
        TEST_PRINTF_ERROR("Failed to TestMapCCRegs");
        return rc;
    }

    TST_SET_ENV_TO_SECURE();

    return 0;
}

void freePlatform(void)
{
    TestUnmapCCRegs();
}


/*
 * @brief This function
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return void -

 */
unsigned int testSetKdrInOtpBuff(unsigned int *otp, unsigned char *kdrBuff)
{
    int  i = 0;
    int  zeroCount = 0;
    int kdrZeroSize = ((0x1 << CC_OTP_MANUFACTURE_FLAG_HUK_ZERO_BITS_BIT_SIZE)-1);
    int kdrZeroMask = (kdrZeroSize << CC_OTP_MANUFACTURE_FLAG_HUK_ZERO_BITS_BIT_SHIFT);

    if ((NULL == otp) ||
        (NULL == kdrBuff)) {
        TEST_PRINTF_ERROR(" ilegal params\n");
        return TEST_INVALID_PARAM_ERR;
    }
    for (i=0; i< CC_OTP_HUK_SIZE_IN_WORDS; i++) {
        TEST_CONVERT_BYTE_ARR_TO_WORD(&kdrBuff[i*sizeof(uint32_t)], otp[CC_OTP_START_OFFSET+i])
    }

    TEST_CALC_BUFF_ZEROS(&otp[CC_OTP_START_OFFSET], CC_OTP_HUK_SIZE_IN_WORDS, zeroCount);
    otp[CC_OTP_MANUFACTURE_FLAG_OFFSET] &= ~(kdrZeroMask);
    otp[CC_OTP_MANUFACTURE_FLAG_OFFSET] |= (zeroCount)<<CC_OTP_MANUFACTURE_FLAG_HUK_ZERO_BITS_BIT_SHIFT;
    return TEST_OK;

}


/*
 * @brief This function
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return void -

 */
unsigned int testSetHbkInOtpBuff(unsigned int *otp, unsigned char *hbkBuff, OtpHbkTypes_t type)
{
    int  i = 0;
    int  zeroCount = 0;
    int otpStartOffset = (type == TEST_OTP_HBK1_TYPE)?CC_OTP_HBK1_OFFSET:CC_OTP_HBK0_OFFSET;
    int hbkStartOffset = (type == TEST_OTP_HBK1_TYPE)?CC_OTP_HBK0_SIZE_IN_WORDS:0;
    int hbkWordSize;


    if ((NULL == otp) ||
        (NULL == hbkBuff)) {
        TEST_PRINTF_ERROR("testSetHbkInOtpBuff ilegal params\n");
        return TEST_INVALID_PARAM_ERR;
    }
    TEST_PRINTF_ERROR("type %d\n", type);
    if ((type == TEST_OTP_HBK0_TYPE) ||
        (type == TEST_OTP_HBK1_TYPE)) {
        hbkWordSize = CC_OTP_HBK0_SIZE_IN_WORDS;
    } else if ((type == TEST_OTP_HBK_256_TYPE) ||
           (type == (TEST_OTP_HBK0_TYPE | TEST_OTP_HBK1_TYPE))) {
        hbkWordSize = CC_OTP_HBK0_SIZE_IN_WORDS+CC_OTP_HBK1_SIZE_IN_WORDS;
    } else {
        TEST_PRINTF_ERROR("ilegal type %d\n", type);
        return TEST_INVALID_PARAM_ERR;
    }
    //clear OEM HBK and its zero count
    memset(&otp[CC_OTP_HBK1_OFFSET], 0, CC_OTP_HBK1_SIZE_IN_WORDS);
    otp[CC_OTP_OEM_FLAG_OFFSET] &= ~(0xFF);
    //clear ICV HBK
    memset(&otp[CC_OTP_HBK0_OFFSET], 0, CC_OTP_HBK0_SIZE_IN_WORDS);
    //clear ICV HBK zero count
    otp[CC_OTP_MANUFACTURE_FLAG_OFFSET] &= ~(0xFF000000);
    // clear HBK0 usage
    otp[CC_OTP_MANUFACTURE_FLAG_OFFSET] |= (1<<CC_OTP_MANUFACTURE_FLAG_HBK0_NOT_IN_USE_BIT_SHIFT);
    TEST_PRINTF_ERROR("writing hbk otpStartOffset %d, hbkWordSize %d\n", otpStartOffset, hbkWordSize);
    for (i=0; i< hbkWordSize; i++) {
        TEST_CONVERT_BYTE_ARR_TO_WORD(&hbkBuff[(hbkStartOffset+i)*sizeof(uint32_t)], otp[otpStartOffset+i]);
    }

    if (type & TEST_OTP_HBK0_TYPE) {
        TEST_CALC_BUFF_ZEROS(&otp[CC_OTP_HBK0_OFFSET], CC_OTP_HBK0_SIZE_IN_WORDS, zeroCount);
        otp[CC_OTP_MANUFACTURE_FLAG_OFFSET] &= ~(0xFF000000);
        otp[CC_OTP_MANUFACTURE_FLAG_OFFSET] |= (zeroCount)<<CC_OTP_MANUFACTURE_FLAG_HBK0_ZERO_BITS_BIT_SHIFT;
        TEST_PRINTF_ERROR("zeros for HBK1 %d\n", zeroCount);
    }
    if (type & TEST_OTP_HBK1_TYPE) {
        TEST_CALC_BUFF_ZEROS(&otp[CC_OTP_HBK1_OFFSET], CC_OTP_HBK1_SIZE_IN_WORDS, zeroCount);
        otp[CC_OTP_OEM_FLAG_OFFSET] &= ~(0xFF);
        otp[CC_OTP_OEM_FLAG_OFFSET] |= (zeroCount)<<CC_OTP_OEM_FLAG_HBK1_ZERO_BITS_BIT_SHIFT;
        TEST_PRINTF_ERROR("zeros for HBK1 %d\n", zeroCount);
    }
    if (type == TEST_OTP_HBK_256_TYPE) {
        TEST_CALC_BUFF_ZEROS(&otp[CC_OTP_HBK_OFFSET], hbkWordSize, zeroCount);
        otp[CC_OTP_OEM_FLAG_OFFSET] &= ~(0xFF);
        otp[CC_OTP_OEM_FLAG_OFFSET] |= (zeroCount)<<CC_OTP_OEM_FLAG_HBK1_ZERO_BITS_BIT_SHIFT;
        TEST_PRINTF_ERROR("zeros for HBK0 %d\n", zeroCount);
    }
    TEST_PRINTF_ERROR("otp[0x%lx] 0x%x, otp[0x%lx] 0x%x\n", CC_OTP_OEM_FLAG_OFFSET,
        otp[CC_OTP_OEM_FLAG_OFFSET], CC_OTP_MANUFACTURE_FLAG_OFFSET, otp[CC_OTP_MANUFACTURE_FLAG_OFFSET]);

    return TEST_OK;
}

/*
 * @brief This function
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return void -

 */
unsigned int testSetKpicvInOtpBuff(unsigned int *otpBuf, unsigned int *kpicvBuff)
{
    int  zeroCount = 0;
    uint32_t hwWord = 0;

    if ((NULL == otpBuf) ||
        (NULL == kpicvBuff)) {
        TEST_PRINTF_ERROR(" ilegal params\n");
        return TEST_INVALID_PARAM_ERR;
    }
    memcpy((uint8_t *)(otpBuf+CC_OTP_KPICV_OFFSET), (uint8_t *)kpicvBuff, CC_OTP_KPICV_SIZE_IN_WORDS*sizeof(uint32_t));

    TEST_CALC_BUFF_ZEROS(&otpBuf[CC_OTP_KPICV_OFFSET], CC_OTP_KPICV_SIZE_IN_WORDS, zeroCount);
    hwWord = otpBuf[CC_OTP_MANUFACTURE_FLAG_OFFSET];
    CC_REG_FLD_SET2(0, OTP_MANUFACTURE_FLAG, KPICV_ZERO_BITS, hwWord, zeroCount);
    CC_REG_FLD_SET2(0, OTP_MANUFACTURE_FLAG, KPICV_NOT_IN_USE, hwWord, 0);
    otpBuf[CC_OTP_MANUFACTURE_FLAG_OFFSET] = hwWord;
    return TEST_OK;
}
/*
 * @brief This function
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return void -

 */
unsigned int testSetKceicvOtpBuff(unsigned int *otpBuf, unsigned int *kceicvBuff)
{
    int  zeroCount = 0;
    uint32_t hwWord = 0;

    if ((NULL == otpBuf) ||
        (NULL == kceicvBuff)) {
        TEST_PRINTF_ERROR(" ilegal params\n");
        return TEST_INVALID_PARAM_ERR;
    }
    memcpy((uint8_t *)(otpBuf+CC_OTP_KCEICV_OFFSET), (uint8_t *)kceicvBuff, CC_OTP_KCEICV_SIZE_IN_WORDS*sizeof(uint32_t));

    TEST_CALC_BUFF_ZEROS(&otpBuf[CC_OTP_KCEICV_OFFSET], CC_OTP_KCEICV_SIZE_IN_WORDS, zeroCount);
    hwWord = otpBuf[CC_OTP_MANUFACTURE_FLAG_OFFSET];
    CC_REG_FLD_SET2(0, OTP_MANUFACTURE_FLAG, KCEICV_ZERO_BITS, hwWord, zeroCount);
    CC_REG_FLD_SET2(0, OTP_MANUFACTURE_FLAG, KCEICV_NOT_IN_USE, hwWord, 0);
    otpBuf[CC_OTP_MANUFACTURE_FLAG_OFFSET] = hwWord;
    return TEST_OK;
}

/*
 * @brief This function
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return void -

 */
unsigned int testSetKcpOtpBuff(unsigned int *otpBuf, unsigned int *kcpBuff)
{
    int  zeroCount = 0;
    uint32_t hwWord = 0;

    if ((NULL == otpBuf) ||
        (NULL == kcpBuff)) {
        TEST_PRINTF_ERROR(" ilegal params\n");
        return TEST_INVALID_PARAM_ERR;
    }
    memcpy((uint8_t *)(otpBuf+CC_OTP_KCP_OFFSET), (uint8_t *)kcpBuff, CC_OTP_KCP_SIZE_IN_WORDS*sizeof(uint32_t));

    TEST_CALC_BUFF_ZEROS(&otpBuf[CC_OTP_KCP_OFFSET], CC_OTP_KCP_SIZE_IN_WORDS, zeroCount);
    hwWord = otpBuf[CC_OTP_OEM_FLAG_OFFSET];
    CC_REG_FLD_SET2(0, OTP_OEM_FLAG, KCP_ZERO_BITS, hwWord, zeroCount);
    CC_REG_FLD_SET2(0, OTP_OEM_FLAG, KCP_NOT_IN_USE, hwWord, 0);
    otpBuf[CC_OTP_OEM_FLAG_OFFSET] = hwWord;
    return TEST_OK;
}

/*
 * @brief This function
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return void -

 */
unsigned int testSetKceOtpBuff(unsigned int *otpBuf, unsigned int *kceBuff)
{
    int  zeroCount = 0;
    uint32_t hwWord = 0;

    if ((NULL == otpBuf) ||
        (NULL == kceBuff)) {
        TEST_PRINTF_ERROR(" ilegal params\n");
        return TEST_INVALID_PARAM_ERR;
    }
    memcpy((uint8_t *)(otpBuf+CC_OTP_KCE_OFFSET), (uint8_t *)kceBuff, CC_OTP_KCE_SIZE_IN_WORDS*sizeof(uint32_t));

    TEST_CALC_BUFF_ZEROS(&otpBuf[CC_OTP_KCE_OFFSET], CC_OTP_KCE_SIZE_IN_WORDS, zeroCount);
    hwWord = otpBuf[CC_OTP_OEM_FLAG_OFFSET];
    CC_REG_FLD_SET2(0, OTP_OEM_FLAG, KCE_ZERO_BITS, hwWord, zeroCount);
    CC_REG_FLD_SET2(0, OTP_OEM_FLAG, KCE_NOT_IN_USE, hwWord, 0);
    otpBuf[CC_OTP_OEM_FLAG_OFFSET] = hwWord;
    return TEST_OK;
}

/*
 * @brief This function suggegst that all keys are already set, and set manufacture and oem flags appropriately
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return void -

 */
unsigned int testSetLcsOtpBuff(unsigned int *otpBuf, unsigned int lcsState)
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
                // continue to clean oem_flag[7:0] bits as for DM
        case TESTS_LCS_DM:
                hwWord = otpBuf[CC_OTP_OEM_FLAG_OFFSET];
                CC_REG_FLD_SET2(0, OTP_OEM_FLAG, HBK_ZERO_BITS, hwWord, 0);
                otpBuf[CC_OTP_OEM_FLAG_OFFSET] = hwWord;
                break;

        case TESTS_LCS_SEC_DISABLED:
        case TESTS_LCS_SEC_ENABLED:
                break;
        default:
                TEST_PRINTF_ERROR(" ilegal lcs params\n");
                return TEST_INVALID_PARAM_ERR;
        }
        return TEST_OK;
}
