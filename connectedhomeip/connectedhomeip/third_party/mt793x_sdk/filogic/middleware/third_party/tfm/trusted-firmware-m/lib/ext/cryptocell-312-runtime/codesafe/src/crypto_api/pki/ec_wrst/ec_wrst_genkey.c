/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "cc_pal_mem.h"
#include "cc_rnd_common.h"
#include "cc_ecpki_types.h"
#include "cc_ecpki_error.h"
#include "cc_ecpki_local.h"
#include "pki.h"
#include "pka_ec_wrst.h"

/***************  EcWrstGenKeyPairBase function  **************/
/**
 * @brief Generates a pair of private and public keys
 *        in little endian ordinary (non-Montgomery) form using a cofigurable base point.
 *
 *    The function performs the following:
 *      1. Checks the validity of all of the function inputs. If one of the received
 *         parameters is not valid, it returns an error.
 *      2. Cleans buffers and generates random private key.
 *      3. Calls the low level function PkaEcWrstScalarMult to generate EC public key.
 *      4. Outputs the user public and private key structures in little endian form.
 *      5. Cleans temporary buffers.
 *      6. Exits.
 *
 * @return  CC_OK On success, otherwise indicates failure
*/
CEXPORT_C CCError_t EcWrstGenKeyPairBase(const CCEcpkiDomain_t  *pDomain,
                                         const uint32_t         ecX [CC_ECPKI_MODUL_MAX_LENGTH_IN_WORDS],
                                         const uint32_t         ecY [CC_ECPKI_MODUL_MAX_LENGTH_IN_WORDS],
                                         CCEcpkiUserPrivKey_t   *pUserPrivKey,
                                         CCEcpkiUserPublKey_t   *pUserPublKey,
                                         CCEcpkiKgTempData_t    *pTempBuff)
{
    CCError_t err = CC_OK;
    CCEcpkiPrivKey_t *pPrivKey;
    CCEcpkiPublKey_t *pPublKey;
    uint32_t  orderSizeInWords;

    if (pDomain == NULL)
        return CC_ECPKI_DOMAIN_PTR_ERROR;

    if (pUserPrivKey == NULL)
        return CC_ECPKI_GEN_KEY_INVALID_PRIVATE_KEY_PTR_ERROR;

    if (pUserPublKey == NULL)
        return CC_ECPKI_GEN_KEY_INVALID_PUBLIC_KEY_PTR_ERROR;

    if (pTempBuff == NULL)
        return CC_ECPKI_GEN_KEY_INVALID_TEMP_DATA_PTR_ERROR;

    if (NULL == ecX || NULL == ecY)
        return CC_ECPKI_INVALID_BASE_POINT_PTR_ERROR;

    /* the pointer to the key database */
    pPrivKey = (CCEcpkiPrivKey_t *)&pUserPrivKey->PrivKeyDbBuff;
    pPublKey = (CCEcpkiPublKey_t *)&pUserPublKey->PublKeyDbBuff;

    orderSizeInWords = (pDomain->ordSizeInBits+CC_BITS_IN_32BIT_WORD-1)/CC_BITS_IN_32BIT_WORD;

    /* calculate public key point coordinates */
    err = PkaEcWrstScalarMult(pDomain,
                              pPrivKey->PrivKey/*scalar*/, orderSizeInWords, /*scalar size*/
                              ecX, ecY, /*in point coordinates*/
                              pPublKey->x, pPublKey->y,  /*out point coordinates*/
                              (uint32_t*)pTempBuff);
    if(err) {
        err = CC_ECPKI_INTERNAL_ERROR;
        goto End;
    }

    if(CC_OK == err) {
        /*     set the EC domain and  keys valid tags        */
        CC_PalMemCopy((uint8_t*)&pPrivKey->domain, (uint8_t*)pDomain, sizeof(pPrivKey->domain));
        pUserPrivKey->valid_tag  = CC_ECPKI_PRIV_KEY_VALIDATION_TAG;

        CC_PalMemCopy((uint8_t*)&pPublKey->domain, (uint8_t*)pDomain, sizeof(pPublKey->domain));
        pUserPublKey->valid_tag  = CC_ECPKI_PUBL_KEY_VALIDATION_TAG;
        return err;
    }

End:
    pUserPrivKey->valid_tag  = 0;
    pUserPublKey->valid_tag  = 0;
    CC_PalMemSet(pPrivKey, 0, sizeof(pPrivKey->PrivKey));
    CC_PalMemSet(pPublKey, 0, 2*sizeof(pPublKey->x));
    return err;

}

/***************  EcWrstGenKeyPair function  **************/
/**
 * @brief Generates a pair of private and public keys
 *        in little endian ordinary (non-Montgomery) form.
 *
 *    The function performs the following:
 *      1. Checks the validity of all of the function inputs. If one of the received
 *         parameters is not valid, it returns an error.
 *      2. Cleans buffers and generates random private key.
 *      3. Calls the low level function PkaEcWrstScalarMult to generate EC public key.
 *      4. Outputs the user public and private key structures in little endian form.
 *      5. Cleans temporary buffers.
 *      6. Exits.
 *
 * @return  CC_OK On success, otherwise indicates failure
*/
CEXPORT_C CCError_t EcWrstGenKeyPair(const CCEcpkiDomain_t  *pDomain,      /*!< [in] Pointer to current EC domain.*/
                    CCEcpkiUserPrivKey_t   *pUserPrivKey, /*!< [out] Pointer to the generated private key structure.*/
                    CCEcpkiUserPublKey_t   *pUserPublKey, /*!< [out] Pointer to the generated public key structure.*/
                    CCEcpkiKgTempData_t   *pTempBuff)    /*!< [in] Pointer to temporary buffer.*/
{

    if (pDomain == NULL)
        return CC_ECPKI_DOMAIN_PTR_ERROR;

    return EcWrstGenKeyPairBase(pDomain, pDomain->ecGx, pDomain->ecGy, pUserPrivKey, pUserPublKey, pTempBuff);

}



