/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


/************* Include Files ****************/

#include "cc_pal_mem.h"
#include "cc_common.h"
#include "cc_common_math.h"
#include "cc_ecpki_error.h"
#include "cc_ecpki_local.h"
#include "cc_fips_defs.h"
#include "ec_wrst.h"

/************************ Defines ***************************************/

/************************ Enums *****************************************/

/************************ Typedefs **************************************/

/************************ Global Data ***********************************/

/************* Private function prototype *******************************/

/************************ Public Functions ******************************/

/**********************************************************************************
 *                    CC_EcpkiPrivKeyBuild function                            *
 **********************************************************************************/
/**
 * The function checks and imports (builds) private key and EC domain into
 * structure of defined type.
 *
 *  This function should be called before using of the private key. Input
 *  domain structure must be initialized by EC parameters and auxiliary
 *  values, using CC_EcpkiGetDomain or CC_EcpkiSetDomain functions.
 *
 *  The function does the following:
 *      - Checks validity of incoming variables and pointers;
 *      - Converts private key to words arrays with little endian order
 *        of the words and copies it in the UserPrivKey buffer.
 *      - Copies EC domain into UserPrivKey  buffer.
 *
 * @author reuvenl (8/11/2014)
 * @param pDomain - The pointer to EC domain structure.
 * @param pPrivKeyIn - The pointer to private key data.
 * @param PrivKeySizeInBytes - The size of private key data in bytes.
 * @param pUserPrivKey - The pointer to private key structure.
 *
 * @return CEXPORT_C CCError_t
 */
CEXPORT_C CCError_t CC_EcpkiPrivKeyBuild(
                                             const CCEcpkiDomain_t *pDomain,        /*in */
                                             const uint8_t             *pPrivKeyIn,     /*in*/
                                             size_t                    privKeySizeInBytes,/*in*/
                                             CCEcpkiUserPrivKey_t  *pUserPrivKey    /*out*/)
{
        /* FUNCTION DECLARATIONS */

        /* the private key structure pointer */
        CCEcpkiPrivKey_t *pPrivKey;
        /*  EC domain info structure and parameters */
        uint32_t  orderSizeInBytes;
        /* the err return code identifier */
        CCError_t err = CC_OK;

        /* FUNCTION LOGIC */
        CHECK_AND_RETURN_ERR_UPON_FIPS_ERROR();

        /* checking the validity of arguments */
        if (pPrivKeyIn == NULL)
                return  CC_ECPKI_BUILD_KEY_INVALID_PRIV_KEY_IN_PTR_ERROR;
        if (pUserPrivKey == NULL)
                return  CC_ECPKI_BUILD_KEY_INVALID_USER_PRIV_KEY_PTR_ERROR;
        if (pDomain == NULL)
                return  CC_ECPKI_DOMAIN_PTR_ERROR;

        /* check EC domain parameters sizes */
        if ((pDomain->modSizeInBits > (CC_ECPKI_MODUL_MAX_LENGTH_IN_WORDS * 32)) ||
            (pDomain->ordSizeInBits > (pDomain->modSizeInBits + 1))) {
                return CC_ECPKI_INVALID_DATA_IN_PASSED_STRUCT_ERROR;
        }


        /****************  FUNCTION LOGIC  **************************************/

        /* the pointer to the key database */
        pPrivKey = (CCEcpkiPrivKey_t *)((void*)pUserPrivKey->PrivKeyDbBuff);

        /* check key size */
        orderSizeInBytes = CALC_FULL_BYTES(pDomain->ordSizeInBits);
        if (privKeySizeInBytes == 0 || privKeySizeInBytes > orderSizeInBytes)
                return  CC_ECPKI_BUILD_KEY_INVALID_PRIV_KEY_SIZE_ERROR;

        /* loading the private key (little endian)*/
        err = CC_CommonConvertMsbLsbBytesToLswMswWords(
                                                         pPrivKey->PrivKey, sizeof(pPrivKey->PrivKey),
                                                         pPrivKeyIn, privKeySizeInBytes);
        if (err != CC_OK) {
                err = CC_ECPKI_BUILD_KEY_INVALID_PRIV_KEY_DATA_ERROR;
                goto End;
        }

        /* check the value of the private key */
        if (privKeySizeInBytes == orderSizeInBytes) {
                if (CC_CommonCmpLsWordsUnsignedCounters(
                                                          pPrivKey->PrivKey, (uint16_t)(privKeySizeInBytes+3)/sizeof(uint32_t),
                                                          pDomain->ecR, (uint16_t)(privKeySizeInBytes+3)/sizeof(uint32_t)) !=
                    CC_COMMON_CmpCounter2GreaterThenCounter1) {
                        err = CC_ECPKI_BUILD_KEY_INVALID_PRIV_KEY_DATA_ERROR;
                        goto End;
                }
        }

        /* compare to 0 */
        if (CC_CommonGetWordsCounterEffectiveSizeInBits(pPrivKey->PrivKey,
                                                           (privKeySizeInBytes+3)/sizeof(uint32_t)) == 0) {
                err = CC_ECPKI_BUILD_KEY_INVALID_PRIV_KEY_DATA_ERROR;
                goto End;
        }


        /* copy EC domain */
        CC_PalMemCopy(&pPrivKey->domain, pDomain, sizeof(CCEcpkiDomain_t));

        /* ................ set the private key validation tag ................... */
        pUserPrivKey->valid_tag = CC_ECPKI_PRIV_KEY_VALIDATION_TAG;

        End:
        /* if the created structure is not valid - clear it */
        if (err != CC_OK) {
                CC_PalMemSetZero(pUserPrivKey, sizeof(CCEcpkiUserPrivKey_t));
        }

        return err;

} /* End of CC_EcpkiPrivKeyBuild() */


