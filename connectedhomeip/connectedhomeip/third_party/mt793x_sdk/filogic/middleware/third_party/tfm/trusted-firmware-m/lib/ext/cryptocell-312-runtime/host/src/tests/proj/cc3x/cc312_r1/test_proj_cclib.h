/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _TEST_PROJ_CCLIB_H_
#define _TEST_PROJ_CCLIB_H_

#include "cc_rnd_common.h"


/****************************************************************************/
/*
 * @brief This function frees previously allocated resources
 *
 * @param[in/out] *pProcessMap - mapping regions
  *
 * @return rc - 0 for success, 1 for failure
 */
void Test_ProjFree(void);

/****************************************************************************/
/*
 * @brief This function
 *
 * @param[in/out]
  *
 * @return rc -
 */
int Test_Proj_CC_LibInit_Wrap(void* p_rng,
                              void* p_entropy,
                              CCRndWorkBuff_t * rndWorkBuff_ptr);

#endif //_TEST_PROJ_CCLIB_H_

