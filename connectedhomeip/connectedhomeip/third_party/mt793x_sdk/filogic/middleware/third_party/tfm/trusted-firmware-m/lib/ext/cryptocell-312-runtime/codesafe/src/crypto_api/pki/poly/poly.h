/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef POLY_H
#define POLY_H

/*
 * All the includes that are needed for code using this module to
 * compile correctly should be #included here.
 */

#include "cc_error.h"
#include "mbedtls_cc_poly.h"


#ifdef __cplusplus
extern "C"
{
#endif

/*! The POLY block size in 32-bit words */
#define CC_POLY_BLOCK_SIZE_IN_WORDS  4
#define CC_POLY_BLOCK_SIZE_IN_BYTES  (CC_POLY_BLOCK_SIZE_IN_WORDS*CC_32BIT_WORD_SIZE)

#define POLY_PRIME_SIZE_IN_BITS   130
#define POLY_PRIME_SIZE_IN_WORDS  CALC_FULL_32BIT_WORDS(POLY_PRIME_SIZE_IN_BITS)

/*! The POLY PKA registers size in 32-bit words */
#define CC_POLY_PKA_REG_SIZE_IN_PKA_WORDS  4
#define CC_POLY_PKA_REG_SIZE_IN_WORDS  (CC_POLY_PKA_REG_SIZE_IN_PKA_WORDS * (CALC_FULL_32BIT_WORDS(CC_PKA_WORD_SIZE_IN_BITS)))
#define CC_POLY_PKA_REG_SIZE_IN_BYTES  (CC_POLY_PKA_REG_SIZE_IN_WORDS*CC_32BIT_WORD_SIZE)


/**
 * @brief Generates the POLY mac according to RFC 7539 section 2.5.1
 *
 * @return  CC_OK On success, otherwise indicates failure
 */
CCError_t PolyMacCalc(mbedtls_poly_key  key,        /*!< [in] Poniter to 256 bits of KEY. */
            const uint8_t       *pAddData,  /*!< [in] Optional - pointer to additional data if any */
            size_t          addDataSize,    /*!< [in] The size of the additional data */
            const uint8_t       *pDataIn,   /*!< [in] Pointer to data buffer to calculate MAC on */
            size_t          dataInSize, /*!< [in] The size of the additional data */
            mbedtls_poly_mac        macRes,     /*!< [out] The calculated MAC */
            bool     isPolyAeadMode);  /*!< [in] Boolean indicating if the Poly MAC operation is part of AEAD or just poly */

#ifdef __cplusplus
}
#endif

#endif  //POLY_H
