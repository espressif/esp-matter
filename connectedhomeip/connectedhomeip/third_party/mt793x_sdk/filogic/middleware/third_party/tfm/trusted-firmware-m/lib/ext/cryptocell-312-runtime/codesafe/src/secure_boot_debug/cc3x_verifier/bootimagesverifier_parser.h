/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _BOOT_IMAGES_VERIFIER_PARSER_H
#define _BOOT_IMAGES_VERIFIER_PARSER_H


#ifdef __cplusplus
extern "C"
{
#endif

#include "secureboot_defs.h"
#include "cc_crypto_boot_defs.h"
#include "secureboot_parser_gen_defs.h"
#include "secdebug_defs.h"

/* Definitions used by the functions */
/*-----------------------------------*/

/* mask to location of Offset to signature bits in the header cert size parameter */
#define CERT_LEN_SIGNATURE_OFFSET_BIT_MASK         0x0000FFFFUL

/* certificate version Major offset */
#define CERT_VERSION_MAJOR_BIT_SHIFT    16

/* Structures used inside the parser code */
/*----------------------------------------*/

/* RSA data (united to one structure) for RSA_PSS_3072 */
typedef struct {
    /* Pointer to N */
    uint32_t  *N_ptr;

    /* Pointer to Np OR H according to algorithm used */
    uint32_t  *NDer_ptr;

    /* Pointer to RSA signature */
    uint32_t *signature;

    /* Size of cert for HASH computation (offset to signature) */
    uint32_t   certSizeInWordsForHash;

}CCSbCertParserRSAData_t;


/*----------------------------
      PUBLIC FUNCTIONS
-----------------------------------*/

/*!
   @brief This function start to load the certificate from flash to RAM,
   including the signed data and the signature.
   It does not include the unsigned data in case of content certificate.

@return CC_OK   On success.
@return A non-zero value from bsv_error.h on failure.
 */
uint32_t CCCertLoadCertificate(CCSbFlashReadFunc flashRead_func,    /*!< [in] Pointer to the flash read function. */
                               void *userContext,           /*!< [in] An additional pointer for flashRead usage. May be NULL. */
                               CCAddr_t certAddress,            /*!< [in] The address where the certificate is located. This address is provided to flashReadFunc. */
                               uint32_t *pCert,             /*!< [in] Buffer for the function's internal use. */
                               uint32_t *pCertBufferWordSize);      /*!< [in/out] Set the maximum certificate size, and get back the actual certificate size. */

/*!
   @brief This function calculates the certificate size that includes the signed data and the signature.

@return CC_OK   On success.
@return A non-zero value from bsv_error.h on failure.
 */
uint32_t CCCertGetUnsignedDataOffset(uint32_t *pCert,           /*!< [in] Buffer for the function's internal use. */
                     uint32_t *pUnsignedDataOffset);    /*!< [out] Get the actual certificate size of the signed data and the signature (in words). */

#ifdef __cplusplus
}
#endif

#endif


