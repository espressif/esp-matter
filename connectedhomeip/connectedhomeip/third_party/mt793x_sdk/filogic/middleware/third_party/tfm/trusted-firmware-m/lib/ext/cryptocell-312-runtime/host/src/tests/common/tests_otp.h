/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef __TST_OTP_H__
#define __TST_OTP_H__

#include <stdint.h>


/* TBD later should these definitions should be taken from shared cc_*.h */

/* LCS */
#define TESTS_LCS_CM        0x0
#define TESTS_LCS_DM        0x1
#define TESTS_LCS_SEC_DISABLED  0x3
#define TESTS_LCS_SEC_ENABLED   0x5
#define TESTS_LCS_RMA       0x7

/* HW KEYS */
#define TEST_HUK_KEY            0
#define TEST_RTL_KEY            1
#define TEST_KCP_KEY            2
#define TEST_KCE_KEY            3
#define TEST_KPICV_KEY          4
#define TEST_KCEICV_KEY         5

#define TEST_HBK0_KEY           16
#define TEST_HBK1_KEY           17
#define TEST_HBK_FULL_KEY       18

typedef enum otpHbkTypes_t{
    TEST_OTP_HBK0_TYPE = 1, //HBK0
    TEST_OTP_HBK1_TYPE = 2, //HBK1
    TEST_OTP_HBK_256_TYPE = 4, //HBK
}OtpHbkTypes_t;


/******************************/
/*   function declaration     */
/*****************************/

/*
unsigned int testBurnOtp(unsigned int  *otp, unsigned int  nextLcs);
unsigned int testGetLcs(unsigned int  *lcs);
unsigned int testGetOtpSize(unsigned int * size);
unsigned int testCheckLcs(unsigned int  nextLcs);
*/
// hash key

// tbd - move functionality to general functions
/**
 * This function sets HBK to otp buffer and update "NotInUse" and "ZerosBits"
 *
 */uint32_t Test_SetHbkInOtpBuff(
         uint32_t *otp,         //!< [out] otp image
         uint8_t *hbkBuff,      //!< [in]  hbk value
         OtpHbkTypes_t type     //!< [in]  hbk type
         );

 /**
  * This function pads with '0's the remaining SW version field after the first '0'
  *
  */uint32_t Test_SetSwVerInOtpBuff(
         uint32_t *otp,         //!< [out] otp image
         uint32_t offset,       //!< [in]  offset within the otp
         uint32_t max_size      //!< [in]  max field size in words
         );

/**
 * This function sets hw key itself to otp buffer and update "NotInUse" and "ZerosBits"
 *
 */
uint32_t Test_SetKeyOtpBuff(
        uint32_t *otpBuf,       //!< [out] otp image
        uint32_t *keyBuff,      //!< [in]  key value
        uint32_t keyType        //!< [in]  hw key type
        );

/**
 * This function gets hw key and its size from otp buffer to user buffer
 *
 */
uint32_t Test_GetKeyOtpBuff(
        uint32_t *otpBuf,         //!< [in] otp image
        uint32_t keyType,         //!< [in] key type
        uint32_t *keySizeInWords, //!< [in] size in words
        uint32_t *keyBuff         //!< [out] hw key value
        );

/**
 * This function sets "notInUse" bit for appropriate hw key
 *
 */
uint32_t Test_SetNotInUseOtpBuff(
        uint32_t *otpBuf,       //!< [out] otp image
        uint32_t keyType,       //!< [in]  hw key type
        uint32_t value          //!< [in] "in use" bit value to set
        );

/**
 * This function sets "ZeroBits" bits for appropriate hw key
 *
 */
uint32_t Test_SetZeroBitsOtpBuff(
        uint32_t *otpBuf,       //!< [out] otp image
        uint32_t keyType,       //!< [in]  hw key type
        uint32_t value          //!< [in]  value
        );

/**
 * This function sets manufacture and oem flags accordingly to reuired LCS state
 *
 */
uint32_t Test_SetLcsOtpBuff(
        uint32_t *otpBuf,       //!< [out] otp image
        uint32_t lcsState       //!< [in] lifecycle state
        );

/**
 * This function sets entire OTP image with set of keys and lcs as parameter
 *
 */
void Test_SetDefaultOtp(
        uint32_t* otpValues,     //!< [out] otp image
        uint32_t otpSize,        //!< [in] otp size
        uint32_t lcsState        //!< [in] lcs
        );

/**
 * This function burns otp image through "back door", performs reset, and check if system booted to required lcs
 *
 */
uint32_t Test_GetKeySizeInWordsOtp(
        uint32_t  keyType,         //!< [in] key Type
        uint32_t* keySizeInWords   //!< [out] key Size in words
        );

/**
 * This function burns otp image through "back door", performs reset, and check if system booted to required lcs
 *
 */
uint32_t Test_BurnOtp(
        uint32_t  *otp,         //!< [in] otp image
        uint32_t  nextLcs       //!< [in] lcs that system should boot to
        );



#endif //__TST_OTP_H__
