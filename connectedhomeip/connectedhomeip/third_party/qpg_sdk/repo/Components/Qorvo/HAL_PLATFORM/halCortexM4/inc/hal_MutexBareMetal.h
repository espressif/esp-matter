/*
 * Copyright (c) 2015-2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
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

#ifndef _HAL_MUTEXISR_H_
#define _HAL_MUTEXISR_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"

/*****************************************************************************
 *                    Definitions
 *****************************************************************************/

#define HAL_CRITICAL_SECTION_DEF(pMutex) void* pMutex;

/*****************************************************************************
 *                    Function Prototype Definitions
 *****************************************************************************/

void hal_MutexCreate(void **dummyVar);
void hal_MutexDestroy(void **dummyVar);
Bool hal_MutexIsValid(void *dummyVar);
Bool hal_MutexIsAcquired(void *dummyVar);
void hal_MutexAcquire(void *dummyVar);
void hal_MutexRelease(void *dummyVar);

#endif //_HAL_MUTEXISR_H_
