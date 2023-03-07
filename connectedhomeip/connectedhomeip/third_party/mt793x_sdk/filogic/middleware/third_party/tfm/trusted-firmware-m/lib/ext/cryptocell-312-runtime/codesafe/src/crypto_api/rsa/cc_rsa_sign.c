/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifdef CC_IOT
    #if defined(MBEDTLS_CONFIG_FILE)
    #include MBEDTLS_CONFIG_FILE
    #endif
#endif

#if !defined(CC_IOT) || ( defined(CC_IOT) && defined(MBEDTLS_RSA_C))

/************* Include Files ****************/
#include "cc_pal_mem.h"
#include <stdbool.h>
#include "cc_rsa_prim.h"
#include "cc_rsa_error.h"
#include "cc_rsa_types.h"
#include "cc_rsa_local.h"
#include "cc_fips_defs.h"

/************************ Defines ******************************/

/* canceling the lint warning:
   Use of goto is deprecated */
/*lint --e{801} */
#if ( CC_HASH_USER_CTX_SIZE_IN_WORDS > CC_PKA_RSA_HASH_CTX_SIZE_IN_WORDS )
#error CC_PKA_RSA_HASH_CTX_SIZE_IN_WORDS OR CC_HASH_USER_CTX_SIZE_IN_WORDS do not defined correctly.
#endif

/************************ Enums ******************************/
/************************ Typedefs ***************************/
/************************ Global Data ************************/
/************* Private function prototype ********************/


/***************** Public Functions **************************/

#ifndef _INTERNAL_CC_NO_RSA_SIGN_SUPPORT
/**********************************************************************************************************/
/**
   \brief RSA_SignInit initializes the Signing
   multi-call algorithm as defined in PKCS#1 v1.5 and 2.1

    NOTE: 1. In PSS_Sign v2.1 MD5 is not supported, since it is not recommended
         by the PKCS#1 v2.1.
          2. According to thesaid standard, implementation of the function
         for version v1.5 is based on DER encoding of the algorithm info.

           This function does not do cryptographic processing. Rather, it
           prepares a context that is used by the Update and Finish functions.

   @param[in,out] UserContext_ptr - A pointer to a Context. The value returned here
                                must be passed to the Update and Finish functions.
   @param[in] UserPrivKey_ptr - A pointer to the private key data structure.
                  \note The representation (pair or quintuple) and hence the
                  algorithm (CRT or not) is determined by the Private Key data structure.
                  Using of the CC_BuildPrivKey or CC_BuildPrivKeyCRT determines
                  which algorithm will be used.
   @param[in] rsaHashMode - The enumerator value, defining the hash function to be used:
             SHA-1SHA224/256/384/512, MD5 (MD5 allowed only in v1.5).
             The hash functions recommended by PKCS#1 v2.1 are:
                         256/384/512. Also allowed "After" HASH modes for said functions.
   @param[in] MGF - The mask generation function. PKCS#1 v2.1
                    defines MGF1, so the only value allowed here is CC_PKCS1_MGF1.
   @param[in] SaltLen - The Length of the Salt buffer (relevant for PKCS#1 Ver 2.1 only,
                        typically lengths is 0 or hLen). FIPS 186-4 requires, that SaltLen <= hlen.
                        If SaltLen > KeySize - hLen - 2, the function returns an error.
   @param[in] PKCS1_ver - Ver 1.5 or 2.1, according to the functionality required.

   @return CCError_t - CC_OK, or error
*/
CEXPORT_C CCError_t CC_RsaSignInit(CCRsaPrivUserContext_t *UserContext_ptr,
                       CCRsaUserPrivKey_t *UserPrivKey_ptr,
                       CCRsaHashOpMode_t rsaHashMode,
                       CCPkcs1Mgf_t MGF,
                       size_t  SaltLen,
                       CCPkcs1Version_t PKCS1_ver)
{
    /* FUNCTION DECLERATIONS */

    /* The return error identifier */
    CCError_t Error = CC_OK;

    /* defining a pointer to the active context allcated by the CCM */
    RSAPrivContext_t *ccmWorkingContext_ptr;
    /*Pointer to the private key*/
    CCRsaPrivKey_t *PrivKey_ptr ;
    /*The modulus size in Octets*/
    uint16_t ModulusSizeBytes = 0;
#ifdef USE_MBEDTLS_CRYPTOCELL
    const mbedtls_md_info_t *md_info=NULL;
#endif

    /* FUNCTION LOGIC */

    /* .... aquiring the RSA context ...... */
    ccmWorkingContext_ptr = (RSAPrivContext_t*)((void*)UserContext_ptr->context_buff);

    /* ............... checking the parameters validity ................... */
    /* -------------------------------------------------------------------- */

    /* if the users context ID pointer is NULL return an error */
    if (UserContext_ptr == NULL){
            return CC_RSA_INVALID_USER_CONTEXT_POINTER_ERROR;
    }

    /*if the private key object is NULL return an error*/
    if (UserPrivKey_ptr == NULL){
            Error = CC_RSA_INVALID_PRIV_KEY_STRUCT_POINTER_ERROR;
            goto End;
    }

    /* check if the hash operation mode is legal */
    if (rsaHashMode >= CC_RSA_HASH_NumOfModes){
            Error = CC_RSA_HASH_ILLEGAL_OPERATION_MODE_ERROR;
            goto End;
    }

    /* check if the MGF operation mode is legal */
    if (MGF >= CC_RSA_NumOfMGFFunctions){
            Error = CC_RSA_MGF_ILLEGAL_ARG_ERROR;
            goto End;
    }

    /* check that the PKCS1 version argument is legal*/
    if (PKCS1_ver >= CC_RSA_NumOf_PKCS1_versions){
            Error = CC_RSA_PKCS1_VER_ARG_ERROR;
            goto End;
    }

    if (UserPrivKey_ptr->valid_tag != CC_RSA_PRIV_KEY_VALIDATION_TAG){
            Error = CC_RSA_PRIV_KEY_VALIDATION_TAG_ERROR;
            goto End;
    }

    /*According to the PKCS1 ver 2.1 standard it is not recommended to use
         MD5 hash therefore we do not support it */
    if (PKCS1_ver == CC_PKCS1_VER21 && rsaHashMode == CC_RSA_HASH_MD5_mode){
            Error = CC_RSA_HASH_ILLEGAL_OPERATION_MODE_ERROR;
            goto End;
    }

    if (PKCS1_ver == CC_PKCS1_VER21) {
        /*Initializing the Modulus Size in Bytes needed for SaltLength parameter check*/
        PrivKey_ptr = (CCRsaPrivKey_t *)UserPrivKey_ptr->PrivateKeyDbBuff;

        /*Note: the (-1) is due to the PKCS#1 Ver2.1 standard section 9.1.1*/
        ModulusSizeBytes =  (uint16_t)((PrivKey_ptr->nSizeInBits -1) / 8);
        if ((PrivKey_ptr->nSizeInBits -1) % 8)
            ModulusSizeBytes++;
    }

    /*Reset the Context handler for improper previous values initialized*/
    CC_PalMemSetZero(UserContext_ptr, sizeof(CCRsaPrivUserContext_t));

    /* ................. loading the context .................................. */
    /* ------------------------------------------------------------------------ */

    /*Initializing the Hash operation mode in the RSA Context level*/
    ccmWorkingContext_ptr->RsaHashOperationMode = rsaHashMode;

    if (RsaSupportedHashModes_t[rsaHashMode] == CC_FALSE){
        Error = CC_RSA_HASH_ILLEGAL_OPERATION_MODE_ERROR;
        goto End;
    }

    ccmWorkingContext_ptr->HashOperationMode = RsaHashInfo_t[rsaHashMode].hashMode;
    ccmWorkingContext_ptr->HASH_Result_Size = RsaHashInfo_t[rsaHashMode].hashResultSize;

    if ( (ccmWorkingContext_ptr->HashOperationMode == CC_HASH_SHA384_mode) ||
         (ccmWorkingContext_ptr->HashOperationMode == CC_HASH_SHA512_mode) )
        ccmWorkingContext_ptr->HashBlockSize = CC_HASH_SHA512_BLOCK_SIZE_IN_WORDS;
    else
        ccmWorkingContext_ptr->HashBlockSize = CC_HASH_BLOCK_SIZE_IN_WORDS;


    if ( (rsaHashMode == CC_RSA_HASH_MD5_mode ) ||
         (rsaHashMode == CC_RSA_HASH_SHA1_mode ) ||
         (rsaHashMode == CC_RSA_HASH_SHA224_mode ) ||
         (rsaHashMode == CC_RSA_HASH_SHA256_mode ) ||
         (rsaHashMode == CC_RSA_HASH_SHA384_mode ) ||
         (rsaHashMode == CC_RSA_HASH_SHA512_mode ) )
        ccmWorkingContext_ptr->doHash = true; /* for actual Hash modes */
    else
        ccmWorkingContext_ptr->doHash = false;


    /* Init HASH */
    if (ccmWorkingContext_ptr->doHash) {
#ifdef USE_MBEDTLS_CRYPTOCELL
        md_info = mbedtls_md_info_from_type(RsaHash_CC_mbedtls_Info[ccmWorkingContext_ptr->HashOperationMode]);
        if (NULL == md_info) {
            goto End;
        }
        mbedtls_md_init(&ccmWorkingContext_ptr->RsaHashCtx);
        Error = mbedtls_md_setup(&ccmWorkingContext_ptr->RsaHashCtx, md_info, 0); // 0 = HASH, not HMAC
        if (Error != 0) {
            goto End;
        }

        Error = mbedtls_md_starts(&ccmWorkingContext_ptr->RsaHashCtx);
        if (Error != 0) {
            goto End;
        }
#else
        Error = CC_HashInit(
            ((CCHashUserContext_t *)((ccmWorkingContext_ptr->RsaHashCtxBuff))),
            ccmWorkingContext_ptr->HashOperationMode);
        if (Error != CC_OK)
            goto End;

#endif
    }


    /* Switch to appropriate PKCS1_version */
    /*-------------------------------------*/
    switch (PKCS1_ver) {
    case CC_PKCS1_VER15:
        ccmWorkingContext_ptr->PKCS1_Version=CC_PKCS1_VER15;
        break;

    case CC_PKCS1_VER21:
        /*Checking restriction of Salt Length ; Hash output size and the mosulus*/
        if (ModulusSizeBytes < (uint32_t)(ccmWorkingContext_ptr->HASH_Result_Size*4 + SaltLen + 2)) {
            Error = CC_RSA_PSS_ENCODING_MODULUS_HASH_SALT_LENGTHS_ERROR;
            goto End;
        }
        ccmWorkingContext_ptr->PKCS1_Version=CC_PKCS1_VER21;
        break;

    default:
        Error = CC_RSA_PKCS1_VER_ARG_ERROR;
        goto End;
    }

    /*  Set MGF indication */
    switch (MGF) {
    case CC_PKCS1_MGF1:
        ccmWorkingContext_ptr->MGF_2use = CC_PKCS1_MGF1;
        break;
    case CC_PKCS1_NO_MGF:
        ccmWorkingContext_ptr->MGF_2use = CC_PKCS1_NO_MGF;
        break;
    default:
        Error = CC_RSA_MGF_ILLEGAL_ARG_ERROR;
        goto End;
    }

    /* Copying the RSA Private key argument to the context*/
    CC_PalMemCopy((uint8_t*)&ccmWorkingContext_ptr->PrivUserKey,
        (uint8_t*)UserPrivKey_ptr, sizeof(CCRsaUserPrivKey_t));

    /*Initial the Salt random length relevant for PKCS#1 Ver2.1*/
    ccmWorkingContext_ptr->SaltLen = SaltLen;

    /* Set the RSA tag to the users context */
    UserContext_ptr->valid_tag = CC_RSA_SIGN_CONTEXT_VALIDATION_TAG;

End:

    /* .... Clearing the users context in case of error ... */
    if (Error != CC_OK) {
#ifdef USE_MBEDTLS_CRYPTOCELL
        if(md_info!=NULL){
                mbedtls_md_free(&ccmWorkingContext_ptr->RsaHashCtx);
        }
#endif
        CC_PalMemSetZero(UserContext_ptr, sizeof(CCRsaPrivUserContext_t));
    }

    return Error;

}/* CC_RsaSignInit */

/**********************************************************************************************************/
/**
   \brief CC_RsaSignUpdate processes the data to be signed
   in a given context.

   \note CC_RsaSignUpdate can be called multiple times
   with data

   @param[in] UserContext_ptr - A pointer to a valid context,
                as returned by CC_RsaSignInit.
   @param[in] DataIn_ptr - A pointer to the data to sign.
   @param[in] DataInSize - The size, in bytes, of the data to sign.

   @return CCError_t - CC_OK, or error
*/

CEXPORT_C CCError_t CC_RsaSignUpdate(CCRsaPrivUserContext_t *UserContext_ptr,
                     uint8_t     *DataIn_ptr,
                     size_t      DataInSize)
{
    /* FUNCTION DECLERATIONS */

    /* The return error identifier */
    CCError_t Error = CC_OK;

    /* defining a pointer to the active context allcated by the CCM */
    RSAPrivContext_t *ccmWorkingContext_ptr;

    /* FUNCTION LOGIC */

    /* ....... aquiring the RSA context ........ */
    ccmWorkingContext_ptr = (RSAPrivContext_t*)((void*)&UserContext_ptr->context_buff);

    /* ............... checking the parameters validity ................... */
    /* -------------------------------------------------------------------- */

    /* if the users context pointer is NULL return an error */
    if (UserContext_ptr == NULL){
        return CC_RSA_INVALID_USER_CONTEXT_POINTER_ERROR;
    }

    /* if the users Data In pointer is illegal return an error */
    if (DataIn_ptr == NULL && DataInSize) {
        Error =  CC_RSA_DATA_POINTER_INVALID_ERROR;
        goto End;
    }

    /* if the data size is larger then 2^29 (to prevant an overflow on the transition to bits )
       return error */
    if (DataInSize >= (1 << 29)) {
        Error =  CC_RSA_INVALID_MESSAGE_DATA_SIZE;
        goto End;
    }

    /* if the users context TAG is illegal return an error - the context is invalid */
    if (UserContext_ptr->valid_tag != CC_RSA_SIGN_CONTEXT_VALIDATION_TAG) {
        Error =  CC_RSA_USER_CONTEXT_VALIDATION_TAG_ERROR;
        goto End;
    }

    if (ccmWorkingContext_ptr->doHash) {
        /*Operate the Hash update function for relevant versions*/
#ifdef USE_MBEDTLS_CRYPTOCELL
        Error = mbedtls_md_update(&ccmWorkingContext_ptr->RsaHashCtx, DataIn_ptr, DataInSize);
        if ( Error != 0 )
            goto End;
#else
        Error=CC_HashUpdate( ((CCHashUserContext_t *)(ccmWorkingContext_ptr->RsaHashCtxBuff)),
                    DataIn_ptr,
                    DataInSize );
        if (Error != CC_OK)
            goto End;
#endif
    } else {
        /* DataInSize must fit exactly to the size of Hash output that we support */
        if (DataInSize != ccmWorkingContext_ptr->HASH_Result_Size*sizeof(uint32_t)) {
            Error = CC_RSA_INVALID_MESSAGE_DATA_SIZE_IN_SSL_CASE;
            goto End;
        }
        /* Copy the DataIn_ptr to the HashResult in case it is an SSL mode*/
        CC_PalMemCopy((uint8_t *)ccmWorkingContext_ptr->HASH_Result, DataIn_ptr, DataInSize);
    }

End:

    /* .... clearing the users context in case of error .... */
    if (Error != CC_OK) {
#ifdef USE_MBEDTLS_CRYPTOCELL
        mbedtls_md_free(&ccmWorkingContext_ptr->RsaHashCtx);
#endif
        CC_PalMemSetZero(UserContext_ptr, sizeof(CCRsaPrivUserContext_t));
    }

    return Error;


}/* CC_RsaSignUpdate */


/**********************************************************************************************************/
/**
   \brief CC_RsaSignFinish calculates the signature on the
   data passed to one or more calls to CC_RsaSignUpdate,
   and releases the context.

   @param[in/out] rndContext_ptr  - Pointer to the RND context buffer.
   @param[in,out] UserContext_ptr - A pointer to the Context
                initialized by the SignInit function
                and used by the SignUpdate function
   @param[out] Output_ptr - A pointer to the signature.
                The buffer must be at least PrivKey_ptr->N.len bytes long
                (that is, the size of the modulus, in bytes).
   @param[in,out] OutputSize_ptr - A pointer to the Signature Size value - the input value
                   is the signature buffer size allocated, the output value is
                   the signature size used.
                   The buffer must be at least PrivKey_ptr->N.len bytes long
                   (that is, the size of the modulus, in bytes).

   @return CCError_t - CC_OK,
             CC_RSA_INVALID_USER_CONTEXT_POINTER_ERROR,
             CC_RSA_USER_CONTEXT_VALIDATION_TAG_ERROR,
             CC_RSA_INVALID_OUTPUT_POINTER_ERROR,
             CC_RSA_INVALID_SIGNATURE_BUFFER_SIZE,
             CC_RSA_HASH_ILLEGAL_OPERATION_MODE_ERROR,
             CC_RSA_PKCS1_VER_ARG_ERROR
*/
CEXPORT_C CCError_t CC_RsaSignFinish(
                     CCRndContext_t *rndContext_ptr,
                     CCRsaPrivUserContext_t *UserContext_ptr,
             uint8_t                   *Output_ptr,
             size_t                    *OutputSize_ptr)
{
    /* FUNCTION DECLERATIONS */

    /* The return error identifier */
    CCError_t Error = CC_OK;
    /* defining a pointer to the active context allocated by the CCM */
    RSAPrivContext_t *ccmWorkingContext_ptr;
    /*The modulus size in Octets*/
    uint16_t K;
    CCRsaPrivKey_t *PrivKey_ptr;


    /* FUNCTION LOGIC */

    /* ................. aquiring the RSA context ............................. */
    ccmWorkingContext_ptr = (RSAPrivContext_t*)((void*)&UserContext_ptr->context_buff);

    /* ............... checking the parameters validity ................... */
    /* -------------------------------------------------------------------- */

    /* if the users context pointer is NULL return an error */
    if (UserContext_ptr == NULL){
        return CC_RSA_INVALID_USER_CONTEXT_POINTER_ERROR;
    }

    /*If the output pointer is NULL return Error*/
    if (Output_ptr == NULL) {
        Error =  CC_RSA_INVALID_OUTPUT_POINTER_ERROR;
        goto End1;
    }

    /*If the output Size pointer is NULL return Error*/
    if (OutputSize_ptr == NULL) {
        Error =  CC_RSA_INVALID_OUTPUT_SIZE_POINTER_ERROR;
        goto End1;
    }

    /* if the users context TAG is illegal return an error - the context is invalid */
    if (UserContext_ptr->valid_tag != CC_RSA_SIGN_CONTEXT_VALIDATION_TAG) {
        Error =  CC_RSA_USER_CONTEXT_VALIDATION_TAG_ERROR;
        goto End1;
    }


    /* ......... checking the validity of the prameters in the context ........ */
    /* ------------------------------------------------------------------------ */

    PrivKey_ptr = (CCRsaPrivKey_t *)ccmWorkingContext_ptr->PrivUserKey.PrivateKeyDbBuff;

    /*Initializing the Modulus Size in Bytes*/
    K =  (uint16_t)CALC_FULL_BYTES(PrivKey_ptr->nSizeInBits);

    /* If the received output buffer is small then return an error */
    if (*OutputSize_ptr < K) {
        Error = CC_RSA_INVALID_SIGNATURE_BUFFER_SIZE;
        goto End1;
    }

    /*Operating the HASH Finish function only in case that Hash operation is needed*/
    if (ccmWorkingContext_ptr->doHash) {
#ifdef USE_MBEDTLS_CRYPTOCELL
        Error = mbedtls_md_finish(&ccmWorkingContext_ptr->RsaHashCtx,
                                    (unsigned char *)ccmWorkingContext_ptr->HASH_Result);
        if ( Error != 0 )
            goto End;
#else
        Error = CC_HashFinish(((CCHashUserContext_t *)(ccmWorkingContext_ptr->RsaHashCtxBuff)),
                       ccmWorkingContext_ptr->HASH_Result);
        if (Error != CC_OK)
            goto End;
#endif
    }
    /* ........................... execute the signiture ........................... */
    /* ----------------------------------------------------------------------------- */

    switch (ccmWorkingContext_ptr->PKCS1_Version) {

#ifndef _INTERNAL_CC_NO_RSA_SCHEME_21_SUPPORT
    case CC_PKCS1_VER21:

        Error = RsaPssSign21(rndContext_ptr, ccmWorkingContext_ptr, Output_ptr);
        if (Error!=CC_OK)
            goto End;
        /* set the output size to the modulus size */
        *OutputSize_ptr = K;
        break;
#endif

#ifndef _INTERNAL_CC_NO_RSA_SCHEME_15_SUPPORT
    case CC_PKCS1_VER15:
        /*The ouput size should be of the Modulus size = K*/
        Error = RsaEmsaPkcs1v15Encode(
                K,
                ccmWorkingContext_ptr->HashOperationMode,
                (uint8_t*)ccmWorkingContext_ptr->HASH_Result,
                ccmWorkingContext_ptr->HASH_Result_Size*sizeof(uint32_t),
                (uint8_t*)ccmWorkingContext_ptr->EBD);

        if (Error!=CC_OK)
            goto End;

        /* ..........    execute RSA encryption   .......... */

        Error = CC_RsaPrimDecrypt(
                &ccmWorkingContext_ptr->PrivUserKey,
                &ccmWorkingContext_ptr->PrimeData,
                (uint8_t*)ccmWorkingContext_ptr->EBD,
                K, Output_ptr);

        if (Error!=CC_OK)
            goto End;
        /* set the output size to the modulus size */
        *OutputSize_ptr = K;
        break;
#endif
    default:
        Error = CC_RSA_PKCS1_VER_ARG_ERROR;
        goto End;

    }/* end of (ccmWorkingContext_ptr->PKCS1_Version ) switch */

End:
    if(Error != CC_OK) {
        CC_PalMemSetZero(Output_ptr, *OutputSize_ptr);
        *OutputSize_ptr = 0;
    }

End1:
#ifdef USE_MBEDTLS_CRYPTOCELL
    mbedtls_md_free(&ccmWorkingContext_ptr->RsaHashCtx);
#endif

    /* .... clearing the users context in case of error  ... */
    CC_PalMemSetZero(UserContext_ptr,sizeof(CCRsaPrivUserContext_t));

    return Error;

}/* CC_RsaSignFinish */


/**********************************************************************************************************/
/**
   @brief
   \brief RSA_Sign implements the RSASSA-PKCS1v15 algorithm
    in a single function as defined in PKCS#1 v2.1 standard, including v1.5.

    The user can call the function by appropriate macro according to choosen
    (and allowed) HASH algorithm SHA1, SHA224... (see macros below).

    NOTE: 1. In PSS_Sign v2.1 MD5 is not supported, since it is not recommended
         by the PKCS#1 v2.1.
          2. According to thesaid standard, implementation of the function
         for version v1.5 is based on DER encoding of the algorithm info.

   @param[in/out] rndContext_ptr  - Pointer to the RND context buffer.
   @param[in] UserContext_ptr - A pointer to a Context. For the use of the
                                function as a space to work on.
   @param[in] UserPrivKey_ptr - A pointer to the private key data
                            structure of the user. \note The representation
                            (pair or quintuple) and hence the
                            algorithm (CRT or not) is determined
                            by the Private Key data
                            structure - using CC_BuildPrivKey
                            or CC_BuildPrivKeyCRT determines
                            which algorithm will be used.
   @param[in] hashFunc - The hash functions supported: SHA1, SHA-256/224/264/512, MD5
                         (MD5 - allowed only for PKCS#1 v1.5).
             Also allowed "After" HASH modes for said functions.
   @param[in] MGF - The mask generation function (enum). Only for PKCS#1 v2.1
                    defines MGF1, so the only value allowed for v2.1
                    is CC_PKCS1_MGF1.
   @param[in] SaltLen - The Length of the Salt buffer (relevant for PKCS#1 Ver 2.1 only)
            Typical lengths are 0 and hLen (20 for SHA1)
            The maximum length allowed is NSize - hLen - 2.
   @param[in] DataIn_ptr - A pointer to the data to sign.
   @param[in] DataInSize - The size, in bytes, of the data to sign.
   @param[out] Output_ptr - A pointer to the signature.
                            The buffer must be at least PrivKey_ptr->N.len bytes long
                            (that is, the size of the modulus in bytes).
   @param[in,out] OutputSize_ptr - A pointer to the Signature Size value - the input value
                            is the signature buffer size allocated, the output value is
                            the signature size actually used.
                            The buffer must be at least PrivKey_ptr->N.len bytes long
                            (that is, the size of the modulus in bytes).
   @param[in] PKCS1_ver - Ver 1.5 or 2.1, according to the functionality required

   @return CCError_t - CC_OK,
                         CC_RSA_INVALID_USER_CONTEXT_POINTER_ERROR,
                         CC_RSA_INVALID_PRIV_KEY_STRUCT_POINTER_ERROR,
                         CC_RSA_PRIV_KEY_VALIDATION_TAG_ERROR,
                         CC_RSA_HASH_ILLEGAL_OPERATION_MODE_ERROR,
                         CC_RSA_MGF_ILLEGAL_ARG_ERROR,
                         CC_RSA_PKCS1_VER_ARG_ERROR,
                         CC_RSA_INVALID_MESSAGE_DATA_SIZE,
                         CC_RSA_INVALID_OUTPUT_POINTER_ERROR,
                         CC_RSA_INVALID_SIGNATURE_BUFFER_SIZE
*/
CEXPORT_C CCError_t CC_RsaSign(
                   CCRndContext_t *rndContext_ptr,
                   CCRsaPrivUserContext_t *UserContext_ptr,
           CCRsaUserPrivKey_t *UserPrivKey_ptr,
           CCRsaHashOpMode_t rsaHashMode,
           CCPkcs1Mgf_t MGF,
           size_t       SaltLen,
           uint8_t     *DataIn_ptr,
           size_t       DataInSize,
           uint8_t     *Output_ptr,
           size_t      *OutputSize_ptr,
           CCPkcs1Version_t PKCS1_ver)

{
    /* FUNCTION DECLARATIONS */

    /* The return error identifier */
    CCError_t Error = CC_OK;


    /* FUNCTION LOGIC */

    CHECK_AND_RETURN_ERR_UPON_FIPS_ERROR();

    /**********************************************************************
     *  RSA_SignInit
     **********************************************************************/
    Error = CC_RsaSignInit( UserContext_ptr,
                  UserPrivKey_ptr,
                  rsaHashMode,
                  MGF,
                  SaltLen,
                  PKCS1_ver);
    if (Error!=CC_OK)
        return Error;

    /**********************************************************************
     *  RSA_SignUpdate
     **********************************************************************/
    Error = CC_RsaSignUpdate(UserContext_ptr,
                   DataIn_ptr,
                   DataInSize);
    if (Error!=CC_OK)
        return Error;

    /**********************************************************************
     * RSA_SignFinish
     **********************************************************************/
    Error = CC_RsaSignFinish(
                   rndContext_ptr,
                   UserContext_ptr,
                   Output_ptr,
                   OutputSize_ptr);
    return Error;

}/* END OF CC_RsaSign */

#endif /*_INTERNAL_CC_NO_RSA_SIGN_SUPPORT*/
#endif /* !defined(CC_IOT) || ( defined(CC_IOT) && defined(MBEDTLS_RSA_C)) */
