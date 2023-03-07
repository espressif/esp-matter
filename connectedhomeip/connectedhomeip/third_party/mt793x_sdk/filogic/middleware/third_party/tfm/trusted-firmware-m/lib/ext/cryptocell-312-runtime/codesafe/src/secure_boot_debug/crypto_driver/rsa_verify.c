/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#define CC_PAL_LOG_CUR_COMPONENT HOST_LOG_MASK_SECURE_BOOT


/************* Include Files ****************/

#include "secureboot_basetypes.h"
#include "secureboot_error.h"
#include "rsa_bsv.h"
#include "cc_pka_hw_plat_defs.h"
#include "secureboot_stage_defs.h"


/************************ Defines ******************************/

/************************ Global Data **************************/

/*********************** Private Functions *********************/

/*
\brief The function implements the RSA_PSS_Decode algorithm (PKCS#1 v2.1).
\note The input data is placed as BE bytes arrays into 32-bit word buffers
      for alignment goal. Order of bytes in arrays is BE (MS byte is left most one).
@return CC_OK on success,
@return A non-zero value from secureboot_error.h on failure.
*/

CCError_t RSA_PSS_Decode(unsigned long hwBaseAddress,  /* [in] HW base address of registers. */
                         CCHashResult_t mHash,         /* [in] SHA256 hash of the message (32 bytes). */
                         uint8_t *pEncodedMsg,         /* [in] Pointer to PSS encoded message (EM). Size = modulus size. */
                         int32_t *pVerifyStat          /* [out] Pointer to validation status value, equalled to:
                                                                1 - if valid, 0 - if not valid. */ )
{
        /* FUNCTION DECLERATIONS */

#define MASKED_DB_SIZE (SB_CERT_RSA_KEY_SIZE_IN_BYTES - HASH_RESULT_SIZE_IN_BYTES - 1)

    /* error variable */
    CCError_t error = CC_OK;
        uint32_t stat = 0;

    /* temp buffers and byte pointers */
    uint32_t tmpBuff[HASH_RESULT_SIZE_IN_WORDS + 1/*for counter*/];
    uint32_t dbMask[SB_CERT_RSA_KEY_SIZE_IN_WORDS];
    uint8_t *pDbMask;

        /* loop variables */
    uint32_t i, counter;

    /* FUNCTION LOGIC */

        /* check input pointers */
        if(mHash == NULL || pEncodedMsg == NULL || pVerifyStat == NULL) {
                return CC_BOOT_RSA_VERIFIER_ALG_FAILURE;
        }

        *pVerifyStat = 0; /*set status to "not valid"*/

        /* setting the EM and masked DB byte-pointers */
    pDbMask = (uint8_t*)dbMask;

        /*   operating the RSA PSS decoding scheme      */
        /*----------------------------------------------*/

        /* 9.1.2 <1,2,3> meet  */
        /* 9.1.2 <4> Check that the rightmost octet of EM = 0xbc */
    if (pEncodedMsg[SB_CERT_RSA_KEY_SIZE_IN_BYTES - 1] != 0xbc) {
        error = CC_BOOT_RSA_VERIFIER_CMP_FAILURE;
        goto End;
    }

        /*  9.1.2 <6> Check that the leftmost (8*emLen - emLenbit) of  *
        *   masked DB are equalled to 0, i.e. in our case MSbit = 0    */
    if (pEncodedMsg[0] & 0x80) {
        error = CC_BOOT_RSA_VERIFIER_CMP_FAILURE;
        goto End;
    }

        /*  9.1.2 <7> Let dbMask = MGF1(H,emLen-hLen-1)                *
         *  B.2.1 MGF1:                                                *
         *  For counter from 0 to  | L / hLen | , do the following:    *
     *  a.  Convert counter to an octet string C of length 4       *
         *  b.  Concatenate the hash of the seed H and C to the octet  *
         *      string T:  T = T || Hash(H || C)                       *
         *      C = C + 1                                              */

        /* copy the HASH from the EM (EncodedMsg) to the temp buffer */
        UTIL_MemCopy((uint8_t*)tmpBuff, &pEncodedMsg[MASKED_DB_SIZE], HASH_RESULT_SIZE_IN_BYTES);

    for (counter = 0; counter <= (MASKED_DB_SIZE/HASH_RESULT_SIZE_IN_BYTES); counter++ ) {

                /* a. tmp = H||C */
        tmpBuff[HASH_RESULT_SIZE_IN_WORDS] = UTIL_INVERSE_UINT32_BYTES(counter);

                /* b. Calculate and concatenate the hash on dbMask buffer: *
                *           T = T || HASH(H || C)                          */
                error = SBROM_CryptoHash(hwBaseAddress, CONVERT_TO_ADDR(tmpBuff),
                                     (HASH_RESULT_SIZE_IN_WORDS+1)*sizeof(uint32_t),
                                     &dbMask[counter*HASH_RESULT_SIZE_IN_WORDS]);

        if (error != CC_OK) {
                        goto End;
        }
    }

        /*  9.1.2 <8> Xor operation */
    for (i=0; i < MASKED_DB_SIZE; i++) {
        pDbMask[i] ^= pEncodedMsg[i];
    }

        /*  9.1.2 <9> Set the leftmost (8emLen - emBits) bits of the leftmost
                      octet in DB to zero (in this case it is MS bit only) */
    pDbMask[0] &= 0x7F;

        /*  9.1.2 <10> Check, that padding PS is zero and next byte = 0x01*/
    for (i = 0; i < SB_CERT_RSA_KEY_SIZE_IN_BYTES - HASH_RESULT_SIZE_IN_BYTES - RSA_PSS_SALT_LENGTH - 2; i++) {
                stat |= pDbMask[i];
    }
        if ((stat != 0) || (pDbMask[i] != 0x01)) {
                error = CC_BOOT_RSA_VERIFIER_CMP_FAILURE;
                goto End;
        }

        /*  9.1.2 <11> Let salt be the last sLen octets in DB */
        /*  9.1.2 <12> Let M' => (0x) 00 00 00 00 00 00 00 00 || mHash || salt*/

    UTIL_MemSet(pEncodedMsg, 0x00, RSA_PSS_PAD1_LEN); /* PS zero padding */
        /* Hash and Salt */
    UTIL_MemCopy(&pEncodedMsg[RSA_PSS_PAD1_LEN], (uint8_t*)mHash, HASH_RESULT_SIZE_IN_BYTES);
    UTIL_MemCopy(&pEncodedMsg[RSA_PSS_PAD1_LEN + HASH_RESULT_SIZE_IN_BYTES],
                     &pDbMask[MASKED_DB_SIZE - RSA_PSS_SALT_LENGTH], RSA_PSS_SALT_LENGTH);

        /*  9.1.2 <13> H' = Hash(M') ==> dbMask*/
    error = SBROM_CryptoHash(hwBaseAddress,
                        CONVERT_TO_ADDR(pEncodedMsg),
                        (RSA_PSS_PAD1_LEN + HASH_RESULT_SIZE_IN_BYTES + RSA_PSS_SALT_LENGTH),
                        dbMask/*H'*/);

        if (error != CC_OK) {
        goto End;
    }

        /*  9.1.2 <14> Compare H' == H; Note: If buffers are equalled,        *
        *   then CC_TRUE = 1 is returned                                      */
    *pVerifyStat = UTIL_MemCmp((uint8_t*)dbMask/*H'*/, (uint8_t*)tmpBuff/*hash on EM*/, sizeof(CCHashResult_t));

    if(*pVerifyStat != CC_TRUE) {
        error = CC_BOOT_RSA_VERIFIER_CMP_FAILURE;
                *pVerifyStat = CC_FALSE;
                goto End;
        }

        /* end of function, clean temp buffers */
End:
        UTIL_MemSet((uint8_t*)tmpBuff, 0, (HASH_RESULT_SIZE_IN_BYTES +4));
        UTIL_MemSet((uint8_t*)dbMask, 0, (SB_CERT_RSA_KEY_SIZE_IN_BYTES));

    return error;

#undef MASKED_DB_SIZE

}/* End of RSA_PSS_Decode */


/*
\brief The function implements the RSA_PSS_Verify algorithm (PKCS#1 v2.1).
@return CC_OK on success,
@return A non-zero value from secureboot_error.h on failure.
*/
CCError_t RSA_PSS_Verify(unsigned long hwBaseAddress,  /* [in] HW base address of registers. */
                         CCHashResult_t mHash,    /* [in] Pointer to the SHA256 hash of the message. */
                         uint32_t      *pN,            /* [in] Pointer to the RSA modulus (LE words array). */
                         uint32_t      *pNp,           /* [in] Pointer to the Barrett tag of the RSA modulus (LE words array). */
                         uint32_t      *pSign          /* [out] Pointer to the signature output (it is placed as BE bytes
                                                                array into words buffer for alignments goal). */)
{
        /* DECLARATIONS */

    CCError_t error = CC_OK;
        int32_t verifyStat = 0; /* 1 - valid, 0 - not valid */

        /* a buffer for the decrypted signiture */
    uint32_t ED[SB_CERT_RSA_KEY_SIZE_IN_WORDS + 1];

    /* FUNCTION LOGIC */

        /* execute the decryption */
    RSA_CalcExponent(hwBaseAddress, pSign/*in*/, pN, pNp, ED/*res*/);
        /* reverse to big.end format */
    UTIL_ReverseBuff((uint8_t*)ED, SB_CERT_RSA_KEY_SIZE_IN_BYTES);

        /*  operating the RSA PSS decoding primitive  */
    /* ------------------------------------------ */

        error = RSA_PSS_Decode(hwBaseAddress,
                         mHash, /*32 bytes*/
                         (uint8_t*)ED/*RSA mod. size*/,
                         &verifyStat);
        if (error) {
                goto End;
    }

        if(verifyStat != 1) {
                error = CC_BOOT_RSA_VERIFIER_CMP_FAILURE;
    }

End:
        /* zeroing temp buffer */
        UTIL_MemSet((uint8_t*)ED, 0, (SB_CERT_RSA_KEY_SIZE_IN_BYTES + 4));

        return error;

}/* END OF RSA_PSS_Verify */



