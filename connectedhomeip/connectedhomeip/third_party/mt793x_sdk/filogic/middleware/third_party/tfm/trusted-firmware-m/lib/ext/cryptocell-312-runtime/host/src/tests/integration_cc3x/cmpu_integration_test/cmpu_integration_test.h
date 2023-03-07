/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _CMPU_INTEGRATION_TEST_H_
#define _CMPU_INTEGRATION_TEST_H_

/* system includes */
#include "stdio.h"
#include "stdint.h"

/************************************************************
 *
 * type decelerations
 *
 ************************************************************/
/**
 * Error Codes
 */
typedef enum CmpuItError_t
{
    CMPUIT_ERROR__OK = 0,
    CMPUIT_ERROR__FAIL = 0x0000FFFF,
}CmpuItError_t;

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
CmpuItError_t cmpuIt_all(void);
#endif //_CMPU_INTEGRATION_TEST_H_
