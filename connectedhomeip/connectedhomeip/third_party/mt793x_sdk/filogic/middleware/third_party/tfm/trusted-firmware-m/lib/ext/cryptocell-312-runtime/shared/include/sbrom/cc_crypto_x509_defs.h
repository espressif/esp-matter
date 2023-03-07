/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _CC_CRYPTO_X509_DEFS_H
#define _CC_CRYPTO_X509_DEFS_H

#include "stdint.h"

#define CERTIFICATE_VALIDITY_ENDLESS  0
#define CC_X509_CERT_PKG_TOKEN  0x43504B47
#define CC_X509_CERT_PKG_VERSION   0x01000000
#define CC_X509_MAX_CERT_SIZE   0xFFFF

/* CC object Id's */
/* all object ID's under CC category */
#define CC_X509_OBJ_ID_DX       0x2
/* enable user category */
#define CC_X509_OBJ_ID_ANY      0x14
/* MAX NONCE size */
#define CC_X509_MAX_NONCE_SIZE_BYTES    8


#define CC_X509_CERT_ISSUER_NAME    "ARM"
#define CC_X509_CERT_KEY_CERT       "KeyCert"
#define CC_X509_CERT_CNT_CERT       "CntCert"
#define CC_X509_CERT_ENABLER_CERT       "EnablerDbg"
#define CC_X509_CERT_DEVELOPER_CERT     "DeveloperDbg"

/* certificate type category */
typedef enum {
    CC_X509_CERT_TYPE_MIN = 0x0,
    CC_X509_CERT_TYPE_KEY = 0x1,    /* 0x1 */
    CC_X509_CERT_TYPE_CONTENT,      /* 0x2 */
    CC_X509_CERT_TYPE_ENABLER_DBG,     /* 0x3 */
    CC_X509_CERT_TYPE_DEVELOPER_DBG,     /* 0x4 */
    CC_X509_CERT_TYPE_MAX,
    CC_X509_CERT_TYPE_RESERVED = 0xFF
}CCX509CertType_t;


/* certificate type category */
typedef enum {
    CC_X509_PKG_TYPE_MIN = 0x0,
    CC_X509_PKG_TYPE_KEY = 0x1, /* 0x1 */
    CC_X509_PKG_TYPE_CONTENT,       /* 0x2 */
    CC_X509_PKG_TYPE_ENABLER_DBG,   /* 0x3 */
    CC_X509_PKG_TYPE_DEVELOPER_DBG, /* 0x4 */
    CC_X509_PKG_TYPE_MAX,
    CC_X509_PKG_TYPE_RESERVED = 0xFF
}CCX509PkgType_t;

#ifdef CC_SB_SUPPORT_IOT
/* specific certificate extension category */
typedef enum {
    CC_X509_ID_EXT_NONE = 0,
    CC_X509_ID_EXT_PROPRIETARY_HEADER,
    CC_X509_ID_EXT_PUB_KEY_NP,
    CC_X509_ID_EXT_KEY_CERT_MAIN_VAL,
    CC_X509_ID_EXT_CONTENT_CERT_MAIN_VAL,
    CC_X509_ID_EXT_ENABLER_CERT_MAIN_VAL,
    CC_X509_ID_EXT_DEVELOPER_CERT_MAIN_VAL,
    CC_X509_ID_EXT_MAX,
    CC_X509_ID_EXT_RESERVED = 0xFF
}CCX509ExtType_t;

#define CC3X_X509_CERT_EXT_NUMBER 3
#else
/* specific certificate extension category */
typedef enum {
    CC_X509_ID_EXT_NONE = 0,                 /*0x0 */
    CC_X509_ID_EXT_NV_COUNTER_ID = 0x1,      /*0x1 */
    CC_X509_ID_EXT_NV_COUNTER_VAL,               /*0x2 */
    CC_X509_ID_EXT_PUB_KEY_NP,           /*0x3 */
    CC_X509_ID_EXT_PUB_KEY_HASH,             /*0x4 */
    CC_X509_ID_EXT_NUM_OF_SW_IMAGES,         /*0x5 */
    CC_X509_ID_EXT_SW_IMAGE_NONCE,               /*0x6 */
    CC_X509_ID_EXT_SW_IMAGE_INFO,                /*0x7 */
    CC_X509_ID_EXT_SOC_SPECIFIC,             /*0x8 */
    CC_X509_ID_EXT_SOC_ID,                   /*0x9 */
    CC_X509_ID_EXT_VALID_LCS,            /*0xA */
    CC_X509_ID_EXT_RMA_MODE,             /*0xB */
#ifdef CC_SB_CERT_USER_DATA_EXT
    CC_X509_ID_EXT_USER_DATA,            /*0xC */
#endif
    CC_X509_ID_EXT_MAX,
    CC_X509_ID_EXT_RESERVED = 0xFF
}CCX509ExtType_t;
#endif

typedef union {
        struct {
               uint32_t      certOffset:16;
               uint32_t      certSize:16;
        }certInfoBits;
        uint32_t      certInfoWord;
}CCX509CertInfo_t;

typedef union {
        struct {
               uint32_t      certType:8;
               uint32_t      imageEnc:8;
               uint32_t      hbkType:8;
               uint32_t      reserved:8;
        }pkgFlagsBits;
        uint32_t      pkgFlagsWord;
}CCX509PkgFlag_t;

typedef struct {
    uint32_t pkgToken;
    uint32_t pkgVer;
    CCX509PkgFlag_t pkgFlags;
    CCX509CertInfo_t certInfo;
}CCX509PkgHeader_t;

#endif
