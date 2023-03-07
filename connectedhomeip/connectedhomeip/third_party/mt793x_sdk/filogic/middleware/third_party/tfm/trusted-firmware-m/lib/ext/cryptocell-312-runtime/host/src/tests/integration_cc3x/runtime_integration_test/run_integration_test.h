/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _RUN_INTEGRATION_TEST_H_
#define _RUN_INTEGRATION_TEST_H_

/************************************************************
 *
 * type decelerations
 *
 ************************************************************/
/**
 * Error Codes
 */
typedef enum RunItError_t
{
    RUNIT_ERROR__OK = 0,
    RUNIT_ERROR__FAIL = 0x0000FFFF,
}RunItError_t;

/************************************************************
 *
 * externs
 *
 ************************************************************/

/************************************************************
 *
 * function prototypes
 *
 ************************************************************/

/**
 * @brief               Call to perform all tests
 *                      Performs:
 *
 * @return              RUNIT_ERROR__OK on success, RUNIT_ERROR__FAIL otherwise
 */
RunItError_t runIt_all(void);

#endif //_RUN_INTEGRATION_TEST_H_
