/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

 /*!
 @addtogroup cc_sb_image_verifier
 @{
 */

/*!
 @file
 @brief This file contains definitions used for the Secure Boot and Secure
 Debug APIs.
 */


#ifndef _BOOT_IMAGES_VERIFIER_DEF_H
#define _BOOT_IMAGES_VERIFIER_DEF_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "cc_pal_types.h"

/*! The maximal number of SW images per content certificate. */
#define CC_SB_MAX_NUM_OF_IMAGES 16

#ifdef CC_SB_X509_CERT_SUPPORTED
/*! The maximal size of an X.509 certificate in bytes.*/
#define CC_SB_MAX_CERT_SIZE_IN_BYTES    (0xB10)
#else
/*! The maximal size of an Arm proprietary certificate in bytes. */
#define CC_SB_MAX_CERT_SIZE_IN_BYTES    (0x700)
#endif
/*! The maximal size of a certificate in words.*/
#define CC_SB_MAX_CERT_SIZE_IN_WORDS    (CC_SB_MAX_CERT_SIZE_IN_BYTES/CC_32BIT_WORD_SIZE)

/*! The size of the Secure Debug workspace in bytes. This workspace is used
to store RSA parameters. For example, modulus and signature. */
#define CC_SB_MIN_DBG_WORKSPACE_SIZE_IN_BYTES   (0x350)

/*!
 @brief The minimal size of the Secure Boot workspace in bytes.

 The Secure Boot APIs use a temporary workspace for processing the data that
 is read from the flash, before loading the SW modules to their designated
 memory addresses. This workspace must be large enough to accommodate the size
 of the certificates, and twice the size of the data that is read from flash
 in each processing round.

 The definition of \c CC_SB_MIN_WORKSPACE_SIZE_IN_BYTES is comprised of
 \c CC_DOUBLE_BUFFER_MAX_SIZE_IN_BYTES and additional space for the
 certificate itself, which resides in the workspace at the same time the SW
 images data is processed.\n
 It is assumed that the optimal size of the data to read in each processing
 round is 4KB, based on the standard flash-memory page size. Therefore, the
 size of the double buffer, \c CC_CONFIG_SB_DOUBLE_BUFFER_MAX_SIZE_IN_BYTES,
 is defined by default as 8KB in the project configuration file. This can be
 changed to accommodate the optimal value in different environments.
 \c CC_DOUBLE_BUFFER_MAX_SIZE_IN_BYTES is defined by the Boot Services
 makefile as equal to \c CC_CONFIG_SB_DOUBLE_BUFFER_MAX_SIZE_IN_BYTES.

 @note When writing code that uses the Secure Boot APIs, and includes the
 bootimagesverifier_def.h file, the value of
 \c CC_DOUBLE_BUFFER_MAX_SIZE_IN_BYTES must be defined by your makefile to be
 exactly the same value as was used when compiling the SBROM library.
 Additionally, \c CC_SB_X509_CERT_SUPPORTED must be defined in the Makefile,
 according to the definition of \c CC_CONFIG_SB_X509_CERT_SUPPORTED. \par

 @note The size of \c CC_DOUBLE_BUFFER_MAX_SIZE_IN_BYTES must be a multiple of
 the hash SHA-256 block size of 64 bytes. \par
*/
#define CC_SB_MIN_WORKSPACE_SIZE_IN_BYTES   (CC_SB_MAX_CERT_SIZE_IN_BYTES + CC_MAX(CC_SB_MIN_DBG_WORKSPACE_SIZE_IN_BYTES, CC_DOUBLE_BUFFER_MAX_SIZE_IN_BYTES))


#ifdef __cplusplus
}
#endif

 /*!
 @}
 */
#endif


