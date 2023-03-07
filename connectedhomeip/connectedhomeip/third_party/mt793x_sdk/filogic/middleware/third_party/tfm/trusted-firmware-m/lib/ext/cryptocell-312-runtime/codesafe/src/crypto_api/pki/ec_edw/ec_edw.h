/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef EC_EDW_H_H
#define EC_EDW_H_H


#ifdef __cplusplus
extern "C"
{
#endif

/*
 * All the includes that are needed for code using this module to
 * compile correctly should be #included here.
 */
#include "cc_pal_types.h"
#include "cc_ec_edw_api.h"
#include "ec_edw_local.h"


/*****************   Definitions    ***********************/

/* Len ID for reduction HASH  */
#define LEN_ID_EDW_REDUCE 2


/**********   Global buffers and variables    **************/


/*********************************************************************/
/*!
@brief The function performs EC Edwards key pair generation using seed.

 @return CCError_t
*/
CCError_t EcEdwSeedKeyPair(
                        uint8_t *pPublKey,       /*!< [out] a pointer to publickey (compressed,
                                                       the size = ec modulus size) */
                        uint8_t *pSecrKey,       /*!< [out] a pointer to secret key, stated as concatenation
                                                      of seed amd public key (the size = 2 * ec modulus size) */
                        const uint8_t  *pSeed,   /*!< [in] a pointer to the seed (the size = ec order size) */
                        const CCEcEdwDomain_t *pEcDomain, /*!< [in] pointer to EC domain (curve). */
                        uint32_t *pEcEdwTempBuff); /*!< [in] pointer to the temp buffer. */


/*********************************************************************/
/*!
 * The function performs input/output parameters for EC Edwards
 * scalar multiplication.
 *
 *         resPoint(Y,_) = k*inPoint(X,Y), where:
 *         the point is given with LE order of the words.
 *
 *         Assuming:
 *              The PKA HW is turned ON and initialized yet;
 *
 * \return CCError_t
 */
CCError_t  EcEdwScalarMultBase(
                                uint32_t  *pResPointX,             /*!< [out] a pointer (optional) to result EC point coordinate X. */
                                uint32_t  *pResPointY,             /*!< [out] a pointer to result EC point coordinate Y. */
                                uint32_t  *pScalar,                /*!< [in]  a pointer to the scalar. */
                                size_t     scalarSizeWords,        /*!< [in]  the scalar size in words. */
                                const CCEcEdwDomain_t *pEcDomain   /*!< [in]  a pointer to EC domain (curve). */);

/*********************************************************************/
/*!
 * The function performs input/output parameters for EC Edwards scalar
 * multiplication for special scalars.
 *
 *         resPoint(Y,_) = k*inPoint(X,Y), where:
 *         the point is given with LE order of the words.
 *
 *         Assuming:
 *              The PKA HW is turned on and initialized yet;
 *
 * \return CCError_t
 */
CCError_t  EcEdwSpecialScalarMultBase(
                                uint32_t  *pResPointX,             /*!< [out] a pointer to result EC point coordinate X. */
                                uint32_t  *pResPointY,             /*!< [out] a pointer to result EC point coordinate Y. */
                                uint32_t  *pScalar,                /*!< [in]  a pointer to the scalar. */
                                size_t     scalarSizeWords,         /*!< [in]  the scalar size in words. */
                                const CCEcEdwDomain_t *pEcDomain /*!< [in]  a pointer to EC domain (curve). */);



/*********************************************************************/
/*!
@brief The function performs two EC Edwards scalar multiplication and adding
       of points:  resPoint = scalarA * Point(X,Y) + scalarB * BasePointG.

       Assuming: - PKA is turned on andinitialized yet (including setting N and NP in registers);
                 - Input point may be given in uncompressed form (given both coord. X,Y)
                   or in compressed form: coordinate Y only and LSBit of X on bit place,
                   equalled to modulus size in bits (255 for ec25519).

 @return CCError_t
*/
CCError_t EcEdwAddTwoScalarMult(
                                 uint32_t *pResPoint,        /*!< [out] the pointer to the result point in compressed
                                                                 form (coordinate Y, the size in bytes = ec modulus size). */
                                 uint32_t *pScalarA,         /*!< [in] pointer to the scalarA, the size = ec modulus size in bytes). */
                                 uint32_t *pPointY,          /*!< [in] a pointer to the input point coordinate Y - in uncompressed
                                                                 form, if given coordinate X, or in compressed form if coordinate
                                                                 X == NULL (the size of Y in bytes = ec modulus size). */
                                 uint32_t *pPointX,          /*!< [in, optional] the pointer to input point coordinate X for uncompressed
                                                                 form of point(the size of Y in bytes = ec modulus size). */
                                 uint32_t *pScalarB,         /*!< [in] pointer to the scalarB, the size = ec modulus sizein bytes). */
                                 const CCEcEdwDomain_t *pEcDomain); /*!< [in] pointer to EC domain (curve). */


/*********************************************************************/
/**
 * The function createss EC Edwards detatched signature on given message
 *
 * Implemented algorithm of Bernstein D. etc.
 *
 * @return CCError_t
 */
CCError_t  EcEdwSign (
                        uint8_t       *pEdwSign,             /*!< [out] - the pointer to the signature. */
                        const uint8_t *pMsg,                 /*!< [in] - the pointer to the message. */
                        size_t         msgSize,              /*!< [in] - the message size in bytes: it must be less, than
                                                                  CC_HASH_UPDATE_DATA_MAX_SIZE_IN_BYTES. */
                        const uint8_t *pSignSecrKey,         /*!< [in] - the pointer to the signer secret key, stated as
                                                                  concatenation of the seed and public key) */
                        const CCEcEdwDomain_t *pEcDomain,    /*!< [in] pointer to EDW EC domain (curve). */
                        CCEcEdwTempBuff_t *pEcEdwTempBuff); /*!< [in] pointer to the temp buffer. */


/*********************************************************************/
/**
 * The function verifies EC Edwards detatched signature on given message
 *
 * Implemented algorithm of Bernstein D. etc.
 *
 * @author reuvenl (1/21/2016)
 *
 * @return CCError_t
 */
CCError_t  EcEdwSignVerify (
                        const uint8_t *pInSign,               /*!< [in] - the pointer to input signature. */
                        const uint8_t *pMsg,                  /*!< [in] - the pointer to the message. */
                        size_t         msgSize,               /*!< [in] - the message size in bytes: it must be not
                                                                   great than CC_HASH_UPDATE_DATA_MAX_SIZE_IN_BYTES. */
                        const uint8_t *pSignPublKey,          /*!< [in] - the pointer to the signer public key, stated
                                                                   in compressed form of coordinate Y */
                        const CCEcEdwDomain_t *pEcDomain,    /*!< [in] pointer to EDW EC domain (curve). */
                        CCEcEdwTempBuff_t *pEcEdwTempBuff); /*!< [in] pointer to the temp buffer. */

/**
 * The function calculates uncompressed coordinate X of EC point,
 * using the given compressed coordinates Y.
 *
 * Implemented algorithm Bernstain D. etc (stated by Klimov A.).
 *
 * @author reuvenl (1/11/2016)
 *
 * Imlicit parametrs
 * @param  [out] rX - ID of PKA register for output decompressed coordinate X.
 * @param  [in/out] rY - ID of PKA register, containing compressed/decompressed coordinate Y.
 * @param  [in] isOddX - indication: "Is the coordinate X odd".
 *
 */
void PkaEcEdwDecompress(uint32_t rX, uint32_t rY,
                        uint32_t isOddX) /*!< one bit indication: "Is the coordinate X odd" */;


/**
 * The function performs multiplication of base point by any scalar > 0:
 *        P(x,y) = k*G(x,y), whre G - EC base point.
 *
 *  Implemented algorithm, enhanced by A.Klimov.
 *
 *  Assuming:
 *  1. PKA registers are implicitly defined in pka_ec_edw_glob_regs_def.h file,
 *     in partial: output point P(x,y) is stated by registers (EDW_REG_XS,
 *     EDW_REG_YS).
 *  2. All needed data must be loaded into PKA registers in caller function.
 *  3. PKA registers are defined in pka_ec_edw_glob_regs_defh file, in partial:
 *      - output point R(x,y) shall be registers (rXR=EDW_REG_XR, rYR=EDW_REG_YR),
 *      - input point P(X,Y) by (rXP=EDW_REG_XP, rYP=EDW_REG_YP).
 *
 * @author reuvenl (11/25/2015)
 *
 * @return CC_OK or an error according to mbedtls_cc_ec_mont_edw_error.h definitions.
 */
CCError_t PkaEcEdwScalarMultBase(
                                uint32_t *pScalar,         /*!< [in] the pointer to the scalsr (LS word is
                                                               leftmost one, MS word - rightmost). */
                                size_t    scalarSizeInBits /*!< exact size of the scalar in bits. */ );

/**
 * The function performs multiplication of base point by scalar of special form:
 *      P(x,y) = k*G(x,y).
 *
 *  Implemented algorithm, enhanced by A.Klimov.
 *
 *  The function can work with scalars of special form: exact size is 255
 *  bit and it is a multiple of 8 bit (as requiered in Edw. KeyGen algorithm).
 *  Assuming:
 *  1. PKA registers are implicitly defined in pka_ec_edw_glob_regs_def.h file,
 *     in partial: output point P(x,y) is stated by registers (EDW_REG_XS,
 *     EDW_REG_YS).
 *  2. All needed data must be loaded into PKA registers in caller function.
 *  3. PKA registers are defined in pka_ec_edw_glob_regs_defh file, in partial:
 *      - output point R(x,y) shall be registers (rXR=EDW_REG_XR, rYR=EDW_REG_YR),
 *      - input point P(X,Y) by (rXP=EDW_REG_XP, rYP=EDW_REG_YP).
 *
 * @author reuvenl (11/25/2015)
 *
 * @return CC_OK or an error according to mbedtls_cc_ec_mont_edw_error.h definitions.
 */
CCError_t PkaEcEdwSpecialScalarMultBase(
                                uint32_t *pScalar,         /*!< [in] the pointer to the scalsr (LS word is
                                                               leftmost one, MS word - rightmost). */
                                size_t    scalarSizeInBits /*!< exact size of the scalar in bits. */ );

/**
 * The function performs two scalar mult. and add of input and base points simultaneously:
 *         R(x,y) = a*P(x,y) + b*G(x,y), where P - point, G - base point.
 *
 *  Implemented algorithm of Bernstein D. etc. (version of A.Klimov).
 *
 *  PKA registers are defined in pka_ec_edw_glob_regs_defh file, in partial:
 *      - output point R(x,y) shall be registers (rXR=EDW_REG_XR, rYR=EDW_REG_YR),
 *      - input point P(X,Y) by (rXP=EDW_REG_XP, rYP=EDW_REG_YP).
 *
 * @author reuvenl (11/25/2015)
 *
 * @return CC_OK or an error according to mbedtls_cc_ec_mont_edw_error.h definitions.
 */
CCError_t PkaEcEdwAddTwoScalarMult(
        uint32_t rXR, uint32_t rYR,       /*!< [out] the ID-s of registers, containing aff.
                                               coordinates of result point P */
        uint32_t rXP, uint32_t rYP,       /*!< [in] the ID-s of registers, containing aff.
                                               coordinates of input point P */
        uint32_t *pScalarA,               /*!< [in] the pointer to the scalsr A (LS word is
                                               leftmost one, MS word - rightmost). */
        size_t    scAsizeInBits,          /*!< exact size of the scalar A in bits. */
        uint32_t *pScalarB,               /*!< [in] the pointer to the scalsr B (LS word is
                                               leftmost one, MS word - rightmost). */
        size_t    scBsizeInBits,          /*!< exact size of the scalar B in bits. */
        const CCEcEdwDomain_t *pEcDomain /*!< [in] pointer to EC domain (curve). */);

#ifdef __cplusplus
}

#endif

#endif


