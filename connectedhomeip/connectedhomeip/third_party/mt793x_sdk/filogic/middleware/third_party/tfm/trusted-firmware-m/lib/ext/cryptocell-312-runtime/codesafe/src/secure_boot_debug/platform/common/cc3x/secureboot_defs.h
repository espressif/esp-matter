/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 @addtogroup cc_sb_defs
 @{
 */

/*!
 @file
 @brief This file contains type definitions for the Secure Boot.
 */


#ifndef _SECURE_BOOT_DEFS_H
#define _SECURE_BOOT_DEFS_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "cc_crypto_boot_defs.h"
#include "cc_sec_defs.h"


/* General definitions */
/***********************/

/*! Input or output structure for the Secure Boot verification API. */
typedef struct{
    /*! The NV counter saved in OTP. */
    uint32_t otpVersion;
    /*! The key hash to retrieve: The 128-bit Hbk0, the 128-bit Hbk1, or the
    256-bit Hbk. */
    CCSbPubKeyIndexType_t keyIndex;
    /*! The value of the SW version for the certificate-chain. */
    uint32_t activeMinSwVersionVal;
    /*! In: The hash of the public key (N||Np), to compare to the public key
    stored in the certificate. Out: The hash of the public key (N||Np) stored
    in the certificate, to be used for verification of the public key of the
    next certificate in the chain. */
    CCHashResult_t pubKeyHash;
    /*! Internal flag for the initialization indication. */
    uint32_t initDataFlag;
}CCSbCertInfo_t;



/*! The size of the data of the SW-image certificate. */
#define SW_REC_SIGNED_DATA_SIZE_IN_BYTES            44  // HS(8W) + load_adddr(1) + maxSize(1) + isCodeEncUsed(1)
/*! The size of the additional-data of the SW-image certificate in bytes. */
#define SW_REC_NONE_SIGNED_DATA_SIZE_IN_BYTES       8   // storage_addr(1) + ActualSize(1)
/*! The size of the additional-data of the SW-image certificate in words. */
#define SW_REC_NONE_SIGNED_DATA_SIZE_IN_WORDS       SW_REC_NONE_SIGNED_DATA_SIZE_IN_BYTES/CC_32BIT_WORD_SIZE
/*! Indication whether or not to load the SW image to memory. */
#define CC_SW_COMP_NO_MEM_LOAD_INDICATION       0xFFFFFFFFUL


#ifdef __cplusplus
}
#endif

/*!
 @}
 */
#endif
