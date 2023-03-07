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
#include <unistd.h>
#include <sys/time.h>
#include "test_pal_time.h"

/******************************************************************************/
void Test_PalDelay(const uint32_t usec)
{
    usleep(usec);
}

/******************************************************************************/
uint32_t Test_PalGetTimestamp(void)
{
    struct timeval te;
    uint32_t ms;

    /* Gets current time */
    gettimeofday(&te, NULL);

    /* Calculates timestamp in milliseconds */
    ms = te.tv_sec*1000LL + te.tv_usec/1000;

    return ms;
}
