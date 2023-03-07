/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/************* Include Files ****************/

#include "cc_pal_mem.h"
#include "cc_common_math.h"
#include "cc_kdf.h"
#include "cc_kdf_error.h"
#include "cc_fips_defs.h"
#include "cc_general_defs.h"
#ifdef USE_MBEDTLS_CRYPTOCELL
#include "md.h"
#else
#include "cc_hash.h"
#endif
#include "cc_hash_defs.h"

/************************ Defines *******************************/

/************************ Enums *********************************/

/************************ macros ********************************/


/************************    Global Data    ******************************/

/************************ Private Functions ******************************/

/************************************************************************
*   The function performs  Hash update for data with the size not
*   aligned to Hash block.
*
*   Note: remBuffSize_ptr - a pointer to the remaining size of the
*         temp buffer to fill by the data.
*
************************************************************************/

/**
 *   The function performs  Hash update for data with the size not
 *   aligned to Hash block.
 *
 *   Note: remBuffSize_ptr - a pointer to the remaining size of the
 *         temp buffer to fill by the data.
 *
 * @author reuvenl (4/3/2013)
 *
 * @param hashContext_ptr - HASH context pointer
 * @param data_ptr        - input data pointer
 * @param dataSize        - input data size in bytes
 * @param buff_ptr        - buffer for remaining data accumulation
 * @param remBuffSize_ptr - size of data on the buffer
 * @param blockSizeBytes  - size of HASH input block in bytes according to mode.
 *
 * @return CCError_t
 */
static CCError_t KdfHashUnalignUpdate(
#ifdef USE_MBEDTLS_CRYPTOCELL
                             mbedtls_md_context_t *p_hash_ctx,
#else
                             CCHashUserContext_t  *hashContext_ptr,
#endif
                             uint8_t *data_ptr, uint32_t dataSize,
                             uint8_t *buff_ptr, uint32_t *remBuffSize_ptr,
                             uint32_t blockSizeBytes)
{
    CCError_t error = CC_OK;
    uint32_t  tmpSize;
    uint8_t  *tmp_ptr;

    /* set buff_ptr to begin of empty part of temp buffer */
    tmp_ptr = buff_ptr + *remBuffSize_ptr;

    /* if the temp buffer not empty, append it by the data and update Hash on it */
    if (dataSize >= blockSizeBytes - *remBuffSize_ptr) {

        CC_PalMemCopy(tmp_ptr, data_ptr, blockSizeBytes - *remBuffSize_ptr);

        /* update on the data in temp buffer */
#ifdef USE_MBEDTLS_CRYPTOCELL
        error = mbedtls_md_update(p_hash_ctx, buff_ptr, blockSizeBytes);
#else
        error = CC_HashUpdate( hashContext_ptr, buff_ptr, blockSizeBytes);
#endif
        if (error != CC_OK)
            return error;

        /* update pointers and sizes */
        data_ptr += blockSizeBytes - *remBuffSize_ptr;
        dataSize -= blockSizeBytes - *remBuffSize_ptr;
        *remBuffSize_ptr = 0;
        tmp_ptr = buff_ptr;
    } else {
        CC_PalMemCopy(tmp_ptr, data_ptr, dataSize);
        *remBuffSize_ptr += dataSize;
        return error;
    }

    /* Update Hash on remaining input data */
    tmpSize = dataSize % blockSizeBytes;
    if (tmpSize > 0) {
        dataSize -= tmpSize;
        CC_PalMemCopy(tmp_ptr, data_ptr + dataSize, tmpSize);
        *remBuffSize_ptr += tmpSize;
    }

    if (dataSize > 0){
#ifdef USE_MBEDTLS_CRYPTOCELL
        error = mbedtls_md_update(p_hash_ctx, data_ptr, dataSize);
#else
        error = CC_HashUpdate( hashContext_ptr, data_ptr, dataSize);
#endif
    }

    return error;
}


/**
 * The function returns CC_HASH defined parameters according to given
 * KDF Hash mode
 *
 */
static CCError_t  KdfGetHashParameters(
                                CCKdfHashOpMode_t kdfhashMode,
                                CCHashOperationMode_t *pHashMode,
                                uint32_t *pHashBlockSize,
                                uint32_t *pHashDigestSize)
{
        switch (kdfhashMode) {
        case CC_KDF_HASH_SHA1_mode:
                *pHashMode = CC_HASH_SHA1_mode;
                *pHashDigestSize = CC_HASH_SHA1_DIGEST_SIZE_IN_BYTES;
                *pHashBlockSize = CC_HASH_BLOCK_SIZE_IN_BYTES;
                break;
        case CC_KDF_HASH_SHA224_mode:
                *pHashMode = CC_HASH_SHA224_mode;
                *pHashDigestSize = CC_HASH_SHA224_DIGEST_SIZE_IN_BYTES;
                *pHashBlockSize = CC_HASH_BLOCK_SIZE_IN_BYTES;
                break;
        case CC_KDF_HASH_SHA256_mode:
                *pHashMode = CC_HASH_SHA256_mode;
                *pHashDigestSize = CC_HASH_SHA256_DIGEST_SIZE_IN_BYTES;
                *pHashBlockSize = CC_HASH_BLOCK_SIZE_IN_BYTES;
                break;

        case CC_KDF_HASH_SHA384_mode:
                *pHashMode = CC_HASH_SHA384_mode;
                *pHashDigestSize = CC_HASH_SHA384_DIGEST_SIZE_IN_BYTES;
                *pHashBlockSize = CC_HASH_SHA512_BLOCK_SIZE_IN_BYTES;
                break;
        case CC_KDF_HASH_SHA512_mode:
                *pHashMode = CC_HASH_SHA512_mode;
                *pHashDigestSize = CC_HASH_SHA512_DIGEST_SIZE_IN_BYTES;
                *pHashBlockSize = CC_HASH_SHA512_BLOCK_SIZE_IN_BYTES;
                break;

        default:
                return CC_KDF_INVALID_ARGUMENT_HASH_MODE_ERROR;
        }

        return CC_OK;
}


/************************ Public Functions ******************************/

/************************ Public Functions ******************************/


/****************************************************************/


/****************************************************************/
/*!
 @brief CC_KdfKeyDerivFunc performs key derivation according to one of the modes defined in standards:
        ANS X9.42-2001, ANS X9.63, ISO/IEC 18033-2.

The present implementation of the function allows the following operation modes:
<ul><li> CC_KDF_ASN1_DerivMode - mode based on  ASN.1 DER encoding; </li>
<li> CC_KDF_ConcatDerivMode - mode based on concatenation;</li>
<li> CC_KDF_X963_DerivMode = CC_KDF_ConcatDerivMode;</li>
<li> CC_KDF_ISO18033_KDF1_DerivMode, CC_KDF_ISO18033_KDF2_DerivMode - specific modes according to
ISO/IEC 18033-2 standard.</li></ul>

The purpose of this function is to derive a keying data from the shared secret value and some
other optional shared information, included in OtherInfo (SharedInfo).

\note All buffers arguments are represented in Big-Endian format.

@return CC_OK on success.
@return A non-zero value on failure as defined cc_kdf_error.h.
*/
CCError_t  CC_KdfKeyDerivFunc(
                    uint8_t              *pZzSecret,            /*!< [in]  A pointer to shared secret value octet string. */
                    size_t                zzSecretSize,         /*!< [in]  The size of the shared secret value in bytes.
                                                                           The maximal size is defined as: ::CC_KDF_MAX_SIZE_OF_SHARED_SECRET_VALUE. */
                    CCKdfOtherInfo_t     *pOtherInfo,           /*!< [in]  A pointer to the structure, containing pointers to the data, shared by
                                       two entities of agreement, depending on KDF mode:
                                                                           1. On KDF ASN1 mode OtherInfo includes ASN1 DER encoding of AlgorithmID (mandatory),
                                                                             and some optional data entries as described in part 7.7.1 of the X9.42 standard;
                                                                           2. On both ISO18033-2 KDF1, KDF2 modes this parameter is ignored and may be set to NULL;
                                                                           3. On other modes it is optional and may be set to NULL. */
                    CCKdfHashOpMode_t     kdfHashMode,          /*!< [in]  The KDF identifier of hash function to be used. The hash function output
                                       must be at least 160 bits. */
                    CCKdfDerivFuncMode_t  derivMode,            /*!< [in]  The enum value, specifies one of above described derivation modes. */
                    uint8_t              *pKeyingData,          /*!< [out] A pointer to the buffer for derived keying data. */
                    size_t                keyingDataSize        /*!< [in]  The size in bytes of the keying data to be derived.
                                                                           The maximal size is defined as :: CC_KDF_MAX_SIZE_OF_KEYING_DATA. */
)

{

    /* FUNCTION DECLARATIONS */

    /* The return error identifier */
    CCError_t error = CC_OK;
    /* HASH function context structure buffer and parameters  */
    CCHashOperationMode_t hashMode;
    uint32_t  hashOutputSize;

    /*The result buffer for the Hash*/
    CCHashResultBuf_t   hashResultBuff;
    /* Total count of full HASH blockss for deriving the keying data */
    uint32_t  countOfHashBlocks;

    /* Loop counters */
    uint32_t  i, j;
    /*counter of Hash blocks (to be hashed with ZZ and OtherInfo) */
    uint32_t counter;
    /* Current output buffer position */
    uint32_t currentOutputBuffPos = 0;

    uint8_t   *pTemp;
    uint32_t  remBuffSize, hashBlockSize;
    uint32_t  kdfHashTempBuff[CC_HASH_SHA512_BLOCK_SIZE_IN_WORDS];
    CCKdfOtherInfoEntries_t fromKdfMode;

#ifdef USE_MBEDTLS_CRYPTOCELL
    const mbedtls_md_info_t *md_info=NULL;
    mbedtls_md_context_t hash_ctx;
#else
    CCHashUserContext_t  hashContext;
#endif

    CHECK_AND_RETURN_ERR_UPON_FIPS_ERROR();

    if (pZzSecret == NULL || pKeyingData == NULL) {
        return CC_KDF_INVALID_ARGUMENT_POINTER_ERROR;
    }

    if (derivMode >= CC_KDF_DerivFunc_NumOfModes) {
        return CC_KDF_INVALID_KEY_DERIVATION_MODE_ERROR;
    }

    if (derivMode == CC_KDF_ASN1_DerivMode &&
        (pOtherInfo == NULL || pOtherInfo->dataPointers[CC_KDF_ALGORITHM_ID] == 0)) {
        return CC_KDF_INVALID_ARGUMENT_POINTER_ERROR;
    }

    /*On KDF1 and KDF2 derivation modes set OtherInfo_ptr = NULL */
    if (derivMode == CC_KDF_ISO18033_KDF1_DerivMode ||
        derivMode == CC_KDF_ISO18033_KDF2_DerivMode) {
        pOtherInfo = NULL;
    }

    /* Check sizes of the input data to be hashed according to KDF        *
    *  limitations                            */
    if (zzSecretSize == 0 || zzSecretSize > CC_KDF_MAX_SIZE_OF_SHARED_SECRET_VALUE) {
        return CC_KDF_INVALID_SHARED_SECRET_VALUE_SIZE_ERROR;
    }

    /* Check the size of keying data output. Note: because max size is
       limited in our implementation by CC_KDF_MAX_SIZE_OF_KEYING_DATA
       bytes */
    if (keyingDataSize == 0 || keyingDataSize > CC_KDF_MAX_SIZE_OF_KEYING_DATA) {
        return  CC_KDF_INVALID_KEYING_DATA_SIZE_ERROR;
    }


        /* Get HASH parameters according to current operation modes */
        /*----------------------------------------------------------*/
        error = KdfGetHashParameters(
                        kdfHashMode,
                        &hashMode,
                        &hashBlockSize,
                        &hashOutputSize);
        if (error != CC_OK)
                goto End;


        /* Count of HASH blocks and temp buffer pointer and size */
    countOfHashBlocks = ( keyingDataSize + hashOutputSize - 1 )/ hashOutputSize;
    pTemp = (uint8_t*)&kdfHashTempBuff[0];

#ifdef USE_MBEDTLS_CRYPTOCELL
    md_info = mbedtls_md_info_from_string( HashAlgMode2mbedtlsString[hashMode] );
    if (NULL == md_info)
    {
        error = CC_KDF_INVALID_ARGUMENT_POINTER_ERROR;
        goto End;
    }
    mbedtls_md_init(&hash_ctx);
    error = mbedtls_md_setup(&hash_ctx, md_info, 0);    // 0 = HASH, not HMAC
    if (error != 0)
    {
        goto End;
    }
#endif

    /* **********  Keying data derivation loop ************ */

    for (i = 0; i < countOfHashBlocks; i++) {
        remBuffSize = 0;

        /*.... HASH Init function .....*/
#ifdef USE_MBEDTLS_CRYPTOCELL
        error = mbedtls_md_starts(&hash_ctx);
#else
        error = CC_HashInit(&hashContext, hashMode);
#endif
        if (error != CC_OK)
            goto End;

        /*....... Hashing input data by calling HASH_Update function .......*/
        /*------------------------------------------------------------------*/

        /*.... Hashing of the shared secret value ....*/
#ifdef USE_MBEDTLS_CRYPTOCELL
        error = KdfHashUnalignUpdate(&hash_ctx,
                                           pZzSecret,zzSecretSize,
                                           pTemp, &remBuffSize, hashBlockSize);
#else
        error = KdfHashUnalignUpdate(&hashContext,
                                           pZzSecret,zzSecretSize,
                                           pTemp, &remBuffSize, hashBlockSize);
#endif
        if (error != CC_OK)
            goto End;

        /*.... Hashing of the AlgorithmID (on ASN1 Derivation Mode only) ....*/
        if (derivMode == CC_KDF_ASN1_DerivMode) {
#ifdef USE_MBEDTLS_CRYPTOCELL
                error = KdfHashUnalignUpdate(&hash_ctx,
                                                       pOtherInfo->dataPointers[CC_KDF_ALGORITHM_ID],
                                                       pOtherInfo->dataSizes[CC_KDF_ALGORITHM_ID],
                                                       pTemp, &remBuffSize, hashBlockSize);
#else
            error = KdfHashUnalignUpdate(&hashContext,
                                                   pOtherInfo->dataPointers[CC_KDF_ALGORITHM_ID],
                                                   pOtherInfo->dataSizes[CC_KDF_ALGORITHM_ID],
                                                   pTemp, &remBuffSize, hashBlockSize);
#endif
            if (error != CC_OK)
                goto End;

            fromKdfMode = CC_KDF_PARTY_U_INFO;
        } else {
            fromKdfMode = CC_KDF_ALGORITHM_ID;
        }

        /* Set the blocks counter in big endianness mode */
        if (derivMode == CC_KDF_ISO18033_KDF1_DerivMode)
            counter = i;
        else
            counter = i+1;

#ifndef BIG__ENDIAN
        counter = CC_COMMON_REVERSE32(counter);
#endif

        /*.... Hashing of the blocks counter ....*/
#ifdef USE_MBEDTLS_CRYPTOCELL
        error = KdfHashUnalignUpdate(&hash_ctx,
                                           (uint8_t *)&counter,
                                           sizeof(uint32_t),
                                           pTemp, &remBuffSize,
                                           hashBlockSize);
#else
        error = KdfHashUnalignUpdate(&hashContext,
                                           (uint8_t *)&counter,
                                           sizeof(uint32_t),
                                           pTemp, &remBuffSize,
                                           hashBlockSize);
#endif
        if (error != CC_OK)
            goto End;

        /* ..... Hashing of remaining data of the OtherInfo ..... */
        if (pOtherInfo != NULL) {

            /* OtherInfo data concatenating and hashing loop */
            for (j = fromKdfMode; j < CC_KDF_MAX_COUNT_OF_ENTRIES; j++) {
                                /* if entry exists then hash it */
                if (pOtherInfo->dataPointers[j] != NULL && pOtherInfo->dataSizes[j] != 0) {
#ifdef USE_MBEDTLS_CRYPTOCELL
                        error = KdfHashUnalignUpdate(
                                                            &hash_ctx,
                                                            pOtherInfo->dataPointers[j]/*pointer to entry data*/,
                                                            pOtherInfo->dataSizes[j]/*size of entry data*/,
                                                            pTemp, &remBuffSize, hashBlockSize);
#else
                    error = KdfHashUnalignUpdate(
                                                        &hashContext,
                                                        pOtherInfo->dataPointers[j]/*pointer to entry data*/,
                                                        pOtherInfo->dataSizes[j]/*size of entry data*/,
                                                        pTemp, &remBuffSize, hashBlockSize);
#endif
                    if (error != CC_OK)
                        goto End;
                }
            }
        }

        /* last Hash update on remaining data in the temp buffer */
        if (remBuffSize > 0) {
#ifdef USE_MBEDTLS_CRYPTOCELL
            error = mbedtls_md_update(&hash_ctx, pTemp, remBuffSize);
#else
            error = CC_HashUpdate(&hashContext, pTemp, remBuffSize);
#endif
            if (error != CC_OK)
                goto End;
        }

        /* ..........  HASH Finish operation ............. */
#ifdef USE_MBEDTLS_CRYPTOCELL
        error = mbedtls_md_finish(&hash_ctx, (unsigned char *)hashResultBuff);
#else
        error = CC_HashFinish(&hashContext, hashResultBuff);
#endif
        if (error != CC_OK)
            goto End;

        /* Correction of output data size for last block ( if it is not full ) */
        if (i == (countOfHashBlocks - 1)){
                hashOutputSize = keyingDataSize - i * hashOutputSize;

        }
        /* Copying HASH data into output buffer */
        CC_PalMemCopy(&pKeyingData[currentOutputBuffPos],(uint8_t *)hashResultBuff, hashOutputSize);

        /* Increment the output buffer position */
        currentOutputBuffPos += hashOutputSize;
    }

End:
#ifdef USE_MBEDTLS_CRYPTOCELL
        if(md_info!=NULL){
                mbedtls_md_free(&hash_ctx);
        }
#endif
        /* clean temp buffers */
        CC_PalMemSetZero(&hashResultBuff, sizeof(CCHashResultBuf_t));
        CC_PalMemSetZero(&kdfHashTempBuff, sizeof(kdfHashTempBuff));
#ifdef USE_MBEDTLS_CRYPTOCELL
        CC_PalMemSetZero(&hash_ctx, sizeof(hash_ctx));
#else
        CC_PalMemSetZero(&hashContext, sizeof(hashContext));
#endif

    return error;

}/* END OF CC_KdfKeyDerivFunc */


