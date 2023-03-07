/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 @addtogroup cc_sb_gen_defs
 @{
 */

/*!
 @file
 @brief This file contains all of the definitions and structures used for the
 Secure Boot and Secure Debug.
 */

#ifndef _SECURE_BOOT_GEN_DEFS_H
#define _SECURE_BOOT_GEN_DEFS_H

#ifdef __cplusplus
extern "C"
{
#endif


#include "secureboot_basetypes.h"
#include "cc_sec_defs.h"
#include "cc_pal_sb_plat.h"

/* General definitions */
/***********************/

/*! The maximal size of the additional-data of the Secure Boot in bytes. */
#define CC_SB_MAX_SIZE_ADDITIONAL_DATA_BYTES    128

/*! Definition of public key hash array. */
typedef uint32_t CCSbCertPubKeyHash_t[HASH_RESULT_SIZE_IN_WORDS];
/*! Definition of <em>SoC_ID</em> array. */
typedef uint32_t CCSbCertSocId_t[HASH_RESULT_SIZE_IN_WORDS];


/********* Function pointer definitions ***********/

/*! @brief Typedef of the pointer to the Flash read function that you
must implement.

The Flash read function is called to read the certificates and SW modules from
flash memory.

  @note It is your responsibility to verify that this function does not copy
  data from restricted memory regions.
 */
typedef uint32_t (*CCSbFlashReadFunc) (
                     /*! [in] The address for reading from flash memory. */
                     CCAddr_t flashAddress,
                     /*! [out] A pointer to the RAM destination address to
                     write the data to. */
                     uint8_t *memDst,
                     /*! [in] The size to read in bytes. */
                     uint32_t sizeToRead,
                     /*! [in] For partner use. */
                     void* context
                     );


/*! @brief Typedef of the pointer to the Flash write function that you must
implement.

  The Flash write function is called to write authenticated and decrypted SW
  modules to flash memory. */
typedef uint32_t (*CCBsvFlashWriteFunc) (
                    /*! [in] The address for writing to flash memory. */
                    CCAddr_t flashAddress,
                    /*! [out] A pointer to the RAM source to read the
                    data from. */
                    uint8_t *memSrc,
                    /*! [in] The size to write in bytes. */
                    uint32_t sizeToWrite,
                    /*! [in] For partner use. */
                    void* context
                    );

/********* End of Function pointer definitions ***********/


#ifdef __cplusplus
}
#endif


/*!
 @}
 */
#endif
