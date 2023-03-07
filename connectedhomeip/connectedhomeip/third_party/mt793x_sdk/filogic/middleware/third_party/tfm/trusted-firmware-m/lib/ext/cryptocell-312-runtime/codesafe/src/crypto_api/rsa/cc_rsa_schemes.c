/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/************* Include Files ****************/
#ifdef CC_IOT
    #if defined(MBEDTLS_CONFIG_FILE)
    #include MBEDTLS_CONFIG_FILE
    #endif
#endif

#if !defined(CC_IOT) || ( defined(CC_IOT) && defined(MBEDTLS_RSA_C))

#include "cc_pal_mem.h"
#include "cc_pal_types.h"
#include "cc_rsa_error.h"
#include "cc_hash_defs.h"
#include "cc_rsa_local.h"
#include "cc_rsa_prim.h"
#include "cc_fips_defs.h"

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

/**********************************************************************************************************/

/**
   @brief
   RSA_SCHEMES_Encrypt implements the RSAES-OAEP algorithm as defined
   in PKCS#1 v2.1 8.1 and in PKCS#1 v1.5 8.1

        This function combines the RSA encryption primitive and the
        EME-OAEP encoding method, to provide an RSA-based encryption
        method that is semantically secure against adaptive
        chosen-ciphertext attacks. For more details, please refere to
        the PKCS#1 standard.

        The actual macro that will be used by the user is:
        CC_RsaOaepEncrypt     - for v2.1
        CC_RsaPkcs1V15Encrypt - for v1.5

   @param[in/out] rndContext_ptr  - Pointer to the RND context buffer.
   @param[in] UserPubKey_ptr - A pointer to the public key data structure of the User.
   @param[in] PrimeData_ptr - A pointer to a CCRsaPrimeData_t
                                that is used for the Encryption operation
   @param[in] hashFunc - The hash function to be used.
                         The hash functions supported: SHA1, SHA-256/284/512,
                         MD5 (MD5 - allowed only for PKCS#1 v1.5).
   @param[in] L - The label input pointer. Relevant for PKCS#1 Ver2.1 only, may be NULL also.
                  For PKCS#1 Ver1.5 it is an empty string (NULL).
   @param[in] Llen - The label length. Relevant for PKCS#1 Ver2.1 only (see notes above).
   @param[in] MGF - the mask generation function. PKCS#1 v2.1
                    defines MGF1, so the currently allowed value is CC_PKCS1_MGF1.
   @param[in] Data_ptr - Pointer to the data to encrypt.
   @param[in] DataSize - The size, in bytes, of the data to encrypt.
                         \Note: The data size must be:
                            1. for PKCS #1 v.2.1  DataSize <= PrivKey_ptr->N.len - 2*HashLen - 2.
                            2. for PKCS #1 v.1.5  DataSize <= PrivKey_ptr->N.len - 11.
   @param[out] Output_ptr - Pointer to the encrypted data. The size of the data is always
                            equal to the RSA key (modulus) size, in bytes. Therefore the size
                            of allocated buffer must be at least of this size.

   @return CCError_t - CC_OK, or error
*/
CEXPORT_C CCError_t CC_RsaSchemesEncrypt(
                                             CCRndContext_t *rndContext_ptr,
                                             CCRsaUserPubKey_t *UserPubKey_ptr,
                                             CCRsaPrimeData_t  *PrimeData_ptr,
                                             CCRsaHashOpMode_t hashFunc,
                                             uint8_t            *L,
                                             size_t             Llen,
                                             CCPkcs1Mgf_t   MGF,
                                             uint8_t           *DataIn_ptr,
                                             size_t             DataInSize,
                                             uint8_t            *Output_ptr,
                                             CCPkcs1Version_t PKCS1_ver)
{
        /* FUNCTION DECLARATIONS */

        /* The return error identifier */
        CCError_t Error = CC_OK;

        /*The modulus size in Bytes*/
        uint16_t K;
        uint8_t HashOutputSize;

        /*In order to save stack memory place -
         * It is required that the Output_ptr is at least the size of the modulus
         * It is also required that the RSA computation is done in-place */
        uint8_t *EB_buff = Output_ptr;

        CCRsaPubKey_t *PubKey_ptr;
        CCHashOperationMode_t hashOpMode;
        uint32_t PSSize;

        /* ............... checking the parameters validity ................... */
        /* -------------------------------------------------------------------- */
    CHECK_AND_RETURN_ERR_UPON_FIPS_ERROR();

        /* if the users context pointer is NULL return an error */
        if (UserPubKey_ptr == NULL)
                return CC_RSA_INVALID_PUB_KEY_STRUCT_POINTER_ERROR;

        /* checking the Prime Data pointer */
        if (PrimeData_ptr == NULL)
                return CC_RSA_PRIM_DATA_STRUCT_POINTER_INVALID;

        /* check if the hash operation mode is legal */
        if (hashFunc >= CC_RSA_HASH_NumOfModes)
                return CC_RSA_HASH_ILLEGAL_OPERATION_MODE_ERROR;

        /* check if the MGF operation mode is legal */
        if (MGF >= CC_RSA_NumOfMGFFunctions)
                return CC_RSA_MGF_ILLEGAL_ARG_ERROR;

        /* check that the PKCS1 version argument is legal*/
        if (PKCS1_ver >= CC_RSA_NumOf_PKCS1_versions)
                return CC_RSA_PKCS1_VER_ARG_ERROR;

        /* if the users Data In pointer is illegal return an error */
        /* note - it is allowed to encrypt a message of size zero ; only on this case a NULL is allowed */
        if (DataIn_ptr == NULL && DataInSize != 0)
                return CC_RSA_DATA_POINTER_INVALID_ERROR;

        /*If the output pointer is NULL return Error*/
        if (Output_ptr == NULL)
                return CC_RSA_INVALID_OUTPUT_POINTER_ERROR;

        PubKey_ptr = (CCRsaPubKey_t *)UserPubKey_ptr->PublicKeyDbBuff;

        if (UserPubKey_ptr->valid_tag != CC_RSA_PUB_KEY_VALIDATION_TAG)
                return CC_RSA_PUB_KEY_VALIDATION_TAG_ERROR;

        if (Llen == 0)
                L = NULL;

        /* .................. initializing local variables ................... */
        /* ------------------------------------------------------------------- */

        /*Initialize K with the modulus size in Bytes*/
        K = (uint16_t)CALC_FULL_BYTES(PubKey_ptr->nSizeInBits);

#ifdef DEBUG
        /*Initialize the Output_ptr to Zero*/
        CC_PalMemSetZero(EB_buff, K);
#endif

        /*-------------------------------------------------------*
         * Perform Encoding and Encryption accordimg to PKCS1    *
         * Versions: VER21 or VER15                              *
         *-------------------------------------------------------*/

        switch (PKCS1_ver) {

#ifndef _INTERNAL_CC_NO_RSA_SCHEME_15_SUPPORT
        case CC_PKCS1_VER15:
                /*-------------------------------------------------------*
                 * Step 1 : Check modulus and data sizes             *
                 *-------------------------------------------------------*/
                /*Check the modulus size is legal*/
                if (K < 3 + PS_MIN_LEN)
                        return CC_RSA_INVALID_MODULUS_SIZE;

                if (DataInSize + 3 + PS_MIN_LEN > K )
                        return CC_RSA_INVALID_MESSAGE_DATA_SIZE;
                /* size of PS buffer, it is >= PS_MIN_LEN  */
                PSSize = K -  3 - DataInSize;

                /*-------------------------------------------------------*
                 * Step 2 :  Encode the message                          *
                 *-------------------------------------------------------*/

                EB_buff[0]=0x00; /*set the 00 */
                EB_buff[1]=0x02; /*Block type for EME-PKCS1-v1_5*/

                /* Generate random non-zero bytes for PS */
                Error = RsaGenRndNonZeroVect(rndContext_ptr, &EB_buff[2], PSSize);
                if (Error != CC_OK) {
                        goto End;
        }
                /* 0-byte after PS */
                EB_buff[K-DataInSize-1] = 0x00;
                /* Copy the message data */
        if (DataInSize > 0)
            CC_PalMemCopy(&EB_buff[K-DataInSize], DataIn_ptr, DataInSize);

                break;
#endif

#ifndef _INTERNAL_CC_NO_RSA_SCHEME_21_SUPPORT

        /* get CC Hash parameters */
        case CC_PKCS1_VER21:

                switch (hashFunc) {
                case CC_RSA_HASH_MD5_mode : /*MD5 is not reccomended in PKCS1 ver 2.1 standard,
                                                hence it is not supported*/
                        return CC_RSA_HASH_ILLEGAL_OPERATION_MODE_ERROR;
                case CC_RSA_HASH_SHA1_mode:
                        HashOutputSize = CC_HASH_SHA1_DIGEST_SIZE_IN_WORDS*CC_32BIT_WORD_SIZE;
                        hashOpMode = CC_HASH_SHA1_mode;/*changing the hash mode to CC definition*/
                        break;
                case CC_RSA_HASH_SHA224_mode:
                        HashOutputSize = CC_HASH_SHA224_DIGEST_SIZE_IN_WORDS*CC_32BIT_WORD_SIZE;
                        hashOpMode = CC_HASH_SHA224_mode;/*changing the hash mode to CC definition*/
                        break;
                case CC_RSA_HASH_SHA256_mode:
                        HashOutputSize = CC_HASH_SHA256_DIGEST_SIZE_IN_WORDS*CC_32BIT_WORD_SIZE;
                        hashOpMode = CC_HASH_SHA256_mode;/*changing the hash mode to CC definition*/
                        break;
                case CC_RSA_HASH_SHA384_mode:
                        HashOutputSize = CC_HASH_SHA384_DIGEST_SIZE_IN_WORDS*CC_32BIT_WORD_SIZE;
                        hashOpMode = CC_HASH_SHA384_mode;/*changing the hash mode to CC definition*/
                        break;
                case CC_RSA_HASH_SHA512_mode:
                        HashOutputSize = CC_HASH_SHA512_DIGEST_SIZE_IN_WORDS*CC_32BIT_WORD_SIZE;
                        hashOpMode = CC_HASH_SHA512_mode;/*changing the hash mode to CC definition*/
                        break;
                default:
                        return CC_RSA_HASH_ILLEGAL_OPERATION_MODE_ERROR;
                }

                /* if mLen>k-2*hLen-2 output message too long */
                if ((uint32_t)DataInSize + 2 * HashOutputSize + 2 > K)
                        return CC_RSA_INVALID_MESSAGE_DATA_SIZE;

                /*-------------------------------------------------------*
                 * Step 2 : Apply the EME-OAEP encoding operation to     *
                 *   the message M and the label L to produce a          *
                 *   ciphertext of length k octets.                      *
                 *-------------------------------------------------------*/

                Error=RsaPssOaepEncode(
                                             rndContext_ptr,   /*! random functions comtext*/
                                             hashOpMode,   /*! hash operation mode enum */
                                             MGF,              /*! MGF function mode enum */
                                             DataIn_ptr,       /*! input data to be encrypted */
                                             DataInSize,       /*! input data size bytes */
                                             L,                /*! label */
                                             Llen,             /*! label length bytes */
                                             K,                /*! modulus size in bytes */
                                             PrimeData_ptr,    /*! temp buffer 1 structure for imternal use */
                                             EB_buff,          /*! temp buffer 2 for imternal use */
                                             PKCS1_ver         /*! PKCS1 version enum */);
                if (Error != CC_OK) {
                        goto End;
                }
                break;
#endif
        default:
                return CC_RSA_PKCS1_VER_ARG_ERROR;
        }

        /*-------------------------------------------*/
        /* Step 3 : RSA computation                  */
        /*-------------------------------------------*/

        Error = CC_RsaPrimEncrypt(UserPubKey_ptr,
                                      PrimeData_ptr,
                                      EB_buff,
                                      K,
                                      Output_ptr);
End:
    if (Error != CC_OK) {
        CC_PalMemSetZero (Output_ptr, K);
    }
    /* clear the temp data buffer */
    CC_PalMemSetZero(PrimeData_ptr, sizeof(CCRsaPrimeData_t));

        return Error;


}/* END OF CC_RsaSchemesEncrypt */
#endif /*!defined(_INTERNAL_CC_NO_RSA_ENCRYPT_SUPPORT) && !defined(_INTERNAL_CC_NO_RSA_VERIFY_SUPPORT)*/

#if !defined(_INTERNAL_CC_NO_RSA_DECRYPT_SUPPORT) && !defined(_INTERNAL_CC_NO_RSA_SIGN_SUPPORT)
/**********************************************************************************************************/
/**
   @brief
   RSA_SCHEMES_Decrypt implements the RSAES-OAEP algorithm as defined
   in PKCS#1 v2.1 8.1 and in PKCS#1 v1.5

           This function combines the RSA decryption primitive and the
           EME-OAEP decoding method, to provide an RSA-based decryption
           method that is semantically secure against adaptive
           chosen-ciphertext attacks. For more details, please refer to
           the PKCS#1 standard.

   @param[in] UserPrivKey_ptr - Pointer to the private key data structure.
                   \Note: The representation (pair or quintuple)
                    and hence the algorithm (CRT or not) is determined
                    by the Private Key data structure. Using CC_BuildPrivKey
                    or CC_BuildPrivKeyCRT determines which algorithm will be used.

   @param[in] PrimeData_ptr - Pointer to a CCRsaPrimeData_t which is used for the
                                                          Encryption operation

   @param[in] hashFunc - The hash function to be used.
                         The hash functions supported: SHA1, SHA-256/284/512,
                         MD5 (MD5 - allowed only for PKCS#1 v1.5).

   @param[in] L - The label input pointer. Relevant for PKCS#1 Ver2.1 only, may be NULL also.
                  For PKCS#1 Ver1.5 it is an empty string (NULL).
   @param[in] Llen - The label length. Relevant for PKCS#1 Ver2.1 only (see notes above).
   @param[in] MGF - The mask generation function. PKCS#1 v2.1 defines MGF1,
                    so the only value allowed here is CC_PKCS1_MGF1.
   @param[in] Data_ptr - Pointer to the data to decrypt.
   @param[in] DataSize - The size, in bytes, of the data to decrypt.
                        \Note: The size must be = the size of the modulus.

   @param[out] Output_ptr - Pointer to the decrypted data, the size of the buffer in bytes
                must be not less than the actual size of Encrypted message, if it is known,
                else the output buffer size must be :
                1. for PKCS #1 v.2.1  *OutputSize_ptr >= PrivKey_ptr->N.len - 2*HashLen - 2.
                2. for PKCS #1 v.1.5  *OutputSize_ptr >= PrivKey_ptr->N.len - 11.
   @param[in/out] OutputSize_ptr - The size of the user passed Output_ptr buffer in bytes [in] and
                actual size of decrypted message [out].
                The minimal input size value of *OutputSize_ptr is described above.
                This value is updated with the actual number of bytes that
                are loaded to Output_ptr buffer byDecrypt function.

   @return CCError_t - CC_OK or appropriate Error message defined in the RSA module.
*/
CEXPORT_C CCError_t CC_RsaSchemesDecrypt(
                                             CCRsaUserPrivKey_t  *UserPrivKey_ptr,
                                             CCRsaPrimeData_t    *PrimeData_ptr,
                                             CCRsaHashOpMode_t  hashFunc,
                                             uint8_t              *L,
                                             size_t                Llen,
                                             CCPkcs1Mgf_t      MGF,
                                             uint8_t              *DataIn_ptr,
                                             size_t                DataInSize,
                                             uint8_t              *Output_ptr,
                                             size_t               *OutputSize_ptr,
                                             CCPkcs1Version_t    PKCS1_ver)
{
        /* FUNCTION DECLARATIONS */

        /* The return error identifier */
        CCError_t Error = CC_OK;
        uint16_t K; /*The modulus size in Bytes*/
        uint8_t EB_buff[CC_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS*CC_32BIT_WORD_SIZE];
        uint16_t HashOutputSizeBytes;
        CCRsaPrivKey_t *PrivKey_ptr;
        /*The Hash enum sent to the lower level functions*/
        /*The initialization is to eliminate a warning of uninitialized variable*/
        CCHashOperationMode_t hashOpMode = CC_HASH_NumOfModes;

        int32_t PSSize, i;


        /* FUNCTION LOGIC */

        /* .................. initializing local variables ................... */
        /* ------------------------------------------------------------------- */

    CHECK_AND_RETURN_ERR_UPON_FIPS_ERROR();
        /* initialize the HASH mode as SHA1 - default */
        hashOpMode = CC_HASH_SHA1_mode;

        /* ............... checking the parameters validity ................... */
        /* -------------------------------------------------------------------- */

        /* if the users context pointer is NULL return an error */
        if (UserPrivKey_ptr == NULL)
                return CC_RSA_INVALID_PRIV_KEY_STRUCT_POINTER_ERROR;

        if (PrimeData_ptr == NULL)
                return CC_RSA_PRIM_DATA_STRUCT_POINTER_INVALID;

        /* check if the hash operation mode is legal */
        if (hashFunc >= CC_RSA_HASH_NumOfModes)
                return CC_RSA_HASH_ILLEGAL_OPERATION_MODE_ERROR;

        /* check if the MGF operation mode is legal */
        if (MGF >= CC_RSA_NumOfMGFFunctions)
                return CC_RSA_MGF_ILLEGAL_ARG_ERROR;

        /* check that the PKCS1 version argument is legal*/
        if (PKCS1_ver >= CC_RSA_NumOf_PKCS1_versions)
                return CC_RSA_PKCS1_VER_ARG_ERROR;

        /* if the users Data In pointer is illegal return an error */
        if (DataIn_ptr == NULL)
                return CC_RSA_DATA_POINTER_INVALID_ERROR;

        /* if the data size is zero or larger then 2^29 (to prevent an overflow on the transition to bits )
           return error */
        if (DataInSize == 0)
                return CC_RSA_INVALID_MESSAGE_DATA_SIZE;

        /*If the output pointer is NULL return Error*/
        if (Output_ptr == NULL)
                return CC_RSA_INVALID_OUTPUT_POINTER_ERROR;

        /*If the output size pointer is NULL return error*/
        if (OutputSize_ptr ==NULL)
                return CC_RSA_DECRYPT_OUTPUT_SIZE_POINTER_ERROR;

        PrivKey_ptr = (CCRsaPrivKey_t *)UserPrivKey_ptr->PrivateKeyDbBuff;
        if (UserPrivKey_ptr->valid_tag != CC_RSA_PRIV_KEY_VALIDATION_TAG)
                return CC_RSA_PRIV_KEY_VALIDATION_TAG_ERROR;

        if (Llen == 0)
                L = NULL;

        /* .................. initializing local variables ................... */
        /* ------------------------------------------------------------------- */

        /*Initialize K with the modulus size in Bytes*/
        K = (uint16_t)(CALC_FULL_BYTES(PrivKey_ptr->nSizeInBits));

        /*Length Checking - both for Ver 1.5 and 2.1*/
        if (DataInSize != K)
                return CC_RSA_INVALID_MESSAGE_DATA_SIZE;

        /*-------------------------------------------------*/
        switch (PKCS1_ver) {

#ifndef _INTERNAL_CC_NO_RSA_SCHEME_15_SUPPORT
        case CC_PKCS1_VER15:
                /*Check the modulus size is legal*/
                if (K < 11)
                        return CC_RSA_INVALID_MODULUS_SIZE;
                break;
#endif

#ifndef _INTERNAL_CC_NO_RSA_SCHEME_21_SUPPORT
        case CC_PKCS1_VER21:

                switch (hashFunc) {
                case CC_RSA_HASH_MD5_mode :
                        /*MD5 is not recommended in PKCS1 ver 2.1 standard, hence it is not supported*/
                        return CC_RSA_HASH_ILLEGAL_OPERATION_MODE_ERROR;
                case CC_RSA_HASH_SHA1_mode:
                        hashOpMode = CC_HASH_SHA1_mode;/*changing the hash mode to CC definition*/
                        HashOutputSizeBytes = CC_HASH_SHA1_DIGEST_SIZE_IN_BYTES;
                        break;
                case CC_RSA_HASH_SHA224_mode:
                        hashOpMode = CC_HASH_SHA224_mode;
                        HashOutputSizeBytes = CC_HASH_SHA224_DIGEST_SIZE_IN_BYTES;
                        break;
                case CC_RSA_HASH_SHA256_mode:
                        hashOpMode = CC_HASH_SHA256_mode;
                        HashOutputSizeBytes = CC_HASH_SHA256_DIGEST_SIZE_IN_BYTES;
                        break;
                case CC_RSA_HASH_SHA384_mode:
                        hashOpMode = CC_HASH_SHA384_mode;
                        HashOutputSizeBytes = CC_HASH_SHA384_DIGEST_SIZE_IN_BYTES;
                        break;
                case CC_RSA_HASH_SHA512_mode:
                        hashOpMode = CC_HASH_SHA512_mode;
                        HashOutputSizeBytes = CC_HASH_SHA512_DIGEST_SIZE_IN_BYTES;
                        break;
                default:
                        return CC_RSA_HASH_ILLEGAL_OPERATION_MODE_ERROR;
                }

                /*Checking that the modulus have enough large */
                if (K < 2*HashOutputSizeBytes + 2)
                        return CC_RSA_INVALID_MODULUS_SIZE;
                break;
#endif
        default:
                return CC_RSA_PKCS1_VER_ARG_ERROR;

        }/* end of switch(PKCS1_ver) */


        /*-------------------------------------------*/
        /* Step 2 <b> : RSA computation              */
        /*-------------------------------------------*/
        Error = CC_RsaPrimDecrypt(UserPrivKey_ptr,
                                      PrimeData_ptr,
                                      DataIn_ptr,
                                      DataInSize,
                                      EB_buff);
        if (Error != CC_OK) {
                goto End;
        }

        /*----------------------------------------------*
         * Step 3 :  EME-OAEP Decoding          *
         *----------------------------------------------*/

        /* for all modes */
        if (EB_buff[0] != 0x00) {
                Error = CC_RSA_ERROR_IN_DECRYPTED_BLOCK_PARSING;
        goto End;
        }

        /*------------------------------------------------*
         * Perform decoding operation according to the    *
         * encoded message EM choosen PKCS1 version       *
         *------------------------------------------------*/

        switch (PKCS1_ver) {

#ifndef _INTERNAL_CC_NO_RSA_SCHEME_15_SUPPORT
        case CC_PKCS1_VER15:

                /*------------------------------------------------*
                 * Check parameters of decrypted buffer,          *
                 *    EM= 0x00||0x02||PS||0x00||M                 *
                 * If EM[0] != 0 or EM[1] != 2 or no 0-byte       *
                 * after PS or PS length < 8, then output "error" *
                 * and stop. Output the message M.            *
                 *------------------------------------------------*/

                if (EB_buff[1] != 0x02/*Block type for EME-PKCS1-v1_5*/) {
                        Error = CC_RSA_ERROR_IN_DECRYPTED_BLOCK_PARSING;
            goto End;
                }

                /* find next 0-byte after PS */
                for (i = 2; i < K; i++) {
                        if (EB_buff[i] == 0x00)
                                break;
                }
                /* if byte 0 not present */
                if (i == K) {
                        Error = CC_RSA_ERROR_IN_DECRYPTED_BLOCK_PARSING;
            goto End;
        }

                /* check PS size >= 8 */
                PSSize = i - 2;
                if (PSSize < PS_MIN_LEN) {
                        Error = CC_RSA_ERROR_IN_DECRYPTED_BLOCK_PARSING;
            goto End;
        }

                if (PSSize + 3 > K) {
                        Error = CC_RSA_ERROR_IN_DECRYPTED_BLOCK_PARSING;
            goto End;
        }

                /* check size of output buffer */
        /* according to the previous check K - 3 > PSSize => it must be positive and therefor can be casted to unsigned */
                if (*OutputSize_ptr < (uint32_t)(K - 3 - PSSize)) {
                        Error = CC_RSA_15_ERROR_IN_DECRYPTED_DATA_SIZE;
            goto End;
        } else {
                        *OutputSize_ptr = K - 3 - PSSize; /* output actual size of decrypted message*/
        }

                /* copy the message into output buffer */
                CC_PalMemCopy(Output_ptr, &EB_buff[3 + PSSize], *OutputSize_ptr);

                break;
#endif

#ifndef _INTERNAL_CC_NO_RSA_SCHEME_21_SUPPORT
        case CC_PKCS1_VER21:

                /*------------------------------------------------*
                 * Apply the EME-OAEP decoding operation to the   *
                 * encoded message EM and the parameter       *
                 * L to recover a message M:                      *
                 * M = EME-OAEP-DECODE (EM, L)                    *
                 * If the decoding operation outputs              *
                 * "decoding error," then output                  *
                 * "decryption error" and stop.                   *
                 *------------------------------------------------*/
                Error=RsaPssOaepDecode(
                                             hashOpMode,
                                             MGF,
                                             &EB_buff[1],
                                             (uint16_t)(K-1),
                                             L,
                                             Llen,
                                             PrimeData_ptr,
                                             Output_ptr,
                                             OutputSize_ptr);
                break;
#endif
        default:
                Error = CC_RSA_PKCS1_VER_ARG_ERROR;

        }
End:
    if (Error != CC_OK) {
        CC_PalMemSetZero (Output_ptr ,*OutputSize_ptr);
        *OutputSize_ptr = 0;
    }
    /* clear the temp data buffer */
    CC_PalMemSetZero(PrimeData_ptr, sizeof(CCRsaPrimeData_t));

        return Error;

}/* END OF CC_RsaSchemesDecrypt */


#endif /*!defined(_INTERNAL_CC_NO_RSA_ENCRYPT_SUPPORT) && !defined(_INTERNAL_CC_NO_RSA_VERIFY_SUPPORT)*/
#endif /*!defined(CC_IOT) || ( defined(CC_IOT) && defined(MBEDTLS_RSA_C)) */


