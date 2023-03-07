/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef _CC_FFCDH_H
#define _CC_FFCDH_H

#include "cc_pal_types.h"
#include "cc_pka_defs_hw.h"
#include "cc_pal_types.h"
#include "cc_pal_compiler.h"
#include "cc_hash_defs.h"
#include "mbedtls_cc_hkdf.h"
#include "cc_ffc_domain.h"
#include "cc_rnd_common.h"

#ifdef __cplusplus
extern "C"
{
#endif


/*!
@file
@brief This file defines the API that supports FFC Diffie-Hellman key exchange, as defined in NIST SP 56A rev.2 standard.
@defgroup cc_ffcdh CryptoCell FFCDH APIs
@{
@ingroup cryptocell_api

*/


/************************ Defines ******************************/

/*! Definition for DH public key.*/

/*!< Size limitation according to NIST SP 56A ver.2 standard */
#define CC_FFCDH_MAX_VALID_KEY_SIZE_VALUE_IN_BITS  2048 /*! Maximal valid key size in bits.*/
/*! Minimal valid key size in bits.*/
#define CC_FFCDH_MIN_VALID_KEY_SIZE_VALUE_IN_BITS  1024 /*!< Size limitation according to NSI standard */
/*! Maximal prime P (modulus) size in bytes.*/
#define CC_FFCDH_MAX_MOD_SIZE_IN_BYTES   (CC_FFCDH_MAX_VALID_KEY_SIZE_VALUE_IN_BITS / CC_BITS_IN_BYTE)
/*! Maximal prime P (modulus) size in words.*/
#define CC_FFCDH_MAX_MOD_SIZE_IN_WORDS   (CC_FFCDH_MAX_MOD_SIZE_IN_BYTES / CC_32BIT_WORD_SIZE)
/*! Prime P (modulus) buffer size in words.*/
#define CC_FFCDH_MAX_MOD_BUFFER_SIZE_IN_WORDS   (CC_FFCDH_MAX_MOD_SIZE_IN_WORDS + 2)
#define CC_FFCDH_MAX_MOD_BUFFER_SIZE_IN_BYTES   (CC_FFCDH_MAX_MOD_BUFFER_SIZE_IN_WORDS * CC_32BIT_WORD_SIZE)

/*! Maximal FFC subgroup order size. */
#define CC_FFCDH_MAX_GENER_ORDER_SIZE_IN_BITS   CC_FFC_DOMAIN_MAX_GENER_ORDER_SIZE_IN_BITS
#define CC_FFCDH_MAX_GENER_ORDER_SIZE_IN_BYTES  (CC_FFCDH_MAX_GENER_ORDER_SIZE_IN_BITS / CC_BITS_IN_BYTE)
#define CC_FFCDH_MAX_GENER_ORDER_SIZE_IN_WORDS  (CC_FFCDH_MAX_GENER_ORDER_SIZE_IN_BITS / CC_BITS_IN_32BIT_WORD)
/*!< Maximal size of buffer for Generator order (added 2 words for internal using) */
#define CC_FFCDH_MAX_GENER_ORDER_BUFF_SIZE_IN_WORDS (CC_FFCDH_MAX_GENER_ORDER_SIZE_IN_WORDS + 2)

/* Size in bytes of Length-counter (used for TLS data transfer etc. in form Len||Data) */
#define CC_FFCDH_LENGTH_COUNTER_SIZE_IN_BYTES 2
#define CC_FFCDH_KDF_COUNTER_SIZE_IN_BYTES 4  /* counter used in some KDF functions and concatenated
                                                 with OtherInfo */
#define CC_FFCDH_PUBL_KEY_TMP_BUFF_IN_WORDS  CC_FFCDH_MAX_MOD_SIZE_IN_WORDS
#define CC_FFCDH_PRIV_KEY_TMP_BUFF_IN_WORDS  (CC_FFCDH_MAX_GENER_ORDER_BUFF_SIZE_IN_WORDS)

/*! Number of other info entries */
#define CC_FFCDH_COUNT_OF_OTHER_INFO_ENTRIES  13

#define CC_FFCDH_MAX_SIZE_OF_ALG_ID_ENTRY_BYTES  32 /*!< Algorithm ID in bytes.*/

/*! Maximal size of supplied Private or Public data entry. */
#define CC_FFCDH_MAX_SIZE_OF_OTHER_INFO_SUPPL_ENTRY_BYTES  64 /*!< Size is in bytes */
#define CC_FFCDH_MAX_SIZE_OF_PARTY_ID_BYTES  32 /*!< implementation limit, in bytes. */

/*! Size of Nonce for Key Confirmation (if it is used) should be equal to FFC sub-group order size (meets to SP 800-56Arev.2, sec. 5.4) */
#define CC_FFCDH_MAX_SIZE_OF_NONCE_SUB_ENTRY_BYTES CC_FFCDH_MAX_GENER_ORDER_SIZE_IN_BYTES
#define CC_FFCDH_MAX_SIZE_OF_PUBL_KEY_DATA_BYTES  CC_FFCDH_MAX_MOD_SIZE_IN_BYTES /*!< Party Public Key max. size in bytes. */
#define CC_FFCDH_MAX_SIZE_OF_PARTY_INFO_OTHER_DATA_BYTES  64 /*!< Party Public Keys Info max. size in bytes.*/
/*!< Count of concatenated sub-entries of Party (U or V) Info  */
#define CC_FFCDH_COUNT_OF_PARTY_INFO_ENTRIES 5
/*! Maximal size of PartyInfo (U or V). Note: Buffers for Nonce and Ephemeral key
 *  are joined, because only one of them is used actually  */
#define CC_FFCDH_MAX_SIZE_OF_PARTY_INFO_BYTES  (CC_FFCDH_MAX_SIZE_OF_PARTY_ID_BYTES  + 2*CC_FFCDH_MAX_SIZE_OF_PUBL_KEY_DATA_BYTES + \
        CC_FFCDH_MAX_SIZE_OF_NONCE_SUB_ENTRY_BYTES + CC_FFCDH_MAX_SIZE_OF_PARTY_INFO_OTHER_DATA_BYTES + \
        CC_FFCDH_COUNT_OF_PARTY_INFO_ENTRIES * CC_FFCDH_LENGTH_COUNTER_SIZE_IN_BYTES)

/*! Maximal size of OtherInfo buffer, including KDF Counter and all entries of actual OtherInfo data */
#define CC_FFCDH_MAX_SIZE_OF_OTHER_INFO_DATA_BYTES  (CC_FFCDH_LENGTH_COUNTER_SIZE_IN_BYTES + CC_FFCDH_MAX_SIZE_OF_ALG_ID_ENTRY_BYTES + \
        2 * (CC_FFCDH_MAX_SIZE_OF_PARTY_INFO_BYTES + CC_FFCDH_LENGTH_COUNTER_SIZE_IN_BYTES + CC_FFCDH_MAX_SIZE_OF_OTHER_INFO_SUPPL_ENTRY_BYTES))
/*! Extended KDF data buffer: containing: Counter||SharedSecretZZ||OtherInfo */
#define CC_FFCDH_MAX_SIZE_OF_KDF_DATA_BUFFER_BYTES  (CC_FFCDH_KDF_COUNTER_SIZE_IN_BYTES + 2*CC_FFCDH_MAX_MOD_SIZE_IN_BYTES/*ZZ size*/ + \
CC_FFCDH_MAX_SIZE_OF_OTHER_INFO_DATA_BYTES)
#define CC_FFCDH_MAX_SIZE_OF_KDF_DATA_BUFFER_WORDS  (CC_FFCDH_MAX_SIZE_OF_KDF_DATA_BUFFER_BYTES / CC_32BIT_WORD_SIZE)

#define CC_FFCDH_MAX_SIZE_OF_KEYING_MATERIAL_BYTES  1024 /*!< Size is in bytes*/

/*! Maximal size of Confirmation MacData in bytes.
 * msg_str || IDp || IDr || EphemDataP || EphemDataR {|| TextP}
 * (Max.size 614 bytes */
#define CC_FFCDH_SIZE_OF_CONFIRM_MSG_STRING_BYTES      6 /*!< standard confirmation message string size in bytes. */
#define CC_FFCDH_MAX_SIZE_OF_CONFIRM_TEXT_DATA_BYTES  32 /*!< party supplied confirmation text size in bytes. */
#define CC_FFCDH_MAX_SIZE_OF_CONFIRM_MAC_DATA_BYTES  (CC_FFCDH_SIZE_OF_CONFIRM_MSG_STRING_BYTES + 2*(CC_FFCDH_MAX_SIZE_OF_PARTY_ID_BYTES + \
        CC_FFCDH_MAX_MOD_SIZE_IN_BYTES) + CC_FFCDH_MAX_SIZE_OF_CONFIRM_TEXT_DATA_BYTES)
/*! Maximal size of Confirmation MacTag (according max. HASH output size) */
#define CC_FFCDH_MAX_SIZE_OF_CONFIRM_MAC_TAG_BYTES  CC_HASH_SHA512_DIGEST_SIZE_IN_BYTES
/*! Minimal size in bytes of Confirmation MacTag (sec 5.9.3, tab.8) */
#define CC_FFCDH_MIN_SIZE_OF_CONFIRM_MAC_TAG_BYTES  8
#define CC_FFCDH_MAX_SIZE_OF_HMAC_SALT_BUFF_BYTES   CC_HASH_SHA512_BLOCK_SIZE_IN_BYTES

/*! Constant size in bytes of Confirmation MacTag defined in this implementation;
 *  note: minimal size according to standards is 6 bytes (112 bit - sec 5.9.3, tab.8) */
#define CC_FFCDH_SIZE_OF_CONFIRM_MAC_KEY_IN_BYTES  8

/*! The size of the buffer for User ID */
//#define CC_FFCDH_USER_ID_SIZE_IN_BYTES  8
/*! Buffer for Barrett Tag - special value, used in modular multiplication */
#define CC_FFCDH_BARRETT_TAG_MAX_SIZE_IN_WORDS  CC_FFC_DOMAIN_BARRETT_TAG_MAX_SIZE_IN_WORDS
#define CC_FFCDH_BARRETT_TAG_MAX_SIZE_IN_BYTES  (CC_FFCDH_BARRETT_TAG_MAX_SIZE_IN_WORDS * CC_32BIT_WORD_SIZE)

/*! Size (in 32-bit words) of additional buffer used in random generation of vector
in range according to FIPS 186-4 sec. B.1.1 */
#define FFCDH_RND_ADDING_SIZE_WORDS 2
#define FFCDH_RND_ADDING_SIZE_BYTES (FFCDH_RND_ADDING_SIZE_WORDS * CC_32BIT_WORD_SIZE)

/*! Max. size of DH Context temp buffer in words */
#define CC_FFCDH_CTX_TMP_BUFF_MAX_SIZE_IN_WORDS  (CC_FFCDH_MAX_MOD_SIZE_IN_WORDS + CC_FFCDH_MAX_GENER_ORDER_SIZE_IN_WORDS)
/* Size of FFCDH Context internal buffer */
#define CC_FFCDH_CONTEXT_BUFF_SIZE_IN_BYTES  \
        ROUNDUP_BYTES_TO_32BIT_WORD((FFC_DOMAIN_SIZE_BYTES + 32/*schemeInfo*/ + CC_FFCDH_MAX_SIZE_OF_HMAC_SALT_BUFF_BYTES + \
        CC_FFCDH_MAX_SIZE_OF_KEYING_MATERIAL_BYTES + 4*CC_FFCDH_MAX_GENER_ORDER_SIZE_IN_BYTES + 2*CC_FFCDH_MAX_SIZE_OF_PARTY_ID_BYTES + \
        4*CC_FFCDH_MAX_MOD_SIZE_IN_BYTES + CC_FFCDH_MAX_SIZE_OF_KDF_DATA_BUFFER_BYTES + 2*CC_HASH_RESULT_SIZE_IN_WORDS*CC_32BIT_WORD_SIZE/*MacTags*/ + \
        84/*dataOffsets*/ + 2*CC_FFCDH_MAX_SIZE_OF_CONFIRM_TEXT_DATA_BYTES + 26*CC_32BIT_WORD_SIZE/*separ.words*/ + \
        CC_FFCDH_CTX_TMP_BUFF_MAX_SIZE_IN_WORDS*CC_32BIT_WORD_SIZE))
#define CC_FFCDH_CONTEXT_BUFF_SIZE_IN_WORDS  (CC_FFCDH_CONTEXT_BUFF_SIZE_IN_BYTES / CC_32BIT_WORD_SIZE)

#define CC_FFCDH_CALC_USER_MAC_TAG   FALSE
#define CC_FFCDH_CALC_PARTN_MAC_TAG  TRUE

/*! Key size used for FIPS tests.*/
#define CC_FFCDH_FIPS_PRIME_SIZE_VALUE_IN_BITS    2048
#define CC_FFCDH_FIPS_PRIME_SIZE_VALUE_IN_WORDS   (CC_FFCDH_FIPS_PRIME_SIZE_VALUE_IN_BITS / CC_BITS_IN_32BIT_WORD)
#define CC_FFCDH_FIPS_ORDER_SIZE_VALUE_IN_WORDS   32



/************************ Enums ********************************/

/*! Key derivation modes according NIST SP 800-56A ver.2 sec. 5.8.2 with reference to
 *  SP 800-56C sec. 4, SP 800-108 and RFC 5869. */
typedef enum
{
    CC_FFCDH_KDF_HMAC_RFC5869_MODE,  /*!< extraction-then-expansion KDF(RFC 5869), based on HMAC function;
                                          note: input salt assumed to be NULL. */
        CC_FFCDH_KDF_NUM_OFF_MODE, /*!< not allowed value */
    CC_FFCDH_KDF_MODE_LAST = 0x7FFFFFFF
} CCFfcDhKdfModeSp56A_t;


/*! FFC DH key Agreement Schemes enumeration according to
    NIST SP 56A Rev. 2, Section 6, tab. 10-12. */
typedef enum
{
    CC_FFCDH_SCHEM_HYBRID1,          /*!< dhHybrid1 C(2e, 2s, FFC DH) */
    CC_FFCDH_SCHEM_HYBRID_ONE_FLOW,  /*!< dhHybridOneFlow C(1e, 2s, FFC DH) */
    CC_FFCDH_SCHEM_EPHEM,            /*!< dhEphem C(2e, 0s, FFC DH) */
    CC_FFCDH_SCHEM_ONE_FLOW,         /*!< dhOneFlow C(1e, 1s, FFC DH) */
    CC_FFCDH_SCHEM_STATIC,           /*!< dhStatic C(0e, 2s, FFC DH) */
    CC_FFCDH_SCHEM_NUM_OFF_MODE,     /*!< not allowed value */
    CC_FFCDH_SCHEM_LAST = 0x7FFFFFFF
} CCFfcDhSchemeId_t;

/*! An enumeration ID, defining user role in DH Agreement, represented as U, V
   in NIST SP 56A Rev. 2, Sections 3.1, 5.8.1.2, 5.9.1, 6 */
typedef enum
{
        CC_FFCDH_PARTY_U,             /*!< party U of Key Agreement */
        CC_FFCDH_PARTY_V,             /*!< party V of Key Agreement */
        CC_FFCDH_PARTY_NUM_OFF_MODE,  /*!< not allowed value */
        CC_FFCDH_PARTY_LAST = 0x7FFFFFFF
} CCFfcDhUserPartyIs_t;

/*! DH Agreement Confirmation mode: which parts is provider or/and receiver.
  NIST SP 56A Rev. 2, Sections 5.9, 6.1, 6.2, 6.3 */
typedef enum
{
    CC_FFCDH_CONFIRM_U_TO_V,          /*!< only party U provides MacTag to V. */
    CC_FFCDH_CONFIRM_V_TO_U,          /*!< only party V provides MacTag to U. */
    CC_FFCDH_CONFIRM_BILATERAL,       /*!< each party provides MacTag to other. */
    CC_FFCDH_CONFIRM_NOT_USED,        /*!< the confirmation is not performed by the scheme */
    CC_FFCDH_CONFIRM_NUM_OFF_MODE,    /*!< not allowed value */
    CC_FFCDH_CONFIRM_MODE_LAST = 0x7FFFFFFF
}CCFfcDhUserConfirmMode_t;


/*! DH key status according to its life time (or purpose): static/ephemeral */
typedef enum
{
    CC_FFCDH_KEY_STATIC,     /*!< static (long term) key  */
    CC_FFCDH_KEY_EPHEMER,    /*!< ephemeral (one-time) key */
    CC_FFCDH_KEY_STATUS_NUM_OFF_MODE, /*!< not allowed value */
    CC_FFCDH_KEY_STATUS_LAST = 0x7FFFFFFF
} CCFfcDhKeyStatus_t;


/*! FFC DH Public Key validation mode definitions :
    (such enumerator mode should be given for each key separately). */
typedef enum {
    CC_FFCDH_KEY_VALIDAT_FULL_MODE,    /*!< full validation (NIST SP 56A Rev. 2) */
    CC_FFCDH_KEY_VALIDAT_PARTIAL_MODE, /*!< checking of sizes, pointers and ranges;
                                                this mode may be used on user's responsibility
                                                when he has assurance about received data */
        CC_FFCDH_KEY_VALIDAT_NUM_OFF_MODE, /*!< not allowed value */
        CC_FFCDH_KEY_VALIDAT_MODE_LAST = 0x7FFFFFFF
} CCFfcDhKeyValidMode_t;

/*! FFC DH both PartyInfo (Public Keys) validation mode definitions (NIST SP 56A Rev. 2).
    Such enumerator mode might be applied to all existed Public keys, belonging to the party,
     namely: static and ephemeral keys. If full mode for any existed key is not defined,
     then it will be validated partially (checking of sizes, pointers and ranges). */
typedef enum {
    CC_FFCDH_STAT_KEY_FULL_VALIDAT_MODE,    /*!< full validation of static key only */
    CC_FFCDH_EPHEM_KEY_FULL_VALIDAT_MODE,   /*!< full validation of ephemeral key only */
    CC_FFCDH_BOTH_KEYS_FULL_VALIDAT_MODE,   /*!< full validation of both keys */
    CC_FFCDH_NO_FULL_VALIDAT_MODE,          /*!< only partial validation of existed keys */
        CC_FFCDH_KEYS_VALIDAT_NUM_OFF_MODE,     /*!< not allowed value */
        CC_FFCDH_KEYS_VALIDAT_MODE_LAST = 0x7FFFFFFF
} CCFfcDhPartyInfoValidMode_t;


/************************ Structures  ***********************************/

/*! FFC Domain parameters structure (p,q,g,{seed,genCounter}. */
//#define CCFfcDhDomain_t  CCFfcDomain_t

/*! FFC DH Domain parameters sets definition: NIST SP 56A Rev. 2, sec. 5.8.1, tab.6,
 *  Note: modulus (prime) size 3072 is not allowed in FFC DH. */
//#define CCFfcDhParamSetId_t  CCFfcParamSetId_t

/**************************************************************/
/*! Definition of PartyInfo entry structure.
The structure (buffer) containing data, which should be supplied, to the key agreement
by any Party (partyU or partyV) and used for derivation of Shared Secret Keying Data. \par
The data should be constructed according to concatenation method, described in
NIST SP 56A rev.2 standard sec. 5.8.1, and the following requirements:

<ul><li> PartyInfo = PartyInfoLen||PartyId||PartyNonce{||PartyOtherData}, where
each sub-entry is formatted as follows: </li>
<li> - entries in {} parenthesis are optional. </li>
<li> - each sub-entry is formatted as length (Len), followed by the data: Len||Data; </li>
<li> - length (Len) is a 2-bytes big endianness counter; </li>
<li> - actual length of PartyInfo shall be not great than CC_FFCDH_MAX_SIZE_OF_OTHER_INFO_ENTRY bytes. </li>
<li> - if any explicitly defined there optionally entry or sub-entry is omitted, then its length
should be set zero and data is empty string. </li></ul>
\note Said requirements are mandatory and should be agreed by both parties as a part of Key
Establishment Agreement protocol.
*/
typedef struct CCFfcDhPartyInfo_t{
        uint8_t data[CC_FFCDH_MAX_SIZE_OF_PARTY_INFO_BYTES];
}CCFfcDhPartyInfo_t;

/**************************************************************/
/*! Definition of OtherInfo structure.
This structure containing "other data", shared by both key agreement parties
and used for derivation of Shared Secret Keying Data. \par
The data should be constructed according to concatenation method, described in
NIST SP 56A rev.2 standard sec. 5.8.1, and the following requirements:

<ul><li>  OtherInfo data should be concatenated according to the roles, performed by each
party in the Key Agreement (partyU or partyV), and include the following entries: </li>
<li>  AlgorithmId||PartyUInfo||PartyVInfo {||SuppPubInfo}{||SuppPrivInfo}, where each entry
is formatted as follows: </li>
<li> - entries in {} parenthesis are optional. </li>
<li> - each entry could include some sub-entries, which are formatted as length (Len), followed
by the data of said length: Len||Data; </li>
<li> - length (Len) of the data is formatted as 2-bytes big endianness counter; </li>
<li> - numerical parameters, such as size of HMAC-Key etc., are considered as separate sub-entry
and formatted accordingly;
<li> - actual length of OtherInfo shall be not great than CC_FFCDH_MAX_SIZE_OF_OTHER_INFO_DATA_BYTES bytes. </li>
<li> - if any explicitly defined optionally entry or sub-entry is omitted, then its length
should be zero and data array remained empty. </li></ul>
<ul><li> Formatting of separate entries is described below (each sub-entry includes Len||Data): </li>
<li> - AlgorithmId entry includes information how the extracted keying material should be parsed between
HMAC Key (used for internal calculation of Confirmation MacTag) and between Key for External Algorithm,
i.e. output SecretKeyingData. This entry also includes ID of algorithms, for which these keys are intended:
  AlgorithmId = HmacKeySize||InternalAlgorithmId||ExternalAlgorithmKeySize||ExternalAlgorithmId, where
first two sub-entries will be set by CC functions as array: 0x00||0x02||0x00||0x04||"HMAC" and
sub-entries, related to ExternalAlgorithm, should be given by the user as input to appropriate CC functions. </li>
<li> - PartyUInfo and PartyVInfo should be constructed as described in CCFfcDhPartyInfo_t structure. </li>
<li> - Optional SuppPubInfo and SuppPrivInfo entries and their sub-entries should be defined in the Key
Agreement protocol. </li></ul>
\note Said requirements are mandatory and should be agreed by both parties as a part of Key Establishment
Agreement protocol.
*/
typedef struct CCFfcDhOtherInfo_t{
        uint8_t data[CC_FFCDH_MAX_SIZE_OF_OTHER_INFO_DATA_BYTES/*884 bytes ???*/];
}CCFfcDhOtherInfo_t;

/**************************************************************/
/*! Definition of MAC Data structure, containing data, known to both key agreement
parties and used for key confirmation. \par
The data should be constructed according to NIST SP 56A rev.2 standard,
sec. 5.9, 5.9.1.1 and the following requirements:
<ul><li> MacData should be concatenated according to the role, performed by the user
in the Key Agreement: is he partyU or partyV and is he confirmation provider (P)
or recipient (R): </li>
<li>  MacDataP = messageStringP||IDP||IDR{||EphemDataP}||EphemDataR{||TextP},
where each entry is formatted as follows: </li>
<li> - actually, instead letters "P" and "R" must be set "U" or "V" according to parties roles in DH Scheme; </li>
<li> - EphemData is an EphemeralPublicKey or Nonce, contributed by the party to the Agreement; </li>
<li> - messageStringP is a 6-byte string, defined in the sec.5.9.1, 5.9.2 according to used DH Scheme; <li>
<li> - TextP is an optional bit-string about Confirmation, known to both parties; </li>
<li> - each entry is formatted as length, followed by bytes-array of data: Len||Data; </li>
<li> - length (Len) of the data is formatted as 2-bytes big endianness counter; </li>
<li> - if any explicitly defined there optionally sub-entry is omitted, then its length should be
zero and data array remained empty. </li></ul>
<li> - total length of MacDataP shall be not great than CC_FFCDH_MAX_SIZE_OF_MAC_DATA bytes. </li></ul>
\note Confirmation is possible (effective) only if Confirmation Receiver contribute an EphemeralKey or Nonce
to the Key Agreement.
\note Said requirements are mandatory and should be agreed by both parties as a part of Key Establishment
Agreement protocol.
*/
typedef struct CCFfcDhConfirmMacData_t{
        uint32_t sizeInBytes; /*!< actual size of data in the MacData buffer, in bytes */
        uint8_t macData[CC_FFCDH_MAX_SIZE_OF_CONFIRM_MAC_DATA_BYTES];
} CCFfcDhConfirmMacData_t;


/**************************************************************/
/*! DH Key Agreement Confirmation MacTag, calculated as HMAC of MacData.
    See NIST SP 56A rev.2 standard, sec. 5.9. Optionally MacTag may be
    truncated (sec. 5.9.3) */
typedef struct CCFfcDhConfirmMacTag_t{
        uint32_t sizeInBytes;
        uint8_t macTag[CC_FFCDH_MAX_SIZE_OF_CONFIRM_MAC_TAG_BYTES];
}CCFfcDhConfirmMacTag_t;


typedef struct CCFfcDhHashBlockAndDigestSizes_t{
        uint32_t blockSizeInBytes;  /*!< HASH function block size in bytes */
        uint32_t digestSizeInBytes; /*!< HASH function digest (output) size in bytes */
} CCFfcDhHashBlockAndDigestSizes_t;


/**************************************************************/
/*! The structure containing the FFC DH Public Key parameters. */
typedef  struct  CCFfcDhPublKey_t
{
        size_t   keySizeBits;
    uint32_t pubKey[CC_FFCDH_MAX_MOD_SIZE_IN_WORDS]; /*!< Public Key .*/
        CCFfcDhKeyStatus_t status; /*! enumerator, defining the key status according to its lifetime
                                    or purpose: static/ephemeral/nonce */
}CCFfcDhPublKey_t;


/**************************************************************/
/*! The FFC DH public key's user structure prototype. This structure must be saved by the user,
and is used as input to the DH functions (such as ::CC_FfcDhGeneratePubPrv etc.). */
typedef struct   CCFfcDhUserPubKey_t
{
    uint32_t validTag; /*!< Validation tag.*/
    uint32_t publKeyDbBuff[CALC_32BIT_WORDS_FROM_BYTES(sizeof(CCFfcDhPublKey_t))]; /*!< Public key data. */
}CCFfcDhUserPubKey_t;

#ifdef FFC_FURTHER_USING
/**************************************************************/
/*! The structure containing the FFC DH Public Keys parameters. */
typedef  struct  CCFfcDhCtxPublKeys_t
{
    uint32_t statKeySizeBytes;
    uint8_t statPublKey[CC_FFCDH_MAX_MOD_SIZE_IN_WORDS];
    uint32_t ephemKeySizeBytes;
    uint32_t ephemPublKey[CC_FFCDH_MAX_MOD_SIZE_IN_WORDS];
        uint32_t nonceSizeBytes;
        uint32_t userNonce[CC_FFCDH_MAX_GENER_ORDER_SIZE_IN_WORDS];
}CCFfcDhCtxPublKeys_t;


/**************************************************************/
/*! The structure containing the FFC DH Private Key parameters.
\note The maximal bit length of private key must be 160, 224 or 256
according to NIST SP 56A rev.2, sec.5.5.1.1 */
typedef  struct  CCFfcDhPrivKey_t
{
    /*! Private Key exponent.*/
        size_t   keySizeBits;
    uint32_t key[CC_FFCDH_MAX_GENER_ORDER_SIZE_IN_WORDS + FFCDH_RND_ADDING_SIZE_BYTES];
        CCFfcDhKeyStatus_t status; /*! enumerator, defining the key status according to its lifetime
                                    or purpose: static/ephemeral/nonce */
}CCFfcDhPrivKey_t;


/**************************************************************/
/*! The FFC DH public key's user structure prototype. This structure must be saved by the user
as secret, and is used as input to the DH functions (such as ::CC_FfcDhGeneratePubPrv etc.). */
typedef struct   CCFfcDhUserPrivKey_t
{
    uint32_t validTag; /*!< key validation tag. */
    uint32_t privKeyDbBuff[CALC_32BIT_WORDS_FROM_BYTES(sizeof(CCFfcDhPrivKey_t))]; /*!< Private key data. */
}CCFfcDhUserPrivKey_t;


/**************************************************************/
/*! The structure defines context temp buffer, used for internal calculations.
\note The maximal bit length of private key must be 160, 224 or 256
according to NIST SP 56A rev.2, sec.5.5.1.1 */
typedef  struct  CCFfcDhCtxTempBuff_t
{
    uint32_t key[CC_FFCDH_CTX_TMP_BUFF_MAX_SIZE_IN_WORDS];
}CCFfcDhCtxTempBuff_t;

#endif

/* temp buffer structure, used for DH functions  */
typedef struct CCFfcDhTemp_t
{
    uint32_t TempBuff[CC_FFCDH_CTX_TMP_BUFF_MAX_SIZE_IN_WORDS];
} CCFfcDhTemp_t;


/* Definition of name of function that translates the FCC Domain and DH Hash modes into
 * HASH, KDF-Hash modes and gives HASH block and digest sizes (in bytes). Note: the function
 * sets on output only required parameters, which pointers are not NULL.
 * */
#define FfcDhGetHashMode  FfcDomainGetHashMode

/***************************************************************************/
/*!< Set of DH FFC parameters size approved by NIST SP 800-56A rev.2. tab.6,8
     Intended for initialisation of array of structures of following type.
*/
#define  CCFfcDhDomainParamSizes_t  CCFfcDomainParamSizes_t

/**************************************************************/
/*! DH Key Agreement user context structure is passed by the user to the DH APIs.
   The context saves the state of the operations and must be saved by the user
   till the end of the APIs flow. */
typedef struct CCFfcDhUserContext_t
{
    /*! Validation tag. */
    uint32_t validTag;
    /*! Private data context buffer. */
        uint32_t  contextBuff[CC_FFCDH_CONTEXT_BUFF_SIZE_IN_WORDS];
//        uint32_t  contextBuff[(sizeof(DhContext_t)+3)/4];
} CCFfcDhUserContext_t;



/***************************************************************************/
/*! Definition of FFC-DH buffer used for FIPS Known Answer Tests. */
typedef struct
{
    /* FFC Domain parameters */
    uint32_t prime[CC_FFCDH_FIPS_PRIME_SIZE_VALUE_IN_WORDS];     /*!< prime modulus - in KAT used 2048 bit size. */
        uint32_t generator[CC_FFCDH_FIPS_PRIME_SIZE_VALUE_IN_WORDS]; /*!< FFC sub-group generator */
        uint32_t order[CC_FFCDH_FIPS_ORDER_SIZE_VALUE_IN_WORDS];     /*!< order of FFC sub-group - in KAT used 256 bit size*/
        uint32_t privKey[CC_FFCDH_FIPS_ORDER_SIZE_VALUE_IN_WORDS];   /*!< private key */
        uint32_t pubKey[CC_FFCDH_FIPS_PRIME_SIZE_VALUE_IN_WORDS];    /*!< public key */
    CCFfcDhTemp_t tmpBuff;                                       /*!< temporary buffer */
} CCFfcDhFipsKat_t;


/************************ Public Variables ******************************/
/*!< Set of DH FFC parameters sizes, approved by NIST SP 800-56A rev.2: sec. 5.8.1, 5.9.3.
     Intended for initialization of array of structures of type CCFfcDhFfcDomainParamSizes_t.
     All sizes are given in bytes.
     \note Index of array is given according to CCFfcDhFfcParamsSetId_t enumerator:
         {CC_FFCDH_PARAMS_SET_FA, CC_FFCDH_PARAMS_SET_FB, CC_FFCDH_PARAMS_SET_FC} = {0,1,2}.
*/
#define CC_FFCDH_DOMAIN_PARAM_SIZES_SET CC_FFC_DOMAIN_PARAM_SIZES_SET
//{{80,1024,160,80},{112,2048,224,112},{112,2048,256,112}}
/*! Define and init parameters array */
//CCFfcDhDomainParamSizes_t ffcDomainParamSizes[(uint32_t)CC_FFCDH_PARAMS_SET_NUM_OFF_MODE] = FFCDH_DOMAIN_PARAM_SIZES_SET;

/*! Array of allowed HASH SHA-x block and digest sizes for all SHA modes (size in bytes).
   \note Index of array is according to CCFfcDhParamsSetId_t enumerator: {CC_HASH_SHA1_mode, CC_HASH_SHA224_mode, CC_HASH_SHA256_mode,
   CC_HASH_SHA384_mode, CC_HASH_SHA512_mode} = {0,1,2,3,4}. */
#define CC_DH_SHA_PARAMETERS_SIZES_IN_BYTES CC_FFC_SHA_PARAMETERS_SIZES_IN_BYTES
//{{64,20},{64,28},{64,32},{128,48},{128,64}}
/*! Define and initialize HASH parameters array */
//CCFfcDhHashBlockAndDigestSizes_t DhHashBlockAndDigestSizes[(uint32_t)CC_FFCDH_HASH_NUM_OFF_MODE] =
//                           DH_SHA_PARAMETERS_SIZES_IN_BYTES;


/************************ Public Functions ******************************/

/*******************************************************************************************/
/*! The functions initializes the DH Context structure:
<li> zeroes context buffers, initializes 3 MS bytes of validation tag by context ID and sets LS byte
to zero to prepare it for further indications of setting appropriate parts of data into context
*/
CIMPORT_C CCError_t  CC_FfcDhInitCtx( CCFfcDhUserContext_t *pDhUserCtx);


/*******************************************************************************************/
/*! The functions destroys (zeroes) the DH Context structure.

@return CC_OK on success.
@return A non-zero value on failure as defined cc_dh_error.h.

*/
CIMPORT_C CCError_t  CC_FfcDhFreeCtx( CCFfcDhUserContext_t *pDhUserCtx);


/*******************************************************************************************/
/*! The function sets into DH context FFCDH Scheme agreed parameters: SchemeId, User role, Confirmation mode etc.
\note The context is used in DH Agreement functions, implementing NIST SP 800-56A rev.2 standard.
\note Assumed, that input FFC Domain is properly generated or imported and validated according to
NIST SP 800-56A and FIPS 186-4 standards.

@return CC_OK on success.
@return A non-zero value on failure as defined cc_dh_error.h.
*/
CEXPORT_C CCError_t  CC_FfcDhCtxSetSchemeParams(
                        CCFfcDhUserContext_t *pDhUserCtx, /*!< [in/out] pointer to context structure, containing all parameters and data,
                                                                    defining DH Key Agreement Scheme */
            CCFfcDomain_t *pDomain,           /*!< [in] pointer to DH FFC Domain structure. */
            uint8_t *pAlgId,                  /*!< [in] pointer to Algorithm ID agreed by both parties and indicates how the derived
                                                        secret keying material will be parsed and for which algorithms (sec.5.8.1.2).
                                                        In partial, Algorithm ID should indicate also how much bits are intended for
                                                        internal confirmation MAC algorithm and how much remaining bits will be
                                                        returned to the user for external applications/algorithms (the total size should
                                                        be equal to chosen secretKeyDataSize). */
            size_t algIdSize,                 /*!< [in] size of Algorithm ID in bytes, should be less than
                                        CC_FFCDH_MAX_SIZE_OF_ALG_ID_SUB_ENTRY. */
            size_t secretKeyingDataSize,      /*!< [in] size in bytes of shared secret keying data, which will be extracted and in
                                                                    the next steps and passed to the user for using in  external algorithm(s).
                                                                    It is used for calculation of Derived Keying material size =
                                                                    key size of the used HMAC function + secretKeyingDataSize. */
                        uint8_t *pUserId,                 /*!< [in] pointer to the user ID - a distinct identifier of the user. */
                        size_t userIdSize,                /*!< [in] size of the user ID in bytes. */
                        uint8_t *pPartnId,                /*!< [in] pointer to the partner ID - a distinct identifier of the party. */
                        size_t partnIdSize,               /*!< [in] size of the partner ID in bytes. */
                        CCFfcDhUserPartyIs_t userParty,   /*!< [in] enumerator, defining role of the user (function's caller) in the
                                                                    DH Agreement Scheme: partyU or partyV. */
            CCFfcDhSchemeId_t dhSchemeId,     /*!< [in] enumerator ID of used FFC DH Key Agreement Scheme, as defined
                                                in sec. 6, tab. 12. */
            CCFfcParamSetId_t ffcParamSetId,  /*!< [in] enumerator, defining the set of FFC domain parameters
                                                                    according to SP 56A rev.2 section 5.5.1.1, tab.1. */
            CCFfcDhKdfModeSp56A_t kdfMode,    /*!< [in] enumerator ID of used KDF function, based on HASH or HMAC algorithms. In current
                                                        implementation is allowed only KDF HMAC_RFC5869 mode, according to KDF_HMAC_RFC-5869. */
                        CCFfcHashOpMode_t ffcHashMode,    /*!< [in] enumerator ID of used SHAXXX HASH mode, supported by the product.
                                                                    Note: HASH SHA1 function may be used only with SA set of domain parameters
                                                                    (sec. 5.8.1, tab.6); with other sets the function returns an error. */
                        CCFfcDhUserConfirmMode_t confirmMode, /*!< enumerator, defining confirmation mode of each party: provider
                                                                    or/and recipient, according to sec. 5.9. */
                        uint8_t *pHmacSalt,               /*!< [in] optional, pointer to the Salt, used as key in HMAC-KDF function on appropriate modes.
                                                                    If HMAC-KDF mode is set, and the pointer and size are zero, then the Salt is
                                                                    treated as full-zero bytes array of size equalled to block-size of used HMAC function.
                                                                    If HMAC-KDF mode is HMAC_RFC5869_MODE, then the Salt is treated as HMAC Key.
                                                                    If only one of parameters (pointer and size) is zero, but other not, then the
                                                                    function returns an error. */
                        size_t  hmacSaltSize,             /*!< [in] optional, size of Salt in bytes, should be equalled to the HMAC block size if
                                                                    salt is used. */
                        size_t  macTagSize                /*!< [in] optional, size in bytes ofof confirmation MacTag. Should be in range:
                                                                    [CC_FFCDH_MIN_SIZE_OF_CONFIRM_MAC_TAG_BYTES, CC_FFCDH_MAX_SIZE_OF_CONFIRM_MAC_TAG_BYTES]. */
);



/*******************************************************************************************/
/*!
@brief The function generates FFC DH key pairs according to DH Scheme and NIST SP 800-56A rev.2 standard:
<ol><li> - count of required key pairs (one or two is dependent on DH Scheme and user Party (U or V),
inserted into Context. For each of key pair the function performs the following steps: </li>
<li> - randomly generates the private key X according to section 5.6.1.1 and FIPS 184-4, B.1.1; </li>
<li> - the sizes of primes P,Q should be taken from DH FFC sizes set previously inserted into Context;  </li>
<li> - calculates the associated public key  Y = G^X mod P; </li>
<li> - sets private and public keys in appropriate place in the Context according to user party (U,V) and keys
status (static, ephemeral); </li>
<li> - exports the public key as big endianness order of bytes. </li></ol>
\note Before calling of this function, DH context should be initialized, DH Scheme parameters and
DH Domain are inserted by calling appropriate functions, else the function returns an error.

@return CC_OK on success.
@return A non-zero value on failure as defined cc_dh_error.h, cc_rnd_error.h.
*/
CIMPORT_C CCError_t CC_FfcDhGeneratePublPrivKeys(
                                CCFfcDhUserContext_t *pDhUserCtx,/*!< [in/out] pointer to DH FFC User Context structure. */
                                CCRndContext_t *pRndContext      /*!< [in] random generation function context. */
);

/*******************************************************************************************/
/*!
@brief This function validates the FFC DH public key according to NIST SP 800-56A rev.2,
       sec.5.6.2.3.1 and checking mode:

<ul><li> - on "partial" mode - checks the pointers and high/low limits of key value;</li>
<li> - on "full" mode - checks also that the the key belongs to the FFC subgroup; </li></ul>
\note Before calling of this function, appropriate FFC Domain parameters should be obtained and validated,
else the function returns an error.
@return CC_OK on success.
@return A non-zero value on failure as defined cc_dh_error.h.
*/
CIMPORT_C CCError_t CC_FfcDhValidatePublKey(
            CCFfcDomain_t *pFfcDomain,         /*!< [in/out] pointer to DH FFC Context structure. */
            uint8_t *pPublKeyData,             /*!< [in] pointer to given DH FFC public key formatted as big endianness array;
                                     it should be in range [2, P-2], where P is the Domain Prime P. */
            size_t publKeyDataSize,            /*!< [in] pointer to public key size, in bytes: should be not great than Prime size. */

            CCFfcDhKeyValidMode_t validatMode, /*!< [in] enumerator ID defining the validation mode:
                                     CC_FFCDH_CHECK_FULL_MODE - full validation (sec. 5.6.2.3.1);
                                     CC_FFCDH_CHECK_PARTIAL_MODE - check pointers, sizes and range of values. */
            uint32_t *pTmpBuff);               /*!< [in] temporary buffer of size not less 2*Prime size. ??? */



/*******************************************************************************************/
/*!
@brief The function checks and sets the FFC DH partner's public key into DH Context
according to NIST SP 800-56A rev.2 sec.5.6.2.3.1 and checking mode:

<ul><li> - if the key belongs to user's party, then the function returns an error, meaning
that the user should use other function to import both public and private keys together;</li>.
<li> - on "partial" mode - checks the pointers and high/low limits of key value;</li>
<li> - on "full" mode - checks also that the the key belongs to the FFC subgroup; </li>
<li> - sets the key data into DH Context according to party's role and key status. </li></ul>
\note Before calling of this function, DH context should be initialized and Scheme and FFC Domain
parameters are inserted by calling appropriate functions, else the function returns an error.
@return CC_OK on success.
@return A non-zero value on failure as defined cc_dh_error.h.
*/
CIMPORT_C CCError_t CC_FfcDhValidateAndImportPublKey(
                                CCFfcDhUserContext_t *pDhUserCtx,  /*!< [in/out] pointer to DH FFC Context structure. */
                                uint8_t *pPublKeyData,             /*!< [in] pointer to given DH FFC public key or Nonce in big endianness;
                                                                          it should be in range [2, P-2], where P is the Domain Prime. */
                                size_t publKeyDataSize,            /*!< [in] public key size, in bytes: should be not great than Domain Prime size. */
                                CCFfcDhKeyValidMode_t validatMode, /*!< [in] enumerator ID defining the validation mode:
                                                                           CC_FFCDH_CHECK_FULL_MODE - full validation (sec. 5.6.2.3.1);
                                                                           CC_FFCDH_CHECK_PARTIAL_MODE - check pointers, sizes and range of values;
                                                                           Note: for Nonce only size and range checking is performed. */
                                CCFfcDhKeyStatus_t keyStatus       /*!< [in] enumerator, defining the key status according to its life time
                                                                          or purpose: static/ephemeral/nonce */
);




/*******************************************************************************************/
/*!
@brief The function checks and sets the FFC DH user's private/public key pair into DH Context
according to NIST SP 800-56A rev.2 sec.5.6.2.3.1 and checking mode:

<ul><li> - if the key belongs to partner's party, then the function returns an error, meaning
that the user should use other function to import only public key;</li>.
<li> - on "partial" mode - checks the pointers and high/low limits of key value;</li>
<li> - on "full" mode - checks also that the the public key meets to private key; </li>
<li> - sets the key data into DH Context according to party's role and key status. </li></ul>
\note Before calling of this function, DH context should be initialized and Scheme and FFC Domain
parameters are inserted by calling appropriate functions, else the function returns an error.
@return CC_OK on success.
@return A non-zero value on failure as defined cc_dh_error.h.
*/
CIMPORT_C CCError_t CC_FfcDhValidateAndImportKeyPair(
                                CCFfcDhUserContext_t *pDhUserCtx,  /*!< [in/out] pointer to DH FFC Context structure. */
                                uint8_t *pPrivKeyData,             /*!< [in] pointer to given DH FFC private key in big endianness;
                                                                             it should be in range [1, n-1], where n is the Domain generator order. */
                                size_t privKeyDataSize,            /*!< [in] private key size, in bytes: should be equaled Domain
                                                                             generator order size. */
                                uint8_t *pPublKeyData,             /*!< [in] pointer to given DH FFC public key in big endianness;
                                                                          it should be in range [2, P-2], where P is the Domain Prime. */
                                size_t publKeyDataSize,            /*!< [in] public key size, in bytes: should be equaled to Domain Prime size,
                                                                             including leading zeros. */
                                CCFfcDhKeyValidMode_t validatMode, /*!< [in] enumerator ID defining the validation mode:
                                                                             CC_FFCDH_CHECK_FULL_MODE - full validation (sec. 5.6.2.3.1);
                                                                             CC_FFCDH_CHECK_PARTIAL_MODE - check pointers, sizes and range of values. */
                                CCFfcDhKeyStatus_t keyStatus       /*!< [in] enumerator, defining the key status according to its life time
                                                                             or purpose: static/ephemeral/nonce */
);

/*******************************************************************************************/
/*!
@brief This function generates random Nonce, used in appropriate DH Schemes (NIST SP 56A rev.2 sec.5.9, 6).
<li> The function generates random vector of given size, sets it into DH context according. </li>
\note Before calling of this function, DH context should be initialized and Scheme parameters and
DH Domain are inserted by calling appropriate functions, else the function returns an error.
\note The Nonce should be generated and the function called only if it is required by DH scheme, and
the Nonce is not inserted previously, else the function returns an error.
\note The function is used when the user not generates an ephemeral key, but requires key confirmation and
therefore Nonce generation.
@return CC_OK on success.
@return A non-zero value on failure as defined cc_dh_error.h.
*/
CIMPORT_C CCError_t CC_FfcDhGenerateRandomNonce(
                                CCFfcDhUserContext_t *pDhUserCtx, /*!< [in/out] pointer to DH FFC Context structure. */
                                CCRndContext_t *pRndContext);     /*!< [in] random generation function context. */



/*******************************************************************************************/
/*!
@brief This function formats the UserInfo according to the user role (PartyU or PartyV) and NIST SP 56A rev.2,
       sec. 5.8.1.2, 5.8.1.2.1.

<ul><li>  Input and previously inserted data is concatenated as defined in the CCFfcDhPartyInfo_t structure and
 sets it into the Context:  UserInfo = UserId||UserStatPublKey||UserStatPublKey||UserNonce}{||UserOtherData}, where: </li>
<li> - UserInfo and each its sub-entry are formatted as length (Len) and then appropriate data: Len||Data,
where each length is a 2-bytes big endianness counter; </li>
<li> - If any sub-entry is not used in chosen DH Scheme, than its lengths should be set 0 and the data is empty. </li>
<li> - total size of PartyInfo, including said lengths, should be not great, than the size of CCDhPartyInfo_t. </li></ul>
\note Before calling of this function the User should initialize DH Context, insert FFC Domain, DH Scheme parameters and
all his Private/Public Keys (or Nonce) using appropriate CC functions.
\note The output from this function will be exported to the other party of the Agreement and vice versa, UserInfo, received
from other party, will be used as input to DhCtxSetSchemeData() function.

@return CC_OK on success.
@return A non-zero value on failure as defined cc_dh_error.h.
*/
CIMPORT_C CCError_t CC_FfcDhCreateUserInfo(
                        CCFfcDhUserContext_t *pDhUserCtx, /*!< [in/out] pointer to context structure, containing all data,
                                                               used in DH Key Agreement Scheme. */
                        uint8_t *pUserOtherData,          /*!< [in] optional, pointer to other data, which the user will
                                                                    insert in addition to its ID, keys and Nonce. */
                        size_t userOtherDataSize,         /*!< [in] optional, size of additional data (in bytes), which the
                                                                    user will include into the UserInfo. */
                        uint8_t *pUserConfirmText,        /*!< [in] optional, pointer to confirmation Text of the User. */
                        size_t  userConfirmTextSize,      /*!< [in] optional size of Text data of partyU, in bytes. */
                        CCFfcDhPartyInfo_t *pUserInfo,    /*!< [out] pointer to the concatenated UserInfo (i.e. PartyU or PartyV Info). */
                        size_t *pUserInfoSize             /*!< [in/out] pointer to the size of UserInfo, in bytes:
                                                                in -  given buffer size (should be not less than CC_FFCDH_MAX_SIZE_OF_OTHER_INFO_ENTRY;
                                                                out - actual size of UserInfo, including length counters */
);


/*******************************************************************************************/
/*!
@brief This function checks and sets given "OtherInfo" entries, calculates shared secret value and
       derives the "secret keying material".
       The function's implementation meets to NIST SP 56A rev.2 standard requirements.
\note Before calling of this function, DH Context should be initialized, DH Scheme, Domain parameters and all
required user's Private, Public keys or nonces are inserted by calling appropriate CC functions.
<ul><li>  The function sets input data into the Context to form the "OtherInfo" (sec. 5.8.1) according to
said standard and the implementation requirements:
<li>  - OtherInfo = AlgorithmId||PartyUInfo||PartyVInfo {||SuppPubInfo}{||SuppPrivInfo}, where each PartyInfo is
formatted as : </li>
<li>  - Remark: AlgorithmId includes information about length in bits of derived Keying Material and its
 parsing between internal using for confirmation HMAC algorithm and output Secret Keying Data
 and algorithm, which it is intended for. </li>
<li>  - PartyInfo = PartyId||PartyStatPublKey||PartyEphemKey||PartyNonce{||PartyOtherData}. </li>
<li>  - for detailed description of "OtherInfo" construction and concatenation its sub-entries, see
CCFfcDhOtherInfo_t structure definition; </li></ul>
\note - the function performs the following calculations:
<ul><li> - calculates shared secret value according to DH Scheme:
   -  SharedSecretVal = (PublKey1 ^ PrivKey1)  modulo Prime  or
   -  SharedSecretVal = (PartnPublKey1 ^ UserPrivKey1) || (PartnPublKey2 ^ UserPrivKey2)  modulo Prime; </li>
<li> - derives the secret keying material of required size from the shared secret value by calling KDF function
with shared OtherInfo data: DerivedKeyingMaterial = KDF(ZZ, OtherInfo, keyingMaterialSize); </li></ul>
<ul><li> - If DH Scheme includes Key Confirmation, then the function calculates confirmation HMAC MacTag, which is
intended to be provided to the partner (sec. 5.2, 5.9, 6); in this case the secret keying material is parsed to MacKey
of size, equaled to HMAC key size. </li>
<li> - in our implementation HMAC key size is defined to be equaled to FFC sub-group order (meets to sec.5.9.3). </li>
<li> - if in the chosen DH Scheme the user is not a Confirmation Provider, then both the pointer and the size of
appropriate MacTag should be set to NULL. </li>
<li>  - for detailed description of Confirmation "MacData" see CCFfcDhConfirmMacData_t structure definition. </li></ul>

@return CC_OK on success.
@return A non-zero value on failure as defined cc_dh_error.h.
*/
CIMPORT_C CCError_t CC_FfcDhSetAndCalculateSchemeData(
                        CCFfcDhUserContext_t *pDhUserCtx, /*!< [in/out] pointer to context structure, containing all data, used in DH Key
                                                                Agreement Scheme, required for implementation of said standard. */
                        /*! Partner's Data to be included into OtherInfo entry. Detailed description see in CCFfcDhOtherInfo_t. */
                        uint8_t *pPartnerInfo,            /*!< [in] pointer to the concatenated PartnerInfo. Detailed description see in CCFfcDhOtherInfo_t. */
                        size_t sizeOfPartnerInfo,         /*!< [in] size of PartnerInfo, in bytes, should be <= CC_FFCDH_MAX_SIZE_OF_PARTY_INFO_BYTES. */
                        CCFfcDhPartyInfoValidMode_t partnInfoValidMode, /*!< enumerator, defining which of public keys (static, ephemeral),
                                                                included in the PartnerInfo, should be full validated and which partial only. */
                        uint8_t *pSuppPubInfo,            /*!< [in] pointer to optional shared public data to be included into SuppPubInfo entry */
                        size_t suppPubInfoSize,           /*!< [in] size of SuppPubInfo data, in bytes. */
                        uint8_t *pSuppPrivInfo,           /*!< [in] pointer to optional shared private data to be included into SuppPrivInfo entry */
                        size_t suppPrivInfoSize,          /*!< [in] size of other SuppPrivInfo data, in bytes (should be not great than
                                                                    CC_FFCDH_MAX_SIZE_OF_OTHER_INFO_SUPPL_ENTRY_BYTES */
                        uint8_t *pUserMacTag,             /*!< [out] optional, pointer to the user-provider confirmation MacTag depending
                                                                   on used Key Agreement Scheme. The tag is calculated by HMAC with given
                                                                   hashMode, as described in SP800-56A sec. 5.9. */
                        size_t  macTagSize                /*!< [in] optional, required size in bytes of confirmation MacTag. */
);


///******************************************************************************************/
///*!
//@brief The function calculates user's confirmation MacTags for FFC DH Schemes according to NIST SP 56A rev.2 standard.
//
//\note Before calling of this function the user should obtain assurance of used FFC Domain and public, private keys,
//involved in the key agreement, using one of the methods, described in sec. 5.6.2 of above named standard.
//<ul><li> - depending on DH Scheme, calculates confirmation HMAC MacTag, which is intended to be provided to the partner
//(sec. 5.2, 5.9, 6); in this case the secret keying material is parsed to MacKey of size, equaled to HMAC key size. </li>
//<li> - in our implementation HMAC key size defined equal to FFC sub-group order (meets to sec.5.9.3). </li>
//<li> - if in the chosen DH Scheme the user is not Confirmation provider, then both the pointer and the size of
//appropriate MacTag should be set to NULL. </li>
//<li>  - for detailed description of Confirmation "MacData" see CCFfcDhConfirmMacData_t structure definition. </li></ul>
//
//@return CC_OK on success.
//@return A non-zero value on failure as defined in cc_dh_error.h, cc_kdf_error.h or cc_hash_error.h.
//*/
//CIMPORT_C CCError_t CC_FfcDhCalcConfirmMacTags(
//                        CCFfcDhUserContext_t *pDhUserCtx, /*!< [in] pointer to the user's DH context structure, containing all data, defining
//                                                                   DH Key Agreement Scheme. The context shall be initialized for user's roles
//                                                                   (U or V; Provider or Receiver) using CC_FfcDhSetCtx function. */
//                        uint8_t *pUserMacTag,             /*!< [out] optional, pointer to the user (provider) confirmation MacTag depending
//                                                                   on used Key Agreement Scheme. The tag is calculated by HMAC with given
//                                                                   hashMode, as described in section 5.9. */
//                        size_t *pMacTagSize,              /*!< [in/out] optional, required size of MacTag, in bytes; maximal allowed size is the
//                                                                   HMAC output size; minimal size is 8 bytes according to tab. 8 of said standard. */
//                        uint8_t *pUserConfirmText,        /*!< [in] optional, pointer to confirmation Text of the User. */
//                        uint8_t  userConfirmTextSize,     /*!< [in] optional size of Text data of partyU, in bytes. */
//                        uint8_t *pPartnerConfirmText,     /*!< [in] optional, pointer to confirmation Text of the Partner. */
//                        uint8_t  partnerConfirmTextSize   /*!< [in] optional, size of Text data of partyV, in bytes. */
//);


/*******************************************************************************************/
/*!
@brief This function performs DH Key Agreement Confirmation and, on success, outputs the shared keying data.
The function calculates expected partner's confirmation MacTag' and compares it to value,
received from the partner.
<li> If the tags are not equaled, then the function returns an error and zeroes the secure
sensitive data. </li>
<li> If no errors, the function puts the derived secret keying data into output buffer. </li>
\note Assumed, that the user yet have obtained assurance of public and private keys,
involved in the key agreement.
\note Before calling this function the user should perform all required DH Key Agreement
operations, including calculation of shared secret keying material by calling
CC_FfcDhCalcUserConfirmMacTag function.
\note If according to chosen Scheme the user is not a Confirmation Recipient,
then all, the pointer and the size of MacTag should be
set to zero, else the function returns an error.

@return CC_OK on success.
@return A non-zero value on failure as defined in cc_dh_error.h
*/
CIMPORT_C CCError_t CC_FfcDhGetSharedSecretKeyingData(
                        CCFfcDhUserContext_t *pDhUserCtx, /*!< [in] pointer to the user's DH context structure, containing all data,
                                                               defining DH Key Agreement Scheme and its results.  */
                        uint8_t *pSecretKeyData,          /*!< [out] pointer to the shared secret keying data, extracted
                                                                from keying material after parsing to . */
                        size_t  *pSecretKeyDataSize,      /*!< [in/out] the pointer to the size of shared secret key data:
                                                               in - size of the given output buffer, out - actual size of extracted
                                                               key data */
                        uint8_t *pPartnerMacTag,          /*!< [in] optional, pointer to the confirmation MacTag, provided by the partner */
                        size_t macTagSize                 /*!< [in] optional, size of partner's MacTag, in bytes */
);



/*******************************************************************************************/
/*!
@brief This function implements FFC DH primitive according to section 5.7.1.1 of NIST SP 56A rev.2 standard.
       The function computes the shared secret value:  SharedSecretVal = partnerPublKey ^ userPrivKey modulo Prime.
\note Before calling of this function the user should obtain assurance of FFC Domain, public and private keys, involved in the key
agreement, using one of methods, described in section 5.6.2 of above named standard.
\note For assurance of keys validity the user can use appropriate APIs for generating or building and validation,
of keys, described in cc_ffcdh.h file.
\note The function intended of-first for internal using in Keying Material derivation inside CC DH functions.
@return CC_OK on success.
@return A non-zero value on failure as defined in cc_dh_error.h or cc_rnd_error.h.
*/
CIMPORT_C CCError_t CC_FfcDhGetSharedSecretVal(
            CCFfcDomain_t *pDomain,         /*!< [in/out] pointer to DH FFC Context structure. */
                        uint8_t *pSharedSecretVal,      /*!< [out] pointer to the shared secret value in big endianness order
                                                                  of bytes in the array (MS-byte is a most left one). This
                                                                  buffer should be at least of prime (modulus) size in bytes. */
                        size_t *pSharedSecretValSize,   /*!< [in/out] pointer to the shared secret value size:
                                                                  input - size of the given buffer, it should be at least
                                                                  prime (modulus) size bytes; output - actual size. */
                        uint8_t *pPrivKeyData,          /*!< [in] pointer to given DH FFC private key in big endianness;
                                                                  the Key should be in range [1, n-1], where n is the Domain
                                                                  generator order. */
                        size_t privKeyDataSize,         /*!< [in] private key size, in bytes: should be not great than Domain
                                                                  generator order size. */
                        uint8_t *pPublKeyData,          /*!< [in] pointer to given DH FFC public key in big endianness;
                                                                  the key should be in range [2, P-2], where P is the Domain Prime. */
                        size_t publKeyDataSize,         /*!< [in] public key size, in bytes: should be not great than Domain Prime size. */
                        uint32_t *pTmpBuff              /*!< [in] pointer to temp buffer of size */
);




#ifdef __cplusplus
}
#endif
/**
@}
 */
#endif
