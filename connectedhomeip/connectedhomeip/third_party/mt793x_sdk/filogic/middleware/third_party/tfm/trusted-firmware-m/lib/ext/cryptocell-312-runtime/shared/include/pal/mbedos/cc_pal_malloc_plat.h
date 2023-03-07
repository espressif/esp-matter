/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef _CC_PAL_MEMALLOC_INT_H
#define _CC_PAL_MEMALLOC_INT_H


#ifdef __cplusplus
extern "C"
{
#endif

#include <stdlib.h>

/**
* @brief File Description:
*        This file contains wrappers for memory operations APIs.
*/


/*----------------------------
      PUBLIC FUNCTIONS
-----------------------------------*/

/*!
 * @brief This function purpose is to allocate a memory buffer according to aSize.
 *
 *
 * @return The function returns a pointer to allocated buffer or NULL if allocation failed.
 */
void* CC_PalMemMallocPlat(size_t  aSize /*!< [in] Number of bytes to allocate. */);

/*!
 * @brief This function purpose is to reallocate a memory buffer according to aNewSize.
 *        The content of the old buffer is moved to the new location.
 *
 * @return The function returns a pointer to the newly allocated buffer or NULL if allocation failed.
 */
void* CC_PalMemReallocPlat(  void* aBuffer,     /*!< [in] Pointer to allocated buffer. */
                             size_t  aNewSize   /*!< [in] Number of bytes to reallocate. */);

/*!
 * @brief This function purpose is to free allocated buffer.
 *
 *
 * @return void.
 */
void CC_PalMemFreePlat(void* aBuffer /*!< [in] Pointer to allocated buffer.*/);
#ifdef __cplusplus
}
#endif

#endif


