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

#include <stdio.h>
#include "test_pal_semphr.h"
#include "FreeRTOS.h"
#include "semphr.h"

/******************************************************************************/
uint8_t Test_PalMutexCreate(Test_PalMutex *ppMutexId)
{
    *ppMutexId = xSemaphoreCreateMutex(); /* function returns
    SemaphoreHandle_t */

    if (*ppMutexId == NULL)
        return 1;

    return 0;
}

/******************************************************************************/
uint8_t Test_PalMutexDestroy(Test_PalMutex *ppMutexId)
{
    vSemaphoreDelete(*ppMutexId);
    return 0;
}

/******************************************************************************/
uint8_t Test_PalMutexLock(Test_PalMutex *ppMutexId, uint32_t timeout)
{
    if (!INFINITE)
        timeout = timeout / portTICK_PERIOD_MS;
    else
        timeout = portMAX_DELAY;

    return xSemaphoreTake(*ppMutexId, timeout) ? 0:1;
}

/******************************************************************************/
uint8_t Test_PalMutexUnlock(Test_PalMutex *ppMutexId)
{
    return xSemaphoreGive(*ppMutexId) ? 0:1;
}

/******************************************************************************/
uint8_t Test_PalBinarySemaphoreCreate(Test_PalBinarySemaphore *ppBinSemphrId)
{
    *ppBinSemphrId = xSemaphoreCreateBinary(); /* function returns
    SemaphoreHandle_t */

    if (*ppBinSemphrId == NULL)
        return 1;

    return 0;
}

/******************************************************************************/
uint8_t Test_PalBinarySemaphoreDestroy(Test_PalBinarySemaphore *ppBinSemphrId)
{
    vSemaphoreDelete(*ppBinSemphrId);
    return 0;
}

/******************************************************************************/
uint8_t Test_PalBinarySemaphoreTake(Test_PalBinarySemaphore *ppBinSemphrId,
                            uint32_t timeout)
{
    if (!INFINITE)
        timeout = timeout / portTICK_PERIOD_MS;
    else
        timeout = portMAX_DELAY;

    return xSemaphoreTake(*ppBinSemphrId, timeout) ? 0:1;
}

/******************************************************************************/
uint8_t Test_PalBinarySemaphoreGive(Test_PalBinarySemaphore *ppBinSemphrId)
{
    return xSemaphoreGive(*ppBinSemphrId) ? 0:1;
}
