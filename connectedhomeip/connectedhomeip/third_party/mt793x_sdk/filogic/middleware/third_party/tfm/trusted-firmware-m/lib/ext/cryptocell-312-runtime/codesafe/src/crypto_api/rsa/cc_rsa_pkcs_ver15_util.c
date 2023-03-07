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

#define CC_PAL_LOG_CUR_COMPONENT CC_LOG_MASK_ASYM_RSA_DH

/************* Include Files ****************/
#include "cc_pal_mem.h"
#include "cc_common_math.h"
#include "cc_rsa_local.h"
#include "cc_rsa_error.h"
#include "cc_rnd_error.h"

/************************ Defines ******************************/

/************************ Enums ******************************/

/************************ Typedefs ***************************/

/************************ Public Functions *******************/


#if !defined(_INTERNAL_CC_NO_RSA_SCHEME_15_SUPPORT)

/************************ Global Data ******************************/
/* DER(BER) encoded data for allowed HASH algorithms */
/*typedef struct HashDerCode_t {
        uint32_t algIdSizeBytes;
        CCHashOperationMode_t hashMode;
        uint8_t algId[HASH_DER_CODE_MAX_SIZE_BYTES];
}*/
/*   Note: order of algorithms in array must be according to HASH mode ID */
static const HashDerCode_t gHashDerCodes[] = {
        {15, CC_HASH_SHA1_mode,  {0x30,0x21,0x30,0x09,0x06,0x05,0x2b,0x0e,0x03,0x02,0x1a,0x05,0x00,0x04,0x14}},                     /*SHA1*/
        {19, CC_HASH_SHA224_mode,{0x30,0x2D,0x30,0x0D,0x06,0x09,0x60,0x86,0x48,0x01,0x65,0x03,0x04,0x02,0x04,0x05,0x00,0x04,0x1C}}, /*SHA224*/
        {19, CC_HASH_SHA256_mode,{0x30,0x31,0x30,0x0D,0x06,0x09,0x60,0x86,0x48,0x01,0x65,0x03,0x04,0x02,0x01,0x05,0x00,0x04,0x20}}, /*SHA256*/
        {19, CC_HASH_SHA384_mode,{0x30,0x41,0x30,0x0D,0x06,0x09,0x60,0x86,0x48,0x01,0x65,0x03,0x04,0x02,0x02,0x05,0x00,0x04,0x30}}, /*SHA384*/
        {19, CC_HASH_SHA512_mode,{0x30,0x51,0x30,0x0D,0x06,0x09,0x60,0x86,0x48,0x01,0x65,0x03,0x04,0x02,0x03,0x05,0x00,0x04,0x40}}, /*SHA512*/
        {18, CC_HASH_MD5_mode,   {0x30,0x20,0x30,0x0C,0x06,0x08,0x2A,0x86,0x48,0x86,0xf7,0x0d,0x02,0x05,0x05,0x00,0x04,0x10}},      /*MD5*/
};

/*************************** Private functions **********************/

/**
 * The function gets DER code of choosen Hash algoritm.
 *
 * @author reuvenl (9/11/2014)
 *
 * @param hashMode - Hash mode enumeration.
 * @param pAlgId - The pointer to Hash algoritm DER code;
 * @param pAlgIdSize - The size of the Hash algoritm DER code;
 *
 * @return CCError_t
 */
static CCError_t GetHashAlgDerCode(
                                    CCHashOperationMode_t hashMode,
                                    const uint8_t **pAlgId,
                                    uint32_t *pAlgIdSize)
{
        if (hashMode >= sizeof(gHashDerCodes) / sizeof(HashDerCode_t))
                return CC_RSA_GET_DER_HASH_MODE_ILLEGAL;
        /* output requirred Hash algorithm DER code */
        *pAlgId = gHashDerCodes[hashMode].algId;
        *pAlgIdSize = (gHashDerCodes[hashMode]).algIdSizeBytes;
        return CC_OK;
}

/*****************************************************************************/
/**
 * The function generates vector of non zero octets.
 *
 * @author reuvenl (9/14/2014)
 *
 * @param rndContext_ptr  - Pointer to the RND context buffer.
 * @param pVect - The pointer to output buffer.
 * @param size  - The size of vector in bytes.
 *
 * @return CCError_t
 */
CCError_t  RsaGenRndNonZeroVect(CCRndContext_t *rndContext_ptr, uint8_t *pVect, uint32_t size)
{
        #define TMP_SIZE 16
        CCError_t err = CC_OK;
        uint32_t i, j, newRnd;
        uint8_t tmp[TMP_SIZE];
        uint8_t zero[TMP_SIZE] = { 0 };


        void   *rndState_ptr;
        CCRndGenerateVectWorkFunc_t RndGenerateVectFunc;

        /* check parameters */
        if (rndContext_ptr == NULL)
                return CC_RND_CONTEXT_PTR_INVALID_ERROR;

        rndState_ptr = rndContext_ptr->rndState;
        RndGenerateVectFunc = rndContext_ptr->rndGenerateVectFunc;

        if (RndGenerateVectFunc == NULL)
                return CC_RND_GEN_VECTOR_FUNC_ERROR;

        /* generate random vector */
        err = RndGenerateVectFunc(rndState_ptr, (unsigned char *)pVect, (size_t)size);
        if (err != CC_OK) {
                return err;
        }

        /* generate auxiliary random buff and change zero octets */
        j = 0; newRnd = 1;
        for (i=0; i<size; i++) {
                while (1) {
                        if (newRnd) {
                                j = 0; newRnd = 0;
                                err = RndGenerateVectFunc(rndState_ptr, (unsigned char *)tmp, sizeof(tmp));
                                // Handle case of RND function which returns zeros
                                if (CC_PalMemCmp(tmp, zero, TMP_SIZE) == 0) {
                                    return CC_RND_GEN_VECTOR_FUNC_ERROR;
                                }
                                if (err != CC_OK) {
                                        return err;
                                }
                        }

                        /* change byte*/
                        if (pVect[i] == 0 ) {
                                if (tmp[j] == 0) {
                                        j++;
                                        if (j == sizeof(tmp)) {
                                            newRnd = 1;
                                        }
                                        continue;
                                }
                                pVect[i] = tmp[j];
                                j++;
                                break;
                        } else
                                break;
                }
                if (j == sizeof(tmp)) {
                        newRnd = 1;
                }
        }
        return err;
}

/*********************************************************************************/
/**
 * @brief The function implements PKCS#1 v1.5 (9.2) EMSA Encoding
 *        algorithm used in Sign/Verify operations.
 *
 * @author reuvenl (9/14/2014)
 *
 * @param K - The size of encoded message in octets.
 * @param hashMode - hash mode ID (enum).
 * @param pM - The Pointer to the Message M. In case of Sign it is a hash (H).
 * @param MSize - Denotes the Message size: for Sig/Ver = hashSize,
 *                for Enc/Dec <= K-hashAlgIdSize-PSS_MIN_LEN-3.
 * @param pOut - The pointer to a buffer which is at least K octets long.
 *
 * @return CCError_t
 */
CCError_t RsaEmsaPkcs1v15Encode(
                                        uint32_t K,
                                        CCHashOperationMode_t hashMode,
                                        uint8_t     *pM, /*mess.digest*/
                                        uint32_t     MSize,
                                        uint8_t     *pOut)
{
        /* The return error identifier */
        CCError_t Error = CC_OK;

        /* Padding String Size  */
        int32_t  PSSize;
        /* The pointer to Hash Alg. ID (DER code) and its size */
        const uint8_t *pHashAlgId = NULL;
        uint32_t hashAlgIdSize = 0;

        /* FUNCTION LOGIC */

#ifdef DEBUG
        /* Init to garbage */
        CC_PalMemSet(pOut, 0xCC, K);
#endif

        /*---------------------------------------------------*/
        /*  Encryption block formating for EMSA-PKCS1-v1_5:  */
        /*          00 || 01 || PS || 00 || T            */
        /*         MSB                      LSB          */
        /* Note:  BT=02, PS=FF...FF, T=DER||Hash(M)          */
        /*---------------------------------------------------*/

        /* Get Hash alg. parametrs including DER code */
        Error = GetHashAlgDerCode(hashMode, &pHashAlgId, &hashAlgIdSize);
        if (Error)
                return Error;

        /* check sizes */
        if (3+MSize+PS_MIN_LEN+hashAlgIdSize > K)
                return CC_RSA_ENCODE_15_MSG_OUT_OF_RANGE;

        PSSize = K-MSize-hashAlgIdSize-3; /*therefore, PSSize >= PS_MIN_LEN*/

        /* Fill the formatted output buffer */
        pOut[0]=0x00;    /* set the 00 */
        pOut[1]=0x01;    /* set Block Type 01 */
        CC_PalMemSet(&pOut[2], 0xFF, PSSize);
        /* copy the Hash Algorithm ID (DER code) */
        CC_PalMemCopy(&pOut[3+PSSize], pHashAlgId, hashAlgIdSize);

        /* set 00-byte after PS */
        pOut[2+PSSize] = 0x00;
        /* copy the message/digest data */
        CC_PalMemCopy(&pOut[K-MSize], pM, MSize);

        return CC_OK;
}

#endif /*defined(_INTERNAL_CC_NO_RSA_SCHEME_15_SUPPORT) && !defined(_INTERNAL_CC_NO_RSA_ENCRYPT_SUPPORT) && !defined(_INTERNAL_CC_NO_RSA_VERIFY_SUPPORT)*/



/******************************************************************************************/

/* just to have a declaarion so the 'C' file passes compilation */
void CRTS_RSA_VER15_UTIL_foo(void) {}

#endif /* !defined(CC_IOT) || ( defined(CC_IOT) && defined(MBEDTLS_RSA_C)) */

