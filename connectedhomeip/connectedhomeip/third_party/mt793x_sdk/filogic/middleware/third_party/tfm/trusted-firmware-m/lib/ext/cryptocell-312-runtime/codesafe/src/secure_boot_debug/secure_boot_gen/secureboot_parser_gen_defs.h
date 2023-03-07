/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _SECUREBOOT_PARSER_GEN_DEFS_H_
#define _SECUREBOOT_PARSER_GEN_DEFS_H_


#include "cc_pal_sb_plat.h"
#include "cc_pka_hw_plat_defs.h"
#include "rsa_bsv.h"
#include "secureboot_defs.h"


/*! Public key data structure. */
typedef struct {
    uint32_t N[SB_CERT_RSA_KEY_SIZE_IN_WORDS];              /*!< N public key, big endian representation. */
    uint32_t Np[RSA_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS];    /*!< Np (Barrett n' value). */
}CCSbNParams_t;


/*! SW image data structure. */
typedef struct {
    uint32_t SwHashResult[HASH_RESULT_SIZE_IN_WORDS];       /*!< Hash calculated on the record.*/
    CCAddr_t    memLoadAddr;                    /*!< Memory load address. */
}CCSbHashRecordInfo_t;


/*! Signature structure. */
typedef struct {
    uint32_t sig[SB_CERT_RSA_KEY_SIZE_IN_WORDS];                /*!< RSA PSS signature. */
}CCSbSignature_t;

/*! SW component additional parameters. */
typedef struct {
    CCAddr_t  StoreAddr;                        /*!< Storage address. */
    uint32_t    Len;                        /*!< Size of the SW component in words. */
}CCSbSwImgAddData_t;


/********* Supported algorithms definitions ***********/

/*! hash supported algorithms. */
typedef enum {
    HASH_SHA256_Alg_Output      = 0x01,     /*!< hash SHA 256 output. */
    HASH_SHA256_Alg_128_Output  = 0x02,     /*!< hash SHA 256 output truncated to 128 bits. */
    HASH_Last                   = 0x7FFFFFFF

}CCSbHashAlg_t;


/*! RSA supported algorithms */
typedef enum {
    RSA_ALG_MIN,
    RSA_PSS_2048           = 0x01,          /*!< RSA PSS 2048 after hash SHA 256 */
    RSA_PSS_3072           = 0x02,          /*!< RSA PSS 3072 after hash SHA 256 */
    RSA_ALG_MAX,
    RSA_Last               = 0x7FFFFFFF
}CCSbSignAlg_t;

#endif /* _GEN_SECUREBOOT_PARSER_GEN_DEFS_H_ */
