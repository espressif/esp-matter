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

#include "cmsis_os2.h"
#include "mbed_wait_api.h"
#include "test_pal_time.h"

/******************************************************************************/
void Test_PalDelay(const uint32_t usec)
{
    wait_us((int)usec);
}

/******************************************************************************/
uint32_t Test_PalGetTimestamp(void)
{
    return (us_ticker_read() / 1000);
}
