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
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "test_pal_map_addrs.h"

/******************************************************************************/
void *Test_PalIOMap(void *physAddr, size_t size)
{
    (void)size;
    return physAddr;
}

/******************************************************************************/
void *Test_PalMapAddr(void *physAddr, void *startingAddr, const char *filename,
                size_t size, uint8_t protAndFlagsBitMask)
{
    (void)startingAddr;
    (void)filename;
    (void)size;
    (void)protAndFlagsBitMask;
    return physAddr;
}

/******************************************************************************/
void Test_PalUnmapAddr(void *virtAddr, size_t size)
{
    (void)virtAddr;
    (void)size;
}
