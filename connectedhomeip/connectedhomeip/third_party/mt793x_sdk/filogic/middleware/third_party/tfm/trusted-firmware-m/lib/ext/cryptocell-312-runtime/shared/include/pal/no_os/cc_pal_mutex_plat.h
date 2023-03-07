/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _CC_PAL_MUTEX_PLAT_H
#define _CC_PAL_MUTEX_PLAT_H


#ifdef __cplusplus
extern "C"
{
#endif
/**
* @brief File Description:
*        This file contains functions for resource management (semaphor operations).
*        The functions implementations are generally just wrappers to different operating system calls.
*        None of the described functions will check the input parameters so the behavior
*        of the APIs in illegal parameters case is dependent on the operating system behavior.
*
*/

typedef uint32_t CC_PalMutex;



/*----------------------------
      PUBLIC FUNCTIONS
-----------------------------------*/


#ifdef __cplusplus
}
#endif

#endif


