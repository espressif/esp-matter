/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef RSA_H
#define RSA_H

/*
 * All the includes that are needed for code using this module to
 * compile correctly should be #included here.
 */

#include "cc_pka_hw_plat_defs.h"
#include "cc_sec_defs.h"
#include "cc_pal_types_plat.h"

#ifdef __cplusplus
extern "C"
{
#endif


/************************ Defines ******************************/

#define CC_BOOT_RSA_VERIFIER_ALG_FAILURE    (CC_SB_RSA_BASE_ERROR + 0x00000001)
#define CC_BOOT_RSA_VERIFIER_CMP_FAILURE    (CC_SB_RSA_BASE_ERROR + 0x00000002)

/* the modulus size ion bits */
#define RSA_EXP_SIZE_WORDS          1


/* PKA max count of SRAM registers: */
#define RSA_HW_PKI_PKA_MAX_COUNT_OF_PHYS_MEM_REGS  PKA_MAX_COUNT_OF_PHYS_MEM_REGS /*32*/
/* PKA required count of SRAM registers: */
#define RSA_PKA_REQUIRED_COUNT_OF_PHYS_MEM_REGS     7

/* maximal size of extended register in "big PKA words" and in 32-bit words:  *
   the size defined according to RSA as more large, and used to define some   *
*  auxiliary buffers sizes                                */
#define RSA_PKA_MAX_REGISTER_SIZE_IN_PKA_WORDS \
        ((SB_CERT_RSA_KEY_SIZE_IN_BITS + RSA_PKA_EXTRA_BITS + RSA_PKA_BIG_WORD_SIZE_IN_BITS - 1)/RSA_PKA_BIG_WORD_SIZE_IN_BITS + 1)
#define RSA_PKA_MAX_REGISTER_SIZE_WORDS  (RSA_PKA_MAX_REGISTER_SIZE_IN_PKA_WORDS*(RSA_PKA_BIG_WORD_SIZE_IN_BITS/CC_BITS_IN_32BIT_WORD))
#define RSA_PKA_MAX_REGISTER_SIZE_BITS   (RSA_PKA_MAX_REGISTER_SIZE_WORDS * CC_BITS_IN_32BIT_WORD)

/* size of Barrett modulus tag NP, used in PKA algorithms */
#define RSA_HW_PKI_PKA_BARRETT_MOD_TAG_SIZE_IN_BITS    (RSA_PKA_BIG_WORD_SIZE_IN_BITS + RSA_PKA_EXTRA_BITS)
#define RSA_HW_PKI_PKA_BARRETT_MOD_TAG_SIZE_IN_BYTES   (CALC_FULL_BYTES(RSA_HW_PKI_PKA_BARRETT_MOD_TAG_SIZE_IN_BITS))
#define RSA_HW_PKI_PKA_BARRETT_MOD_TAG_SIZE_IN_WORDS   (CALC_FULL_32BIT_WORDS(RSA_HW_PKI_PKA_BARRETT_MOD_TAG_SIZE_IN_BITS))

/* size of buffer for Barrett modulus tag NP, used for both 64- and 128-bits PKA */
#define RSA_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS  5 /*maximal of RSA_HW_PKI_PKA_BARRETT_MOD_TAG_SIZE_IN_WORDS*/
#define RSA_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_BYTES  (RSA_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS*CC_32BIT_WORD_SIZE)

/* the public exponent */
#define RSA_PUBL_EXP_SIZE_IN_BITS  17UL
#define RSA_PUBL_EXP_SIZE_IN_BYTES (CALC_FULL_BYTES(RSA_PUBL_EXP_SIZE_IN_BITS))
#ifndef BIG__ENDIAN
#define RSA_EXP_VAL          0x00010001UL
#else
#define RSA_EXP_VAL          0x01000100UL
#endif

/* RSA PSS verify definitions */
#define RSA_HASH_LENGTH  32 /*SHA256*/
#define RSA_PSS_SALT_LENGTH  32
#define RSA_PSS_PAD1_LEN     8

/* RSA Encrypt definitions */
#define RSA_ENCR_RND_PS_SIZE_BYTES   8
#define RSA_ENCR_DATA_IN_SIZE_BYTES  16

#define RSA_PKCS1_VER21   1



/************************ Enums ********************************/

/************************ Typedefs  ****************************/

/************************ Structs  *****************************/

/************************ Public Variables *********************/

/************************ Public Functions *********************/


/*************************************************************************/
/**

   \brief RSA_PSS_Verify implements the Verify algorithm
   as defined in PKCS#1 v2.1

   @param[in] hwBaseAddress - HW base address. Relevant for HW
                              implementation, for SW it is ignored.
   @param[in] dataInHashResult - the DataIn hash result.
   @param[in] pN           - the modulus (2048 bits).
   @param[in] pNp          - the Barrett tag. Relevant for HW
                              implementation, for SW it is ignored.
   @param[in] pSig   - the pointer to the signature to be
                   verified.
        Note: All input arrays are in LE format of bytes and words.

   @return CCError_t - On success the value CC_OK is returned,
           on failure - a value from secureboot_error.h
*/

CCError_t RSA_PSS_Verify( unsigned long    hwBaseAddress,
                           CCHashResult_t  dataInHashResult,
                           uint32_t      *pN,
                           uint32_t      *pNp,
                           uint32_t      *pSig);


/*************************************************************************/
/**
 * @brief The RSA_CalcExponent calculates The following:
 *
 *                   Res = (Base ^ Exp) mod N ( Exp = 0x10001 )
 *
 *        The calculation is done in a secured way using the PIC.
 *
 * @param[in] hwBaseAddress - HW base address. Relevant for HW
 *                      implementation, for SW it is ignored.
 * @Base_ptr[in]         - The pointer to the base buffer.
 * @pN[in]            - The pointer to the modulus buffer (2048 bits).
 * @pNp[in]           - The np vector buffer (160 bits). Relevant for HW
 *                         implementation, for SW it is ignored.
 * @pRes[out]         - The pointer to the buffer that will contain the result.
 *
 *     NOTE:  The SW version of the function uses a temp buffer, pointed by
 *            global pointer gSecBootExpTempBuff; the size of the buffer must be
 *            not less than 8*SB_CERT_RSA_KEY_SIZE_IN_WORDS + 1 word.
 *
 * @return CCError_t - On success CC_OK is returned, on failure a
 *                        value MODULE_* as defined in ...
 */
void RSA_CalcExponent(
        unsigned long hwBaseAddress,
        uint32_t *Base_ptr,
        uint32_t *pN,
        uint32_t *pNp,
        uint32_t *pRes);


/*************************************************************************/
/**
 * @brief The RSA_CalcNp calculates Np value and saves it into pNp:
 *
 * @param[in] hwBaseAddress - The HW base address. Relevant for HW
 *                      implementation, for SW it is ignored.
 * @pN[in]    - The pointer to the modulus buffer.
 * @pNp[out]  - The pointer to Np vector buffer. Its size must be >= 160.
 */
void RSA_CalcNp( unsigned long hwBaseAddress,
                        uint32_t *pN,
                        uint32_t *pNp);


#ifdef __cplusplus
}
#endif

#endif



