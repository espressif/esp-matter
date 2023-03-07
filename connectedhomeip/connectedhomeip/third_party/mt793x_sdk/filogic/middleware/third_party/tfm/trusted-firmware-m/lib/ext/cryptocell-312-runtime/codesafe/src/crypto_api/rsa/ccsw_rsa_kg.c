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
#include "cc_rnd_local.h"
#include "cc_rsa_error.h"
#include "cc_rsa_local.h"
#include "sw_llf_pki_rsa.h"
#include "ccsw_cc_rsa_types.h"


/************************ Defines ******************************/

/************************ Enums ************************************/

/************************ Typedefs *********************************/

/************************ Global Data ******************************/

/*
 For debugging the RSA_KG module define the following flags in project properties
 and perform the following:
   1. Compile project in DEBUG=1 mode.
   2. Define LLF_PKI_PKA_DEBUG.
   3. For findingthe bad random factors (P,Q,P1pR,P2pR,P1qR,P2qR):
      define RSA_KG_FIND_BAD_RND flag, perform test and save (from memory)
      the found bad vectors.
   4. For repeat the testing of found bad vectors, write they as HW
      initialization of the following buffers:
      P=>RSA_KG_debugPvect, Q=>RSA_KG_debugQvect - in the cc_rsa_kg.c
      file, and P1pR=>rBuff1, P2pR=>rBuff1, P1qR=>rBuff3, P2qR=>rBuff4 in the
      LLF_PKI_GenKeyX931FindPrime.c file. Define the flag RSA_KG_NO_RND instead
      previously defined RSA_KG_FIND_BAD_RND flag and perform the test.
   5. For ordinary ATP or other tests (without debug) undef all the named flags.
*/

#if ((defined RSA_KG_FIND_BAD_RND || defined RSA_KG_NO_RND) && defined DEBUG)
uint8_t   RSA_KG_debugPvect[CC_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_BYTES] =
{ 0x78,0x71,0xDF,0xC5,0x36,0x98,0x12,0x21,0xCA,0xAC,0x48,0x22,0x01,0x94,0xF7,0x1A,
    0x1C,0xBF,0x82,0xE9,0x8A,0xE4,0x2C,0x84,0x43,0x46,0xCF,0x6D,0x60,0xFB,0x5B,0xD3};
uint8_t   RSA_KG_debugQvect[CC_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_BYTES] =
{ 0x46,0x13,0x9F,0xBA,0xBC,0x8E,0x21,0x13,0x35,0x8C,0x2C,0x2D,0xA8,0xD6,0x59,0x78,
    0x8A,0x14,0x17,0x5F,0xA5,0xEC,0x22,0xD5,0x87,0xF9,0x99,0x45,0x1B,0x38,0xA3,0xF0};
#endif


/************* Private function prototype **************************/


/************************ Public Functions ******************************/


/***********************************************************************************************/
#ifndef _INTERNAL_CC_NO_RSA_KG_SUPPORT
/**
   @brief CC_RsaKgKeyPairGenerate generates a Pair of public and private keys on non CRT mode.

   @param [in/out] rndContext_ptr  - Pointer to the RND context buffer.
   @param [in] PubExp_ptr - The pointer to the public exponent (public key)
   @param [in] PubExpSizeInBytes - The public exponent size in bytes.
   @param [in] KeySize  - The size of the key, in bits. Supported sizes are:
                - for PKI without PKA HW: all 256 bit multiples between 512 - 2048;
                - for PKI with PKA: HW all 32 bit multiples between 512 - 2112;
   @param [out] UserPrivKey_ptr - A pointer to the private key structure.
               This structure is used as input to the CC_RsaPrimDecrypt API.
   @param [out] UserPubKey_ptr - A pointer to the public key structure.
               This structure is used as input to the CC_RsaPrimEncrypt API.
   @param [in] KeyGenData_ptr - a pointer to a structure required for the KeyGen
      operation.
 * @param rndCtx_ptr - The pointer to structure, containing context ID and void
 *              pointer to RND State structure, which should be converted to
 *              actual type inside of the function according to used platform
 *                  (External or CC). also containig the generate random vector pointer

   @return CCError_t - CC_OK,
             CC_RSA_INVALID_EXPONENT_POINTER_ERROR,
             CC_RSA_INVALID_PRIV_KEY_STRUCT_POINTER_ERROR,
             CC_RSA_INVALID_PUB_KEY_STRUCT_POINTER_ERROR,
             CC_RSA_KEY_GEN_DATA_STRUCT_POINTER_INVALID,
             CC_RSA_INVALID_MODULUS_SIZE,
             CC_RSA_INVALID_EXPONENT_SIZE
*/
CEXPORT_C CCError_t CC_SwRsaKgGenerateKeyPair(CCRndContext_t *rndContext_ptr,
                            uint8_t             *PubExp_ptr,
                            uint16_t             PubExpSizeInBytes,
                            uint32_t             KeySize,
                            CCSwRsaUserPrivKey_t *UserPrivKey_ptr,
                            CCSwRsaUserPubKey_t  *UserPubKey_ptr,
                            CCSwRsaKgData_t      *KeyGenData_ptr)
{
    /* LOCAL INITIALIZATIONS AND DECLERATIONS */

    /* the error identifier */
    CCError_t Error;

    /* the pointers to the key structures */
    SwRsaPubKey_t  *PubKey_ptr;
    SwRsaPrivKey_t *PrivKey_ptr;

    /* a temp definition to solve a problem on release mode on VC++ */
    volatile uint32_t dummy = PubExpSizeInBytes;

    uint32_t  KeySizeInWords = KeySize/32;


    /* FUNCTION LOGIC */
    /* ................ initializaions and local declarations ............ */
    /* ------------------------------------------------------------------- */

    /* to avoid compilers warnings */
    dummy = dummy;

    /* initialize the error identifier to O.K */
    Error = CC_OK;


    /* ................. checking the validity of the pointer arguments ....... */
    /* ------------------------------------------------------------------------ */

    /* ...... checking the key database handle pointer .................... */
    if (PubExp_ptr == NULL)
        return CC_RSA_INVALID_EXPONENT_POINTER_ERROR;

    /* ...... checking the validity of the exponent pointer ............... */
    if (UserPrivKey_ptr == NULL)
        return CC_RSA_INVALID_PRIV_KEY_STRUCT_POINTER_ERROR;

    /* ...... checking the validity of the modulus pointer .............. */
    if (UserPubKey_ptr == NULL)
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
    PubKey_ptr  = ( SwRsaPubKey_t *)UserPubKey_ptr->PublicKeyDbBuff;
    PrivKey_ptr = ( SwRsaPrivKey_t *)UserPrivKey_ptr->PrivateKeyDbBuff;

    CC_PalMemSet( UserPrivKey_ptr, 0, sizeof(CCSwRsaUserPrivKey_t) );
    CC_PalMemSet( UserPubKey_ptr, 0, sizeof(CCSwRsaUserPubKey_t) );
    CC_PalMemSet( KeyGenData_ptr , 0, sizeof(CCSwRsaKgData_t) );

    /* ................ loading the public exponent to the structure .......... */
    /* ------------------------------------------------------------------------- */
    /* loading the buffers to start from LS word to MS word */
    CC_CommonReverseMemcpy( (uint8_t*)PubKey_ptr->e , PubExp_ptr , PubExpSizeInBytes );
    /* .......... initializing the effective counters size in bits .......... */
    PubKey_ptr->eSizeInBits = CC_CommonGetBytesCounterEffectiveSizeInBits( (uint8_t*)PubKey_ptr->e,PubExpSizeInBytes );

    /* if the size in bits is 0 - return error */
    if (PubKey_ptr->eSizeInBits == 0) {
        Error = CC_RSA_INVALID_EXPONENT_SIZE;
        goto End;
    }

    /* verifying the exponent has legal value (currently only 0x3,0x11 and 0x10001) */
    if (PubKey_ptr->e[0] != 0x3  &&
        PubKey_ptr->e[0] != 0x11 &&
        PubKey_ptr->e[0] != 0x010001) {
        Error = CC_RSA_INVALID_EXPONENT_VAL;
        goto End;
    }
    /* .......... initialize the public key on the private structure ............... */
    CC_PalMemCopy( PrivKey_ptr->PriveKeyDb.NonCrt.e , PubKey_ptr->e , 4*((PubExpSizeInBytes+3)/4) );
    PrivKey_ptr->PriveKeyDb.NonCrt.eSizeInBits = PubKey_ptr->eSizeInBits;

    /* .......... initializing the key size in bits ......................... */

    /* this initialization is required for the low level function (LLF) - indicates the required
       size of the key to be found */
    PubKey_ptr->nSizeInBits  = KeySize;
    PrivKey_ptr->nSizeInBits = KeySize;

    /* .......... set the private mode to non CRT .............................. */
    /* ------------------------------------------------------------------------- */

    /* set the mode to non CRT */
    PrivKey_ptr->OperationMode = CC_RSA_NoCrt;

    /* set the key source as internal */
    PrivKey_ptr->KeySource = CC_RSA_InternalKey;

    /* ................ executing the key generation ........................... */
    /* ------------------------------------------------------------------------- */
    /* generate the random */

#if ( (!defined RSA_KG_FIND_BAD_RND && !defined RSA_KG_NO_RND) || defined RSA_KG_FIND_BAD_RND || !defined DEBUG)
    Error = CC_RsaGenerateVectorInRangeX931(rndContext_ptr, KeySizeInWords / 2, KeyGenData_ptr->KGData.p);

    if (Error != CC_OK)
        goto End;

    Error = CC_RsaGenerateVectorInRangeX931(rndContext_ptr, KeySizeInWords / 2, KeyGenData_ptr->KGData.q);

    if (Error != CC_OK)
        goto End;
#endif

#if (defined RSA_KG_FIND_BAD_RND && defined DEBUG)
    CC_PalMemCopy( RSA_KG_debugPvect, (uint8_t*)KeyGenData_ptr->KGData.p, KeySizeInBytes / 2 );
    CC_PalMemCopy( RSA_KG_debugQvect, (uint8_t*)KeyGenData_ptr->KGData.q, KeySizeInBytes / 2);
#endif

#if (defined RSA_KG_NO_RND  && defined DEBUG)
    CC_PalMemCopy( (uint8_t*)KeyGenData_ptr->KGData.p, RSA_KG_debugPvect, KeySizeInBytes / 2 );
    CC_PalMemCopy( (uint8_t*)KeyGenData_ptr->KGData.q, RSA_KG_debugQvect, KeySizeInBytes / 2 );
#endif

#if ((defined RSA_KG_FIND_BAD_RND || defined RSA_KG_NO_RND) && defined DEBUG)
  #ifdef BIG__ENDIAN
    /* for big endiannes machine reverse bytes order in words according to Big Endian  */
    CC_COMMON_INVERSE_UINT32_IN_ARRAY( KeyGenData_ptr->KGData.p, KeySizeInWords / 2);
    CC_COMMON_INVERSE_UINT32_IN_ARRAY( KeyGenData_ptr->KGData.q, KeySizeInWords / 2 );
  #endif
#endif
    /* clean the n-buffer */
    CC_PalMemSetZero( PrivKey_ptr->n, 4*CC_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS );

    /* ................ execute the low level keygen ........................... */
    Error = SW_LLF_PKI_RSA_GenerateKeyPair( PubKey_ptr,
                        PrivKey_ptr,
                        KeyGenData_ptr);

    /* on failure exit the function */
    if (Error != CC_OK)
        goto End;

    /* ................ initialize the low level key structures ................ */
    /* ------------------------------------------------------------------------- */
    Error = SW_LLF_PKI_RSA_InitPubKeyDb(PubKey_ptr);

    if (Error != CC_OK)
        goto End;

    Error = SW_LLF_PKI_RSA_InitPrivKeyDb(PrivKey_ptr);

    if (Error != CC_OK)
        goto End;

    /* ................ set the key valid tags ................................. */
    /* ------------------------------------------------------------------------- */
    UserPrivKey_ptr->valid_tag = CC_RSA_PRIV_KEY_VALIDATION_TAG;
    UserPubKey_ptr->valid_tag  = CC_RSA_PUB_KEY_VALIDATION_TAG;



End:
        /* clear the KG data structure and keys */
    CC_PalMemSetZero(KeyGenData_ptr, sizeof(CCSwRsaKgData_t));
        /*............  end on error  .............*/
        if(Error) {
                CC_PalMemSetZero(PubKey_ptr, sizeof(CCSwRsaUserPubKey_t));
                CC_PalMemSetZero(PrivKey_ptr, sizeof(CCSwRsaUserPrivKey_t));
        }

        return Error;

}/* END OF CC_SwRsaKgGenerateKeyPair */


/***********************************************************************************************/
/**
   @brief CC_SwRsaKgGenerateKeyPairCRT generates a Pair of public and private keys on CRT mode.

   @param [in/out] rndContext_ptr  - Pointer to the RND context buffer.
   @param [in] PubExp_ptr - The pointer to the public exponent (public key)
   @param [in] PubExpSizeInBytes - The public exponent size in bits.
   @param [in] KeySize  - The size of the key, in bits. Supported sizes are:
                - for PKI without PKA HW: all 256 bit multiples between 512 - 2048;
                - for PKI with PKA: HW all 32 bit multiples between 512 - 2112;
   @param [out] UserPrivKey_ptr - A pointer to the private key structure.
               This structure is used as input to the CC_RsaPrimDecrypt API.
   @param [out] UserPubKey_ptr - A pointer to the public key structure.
               This structure is used as input to the CC_RsaPrimEncryped API.
   @param [in] KeyGenData_ptr - a pointer to a structure required for the KeyGen operation.
 * @param [in/out] RndCtx_ptr - The pointer to structure, containing context ID and void
 *              pointer to RND State structure, which should be converted to
 *              actual type inside of the function according to used platform
 *                  (External or CC).

   @return CCError_t - CC_OK,
             CC_RSA_INVALID_EXPONENT_POINTER_ERROR,
             CC_RSA_INVALID_PRIV_KEY_STRUCT_POINTER_ERROR,
             CC_RSA_INVALID_PUB_KEY_STRUCT_POINTER_ERROR,
             CC_RSA_KEY_GEN_DATA_STRUCT_POINTER_INVALID,
             CC_RSA_INVALID_MODULUS_SIZE,
             CC_RSA_INVALID_EXPONENT_SIZE
*/

CEXPORT_C CCError_t CC_SwRsaKgGenerateKeyPairCRT(CCRndContext_t *rndContext_ptr,
                               uint8_t  *PubExp_ptr,
                               uint16_t  PubExpSizeInBytes,
                               uint32_t  KeySize,
                               CCSwRsaUserPrivKey_t *UserPrivKey_ptr,
                               CCSwRsaUserPubKey_t  *UserPubKey_ptr,
                               CCSwRsaKgData_t      *KeyGenData_ptr)
{
    /* LOCAL INITIALIZATIONS AND DECLERATIONS */

    /* the error identifier */
    CCError_t Error = CC_OK;

    /* the pointers to the key structures */
    SwRsaPubKey_t  *PubKey_ptr;
    SwRsaPrivKey_t *PrivKey_ptr;

    /* FUNCTION LOGIC */
    uint32_t  KeySizeInBytes = KeySize/8;
    uint32_t  KeySizeInWords = KeySize/32;

    /* ................ initializations and local declarations ............ */
    /* ------------------------------------------------------------------- */



    /* ................. checking the validity of the pointer arguments ....... */
    /* ------------------------------------------------------------------------ */
    /* ...... checking the key database handle pointer .................... */
    if (PubExp_ptr == NULL)
        return CC_RSA_INVALID_EXPONENT_POINTER_ERROR;

    /* ...... checking the validity of the exponent pointer ............... */
    if (UserPrivKey_ptr == NULL)
        return CC_RSA_INVALID_PRIV_KEY_STRUCT_POINTER_ERROR;

    /* ...... checking the validity of the modulus pointer .............. */
    if (UserPubKey_ptr == NULL)
        return CC_RSA_INVALID_PUB_KEY_STRUCT_POINTER_ERROR;

    /* ...... checking the validity of the keygen data .................. */
    if (KeyGenData_ptr == NULL)
        return CC_RSA_KEY_GEN_DATA_STRUCT_POINTER_INVALID;

    /* ...... checking the required key size ............................ */
    if (( KeySize < CC_RSA_MIN_VALID_KEY_SIZE_VALUE_IN_BITS ) ||
        ( KeySize > CC_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_BITS ) ||
        ( KeySize % CC_RSA_VALID_KEY_SIZE_MULTIPLE_VALUE_IN_BITS ))
        return CC_RSA_INVALID_MODULUS_SIZE;

    if (PubExpSizeInBytes > CC_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_BYTES)
        return CC_RSA_INVALID_EXPONENT_SIZE;


    /* set the public and private key structure pointers */
    PubKey_ptr  = ( SwRsaPubKey_t *)UserPubKey_ptr->PublicKeyDbBuff;
    PrivKey_ptr = ( SwRsaPrivKey_t *)UserPrivKey_ptr->PrivateKeyDbBuff;


    /* ................ clear all input structures ............................. */
    /* ------------------------------------------------------------------------- */
    /* RL Clean buffers  */
    CC_PalMemSet( UserPrivKey_ptr , 0 , sizeof(CCSwRsaUserPrivKey_t) );
    CC_PalMemSet( UserPubKey_ptr  , 0 , sizeof(CCSwRsaUserPubKey_t) );
    CC_PalMemSet( KeyGenData_ptr  , 0 , sizeof(CCSwRsaKgData_t) );

    /* ................ loading the public exponent to the structure .......... */
    /* ------------------------------------------------------------------------- */

    /* loading the buffers to start from LS word to MS word */
    CC_CommonReverseMemcpy( (uint8_t*)PubKey_ptr->e , PubExp_ptr , PubExpSizeInBytes );

    /* .......... initializing the effective counters size in bits .......... */
    PubKey_ptr->eSizeInBits = CC_CommonGetBytesCounterEffectiveSizeInBits( (uint8_t*)PubKey_ptr->e, PubExpSizeInBytes );

    /* if the size in bits is 0 - return error */
    if (PubKey_ptr->eSizeInBits == 0) {
        Error = CC_RSA_INVALID_EXPONENT_SIZE;
        goto End;
    }

    /* verifing the exponent has legal value (currently only 0x3,0x11 and 0x10001) */
        if(PubKey_ptr->e[0] != 0x3   &&
           PubKey_ptr->e[0] != 0x11  &&
           PubKey_ptr->e[0] != 0x010001){
            Error = CC_RSA_INVALID_EXPONENT_VAL;
            goto End;
    }


    /* .......... initializing the key size in bits ......................... */

    /* this initialization is required for the low level function (LLF) - indicates the required
       size of the key to be found */
    PubKey_ptr->nSizeInBits  = KeySize;
    PrivKey_ptr->nSizeInBits = KeySize;

    /* .......... set the private mode to CRT .................................. */
    /* ------------------------------------------------------------------------- */

    /* set the mode to CRT */
    PrivKey_ptr->OperationMode = CC_RSA_Crt;

    /* set the key source as internal */
    PrivKey_ptr->KeySource = CC_RSA_InternalKey;

    /* ................ executing the key generation ........................... */
    /* ------------------------------------------------------------------------- */

    /* ................ generate the prime1 and prime2 random numbers .......... */

    /* generate the random */
    Error = CC_RsaGenerateVectorInRangeX931(rndContext_ptr, KeySizeInWords / 2, KeyGenData_ptr->KGData.p);

    if (Error != CC_OK)
        goto End;

    Error = CC_RsaGenerateVectorInRangeX931(rndContext_ptr, KeySizeInWords / 2, KeyGenData_ptr->KGData.q);

    if (Error != CC_OK)
        goto End;

  #ifdef BIG__ENDIAN
    /* for big endianness machine reverse bytes order according to Big Endian words */
    CC_COMMON_INVERSE_UINT32_IN_ARRAY( KeyGenData_ptr->KGData.p, KeySizeInWords /2 );
    CC_COMMON_INVERSE_UINT32_IN_ARRAY( KeyGenData_ptr->KGData.q, KeySizeInWords /2 );
  #endif

    /* clean the n-buffer */
    CC_PalMemSetZero( PrivKey_ptr->n, 4*CC_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS );

    /* ................ execute the low level key gen ........................... */
    Error = SW_LLF_PKI_RSA_GenerateKeyPair( PubKey_ptr,
                        PrivKey_ptr,
                        KeyGenData_ptr );

    /* on failure exit the function */
    if (Error != CC_OK)
        goto End;

    /* ................ set the vector sizes ................................... */
    /* ------------------------------------------------------------------------- */

    PrivKey_ptr->PriveKeyDb.Crt.PSizeInBits =
    CC_CommonGetBytesCounterEffectiveSizeInBits((uint8_t*)PrivKey_ptr->PriveKeyDb.Crt.P, KeySizeInBytes / 2);

    PrivKey_ptr->PriveKeyDb.Crt.QSizeInBits =
    CC_CommonGetBytesCounterEffectiveSizeInBits((uint8_t*)PrivKey_ptr->PriveKeyDb.Crt.Q, KeySizeInBytes / 2);

    PrivKey_ptr->PriveKeyDb.Crt.dPSizeInBits =
    CC_CommonGetBytesCounterEffectiveSizeInBits((uint8_t*)PrivKey_ptr->PriveKeyDb.Crt.dP, KeySizeInBytes / 2);

    PrivKey_ptr->PriveKeyDb.Crt.dQSizeInBits =
    CC_CommonGetBytesCounterEffectiveSizeInBits((uint8_t*)PrivKey_ptr->PriveKeyDb.Crt.dQ, KeySizeInBytes / 2);

    PrivKey_ptr->PriveKeyDb.Crt.qInvSizeInBits =
    CC_CommonGetBytesCounterEffectiveSizeInBits((uint8_t*)PrivKey_ptr->PriveKeyDb.Crt.qInv, KeySizeInBytes / 2);

    /* ................ initialize the low level key structures ................ */
    /* ------------------------------------------------------------------------- */

    Error = SW_LLF_PKI_RSA_InitPubKeyDb( PubKey_ptr );

    if (Error != CC_OK)
        goto End;

    Error = SW_LLF_PKI_RSA_InitPrivKeyDb( PrivKey_ptr );

    if (Error != CC_OK)
        goto End;

    /* ................ set the key valid tags ................................. */
    /* ------------------------------------------------------------------------- */

    UserPrivKey_ptr->valid_tag = CC_RSA_PRIV_KEY_VALIDATION_TAG;
    UserPubKey_ptr->valid_tag  = CC_RSA_PUB_KEY_VALIDATION_TAG;

    End:

    /* clear the KG data structure */
    CC_PalMemSetZero ( KeyGenData_ptr, sizeof(CCSwRsaKgData_t));

        /* on error clear the keys */
        if (Error != CC_OK) {
                CC_PalMemSetZero(UserPrivKey_ptr, sizeof(CCRsaUserPrivKey_t));
                CC_PalMemSetZero(UserPubKey_ptr, sizeof(CCRsaUserPubKey_t));
        }

    return Error;

}/* END OF CC_RsaKgKeyPairCrtGenerate */

#endif /*_INTERNAL_CC_NO_RSA_KG_SUPPORT*/
