/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef _CC_RSA_ERROR_H
#define _CC_RSA_ERROR_H

#ifdef CC_IOT
    #if defined(MBEDTLS_CONFIG_FILE)
    #include MBEDTLS_CONFIG_FILE
    #endif
#endif

#if !defined(CC_IOT) || ( defined(CC_IOT) && defined(MBEDTLS_RSA_C))

#include "cc_error.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*! @file
@brief This file contains the definitions of the CryptoCell RSA errors.
@defgroup cc_rsa_error CryptoCell RSA specific errors
@{
@ingroup cc_rsa

*/

/************************ Defines ******************************/

/* PKI RSA module on the CryptoCell layer base address - 0x00F00400 */

/*! CryptoCell RSA module errors */
/*! Illegal modulus size. */
#define CC_RSA_INVALID_MODULUS_SIZE                        (CC_RSA_MODULE_ERROR_BASE + 0x0UL)
/*! Illegal modulus pointer. */
#define CC_RSA_INVALID_MODULUS_POINTER_ERROR               (CC_RSA_MODULE_ERROR_BASE + 0x1UL)
/*! Illegal exponent pointer. */
#define CC_RSA_INVALID_EXPONENT_POINTER_ERROR              (CC_RSA_MODULE_ERROR_BASE + 0x2UL)
/*! Illegal public key structure pointer. */
#define CC_RSA_INVALID_PUB_KEY_STRUCT_POINTER_ERROR        (CC_RSA_MODULE_ERROR_BASE + 0x3UL)
/*! Illegal private key structure pointer. */
#define CC_RSA_INVALID_PRIV_KEY_STRUCT_POINTER_ERROR       (CC_RSA_MODULE_ERROR_BASE + 0x4UL)
/*! Illegal exponent value. */
#define CC_RSA_INVALID_EXPONENT_VAL                        (CC_RSA_MODULE_ERROR_BASE + 0x5UL)
/*! Illegal exponent size. */
#define CC_RSA_INVALID_EXPONENT_SIZE                       (CC_RSA_MODULE_ERROR_BASE + 0x6UL)
/*! Illegal CRT first factor pointer (P_ptr) . */
#define CC_RSA_INVALID_CRT_FIRST_FACTOR_POINTER_ERROR      (CC_RSA_MODULE_ERROR_BASE + 0x7UL)
/*! Illegal CRT second factor pointer (Q_ptr) . */
#define CC_RSA_INVALID_CRT_SECOND_FACTOR_POINTER_ERROR     (CC_RSA_MODULE_ERROR_BASE + 0x8UL)
/*! Illegal CRT first exponent factor pointer (dP_ptr) . */
#define CC_RSA_INVALID_CRT_FIRST_FACTOR_EXP_PTR_ERROR      (CC_RSA_MODULE_ERROR_BASE + 0x9UL)
/*! Illegal CRT second exponent factor pointer (dQ_ptr) . */
#define CC_RSA_INVALID_CRT_SECOND_FACTOR_EXP_PTR_ERROR     (CC_RSA_MODULE_ERROR_BASE + 0xAUL)
/*! Illegal CRT coefficient pointer (qInv_ptr) . */
#define CC_RSA_INVALID_CRT_COEFFICIENT_PTR_ERROR           (CC_RSA_MODULE_ERROR_BASE + 0xBUL)
/*! Illegal CRT first factor size (Psize). */
#define CC_RSA_INVALID_CRT_FIRST_FACTOR_SIZE               (CC_RSA_MODULE_ERROR_BASE + 0xCUL)
/*! Illegal CRT second factor size (Qsize). */
#define CC_RSA_INVALID_CRT_SECOND_FACTOR_SIZE              (CC_RSA_MODULE_ERROR_BASE + 0xDUL)
/*! Illegal CRT first and second factor size (Psize + Qsize). */
#define CC_RSA_INVALID_CRT_FIRST_AND_SECOND_FACTOR_SIZE    (CC_RSA_MODULE_ERROR_BASE + 0xEUL)
/*! Illegal CRT first factor exponent value (dP). */
#define CC_RSA_INVALID_CRT_FIRST_FACTOR_EXPONENT_VAL       (CC_RSA_MODULE_ERROR_BASE + 0xFUL)
/*! Illegal CRT first factor exponent value (dQ). */
#define CC_RSA_INVALID_CRT_SECOND_FACTOR_EXPONENT_VAL      (CC_RSA_MODULE_ERROR_BASE + 0x10UL)
/*! Illegal CRT coefficient value (qInv). */
#define CC_RSA_INVALID_CRT_COEFF_VAL                       (CC_RSA_MODULE_ERROR_BASE + 0x11UL)
/*! Illegal data in. */
#define CC_RSA_DATA_POINTER_INVALID_ERROR                  (CC_RSA_MODULE_ERROR_BASE + 0x12UL)
/*! Illegal message data size. */
#define CC_RSA_INVALID_MESSAGE_DATA_SIZE                   (CC_RSA_MODULE_ERROR_BASE + 0x13UL)
/*! Illegal message value. */
#define CC_RSA_INVALID_MESSAGE_VAL                         (CC_RSA_MODULE_ERROR_BASE + 0x14UL)
/*! Modulus even error. */
#define CC_RSA_MODULUS_EVEN_ERROR                          (CC_RSA_MODULE_ERROR_BASE + 0x15UL)
/*! Illegal context pointer. */
#define CC_RSA_INVALID_USER_CONTEXT_POINTER_ERROR          (CC_RSA_MODULE_ERROR_BASE + 0x16UL)
/*! Illegal hash operation mode. */
#define CC_RSA_HASH_ILLEGAL_OPERATION_MODE_ERROR           (CC_RSA_MODULE_ERROR_BASE + 0x17UL)
/*! Illegal MGF value. */
#define CC_RSA_MGF_ILLEGAL_ARG_ERROR                       (CC_RSA_MODULE_ERROR_BASE + 0x18UL)
/*! Illegal PKCS1 version. */
#define CC_RSA_PKCS1_VER_ARG_ERROR                         (CC_RSA_MODULE_ERROR_BASE + 0x19UL)
/*! Invalid private key. */
#define CC_RSA_PRIV_KEY_VALIDATION_TAG_ERROR               (CC_RSA_MODULE_ERROR_BASE + 0x1AUL)
/*! Invalid public key. */
#define CC_RSA_PUB_KEY_VALIDATION_TAG_ERROR                (CC_RSA_MODULE_ERROR_BASE + 0x1BUL)
/*! Invalid context. */
#define CC_RSA_USER_CONTEXT_VALIDATION_TAG_ERROR           (CC_RSA_MODULE_ERROR_BASE + 0x1CUL)
/*! Illegal output pointer. */
#define CC_RSA_INVALID_OUTPUT_POINTER_ERROR                (CC_RSA_MODULE_ERROR_BASE + 0x1DUL)
/*! Illegal output size pointer. */
#define CC_RSA_INVALID_OUTPUT_SIZE_POINTER_ERROR           (CC_RSA_MODULE_ERROR_BASE + 0x1FUL)
/*! Illegal temporary buffer pointer. */
#define CC_RSA_CONV_TO_CRT_INVALID_TEMP_BUFF_POINTER_ERROR (CC_RSA_MODULE_ERROR_BASE + 0x20UL)
/*! OAEP encode parameter string is too long. */
#define CC_RSA_BASE_OAEP_ENCODE_PARAMETER_STRING_TOO_LONG  (CC_RSA_MODULE_ERROR_BASE + 0x22UL)
/*! OAEP decode parameter string is too long. */
#define CC_RSA_BASE_OAEP_DECODE_PARAMETER_STRING_TOO_LONG  (CC_RSA_MODULE_ERROR_BASE + 0x23UL)
/*! OAEP encode message is too long. */
#define CC_RSA_BASE_OAEP_ENCODE_MESSAGE_TOO_LONG           (CC_RSA_MODULE_ERROR_BASE + 0x24UL)
/*! OAEP decode message is too long. */
#define CC_RSA_BASE_OAEP_DECODE_MESSAGE_TOO_LONG           (CC_RSA_MODULE_ERROR_BASE + 0x25UL)
/*! Illegal key generation data struct pointer. */
#define CC_RSA_KEY_GEN_DATA_STRUCT_POINTER_INVALID         (CC_RSA_MODULE_ERROR_BASE + 0x26UL)
/*! Illegal PRIM data struct pointer. */
#define CC_RSA_PRIM_DATA_STRUCT_POINTER_INVALID            (CC_RSA_MODULE_ERROR_BASE + 0x27UL)
/*! Illegal message buffer size. */
#define CC_RSA_INVALID_MESSAGE_BUFFER_SIZE                 (CC_RSA_MODULE_ERROR_BASE + 0x28UL)
/*! Illegal signature buffer size. */
#define CC_RSA_INVALID_SIGNATURE_BUFFER_SIZE               (CC_RSA_MODULE_ERROR_BASE + 0x29UL)
/*! Illegal modulus size pointer. */
#define CC_RSA_INVALID_MOD_BUFFER_SIZE_POINTER             (CC_RSA_MODULE_ERROR_BASE + 0x2AUL)
/*! Illegal exponent size pointer. */
#define CC_RSA_INVALID_EXP_BUFFER_SIZE_POINTER             (CC_RSA_MODULE_ERROR_BASE + 0x2BUL)
/*! Illegal signature pointer. */
#define CC_RSA_INVALID_SIGNATURE_BUFFER_POINTER            (CC_RSA_MODULE_ERROR_BASE + 0x2CUL)
/*! Wrong private key type. */
#define CC_RSA_WRONG_PRIVATE_KEY_TYPE                      (CC_RSA_MODULE_ERROR_BASE + 0x2DUL)
/*! Illegal CRT first factor size pointer (Psize) . */
#define CC_RSA_INVALID_CRT_FIRST_FACTOR_SIZE_POINTER_ERROR (CC_RSA_MODULE_ERROR_BASE + 0x2EUL)
/*! Illegal CRT second factor size pointer (Qsize) . */
#define CC_RSA_INVALID_CRT_SECOND_FACTOR_SIZE_POINTER_ERROR (CC_RSA_MODULE_ERROR_BASE + 0x2FUL)
/*! Illegal CRT first factor exponent size pointer (dPsize) . */
#define CC_RSA_INVALID_CRT_FIRST_FACTOR_EXP_SIZE_PTR_ERROR (CC_RSA_MODULE_ERROR_BASE + 0x30UL)
/*! Illegal CRT second factor exponent size pointer (dQsize) . */
#define CC_RSA_INVALID_CRT_SECOND_FACTOR_EXP_SIZE_PTR_ERROR (CC_RSA_MODULE_ERROR_BASE + 0x31UL)
/*! Illegal CRT coefficient size pointer (qInvsize) . */
#define CC_RSA_INVALID_CRT_COEFFICIENT_SIZE_PTR_ERROR        (CC_RSA_MODULE_ERROR_BASE + 0x32UL)
/*! Illegal CRT first factor size (Psize) . */
#define CC_RSA_INVALID_CRT_FIRST_FACTOR_SIZE_ERROR         (CC_RSA_MODULE_ERROR_BASE + 0x33UL)
/*! Illegal CRT second factor size (Qsize) . */
#define CC_RSA_INVALID_CRT_SECOND_FACTOR_SIZE_ERROR        (CC_RSA_MODULE_ERROR_BASE + 0x34UL)
/*! Illegal CRT first factor exponent size (dPsize) . */
#define CC_RSA_INVALID_CRT_FIRST_FACTOR_EXP_SIZE_ERROR     (CC_RSA_MODULE_ERROR_BASE + 0x35UL)
/*! Illegal CRT second factor exponent size (dQsize) . */
#define CC_RSA_INVALID_CRT_SECOND_FACTOR_EXP_SIZE_ERROR    (CC_RSA_MODULE_ERROR_BASE + 0x36UL)
/*! Illegal CRT coefficient size (qInvsize) . */
#define CC_RSA_INVALID_CRT_COEFFICIENT_SIZE_ERROR            (CC_RSA_MODULE_ERROR_BASE + 0x37UL)
/*! Key generation conditional test failed. */
#define CC_RSA_KEY_GEN_CONDITIONAL_TEST_FAIL_ERROR       (CC_RSA_MODULE_ERROR_BASE + 0x38UL)
/*! Random generation in range failed. */
#define CC_RSA_CAN_NOT_GENERATE_RAND_IN_RANGE            (CC_RSA_MODULE_ERROR_BASE + 0x39UL)
/*! Illegal CRT parameter size. */
#define CC_RSA_INVALID_CRT_PARAMETR_SIZE_ERROR             (CC_RSA_MODULE_ERROR_BASE + 0x3AUL)
/*! Illegal modulus. */
#define CC_RSA_INVALID_MODULUS_ERROR                       (CC_RSA_MODULE_ERROR_BASE + 0x40UL)
/*! Illegal pointer. */
#define CC_RSA_INVALID_PTR_ERROR                           (CC_RSA_MODULE_ERROR_BASE + 0x41UL)
/*! Illegal decryption mode. */
#define CC_RSA_INVALID_DECRYPRION_MODE_ERROR               (CC_RSA_MODULE_ERROR_BASE + 0x42UL)
/*! Illegal generated private key. */
#define CC_RSA_GENERATED_PRIV_KEY_IS_TOO_LOW               (CC_RSA_MODULE_ERROR_BASE + 0x43UL)
/*! Key generation error. */
#define CC_RSA_KEY_GENERATION_FAILURE_ERROR                (CC_RSA_MODULE_ERROR_BASE + 0x44UL)
#define CC_RSA_INTERNAL_ERROR                              (CC_RSA_MODULE_ERROR_BASE + 0x45UL)


/****************************************************************************************
 * PKCS#1 VERSION 1.5 ERRORS
 ****************************************************************************************/
/*! BER encoding passed. */
#define CC_RSA_BER_ENCODING_OK                            CC_OK
/*! Error in BER parsing. */
#define CC_RSA_ERROR_BER_PARSING                         (CC_RSA_MODULE_ERROR_BASE+0x51UL)
/*! Error in PKCS15 message. */
#define CC_RSA_ENCODE_15_MSG_OUT_OF_RANGE                (CC_RSA_MODULE_ERROR_BASE+0x52UL)
/*! Error in PKCS15 PS. */
#define CC_RSA_ENCODE_15_PS_TOO_SHORT                    (CC_RSA_MODULE_ERROR_BASE+0x53UL)
/*! PKCS15 block type is not supported. */
#define CC_RSA_PKCS1_15_BLOCK_TYPE_NOT_SUPPORTED         (CC_RSA_MODULE_ERROR_BASE+0x54UL)
/*! Error in PKCS15 decrypted block parsing. */
#define CC_RSA_15_ERROR_IN_DECRYPTED_BLOCK_PARSING       (CC_RSA_MODULE_ERROR_BASE+0x55UL)
/*! Error in random operation. */
#define CC_RSA_ERROR_IN_RANDOM_OPERATION_FOR_ENCODE      (CC_RSA_MODULE_ERROR_BASE+0x56UL)
/*! PKCS15 verification failed. */
#define CC_RSA_ERROR_VER15_INCONSISTENT_VERIFY           (CC_RSA_MODULE_ERROR_BASE+0x57UL)
/*! Illegal message size (in no hash operation case). */
#define CC_RSA_INVALID_MESSAGE_DATA_SIZE_IN_NO_HASH_CASE (CC_RSA_MODULE_ERROR_BASE+0x58UL)
/*! Illegal message size. */
#define CC_RSA_INVALID_MESSAGE_DATA_SIZE_IN_SSL_CASE       (CC_RSA_MODULE_ERROR_BASE+0x59UL)
/*! PKCS#1 Ver 1.5 verify hash input inconsistent with hash mode derived from signature. */
#define CC_RSA_PKCS15_VERIFY_BER_ENCODING_HASH_TYPE    (CC_RSA_MODULE_ERROR_BASE+0x60UL)  /*!< \internal PKCS#1 Ver 1.5 verify hash input inconsistent with hash mode derived from signature*/
/*! Illegal DER hash mode */
#define CC_RSA_GET_DER_HASH_MODE_ILLEGAL                 (CC_RSA_MODULE_ERROR_BASE+0x61UL)

/****************************************************************************************
 * PKCS#1 VERSION 2.1 ERRORS
 ****************************************************************************************/
 /*! Illegal salt length. */
#define CC_RSA_PSS_ENCODING_MODULUS_HASH_SALT_LENGTHS_ERROR  (CC_RSA_MODULE_ERROR_BASE+0x80UL)
/*! Illegal MGF mask. */
#define CC_RSA_BASE_MGF_MASK_TOO_LONG                   (CC_RSA_MODULE_ERROR_BASE+0x81UL)
/*! PSS verification failed. */
#define CC_RSA_ERROR_PSS_INCONSISTENT_VERIFY            (CC_RSA_MODULE_ERROR_BASE+0x82UL)
/*! OAEP message too long. */
#define CC_RSA_OAEP_VER21_MESSAGE_TOO_LONG              (CC_RSA_MODULE_ERROR_BASE+0x83UL)
/*! OAEP error in decrypted block parsing. */
#define CC_RSA_ERROR_IN_DECRYPTED_BLOCK_PARSING         (CC_RSA_MODULE_ERROR_BASE+0x84UL)
/*! OAEP decoding error. */
#define CC_RSA_OAEP_DECODE_ERROR                        (CC_RSA_MODULE_ERROR_BASE+0x85UL)
/*! Error in decrypted data size. */
#define CC_RSA_15_ERROR_IN_DECRYPTED_DATA_SIZE          (CC_RSA_MODULE_ERROR_BASE+0x86UL)
/*! Error in decrypted data. */
#define CC_RSA_15_ERROR_IN_DECRYPTED_DATA               (CC_RSA_MODULE_ERROR_BASE+0x87UL)
/*! Illegal L pointer. */
#define CC_RSA_OAEP_L_POINTER_ERROR                     (CC_RSA_MODULE_ERROR_BASE+0x88UL)
/*! Illegal output size. */
#define CC_RSA_DECRYPT_INVALID_OUTPUT_SIZE              (CC_RSA_MODULE_ERROR_BASE+0x89UL)
/*! Illegal output size pointer. */
#define CC_RSA_DECRYPT_OUTPUT_SIZE_POINTER_ERROR        (CC_RSA_MODULE_ERROR_BASE+0x8AUL)
/*! Illegal parameters. */
#define CC_RSA_ILLEGAL_PARAMS_ACCORDING_TO_PRIV_ERROR   (CC_RSA_MODULE_ERROR_BASE + 0x93UL)
/*! RSA is not supported. */
#define CC_RSA_IS_NOT_SUPPORTED                         (CC_RSA_MODULE_ERROR_BASE+0xFFUL)


/************************ Enums ********************************/


/************************ Typedefs  ****************************/


/************************ Structs  ******************************/


/************************ Public Variables **********************/


/************************ Public Functions **********************/

#ifdef __cplusplus
}
#endif
/**
@}
 */
#endif /* !defined(CC_IOT) || ( defined(CC_IOT) && defined(MBEDTLS_RSA_C)) */
#endif /* _CC_RSA_ERROR_H */

