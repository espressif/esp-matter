/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _CC_FFCDH_LOCAL_H
#define _CC_FFCDH_LOCAL_H

#include "cc_ffc_domain.h"
#include "cc_ffcdh.h"

#ifdef __cplusplus
extern "C"
{
#endif


/************************ Defines ******************************/



/*! DH Context Validation Tag initial value.                                  *
\note low bytes of validation tag are used for setting bits, indicate         *
   that into the context inserted data appropriate to DH Agreement functions  *
*  flow                                                                       */
#define FFCDH_CTX_VALID_TAG_INIT_VAL     0xFFCD0000

/*!< DH Context Validation Tag Bits for appropriate steps of functions flow   *
* according to description in the CCDhCtx_t structure definition.             */
#define FFCDH_CTX_VALID_TAG_INITIALIZATION_BIT  (1UL<<0)  /*!< FFCDH Context is initialised to zero data */
#define FFCDH_CTX_VALID_TAG_SCHEM_PARAM_BIT     (1UL<<1)  /*!< FFCDH Scheme parameters are set */
#define FFCDH_CTX_VALID_TAG_DOMAIN_BIT          (1UL<<2)  /*!< FFCDH Domain is set into Context */
#define FFCDH_CTX_VALID_TAG_USER_STAT_KEY_BIT   (1UL<<3)  /*!< FFCDH user static keys (private and public) are set */
#define FFCDH_CTX_VALID_TAG_USER_EPHEM_KEY_BIT  (1UL<<4)  /*!< FFCDH user ephemeral keys (private and public) are set */
#define FFCDH_CTX_VALID_TAG_USER_NONCE_BIT      (1UL<<5)  /*!< FFCDH user Nonce is set */
#define FFCDH_CTX_VALID_TAG_PARTN_STAT_KEY_BIT  (1UL<<6)  /*!< FFCDH partner static public key */
#define FFCDH_CTX_VALID_TAG_PARTN_EPHEM_KEY_BIT (1UL<<7)  /*!< FFCDH partner ephemeral public key is set */
#define FFCDH_CTX_VALID_TAG_PARTN_NONCE_BIT     (1UL<<8)  /*!< FFCDH partner Nonce is set */
#define FFCDH_CTX_VALID_TAG_USER_INFO_BIT       (1UL<<9)  /*!< FFCDH UserInfo Data is set */
#define FFCDH_CTX_VALID_TAG_SCHEM_DATA_BIT      (1UL<<10) /*!< FFCDH Scheme Data is set */
#define FFCDH_CTX_VALID_TAG_AGREEM_COMPLET_BIT  (1UL<<11) /*!< FFCDH Key Agreement is completed */

/* All validation bits for Private/Public Keys joined together */
#define FFCDH_CTX_VALID_TAG_ALL_KEYS_BITS          \
    (FFCDH_CTX_VALID_TAG_USER_STAT_KEY_BIT  || \
    FFCDH_CTX_VALID_TAG_USER_EPHEM_KEY_BIT  || \
    FFCDH_CTX_VALID_TAG_USER_NONCE_BIT      || \
    FFCDH_CTX_VALID_TAG_PARTN_STAT_KEY_BIT  || \
    FFCDH_CTX_VALID_TAG_PARTN_EPHEM_KEY_BIT || \
    FFCDH_CTX_VALID_TAG_PARTN_NONCE_BIT)

/*!< DH Context Validation Tags for appropriate steps of functions flow       *
* according to description in the CCDhCtx_t structure definition.             */
#define FFCDH_CTX_VALID_TAG_SCHEM_PARAM_SET   (FFCDH_CTX_VALID_TAG_INIT_VAL         || FFCDH_CTX_VALID_TAG_SCHEM_PARAM_BIT)    /*!< DH Scheme parameters are set */
#define FFCDH_CTX_VALID_TAG_DOMAIN_SET        (FFCDH_CTX_VALID_TAG_SCHEM_PARAM_SET  || FFCDH_CTX_VALID_TAG_DOMAIN_BIT)         /*!< DH Domain is set into Context */
#define FFCDH_CTX_VALID_TAG_ALL_KEYS_SET      (FFCDH_CTX_VALID_TAG_DOMAIN_SET       || FFCDH_CTX_VALID_TAG_ALL_KEYS_BITS)      /*!< DH all keys are set */
#define FFCDH_CTX_VALID_TAG_USER_INFO_SET     (FFCDH_CTX_VALID_TAG_ALL_KEYS_SET     || FFCDH_CTX_VALID_TAG_USER_INFO_BIT)      /*!< DH User Info is set */
#define FFCDH_CTX_VALID_TAG_SCHEM_DATA_SET    (FFCDH_CTX_VALID_TAG_USER_INFO_SET    || FFCDH_CTX_VALID_TAG_SCHEM_DATA_BIT)     /*!< DH Scheme Data is set */
#define FFCDH_CTX_VALID_TAG_AGREEM_COMPLETED  (FFCDH_CTX_VALID_TAG_KEY_MATERIAL_SET || FFCDH_CTX_VALID_TAG_AGREEM_COMPLET_BIT) /*!< DH Key Agreement is completed */

/* the macro converts bit value (0,1) and offset to to appropriate number = bit<<offset. */
#define FFCDH_OFFS_TO_VAL(bit, offset)  ((bit) << (offset))

#ifdef FFC_FURTHER_USING
/*! Offsets of bits defining FFC DH Scheme parameters in Scheme Info variable.
 *  Each enum. value defines offset of the bit, indicating that the named parameter
 *  is needed according to Scheme, user party (U,V) and confirmation role */
typedef enum
{
    FFCDH_DO_CTX_INIT,             /*!< do DH context init */
    FFCDH_DO_SCHEME_PARAMS,        /*!< insert Scheme parameters */
    FFCDH_DO_DOMAIN,               /*!< insert Domain parameters */
    FFCDH_DO_USER_STAT_KEY,        /*!< user should have static key */
    FFCDH_DO_USER_EPHEM_KEY,       /*!< user should have ephemeral key */
    FFCDH_DO_USER_NONCE,           /*!< user should have Nonce key */
    FFCDH_DO_PARTN_STAT_KEY,       /*!< partner should have static key */
    FFCDH_DO_PARTN_EPHEM_KEY,      /*!< partner should have static key */
    FFCDH_DO_PARTN_NONCE,          /*!< partner should have static key */
    FFCDH_DO_CONFIRM_PROVID,       /*!< user is confirm provider */
    FFCDH_DO_CONFIRM_RECIP,        /*!< user is confirm recipient */

    FFCDH_DO_OFF_MODE,             /*!< not allowed value */
    FFCDH_DO_PARAMS_LAST = 0x7FFFFFFF
} FfcDhSchemeInfoBits_t;


/* Define DH Scheme Info bits for given FFCDH Scheme , user party (U,V) and confirmation role */
/* Scheme: dhHybrid1 */
#define FFCDH_HYBRID1_U  ((1UL<<FFCDH_DO_USER_STAT_KEY)||(1<<FFCDH_DO_USER_EPHEM_KEY)|| \
                      (1<<FFCDH_DO_PARTN_STAT_KEY)||(1<<FFCDH_DO_PARTN_EPHEM_KEY))
#define FFCDH_HYBRID1_V FFCDH_HYBRID1_U
/* Scheme: dhEphem: NoConfirm */
#define FFCDH_EPHEM_U  ((1UL<<FFCDH_DO_USER_STAT_KEY)||(1<<FFCDH_DO_USER_EPHEM_KEY)|| \
                    (1<<FFCDH_DO_PARTN_STAT_KEY)||(0<<FFCDH_DO_PARTN_EPHEM_KEY))
#define FFCDH_EPHEM_V  FFCDH_EPHEM_U
/* Scheme: dhHybridOneFlow_U */
#define FFCDH_HYBRID_ONE_FLOW_U  ((1UL<<FFCDH_DO_USER_STAT_KEY)||(1<<FFCDH_DO_USER_EPHEM_KEY)|| \
                              (1<<FFCDH_DO_PARTN_STAT_KEY)||(0<<FFCDH_DO_PARTN_EPHEM_KEY))
#define FFCDH_HYBRID_ONE_FLOW_V  ((1UL<<FFCDH_DO_USER_STAT_KEY)||(0<<FFCDH_DO_USER_EPHEM_KEY)|| \
                              (1<<FFCDH_DO_PARTN_STAT_KEY)||(1<<FFCDH_DO_PARTN_EPHEM_KEY))
/* Scheme: dhOneFlow_U */
#define FFCDH_ONE_FLOW_U  ((0UL<<FFCDH_DO_USER_STAT_KEY)||(1<<FFCDH_DO_USER_EPHEM_KEY)|| \
                       (1<<FFCDH_DO_PARTN_STAT_KEY)||(0<<FFCDH_DO_PARTN_EPHEM_KEY))
#define FFCDH_ONE_FLOW_V  ((1UL<<FFCDH_DO_USER_STAT_KEY)||(0<<FFCDH_DO_USER_EPHEM_KEY)|| \
                       (0<<FFCDH_DO_PARTN_STAT_KEY)||(1<<FFCDH_DO_PARTN_EPHEM_KEY))
/* Scheme: dhStatic */
#define FFCDH_STATIC_U  ((1UL<<FFCDH_DO_USER_STAT_KEY)||(0<<FFCDH_DO_USER_EPHEM_KEY)|| \
                      (1<<FFCDH_DO_PARTN_STAT_KEY)||(0<<FFCDH_DO_PARTN_EPHEM_KEY))
#define FFCDH_STATIC_V  FFCDH_STATIC_U
#endif


/************************** Macros ******************************/

/* The macro sets uint16 length (len) into bytes-buffer, given by the pointer (ptr) as
 * 2-bytes big endianness counter, and then promote the pointer by 2 bytes.
 * Note: Dst buffer should be given by pointer (not by buffer name). */
#define FFCDH_SET_LENGTH(ptr, len) \
    (ptr)[0] = ((len)>>1) & 0xFF; (ptr)[1] = (len) & 0xFF; \
    (ptr) += CC_FFCDH_LENGTH_COUNTER_SIZE_IN_BYTES;
/* The macro reads length (2 bytes BE number) from bytes-buffer into uint16 value
 * and promotes pointer to next buffer (sub-entry).
 * Note: Dst buffer should be given by pointer (not by buffer name). */
#define FFCDH_GET_LENGTH(ptr) (((uint16_t)((ptr)[0])<< 8) | (ptr)[1]); \
    (ptr) += CC_FFCDH_LENGTH_COUNTER_SIZE_IN_BYTES;
#define FFCDH_SET_OFFSET(prevOffset, prevDataSize)  ((prevOffset) + (prevDataSize) + CC_FFCDH_LENGTH_COUNTER_SIZE_IN_BYTES)
/* macro sets offset depending on condition value which shall be 1 or 0. If it is 1,
 * then prevDataSize is added to offset, else - not added. All sizes are uint16_t values. */
#define FFCDH_COND_SET_OFFSET(prevOffset, prevDataSize, conditionVal)  \
    ((prevOffset) + CC_FFCDH_LENGTH_COUNTER_SIZE_IN_BYTES + ((prevDataSize) & (uint16_t)((0UL - (conditionVal)) & 0xFFFFUL)))

/* Macros for checking DH Context validation tag according to
 * DH functions flow stage. In other words, check that all required
 * previous stages (functions calls) are done. If all is done, the macro
 * returns 0, else returns 1.
 */
#define FFCDH_CHECK_CTX_VALID_TAG_BITS(validTag, checkBits) (((validTag) & (checkBits)) != (checkBits))

/* enumerator, defining parameters */
typedef enum
{
    FFCDH_NO_PARAM,
    FFCDH_DO_PARAM,
    FFCDH_PARAM_DONE
}FfcDhParamsSetting_t;


/************************ Structures ******************************/


/*! Structure, defining which FFC DH parameters are involved in the Scheme
    according to Scheme ID, user party (U,V) and confirmation role. If some
    member of the structure is equal to 1, then appropriate  parameter is
    needed and if 0 - not. */
typedef struct
{
    uint32_t doUserStatKey;
    uint32_t doUserEphemKey;
    uint32_t doUserNonce;
    uint32_t doPartnerStatKey;
    uint32_t doPartnerEphemKey;
    uint32_t doPartnerNonce;
    uint32_t doConfirmProvid;
    uint32_t doConfirmRecip;

}FfcDhSchemeInfo_t;


/*! Structure, defining sizes and offsets of OtherInfo sub-entries
 *  inside the KdfBuffer buffer, placed in the DH context (see SP 56A, sec. 5.8.1.2, 5.8.1.2.1).
 *  extendOtherInfo = counter||ZZ||AlgId||PartyUInfo||PartyVinfo||suppPublInfo||suppPrivInfo;
 *  Each PartyInfo = PartyId||StatPublKeyInfo||EphemKeyInfo||nonce||OtherPartyData
 *  Note: - Each sub-entry includes 2-bytes, defining its size.
 *        - If any sub-entry is not used then its size should be set to 0 and data - empty.
 *        - size of the structure - 21 word = 84 bytes. */
typedef struct
{
    uint32_t  kdfCounterOffset;        /*!< KDF counter offset */
    uint32_t  sharedSecrOffset;        /*!< offset of the Algorithm ID data */

    /* Other Info Entries */
    uint32_t  algIdOffset;             /*!< offset of the Algorithm ID data */
    uint32_t  userIdOffset;            /*!< offset of the userIdOffset = userInfo */
    uint32_t  userStatPublKeyOffset;   /*!< offset of the static public key. */
    uint32_t  userEphemPublKeyOffset;  /*!< offset of the ephemeral key. */
    uint32_t  userNonceOffset;         /*!< offset of nonce (if present) */
    uint32_t  userOtherDataOffset;     /*!< offset of user other (additional) data */

    uint32_t  partnIdOffset;           /*!< offset of the partnIdOffset = partnInfo */
    uint32_t  partnStatPublKeyOffset;  /*!< offset of the static public key. */
    uint32_t  partnEphemPublKeyOffset; /*!< offset of the ephemeral key. */
    uint32_t  partnNonceOffset;        /*!< offset of nonce (if present) */
    uint32_t  partnOtherDataOffset;    /*!< offset of partner other (additional) data */

    uint32_t  suppPublInfoOffset;      /*!< offset of supplied public Info data */
    uint32_t  suppPrivInfoOffset;      /*!< offset of supplied private Info data */

    /* data entries sizes in bytes */
    uint32_t  sharedSecrSize;          /*!< size of the Algorithm ID data */
    uint32_t  algIdSize;               /*!< offset of the Algorithm ID data */
    uint32_t  userInfoSize;            /*!< full size of user data */
    uint32_t  partnInfoSize;           /*!< full size of partner data */
    uint32_t  suppPublInfoSize;        /*!< size of supplied public Info data */
    uint32_t  suppPrivInfoSize;        /*!< size of supplied private Info data */

}FfcDhSchemeDataOffsets_t;





/**************************************************************/
/*! DH Key Agreement  context structure, used in DH functions.
    The context contains appropriate parameters and data in accordance with NIST SP 800-56A Rev. 2 standard
    and this implementation definitions.
    \note  Some buffers for internal using of functions may be set on tmpBuff structure in the context.
    \note  The domain parameters and keys are saved in arrays with little endianness order of bytes and words
            (i.e. LS byte/word are right most) with indication of entry size.
    \note  Public keys, when are used in KDF and MAC calculations, are presented as big endianness bytes arrays
           with constant size, equalled to field size, and leading zeros, if existed.
    \note  Max.size of Context:
*/
typedef struct
{
        uint32_t validTag; /*!< the context validation tag, indicates that the context and its members are set
                                according to current stage of DH functions flow. The bits of LS Bytes of validTag
                                indicate, that appropriate function was done or data inserted (1 - done, 0 - not done):
                                b'0 - context initialised to zero;
                                b'1 - DH Scheme parameters (DH Scheme, HASH mode, KDF mode, user party,
                                      confirmation mode etc.) are inserted;
                                b'2 - FFC Domain is inserted;
                                b'3,4 - user's static/ephemeral keys, needed by the Scheme, are inserted;
                                b'5,6 - partner's static/ephemeral keys, needed by the Scheme, are inserted;
                                b'7 - PartyUInfo, PartyVInfo and Confirmation TextU, TextV are inserted;
                                b'8 - Shared Secret Keying Material and user's MacTag are calculated;
                                b'9 - calculating of expected partner's MacTag and its comparing with the
                                      value, received from the partner, are done. */

        /*! DH FFC Domain parameters structure (p,q,g,{seed,genCounter}. */
        CCFfcDomain_t ffcDomain; // 636 bytes
        /*!  FFC DH Key Agreement Scheme parameters */
        CCFfcDhSchemeId_t dhSchemeId;      /*!< enumerator ID of used FFC DH Key Agreement Scheme (see sec. 6, table 12). */
        CCFfcDhKdfModeSp56A_t kdfMode;     /*!< enumerator ID of used KDF function supported KDF, based on
                                             HASH or HMAC algorithms. */
        CCFfcHashOpMode_t ffcHashMode;     /*!< enumerator ID of used SHA-x HASH mode, supported by the
                                             product (Note: MD5 is not supported). */
        CCFfcDhUserPartyIs_t userParty;    /*!< enumerator, defining user (i.e. the function's caller) party in DH Agreement (U or V). */
        CCFfcDhUserConfirmMode_t confirmMode; /*!< enumerator, defining confirmation mode of each party: is it provider
                                                   or/and recipient, according to sec. 5.9. */
        CCFfcParamSetId_t ffcParamSet;     /*!< enumerator, defining the set of FFC domain parameters
                                              according to SP 56A rev.2 section 5.5.1.1, tab.1. and FIPS 186-4 sec.4.2. */
        /*! Internal structure "schemeInfo": each its member indicates that appropriate scheme parameter or operation is required
            according to chosen Scheme, user party (U,V) and Confirmation role. Size of buffer 8*sizeof(uint32_t). */
        FfcDhSchemeInfo_t schemeInfo;

        uint32_t hmacSaltSizeBytes;
        uint32_t hmacSalt[CC_FFCDH_MAX_SIZE_OF_HMAC_SALT_BUFF_BYTES/CC_32BIT_WORD_SIZE];
        uint32_t nonceSize; /* size of nonce, agreed by both parties: shall be = order size, if exist, or 0 otherwise */

        /*! Derived secret keying data and keying material sizes (in bytes) and buffer for it placing. */
        uint32_t secretKeyingDataSize; /* size keying data to be derived and used as result output secretKeyingData. */
        /*! the derived Keying material, which consists (parsed) from two keys: HMAC Key (of size = size of HASH,
         *  used in confirmation) and secretKeyingData of above stated size */
        uint8_t  derivedKeyingMaterial[CC_FFCDH_MAX_SIZE_OF_KEYING_MATERIAL_BYTES];
        uint32_t derivedKeyingMaterialSize; /* size in bytes). */

        /*!  User's private static, ephemeral keys and Nonce (if present):
         *   Note: Size of each one of private keys buffer is equaled to actual FFC generator order size;
         *         bytes and words order is little endianness and leading zeros are present. */
        uint32_t statPrivKeySizeBytes;
        uint32_t statPrivKey[CC_FFCDH_MAX_GENER_ORDER_SIZE_IN_WORDS];  /*!<  user's static private key. */
        uint32_t ephemPrivKeySizeBytes;
        uint32_t ephemPrivKey[CC_FFCDH_MAX_GENER_ORDER_SIZE_IN_WORDS];  /*!<  user's ephemeral private key. */

//       uint32_t userInfoSize, partnInfoSize;
        /*!  User and partner public keys: static, ephemeral and nonce (if present):
         *   userStaticPublic, userEphemerPublic keys and nonce,
         *   Max.size of two structures: 2*(2*ModSize + OrderSize + 3w) = 520 bytes*/
//        CCFfcDhCtxPublKeys_t userPublKeys; // max.size: 2*ModSize + OrderSize + 8 = 520 bytes
//        CCFfcDhCtxPublKeys_t partnPublKeys; // max.size 2*256+8 = 520 bytes
        // The structure userPublKeys includes:
        uint32_t userIdSizeBytes; /*!< user's ID actual size in bytes  */
        uint8_t  userId[CC_FFCDH_MAX_SIZE_OF_PARTY_ID_BYTES]; /*!<  user's ID. */
        uint32_t userStatPublKeySizeBytes;  /*!< user's static ephemeral public key size in bytes  */
        uint32_t userStatPublKey[CC_FFCDH_MAX_MOD_SIZE_IN_WORDS]; /*!<  user's static ephemeral public key. */
        uint32_t userEphemPublKeySizeBytes;  /*!< user's ephemeral public key size in bytes  */
        uint32_t userEphemPublKey[CC_FFCDH_MAX_MOD_SIZE_IN_WORDS]; /*!<  user's ephemeral public key. */
        uint32_t userNonceSizeBytes;  /*!< actual size of user's Nonce {0 if not inserted)  */
        uint8_t  userNonce[CC_FFCDH_MAX_GENER_ORDER_SIZE_IN_BYTES]; /*!<  user's Nonce */
        uint32_t userConfirmTextSize; /*!< [in] optional size of Text data of partyU, in bytes. */
        uint8_t  userConfirmText[CC_FFCDH_MAX_SIZE_OF_CONFIRM_TEXT_DATA_BYTES]; /*!< confirmation Text of the User. */

        /*!  Partner's public key/keys: static and ephemeral (if present): */
        // The structure partnPublKeys includes:
        uint32_t partnerIdSizeBytes;  /*!< user's ID actual size in bytes  */
        uint8_t  partnerId[CC_FFCDH_MAX_SIZE_OF_PARTY_ID_BYTES];  /*!<  user's ID. */
        uint32_t partnerStatPublKeySizeBytes;  /*!< partner's static ephemeral public key size in bytes  */
        uint32_t partnerStatPublKey[CC_FFCDH_MAX_MOD_SIZE_IN_WORDS];  /*!<  partner's static ephemeral public key. */
        uint32_t partnerEphemPublKeySizeBytes;  /*!< partner's ephemeral public key size in bytes  */
        uint32_t partnerEphemPublKey[CC_FFCDH_MAX_MOD_SIZE_IN_WORDS];  /*!<  partner's ephemeral public key. */
        uint32_t partnerNonceSizeBytes;  /*!< partner's's Nonce */
        uint8_t  partnerNonce[CC_FFCDH_MAX_GENER_ORDER_SIZE_IN_BYTES];  /*!<  partner Nonce */
        uint32_t partnerConfirmTextSize; /*!< [in] size of the Text data of the partner, in bytes. */
        uint8_t  partnerConfirmText[CC_FFCDH_MAX_SIZE_OF_CONFIRM_TEXT_DATA_BYTES]; /*!< confirmation Text of the Partner. */

        /* buffers to be included in DH KDF data and OtherInfo data */
//        uint8_t commonBuffForKdf[CC_FFCDH_MAX_SIZE_OF_OTHER_INFO_COMMON_BUFF]; /* KdfCounter || Zz || AlgId */
//        uint8_t userInfo[CC_FFCDH_MAX_SIZE_OF_PARTY_INFO_BYTES];    /* user Info: userId||userStatPublKey||userephemerPublKey||userNonce */
//        uint8_t partnerInfo[CC_FFCDH_MAX_SIZE_OF_PARTY_INFO_BYTES]; /* partner Info: partnerId||partnerStatPublKey||partnerEphemerPublKey||partnerNonce */
//        uint8_t supplPublInfo[CC_FFCDH_MAX_SIZE_OF_OTHER_INFO_SUPPL_ENTRY_BYTES]; /* supplied public data known for both parties */
//        uint8_t supplPrivInfo[CC_FFCDH_MAX_SIZE_OF_OTHER_INFO_SUPPL_ENTRY_BYTES]; /* supplied private data known for both parties */



        /*!  FFC DH extended Data buffer, containing data used for extraction secret keying material by KDF function.
         *   The buffer includes: Counter||SharedSecretZz||OtherInfo data. See sec. 5.8.1, 5.8.1.1, 5.8.1.2. */
        uint8_t  extendDataBuffer[CC_FFCDH_MAX_SIZE_OF_KDF_DATA_BUFFER_BYTES];
        uint32_t currInsertedDataSize;
        uint32_t otherInfoSize;

//        CCFfcDhOtherInfo_t otherInfoBuff;   /*!< buffer, containing "other data", shared by both key agreement
//                                             parties and constructed according to concatenation method, described
//                                             in sec. 5.8.1 of said standard current implementation requirements.
//                                             For detailed description see CCFfcDhOtherInfo_t definition.
//                                             Max.size 884 bytes.  */

        /*! Structure, containing offsets and sizes of  parties Info in OterInfo buffer (in bytes) */
        FfcDhSchemeDataOffsets_t dataOffsets;

//        CCFfcDhConfirmMacData_t macData;  /*!< buffer for confirmation data, ordered according
//                                            to sec. 5.9, 5.9.1.1 and party role (U,V). Max size 614 bytes. */
        //        CCFfcDhConfirmMacData_t macDataV;  /*!< structure, containing confirmation data ordered by provider V,
//                                             according to sec. 5.9, 5.9.1.1 */
        /*!  FFC DH Key Agreement Scheme MacTags */
        CCHashResultBuf_t userMacTag;    /*!< buffer for calculation confirmation MacTag of user as provider. */
        CCHashResultBuf_t partnerMacTag; /*!< buffer for calculation confirmation MacTag of partner as provider. */
        uint32_t macTagSize;             /*!< size in bytes of confirmation MacTag-s */
        /*! Temp buffer, used in internal calculations */
        CCFfcDhTemp_t tmpBuff;           /*!< [in] structure, containing internal temp buffers, used in DH functions,
                                             such as Derived Keying Data, offsets of specific entries in otherInfo, MacData ,
                                             work space buffers etc. */
} DhContext_t;


/* Check Context size: print error message if the size is not correct */
#define DH_LOCAL_CONTEXT_SIZE_BYTES \
ROUNDUP_BYTES_TO_32BIT_WORD((FFC_DOMAIN_SIZE_BYTES + CC_FFCDH_MAX_SIZE_OF_HMAC_SALT_BUFF_BYTES + CC_FFCDH_MAX_SIZE_OF_KEYING_MATERIAL_BYTES + \
4*CC_FFCDH_MAX_GENER_ORDER_SIZE_IN_BYTES/*2priv,2nonce*/ + 2*CC_FFCDH_MAX_SIZE_OF_PARTY_ID_BYTES + 4*CC_FFCDH_MAX_MOD_SIZE_IN_BYTES + \
CC_FFCDH_MAX_SIZE_OF_KDF_DATA_BUFFER_BYTES + 2*CC_HASH_RESULT_SIZE_IN_WORDS*CC_32BIT_WORD_SIZE/*MacTags*/ + 32/*schemeInfo*/ + \
84/*dataOffsets*/ + 2*CC_FFCDH_MAX_SIZE_OF_CONFIRM_TEXT_DATA_BYTES + 26*CC_32BIT_WORD_SIZE/*separ.words*/ + CC_FFCDH_CTX_TMP_BUFF_MAX_SIZE_IN_WORDS*CC_32BIT_WORD_SIZE))

#if (CC_FFCDH_CONTEXT_BUFF_SIZE_IN_BYTES != DH_LOCAL_CONTEXT_SIZE_BYTES)
#error CC_FFCDH_CONTEXT_SIZE_IN_WORDS defined not correct.
#endif

/* Data base array, indicating combination of parameters and operations, required by different Schemes.
 *
 * This 3-dimensional array contains structures of mentioned above parameters appropriates, for each Schemes.
 */
//FfcDhSchemeInfo_t ffcdhSchemeMatrix[CC_FFCDH_SCHEM_NUM_OFF_MODE][CC_FFCDH_PARTY_NUM_OFF_MODE][CC_FFCDH_CONFIRM_NUM_OFF_MODE];
//
//#define FFCDH_SCHEMES_MATRIX




#ifdef __cplusplus
}
#endif

#endif

