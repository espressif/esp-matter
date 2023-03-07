/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */



/************* Include Files ****************/

#include "cc_pal_mem.h"
#include "cc_ecpki_error.h"
#include "cc_ecpki_local.h"
#include "ec_wrst.h"
#include "cc_fips_defs.h"

/************************ Defines *************************************/

/************************ Enums ***************************************/

/************************ Typedefs ************************************/

/************************ Global Data *********************************/

/************* Private function prototype *****************************/


/************************ Public Functions ****************************/


/***********************************************************************
 *               CC_EcdhSvdpDh function                            *
 ***********************************************************************/
CEXPORT_C CCError_t CC_EcdhSvdpDh(
                                       CCEcpkiUserPublKey_t *PartnerPublKey_ptr,        /*in*/
                                       CCEcpkiUserPrivKey_t *UserPrivKey_ptr,           /*in*/
                                       uint8_t                  *SharedSecretValue_ptr,     /*out*/
                                       size_t                   *SharedSecrValSize_ptr,     /*in/out*/
                                       CCEcdhTempData_t     *TempBuff_ptr               /*in*/ )
{
        /* LOCAL INITIALIZATIONS AND DECLERATIONS */

        /* the error identifier */
        CCError_t Error = CC_OK;

        CCEcpkiPublKey_t *PublKey_ptr;
        CCEcpkiPrivKey_t *PrivKey_ptr;

        /*  pointer to the current Domain structure */
        CCEcpkiDomain_t *pDomain, *pPublDomain;
        uint32_t modSizeInBytes;

    CHECK_AND_RETURN_ERR_UPON_FIPS_ERROR();

        /* ...... checking the validity of the user private key pointer .......... */
        if (UserPrivKey_ptr == NULL)
                return CC_ECDH_SVDP_DH_INVALID_USER_PRIV_KEY_PTR_ERROR;

        /* ...... checking the valid tag of the user private key pointer ......... */
        if (UserPrivKey_ptr->valid_tag != CC_ECPKI_PRIV_KEY_VALIDATION_TAG)
                return CC_ECDH_SVDP_DH_USER_PRIV_KEY_VALID_TAG_ERROR;

        /* .... checking the validity of the other partner public key pointer .... */
        if (PartnerPublKey_ptr == NULL)
                return CC_ECDH_SVDP_DH_INVALID_PARTNER_PUBL_KEY_PTR_ERROR;

        /* ...... checking the valid tag of the user private key pointer ......... */
        if (PartnerPublKey_ptr->valid_tag != CC_ECPKI_PUBL_KEY_VALIDATION_TAG)
                return CC_ECDH_SVDP_DH_PARTNER_PUBL_KEY_VALID_TAG_ERROR;

        /* ...... checking the validity of the SharedSecretValue pointer ..........*/
        if (SharedSecretValue_ptr == NULL)
                return CC_ECDH_SVDP_DH_INVALID_SHARED_SECRET_VALUE_PTR_ERROR;

        /* ...... checking the validity of SharedSecrValSize_ptr pointer ......... */
        if (SharedSecrValSize_ptr == NULL)
                return CC_ECDH_SVDP_DH_INVALID_TEMP_DATA_PTR_ERROR;

        /* ...... checking the validity of temp buffers         .................. */
        if (TempBuff_ptr == NULL)
                return CC_ECDH_SVDP_DH_INVALID_SHARED_SECRET_VALUE_SIZE_PTR_ERROR;

        /* ..  initializtions  and other checking   .... */
        /* --------------------------------------------- */

        /* derive  public and private keys pointers */
        PublKey_ptr = (CCEcpkiPublKey_t*)&PartnerPublKey_ptr->PublKeyDbBuff;
        PrivKey_ptr = (CCEcpkiPrivKey_t*)&UserPrivKey_ptr->PrivKeyDbBuff;

        /* the pointers to private and public keys domains */
        pDomain = &PrivKey_ptr->domain;
        pPublDomain = &PublKey_ptr->domain;

        /* if domains are not identical, return an error */
        if(CC_PalMemCmp(pDomain, pPublDomain, sizeof(CCEcpkiDomain_t))) {
                return CC_ECDH_SVDP_DH_NOT_CONCENT_PUBL_AND_PRIV_DOMAIN_ID_ERROR;
        }

        /* modulus size */
        modSizeInBytes = CALC_FULL_BYTES(pDomain->modSizeInBits);

        /*  check the size of the buffer for Shared value  */
        if (*SharedSecrValSize_ptr < modSizeInBytes) {
                *SharedSecrValSize_ptr = modSizeInBytes;
                return CC_ECDH_SVDP_DH_INVALID_SHARED_SECRET_VALUE_SIZE_ERROR;
        }

        /* performing DH operations by calling  EcWrstDhDeriveSharedSecret() function */
        /*------------------------------------------------------------------*/
        Error = EcWrstDhDeriveSharedSecret(
                                 PublKey_ptr, PrivKey_ptr,
                                 SharedSecretValue_ptr,
                                 TempBuff_ptr);

        if (Error != CC_OK)
                goto End;

        /* Set SharedSecrValSize = ModSizeInWords  for user control */
        *SharedSecrValSize_ptr = modSizeInBytes;

End:
        if (Error != CC_OK) {
        CC_PalMemSetZero(SharedSecretValue_ptr, *SharedSecrValSize_ptr);
        *SharedSecrValSize_ptr = 0;
    }
    CC_PalMemSetZero(TempBuff_ptr, sizeof(CCEcdhTempData_t));

        return Error;

}/* END OF CC_EcdhSvdpDh */

