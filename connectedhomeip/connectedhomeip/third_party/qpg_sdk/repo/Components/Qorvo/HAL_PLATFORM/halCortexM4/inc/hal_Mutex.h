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

#ifndef _HAL_MUTEX_H_
#define _HAL_MUTEX_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"
#include "gpAssert.h"

#ifdef GP_DIVERSITY_FREERTOS
#include "hal_MutexFreeRTOS.h"
#else
#include "hal_MutexBareMetal.h"
#endif // GP_DIVERSITY_FREERTOS

/*****************************************************************************
 *                    MUTEX Macros definitions
 *****************************************************************************/

#define HAL_CREATE_MUTEX(pMutex)      hal_MutexCreate(pMutex)
#define HAL_DESTROY_MUTEX(pMutex)     hal_MutexDestroy(pMutex)
#define HAL_ACQUIRE_MUTEX(mutex)      hal_MutexAcquire(mutex)
#define HAL_RELEASE_MUTEX(mutex)      hal_MutexRelease(mutex)
#define HAL_VALID_MUTEX(mutex)        hal_MutexIsValid(mutex)
#define HAL_IS_MUTEX_ACQUIRED(mutex)  hal_MutexIsAcquired(mutex)

#endif //_HAL_MUTEX_H_
