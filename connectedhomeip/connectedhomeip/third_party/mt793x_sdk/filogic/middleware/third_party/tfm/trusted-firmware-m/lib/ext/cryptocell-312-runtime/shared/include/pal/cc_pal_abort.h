/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

 /*!
 @addtogroup cc_pal_abort
 @{
 */

/*!
 @file
 @brief This file includes all PAL APIs.
 */

#ifndef _CC_PAL_ABORT_H
#define _CC_PAL_ABORT_H


#include "cc_pal_abort_plat.h"


/*!
  @brief This function performs the "Abort" operation.

  Must be implemented according to platform and OS.
*/
void CC_PalAbort(
    /*! [in] An optional parameter for a string of chars to indicate the abort
    operation. */
    const char * exp
);

/*!
 @}
 */
#endif

