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
#include "cc_pal_types.h"
#include "cc_common_math.h"
#include "cc_rsa_error.h"
#ifndef USE_MBEDTLS_CRYPTOCELL
#include "cc_hash.h"
#endif
#include "cc_hash_defs.h"
#include "cc_rsa_local.h"
#include "cc_rnd_error.h"
#include "cc_general_defs.h"

/************************ Defines ****************************/

/************************ Enums ******************************/

/************************ Typedefs ***************************/

/************************ Global Data *************************/

#ifdef DEBUG_OAEP_SEED
#include "CRYS_RSA_PSS21_defines.h"
extern uint8_t SaltDB[NUM_OF_SETS_TEST_VECTORS][NUM_OF_TEST_VECTOR_IN_SET][CC_RSA_PSS_SALT_LENGTH];
extern uint16_t Global_Set_Index;
extern uint16_t Global_vector_Index;
#endif

/************* Private function prototype ****************/



#if !defined(_INTERNAL_CC_NO_RSA_ENCRYPT_SUPPORT) && !defined(_INTERNAL_CC_NO_RSA_VERIFY_SUPPORT)
/************************ Public Functions ******************************/
/**
   @brief
   RsaPssOaepEncode implements the the Encoding operation according to the PKCS#1 as defined
   in PKCS#1 v2.1 7.1.1 (2) and PKCS#1 v2.0
*/
CCError_t RsaPssOaepEncode(
                                   CCRndContext_t *rndContext_ptr, /* random functions context */
                                   CCPkcs1HashFunc_t hashFunc,     /* PKCS1 hash mode enum */
                                   CCPkcs1Mgf_t MGF,               /* MGF function type enum */
                                   uint8_t *M_ptr,                     /* a pointer to the message to be encoded */
                                   uint16_t MSize,                     /* the message size in bytes */
                                   uint8_t *P_ptr,                     /* a pointer to the label; can be empty string */
                                   size_t   PSize,                     /* the size of the label in bytes */
                                   uint16_t emLen, /* The value is set before the call */
                                   CCRsaPrimeData_t  *PrimeData_ptr,/* temp buffer */
                                   uint8_t  *EMInput_ptr,              /* encoded message output */
                                   CCPkcs1Version_t PKCS1_ver)
{

        /* FUNCTION DECLARATIONS */

        /* The return error identifier */
        CCError_t Error = CC_OK;

        /*For PKCS1 Ver21 standard: emLen = k = Public mod N size*/
        /*Used for output of MGF1 function ; the size is at most emLen */
        uint8_t *MaskDB_Ptr =((CCRsaOaepData_t*)((void*)PrimeData_ptr->InternalBuff))->MaskDB;
        uint32_t HashOutputSize;
        uint16_t TmpSize, I;
        uint8_t *TmpByte_ptr;
        uint8_t *EM_ptr = &EMInput_ptr[1];
        uint8_t VersionConstant;   /*Used to distinguish between Ver 2.1 and others for some memory manipulations*/

#ifdef DEBUG_OAEP_SEED
        uint8_t *SeedTEST = SaltDB[Global_Set_Index][Global_vector_Index]; /*A Data Base created in the test file for checking the Encrypted operation*/
#endif/*otherwise the seed is generated strait to EM_ptr*/

#ifdef USE_MBEDTLS_CRYPTOCELL
        const mbedtls_md_info_t *md_info=NULL;
        mbedtls_md_context_t *md_ctx=NULL;
#endif

        void   *rndState_ptr;
        CCRndGenerateVectWorkFunc_t RndGenerateVectFunc;

        /* FUNCTION LOGIC */

        /* check parameters */
        if (rndContext_ptr == NULL)
                return CC_RND_CONTEXT_PTR_INVALID_ERROR;

        rndState_ptr = (rndContext_ptr->rndState);
        RndGenerateVectFunc = rndContext_ptr->rndGenerateVectFunc;

        if (RndGenerateVectFunc == NULL)
                return CC_RND_GEN_VECTOR_FUNC_ERROR;

        /* .................. initializing local variables ................... */
        /* ------------------------------------------------------------------- */

        /*Initializing the first Byte to Zero*/
        EMInput_ptr[0] = 0;

        /* get HASH output size */
        switch (hashFunc) {
        case CC_HASH_MD5_mode :
                HashOutputSize = CC_HASH_MD5_DIGEST_SIZE_IN_BYTES;
                break;
        case CC_HASH_SHA1_mode:
                HashOutputSize = CC_HASH_SHA1_DIGEST_SIZE_IN_BYTES;
                break;
        case CC_HASH_SHA224_mode:
                HashOutputSize = CC_HASH_SHA224_DIGEST_SIZE_IN_BYTES;
                break;
        case CC_HASH_SHA256_mode:
                HashOutputSize = CC_HASH_SHA256_DIGEST_SIZE_IN_BYTES;
                break;
        case CC_HASH_SHA384_mode:
                HashOutputSize = CC_HASH_SHA384_DIGEST_SIZE_IN_BYTES;
                break;
        case CC_HASH_SHA512_mode:
                HashOutputSize = CC_HASH_SHA512_DIGEST_SIZE_IN_BYTES;
                break;
        default:
                return CC_RSA_HASH_ILLEGAL_OPERATION_MODE_ERROR;
        }
        // RL Version = 2
        if (PKCS1_ver == CC_PKCS1_VER21)
                VersionConstant = 2;
        else
                VersionConstant = 1;

/*------------------------------------------------------------*
 * Step 1 : If the length of P is greater than                *
 *           the input limitation for the hash                *
 *           function (2^29 octets for SHA-1)                 *
 *           then output "parameter string too long" and stop *
 *                                                            *
 *           In PKCS1_Ver2.1 L = P                            *
 *------------------------------------------------------------*/

/* if the P  larger then 2^29 which is the input limitation for HASH
   return error (to prevent an overflow on the transition to bits ) */
        if (PSize >= (1 << 29))
                return CC_RSA_BASE_OAEP_ENCODE_PARAMETER_STRING_TOO_LONG;

/*-------------------------------------------------*
 * Step 2 : If ||M|| > emLen-2hLen-1 then output   *
 *   "message too long" and stop.                  *
 *                                                 *
 * for PKCS1_Ver2.1 Step 1 <b>:            *
 * If ||M|| > emLen - 2hLen - 2                    *
 *-------------------------------------------------*/

        if ((uint32_t)MSize + 2 * HashOutputSize + VersionConstant > emLen)
                return CC_RSA_BASE_OAEP_ENCODE_MESSAGE_TOO_LONG;

/*-----------------------------------------------------*
 * Step 3 : Generate an octet string PS consisting of  *
 *           emLen-||M||-2hLen-1 zero octets.          *
 *           The length of PS may be 0.                *
 *                                                     *
 * PKCS1_VER21 Step 2 <b>                  *
 *       Generate an octet string PS consisting of *
 *           emLen-||M||-2hLen-2 zero octets.          *
 *           The length of PS may be 0                 *
 *-----------------------------------------------------*/
        CC_PalMemSetZero((uint8_t*)&EM_ptr[2*HashOutputSize], emLen-MSize-2*HashOutputSize-VersionConstant);

/*--------------------------------------------------------------*
 * Step 4 : Let pHash = Hash(P), an octet string of length hLen.*
 * PKCS1_VER21 Step 2 <a> where L is denoted by P and usually   *
 * an empty string                                              *
 *--------------------------------------------------------------*/
        if (P_ptr!=NULL) {
#ifdef USE_MBEDTLS_CRYPTOCELL
                md_info = mbedtls_md_info_from_string( HashAlgMode2mbedtlsString[hashFunc] );
                if (NULL == md_info) {
                     return CC_RSA_HASH_ILLEGAL_OPERATION_MODE_ERROR;
                }
                Error = mbedtls_md(md_info,
                                   P_ptr,
                                   PSize,
                                   (unsigned char *)((CCRsaOaepData_t*)((void*)PrimeData_ptr->InternalBuff))->HashResultBuff);
 #else
                Error=CC_Hash( hashFunc,
                                 P_ptr,
                                 PSize,
                                 ((CCRsaOaepData_t*)((void*)PrimeData_ptr->InternalBuff))->HashResultBuff);
#endif
                if (Error!=CC_OK) {
                        goto End;
                }
        } else {/* if empty string */
#ifdef USE_MBEDTLS_CRYPTOCELL
                md_ctx = &(((CCRsaOaepData_t*)((void*)PrimeData_ptr->InternalBuff))->hash_ctx);
                md_info = mbedtls_md_info_from_string( HashAlgMode2mbedtlsString[hashFunc] );
                if (NULL == md_info) {
                        return CC_RSA_HASH_ILLEGAL_OPERATION_MODE_ERROR;
                }
                mbedtls_md_init(md_ctx);
                Error = mbedtls_md_setup(md_ctx, md_info, 0); // 0 = HASH, not HMAC
                if (Error != 0) {
                        goto End;
                }
                Error = mbedtls_md_starts(md_ctx);
#else
                Error=CC_HashInit(&(((CCRsaOaepData_t*)((void*)PrimeData_ptr->InternalBuff))->HashUsercontext),
                                     hashFunc);
#endif
                if (Error!=CC_OK) {
                        goto End;
                }
#ifdef USE_MBEDTLS_CRYPTOCELL
                Error = mbedtls_md_finish(md_ctx, (unsigned char *)((CCRsaOaepData_t*)((void*)PrimeData_ptr->InternalBuff))->HashResultBuff);
#else
                Error=CC_HashFinish( &(((CCRsaOaepData_t*)((void*)PrimeData_ptr->InternalBuff))->HashUsercontext) ,
                                        ((CCRsaOaepData_t*)((void*)PrimeData_ptr->InternalBuff))->HashResultBuff);
#endif
                if (Error!=CC_OK) {
                        goto End;
                }
        }
        /*Copy the Hash result to the proper place in the output buffer*/
        CC_PalMemCopy(&EM_ptr[HashOutputSize], (uint8_t *)(((CCRsaOaepData_t*)((void*)PrimeData_ptr->InternalBuff))->HashResultBuff), HashOutputSize);

/*---------------------------------------------------------*
 * Step 5 : Concatenate pHash, PS, the message M,          *
 *           and other padding to form a data block DB     *
 * PKCS1_VER21 Step 2 <c>                                  *
 *      Concatenate pHash, PS, the message M,              *
 *       and other padding to form a data block DB as      *
 *       DB = pHash || PS || 01 || M                       *
 *---------------------------------------------------------*/
        /* EM_ptr[emLen-MSize-1]=0x01;*/
        EM_ptr[emLen - MSize - 1 - (VersionConstant-1)] = 0x01;/*because emLen = PubNSize; but EM_ptr points to place 1*/
    if (MSize > 0) // need to copy only if there is data
        CC_PalMemCopy((uint8_t*)&EM_ptr[emLen - MSize - (VersionConstant - 1)], M_ptr, MSize);/*because emLen = PubNSize; but EM_ptr points to place 1 */

/*--------------------------------------------------------------*
 * Step 6 : Generate a random octet string seed of length hLen. *
 * PKCS1_VER21 Step 2.d: Generate a random octet string seed    *
 *                       of length hLen.                        *
 *--------------------------------------------------------------*/

#ifdef DEBUG_OAEP_SEED
        /*Only for PKCS#1 ver2.1 SHA1 - Salt length is 20*/
        CC_PalMemCopy(EM_ptr, SeedTEST,CC_RSA_PSS_SALT_LENGTH);
#else
        Error = RndGenerateVectFunc(rndState_ptr, (unsigned char *)EM_ptr, (size_t)HashOutputSize);
        if (Error != CC_OK) {
                goto End;
        }

#endif

/*-----------------------------------------------*
 * Step 7 : Let dbMask = MGF(seed, emLen-hLen).  *
 * PKCS1_VER21 Step 2 <e> Let                    *
 *      dbMask = MGF(seed, emLen-hLen-1).        *
 *-----------------------------------------------*/

        switch (MGF) {
        case CC_PKCS1_MGF1:

                Error = RsaOaepMGF1(
                                         HashOutputSize,                                                          /*hashLen*/
                                         &EM_ptr[0],                                                              /*mgfSeed*/
                                         HashOutputSize,                                                          /*seedLen*/
                                         emLen-HashOutputSize-(VersionConstant-1),                                /*maskLen*/
                                         &(((CCRsaOaepData_t*)((void*)PrimeData_ptr->InternalBuff))->MaskDB[0]), /*mask*/
                                         hashFunc,                                                                /*hashMode*/
                                         (uint8_t *)PrimeData_ptr->DataIn,                                        /*1-st tempBuff*/
                                         (uint8_t *)PrimeData_ptr->DataOut);                                      /*2-nd tempBuff*/

                if (Error != CC_OK) {
                        goto End;
                }
                break;

        /*Currently for PKCS1 Ver 2.1 only MGF1 is implemented*/
        case CC_PKCS1_NO_MGF:
        default:
                Error = CC_RSA_MGF_ILLEGAL_ARG_ERROR;
                goto End;
        }

        /*----------------------------------------------*
        *  Step 8 : PKCS1_VER21 Step 2.f:               *
        *             Let maskedDB = DB xor dbMask.     *
        *-----------------------------------------------*/

        TmpSize = emLen - HashOutputSize - (VersionConstant - 1);
        TmpByte_ptr = &EM_ptr[HashOutputSize];
        for (I = 0; I < TmpSize; I++)
                *(TmpByte_ptr + I) ^= ((CCRsaOaepData_t*)((void*)PrimeData_ptr->InternalBuff))->MaskDB[I];

        MaskDB_Ptr = TmpByte_ptr;

/*-----------------------------------------------*
 * Step 9 : Let seedMask = MGF(maskedDB, hLen).  *
 * PKCS1_VER21 Step 2.g                          *
 *-----------------------------------------------*/

        switch (MGF) {
        case CC_PKCS1_MGF1:

                Error = RsaOaepMGF1(
                                 HashOutputSize, /* This is important its uint32 as a result of size limits */
                                 MaskDB_Ptr,
                                 (uint16_t)(emLen-HashOutputSize-(VersionConstant-1)),
                                 HashOutputSize,
                                 &(((CCRsaOaepData_t*)((void*)PrimeData_ptr->InternalBuff))->SeedMask[0]),
                                 hashFunc,
                                 (uint8_t *)PrimeData_ptr->DataIn,
                                 (uint8_t *)PrimeData_ptr->DataOut);

                if (Error != CC_OK)
                        goto End;

                break;

                /*Currently for PKCS1 Ver 2.1 only MGF1 is implemented*/
        case CC_PKCS1_NO_MGF:
        default:
                Error = CC_RSA_MGF_ILLEGAL_ARG_ERROR;
                goto End;

        }/* end of MGF type switch */

/*-----------------------------------------------*
 * Step 10: Let maskedSeed = seed \xor seedMask. *
 * PKCS1_VER21 Step 2 <h>                        *
 *-----------------------------------------------*/

        TmpSize = HashOutputSize;
        TmpByte_ptr =& EM_ptr[0];
        for (I = 0;I < TmpSize; I++)
                *(TmpByte_ptr + I)^=((CCRsaOaepData_t*)((void*)PrimeData_ptr->InternalBuff))->SeedMask[I];

/*---------------------------------------------*
 *  * Step 11:  PKCS1_VER21 Step 2.i:          *
 * Let EM = 0x00 || maskedSeed || maskedDB.    *
 * This step is done !                         *
 *---------------------------------------------*/

        End:
#ifdef USE_MBEDTLS_CRYPTOCELL
        if((md_info!=NULL) && (md_ctx!=NULL)) {
                mbedtls_md_free(md_ctx);
        }
#endif
        /*Clear Internal buffers*/
        CC_PalMemSetZero((PrimeData_ptr->InternalBuff), sizeof(PrimeData_ptr->InternalBuff));
        return Error;

}
#endif /* !defined(CC_NO_RSA_ENCRYPT_SUPPORT) && !defined(_INTERNAL_CC_NO_RSA_VERIFY_SUPPORT)*/

#if !defined(CC_NO_RSA_DECRYPT_SUPPORT) && !defined(_INTERNAL_CC_NO_RSA_SIGN_SUPPORT)

/**
   @brief
   RsaPssOaepDecode implements the Decoding operation according to the
   PKCS#1 as defined in PKCS#1 v2.1 and PKCS#1 v2.0
*/
CCError_t RsaPssOaepDecode(
                                   CCPkcs1HashFunc_t hashFunc,
                                   CCPkcs1Mgf_t MGF,
                                   uint8_t  *EM_ptr,
                                   uint16_t EMSize,
                                   uint8_t *P_ptr,
                                   size_t  PSize,
                                   CCRsaPrimeData_t  *PrimeData_ptr, /*Only for stack memory save*/
                                   uint8_t *M_ptr,
                                   size_t  *MSize_ptr)
{
        /* FUNCTION DECLERATIONS */

        uint8_t HashOutputSize;
        /* The return error identifier */
        CCError_t Error = CC_OK;
        uint8_t  *maskedDB_ptr;
        uint16_t  I, TmpSize;
        uint8_t  *TmpByte_ptr;
#ifdef USE_MBEDTLS_CRYPTOCELL
        const mbedtls_md_info_t *md_info=NULL;
        mbedtls_md_context_t *md_ctx=NULL;
#endif

        /* FUNCTION LOGIC */

        /* .................. initializing local variables ................... */
        /* ------------------------------------------------------------------- */

        /* get hash output size */
        switch (hashFunc) {
        case CC_HASH_MD5_mode : /*MD5 is not recomended in PKCS1 ver 2.1 standard,
                                    henceit is not supported */
                return CC_RSA_HASH_ILLEGAL_OPERATION_MODE_ERROR;
        case CC_HASH_SHA1_mode:
                HashOutputSize = CC_HASH_SHA1_DIGEST_SIZE_IN_WORDS*CC_32BIT_WORD_SIZE;
                break;
        case CC_HASH_SHA224_mode:
                HashOutputSize = CC_HASH_SHA224_DIGEST_SIZE_IN_WORDS*CC_32BIT_WORD_SIZE;
                break;
        case CC_HASH_SHA256_mode:
                HashOutputSize = CC_HASH_SHA256_DIGEST_SIZE_IN_WORDS*CC_32BIT_WORD_SIZE;
                break;
        case CC_HASH_SHA384_mode:
                HashOutputSize = CC_HASH_SHA384_DIGEST_SIZE_IN_WORDS*CC_32BIT_WORD_SIZE;
                break;
        case CC_HASH_SHA512_mode:
                HashOutputSize = CC_HASH_SHA512_DIGEST_SIZE_IN_WORDS*CC_32BIT_WORD_SIZE;
                break;
        default:
                return CC_RSA_HASH_ILLEGAL_OPERATION_MODE_ERROR;
        }

/*For PKCS1 Ver 2.1 P=L*/
/*--------------------------------------------------------*
 * Step 1: if the length of P is greater than the input   *
 *         limitation of the hash function (2^29 octets *
 *         for SHA-1) then output "decoding error"        *
 *          and stop.                                     *
 *--------------------------------------------------------*/
        if (PSize >= (1 << 29))
                return CC_RSA_BASE_OAEP_DECODE_PARAMETER_STRING_TOO_LONG;

/*-------------------------------------------------------*
 * Step 2: If ||EM|| < 2hLen+2, then output              *
 *        "decoding error" and stop.                     *
 *        Note: There EMSize = ||EM|| - 1                *
 *-------------------------------------------------------*/
        if (EMSize <  2*HashOutputSize + 1)
                return CC_RSA_BASE_OAEP_DECODE_MESSAGE_TOO_LONG;
/*-------------------------------------------------------*
 * Step 3: Let maskedSeed be the first hLen octets of EM *
 *         and let maskedDB be the remaining             *
 *         ||EM|| - hLen octets.                         *
 * PKCS1 Ver2.1: Step <3> <b>                            *
 *-------------------------------------------------------*/
        maskedDB_ptr = EM_ptr + HashOutputSize;

/*-------------------------------------------------------*
 * Step 4: Let seedMask = MGF(maskedDB, hLen).           *
 * PKCS1 Ver2.1: Step <3> <c>                            *
 *-------------------------------------------------------*/

        switch (MGF) {
        case CC_PKCS1_MGF1:

                Error = RsaOaepMGF1(
                                         HashOutputSize,                                                             /*hashLen*/
                                         maskedDB_ptr,                                                               /*mgfSeed - in*/
                                         (uint16_t)(EMSize - HashOutputSize),                                        /*seedLen*/
                                         HashOutputSize,                                                             /*maskLen*/
                                         &(((CCRsaOaepData_t*)((void*)PrimeData_ptr->InternalBuff))->SeedMask[0]),  /*mask - out*/
                                         hashFunc,                                                                   /*hashMode*/
                                         (uint8_t *)PrimeData_ptr->DataIn,                                           /*1-st tempBuff*/
                                         (uint8_t *)PrimeData_ptr->DataOut);                                         /*2-nd tempBuff*/
                if (Error != CC_OK) {
                        return Error;
                }
                break;

        /*Currently for PKCS1 Ver 2.1 only MGF1 is implemented*/
        case CC_PKCS1_NO_MGF:
        default:
                return CC_RSA_MGF_ILLEGAL_ARG_ERROR;

        }

/*-------------------------------------------------------*
 * Step 5: Let seed = maskedSeed xor seedMask.           *
 * PKCS1 Ver2.1: Step <3> <d>                            *
 *-------------------------------------------------------*/
        for (I = 0; I < HashOutputSize; I++)
                EM_ptr[I] ^= ((CCRsaOaepData_t*)((void*)PrimeData_ptr->InternalBuff))->SeedMask[I];

/*-------------------------------------------------------*
 * Step 6: Let dbMask = MGF(seed, ||EM|| - hLen).        *
 * PKCS1 Ver2.1: Step <3> <e>                            *
 *-------------------------------------------------------*/
        Error=RsaOaepMGF1(
                               HashOutputSize,
                               EM_ptr,
                               HashOutputSize,
                               EMSize - HashOutputSize,
                               &(((CCRsaOaepData_t*)((void*)PrimeData_ptr->InternalBuff))->MaskDB[0]),
                               hashFunc,
                               (uint8_t *)PrimeData_ptr->DataIn,
                               (uint8_t *)PrimeData_ptr->DataOut);

        if (Error!=CC_OK)
                return Error;

/*-------------------------------------------------------*
 * Step 7: Let DB = maskedDB xor dbMask.                 *
 *         PKCS1 Ver2.1: Step <3> <f>                    *
 *-------------------------------------------------------*/
        TmpSize = EMSize - HashOutputSize;
        TmpByte_ptr = &EM_ptr[0] + HashOutputSize;
        for (I = 0; I < TmpSize; I++)
                TmpByte_ptr[I] ^= ((CCRsaOaepData_t*)((void*)PrimeData_ptr->InternalBuff))->MaskDB[I];

/*-------------------------------------------------------*
 * Step 8: Let pHash = Hash(P), an octet string of       *
 *         length hLen.                                  *
 *                                                       *
 * PKCS1 Ver2.1: Step <3> <a>                            *
 *-------------------------------------------------------*/
        if (P_ptr!=NULL) {
#ifdef USE_MBEDTLS_CRYPTOCELL
                md_info = mbedtls_md_info_from_string( HashAlgMode2mbedtlsString[hashFunc] );
                if (NULL == md_info)
                {
                    return CC_RSA_HASH_ILLEGAL_OPERATION_MODE_ERROR;
                }
                Error = mbedtls_md(md_info,
                                   P_ptr,
                                   PSize,
                                   (unsigned char *)((CCRsaOaepData_t*)((void*)PrimeData_ptr->InternalBuff))->HashResultBuff);
#else
                Error = CC_Hash(
                                 hashFunc,
                                 P_ptr,
                                 PSize,
                                 ((CCRsaOaepData_t*)((void*)PrimeData_ptr->InternalBuff))->HashResultBuff);
#endif
                if (Error!=CC_OK) {
                        return Error;
                }
        } else {
#ifdef USE_MBEDTLS_CRYPTOCELL
                md_ctx = &(((CCRsaOaepData_t*)((void*)PrimeData_ptr->InternalBuff))->hash_ctx);
                md_info = mbedtls_md_info_from_string( HashAlgMode2mbedtlsString[hashFunc] );
                if (NULL == md_info) {
                        return CC_RSA_HASH_ILLEGAL_OPERATION_MODE_ERROR;
                }
                mbedtls_md_init(md_ctx);
                Error = mbedtls_md_setup(md_ctx, md_info, 0); // 0 = HASH, not HMAC
                if (Error != 0) {
                        goto End;
                }
                Error = mbedtls_md_starts(md_ctx);
#else
                Error=CC_HashInit(&(((CCRsaOaepData_t*)((void*)PrimeData_ptr->InternalBuff))->HashUsercontext),
                                  hashFunc);
#endif
                if (Error!=CC_OK) {
                        goto End;
                }
#ifdef USE_MBEDTLS_CRYPTOCELL
                Error = mbedtls_md_finish(md_ctx, (unsigned char *)((CCRsaOaepData_t*)((void*)PrimeData_ptr->InternalBuff))->HashResultBuff);
#else
                Error=CC_HashFinish( &(((CCRsaOaepData_t*)((void*)PrimeData_ptr->InternalBuff))->HashUsercontext),
                                        ((CCRsaOaepData_t*)((void*)PrimeData_ptr->InternalBuff))->HashResultBuff);
#endif
                if (Error!=CC_OK) {
                        goto End;
                }
        }


/*---------------------------------------------------------*
 * Step 9: Separate DB into an octet string pHash'         *
 *         consisting of the first hLen octets of DB,      *
 *         a (possibly empty) octet string PS consisting   *
 *         of consecutive zero octets following pHash',    *
 *         and a message M as DB = pHash' || PS || 01 || M *
 *         If there is no 01 octet to separate PS from M,  *
 *         output "decoding error" and stop.               *
 *                                                         *
 * PKCS1 Ver2.1: Step <3> <g>                              *
 *---------------------------------------------------------*/
        TmpSize = EMSize - 2*HashOutputSize;
        TmpByte_ptr = &EM_ptr[0] + 2*HashOutputSize;
        for (I = 0; I < TmpSize; I++) {
                if (TmpByte_ptr[I] != 0x00)
                        break;
        }

        if (TmpByte_ptr[I] != 0x01){
                Error = CC_RSA_OAEP_DECODE_ERROR;
                goto End;
        }

        TmpByte_ptr += I;

/*------------------------------------------------------*
 * Step 10: If pHash' does not equal pHash, output      *
 *          "decoding error" and stop.                  *
 *                                                      *
 * PKCS1 Ver2.1: Step <3> <g>                           *
 *------------------------------------------------------*/
        if (CC_PalMemCmp(&EM_ptr[0] + HashOutputSize,
                        (uint8_t *)(((CCRsaOaepData_t*)((void*)PrimeData_ptr->InternalBuff))->HashResultBuff),
                        HashOutputSize)) {
                Error = CC_RSA_OAEP_DECODE_ERROR;
                goto End;
        }

/*-----------------------------------------------*
 * Step 11: Output M.                            *
 *-----------------------------------------------*/
        /*Checking that the Output buffer Size is enough large for result output */
        if (*MSize_ptr < (uint32_t)(EMSize - 2*HashOutputSize - I - 1)) {
                Error = CC_RSA_DECRYPT_INVALID_OUTPUT_SIZE;
                goto End;
        }

        else if (*MSize_ptr > (uint32_t)(EMSize - 2*HashOutputSize - I - 1)) {
                /* set the actual output message size */
                *MSize_ptr = EMSize - 2*HashOutputSize - I - 1;
        }

        /* at this point TmpByte_ptr points to 01 */
        TmpByte_ptr += 1;

        /* at this point TmpByte_ptr points to M  */
        CC_PalMemCopy( M_ptr, TmpByte_ptr, *MSize_ptr );

        End:
#ifdef USE_MBEDTLS_CRYPTOCELL
        if((md_info!=NULL) && (md_ctx!=NULL)) {
                mbedtls_md_free(md_ctx);
        }
#endif

        /*Clear Internal buffers*/
        CC_PalMemSetZero((PrimeData_ptr->InternalBuff), sizeof(PrimeData_ptr->InternalBuff));

        return Error;

}
#endif /*!defined(CC_NO_RSA_DECRYPT_SUPPORT) && !defined(_INTERNAL_CC_NO_RSA_SIGN_SUPPORT)*/


/**********************************************************************************************************/

/* -------------------------------------------------------------
 *  Function Name: RsaOaepMGF1
 *  Date:   09-1-2005
 *  Author: Ohad Shperling
 *
 *  Inputs:
 *  Outputs:
 *
 *  Algorithm: according to PKCS1-v.2_1
 *
 *  Update History:
 *  Date:       Description:
 * ----------------------------------------------------------- */
CCError_t RsaOaepMGF1( uint16_t hLen,                  /*hashLen*/
                               uint8_t *Z_ptr,                 /*mgfSeed*/
                               uint16_t ZSize,                 /*seedLen*/
                               uint32_t L,                     /*maskLen*/
                               uint8_t  *Mask_ptr,             /*mask*/
                               CCPkcs1HashFunc_t hashFunc, /*hashMode*/
                               uint8_t  *T_Buf,                /*1-st tempBuff*/
                               uint8_t  *T_TMP_Buf)            /*2-nd tempBuff*/
{

        /* FUNCTION DECLARATIONS */

        CCError_t Error;
        uint32_t Counter = 0;
        uint32_t CounterMaxSize, Tmp32Bit;
        CCHashResultBuf_t  HashResultBuff;
        uint8_t *Output_ptr;
        uint8_t *T = T_Buf;/*The size of T for MGF1 used to be 2048/8 now the size of T_Buf in the context is even bigger*/
        uint8_t *T_TMP = T_TMP_Buf;
#ifdef USE_MBEDTLS_CRYPTOCELL
        const mbedtls_md_info_t *md_info=NULL;
#endif


/*---------------------------------------------------------------------*
 * Step 1:  If l > 2^32 hLen, output "mask too long" and stop.          *
 *---------------------------------------------------------------------*/

        /* note: check L > (uint32_t)(CC_RSA_MGF_2_POWER_32 *hLen) not needed
                 because next check is more stronger */

        /* limit the size to the temp buffer size that is the maximum key length */
        if (L > CC_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS * sizeof(uint32_t))
                return CC_RSA_BASE_MGF_MASK_TOO_LONG ;

/*---------------------------------------------------------------------*
 * Step 2:  Let T  be the empty octet string.                          *
 *---------------------------------------------------------------------*/

/*---------------------------------------------------------------------*
 * Step 3:  For counter from 0 to  | l / hLen | -1 , do the following: *
 *          a.  Convert counter to an octet string C of length 4       *
 *              with the primitive I2OSP:                              *
 *               C = I2OSP (counter, 4)                                *
 *          b.  Concatenate the hash of the seed Z and C to the octet  *
 *               string T:   T = T || Hash (Z || C)                    *
 *---------------------------------------------------------------------*/
        Output_ptr = T;
        CC_PalMemCopy(T_TMP, Z_ptr/*seed*/, ZSize);

        /* count of Hash blocks needed for mask calculation */
        CounterMaxSize = (uint32_t)((L + hLen - 1)/hLen);

        for (Counter = 0; Counter < CounterMaxSize; Counter++) {

                /*--------------------------------------------------------------------
                 *          a.  Convert counter to an octet string C of length 4
                 *              with the primitive I2OSP:   C = I2OSP (counter, 4)
                 *--------------------------------------------------------------------*/

                /* T_TMP = H||C */

#ifndef BIG__ENDIAN
                Tmp32Bit = CC_COMMON_REVERSE32(Counter);
#else
                Tmp32Bit = Counter;
#endif

                CC_PalMemCopy(&T_TMP[0] + ZSize, &Tmp32Bit, sizeof(uint32_t));

                /*--------------------------------------------------------------------
                 *          b.  Concatenate the hash of the seed Z and C to the octet
                 *               string T: T = T || Hash (Z || C)
                 *--------------------------------------------------------------------*/

                /* Hash Z||C */
#ifdef USE_MBEDTLS_CRYPTOCELL
                md_info = mbedtls_md_info_from_string( HashAlgMode2mbedtlsString[hashFunc] );
                if (NULL == md_info)
                {
                    return CC_RSA_HASH_ILLEGAL_OPERATION_MODE_ERROR;
                }
                Error = mbedtls_md(md_info,
                        T_TMP,
                        ZSize + sizeof(uint32_t)/*counterSize*/,
                        (unsigned char *)HashResultBuff);
                if (0 != Error)
                {
                        return CC_RSA_HASH_ILLEGAL_OPERATION_MODE_ERROR;
                }
#else
                Error = CC_Hash(hashFunc,
                                T_TMP,
                                ZSize + sizeof(uint32_t)/*counterSize*/,
                                HashResultBuff);

                if (Error!=CC_OK)
                        return Error;
#endif
                CC_PalMemCopy(Output_ptr, (uint8_t *)HashResultBuff, hLen);
                Output_ptr += hLen;
        }

/*---------------------------------------------------------------------*
 * Step 4:  Output the leading L octets of T as the octet string mask. *
 *---------------------------------------------------------------------*/
        CC_PalMemCopy(Mask_ptr,(uint8_t *)T, L);

        return CC_OK;
}
#endif /* !defined(CC_IOT) || ( defined(CC_IOT) && defined(MBEDTLS_RSA_C)) */
