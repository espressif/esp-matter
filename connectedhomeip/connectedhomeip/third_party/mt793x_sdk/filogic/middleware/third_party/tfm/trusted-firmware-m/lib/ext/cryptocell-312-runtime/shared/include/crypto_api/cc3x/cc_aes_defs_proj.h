/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 @addtogroup cc_aes_defs_proj
 @{
 */

/*!
 @file
 @brief This file contains project definitions that are used for CryptoCell
 AES APIs.
 */

#ifndef CC_AES_DEFS_PROJ_H
#define CC_AES_DEFS_PROJ_H

#include "cc_pal_types.h"


#ifdef __cplusplus
extern "C"
{
#endif

/************************ Defines ******************************/

/*! The size of the context prototype of the user in words.
See ::CCAesUserContext_t.*/
#define CC_AES_USER_CTX_SIZE_IN_WORDS (4+8+8+4)

/*! The maximal size of the AES key in words. */
#define CC_AES_KEY_MAX_SIZE_IN_WORDS 8
/*! The maximal size of the AES key in bytes. */
#define CC_AES_KEY_MAX_SIZE_IN_BYTES (CC_AES_KEY_MAX_SIZE_IN_WORDS * sizeof(uint32_t))


#ifdef __cplusplus
}
#endif


/*!
 @}
 */

#endif /* #ifndef CC_AES_DEFS_PROJ_H */

