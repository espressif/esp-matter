/*******************************************************************************
* The confidential and proprietary information contained in this file may      *
* only be used by a person authorised under and to the extent permitted        *
* by a subsisting licensing agreement from ARM Limited or its affiliates.      *
*   (C) COPYRIGHT [2001-2017] ARM Limited or its affiliates.                   *
*       ALL RIGHTS RESERVED                                                    *
* This entire notice must be reproduced on all copies of this file             *
* and copies of this file may only be made by a person if such person is       *
* permitted to do so under the terms of a subsisting license agreement         *
* from ARM Limited or its affiliates.                                          *
*******************************************************************************/

#ifndef TEST_PAL_TIME_H_
#define TEST_PAL_TIME_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/*
 * @brief This function suspends execution of the calling thread
 * for microsecond intervals.
 *
 * @param[in] time to suspend in microseconds.
 *
 * @param[out]
 *
 * @return
 */
void Test_PalDelay(const uint32_t usec);

/******************************************************************************/
/*
 * @brief This function returns a timestamp in milliseconds.
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return - timestamp in milliseconds.
 */
uint32_t Test_PalGetTimestamp(void);

#ifdef __cplusplus
}
#endif

#endif /* TEST_PAL_TIME_H_ */
