/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 @addtogroup cc_lib

 @{
 */

/*!
 @file
 @brief This file contains all of the basic APIs of the CryptoCell library,
 their enums and definitions.
 */


#ifndef __CC_LIB_H__
#define __CC_LIB_H__

#include "cc_pal_types.h"
#include "cc_rnd_common.h"

#ifdef __cplusplus
extern "C"
{
#endif


/*! Definitions for error returns from ::CC_LibInit or ::CC_LibFini functions. */
typedef enum {
        /*! Success.*/
        CC_LIB_RET_OK = 0,
        /*! Illegal context pointer.*/
        CC_LIB_RET_EINVAL_CTX_PTR,
        /*! Illegal work-buffer pointer.*/
        CC_LIB_RET_EINVAL_WORK_BUF_PTR,
        /*! Error returned from the HAL layer.*/
        CC_LIB_RET_HAL,
        /*! Error returned from the PAL layer.*/
        CC_LIB_RET_PAL,
        /*! RND instantiation failed.*/
        CC_LIB_RET_RND_INST_ERR,
        /*! Invalid peripheral ID. */
        CC_LIB_RET_EINVAL_PIDR,
        /*! Invalid component ID. */
        CC_LIB_RET_EINVAL_CIDR,
        /*! Error returned from AO write operation. */
        CC_LIB_AO_WRITE_FAILED_ERR,
        /*! Reserved.*/
        CC_LIB_RESERVE32B = 0x7FFFFFFFL
} CClibRetCode_t;


/*! Internal definition for the product register. */
#define DX_VERSION_PRODUCT_BIT_SHIFT    0x18UL
/*! Internal definition for the product register size. */
#define DX_VERSION_PRODUCT_BIT_SIZE     0x8UL



/*!
 @brief This function performs global initialization of the CryptoCell runtime
 library.

 It must be called once per CryptoCell cold-boot cycle.
 Among other initializations, this function initializes the CTR-DRBG context,
 including the TRNG seeding. An initialized DRBG context is required for
 calling DRBG APIs, as well as asymmetric-cryptography key-generation and
 signatures.\n
 The primary context returned by this function can be used as a single global
 context for all DRBG needs. Alternatively, other contexts may be initialized
 and used with a more limited scope, for specific applications or specific
 threads.

 @note If used, the Mutexes are initialized by this API. Therefore, unlike
 other APIs in the library, this API is not thread-safe. \par
 @note The \p rndWorkBuff_ptr parameter can be NULL in case full entropy mode
 is used. \par

 @return \c CC_LIB_RET_OK on success.
 @return A non-zero value on failure.
 */
CClibRetCode_t CC_LibInit(
        /*! [in/out] A pointer to the RND context buffer allocated by the user.
        The context is used to maintain the RND state as well as
        pointers to a function used for random vector generation.
        This context must be saved and provided as a parameter to
        any API that uses the RND module.*/
        CCRndContext_t *rndContext_ptr,
        /*! [in] A scratchpad for the work of the RND module. */
        CCRndWorkBuff_t  *rndWorkBuff_ptr
        );

/*!
 @brief This function finalizes library operations.

 It performs the following operations:
 <ul><li>Frees the associated resources (mutexes).</li>
 <li>Calls the HAL and PAL terminate functions.</li>
 <li>Cleans the DRBG context.</li></ul>

 @return \c CC_LIB_RET_OK on success.
 @return A non-zero value on failure.
*/
CClibRetCode_t CC_LibFini(
        /*! [in/out] A pointer to the RND context buffer that was initialized
        in #CC_LibInit.*/
        CCRndContext_t *rndContext_ptr
        );

#ifdef __cplusplus
}
#endif

/*!
@}
 */

#endif /*__CC_LIB_H__*/
