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

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "test_pal_log.h"
#include "test_pal_thread.h"

/******************************************************************************/
size_t Test_PalGetMinimalStackSize(void)
{
    return 0;
}

/******************************************************************************/
uint32_t Test_PalGetHighestPriority(void)
{
    return 0;
}

/******************************************************************************/
uint32_t Test_PalGetLowestPriority(void)
{
    return 0;
}

/******************************************************************************/
uint32_t Test_PalGetDefaultPriority(void)
{
    return 0;
}

/******************************************************************************/
ThreadHandle Test_PalThreadCreate(size_t stackSize,
                  void *(*threadFunc)(void *),
                  int priority, void *args,
                  const char *threadName,
                  uint8_t nameLen, uint8_t dmaAble)
{
    return NULL;
}

/******************************************************************************/
uint32_t Test_PalThreadJoin(ThreadHandle threadHandle, void **threadRet)
{
    return 0;
}

/******************************************************************************/
uint32_t Test_PalThreadDestroy(ThreadHandle threadHandle)
{
    return 0;
}
