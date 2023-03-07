/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 @addtogroup cc_pal_mutex
 @{
 */

/*!
 @file
 @brief This file contains functions for resource management (mutex operations).

 These functions are generally implemented as wrappers to different
 operating-system calls.

 \note None of the described functions validate the input parameters, so that
 the behavior of the APIs in case of an illegal parameter is dependent on the
 behavior of the operating system.
 */

#ifndef _CC_PAL_MUTEX_H
#define _CC_PAL_MUTEX_H

#include "cc_pal_mutex_plat.h"
#include "cc_pal_types_plat.h"

#ifdef __cplusplus
extern "C"
{
#endif



/*----------------------------
      PUBLIC FUNCTIONS
-----------------------------------*/

/*!
  @brief This function creates a mutex.


  @return \c 0 on success.
  @return A non-zero value on failure.
 */
CCError_t CC_PalMutexCreate(
        /*! [out] A pointer to the handle of the created mutex. */
        CC_PalMutex *pMutexId
        );


/*!
  @brief This function destroys a mutex.


  @return \c 0 on success.
  @return A non-zero value on failure.
 */
CCError_t CC_PalMutexDestroy(
        /*! [in] A pointer to handle of the mutex to destroy. */
        CC_PalMutex *pMutexId
        );


/*!
  @brief This function waits for a mutex with \p aTimeOut.

  \p aTimeOut is specified in milliseconds. A value of \p aTimeOut=CC_INFINITE
  means that the function will not return.

  @return \c 0 on success.
  @return A non-zero value on failure.
 */
CCError_t CC_PalMutexLock(
        /*! [in] A pointer to handle of the mutex. */
        CC_PalMutex *pMutexId,
        /*! [in] The timeout in mSec, or CC_INFINITE. */
        uint32_t aTimeOut
        );


/*!
  @brief This function releases the mutex.

  @return \c 0 on success.
  @return A non-zero value on failure.
 */
CCError_t CC_PalMutexUnlock(
        /*! [in] A pointer to the handle of the mutex. */
        CC_PalMutex *pMutexId
        );


#ifdef __cplusplus
}
#endif

/*!
 @}
 */
#endif

