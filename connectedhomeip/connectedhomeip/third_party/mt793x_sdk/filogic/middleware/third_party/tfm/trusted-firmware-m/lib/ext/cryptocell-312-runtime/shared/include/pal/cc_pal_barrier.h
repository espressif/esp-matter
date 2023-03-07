/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 @addtogroup cc_pal_barrier
 @{
 */

/*!
 @file
 @brief This file contains the definitions and APIs for memory-barrier
 implementation.

 This is a placeholder for platform-specific memory barrier implementation.
 The secure core driver should include a memory barrier, before and after
 the last word of the descriptor, to allow correct order between the words
 and different descriptors.
 */


#ifndef _CC_PAL_BARRIER_H
#define _CC_PAL_BARRIER_H


/*!
  This macro puts the memory barrier after the write operation.

  @return None
 */

void CC_PalWmb(void);

/*!
  This macro puts the memory barrier before the read operation.

  @return None
 */
void CC_PalRmb(void);

/*!
 @}
*/
#endif

