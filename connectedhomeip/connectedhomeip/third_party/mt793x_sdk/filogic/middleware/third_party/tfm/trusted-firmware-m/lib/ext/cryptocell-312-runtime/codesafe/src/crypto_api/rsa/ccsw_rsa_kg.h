/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef CCSW_RSA_KG_H
#define CCSW_RSA_KG_H


#include "cc_pal_types.h"
#include "ccsw_cc_rsa_types.h"

#ifdef __cplusplus
extern "C"
{
#endif


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
 *                  (External or CC). Also contains the RND generate vecotr function pointer.

   @return CCError_t - CC_OK,
                         CC_RSA_INVALID_EXPONENT_POINTER_ERROR,
                         CC_RSA_INVALID_PRIV_KEY_STRUCT_POINTER_ERROR,
                         CC_RSA_INVALID_PUB_KEY_STRUCT_POINTER_ERROR,
                         CC_RSA_KEY_GEN_DATA_STRUCT_POINTER_INVALID,
                         CC_RSA_INVALID_MODULUS_SIZE,
                         CC_RSA_INVALID_EXPONENT_SIZE
*/
CEXPORT_C CCError_t CC_SwRsaKgGenerateKeyPair(
            CCRndContext_t *rndContext_ptr,
            uint8_t             *PubExp_ptr,
            uint16_t             PubExpSizeInBytes,
            uint32_t             KeySize,
            CCSwRsaUserPrivKey_t *UserPrivKey_ptr,
            CCSwRsaUserPubKey_t  *UserPubKey_ptr,
            CCSwRsaKgData_t      *KeyGenData_ptr);


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
 *                  (External or CC). Also contains the RND generate vecotr function pointer.

   @return CCError_t - CC_OK,
                         CC_RSA_INVALID_EXPONENT_POINTER_ERROR,
                         CC_RSA_INVALID_PRIV_KEY_STRUCT_POINTER_ERROR,
                         CC_RSA_INVALID_PUB_KEY_STRUCT_POINTER_ERROR,
                         CC_RSA_KEY_GEN_DATA_STRUCT_POINTER_INVALID,
                         CC_RSA_INVALID_MODULUS_SIZE,
                         CC_RSA_INVALID_EXPONENT_SIZE
*/

CEXPORT_C CCError_t CC_SwRsaKgGenerateKeyPairCRT(
                CCRndContext_t *rndContext_ptr,
                uint8_t             *PubExp_ptr,
                uint16_t             PubExpSizeInBytes,
                uint32_t             KeySize,
                CCSwRsaUserPrivKey_t *UserPrivKey_ptr,
                CCSwRsaUserPubKey_t  *UserPubKey_ptr,
                CCSwRsaKgData_t      *KeyGenData_ptr);

#ifdef __cplusplus
}
#endif

#endif


