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
#include "dx_reg_base_host.h"
#include "cc_regs.h"
#include "dx_host.h"
#include "tests_hw_access.h"
#include "tests_phys_map.h"
#include "test_log.h"
#include "dx_crys_kernel.h"



/////////////////////////////////////
//         macro defines           //
/////////////////////////////////////


/*************************/
/*   Global variables    */
/*************************/


/******************************/
/*   function definitions     */
/*****************************/
unsigned int testCheckLcs(unsigned int  nextLcs)
{
    unsigned int regVal = 0;
    unsigned int error = TEST_OK;

    /* Wait on LCS valid before reading the LCS register */
    do {
        regVal = READ_REG(CC_REG_OFFSET(HOST_RGF, LCS_IS_VALID));
        regVal = CC_REG_FLD_GET(0, LCS_IS_VALID, VALUE, regVal);
    }while (!regVal);

    /* Read the LCS register */
    regVal = READ_REG(CC_REG_OFFSET(HOST_RGF, LCS_REG));
    regVal = CC_REG_FLD_GET(0, LCS_REG, LCS_REG, regVal);
    TEST_PRINTF_ERROR("regVal is 0x%x", regVal);

    /* Verify lcs */
    if(regVal != nextLcs) {
        TEST_PRINTF_ERROR("Failed to verify new LCS !!!\n");
        return TEST_BURN_OTP_LCS_ERR;
    }

    /* Check HW errors */
    switch(nextLcs){
    case TESTS_LCS_SEC_ENABLED:
        /* Check RKEK error bit */
        regVal = READ_REG(CC_REG_OFFSET(HOST_RGF, LCS_REG));
        regVal = CC_REG_FLD_GET(0, LCS_REG, ERROR_KDR_ZERO_CNT, regVal);
        if (regVal) {
            error = TEST_BURN_OTP_KDR_ERR;
            break;
        }
        /* FALLTHROUGH */

    case TESTS_LCS_DM:
        /* Check Provision (Scp) error bit */
        regVal = READ_REG(CC_REG_OFFSET(HOST_RGF, LCS_REG));
        regVal = CC_REG_FLD_GET(0, LCS_REG, ERROR_PROV_ZERO_CNT, regVal);
        if (regVal) {
            error = TEST_BURN_OTP_SCP_ERR;
        }
        break;
    default:
        break;
    }

    return error;
}


/* Global Reset of CC and AO and env regs */
void performPowerOnReset(void)
{
    /* Call sw reset to reset the CC before starting to work with it */
    WRITE_ENV(DX_ENV_CC_POR_N_ADDR_REG_OFFSET , 0x1UL);
    usleep(1000);

    WRITE_REG(CC_REG_OFFSET(HOST_RGF, AXIM_CACHE_PARAMS), 0x277UL);
    usleep(1000);

#ifdef BIG__ENDIAN
    /* Set DMA endianess to big */
    WRITE_REG(CC_REG_OFFSET(HOST_RGF, HOST_ENDIAN) , 0xCCUL);
#else /* LITTLE__ENDIAN */
    WRITE_REG(CC_REG_OFFSET(HOST_RGF, HOST_ENDIAN) , 0x00UL);
#endif
    return;

}


/* Reset both CC and AO regs */
void performColdReset(void)
{
    /* perform global reset */
    WRITE_ENV(DX_ENV_CC_COLD_RST_REG_OFFSET , 0x1UL);
    usleep(1000);

    WRITE_REG(CC_REG_OFFSET(HOST_RGF, AXIM_CACHE_PARAMS), 0x277UL);
    usleep(1000);

#ifdef BIG__ENDIAN
    /* Set DMA endianess to big */
    WRITE_REG(CC_REG_OFFSET(HOST_RGF, HOST_ENDIAN) , 0xCCUL);
#else /* LITTLE__ENDIAN */
    WRITE_REG(CC_REG_OFFSET(HOST_RGF, HOST_ENDIAN) , 0x00UL);
#endif
    return;
}


/* Reset only CC regs */
void performWarmReset(void)
{
    WRITE_ENV(DX_ENV_CC_RST_N_REG_OFFSET , 0x1UL);
    usleep(1000);

    WRITE_REG(CC_REG_OFFSET(HOST_RGF, AXIM_CACHE_PARAMS), 0x277UL);
    usleep(1000);

#ifdef BIG__ENDIAN
    /* Set DMA endianess to big */
    WRITE_REG(CC_REG_OFFSET(HOST_RGF, HOST_ENDIAN) , 0xCCUL);
#else /* LITTLE__ENDIAN */
    WRITE_REG(CC_REG_OFFSET(HOST_RGF, HOST_ENDIAN) , 0x00UL);
#endif
    return;

}

unsigned int testBurnOtp(unsigned int  *otp, unsigned int  nextLcs)
{

    CCError_t error = TEST_OK;
    unsigned int i = 0;

#if 0
    /* Clear OTP is not needed any more since we call to powerOnReset to clear env regs */
    error = testClearOtp();
    if (error) {
        TEST_PRINTF_ERROR("Failed to clear OTP!!!\n");
        return error;
    }
#endif

    /*  Perform SW reset to reach CM LCS */
    performPowerOnReset();

    for (i = 0; i < TEST_OTP_SIZE_IN_WORDS; i++) {
                TEST_PRINTF("writing Otp [%d] 0x%X", i, otp[i]);
        WRITE_OTP(i, otp[i]);
        usleep(1000);

    }
#ifdef DEBUG
    uint32_t tempOtp;
    for (i = 0; i < TEST_OTP_SIZE_IN_WORDS; i++) {
        tempOtp = READ_OTP(i);
        TEST_PRINTF_ERROR("tempOtp [%d] 0x%X", i, tempOtp);

        }
#endif

    /*  Perform SW reset after writing to OTP new values */
    performPowerOnReset();

    /* verify LCS */
    error = testCheckLcs(nextLcs);
    if (error == TEST_OK) {
        TEST_PRINTF(" OTP burn succeeded with new LCS = 0x%02x \n", nextLcs);
    } else {
        TEST_PRINTF_ERROR("Error: Failed to burn OTP lcs is %d!!!\n", nextLcs);
    }

    return error;
}


unsigned int testClearOtp(void)
{
    CCError_t error = TEST_OK;
    unsigned int i = 0;

    /*  Perform SW reset to reach CM LCS */
    performPowerOnReset();

    for (i = 0; i < TEST_OTP_SIZE_IN_WORDS; i++) {
        WRITE_OTP(i, 0);
        usleep(1000);

    }
#ifdef DEBUG
    uint32_t tempOtp;
    for (i = 0; i < TEST_OTP_SIZE_IN_WORDS; i++) {
        tempOtp = READ_OTP(i);
        TEST_PRINTF_ERROR("tempOtp [%d] 0x%X", i, tempOtp);

        }
#endif

    /*  Perform SW reset after writing to OTP new values */
    performPowerOnReset();

    /* verify LCS */
    error = testCheckLcs(0);
    if (error == TEST_OK) {
        TEST_PRINTF(" OTP burn succeeded with new LCS = CM \n");
    } else {
        TEST_PRINTF_ERROR("Error: Failed to burn OTP \n");
    }

    return error;
}



unsigned int testSetScpInOtpBuff(unsigned int *otp, unsigned char *scpBuff)
{
    int  i = 0;
    int  zeroCount = 0;
    int scpZeroSize = ((0x1 << OTP_MANUFACTOR_FLAG_SCP_ZEROS_NUM_BITS)-1);
    int scpZeroMask = (scpZeroSize << OTP_MANUFACTOR_FLAG_SCP_ZEROS_BIT_OFFSET);

    if ((NULL == otp) ||
        (NULL == scpBuff)) {
        TEST_PRINTF_ERROR("testSetScpInOtpBuff ilegal params\n");
        return TEST_INVALID_PARAM_ERR;
    }
    for (i=0; i< OTP_SCP_SIZE_IN_WORDS; i++) {
        TEST_CONVERT_BYTE_ARR_TO_WORD(&scpBuff[i*sizeof(uint32_t)], otp[OTP_SCP_START_WORD_OFFSET+i])
    }

    TEST_CALC_BUFF_ZEROS(&otp[OTP_SCP_START_WORD_OFFSET], OTP_SCP_SIZE_IN_WORDS, zeroCount);
    otp[OTP_MANUFACTOR_FLAG_START_WORD_OFFSET] &= ~(scpZeroMask);
    otp[OTP_MANUFACTOR_FLAG_START_WORD_OFFSET] |= (zeroCount)<<OTP_MANUFACTOR_FLAG_SCP_ZEROS_BIT_OFFSET;
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
unsigned int testSetKdrInOtpBuff(unsigned int *otp, unsigned char *kdrBuff)
{
    int  i = 0;
    int  zeroCount = 0;
    int kdrZeroSize = ((0x1 << OTP_MANUFACTOR_FLAG_KDR_ZEROS_NUM_BITS)-1);
    int kdrZeroMask = (kdrZeroSize << OTP_MANUFACTOR_FLAG_KDR_ZEROS_BIT_OFFSET);

    if ((NULL == otp) ||
        (NULL == kdrBuff)) {
        TEST_PRINTF_ERROR(" ilegal params\n");
        return TEST_INVALID_PARAM_ERR;
    }
    for (i=0; i< OTP_KRD_SIZE_IN_WORDS; i++) {
        TEST_CONVERT_BYTE_ARR_TO_WORD(&kdrBuff[i*sizeof(uint32_t)], otp[OTP_KRD_START_WORD_OFFSET+i])
    }

    TEST_CALC_BUFF_ZEROS(&otp[OTP_KRD_START_WORD_OFFSET], OTP_KRD_SIZE_IN_WORDS, zeroCount);
    otp[OTP_MANUFACTOR_FLAG_START_WORD_OFFSET] &= ~(kdrZeroMask);
    otp[OTP_MANUFACTOR_FLAG_START_WORD_OFFSET] |= (zeroCount)<<OTP_MANUFACTOR_FLAG_KDR_ZEROS_BIT_OFFSET;
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
    int otpStartOffset = (type == TEST_OTP_HBK1_TYPE)?OTP_HBK1_START_WORD_OFFSET:OTP_HBK0_START_WORD_OFFSET;
    int hbkStartOffset = (type == TEST_OTP_HBK1_TYPE)?OTP_HBK0_SIZE_IN_WORDS:0;
    int hbkWordSize;
    int hbkZeroSizeBits = 0;
    int hbkZeroMaskBits = 0;


    if ((NULL == otp) ||
        (NULL == hbkBuff)) {
        TEST_PRINTF_ERROR("testSetHbkInOtpBuff ilegal params\n");
        return TEST_INVALID_PARAM_ERR;
    }
    TEST_PRINTF_ERROR("type %d\n", type);
    if ((type == TEST_OTP_HBK0_TYPE) ||
        (type == TEST_OTP_HBK1_TYPE)) {
        hbkWordSize = OTP_HBK0_SIZE_IN_WORDS;
    } else if ((type == TEST_OTP_HBK_256_TYPE) ||
           (type == (TEST_OTP_HBK0_TYPE | TEST_OTP_HBK1_TYPE))) {
        hbkWordSize = OTP_HBK0_SIZE_IN_WORDS+OTP_HBK1_SIZE_IN_WORDS;
    } else {
        TEST_PRINTF_ERROR("ilegal type %d\n", type);
        return TEST_INVALID_PARAM_ERR;
    }
    TEST_PRINTF_ERROR("writing hbk otpStartOffset %d, hbkWordSize %d\n", otpStartOffset, hbkWordSize);
    for (i=0; i< hbkWordSize; i++) {
        TEST_CONVERT_BYTE_ARR_TO_WORD(&hbkBuff[(hbkStartOffset+i)*sizeof(uint32_t)], otp[otpStartOffset+i]);
    }

    /* calculate HBK's zeros and save in OTP */
    if (type == TEST_OTP_HBK0_TYPE) {
        memset(&otp[OTP_HBK1_START_WORD_OFFSET], 0, OTP_HBK1_SIZE_IN_WORDS);
    }
    if (type == TEST_OTP_HBK1_TYPE) {
        memset(&otp[OTP_HBK0_START_WORD_OFFSET], 0, OTP_HBK0_SIZE_IN_WORDS);
    }
    otp[OTP_DM_DEF_START_WORD_OFFSET] &= ~(0xFFFF);
    if (type == TEST_OTP_HBK_256_TYPE) {
        otp[OTP_DM_DEF_START_WORD_OFFSET] |= 0xFF00;
    }
    if (type & TEST_OTP_HBK1_TYPE) {
        hbkZeroSizeBits = ((0x1 << OTP_DM_DEF_HBK1_ZEROS_NUM_BITS)-1);
        hbkZeroMaskBits = (hbkZeroSizeBits << OTP_DM_DEF_HBK1_ZEROS_BIT_OFFSET);
        TEST_CALC_BUFF_ZEROS(&otp[OTP_HBK1_START_WORD_OFFSET], OTP_HBK1_SIZE_IN_WORDS, zeroCount);
        otp[OTP_DM_DEF_START_WORD_OFFSET] &= ~(hbkZeroMaskBits);
        otp[OTP_DM_DEF_START_WORD_OFFSET] |= (zeroCount)<<OTP_DM_DEF_HBK1_ZEROS_BIT_OFFSET;
        TEST_PRINTF_ERROR("zeros for HBK1 %d\n", zeroCount);
    }
    if (type == (TEST_OTP_HBK0_TYPE | TEST_OTP_HBK1_TYPE)) {
        hbkWordSize = OTP_HBK0_SIZE_IN_WORDS; // OTP_HBK1_SIZE_IN_WORDS  was already writen
    }
    if ((type & TEST_OTP_HBK0_TYPE) ||
        (type == TEST_OTP_HBK_256_TYPE)) {
        hbkZeroSizeBits = ((0x1 << OTP_DM_DEF_HBK0_ZEROS_NUM_BITS)-1);
        hbkZeroMaskBits = (hbkZeroSizeBits << OTP_DM_DEF_HBK0_ZEROS_BIT_OFFSET);
        TEST_CALC_BUFF_ZEROS(&otp[OTP_HBK0_START_WORD_OFFSET], hbkWordSize, zeroCount);
        otp[OTP_DM_DEF_START_WORD_OFFSET] &= ~(hbkZeroMaskBits);
        otp[OTP_DM_DEF_START_WORD_OFFSET] |= (zeroCount)<<OTP_DM_DEF_HBK0_ZEROS_BIT_OFFSET;
        TEST_PRINTF_ERROR("zeros for HBK0 %d\n", zeroCount);
    }
    TEST_PRINTF_ERROR("otp[OTP_DM_DEF_START_WORD_OFFSET] 0x%x\n", otp[OTP_DM_DEF_START_WORD_OFFSET]);

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
unsigned int testSetSecureInOtpBuff(unsigned int *otp, unsigned char enFlag)
{
    int   secEnSizeBits = ((0x1 << OTP_MANUFACTOR_FLAG_SEC_EN_NUM_BITS)-1);
    int   secEnMaskBits = (secEnSizeBits << OTP_MANUFACTOR_FLAG_SEC_EN_BIT_OFFSET);
    if (NULL == otp) {
        TEST_PRINTF_ERROR("testSetSecureInOtpBuff ilegal params\n");
        return TEST_INVALID_PARAM_ERR;
    }
    otp[OTP_MANUFACTOR_FLAG_START_WORD_OFFSET] &= ~(secEnMaskBits);
    otp[OTP_MANUFACTOR_FLAG_START_WORD_OFFSET] |= (enFlag)<<OTP_MANUFACTOR_FLAG_SEC_EN_BIT_OFFSET;
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


int testReadKdrZeros()
{
    int otpVal = 0;
    int ZeroSize = 0;
    int ZeroMask = 0;

    ZeroSize = ((0x1 << OTP_MANUFACTOR_FLAG_KDR_ZEROS_NUM_BITS)-1);
    ZeroMask = (ZeroSize << OTP_MANUFACTOR_FLAG_KDR_ZEROS_BIT_OFFSET);

    /* NOTE! manufacturer word bits[15:0] acn be read only if LCS is CM or DM */
    READ_AIB(OTP_MANUFACTOR_FLAG_START_WORD_OFFSET*sizeof(uint32_t), otpVal);

    return (otpVal&ZeroMask);
}

unsigned int testReadOtp(unsigned int offset, unsigned int size, uint32_t *buff)
{
        unsigned int i = 0;

        if ((size == 0) || (buff == NULL)) {
        return TEST_INVALID_PARAM_ERR;
    }

        for (i = 0; i < size; i++) {
                buff[i] = READ_OTP(offset+i);
                TEST_PRINTF("buff[%d] 0x%X", i, buff[i]);
        }

    return TEST_OK;
}

