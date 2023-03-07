/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _CC_PAL_MEM_PLAT_H
#define _CC_PAL_MEM_PLAT_H


#ifdef __cplusplus
extern "C"
{
#endif

#include <stdlib.h>
#include <string.h>


/**
 * @brief File Description:
 *        This file contains the implementation for memory operations APIs.
 *        The functions implementations are generally just wrappers to different operating system calls.
 */


/*----------------------------
  PUBLIC FUNCTIONS
-------------------------------*/


/*!
 * @brief This function purpose is to compare between two given buffers according to given size.
 *
 * @return The return values is according to operating system return values.
 */
int32_t CC_PalMemCmpPlat(  const void* aTarget, /*!< [in] The target buffer to compare. */
                           const void* aSource, /*!< [in] The Source buffer to compare to. */
                           size_t      aSize    /*!< [in] Number of bytes to compare. */);

/*!
 * @brief This function purpose is to copy aSize bytes from source buffer to destination buffer.
 *
 * @return void.
 */
void* CC_PalMemCopyPlat(     void* aDestination, /*!< [out] The destination buffer to copy bytes to. */
                               const void* aSource,      /*!< [in] The Source buffer to copy from. */
                               size_t      aSize     /*!< [in] Number of bytes to copy. */ );


/*!
 * @brief This function purpose is to copy aSize bytes from source buffer to destination buffer.
 * This function Supports overlapped buffers.
 *
 * @return void.
 */
void CC_PalMemMovePlat(   void* aDestination, /*!< [out] The destination buffer to copy bytes to. */
                          const void* aSource,      /*!< [in] The Source buffer to copy from. */
                          size_t      aSize     /*!< [in] Number of bytes to copy. */);



/*!
 * @brief This function purpose is to set aSize bytes in the given buffer with aChar.
 *
 * @return void.
 */
void CC_PalMemSetPlat(   void* aTarget, /*!< [out]  The target buffer to set. */
                         uint8_t aChar, /*!< [in] The char to set into aTarget. */
                         size_t        aSize  /*!< [in] Number of bytes to set. */);

/*!
 * @brief This function purpose is to set aSize bytes in the given buffer with zeroes.
 *
 * @return void.
 */
void CC_PalMemSetZeroPlat(    void* aTarget, /*!< [out]  The target buffer to set. */
                              size_t      aSize    /*!< [in] Number of bytes to set. */);


#ifdef __cplusplus
}
#endif

#endif


