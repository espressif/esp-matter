/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _TEST_PROJ_OTP_H__
#define _TEST_PROJ_OTP_H__

#include <stdint.h>
#include "cc_otp_defs.h"
#include "test_proj.h"

/* HW KEYS */
#define TEST_HUK_KEY            0
#define TEST_RTL_KEY        1
#define TEST_KCP_KEY            2
#define TEST_KCE_KEY            3
#define TEST_KPICV_KEY          4
#define TEST_KCEICV_KEY         5

#define TEST_HBK0_KEY           16
#define TEST_HBK1_KEY           17
#define TEST_HBK_FULL_KEY       18

/* OTP memory mapping */
#define ENV_OTP_START_OFFSET        0x2000UL
#define TEST_OTP_SIZE_IN_WORDS      0x2C
#define MAX_OTP_SIZE_IN_WORDS       0x7FF
#define TEST_OTP_LAST_WORD_IN_MASK 0x20

typedef enum {
    TEST_CHIP_STATE_NOT_INITIALIZED = 0,
    TEST_CHIP_STATE_TEST = 1,
    TEST_CHIP_STATE_PRODUCTION = 2,
    TEST_CHIP_STATE_ERROR = 3,
} OtpChipState_t;

typedef enum otpHbkTypes_t {
    TEST_OTP_HBK0_TYPE = 1,    //HBK0
    TEST_OTP_HBK1_TYPE = 2,    //HBK1
    TEST_OTP_HBK_256_TYPE = 4,    //HBK
} OtpHbkTypes_t;

#define TEST_CALC_BUFF_ZEROS(wordBuf, buffWordSize, zeros) {\
    uint32_t i = 0;\
    uint32_t j = 0;\
    uint32_t mask = 0;\
    zeros = 0;\
    for (i = 0; i< buffWordSize; i++) {\
        for (j = 0; j< BITS_IN_32BIT_WORD; j++) {\
            mask = 0x1;\
            if (!(*(wordBuf+i) & (mask << j))) {\
                zeros++;\
            }\
        }\
    }\
}

#define TEST_WRITE_OTP_BY_REG(offset, val)  \
        TEST_WRITE_TEE_CC_REG(CC_OTP_BASE_ADDR +(offset*sizeof(uint32_t)), val)

#define TEST_READ_OTP_BY_REG(offset)   \
        TEST_READ_TEE_CC_REG(CC_OTP_BASE_ADDR+ (offset*sizeof(uint32_t)))

#define TEST_WRITE_OTP_BY_ENV(wordOffset, val) \
        TEST_WRITE_TEE_ENV_REG(ENV_OTP_START_OFFSET + ((wordOffset)*sizeof(uint32_t)), val)

#define TEST_READ_OTP_BY_ENV(wordOffset) \
        TEST_READ_TEE_ENV_REG(ENV_OTP_START_OFFSET + ((wordOffset)*sizeof(uint32_t)))

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
        otpLength = TEST_READ_TEE_CC_REG(CC_REG_OFFSET(HOST_RGF, OTP_ADDR_WIDTH_DEF));  \
        otpLength = CC_REG_FLD_GET(0, OTP_ADDR_WIDTH_DEF, VALUE, otpLength);            \
        otpLength = (1 << otpLength);                               \
    }while(0)

typedef struct TestOtpMask_t {
    uint32_t isPCI;
    uint32_t* mask;
} TestOtpMask_t;

extern uint32_t OTP_CM_VALUES[];
extern uint32_t OTP_DM_HBK_VALUES[];
extern uint32_t gTestOtpMaskV1[];
extern uint32_t gTestOtpMaskV2[];

extern TestOtpMask_t gOtpStatus;

/******************************/
/*   function declaration     */
/*****************************/

/*
 * @brief This function sets teh chip indication status in a buffer
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return void -

 */
void Test_ProjSetChipIndication(uint32_t *otpValues, OtpChipState_t chipIndication);

/*
 * @brief This function writes to OTP using environment register including RTL mask
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return void -
 */
void Test_ProjWriteOtpWord(uint32_t offsetInWords, uint32_t value);

/*
 * @brief This function reads to OTP using environment register including RTL mask
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return void -
 */
uint32_t Test_ProjReadOtpWord(uint32_t offsetInWords);

/*
 * @brief This function sets the key in OTP buffer including number of zeroes and RTL mask
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return void -
 */
uint32_t Test_ProjSetKeyOtpBuff(uint32_t *otpBuf, uint32_t *keyBuff, uint32_t keyType);

/*
 * @brief This function burns OTP buffer with the required chip indication flag
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return void -
 */
uint32_t Test_ProjBurnCmOtp(OtpChipState_t chipIndication);

/*
 * @brief This function burns OTP buffer with the required chip indication flag
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return void -
 */
uint32_t Test_ProjBurnDmOtp(uint32_t *otpBuf, OtpChipState_t chipIndication);

/*
 * @brief This function burns OTP buffer (as is) with the required chip indication flag
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return void -
 */
uint32_t Test_ProjBurnOtpPlain(uint32_t *otpBuf, uint32_t nextLcs, OtpChipState_t chipIndication);

/*
 * @brief This function burns OTP buffer with the required chip indication flag (including key integrity setting)
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return void -

 */
uint32_t Test_ProjBurnOtp(uint32_t *otpBuf, uint32_t nextLcs, OtpChipState_t chipIndication);

/*
 * @brief This function sets the chip indication status in global variable and defines the right mask to use
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return void -
 */
void Test_ProjSetOtpBufState(uint8_t isPCIMode);

/*
 * @brief This function sets the zero count in OTP buffer
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return void -
 */
uint32_t Test_ProjSetZeroBitsOtpBuff(uint32_t *otpBuf, uint32_t keyType, uint32_t value);

/*
 * @brief This function sets HUK buffer in OTP buffer
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return void -
 */
uint32_t Test_ProjSetKdrInOtpBuff(uint32_t *otp, uint8_t *kdrBuff);

/*
 * @brief This function sets HBK buffer in OTP buffer
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return void -
 */
uint32_t Test_ProjSetHbkInOtpBuff(uint32_t *otp,
                                  uint8_t *hbkBuff,
                                  OtpHbkTypes_t type,
                                  uint8_t isFullHbk);

/*
 * @brief This function sets KPICV buffer in OTP buffer
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return void -
 */
uint32_t Test_ProjSetKpicvInOtpBuff(uint32_t *otpBuf, uint8_t *kpicvBuff);

/*
 * @brief This function sets KCEICV buffer in OTP buffer
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return void -
 */
uint32_t Test_ProjSetKceicvOtpBuff(uint32_t *otpBuf, uint8_t *kceicvBuff);

/*
 * @brief This function sets KCP buffer in OTP buffer
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return void -
 */
uint32_t Test_ProjSetKcpOtpBuff(uint32_t *otpBuf, uint8_t *kcpBuff);

/*
 * @brief This function sets KCE buffer in OTP buffer
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return void -
 */
uint32_t Test_ProjSetKceOtpBuff(uint32_t *otpBuf, uint8_t *kceBuff);

/*
 * @brief This function sets default OTP used for llhw tests
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return void -
 */
uint32_t Test_ProjSetNotInUseOtpBuff(uint32_t *otpBuf, uint32_t keyType, uint32_t value);

/*
 * @brief This function sets default OTP used for llhw tests
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return void -
 */
uint32_t Test_ProjGetKeySizeInWordsOtp(uint32_t keyType, uint32_t* keySizeInWords);

/*
 * @brief This function sets default OTP used for llhw tests
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return void -
 */
uint32_t Test_ProjGetKeyOtpBuff(uint32_t *otpBuf,
                                uint32_t keyType,
                                uint32_t *keySizeInWords,
                                uint32_t *keyBuff);

/*
 * @brief This function sets default OTP used for llhw tests
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return void -
 */
uint32_t Test_ProjSetSwVerInOtpBuff(uint32_t *otp, uint32_t offset, uint32_t max_size);

/*
 * @brief This function sets default OTP used for llhw tests
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return void -
 */

#endif //_TEST_PROJ_OTP_H__
