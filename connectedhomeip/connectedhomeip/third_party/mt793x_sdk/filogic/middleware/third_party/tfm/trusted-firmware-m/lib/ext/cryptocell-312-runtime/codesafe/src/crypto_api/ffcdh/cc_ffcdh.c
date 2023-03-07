/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/************* Include Files ****************/
#include "cc_bitops.h"
#include "cc_pal_types_plat.h"
#include "cc_pal_types.h"
#include "cc_pal_mem.h"
#include "cc_error.h"
#include "cc_common.h"
#include "cc_common_math.h"
#include "cc_rnd_common.h"
#include "cc_rnd_error.h"
#include "cc_rnd_local.h"
#include "cc_hash_defs.h"
#include "cc_general_defs.h"
#include "mbedtls/md.h"
#include "mbedtls_cc_hkdf.h"
#include "cc_ffcdh_error.h"
#include "cc_ffcdh.h"
#include "cc_ffc_domain.h"
#include "cc_ffcdh.h"
#include "cc_ffcdh_local.h"
#include "cc_fips_defs.h"
#include "pki.h"

/************************ Defines *******************************/

/************************ Enums *********************************/

/************************ macros ********************************/

/************************ global data ***********************************/

extern CCError_t FfcGetHashMode(
                    CCHashOperationMode_t *pHashMode, /* optional */
                    mbedtls_hkdf_hashmode_t *pHkdfHashMode, /* optional */
                    uint32_t *pBlockSize,              /* optional */
                    uint32_t *pDigestSize,             /* optional */
                    CCFfcHashOpMode_t ffcHashMode);    /* ffc HASH mode */


extern CCError_t FfcGetDomainSizes(
                    uint32_t *pMaxSecurStrength,        /* Maximum security strength supported, in bits. */
                    uint32_t *pPrimeLen,                /* Field (prime P) length in bytes. */
                    uint32_t *pOrderLen,                /* Subgroup order Q length in bytes. */
                    uint32_t *pMinHashLen,              /* Minimum length of HASH output in bytes. */
                    CCFfcParamSetId_t  ffcParamSetId);  /* Enum. defining set of lengths of domain parameters, approved
                                                         by SP 800-56A and FIPS 186-4 standards */

/************************ Private Functions ******************************/

#ifdef FFC_FURTHER_USING
/* The function converts little endianness bytes array to big endianness without leading zeros.
 * Note: The function is not constant-time and intended for work with non secure data.
 *       Overlapping of buffers is not allowed.  */
static CCError_t LeBytesArrayToBeBytesWithoutZeros (
                                uint8_t *pBeDst,   /* LE destination */
                                uint8_t *pLeSrc,   /* BE source */
                                size_t *pSizeBytes /* pointer to size: in - buffer size,
                                                       out -  actual size without leading zeros */
)
{
    CCError_t err = CC_OK;
    int32_t i, j;

    CHECK_AND_SET_ERROR((*pSizeBytes == 0), CC_FFCDH_INVALID_ARGUMENT_SIZE_ERROR);

        j = 0;
        i = *pSizeBytes-1;
        while (pLeSrc[i] == 0) {  /* remove leading zeros */
            i--;
        }

        /* copy actual data */
        for (; i>=0; i--) {
            pBeDst[j++] = pLeSrc[i];
        }

        *pSizeBytes = j; /* output actual size */
End:
        return err;
}
#endif

#ifdef FFC_FURTHER_USING
/* The function converts big endianness bytes array to little endianness without leading zeros.
 * Note: The function is not constant-time and intended for work with non secure data */
static void BeBytesArrayToLeEBytes (
                                uint8_t  *pLeDst, /* LE destination */
                                uint8_t  *pBeSrc, /* BE source */
                                uint32_t *pSizeBytes) /* pointer to size: in - buffer size,
                                                         out -  actual size without leading zeros */
{
        int32_t i, j;
        int32_t count0 = 0;

        j = 0;

        if(*pSizeBytes == 0)
            return;

        /* remove leading zeros */
    while (pBeSrc[count0] == 0) {
        count0++;
    }

    /* copy actual data */
    for (i = *pSizeBytes - 1; i >= count0; i--) {
            pLeDst[j++] = pBeSrc[i];
    }
    *pSizeBytes = *pSizeBytes -count0;

    return;
}
#endif


#ifdef FFC_FURTHER_USING
/*******************************************************************************
 *             CC_CommonGetBytesCounterEffectiveSizeInBits                  *
 *******************************************************************************
 *
 * @brief This function returns the effective number of bits in the byte stream counter
 *        ( searching the highest '1' in the counter )
 *
 *        Assumed, that MSB of the counter is stored in the first cell in the
 *         array. For example, the value of the 8-Bytes numberer B is :
 *             B[0]<<56 | B[1]<<48 ............ B[6]<<8 | B[7] .
 *
 * @param[in] pCounter -  The counter buffer.
 * @param[in] CounterSize -  the counter size in bytes.
 *
 * @return result - The effective counters size in bits.
 */

static uint32_t FfcDhGetBeCounterEffectiveSizeInBits(const uint8_t *pCounterBuffp,
                                                     uint32_t *pCountLeadingZeros,
                                                     uint32_t counterSize)
{
    /* FUNCTION LOCAL DECLARATIONS */

    /* loop variable */
    int32_t i;

    /* the effective size in bits */
    uint32_t effectSizeInBits;

    /* the effective MS byte (the one that is not zero) */
    uint8_t effectMsByteVal;

    /* FUNCTION LOGIC */

    /* STEP1 : a loop for adjusting the counter size by neglecting the MSB zeros */
        *pCountLeadingZeros = 0
    while (i < counterSize) {
        if (pCounterBuff[i] != 0)
            break;
        else
            *pCountLeadingZeros++;
    }

    /* STEP2 : if counter size is 0 - return 0 */
    if (CounterSize == 0)
        return 0;

    /* set the effective MS byte */
    effectMsByteVal = pCounterBuff[i];

    /* initialize the effective size as the counters size (with MSB zeros) */
    counterEffectSizeInBits = counterSize * 8;

    /* STEP 3 : adjusting the effective size in bits */
    for (i = 0; i < 8; i++) {
        if (effectMsByteVal & 0x80)
            break;

        counterEffectSizeInBits--;
        effectMsByteVal <<= 1;

    }/* end of adjusting the effective size in bits loop */

    return counterEffectSizeInBits;

}/* END OF CC_DhGetBeCounterEffectiveSizeInBits */
#endif

/* The function writes the separate bytes-array (Src) into the Dst buffer.
 *
 * Of first, he function sets the string length as first 2-bytes big endianness counter,
 * then writes the data, promote the pointer to next after the data position. If
 * pointer to FullLen != NULL, then add the size of written source to FullLen value.
 *
 * \note: Assumed that all parameters, including possible buffers overflow, are
 * checked and correct.
 * \note: Dst buffer should be given by allocated pointer to pointer and not by
 *  address of the buffer address.
 */
static void FfcDhWriteBufferBeToBe(
                uint8_t **ppDst,      /* [in/out] pointer to pointer to destination buffer:
                                         in - start address, out - next address after writing data. */
                uint8_t  *pSrc,       /* [in] pointer to source buffer */
                uint32_t  curLen,     /* [in] size of source buffer in bytes. */
                uint32_t *pFullLen)   /* [in/out] pointer to accumulated full length of
                                          written data. */
{
    /* set length and data */
    (*ppDst)[0] = (uint8_t)((curLen>>8) & 0xFF); (*ppDst)[1] = (uint8_t)(curLen & 0xFF);
    *ppDst += CC_FFCDH_LENGTH_COUNTER_SIZE_IN_BYTES;
    CC_PalMemCopy(*ppDst, pSrc, curLen);

    /* update the Dst pointer */
    *ppDst += curLen;
    if(pFullLen != NULL){
        *pFullLen += (curLen + CC_FFCDH_LENGTH_COUNTER_SIZE_IN_BYTES);
    }

    return;
}


/* The function writes the separate LE words Src array into the Dst BE bytes buffer.
 *
 * Of first, the function sets the Src length as 2-bytes big endianness length-counter,
 * then writes the data, converted to BE bytes and promote the pointer to next position
 * after the data . If pointer to FullLen != NULL, then the function adds the size of
 * source to FullLen value.
 *
 * \note: Assumed that all parameters, including possible buffers overflow, are
 * checked and correct.
 * \note: Dst buffer should be given by allocated pointer to pointer and not by
 *  address of the buffer address.
 */
static void FfcDhWriteBufferLeToBe(
                uint8_t **ppDst,      /* [in/out] pointer to pointer to destination buffer:
                                         in - start address, out - next address after writing data. */
                uint32_t *pSrc,       /* [in] pointer to source buffer, given as LE words array. */
                uint32_t  curLen,     /* [in] size of source in words. */
                uint32_t *pFullLen)   /* [in/out] pointer to accumulated full length (in bytes) of
                                          all written data. */
{
    int32_t i, j;
    curLen = curLen << 2; /* now size is in bytes*/
    /* set length and data */
    (*ppDst)[0] = (uint8_t)((curLen>>8) & 0xFF); (*ppDst)[1] = (uint8_t)(curLen & 0xFF);
    *ppDst += CC_FFCDH_LENGTH_COUNTER_SIZE_IN_BYTES;
//  CHECK_AND_SET_ERROR(CC_CommonConvertLswMswWordsToMsbLsbBytes(*ppDst, curLen, pSrc, curLen));

    for (i = curLen - 1, j = 0; i >= 0; i--, j++) {
        (*ppDst)[i] = (uint8_t)(pSrc[j / CC_32BIT_WORD_SIZE] >> ((j % CC_32BIT_WORD_SIZE) << 3));
    }

    /* update the Dst pointer */
    *ppDst += curLen;
    if(pFullLen != NULL){
        *pFullLen += (curLen + CC_FFCDH_LENGTH_COUNTER_SIZE_IN_BYTES);
    }

    return;
}


#ifdef FFC_FURTHER_USING
/* The function writes the byte-string Src into the Dst buffer with
 * setting the length as first 2-bytes counter and control of remaining
 * size of destination buffer.
 * \note: The function promotes the destination pointer by written bytes and
 * decreases the remaining size */
static CCError_t FfcDhWriteBuffers(uint8_t **ppDst, uint8_t *pSrc,
                           size_t len, size_t *pRemainLen)
{
    if(ppDst == NULL || *ppDst == NULL)
        return CC_FFCDH_INVALID_ARGUMENT_POINTER_ERROR;
    if(len + 2 > *pRemainLen)
        return CC_FFCDH_BUFFER_OVERFLOW_ERROR;
    /* set length and data */
    (*ppDst)[0] = (uint8_t)((len>>1) & 0xFF); (*ppDst)[1] = (uint8_t)(len & 0xFF);
    *ppDst += CC_FFCDH_LENGTH_COUNTER_SIZE_IN_BYTES;

    if(pSrc != NULL && len != 0) {
        CC_PalMemCopy(*ppDst, pSrc, len);
    }
    /* update the Dst pointer and Remaining length */
    *ppDst += len;
    *pRemainLen -= (len + CC_FFCDH_LENGTH_COUNTER_SIZE_IN_BYTES);

    return CC_OK;
}


/*! The function copies "len" bytes from pSrc to pDst buffer and sets
 *  offset of pDst buffer from base address to pCurrOffset. */
static void FfcDhWriteBufferAndOffset(uint8_t **ppDst, uint8_t *pSrc,
                              size_t len, uint16_t *pCurrOffset, uint8_t *pBaseAddr)
{
    *pCurrOffset = (uint16_t)((*ppDst - pBaseAddr)&0xFFFF);
    /* set length and data */
    (*ppDst)[0] = (uint8_t)((len>>1) & 0xFF); (*ppDst)[1] = (uint8_t)(len & 0xFF);
    *ppDst += CC_FFCDH_LENGTH_COUNTER_SIZE_IN_BYTES;

    if(len != 0) {
        CC_PalMemCopy(*ppDst, pSrc, len);
    }
    /* update the Dst pointer and Offset of next buffer */
    *ppDst += len;

    return;
}
#endif

#ifdef FFC_FURTHER_USING
static CCError_t FfcGetBufferSize(uint8_t **ppSrc, size_t *pLen, size_t *pRemainLen)
{
    size_t len;
    len = (((size_t)((*ppSrc)[0])) << 8) | (*ppSrc)[1];
    if(len > 0) {
        if((*pRemainLen < CC_FFCDH_LENGTH_COUNTER_SIZE_IN_BYTES) ||
           (*pRemainLen - CC_FFCDH_LENGTH_COUNTER_SIZE_IN_BYTES < len)) {
            return CC_FFCDH_BUFFER_OVERFLOW_ERROR;
        }
    }
    *pRemainLen -= CC_FFCDH_LENGTH_COUNTER_SIZE_IN_BYTES;
    *ppSrc += CC_FFCDH_LENGTH_COUNTER_SIZE_IN_BYTES;
    *pLen = len;

    return CC_OK;
}
#endif

/************************ Public Functions ******************************/

/*******************************************************************************************/
/*! The functions initializes the DH Context structure:
<li> zeroes context buffers, initializes 3 MS bytes of validation tag by context ID and sets LS byte
to zero to prepare it for further indications of setting appropriate parts of data into context
*/
CEXPORT_C CCError_t  CC_FfcDhInitCtx( CCFfcDhUserContext_t *pDhUserCtx)
{
       /* FUNCTION DECLARATIONS */

        /* The return error identifier */
        CCError_t err = CC_OK;

    CHECK_AND_RETURN_ERR_UPON_FIPS_ERROR();

        /* check contexts pointers */
    CHECK_AND_SET_ERROR(pDhUserCtx == NULL, CC_FFCDH_INVALID_CONTEXT_PTR_ERROR);

        /* zeroing the Context */
        CC_PalMemSetZero((uint8_t*)pDhUserCtx, sizeof(CCFfcDhUserContext_t));

        /* check that DhCtx is initialized to zero */
        pDhUserCtx->validTag = FFCDH_CTX_VALID_TAG_INIT_VAL;

End:
        return err;
}


/*******************************************************************************************/
/*! The functions destroys (zeroes) the DH Context structure.

@return CC_OK on success.
@return A non-zero value on failure as defined cc_dh_error.h.

*/
CEXPORT_C CCError_t  CC_FfcDhFreeCtx( CCFfcDhUserContext_t *pDhUserCtx)
{
       /* FUNCTION DECLARATIONS */

        /* The return error identifier */
        CCError_t err = CC_OK;

    CHECK_AND_RETURN_ERR_UPON_FIPS_ERROR();

        /* check contexts pointers */
    CHECK_AND_SET_ERROR(pDhUserCtx == NULL, CC_FFCDH_INVALID_CONTEXT_PTR_ERROR);

        /* zeroing the Context */
        CC_PalMemSetZero((uint8_t*)pDhUserCtx, sizeof(CCFfcDhUserContext_t));

End:
        return err;
}



/**************************************************************/
/* The function calculates internal structures FfcDhSchemeInfo_t and FfcDhSchemeDataOffsets_t,
 * describing which parameters and data are used in the scheme and where appropriate  entries of
 * the data are saved in the Context.
 *
 * When some member of the structure FfcDhSchemeInfo_t in the context is equal to 1,
 * then appropriate parameter is needed for the Scheme and if it is 0 - not.
 * The places of the data entries are given by offsets in FfcDhSchemeDataOffsets_t structure.
 * Assumed: all input parameters are checked and set correctly.
 *
 * Note: Offsets of OtherInfo Sub-Entries given from base address = &pDhCth->extendDataBuffer[0];
 */
static CCError_t FfcDhSchemeInfo(
            DhContext_t *pDhCtx,   /*!< [in/out] pointer to internal DH context structure */
            uint8_t *pAlgId,       /*!< [in] pointer to Algorithm ID agreed by both parties. */
            size_t algIdSize,      /*!< [in] size of Algorithm ID in bytes. Should be less than
                            CC_FFCDH_MAX_SIZE_OF_ALG_ID_SUB_ENTRY. */
            uint8_t *pUserId,      /*!< [in] pointer to the user ID - a distinct identifier of the user. */
            size_t userIdSize,     /*!< [in] size of the user ID in bytes. */
            uint8_t *pPartnId,     /*!< [in] pointer to the partner ID - a distinct identifier of the party. */
            size_t partnIdSize)     /*!< [in] size of the partner ID in bytes. */
{

    /* FUNCTION DECLARATIONS */

    uint32_t publKeySize;
//  uint32_t nonceSize;
    /* pointer to structure, containing information about input
     * parameters needed, for chosen Scheme, party role (U,V) and
     * confirmation role (provider, recipient). */
    FfcDhSchemeInfo_t *schemeInfo = &pDhCtx->schemeInfo;
    /* Structure, containing offsets and sizes of  parties Info */
    FfcDhSchemeDataOffsets_t *pDataOffsets = &pDhCtx->dataOffsets;
    CCFfcDhSchemeId_t dhSchemeId = pDhCtx->dhSchemeId;
        CCFfcDhUserPartyIs_t userParty = pDhCtx->userParty;
        CCFfcDhUserConfirmMode_t confirmMode = pDhCtx->confirmMode;
        uint8_t *pBaseAddr = (uint8_t*)&pDhCtx->extendDataBuffer;


    publKeySize = pDhCtx->ffcDomain.modLenWords*CC_32BIT_WORD_SIZE;
    pDhCtx->nonceSize = pDhCtx->ffcDomain.ordLenWords*CC_32BIT_WORD_SIZE;
//  pDhCtx->macTagSize = DH_SHA_PARAMETERS_SIZES_IN_BYTES[pDhCtx->];

    /* FUNCTION  logic */

    /* zeroing of schemeInfo */
    CC_PalMemSetZero(schemeInfo, sizeof(FfcDhSchemeInfo_t));

    /* calculate buffers offsets, dependent on user and partner roles. */
    /* Note: these offsets points directly on data (no length counter before) */
    pDataOffsets->kdfCounterOffset = 0; /*!< KDF counter offset: is a base=0 for counting other offsets */
    /* set shared secret value size for Hybrid modes = 2*primeSize and for other modes = 1*primeSize. */
    pDataOffsets->sharedSecrSize = publKeySize*(1 + ((dhSchemeId == CC_FFCDH_SCHEM_HYBRID_ONE_FLOW) || \
                                                 (dhSchemeId == CC_FFCDH_SCHEM_HYBRID1)));
    pDataOffsets->sharedSecrOffset = pDataOffsets->kdfCounterOffset + CC_FFCDH_KDF_COUNTER_SIZE_IN_BYTES;
    /* The following offsets points on the length counters of OtherInfo entries */
    pDataOffsets->algIdOffset = FFCDH_SET_OFFSET(pDataOffsets->sharedSecrOffset, pDataOffsets->sharedSecrSize);

    /*-------------------------------------------------------------*/
    /* set Scheme Info indication values about required parameters */
    /*-------------------------------------------------------------*/
    if(userParty == CC_FFCDH_PARTY_U) {
        switch(dhSchemeId) {
        case CC_FFCDH_SCHEM_HYBRID1: /* user: stat=1,ephem=1,nonce=0;  partn: stat=1,ephem=1,nonce=0; */
            /* set keys used info */
            schemeInfo->doUserStatKey = 1;
            schemeInfo->doUserEphemKey = 1;
            schemeInfo->doPartnerStatKey = 1;
            schemeInfo->doPartnerEphemKey = 1;

            /* set confirmation roles and nonces info */
            if(confirmMode == CC_FFCDH_CONFIRM_NOT_USED)
                break;
            schemeInfo->doConfirmProvid = /* = bilateral||VtoU */
                ((confirmMode == CC_FFCDH_CONFIRM_U_TO_V) || (confirmMode == CC_FFCDH_CONFIRM_BILATERAL));
            schemeInfo->doConfirmRecip =  /* = bilateral||UtoV */
                ((confirmMode == CC_FFCDH_CONFIRM_V_TO_U) || (confirmMode == CC_FFCDH_CONFIRM_BILATERAL));
            break;

        case CC_FFCDH_SCHEM_EPHEM: /* user: stat=0,ephem=1,nonce=0; partn: stat=0,ephem=1,nonce=0; */
            /* set keys used info */
            schemeInfo->doUserEphemKey = 1;
            schemeInfo->doPartnerEphemKey = 1;
            /* no confirmation */
            break;

        case CC_FFCDH_SCHEM_HYBRID_ONE_FLOW: /* user: stat=1,ephem=1,nonce=0;  partn: stat=1,ephem=0,nonce=(confirm==VtoU); */
            /* set keys used info */
            schemeInfo->doUserStatKey = 1;
            schemeInfo->doUserEphemKey = 1;
            schemeInfo->doPartnerStatKey = 1;

            /* set confirmation roles and nonces info */
            if(confirmMode == CC_FFCDH_CONFIRM_NOT_USED)
                break;
            schemeInfo->doConfirmProvid = /* = bilateral||VtoU */
                ((confirmMode == CC_FFCDH_CONFIRM_U_TO_V) || (confirmMode == CC_FFCDH_CONFIRM_BILATERAL));
            schemeInfo->doConfirmRecip =  /* = bilateral||UtoV */
                ((confirmMode == CC_FFCDH_CONFIRM_V_TO_U) || (confirmMode == CC_FFCDH_CONFIRM_BILATERAL));
            schemeInfo->doPartnerNonce = schemeInfo->doConfirmProvid;
            break;

        case CC_FFCDH_SCHEM_ONE_FLOW: /* user: stat=0,ephem=1,nonce=0;  partn: stat=1,ephem=0,nonce=0 */
            /* set keys used info */
            schemeInfo->doUserEphemKey = 1;
            schemeInfo->doPartnerStatKey = 1;

            /* set confirmation roles and nonces info */
            if(confirmMode == CC_FFCDH_CONFIRM_NOT_USED)
                break;
            if(confirmMode != CC_FFCDH_CONFIRM_V_TO_U) {
                return CC_FFCDH_INVALID_CONFIRM_MODE_ERROR;
            } else {
                schemeInfo->doConfirmRecip = 1;  /*user is recipient*/
                schemeInfo->doPartnerNonce = 1;
            }
            break;

        case CC_FFCDH_SCHEM_STATIC: /* user: stat=1,ephem=1,nonce=0;  partn: stat=1,ephem=0,nonce=(confirm?); */
            /* set keys used info */
            schemeInfo->doUserStatKey = 1;
            schemeInfo->doPartnerStatKey = 1;

            /* set confirmation roles and nonces info */
            if(confirmMode == CC_FFCDH_CONFIRM_NOT_USED)
                break;
            schemeInfo->doConfirmProvid = /* = bilateral||UtoV */
                ((confirmMode == CC_FFCDH_CONFIRM_U_TO_V) || (confirmMode == CC_FFCDH_CONFIRM_BILATERAL));
            schemeInfo->doConfirmRecip =  /* = bilateral||UtoV */
                ((confirmMode == CC_FFCDH_CONFIRM_V_TO_U) || (confirmMode == CC_FFCDH_CONFIRM_BILATERAL));
            schemeInfo->doUserNonce = 1; /* always = 1: see sec.6.3.3.1*/
            schemeInfo->doPartnerNonce = schemeInfo->doConfirmProvid;
            break;

        default:
            return CC_FFCDH_INVALID_SCHEM_ID_ERROR;

            /* calculate user and partner Info data offsets (starts from Party ID). */
//          pDataOffsets->userIdOffset = FFCDH_SET_OFFSET(pDataOffsets->algIdOffset, algIdSize);
//          pDataOffsets->partnIdOffset= FFCDH_SET_OFFSET(pDataOffsets->algIdOffset, CC_FFCDH_MAX_SIZE_OF_PARTY_INFO_BYTES);
        }
    } else { /* userParty == CC_FFCDH_PARTY_V */
        switch(dhSchemeId) {
        case CC_FFCDH_SCHEM_HYBRID1:
            /* set keys used info */
            schemeInfo->doUserStatKey = 1;
            schemeInfo->doUserEphemKey = 1;
            schemeInfo->doPartnerStatKey = 1;
            schemeInfo->doPartnerEphemKey = 1;

            /* set confirmation roles and nonce info */
            if(confirmMode == CC_FFCDH_CONFIRM_NOT_USED)
                break;
            schemeInfo->doConfirmProvid =
                ((confirmMode == CC_FFCDH_CONFIRM_V_TO_U) || (confirmMode == CC_FFCDH_CONFIRM_BILATERAL));
            schemeInfo->doConfirmRecip =
                ((confirmMode == CC_FFCDH_CONFIRM_U_TO_V) || (confirmMode == CC_FFCDH_CONFIRM_BILATERAL));
            break;

        case CC_FFCDH_SCHEM_EPHEM:
            /* set keys used info */
            schemeInfo->doUserEphemKey = 1;
            schemeInfo->doPartnerEphemKey = 1;
            /* no confirmation */
            break;

        case CC_FFCDH_SCHEM_HYBRID_ONE_FLOW:
            /* set keys used info */
            schemeInfo->doUserStatKey = 1; /* ephem=0*/
            schemeInfo->doPartnerStatKey = 1;
            schemeInfo->doPartnerEphemKey = 1;

            /* set confirmation roles and nonces info */
            if(confirmMode == CC_FFCDH_CONFIRM_NOT_USED)
                break;
            schemeInfo->doConfirmProvid = /* = bilateral||VtoU */
                ((confirmMode == CC_FFCDH_CONFIRM_V_TO_U) || (confirmMode == CC_FFCDH_CONFIRM_BILATERAL));
            schemeInfo->doConfirmRecip =  /* = bilateral||UtoV */
                ((confirmMode == CC_FFCDH_CONFIRM_U_TO_V) || (confirmMode == CC_FFCDH_CONFIRM_BILATERAL));
            schemeInfo->doUserNonce = schemeInfo->doConfirmRecip;
            break;

        case CC_FFCDH_SCHEM_ONE_FLOW: /* user: stat=1,ephem=0,nonce=0;  partn: stat=0,ephem=1,nonce=0 */
            /* set keys used info */
            schemeInfo->doUserStatKey = 1;
            schemeInfo->doPartnerEphemKey = 1;

            /* set confirmation roles and nonces info */
            if(confirmMode == CC_FFCDH_CONFIRM_NOT_USED)
                break;
            if((confirmMode != CC_FFCDH_CONFIRM_V_TO_U)) {
                return CC_FFCDH_INVALID_CONFIRM_MODE_ERROR;
            } else {
                schemeInfo->doConfirmProvid = 1; /*user is provider*/
                schemeInfo->doUserNonce = 1;
            }
            break;

        case CC_FFCDH_SCHEM_STATIC: /* user: stat=1,ephem=1,nonce=0;  partn: stat=1,ephem=0,nonce=(confirm?); */
            /* set keys used info */
            schemeInfo->doUserStatKey = 1;
            schemeInfo->doPartnerStatKey = 1;

            /* set confirmation roles and nonces info */
            if(confirmMode == CC_FFCDH_CONFIRM_NOT_USED)
                break;
            schemeInfo->doConfirmProvid = /* = bilateral||VtoU */
                ((confirmMode == CC_FFCDH_CONFIRM_V_TO_U) || (confirmMode == CC_FFCDH_CONFIRM_BILATERAL));
            schemeInfo->doConfirmRecip =  /* = bilateral||UtoV */
                ((confirmMode == CC_FFCDH_CONFIRM_U_TO_V) || (confirmMode == CC_FFCDH_CONFIRM_BILATERAL));
            schemeInfo->doUserNonce = schemeInfo->doConfirmRecip;
            schemeInfo->doPartnerNonce = schemeInfo->doConfirmProvid;
            break;

        default:
            return CC_FFCDH_INVALID_SCHEM_ID_ERROR;
        }

        /* calculate user and partner PartyInfo data offsets (starts from Party ID). */
//      pDataOffsets->userIdOffset = FFCDH_SET_OFFSET(pDataOffsets->algIdOffset, CC_FFCDH_MAX_SIZE_OF_PARTY_INFO_BYTES);
//      pDataOffsets->partnIdOffset = FFCDH_SET_OFFSET(pDataOffsets->algIdOffset, algIdSize);
    }

    /* calculate user Info data sub-entries offsets */
//  pDataOffsets->userStatPublKeyOffset = FFCDH_SET_OFFSET(pDataOffsets->userIdOffset, userIdSize);
//  pDataOffsets->userEphemPublKeyOffset = FFCDH_COND_SET_OFFSET(pDataOffsets->userStatPublKeyOffset, publKeySize, schemeInfo->doUserStatKey);
//  pDataOffsets->userNonceOffset = FFCDH_COND_SET_OFFSET(pDataOffsets->userEphemPublKeyOffset, publKeySize, schemeInfo->doUserEphemKey);
//  pDataOffsets->userOtherDataOffset = FFCDH_COND_SET_OFFSET(pDataOffsets->userNonceOffset, nonceSize, schemeInfo->doUserNonce);
//
//  /* calculate partner Info data sub-entries offsets */
//  pDataOffsets->partnStatPublKeyOffset = FFCDH_SET_OFFSET(pDataOffsets->partnIdOffset, partnIdSize);
//  pDataOffsets->partnEphemPublKeyOffset = FFCDH_COND_SET_OFFSET(pDataOffsets->partnStatPublKeyOffset, publKeySize, schemeInfo->doPartnerStatKey);
//  pDataOffsets->partnNonceOffset = FFCDH_COND_SET_OFFSET(pDataOffsets->partnEphemPublKeyOffset, publKeySize, schemeInfo->doPartnerEphemKey);
//  pDataOffsets->partnOtherDataOffset = FFCDH_COND_SET_OFFSET(pDataOffsets->partnNonceOffset, nonceSize, schemeInfo->doPartnerNonce); ;
//
//  pDataOffsets->suppPublInfoOffset = FFCDH_SET_OFFSET(pDataOffsets->algIdOffset, algIdSize + 2*CC_FFCDH_MAX_SIZE_OF_PARTY_INFO_BYTES);
//  pDataOffsets->suppPrivInfoOffset = FFCDH_SET_OFFSET(pDataOffsets->suppPublInfoOffset, CC_FFCDH_MAX_SIZE_OF_OTHER_INFO_SUPPL_ENTRY_BYTES);

    /* copy AlgId, User ID and Partner ID into Context */
    CC_PalMemCopy(pBaseAddr + pDataOffsets->algIdOffset, pAlgId, algIdSize);
    CC_PalMemCopy(pDhCtx->userId, pUserId, userIdSize);
    pDhCtx->userIdSizeBytes = userIdSize;
    CC_PalMemCopy(pDhCtx->partnerId, pPartnId, partnIdSize);
    pDhCtx->partnerIdSizeBytes = partnIdSize;

//  pDataOffsets->userInfoSize;            /*!< full size of Party U data */
//  pDataOffsets->partnInfoSize;           /*!< full size of partner data */
//  pDataOffsets->suppPublInfoSize;
//  pDataOffsets->suppPublInfoSize;
//
//End:

    return CC_OK;
}



/*******************************************************************************************/
/*! The function sets into DH context FFCDH Scheme agreed parameters: SchemeId, User role, Confirmation mode etc.
\note The context is used in DH Agreement functions, implementing NIST SP 800-56A rev.2 standard.
\note Assumed, that input FFC Domain is properly generated or imported and validated according to
NIST SP 800-56A and FIPS 186-4 standards.

@return CC_OK on success.
@return A non-zero value on failure as defined cc_dh_error.h.
*/
CEXPORT_C CCError_t  CC_FfcDhCtxSetSchemeParams(
                            CCFfcDhUserContext_t *pDhUserCtx, /*!< [in/out] pointer to context structure, containing all parameters and data,
                                                                        defining DH Key Agreement Scheme */
                            CCFfcDomain_t *pDomain,           /*!< [in] pointer to DH FFC Domain structure. */
                            uint8_t *pAlgId,                  /*!< [in] pointer to Algorithm ID agreed by both parties and indicates how the derived
                                                                        secret keying material will be parsed and for which algorithms (sec.5.8.1.2).
                                                                        In partial, Algorithm ID should indicate also how much bits are intended for
                                                                        internal confirmation MAC algorithm and how much remaining bits will be
                                                                        returned to the user for external applications/algorithms (the total size should
                                                                        be equal to chosen secretKeyDataSize). */
                            size_t algIdSize,                 /*!< [in] size of Algorithm ID in bytes, should be less than
                                                        CC_FFCDH_MAX_SIZE_OF_ALG_ID_SUB_ENTRY. */
                            size_t secretKeyingDataSize,      /*!< [in] size in bytes of shared secret keying data, which will be extracted and in
                                                                                    the next steps and passed to the user for using in  external algorithm(s).
                                                                                    It is used for calculation of Derived Keying material size =
                                                                                    key size of the used HMAC function + secretKeyingDataSize. */
                            uint8_t *pUserId,                 /*!< [in] pointer to the user ID - a distinct identifier of the user. */
                            size_t userIdSize,                /*!< [in] size of the user ID in bytes. */
                            uint8_t *pPartnId,                /*!< [in] pointer to the partner ID - a distinct identifier of the party. */
                            size_t partnIdSize,               /*!< [in] size of the partner ID in bytes. */
                            CCFfcDhUserPartyIs_t userParty,   /*!< [in] enumerator, defining role of the user (function's caller) in the
                                                                        DH Agreement Scheme: partyU or partyV. */
                            CCFfcDhSchemeId_t dhSchemeId,     /*!< [in] enumerator ID of used FFC DH Key Agreement Scheme, as defined
                                                                in sec. 6, tab. 12. */
                            CCFfcParamSetId_t ffcParamSetId,  /*!< [in] enumerator, defining the set of FFC domain parameters
                                                                                    according to SP 56A rev.2 section 5.5.1.1, tab.1. */
                            CCFfcDhKdfModeSp56A_t kdfMode,    /*!< [in] enumerator ID of used KDF function, based on HASH or HMAC algorithms. In current
                                                                        implementation is allowed only KDF HMAC_RFC5869 mode, according to KDF_HMAC_RFC-5869. */
                            CCFfcHashOpMode_t ffcHashMode,    /*!< [in] enumerator ID of used SHAXXX HASH mode, supported by the product.
                                                                        Note: HASH SHA1 function may be used only with SA set of domain parameters
                                                                        (sec. 5.8.1, tab.6); with other sets the function returns an error. */
                            CCFfcDhUserConfirmMode_t confirmMode, /*!< enumerator, defining confirmation mode of each party: provider
                                                                        or/and recipient, according to sec. 5.9. */
                            uint8_t *pHmacSalt,               /*!< [in] optional, pointer to the Salt, used as key in HMAC-KDF function on appropriate modes.
                                                                        If HMAC-KDF mode is set, and the pointer and size are zero, then the Salt is
                                                                        treated as full-zero bytes array of size equalled to block-size of used HMAC function.
                                                                        If HMAC-KDF mode is HMAC_RFC5869_MODE, then the Salt is treated as HMAC Key.
                                                                        If only one of parameters (pointer and size) is zero, but other not, then the
                                                                        function returns an error. */
                            size_t  hmacSaltSize,             /*!< [in] optional, size of Salt in bytes, should be equalled to the HMAC block size if
                                                                        salt is used. */
                            size_t  macTagSize                /*!< [in] optional, size in bytes of confirmation MacTag. Should be in range:
                                                                        [CC_FFCDH_MIN_SIZE_OF_CONFIRM_MAC_TAG_BYTES, CC_FFCDH_MAX_SIZE_OF_CONFIRM_MAC_TAG_BYTES]. */
)
{

        /* FUNCTION DECLARATIONS */

        /* the return error identifier */
        CCError_t err = CC_OK;
        /* pointer to internal DH context structure */
        DhContext_t *pDhCtx;
        uint32_t hashSize, minHashSize;
        uint32_t ffcPrimeSize, ffcOrderSize;
        CCHashOperationMode_t hashMode;
//        FfcDhSchemeDataOffsets_t *pOffsets;
//        uint32_t ffcOrderSizeBytes;
//        uint8_t *pTmp, *pBaseAddr;
//        uint16_t zzSize; /* size of shared secret ZZ, depended on Scheme */
//        uint16_t remainLen; /* remaining size of buffers to write */
//        uint32_t ffcOrderSize;

    CHECK_AND_RETURN_ERR_UPON_FIPS_ERROR();


        /* ...... checking the parameters validity ........ */
        /* ------------------------------------------------ */

        /* check contexts pointers and tags */
    CHECK_AND_RETURN_ERROR(pDhUserCtx == NULL, CC_FFCDH_INVALID_CONTEXT_PTR_ERROR);
        /* check that DhCtx is valid */
    CHECK_AND_RETURN_ERROR(pDhUserCtx == NULL, CC_FFCDH_INVALID_CONTEXT_PTR_ERROR);
    CHECK_AND_SET_ERROR(FFCDH_CHECK_CTX_VALID_TAG_BITS(pDhUserCtx->validTag, FFCDH_CTX_VALID_TAG_INIT_VAL),
                CC_FFCDH_CONTEXT_VALIDATION_TAG_ERROR);
    /* Check FFC Domain pointer and validation tag */
    CHECK_AND_RETURN_ERROR(pDomain == NULL, CC_FFCDH_INVALID_DOMAIN_PTR_ERROR);
    CHECK_AND_SET_ERROR(pDomain->validTag != CC_FFC_DOMAIN_VALIDATION_TAG,
                CC_FFCDH_INVALID_DOMAIN_VALIDAT_TAG_ERROR);

    /* check user and algorithm IDs pointers and sizes */
    CHECK_AND_RETURN_ERROR((pUserId == NULL || userIdSize == 0 || userIdSize > CC_FFCDH_MAX_SIZE_OF_PARTY_ID_BYTES),
                CC_FFCDH_INVALID_USER_PARTY_ID_ERROR);
    CHECK_AND_RETURN_ERROR((pAlgId == NULL) || (algIdSize == 0) || (algIdSize > CC_FFCDH_MAX_SIZE_OF_ALG_ID_ENTRY_BYTES),
                    CC_FFCDH_ALGORITHM_ID_ERROR);
        /* check optional Salt data */
    CHECK_AND_RETURN_ERROR((pHmacSalt == NULL) != (hmacSaltSize == 0) ||
                    (hmacSaltSize > CC_FFCDH_MAX_SIZE_OF_ALG_ID_ENTRY_BYTES), CC_FFCDH_ALGORITHM_ID_ERROR);

         /* check all enumerators */
    CHECK_AND_RETURN_ERROR(dhSchemeId    >= CC_FFCDH_SCHEM_NUM_OFF_MODE, CC_FFCDH_INVALID_SCHEM_ID_ERROR);
    CHECK_AND_RETURN_ERROR(ffcParamSetId >= CC_FFC_PARAMS_SET_FC, CC_FFCDH_INVALID_DOMAIN_SIZES_SET_ID_ERROR);
    CHECK_AND_RETURN_ERROR(kdfMode       >= CC_FFCDH_KDF_NUM_OFF_MODE, CC_FFCDH_INVALID_KDF_MODE_ERROR);
    CHECK_AND_RETURN_ERROR(ffcHashMode   >= CC_FFC_HASH_NUM_OFF_MODE, CC_FFCDH_INVALID_HASH_MODE_ERROR);
    CHECK_AND_RETURN_ERROR(userParty     >= CC_FFCDH_PARTY_NUM_OFF_MODE, CC_FFCDH_INVALID_USER_PARTY_ID_ERROR);
    CHECK_AND_RETURN_ERROR(confirmMode   >= CC_FFCDH_CONFIRM_NUM_OFF_MODE, CC_FFCDH_INVALID_CONFIRM_MODE_ERROR);

        /* Get internal context structure */
        pDhCtx = (DhContext_t*)&pDhUserCtx->contextBuff;

        /* check that FFC params Set in Domain meets to given by input value. */
        CHECK_AND_SET_ERROR(pDomain->ffcParamSetId != ffcParamSetId, CC_FFCDH_INVALID_DOMAIN_DATA_ERROR);

        /* get HASH related parameters */
        CHECK_ERROR(FfcGetHashMode(&hashMode, NULL/*pHkdfHashMode*/, NULL/*pBlockSize*/, &hashSize, ffcHashMode/*in*/));

        /* get DH Domain related parameters */
        CHECK_ERROR(FfcGetDomainSizes(NULL/*pMaxSecurStrengthBytes*/, &ffcPrimeSize/*pPrimeLenBytes*/,
                &ffcOrderSize/*pOrderLenBytes*/, &minHashSize, ffcParamSetId));

        /* check that Domain generation parameters are meets to given Domain sizes set ffcParamSetId. */
        CHECK_AND_SET_ERROR((pDomain->modLenWords * CC_32BIT_WORD_SIZE != ffcPrimeSize) ||
                    (pDomain->ordLenWords * CC_32BIT_WORD_SIZE != ffcOrderSize) ||
                    (pDomain->ffcHashMode != ffcHashMode), CC_FFCDH_INVALID_DOMAIN_DATA_ERROR);

        /* check HASH mode (size) according to standard security for FFC Sizes Set:
         * SP 800-56A rev.2, tab.6,8 */
        CHECK_AND_SET_ERROR(hashSize < minHashSize, CC_FFCDH_INVALID_LOW_HASH_SIZE_ERROR);

        /* check confirmation mode according to Scheme and user party (U,V).
           Note: in other schemes allowed all modes */
        if(confirmMode != CC_FFCDH_CONFIRM_NOT_USED) {
            /* check Scheme and Confirmation accordance */
            CHECK_AND_SET_ERROR(
               (dhSchemeId == CC_FFCDH_SCHEM_EPHEM)         ||
           ((dhSchemeId == CC_FFCDH_SCHEM_ONE_FLOW) &&
            (confirmMode != CC_FFCDH_CONFIRM_V_TO_U))   ||
           ((dhSchemeId == CC_FFCDH_SCHEM_HYBRID_ONE_FLOW) &&
            (confirmMode != CC_FFCDH_CONFIRM_U_TO_V)),
            CC_FFCDH_INVALID_CONFIRM_MODE_ERROR);

                /* if confirmation is used, then check MacTag size */
                CHECK_AND_SET_ERROR((macTagSize > CC_FFCDH_MAX_SIZE_OF_CONFIRM_MAC_TAG_BYTES) ||
                                (macTagSize < CC_FFCDH_MIN_SIZE_OF_CONFIRM_MAC_TAG_BYTES),
                                CC_FFCDH_MAC_TAG_SIZE_INVALID_ERROR);
                /* set MacTag size */
                pDhCtx->macTagSize = macTagSize;
        }

        /* check the optional HMAC salt, used in key derivation on HMAC mode */
        CHECK_AND_SET_ERROR((pHmacSalt != NULL) != (hmacSaltSize != 0),
                    CC_FFCDH_INVALID_HMAC_SALT_PARAMS_ERROR);


        /*------------------------------------------*/
        /*            set DH Scheme parameters      */
        /*------------------------------------------*/

        /* HMAC KDF parameters */
        if(kdfMode == CC_FFCDH_KDF_HMAC_RFC5869_MODE) {
        /* check that the user not trying to pass redundant HMAC salt */
        CHECK_AND_SET_ERROR((pHmacSalt == NULL) || (hmacSaltSize == 0), CC_FFCDH_INVALID_HMAC_SALT_PARAMS_ERROR);
        }
#ifdef CC_FFCDH_KDF_HMAC_ONE_STEP_MODE
        else if(kdfMode == CC_FFCDH_KDF_HMAC_SINGLE_STEP_MODE) {
            CHECK_AND_SET_ERROR((pHmacSalt == NULL) ||
                        (hmacSaltSize != DhHashBlockAndDigestSizes[hashMode].blockSizeInBytes),
                            CC_FFCDH_INVALID_HMAC_SALT_PARAMS_ERROR);
        }
        else {
            err = CC_FFCDH_INVALID_KDF_MODE_ERROR;
            goto End;
        }
#endif

        /* copy FFC Domain into DH context */
        CC_PalMemCopy(&pDhCtx->ffcDomain, pDomain, sizeof(pDhCtx->ffcDomain));

        /* copy given Salt into DH context */
        if(pHmacSalt != NULL) {
            pDhCtx->hmacSaltSizeBytes = hmacSaltSize;
                CC_PalMemCopy(pDhCtx->hmacSalt, pHmacSalt, hmacSaltSize);
                pDhCtx->hmacSaltSizeBytes = hmacSaltSize;
        }

        /* set DH Scheme parameters into Context */
        pDhCtx->dhSchemeId   = dhSchemeId;
        pDhCtx->ffcHashMode  = ffcHashMode;
        pDhCtx->kdfMode      = kdfMode;
        pDhCtx->userParty    = userParty;
        pDhCtx->confirmMode  = confirmMode;
        pDhCtx->ffcParamSet  = ffcParamSetId;
        pDhCtx->nonceSize    = ffcOrderSize;
        pDhCtx->secretKeyingDataSize = secretKeyingDataSize;
        pDhCtx->macTagSize = macTagSize;

        /* set required size of keying material to be derived according to confirmation mode and HMAC size */
        if(confirmMode < CC_FFCDH_CONFIRM_NOT_USED) {
            pDhCtx->derivedKeyingMaterialSize = secretKeyingDataSize + CC_FFCDH_SIZE_OF_CONFIRM_MAC_KEY_IN_BYTES;
        }

        err = FfcDhSchemeInfo(pDhCtx, pAlgId, algIdSize, pUserId, userIdSize, pPartnId, partnIdSize);
        if(err)
            goto End;

//        ffcOrderSize = pDhCtx->ffcDomain.ordLenBytes;
//
//        /* size of shared secret ZZ */
//        zzSize = pDhCtx->ffcDomain.modLenBytes;
//        if((dhSchemeId == CC_FFCDH_SCHEM_HYBRID1) || (dhSchemeId == CC_FFCDH_SCHEM_HYBRID_ONE_FLOW)) {
//          zzSize += pDhCtx->ffcDomain.modLenBytes;
//        }

         /* set validation tag */
        pDhUserCtx->validTag |= FFCDH_CTX_VALID_TAG_SCHEM_PARAM_BIT;

End:
        if (err != CC_OK) {
                /* delete secure sensitive data */
                CC_PalMemSetZero((uint8_t*)pDhUserCtx, sizeof(CCFfcDhUserContext_t));
    }

        return err;

}/* END OF CC_FfcDhCtxSetSchemeParams function */



/*******************************************************************************************/
/*! The function generates public/private FFC Key pair according to
 *  NIST SP 800-56A rev.2 standard.
 *  Assumed: all in/out pointers and contexts content are correct;
 *           all arrays are set with little endianness order of bytes and words;
 *           size of output private key = FFC generator order size;
 *           size of output public key = FFC prime (module) size;
 *           leading zeros are given, if existing.
 */
static CCError_t FfcDhGenKeyPair ( // RL check, that result keys are put out in CC only on end of generation
            CCFfcDomain_t *pFfcDomain,    /*!< [in] pointer to DH FFC Domain structure. */
            CCRndContext_t *pRndContext,    /*!< [in] random generation function context. */
            uint32_t *pPrivKey,             /*!< [out] pointer to private key buffer. */
            uint32_t *pPublKey)             /*!< [out] pointer to public key buffer. */
{
    CCError_t err = CC_OK;

    uint32_t primeModSize = pFfcDomain->modLenWords*CC_32BIT_WORD_SIZE;
    uint32_t ffcOrderSize = pFfcDomain->ordLenWords*CC_32BIT_WORD_SIZE;

    /* generate random in range [1, q-1]- little endianness  */
    err = CC_RndGenerateVectorInRange(
            pRndContext,                             /*in*/
            pFfcDomain->ordLenWords*CC_BITS_IN_32BIT_WORD, /*rndSizeInBits*/
            (uint8_t*)&pFfcDomain->order[0],         /*maxVect*/
            (uint8_t*)pPrivKey);                     /*out*/

    if (err != CC_OK) {
        goto End;
    }

// RL convert bytes to words only for #ifdef BIG__ENDIAN
    /* convert key to big endianness bytes */
//  CC_CommonReverseMemcpy(pPrivKey, pPrivKey, ffcOrderSize);


    /* ----------------------------------------------------------- */
    /*           Create the public key                             */
    /* ----------------------------------------------------------- */
    err = PkiExecModExpLeW(
        pPublKey,                       /*public key - out*/
        pFfcDomain->genG,               /*generator G*/
        primeModSize,                   /*generator size in words*/
        pFfcDomain->prime,              /*prime P*/
        primeModSize*CC_BITS_IN_BYTE,   /*P size in bits*/
        pPrivKey,                       /*priv.key - exponent*/
        ffcOrderSize);                   /*priv.key size in word*/

    /* check error */
    if (err != CC_OK) {
        goto End;
    }


End:
    return err;

}


/*******************************************************************************************/
/*!
@brief The function generates FFC DH key pairs according to DH Scheme and NIST SP 800-56A rev.2 standard:
<ol><li> - count of required key pairs (one or two is dependent on DH Scheme and user Party (U or V),
inserted into Context. For each of key pair the function performs the following steps: </li>
<li> - randomly generates the private key X according to section 5.6.1.1 and FIPS 184-4, B.1.1; </li>
<li> - the sizes of primes P,Q should be taken from DH FFC sizes set previously inserted into Context;  </li>
<li> - calculates the associated public key  Y = G^X mod P; </li>
<li> - sets private and public keys in appropriate place in the Context according to user party (U,V) and keys
status (static, ephemeral); </li>
<li> - exports the public key as big endianness order of bytes. </li></ol>
\note Before calling of this function, DH context should be initialized, DH Scheme parameters and
DH Domain are inserted by calling appropriate functions, else the function returns an error.

@return CC_OK on success.
@return A non-zero value on failure as defined cc_dh_error.h, cc_rnd_error.h.
*/
CEXPORT_C CCError_t CC_FfcDhGeneratePublPrivKeys(
                                CCFfcDhUserContext_t *pDhUserCtx,/*!< [in/out] pointer to DH FFC User Context structure. */
                                CCRndContext_t *pRndContext )     /*!< [in] random generation function context. */
{
        /* FUNCTION DECLARATIONS */

        /* The return error identifier */
        CCError_t err = CC_OK;
        /* temp pointers */
        uint32_t *pPrivKey, *pPublKey;
//        uint8_t *pprime;
//        uint32_t nonceSize;
        DhContext_t *pDhCtx; /* pointer to the internal DH context structure */
        /* RND state and function pointers */
        CCRndState_t   *pRndState;
        CCRndGenerateVectWorkFunc_t RndGenerateVectFunc;

    CHECK_AND_RETURN_ERR_UPON_FIPS_ERROR();


        /* ............... checking the parameters validity ................... */
        /* -------------------------------------------------------------------- */

        /* check contexts pointers and tags */
    CHECK_AND_RETURN_ERROR(pDhUserCtx == NULL, CC_FFCDH_INVALID_CONTEXT_PTR_ERROR);
        /* check that DhCtx is valid for user keys generation step: *
        *  DH Scheme and Domain are set.                           */
    CHECK_AND_RETURN_ERROR(FFCDH_CHECK_CTX_VALID_TAG_BITS(pDhUserCtx->validTag, FFCDH_CTX_VALID_TAG_INIT_VAL),
                                                   CC_FFCDH_CONTEXT_VALIDATION_TAG_ERROR);

        /* Check RND context pointer. Note: full check of context will be  *
        *  performed in the called CC_RndGenerateVectorInRange function */
    CHECK_AND_RETURN_ERROR(pRndContext == NULL, CC_RND_CONTEXT_PTR_INVALID_ERROR);
        pRndState = (CCRndState_t*)&(pRndContext->rndState);
        RndGenerateVectFunc = pRndContext->rndGenerateVectFunc;
        CHECK_AND_RETURN_ERROR(RndGenerateVectFunc == NULL, CC_RND_GEN_VECTOR_FUNC_ERROR);

        pDhCtx = (DhContext_t*)&pDhUserCtx->contextBuff; /* get internal DH context */
//        pBaseAddr = &pDhCtx->extendDataBuffer[0];

        /* get FFC domain parameters */
//        ffcOrderSize = pDhCtx->ffcDomain.modLenBytes;
//        ffcOrderSize = pDhCtx->ffcDomain.ordLenBytes;
//        pprime = &pDhCtx->ffcDomain.prime[0];

//        nonceSize = pDhCtx->ffcDomain.ordLenBytes;


        /* ------------------------------------------------------------  *
         *  Generate user private and public keys using method, defined  *
         *  in FIPS 186-4 B.1.2 (by testing candidates)                  *
         * ------------------------------------------------------------  */

        /* static key pair */
    if(pDhCtx->schemeInfo.doUserStatKey == 1) {
        /* set static key */
        pPrivKey = &pDhCtx->statPrivKey[0];
        pPublKey = &pDhCtx->userStatPublKey[0];
        /* generate static private and public keys */
        err = FfcDhGenKeyPair(&pDhCtx->ffcDomain, pRndContext, pPrivKey, pPublKey);
        if (err != CC_OK) {
            goto End;
        }
        pDhCtx->validTag |= FFCDH_CTX_VALID_TAG_USER_STAT_KEY_BIT;

    }
    /* ephemeral key pair */
    if(pDhCtx->schemeInfo.doUserEphemKey == 1){
        pPrivKey = &pDhCtx->ephemPrivKey[0];
        pPublKey = &pDhCtx->userEphemPublKey[0];
        /* generate static private and public keys */
        err = FfcDhGenKeyPair(&pDhCtx->ffcDomain, pRndContext, pPrivKey, pPublKey);
        if (err != CC_OK) {
            goto End;
        }
        pDhCtx->validTag |= FFCDH_CTX_VALID_TAG_USER_EPHEM_KEY_BIT;
    } else if(pDhCtx->schemeInfo.doUserNonce == 1) {
        /* generate random nonce */
        CHECK_ERROR(RndGenerateVectFunc(pRndState, pDhCtx->userNonce, pDhCtx->nonceSize));
        pDhCtx->validTag |= FFCDH_CTX_VALID_TAG_USER_NONCE_BIT;
    }

End:
        if (err != CC_OK) {
                /* delete secure sensitive data */
                CC_PalMemSetZero((uint8_t*)pDhUserCtx, sizeof(CCFfcDhUserContext_t));
    }

        return err;

}/* END OF CC_FfcDhGeneratePublPrivKeys function */




/*******************************************************************************************/
/*!
@brief This function validates the FFC DH public key according to NIST SP 800-56A rev.2,
       sec.5.6.2.3.1 and checking mode:

<ul><li> - on "partial" mode - checks the pointers and high/low limits of key value;</li>
<li> - on "full" mode - checks also that the the key belongs to the FFC subgroup; </li></ul>
\note Before calling of this function, appropriate FFC Domain parameters should be obtained and validated,
else the function returns an error.
@return CC_OK on success.
@return A non-zero value on failure as defined cc_dh_error.h.
*/
CEXPORT_C CCError_t CC_FfcDhValidatePublKey(
            CCFfcDomain_t *pDomain,          /*!< [in/out] pointer to DH FFC Context structure. */
            uint8_t *pPublKeyData,             /*!< [in] pointer to given DH FFC public key formatted as big endianness array;
                                     it should be in range [2, P-2], where P is the Domain Prime P. */
            size_t publKeyDataSize,            /*!< [in] pointer to public key size, in bytes: should be not great than Prime size. */

            CCFfcDhKeyValidMode_t validatMode, /*!< [in] enumerator ID defining the validation mode:
                                     CC_FFCDH_CHECK_FULL_MODE - full validation (sec. 5.6.2.3.1);
                                     CC_FFCDH_CHECK_PARTIAL_MODE - check pointers, sizes and range of values. */
            uint32_t *pTmpBuff)                 /*!< [in] temporary buffer of size not less Prime size. */
{
        /* FUNCTION DECLARATIONS */

        /* The return error identifier */
        CCError_t err = CC_OK;
    uint32_t *pPrime;
    uint32_t *pPublKey32;
        size_t  primeSizeBytes;
        size_t  primeSizeWords, orderSizeWords;
        uint32_t  one;
        CCCommonCmpCounter_t cmp;

        CHECK_AND_RETURN_ERR_UPON_FIPS_ERROR();


        /* ............... check parameters validity ................... */
        /* ------------------------------------------------------------- */

        CHECK_AND_RETURN_ERROR(pDomain == NULL, CC_FFCDH_INVALID_DOMAIN_PTR_ERROR);
        CHECK_AND_RETURN_ERROR(pDomain->validTag != CC_FFC_DOMAIN_VALIDATION_TAG,
                           CC_FFCDH_INVALID_DOMAIN_DATA_ERROR);
        CHECK_AND_RETURN_ERROR((pPublKeyData == NULL) || (pTmpBuff == NULL),
                    CC_FFCDH_INVALID_PUBL_KEY_PTR_ERROR);
        CHECK_AND_RETURN_ERROR(publKeyDataSize == 0, CC_FFCDH_INVALID_PUBLIC_KEY_SIZE_ERROR);

        /* get FFC domain parameters */
        primeSizeWords = pDomain->modLenWords;
    orderSizeWords = pDomain->ordLenWords;
        primeSizeBytes = primeSizeWords*CC_32BIT_WORD_SIZE;

        pPrime = &pDomain->prime[0];
        pPublKey32 = pTmpBuff;


        /* check input enumerators */
        CHECK_AND_RETURN_ERROR(validatMode >= CC_FFCDH_KEY_VALIDAT_NUM_OFF_MODE,
                       CC_FFCDH_INVALID_VALIDAT_MODE_ERROR);

    /* convert pPublKeyData to LE words array */
        CHECK_ERROR(CC_CommonConvertMsbLsbBytesToLswMswWords(pPublKey32/*out32*/, primeSizeBytes,
                                     pPublKeyData/*in8*/, publKeyDataSize));

        /*----------------------------------------------------------------------*/
        /*   Public Key Validation: sec.5.6.2.3.1, step.1 (partial validation)  */
        /*----------------------------------------------------------------------*/
        /* preliminary check the key size */
//        CHECK_AND_SET_ERROR(publKeyDataSize != ffcPrimeSize, CC_FFCDH_INVALID_PUBLIC_KEY_SIZE_ERROR);

    /* check public key value range [2,P-2]:  1 < publKey < P-1; */
        pPrime[0] -= 1; /*temporary set P = P-1*/
    cmp = CC_CommonCmpLsWordsUnsignedCounters(pPrime, primeSizeWords, pPublKey32, primeSizeWords);
    CHECK_AND_SET_ERROR(cmp != CC_COMMON_CmpCounter1GreaterThenCounter2, CC_FFCDH_INVALID_PUBLIC_KEY_VALUE_ERROR);
    pPrime[0] += 1; /*repair P */
    /* compare to 1 */
    one = 1;
    cmp = CC_CommonCmpLsWordsUnsignedCounters(pPublKey32, primeSizeWords, &one, 1);
    CHECK_AND_SET_ERROR(cmp != CC_COMMON_CmpCounter1GreaterThenCounter2,
                CC_FFCDH_INVALID_PUBLIC_KEY_VALUE_ERROR);


        /*--------------------------------------------------------------------*/
        /*         For Full Validation perform step.2 of sec.5.6.2.3.1:       */
    /*               check, that 1 = (PublKey ^ FfcOrder) mod P.                   */
        /*--------------------------------------------------------------------*/
    if(validatMode == CC_FFCDH_KEY_VALIDAT_FULL_MODE) {

        err = PkiExecModExpLeW(
            pPublKey32,                       /*result - out*/
            pPublKey32,                       /*public key - in*/
            primeSizeWords,                   /*public key size - in*/
            pPrime,                           /*prime P - modulus*/
            primeSizeWords*CC_BITS_IN_32BIT_WORD, /*P size in bits*/
            &pDomain->order[0],  /* sub-group order Q*/
            orderSizeWords);                  /* Q size in words*/

        /* check error */
        if (err != CC_OK) {
            goto End;
        }

        cmp = CC_CommonCmpLsWordsUnsignedCounters(pPublKey32, primeSizeWords, &one, 1);
        CHECK_AND_SET_ERROR(cmp != CC_COMMON_CmpCounter1AndCounter2AreIdentical,
                    CC_FFCDH_INVALID_PUBLIC_KEY_VALUE_ERROR);
    }

End:

    CC_PalMemSetZero((uint8_t*)pTmpBuff, primeSizeBytes);

    return err;

} /* CC_FfcDhValidatePublKey */



/*******************************************************************************************/
/*!
@brief The function checks and sets the FFC DH partner's public key into DH Context
according to NIST SP 800-56A rev.2 sec.5.6.2.3.1 and checking mode:

<ul><li> - if the key belongs to user's party, then the function returns an error, meaning
that the user should use other function to import both public and private keys together;</li>.
<li> - on "partial" mode - checks the pointers and high/low limits of key value;</li>
<li> - on "full" mode - checks also that the the key belongs to the FFC subgroup; </li>
<li> - sets the key data into DH Context according to party's role and key status. </li></ul>
\note Before calling of this function, DH context should be initialized and Scheme and FFC Domain
parameters are inserted by calling appropriate functions, else the function returns an error.
@return CC_OK on success.
@return A non-zero value on failure as defined cc_dh_error.h.
*/
CEXPORT_C CCError_t CC_FfcDhValidateAndImportPublKey(
                                CCFfcDhUserContext_t *pDhUserCtx,  /*!< [in/out] pointer to DH FFC Context structure. */
                                uint8_t *pPublKeyData,             /*!< [in] pointer to given DH FFC public key or Nonce in big endianness;
                                                                          it should be in range [2, P-2], where P is the Domain Prime. */
                                size_t publKeyDataSize,            /*!< [in] public key size, in bytes: should be not great than Domain Prime size. */
                                CCFfcDhKeyValidMode_t validatMode, /*!< [in] enumerator ID defining the validation mode:
                                                                           CC_FFCDH_CHECK_FULL_MODE - full validation (sec. 5.6.2.3.1);
                                                                           CC_FFCDH_CHECK_PARTIAL_MODE - check pointers, sizes and range of values. */
                                CCFfcDhKeyStatus_t keyStatus       /*!< [in] enumerator, defining the key status according to its life time
                                                                          or purpose: static/ephemeral */
)
{
        /* FUNCTION DECLARATIONS */

        /* The return error identifier */
        CCError_t err = CC_OK;
        DhContext_t *pDhCtx; /* internal DH context */
        CCFfcDomain_t *pFfcDomain;
        uint32_t *pPublKey32, *pPublKeySizeBytes;
        uint32_t  primeSizeBytes, primeSizeWords, previousKeySize;
        uint32_t testBit = 0;


        CHECK_AND_RETURN_ERR_UPON_FIPS_ERROR();

        /* check contexts pointers and tags */
        CHECK_AND_RETURN_ERROR(pDhUserCtx == NULL, CC_FFCDH_INVALID_CONTEXT_PTR_ERROR);
        /* check that DhCtx is valid for user keys generation step: *
        *  DH Scheme and Domain are set.                           */
        CHECK_AND_SET_ERROR(FFCDH_CHECK_CTX_VALID_TAG_BITS(pDhUserCtx->validTag,
                   FFCDH_CTX_VALID_TAG_SCHEM_PARAM_SET), CC_FFCDH_CONTEXT_VALIDATION_TAG_ERROR);

        /* Get internal context structure */
        pDhCtx = (DhContext_t*)&pDhUserCtx->contextBuff;
        pFfcDomain = &pDhCtx->ffcDomain;
        /* check domain validation tag */
        CHECK_AND_SET_ERROR(pFfcDomain->validTag != CC_FFC_DOMAIN_VALIDATION_TAG,
                    CC_FFCDH_INVALID_DOMAIN_DATA_ERROR);

        /* get FFC domain parameters */
        primeSizeWords = pFfcDomain->modLenWords;
        primeSizeBytes = primeSizeWords*CC_32BIT_WORD_SIZE;

        CHECK_AND_SET_ERROR(pPublKeyData == NULL, CC_FFCDH_INVALID_ARGUMENT_POINTER_ERROR);
        /* check input enumerators */
        CHECK_AND_SET_ERROR(keyStatus >= CC_FFCDH_KEY_STATUS_NUM_OFF_MODE, CC_FFCDH_INVALID_KEY_STATUS_ERROR);
        CHECK_AND_SET_ERROR(validatMode >= CC_FFCDH_KEY_VALIDAT_NUM_OFF_MODE, CC_FFCDH_INVALID_VALIDAT_MODE_ERROR);
        /* preliminary check of public key size */
        CHECK_AND_SET_ERROR(publKeyDataSize == 0, CC_FFCDH_INVALID_PUBLIC_KEY_SIZE_ERROR);

        /* check that the key meets to scheme requirements and set the pointer to
         * the appropriate place in the context */
    if(keyStatus == CC_FFCDH_KEY_STATIC) {
        previousKeySize = pDhCtx->partnerStatPublKeySizeBytes; /*size of previous inserted key*/
        if(pDhCtx->schemeInfo.doPartnerStatKey == 1) {
            pPublKey32 = pDhCtx->partnerStatPublKey;
            pPublKeySizeBytes = &pDhCtx->partnerStatPublKeySizeBytes;
            testBit = FFCDH_CTX_VALID_TAG_PARTN_STAT_KEY_BIT;

        } else {
            err = CC_FFCDH_THE_KEY_IS_NOT_REQUIRED_ERROR;
            goto End;
        }
    } else if(keyStatus == CC_FFCDH_KEY_EPHEMER) {
        previousKeySize = pDhCtx->partnerEphemPublKeySizeBytes; /*size of previous inserted key*/
        if(pDhCtx->schemeInfo.doPartnerEphemKey == 1) {
            pPublKey32 = pDhCtx->partnerEphemPublKey;
            pPublKeySizeBytes = &pDhCtx->partnerEphemPublKeySizeBytes;
            testBit = FFCDH_CTX_VALID_TAG_PARTN_EPHEM_KEY_BIT;
        } else {
            err = CC_FFCDH_THE_KEY_IS_NOT_REQUIRED_ERROR;
            goto End;
        }
    }

    /* check that other key wasn't inserted previously on this place */
        CHECK_AND_SET_ERROR(previousKeySize != 0, CC_FFCDH_ILLEGAL_TRY_REWRITE_PARAM_ERROR);

       /* Check Public Key Data validity */
        err = CC_FfcDhValidatePublKey(
                &pDhCtx->ffcDomain,
                pPublKeyData,
                        publKeyDataSize, /*size with leading zeros*/
                        validatMode,
                        (uint32_t*)&pDhCtx->tmpBuff);

        if (err != CC_OK) {
                goto End;
        }

        /* copy the public key into context as LE words array. */
        CHECK_ERROR(CC_CommonConvertMsbLsbBytesToLswMswWords(pPublKey32/*out32*/, primeSizeBytes,
                                     pPublKeyData/*in8*/, publKeyDataSize));

        *pPublKeySizeBytes = primeSizeBytes;

        /* set valid tag bit according to key status */
        pDhCtx->validTag |= testBit;

End:
        if (err != CC_OK) {
                /* delete secure sensitive data */
                CC_PalMemSetZero((uint8_t*)pDhUserCtx, sizeof(CCFfcDhUserContext_t));
    }

        return err;
}

/*******************************************************************************************/
/*!
@brief The function checks and sets the FFC DH user's private/public key pair into DH Context
according to NIST SP 800-56A rev.2 sec.5.6.2.3.1 and checking mode:

<ul><li> - if the key belongs to partner's party, then the function returns an error, meaning
that the user should use other function to import only public key;</li>.
<li> - on "partial" mode - checks the pointers and high/low limits of key value;</li>
<li> - on "full" mode - checks also that the the public key meets to private key; </li>
<li> - sets the key data into DH Context according to party's role and key status. </li></ul>
\note Before calling of this function, DH context should be initialized and Scheme and FFC Domain
parameters are inserted by calling appropriate functions, else the function returns an error.
@return CC_OK on success.
@return A non-zero value on failure as defined cc_dh_error.h.
*/
CEXPORT_C CCError_t CC_FfcDhValidateAndImportKeyPair(
                                CCFfcDhUserContext_t *pDhUserCtx,  /*!< [in/out] pointer to DH FFC Context structure. */
                                uint8_t *pPrivKeyData,             /*!< [in] pointer to given DH FFC private key in big endianness;
                                                                             it should be in range [1, n-1], where n is the Domain generator order. */
                                size_t privKeyDataSize,            /*!< [in] private key size, in bytes: should be equaled Domain
                                                                             generator order size. */
                                uint8_t *pPublKeyData,             /*!< [in] pointer to given DH FFC public key in big endianness;
                                                                          it should be in range [2, P-2], where P is the Domain Prime. */
                                size_t publKeyDataSize,            /*!< [in] public key size, in bytes: should be equaled to Domain Prime size,
                                                                             including leading zeros. */
                                CCFfcDhKeyValidMode_t validatMode, /*!< [in] enumerator ID defining the validation mode:
                                                                             CC_FFCDH_CHECK_FULL_MODE - full validation (sec. 5.6.2.3.1);
                                                                             CC_FFCDH_CHECK_PARTIAL_MODE - check pointers, sizes and range of values. */
                                CCFfcDhKeyStatus_t keyStatus       /*!< [in] enumerator, defining the key status according to its life time
                                                                             or purpose: static/ephemeral/nonce */
)
{
        /* FUNCTION DECLARATIONS */

        /* The return error identifier */
        CCError_t err = CC_OK;
        DhContext_t *pDhCtx; /* internal DH context */
        CCFfcDomain_t *pFfcDomain;
        uint32_t *pPublKey32, *pPrivKey32;
//        uint16_t   *pPublKey32Size;
        uint32_t  *pPrime, *pOrder, one = 0;
        size_t  primeSizeBytes, orderSizeBytes;
        size_t  primeSizeWords, orderSizeWords;
        uint32_t  testBit;
        uint32_t  *pPrivKeySize, *pPublKeySize;
//        uint8_t  *pBaseAddr; /* start address for offsets counting */
//        CCFfcDhCtxPublKeys_t *pPublKey32s;
        CCCommonCmpCounter_t cmp;
//        bool isKeyRequested;
        uint32_t *pTmpBuff; /* used size = Prime size */


        CHECK_AND_RETURN_ERR_UPON_FIPS_ERROR();

        /* check contexts pointers and tags */
        CHECK_AND_RETURN_ERROR(pDhUserCtx == NULL, CC_FFCDH_INVALID_CONTEXT_PTR_ERROR);
        /* check that DhCtx is valid for user keys generation step: *
        *  DH Scheme and Domain are set.                           */
        CHECK_AND_SET_ERROR(FFCDH_CHECK_CTX_VALID_TAG_BITS(pDhUserCtx->validTag,
                FFCDH_CTX_VALID_TAG_DOMAIN_SET), CC_FFCDH_CONTEXT_VALIDATION_TAG_ERROR );

        /* preliminary check of private/public key pointers and sizes */
        CHECK_AND_SET_ERROR(pPrivKeyData == NULL, CC_FFCDH_INVALID_ARGUMENT_POINTER_ERROR);
        CHECK_AND_SET_ERROR(pPublKeyData == NULL, CC_FFCDH_INVALID_ARGUMENT_POINTER_ERROR);
        CHECK_AND_SET_ERROR(privKeyDataSize == 0, CC_FFCDH_INVALID_PRIVATE_KEY_SIZE_ERROR);
        CHECK_AND_SET_ERROR(publKeyDataSize == 0, CC_FFCDH_INVALID_PUBLIC_KEY_SIZE_ERROR);

        /* check input enumerators */
        CHECK_AND_SET_ERROR(keyStatus >= CC_FFCDH_KEY_STATUS_NUM_OFF_MODE, CC_FFCDH_INVALID_KEY_STATUS_ERROR);
        CHECK_AND_SET_ERROR(validatMode >= CC_FFCDH_KEY_VALIDAT_NUM_OFF_MODE, CC_FFCDH_INVALID_VALIDAT_MODE_ERROR);

        /* Get internal context structure */
        pDhCtx = (DhContext_t*)&pDhUserCtx->contextBuff;
        pFfcDomain = &pDhCtx->ffcDomain;
        /* check domain validation tag */
        CHECK_AND_SET_ERROR(pFfcDomain->validTag != CC_FFC_DOMAIN_VALIDATION_TAG, CC_FFCDH_INVALID_DOMAIN_DATA_ERROR);

        /* get FFC domain parameters */
        primeSizeWords = pFfcDomain->modLenWords;
        orderSizeWords = pFfcDomain->ordLenWords;
        primeSizeBytes = primeSizeWords*CC_32BIT_WORD_SIZE;
        orderSizeBytes = orderSizeWords*CC_32BIT_WORD_SIZE;

        pPrime = &pDhCtx->ffcDomain.prime[0]; /* Prime p */
        pOrder = &pDhCtx->ffcDomain.order[0]; /* Order q */
        pTmpBuff = &pDhCtx->tmpBuff.TempBuff[0]; /* used size = Prime size */

        /* set base address for offsets counting */
//        pBaseAddr = &pDhCtx->extendDataBuffer[0];

        /* check that the key meets to scheme requirements and set the pointers to
         * appropriate place in the context */
    if(keyStatus == CC_FFCDH_KEY_STATIC) {
        if(pDhCtx->schemeInfo.doUserStatKey == 1) {
            pPrivKey32 = &pDhCtx->statPrivKey[0];
            pPublKey32 = &pDhCtx->userStatPublKey[0];
            testBit = FFCDH_CTX_VALID_TAG_USER_STAT_KEY_BIT;
            pPrivKeySize = &pDhCtx->statPrivKeySizeBytes;
            pPublKeySize = &pDhCtx->userStatPublKeySizeBytes;
        } else {
            err = CC_FFCDH_INVALID_KEY_STATUS_ERROR;
            goto End;
        }
    } else if(keyStatus == CC_FFCDH_KEY_EPHEMER) {
        if(pDhCtx->schemeInfo.doUserEphemKey == 1) {
            pPrivKey32 = &pDhCtx->ephemPrivKey[0];
            pPublKey32 = &pDhCtx->userEphemPublKey[0];
            testBit = FFCDH_CTX_VALID_TAG_USER_EPHEM_KEY_BIT;
            pPrivKeySize = &pDhCtx->ephemPrivKeySizeBytes;
            pPublKeySize = &pDhCtx->userEphemPublKeySizeBytes;
        } else {
            err = CC_FFCDH_INVALID_KEY_STATUS_ERROR;
            goto End;
        }
    }

    /* check that the key is not inserted previously */
    CHECK_AND_SET_ERROR((*pPrivKeySize != 0) || (*pPublKeySize != 0),
                CC_FFCDH_ILLEGAL_TRY_REWRITE_PARAM_ERROR);

    /* convert the keys to LE words array and set into Context */
        CHECK_ERROR(CC_CommonConvertMsbLsbBytesToLswMswWords(pPublKey32/*out32*/, primeSizeBytes,
                                     pPublKeyData/*in8*/, publKeyDataSize));
        CHECK_ERROR(CC_CommonConvertMsbLsbBytesToLswMswWords(pPrivKey32/*out32*/, orderSizeBytes,
                                     pPrivKeyData/*in8*/, privKeyDataSize));

    /* check private key value range [1,q-1]:  1 <= privlKey <= q-1; */
    cmp = CC_CommonCmpLsWordsUnsignedCounters(pOrder, orderSizeWords, pPrivKey32, orderSizeWords);
    CHECK_AND_SET_ERROR(cmp != CC_COMMON_CmpCounter1GreaterThenCounter2, CC_FFCDH_INVALID_PRIVATE_KEY_VALUE_ERROR);
    one = 0;
    cmp = CC_CommonCmpLsWordsUnsignedCounters(pPrivKey32, orderSizeWords, &one, 1);
    CHECK_AND_SET_ERROR(cmp != CC_COMMON_CmpCounter1GreaterThenCounter2, CC_FFCDH_INVALID_PRIVATE_KEY_VALUE_ERROR);

    /* check public key value range [2,p-2]:  1 < privlKey < p-1; */
    pPrime[0] -= 1;
    cmp = CC_CommonCmpLsWordsUnsignedCounters(pPrime, primeSizeWords, pPublKey32, primeSizeWords);
    CHECK_AND_SET_ERROR(cmp != CC_COMMON_CmpCounter1GreaterThenCounter2, CC_FFCDH_INVALID_PUBLIC_KEY_VALUE_ERROR);
    pPrime[0] += 1; /*reset p*/
    one = 1;
    cmp = CC_CommonCmpLsWordsUnsignedCounters(pPublKey32, primeSizeWords, &one, 1);
    CHECK_AND_SET_ERROR(cmp != CC_COMMON_CmpCounter1GreaterThenCounter2, CC_FFCDH_INVALID_PUBLIC_KEY_VALUE_ERROR);


        /* copy private and public key data into context and set leading zeros */
//        CC_PalMemCopy(pPrivKey32 + (ffcOrderSize - privKeyDataSize), pPrivKeyData, privKeyDataSize);
//        CC_PalMemSet(pPrivKey32, 0, (ffcOrderSize - privKeyDataSize)); /*zeroing leading zeros*/
//        CC_PalMemCopy(pPublKey32 + (ffcPrimeSize - publKeyDataSize), pPublKeyData, publKeyDataSize);
//        CC_PalMemSet(pPublKey32, 0, (ffcPrimeSize - publKeyDataSize)); /*zeroing leading zeros*/


        /* Full check of Public Key Data validity by recomputing the public key from private:
         * (SP 56A rev.2, sec.5.6.2.1.4).  */
    if(validatMode == CC_FFCDH_KEY_VALIDAT_FULL_MODE) {
        /* calculate PublKey as LE words array */
        err = PkiExecModExpLeW(
            pTmpBuff,               /*exp.result - out*/
            &pFfcDomain->genG[0],   /*FFC generator - in*/
            primeSizeWords,         /*generator size - in*/
            pPrime,                 /*prime P - modulus*/
            primeSizeWords*CC_BITS_IN_32BIT_WORD, /*P size in bits*/
            pPrivKey32,             /* private key */
            orderSizeWords);        /* private key full size in bytes */

        /* check error */
        if (err != CC_OK) {
            goto End;
        }

        /* check that recalculated publ. key is equaled to input key. */
        cmp = CC_CommonCmpLsWordsUnsignedCounters(pTmpBuff, primeSizeWords, pPublKey32, primeSizeWords);
        CHECK_AND_SET_ERROR(cmp != CC_COMMON_CmpCounter1AndCounter2AreIdentical, CC_FFCDH_INVALID_PUBLIC_KEY_VALUE_ERROR);
    }


        /* set valid tag bit according to key status */
    pDhCtx->validTag |= testBit;

End:
        if (err != CC_OK) {
                /* delete secure sensitive data */
                CC_PalMemSetZero((uint8_t*)pDhUserCtx, sizeof(CCFfcDhUserContext_t));
    }

        return err;
}


/*******************************************************************************************/
/*!
@brief This function generates random Nonce, used in appropriate DH Schemes (NIST SP 56A rev.2 sec.5.9, 6).
<li> The function generates random vector of given size, sets it into DH context according. </li>
\note Before calling of this function, DH context should be initialized and Scheme parameters and
DH Domain are inserted by calling appropriate functions, else the function returns an error.
\note The Nonce should be generated and the function called only if it is required by DH scheme, and
the Nonce is not inserted previously, else the function returns an error.
\note The function is used when the user not generates an ephemeral key, but requires key confirmation and
therefore Nonce generation.
@return CC_OK on success.
@return A non-zero value on failure as defined cc_dh_error.h.
*/
CEXPORT_C CCError_t CC_FfcDhGenerateRandomNonce(
                                CCFfcDhUserContext_t *pDhUserCtx, /*!< [in/out] pointer to DH FFC Context structure. */
                                CCRndContext_t *pRndContext)      /*!< [in] random generation function context. */
{
    CCError_t err = CC_OK;
    DhContext_t *pDhCtx;
//  uint8_t *pNonce;

        /* RND state and function pointers */
        CCRndState_t   *pRndState;
        CCRndGenerateVectWorkFunc_t RndGenerateVectFunc;


        CHECK_AND_RETURN_ERR_UPON_FIPS_ERROR();

        /* check context pointer and tag */
        CHECK_AND_RETURN_ERROR(pDhUserCtx == NULL, CC_FFCDH_INVALID_CONTEXT_PTR_ERROR);
        /* check that DhCtx is valid for user keys generation step: */
        CHECK_AND_SET_ERROR(pDhUserCtx->validTag != FFCDH_CTX_VALID_TAG_DOMAIN_SET, CC_FFCDH_CONTEXT_VALIDATION_TAG_ERROR);

        /* Get internal context structure */
        pDhCtx = (DhContext_t*)&pDhUserCtx->contextBuff;

        /* check that Nonce is required according to DH Scheme */
        CHECK_AND_SET_ERROR(pDhCtx->schemeInfo.doUserNonce == 0, CC_FFCDH_NONCE_IS_NOT_REQUIRED_ERROR);

//       /* check userParty enumerator. */
//        CHECK_AND_SET_ERROR(userParty >= CC_FFCDH_PARTY_NUM_OFF_MODE, CC_FFCDH_INVALID_USER_PARTY_ID_ERROR);
//
        /* Check RND context pointer. Note: full check of RND context will be  *
        *  performed in the called CC_RndGenerateVectorInRange function */
    CHECK_AND_SET_ERROR(pRndContext == NULL, CC_RND_CONTEXT_PTR_INVALID_ERROR);
        RndGenerateVectFunc = pRndContext->rndGenerateVectFunc;
        CHECK_AND_SET_ERROR(RndGenerateVectFunc == NULL, CC_RND_GEN_VECTOR_FUNC_ERROR);
        pRndState = (CCRndState_t*)&(pRndContext->rndState);

          /* check that this Nonce is not inserted previously */
        CHECK_AND_SET_ERROR(pDhCtx->userNonceSizeBytes != 0, CC_FFCDH_ILLEGAL_TRY_REWRITE_PARAM_ERROR);

        /* generate random Nonce and set it into Context. Note: Nonce is not an integer. */
        CHECK_ERROR(RndGenerateVectFunc(pRndState, &pDhCtx->userNonce[0], pDhCtx->nonceSize));

End:
    if (err != CC_OK) {
        /* delete secure sensitive data */
        CC_PalMemSetZero((uint8_t*)pDhUserCtx, sizeof(CCFfcDhUserContext_t));
    }

    return err;
}


/*******************************************************************************************/
/*!
@brief This function formats the UserInfo according to the user role (PartyU or PartyV) and NIST SP 56A rev.2,
       sec. 5.8.1.2, 5.8.1.2.1.

<ul><li>  Input and previously inserted data is concatenated as defined in the CCFfcDhPartyInfo_t structure and
 sets it into the Context:  UserInfo = UserId||UserStatPublKey||UserStatPublKey||UserNonce}{||UserOtherData}, where: </li>
<li> - UserInfo and each its sub-entry are formatted as length (Len) and then appropriate data: Len||Data,
where each length is a 2-bytes big endianness counter; </li>
<li> - If any sub-entry is not used in chosen DH Scheme, than its lengths should be set 0 and the data is empty. </li>
<li> - total size of PartyInfo, including said lengths, should be not great, than the size of CCDhPartyInfo_t. </li></ul>
\note Before calling of this function the User should initialize DH Context, insert FFC Domain, DH Scheme parameters and
all his Private/Public Keys (or Nonce) using appropriate CC functions.
\note The output from this function will be exported to the other party of the Agreement and vice versa, UserInfo, received
from other party, will be used as input to DhCtxSetSchemeData() function.

@return CC_OK on success.
@return A non-zero value on failure as defined cc_dh_error.h.
*/
CEXPORT_C CCError_t CC_FfcDhCreateUserInfo(
                        CCFfcDhUserContext_t *pDhUserCtx, /*!< [in/out] pointer to context structure, containing all data,
                                                               used in DH Key Agreement Scheme. */
                        uint8_t *pUserOtherData,          /*!< [in] optional, pointer to other data, which the user will
                                                                    insert in addition to its ID, keys and Nonce. */
                        size_t userOtherDataSize,         /*!< [in] optional, size of additional data (in bytes), which the
                                                                    user will include into the UserInfo. */
                        uint8_t *pUserConfirmText,        /*!< [in] optional, pointer to confirmation Text of the User. */
                        size_t  userConfirmTextSize,      /*!< [in] optional size of Text data of partyU, in bytes. */
                        CCFfcDhPartyInfo_t *pUserInfo,    /*!< [out] pointer to the concatenated UserInfo (i.e. PartyU or PartyV Info). */
                        size_t *pUserInfoSize             /*!< [in/out] pointer to the size of UserInfo, in bytes:
                                                                in -  given buffer size (should be not less than CC_FFCDH_MAX_SIZE_OF_OTHER_INFO_ENTRY;
                                                                out - actual size of UserInfo, including length counters */
)
{
    /* FUNCTION DECLARATIONS */

        CCError_t err = CC_OK; /* return error identifier */
        DhContext_t  *pDhCtx;
        /* actual size of UserInfo data */
        uint8_t *pTmp; /* temp pointer */
        uint8_t *pBaseAddr;
        uint32_t fullLen = 0;

        CHECK_AND_RETURN_ERR_UPON_FIPS_ERROR();

        /* check contexts pointers and tags */
        if(pDhUserCtx == NULL) {
            return CC_FFCDH_INVALID_CONTEXT_PTR_ERROR;
        }
        /* check that DhCtx is valid for user keys generation step: *
        *  DH Scheme, Domain and Keys are set.                      */
        CHECK_AND_SET_ERROR(pDhUserCtx->validTag != FFCDH_CTX_VALID_TAG_ALL_KEYS_SET, CC_FFCDH_CONTEXT_VALIDATION_TAG_ERROR);
        /*  check the mandatory data pointers  */
        CHECK_AND_SET_ERROR(pUserInfo == NULL || pUserInfoSize == NULL, CC_FFCDH_INVALID_ARGUMENT_POINTER_ERROR);
    /* check the optional data pointers and sizes */
        CHECK_AND_SET_ERROR((pUserOtherData == NULL) != (userOtherDataSize == 0), CC_FFCDH_OPTIONAL_DATA_ERROR);
        CHECK_AND_SET_ERROR((pUserConfirmText == NULL) != (userConfirmTextSize == 0), CC_FFCDH_OPTIONAL_DATA_ERROR);

    /* check sizes */
        CHECK_AND_SET_ERROR(userOtherDataSize > CC_FFCDH_MAX_SIZE_OF_PARTY_INFO_OTHER_DATA_BYTES, CC_FFCDH_PARTY_INFO_SUB_ENTRY_SIZE_ERROR);
        CHECK_AND_SET_ERROR(userConfirmTextSize > CC_FFCDH_MAX_SIZE_OF_CONFIRM_TEXT_DATA_BYTES, CC_FFCDH_PARTY_INFO_SUB_ENTRY_SIZE_ERROR);

        /* Get internal context structure */
    pDhCtx = (DhContext_t*)&pDhUserCtx->contextBuff;

    /* address of extended data buffer */
    pBaseAddr = &pDhCtx->extendDataBuffer[0];

    /* set temp pointer and save userOtherData in the Context */
    pTmp = pBaseAddr + pDhCtx->dataOffsets.userIdOffset;

    /* concatenate generated previously user data into context extendDataBuffer */
    FfcDhWriteBufferBeToBe(&pTmp, pDhCtx->userId, pDhCtx->userIdSizeBytes, &fullLen);
    FfcDhWriteBufferLeToBe(&pTmp, pDhCtx->userStatPublKey, pDhCtx->userStatPublKeySizeBytes, &fullLen);
    FfcDhWriteBufferLeToBe(&pTmp, pDhCtx->userEphemPublKey, pDhCtx->userEphemPublKeySizeBytes, &fullLen);
    FfcDhWriteBufferBeToBe(&pTmp, pDhCtx->userNonce, pDhCtx->userNonceSizeBytes, &fullLen);
    FfcDhWriteBufferBeToBe(&pTmp, pUserOtherData, userOtherDataSize, &fullLen);


    CHECK_AND_SET_ERROR(fullLen > *pUserInfoSize, CC_FFCDH_LOW_OUTPUT_BUFF_SIZE_ERROR);

    /* output concatenated UserInfo data */
    pDhCtx->currInsertedDataSize = fullLen;
    pDhCtx->dataOffsets.userInfoSize = fullLen;
    CC_PalMemCopy(pUserInfo, pBaseAddr + pDhCtx->dataOffsets.userIdOffset, fullLen);
    /* concatenate user's Confirmation Text */
    pTmp = (uint8_t*)&pUserInfo + fullLen;
    FfcDhWriteBufferBeToBe(&pTmp, pUserConfirmText, userConfirmTextSize, &fullLen);
    *pUserInfoSize = fullLen;

    /* update validation tag */
    pDhUserCtx->validTag |= FFCDH_CTX_VALID_TAG_USER_INFO_BIT;


End:
    if(err != CC_OK) {
        CC_PalMemSetZero((uint8_t*)pDhUserCtx, sizeof(CCFfcDhUserContext_t));
    }

    return err;

}


static CCError_t FfcDhValidatePartnInfoPublKey(
                DhContext_t  *pDhCtx,                 /*!< [in/out] pointer to internal DH Context structure. */
                CCFfcDhKeyValidMode_t keyValidatMode, /*!< [in] enum. key validation mode */
                uint8_t **ppCtxKey,                   /*!< [in/out] pointer to pointer to context publ.key buffer */
                uint8_t **ppInKey)                    /*!< [in/out] pointer to pointer to partnerInfo publ.keydata */
{
        CCError_t err = CC_OK; /* return error identifier */
    uint32_t size1;  /*key size in Context*/
    uint32_t size2;  /*size received*/
    uint8_t *pKey = *ppCtxKey;


    /* get key sizes from Context and from the PartnerInfo */
    size1 = FFCDH_GET_LENGTH(*ppCtxKey); /*in ctx*/
    size2 = FFCDH_GET_LENGTH(*ppInKey);  /*in partner Info*/

    /* if the key was inserted previously, compare the data, else check and import the key */
    if(size1 == pDhCtx->ffcDomain.modLenWords*CC_32BIT_WORD_SIZE) {
        CHECK_AND_SET_ERROR(size1 != size2,  CC_FFCDH_PARTN_INFO_PARSING_SIZE_ERROR);
        CHECK_AND_SET_ERROR(CC_PalMemCmp(*ppCtxKey, *ppInKey, size1), CC_FFCDH_PARTN_INFO_PARSING_DATA_ERROR);

    } else if(size1 == 0){
        /* check and import the partner's key */
        err = CC_FfcDhValidatePublKey(
            &pDhCtx->ffcDomain,
            *ppInKey,  size2, /*received PublKey*/
            keyValidatMode,
            &pDhCtx->tmpBuff.TempBuff[0]);
        CHECK_ERROR(err);

        /* copy the key into Context and set size */
        FfcDhWriteBufferBeToBe(&pKey, *ppInKey, size2, &pDhCtx->currInsertedDataSize);
    } else {
        err = CC_FFCDH_PARTN_INFO_PARSING_DATA_ERROR;
    }

    /* promote pointers to next Info entries */
    *ppCtxKey += size1;
    *ppInKey  += size2;

End:
    return err;

}


/*******************************************************************************************/
/*!
@brief This function implements FFC DH primitive according to section 5.7.1.1 of NIST SP 56A rev.2 standard.
       The function computes and concatenates the shared secret values according to DH Scheme:
        - on all schemes:  SharedSecretVal = partnerPublKey1 ^ userPrivKey1 modulo Prime;
        - on hybrid schemes: SharedSecretVal2 = partnerPublKey2 ^ userPrivKey2 modulo Prime, and then
          concatenates them:  SharedSecretVal = SharedSecretVal || SharedSecretVal2;

\note Before calling of this function the user should obtain assurance of public and private keys, involved in the key
agreement, using one of methods, described in section 5.6.2 of above named standard.
\note For assurance of keys validity the user can use appropriate APIs for generating or building and validation,
of keys, described in CC_dh.h file.
\note The function intended for internal using in Keying Material derivation inside CC DH functions.
Assumed, that all required data is inserted properly in the DH Context.
@return CC_OK on success.
@return A non-zero value on failure as defined in cc_dh_error.h or cc_rnd_error.h.
*/
static CCError_t FfcDhCalcSharedSecretVal(DhContext_t *pDhCtx) /*!< [in/out] pointer to context structure, containing all data,
                                                                       and buffers, used in DH Key Agreement Scheme. */

{
    CCError_t err = CC_OK;
    CCFfcDomain_t *pFfcDomain = &pDhCtx->ffcDomain;
    FfcDhSchemeInfo_t *schemeInfo = &pDhCtx->schemeInfo;
//  FfcDhSchemeDataOffsets_t *pDataOffsets = &pDhCtx->dataOffsets;
    uint32_t *userPrivKey1 = NULL, *partnPublKey1 = NULL;
    uint32_t *userPrivKey2 = NULL, *partnPublKey2 = NULL;
    uint8_t *pSharedSecretVal1 = NULL, *pSharedSecretVal2 = NULL;
//  uint32_t publKeyDataSize = 0; //, privKeySize;
    uint32_t primeSizeBytes, orderSizeBytes;
    uint32_t primeSizeWords, orderSizeWords;
    uint32_t *pZ; /* aligned pointer to Shared Secret */

        /* get FFC domain parameters */
        primeSizeWords = pFfcDomain->modLenWords;
    orderSizeWords = pFfcDomain->ordLenWords;
        primeSizeBytes = pFfcDomain->modLenWords*CC_32BIT_WORD_SIZE;
    orderSizeBytes = pFfcDomain->ordLenWords*CC_32BIT_WORD_SIZE;

    /* set pointers to shared secret value according to user and partner roles (U,V). */
    if(pDhCtx->userParty == CC_FFCDH_PARTY_U) {
        pSharedSecretVal1 = &pDhCtx->extendDataBuffer[0] + CC_FFCDH_KDF_COUNTER_SIZE_IN_BYTES;
        pSharedSecretVal2 = pSharedSecretVal1 + primeSizeBytes;
    } else if(pDhCtx->userParty == CC_FFCDH_PARTY_V){
        pSharedSecretVal2 = &pDhCtx->extendDataBuffer[0] + CC_FFCDH_KDF_COUNTER_SIZE_IN_BYTES;
        pSharedSecretVal1 = pSharedSecretVal2 + primeSizeBytes;
    }

    /* set first pointers to appropriate keys for all DH Schemes */
    if(schemeInfo->doUserStatKey) {
        userPrivKey1 = &pDhCtx->statPrivKey[0];
        pDhCtx->statPrivKeySizeBytes = orderSizeBytes;
    } else {
        userPrivKey1 = &pDhCtx->ephemPrivKey[0];
        pDhCtx->ephemPrivKeySizeBytes = orderSizeBytes;
    }

    if(schemeInfo->doPartnerStatKey) {
        partnPublKey1 = &pDhCtx->partnerStatPublKey[0];
        pDhCtx->partnerStatPublKeySizeBytes = primeSizeBytes;
    } else {
        partnPublKey1 = &pDhCtx->partnerEphemPublKey[0];
        pDhCtx->partnerEphemPublKeySizeBytes = primeSizeBytes;
    }

//  publKeyDataSize = primeSizeBytes;
    /* align pointer to address */
    pZ = (uint32_t*)((uint32_t)pSharedSecretVal1 & (~(uint32_t)3));
// !!!  pDhCtx-> ???

    /* calculate first shared secret */
    err = PkiExecModExpLeW(
        pZ,              /*exp.result - out*/
        partnPublKey1,   /*public key - in*/
        primeSizeWords,  /*public key size - in*/
        &pDhCtx->ffcDomain.prime[0], /*prime P - modulus*/
        primeSizeWords*CC_BITS_IN_32BIT_WORD,  /*P size in bits*/
        userPrivKey1,    /* sub-group order Q*/
        orderSizeWords); /* Q size in words*/

    /* check error */
    if (err != CC_OK) {
        goto End;
    }
    /* convert Z to BE bytes */
    CC_CommonInPlaceConvertBytesWordsAndArrayEndianness(pZ, primeSizeWords);

    /* calculation of second shared secret, if needed */
    if(pDhCtx->dhSchemeId == CC_FFCDH_SCHEM_HYBRID1 ||
       pDhCtx->dhSchemeId == CC_FFCDH_SCHEM_HYBRID_ONE_FLOW) {

        /* set second pointers to appropriate keys for all DH Schemes */
        if(schemeInfo->doUserEphemKey) {
            userPrivKey2 = &pDhCtx->ephemPrivKey[0];
            pDhCtx->ephemPrivKeySizeBytes = orderSizeBytes;
        } else {
            userPrivKey2 = &pDhCtx->statPrivKey[0];
            pDhCtx->statPrivKeySizeBytes = orderSizeBytes;
        }

        if(schemeInfo->doPartnerEphemKey) {
            partnPublKey2 = &pDhCtx->partnerEphemPublKey[0];
            pDhCtx->partnerEphemPublKeySizeBytes = primeSizeBytes;
        } else  {
            partnPublKey2 = &pDhCtx->partnerStatPublKey[0];
            pDhCtx->partnerStatPublKeySizeBytes = primeSizeBytes;
        }

        /* calculate second shared secret */
        pZ = (uint32_t*)((uint32_t)pSharedSecretVal2 & (~(uint32_t)3));
        err = PkiExecModExpLeW(
            pZ,            /*exp.result - out*/
            partnPublKey2, /*public key - in*/
            primeSizeWords, /*public key size - in*/
            &pDhCtx->ffcDomain.prime[0],             /*prime P - modulus*/
            primeSizeWords*CC_BITS_IN_BYTE, /*P size in bits*/
            userPrivKey2,       /* sub-group order Q*/
            orderSizeWords); /* Q size in words*/

        /* check error */
        if (err != CC_OK) {
            goto End;
        }

        /* convert Z to BE bytes */
        CC_CommonInPlaceConvertBytesWordsAndArrayEndianness(pZ, primeSizeWords);

    }

End:
    if (err != CC_OK) {
        /* delete secure sensitive data */
        CC_PalMemSetZero((uint8_t*)pDhCtx, sizeof(DhContext_t));
    }

    return err;

}

#ifdef FFC_FURTHER_USING

/* The function updates offsets for further using */
static void FfcDhUpdateOffsets(DhContext_t *pDhCtx,
                   CCFfcDhUserPartyIs_t userParty)
{
    uint8_t *pTmp1;
    uint16_t size1, partnerInfoNewOffset;
    FfcDhSchemeDataOffsets_t *dataOffsets = &pDhCtx->dataOffsets;
    int32_t diffOffsets;

    if(userParty == CC_FFCDH_PARTY_U) {

        pTmp1 = &pDhCtx->extendDataBuffer[0] + pDhCtx->dataOffsets.userOtherDataOffset;
        size1 = FFCDH_GET_LENGTH(pTmp1); /* size of user Other data*/
        partnerInfoNewOffset = pDhCtx->dataOffsets.userOtherDataOffset + size1 +
                           CC_FFCDH_LENGTH_COUNTER_SIZE_IN_BYTES;
        /* difference between new and old offsets */
        diffOffsets = pDhCtx->dataOffsets.partnIdOffset - partnerInfoNewOffset;

        /* move partner info to new position */
        pTmp1 = &pDhCtx->extendDataBuffer[0] + partnerInfoNewOffset;
        FfcDhWriteBufferBeToBe(&pTmp1, &pDhCtx->extendDataBuffer[0] + pDhCtx->dataOffsets.partnIdOffset,
                             pDhCtx->dataOffsets.partnInfoSize + pDhCtx->dataOffsets.suppPublInfoSize +
                             pDhCtx->dataOffsets.suppPrivInfoSize);
        /* update */
        dataOffsets->partnIdOffset -= diffOffsets;
        dataOffsets->partnStatPublKeyOffset -= diffOffsets;
        dataOffsets->partnEphemPublKeyOffset -= diffOffsets;
        dataOffsets->partnNonceOffset -= diffOffsets;
        dataOffsets->partnOtherDataOffset -= diffOffsets;

        dataOffsets->suppPublInfoOffset = dataOffsets->partnIdOffset + CC_FFCDH_LENGTH_COUNTER_SIZE_IN_BYTES +
                          pDhCtx->dataOffsets.partnInfoSize;
        dataOffsets->suppPrivInfoOffset = dataOffsets->suppPublInfoOffset + CC_FFCDH_LENGTH_COUNTER_SIZE_IN_BYTES +
                                      dataOffsets->suppPublInfoSize;
    } else {
        pTmp1 = &pDhCtx->extendDataBuffer[0] + pDhCtx->dataOffsets.partnOtherDataOffset;
        size1 = FFCDH_GET_LENGTH(pTmp1); /* size of user Other data*/
        partnerInfoNewOffset = pDhCtx->dataOffsets.userOtherDataOffset + size1 +
                           CC_FFCDH_LENGTH_COUNTER_SIZE_IN_BYTES;
        /* difference between new and old offsets */
        diffOffsets = pDhCtx->dataOffsets.userOtherDataOffset - partnerInfoNewOffset;
        /* move user info to new position */
        FfcDhWriteBufferBeToBe(&pDhCtx->extendDataBuffer[0] + partnerInfoNewOffset,
                             &pDhCtx->extendDataBuffer[0] + pDhCtx->dataOffsets.partnIdOffset,
                             pDhCtx->dataOffsets.partnInfoSize + pDhCtx->dataOffsets.suppPublInfoSize +
                             pDhCtx->dataOffsets.suppPrivInfoSize);

        dataOffsets->userIdOffset -= diffOffsets;
        dataOffsets->userStatPublKeyOffset -= diffOffsets;
        dataOffsets->userEphemPublKeyOffset -= diffOffsets;
        dataOffsets->userNonceOffset -= diffOffsets;
        dataOffsets->userOtherDataOffset -= diffOffsets;

        dataOffsets->suppPublInfoOffset = dataOffsets->userIdOffset + CC_FFCDH_LENGTH_COUNTER_SIZE_IN_BYTES +
                          pDhCtx->dataOffsets.userInfoSize;
        dataOffsets->suppPrivInfoOffset = dataOffsets->suppPublInfoOffset + CC_FFCDH_LENGTH_COUNTER_SIZE_IN_BYTES +
                                      dataOffsets->suppPublInfoSize;
    }

    return;
}
#endif


/* The function compares party Info entries, inserted into DH context and
 * given by the partner in the PartnerInfo buffer.
 * If the data is not equaled, then the function returns an error. After the data
 * checking the function promotes the data pointers to the next Info entry.
 *
 */
static CCError_t  FfcDhCmpInfoEntries(uint8_t **ppCtxEntry, uint8_t **ppPartnEntry)
{
        CCError_t err = CC_OK; /* return error identifier */
        uint32_t calcSize, partnSize;

        calcSize = FFCDH_GET_LENGTH(*ppCtxEntry); /* size of PartnerId from Ctx; & pTmp1 += 2 */
        partnSize = FFCDH_GET_LENGTH(*ppPartnEntry); /*  -- " -- from Partner's data */
    CHECK_AND_SET_ERROR(calcSize != partnSize,  CC_FFCDH_PARTN_INFO_PARSING_SIZE_ERROR);
    CHECK_AND_SET_ERROR(CC_PalMemCmp(*ppCtxEntry, *ppPartnEntry, partnSize), CC_FFCDH_PARTN_INFO_PARSING_DATA_ERROR);
    /* promote pointers to next entry (partnStatPublKey) */
    *ppCtxEntry += calcSize;
    *ppPartnEntry += partnSize;

End:
    return err;
}


/******************************************************************************************/
/*!
@brief The function calculates user's confirmation MacTags for FFC DH Schemes according to NIST SP 56A rev.2 standard.

\note Before calling of this function the user should obtain assurance of used FFC Domain and public, private keys,
involved in the key agreement, using one of the methods, described in sec. 5.6.2 of above named standard.
<ul><li> - depending on DH Scheme, calculates confirmation HMAC MacTag, which is intended to be provided to the partner
(sec. 5.2, 5.9, 6); in this case the secret keying material is parsed to MacKey of size, equaled to HMAC key size. </li>
<li> - in our implementation HMAC key size defined equal to FFC sub-group order (meets to sec.5.9.3). </li>
<li> - if in the chosen DH Scheme the user is not Confirmation provider, then both the pointer and the size of
appropriate MacTag should be set to NULL. </li>
<li>  - for detailed description of Confirmation "MacData" see CCFfcDhConfirmMacData_t structure definition. </li></ul>

@return CC_OK on success.
@return A non-zero value on failure as defined in cc_dh_error.h, cc_kdf_error.h or cc_hash_error.h.
*/

static CCError_t FfcDhCalcConfirmMacTags(
                        DhContext_t *pDhCtx,               /*!< [in] pointer to the user's DH context structure, containing all data, defining
                                                                   DH Key Agreement Scheme. The context shall be initialized for user's roles
                                                                   (U or V; Provider or Receiver) using CC_FfcDhSetCtx function. */
                        uint8_t  *pUserConfirmText,        /*!< [in] optional, pointer to confirmation Text of the User. */
                        uint32_t  userConfirmTextSize,     /*!< [in] optional size of Text data of partyU, in bytes. */
                        uint8_t  *pPartnerConfirmText,     /*!< [in] optional, pointer to confirmation Text of the Partner. */
                        uint32_t  partnerConfirmTextSize)  /*!< [in] optional, size of Text data of partyV, in bytes. */

{
    /* Function Declarations    */

        CCError_t err = CC_OK; /* return error identifier */
        uint8_t messStr1[] = "KS_1_U", messStr2[] = "KS_2_U", strV[] = "V";
        uint8_t *pMessStr; /*a pointer to messageString*/
        uint8_t *pTmp;
    /* address of extended data buffer */
        uint8_t *pBaseAddr = &pDhCtx->extendDataBuffer[0];
        uint32_t fullLen = 0;

        CCHashOperationMode_t hashMode;
    const mbedtls_md_info_t *mdInfo = NULL;



#define MESSAGE_STR_SIZE 6


    /* if confirmation is required, then set formatted MacData string according
     * to scheme and user roles in confirmation (U,V and Provider - P, Recipient - R):
     *   MacData = messageString||IdP||IdR||EphemDataP||EphemDataR{||TextP}
     *   SP 800-56A sec. 5.9. */

/* set messageString according to Hybrid (or not) confirm. mode  */
    if((pDhCtx->dhSchemeId == CC_FFCDH_SCHEM_HYBRID1) ||
            (pDhCtx->dhSchemeId == CC_FFCDH_SCHEM_HYBRID_ONE_FLOW)) {
        pMessStr = &messStr2[0];
    } else {
        pMessStr = &messStr1[0];
    }

    /* get HASH mode and op. size */
    CHECK_ERROR(FfcGetHashMode(&hashMode, NULL, NULL/*pBlockSize*/,
                     NULL/*pDigestSize*/, pDhCtx->ffcHashMode));

        /* calculate MacTag for user as provider */
    if(pDhCtx->schemeInfo.doConfirmProvid == 1) {
        if(pDhCtx->userParty == CC_FFCDH_PARTY_V) {
            CC_PalMemCopy(&pMessStr[5], strV, 1); /*set user V letter*/
        }

        /* copy all required confirmation data to context buffer */
        pTmp = pBaseAddr + pDhCtx->dataOffsets.algIdOffset;
        FfcDhWriteBufferBeToBe(&pTmp, pMessStr, MESSAGE_STR_SIZE, &fullLen);
        FfcDhWriteBufferBeToBe(&pTmp, pDhCtx->userId, pDhCtx->userIdSizeBytes, &fullLen);
        FfcDhWriteBufferBeToBe(&pTmp, pDhCtx->partnerId, pDhCtx->partnerIdSizeBytes, &fullLen);
        FfcDhWriteBufferLeToBe(&pTmp, &pDhCtx->userEphemPublKey[0], pDhCtx->userStatPublKeySizeBytes>>2, &fullLen);
        FfcDhWriteBufferBeToBe(&pTmp, pDhCtx->userNonce, pDhCtx->userNonceSizeBytes, &fullLen);
        FfcDhWriteBufferLeToBe(&pTmp, &pDhCtx->partnerEphemPublKey[0], pDhCtx->partnerStatPublKeySizeBytes>>2, &fullLen);
        FfcDhWriteBufferBeToBe(&pTmp, pDhCtx->partnerNonce, pDhCtx->partnerNonceSizeBytes, &fullLen);
        FfcDhWriteBufferBeToBe(&pTmp, pUserConfirmText, userConfirmTextSize, &fullLen);

        /* call HMAC function with Key derived from pDhCtx->derivedKeyingMaterial  */
        mdInfo = mbedtls_md_info_from_string( HashAlgMode2mbedtlsString[hashMode] );
        if( mdInfo == NULL ){
            return CC_FFCDH_INVALID_HASH_MODE_ERROR;
        }
        err = mbedtls_md_hmac( mdInfo,
                            &pDhCtx->derivedKeyingMaterial[0]/*HmacKey*/,
                            CC_FFCDH_SIZE_OF_CONFIRM_MAC_KEY_IN_BYTES/*HmacKeySize*/,
                            pBaseAddr + pDhCtx->dataOffsets.algIdOffset/*data*/,
                            fullLen/*dataInSize*/,
                            (unsigned char*)pDhCtx->userMacTag/*result*/ );
        if (err != CC_OK){
            goto End;
        }
    }



    /* calculate MacTag for partner as provider */
    if(pDhCtx->schemeInfo.doConfirmRecip == 1) {
        if(pDhCtx->userParty == CC_FFCDH_PARTY_U) {
            CC_PalMemCopy(&pMessStr[5], strV, 1); /*set partner V letter*/
        }

        /* copy all required confirmation data to context buffer */
        pTmp = pBaseAddr + pDhCtx->dataOffsets.algIdOffset;
        FfcDhWriteBufferBeToBe(&pTmp, pMessStr, MESSAGE_STR_SIZE, &fullLen);
        FfcDhWriteBufferBeToBe(&pTmp, pDhCtx->partnerId, pDhCtx->partnerIdSizeBytes, &fullLen);
        FfcDhWriteBufferBeToBe(&pTmp, pDhCtx->userId, pDhCtx->userIdSizeBytes, &fullLen);
        FfcDhWriteBufferLeToBe(&pTmp, pDhCtx->partnerEphemPublKey, pDhCtx->partnerStatPublKeySizeBytes>>2, &fullLen);
        FfcDhWriteBufferBeToBe(&pTmp, pDhCtx->partnerNonce, pDhCtx->partnerNonceSizeBytes, &fullLen);
        FfcDhWriteBufferLeToBe(&pTmp, pDhCtx->userEphemPublKey, pDhCtx->userStatPublKeySizeBytes>>2, &fullLen);
        FfcDhWriteBufferBeToBe(&pTmp, pDhCtx->userNonce, pDhCtx->userNonceSizeBytes, &fullLen);
        FfcDhWriteBufferBeToBe(&pTmp, pPartnerConfirmText, partnerConfirmTextSize, &fullLen);


        err = mbedtls_md_hmac( mdInfo,
                            &pDhCtx->derivedKeyingMaterial[0]/*HmacKey*/,
                            CC_FFCDH_SIZE_OF_CONFIRM_MAC_KEY_IN_BYTES/*KeySize*/,
                            pBaseAddr + pDhCtx->dataOffsets.algIdOffset/*data*/,
                            fullLen/*dataInSize*/,
                            (unsigned char*)pDhCtx->partnerMacTag/*result*/);
        if (err != CC_OK){
            goto End;

        }
    }


End:
    return err;

}




/*******************************************************************************************/
/*!
@brief This function checks and sets given "OtherInfo" entries, calculates shared secret value and
       derives the "secret keying material".
       The function's implementation meets to NIST SP 56A rev.2 standard requirements.
\note Before calling of this function, DH Context should be initialized, DH Scheme, Domain parameters and all
required user's Private, Public keys or nonces are inserted by calling appropriate CC functions.
<ul><li>  The function sets input data into the Context to form the "OtherInfo" (sec. 5.8.1) according to
said standard and the implementation requirements:
<li>  - OtherInfo = AlgorithmId||PartyUInfo||PartyVInfo {||SuppPubInfo}{||SuppPrivInfo}, where each PartyInfo is
formatted as : </li>
<li>  - Remark: AlgorithmId includes information about length in bits of derived Keying Material and its
 parsing between internal using for confirmation HMAC algorithm and output Secret Keying Data
 and algorithm, which it is intended for. </li>
<li>  - PartyInfo = PartyId||PartyStatPublKey||PartyEphemKey||PartyNonce{||PartyOtherData}. </li>
<li>  - for detailed description of "OtherInfo" construction and concatenation its sub-entries, see
CCFfcDhOtherInfo_t structure definition; </li></ul>
\note - the function performs the following calculations:
<ul><li> - calculates shared secret value according to DH Scheme:
   -  SharedSecretVal = (PublKey1 ^ PrivKey1)  modulo Prime  or
   -  SharedSecretVal = (PartnPublKey1 ^ UserPrivKey1) || (PartnPublKey2 ^ UserPrivKey2)  modulo Prime; </li>
<li> - derives the secret keying material of required size from the shared secret value by calling KDF function
with shared OtherInfo data: DerivedKeyingMaterial = KDF(ZZ, OtherInfo, keyingMaterialSize); </li></ul>
<ul><li> - If DH Scheme includes Key Confirmation, then the function calculates confirmation HMAC MacTag, which is
intended to be provided to the partner (sec. 5.2, 5.9, 6); in this case the secret keying material is parsed to MacKey
of size, equaled to HMAC key size. </li>
<li> - in our implementation HMAC key size is defined to be equaled to FFC sub-group order (meets to sec.5.9.3). </li>
<li> - if in the chosen DH Scheme the user is not a Confirmation Provider, then both the pointer and the size of
appropriate MacTag should be set to NULL. </li>
<li>  - for detailed description of Confirmation "MacData" see CCFfcDhConfirmMacData_t structure definition. </li></ul>

@return CC_OK on success.
@return A non-zero value on failure as defined cc_dh_error.h.
*/

CEXPORT_C CCError_t CC_FfcDhSetAndCalculateSchemeData(
                        CCFfcDhUserContext_t *pDhUserCtx, /*!< [in/out] pointer to context structure, containing all data, used in DH Key
                                                                Agreement Scheme, required for implementation of said standard. */
                        /*! Partner's Data to be included into OtherInfo entry. Detailed description see in CCFfcDhOtherInfo_t. */
                        uint8_t *pPartnerInfo,            /*!< [in] pointer to the concatenated PartnerInfo. Detailed description see in CCFfcDhOtherInfo_t. */
                        size_t sizeOfPartnerInfo,         /*!< [in] size of PartnerInfo, in bytes, should be <= CC_FFCDH_MAX_SIZE_OF_PARTY_INFO_BYTES. */
                        CCFfcDhPartyInfoValidMode_t partnInfoValidMode, /*!< enumerator, defining which of public keys (static, ephemeral),
                                                                included in the PartnerInfo, should be full validated and which partial only. */
                        uint8_t *pSuppPubInfo,            /*!< [in] pointer to optional shared public data to be included into SuppPubInfo entry */
                        size_t suppPubInfoSize,           /*!< [in] size of SuppPubInfo data, in bytes. */
                        uint8_t *pSuppPrivInfo,           /*!< [in] pointer to optional shared private data to be included into SuppPrivInfo entry */
                        size_t suppPrivInfoSize,          /*!< [in] size of other SuppPrivInfo data, in bytes (should be not great than
                                                                    CC_FFCDH_MAX_SIZE_OF_OTHER_INFO_SUPPL_ENTRY_BYTES */
                        uint8_t *pUserMacTag,             /*!< [out] optional, pointer to the user-provider confirmation MacTag depending
                                                                   on used Key Agreement Scheme. The tag is calculated by HMAC with given
                                                                   hashMode, as described in SP800-56A sec. 5.9. */
                        size_t  macTagSize                /*!< [in] optional, required size in bytes of confirmation MacTag. */
)
{
    /* FUNCTION DECLARATIONS */

    CCError_t err = CC_OK; /* return error identifier */
    DhContext_t  *pDhCtx = NULL;
    uint8_t *pBaseAddr = NULL;
    uint8_t *pTmp1 = NULL/* Context data pointer */;
    uint8_t *pTmp2 = NULL/* Partner given data pointer */;
    uint32_t size1 = 0, size2 = 0, ffcOrderSize;
    CCFfcDhKeyValidMode_t partnStatKeyValidMode, partnEphemKeyValidMode;
    /* pointer and size of shared secret value. */
    uint8_t *pZZ;
    uint32_t zzSize;
    uint8_t *pOtherInfo;
    mbedtls_hkdf_hashmode_t hkdfHashMode;

//        CCFfcDhKeyValidMode_t validatMode;

    CHECK_AND_RETURN_ERR_UPON_FIPS_ERROR();

    /* check contexts pointers and tags */
    if (pDhUserCtx == NULL) {
    return CC_FFCDH_INVALID_CONTEXT_PTR_ERROR;
    }

    /* check that DhCtx is valid for user keys generation step: *
    *  DH Scheme, Domain and Keys are set.                      */
    CHECK_AND_SET_ERROR((pDhUserCtx->validTag != FFCDH_CTX_VALID_TAG_USER_INFO_SET),
                     CC_FFCDH_CONTEXT_VALIDATION_TAG_ERROR);
    /*  check the mandatory data pointers and sizes */
    CHECK_AND_SET_ERROR((pPartnerInfo == NULL), CC_FFCDH_INVALID_ARGUMENT_POINTER_ERROR);
    CHECK_AND_SET_ERROR(((sizeOfPartnerInfo == 0) || (sizeOfPartnerInfo > CC_FFCDH_MAX_SIZE_OF_PARTY_INFO_BYTES)),
                              CC_FFCDH_INVALID_ARGUMENT_SIZE_ERROR);
    /*  check the optional data pointers and sizes */
    CHECK_AND_SET_ERROR(((pSuppPubInfo == NULL) != (suppPubInfoSize == 0)) ||
                        (suppPubInfoSize > CC_FFCDH_MAX_SIZE_OF_OTHER_INFO_SUPPL_ENTRY_BYTES),
                              CC_FFCDH_OPTIONAL_DATA_ERROR);
    CHECK_AND_SET_ERROR(((pSuppPrivInfo == NULL) != (suppPrivInfoSize == 0)) ||
                        (suppPrivInfoSize > CC_FFCDH_MAX_SIZE_OF_OTHER_INFO_SUPPL_ENTRY_BYTES),
                              CC_FFCDH_OPTIONAL_DATA_ERROR);
    CHECK_AND_SET_ERROR(partnInfoValidMode >= CC_FFCDH_NO_FULL_VALIDAT_MODE, CC_FFCDH_INVALID_VALIDAT_MODE_ERROR);
    /*  check optional confirmation data pointers and sizes */
    CHECK_AND_SET_ERROR((pUserMacTag == NULL) != (macTagSize == 0), CC_FFCDH_OPTIONAL_DATA_ERROR);
//        CHECK_AND_SET_ERROR(((pUserConfirmText == NULL) != (userConfirmTextSize == 0)) ||
//                                 (userConfirmTextSize > CC_FFCDH_MAX_SIZE_OF_CONFIRM_TEXT_DATA_BYTES),
//                CC_FFCDH_OPTIONAL_DATA_ERROR);
//        CHECK_AND_SET_ERROR(((pPartnerConfirmText == NULL) != (partnerConfirmTextSize == 0)) ||
//                                 (partnerConfirmTextSize > CC_FFCDH_MAX_SIZE_OF_CONFIRM_TEXT_DATA_BYTES),
//                                  CC_FFCDH_OPTIONAL_DATA_ERROR);


        /* Get internal context structure */
    pDhCtx = (DhContext_t*)&pDhUserCtx->contextBuff;

    if(pDhCtx->schemeInfo.doConfirmProvid == 1) {
        CHECK_AND_SET_ERROR((pUserMacTag == NULL) || (macTagSize != pDhCtx->macTagSize),
                             CC_FFCDH_OPTIONAL_DATA_ERROR);
    }

    /* set base address to partner info = PartnerId in the DH Context. */
    pBaseAddr = &pDhCtx->extendDataBuffer[0] + pDhCtx->dataOffsets.partnIdOffset;
    pTmp1 = pBaseAddr;        /* in ctx */
    pTmp2 = &pPartnerInfo[0]; /* in received data */

        /* get FFC domain parameters */
    ffcOrderSize = pDhCtx->ffcDomain.ordLenWords*CC_32BIT_WORD_SIZE;

    /*-----------------------------------*/
    /* Check and import PartnerInfo data */
    /*-----------------------------------*/

    /* set validation mode for each of keys */
    if(partnInfoValidMode == CC_FFCDH_BOTH_KEYS_FULL_VALIDAT_MODE) {
        partnStatKeyValidMode = CC_FFCDH_KEY_VALIDAT_FULL_MODE;
        partnEphemKeyValidMode = CC_FFCDH_KEY_VALIDAT_FULL_MODE;
    } else {
        partnStatKeyValidMode  = CC_FFCDH_KEY_VALIDAT_PARTIAL_MODE;
        partnEphemKeyValidMode = CC_FFCDH_KEY_VALIDAT_PARTIAL_MODE;
    }
    if (partnInfoValidMode == CC_FFCDH_STAT_KEY_FULL_VALIDAT_MODE) {
        partnStatKeyValidMode = CC_FFCDH_KEY_VALIDAT_FULL_MODE;
    }
    if(partnInfoValidMode == CC_FFCDH_EPHEM_KEY_FULL_VALIDAT_MODE) {
        partnEphemKeyValidMode = CC_FFCDH_KEY_VALIDAT_FULL_MODE;
    }

    /* check partner ID data */
    CHECK_ERROR(FfcDhCmpInfoEntries(&pTmp1, &pTmp2));

//  size1 = FFCDH_GET_LENGTH(pTmp1); /* size of PartnerId from Ctx; & pTmp1 += 2 */
//  size2 = FFCDH_GET_LENGTH(pTmp2); /*  -- " -- from Partner's data */
//  CHECK_AND_SET_ERROR(size1 != size2,  CC_FFCDH_PARTN_INFO_PARSING_SIZE_ERROR);
//  CHECK_AND_SET_ERROR(CC_PalMemCmp(pTmp1, pTmp2, size2), CC_FFCDH_PARTN_INFO_PARSING_DATA_ERROR);
//  /* promote pointers to next entry (partnStatPublKey) */
//  pTmp1 += size1;
//  pTmp2 += size2;



/* ????? should be revised:  check that inserted = passeD  */

    /* check and (if needed) import the partner`s static PublKey.
     * note: the pointers will be updated to next buffer */
    if(pDhCtx->schemeInfo.doPartnerStatKey) {
        /* check and import the Key, promote pointers to next Info entries */
        err = FfcDhValidatePartnInfoPublKey(
            pDhCtx,
            partnStatKeyValidMode,
            &pTmp1   /* in context*/,
            &pTmp2); /* in PartnerInfo */
        CHECK_ERROR(err);
    } else {
        /* get sizes and promote pointers to next entry */
        size1 = FFCDH_GET_LENGTH(pTmp1);
        size2 = FFCDH_GET_LENGTH(pTmp2);
        /* check that given key size == 0, i.e. that partner not passes redundant key */
        CHECK_AND_SET_ERROR((size1 != 0) || (size2 != 0), CC_FFCDH_PARTN_INFO_PARSING_SIZE_ERROR);
    }

    /* check and (if needed) import the partner`s static PublKey.
     * note: the pointers will be updated to next buffer */
    if(pDhCtx->schemeInfo.doPartnerEphemKey) {
        /* check and import (if needed) the Key */
        err = FfcDhValidatePartnInfoPublKey(
            pDhCtx,
            partnEphemKeyValidMode,
            &pTmp1 /* in context*/,
            &pTmp2 /* in PartnerInfo */);
        CHECK_ERROR(err);
// ????
    } else if (pDhCtx->schemeInfo.doPartnerNonce){
        /* check and import nonce */
        size1 = FFCDH_GET_LENGTH(pTmp1); /* size of Partner's nonce from Ctx; & pTmp1 += 2 */
        size2 = FFCDH_GET_LENGTH(pTmp2); /*  -- " -- from Partner's data */
        if(size1 != 0) {
            CHECK_AND_SET_ERROR(size1 != size2, CC_FFCDH_PARTN_INFO_PARSING_SIZE_ERROR);
            CHECK_AND_SET_ERROR(CC_PalMemCmp(pTmp1, pTmp2, size1), CC_FFCDH_PARTN_INFO_PARSING_DATA_ERROR);
        } else {
            CHECK_AND_SET_ERROR(size2 != ffcOrderSize, CC_FFCDH_PARTN_INFO_PARSING_SIZE_ERROR);
            /* copy nonce into context */
            FfcDhWriteBufferBeToBe(&pTmp1, pTmp2,  size2, &pDhCtx->currInsertedDataSize);
        }
    } else {
        /* check that key size, given by Partner == 0, i.e. that partner not passes redundant key */
        CHECK_AND_SET_ERROR(size2 != 0, CC_FFCDH_PARTN_INFO_PARSING_SIZE_ERROR);
    }

    /* import Partner's Other data */
    size1 = FFCDH_GET_LENGTH(pTmp1); /* size of Partner Other data from Ctx; & pTmp1 += 2 */
    size2 = FFCDH_GET_LENGTH(pTmp2); /*  -- " -- from Partner's data */
    CHECK_AND_SET_ERROR(size1 != 0, CC_FFCDH_PARTN_INFO_PARSING_SIZE_ERROR);
    CHECK_AND_SET_ERROR(size2 > CC_FFCDH_MAX_SIZE_OF_PARTY_INFO_OTHER_DATA_BYTES, CC_FFCDH_PARTN_INFO_PARSING_SIZE_ERROR);
    FfcDhWriteBufferBeToBe(&pTmp1, pTmp2, size2, &pDhCtx->currInsertedDataSize); /* copy to Context */
    /* import Supplied public and private data */
    CHECK_AND_SET_ERROR(suppPubInfoSize > CC_FFCDH_MAX_SIZE_OF_OTHER_INFO_SUPPL_ENTRY_BYTES, CC_FFCDH_PARTN_INFO_PARSING_SIZE_ERROR);
    CHECK_AND_SET_ERROR(suppPrivInfoSize > CC_FFCDH_MAX_SIZE_OF_OTHER_INFO_SUPPL_ENTRY_BYTES, CC_FFCDH_PARTN_INFO_PARSING_SIZE_ERROR);
    FfcDhWriteBufferBeToBe(&pTmp1, pSuppPubInfo, suppPubInfoSize, &pDhCtx->currInsertedDataSize);
    FfcDhWriteBufferBeToBe(&pTmp1, pSuppPrivInfo, suppPrivInfoSize, &pDhCtx->currInsertedDataSize);

    /*--------------------------------------------------*/
    /* Calculate Shared Secret Value using DH primitive */
    /*--------------------------------------------------*/
    FfcDhCalcSharedSecretVal(pDhCtx);

    /*----------------------------------------------------------------------
    * Calculate Shared Secret Keying Data using HASH KDF using HMAC KDF.
    * Derivation method meets to requirements of SP 800-56A sec. 5.8, refer
    * to SP 800-56C. Note, that these standards allowing implementation
    * with application specified formatting of concatenated OtherInfo entries
    * and used HMAC modes. In partial, is allowed using RFC 5869 mode of HMAC
    * (SP 800-56C sec.4) with formatting, described below.
    *----------------------------------------------------------------------*/

    if(pDhCtx->kdfMode == CC_FFCDH_KDF_HMAC_RFC5869_MODE) {
        /* on this RFC 5869 mode is used the following format of input data to
         * key derivation function: on randomness extraction step are used: Salt (s)
         * and ShredSecretValue ZZ as message to generate pseudo random key (PRK);
         * on expansion step the function concatenates input DhOtherInfo and 1-byte counter
         * to derive the KeyingMaterial */

        /* get pointer and size of shared secret value */
        pZZ = &pDhCtx->extendDataBuffer[0] + (size_t)pDhCtx->dataOffsets.sharedSecrOffset;
        zzSize = FFCDH_GET_LENGTH(pZZ); /*get size and promote the pointer*/
        pOtherInfo = pZZ + zzSize; /*get OtherInfo*/

        /* get HKDF Hash mode ID */
        CHECK_ERROR(FfcGetHashMode(NULL/*pHashMode*/, &hkdfHashMode, NULL/*pBlockSize*/,
                         NULL/*pDigestSize*/, pDhCtx->ffcHashMode));

        /* call HKDF RFC5869 function */
        err = mbedtls_hkdf_key_derivation(
                hkdfHashMode,
                (uint8_t*)&pDhCtx->hmacSalt[0],
                (size_t)pDhCtx->hmacSaltSizeBytes,
                pZZ/*shared secret value - Ikm*/,
                zzSize/*shared secret size - IkmLen*/,
                pOtherInfo /*OtherInfo - Info*/,
                pDhCtx->currInsertedDataSize/*InfoLen*/,
                pDhCtx->derivedKeyingMaterial/*Okm*/,
                pDhCtx->derivedKeyingMaterialSize/*OkmLen*/,
                CC_FALSE/*IsStrongKkey*/);
    }

    /* if confirmation is required, then set formatted MacData string according
     * to scheme and user roles in confirmation (U,V and Provider - P, Recipient - R):
     *   MacData = messageString||IdP||IdR||EphemDataP||EphemDataR{||TextP}
     * (see SP 800-56A sec. 5.9). */
    if(pDhCtx->schemeInfo.doConfirmProvid) {
        err = FfcDhCalcConfirmMacTags(pDhCtx, pDhCtx->userConfirmText, pDhCtx->userConfirmTextSize,
                pDhCtx->partnerConfirmText, pDhCtx->partnerConfirmTextSize);
        CHECK_ERROR(err);

        /* output the user MacTag */
        CC_PalMemCopy(pUserMacTag, pDhCtx->userMacTag, macTagSize);
    }

    /* update validation tag  */
        pDhCtx->validTag |= FFCDH_CTX_VALID_TAG_SCHEM_DATA_BIT;

End:
    if(err != CC_OK) {
        CC_PalMemSetZero(pDhUserCtx, sizeof(CCFfcDhUserContext_t));
    }

        return err;

}


/*******************************************************************************************/
/*!
@brief This function performs DH Key Agreement Confirmation and, on success, outputs the shared keying data.
The function calculates expected partner's confirmation MacTag' and compares it to value,
received from the partner.
<li> If the tags are not equaled, then the function returns an error and zeroes the secure
sensitive data. </li>
<li> If no errors, the function puts the derived secret keying data into output buffer. </li>
\note Assumed, that the user yet have obtained assurance of public and private keys,
involved in the key agreement.
\note Before calling this function the user should perform all required DH Key Agreement
operations, including calculation of shared secret keying material by calling
CC_FfcDhCalcUserConfirmMacTag function.
\note If according to chosen Scheme the user is not a Confirmation Recipient,
then all, the pointer and the size of MacTag should be
set to zero, else the function returns an error.

@return CC_OK on success.
@return A non-zero value on failure as defined in cc_dh_error.h
*/
CEXPORT_C CCError_t CC_FfcDhGetSharedSecretKeyingData(
                        CCFfcDhUserContext_t *pDhUserCtx, /*!< [in] pointer to the user's DH context structure, containing all data,
                                                               defining DH Key Agreement Scheme and its results.  */
                        uint8_t *pSecretKeyData,          /*!< [out] pointer to the shared secret keying data, extracted
                                                                from keying material after parsing to . */
                        size_t  *pSecretKeyDataSize,      /*!< [in/out] the pointer to the size of shared secret key data:
                                                               in - size of the given output buffer, out - actual size of extracted
                                                               key data */
                        uint8_t *pPartnerMacTag,          /*!< [in] optional, pointer to the confirmation MacTag, provided by the partner */
                        size_t   macTagSize)              /*!< [in] optional, size of partner's MacTag, in bytes */
{
    CCError_t err = CC_OK; /* return error identifier */
    DhContext_t *pDhCtx;
    uint8_t *pKeyData;
    uint32_t hashBlockSize;

    CHECK_AND_RETURN_ERR_UPON_FIPS_ERROR();

    /* check contexts pointers and tags */
    if (pDhUserCtx == NULL) {
    return CC_FFCDH_INVALID_CONTEXT_PTR_ERROR;
    }
    /* check that DhCtx is valid for user keys generation step: *
    *  DH Scheme, Domain and Keys are set.                      */
    CHECK_AND_SET_ERROR((pDhUserCtx->validTag != FFCDH_CTX_VALID_TAG_SCHEM_DATA_SET),
                    CC_FFCDH_CONTEXT_VALIDATION_TAG_ERROR);
    /* check output keying data parameters */
    CHECK_AND_SET_ERROR(pSecretKeyData == NULL, CC_FFCDH_KEYING_DATA_PTR_INVALID_ERROR);
    CHECK_AND_SET_ERROR(pSecretKeyDataSize == NULL, CC_FFCDH_KEYING_DATA_SIZE_PTR_INVALID_ERROR);

    /* Get internal context structure */
    pDhCtx = (DhContext_t*)&pDhUserCtx->contextBuff;

    /* get HMAC key size in bytes */
    CHECK_ERROR(FfcGetHashMode(NULL/*pHashMode*/, NULL/*hkdfHashMode*/, &hashBlockSize,
                   NULL/*pDigestSize*/, pDhCtx->ffcHashMode));

    /* check that given output buffer is enough for the keying data */
    CHECK_AND_SET_ERROR(*pSecretKeyDataSize < pDhCtx->secretKeyingDataSize,
                   CC_FFCDH_KEYING_DATA_SIZE_INVALID_ERROR);
    /*  check optional confirmation data pointers and sizes */
    CHECK_AND_SET_ERROR((pPartnerMacTag == NULL) != (macTagSize == 0), CC_FFCDH_OPTIONAL_DATA_ERROR);

    /* if confirmation is needed check appropriate parameters */
    if(pDhCtx->schemeInfo.doConfirmRecip == 1) {
        CHECK_AND_SET_ERROR(pPartnerMacTag == NULL, CC_FFCDH_MAC_TAG_PTR_INVALID_ERROR);
        CHECK_AND_SET_ERROR(macTagSize != pDhCtx->macTagSize, CC_FFCDH_MAC_TAG_SIZE_INVALID_ERROR);

    /* compare calculated partner's MacTag with received value and return error,
     * if they are not equaled  */
    CHECK_AND_SET_ERROR(CC_PalMemCmp(pPartnerMacTag, pDhCtx->partnerMacTag, macTagSize),
                    CC_FFCDH_MAC_TAG_DATA_INVALID_ERROR);
    }

    /* set pointer to the second part of keying material, and output the
     * Secret Keying Data */
    pKeyData = &pDhCtx->extendDataBuffer[0] + pDhCtx->dataOffsets.sharedSecrOffset + hashBlockSize;
    *pSecretKeyDataSize = pDhCtx->secretKeyingDataSize;
    CC_PalMemCopy(pSecretKeyData, pKeyData, pDhCtx->secretKeyingDataSize);

End:
    CC_PalMemSetZero((uint8_t*)pDhUserCtx, sizeof(CCFfcDhUserContext_t));


    return err;

}



/*******************************************************************************************/
/*!
@brief This function implements FFC DH primitive according to section 5.7.1.1 of NIST SP 56A rev.2 standard.
       The function computes the shared secret value:  SharedSecretVal = partnerPublKey ^ userPrivKey modulo Prime.
\note Before calling of this function the user should obtain assurance of FFC Domain, public and private keys,
involved in the key agreement, using one of methods, described in section 5.6.2 of above named standard.
\note For assurance of keys validity the user can use appropriate APIs for generating or building and validation,
of keys, described in cc_ffcdh.h file.
@return CC_OK on success.
@return A non-zero value on failure as defined in cc_dh_error.h or cc_rnd_error.h.
*/
CEXPORT_C CCError_t CC_FfcDhGetSharedSecretVal(
                    CCFfcDomain_t *pDomain,         /*!< [in/out] pointer to DH FFC Context structure. */
                    uint8_t *pSharedSecretVal,      /*!< [out] pointer to the shared secret value in big endianness order
                                                              of bytes in the array (MS-byte is a most left one). This
                                                              buffer should be at least of prime (modulus) size in bytes. */
                    size_t *pSharedSecretValSize,   /*!< [in/out] pointer to the shared secret value size:
                                                              input - size of the given buffer, it should be at least
                                                              prime (modulus) size bytes; output - actual size. */
                    uint8_t *pPrivKeyData,          /*!< [in] pointer to given DH FFC private key in big endianness;
                                                              the Key should be in range [1, n-1], where n is the Domain
                                                              generator order. */
                    size_t privKeyDataSize,         /*!< [in] private key size, in bytes: should be not great than Domain
                                                              generator order size. */
                    uint8_t *pPublKeyData,          /*!< [in] pointer to given DH FFC public key in big endianness;
                                                              the key should be in range [2, P-2], where P is the Domain Prime. */
                    size_t publKeyDataSize,         /*!< [in] public key size, in bytes: should be not great than Domain Prime size. */
                    uint32_t *pTmpBuff              /*!< [in] pointer to temporary buffer of size = (Prime size + Order size). */
)
{

    CCError_t err = CC_OK; /* return error identifier */
    size_t primeSizeBytes, orderSizeBytes;
    size_t primeSizeWords, orderSizeWords;
    size_t sharSecrSizeBits;
    uint32_t *pPublKey32, *pPrivKey32;
    uint32_t word = 1; /* buffer = 1*/
    CCCommonCmpCounter_t cmp1, cmp2;

    CHECK_AND_RETURN_ERR_UPON_FIPS_ERROR();

    primeSizeWords = pDomain->modLenWords;
    orderSizeWords = pDomain->ordLenWords;
    primeSizeBytes = primeSizeWords * CC_32BIT_WORD_SIZE;
    orderSizeBytes = orderSizeWords * CC_32BIT_WORD_SIZE;

    /* check FFC Domain pointer and validation tag */
    CHECK_AND_RETURN_ERROR(pDomain == NULL, CC_FFCDH_INVALID_DOMAIN_PTR_ERROR);
    CHECK_AND_RETURN_ERROR((pDomain->validTag != CC_FFC_DOMAIN_VALIDATION_TAG),
                CC_FFCDH_INVALID_DOMAIN_DATA_ERROR);

    /* check in/out data pointers */
    CHECK_AND_SET_ERROR((pSharedSecretVal == NULL) || (pSharedSecretValSize == NULL),
                 CC_FFCDH_INVALID_SHARED_SECR_VAL_PTR_ERROR);
    CHECK_AND_SET_ERROR(pPrivKeyData == NULL, CC_FFCDH_INVALID_PRIV_KEY_PTR_ERROR);
    CHECK_AND_SET_ERROR(pPublKeyData == NULL, CC_FFCDH_INVALID_PUBL_KEY_PTR_ERROR);

    /* check that in/out sizes meets to DH Domain parameters and buffers sizes. */
    CHECK_AND_SET_ERROR(*pSharedSecretValSize < primeSizeBytes, CC_FFCDH_LOW_OUTPUT_BUFF_SIZE_ERROR);

    /* convert public/private keys to LE words */
    pPublKey32 = pTmpBuff;
    pPrivKey32 = pPublKey32 + pDomain->modLenWords;
    CHECK_AND_SET_ERROR(CC_CommonConvertMsbLsbBytesToLswMswWords(
                     pPublKey32, primeSizeBytes, pPublKeyData, publKeyDataSize),
                         CC_FFCDH_INVALID_PUBLIC_KEY_SIZE_ERROR);
    CHECK_AND_SET_ERROR(CC_CommonConvertMsbLsbBytesToLswMswWords(
                     pPrivKey32, orderSizeBytes, pPrivKeyData, privKeyDataSize),
                         CC_FFCDH_INVALID_PRIVATE_KEY_SIZE_ERROR);

    /* check public key 1 < publKey < prime-1 */
        word = 1;
        pDomain->prime[pDomain->modLenWords-1] ^= 1UL; /* temporary prime -= 1; */
    cmp1 = CC_CommonCmpLsWordsUnsignedCounters(pPublKey32, primeSizeWords, &word, 1);
    cmp2 = CC_CommonCmpLsWordsUnsignedCounters(pPublKey32, primeSizeWords, pDomain->prime, primeSizeWords);
    pDomain->prime[primeSizeWords-1] ^= 1UL; /* reset prime */
    CHECK_AND_SET_ERROR((cmp1 != CC_COMMON_CmpCounter1GreaterThenCounter2) ||
                (cmp2 != CC_COMMON_CmpCounter2GreaterThenCounter1),
                CC_FFCDH_INVALID_PUBLIC_KEY_VALUE_ERROR );

    /* check private key 0 < privlKey < order */
    word = 0;
    cmp1 = CC_CommonCmpLsWordsUnsignedCounters(pPrivKey32, primeSizeWords, &word, 1);
    cmp2 = CC_CommonCmpLsWordsUnsignedCounters(pPrivKey32, primeSizeWords, pDomain->order, orderSizeWords);
    CHECK_AND_SET_ERROR((cmp1 != CC_COMMON_CmpCounter1GreaterThenCounter2) ||
                (cmp2 != CC_COMMON_CmpCounter2GreaterThenCounter1),
                CC_FFCDH_INVALID_PRIVATE_KEY_VALUE_ERROR);

    /* calc. shared secret value */
    CHECK_ERROR(PkiExecModExpLeW(
            pPublKey32/*pOut*/, pPublKey32/*pIn data*/, primeSizeWords/*inSizeWords*/,
                    pDomain->prime/**pMod*/, primeSizeBytes*CC_BITS_IN_BYTE /*modSizeBits*/,
                    pPrivKey32/*pExp*/,  orderSizeWords/*expSizeWords*/));

    /* get actual size of shared secret value */
    sharSecrSizeBits = CC_CommonGetWordsCounterEffectiveSizeInBits(pPublKey32, primeSizeWords);
    *pSharedSecretValSize = ROUNDUP_BITS_TO_BYTES(sharSecrSizeBits);

    /* output result as BE bytes array without leading zeros */
    CHECK_ERROR(CC_CommonConvertLswMswWordsToMsbLsbBytes(pSharedSecretVal, *pSharedSecretValSize,
                                                 pPublKey32, primeSizeWords));

End:
    /* zero secret data */
    CC_PalMemSetZero(pTmpBuff, primeSizeBytes + orderSizeBytes);
    if(err) {
        CC_PalMemSetZero(pPrivKeyData, privKeyDataSize);
        CC_PalMemSetZero(pSharedSecretVal, primeSizeBytes);
    }


    return err;
}



