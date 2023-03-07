/*
 * Copyright (c) 2021, Qorvo Inc
 *
 * hal_Mutex.h
 *   Hardware Abstraction Layer Mutex for ARM devices.
 *
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

#ifndef _HAL_MUTEXFREERTOS_H_
#define _HAL_MUTEXFREERTOS_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"
#include "FreeRTOS.h"
#include "semphr.h"

/*****************************************************************************
 *                    Definitions
 *****************************************************************************/

#define HAL_CRITICAL_SECTION_DEF(pMutex) SemaphoreHandle_t pMutex;

/*****************************************************************************
 *                    Function Prototype Definitions
 *****************************************************************************/

void hal_MutexCreate(SemaphoreHandle_t* pMutex);
void hal_MutexDestroy(SemaphoreHandle_t* pMutex);
Bool hal_MutexIsValid(SemaphoreHandle_t pMutex);
Bool hal_MutexIsAcquired(SemaphoreHandle_t pMutex);
void hal_MutexAcquire(SemaphoreHandle_t pMutex);
void hal_MutexRelease(SemaphoreHandle_t pMutex);

#endif //_HAL_MUTEXFREERTOS_H_
