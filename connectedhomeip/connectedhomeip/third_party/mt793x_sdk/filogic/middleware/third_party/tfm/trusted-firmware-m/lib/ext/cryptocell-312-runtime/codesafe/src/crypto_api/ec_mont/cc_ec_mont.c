/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <string.h>
#include "cc_pal_types.h"
#include "cc_pal_mem.h"
#include "cc_hash_defs.h"
#include "cc_common.h"
#include "cc_rnd_error.h"
#include "cc_ec_mont_api.h"
#include "mbedtls_cc_ec_mont_edw_error.h"
#include "ec_mont_local.h"
#include "ec_mont.h"
#include "cc_general_defs.h"
#include "md.h"

// RL Debug
#include "pki_dbg.h"

/******************************************************************************/
/*!
   The function performs EC Montgomery scalar multiplication:
         resPoint = scalar * point.<br>
  <br>
         Note: all byte arrays have LE order of bytes, i.e. LS byte is on left most place.<br>
  <br>
  return CCError_t
*/
CEXPORT_C CCError_t CC_EcMontScalarmult(
                                uint8_t       *pResPoint,       /*!< [out] Pointer to the public (secret) key. */
                                size_t        *pResPointSize,   /*!< [in/out] Pointer to the size of the public key in bytes.
                                                                       In  - the size of the buffer. must be at least EC modulus
                                                                             size (for curve25519 - 32 bytes).
                                                                       Out - the actual size. */
                                const uint8_t *pScalar,         /*!< [in] Pointer to the secret (private) key. */
                                size_t         scalarSize,      /*!< [in] Pointer to the size of the secret key in bytes;
                                                                     must be equal to EC order size (for curve25519 - 32 bytes). */
                                const uint8_t *pInPoint,        /*!< [in] Pointer to the input point (compressed). */
                                size_t         inPointSize,     /*!< [in] Size of the point - must be equal to CC_EC_MONT_MOD_SIZE_IN_BYTES. */
                                CCEcMontTempBuff_t *pEcMontTempBuff)  /*!< [in] pointer temp buffer. */
{
        CCError_t err = CC_OK;
        /* pointers to aligned buffers */
        uint32_t *pResPoint32, *pScalar32, *pInPoint32;
        size_t ecOrdSizeBytes, ecModSizeBytes;
        size_t scalarSizeWords;
        /* the pointer to EC domain (curve). */
        const CCEcMontDomain_t *pEcDomain;
        uint32_t ls, ms;


        if (pResPoint == NULL || pResPointSize == NULL ||
            pScalar == NULL || pInPoint == NULL ||
            pEcMontTempBuff == NULL) {
                return CC_EC_MONT_INVALID_INPUT_POINTER_ERROR;
        }

        /* get domain */
        pEcDomain = EcMontGetCurve25519Domain();


        /* set EC domain parameters sizes */
        scalarSizeWords = pEcDomain->ecOrdSizeInWords;
        ecModSizeBytes = pEcDomain->ecModSizeInWords * sizeof(uint32_t);
        ecOrdSizeBytes = pEcDomain->ecOrdSizeInWords * sizeof(uint32_t);

        /* check sizes */
        if (*pResPointSize < ecModSizeBytes ||
            inPointSize != ecModSizeBytes  ||
            scalarSize != ecOrdSizeBytes) {
                return CC_EC_MONT_INVALID_INPUT_SIZE_ERROR;
        }

        /* set pointers to temp buffer */
        pScalar32 = pEcMontTempBuff->ecMontScalar;
        pResPoint32 = pEcMontTempBuff->ecMontResPoint;
        pInPoint32 = pEcMontTempBuff->ecMontInPoint;

        /* convert input byte arrays to LE word arrays */
        CC_CommonConvertLsbMsbBytesToLswMswWords(pScalar32, (uint8_t*)pScalar, scalarSize);
        CC_CommonConvertLsbMsbBytesToLswMswWords(pInPoint32, (uint8_t*)pInPoint, ecModSizeBytes);

        /* revert changed bytes in the scalar */
        ls = pScalar32[0]; ms = pScalar32[scalarSizeWords-1];

        /* set scalar bits according to EC Montgomery curve25519 algorithm:
           byte[31] = (byte[31] & 127) | 64; byte[0] &= 248; */
        pScalar32[scalarSizeWords-1] = (pScalar32[scalarSizeWords-1] & 0x7FFFFFFF) | 0x40000000;
        pScalar32[0] &= 0xFFFFFFF8;

        /* call llf pScalar multiplication function */
        err = EcMontScalarmult(
                              pResPoint32,
                              pScalar32,
                              pInPoint32,
                              pEcDomain);
        if (err) {
                err = CC_ECMONT_INTERNAL_ERROR;
                goto EndWithError;
        }

        /* revert changed bytes in the scalar */
        pScalar32[0] = ls; pScalar32[scalarSizeWords-1] = ms;

        /* output pResPoint to LE bytes array */
        CC_CommonConvertLswMswWordsToLsbMsbBytes(pResPoint, pResPoint32, pEcDomain->ecModSizeInWords);
        *pResPointSize = ecModSizeBytes;

        EndWithError:
        /* zeroing temp buffers, they are not used as input/output */
        if ((uint8_t*)pScalar32 != pScalar) {
                CC_PalMemSetZero((uint8_t*)pScalar32, ecModSizeBytes);
        }
        if ((uint8_t*)pResPoint32 != pResPoint) {
                CC_PalMemSetZero((uint8_t*)pResPoint32, ecModSizeBytes);
        }
        if ((uint8_t*)pInPoint32 != pInPoint) {
                CC_PalMemSetZero((uint8_t*)pInPoint32, ecModSizeBytes);
        }

        CC_PalMemSetZero(&pEcMontTempBuff->ecMontScalrMultTempBuff, sizeof(CCEcMontScalrMultTempBuff_t));
        return err;

}

/*********************************************************************/
/*!
@brief The function performs EC Montgomery (Curve25519) scalar multiplication of base point:
       res = scalar * base_point.

       Note: all byte arrays have LE order of bytes, i.e. LS byte is on left most place.


@return CC_OK on success,
@return A non-zero value on failure as defined mbedtls_cc_ec_mont_edw_error.h.
*/

CEXPORT_C CCError_t CC_EcMontScalarmultBase(
                                uint8_t       *pResPoint,      /*!< [out] Pointer to the public (secret) key. */
                                size_t        *pResPointSize,  /*!< [in/out] Pointer to the size of the public key in bytes.
                                                                      In  - the size of buffer must be at least EC modulus size
                                                                          (for curve25519 - 32 bytes);
                                                                      Out - the actual size. */
                                const uint8_t *pScalar,        /*!< [in] Pointer to the secret (private) key. */
                                size_t         scalarSize,     /*!< [in] Pointer to the size of the scalar in bytes -
                                                                    must be equal to EC order size (for curve25519 - 32 bytes). */
                                CCEcMontTempBuff_t *pEcMontTempBuff) /*!< [in] pointer to the temp buffer. */
{
        CCError_t err = CC_OK;
        /* pointers to aligned buffers */
        uint32_t *pResPoint32, *pScalar32;
        size_t ecOrdSizeBytes, ecModSizeBytes;
        size_t scalarSizeWords;
        const CCEcMontDomain_t *pEcDomain;
        uint32_t ls, ms;

        /* check parameters */
        if (pResPoint  == NULL || pResPointSize == NULL ||
            pScalar == NULL || pEcMontTempBuff == NULL) {
                return CC_EC_MONT_INVALID_INPUT_POINTER_ERROR;
        }

        /* get domain */
        pEcDomain = EcMontGetCurve25519Domain();

        /* set EC domain parameters sizes */
        scalarSizeWords = pEcDomain->ecOrdSizeInWords;
        ecModSizeBytes = pEcDomain->ecModSizeInWords * sizeof(uint32_t);
        ecOrdSizeBytes = scalarSizeWords * sizeof(uint32_t);

        /* check sizes */
        if (*pResPointSize < ecModSizeBytes ||
            scalarSize != ecOrdSizeBytes) {
                return CC_EC_MONT_INVALID_INPUT_SIZE_ERROR;
        }

        /* set pointers to temp buffer */
        pScalar32 = pEcMontTempBuff->ecMontScalar;
        pResPoint32 = pEcMontTempBuff->ecMontResPoint;

        /* convert input byte arrays to LE word arrays */
        CC_CommonConvertLsbMsbBytesToLswMswWords(pScalar32, (uint8_t*)pScalar, scalarSize);

        /* revert changed bytes in the scalar */
        ls = pScalar32[0]; ms = pScalar32[scalarSizeWords-1];

        /* set scalar bits according to EC Montgomery curve25519 algorithm:
           byte[31] = (byte[31] & 127) | 64; byte[0] &= 248; */
        pScalar32[scalarSizeWords-1] = (pScalar32[scalarSizeWords-1] & 0x7FFFFFFF) | 0x40000000;
        pScalar32[0] &= 0xFFFFFFF8;

        /* call llf pScalar multiplication function */
        err = EcMontScalarmult(
                              pResPoint32,
                              pScalar32,
                              (uint32_t*)pEcDomain->ecGenX,
                              pEcDomain);
        if (err) {
                err = CC_ECMONT_INTERNAL_ERROR;
                goto EndWithError;
        }

        /* revert changed bytes in the scalar */
        pScalar32[0] = ls; pScalar32[scalarSizeWords-1] = ms;

        /* output pResPoint to LE bytes array */
        CC_CommonConvertLswMswWordsToLsbMsbBytes(pResPoint, pResPoint32,
                                                    pEcDomain->ecModSizeInWords);
        *pResPointSize = ecModSizeBytes;


        EndWithError:
        /* zeroing temp buffers if they are not used as input/output */
        if ((uint8_t*)pScalar32 != pScalar) {
                CC_PalMemSetZero((uint8_t*)pScalar32, ecModSizeBytes);
        }
        if ((uint8_t*)pResPoint32 != pResPoint) {
                CC_PalMemSetZero((uint8_t*)pResPoint32, ecModSizeBytes);
        }

        return err;

}
/********************************************************************/

static CCError_t ecMontKeyPairBase (
                              uint8_t *pPublKey,                    /*!< [out] Pointer to the public key. */
                              size_t  *pPublKeySize,                /*!< [in/out] Pointer to the size of the public key in bytes
                                                                    In  - the size of the buffer must be at least EC order size
                                                                   (for curve25519 - 32 bytes);
                                                                                     Out - the actual size. */
                              uint8_t *pSecrKey,                    /*!< [out] Pointer to the secret key, including. */
                              size_t  *pSecrKeySize,                /*!< [in/out] Pointer to the size of buffer for the secret key in bytes -
                                                                                    must be at least EC order size (for curve25519 - 32 bytes). */
                              const uint8_t *pInPoint,        /*!< [in] Pointer to the input point (compressed). must be of size CC_EC_MONT_MOD_SIZE_IN_BYTES */
                              CCRndContext_t *pRndContext,      /*!< [in/out] Pointer to the RND context buffer. */
                              CCEcMontTempBuff_t *pEcMontTempBuff) /*!< [in] pointer to EC domain (curve). */
{
        /* DEFINITIONS */

        CCError_t err = CC_OK;
        uint32_t ecScalarSizeBytes, ecModSizeBytes;
        CCRndGenerateVectWorkFunc_t RndGenerateVectFunc;
        /* the pointer to EC domain (curve). */
        const CCEcMontDomain_t *pEcDomain;


        /* FUNCTION LOGIC */

        /* check input parameters */
        if (pSecrKey  == NULL || pSecrKeySize == NULL ||
            pPublKey  == NULL || pPublKeySize == NULL ||
            pRndContext == NULL || pEcMontTempBuff == NULL) {
                return CC_EC_MONT_INVALID_INPUT_POINTER_ERROR;
        }

        /* get domain */
        pEcDomain = EcMontGetCurve25519Domain();

        /* EC pScalar size in bytes */
        ecScalarSizeBytes = pEcDomain->ecOrdSizeInWords * sizeof(uint32_t);
        ecModSizeBytes = pEcDomain->ecModSizeInWords * sizeof(uint32_t);

        if (*pSecrKeySize < ecScalarSizeBytes ||
            *pPublKeySize < ecScalarSizeBytes) {
                return CC_EC_MONT_INVALID_INPUT_SIZE_ERROR;
        }

        /* generaate secret key (seed) */
        RndGenerateVectFunc = pRndContext->rndGenerateVectFunc;
        if(RndGenerateVectFunc == NULL)
                return CC_RND_GEN_VECTOR_FUNC_ERROR;

        err = RndGenerateVectFunc(
                        (void *)pRndContext->rndState,
                        (unsigned char *)pSecrKey,
                        (size_t)ecScalarSizeBytes);
        if (err) {
                goto End;
        }

        *pPublKeySize = ecModSizeBytes;

        /* calculate public key by pScalar mult. */
        if (pInPoint != NULL)
        {
            err = CC_EcMontScalarmult(
                                    pPublKey,
                                    pPublKeySize,
                                    pSecrKey,
                                    ecScalarSizeBytes,
                                    pInPoint,
                                    CC_EC_MONT_MOD_SIZE_IN_BYTES,
                                    pEcMontTempBuff);
        }
        else /* use the curve base point*/
        {
            err = CC_EcMontScalarmultBase(
                                    pPublKey,
                                    pPublKeySize,
                                    pSecrKey,
                                    ecScalarSizeBytes,
                                    pEcMontTempBuff);
        }
        if (err) {
                goto End;
        }

        /* output results */
        *pSecrKeySize = ecScalarSizeBytes;

        End:

        CC_PalMemSetZero(pEcMontTempBuff, sizeof(CCEcMontTempBuff_t));

        if (err) {
                CC_PalMemSetZero(pPublKey, ecModSizeBytes);
                CC_PalMemSetZero(pSecrKey, ecScalarSizeBytes);
        }
        return err;

}


/*******************************************************************/
/*!
@brief The function randomly generates  private and public keys for Montgomery
       Curve25519, with a configurable base point.


\note All byte arrays are in LE order of bytes, i.e. LS byte is on the left most place. \par
\note LS and MS bits of the Secret key are set according to EC Montgomery scalar mult. algorithm:
                secrKey[0] &= 248; secrKey[31] &= 127; secrKey[31] |= 64;

@return CC_OK on success,
@return A non-zero value on failure as defined mbedtls_cc_ec_mont_edw_error.h.

*/
CEXPORT_C CCError_t CC_EcMontKeyPairBase(
                                         uint8_t *pPublKey,                    /*!< [out] Pointer to the public key. */
                                         size_t  *pPublKeySize,                /*!< [in/out] Pointer to the size of the public key in bytes.
                                                                                     In  - the size of the buffer must be at least EC order size
                                                                                           (for curve25519 - 32 bytes);
                                                                                     Out - the actual size. */
                                         uint8_t *pSecrKey,                    /*!< [out] Pointer to the secret key, including. */
                                         size_t  *pSecrKeySize,                /*!< [in/out] Pointer to the size of buffer for the secret key in bytes -
                                                                                    must be at least EC order size (for curve25519 - 32 bytes). */
                                         const uint8_t *pInPoint,        /*!< [in] Pointer to the input point (compressed). */
                                         size_t         inPointSize,     /*!< [in] Size of the point - must be equal to CC_EC_MONT_MOD_SIZE_IN_BYTES. */
                                         CCRndContext_t *pRndContext,      /*!< [in/out] Pointer to the RND context buffer. */
                                         CCEcMontTempBuff_t *pEcMontTempBuff) /*!< [in] pointer to EC domain (curve). */
{
    if (inPointSize != CC_EC_MONT_MOD_SIZE_IN_BYTES)
    {
        return CC_EC_MONT_INVALID_INPUT_SIZE_ERROR;
    }

    return ecMontKeyPairBase(pPublKey, pPublKeySize, pSecrKey, pSecrKeySize, pInPoint, pRndContext, pEcMontTempBuff);
}

/*******************************************************************/
/*!
@brief The function randomly generates  private and public keys for Montgomery
       Curve25519. it uses CC_EcMontKeyPair with the Generator point of the Curve


\note All byte arrays are in LE order of bytes, i.e. LS byte is on the left most place. \par
\note LS and MS bits of the Secret key are set according to EC Montgomery scalar mult. algorithm:
                secrKey[0] &= 248; secrKey[31] &= 127; secrKey[31] |= 64;

@return CC_OK on success,
@return A non-zero value on failure as defined mbedtls_cc_ec_mont_edw_error.h.

*/
CEXPORT_C CCError_t CC_EcMontKeyPair(
                                         uint8_t *pPublKey,                    /*!< [out] Pointer to the public key. */
                                         size_t  *pPublKeySize,                /*!< [in/out] Pointer to the size of the public key in bytes.
                                                                                     In  - the size of the buffer must be at least EC order size
                                                                                          (for curve25519 - 32 bytes);
                                                                                     Out - the actual size. */
                                         uint8_t *pSecrKey,                    /*!< [out] Pointer to the secret key, including. */
                                         size_t  *pSecrKeySize,                /*!< [in/out] Pointer to the size of buffer for the secret key in bytes -
                                                                                    must be at least EC order size (for curve25519 - 32 bytes). */
                                         CCRndContext_t *pRndContext,      /*!< [in/out] Pointer to the RND context buffer. */
                                         CCEcMontTempBuff_t *pEcMontTempBuff) /*!< [in] pointer to EC domain (curve). */
{

    return ecMontKeyPairBase(pPublKey, pPublKeySize, pSecrKey, pSecrKeySize, NULL, pRndContext, pEcMontTempBuff);
}

/*******************************************************************/
/**
@brief The function generates private and public keys for Montgomery Curve25519
       using given seed.

       Note: all byte arrays have LE order of bytes, i.e. LS byte is on left most place.

@return CC_OK on success,
@return a non-zero value on failure as defined mbedtls_cc_ec_mont_edw_error.h.
*/
CEXPORT_C CCError_t CC_EcMontSeedKeyPair (
                      uint8_t       *pPublKey,       /*!< [out] Pointer to the public (secret) key. */
                      size_t        *pPublKeySize,   /*!< [in/out] Pointer to the size of the public key in bytes.
                                                             In  - the size of buffer must be at least EC order size
                                                                (for curve25519 - 32 bytes);
                                                             Out - the actual size. */
                      uint8_t       *pSecrKey,       /*!< [out] Pointer to the secret (private) key. */
                      size_t        *pSecrKeySize,   /*!< [in/out] Pointer to the size of the secret key in bytes
                                                              In  - the size of buffer must be at least EC order size
                                                                (for curve25519 - 32 bytes);
                                                              Out - the actual size. */
                      const uint8_t *pSeed,          /*!< [in] Pointer to the given seed - 32 bytes. */
                      size_t         seedSize,       /*!< [in/] Size of the seed in bytes (must be equal to CC_EC_MONT_SEEDBYTES). */
                      CCEcMontTempBuff_t *pEcMontTempBuff)  /*!< [in] pointer temp buffer. */
{
        /* DEFINITIONS */

        CCError_t err = CC_OK;
        uint8_t *pScalar, *pRes;
        CCHashResultBuf_t *pHashResult; /*takes 64 bytes in pEcMontTempBuff*/
        uint32_t ecOrdSizeBytes, ecMontSizeBytes;
        /* the pointer to EC domain (curve). */
        const CCEcMontDomain_t *pEcDomain;
        const mbedtls_md_info_t *md_info=NULL;

        /* FUNCTION LOGIC */


        /* check input parameters */
        if (pSecrKey  == NULL || pSecrKeySize == NULL ||
            pPublKey == NULL || pPublKeySize == NULL ||
            pSeed == NULL || pEcMontTempBuff == NULL) {
                return CC_EC_MONT_INVALID_INPUT_POINTER_ERROR;
        }

        /* get domain */
        pEcDomain = EcMontGetCurve25519Domain();

        ecOrdSizeBytes = pEcDomain->ecOrdSizeInWords*sizeof(uint32_t);
        ecMontSizeBytes = pEcDomain->ecModSizeInWords*sizeof(uint32_t);

        if (*pSecrKeySize < ecOrdSizeBytes ||
             seedSize != ecOrdSizeBytes) {
                return CC_EC_MONT_INVALID_INPUT_SIZE_ERROR;
        }

        pHashResult = (CCHashResultBuf_t*)pEcMontTempBuff;
        pScalar = (uint8_t*)&pEcMontTempBuff->ecMontScalar;
        pRes = (uint8_t*)&pEcMontTempBuff->ecMontResPoint;

        /* copy seed into buffer with phys. memory (HW HASH requirement) */
        CC_PalMemCopy(pScalar, pSeed, seedSize);

        /* calculate secret key; note pScalar points  *
        *  to the same mem. as pHashResult            */
        md_info = mbedtls_md_info_from_string( HashAlgMode2mbedtlsString[CC_HASH_SHA512_mode] );
        if (NULL == md_info) {
             err = CC_EC_MONT_INVALID_INPUT_SIZE_ERROR;
             goto End;

        }
        err = mbedtls_md(md_info,
                         pScalar,
                         seedSize,
                        (unsigned char *)(*pHashResult));
        if (err) {
                goto End;
        }

        /* calculate the public key */
        err = CC_EcMontScalarmultBase(
                                (uint8_t*)&pEcMontTempBuff->ecMontResPoint[0],
                                pPublKeySize,
                                pScalar,  // check endianness !
                                ecOrdSizeBytes,
                                pEcMontTempBuff);

        if (err) {
                goto End;
        }

        /* set actual sizes of Secr. and publ. keys */
        *pSecrKeySize = ecOrdSizeBytes;
        *pPublKeySize = ecMontSizeBytes;

        /* output secret and publ. keys */
        CC_PalMemCopy(pSecrKey, (uint8_t*)pScalar, *pSecrKeySize);
        CC_PalMemCopy(pPublKey, pRes, *pPublKeySize);

        End:
        /* clean the temp buffers */
        CC_PalMemSetZero((uint8_t*)pEcMontTempBuff, sizeof(CCEcMontTempBuff_t));
        if (err) {
                CC_PalMemSetZero(pSecrKey, ecOrdSizeBytes);
                CC_PalMemSetZero(pPublKey, ecMontSizeBytes);
        }

        return err;
}

