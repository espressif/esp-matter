/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef __TST_HW_ACCESS_H__
#define __TST_HW_ACCESS_H__

#include <stdint.h>

#include "dx_reg_base_host.h"
#include "dx_env.h"

/* rotate 32-bits word by 16 bits */
#define ROT32(x) ( (x) >> 16 | (x) << 16 )

/* inverse the bytes order in a word */
#define REVERSE32(x)  ( ((ROT32((x)) & 0xff00ff00UL) >> 8) | ((ROT32((x)) & 0x00ff00ffUL) << 8) )

#ifndef BIG__ENDIAN
/* define word endiannes*/
#define  SET_WORD_ENDIANESS
#else
#define  SET_WORD_ENDIANESS(val) REVERSE32(val)
#endif

/* LCS */
#define TESTS_LCS_CM        0x0
#define TESTS_LCS_DM        0x1
#define TESTS_LCS_SEC_DISABLED  0x3
#define TESTS_LCS_SEC_ENABLED   0x5
#define TESTS_LCS_RMA       0x7

/* errors */
#define TEST_OK         0
#define TEST_BURN_OTP_ERR   1
#define TEST_BURN_OTP_KDR_ERR   2
#define TEST_BURN_OTP_SCP_ERR   3
#define TEST_BURN_OTP_LCS_ERR   4
#define TEST_INVALID_PARAM_ERR  5

/* OTP memeory mapping */
#define ENV_OTP_START_OFFSET        0x2000UL
#define TEST_OTP_SIZE_IN_WORDS      0x2C
#define MAX_OTP_SIZE_IN_WORDS       0x7FF

typedef enum otpHbkTypes_t{
    TEST_OTP_HBK0_TYPE = 1, //HBK0
    TEST_OTP_HBK1_TYPE = 2, //HBK1
    TEST_OTP_HBK_256_TYPE = 4, //HBK
}OtpHbkTypes_t;

#ifdef BIG__ENDIAN
#define TEST_CONVERT_BYTE_ARR_TO_WORD(inPtr, outWord) {\
    outWord = (*inPtr<<24);\
    outWord |= (*(inPtr+1)<<16);\
    outWord |= (*(inPtr+2)<<8);\
    outWord |= (*(inPtr+3));\
}
#else
#define TEST_CONVERT_BYTE_ARR_TO_WORD(inPtr, outWord) {\
    outWord = (*(inPtr+3))<<24;\
    outWord |= (*(inPtr+2))<<16;\
    outWord |= (*(inPtr+1))<<8;\
    outWord |= (*inPtr);\
}
#endif


#define TEST_CALC_BUFF_ZEROS(wordBuf, buffWordSize, zeros) {\
    int i = 0;\
    int j = 0;\
    int mask = 0;\
    zeros = 0;\
    for (i = 0; i< buffWordSize; i++) {\
        for (j = 0; j<32; j++) {\
            mask = 0x1;\
            if (!(*(wordBuf+i) & (mask << j))) {\
                zeros++;\
            }\
        }\
    }\
}



#define READ_REG(offset) \
        *(volatile uint32_t *)(g_testHwRegBaseAddr + (offset))

#define WRITE_REG(offset, val)  { \
    volatile uint32_t ii1; \
        (*(volatile uint32_t *)(g_testHwRegBaseAddr + (offset))) = (uint32_t)(val); \
        for(ii1=0; ii1<500; ii1++); \
}

#define WRITE_REG_OTP(offset, val)  { \
    volatile uint32_t ii1; \
        (*(volatile uint32_t *)(g_testHwRegBaseAddr + CC_OTP_BASE_ADDR +(offset*sizeof(uint32_t)))) = (uint32_t)(val); \
        for(ii1=0; ii1<500; ii1++); \
}

#define READ_REG_OTP(offset)   \
    *(volatile uint32_t *)(g_testHwRegBaseAddr + CC_OTP_BASE_ADDR+ (offset))

#define WRITE_ENV(offset, val) { \
    volatile uint32_t ii1; \
        (*(volatile uint32_t *)(g_testHwEnvBaseAddr + (offset))) = (uint32_t)(val); \
        for(ii1=0; ii1<500; ii1++);\
}

#define READ_ENV(offset) \
    *(volatile uint32_t *)(g_testHwEnvBaseAddr + (offset))


#define WRITE_OTP(wordOffset, val) { \
    volatile uint32_t ii1; \
        (*(volatile uint32_t *)(g_testHwEnvBaseAddr + ENV_OTP_START_OFFSET+ ((wordOffset)*sizeof(uint32_t)))) = (uint32_t)(val); \
        for(ii1=0; ii1<500; ii1++);\
}

#define READ_OTP(wordOffset) \
    *(volatile uint32_t *)(g_testHwEnvBaseAddr + ENV_OTP_START_OFFSET + ((wordOffset)*sizeof(uint32_t)))

#ifdef DX_PLAT_ZYNQ7000
#define TST_SET_ENV_TO_SECURE()                                 \
        do {                                                        \
        WRITE_ENV( DX_ENV_APB_PPROT_OVERRIDE_REG_OFFSET, 0x9);  \
    }while(0)
#else
#define TST_SET_ENV_TO_SECURE()                     \
    do {                                \
    }while(0)
#endif


/* poll NVM register to be assure that the NVM boot is finished (and LCS and the keys are valid) */
#define WAIT_NVM_IDLE(regVal) \
    do {                                            \
        uint32_t regVal;                                \
        do {                                        \
            regVal = READ_REG(CC_REG_OFFSET(HOST_RGF, NVM_IS_IDLE));            \
            regVal = CC_REG_FLD_GET(0, NVM_IS_IDLE, VALUE, regVal);         \
        }while( !regVal );                              \
    }while(0)

/* turn off DFA  */
#define TURN_DFA_OFF() {\
    uint32_t regVal;                            \
    regVal = READ_REG(CC_REG_OFFSET(HOST_RGF, HOST_AO_LOCK_BITS));          \
    CC_REG_FLD_SET(0, HOST_AO_LOCK_BITS, HOST_FORCE_DFA_ENABLE, regVal, 0); \
    CC_REG_FLD_SET(0, HOST_AO_LOCK_BITS, HOST_DFA_ENABLE_LOCK, regVal, 1);  \
    WRITE_REG(CC_REG_OFFSET(HOST_RGF, HOST_AO_LOCK_BITS)  ,regVal );    \
    WRITE_REG(CC_REG_OFFSET(HOST_RGF, AES_DFA_IS_ON)  ,0 );         \
}


#define SET_OTP_DCU_LOCK(otpBuff, val) {\
    uint32_t ii = 0; \
    for (ii = 0; ii < CC_OTP_DCU_SIZE_IN_WORDS; ii++) { \
        otpBuff[CC_OTP_DCU_OFFSET+ii] = val; \
    } \
}

/* calc OTP memory length:
   read RTL OTP address width. The supported sizes are 6 (for 2 Kbits),7,8,9,10,11 (for 64 Kbits).
   convert value parameter to addresses of 32b words */
#define GET_OTP_LENGTH(otpLength)                           \
    do {                                                \
        otpLength = READ_REG(CC_REG_OFFSET(HOST_RGF, OTP_ADDR_WIDTH_DEF));  \
        otpLength = CC_REG_FLD_GET(0, OTP_ADDR_WIDTH_DEF, VALUE, otpLength);            \
        otpLength = (1 << otpLength);                               \
    }while(0)


/******************************/
/*   function declaration     */
/*****************************/

void performPowerOnReset(void);
unsigned int testBurnOtp(unsigned int  *otp, unsigned int  nextLcs);
void dumpOTP(void);
unsigned int testGetLcs(unsigned int  *lcs);
unsigned int testGetOtpSize(unsigned int * size);
unsigned int testCheckLcs(unsigned int  nextLcs);
unsigned int testCheckLcsAndError(unsigned int  nextLcs);

// hash key
unsigned int testSetHbkInOtpBuff(unsigned int *otp, unsigned char *hbkBuff, OtpHbkTypes_t type);

// huk key
unsigned int testSetKdrInOtpBuff(unsigned int *otp, unsigned char *kdrBuff);

// OEM keys
unsigned int testSetKcpOtpBuff(unsigned int *otp, unsigned int *kpicvBuff);
unsigned int testSetKceOtpBuff(unsigned int *otp, unsigned int *kpicvBuff);

// ICV keys
unsigned int testSetKpicvInOtpBuff(unsigned int *otp, unsigned int *kpicvBuff);
unsigned int testSetKceicvOtpBuff(unsigned int *otp, unsigned int *kpicvBuff);

unsigned int testSetLcsOtpBuff(unsigned int *otpBuf, unsigned int lcsState);

uint32_t initPlatform(void);
void freePlatform(void);

#endif //__TST_HW_ACCESS_IOT_H__
