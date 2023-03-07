/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */



/*!
 @addtogroup cc_error
 @{
 */

/*!
 @file
 @brief This file defines the error return code types and the numbering spaces
        for each module of the layers listed.
*/


#ifndef _CC_ERROR_H
#define _CC_ERROR_H

#include "cc_pal_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*! The definitions of the error number-space used for the different modules */

/* ........... Error base numeric mapping definitions ................... */
/* ----------------------------------------------------------------------- */

 /*! The error base number for CryptoCell. */
#define CC_ERROR_BASE          0x00F00000UL

/*! The error range number assigned for each layer. */
#define CC_ERROR_LAYER_RANGE   0x00010000UL

/*! The error range number assigned to each module on its specified layer. */
#define CC_ERROR_MODULE_RANGE  0x00000100UL

/* Defines the layer index for the error mapping. */
/*! The CryptoCell error-layer index. */
#define CC_LAYER_ERROR_IDX     0x00UL
/*! The error-layer index for low-level functions. */
#define LLF_LAYER_ERROR_IDX      0x01UL
/*! The generic error-layer index. */
#define GENERIC_ERROR_IDX        0x05UL

/* Defines the module index for error mapping */
/*! The AES error index.*/
#define AES_ERROR_IDX            0x00UL
/*! The DES error index.*/
#define DES_ERROR_IDX            0x01UL
/*! The hash error index.*/
#define HASH_ERROR_IDX           0x02UL
/*! The HMAC error index.*/
#define HMAC_ERROR_IDX           0x03UL
/*! The RSA error index.*/
#define RSA_ERROR_IDX            0x04UL
/*! The DH error index.*/
#define DH_ERROR_IDX             0x05UL
/*! The ECPKI error index.*/
#define ECPKI_ERROR_IDX          0x08UL
/*! The RND error index.*/
#define RND_ERROR_IDX            0x0CUL
/*! The Common error index.*/
#define COMMON_ERROR_IDX         0x0DUL
/*! The KDF error index.*/
#define KDF_ERROR_IDX            0x11UL
/*! The HKDF error index.*/
#define HKDF_ERROR_IDX           0x12UL
/*! The AESCCM error index.*/
#define AESCCM_ERROR_IDX         0x15UL
/*! The FIPS error index.*/
#define FIPS_ERROR_IDX           0x17UL
/*! The PKA error index.*/

#define PKA_MODULE_ERROR_IDX     0x21UL
/*! The ChaCha error index.*/
#define CHACHA_ERROR_IDX         0x22UL
/*! The EC Montgomery and Edwards error index.*/
#define EC_MONT_EDW_ERROR_IDX    0x23UL
/*! The ChaCha-POLY error index.*/
#define CHACHA_POLY_ERROR_IDX    0x24UL
/*! The POLY error index.*/
#define POLY_ERROR_IDX           0x25UL
/*! The SRP error index.*/
#define SRP_ERROR_IDX            0x26UL


/*! The AESGCM error index.*/
#define AESGCM_ERROR_IDX         0x27UL

/*! The AES key-wrap error index.*/
#define AES_KEYWRAP_ERROR_IDX    0x28UL

/*! Management error index.*/
#define MNG_ERROR_IDX            0x29UL

/*! Production error index.*/
#define PROD_ERROR_IDX           0x2AUL

/*! The FFCDH error index. */
#define FFCDH_ERROR_IDX          0x2BUL
/*! The FFC domain error index. */
#define FFC_DOMAIN_ERROR_IDX     0x2CUL

/*! Do not change! Error definition, reserved for Secure Boot ECDSA */
#define SB_ECC_ERROR_IDX_        0x2DUL
/*! External DMA error index. */
#define EXT_DMA_ERROR_IDX        0x2EUL



/* .......... defining the error spaces for each module on each layer ........... */
/* ------------------------------------------------------------------------------ */

/*! The error base address of the AES module - 0x00F00000. */
#define CC_AES_MODULE_ERROR_BASE  (CC_ERROR_BASE + \
                                     (CC_ERROR_LAYER_RANGE * CC_LAYER_ERROR_IDX) + \
                                     (CC_ERROR_MODULE_RANGE * AES_ERROR_IDX ) )

/*! The error base address of the DES module - 0x00F00100. */
#define CC_DES_MODULE_ERROR_BASE  (CC_ERROR_BASE + \
                                     (CC_ERROR_LAYER_RANGE * CC_LAYER_ERROR_IDX) + \
                                     (CC_ERROR_MODULE_RANGE * DES_ERROR_IDX ) )

/*! The error base address of the hash module - 0x00F00200. */
#define CC_HASH_MODULE_ERROR_BASE (CC_ERROR_BASE + \
                                     (CC_ERROR_LAYER_RANGE * CC_LAYER_ERROR_IDX) + \
                                     (CC_ERROR_MODULE_RANGE * HASH_ERROR_IDX ) )

/*! The error base address of the HMAC module - 0x00F00300. */
#define CC_HMAC_MODULE_ERROR_BASE (CC_ERROR_BASE + \
                                     (CC_ERROR_LAYER_RANGE * CC_LAYER_ERROR_IDX) + \
                                     (CC_ERROR_MODULE_RANGE * HMAC_ERROR_IDX ) )

/*! The error base address of the RSA module - 0x00F00400. */
#define CC_RSA_MODULE_ERROR_BASE (CC_ERROR_BASE + \
                                   (CC_ERROR_LAYER_RANGE * CC_LAYER_ERROR_IDX) + \
                                   (CC_ERROR_MODULE_RANGE * RSA_ERROR_IDX ) )

/*! The error base address of the DH module - 0x00F00500. */
#define CC_DH_MODULE_ERROR_BASE  (CC_ERROR_BASE + \
                                   (CC_ERROR_LAYER_RANGE * CC_LAYER_ERROR_IDX) + \
                                   (CC_ERROR_MODULE_RANGE * DH_ERROR_IDX ) )

/*! The error base address of the ECPKI module - 0x00F00800. */
#define CC_ECPKI_MODULE_ERROR_BASE (CC_ERROR_BASE + \
                                     (CC_ERROR_LAYER_RANGE * CC_LAYER_ERROR_IDX) + \
                                     (CC_ERROR_MODULE_RANGE * ECPKI_ERROR_IDX ) )

/*! The error base address of the low-level ECPKI module -  0x00F10800. */
#define LLF_ECPKI_MODULE_ERROR_BASE  (CC_ERROR_BASE + \
                                     (CC_ERROR_LAYER_RANGE * LLF_LAYER_ERROR_IDX) + \
                                     (CC_ERROR_MODULE_RANGE * ECPKI_ERROR_IDX ) )

/*! The error base address of the RND module - 0x00F00C00. */
#define CC_RND_MODULE_ERROR_BASE   (CC_ERROR_BASE + \
                                     (CC_ERROR_LAYER_RANGE * CC_LAYER_ERROR_IDX) + \
                                     (CC_ERROR_MODULE_RANGE * RND_ERROR_IDX ) )

/*! The error base address of the low-level RND module -  0x00F10C00. */
#define LLF_RND_MODULE_ERROR_BASE    (CC_ERROR_BASE + \
                                     (CC_ERROR_LAYER_RANGE * LLF_LAYER_ERROR_IDX) + \
                                     (CC_ERROR_MODULE_RANGE * RND_ERROR_IDX ) )

/*! The error base address of the common module - 0x00F00D00. */
#define CC_COMMON_MODULE_ERROR_BASE (CC_ERROR_BASE + \
                                     (CC_ERROR_LAYER_RANGE * CC_LAYER_ERROR_IDX) + \
                                     (CC_ERROR_MODULE_RANGE * COMMON_ERROR_IDX ) )

/*! The error base address of the KDF module - 0x00F01100. */
#define CC_KDF_MODULE_ERROR_BASE (CC_ERROR_BASE + \
                                  (CC_ERROR_LAYER_RANGE * CC_LAYER_ERROR_IDX) + \
                                  (CC_ERROR_MODULE_RANGE * KDF_ERROR_IDX ) )

/*! The error base address of the HKDF module - 0x00F01100. */
#define CC_HKDF_MODULE_ERROR_BASE (CC_ERROR_BASE + \
                                  (CC_ERROR_LAYER_RANGE * CC_LAYER_ERROR_IDX) + \
                                  (CC_ERROR_MODULE_RANGE * HKDF_ERROR_IDX ) )

/*! The error base address of the AESCCM module - 0x00F01500. */
#define CC_AESCCM_MODULE_ERROR_BASE  (CC_ERROR_BASE + \
                                       (CC_ERROR_LAYER_RANGE * CC_LAYER_ERROR_IDX) + \
                                       (CC_ERROR_MODULE_RANGE * AESCCM_ERROR_IDX ) )

/*! The error base address of the FIPS module - 0x00F01700. */
#define CC_FIPS_MODULE_ERROR_BASE  (CC_ERROR_BASE + \
                                       (CC_ERROR_LAYER_RANGE * CC_LAYER_ERROR_IDX) + \
                                       (CC_ERROR_MODULE_RANGE * FIPS_ERROR_IDX ) )

/*! The error base address of the PKA module - 0x00F02100. */
#define PKA_MODULE_ERROR_BASE             (CC_ERROR_BASE + \
                                           (CC_ERROR_LAYER_RANGE * CC_LAYER_ERROR_IDX) + \
                                           (CC_ERROR_MODULE_RANGE * PKA_MODULE_ERROR_IDX ) )

/*! The error base address of the ChaCha module - 0x00F02200. */
#define CC_CHACHA_MODULE_ERROR_BASE  (CC_ERROR_BASE + \
                                           (CC_ERROR_LAYER_RANGE * CC_LAYER_ERROR_IDX) + \
                                           (CC_ERROR_MODULE_RANGE * CHACHA_ERROR_IDX ) )
/*! The error base address of the EC MONT_EDW module - 0x00F02300.  */
#define CC_EC_MONT_EDW_MODULE_ERROR_BASE (CC_ERROR_BASE + \
                                           (CC_ERROR_LAYER_RANGE * CC_LAYER_ERROR_IDX) + \
                                           (CC_ERROR_MODULE_RANGE * EC_MONT_EDW_ERROR_IDX ) )

/*! The error base address of the Chacha-POLY module - 0x00F02400. */
#define CC_CHACHA_POLY_MODULE_ERROR_BASE  (CC_ERROR_BASE + \
                                           (CC_ERROR_LAYER_RANGE * CC_LAYER_ERROR_IDX) + \
                                           (CC_ERROR_MODULE_RANGE * CHACHA_POLY_ERROR_IDX ) )
/*! The error base address of the POLY module - 0x00F02500. */
#define CC_POLY_MODULE_ERROR_BASE  (CC_ERROR_BASE + \
                                           (CC_ERROR_LAYER_RANGE * CC_LAYER_ERROR_IDX) + \
                                           (CC_ERROR_MODULE_RANGE * POLY_ERROR_IDX ) )

/*! The error base address of the SRP module - 0x00F02600. */
#define CC_SRP_MODULE_ERROR_BASE (CC_ERROR_BASE + \
                                           (CC_ERROR_LAYER_RANGE * CC_LAYER_ERROR_IDX) + \
                                           (CC_ERROR_MODULE_RANGE * SRP_ERROR_IDX ) )

/*! The error base address of the AESGCM module - 0x00F02700. */
#define CC_AESGCM_MODULE_ERROR_BASE  (CC_ERROR_BASE + \
                                       (CC_ERROR_LAYER_RANGE * CC_LAYER_ERROR_IDX) + \
                                       (CC_ERROR_MODULE_RANGE * AESGCM_ERROR_IDX ) )

/*! The error base address of the AES key-wrap module - 0x00F02800. */
#define CC_AES_KEYWRAP_MODULE_ERROR_BASE  (CC_ERROR_BASE + \
                          (CC_ERROR_LAYER_RANGE * CC_LAYER_ERROR_IDX) + \
                          (CC_ERROR_MODULE_RANGE * AES_KEYWRAP_ERROR_IDX ) )

/*! The error base address of the Management module - 0x00F02900. */
#define CC_MNG_MODULE_ERROR_BASE      (CC_ERROR_BASE + \
                        (CC_ERROR_LAYER_RANGE * CC_LAYER_ERROR_IDX) + \
                        (CC_ERROR_MODULE_RANGE * MNG_ERROR_IDX ) )

/*! The error base address of the production library - 0x00F02A00 */
#define CC_PROD_MODULE_ERROR_BASE  (CC_ERROR_BASE + \
                                     (CC_ERROR_LAYER_RANGE * CC_LAYER_ERROR_IDX) + \
                                     (CC_ERROR_MODULE_RANGE * PROD_ERROR_IDX ) )

/*! The error base address of the FFCDH module - 0x00F02B00. */
#define CC_FFCDH_MODULE_ERROR_BASE (CC_ERROR_BASE + \
                                           (CC_ERROR_LAYER_RANGE * CC_LAYER_ERROR_IDX) + \
                                           (CC_ERROR_MODULE_RANGE * FFCDH_ERROR_IDX ) )

/*! The error base address of the FFCDH module - 0x00F02B00. */
#define CC_FFC_DOMAIN_MODULE_ERROR_BASE (CC_ERROR_BASE + \
                                           (CC_ERROR_LAYER_RANGE * CC_LAYER_ERROR_IDX) + \
                                           (CC_ERROR_MODULE_RANGE * FFC_DOMAIN_ERROR_IDX ) )

/*! The error base address of the External DMA module - 0x00F02B00. */
#define CC_EXT_DMA_MODULE_ERROR_BASE (CC_ERROR_BASE + \
                                           (CC_ERROR_LAYER_RANGE * CC_LAYER_ERROR_IDX) + \
                                           (CC_ERROR_MODULE_RANGE * EXT_DMA_ERROR_IDX ) )

/*! The generic error base address of the user - 0x00F50000 */
#define GENERIC_ERROR_BASE ( CC_ERROR_BASE + (CC_ERROR_LAYER_RANGE * GENERIC_ERROR_IDX) )
/*! CryptoCell fatal error. */
#define CC_FATAL_ERROR          (GENERIC_ERROR_BASE + 0x00UL)
/*! CryptoCell out of resources error. */
#define CC_OUT_OF_RESOURCE_ERROR        (GENERIC_ERROR_BASE + 0x01UL)
/*! CryptoCell illegal resource value error. */
#define CC_ILLEGAL_RESOURCE_VAL_ERROR       (GENERIC_ERROR_BASE + 0x02UL)



/* ............ The OK (success) definition ....................... */

/*! A macro that defines the CryptoCell return value. */
#define CC_CRYPTO_RETURN_ERROR(retCode, retcodeInfo, funcHandler) \
    ((retCode) == 0 ? CC_OK : funcHandler(retCode, retcodeInfo))

/************************ Enums ********************************/


/************************ Typedefs  ****************************/


/************************ Structs  ******************************/


/************************ Public Variables **********************/


/************************ Public Functions **********************/

#ifdef __cplusplus
}
#endif

/*!
@}
 */

#endif
