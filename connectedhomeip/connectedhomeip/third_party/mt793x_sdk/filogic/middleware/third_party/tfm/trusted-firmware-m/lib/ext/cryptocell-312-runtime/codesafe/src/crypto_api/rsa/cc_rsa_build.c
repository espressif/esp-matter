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
#include "cc_common.h"
#include "cc_common_math.h"
#include "cc_rsa_error.h"
#include "cc_rsa_local.h"
#include "pki.h"
#include "rsa.h"
#include "rsa_public.h"
#include "rsa_private.h"
#include "cc_fips_defs.h"

/************************ Defines ******************************/

/************************ Enums ******************************/

/************************ Typedefs ***************************/

/************************ Global Data ************************/

/************************ Public Functions ******************************/

/******************************************************************************************/
/**
 * @brief CC_RsaPubKeyBuild populates a CCRsaPubKey_t structure with
 *       the provided modulus and exponent.
 *
 *  Assumption : the modulus and the exponent are presented in big endian.
 *
 * @param[out] PubKey_ptr - a pointer to the public key structure. This structure will be
 *            used as an input to the CC_RsaPrimEncrypt API.
 *
 * @param[in] Exponent_ptr - a pointer to the exponent stream of bytes ( Big endian ).
 * @param[in] ExponentSize - The size of the exponent in bytes.
 * @param[in] Modulus_ptr  - a pointer to the modulus stream of bytes ( Big endian ) the MS
 *           bit must be set to '1'.
 * @param[in] ModulusSize  - The size of the modulus in bytes. Sizes supported according to
 *           used platform from 64 to 256 bytes and in some platforms up to 512 bytes.
 *
 * @return CCError_t - On success CC_OK is returned, on failure a
 *                        value MODULE_* as defined in .
 */
CEXPORT_C CCError_t CC_RsaPubKeyBuild(
               CCRsaUserPubKey_t *UserPubKey_ptr,
               uint8_t *Exponent_ptr,
               size_t   ExponentSize,
               uint8_t *Modulus_ptr,
               size_t ModulusSize )
{
    /* FUNCTION DECLARATIONS */

    /* the counter compare result */
    CCCommonCmpCounter_t CounterCmpResult;

    /* the effective size in bits of the modulus buffer */
    uint32_t ModulusEffectiveSizeInBits;

    /* the effective size in bits of the exponent buffer */
    uint32_t ExponentEffectiveSizeInBits;

    /* the public key database pointer */
    CCRsaPubKey_t *PubKey_ptr;

    /* the Error return code identifier */
    CCError_t Error = CC_OK;

    /* Max Size of buffers in Key structure */
    uint32_t  buffSizeBytes = CC_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_BYTES;

    /* FUNCTION LOGIC */
    /* ................. checking the validity of the pointer arguments ....... */
    /* ------------------------------------------------------------------------ */

    CHECK_AND_RETURN_ERR_UPON_FIPS_ERROR();

    /* ...... checking the key database handle pointer .................... */
    if (UserPubKey_ptr == NULL)
        return CC_RSA_INVALID_PUB_KEY_STRUCT_POINTER_ERROR;

    /* ...... checking the validity of the exponent pointer ............... */
    if (Exponent_ptr == NULL)
        return CC_RSA_INVALID_EXPONENT_POINTER_ERROR;

    /* ...... checking the validity of the modulus pointer .............. */
    if (Modulus_ptr == NULL)
        return CC_RSA_INVALID_MODULUS_POINTER_ERROR;

    if (ModulusSize > CC_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_BYTES)
        return CC_RSA_INVALID_MODULUS_SIZE;

    if (ExponentSize > CC_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_BYTES)
        return CC_RSA_INVALID_EXPONENT_SIZE;

    /* .................. copy the buffers to the key handle structure .... */
    /* -------------------------------------------------------------------- */
    /* setting the pointer to the key database */
    PubKey_ptr = ( CCRsaPubKey_t * )UserPubKey_ptr->PublicKeyDbBuff;

    /* clear the public key db */
    CC_PalMemSetZero( PubKey_ptr, sizeof(CCRsaPubKey_t) );

    /* loading the buffers to little endian order of words in array; each word is loaded according to CPU endianness */
    Error = CC_CommonConvertMsbLsbBytesToLswMswWords(PubKey_ptr->n, buffSizeBytes, Modulus_ptr, ModulusSize);
    if (Error != CC_OK) {
        Error = CC_RSA_INVALID_MODULUS_ERROR;
        return Error;
    }

    Error = CC_CommonConvertMsbLsbBytesToLswMswWords(PubKey_ptr->e, buffSizeBytes, Exponent_ptr, ExponentSize);
    if (Error != CC_OK) {
        Error = CC_RSA_INVALID_EXPONENT_VAL;
        goto End;
    }

    /* .................. initializing local variables ................... */
    /* ------------------------------------------------------------------- */

    /* .......... initializing the effective counters size in bits .......... */
    ModulusEffectiveSizeInBits =  CC_CommonGetWordsCounterEffectiveSizeInBits(PubKey_ptr->n, (ModulusSize+3)/4);
    ExponentEffectiveSizeInBits = CC_CommonGetWordsCounterEffectiveSizeInBits(PubKey_ptr->e, (ExponentSize+3)/4);

    /* .................. checking the validity of the counters ............... */
    /* ------------------------------------------------------------------------ */
    if ((ModulusEffectiveSizeInBits < CC_RSA_MIN_VALID_KEY_SIZE_VALUE_IN_BITS ) ||
        (ModulusEffectiveSizeInBits > CC_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_BITS ) ||
        (ModulusEffectiveSizeInBits % CC_RSA_VALID_KEY_SIZE_MULTIPLE_VALUE_IN_BITS)) {

        Error = CC_RSA_INVALID_MODULUS_SIZE;
        goto End;
    }
    /*  verifying the modulus is odd  */
    if ((PubKey_ptr->n[0] & 1UL) == 0) {
        Error = CC_RSA_MODULUS_EVEN_ERROR;
        goto End;
    }

    /*  checking the exponent size is not 0 in bytes */
    if (ExponentEffectiveSizeInBits == 0) {
        Error = CC_RSA_INVALID_EXPONENT_SIZE;
        goto End;
    }

    /*  verifying the exponent is less then the modulus */
    CounterCmpResult = CC_CommonCmpMsbUnsignedCounters(Exponent_ptr, ExponentSize, Modulus_ptr, ModulusSize);

    if (CounterCmpResult != CC_COMMON_CmpCounter2GreaterThenCounter1) {
        Error = CC_RSA_INVALID_EXPONENT_VAL;
        goto End;
    }

    /*  verifying the exponent is not less then 3 */
    if (ExponentEffectiveSizeInBits < 32 && PubKey_ptr->e[0] < CC_RSA_MIN_PUB_EXP_VALUE) {
        Error = CC_RSA_INVALID_EXPONENT_VAL;
        goto End;
    }

    /* ................. building the structure ............................. */
    /* ---------------------------------------------------------------------- */

    /* setting the modulus and exponent size in bits */
    PubKey_ptr->nSizeInBits = ModulusEffectiveSizeInBits;
    PubKey_ptr->eSizeInBits = ExponentEffectiveSizeInBits;

    /* ................ initialize the low level data .............. */
    Error = RsaInitPubKeyDb(PubKey_ptr);

    if (Error != CC_OK) {
        Error = CC_RSA_KEY_GENERATION_FAILURE_ERROR;
        goto End;
    }

    /* ................ set the tag ................ */
    UserPubKey_ptr->valid_tag = CC_RSA_PUB_KEY_VALIDATION_TAG;

    /* ................. end of the function .................................. */
    /* ------------------------------------------------------------------------ */

    End:

    /* if the structure created is not valid - clear it */
    if (Error != CC_OK) {
        CC_PalMemSetZero(UserPubKey_ptr, sizeof(CCRsaUserPubKey_t));
        return Error;
    }

    return CC_OK;

}/* END OF CC_RsaPubKeyBuild */


/******************************************************************************************/
/**
 * @brief CC_RsaPrivKeyBuild populates a CCRsaPrivKey_t structure with
 *        the provided modulus and exponent, marking the key as a "non-CRT" key.
 *
 *        Assumption : the modulus and the exponent are presented in big endian.
 *
 * @param[out] UserPrivKey_ptr - a pointer to the public key structure. this structure will be used as
 *                          an input to the CC_RsaPrimDecrypt API.
 * @param[in] PrivExponent_ptr - a pointer to the private exponent stream of bytes ( Big endian ).
 * @param[in] PrivExponentSize - the size of the private exponent in bytes.
 * @param[in] Exponent_ptr - a pointer to the exponent stream of bytes ( Big endian ).
 * @param[in] ExponentSize - the size of the exponent in bytes.
 * @param[in] Modulus_ptr  - a pointer to the modulus stream of bytes ( Big endian ) the MS
 *            bit must be set to '1'.
 * @param[in] ModulusSize  - the size of the modulus in bytes. Sizes supported according to
 *            used platform from 64 to 256 bytes and in some platforms up to 512 bytes.
 *
 * @return CCError_t - On success CC_OK is returned, on failure a
 *                       value MODULE_* as defined in ...
 *
 */
CEXPORT_C CCError_t CC_RsaPrivKeyBuild(CCRsaUserPrivKey_t       *UserPrivKey_ptr,
                         uint8_t             *PrivExponent_ptr,
                         size_t               PrivExponentSize,
                         uint8_t             *PubExponent_ptr,
                         size_t               PubExponentSize,
                         uint8_t             *Modulus_ptr,
                         size_t               ModulusSize )
{
    /* FUNCTION DECLARATIONS */

    /* the counter compare result */
    CCCommonCmpCounter_t CounterCmpResult;

    /* the effective size in bits of the modulus buffer */
    uint32_t ModulusEffectiveSizeInBits;

    /* the effective sizes in bits of the private and public exponents */
    uint32_t PrivExponentEffectiveSizeInBits, PubExponentEffectiveSizeInBits;

    /* the private key database pointer */
    CCRsaPrivKey_t *PrivKey_ptr;

    /* Max Size of buffers in Key structure */
    uint32_t  buffSizeBytes = CC_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_BYTES;

    /* the Error return code identifier */
    CCError_t Error = CC_OK;

    /* FUNCTION LOGIC */

    /* ................. checking the validity of the pointer arguments ....... */
    /* ------------------------------------------------------------------------ */
    CHECK_AND_RETURN_ERR_UPON_FIPS_ERROR();

    /* ...... checking the key database handle pointer .................... */
    if (UserPrivKey_ptr == NULL)
        return CC_RSA_INVALID_PRIV_KEY_STRUCT_POINTER_ERROR;

    /* ...... checking the validity of the exponents pointers ........... */
    if (PrivExponent_ptr == NULL)
        return CC_RSA_INVALID_EXPONENT_POINTER_ERROR;

    /* ...... checking the validity of the modulus pointer .............. */
    if (Modulus_ptr == NULL)
        return CC_RSA_INVALID_MODULUS_POINTER_ERROR;

    /* ...... checking the validity of the modulus size, private exponent can not be more than 256 bytes .............. */
    if (ModulusSize > CC_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_BYTES)
        return CC_RSA_INVALID_MODULUS_SIZE;

    if (PrivExponentSize > CC_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_BYTES)
        return CC_RSA_INVALID_EXPONENT_SIZE;

    if (PubExponent_ptr != NULL &&
        PubExponentSize > CC_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_BYTES)
        return CC_RSA_INVALID_EXPONENT_SIZE;


    /* .................. copy the buffers to the key handle structure .... */
    /* -------------------------------------------------------------------- */

    /* setting the pointer to the key database */
    PrivKey_ptr = (CCRsaPrivKey_t *)UserPrivKey_ptr->PrivateKeyDbBuff;

    /* clear the private key db */
    CC_PalMemSetZero(PrivKey_ptr, sizeof(CCRsaPrivKey_t));

    /* loading the buffers to little endian order of words in array; each word is loaded according to CPU endianness */
    Error = CC_CommonConvertMsbLsbBytesToLswMswWords(PrivKey_ptr->n, buffSizeBytes, Modulus_ptr, ModulusSize);
    if (Error != CC_OK) {
        Error = CC_RSA_INVALID_MODULUS_ERROR;
        return Error;
    }

    Error = CC_CommonConvertMsbLsbBytesToLswMswWords(PrivKey_ptr->PriveKeyDb.NonCrt.d, buffSizeBytes,
                                PrivExponent_ptr, PrivExponentSize);
    if (Error != CC_OK) {
        Error = CC_RSA_INVALID_EXPONENT_VAL;
        goto End;
    }

    /* get actual sizes of modulus and private exponent */
    ModulusEffectiveSizeInBits =
    CC_CommonGetWordsCounterEffectiveSizeInBits(PrivKey_ptr->n, (ModulusSize+3)/4);

    PrivExponentEffectiveSizeInBits =
    CC_CommonGetWordsCounterEffectiveSizeInBits(PrivKey_ptr->PriveKeyDb.NonCrt.d, (PrivExponentSize+3)/4);


    /* .................. checking the validity of the counters ............... */
    /* ------------------------------------------------------------------------ */

    /*  checking the size of the modulus  */
    if (( ModulusEffectiveSizeInBits < CC_RSA_MIN_VALID_KEY_SIZE_VALUE_IN_BITS ) ||
        ( ModulusEffectiveSizeInBits > CC_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_BITS ) ||
        ( ModulusEffectiveSizeInBits % CC_RSA_VALID_KEY_SIZE_MULTIPLE_VALUE_IN_BITS)) {
        Error = CC_RSA_INVALID_MODULUS_SIZE;
        goto End;
    }

    /*  verifying the modulus is odd  */
    if ((PrivKey_ptr->n[0] & 1UL) == 0) {
        Error = CC_RSA_MODULUS_EVEN_ERROR;
        goto End;
    }

    /*  checking the priv. exponent size is not 0 in bytes */
    if (PrivExponentEffectiveSizeInBits == 0) {
        Error = CC_RSA_INVALID_EXPONENT_SIZE;
        goto End;
    }

    /* verifying the priv. exponent is less then the modulus */
    CounterCmpResult = CC_CommonCmpMsbUnsignedCounters(PrivExponent_ptr, PrivExponentSize,
                                  Modulus_ptr, ModulusSize);

    if (CounterCmpResult != CC_COMMON_CmpCounter2GreaterThenCounter1) {
        Error = CC_RSA_INVALID_EXPONENT_VAL;
        goto End;
    }

    /* verifying the priv. exponent is not less then 1 */
    if (PrivExponentEffectiveSizeInBits < 32 &&
        PrivKey_ptr->PriveKeyDb.NonCrt.d[0] < CC_RSA_MIN_PRIV_EXP_VALUE) {
        Error = CC_RSA_INVALID_EXPONENT_VAL;
        goto End;
    }

    /*  checking that the public exponent is an integer between 3 and modulus - 1 */
    if (PubExponent_ptr != NULL) {
        /* loading the buffer to little endian order of words in array; each word is loaded according to CPU endianness */
        Error = CC_CommonConvertMsbLsbBytesToLswMswWords( PrivKey_ptr->PriveKeyDb.NonCrt.e, buffSizeBytes,
                                     PubExponent_ptr, PubExponentSize);
        if (Error) {
            Error = CC_RSA_INVALID_EXPONENT_VAL;
            goto End;
        }

        PubExponentEffectiveSizeInBits =
        CC_CommonGetWordsCounterEffectiveSizeInBits(PrivKey_ptr->PriveKeyDb.NonCrt.e, (PubExponentSize+3)/4);

        /* verifying that the exponent is not less than 3 */
        if (PubExponentEffectiveSizeInBits < 32 &&
            PrivKey_ptr->PriveKeyDb.NonCrt.e[0] < CC_RSA_MIN_PUB_EXP_VALUE) {
            Error = CC_RSA_INVALID_EXPONENT_VAL;
            goto End;
        }

        /* verifying that the public exponent is less than the modulus */
        CounterCmpResult = CC_CommonCmpMsbUnsignedCounters(PubExponent_ptr, PubExponentSize,
                                      Modulus_ptr, ModulusSize);

        if (CounterCmpResult != CC_COMMON_CmpCounter2GreaterThenCounter1) {
            Error = CC_RSA_INVALID_EXPONENT_VAL;
            goto End;
        }
    } else {
        PubExponentEffectiveSizeInBits = 0;
    }


    /* ................. building the structure ............................. */
    /* ---------------------------------------------------------------------- */

    /* set the mode to non CRT mode */
    PrivKey_ptr->OperationMode = CC_RSA_NoCrt;

    /* set the key source as external */
    PrivKey_ptr->KeySource = CC_RSA_ExternalKey;

    /* setting the modulus and exponent size in bits */
    PrivKey_ptr->nSizeInBits                   = ModulusEffectiveSizeInBits;
    PrivKey_ptr->PriveKeyDb.NonCrt.dSizeInBits = PrivExponentEffectiveSizeInBits;
    PrivKey_ptr->PriveKeyDb.NonCrt.eSizeInBits = PubExponentEffectiveSizeInBits;

    /* ................ initialize the low level data .............. */
    Error = RsaInitPrivKeyDb(PrivKey_ptr);

    if (Error) {
        Error = CC_RSA_INTERNAL_ERROR;
        goto End;
    }

    /* ................ set the tag ................ */
    UserPrivKey_ptr->valid_tag = CC_RSA_PRIV_KEY_VALIDATION_TAG;

    /* ................. end of the function .................................. */
    /* ------------------------------------------------------------------------ */

    End:

    /* if the structure created is not valid - clear it */
    if (Error != CC_OK) {
        CC_PalMemSetZero(UserPrivKey_ptr, sizeof(CCRsaUserPrivKey_t));
    }

    return Error;

}/* END OF CC_RsaPrivKeyBuild */

/******************************************************************************************

   @brief CC_RsaPrivKeyCrtBuild populates a CCRsaPrivKey_t structure with
      the provided parameters, marking the key as a "CRT" key.

    Note: The "First" factor P must be great, than the "Second" factor Q.


   @param[out] UserPrivKey_ptr - A pointer to the public key structure.
                This structure is used as input to the CC_RsaPrimDecrypt API.
   @param[in] P_ptr - A pointer to the first factor stream of bytes (Big-Endian format)
   @param[in] PSize - The size of the first factor, in bytes.
   @param[in] Q_ptr - A pointer to the second factor stream of bytes (Big-Endian format)
   @param[in] QSize - The size of the second factor, in bytes.
   @param[in] dP_ptr - A pointer to the first factor's CRT exponent stream of bytes (Big-Endian format)
   @param[in] dPSize - The size of the first factor's CRT exponent, in bytes.
   @param[in] dQ_ptr - A pointer to the second factor's CRT exponent stream of bytes (Big-Endian format)
   @param[in] dQSize - The size of the second factor's CRT exponent, in bytes.
   @param[in] qInv_ptr - A pointer to the first CRT coefficient stream of bytes (Big-Endian format)
   @param[in] qInvSize - The size of the first CRT coefficient, in bytes.

*/
CEXPORT_C CCError_t CC_RsaPrivKeyCrtBuild(
                           CCRsaUserPrivKey_t *UserPrivKey_ptr,
                           uint8_t *P_ptr,
                           size_t   PSize,
                           uint8_t *Q_ptr,
                           size_t   QSize,
                           uint8_t *dP_ptr,
                           size_t   dPSize,
                           uint8_t *dQ_ptr,
                           size_t   dQSize,
                           uint8_t *qInv_ptr,
                           size_t   qInvSize)
{
    /* FUNCTION DECLARATIONS */

    /* the counter compare result */
    CCCommonCmpCounter_t CounterCmpResult;

    /* the effective size in bits of the modulus factors buffer */
    uint32_t P_EffectiveSizeInBits;
    uint32_t Q_EffectiveSizeInBits;
    uint32_t dP_EffectiveSizeInBits;
    uint32_t dQ_EffectiveSizeInBits;
    uint32_t qInv_EffectiveSizeInBits;
    uint32_t ModulusEffectiveSizeInBits;

    /* the private key database pointer */
    CCRsaPrivKey_t *PrivKey_ptr;

    /* Max Size of buffers in CRT Key structure */
    uint32_t  buffSizeBytes;

    /* the Error return code identifier */
    CCError_t Error = CC_OK;

    /* FUNCTION LOGIC */


    /* ................. checking the validity of the pointer arguments ....... */
    /* ------------------------------------------------------------------------ */
    CHECK_AND_RETURN_ERR_UPON_FIPS_ERROR();

    /* ...... checking the key database handle pointer .................... */
    if (UserPrivKey_ptr == NULL)
        return CC_RSA_INVALID_PRIV_KEY_STRUCT_POINTER_ERROR;

    /* checking the first factor pointer validity */
    if (P_ptr == NULL)
        return CC_RSA_INVALID_CRT_FIRST_FACTOR_POINTER_ERROR;

    /* checking the second factor pointer validity */
    if (Q_ptr == NULL)
        return CC_RSA_INVALID_CRT_SECOND_FACTOR_POINTER_ERROR;

    /* checking the first factor exponent pointer validity */
    if (dP_ptr == NULL)
        return CC_RSA_INVALID_CRT_FIRST_FACTOR_EXP_PTR_ERROR;

    /* checking the second factor exponent pointer validity */
    if (dQ_ptr == NULL)
        return CC_RSA_INVALID_CRT_SECOND_FACTOR_EXP_PTR_ERROR;

    /* checking the CRT coefficient */
    if (qInv_ptr == NULL)
        return CC_RSA_INVALID_CRT_COEFFICIENT_PTR_ERROR;

    /* checking the input sizes */
    if (PSize > CC_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_BYTES/2 ||
        QSize > CC_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_BYTES/2) {
        return CC_RSA_INVALID_CRT_PARAMETR_SIZE_ERROR;
    }

    if (dPSize > PSize ||
        dQSize > QSize ||
        qInvSize > PSize) {
        return CC_RSA_INVALID_CRT_PARAMETR_SIZE_ERROR;
    }

    buffSizeBytes = 4*((PSize + 3)/4) + 4;
    /* verifying the first factor exponent is less then the first factor */
    CounterCmpResult =
    CC_CommonCmpMsbUnsignedCounters(dP_ptr, dPSize, P_ptr, PSize);

    if (CounterCmpResult != CC_COMMON_CmpCounter2GreaterThenCounter1) {
        return CC_RSA_INVALID_CRT_FIRST_FACTOR_EXPONENT_VAL;
    }

    /* verifying the second factor exponent is less then the second factor */
    CounterCmpResult =
    CC_CommonCmpMsbUnsignedCounters(dQ_ptr, dQSize, Q_ptr, QSize);

    if (CounterCmpResult != CC_COMMON_CmpCounter2GreaterThenCounter1) {
        return CC_RSA_INVALID_CRT_SECOND_FACTOR_EXPONENT_VAL;
    }

    /* verifying the CRT coefficient is less then the first factor */
    CounterCmpResult =
    CC_CommonCmpMsbUnsignedCounters(qInv_ptr, qInvSize, P_ptr, PSize);

    if (CounterCmpResult != CC_COMMON_CmpCounter2GreaterThenCounter1) {
        return CC_RSA_INVALID_CRT_COEFF_VAL;
    }


    /* .................. copy the buffers to the key handle structure .... */
    /* -------------------------------------------------------------------- */

    /* setting the pointer to the key database */
    PrivKey_ptr = (CCRsaPrivKey_t*)UserPrivKey_ptr->PrivateKeyDbBuff;

    /* clear the private key db */
    CC_PalMemSetZero(PrivKey_ptr, sizeof(CCRsaPrivKey_t));

    /* load the buffers to the data base */
    Error = CC_CommonConvertMsbLsbBytesToLswMswWords(PrivKey_ptr->PriveKeyDb.Crt.P, buffSizeBytes, P_ptr, PSize);
    if (Error != CC_OK) {
        Error = CC_RSA_INVALID_CRT_FIRST_FACTOR_SIZE_ERROR;
        goto End;
    }

    Error = CC_CommonConvertMsbLsbBytesToLswMswWords(PrivKey_ptr->PriveKeyDb.Crt.Q, buffSizeBytes, Q_ptr, QSize);
    if (Error != CC_OK) {
        Error = CC_RSA_INVALID_CRT_SECOND_FACTOR_SIZE_ERROR;
        goto End;
    }

    Error = CC_CommonConvertMsbLsbBytesToLswMswWords(PrivKey_ptr->PriveKeyDb.Crt.dP, buffSizeBytes, dP_ptr, dPSize);
    if (Error != CC_OK) {
        Error = CC_RSA_INVALID_CRT_FIRST_FACTOR_EXP_SIZE_ERROR;
        goto End;
    }

    Error = CC_CommonConvertMsbLsbBytesToLswMswWords(PrivKey_ptr->PriveKeyDb.Crt.dQ, buffSizeBytes, dQ_ptr, dQSize);
    if (Error != CC_OK) {
        Error = CC_RSA_INVALID_CRT_SECOND_FACTOR_EXP_SIZE_ERROR;
        goto End;
    }

    Error = CC_CommonConvertMsbLsbBytesToLswMswWords(PrivKey_ptr->PriveKeyDb.Crt.qInv, buffSizeBytes, qInv_ptr, qInvSize);
    if (Error != CC_OK) {
        Error = CC_RSA_INVALID_CRT_COEFFICIENT_SIZE_ERROR;
        goto End;
    }

    /* ............... initialize local variables ......................... */
    /* -------------------------------------------------------------------- */

    /* initializing the effective counters size in bits */
    P_EffectiveSizeInBits =
    CC_CommonGetWordsCounterEffectiveSizeInBits(PrivKey_ptr->PriveKeyDb.Crt.P, (PSize+3)/4);

    Q_EffectiveSizeInBits =
    CC_CommonGetWordsCounterEffectiveSizeInBits(PrivKey_ptr->PriveKeyDb.Crt.Q, (QSize+3)/4);

    dP_EffectiveSizeInBits =
    CC_CommonGetWordsCounterEffectiveSizeInBits(PrivKey_ptr->PriveKeyDb.Crt.dP, (dPSize+3)/4);

    dQ_EffectiveSizeInBits =
    CC_CommonGetWordsCounterEffectiveSizeInBits(PrivKey_ptr->PriveKeyDb.Crt.dQ, (dQSize+3)/4);

    qInv_EffectiveSizeInBits =
    CC_CommonGetWordsCounterEffectiveSizeInBits(PrivKey_ptr->PriveKeyDb.Crt.qInv, (qInvSize+3)/4);

    /*  the first factor size is not 0 in bits */
    if (P_EffectiveSizeInBits == 0|| P_EffectiveSizeInBits > 8*PSize) {
        Error = CC_RSA_INVALID_CRT_FIRST_FACTOR_SIZE;
        goto End;
    }

    /* the second factor size is not 0 in bits */
    if (Q_EffectiveSizeInBits == 0 || Q_EffectiveSizeInBits > 8*QSize) {
        Error = CC_RSA_INVALID_CRT_SECOND_FACTOR_SIZE;
        goto End;
    }

    /* checking that sizes of dP, dQ, qInv > 0 */
    if (dP_EffectiveSizeInBits == 0 || dQ_EffectiveSizeInBits == 0 || qInv_EffectiveSizeInBits == 0) {
        Error = CC_RSA_INVALID_CRT_PARAMETR_SIZE_ERROR;
        goto End;
    }



    /* ............... calculate the modulus N ........................... */
    /* -------------------------------------------------------------------- */


    Error = PkiLongNumMul(PrivKey_ptr->PriveKeyDb.Crt.P, P_EffectiveSizeInBits,
                     PrivKey_ptr->PriveKeyDb.Crt.Q, PrivKey_ptr->n);
    if (Error != CC_OK) {
        Error = CC_RSA_INTERNAL_ERROR;
        goto End;
    }

    ModulusEffectiveSizeInBits =
    CC_CommonGetWordsCounterEffectiveSizeInBits(PrivKey_ptr->n, (2*CALC_FULL_32BIT_WORDS(P_EffectiveSizeInBits)));

    /* .................. checking the validity of the counters ............... */
    /* ------------------------------------------------------------------------ */

    /* the size of the modulus  */
    if (( ModulusEffectiveSizeInBits < CC_RSA_MIN_VALID_KEY_SIZE_VALUE_IN_BITS) ||
        ( ModulusEffectiveSizeInBits > CC_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_BITS) ||
        ( ModulusEffectiveSizeInBits % CC_RSA_VALID_KEY_SIZE_MULTIPLE_VALUE_IN_BITS)) {
        Error = CC_RSA_INVALID_MODULUS_SIZE;
        goto End;
    }

    if ((P_EffectiveSizeInBits + Q_EffectiveSizeInBits != ModulusEffectiveSizeInBits) &&
        (P_EffectiveSizeInBits + Q_EffectiveSizeInBits != ModulusEffectiveSizeInBits - 1)) {
        Error = CC_RSA_INVALID_CRT_FIRST_AND_SECOND_FACTOR_SIZE;
        goto End;
    }


    /* ................. building the structure ............................. */
    /* ---------------------------------------------------------------------- */

    /* set the mode to CRT mode */
    PrivKey_ptr->OperationMode = CC_RSA_Crt;

    /* set the key source as external */
    PrivKey_ptr->KeySource = CC_RSA_ExternalKey;

    /* loading to structure the buffer sizes... */

    PrivKey_ptr->PriveKeyDb.Crt.PSizeInBits    = P_EffectiveSizeInBits;
    PrivKey_ptr->PriveKeyDb.Crt.QSizeInBits    = Q_EffectiveSizeInBits;
    PrivKey_ptr->PriveKeyDb.Crt.dPSizeInBits   = dP_EffectiveSizeInBits;
    PrivKey_ptr->PriveKeyDb.Crt.dQSizeInBits   = dQ_EffectiveSizeInBits;
    PrivKey_ptr->PriveKeyDb.Crt.qInvSizeInBits = qInv_EffectiveSizeInBits;
    PrivKey_ptr->nSizeInBits = ModulusEffectiveSizeInBits;

    /* ................ initialize the low level data .............. */
    Error = RsaInitPrivKeyDb(PrivKey_ptr);

    if (Error != CC_OK) {
        Error = CC_RSA_INTERNAL_ERROR;
        goto End;
    }

    /* ................ set the tag ................ */
    UserPrivKey_ptr->valid_tag = CC_RSA_PRIV_KEY_VALIDATION_TAG;

    /* ................. end of the function .................................. */
    /* ------------------------------------------------------------------------ */

    End:

    /* if the structure created is not valid - clear it */
    if (Error != CC_OK) {
        CC_PalMemSetZero(UserPrivKey_ptr, sizeof(CCRsaUserPrivKey_t));
        return Error;
    }

    return Error;

}/* END OF CC_RsaPrivKeyCrtBuild */


/******************************************************************************************
   @brief CC_RsaPubKeyGet gets the e,n public key from the database.

   @param[in] UserPubKey_ptr - A pointer to the public key structure.
                   This structure is used as input to the CC_RsaPrimEncrypt API.

   @param[out] Exponent_ptr - A pointer to the exponent stream of bytes (Big-Endian format)
   @param[in,out] ExponentSize_ptr - the size of the exponent buffer in bytes, it is updated to the
          actual size of the exponent, in bytes.
   @param[out] Modulus_ptr  - A pointer to the modulus stream of bytes (Big-Endian format).
               The MS (most significant) bit must be set to '1'.
   @param[in,out] ModulusSize_ptr  - the size of the modulus buffer in bytes, it is updated to the
          actual size of the modulus, in bytes.

   NOTE: All members of input UserPrivKey structure must be initialized, including public key
     e pointer and it size.

*/
CEXPORT_C CCError_t CC_RsaPubKeyGet(
                     CCRsaUserPubKey_t *UserPubKey_ptr,
                     uint8_t  *Exponent_ptr,
                     size_t   *ExponentSize_ptr,
                     uint8_t  *Modulus_ptr,
                     size_t   *ModulusSize_ptr )
{
    /* LOCAL DECLERATIONS */

    /* the size in bytes of the modulus and the exponent */
    uint32_t nSizeInBytes;
    uint32_t eSizeInBytes;
    /* the public key database pointer */
    CCRsaPubKey_t *PubKey_ptr;

    CCError_t Error;

    /* FUNCTION DECLERATIONS */


    /* ................. checking the validity of the pointer arguments ....... */
    /* ------------------------------------------------------------------------ */
    CHECK_AND_RETURN_ERR_UPON_FIPS_ERROR();

    /* ...... checking the key database handle pointer .................... */
    if (UserPubKey_ptr == NULL)
        return CC_RSA_INVALID_PUB_KEY_STRUCT_POINTER_ERROR;

    /* ...... checking the validity of the exponent pointer ............... */
    if (Exponent_ptr == NULL && Modulus_ptr != NULL)
        return CC_RSA_INVALID_EXPONENT_POINTER_ERROR;

    /* ...... checking the validity of the modulus pointer .............. */
    if (Modulus_ptr == NULL && Exponent_ptr != NULL)
        return CC_RSA_INVALID_MODULUS_POINTER_ERROR;

    if (ExponentSize_ptr == NULL)
        return CC_RSA_INVALID_EXP_BUFFER_SIZE_POINTER;

    if (ModulusSize_ptr == NULL)
        return CC_RSA_INVALID_MOD_BUFFER_SIZE_POINTER;

    /* if the users TAG is illegal return an error - the context is invalid */
    if (UserPubKey_ptr->valid_tag != CC_RSA_PUB_KEY_VALIDATION_TAG)
        return CC_RSA_PUB_KEY_VALIDATION_TAG_ERROR;

    /* ...... checking the exponent size ................................ */

    /* setting the pointer to the key database */
    PubKey_ptr = ( CCRsaPubKey_t * )UserPubKey_ptr->PublicKeyDbBuff;

    /* calculating the required size in bytes */
    nSizeInBytes = CALC_FULL_BYTES(PubKey_ptr->nSizeInBits);
    eSizeInBytes = CALC_FULL_BYTES(PubKey_ptr->eSizeInBits);

    /* return the modulus size and exit */
    if (Exponent_ptr == NULL && Modulus_ptr == NULL){
        *ModulusSize_ptr  = nSizeInBytes;
        *ExponentSize_ptr = eSizeInBytes;
        return CC_OK;
    }
    /* if the size of the modulus is to small return error */
    if (nSizeInBytes > *ModulusSize_ptr)
        return CC_RSA_INVALID_MODULUS_SIZE;

    /* if the size of the exponent buffer is to small return error */
    if (eSizeInBytes > *ExponentSize_ptr)
        return CC_RSA_INVALID_EXPONENT_SIZE;

    /* .............. loading the output arguments and buffers ............... */
    /* ----------------------------------------------------------------------- */

    /* loading the buffers */

    Error = CC_CommonConvertLswMswWordsToMsbLsbBytes(Exponent_ptr, 4*((*ExponentSize_ptr+3)/4),
                                PubKey_ptr->e, eSizeInBytes );
    if (Error != CC_OK) {
        Error = CC_RSA_INVALID_EXPONENT_SIZE;
        goto End;
    }

    Error = CC_CommonConvertLswMswWordsToMsbLsbBytes(Modulus_ptr, 4*((*ModulusSize_ptr+3)/4),
                                PubKey_ptr->n, nSizeInBytes );
    if (Error != CC_OK) {
        Error = CC_RSA_INVALID_MODULUS_SIZE;
        goto End;
    }

    /* updating the buffer sizes */
    *ModulusSize_ptr  = (uint16_t)nSizeInBytes;
    *ExponentSize_ptr = (uint16_t)eSizeInBytes;

End:
    if (Error != CC_OK) {
        CC_PalMemSetZero(Modulus_ptr, nSizeInBytes);
        CC_PalMemSetZero(Exponent_ptr, eSizeInBytes);
        return Error;
    }

    return CC_OK;

}/* END OF CC_RsaPubKeyGet */

#endif /* !defined(CC_IOT) || ( defined(CC_IOT) && defined(MBEDTLS_RSA_C)) */
