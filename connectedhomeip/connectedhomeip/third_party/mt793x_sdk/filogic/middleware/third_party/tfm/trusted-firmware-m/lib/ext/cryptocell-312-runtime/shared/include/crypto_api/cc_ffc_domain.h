/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _CC_FFC_DOMAIN_H
#define _CC_FFC_DOMAIN_H

#include "cc_pka_defs_hw.h"
#include "cc_pal_types.h"
#include "cc_pal_compiler.h"
#include "cc_hash_defs.h"
#include "cc_rnd_common.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*! @file
@brief This file defines the API that supports FFC Domain generation domain.
@defgroup cc_ffcdh_domain CryptoCell FFC Domain Generation APIs
@{
@ingroup cc_ffc_domain

*/

/************************ Defines ******************************/

/*! Maximal prime P (modulus) size .*/
#define CC_FFC_DOMAIN_MAX_MOD_SIZE_IN_BITS    2048  /* 3072 - for FFC DSA FIPS 186-4 sec. 4.2 . */
#define CC_FFC_DOMAIN_MAX_MOD_SIZE_IN_BYTES   (CC_FFC_DOMAIN_MAX_MOD_SIZE_IN_BITS / CC_BITS_IN_BYTE)
#define CC_FFC_DOMAIN_MAX_MOD_SIZE_IN_WORDS   (CC_FFC_DOMAIN_MAX_MOD_SIZE_IN_BYTES/CC_32BIT_WORD_SIZE)
/*! Minimal valid key size in bits.*/
#define CC_FFC_DOMAIN_MIN_VALID_MOD_SIZE_VALUE_IN_BITS  1024 /*!< Size limitation according the same standard */

/*! Prime P (modulus) buffer size in words.*/
#define CC_FFC_DOMAIN_MAX_MOD_BUFFER_SIZE_IN_WORDS   (CC_FFC_DOMAIN_MAX_MOD_SIZE_IN_WORDS + 2)
#define CC_FFC_DOMAIN_MAX_MOD_BUFFER_SIZE_IN_BYTES   (CC_FFC_DOMAIN_MAX_MOD_BUFFER_SIZE_IN_WORDS * CC_32BIT_WORD_SIZE)

/*! Maximal FFC subgroup order size. */
#define CC_FFC_DOMAIN_MAX_GENER_ORDER_SIZE_IN_BITS   256
#define CC_FFC_DOMAIN_MAX_GENER_ORDER_SIZE_IN_BYTES  (CC_FFC_DOMAIN_MAX_GENER_ORDER_SIZE_IN_BITS / CC_BITS_IN_BYTE)
#define CC_FFC_DOMAIN_MAX_GENER_ORDER_SIZE_IN_WORDS  (CC_FFC_DOMAIN_MAX_GENER_ORDER_SIZE_IN_BITS / CC_BITS_IN_32BIT_WORD)
/*!< Maximal size of buffer for generator order (added 2 words for internal using) */
#define CC_FFC_DOMAIN_MAX_GENER_ORDER_BUFF_SIZE_IN_WORDS (CC_FFC_DOMAIN_MAX_GENER_ORDER_SIZE_IN_WORDS + 2)

/*! Minimal and maximal sizes of FFC Seed in bytes. */
#define CC_FFC_DOMAIN_SEED_MIN_SIZE_IN_BYTES    CC_HASH_SHA1_DIGEST_SIZE_IN_BYTES
#define CC_FFC_DOMAIN_SEED_MAX_SIZE_IN_BYTES    CC_HASH_SHA512_DIGEST_SIZE_IN_BYTES
/*! Minimal size of FFC seed in bits. */
#define CC_FFC_DOMAIN_SEED_MIN_SIZE_IN_BITS     (CC_FFC_DOMAIN_SEED_MIN_SIZE_IN_BYTES * 8)

/*! The size of the buffer for User ID */
//#define CC_FFCDH_USER_ID_SIZE_IN_BYTES  8
/*! Buffer for Barrett Tag - special value, used in modular multiplication */
#define CC_FFC_DOMAIN_BARRETT_TAG_MAX_SIZE_IN_WORDS  5
#define CC_FFC_DOMAIN_BARRETT_TAG_MAX_SIZE_IN_BYTES  (CC_FFC_DOMAIN_BARRETT_TAG_MAX_SIZE_IN_WORDS * CC_32BIT_WORD_SIZE)


/* Macros for checking and return errors */
#define CHECK_ERROR(err)  if((err)) goto End
#define CHECK_AND_SET_ERROR(expr, errMsg)  if((expr)) {err = (errMsg); goto End;}
#define CHECK_AND_RETURN_ERROR(expr, errMsg)  if((expr)) {err = (errMsg); return err;}

/* check that ptr != NULL and outSize <= buffSize */
#define CHECK_PTR_AND_SIZE(pOut, outSize, buffSize) { \
    if((pOut == NULL) err = CC_FFCDH_INVALID_ARGUMENT_POINTER_ERROR; goto End; \
    if((outSize > buffSize) err = CC_FFCDH_INVALID_ARGUMENT_SIZE_ERROR; goto End; \
}

/* primality testing definitions */
#define CC_FFC_PRIME_TEST_MODE CC_DH_PRIME_TEST_MODE
#define CCFfcPrimeTestMode_t CCRsaDhPrimeTestMode_t

/*!< the DH Domain user validity TAG */
#define CC_FFC_DOMAIN_VALIDATION_TAG   0xFFCD8000

#define CC_FFC_DOMAIN_TMP_BUFF_SIZE_IN_WORDS  \
    (5*CC_FFC_DOMAIN_MAX_GENER_ORDER_SIZE_IN_WORDS + 3*CC_FFC_DOMAIN_MAX_MOD_BUFFER_SIZE_IN_WORDS + 3)


/************************ Enums ********************************/
/*! HASH operation modes. */
typedef enum
{
    CC_FFC_HASH_SHA1_MODE    = 0,
    CC_FFC_HASH_SHA224_MODE  = 1,
    CC_FFC_HASH_SHA256_MODE  = 2,
    CC_FFC_HASH_SHA384_MODE  = 3,
    CC_FFC_HASH_SHA512_MODE  = 4,
    CC_FFC_HASH_NUM_OFF_MODE,
    CC_FFC_HASH_OP_MODE_LAST = 0x7FFFFFFF
}CCFfcHashOpMode_t;


/*! FFC DH Domain validation mode definitions:
    NIST SP 56A Rev. 2, */
typedef enum {
    CC_FFC_DOMAIN_VALIDAT_FULL_MODE,    /*!< full validation */
    CC_FFC_DOMAIN_TRUSTED_DATA_MODE,    /*!< minimal checking: sizes and pointers;
                                                   this mode may be used on user's responsibility and
                                                   only when he obtains full assurance about Domain data */
        CC_FFC_DOMAIN_VALIDAT_NUM_OFF_MODE, /*!< not allowed value */
        CC_FFC_DOMAIN_VALIDAT_MODE_LAST = 0x7FFFFFFF
} CCFfcDomainValidMode_t;

/*! FFC DH Domain parameters sets definition: NIST SP 56A Rev. 2, sec. 5.8.1, tab.6. */
typedef enum
{
    /* domain sets according to SP 800-56A rev.2. */
        CC_FFC_PARAMS_SET_FA,  /*!< FA - min. parameters sizes and security strength */
        CC_FFC_PARAMS_SET_FB,  /*!< FB - middle 1 */
        CC_FFC_PARAMS_SET_FC,  /*!< FC - middle 2 (max.sizes allowed for FFC-DH) */
        /*!< DSA - added for FFC-DSA allowed sizes according to FIPS 186-4 sec.4.2, */
        CC_FFC_PARAMS_ADD_SET_DSA, /*!< max sizes (allowed for FFC-DSA, not standard for FFC-DH) */
        CC_FFC_PARAMS_SET_NUM_OFF_MODE,  /*!< not allowed value */
        CC_FFC_PARAMS_SET_LAST = 0x7FFFFFFF
} CCFfcParamSetId_t;

/*! FFC DH Domain parameters sets definition: NIST SP 56A Rev. 2, sec. 5.8.1, tab.6. */
typedef enum
{
    /* domain sets according to SP 800-56A rev.2. */
        CC_FFC_USE_GIVEN_SEED,    /*!< generate domain from given Seed */
        CC_FFC_GENERATE_NEW_SEED, /*!< generate new seed and Domain */
        CC_FFC_SEED_NOT_USED,     /*!< seed not used in appropriate function. */
        CC_FFC_GEN_SEED_NUM_OFF_MODE,  /*!< not allowed value */
        CC_FFC_GEN_SEED_NUM_LAST = 0x7FFFFFFF
} CCFfcGenerateSeed_t;


/************************ Typedefs  ****************************/

/* temp buffers, used in different DH KG functions */

/*! Temporary data buffer structure for domain parameters generation in DH. */
typedef struct CCFfcDomainTmpBuff_t
{
    /* The aligned input and output temp buffers */
    /*! Temporary buffer. */
    uint32_t TmpBuff[CC_FFC_DOMAIN_TMP_BUFF_SIZE_IN_WORDS];
}CCFfcDomainTmpBuff_t;


/**************************************************************/
/*! FFC Domain parameters structure (p,q,g,{seed,genCounter}.
 *  NIST SP 800-56A sec.5.5.1.1. Max. size of structure:
 *  2*(MaxModSize + MaxOrderSize) + 5w*4 + 40bytes = 636 bytes */
typedef struct CCFfcDomain_t {

    uint32_t prime[CC_FFC_DOMAIN_MAX_MOD_SIZE_IN_WORDS]; /*!< prime modulus. */
    uint32_t modLenWords;  /*!< prime modulus size in bytes  */
    uint32_t genG[CC_FFC_DOMAIN_MAX_MOD_SIZE_IN_WORDS];  /*!< FFC sub-group generator */
    uint32_t order[CC_FFC_DOMAIN_MAX_GENER_ORDER_SIZE_IN_WORDS]; /*!< order of FFC sub-group */
    uint32_t ordLenWords;  /*!< group order size in bytes */
    uint8_t  seed[CC_FFC_DOMAIN_MAX_GENER_ORDER_SIZE_IN_BYTES]; /*!< seed for domain generation and validation */
    uint32_t seedSizeBytes; /*!< seed size in bytes */
    uint32_t barrettTag[CC_FFC_DOMAIN_BARRETT_TAG_MAX_SIZE_IN_WORDS]; /*!< buffer for Barrett Tag - special value, used in
                                                                      modular reduction and multiplication. */
    uint32_t genCounter;             /*!< count of iterations, needed for successful domain generation */
    CCFfcParamSetId_t ffcParamSetId; /*!< enumerator, defining the set of FFC domain parameters
                                          according to SP 56A rev.2 section 5.5.1.1, tab.1. */
    CCFfcHashOpMode_t ffcHashMode;   /*!< enumerator ID of HASH mode, chosen for domain generation.
                      Note: HASH SHA1 function allowed only for SA set of domain parameters. */
    uint32_t hashDigestSize;         /*!< size in bytes of HASH digest for chosen mode. */
    uint32_t hashBlockSize;          /*!< size in bytes of HASH block for chosen mode. */
    uint32_t indexOfGenerator;       /*!< index, of currently created FFC Generator (allows create different
                                          Generators for existed prime P, Order Q, and Seed). */
    uint32_t validTag;  /*!< validation tag.*/
}CCFfcDomain_t;

#define FFC_DOMAIN_SIZE_BYTES  ((2*CC_FFC_DOMAIN_MAX_MOD_SIZE_IN_WORDS + 2*CC_FFC_DOMAIN_MAX_GENER_ORDER_SIZE_IN_WORDS + \
        CC_FFC_DOMAIN_BARRETT_TAG_MAX_SIZE_IN_WORDS + 10) * CC_32BIT_WORD_SIZE)

/***************************************************************************/
/*!< Set of FFC Domain parameters size approved by NIST SP 800-56A rev.2. tab.6,8
 *   and FIPS 186-4
     Intended for initialisation of array of structures of following type.
     Note: Bit-size of each parameters = 8*ByteSize.
 */
typedef struct CCFfcDomainParamSizes_t
{
    uint32_t maxSecurStrength;  /*!< Maximum security strength supported, in bytes. */
    uint32_t primeSize;         /*!< Field (prime P) size in bytes. */
    uint32_t orderSize;         /*!< Subgroup order Q size in bytes. */
    uint32_t minHashLen;        /*!< Minimum length of HASH output in bytes. */
} CCFfcDomainParamSizes_t;


/*!< Set of DH FFC parameters sizes, approved by NIST SP 800-56A rev.2: sec. 5.8.1, 5.9.3.
     Intended for initialization of array of structures of type CCFfcDhFfcDomainParamSizes_t.
     All sizes are given in bytes (see CCFfcDomainParamSizes_t struct).
     \note Index of array is given according to CCFfcDhFfcParamsSetId_t enumerator:
         {CC_FFCDH_PARAMS_SET_FA, CC_FFCDH_PARAMS_SET_FB, CC_FFCDH_PARAMS_SET_FC} = {0,1,2}.
*/
#define CC_FFC_DOMAIN_PARAM_SIZES_SET  {{80,1024,160,80},{112,2048,224,112},{112,2048,256,112}}
/*! Define and init parameters array */
//CCFfcDomainParamSizes_t ffcDomainParamSizes[(uint32_t)CC_FFC_DOMAIN_PARAMS_SET_NUM_OFF_MODE] = CC_FFC_DOMAIN_PARAM_SIZES_SET;

/*! Array of allowed HASH SHA-x block and digest sizes for all SHA modes (size in bytes).
   \note Index of array is according to CCFfcDhParamsSetId_t enumerator: {CC_HASH_SHA1_mode, CC_HASH_SHA224_mode, CC_HASH_SHA256_mode,
   CC_HASH_SHA384_mode, CC_HASH_SHA512_mode} = {0,1,2,3,4}.
 */
#define CC_FFC_SHA_PARAMETERS_SIZES_IN_BYTES {{64,20},{64,28},{64,32},{128,48},{128,64}}

/*! Define and initialize HASH parameters array */
//CCFfcDhHashBlockAndDigestSizes_t DhHashBlockAndDigestSizes[(uint32_t)CC_FFCDH_HASH_NUM_OFF_MODE] =
//                           CC_FFC_SHA_PARAMETERS_SIZES_IN_BYTES;

//
///*! Temporary buffer structure . */
//typedef struct CCFfcDhKgCheckTemp_t
//{
//  /*! Temporary buffer. */
//  uint32_t   checkTempBuff[3*CC_FFC_DOMAIN_MAX_MOD_SIZE_IN_WORDS];
//  /*! Temporary buffer. */
//  CCFfcDomainTmpBuff_t    domainBuff;
//}CCFfcDomainCheckTemp_t;


/************************ Structs  ******************************/

/************************ Public Variables **********************/

/************************ Public Functions **********************/

/*******************************************************************************************/
/*!
@brief This function generates FFC domain parameters according to NIST SP 56A rev.2, referring to FIPS 184-4 standard.
\par<ol><li>
<li> The function generates FFC Domain from given Seed and iterations count and sets them into Domain structure.
If actual count of iterations is not equalled to given value, then the function returns an error. </li>
<li> The function calculates prime modulus P, subgroup generator G with order Q using Seed and given Generator
index, allowing to generate different FFC generators with same P and Q, according to SP 56A rev.2 sec.5.5.1.1
and FIPS 184-4 A.1.1.2, A.2.3. </li>
<li> The function allows generation domains only for approved set of parameters sizes (SP 56A rev.2 5.5.1.1),
given by enumerator ID of type CCFfcDhParamSetId_t. </li></ol>
@return CC_OK on success.
@return A non-zero value on failure as defined cc_ffc_domain_error.h.

*/
CIMPORT_C CCError_t CC_FfcGenerateDomainFromSeed(
                CCFfcDomain_t *pDomain,        /*!< [out] pointer to  FFC Domain structure. */
                CCRndContext_t *pRndContext,   /*!< [in] random generation function context. */
                size_t primeSizeBits,          /*!< [in] size of domain's prime modulus in bits (see requirements above). */
                size_t orderSizeBits,          /*!< [in] size of domain's sub-group order in bits (see requirements above). */
                uint8_t  *pSeed,               /*!< [in] pointer to the seed for domain generation and validation; */
                size_t   seedSizeBytes,        /*!< [in] seed size in bytes */
                uint32_t genCounter,           /*!< [in] exact value of count of main loop iterations, required for generation
                                     FFC Domain from given Seed. If actual count is not equal to given,
                                     then the function returns an error. */
                CCFfcParamSetId_t ffcParamSetId,/*!< [in] enumerator, defining the set of FFC domain parameters
                                     according to SP 56A rev.2 section 5.5.1.1, tab.1. */
                CCFfcHashOpMode_t ffcHashMode, /*!< [in] enumerator ID of SHAx HASH mode. Note: HASH SHA1 mode may be
                                     used only with SA set of domain parameters (sec. 5.8.1, tab.6). */
                uint8_t generIndex,            /*!< [in] an index of FFC Generator,  allowing to generate different FFC generators with
                                    the same FFC parameters prime P and Order Q, existed in the domain. */
                CCFfcDomainTmpBuff_t *pTmpBuff /*!< [in] pointer to FFC Domain temp buffer structure. */
);

/*******************************************************************************************/
/*!
@brief This function generates FFC Domain parameters including new Seed Seed according to
 NIST SP 56A rev.2 with referring to FIPS 184-4 standard.
\par<ol><li>
<li> The function generates a new Seed, calculates FFC Domain parameters and sets them into Domain. </li>
<li> The function calculates prime modulus P, subgroup generator G with order Q using Seed and given Generator
index, allowing to generate different FFC generators with same P and Q, according to SP 56A rev.2 sec.5.5.1.1
and FIPS 184-4 A.1.1.2, A.2.3. </li>
<li> The function allows generation Domain only for approved set of parameters sizes (SP 56A rev.2 5.5.1.1),
given by enumerator ID of type CCFfcDhParamSetId_t. </li></ol>
@return CC_OK on success.
@return A non-zero value on failure as defined cc_dh_error.h, cc_rnd_error.h.
 */
CIMPORT_C CCError_t CC_FfcGenerateDomainAndSeed(
                CCFfcDomain_t *pDomain,         /*!< [out] pointer to  FFC Domain structure. */
                CCRndContext_t *pRndContext,    /*!< [in] random generation function context. */
                size_t primeSizeBits,           /*!< [in] size of domain's prime modulus in bits (see requirements above). */
                size_t orderSizeBits,           /*!< [in] size of domain's sub-group order in bits (see requirements above). */
                size_t seedSizeBytes,           /*!< [in] required size of the seed in bytes; it must be not less than
                                                     HASH security strength, defined in given ffcParamsSet. */
                CCFfcParamSetId_t ffcParamSetId,/*!< [in] enumerator, defining the set of FFC domain parameters
                                                     according to SP 56A rev.2 section 5.5.1.1, tab.1. */
                CCFfcHashOpMode_t ffcHashMode,  /*!< [in] enumerator ID of SHAx HASH mode. Note: HASH SHA1 mode may be
                                                     used only with SA set of domain parameters (sec. 5.8.1, tab.6). */
                uint8_t generIndex,             /*!< [in] an index of FFC Generator, allowing to generate different FFC generators with
                                                     the same FFC parameters prime P and Order Q, existed in the domain. */
                CCFfcDomainTmpBuff_t *pTmpBuff  /*!< [in] pointer to FFC Domain temp buffer structure. */
);


/*******************************************************************************************/
/*!
@brief The function validates received FFC domain parameters and sets them into Domain structure.
<ol><li> Validation of performed according to NIST SP 56A rev.2, sec. 5.5.2 and to FIPS 184-4 standard. </li>
</li> If optional parameters (Seed and pgenCounter) are given, then the function performs full validation by generation
primes P,Q from the given Seed and compares calculated and received parameters according to the FIPS 184-4, A.1.1.3. </li>
</li> Generator G is validated according to sec. A.2.3. </li>
</li> If optional parameters pSeed, seedSize, pgenCounter are zero, and the user explicitly sets validation mode to
"Trusted Data", then the function performs only checking of pointers, sizes and some relations between parameters. <li>.
</li> All  input byte-arrays should be set with big endianness order of bytes, i.e. MS Byte is a leftmost one. </li></ol>
@return CC_OK on success.
@return A non-zero value on failure, as defined in cc_dh_error.h, cc_rnd_error.h.
 */
CIMPORT_C CCError_t CC_FfcValidateAndImportDomain(
                CCFfcDomain_t *pDomain,          /*!< [out] pointer to  FFC Domain structure. */
                CCRndContext_t *pRndContext,     /*!< [in] optional (used on Full Validation mode only), random generation
                                                           function context. */
                uint8_t *pPrime,                 /*!< [in] pointer to prime modulus of the finite field (P). */
                size_t  primeSizeBits,           /*!< [in] prime P size in bits. */
                uint8_t *pOrder,                 /*!< [in] pointer to the order Q of the generator. */
                size_t  orderSizeBits,           /*!< [in] order size in bits. */
                uint8_t *pGenerator,             /*!< [in] pointer to generator G of subgroup of FFC. */
                size_t  generSizeBytes,          /*!< [in] generator G size in bytes (see note bellow). */
                uint8_t *pSeed,                  /*!< [in] optional (used on Full Validation mode only), pointer to the Seed,
                                                           if the Seed is not given, then should be set to NULL. */
                size_t  seedSizeBytes,           /*!< [in] optional size of Seed in bytes; if Seed not given, then
                                                      should be set to 0. */
                CCFfcParamSetId_t ffcParamSetId, /*!< [in] enumerator, defining the set of FFC domain parameters
                                                      according to SP 56A rev.2 section 5.5.1.1, tab.1. */
                CCFfcHashOpMode_t ffcHashMode,   /*!< [in] enumerator ID of SHAx HASH mode. Note: HASH SHA1 mode may be
                                                      used only with SA set of domain parameters (sec. 5.8.1, tab.6). */
                uint32_t genCounter,             /*!< [in] optional, counter of main iterations loop, performed during
                                                      domain generation with Seed. */
                    uint8_t generIndex,          /*!< [in] an index of FFC Generator, allowing to generate different FFC generators with
                                                          the same FFC parameters prime P and Order Q, existed in the domain. */
                CCFfcDomainValidMode_t validMode,/*!< [in] enumerator, defining validation mode of of domain parameters:
                                                     "full" (approved by FIPS standard), "partial"
                                                      and "trusted" (validated previously); using of both second
                                                      modes is not approved by standards and is fully on the user
                                                      responsibility. */
                CCFfcDomainTmpBuff_t *pTmpBuff   /*!< [in] optional pointer to FFC Domain temp buffer structure. Used only
                                                       on Full validation mode, on Trusted mode may be set to NULL. */
);


/*******************************************************************************************/
/*!
@brief This function extracts FFC domain parameters from Domain structure for external using.
<ol><li> Assumed, that FFC domain is properly generated by CC_FfcGenerateDomain or other function
according to the FIPS 184-4, A.1.1.2 standard. </li>
<li> The function checks input/output pointers and buffers sizes, converts the DH Domain parameters
to big endianness output arrays (with leading zeros if exists). </li>
<li> Note: Sizes of parameters are given by pointers, were [in/out] values are: in - buffer size,
out - actual size. </li></ol>

@return CC_OK on success.
@return A non-zero value on failure as defined cc_dh_error.h, cc_rnd_error.h.
*/
CIMPORT_C CCError_t CC_FfcExportDomain(
                CCFfcDomain_t *pDomain,          /*!< [in] pointer to FFC Domain to be exported. */
                uint8_t *pPrime,                 /*!< [out] pointer to prime modulus of the finite field (P). */
                size_t  *pPrimeSize,             /*!< [in/out] pointer to prime P size in bytes. */
                uint8_t *pGenerator,             /*!< [out] pointer to generator of subgroup (G). */
                size_t  *pGeneratorSize,         /*!< [in/out] pointer to generator G size in bytes. */
                uint8_t *pOrder,                 /*!< [out] pointer to the order of the generator G. */
                size_t  *pOrderSize,             /*!< [in/out] pointer to order of generator Q size in bytes. */
                uint8_t *pSeed,                  /*!< [out] optional, pointer to the Seed, used for Domain generation;
                                                      if Seed is not required, then the pointer and size should be NULL. */
                size_t  *pSeedSize,              /*!< [in/out] optional, size of the Seed in bytes - if the Seed not exist,
                                                      in the Domain, the function sets the size = 0. */
                CCFfcParamSetId_t *pFfcParamSetId, /*!< [in] pointer to enumerator ID, defining the set of FFC domain parameters
                                                       parameters according to SP 56A rev.2 section 5.5.1.1, tab.1. */
                CCFfcHashOpMode_t *pFfcHashMode,  /*!< [in] pointer to enumerator ID of SHAx HASH mode. Note: HASH SHA1 mode
                                                       may be used only with SA set of domain parameters (sec. 5.8.1, tab.6). */
                uint32_t *pGenCounter,            /*!< [out] pointer to count of iterations, which were performed
                                                       during Domain generation. */
                uint8_t  *pIndexOfGenerator       /*!< pointer to index, of  FFC Generator existed in the Domain. */
);


/*******************************************************************************************/
/*!
@brief The function creates a new FFC subgroup Generator for existed FFC Domain.
<ol><li> Assumed, that FFC domain is properly generated or imported previously and meets
to the FIPS 184-4, sec. A.1.1.2 standard. </li>
<li> The function checks input/output pointers and buffers sizes and creates new Generator
according to sec. A.2.3. and sets it into Domain structure. </li></ol>

@return CC_OK on success.
@return A non-zero value on failure as defined cc_dh_error.h, cc_rnd_error.h.
*/
CIMPORT_C CCError_t CC_FfcCreateNewGenerator(
                CCFfcDomain_t *pDomain,        /*!< [in/out] pointer to  FFC Domain structure. */
                CCRndContext_t *pRndContext,   /*!< [in] random generation function context. */
                uint8_t index,                 /*!< [in] index allowing to generate some FFC generators with
                                                    the same FFC parameters prime P and Order Q, existed in the domain. */
                CCFfcDomainTmpBuff_t *pTmpBuff /*!< [in] pointer to FFC Domain temp buffer structure. */
);

#ifdef __cplusplus
}
#endif
/**
@}
 */
#endif
