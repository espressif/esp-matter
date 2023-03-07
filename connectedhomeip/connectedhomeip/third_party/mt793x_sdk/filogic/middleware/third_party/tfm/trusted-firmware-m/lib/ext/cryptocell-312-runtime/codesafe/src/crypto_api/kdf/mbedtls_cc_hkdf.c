/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/************* Include Files ****************/

#include "cc_pal_mem.h"
#include "cc_common_math.h"
#include "mbedtls_cc_hkdf.h"
#include "mbedtls_cc_hkdf_error.h"
#include "cc_fips_defs.h"
#include "mbedtls/md.h"
#include "cc_general_defs.h"

/************************ Defines *******************************/

/************************ Enums *********************************/

/************************ macros ********************************/


/************************    Global Data    ******************************/

/************************ Private Functions ******************************/

/**
 *   The function returns a number of attributes related to a given hkdf hash mode
 */
static CCError_t GetParamsFromHKDFHashMode(
        mbedtls_hkdf_hashmode_t    HKDFhashMode,
                CCHashOperationMode_t* HashMode_ptr,
                uint32_t*                  HashOutputSizeBytes_ptr,
                uint32_t*                  BlockSizeBytes_ptr,
        const mbedtls_md_info_t **mdInfo)
{
    *BlockSizeBytes_ptr = CC_HASH_BLOCK_SIZE_IN_BYTES;    /*for all modes, besides SHA512*/

    switch (HKDFhashMode) {
    case CC_HKDF_HASH_SHA1_mode:
        *HashMode_ptr = CC_HASH_SHA1_mode;
        *HashOutputSizeBytes_ptr = CC_HASH_SHA1_DIGEST_SIZE_IN_BYTES;
        break;
    case CC_HKDF_HASH_SHA224_mode:
        *HashMode_ptr  = CC_HASH_SHA224_mode;
        *HashOutputSizeBytes_ptr = CC_HASH_SHA224_DIGEST_SIZE_IN_BYTES;
        break;
    case CC_HKDF_HASH_SHA256_mode:
        *HashMode_ptr  = CC_HASH_SHA256_mode;
        *HashOutputSizeBytes_ptr = CC_HASH_SHA256_DIGEST_SIZE_IN_BYTES;
        break;
    case CC_HKDF_HASH_SHA384_mode:
        *HashMode_ptr = CC_HASH_SHA384_mode;
        *HashOutputSizeBytes_ptr = CC_HASH_SHA384_DIGEST_SIZE_IN_BYTES;
        *BlockSizeBytes_ptr = CC_HASH_SHA512_BLOCK_SIZE_IN_BYTES;
        break;
    case CC_HKDF_HASH_SHA512_mode:
        *HashMode_ptr = CC_HASH_SHA512_mode;
        *HashOutputSizeBytes_ptr = CC_HASH_SHA512_DIGEST_SIZE_IN_BYTES;
        *BlockSizeBytes_ptr = CC_HASH_SHA512_BLOCK_SIZE_IN_BYTES;
        break;
    default:
        *mdInfo = NULL;
        return CC_HKDF_INVALID_ARGUMENT_HASH_MODE_ERROR;
    }

    *mdInfo = mbedtls_md_info_from_string( HashAlgMode2mbedtlsString[*HashMode_ptr] );
    if( *mdInfo == NULL ){
        return CC_HKDF_INVALID_ARGUMENT_HASH_MODE_ERROR;
    }

    return CC_OK;
}

/****************************************************************/
/**
 * @brief HkdfExtract performs the extract stage of the HMAC-based key derivation, according to RFC5869.
    Computes a pseudo random key as PRK = HMAC_HASH (key=Salt , Data=Ikm)
*/
CCError_t  HkdfExtract(mbedtls_hkdf_hashmode_t HKDFhashMode,
                uint8_t*                salt_ptr,
                size_t                  salt_len,
                uint8_t*                ikm_ptr,
                uint32_t                ikm_len,
                uint8_t*                prk_ptr,
                uint32_t*               prk_len_ptr)
{
    /* The return error identifier */
    CCError_t Error = CC_OK;
    /* HASH function context structure buffer and parameters  */
    CCHashOperationMode_t hashMode;
    uint32_t  HashOutputSizeBytes;
    uint32_t  BlockSizeBytes;

    /*The result buffer for the Hash*/
    CCHashResultBuf_t   HmacResultBuff;
    uint8_t   SaltBuffer[CC_HKDF_MAX_HASH_KEY_SIZE_IN_BYTES]={0};
    const mbedtls_md_info_t *mdInfo = NULL;


    CHECK_AND_RETURN_ERR_UPON_FIPS_ERROR();

    if (prk_ptr == NULL || prk_len_ptr == NULL || ikm_ptr == NULL) {
        return CC_HKDF_INVALID_ARGUMENT_POINTER_ERROR;
    }

    Error = GetParamsFromHKDFHashMode(HKDFhashMode, &hashMode, &HashOutputSizeBytes, &BlockSizeBytes, &mdInfo);
    if (Error != CC_OK)
        goto End;

    if (*prk_len_ptr < HashOutputSizeBytes ) {
        return CC_HKDF_INVALID_ARGUMENT_SIZE_ERROR;
    }

    if (salt_ptr == NULL){
        if (salt_len!=0)
            return CC_HKDF_INVALID_ARGUMENT_SIZE_ERROR;
    }

    if (salt_len==0) {
        salt_len = (uint16_t)HashOutputSizeBytes;
        salt_ptr = SaltBuffer;
    }

    //check that salt len is not bigger than the macximum allowed size key
    if ( salt_len != (uint16_t)salt_len )
        return CC_HKDF_INVALID_ARGUMENT_SIZE_ERROR;

    Error = mbedtls_md_hmac( mdInfo,
                 salt_ptr, (uint16_t)salt_len,
                 ikm_ptr, ikm_len,
                 (unsigned char*)HmacResultBuff );
    if (Error != CC_OK){
        goto End;
    }


    /* Copying HASH data into output buffer */
    CC_PalMemCopy(prk_ptr,(uint8_t *)HmacResultBuff, HashOutputSizeBytes);
    *prk_len_ptr = HashOutputSizeBytes;

End:
    /* clean temp buffers */
    CC_PalMemSetZero(&HmacResultBuff, sizeof(CCHashResultBuf_t));

    return Error;

}/* END OF HkdfExtract */

/**
 * @brief HkdfExpand performs the expand stage of the HMAC-based key derivation, according to RFC5869.
    N = Ceil(L/HashLen)
    T = T(1) | T(2) | T(3) . . . . . | T(N)
    Computes the output key Material as follow OKM = first L octets of T
    where:
    T(0) = empty_string (zero length)
    T(1) = HMAC_HASH ( PRK, T(0) | info |0x01 )
    T(2) = HMAC_HASH ( PRK, T(1) | info |0x02 )
    T(N) = HMAC_HASH ( PRK, T(N-1) | info |N )   N<=255
*/
CCError_t  HkdfExpand(mbedtls_hkdf_hashmode_t HKDFhashMode,
                   uint8_t*                prk_ptr,
                   uint32_t                prk_len,
                   uint8_t*                info,
                   uint32_t                info_len,
                   uint8_t*                okm_ptr,
                   uint32_t                okm_len)
{
    uint32_t T[CC_HKDF_MAX_HASH_DIGEST_SIZE_IN_BYTES/sizeof(uint32_t)]={0};
    /* The return error identifier */
    CCError_t Error = CC_OK;
    /* HASH function context structure buffer and parameters  */
    CCHashOperationMode_t hashMode;
    uint32_t  HashOutputSizeBytes;
    uint32_t  BlockSizeBytes;

    uint32_t N;
    uint32_t i;
    uint8_t counter;
    uint32_t disp=0;
    const mbedtls_md_info_t *mdInfo = NULL;
    mbedtls_md_context_t hmacCtx;

    CHECK_AND_RETURN_ERR_UPON_FIPS_ERROR();

    if (info == NULL) {
        info_len = 0;
    }

    if (prk_ptr == NULL || prk_len == 0 || okm_ptr == NULL) {
        return CC_HKDF_INVALID_ARGUMENT_POINTER_ERROR;
    }

    Error = GetParamsFromHKDFHashMode(HKDFhashMode, &hashMode, &HashOutputSizeBytes, &BlockSizeBytes, &mdInfo);
    if (Error != CC_OK)
        return Error;

    if (prk_len < HashOutputSizeBytes) {
        return CC_HKDF_INVALID_ARGUMENT_SIZE_ERROR;
    }

    N = okm_len / HashOutputSizeBytes;
    if ( N*HashOutputSizeBytes != okm_len ){
        ++N;
    }

    if (N > 255)
        return CC_HKDF_INVALID_ARGUMENT_SIZE_ERROR;

    /* Initialize the hmac ctx */
    mbedtls_md_init( &hmacCtx );
    Error = mbedtls_md_setup( &hmacCtx, mdInfo, 1 );
    if (Error != CC_OK){
        goto End;
    }

    for (i=1; i<=N; i++) {
        counter = (uint8_t)i;


        Error = mbedtls_md_hmac_starts( &hmacCtx, prk_ptr, prk_len );
        if(Error != CC_OK) {
            goto End;
        }

        if (i != 1) {
            Error = mbedtls_md_hmac_update( &hmacCtx, (uint8_t*)T, HashOutputSizeBytes );
            if(Error != CC_OK) {
                goto End;
            }
        }

        Error = mbedtls_md_hmac_update( &hmacCtx, info, info_len );
        if(Error != CC_OK) {
            goto End;
        }

        Error = mbedtls_md_hmac_update( &hmacCtx, &counter, 1 );
        if(Error != CC_OK) {
            goto End;
        }

        Error = mbedtls_md_hmac_finish(&hmacCtx, (unsigned char*)T);
        if(Error != CC_OK) {
            goto End;
        }

        CC_PalMemCopy(okm_ptr+disp, T, (i!=N)?HashOutputSizeBytes:okm_len-disp);
        disp += HashOutputSizeBytes;
    }

    mbedtls_md_free( &hmacCtx );
    return Error;

End:
    /* Free the md context */
    mbedtls_md_free( &hmacCtx );
    /* clean outbuffer when error  */
    CC_PalMemSetZero(okm_ptr, okm_len);

    return Error;

}

/************************ Public Functions ******************************/

/**
 * @brief mbedtls_hkdf_key_derivation performs the HMAC-based key derivation, according to RFC5869
*/
CCError_t  mbedtls_hkdf_key_derivation(
            mbedtls_hkdf_hashmode_t HKDFhashMode,
                        uint8_t*                Salt_ptr,
                        size_t                SaltLen,
                        uint8_t*                Ikm_ptr,
                        uint32_t                IkmLen,
                        uint8_t*                Info,
                        uint32_t                InfoLen,
                        uint8_t*                Okm,
                        uint32_t                OkmLen,
                        CCBool                  IsStrongKkey
                        )
{
    /* The return error identifier */
    CCError_t Error = CC_OK;
    uint8_t   PRKBuffer[CC_HKDF_MAX_HASH_DIGEST_SIZE_IN_BYTES];
    uint32_t  PRKBuffer_Len = sizeof(PRKBuffer);

    if (IsStrongKkey == CC_FALSE) {
        Error = HkdfExtract(HKDFhashMode, Salt_ptr, SaltLen, Ikm_ptr, IkmLen,
                    PRKBuffer, &PRKBuffer_Len);

        if (Error != CC_OK){
            return Error;
        }


        Error = HkdfExpand(HKDFhashMode, PRKBuffer, PRKBuffer_Len, Info, InfoLen, Okm, OkmLen);
    }
    else { //skip extraction phase
        Error = HkdfExpand(HKDFhashMode, Ikm_ptr, IkmLen, Info, InfoLen, Okm, OkmLen);
    }

    return Error;
}


