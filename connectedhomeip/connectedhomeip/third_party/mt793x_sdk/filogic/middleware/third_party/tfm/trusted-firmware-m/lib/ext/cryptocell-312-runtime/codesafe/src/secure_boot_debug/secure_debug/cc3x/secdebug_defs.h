/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef _SECDEBUG_DEFS_H
#define _SECDEBUG_DEFS_H

//#include "cc_pal_types_plat.h"
#include "cc_pal_sb_plat.h"
#include "secureboot_basetypes.h"
#include "secdebug_api.h"
#include "bootimagesverifier_def.h"
#include "dx_nvm.h"
#include "dx_crys_kernel.h"
#include "rsa_bsv.h"
#include "cc_crypto_defs.h"
#include "secureboot_gen_defs.h"
#include  "secureboot_parser_gen_defs.h"

#define CC_BSV_SEC_DEBUG_HASH_SIZE_IN_WORDS (HASH_SHA256_DIGEST_SIZE_IN_BYTES/CC_32BIT_WORD_SIZE)

#define CC_BSV_SEC_DEBUG_DCU_SIZE_IN_BITS   128
#define CC_BSV_SEC_DEBUG_DCU_SIZE_IN_BYTES   (CC_BSV_SEC_DEBUG_DCU_SIZE_IN_BITS/CC_BITS_IN_BYTE)
#define CC_BSV_SEC_DEBUG_DCU_SIZE_IN_WORDS   (CC_BSV_SEC_DEBUG_DCU_SIZE_IN_BITS/CC_BITS_IN_32BIT_WORD)

/*! Defines SOC ID */
typedef uint8_t        SocId_t[CC_BSV_SEC_DEBUG_SOC_ID_SIZE];
/*! Defines DCU */
typedef uint32_t       Dcu_t[CC_BSV_SEC_DEBUG_DCU_SIZE_IN_WORDS];

typedef struct {  // must be word aligned!!!
        CCSbNParams_t   pubKey;
        CCSbSignature_t     signature;
} workspaceInt_t;


/*! Secure Boot key certificate magic number. "S,B,K,C" */
#define CC_SB_KEY_CERT_MAGIC_NUMBER     0x53426b63
/*! Secure Boot content certificate magic number.  "S,B,C,C" */
#define CC_SB_CONTENT_CERT_MAGIC_NUMBER     0x53426363
/*! Certificate debug enabler magic number. */
#define CC_CERT_SEC_DEBUG_ENABLER_MAGIC     0x5364656E
/*! Certificate debug developer magic number. */
#define CC_CERT_SEC_DEBUG_DEVELOPER_MAGIC   0x53646465



/* The  restiction mask is actualy teh ICV ownership mask; meaning all ICV bits are set to 1 , OEM bit are 0 */
#define DX_AO_ICV_DCU_OWNERSHIP_MASK0_REG_OFFSET                 DX_AO_ICV_DCU_RESTRICTION_MASK0_REG_OFFSET
#define DX_AO_ICV_DCU_OWNERSHIP_MASK0_VALUE_BIT_SHIFT           DX_AO_ICV_DCU_RESTRICTION_MASK0_VALUE_BIT_SHIFT
#define DX_AO_ICV_DCU_OWNERSHIP_MASK0_VALUE_BIT_SIZE               DX_AO_ICV_DCU_RESTRICTION_MASK0_VALUE_BIT_SIZE
#define DX_AO_ICV_DCU_OWNERSHIP_MASK1_REG_OFFSET                 DX_AO_ICV_DCU_RESTRICTION_MASK1_REG_OFFSET
#define DX_AO_ICV_DCU_OWNERSHIP_MASK1_VALUE_BIT_SHIFT           DX_AO_ICV_DCU_RESTRICTION_MASK1_VALUE_BIT_SHIFT
#define DX_AO_ICV_DCU_OWNERSHIP_MASK1_VALUE_BIT_SIZE             DX_AO_ICV_DCU_RESTRICTION_MASK1_VALUE_BIT_SIZE
#define DX_AO_ICV_DCU_OWNERSHIP_MASK2_REG_OFFSET                 DX_AO_ICV_DCU_RESTRICTION_MASK2_REG_OFFSET
#define DX_AO_ICV_DCU_OWNERSHIP_MASK2_VALUE_BIT_SHIFT           DX_AO_ICV_DCU_RESTRICTION_MASK2_VALUE_BIT_SHIFT
#define DX_AO_ICV_DCU_OWNERSHIP_MASK2_VALUE_BIT_SIZE               DX_AO_ICV_DCU_RESTRICTION_MASK2_VALUE_BIT_SIZE
#define DX_AO_ICV_DCU_OWNERSHIP_MASK3_REG_OFFSET                 DX_AO_ICV_DCU_RESTRICTION_MASK3_REG_OFFSET
#define DX_AO_ICV_DCU_OWNERSHIP_MASK3_VALUE_BIT_SHIFT           DX_AO_ICV_DCU_RESTRICTION_MASK3_VALUE_BIT_SHIFT
#define DX_AO_ICV_DCU_OWNERSHIP_MASK3_VALUE_BIT_SIZE               DX_AO_ICV_DCU_RESTRICTION_MASK3_VALUE_BIT_SIZE

/********* Certificate structure definitions ***********/


/*! Certificate header structure. */
typedef struct {
        uint32_t magicNumber;           /*!< Magic number to validate the certificate. */
        uint32_t certVersion;           /*!< Certificate version to validate the certificate. */
        uint32_t certSize;              /*!< Offset in words to the Certificate signature.
                                            And number of SW components , if any exist.*/
        uint32_t certFlags;             /*!< Bit field according to certificate type */
}CCSbCertHeader_t;


// All certificate header flags, first 4 bits are for certificate type,
// next 4 bits are rsa algorithm used.
// for  key certficate and enabler ecrtificate next 4 bits are HBK-id used

/* Key certificate definitions */
typedef union {
        struct {
                uint32_t      hbkId:4;  // must be first
                uint32_t      reserved:28;
        }flagsBits;
        uint32_t      flagsWord;
} keyCertFlags_t;

typedef struct {
        uint32_t    swVer;
        CCHashResult_t      nextPubKeyHash;
} KeyCertMain_t;


typedef struct {
        CCSbCertHeader_t    certHeader;
        CCSbNParams_t           certPubKey;
        KeyCertMain_t       certBody;
        CCSbSignature_t         certSign;
} KeyCert_t;

/* Content certificate definitions */
/*! Content Certificate flag bit field structure. */
typedef union {
        /*! Flags definitions in bits.*/
        struct {
                uint32_t      hbkId:4;
                uint32_t      swCodeEncType:4;
                uint32_t      swLoadVerifyScheme:4;
                uint32_t      swCryptoType:4;
                uint32_t      numOfSwCmp:16;
        }flagsBits;
        /*! Flags definition as a word.*/
        uint32_t      flagsWord;
} CCSbCertFlags_t;


typedef struct {
        CCHashResult_t      imageHash;
        CCAddr_t    loadAddr;
        uint32_t    imageMaxSize;
        uint32_t    isAesCodeEncUsed;
} ContentCertImageRecord_t;

typedef struct {
        uint32_t    swVer;
        CCSbNonce_t     nonce;
        ContentCertImageRecord_t    imageRec[CC_SB_MAX_NUM_OF_IMAGES];
} ContentCertMain_t;



typedef struct {
        CCSbCertHeader_t    certHeader;
        CCSbNParams_t           certPubKey;
        ContentCertMain_t   certBody;
        CCSbSignature_t         certSign;
} ContentCert_t;


/* Enabler certificate definitions */
typedef union {
        struct {
                uint32_t      hbkId:4; // must be first
                uint32_t      lcs:4;
                uint32_t      isRma:4;
                uint32_t      reserved:20;
        }flagsBits;
        uint32_t      flagsWord;
} EnablerCertFlags_t;

/* definition for enabler certificate */
typedef struct {
        Dcu_t       debugMask;
        Dcu_t       debugLock;
        CCHashResult_t      nextPubKeyHash;
} EnablerCertMain_t;

typedef struct {
        CCSbCertHeader_t    certHeader;
        CCSbNParams_t       certPubKey;
        EnablerCertMain_t   certBody;
        CCSbSignature_t         certSign;
} EnablerCert_t;

/* Developer certificate definitions */
typedef struct {
        struct {
                uint32_t      reserved:32;
        }flagsBits;
        uint32_t      flagsWord;
} DeveloperCertFlags_t;

typedef struct {
        Dcu_t       debugMask;
        SocId_t         socId;
} DeveloperCertMain_t;

typedef struct {
        CCSbCertHeader_t    certHeader;
        CCSbNParams_t        certPubKey;
        DeveloperCertMain_t certBody;
        CCSbSignature_t         certSign;
} DeveloperCert_t;



/*! Certificate types structure. */
typedef enum {
        /*! Reserved.*/
        CC_SB_MIN_CERT,
        /*! Key certificate. */
        CC_SB_KEY_CERT = 1,
        /*! Content certificate. */
        CC_SB_CONTENT_CERT = 2,
        /*! Key or content certificate. */
        CC_SB_KEY_OR_CONTENT_CERT = 3,
        /*! Debug enabler certificate. */
        CC_SB_ENABLER_CERT = 4,
        /*! Debug developer certificate. */
        CC_SB_DEVELOPER_CERT = 5,
        /*! Max number of certificates types.*/
        CC_SB_MAX_CERT,
        /*! Reserved.*/
        CC_SB_CERT_TYPE_LAST = 0x7FFFFFFF

}CCSbCertTypes_t;


/*! Certificate types structure. */
typedef enum {
        /*! First certificate in chain.*/
        CC_SB_FIRST_CERT_IN_CHAIN = 0,
        /*! Second certificate in chain.*/
        CC_SB_SECOND_CERT_IN_CHAIN = 1,
        /*! Third and last certificate in chain.*/
        CC_SB_THIRD_CERT_IN_CHAIN = 2,
        /*! Last certificate in chain.*/
        CC_SB_LAST_CERT_IN_CHAIN = 3,
        /*! Reserved.*/
        CC_SB_RESERVED_CERT_IN_CHAIN = 0x7FFFFFFF

}CCSbCertOrderInChain_t;



/*!  MAX size of certificate pkg. */
#ifdef CC_SB_X509_CERT_SUPPORTED
#define CC_SB_MAX_KEY_CERT_SIZE_IN_BYTES    (0x500UL)
#define CC_SB_MAX_CONTENT_CERT_SIZE_IN_BYTES    (0x7A0UL) /* may contain up to 16 signed sw images */
#define CC_SB_MAX_ENABLER_CERT_SIZE_IN_BYTES    (0x500UL)
#define CC_SB_MAX_DEVELOPER_CERT_SIZE_IN_BYTES  (0x500UL)
#define CC_SB_MAX_CERT_SIGN_SIZE_IN_BYTES               (0x1D0)
#else
#define CC_SB_MAX_KEY_CERT_SIZE_IN_BYTES    (sizeof(KeyCert_t))
#define CC_SB_MAX_CONTENT_CERT_SIZE_IN_BYTES    (sizeof(ContentCert_t)) /* may contain up to 16 signed sw images */
#define CC_SB_MAX_ENABLER_CERT_SIZE_IN_BYTES    (sizeof(EnablerCert_t))
#define CC_SB_MAX_DEVELOPER_CERT_SIZE_IN_BYTES  (sizeof(DeveloperCert_t))
#define CC_SB_MAX_CERT_SIGN_SIZE_IN_BYTES               (sizeof(CCSbSignature_t))
#endif

#define CC_SB_MAX_KEY_CERT_BODY_SIZE_IN_BYTES   (sizeof(KeyCertMain_t))
#define CC_SB_MAX_CONTENT_CERT_BODY_SIZE_IN_BYTES   (sizeof(ContentCertMain_t)) /* may contain up to 16 signed sw images */
#define CC_SB_MAX_ENABLER_CERT_BODY_SIZE_IN_BYTES   (sizeof(EnablerCertMain_t))
#define CC_SB_MAX_DEVELOPER_CERT_BODY_SIZE_IN_BYTES (sizeof(DeveloperCertMain_t))

#define CC_SB_MAX_CONTENT_PKG_SIZE_IN_BYTES     (CC_SB_MAX_CONTENT_CERT_SIZE_IN_BYTES + SW_REC_NONE_SIGNED_DATA_SIZE_IN_BYTES*CC_SB_MAX_NUM_OF_IMAGES)

#define CC_SB_MAX_CERT_PKG_SIZE_IN_BYTES    (CC_SB_MAX_KEY_CERT_SIZE_IN_BYTES+CC_SB_MAX_ENABLER_CERT_SIZE_IN_BYTES+CC_SB_MAX_DEVELOPER_CERT_SIZE_IN_BYTES)


/* check KDR error bit in LCS register */
#define DCU_RESET_OVERRIDE_BIT_SHIFT    0x0
#define DCU_RESET_OVERRIDE_BIT_SIZE 0x1
#define IS_DCU_RESET_OVERRIDE(dcuVal)   ((dcuVal>>DCU_RESET_OVERRIDE_BIT_SHIFT) & DCU_RESET_OVERRIDE_BIT_SIZE)

#define CLEAR_ALL_DCU(dcuVal) {\
    dcuVal[0] = DCU_DISABLE_ALL_DBG; \
    dcuVal[1] = DCU_DISABLE_ALL_DBG; \
    dcuVal[2] = DCU_DISABLE_ALL_DBG; \
    dcuVal[3] = DCU_DISABLE_ALL_DBG; \
}

#define LOCK_ALL_DCU(dcuLock) {\
    dcuLock[0] = DCU_ENABLE_ALL_DBG; \
    dcuLock[1] = DCU_ENABLE_ALL_DBG; \
    dcuLock[2] = DCU_ENABLE_ALL_DBG; \
    dcuLock[3] = DCU_ENABLE_ALL_DBG; \
}


#define WRITE_DCU_LOCK(hwBaseAddress, dcuLock, rc) {\
    uint32_t ii = 0;\
    volatile uint32_t rr = 0;\
    for (ii = 0; ii < CC_BSV_SEC_DEBUG_DCU_SIZE_IN_WORDS; ii++) {\
        SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, HOST_DCU_LOCK0) + ii * sizeof(uint32_t), dcuLock[ii]); \
        SB_HAL_READ_REGISTER(SB_REG_ADDR(hwBaseAddress,HOST_DCU_LOCK0) + ii * sizeof(uint32_t), rr);       \
        if(rr!=dcuLock[ii]) { \
            rc = CC_BSV_AO_WRITE_FAILED_ERR; \
        } \
    }\
}

#define WRITE_DCU_VAL(hwBaseAddress, dcuVal) {\
    uint32_t ii = 0;\
    for (ii = 0; ii < CC_BSV_SEC_DEBUG_DCU_SIZE_IN_WORDS; ii++) {\
        SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, HOST_DCU_EN0) + ii * sizeof(uint32_t), dcuVal[ii]); \
        CC_BSV_WAIT_ON_NVM_IDLE_BIT(hwBaseAddress); \
    }\
}

/* Read-Modify-Write a field of a register */
#define READ_MODIFY_WRITE_AO_REGISTER(hwBaseAddress, regName, fldName, fldVal, tc) \
do {                                            \
    volatile uint32_t regVal = 0; \
    volatile uint32_t rr = 0; \
    SB_HAL_READ_REGISTER(SB_REG_ADDR(hwBaseAddress,regName), regVal);       \
    CC_REG_FLD_SET(DX, regName, fldName, regVal, fldVal); \
    SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, regName), regVal);       \
    SB_HAL_READ_REGISTER(SB_REG_ADDR(hwBaseAddress,regName), rr);       \
    if(rr!=regVal) { \
        rc = CC_BSV_AO_WRITE_FAILED_ERR; \
    } \
} while (0)


#define READ_DCU_LOCK_DEFAULT(hwBaseAddress, dcuLock, rc) \
do {\
    uint32_t ii; \
    for (ii = 0; ii<CC_OTP_DCU_SIZE_IN_WORDS; ii++) { \
        rc = CC_BsvOTPWordRead(hwBaseAddress, (CC_OTP_DCU_OFFSET+ii), &dcuLock[ii]); \
        if (rc != CC_OK) { \
            break;\
        } \
    }  \
} while(0)
#endif


