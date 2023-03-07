/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#define CC_PAL_LOG_CUR_COMPONENT CC_LOG_MASK_ASYM_ECC

/************* Include Files ****************/


#include "cc_pal_mem.h"
#include "cc_ecpki_error.h"
#include "cc_ecpki_local.h"
#include "cc_ecpki_ecdsa.h"
#include "cc_common.h"
#include "cc_common_math.h"
#include "cc_hash_defs.h"
#include "cc_rnd_common.h"
#include "cc_rsa_types.h"
#include "cc_fips_defs.h"
#include "ec_wrst.h"
#ifdef USE_MBEDTLS_CRYPTOCELL
#include "cc_general_defs.h"
#include "md.h"
#endif

/************************ Defines *****************************************/
#if ( CC_HASH_USER_CTX_SIZE_IN_WORDS > CC_PKA_RSA_HASH_CTX_SIZE_IN_WORDS )
        #error CC_PKA_RSA_HASH_CTX_SIZE_IN_WORDS or CC_HASH_USER_CTX_SIZE_IN_WORDS  defined not correctly.
#endif

/************************ Enums *******************************************/

/************************ Typedefs ****************************************/

/************************ Global Data *************************************/

extern const CCEcpkiHash_t ecpki_hash_info[CC_ECPKI_HASH_NumOfModes];
extern const uint8_t ecpki_supported_hash_modes[CC_ECPKI_HASH_NumOfModes];
/************* Private function prototype *********************************/

/************************ Public Functions ********************************/


/**************************************************************************
 *                EcdsaSignInit function
 **************************************************************************/
/**
   \brief
   The EcdsaSignInit functions user shall call first to perform the
   EC DSA Signing operation.

   The function performs the following steps:
   -# Validates all the inputs of the function. If one of the received
      parameters is not valid, the function returns an error.
   -# Initializes the working context and other variables and structures.
   -# Calls the CC_HashInit() function.
   -# Exits the handler with the OK code.

   This function does not do ECDSA cryptographic processing. Rather, it
   prepares a context that is used by the Update() and Finish() functions.

   NOTE: Using of HASH functions with HASH size great, than EC modulus size, is not recommended!


   @param[in,out] pSignUserContext A pointer to the user buffer for signing data.
   @param[in]     pSignerPrivKey   A pointer to the private key that will be used to
                                      sign the data.
   @param[in]     hashMode            Defines the hash mode used for DSA.

   @return <b>CCError_t</b>: <br>
                         CC_OK<br>
                         CC_ECDSA_SIGN_INVALID_USER_CONTEXT_PTR_ERROR
                         CC_ECDSA_SIGN_INVALID_USER_PRIV_KEY_PTR_ERROR
                         CC_ECDSA_SIGN_USER_PRIV_KEY_VALIDATION_TAG_ERROR
                         CC_ECDSA_SIGN_INVALID_DOMAIN_ID_ERROR
                         CC_ECDSA_SIGN_ILLEGAL_HASH_OP_MODE_ERROR
*/
CEXPORT_C CCError_t EcdsaSignInit(CCEcdsaSignUserContext_t  *pSignUserContext, /*in/out*/
                                    CCEcpkiUserPrivKey_t      *pSignerPrivKey,   /*in*/
                                    CCEcpkiHashOpMode_t       hashMode          /*in*/ )
{

        /* FUNCTION DECLARATIONS */

        /* The return error identifier */
        CCError_t err = CC_OK;
        /* defining a pointer to the active context allcated by the CCM */
        EcdsaSignContext_t *pWorkingContext;
#ifdef USE_MBEDTLS_CRYPTOCELL
        const mbedtls_md_info_t *md_info=NULL;
#endif

        /* FUNCTION LOGIC */
        pWorkingContext = (EcdsaSignContext_t*)&pSignUserContext->context_buff;

        /* ............... checking the parameters validity ................... */
        /* -------------------------------------------------------------------- */

        /* if the users context ID pointer is NULL return an error */
        if (pSignUserContext == NULL){
                return CC_ECDSA_SIGN_INVALID_USER_CONTEXT_PTR_ERROR;
        }

        /*if the private key object is NULL return an error*/
        if (pSignerPrivKey == NULL){
                err = CC_ECDSA_SIGN_INVALID_USER_PRIV_KEY_PTR_ERROR;
                goto End;
        }

        /* check if the hash operation mode is legal */
        if (hashMode >= CC_ECPKI_HASH_NumOfModes){
                err = CC_ECDSA_SIGN_ILLEGAL_HASH_OP_MODE_ERROR;
                goto End;
        }

        if (pSignerPrivKey->valid_tag != CC_ECPKI_PRIV_KEY_VALIDATION_TAG){
                err = CC_ECDSA_SIGN_USER_PRIV_KEY_VALIDATION_TAG_ERROR;
                goto End;
        }

        /* reset the Context handler for improper previous values initialized */
        CC_PalMemSetZero(pWorkingContext, sizeof(EcdsaSignContext_t));

        /* ................. loading the context .................................. */
        /* ------------------------------------------------------------------------ */

        /*Initializing the Hash operation mode in the ECDSA Context */

        if (ecpki_supported_hash_modes[hashMode] == CC_FALSE) {
                err = CC_ECDSA_SIGN_ILLEGAL_HASH_OP_MODE_ERROR;
                goto End;
        }

        pWorkingContext->hashMode = hashMode;
        pWorkingContext->hashResultSizeWords = ecpki_hash_info[pWorkingContext->hashMode].hashResultSize;

        if (ecpki_hash_info[pWorkingContext->hashMode].hashMode < CC_HASH_NumOfModes) {
#ifdef USE_MBEDTLS_CRYPTOCELL
        md_info = mbedtls_md_info_from_string( HashAlgMode2mbedtlsString[ecpki_hash_info[pWorkingContext->hashMode].hashMode] );
        if (NULL == md_info) {
                err = CC_ECDSA_SIGN_ILLEGAL_HASH_OP_MODE_ERROR;
                goto End;
        }
        mbedtls_md_init(&(pWorkingContext->hash_ctx));
        err = mbedtls_md_setup(&(pWorkingContext->hash_ctx), md_info, 0); // 0 = HASH, not HMAC
        if (err != 0) {
                goto End;
        }
        err = mbedtls_md_starts(&(pWorkingContext->hash_ctx));
#else
                err = CC_HashInit(&(pWorkingContext->hashUserCtxBuff),
                                     ecpki_hash_info[pWorkingContext->hashMode].hashMode);
#endif
                if (err != CC_OK)
                        goto End;
        }

        /* copy the ECPKI Private key to the context*/
        CC_PalMemCopy(&pWorkingContext->ECDSA_SignerPrivKey, pSignerPrivKey, sizeof(CCEcpkiUserPrivKey_t));

        /* set the ECDSA validation tag */
        pSignUserContext->valid_tag = CC_ECDSA_SIGN_CONTEXT_VALIDATION_TAG;

        End:
        /*  clear the users context in case of error */
        if (err != CC_OK) {
#ifdef USE_MBEDTLS_CRYPTOCELL
                if(md_info!=NULL){
                        mbedtls_md_free(&(pWorkingContext->hash_ctx));
                }
#endif
                CC_PalMemSetZero(pSignUserContext, sizeof(CCEcdsaSignUserContext_t));
        }

        return err;

}/* _DX_ECDSA_SignInit */



/**************************************************************************
 *                EcdsaSignUpdate function
 **************************************************************************/
/**
   @brief  Performs a hash  operation on data allocated by the user
           before finally signing it.

           In case user divides signing data by block, he must call the Update function
           continuously a number of times until processing of the entire data block is complete.

       NOTE: Using of HASH functions with HASH size great, than EC modulus size,
             is not recommended!

   @param [in,out] pSignUserContext - The pointer to the user buffer for signing the database.
   @param [in] pMessageDataIn - The pointer to the message data block for calculating the HASH.
   @param [in] dataInSize -  The size of the message data block, in bytes. The data size,
                   passed on each call of the function, besides the last call, must be a multiple of
                   the HASH block size according to used HASH mode.

   @return <b>CCError_t</b>: <br>
                         CC_OK<br>
                         CC_ECDSA_SIGN_INVALID_USER_CONTEXT_PTR_ERROR
                         CC_ECDSA_SIGN_USER_CONTEXT_VALIDATION_TAG_ERROR
                         CC_ECDSA_SIGN_INVALID_MESSAGE_DATA_IN_PTR_ERROR
                         CC_ECDSA_SIGN_INVALID_MESSAGE_DATA_IN_SIZE_ERROR
                         CC_ECDSA_SIGN_ILLEGAL_HASH_OP_MODE_ERROR
 */
CEXPORT_C CCError_t EcdsaSignUpdate(
                                           CCEcdsaSignUserContext_t  *pSignUserContext,  /*in/out*/
                                           uint8_t                       *pMessageDataIn,    /* in */
                                           size_t                        dataInSize         /* in */ )
{
        /* FUNCTION DECLERATIONS */

        /* The return error identifier */
        CCError_t err = CC_OK;

        /*  pointers to the inner contexts */
        EcdsaSignContext_t *pWorkingContext;

        /* FUNCTION LOGIC */

        /* sign working context */
        pWorkingContext = (EcdsaSignContext_t*)&pSignUserContext->context_buff;

        /* ....... checking the parameters validity ......... */
        /* -------------------------------------------------------------------- */

        /* if the users context pointer is NULL return an error */
        if (pSignUserContext == NULL){
                return CC_ECDSA_SIGN_INVALID_USER_CONTEXT_PTR_ERROR;
        }

        /* if the users context TAG is illegal return an error - the context is invalid */
        if (pSignUserContext->valid_tag != CC_ECDSA_SIGN_CONTEXT_VALIDATION_TAG) {
                err = CC_ECDSA_SIGN_USER_CONTEXT_VALIDATION_TAG_ERROR;
                goto End;
        }

        /* if the users MessageDataIn pointer is illegal return an error */
        if (pMessageDataIn == NULL && dataInSize) {
                err = CC_ECDSA_SIGN_INVALID_MESSAGE_DATA_IN_PTR_ERROR;
                goto End;
        }

        /* check that the data size < 2^29 (to prevent an overflow on the
           transition to bits ) */
        if (dataInSize >= (1UL << 29)) {
                err = CC_ECDSA_SIGN_INVALID_MESSAGE_DATA_IN_SIZE_ERROR;
                goto End;
        }

        /* HASH update operations */
        if (ecpki_hash_info[pWorkingContext->hashMode].hashMode < CC_HASH_NumOfModes) {
                /*Operate the Hash update function for relevant version */
#ifdef USE_MBEDTLS_CRYPTOCELL
                err = mbedtls_md_update(&(pWorkingContext->hash_ctx), pMessageDataIn, dataInSize);
#else
                err = CC_HashUpdate( &(pWorkingContext->hashUserCtxBuff), pMessageDataIn, dataInSize );
#endif
                if (err != CC_OK) {
                        goto End;
                }
        } else {
                if (dataInSize != pWorkingContext->hashResultSizeWords*sizeof(uint32_t)) {
                        /* DataInSize must fit exactly to the size of Hash output that we support */
                        err = CC_ECDSA_SIGN_INVALID_MESSAGE_DATA_IN_SIZE_ERROR;
                        goto End;
                }
                /* Copy the DataIn_ptr to the HASH_Result */
                CC_PalMemCopy((uint8_t*)pWorkingContext->hashResult, pMessageDataIn, dataInSize);
        }


        End:
        /*  clear the users context in case of error */
        if (err != CC_OK) {
#ifdef USE_MBEDTLS_CRYPTOCELL
                mbedtls_md_free(&(pWorkingContext->hash_ctx));
#endif
                CC_PalMemSetZero(pSignUserContext, sizeof(CCEcdsaSignUserContext_t));
        }

        return err;

}/* EcdsaSignUpdate */




/**************************************************************************
 *                EcdsaSignFinishInt function
 **************************************************************************/
/**
   @brief  Performs initialization of variables and structures, calls the hash function
           for the last block of data (if necessary) and then calculates digital signature.

           NOTE: Using of HASH functions with HASH size great, than EC modulus size,
                 is not recommended!
             Algorithm according ANS X9.62 standard

   @param[in] pSignUserContext -  A pointer to the user buffer for signing database.
   @param[in,out] pRndContext - A pointer to the random generation function context.
   @param[in] pSignatureOut - A pointer to a buffer for output of signature.
   @param[in,out] pSignatureOutSize- A pointer to the size of a user passed buffer
                     for signature (in), be not less than 2*orderSizeInBytes.
   @param[out] isEphemerKeyInternal - A parameter defining whether the ephemeral key
                     is internal or external (1 or 0).
   @param[out] pEphemerKeyData - A pointer to external ephemeral key data. If it is given
               (in case isEphemerKeyInternal=0), then the buffer must containing the
               ephemeral private key of size equal to EC generator order size, where
               LS-word is left most and MS-word is right most one.

   @return <b>CCError_t</b>: <br>
                         CC_OK<br>
                         CC_ECDSA_SIGN_INVALID_USER_CONTEXT_PTR_ERROR <br>
                         CC_ECDSA_SIGN_USER_CONTEXT_VALIDATION_TAG_ERROR <br>
                         CC_ECDSA_SIGN_INVALID_RND_CONTEXT_PTR_ERROR <br>
                         CC_ECDSA_SIGN_INVALID_SIGNATURE_OUT_PTR_ERROR <br>
                         CC_ECDSA_SIGN_ILLEGAL_HASH_OP_MODE_ERROR <br>
                         CC_ECDSA_SIGN_INVALID_SIGNATURE_OUT_SIZE_PTR_ERROR <br>
                         CC_ECDSA_SIGN_INVALID_SIGNATURE_OUT_SIZE_ERROR <br>
                         CC_ECDSA_SIGN_INVALID_IS_EPHEMER_KEY_INTERNAL_ERROR <br>
                         CC_ECDSA_SIGN_INVALID_EPHEMERAL_KEY_PTR_ERROR <br>
**/
CEXPORT_C  CCError_t EcdsaSignFinishInt(
                                           CCEcdsaSignUserContext_t   *pSignUserContext, /*in*/
                                           CCRndContext_t             *pRndContext,      /*in/out*/
                                           uint8_t                        *pSignOut,         /*out*/
                                           size_t                         *pSignOutSize,     /*in/out*/
                                           uint32_t                        isEphemerKeyInternal,/*in*/
                                           uint32_t                       *pEphemerKeyData   /*in*/)
{
        /* FUNCTION DECLARATIONS */

        /* The return error identifier */
        CCError_t err = CC_OK;

        /* pointer to the active context  */
        EcdsaSignContext_t  *pWorkingContext;

        /* pointer to private key structure in ccmWorkingContext  */
        CCEcpkiPrivKey_t  *pPrivKey;
        /*  pointer to the current domain */
        CCEcpkiDomain_t  *pDomain;

        uint32_t  orderSizeInBytes = 0, orderSizeInWords = 0;
        uint32_t  *pSignC, *pSignD;
        uint32_t  *pMessRepres;
        uint32_t  hashSizeWords;
        uint32_t  *pTempBuff;

        /* FUNCTION LOGIC */

        /* the pointer to the internal Sign context */
        pWorkingContext = (EcdsaSignContext_t*)&pSignUserContext->context_buff;

        /* ............... checking the parameters validity ................... */
        /* -------------------------------------------------------------------- */

        /* check the user context and RND context pointers */
        if (pSignUserContext == NULL){
                return CC_ECDSA_SIGN_INVALID_USER_CONTEXT_PTR_ERROR;
        }

        if (pRndContext == NULL){
                err = CC_ECDSA_SIGN_INVALID_RND_CONTEXT_PTR_ERROR;
                goto End;
        }

        /* check the user's context tag  */
        if (pSignUserContext->valid_tag != CC_ECDSA_SIGN_CONTEXT_VALIDATION_TAG){
                err = CC_ECDSA_SIGN_USER_CONTEXT_VALIDATION_TAG_ERROR;
                goto End;
        }

        /* check the user's SignatureOut and SignatureOutSize pointers */
        if (pSignOut == NULL){
                err = CC_ECDSA_SIGN_INVALID_SIGNATURE_OUT_PTR_ERROR;
                goto End;
        }

        if (pSignOutSize == NULL){
                err = CC_ECDSA_SIGN_INVALID_SIGNATURE_OUT_SIZE_PTR_ERROR;
                goto End;
        }

        /* check isEphemerKeyInternal value and ephemeral key data pointer */
        if (isEphemerKeyInternal > 1){
                err = CC_ECDSA_SIGN_INVALID_IS_EPHEMER_KEY_INTERNAL_ERROR;
                goto End;
        }

        if (isEphemerKeyInternal == 0 && pEphemerKeyData == NULL){
                err = CC_ECDSA_SIGN_INVALID_EPHEMERAL_KEY_PTR_ERROR;
                goto End;
        }

        /* ............. checking the validity of context ........ */
        /* ------------------------------------------------------- */

        /* check Hash mode */
        if (pWorkingContext->hashMode >= CC_ECPKI_HASH_NumOfModes){
                err = CC_ECDSA_SIGN_ILLEGAL_HASH_OP_MODE_ERROR;
                goto End;
        }

        pPrivKey = (CCEcpkiPrivKey_t *)&pWorkingContext->ECDSA_SignerPrivKey.PrivKeyDbBuff;
        /* Initializing domain parameters */
        pDomain = &pPrivKey->domain;
        orderSizeInBytes =  CALC_FULL_BYTES(pDomain->ordSizeInBits);
        orderSizeInWords  =  CALC_FULL_32BIT_WORDS(pDomain->ordSizeInBits);
        hashSizeWords  = pWorkingContext->hashResultSizeWords;

        /* Temp buffers */
        pMessRepres = ((EcWrstDsaSignDb_t*)&pWorkingContext->ecdsaSignIntBuff)->tempBuff;
        pSignC       = pMessRepres + orderSizeInWords;
        pSignD       = pSignC + orderSizeInWords;
        pTempBuff    = pSignD + orderSizeInWords;

        /* If the received output buffer is small than 2*orderSizeInBytes then return an error */
        if (*pSignOutSize < 2*orderSizeInBytes){
                err = CC_ECDSA_SIGN_INVALID_SIGNATURE_OUT_SIZE_ERROR;
                goto End;
        }

        /* Operating the HASH Finish function; only if it is needed */
        if (pWorkingContext->hashMode <= CC_ECPKI_HASH_SHA512_mode) {
#ifdef USE_MBEDTLS_CRYPTOCELL
                err = mbedtls_md_finish(&(pWorkingContext->hash_ctx), (unsigned char *)pWorkingContext->hashResult);
#else
                err = CC_HashFinish(&(pWorkingContext->hashUserCtxBuff), pWorkingContext->hashResult);
#endif
                if (err != CC_OK)
                        goto End;
        }

        /* Derive message representative from HASH_Result: MessageRepresent =
           leftmost OrderSizeInBits bits of HASH_Result */

        /* Set 0 to MessageRepresent buffer of length OrdSizeInWords */
        CC_PalMemSetZero(pMessRepres, sizeof(uint32_t)*orderSizeInWords);

        /* Derive message representative = leftmost OrderSizeInBits bits of HASH_Result */
        /* Add change Endianness for BE CPU */
        if (pDomain->ordSizeInBits >= 32*hashSizeWords) {
                CC_CommonReverseMemcpy((uint8_t*)pMessRepres, (uint8_t*)(pWorkingContext->hashResult),
                                       sizeof(uint32_t)*hashSizeWords);
        } else {
                EcWrstDsaTruncateMsg(pMessRepres,
                                     (uint8_t*)(pWorkingContext->hashResult), pDomain->ordSizeInBits);
        }


        /* ********  Call LLF ECDSA Sinature function  ************ */
        err =  EcWrstDsaSign(
                                     pRndContext, pPrivKey,
                                     pMessRepres,
                                     isEphemerKeyInternal, pEphemerKeyData,
                                     pSignC, pSignD, pTempBuff);

        if (err != CC_OK) {
                err = CC_ECDSA_SIGN_SIGNING_ERROR;
                goto End;
        }

        /* Output the reversed C,D strings of length orderSizeInBytes */
        err = CC_CommonConvertLswMswWordsToMsbLsbBytes(
                                                         pSignOut, orderSizeInBytes,
                                                         pSignC, orderSizeInBytes);
        if (err != CC_OK) {
                err = CC_ECDSA_SIGN_INVALID_SIGNATURE_OUT_SIZE_ERROR;
                goto End;
        }

        err = CC_CommonConvertLswMswWordsToMsbLsbBytes(
                                                         pSignOut + orderSizeInBytes, orderSizeInBytes,
                                                         pSignD, orderSizeInBytes);
        if (err != CC_OK) {
                err = CC_ECDSA_SIGN_INVALID_SIGNATURE_OUT_SIZE_ERROR;
                goto End;
        }

        *pSignOutSize = 2*orderSizeInBytes;


        End:
        if (err != CC_OK) {
                CC_PalMemSetZero(pSignOut, (2*orderSizeInBytes));
        }
#ifdef USE_MBEDTLS_CRYPTOCELL
       mbedtls_md_free(&(pWorkingContext->hash_ctx));
#endif
        /* clear the users context  */
        CC_PalMemSetZero(pSignUserContext, sizeof(CCEcdsaSignUserContext_t));

        return err;

}/* EcdsaSignFinish */



/**************************************************************************
 *                CC_EcdsaSign - integrated function
 **************************************************************************/
/**
   @brief  Performs all of the ECDSA signing operations simultaneously.

This function simply calls the Init, Update and Finish functions continuously.
               This function's prototype is similar to the prototypes of the called functions
               and includes all of their input and output arguments.

   NOTE: Signature lgorithm according ANS X9.62 standard
         Using of HASH functions with HASH size great, than EC modulus size, is not recommended!

   @param[in,out] pRndContext - A pointer to the random generation function context.
   @param[in,out] pSignUserContext - A pointer to the user buffer for signing database.
   @param[in]     pSignerPrivKey   - A pointer to a user private key structure.
   @param[in]     hashMode         - The enumerator variable defines hash function to be used.
   @param[in]     pMessageDataIn   - A message data for calculation of hash.
   @param[in]     messageSizeInBytes  - A size of block of message data in bytes.
   @param[in]     SignOut_ptr      - A pointer to a buffer for output of signature.
   @param[in,out] SignOutSize_ptr  - A pointer to the size of user passed buffer for signature (in)
                                        and size of actual signature (out). The size of buffer
                                        must be not less than 2*OrderSizeInBytes.

   @return <b>CCError_t
**/
CEXPORT_C CCError_t CC_EcdsaSign(
                                     CCRndContext_t              *pRndContext,           /*in/out*/
                                     CCEcdsaSignUserContext_t    *pSignUserContext,      /*in/out*/
                                     CCEcpkiUserPrivKey_t        *pSignerPrivKey,        /*in*/
                                     CCEcpkiHashOpMode_t         hashMode,              /*in*/
                                     uint8_t                         *pMessageDataIn,        /*in*/
                                     size_t                           messageSizeInBytes,    /*in*/
                                     uint8_t                         *pSignOut,              /*out*/
                                     size_t                          *pSignOutSize           /*in*/)
{
        /* FUNCTION DECLARATIONS */

        /* The return error identifier */
        CCError_t err = CC_OK;

        /* FUNCTION LOGIC */
        CHECK_AND_RETURN_ERR_UPON_FIPS_ERROR();

        /*****  EcdsaSignInit  ********/
        err = EcdsaSignInit( pSignUserContext, pSignerPrivKey, hashMode );

        if (err!=CC_OK)
                return err;

        /*****  EcdsaSignUpdate  ********/
        err = EcdsaSignUpdate(pSignUserContext, pMessageDataIn,
                                    messageSizeInBytes);
        if (err!=CC_OK)
                return err;

        /*****  EcdsaSignFinish  ********/
        err = EcdsaSignFinish(pSignUserContext, pRndContext,
                                    pSignOut, pSignOutSize);
        return err;

}/* END OF CC_EcdsaSign */




