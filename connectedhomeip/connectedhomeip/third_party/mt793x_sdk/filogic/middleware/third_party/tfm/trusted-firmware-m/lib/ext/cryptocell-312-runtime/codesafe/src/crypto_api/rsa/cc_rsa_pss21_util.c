/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*  Inculde Files */
#ifdef CC_IOT
    #if defined(MBEDTLS_CONFIG_FILE)
    #include MBEDTLS_CONFIG_FILE
    #endif
#endif

#ifndef USE_MBEDTLS_CRYPTOCELL
#include "cc_hash.h"
#endif

#if !defined(CC_IOT) || ( defined(CC_IOT) && defined(MBEDTLS_RSA_C))

#include "cc_pal_mem.h"
#include "cc_common_math.h"
#include "cc_rsa_local.h"
#include "cc_rsa_error.h"
#include "cc_hash_defs.h"
#include "cc_rnd_error.h"
#include "cc_rsa_prim.h"
#include "cc_general_defs.h"

#ifdef CC_RSA_SIGN_USE_TEMP_SALT
#include "CRYS_RSA_PSS21_defines.h"
extern uint8_t SaltDB_T[NUM_OF_SETS_TEST_VECTORS][NUM_OF_TEST_VECTOR_IN_SET][CC_RSA_PSS_SALT_LENGTH];
extern uint16_t Global_Set_Index_T;
extern uint16_t Global_vector_Index_T;
#endif


#if !defined(_INTERNAL_CC_NO_RSA_SCHEME_21_SUPPORT) && !defined(_INTERNAL_CC_NO_RSA_VERIFY_SUPPORT)

/**********************************************************************************************************/
/**
        Function Name: RsaPssVerify21
        Date:   06-12-2004
        Author: Ohad Shperling

    \brief RsaPssVerify21 implements EMSA-PSS-Verify algorithm
   as defined in PKCS#1 v2.1 Sec 9.1.2

   @param[in] Context_ptr - Pointer to a valid context as
                            given from the VerifyFinish function.

   The field HASH_Result inside the Context_ptr is initialized with the Hashed digested message.
   The field HASH_Result_Size inside the Context_ptr is initialized with the Hash digested message size

   @return CCError_t - CC_OK, or error

*/
CCError_t RsaPssVerify21(RSAPubContext_t *Context_ptr)
{
        /**********Fitting to the spec******************************/
        /* Context_ptr->MsgDigestCAL = mHash = Hash(M)
     * &Context_ptr->KeyObj.PubObj.EBD[0] = pointer to EM = S^E mod N
     * &Context_ptr->KeyObj.PubObj.EBDSize = pointer to EM size
        */


        CCError_t Error ;
        uint8_t   *ED_ptr;
        uint32_t  EDSizeInBytes;
        uint32_t PubNNewSizeBytes,i;
        uint8_t *maskedDB_ptr;

        uint32_t maskedDB_size;
        uint32_t TempIndex ;
        uint8_t *dbMask_ptr = Context_ptr->T_Buf;
#ifdef USE_MBEDTLS_CRYPTOCELL
        const mbedtls_md_info_t *md_info=NULL;
#endif

        CCHashResultBuf_t H_Saved_buf;
        /* Set the ED block pointer */

        /*Temporary - only for the size of N*/
        CCRsaPubKey_t *PubKey_ptr = (CCRsaPubKey_t *)Context_ptr->PubUserKey.PublicKeyDbBuff;

        /* FUNCTION LOGIC */

        ED_ptr=(uint8_t*)&Context_ptr->EBD[0];/* = EM*/
        EDSizeInBytes = Context_ptr->EBDSizeInBits/8;
        if (Context_ptr->EBDSizeInBits % 8)
                EDSizeInBytes++;

        /*Round up the new bytes number - According to the Spec*/
        PubNNewSizeBytes = (PubKey_ptr->nSizeInBits - 1)/8;

        if (((PubKey_ptr->nSizeInBits - 1) % 8) != 0) {/*Rounding Only in case that (PubNSizebits -1) is not divisble by 8 */
                PubNNewSizeBytes++;
        } else {/*(PubNSizebits -1) is divisble by 8 hence ED_ptr has to be shortened by the first octet according to the spec*/
                ED_ptr += 1;
                EDSizeInBytes -= 1;
        }

        /*
         *  9.1.2 <3> Check restriction of PubNNewSizeBytes - already checked in Verify Init
         *  9.1.2 <4> Check that the rightmost octet of EM have the hexadecimal value 0xbc
         */
        if (ED_ptr[EDSizeInBytes-1] != 0xbc)
                return CC_RSA_ERROR_PSS_INCONSISTENT_VERIFY;

        /*
         *  9.1.2 <5> Define the H and the maskedDB
         */
        maskedDB_ptr = ED_ptr;
        maskedDB_size = PubNNewSizeBytes - Context_ptr->HASH_Result_Size*sizeof(uint32_t) - 1 ;

        /*
         *  9.1.2 <6> Check that the leftmost bits in the leftmost octet of EM have the value 0.
         *     Note: In CC implementation only the left most bit must be checked, because the
         *           modulus size is always a multiple of 8 bits.
         */
        if (maskedDB_ptr[0] & 0x80)
                return CC_RSA_ERROR_PSS_INCONSISTENT_VERIFY;

        /*need to save H because ED_ptr is to be used - Context_ptr->MsgDigestSRC = H;
          i.e. COPIed hash size bytes directly before 0xbc byte (FIPS 186-4, 5.4)      */
        CC_PalMemCopy((uint8_t *)H_Saved_buf, &ED_ptr[maskedDB_size], Context_ptr->HASH_Result_Size*4);

        /* Calculate the mask by MGF */
        switch (Context_ptr->MGF_2use) {
        case CC_PKCS1_MGF1:

                Error = RsaOaepMGF1( (uint16_t)(Context_ptr->HASH_Result_Size*sizeof(uint32_t)), /*hashLen*/
                                           (uint8_t *)H_Saved_buf,                                 /*mgfSeed = hash */
                                           (uint16_t)(Context_ptr->HASH_Result_Size*sizeof(uint32_t)),/*seedLen*/
                                           PubNNewSizeBytes - Context_ptr->HASH_Result_Size*sizeof(uint32_t) - 1, /*maskLen*/
                                           dbMask_ptr,                                             /*mask out*/
                                           Context_ptr->HashOperationMode,                         /*hashMode*/
                                           (uint8_t *)Context_ptr->PrimeData.DataOut,              /*1-st tempBuff*/
                                           (uint8_t *)Context_ptr->PrimeData.DataIn);              /*2-nd tempBuff*/
                if (Error != CC_OK) {
                        return Error;
                }

                break;

        /*Currently for PKCS1 Ver 2.1 only MGF1 is implemented*/
        case CC_PKCS1_NO_MGF:
        default:
                return CC_RSA_MGF_ILLEGAL_ARG_ERROR;
        }

        /*
         *  9.1.2 <8> Xor operation on length (PubNNewSizeBytes - Context_ptr->hLen - 1)
         */

        for (i=0;i<maskedDB_size;i++) {
                dbMask_ptr[i] = dbMask_ptr[i] ^ maskedDB_ptr[i] ;
        }

        /*
         *  9.1.2 <9> Set the leftmost 8emLen - emBits bits of the leftmost octet in DB to zero
         *     Note: In CC implementation only NS bit must be zeroed, because modulus size is
         *           always a multiple of 8 bits.
         */
        dbMask_ptr[0] &= 0x7F;

        /*
         *  9.1.2 <10>
         */

        i=0;
        while (dbMask_ptr[i] == 0) {
                i++;
        }

        if (Context_ptr->SaltLen == CC_RSA_VERIFY_SALT_LENGTH_UNKNOWN) {

                /* For security goals and preventing memory overflow
                   check that the buffer parts are consistent */
                if (PubNNewSizeBytes < Context_ptr->HASH_Result_Size*sizeof(uint32_t) + 2 + i) {
                        return CC_RSA_ERROR_PSS_INCONSISTENT_VERIFY;
                }
                /*Derive the salt length if not supplied */
                Context_ptr->SaltLen = (uint16_t)(PubNNewSizeBytes - Context_ptr->HASH_Result_Size*sizeof(uint32_t) - 2 - i);
        } else {
            /* Sanity check - verify that the given Saltlen equals the computed saltlen*/
                if (Context_ptr->SaltLen != (uint16_t)(PubNNewSizeBytes - Context_ptr->HASH_Result_Size*sizeof(uint32_t) - 2 - i))
                {
                    return CC_RSA_ERROR_IN_DECRYPTED_BLOCK_PARSING;
                }
                TempIndex = PubNNewSizeBytes - Context_ptr->HASH_Result_Size*sizeof(uint32_t) - Context_ptr->SaltLen - 2;
                for (i = 0; i < TempIndex; i++) {
                        if (dbMask_ptr[i] != 0)
                                return CC_RSA_ERROR_PSS_INCONSISTENT_VERIFY;
                }
        }

        if (dbMask_ptr[i] != 0x01)
                return CC_RSA_ERROR_PSS_INCONSISTENT_VERIFY;

        /*
         *  9.1.2 <11> Let salt be the last sLen octets in DB
         *  9.1.2 <12> Let M' ==>
         *   (0x) 00 00 00 00 00 00 00 00 || mHash || salt
         *   Note: ED is used now as M' temp buffer
         */
        CC_PalMemSetZero(ED_ptr, CC_RSA_PSS_PAD1_LEN);/*CC_RSA_PSS_PAD1_LEN = 8*/

        /*copy the Hash output */
        CC_PalMemCopy(&ED_ptr[CC_RSA_PSS_PAD1_LEN], (uint8_t *)Context_ptr->HASH_Result, Context_ptr->HASH_Result_Size*sizeof(uint32_t));
        CC_PalMemCopy(&ED_ptr[CC_RSA_PSS_PAD1_LEN + Context_ptr->HASH_Result_Size*sizeof(uint32_t)],
                        &dbMask_ptr[maskedDB_size - Context_ptr->SaltLen], Context_ptr->SaltLen);

        /*
         *  9.1.2 <13> H' = Hash(M')
         */
#ifdef USE_MBEDTLS_CRYPTOCELL
        md_info = mbedtls_md_info_from_string( HashAlgMode2mbedtlsString[Context_ptr->HashOperationMode] );
        if (NULL == md_info)
        {
            return CC_RSA_HASH_ILLEGAL_OPERATION_MODE_ERROR;
        }
        Error = mbedtls_md(md_info,
                ED_ptr,
                CC_RSA_PSS_PAD1_LEN + Context_ptr->HASH_Result_Size*sizeof(uint32_t) + Context_ptr->SaltLen,
                (unsigned char *)Context_ptr->HASH_Result);
        if (Error != 0)
        {
            return CC_RSA_HASH_ILLEGAL_OPERATION_MODE_ERROR;
        }
#else
        Error = CC_Hash(Context_ptr->HashOperationMode,
                          ED_ptr,
                          CC_RSA_PSS_PAD1_LEN + Context_ptr->HASH_Result_Size*sizeof(uint32_t) + Context_ptr->SaltLen,/*8+20+20*/
                          Context_ptr->HASH_Result);

        if (Error != CC_OK) {
                return Error;
        }
#endif
        if (CC_PalMemCmp((uint8_t *)Context_ptr->HASH_Result, (uint8_t *)H_Saved_buf, Context_ptr->HASH_Result_Size*sizeof(uint32_t))) {
                return CC_RSA_ERROR_PSS_INCONSISTENT_VERIFY;
        } else {
                return CC_OK;
        }
}

#endif /*!defined(_INTERNAL_CC_NO_RSA_SCHEME_21_SUPPORT) && !defined(_INTERNAL_CC_NO_RSA_VERIFY_SUPPORT)*/



#if !defined(_INTERNAL_CC_NO_RSA_SCHEME_21_SUPPORT) && !defined(_INTERNAL_CC_NO_RSA_SIGN_SUPPORT)
#ifndef _INTERNAL_CC_NO_RSA_SIGN_SUPPORT
/* -------------------------------------------------------------
 *  Function Name: RsaPssSign21
 *  Date:   06-12-2004
 *  Author: Ohad Shperling
 *
 *  Inputs:
 *  Outputs:
 *
 *  Algorithm: According to PKCS1 v.2.1
 *
 *  Update History:
 *  Date:       Description:
 *
 * ----------------------------------------------------------- */

CCError_t RsaPssSign21(
                               CCRndContext_t *rndContext_ptr, /*! random context */
                               RSAPrivContext_t *Context_ptr,
                               uint8_t  *Output_ptr)
{

#ifdef CC_RSA_SIGN_USE_TEMP_SALT
        /*only for debug of signing*/
        /*Using a known Salt for debug*/
        uint8_t *Salt = SaltDB_T[Global_Set_Index_T][Global_vector_Index_T];
#else
        /*In operational mode Salt is a random number*/
        uint8_t *Salt = Output_ptr;/*This stack memory saving is ok because Output_ptr is used only in the Primitive operation*/
#endif
#ifdef USE_MBEDTLS_CRYPTOCELL
                const mbedtls_md_info_t *md_info=NULL;
#endif
        /* The return error identifier */
        CCError_t Error = CC_OK;
        uint32_t i;
        uint32_t TempIndex;

        void   *rndState_ptr;
        CCRndGenerateVectWorkFunc_t RndGenerateVectFunc;

        /*Parameter for the actual size of the modulus N in bits*/
        uint32_t PrvNSizebits;

        /*Parameter for the new size of the modulus N in bytes according to PKCS1 Ver 2.1*/
        uint32_t PrvNNewSizeBytes;/*rounded number of Bytes for padding2 length*/
        uint32_t Index4PSLength;

        uint8_t *EMPadOutputBuffer;
        uint8_t *MaskOutput_ptr = Context_ptr->T_Buf;/*for stack space saving*/

        CCRsaPrivKey_t *PrivKey_ptr = (CCRsaPrivKey_t *)Context_ptr->PrivUserKey.PrivateKeyDbBuff;
        uint32_t hashResultSize; /*HASH size in bytes*/


        /* FUNCTION LOGIC */

        /* check parameters */
        if (rndContext_ptr == NULL)
                return CC_RND_CONTEXT_PTR_INVALID_ERROR;

        rndState_ptr = rndContext_ptr->rndState;
        RndGenerateVectFunc = rndContext_ptr->rndGenerateVectFunc;

        if (RndGenerateVectFunc == NULL)
                return CC_RND_GEN_VECTOR_FUNC_ERROR;

        /* .................. initializing local variables ................... */
        /* ------------------------------------------------------------------- */

        EMPadOutputBuffer = (uint8_t*) Context_ptr->EBD;
        hashResultSize = Context_ptr->HASH_Result_Size*sizeof(uint32_t);

        /*
         *  9.1.1 <1> checking length restriction of the message M - done in the Update phase
         *  9.1.1 <2> Hash operation - done in the Update phase
         */

        /*
         *  Finding Actual size in bits and new size of Bytes of the modulus N
         *  This value is already calculated in  Context_ptr->KeyObj.PrvCRTObj.nSizeInBits
         *  or in  Context_ptr->KeyObj.PrvPAIRObj.nSizeInBits
         */

        /* Reset the working buffer - RL - check */
        CC_PalMemSet(EMPadOutputBuffer, 0x00, CC_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS*sizeof(uint32_t));

        /*Round up the new bytes number*/
        PrvNNewSizeBytes = (PrivKey_ptr->nSizeInBits -1)/8;
        PrvNSizebits = PrivKey_ptr->nSizeInBits;

        if (((PrvNSizebits -1) % 8) != 0)
                PrvNNewSizeBytes++;
        /*rounding */

        /*
         *  9.1.1 <3> Check restriction of PrvNNewSizeBytes - already checked in Sign Init
         *  9.1.1 <5> Generating M' ==> using the output buffer as a container
         *  EMPadOutputBuffer = (0x) 00 00 00 00 00 00 00 00 || mHash || salt
         */

        CC_PalMemSet(EMPadOutputBuffer, 0x00, CC_RSA_PSS_PAD1_LEN);/*CC_RSA_PSS_PAD1_LEN = 8*/

        /*copy the Hash output */
        CC_PalMemCopy(&EMPadOutputBuffer[CC_RSA_PSS_PAD1_LEN], (uint8_t*)Context_ptr->HASH_Result, hashResultSize);

        /*
         *  9.1.1 <4> Generating a random salt ==> using the output buffer as a container
         */
#ifndef CC_RSA_SIGN_USE_TEMP_SALT /*If not using a known salt for Debug then generate random*/
        Error = RndGenerateVectFunc(rndState_ptr, (unsigned char *)Salt, (size_t)Context_ptr->SaltLen);
        if (Error != CC_OK) {
                return Error;
        }
#endif

        CC_PalMemCopy(&EMPadOutputBuffer[CC_RSA_PSS_PAD1_LEN + hashResultSize], Salt, Context_ptr->SaltLen);
#ifdef USE_MBEDTLS_CRYPTOCELL
        md_info = mbedtls_md_info_from_string( HashAlgMode2mbedtlsString[Context_ptr->HashOperationMode] );
        if (NULL == md_info)
        {
            return CC_RSA_HASH_ILLEGAL_OPERATION_MODE_ERROR;
        }
        Error = mbedtls_md(md_info,
                EMPadOutputBuffer,
                CC_RSA_PSS_PAD1_LEN + hashResultSize + Context_ptr->SaltLen,/*8+hLen+20*/
                (unsigned char *)Context_ptr->HASH_Result);

        if (Error != 0)
        {
            return CC_RSA_HASH_ILLEGAL_OPERATION_MODE_ERROR;
        }
#else
        Error = CC_Hash(Context_ptr->HashOperationMode,
                          EMPadOutputBuffer,
                          CC_RSA_PSS_PAD1_LEN + hashResultSize + Context_ptr->SaltLen,/*8+hLen+20*/
                          Context_ptr->HASH_Result);

        if (Error != CC_OK) {
                return Error;
        }
#endif
        /*
         *  9.1.1 <7+8> Generate an octet string of zeros of size emLen-sLen-hLen-2 ==> use the output buffer as a container
         *              DB = PS || 0x01 || salt
         */

        Index4PSLength = PrvNNewSizeBytes - Context_ptr->SaltLen - hashResultSize - 2;

        CC_PalMemSet(EMPadOutputBuffer, 0x00, Index4PSLength);
        EMPadOutputBuffer[Index4PSLength] = 0x01;
        CC_PalMemCopy(&(EMPadOutputBuffer[Index4PSLength+1]), Salt, Context_ptr->SaltLen);

        /*
         *  9.1.1 <9> MGF operation
         */

        switch (Context_ptr->MGF_2use) {
        case CC_PKCS1_MGF1:

                Error = RsaOaepMGF1( (uint16_t)hashResultSize, /* hashLen */
                                           (uint8_t *)Context_ptr->HASH_Result,         /* mgfSeed */
                                           (uint16_t)hashResultSize, /* seedLen */
                                           PrvNNewSizeBytes - hashResultSize - 1, /* maskLen */
                                           MaskOutput_ptr,                              /* mask */
                                           Context_ptr->HashOperationMode,              /* hashMode */
                                           (uint8_t *)Context_ptr->PrimeData.DataOut,   /* temp1 */
                                           (uint8_t *)Context_ptr->PrimeData.DataIn);   /* temp2 */
                if (Error != CC_OK) {
                        return Error;
                }

                break;

        /* Currently for PKCS1 Ver 2.1 only MGF1 is implemented */
        case CC_PKCS1_NO_MGF:

        default:

                return CC_RSA_MGF_ILLEGAL_ARG_ERROR;

        }/* end of MGF type switch case */


        /*
         *  9.1.1 <10> Xor operation on length (PrvNNewSizeBytes - Context_ptr->hLen - 1)
         */

        TempIndex = PrvNNewSizeBytes - hashResultSize - 1;
        for (i = 0; i < TempIndex; i++) {
                EMPadOutputBuffer[i] = EMPadOutputBuffer[i] ^ MaskOutput_ptr[i];
        }


        /*
         *   9.1.1 <11> Set the leftmost 8*emLen-emBits bits of the leftmost octet in maskedDB to zero
         *      Because the RSA modulus in CC always is a multiple of 8, only one (left most) bit
         *              need to be zeroed.
         */
         EMPadOutputBuffer[0] &= 0x7F;

        /*
         *  ? 9.1.1 <12> Let EM = maskedDB || H || 0xbc
         *      Note: maskedDB is already generated from the Xor operation.
         */
        CC_PalMemCopy(&(EMPadOutputBuffer[PrvNNewSizeBytes - hashResultSize - 1]),
                         (uint8_t *)Context_ptr->HASH_Result, hashResultSize);

        EMPadOutputBuffer[PrvNNewSizeBytes - 1] = 0xbc;

        /*
         *  FINISH 9.1.1
         *
         *  8.1.1 <2.b>
         *  Apply the RSASP1 signature primitive to the RSA private key K and the message
         *  representative m to produce an integer signature representative s
         *
         */

        /* ...  execute RSA encrypt using RSA_PRIM_Decrypt for exponentiation ... */
        /* ---------------------------------------------------------------------- */

        Error = CC_RsaPrimDecrypt(&Context_ptr->PrivUserKey,
                                      &Context_ptr->PrimeData,
                                      EMPadOutputBuffer,
                                      (uint16_t)PrvNNewSizeBytes,
                                      Output_ptr);

        return Error;

}

#endif /*!defined(_INTERNAL_CC_NO_RSA_SCHEME_21_SUPPORT) && !defined(_INTERNAL_CC_NO_RSA_SIGN_SUPPORT)*/


void CC_RSA_PSS21_UTIL_foo(void) {}
#endif //_INTERNAL_CC_NO_RSA_SIGN_SUPPORT
#endif /* !defined(CC_IOT) || ( defined(CC_IOT) && defined(MBEDTLS_RSA_C)) */
