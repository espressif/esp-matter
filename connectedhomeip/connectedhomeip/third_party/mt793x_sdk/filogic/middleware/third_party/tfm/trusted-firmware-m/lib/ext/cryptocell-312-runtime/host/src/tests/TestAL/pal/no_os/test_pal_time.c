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

#include <stdint.h>
#include <time.h>
#include <math.h>
#include "test_pal_time.h"

/******************************************************************************/
/* The smallest resolution in no_os is seconds */
void Test_PalDelay(const uint32_t usec)
{
    /* Rounds up */
    uint32_t secs = (usec + 999) / 1000;

    /* Gets the end time */
    uint32_t retTime = time(0) + secs;

    /* Loop until end time arrives */
    while (time(0) < retTime);
}
