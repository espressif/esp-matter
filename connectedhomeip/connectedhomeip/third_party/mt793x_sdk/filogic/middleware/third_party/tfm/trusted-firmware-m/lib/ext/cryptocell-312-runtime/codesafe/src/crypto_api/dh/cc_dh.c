/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/************* Include Files ****************/

#include "cc_pal_mem.h"
#include "cc_common.h"
#include "cc_common_math.h"
#include "cc_rnd_common.h"
#include "cc_rnd_error.h"
#include "cc_rnd_local.h"
#include "cc_dh_error.h"
#include "cc_dh.h"
#include "cc_dh_kg.h"
#include "cc_rsa_build.h"
#include "cc_rsa_prim.h"
#include "rsa.h"
#include "rsa_public.h"
#include "rsa_private.h"
#include "cc_fips_defs.h"


/************************ Defines *******************************/

/************************ Enums *********************************/

/************************ macros ********************************/


/************************ global data ***********************************/

/************************ Private Functions ******************************/
/* This function translates the DH hash modes into KDF hash modes */
static CCKdfHashOpMode_t MakeKDFHashMode(CCDhHashOpMode_t hashMode)
{

        CCKdfHashOpMode_t outMode;

        switch (hashMode) {
        case CC_DH_HASH_SHA1_mode:
                outMode = CC_KDF_HASH_SHA1_mode;
                break;
        case CC_DH_HASH_SHA224_mode:
                outMode = CC_KDF_HASH_SHA224_mode;
                break;
        case CC_DH_HASH_SHA256_mode:
                outMode = CC_KDF_HASH_SHA256_mode;
                break;
        case CC_DH_HASH_SHA384_mode:
                outMode = CC_KDF_HASH_SHA384_mode;
                break;
        case CC_DH_HASH_SHA512_mode:
                outMode = CC_KDF_HASH_SHA512_mode;
                break;
        default:
                outMode = CC_KDF_HASH_OpModeLast;
        }

        return outMode;
}

/* This function translates the DH deriveFunc enum to KDF derive func enum */
static CCKdfDerivFuncMode_t MakeKDFDeriveFuncMode(CCDhDerivationFuncMode_t deriveFunc)
{

        CCKdfDerivFuncMode_t outDeriveFunc;

        switch (deriveFunc) {
        case CC_DH_ASN1_Der_mode:
                outDeriveFunc = CC_KDF_ASN1_DerivMode;
                break;
        case CC_DH_X963_DerMode:
                outDeriveFunc = CC_KDF_ConcatDerivMode;
                break;
        default:
                outDeriveFunc = CC_KDF_DerivFuncModeLast;
        }

        return outDeriveFunc;
}
/************************ Public Functions ******************************/


CEXPORT_C CCError_t CC_DhGeneratePubPrv(
                                           CCRndContext_t *rndContext_ptr,
                                           uint8_t *Generator_ptr,              /*generator*/
                                           size_t GeneratorSize,
                                           uint8_t *Prime_ptr,                  /*modulus*/
                                           size_t PrimeSize,
                                           uint16_t L,         /*Exact length of Private key in bits*/
                                           uint8_t *Q_ptr,                      /*order/in*/
                                           size_t QSize,
                                           CCDhOpMode_t DH_mode,            /*in*/
                                           CCDhUserPubKey_t *tmpPubKey_ptr,  /*temp buff*/
                                           CCDhPrimeData_t  *tmpPrimeData_ptr, /*temp buff*/
                                           uint8_t *ClientPrvKey_ptr,           /*out*/
                                           size_t  *ClientPrvKeySize_ptr,      /*in/out*/
                                           uint8_t *ClientPub1_ptr,             /*out*/
                                           size_t  *ClientPubSize_ptr)         /*in/out*/

{
        /* FUNCTION DECLARATIONS */

        /* The return error identifier */
        CCError_t Error = CC_OK;

        /* temporary byte shift masks */
        uint8_t tmpByte, mask, mask1, shift;

        /* the vector 2^(L-1) size*/
        uint16_t tmpSize;

        /* the comparing value, returned from the vectors compare */
        CCCommonCmpCounter_t comp;

        CCRndState_t   *rndState_ptr;
        CCRndGenerateVectWorkFunc_t RndGenerateVectFunc;

    CHECK_AND_RETURN_ERR_UPON_FIPS_ERROR();


        /* ............... checking the parameters validity ................... */
        /* -------------------------------------------------------------------- */

        /* check parameters */
        if (rndContext_ptr == NULL)
                return CC_RND_CONTEXT_PTR_INVALID_ERROR;
        if (rndContext_ptr->rndGenerateVectFunc == NULL)
                return CC_RND_GEN_VECTOR_FUNC_ERROR;

        rndState_ptr = (CCRndState_t *)(rndContext_ptr->rndState);
        RndGenerateVectFunc = rndContext_ptr->rndGenerateVectFunc;

        /* if an argument pointer is NULL return an error */
        if (Generator_ptr == NULL || Prime_ptr == NULL ||
            ClientPrvKey_ptr == NULL || ClientPub1_ptr == NULL ||
            ClientPrvKeySize_ptr == NULL || ClientPubSize_ptr == NULL ||
            tmpPubKey_ptr == NULL || tmpPrimeData_ptr == NULL)
                return CC_DH_INVALID_ARGUMENT_POINTER_ERROR;

        /* check DH mode */
        if (DH_mode > CC_DH_NumOfModes)
                return CC_DH_INVALID_ARGUMENT_OPERATION_MODE_ERROR;

        /* preliminary check of sizes */
        if (PrimeSize > CC_DH_MAX_MOD_SIZE_IN_BYTES || PrimeSize == 0)
                return CC_DH_INVALID_MODULUS_SIZE_ERROR;

        if (GeneratorSize == 0 || GeneratorSize > PrimeSize)
                return CC_DH_INVALID_ARGUMENT_SIZE_ERROR;

        if (*ClientPubSize_ptr < PrimeSize)
                return CC_DH_INVALID_PUBLIC_KEY_SIZE_ERROR;

        if (*ClientPrvKeySize_ptr < PrimeSize)
                return CC_DH_SECRET_KEY_SIZE_OUTPUT_ERROR;


        /* Check the the generator according to DH mode */
    if (DH_mode == CC_DH_PKCS3_mode)
            tmpByte = 0; /* for checking PKCS3 part 6.  0 < g < p */
    else  /* CC_DH_ANSI_X942_mode */
        tmpByte = 1; /* for checking ANSI_X942 part 7.2.  1 < g < p-1 */

        comp = CC_CommonCmpMsbUnsignedCounters( Generator_ptr,
                                                   GeneratorSize,
                                                   &tmpByte, 1);

        if (comp != CC_COMMON_CmpCounter1GreaterThenCounter2)
                return CC_DH_ARGUMENT_GENERATOR_SMALLER_THAN_ZERO_ERROR;

        /*Compare the generator and the Prime: requested that g < P-1 */
        Prime_ptr[PrimeSize-1] -= tmpByte; /* temporary p = p-1 */
        comp = CC_CommonCmpMsbUnsignedCounters( Generator_ptr, GeneratorSize,
                                                   Prime_ptr, PrimeSize );

        if (comp != CC_COMMON_CmpCounter2GreaterThenCounter1)
                return CC_DH_ARGUMENT_PRIME_SMALLER_THAN_GENERATOR_ERROR;

        /* repair P */
        Prime_ptr[PrimeSize-1] += tmpByte;

        /*--------------------------------------------------------------------------------*/
        /*         DH public-private keys generation                                      */
        /*--------------------------------------------------------------------------------*/

        /* temporary set prime modulus into temp buffer in little endianness */
        // RL Endianness
        CC_CommonReverseMemcpy( (uint8_t*)tmpPrimeData_ptr->DataIn , Prime_ptr , PrimeSize );

        /* get actual size of prime in bits: min() used to prevent warnings */
        tmpSize = CC_MIN( PrimeSize*8,
                       (uint16_t)CC_CommonGetBytesCounterEffectiveSizeInBits(
                                           (uint8_t*)tmpPrimeData_ptr->DataIn, PrimeSize) );

        /* correction of Prime_ptr pointer and Size for removing of not significant zero-bytes */
        if (PrimeSize - CALC_FULL_BYTES(tmpSize) > 0) {
                Prime_ptr += PrimeSize - CALC_FULL_BYTES(tmpSize);
                PrimeSize = CALC_FULL_BYTES(tmpSize);
        }

        switch (DH_mode) {
        case CC_DH_PKCS3_mode:
                /* ----------------------------------------------------------- *
            PKCS#3:  set x private random value according to following:
                                   1) If L = 0: set    0 < x < P-1;
                                   2) If L > 0: set  2^(L-1) <= x < 2^L ,
                                           where 2^(L-1) <= P.
                  ----------------------------------------------------------- */
                if (L == 0) {
                        /* Option 1: L is not provided - check the minimum size of the private key buffer */
                        if (*ClientPrvKeySize_ptr < PrimeSize) {
                                Error = CC_DH_ARGUMENT_PRV_SIZE_ERROR;
                                goto End1;
                        }

                        /* random generation in range:  0 < x < P-1  (in little endian */
                        Error = CC_RndGenerateVectorInRange( rndContext_ptr,
                                                                tmpSize /*rndSizeInBits*/,
                                                                (uint8_t*)tmpPrimeData_ptr->DataIn/*maxVect*/,
                                                                (uint8_t*)tmpPrimeData_ptr->DataOut/*out*/ );
                        if (Error != CC_OK)
                                goto End;

                        /* reverse privKey to big endianness */
                        CC_CommonReverseMemcpy( ClientPrvKey_ptr , (uint8_t*)tmpPrimeData_ptr->DataOut , PrimeSize );

                        /* private key size in bytes */
                        *ClientPrvKeySize_ptr = PrimeSize;
                }
                else {  /* Option 2:  L > 0 and bit length of privKey must be exactly L bit */
                        /* check L and the minimum size of the private key buffer */
                        if (L > tmpSize) {
                                Error = CC_DH_INVALID_L_ARGUMENT_ERROR;
                                goto End1;
                        }
                        if ((*ClientPrvKeySize_ptr)*8 < L) {
                                Error = CC_DH_ARGUMENT_PRV_SIZE_ERROR;
                                goto End1;
                        }

                        /* actual private key size in bytes and shift value */
                        *ClientPrvKeySize_ptr = CALC_FULL_BYTES(L);
                        if (*ClientPrvKeySize_ptr > CC_DH_MAX_MOD_SIZE_IN_BYTES) {
                                Error = CC_DH_ARGUMENT_PRV_SIZE_ERROR;
                                goto End1;
            }
                        shift = ((8 - (L & 7)) & 7);

                        /* if L = modulus size, then generate random x with exact bit-size = L
                           and value in range:  2^(L-1) < x < P */
                        if (L == tmpSize) {
                                mask   = 0x7F >> shift;
                                mask1  = 0x80 >> shift;

                                /* set temporary MSBit of modulus = 0 for generation random in range without MSbit */
                                ((uint8_t*)tmpPrimeData_ptr->DataIn)[*ClientPrvKeySize_ptr - 1] &= mask;

                                /* generate random in range */
                                Error = CC_RndGenerateVectorInRange(rndContext_ptr,
                                                                      tmpSize /*rndSizeInBits*/,
                                                                      (uint8_t*)tmpPrimeData_ptr->DataIn/*maxVect*/,
                                                                      (uint8_t*)tmpPrimeData_ptr->DataOut/*out*/ );

                                if (Error != CC_OK)
                                        goto End;

                                /* set MSBit of random to 1 */
                                ((uint8_t*)tmpPrimeData_ptr->DataIn)[*ClientPrvKeySize_ptr - 1] |= mask1;

                                /* reverse privKey to big endianness */
                                CC_CommonReverseMemcpy( ClientPrvKey_ptr , (uint8_t*)tmpPrimeData_ptr->DataOut , *ClientPrvKeySize_ptr );
                        }
                        /* if L < modulus size, then generate random x of size L bits */
                        else {
                                /* random generation */
                                Error = RndGenerateVectFunc((void *)rndState_ptr, (unsigned char *)ClientPrvKey_ptr, *ClientPrvKeySize_ptr );

                                if (Error != CC_OK)
                                        goto End;

                                /* set two appropriate high bits of privKey to 00..1 to met the requirement 2^(L-1) <= x < 2^L */
                                if ((L & 7) > 0) {
                                        mask  = 0xFF >> shift;
                                        mask1 = 0x80 >> shift;
                                        ClientPrvKey_ptr[0] = (ClientPrvKey_ptr[0] & mask) | mask1;
                                }
                                /* if( (L & 7) == 0 ) */
                                else {
                                        ClientPrvKey_ptr[0] |= 0x80;
                                }
                        }
                }

                break;

        case CC_DH_ANSI_X942_mode:
                /* ----------------------------------------------------------- *
                                              ANS X9.42:
                                 1<= X <= q-1 or    1< X <= q-1
                 --------------------------------------------------------------*/

                /* check order */
                if (Q_ptr == NULL) {
                        Error = CC_DH_INVALID_ARGUMENT_POINTER_ERROR;
                        goto End1;
                }

                if (QSize == 0 || QSize > PrimeSize) {
                        Error = CC_DH_INVALID_ORDER_SIZE_ERROR;
                        goto End1;
                }

                /* check client private key buffer size */
                if (*ClientPrvKeySize_ptr < QSize) {
                        Error = CC_DH_ARGUMENT_PRV_SIZE_ERROR;
                        goto End1;
                }

                /* set order Q into temp buffer in little endianness */
                CC_CommonReverseMemcpy( (uint8_t*)tmpPrimeData_ptr->DataIn, Q_ptr, QSize );

                /* get actual size in bits */
                tmpSize = (uint16_t)CC_CommonGetBytesCounterEffectiveSizeInBits(
                                                                                    (uint8_t*)tmpPrimeData_ptr->DataIn,
                                                                                    QSize );
                /* private key size in bytes */
                *ClientPrvKeySize_ptr = CALC_FULL_BYTES(tmpSize);

                /* random in range:  1 < x < Q  (little endianness) */
                Error = CC_RndGenerateVectorInRange( rndContext_ptr,
                                                        tmpSize /*rndSizeInBits*/,
                                                        (uint8_t*)tmpPrimeData_ptr->DataIn/*maxVect*/,
                                                        (uint8_t*)tmpPrimeData_ptr->DataOut/*out*/);
                if (Error != CC_OK)
                        goto End;

                /* reverse privKey to big endianness */
                CC_CommonReverseMemcpy(ClientPrvKey_ptr, (uint8_t*)tmpPrimeData_ptr->DataOut, *ClientPrvKeySize_ptr);

                break;

        default:
                Error = CC_DH_INVALID_ARGUMENT_OPERATION_MODE_ERROR;
                goto End1;

        }

        /* ----------------------------------------------------------- */
        /*           Create the public key                             */
        /* ----------------------------------------------------------- */

        /* Build the RSA PublKey data structure for the Exp operation, using RSA_Encrypt primitive */
        Error = CC_RsaPubKeyBuild(
                                     tmpPubKey_ptr,
                                     ClientPrvKey_ptr,
                                     *ClientPrvKeySize_ptr,
                                     Prime_ptr,
                                     PrimeSize );
        /* check error */
        if (Error != CC_OK) {
                goto End;
        }

        /*Call the exponent operation to calculate the ClientPub1 = Generator^privKey mod Prime */
        Error = CC_RsaPrimEncrypt(
                                     tmpPubKey_ptr,
                                     tmpPrimeData_ptr,
                                     Generator_ptr,
                                     GeneratorSize,
                                     ClientPub1_ptr );
        if (Error != CC_OK) {
                goto End;
        }

        *ClientPubSize_ptr = PrimeSize;

End:

    if (Error != CC_OK) {
        CC_PalMemSetZero( ClientPrvKey_ptr, *ClientPrvKeySize_ptr);
        *ClientPrvKeySize_ptr = 0;
    }
End1:
        /* delete secure sensitive data */
        CC_PalMemSetZero( tmpPubKey_ptr, sizeof(CCDhUserPubKey_t) );
        CC_PalMemSetZero( tmpPrimeData_ptr, sizeof(CCDhPrimeData_t) );

        return Error;

}/* END OF CC_DhGeneratePubPrv function */


CEXPORT_C CCError_t CC_DhGetSecretKey(
                                          uint8_t *ClientPrvKey_ptr,
                                          size_t ClientPrvKeySize,
                                          uint8_t *ServerPubKey_ptr,
                                          size_t ServerPubKeySize,
                                          uint8_t *Prime_ptr,
                                          size_t PrimeSize,
                                          CCDhUserPubKey_t *tmpPubKey_ptr,
                                          CCDhPrimeData_t  *tmpPrimeData_ptr,
                                          uint8_t *SecretKey_ptr,
                                          size_t *SecretKeySize_ptr)
{
        /* FUNCTION DECLARATIONS */

        /* The return error identifier */
        CCError_t Error = CC_OK;

        CCCommonCmpCounter_t cmpResult;
        uint8_t one = 1;

    CHECK_AND_RETURN_ERR_UPON_FIPS_ERROR();


        /* ............... checking the parameters validity ................... */
        /* -------------------------------------------------------------------- */

        /* if an argument pointer is NULL return an error */
        if (ClientPrvKey_ptr == NULL || ServerPubKey_ptr == NULL ||
            Prime_ptr == NULL || tmpPubKey_ptr == NULL ||
            tmpPrimeData_ptr == NULL || SecretKey_ptr == NULL || SecretKeySize_ptr == NULL)

                return CC_DH_INVALID_ARGUMENT_POINTER_ERROR;

        /*If an argument buffer size is zero return an error*/
        if (PrimeSize == 0 || PrimeSize > CC_DH_MAX_MOD_SIZE_IN_BYTES ||
            ClientPrvKeySize == 0 || ClientPrvKeySize > PrimeSize ||
            ServerPubKeySize == 0 || ServerPubKeySize > PrimeSize ||
            *SecretKeySize_ptr == 0 || *SecretKeySize_ptr < PrimeSize)
                return CC_DH_INVALID_ARGUMENT_SIZE_ERROR;

        /* 1. verifying that the private exponent is less than modulus, else subtract the modulus */
        cmpResult = CC_CommonCmpMsbUnsignedCounters( ClientPrvKey_ptr, ClientPrvKeySize,
                                                        Prime_ptr, PrimeSize );

        if (cmpResult != CC_COMMON_CmpCounter2GreaterThenCounter1) {
                /* subtract modulus prime from private key and set result in temp buffer */
                CC_CommonSubtractMSBUint8Arrays(ClientPrvKey_ptr, ClientPrvKeySize, Prime_ptr, PrimeSize,
                                                   (uint8_t*)tmpPrimeData_ptr->DataIn);

                /* build the Data for the Exp operation.
                  Note: the user private key is set into public key structure */
                Error = CC_RsaPubKeyBuild(
                                             tmpPubKey_ptr,
                                             (uint8_t*)tmpPrimeData_ptr->DataIn,
                                             ClientPrvKeySize,
                                             Prime_ptr,
                                             PrimeSize);
        } else {
                /* build the Data for the Exp operation */
                Error = CC_RsaPubKeyBuild(
                                             tmpPubKey_ptr,
                                             ClientPrvKey_ptr,
                                             ClientPrvKeySize,
                                             Prime_ptr,
                                             PrimeSize);
        }

        if (Error != CC_OK)
                goto End;

        /* 3. create: Secret_key (or shared secret value) = Server_public_key *
        *  ^ Prv mod Prime                                                    */
        Error = CC_RsaPrimEncrypt(
                                     tmpPubKey_ptr, /* Note: this is the private key */
                                     tmpPrimeData_ptr,
                                     ServerPubKey_ptr,
                                     ServerPubKeySize,
                                     SecretKey_ptr);

        if (Error != CC_OK)
                goto End;

        /* Secret key (shared secret value) size in bytes, including leading  *
        *  zeroes                                                             */
        *SecretKeySize_ptr = PrimeSize;

        /* Note: X9.42 7.5.1 requires that shared secret value != 1 */
        cmpResult = CC_CommonCmpMsbUnsignedCounters(
                                                &one, 1/*size*/,
                                                SecretKey_ptr, PrimeSize);

        if (cmpResult != CC_COMMON_CmpCounter2GreaterThenCounter1) {
                Error = CC_DH_ARGUMENT_BUFFER_SIZE_ERROR;
                goto End;
        }

End:
        /* delete secure sensitive data */
        CC_PalMemSetZero(tmpPubKey_ptr, sizeof(CCDhUserPubKey_t));
        CC_PalMemSetZero(tmpPrimeData_ptr, sizeof(CCDhPrimeData_t));

        return Error;
}  /* END of CC_DhGetSecretKey function */



CEXPORT_C CCError_t CC_DhX942GetSecretData(
                                                uint8_t                  *ClientPrvKey_ptr,
                                                size_t                  ClientPrvKeySize,
                                                uint8_t                  *ServerPubKey_ptr,
                                                size_t                  ServerPubKeySize,
                                                uint8_t                  *Prime_ptr,
                                                size_t                  PrimeSize,
                                                CCDhOtherInfo_t      *otherInfo_ptr,
                                                CCDhHashOpMode_t     hashMode,
                                                CCDhDerivationFuncMode_t DerivFunc_mode,
                                                CCDhTemp_t           *tmpBuff_ptr,
                                                uint8_t                  *SecretKeyingData_ptr,
                                                size_t                   SecretKeyingDataSize )
{
        /* FUNCTION DECLARATIONS */

        /* The return error identifier */
        CCError_t Error = CC_OK;

        size_t  SecretKeySize = PrimeSize;

    CHECK_AND_RETURN_ERR_UPON_FIPS_ERROR();

        /* check pointers */
        if (tmpBuff_ptr == NULL || SecretKeyingData_ptr == NULL)
                return CC_DH_INVALID_ARGUMENT_POINTER_ERROR;

        /*check that the size of derived secret key is not NULL  */
        if (SecretKeyingDataSize == 0)
                return CC_DH_SECRET_KEYING_DATA_SIZE_ILLEGAL_ERROR;

        /*check that the keying data size is not too large  */
        if (SecretKeyingDataSize > CC_DH_MAX_SIZE_OF_KEYING_DATA)
                return CC_DH_SECRET_KEYING_DATA_SIZE_ILLEGAL_ERROR;

        /*Call the PKCS#3 get secret key function*/
        Error = CC_DhGetSecretKey(
                                    ClientPrvKey_ptr,
                                    ClientPrvKeySize,
                                    ServerPubKey_ptr,
                                    ServerPubKeySize,
                                    Prime_ptr,
                                    PrimeSize,
                                    &tmpBuff_ptr->UserPubKey,
                                    &tmpBuff_ptr->PrimeData,
                                    (uint8_t*)tmpBuff_ptr->TempBuff,
                                    &SecretKeySize);

        if (Error != CC_OK)
                goto ExitOnError;


        /*Let the keydataSize from the previous function determine the key data length in the next function*/
        Error = CC_KdfKeyDerivFunc(
                                     (uint8_t*)tmpBuff_ptr->TempBuff,
                                     SecretKeySize,
                                     otherInfo_ptr,
                                     MakeKDFHashMode(hashMode),
                                     MakeKDFDeriveFuncMode(DerivFunc_mode),
                                     SecretKeyingData_ptr,
                                     SecretKeyingDataSize);

ExitOnError:

        CC_PalMemSetZero(tmpBuff_ptr, sizeof(CCDhTemp_t));


        return Error;

}/* END OF _DX_DH_X942_GetSecretData */


CEXPORT_C CCError_t CC_DhX942HybridGetSecretData(
                                                      uint8_t            *ClientPrvKey_ptr1,
                                                      size_t             ClientPrvKeySize1,
                                                      uint8_t            *ClientPrvKey_ptr2,
                                                      size_t             ClientPrvKeySize2,
                                                      uint8_t            *ServerPubKey_ptr1,
                                                      size_t             ServerPubKeySize1,
                                                      uint8_t            *ServerPubKey_ptr2,
                                                      size_t             ServerPubKeySize2,
                                                      uint8_t            *Prime_ptr,
                                                      size_t             PrimeSize,
                                                      CCDhOtherInfo_t  *otherInfo_ptr,
                                                      CCDhHashOpMode_t hashMode,
                                                      CCDhDerivationFuncMode_t DerivFunc_mode,
                                                      CCDhHybrTemp_t   *tmpDhHybr_ptr,
                                                      uint8_t            *SecretKeyingData_ptr,
                                                      size_t             SecretKeyingDataSize)
{
        /* FUNCTION DECLARATIONS */

        /* The return error identifier */
        CCError_t Error = CC_OK;

        /*The assignment to Prime size is according to the real size of the buffer SecretKeyData_ptr*/
        size_t SecretKeyDataSize1 = PrimeSize;
        size_t SecretKeyDataSize2 = PrimeSize;

    CHECK_AND_RETURN_ERR_UPON_FIPS_ERROR();

        /* check pointers */
        if (tmpDhHybr_ptr == NULL || SecretKeyingData_ptr == NULL)
                return CC_DH_INVALID_ARGUMENT_POINTER_ERROR;

        /*check that the size of derived secret key is not NULL  */
        if (SecretKeyingDataSize == 0)
                return CC_DH_SECRET_KEYING_DATA_SIZE_ILLEGAL_ERROR;

        /*check that the keying data size is not too large  */
        if (SecretKeyingDataSize > CC_DH_MAX_SIZE_OF_KEYING_DATA)
                return CC_DH_SECRET_KEYING_DATA_SIZE_ILLEGAL_ERROR;

        /* Note: other input parameters will be shecked in called functions */

        /* get shared secret key (value) 1 */
        Error = CC_DhGetSecretKey(
                                    ClientPrvKey_ptr1,
                                    ClientPrvKeySize1,
                                    ServerPubKey_ptr1,
                                    ServerPubKeySize1,
                                    Prime_ptr,
                                    PrimeSize,
                                    &tmpDhHybr_ptr->UserPubKey,
                                    &tmpDhHybr_ptr->PrimeData,
                                    (uint8_t*)&tmpDhHybr_ptr->TempBuff,
                                    &SecretKeyDataSize1);
        if (Error != CC_OK)
                goto End;

        /* get shared secret key (value) 2 */
        Error = CC_DhGetSecretKey(
                                    ClientPrvKey_ptr2,
                                    ClientPrvKeySize2,
                                    ServerPubKey_ptr2,
                                    ServerPubKeySize2,
                                    Prime_ptr,
                                    PrimeSize,
                                    &tmpDhHybr_ptr->UserPubKey,
                                    &tmpDhHybr_ptr->PrimeData,
                                    (uint8_t*)&tmpDhHybr_ptr->TempBuff+SecretKeyDataSize1,
                                    &SecretKeyDataSize2);
        if (Error != CC_OK)
                goto End;

        /* Derive the secret key according to the secret key size and value   *
        *  key1||key2                                 */


        Error = CC_KdfKeyDerivFunc(
                                     (uint8_t*)&tmpDhHybr_ptr->TempBuff,
                                     (uint16_t)(SecretKeyDataSize1 + SecretKeyDataSize2),
                                     otherInfo_ptr,
                                     MakeKDFHashMode(hashMode),
                                     MakeKDFDeriveFuncMode(DerivFunc_mode),
                                     SecretKeyingData_ptr,
                                     SecretKeyingDataSize );

End:
        CC_PalMemSetZero(tmpDhHybr_ptr, sizeof(CCDhHybrTemp_t));

        return Error;

}/* END OF CC_DhX942HybridGetSecretData */


CEXPORT_C CCError_t CC_DhCheckPubKey(
                                         uint8_t              *modP_ptr,             /*in */
                                         size_t               modPsizeBytes,        /*in */
                                         uint8_t              *orderQ_ptr,           /*in */
                                         size_t               orderQsizeBytes,      /*in */
                                         uint8_t              *pubKey_ptr,           /*in */
                                         size_t               pubKeySizeBytes,      /*in */
                                         CCDhTemp_t       *tempBuff_ptr          /*in */)
{

        /* FUNCTION DECLARATIONS */

        /* The return error identifier */
        CCError_t Error = CC_OK;

        /* size in bits of modulus P and order Q and public key */
        uint32_t  modPsizeBits;
        uint32_t  orderQsizeBits;
        uint32_t  pubKeySizeBits;

        /* comparing result */
        int  cmpRes;
        CCCommonCmpCounter_t cmpCounters;
        CCDhPubKey_t  *tmpPubKey_ptr;

    CHECK_AND_RETURN_ERR_UPON_FIPS_ERROR();

        /*------------------------------- */
        /* Step 1. Check input pointers   */
        /*------------------------------- */

        /* check pointers: modP, generator and tempBuff. Note: other pointers may be NULL  */
        if (modP_ptr == NULL ||
            orderQ_ptr == NULL ||
            pubKey_ptr == NULL ||
            tempBuff_ptr == NULL) {
                return CC_DH_INVALID_ARGUMENT_POINTER_ERROR;
        }
        /* temp public key buffer */
        tmpPubKey_ptr = (CCDhPubKey_t*)((void*)&tempBuff_ptr->UserPubKey.PublicKeyDbBuff);

        /*----------------------------------------------------------- */
        /* Step 2. Calculate and check the sizes of modulus and order */
        /*----------------------------------------------------------- */
        /* preliminary check */
        if (modPsizeBytes > CC_DH_MAX_VALID_KEY_SIZE_VALUE_IN_BITS / 8)
                return CC_DH_INVALID_MODULUS_SIZE_ERROR;

        if (orderQsizeBytes > modPsizeBytes)
                return CC_DH_INVALID_ORDER_SIZE_ERROR;

        if (pubKeySizeBytes > modPsizeBytes)
                return CC_DH_INVALID_PUBLIC_KEY_SIZE_ERROR;


        /* convert input data into LSW arrays */
        /*------------------------------------*/
// RL - restrict zeroing
        CC_PalMemSetZero( tempBuff_ptr, sizeof(CCDhTemp_t) );

        Error = CC_CommonConvertMsbLsbBytesToLswMswWords(tmpPubKey_ptr->n, modPsizeBytes, modP_ptr, modPsizeBytes);
        if (Error) {
                Error = CC_DH_INVALID_MODULUS_SIZE_ERROR;
                return Error;
    }

        Error = CC_CommonConvertMsbLsbBytesToLswMswWords(tmpPubKey_ptr->e, modPsizeBytes, orderQ_ptr, orderQsizeBytes);
        if (Error) {
                Error = CC_DH_INVALID_ORDER_SIZE_ERROR;
                goto End;
    }

        Error = CC_CommonConvertMsbLsbBytesToLswMswWords(tempBuff_ptr->PrimeData.DataIn, modPsizeBytes, pubKey_ptr, pubKeySizeBytes);
        if (Error) {
                Error = CC_DH_INVALID_PUBLIC_KEY_SIZE_ERROR;
                goto End;
    }

        /* calculate sizes in bits of input parameters */
        modPsizeBits   = CC_MIN(8*modPsizeBytes, CC_CommonGetWordsCounterEffectiveSizeInBits(tmpPubKey_ptr->n, (uint16_t)modPsizeBytes/4));
        orderQsizeBits = CC_MIN(8*orderQsizeBytes, CC_CommonGetWordsCounterEffectiveSizeInBits(tmpPubKey_ptr->e, (uint16_t)(orderQsizeBytes+3)/4));
        pubKeySizeBits = CC_MIN(8*pubKeySizeBytes, CC_CommonGetWordsCounterEffectiveSizeInBits(tempBuff_ptr->PrimeData.DataIn, (uint16_t)(pubKeySizeBytes+3)/4));

        /* check sizes */
        if (modPsizeBits < CC_DH_MIN_VALID_KEY_SIZE_VALUE_IN_BITS ||
            modPsizeBits % 256 != 0 ||
            modPsizeBits > CC_DH_MAX_VALID_KEY_SIZE_VALUE_IN_BITS) {
                Error = CC_DH_INVALID_MODULUS_SIZE_ERROR;
                goto End;
        }

        if (orderQsizeBits < CC_DH_SEED_MIN_SIZE_IN_BITS ||
            orderQsizeBits % 32 != 0) {
                Error = CC_DH_INVALID_ORDER_SIZE_ERROR;
                goto End;
        }

        if (pubKeySizeBits > modPsizeBits ||
            pubKeySizeBits <= 1) {
                Error = CC_DH_INVALID_PUBLIC_KEY_SIZE_ERROR;
                goto End;
        }


        /*----------------------------------------------------------- */
        /* Step 2. Check value of public key:   pubKey < P-1          */
        /*         Note: pubKey > 1 already is checked above          */
        /*----------------------------------------------------------- */

        /* decrement modulus in temp buffer n (in little endianness). Note: the modulus is odd */
        tmpPubKey_ptr->n[0] -= 1;

        /* compare pub key saved in temp buff e to P-1 */
        cmpCounters = CC_CommonCmpLsWordsUnsignedCounters(
                                                            tmpPubKey_ptr->e, /* counter1 - pubKey */
                                                            (uint16_t)(pubKeySizeBytes+3)/4,
                                                            tmpPubKey_ptr->n, /* counter2 - (P-1) */
                                                            (uint16_t)modPsizeBytes/4);

        if (cmpCounters != CC_COMMON_CmpCounter2GreaterThenCounter1) {
                Error = CC_DH_INVALID_PUBLIC_KEY_ERROR;
                goto End;
        }

        /*----------------------------------------------------*/
        /* Step 4. Initialization of PubKey and PrivData      */
        /*         structures for exponentiation              */
        /*----------------------------------------------------*/

        /* increment (revert) modulus in temp buffer n (in little endianness) */
        tmpPubKey_ptr->n[0] += 1;

        /* set modulus and exponent sizes in DH_PubKey structure  */
        tmpPubKey_ptr->nSizeInBits = modPsizeBits;
        tmpPubKey_ptr->eSizeInBits = orderQsizeBits;

        /*  initialize the H value in LLF of PubKey for exponentiation  */
        Error = RsaInitPubKeyDb( tmpPubKey_ptr );

        if (Error != CC_OK) {
                Error = CC_DH_INVALID_PUBLIC_KEY_ERROR;
                goto End;
        }

        /*-----------------------------------------------------------*/
        /* Step 3. Calculate Res = Key ^ Q mod P , if Res == 1,      */
        /*         then key is valid, else non valid                 */
        /*-----------------------------------------------------------*/

        /* exponentiation DataOut = DataIn ^ exp mod n */
        Error = RsaExecPubKeyExp(tmpPubKey_ptr, &tempBuff_ptr->PrimeData);

        if (Error != CC_OK) {
                Error = CC_DH_INVALID_PUBLIC_KEY_ERROR;
                goto End;
        }

        /* set 1 to PubKey_ptr->n buffer (used as temp buffer) */
        CC_PalMemSetZero((uint8_t*)&tmpPubKey_ptr->n, modPsizeBytes);
        tmpPubKey_ptr->n[0] = 1;

        /* compare DataOut to 1: */
        cmpRes = CC_CommonCmpLsWordsUnsignedCounters(
                                                       tempBuff_ptr->PrimeData.DataOut, modPsizeBytes/4, tmpPubKey_ptr->n, modPsizeBytes/4);

        if (cmpRes != 0)  {/* if Res != 1 */
                Error = CC_DH_INVALID_PUBLIC_KEY_ERROR;
                goto End;
    }

End:

        /* clean temp buffers */
        CC_PalMemSetZero(tempBuff_ptr, sizeof(CCDhTemp_t));

        return Error;
}

