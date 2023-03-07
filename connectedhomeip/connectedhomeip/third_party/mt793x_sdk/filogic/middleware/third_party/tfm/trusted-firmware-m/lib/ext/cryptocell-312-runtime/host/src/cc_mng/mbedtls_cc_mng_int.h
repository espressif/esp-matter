/*
 * Copyright (c) 2001-2020, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _MBEDTLS_CC_MNG_INT_H
#define _MBEDTLS_CC_MNG_INT_H

/************************ Includes ******************************/
#include "cc_hal_plat.h"
#include "cc_regs.h"
#include "dx_nvm.h"
#include "cc_pal_types_plat.h"

/************************ Enums ******************************/
/*! HASH boot key definition. */
typedef enum {
    CC_MNG_HASH_BOOT_KEY_0_128B         = 0,        /*!< 128-bit truncated SHA256 digest of public key 0. */
    CC_MNG_HASH_BOOT_KEY_1_128B     = 1,        /*!< 128-bit truncated SHA256 digest of public key 1. */
    CC_MNG_HASH_BOOT_KEY_256B       = 2,        /*!< 256-bit SHA256 digest of public key. */
    CC_MNG_HASH_BOOT_NOT_USED       = 0xF,
    CC_MNG_HASH_MAX_NUM                 = 0x7FFFFFFF,   /*!\internal use external 128-bit truncated SHA256 digest */
}mbedtls_mng_pubKeyType_t;

/************************ Defines ******************************/
#define CC_MNG_LCS_CM           0x0 /*! Chip manufacturer (CM LCS). */
#define CC_MNG_LCS_DM           0x1 /*! Device manufacturer (DM LCS). */
#define CC_MNG_LCS_SEC_ENABLED  0x5 /*! Security enabled (Secure LCS). */
#define CC_MNG_LCS_RMA          0x7 /*! RMA (RMA LCS). */

#define CC_MNG_INVALID_REG_VAL          0xFFFFFFFF

#define CC_MNG_OEM_RMA_SHFT         30
#define CC_MNG_OEM_RMA_MSK          3

#define CC_MNG_HOST_KPICV_LOCK_BIT_SHFT     1
#define CC_MNG_HOST_KCEICV_LOCK_BIT_SHFT    2
#define CC_MNG_HOST_KCP_LOCK_BIT_SHFT       3
#define CC_MNG_HOST_KCE_LOCK_BIT_SHFT       4

/* Peripheral ID registers values */
#define CC_MNG_PID_0_VAL        0x000000C0UL
#define CC_MNG_PID_1_VAL        0x000000B0UL
#define CC_MNG_PID_2_VAL        0x0000000BUL
#define CC_MNG_PID_3_VAL        0x00000000UL
#define CC_MNG_PID_4_VAL        0x00000004UL
#define CC_MNG_PID_SIZE_WORDS       5

#define CC_MNG_PID_2_1_VAL      0x0000002BUL

/* Component ID registers values */
#define CC_MNG_CID_0_VAL        0x0DUL
#define CC_MNG_CID_1_VAL        0xF0UL
#define CC_MNG_CID_2_VAL        0x05UL
#define CC_MNG_CID_3_VAL        0xB1UL
#define CC_MNG_CID_SIZE_WORDS       4

/*! Definition for all ones word. */
#define CC_MNG_ALL_ONES_VALUE   0xffffffffUL
/*! Definition for number of bits in a 32bit word. */
#define CC_MNG_ALL_ONES_NUM_BITS 32

/*! Defines the maximal hash boot key size in words. */
#define CC_MNG_MAX_HASH_SIZE_IN_WORDS       8
/*! Defines the maximal hash boot key size in bytes. */
#define CC_MNG_MAX_HASH_SIZE_IN_BYTES       (CC_MNG_MAX_HASH_SIZE_IN_WORDS*sizeof(uint32_t))
/*! Defines the maximal full-hash boot key size in words. */
#define CC_MNG_256B_HASH_SIZE_IN_WORDS      CC_MNG_MAX_HASH_SIZE_IN_WORDS
/*! Defines the maximal dual-hash boot key size in words. */
#define CC_MNG_128B_HASH_SIZE_IN_WORDS      CC_MNG_MAX_HASH_SIZE_IN_WORDS/2

/************************ Macros ******************************/
/* calc OTP memory length:
   read RTL OTP address width. The supported sizes are 6 (for 2 Kbits),7,8,9,10,11 (for 64 Kbits).
   convert value parameter to addresses of 32b words */
#define CC_GET_OTP_LENGTH(otpLength)                            \
    do {                                                \
        otpLength = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, OTP_ADDR_WIDTH_DEF));  \
        otpLength = CC_REG_FLD_GET(0, OTP_ADDR_WIDTH_DEF, VALUE, otpLength);            \
        otpLength = (1 << otpLength);                               \
    }while(0)

/* read a word directly from OTP memory */
#define CC_READ_OTP_WORD(otpAddr, otpData)                          \
    do {                                                                                \
        otpData = CC_HAL_READ_REGISTER(CC_OTP_BASE_ADDR + otpAddr);     \
    }while(0)

#define CC_READ_MRAM_WORD(mramOffs, mramData)                               \
    do {                                                                    \
        mramData = (*((volatile uint32_t *)(DX_MRAM_CC + (mramOffs))));     \
    }while(0)

/*! Poll NVM register to assure that the NVM boot is finished (and LCS and the keys are valid). */
#define CC_WAIT_ON_NVM_IDLE_BIT()                                        \
    do {                                             \
        uint32_t regVal;                                 \
        do {                                         \
            regVal = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, NVM_IS_IDLE));     \
            regVal = CC_REG_FLD_GET(0, NVM_IS_IDLE, VALUE, regVal);          \
        }while( !regVal );                               \
    }while(0)

/*! Check Hbk configuration in OTP memory. */
#define CC_IS_HBK_FULL(isHbkFull, error)                               \
    do {                                               \
        error = mbedtls_mng_otpWordRead(CC_OTP_MANUFACTURE_FLAG_OFFSET, &isHbkFull);       \
        isHbkFull = CC_REG_FLD_GET2(0, OTP_MANUFACTURE_FLAG, HBK0_NOT_IN_USE, isHbkFull);  \
    }while(0)

/*! Poll on the crypto busy till it is = 0. */
#define CC_WAIT_ON_CRYPTO_BUSY()                                \
    do {                                            \
        uint32_t regVal=1;                              \
        do {                                        \
            regVal = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, CRYPTO_BUSY));    \
                }while( regVal );                               \
        }while(0)

/* MACRO to count one bits */
#define CC_MNG_COUNT_ONE_BITS(number, BitCount) \
    do \
    { \
          uint32_t tmp_num = number; \
          BitCount = 0; \
          while (tmp_num)   \
          {         \
        tmp_num = tmp_num & (tmp_num - 1); \
        BitCount = BitCount + 1; \
          }         \
    } while (0)

/*! This macro counts the number of zeroes in a 32bits word. */
#define CC_MNG_COUNT_ZEROES(regVal, regZero)                    \
    do {                                    \
        uint32_t val = regVal;                                          \
        val = val - ((val >> 1) & 0x55555555);                          \
        val = (val & 0x33333333) + ((val >> 2) & 0x33333333);           \
        val = ((((val + (val >> 4)) & 0xF0F0F0F) * 0x1010101) >> 24);   \
        regZero += (32 - val);                      \
    }while(0)


/************************ API's ******************************/
/*!
@brief This function reads an OTP word from from 'otpAddress' to 'pOtpWord'.

@return CC_OK on success.
@return A non-zero value from cc_manage.h on failure.
 */
int mbedtls_mng_otpWordRead(uint32_t otpAddress,            /*!< [in]       OTP Address: An Offset in Words in the OTP address space. */
                uint32_t *pOtpWord);            /*!< [in/out]   OTP Word pointer: An address to store the read Word. */

/*!
@brief This function returns the LCS value'.

@return CC_OK on success.
@return A non-zero value from cc_manage.h on failure.
 */
int mbedtls_mng_lcsGet(uint32_t *pLcs);                 /*!< [in/out]   LCS Value: An address to store the LCS Value. */

/*!
@brief This function reads software revocation counter from OTP memory, according to the provided key index.

@return CC_OK on success.
@return A non-zero value from cc_manage.h on failure.
*/
int mbedtls_mng_swVersionGet(
                mbedtls_mng_pubKeyType_t keyIndex,      /*!< [in] Enumeration defining the key hash to retrieve: 128-bit HBK0, 128-bit HBK1, or 256-bit HBK. */
    uint32_t *swVersion                                 /*!< [out] The value of the requested counter as read from OTP memory. */
    );

/*!
@brief This function retrieves the public key hash from OTP memory, according to the provided index.

@return CC_OK on success.
@return A non-zero value from cc_manage.h on failure.
*/
int mbedtls_mng_pubKeyHashGet(
                mbedtls_mng_pubKeyType_t keyIndex,      /*!< [in] Enumeration defining the key hash to retrieve: 128-bit HBK0, 128-bit HBK1, or 256-bit HBK. */
                uint32_t *hashedPubKey,                 /*!< [out] A buffer to contain the public key HASH. */
                uint32_t hashResultSizeWords            /*!< [in] The size of the hash in 32-bit words:
                                                        - Must be 4 for 128-bit hash.
                                                        - Must be 8 for 256bit hash. */
    );

#endif // _MBEDTLS_CC_MNG_INT_H

