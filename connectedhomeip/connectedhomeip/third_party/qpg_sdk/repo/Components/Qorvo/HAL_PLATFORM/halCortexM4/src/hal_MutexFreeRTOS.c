/*
 * Copyright (c) 2021-2022, Qorvo Inc
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 * $Header$
 * $Change$
 * $DateTime$
 *
 */

/** @file Implementation of FreeRTOS specific Mutex wrappers */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/
#define GP_COMPONENT_ID GP_COMPONENT_ID_HALCORTEXM4

#include "gp_kx.h"
#include "gpAssert.h"

#include "FreeRTOS.h"
#include "semphr.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

#ifndef HAL_MUTEX_SUPPORTED
#error FreeRTOS has mutex capability - HAL_MUTEX_SUPPORTED should be set from build env
#endif //HAL_MUTEX_SUPPORTED

#ifndef HAL_MAX_NUMBER_USED_STATIC_MUTEX
#define HAL_MAX_NUMBER_USED_STATIC_MUTEX 12
#endif

#define HAL_IS_HARDFAULT_CONTEXT()    (SCB->HFSR & SCB_HFSR_FORCED_Msk)

/*****************************************************************************
 *                    Local Variable
 *****************************************************************************/

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/
#if configSUPPORT_DYNAMIC_ALLOCATION == 0
static StaticSemaphore_t xMutexBufferTab[ HAL_MAX_NUMBER_USED_STATIC_MUTEX ];
#endif

void hal_MutexCreate(SemaphoreHandle_t* pMutex)
{
    GP_ASSERT_SYSTEM(pMutex !=NULL);
#if configSUPPORT_DYNAMIC_ALLOCATION
    *pMutex = xSemaphoreCreateMutex();
#else    // Create static buffer for mutex memory outside of function
    static uint8_t staticMutexCouter = 0;

    // check static semaphore table size - The size of the static semaphore buffers array is insufficient
    GP_ASSERT_SYSTEM(staticMutexCouter < HAL_MAX_NUMBER_USED_STATIC_MUTEX);

    *pMutex = xSemaphoreCreateMutexStatic(&xMutexBufferTab[staticMutexCouter++]);
#endif //configSUPPORT_DYNAMIC_ALLOCATION
}

void hal_MutexDestroy(SemaphoreHandle_t* pMutex)
{
    GP_ASSERT_SYSTEM(pMutex !=NULL);
#if configSUPPORT_DYNAMIC_ALLOCATION
    vSemaphoreDelete(*pMutex);
    *pMutex = 0;
#else
    (void)pMutex;
#endif //configSUPPORT_DYNAMIC_ALLOCATION
}

Bool hal_MutexIsValid(SemaphoreHandle_t pMutex)
{
    return pMutex != 0;
}

Bool hal_MutexIsAcquired(SemaphoreHandle_t pMutex)
{
    xPSR_Type psr;
    psr.w = __get_xPSR();

    // Use ISR safe function
    if(psr.b.ISR != 0)
    {
        return (uxQueueMessagesWaitingFromISR((QueueHandle_t)(pMutex)) == 0);
    }
    else
    {
        if ( xTaskGetSchedulerState() == taskSCHEDULER_RUNNING )
        {
            return (uxSemaphoreGetCount((pMutex)) == 0);
        }
        else
        {
            return true;
        }
    }
}

void hal_MutexAcquire(SemaphoreHandle_t pMutex)
{
    xPSR_Type psr;
    psr.w = __get_xPSR();

    if(psr.b.ISR != 0)
    {
        // Mutex type semaphores cannot be used from within interrupt service routines.
        // Moving over the claim from an ISR only possible if not claimed.
        // Allow a hardfault to bypass the mutexes as it will not return from assert
        if (!HAL_IS_HARDFAULT_CONTEXT())
        {
            GP_ASSERT_SYSTEM(!hal_MutexIsAcquired(pMutex));
        }
    }
    else
    {
        if ( xTaskGetSchedulerState() == taskSCHEDULER_RUNNING )
        {
            if ( xSemaphoreTake((pMutex), portMAX_DELAY) != pdTRUE )
            {
                GP_ASSERT_SYSTEM(false);
            }
        }
    }
}

void hal_MutexRelease(SemaphoreHandle_t pMutex)
{
    xPSR_Type psr;
    psr.w = __get_xPSR();

    if(psr.b.ISR != 0)
    {
        // Mutex type semaphores cannot be used from within interrupt service routines.
        // Moving over the release only possible if not claimed from a non-isr function.
        // Allow a hardfault to bypass the mutexes as it will not return from assert
        if (!HAL_IS_HARDFAULT_CONTEXT())
        {
            GP_ASSERT_SYSTEM(!hal_MutexIsAcquired(pMutex));
        }
    }
    else
    {
        if ( xTaskGetSchedulerState() == taskSCHEDULER_RUNNING )
        {
            if ( xSemaphoreGive((pMutex)) != pdTRUE )
            {
                GP_ASSERT_SYSTEM(false);
            }
        }
    }
}
