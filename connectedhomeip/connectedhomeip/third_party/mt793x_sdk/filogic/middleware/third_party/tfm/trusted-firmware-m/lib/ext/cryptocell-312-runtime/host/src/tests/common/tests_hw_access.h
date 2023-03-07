/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef __TST_HW_ACCESS_H__
#define __TST_HW_ACCESS_H__

#include <stdint.h>

extern unsigned long    g_testHwRegBaseAddr;
extern unsigned long    g_testHwReeRegBaseAddr;
extern unsigned long    g_testHwEnvBaseAddr;

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


#define SESSION_KEY_LEN_IN_BYTES 16

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

#define ENV_OTP_START_OFFSET        0x800UL
#define TEST_OTP_SIZE_IN_WORDS      0x25

#define OTP_KRD_START_WORD_OFFSET    0x00
#define OTP_KRD_SIZE_IN_WORDS        0x08
#define OTP_KRD_SIZE_IN_BYTES        (OTP_KRD_SIZE_IN_WORDS * sizeof(uint32_t))
#define OTP_SCP_START_WORD_OFFSET    0x08
#define OTP_SCP_SIZE_IN_WORDS        0x02

#define OTP_MANUFACTOR_FLAG_START_WORD_OFFSET 0x0A
#define OTP_MANUFACTOR_FLAG_SIZE_IN_WORDS     0x01
#define OTP_MANUFACTOR_FLAG_KDR_ZEROS_BIT_OFFSET 0x00
#define OTP_MANUFACTOR_FLAG_KDR_ZEROS_NUM_BITS 0x08
#define OTP_MANUFACTOR_FLAG_SCP_ZEROS_BIT_OFFSET 0x08
#define OTP_MANUFACTOR_FLAG_SCP_ZEROS_NUM_BITS 0x07
#define OTP_MANUFACTOR_FLAG_SEC_EN_BIT_OFFSET 0x10
#define OTP_MANUFACTOR_FLAG_SEC_EN_NUM_BITS   0x04
#define OTP_MANUFACTOR_FLAG_SEC_ENABLE 0x3
#define OTP_MANUFACTOR_FLAG_SEC_DISABLE 0xC


#define OTP_DM_DEF_START_WORD_OFFSET 0x0B
#define OTP_DM_DEF_SIZE_IN_WORDS     0x02
#define OTP_DM_DEF_HBK0_ZEROS_BIT_OFFSET 0x00
#define OTP_DM_DEF_HBK0_ZEROS_NUM_BITS 0x08
#define OTP_DM_DEF_HBK1_ZEROS_BIT_OFFSET 0x08
#define OTP_DM_DEF_HBK1_ZEROS_NUM_BITS 0x08

#define OTP_KCE_START_WORD_OFFSET    0x0C
#define OTP_KCE_SIZE_IN_WORDS        0x04
#define OTP_HBK0_START_WORD_OFFSET   0x10
#define OTP_HBK0_SIZE_IN_WORDS       0x04
#define OTP_HBK1_START_WORD_OFFSET   0x14
#define OTP_HBK1_SIZE_IN_WORDS       0x04
#define OTP_SB_VER_START_WORD_OFFSET 0x18
#define OTP_SB_VER_SIZE_IN_WORDS     0x01


#define AIB_ADDR_REG_READ_ACCESS_BIT_SHIFT    0x10UL
#define AIB_ADDR_REG_WRITE_ACCESS_BIT_SHIFT   0x11UL

#define OTP_BASE_ADDR                0x00
#define OTP_WRITE_ADDR               (0x1 << AIB_ADDR_REG_WRITE_ACCESS_BIT_SHIFT)
#define OTP_READ_ADDR                (0x1 << AIB_ADDR_REG_READ_ACCESS_BIT_SHIFT)

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


#define READ_REE_REG(offset) \
        (*(volatile uint32_t *)(g_testHwReeRegBaseAddr + (offset)));

#define WRITE_REE_REG(offset, val)   { \
    volatile uint32_t ii1; \
        (*(volatile uint32_t *)(g_testHwReeRegBaseAddr + (offset))) = (uint32_t)(val); \
        for(ii1=0; ii1<500; ii1++); \
}


#define GET_LCS(val) {\
    do {\
            val = READ_REG(DX_LCS_IS_VALID_REG_OFFSET);\
    }while( !(val & 0x1));\
    val = READ_REG(DX_LCS_REG_REG_OFFSET);\
    val &= 0xFF;\
    }

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
    (*(volatile uint32_t *)(g_testHwEnvBaseAddr + ENV_OTP_START_OFFSET+ ((wordOffset)*sizeof(uint32_t))))



/* Poll on the AIB bit */
#define WAIT_ON_AIB_PROG_COMP_BIT() \
do {\
    int regVal;\
    do {\
        regVal = READ_REG(DX_AIB_FUSE_PROG_COMPLETED_REG_OFFSET);\
    }while( !(regVal & 0x1 ));\
}while(0)

/* Poll on the AIB acknowledge bit */
#define WAIT_ON_AIB_ACK_BIT() \
do {\
    int regVal;\
    do {\
        regVal = READ_REG(DX_AIB_FUSE_ACK_REG_OFFSET);\
    }while( !(regVal & 0x1));\
}while(0)

#define WRITE_AIB(addr, val) { \
            WRITE_REG(DX_HOST_AIB_WDATA_REG_REG_OFFSET, val);\
            WRITE_REG(DX_HOST_AIB_ADDR_REG_REG_OFFSET, (OTP_BASE_ADDR|OTP_WRITE_ADDR)+addr);\
            WAIT_ON_AIB_ACK_BIT();\
            WAIT_ON_AIB_PROG_COMP_BIT();\
}

#define READ_AIB(addr, val) { \
        WRITE_REG(DX_HOST_AIB_ADDR_REG_REG_OFFSET, (OTP_BASE_ADDR|OTP_READ_ADDR)+addr); \
        WAIT_ON_AIB_ACK_BIT(); \
        val = READ_REG(DX_HOST_AIB_RDATA_REG_REG_OFFSET); \
}

typedef enum otpHbkTypes_t{
    TEST_OTP_HBK0_TYPE = 1, //HBK0
    TEST_OTP_HBK1_TYPE = 2, //HBK1
    TEST_OTP_HBK_256_TYPE = 4, //HBK
}OtpHbkTypes_t;


/******************************/
/*   function declaration     */
/*****************************/

unsigned int testBurnOtp(unsigned int  *otp, unsigned int  nextLcs);
unsigned int testClearOtp(void);
int testReadKdrZeros(void);

void performPowerOnReset(void);
void performColdReset(void);
void performWarmReset(void);
unsigned int testCheckLcs(unsigned int  nextLcs);

unsigned int testSetKdrInOtpBuff(unsigned int *otp, unsigned char *kdrBuff);
unsigned int testSetScpInOtpBuff(unsigned int *otp, unsigned char *scpBuff);
unsigned int testSetHbkInOtpBuff(unsigned int *otp, unsigned char *hbkBuff, OtpHbkTypes_t type);
unsigned int testSetSecureInOtpBuff(unsigned int *otp, unsigned char enFlag);
unsigned int testReadOtp(unsigned int offset, unsigned int size, uint32_t *buff);

#endif //__TST_HW_ACCESS_H__
