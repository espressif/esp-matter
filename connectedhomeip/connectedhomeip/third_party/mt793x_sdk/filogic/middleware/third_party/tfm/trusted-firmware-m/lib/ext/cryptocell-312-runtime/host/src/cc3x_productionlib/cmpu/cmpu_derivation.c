/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


/************* Include Files ****************/
#include <stdint.h>
#include <string.h>
#include "cc_pal_log.h"
#include "cc_prod_error.h"
#include "prod_hw_defs.h"
#include "prod_crypto_driver.h"
#include "cmpu_derivation.h"
#include "cc_pal_mem.h"
#include "llf_rnd.h"
#include "driver_defs.h"


/* Description: Implementation of CTR_DRBG_Generate_algorithm, Taken from NIST SP800-90A, section 10.4.2
   Parameters:
    pInputBuff[IN] - the seed_material with some additional bytes to avoid copy of large buffers
    actualInputDataSize[IN] - the actual size of teh seed without teh additions
    pOutputBuff[OUT] - the generated buffer - 32 bytes
 */
static uint32_t Derivation_Block_Cipher_df(uint32_t   *pInputBuff,
                                           uint32_t   actualInputDataSize,
                                           uint32_t    *pOutput)
{
    uint32_t  error = CC_OK;
    uint8_t *initMac_ptr;
    uint32_t i;
    uint32_t additionalBytesSize = 0;
    uint8_t *inputPtr = (uint8_t*)pInputBuff;
    uint8_t keyBuff[CC_PROD_AES_Key256Bits_SIZE_IN_BYTES];
    uint8_t ivBuff[CC_PROD_AES_IV_COUNTER_SIZE_IN_BYTES];
    uint32_t  outDataSizeBytes = CC_PROD_AES_Key256Bits_SIZE_IN_BYTES+CC_PROD_AES_IV_COUNTER_SIZE_IN_BYTES;
    uint32_t inputDataSize = actualInputDataSize;
    uint8_t Key[CC_PROD_AES_Key256Bits_SIZE_IN_BYTES] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
            0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F};
    uint8_t InitialMac256[3][16] = {{0xF2,0x90,0x00,0xB6,0x2A,0x49,0x9F,0xD0,0xA9,0xF3,0x9A,0x6A,0xDD,0x2E,0x77,0x80},
            {0x9D,0xBA,0x41,0xA7,0x77,0xF3,0xB4,0x6A,0x37,0xB7,0xAA,0xAE,0x49,0xD6,0xDF,0x8D},
            {0x2F,0x7A,0x3C,0x60,0x07,0x08,0xD1,0x24,0xAC,0xD3,0xC5,0xDE,0x3B,0x65,0x84,0x47}};

    if ((NULL == pInputBuff) || (actualInputDataSize == 0) || (NULL == pOutput)) {
        CC_PAL_LOG_ERR("invalid parameters\n");
        return CC_PROD_INVALID_PARAM_ERR;
    }
    /* Steps from NIST - ignored
       1. If (number_of_bits_to_return > max_number_of_bits), then return an ERROR_FLAG. */

    /* Steps from NIST
       2. L = len (input_string)/8. Comment: L is the bitstring represention of the integer resulting from len (input_string)/8. L shall be represented as a 32-bit integer.
       3. N = number_of_bits_to_return/8. Comment : N is the bitstring represention of the integer resulting from number_of_bits_to_return/8                    .
          N shall be represented as a 32-bit integer.
       4. S = L || N || input_string || 0x80.
          Comment : Pad S with zeros, if necessary.
       5. While (len (S) mod outlen) 0, S = S || 0x00. */
    /* convert L,N to little endian */
    pInputBuff[0] = CC_PROD_SET_WORD_AS_BE(actualInputDataSize); /* L */
    pInputBuff[1] = CC_PROD_SET_WORD_AS_BE(outDataSizeBytes);    /* N */

    inputPtr[8+actualInputDataSize] = 0x80;
    // pad with 0's the remaining bytes until we reach size which is outlenSize multiply
    additionalBytesSize = CC_PROD_AES_BLOCK_SIZE_IN_BYTES - ((8+1+actualInputDataSize)&(CC_PROD_AES_BLOCK_SIZE_IN_BYTES-1));
    CC_PalMemSetZero(&inputPtr[8+1+actualInputDataSize], additionalBytesSize);

    /* size of input to AES-MAC, rounded up to AES block */
    inputDataSize += (8/*2w*/ + 1/*0x80*/ + additionalBytesSize);


    /* Steps from NIST
       Comment : Compute the starting value.
       6. temp = the Null string.
       7. i = 0. Comment : i shall be represented as a 32-bit integer, i.e., len (i) = 32.
       8. K = Leftmost keylen bits of 0x00010203...1D1E1F.
       9. While len (temp) < keylen + outlen, do
       9.1 IV = i || 0outlen - len (i). Comment: The 32-bit integer represenation of i is padded with zeros to outlen bits.
       9.2 temp = temp || BCC (K, (IV || S)).
       9.3 i = i + 1. */
    for (i = 0; i < (outDataSizeBytes >> 4); i++) {
        /* set pointer to initial precomputed IV  value */
        initMac_ptr = (uint8_t*)&InitialMac256[i][0];

        /* AES MAC */
        error =  CC_PROD_Aes(CIPHER_CBC_MAC, CRYPTO_DIRECTION_ENCRYPT,
                             USER_KEY,
                             Key,sizeof(Key),
                             initMac_ptr,CC_PROD_AES_IV_COUNTER_SIZE_IN_BYTES,
                             pInputBuff,
                             inputDataSize,
                             pOutput + i*CC_PROD_AES_BLOCK_SIZE_IN_WORDS);
        if (error != CC_OK) {
            CC_PAL_LOG_ERR("Failed to CC_PROD_Aes, error 0x%x\n", error);
            return error;
        }

    }

    /* Steps from NIST
       Comment: Compute the requested number of bits.
       10. K = Leftmost keylen bits of temp.
       11. X = Next outlen bits of temp. */
    CC_PalMemCopy(keyBuff, (uint8_t*)pOutput, sizeof(keyBuff));
    CC_PalMemCopy(ivBuff, (uint8_t*)(pOutput + CC_PROD_AES_Key256Bits_SIZE_IN_WORDS), sizeof(ivBuff));


    /* Steps from NIST
       12. temp = the Null string.
       13. While len (temp) < number_of_bits_to_return, do
       13.1 X = Block_Encrypt (K, X).
       13.2 temp = temp || X.
       14. requested_bits = Leftmost number_of_bits_to_return of temp. */
    /* Encrypt (K,IV) by AES-CBC using output buff */
    CC_PalMemSetZero((uint8_t*)pOutput, outDataSizeBytes);
    error = CC_PROD_Aes(CIPHER_CBC, CRYPTO_DIRECTION_ENCRYPT,
                        USER_KEY,
                        keyBuff, sizeof(keyBuff),
                        ivBuff, CC_PROD_AES_IV_COUNTER_SIZE_IN_BYTES,
                        pOutput,
                        outDataSizeBytes,
                        pOutput);
    if (error != CC_OK) {
        CC_PAL_LOG_ERR("Failed to CC_PROD_Aes, error 0x%x\n", error);
        return error;
    }

    /* Steps from NIST
       15. Return SUCCESS and requested_bits.*/
    return CC_OK;
} /* END of Derivation_Block_Cipher_df */




/* increment vector by value of 1 */
static void Derivation_incrementVector(uint8_t *iv_ptr, uint32_t vecSizeInwords)
{
    int32_t i;
    uint32_t tmp, curr, tmp1;

    for (i = vecSizeInwords-1; i >= 0; i--) {
        CC_PalMemCopy((uint8_t *)&tmp1, &iv_ptr[i*sizeof(uint32_t)], sizeof(tmp1));
        tmp = CC_PROD_SET_WORD_AS_BE(tmp1);
        curr = tmp;
        tmp = ((tmp+1) & 0xFFFFFFFF);

        /* inverse the bytes order in a word */
        tmp1 = CC_PROD_SET_WORD_AS_BE(tmp);
        CC_PalMemCopy(&iv_ptr[i*sizeof(uint32_t)], (uint8_t *)&tmp1, sizeof(tmp));
        if (tmp > curr) {
            break;
        }
    }
}



/* Description: Implementation of  CTR_DRBG_Update, Taken from NIST SP800-90A, section 10.2.1.2
   Parameters:
    provided_data[IN]: The data to be used. This must be exactly seedlen bits in length;
            this length is guaranteed by the construction of the provided_data in the instantiate, reseed and generate functions.
    Key[IN/OUT]: The current value of Key.
    V[IN/OUT]: The current value of V.
 */
static uint32_t Derivation_DRBG_Update(uint32_t   *providedData_ptr,
                                       uint8_t   *pKey,
                                       uint8_t   *pIv)
{

    uint32_t  error = CC_OK;
    uint32_t outDataSize = (CC_PROD_AES_Key256Bits_SIZE_IN_BYTES+CC_PROD_AES_IV_COUNTER_SIZE_IN_BYTES);
    uint32_t  pOutput[CC_PROD_AES_Key256Bits_SIZE_IN_WORDS+CC_PROD_AES_IV_COUNTER_SIZE_IN_WORDS];
    uint32_t i = 0;
    AesContext_t aesCtx;

    /* Steps taken from NIST
       1. temp = Null.
       2. While (len (temp) < seedlen) do
       2.1 V = (V + 1) mod 2outlen.
       2.2 output_block = Block_Encrypt (Key, V).
       2.3 temp = temp || ouput_block.
       3. temp = Leftmost seedlen bits of temp.
       4 temp = temp XOR provided_data. */
    error =  CC_PROD_AesInit(&aesCtx, CIPHER_CTR, CRYPTO_DIRECTION_ENCRYPT,
                             USER_KEY,
                             pKey,CC_PROD_AES_Key256Bits_SIZE_IN_BYTES,
                             pIv,CC_PROD_AES_IV_COUNTER_SIZE_IN_BYTES);
    if (error != CC_OK) {
        return error;
    }
    // dummy block to increment IV
    error =  CC_PROD_AesProcess(&aesCtx,
                                providedData_ptr,  CC_PROD_AES_BLOCK_SIZE_IN_BYTES,
                                pOutput);
    if (error != CC_OK) {
        return error;
    }
    for (i = 0; i < (outDataSize >> 4); i++) {
        error =  CC_PROD_AesProcess(&aesCtx,
                                    providedData_ptr + i*CC_PROD_AES_BLOCK_SIZE_IN_WORDS, CC_PROD_AES_BLOCK_SIZE_IN_BYTES,
                                    pOutput + i*CC_PROD_AES_BLOCK_SIZE_IN_WORDS);
        if (error != CC_OK) {
            return error;
        }
    }

    /* Steps taken from NIST
       5. Key = Leftmost keylen bits of temp.
       6. V = Rightmost outlen bits of temp. */
    CC_PalMemCopy(pKey, (uint8_t*)pOutput, CC_PROD_AES_Key256Bits_SIZE_IN_BYTES);
    CC_PalMemCopy(pIv, (uint8_t*)(pOutput+CC_PROD_AES_Key256Bits_SIZE_IN_WORDS), CC_PROD_AES_IV_COUNTER_SIZE_IN_BYTES);

    /*Steps taken from NIST
      7. Return the new values of Key and V. */
    return CC_OK;
}



/* Description: Implementation of CTR_DRBG_Instantiate_algorithm, Taken from NIST SP800-90A, section 10.2.1.3.2
   Parameters:
    pKey[OUT] - the generated key
    pIv[OUT] - the generated Iv
 */
uint32_t CC_PROD_Derivation_Instantiate (uint32_t *pEntrSrc,
                                         uint32_t  sourceSize,
                                         uint8_t *pKey,
                                         uint8_t *pIv)
{
    uint32_t  error = CC_OK;
    uint32_t   pDfOutput[CC_PROD_AES_Key256Bits_SIZE_IN_WORDS+CC_PROD_AES_IV_COUNTER_SIZE_IN_WORDS] = {0};

    if ((NULL == pKey) || (NULL == pIv) || (NULL == pEntrSrc) || (0 == sourceSize) || (!IS_ALIGNED(sourceSize, sizeof(uint32_t)))) {
        CC_PAL_LOG_ERR("invalid key parameters\n");
        return CC_PROD_INVALID_PARAM_ERR;
    }
    /* Steps taken from NIST  - already done before calling CC_PROD_Derivation_Instantiate
       1. seed_material = entropy_input || nonce || personalization_string.
        (Comment: Ensure that the length of the seed_material is exactly seedlen bits.) */
    /* Steps taken from NIST
       2. seed_material = Derivation_Block_Cipher_df (seed_material, seedlen). */
    error = Derivation_Block_Cipher_df(pEntrSrc, sourceSize, pDfOutput);
    if (error != CC_OK) {
        CC_PAL_LOG_ERR("failed Derivation_Block_Cipher_df, error is 0x%X\n", error);
        return error;
    }

    /* Steps taken from NIST
       3. Key = 0keylen. Comment: keylen bits of zeros.
       4. V = 0outlen. Comment: outlen bits of zeros. */
    CC_PalMemSetZero(pKey, CC_PROD_AES_Key256Bits_SIZE_IN_BYTES);
    CC_PalMemSetZero(pIv, CC_PROD_AES_IV_COUNTER_SIZE_IN_BYTES);
    /* Steps taken from NIST
       5. (Key, V) = Derivation_DRBG_Update (seed_material, Key, V). */
    error = Derivation_DRBG_Update(pDfOutput, pKey, pIv);
    if (error != CC_OK) {
        CC_PAL_LOG_ERR("failed Derivation_DRBG_Update, error is 0x%X\n", error);
        return error;
    }

    /* Steps taken from NIST
       6. reseed_counter = 1.  -- ignored here
       7. Return V, Key, and reseed_counter as the initial_working_state. */
    return CC_OK;

}


/* Description: Implementation of CTR_DRBG_Generate_algorithm, Taken from NIST SP800-90A, section 10.2.1.5.2
   Parameters:
    pKey[IN] - the generated key
    pIv[IN] - the generated Iv
    pOutputBuff[OUT] - the generated buffer - 32 bytes
 */
uint32_t CC_PROD_Derivation_Generate(uint8_t *pKey,
                                     uint8_t *pIv,
                                     uint32_t *pOutputBuff,
                                     uint32_t  outDataSize)
{
    uint32_t  error = CC_OK;
    uint32_t  i = 0;
    uint32_t  numBlocks = (outDataSize >> 4);
    uint32_t   temp[CC_PROD_AES_BLOCK_SIZE_IN_WORDS] = {0};
#ifndef CMPU_KAT
    uint32_t   firstDummyBytes[CC_PROD_AES_BLOCK_SIZE_IN_WORDS] = {0};
#endif
    AesContext_t aesCtx;

    if ((pKey == NULL) || (pIv == NULL) || (pOutputBuff == NULL)) {
        CC_PAL_LOG_ERR("Invalid input Param\n");
        return CC_PROD_INVALID_PARAM_ERR;
    }

    /* Steps taken from NIST - all following are ignored, since th eworking state is one time
       1. If reseed_counter > reseed_interval, then return an indication that a reseed is required.
       2. If (additional_input Null), then
       2.1 additional_input = Derivation_Block_Cipher_df (additional_input, seedlen).
       2.2 (Key, V) = Derivation_DRBG_Update (additional_input, Key, V).
       Else additional_input = 0seedlen. */



    /* Steps taken from NIST
       3. temp = Null.
       4. While (len (temp) < requested_number_of_bits) do:
       4.1 V = (V + 1) mod 2outlen.
       4.2 output_block = Block_Encrypt (Key, V).
       4.3 temp = temp || output_block.
       5. returned_bits = Leftmost requested_number_of_bits of temp. */
    /* Increment counter V = V+1 */
    Derivation_incrementVector(pIv, CC_PROD_AES_IV_COUNTER_SIZE_IN_WORDS);
    /* Init AES operation on CTR mode */
    error = CC_PROD_AesInit(&aesCtx, CIPHER_CTR, CRYPTO_DIRECTION_ENCRYPT,
                            USER_KEY,
                            pKey, CC_PROD_AES_Key256Bits_SIZE_IN_BYTES,
                            pIv, CC_PROD_AES_IV_COUNTER_SIZE_IN_BYTES);
    if (error != CC_OK) {
        CC_PAL_LOG_ERR("Faild to CC_PROD_AesInit, error 0x%x\n", error);
        return error;
    }
#ifndef CMPU_KAT
    error = CC_PROD_AesProcess(&aesCtx,
                               temp, sizeof(temp),
                               firstDummyBytes);
    if (error != CC_OK) {
        CC_PAL_LOG_ERR("Faild to CC_PROD_AesProcess, error 0x%x\n", error);
        return error;
    }
#endif

    /* generate full blocks of input data */
    for (i = 0; i < numBlocks; i++) {
        error = CC_PROD_AesProcess(&aesCtx,
                                   temp, CC_PROD_AES_BLOCK_SIZE_IN_BYTES,
                                   pOutputBuff + i*CC_PROD_AES_BLOCK_SIZE_IN_WORDS);
        if (error != CC_OK) {
            CC_PAL_LOG_ERR("Faild to CC_PROD_AesProcess, error 0x%x\n", error);
            return error;
        }
    }

    /* Steps taken from NIST - all following are ignored, since th eworking state is one time
       6. (Key, V) = Derivation_DRBG_Update (additional_input, Key, V).
       7. reseed_counter = reseed_counter + 1. */

    /* Steps taken from NIST
       8. Return SUCCESS and returned_bits; also return Key, V, and reseed_counter as the new_working_state. */
    return CC_OK;
}

