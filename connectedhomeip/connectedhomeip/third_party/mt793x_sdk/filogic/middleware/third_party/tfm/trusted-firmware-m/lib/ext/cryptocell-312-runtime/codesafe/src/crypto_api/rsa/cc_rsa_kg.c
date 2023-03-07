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
#include "cc_common.h"
#include "cc_rsa_error.h"
#include "cc_rsa_local.h"
#include "cc_common_math.h"
#include "cc_rnd_error.h"
#include "cc_rsa_kg.h"
#include "rsa.h"
#include "rsa_public.h"
#include "rsa_private.h"
#include "cc_general_defs.h"
#include "cc_fips_defs.h"
#include "cc_pal_mutex.h"
#include "cc_pal_abort.h"
#ifdef CC_SOFT_KEYGEN
#include "ccsw_rsa_kg.h"
#endif
/************************ Defines ******************************/

/************************ Enums ************************************/

/************************ Typedefs *********************************/

/************************ Global Data ******************************/
#ifdef FIPS_CERTIFICATION
extern rsaKgInternalDataStruct_t rsaKgOutParams;
#endif
/************* Private function prototype **************************/

/************************ Public Functions ******************************/

static CCError_t  KGCheckAndSetParamsRSA(
                      CCRndContext_t      *rndContext_ptr,
                                          uint8_t             *PubExp_ptr,
                                          size_t               PubExpSizeInBytes,
                                          size_t               KeySize,
                                          CCRsaUserPrivKey_t   *pCcUserPrivKey,
                                          CCRsaUserPubKey_t    *pCcUserPubKey,
                                          CCRsaKgData_t        *KeyGenData_ptr ){

    /* the error identifier */
        CCError_t Error = CC_OK;
        /* the pointers to the key structures */
        CCRsaPubKey_t  *pCcPubKey;
        CCRsaPrivKey_t *pCcPrivKey;

        CHECK_AND_RETURN_ERR_UPON_FIPS_ERROR();
        /* ...... checking the random context pointer ........................ */
        if (rndContext_ptr == NULL)
                       return CC_RND_CONTEXT_PTR_INVALID_ERROR;

        /* ...... checking the random generate vector function pointer ....... */
        if (rndContext_ptr->rndGenerateVectFunc == NULL)
            return CC_RND_GEN_VECTOR_FUNC_ERROR;

        /* ...... checking the key database handle pointer .................. */
        if (PubExp_ptr == NULL)
            return CC_RSA_INVALID_EXPONENT_POINTER_ERROR;

        /* ...... checking the validity of the exponent pointer ............. */
        if (pCcUserPrivKey == NULL)
            return CC_RSA_INVALID_PRIV_KEY_STRUCT_POINTER_ERROR;

        /* ...... checking the validity of the modulus pointer .............. */
        if (pCcUserPubKey == NULL)
            return CC_RSA_INVALID_PUB_KEY_STRUCT_POINTER_ERROR;

        /* ...... checking the validity of the keygen data .................. */
        if (KeyGenData_ptr == NULL)
            return CC_RSA_KEY_GEN_DATA_STRUCT_POINTER_INVALID;

        /* ...... checking the exponent size .................. */
        if (PubExpSizeInBytes > CC_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_BYTES)
            return CC_RSA_INVALID_EXPONENT_SIZE;

        /* ...... checking the required key size ............................ */
        if (( KeySize < CC_RSA_MIN_VALID_KEY_SIZE_VALUE_IN_BITS ) ||
                ( KeySize > CC_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_BITS ) ||
            ( KeySize % CC_RSA_VALID_KEY_SIZE_MULTIPLE_VALUE_IN_BITS )) {
            return CC_RSA_INVALID_MODULUS_SIZE;
        }

        /* set the public and private key structure pointers */
        pCcPubKey  = (CCRsaPubKey_t*)pCcUserPubKey->PublicKeyDbBuff;
        pCcPrivKey = (CCRsaPrivKey_t*)pCcUserPrivKey->PrivateKeyDbBuff;


        /* ................ clear all input structures ............................. */
        /* ------------------------------------------------------------------------- */

        CC_PalMemSetZero(pCcUserPrivKey, sizeof(CCRsaUserPrivKey_t));
        CC_PalMemSetZero(pCcUserPubKey, sizeof(CCRsaUserPubKey_t));
        CC_PalMemSetZero(KeyGenData_ptr, sizeof(CCRsaKgData_t));

        /* ................ loading the public exponent to the structure .......... */
        /* ------------------------------------------------------------------------- */

        /* loading the buffers to start from LS word to MS word */
        Error = CC_CommonConvertMsbLsbBytesToLswMswWords(
                pCcPubKey->e, sizeof(pCcPubKey->e),
            PubExp_ptr, PubExpSizeInBytes);
        if (Error != CC_OK) {
            Error = CC_RSA_INVALID_EXPONENT_VAL;
            goto End;
        }

        /* .......... initializing the effective counters size in bits .......... */
        pCcPubKey->eSizeInBits = CC_CommonGetWordsCounterEffectiveSizeInBits(pCcPubKey->e, (PubExpSizeInBytes+3)/4);

        /* if the size in bits is 0 - return error */
        if (pCcPubKey->eSizeInBits == 0 || pCcPubKey->eSizeInBits > 17) { /* MV - check if needed*/
            Error = CC_RSA_INVALID_EXPONENT_SIZE;
            goto End;
        }

        /* verifing the exponent has legal value (currently only 0x3,0x11 and 0x10001) */
        /* verifying the exponent has legal value (currently only 0x3,0x11 and 0x10001) */
        if (pCcPubKey->e[0] != CC_RSA_KG_PUB_EXP_ALLOW_VAL_1  &&
                pCcPubKey->e[0] != CC_RSA_KG_PUB_EXP_ALLOW_VAL_2 &&
            pCcPubKey->e[0] != CC_RSA_KG_PUB_EXP_ALLOW_VAL_3) {
            Error = CC_RSA_INVALID_EXPONENT_VAL;
            goto End;
        }

        /* .......... initializing the key sizes  ......................... */

        /* this initialization is required for the low level function (LLF) - indicates the required
         *             size of the key to be found */
        pCcPubKey->nSizeInBits  = KeySize;
        pCcPrivKey->nSizeInBits = KeySize;

 End:
    return Error;
}

/***********************************************************************************************/
#ifndef _INTERNAL_CC_NO_RSA_KG_SUPPORT
/**
   @brief CC_RsaKgKeyPairGenerate generates a Pair of public and private keys on non CRT mode.

   Note: FIPS 186-4 [5.1] standard specifies three choices for the length of the RSA
         keys (modules): 1024, 2048 and 3072 bits and public exponent value >= 0x10001.
         This implementation allows to generate keys also with other (not FIPS approved)
         sizes on the user's responcibility.

   @param [in/out] rndContext_ptr  - Pointer to the RND context buffer.
   @param [in] PubExp_ptr - The pointer to the public exponent (public key).
          Allowed values 0x3, 0x11, 0x10001.
   @param [in] PubExpSizeInBytes - The public exponent size in bytes.
   @param [in] KeySize  - The size of the key in bits. Supported sizes are 256 bit multiples
                          between 512 - 4096;
   @param [out] pCcUserPrivKey - A pointer to the private key structure.
                           This structure is used as input to the CC_RsaPrimDecrypt API.
   @param [out] pCcUserPubKey - A pointer to the public key structure.
                           This structure is used as input to the CC_RsaPrimEncrypt API.
   @param [in] KeyGenData_ptr - a pointer to a structure required for the KeyGen operation.

   @return CCError_t - CC_OK,
                         CC_RSA_INVALID_EXPONENT_POINTER_ERROR,
                         CC_RSA_INVALID_PRIV_KEY_STRUCT_POINTER_ERROR,
                         CC_RSA_INVALID_PUB_KEY_STRUCT_POINTER_ERROR,
                         CC_RSA_KEY_GEN_DATA_STRUCT_POINTER_INVALID,
                         CC_RSA_INVALID_MODULUS_SIZE,
                         CC_RSA_INVALID_EXPONENT_SIZE
*/
CEXPORT_C CCError_t CC_RsaKgKeyPairGenerate(
                                                 CCRndContext_t *rndContext_ptr,
                                                 uint8_t             *PubExp_ptr,
                                                 size_t               PubExpSizeInBytes,
                                                 size_t               KeySize,
                                                 CCRsaUserPrivKey_t *pCcUserPrivKey,
                                                 CCRsaUserPubKey_t  *pCcUserPubKey,
                                                 CCRsaKgData_t      *KeyGenData_ptr,
                         CCRsaKgFipsContext_t    *pFipsCtx)
{
        /* LOCAL INITIALIZATIONS AND DECLERATIONS */

        /* the error identifier */
        CCError_t error = CC_OK;

        /* the pointers to the key structures */
        CCRsaPubKey_t  *pCcPubKey;
        CCRsaPrivKey_t *pCcPrivKey;

        error = KGCheckAndSetParamsRSA( rndContext_ptr,
                    PubExp_ptr,
                    PubExpSizeInBytes,
                    KeySize,
                    pCcUserPrivKey,
                    pCcUserPubKey,
                    KeyGenData_ptr );
        if (error != CC_OK){
            return error;
        }

        /* set the public and private key structure pointers */
        pCcPubKey  = ( CCRsaPubKey_t *)pCcUserPubKey->PublicKeyDbBuff;
        pCcPrivKey = ( CCRsaPrivKey_t *)pCcUserPrivKey->PrivateKeyDbBuff;

        /* .......... initialize the public key on the private structure ............... */
        CC_PalMemCopy(pCcPrivKey->PriveKeyDb.NonCrt.e, pCcPubKey->e, ROUNDUP_BYTES_TO_32BIT_WORD(PubExpSizeInBytes));
        pCcPrivKey->PriveKeyDb.NonCrt.eSizeInBits = pCcPubKey->eSizeInBits;

        /* .......... set the private mode to non CRT .............................. */
        /* ------------------------------------------------------------------------- */

        /* set the mode to non CRT */
        pCcPrivKey->OperationMode = CC_RSA_NoCrt;

        /* ................ executing the key generation ........................... */
        /* ------------------------------------------------------------------------- */
        /* ................ execute the low level key gen .......................... */
        do{
            error = RsaGenPandQ(
                    rndContext_ptr, /*!< [in/out] Pointer to the RND context buffer. */
                KeySize,
                pCcPubKey->eSizeInBits,
                pCcPubKey->e,
                KeyGenData_ptr);
            if (error != CC_OK) {
                goto End;
            }


            /* calculate modulus n and private exponent d */
            error = RsaCalculateNandD(
                            pCcPubKey,
                            pCcPrivKey,
                            KeyGenData_ptr,
                            KeySize/2);

            if (error != CC_RSA_GENERATED_PRIV_KEY_IS_TOO_LOW && error != CC_OK) { \
                goto End;
            }
        }while(error!=CC_OK);

#ifdef FIPS_CERTIFICATION
        CC_CommonReverseMemcpy(rsaKgOutParams.nModulus, (uint8_t*)pCcPubKey->n, KeySize/CC_BITS_IN_BYTE);
        CC_CommonReverseMemcpy(rsaKgOutParams.pPrim, (uint8_t*)KeyGenData_ptr->KGData.p, KeySize/(2*CC_BITS_IN_BYTE));
        CC_CommonReverseMemcpy(rsaKgOutParams.qPrim, (uint8_t*)KeyGenData_ptr->KGData.q, KeySize/(2*CC_BITS_IN_BYTE));
        CC_CommonReverseMemcpy(rsaKgOutParams.dPrivExponent, (uint8_t*)pCcPrivKey->PriveKeyDb.NonCrt.d, KeySize/CC_BITS_IN_BYTE);
#endif



        /* ................ initialize the low level key structures ................ */
        /* ------------------------------------------------------------------------- */

        error = RsaInitPubKeyDb( pCcPubKey );
        if (error != CC_OK) {
            goto End;
        }

        error = RsaInitPrivKeyDb( pCcPrivKey );
        if (error != CC_OK) {
                goto End;
    }

        /* ................ set the key valid tags ................................. */
        /* ------------------------------------------------------------------------- */
        pCcUserPrivKey->valid_tag = CC_RSA_PRIV_KEY_VALIDATION_TAG;
        pCcUserPubKey->valid_tag  = CC_RSA_PUB_KEY_VALIDATION_TAG;

        // run conditional test
        error = FIPS_RSA_VALIDATE(rndContext_ptr,pCcUserPrivKey,pCcUserPubKey,pFipsCtx);


End:
        /* on failure clear the generated key */
        if (error != CC_OK) {
                error = CC_RSA_INTERNAL_ERROR;
            CC_PalMemSetZero((uint8_t*)pCcUserPrivKey,  sizeof(CCRsaUserPrivKey_t) );
            CC_PalMemSetZero((uint8_t*)pCcUserPubKey, sizeof(CCRsaUserPubKey_t) );
    }
    if (pFipsCtx != NULL) {
        CC_PalMemSetZero((uint8_t*)pFipsCtx, sizeof(CCRsaKgFipsContext_t));
    }
        /* clear the KG data structure */
        CC_PalMemSetZero ((uint8_t*)KeyGenData_ptr ,sizeof(CCRsaKgData_t) );

        return error;


}/* END OF CC_RsaKgKeyPairGenerate */


/***********************************************************************************************/
/**
   @brief CC_RsaKgKeyPairCrtGenerate generates a Pair of public and private keys on CRT mode.

   Note: FIPS 186-4 [5.1] standard specifies three choices for the length of the RSA
         keys (modules): 1024, 2048 and 3072 bits. This implementation allows
         generate also some other (not FIPS approved) sizes on the user's responcibility.

   @param [in/out] rndContext_ptr  - Pointer to the RND context buffer.
   @param [in] PubExp_ptr - The pointer to the public exponent (public key)
   @param [in] PubExpSizeInBytes - The public exponent size in bits.
   @param [in] KeySize  - The size of the key in bits. Supported sizes are 256 bit multiples
                          between 512 - 4096;
   @param [out] pCcUserPrivKey - A pointer to the private key structure.
                           This structure is used as input to the CC_RsaPrimDecrypt API.
   @param [out] pCcUserPubKey - A pointer to the public key structure.
                           This structure is used as input to the CC_RsaPrimEncrypt API.
   @param [in] KeyGenData_ptr - a pointer to a structure required for the KeyGen operation.

   @return CCError_t - CC_OK,
                         CC_RSA_INVALID_EXPONENT_POINTER_ERROR,
                         CC_RSA_INVALID_PRIV_KEY_STRUCT_POINTER_ERROR,
                         CC_RSA_INVALID_PUB_KEY_STRUCT_POINTER_ERROR,
                         CC_RSA_KEY_GEN_DATA_STRUCT_POINTER_INVALID,
                         CC_RSA_INVALID_MODULUS_SIZE,
                         CC_RSA_INVALID_EXPONENT_SIZE
*/

CEXPORT_C CCError_t CC_RsaKgKeyPairCrtGenerate(
                                                CCRndContext_t      *rndContext_ptr,
                                                uint8_t                 *PubExp_ptr,
                                                size_t                   PubExpSizeInBytes,
                                                size_t                   KeySize,
                                                CCRsaUserPrivKey_t   *pCcUserPrivKey,
                                                CCRsaUserPubKey_t    *pCcUserPubKey,
                                                CCRsaKgData_t        *KeyGenData_ptr,
                                                CCRsaKgFipsContext_t *pFipsCtx )
{
        /* LOCAL INITIALIZATIONS AND DECLERATIONS */

        /* the error identifier */
        CCError_t Error = CC_OK;

        /* the pointers to the key structures */
        CCRsaPubKey_t  *pCcPubKey;
        CCRsaPrivKey_t *pCcPrivKey;
        uint16_t   pSizeWords;

        Error = KGCheckAndSetParamsRSA( rndContext_ptr,
                    PubExp_ptr,
                    PubExpSizeInBytes,
                    KeySize,
                    pCcUserPrivKey,
                    pCcUserPubKey,
                    KeyGenData_ptr );
        if (Error != CC_OK){
            return Error;
        }
        /* set the public and private key structure pointers */
        pCcPubKey  = (CCRsaPubKey_t*)pCcUserPubKey->PublicKeyDbBuff;
        pCcPrivKey = (CCRsaPrivKey_t*)pCcUserPrivKey->PrivateKeyDbBuff;

        /* .......... initializing the key sizes  ......................... */
        pSizeWords =(uint16_t) KeySize / (2*CC_BITS_IN_32BIT_WORD);  // RL

        /* .......... set the private mode to CRT .................................. */
        /* ------------------------------------------------------------------------- */

        /* set the mode to CRT */
        pCcPrivKey->OperationMode = CC_RSA_Crt;


        /* ................ executing the key generation ........................... */
        /* ------------------------------------------------------------------------- */

        /* ................ execute the low level key gen ........................... */

        /*Generate P and Q*/
        Error = RsaGenPandQ(
                rndContext_ptr,
            KeySize,
            pCcPubKey->eSizeInBits,
            pCcPubKey->e,
            KeyGenData_ptr);

        /* on failure exit the function */
        if (Error != CC_OK) {
            goto End;
        }

        /* Calculate CRT private Key parameters*/
        /* calculate modulus n  */
        Error = RsaCalculateNandD(
                pCcPubKey,
            pCcPrivKey,
            KeyGenData_ptr,
            KeySize/2);
        /* on failure exit the function */
        if (Error != CC_OK) {
            goto End;
        }

        Error = RsaCalculateCrtParams(
                pCcPubKey->e, pCcPubKey->eSizeInBits,
            KeySize,
            KeyGenData_ptr->KGData.p, KeyGenData_ptr->KGData.q,
            pCcPrivKey->PriveKeyDb.Crt.dP,
            pCcPrivKey->PriveKeyDb.Crt.dQ,
            pCcPrivKey->PriveKeyDb.Crt.qInv);

        if (Error !=CC_OK) {
            goto End;
        }

#ifdef FIPS_CERTIFICATION
        CC_CommonReverseMemcpy(rsaKgOutParams.nModulus, (uint8_t*)pCcPubKey->n, KeySize/CC_BITS_IN_BYTE);
        CC_CommonReverseMemcpy(rsaKgOutParams.pPrim, (uint8_t*)KeyGenData_ptr->KGData.p,KeySize/(2*CC_BITS_IN_BYTE));
        CC_CommonReverseMemcpy(rsaKgOutParams.qPrim, (uint8_t*)KeyGenData_ptr->KGData.q, KeySize/(2*CC_BITS_IN_BYTE));
        CC_CommonReverseMemcpy(rsaKgOutParams.dPrivExponent, NULL, KeySize/CC_BITS_IN_BYTE);
#endif

        /* ................ Set private and public key data ........................ */
        /* ------------------------------------------------------------------------- */
        /* Load P,Q vectors */
        CC_PalMemCopy(pCcPrivKey->PriveKeyDb.Crt.P, KeyGenData_ptr->KGData.p, pCcPubKey->nSizeInBits / 16);
        CC_PalMemCopy(pCcPrivKey->PriveKeyDb.Crt.Q, KeyGenData_ptr->KGData.q, pCcPubKey->nSizeInBits / 16);


        /* ................ set the vector sizes ................................... */
        /* ------------------------------------------------------------------------- */

        pCcPrivKey->PriveKeyDb.Crt.PSizeInBits =
        CC_CommonGetWordsCounterEffectiveSizeInBits(pCcPrivKey->PriveKeyDb.Crt.P, (uint16_t)pSizeWords);

        pCcPrivKey->PriveKeyDb.Crt.QSizeInBits =
        CC_CommonGetWordsCounterEffectiveSizeInBits(pCcPrivKey->PriveKeyDb.Crt.Q, (uint16_t)pSizeWords);

        pCcPrivKey->PriveKeyDb.Crt.dPSizeInBits =
        CC_CommonGetWordsCounterEffectiveSizeInBits(pCcPrivKey->PriveKeyDb.Crt.dP, (uint16_t)pSizeWords);

        pCcPrivKey->PriveKeyDb.Crt.dQSizeInBits =
        CC_CommonGetWordsCounterEffectiveSizeInBits(pCcPrivKey->PriveKeyDb.Crt.dQ, (uint16_t)pSizeWords);

        pCcPrivKey->PriveKeyDb.Crt.qInvSizeInBits =
        CC_CommonGetWordsCounterEffectiveSizeInBits(pCcPrivKey->PriveKeyDb.Crt.qInv, (uint16_t)pSizeWords);

        /* ................ initialize the low level key structures ................ */
        /* ------------------------------------------------------------------------- */

        Error = RsaInitPubKeyDb(pCcPubKey);
        if (Error != CC_OK) {
                goto End;
        }

        Error = RsaInitPrivKeyDb(pCcPrivKey);
        if (Error != CC_OK) {
                goto End;
        }

        pCcUserPrivKey->valid_tag = CC_RSA_PRIV_KEY_VALIDATION_TAG;
        pCcUserPubKey->valid_tag  = CC_RSA_PUB_KEY_VALIDATION_TAG;

        /* run conditional test */
        Error = FIPS_RSA_VALIDATE(rndContext_ptr,pCcUserPrivKey,pCcUserPubKey,pFipsCtx);

End:

        /* on failure clear the generated key */
        if (Error != CC_OK) {
                Error = CC_RSA_INTERNAL_ERROR;
        CC_PalMemSetZero(pCcUserPrivKey,  sizeof(CCRsaUserPrivKey_t) );
        CC_PalMemSetZero(pCcUserPubKey, sizeof(CCRsaUserPubKey_t) );
    }
    if (pFipsCtx != NULL) {
        CC_PalMemSetZero(pFipsCtx, sizeof(CCRsaKgFipsContext_t));
    }
        /* clear the KG data structure */
        CC_PalMemSetZero (KeyGenData_ptr ,sizeof(CCRsaKgData_t) );

        return Error;


}/* END OF CC_RsaKgKeyPairCrtGenerate */



/**********************************************************************************************************/
/**
 * @brief The CC_RsaGenerateVectorInRangeX931 function generates a random vector in range:
 *            MinVect < RandVect < MaxVect, where:
 *            MinVect = squareRoot(2) * 2^(RndSizeInBits-1),  MaxVect = 2^RndSizeInBits.
 *
 *            Note: 1. MSBit of RandVect must be set to 1.
 *                  2. Words order of output vector is set from LS word to MS
 *                 word.
 *
 *        This function is used in PKI RSA for random generation according to ANS X9.31 standard.
 *        If PKI_RSA is not supported, the function does nothing.
 *
 *        Functions algorithm::
 *
 *        1.  Calls the CC_RndGenerateVector() function for generating random vector
 *            RndVect of size RndSizeInWords, rounded up to bytes. Set index i
 *            to high word i = SizeInWords-1.
 *        2.  Check and adust candidate for msWord inside the random vector
 *            starting from msWord himselv, if msWord > high word of MinVect,
 *            goto step 3, else try next word i--; if no words to try, then goto
 *            step 1.
 *        3.  Set the found msWord to high position in array and generate new
 *            random words instead all checked and rejected words.
 *
 * @param[in/out] rndContext_ptr  - Pointer to the RND context buffer.
 * @rndSizeWords[in]  - The size of random vectore that is required.
 * @rnd_ptr[out]      - The output buffer of size not less, than rndSizeWords.
 *
 * @return CCError_t - On success CC_OK is returned, on failure a
 *                       value MODULE_* as defined in ...
 */
CCError_t CC_RsaGenerateVectorInRangeX931(
                                              CCRndContext_t *rndContext_ptr,
                                              uint32_t   rndSizeWords,
                                              uint32_t  *rnd_ptr)
{
        /* MIN_WORD = rounded up MS word of (2^(32*rndSizeWords-1))*sqwRoot(2)*/
#define  MIN_VAL 0xB504F334  /* RL minimal value of MS word of rnd vect. */

        /* FUNCTION DECLARATIONS */

        CCError_t error = CC_OK;
        uint32_t  msWord;
        int32_t   i;
        CCBool_t isFound = CC_FALSE;
        void   *rndState_ptr;
        CCRndGenerateVectWorkFunc_t RndGenerateVectFunc;

        /* FUNCTION LOGIC */

        /* check parameters */
        if (rndContext_ptr == NULL)
                return CC_RND_CONTEXT_PTR_INVALID_ERROR;

        rndState_ptr = rndContext_ptr->rndState;
        RndGenerateVectFunc = rndContext_ptr->rndGenerateVectFunc;

        if (RndGenerateVectFunc == NULL)
                return CC_RND_GEN_VECTOR_FUNC_ERROR;

        /* .........         Rnd generation       .............. */
        /* ----------------------------------------------------- */

        while (1) {
                /* Genrate random prime candidate, considered as 32-bit words */
                error = RndGenerateVectFunc((void*)rndState_ptr,
                                            (unsigned char *)rnd_ptr,
                                            (size_t)rndSizeWords*sizeof(uint32_t));
                if (error)
                        goto End;

                /* Find and adust candidate for msWord inside the random *
                *  vector starting from msWord itself             */

                for (i = rndSizeWords-1; i >= 0; i--) {
                        /* Set MSBit to 1 */
                        msWord = rnd_ptr[i] | 0x80000000;

                        if (msWord > MIN_VAL) {
                                rnd_ptr[rndSizeWords-1] = msWord;
                                isFound = 1;
                        }

                        /* Generate new random words instead the checked yet  *
                        *  (for sequrity goals)                   */
                        if ((isFound == 1) && (i < (int32_t)rndSizeWords - 1)) {
                                error = RndGenerateVectFunc((void*)rndState_ptr,
                                                            (unsigned char *)&rnd_ptr[i],
                                                            (size_t)(rndSizeWords - 1 - i)*sizeof(uint32_t));
                                if (error)
                                        goto End;
                        }

                        if (isFound == 1)
                                break;
                }

                if (isFound) {
                        rnd_ptr[0] |= 1; /* ensure odd result */
                        break;
                }
        }

        End:
        return error;

} /* End of CC_RsaGenerateVectorInRangeX931 */

#endif /*_INTERNAL_CC_NO_RSA_KG_SUPPORT*/
#endif /* !defined(CC_IOT) || ( defined(CC_IOT) && defined(MBEDTLS_RSA_C)) */
