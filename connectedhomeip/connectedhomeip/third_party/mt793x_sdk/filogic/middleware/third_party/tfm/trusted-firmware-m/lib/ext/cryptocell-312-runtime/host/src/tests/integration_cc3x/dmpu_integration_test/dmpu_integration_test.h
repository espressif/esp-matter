/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _DMPU_INTEGRATION_TEST_H_
#define _DMPU_INTEGRATION_TEST_H_

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
typedef enum DmpuItError_t
{
    DMPUIT_ERROR__OK = 0,
    DMPUIT_ERROR__FAIL = 0x0000FFFF,
}DmpuItError_t;

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
DmpuItError_t dmpuIt_all(void);
#endif //_DMPU_INTEGRATION_TEST_H_
