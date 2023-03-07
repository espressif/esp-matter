/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 @addtogroup cc_pal_init
 @{
 */

/*!
 @file
 @brief This file contains the PAL layer entry point.

 It includes the definitions and APIs for PAL initialization and termination.
 */

#ifndef _CC_PAL_INIT_H
#define _CC_PAL_INIT_H

#include "cc_pal_types.h"

#ifdef __cplusplus
extern "C"
{
#endif


/*!
  @brief This function performs all initializations that may be required by
  your PAL implementation, specifically by the DMA-able buffer scheme.

  It is called by ::CC_LibInit.

  The existing implementation allocates a contiguous memory pool that is later
  used by the CryptoCell implementation.
  If no initializations are needed in your environment, the function can be
  minimized to return OK.

  @return A non-zero value on failure.
 */
int CC_PalInit(void);



/*!
  @brief This function terminates the PAL implementation and frees the resources
  that were allocated by ::CC_PalInit.

  @return Void.
 */
void CC_PalTerminate(void);



#ifdef __cplusplus
}
#endif

/*!
 @}
 */
#endif
