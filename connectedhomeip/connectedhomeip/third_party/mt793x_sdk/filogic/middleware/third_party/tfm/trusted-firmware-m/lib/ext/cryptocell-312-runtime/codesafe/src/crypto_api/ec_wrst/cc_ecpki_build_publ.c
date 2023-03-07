/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


/************* Include Files ****************/

#include "cc_pal_mem.h"
#include "cc_ecpki_types.h"
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

CEXPORT_C CCError_t CC_EcpkiPublKeyBuildAndCheck(
                                            const CCEcpkiDomain_t   *pDomain,           /*in*/
                                            uint8_t                     *pPubKeyIn,         /*in*/
                                            size_t                      publKeySizeInBytes, /*in*/
                                            ECPublKeyCheckMode_t        checkMode,          /*in*/
                                            CCEcpkiUserPublKey_t    *pUserPublKey,       /*out*/
                                            CCEcpkiBuildTempData_t *tempBuff           /*in*/)

{
        /* FUNCTION DECLARATIONS */

        /* the private key structure pointer */
        CCEcpkiPublKey_t *pPublKey;
        /* EC modulus size in bytes*/
        uint32_t  modSizeInBytes, modSizeInWords;
        /* Point control pc pc and pc1 = pc&6*/
        uint32_t  pc, pc1;
        /* the err return code identifier */
        CCError_t err = CC_OK;
        CCCommonCmpCounter_t cmp;

        /* FUNCTION LOGIC */
        CHECK_AND_RETURN_ERR_UPON_FIPS_ERROR();

        /* ...... checking the validity of the User given pointers ......... */
        if (pUserPublKey == NULL)
                return  CC_ECPKI_BUILD_KEY_INVALID_USER_PUBL_KEY_PTR_ERROR;
        if (pPubKeyIn == NULL)
                return  CC_ECPKI_BUILD_KEY_INVALID_PUBL_KEY_IN_PTR_ERROR;
        if (pDomain == NULL)
                return  CC_ECPKI_DOMAIN_PTR_ERROR;
        /* check input values */
        if (checkMode >= PublKeyChecingOffMode)
                return  CC_ECPKI_BUILD_KEY_INVALID_CHECK_MODE_ERROR;
        if ((checkMode != CheckPointersAndSizesOnly) && (tempBuff == NULL))
                return  CC_ECPKI_BUILD_KEY_INVALID_TEMP_BUFF_PTR_ERROR;

        /* check EC domain parameters sizes */
        if ((pDomain->modSizeInBits > (CC_ECPKI_MODUL_MAX_LENGTH_IN_WORDS * 32)) ||
            (pDomain->ordSizeInBits > (pDomain->modSizeInBits + 1))) {
                return CC_ECPKI_INVALID_DATA_IN_PASSED_STRUCT_ERROR;
        }


        /* ...... Initializations  ............... */

        pPublKey = (CCEcpkiPublKey_t*)((void*)pUserPublKey->PublKeyDbBuff);
        modSizeInBytes = CALC_FULL_BYTES(pDomain->modSizeInBits);
        modSizeInWords = CALC_32BIT_WORDS_FROM_BYTES(modSizeInBytes);

        if(modSizeInWords>=CC_ECPKI_MODUL_MAX_LENGTH_IN_WORDS){ /*To remove static analyzer warning*/
                return CC_ECPKI_INVALID_DATA_IN_PASSED_STRUCT_ERROR;
        }

        /* point control */
        pc = pPubKeyIn[0];
        if (pc >= CC_EC_PointCompresOffMode || pc == CC_EC_PointContWrong)
                return CC_ECPKI_BUILD_KEY_INVALID_COMPRESSION_MODE_ERROR;
        pc1 = pc & 0x6; /*compression. mode*/

        /* preliminary check key size */
        if (pc1 == CC_EC_PointCompressed) {
                if (publKeySizeInBytes != modSizeInBytes + 1)
                        return  CC_ECPKI_BUILD_KEY_INVALID_PUBL_KEY_SIZE_ERROR;
        } else {
                if (publKeySizeInBytes != 2*modSizeInBytes + 1)
                        return  CC_ECPKI_BUILD_KEY_INVALID_PUBL_KEY_SIZE_ERROR;
        }

        /* ...... copy the buffers to the key handle structure ................ */
        /* -------------------------------------------------------------------- */

        /* RL ? clear the public key db */
        CC_PalMemSetZero((uint8_t*)pUserPublKey, sizeof(CCEcpkiUserPublKey_t));

        /* copy public key Xin to X, Yin to Y */
        err = CC_CommonConvertMsbLsbBytesToLswMswWords(pPublKey->x, sizeof(pPublKey->x),
                                                       pPubKeyIn + 1, modSizeInBytes);
        if (err != CC_OK) {
                err = CC_ECPKI_BUILD_KEY_INVALID_PUBL_KEY_DATA_ERROR;
                goto End;
        }

    /* check public key X value */
    cmp = CC_CommonCmpLsWordsUnsignedCounters(pPublKey->x, modSizeInWords,
                             pDomain->ecP, modSizeInWords);
    if(cmp != CC_COMMON_CmpCounter2GreaterThenCounter1) {
        return CC_ECPKI_BUILD_KEY_INVALID_PUBL_KEY_DATA_ERROR;
    }

        if (pc1 == CC_EC_PointUncompressed || pc1 == CC_EC_PointHybrid) {
                /*  PC1 = 4 or PC1 = 6 */
                err = CC_CommonConvertMsbLsbBytesToLswMswWords(
                                                                 pPublKey->y, sizeof(pPublKey->y),
                                                                 pPubKeyIn + 1 + modSizeInBytes, modSizeInBytes);
                if (err != CC_OK) {
                        err = CC_ECPKI_BUILD_KEY_INVALID_PUBL_KEY_DATA_ERROR;
                        goto End;
                }

        /* check public key Y value */
        cmp = CC_CommonCmpLsWordsUnsignedCounters(pPublKey->y, modSizeInWords,
                              pDomain->ecP, modSizeInWords);
        if(cmp != CC_COMMON_CmpCounter2GreaterThenCounter1) {
            return CC_ECPKI_BUILD_KEY_INVALID_PUBL_KEY_DATA_ERROR;
        }

        }

        /* initialize LLF public key   */
        /*-----------------------------*/
        /* copy EC domain */
        CC_PalMemCopy(&pPublKey->domain, pDomain, sizeof(CCEcpkiDomain_t));

        /* Initialization, partly checking and uncompressing (if needed) of the public key */
        err = EcWrstInitPubKey(pPublKey, pc);
        if (err != CC_OK) {
                err = CC_ECPKI_INTERNAL_ERROR;
                goto End;
        }


        /*  additional (full) checking of public key  */
        /*--------------------------------------------*/
        if (checkMode == ECpublKeyFullCheck) {
                err = EcWrstFullCheckPublKey(pPublKey, (uint32_t*)tempBuff);
                if (err != CC_OK) {
                        goto End;
                }
        }


        /* ................ set the private key validation tag ................... */
        pUserPublKey->valid_tag = CC_ECPKI_PUBL_KEY_VALIDATION_TAG;

        End:
        /* if the created structure is not valid - clear it */
        if (err != CC_OK) {
                CC_PalMemSetZero(pUserPublKey, sizeof(CCEcpkiUserPublKey_t));
        }
    if (tempBuff != NULL) {
        CC_PalMemSetZero(tempBuff, sizeof(CCEcpkiBuildTempData_t));
    }

        return err;


} /* End of CC_EcpkiPublKeyBuildAndCheck() */


/***********************************************************************************
 *                     CC_EcpkiPubKeyExport function                           *
 ***********************************************************************************/
/**
  @brief The function converts an existed public key into the big endian and outputs it.

                 The function performs the following steps:
                 - checks input parameters,
                 - Converts the X,Y coordinates of public key EC point to big endianness.
                 - Sets the public key as follows:
                          In case "Uncompressed" point:  PubKey = PC||X||Y, PC = 0x4 - single byte;
                          In other cases returns an error.
                 - Exits.

                 NOTE: - At this stage supported only uncompressed point form,
                       - Size of output X and Y coordinates is equal to ModSizeInBytes.

  @param[in]  pUserPublKey -   A pointer to the public key structure initialized by CC.
  @param[in]  compression  -   An enumerator parameter, defines point compression.
  @param[out] pExportPublKey - A pointer to the buffer for export the public key bytes
                       array in big endian order of bytes. Size of buffer must be
                       not less than 2*ModSiseInBytes+1 bytes.
  @param[in/out] pPublKeySizeBytes - A pointer to size of the user passed
                       public key buffer (in) and the actual size of exported
                       public key (out).

  @return CCError_t - CC_OK,
                        CC_ECPKI_EXPORT_PUBL_KEY_INVALID_USER_PUBL_KEY_PTR_ERROR
                        CC_ECPKI_EXPORT_PUBL_KEY_ILLEGAL_COMPRESSION_MODE_ERROR
                        CC_ECPKI_EXPORT_PUBL_KEY_INVALID_EXTERN_PUBL_KEY_PTR_ERROR
                        CC_ECPKI_EXPORT_PUBL_KEY_INVALID_PUBL_KEY_SIZE_PTR_ERROR
                        CC_ECPKI_EXPORT_PUBL_KEY_INVALID_PUBL_KEY_SIZE_ERROR
                        CC_ECPKI_EXPORT_PUBL_KEY_ILLEGAL_DOMAIN_ID_ERROR
*/
CEXPORT_C CCError_t CC_EcpkiPubKeyExport(
                                              CCEcpkiUserPublKey_t      *pUserPublKey,       /*in*/
                                              CCEcpkiPointCompression_t  compression,        /*in*/
                                              uint8_t                       *pExportPublKey,     /*in*/
                                              size_t                        *pPublKeySizeBytes   /*in/out*/)
{
        /*-------------------- FUNCTION DECLARATIONS ------------------------*/

        /* the private key structure pointer */
        CCEcpkiPublKey_t *publKey;

        /* EC modulus size in words and in bytes*/
        uint32_t   modSizeInBytes, modSizeInWords;
        uint8_t    yBit;

        /* the err return code identifier */
        CCError_t err = CC_OK;

        /*............. Checking input parameters   ..............................*/
        CHECK_AND_RETURN_ERR_UPON_FIPS_ERROR();

        if (pUserPublKey == NULL)
                return  CC_ECPKI_EXPORT_PUBL_KEY_INVALID_USER_PUBL_KEY_PTR_ERROR;

        if (pExportPublKey == NULL)
                return  CC_ECPKI_EXPORT_PUBL_KEY_INVALID_EXTERN_PUBL_KEY_PTR_ERROR;

        if (pUserPublKey->valid_tag != CC_ECPKI_PUBL_KEY_VALIDATION_TAG)
                return CC_ECPKI_EXPORT_PUBL_KEY_ILLEGAL_VALIDATION_TAG_ERROR;

        if (pPublKeySizeBytes == NULL)
                return  CC_ECPKI_EXPORT_PUBL_KEY_INVALID_PUBL_KEY_SIZE_PTR_ERROR;

        if (compression == CC_EC_PointContWrong || compression >= CC_EC_PointCompresOffMode)
                return CC_ECPKI_EXPORT_PUBL_KEY_ILLEGAL_COMPRESSION_MODE_ERROR;


        /*   FUNCTION LOGIC  */

        publKey = (CCEcpkiPublKey_t *)((void*)pUserPublKey->PublKeyDbBuff);

        /* EC modulus size */
        modSizeInBytes = CALC_FULL_BYTES(publKey->domain.modSizeInBits);
        modSizeInWords = CALC_FULL_32BIT_WORDS(publKey->domain.modSizeInBits);

        /* calc. MS Bit of Y */
        yBit = (uint8_t)(publKey->y[0] & 1);

        /* Convert public key to big endianness export form */
        switch (compression) {
        case CC_EC_PointCompressed:

                if (*pPublKeySizeBytes < modSizeInBytes + 1)
                        return  CC_ECPKI_EXPORT_PUBL_KEY_INVALID_PUBL_KEY_SIZE_ERROR;

                /* point control byte */
                pExportPublKey[0] = 0x02 | yBit;

                err = CC_CommonConvertLswMswWordsToMsbLsbBytes(
                                                                 pExportPublKey + 1, 4*modSizeInWords,
                                                                 publKey->x, modSizeInBytes);
                if (err != CC_OK) {
                        err = CC_ECPKI_EXPORT_PUBL_KEY_INVALID_PUBL_KEY_DATA_ERROR;
                        goto End;
                }

                *pPublKeySizeBytes = modSizeInBytes + 1;
                break;

        case CC_EC_PointUncompressed:
        case CC_EC_PointHybrid:

                if (*pPublKeySizeBytes < 2*modSizeInBytes + 1)
                        return  CC_ECPKI_EXPORT_PUBL_KEY_INVALID_PUBL_KEY_SIZE_ERROR;

                /* Point control byte */
                if (compression == CC_EC_PointUncompressed)
                        pExportPublKey[0] = 0x04;
                else
                        pExportPublKey[0] = (0x06 | yBit);

                err = CC_CommonConvertLswMswWordsToMsbLsbBytes(
                                                                 pExportPublKey + 1, 4*((modSizeInBytes+3)/4),
                                                                 publKey->x, modSizeInBytes );
                if (err != CC_OK) {
                        err = CC_ECPKI_EXPORT_PUBL_KEY_INVALID_PUBL_KEY_DATA_ERROR;
                        goto End;
                }
                err = CC_CommonConvertLswMswWordsToMsbLsbBytes(
                                                                 pExportPublKey + 1 + modSizeInBytes, 4*((modSizeInBytes+3)/4),
                                                                 publKey->y, modSizeInBytes );
                if (err != CC_OK) {
                        err = CC_ECPKI_EXPORT_PUBL_KEY_INVALID_PUBL_KEY_DATA_ERROR;
                        goto End;
                }

                /* Set publKeySizeInBytes */
                *pPublKeySizeBytes = 2*modSizeInBytes + 1;
                break;

        default:
                return CC_ECPKI_EXPORT_PUBL_KEY_ILLEGAL_COMPRESSION_MODE_ERROR;
        }
        End:
        if (err != CC_OK) {
                CC_PalMemSetZero(pExportPublKey, *pPublKeySizeBytes);
                *pPublKeySizeBytes = 0;
        }
        return err;

} /* End of CC_EcpkiPubKeyExport */



