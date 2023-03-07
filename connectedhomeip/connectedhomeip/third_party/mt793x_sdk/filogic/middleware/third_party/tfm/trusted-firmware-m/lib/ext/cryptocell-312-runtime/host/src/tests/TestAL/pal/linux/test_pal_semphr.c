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
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#include "test_pal_semphr.h"
#include "test_pal_mem.h"
#include "test_pal_log.h"

struct binary_semaphore {
    pthread_mutex_t mutex;
    pthread_cond_t cvar;
    int val;
};

typedef struct binary_semaphore *pBinary_semaphore;

/******************************************************************************/
uint8_t Test_PalMutexCreate(Test_PalMutex *ppMutexId)
{
    pthread_mutex_t *mtx = Test_PalMalloc(sizeof(pthread_mutex_t));
    if (mtx == NULL)
        return 1;

    if (pthread_mutex_init(mtx, NULL)) {
        Test_PalFree(mtx);
        return 1;
    }

    *ppMutexId = mtx;
    return 0;
}

/******************************************************************************/
uint8_t Test_PalMutexDestroy(Test_PalMutex *ppMutexId)
{
    pthread_mutex_t *mtx = *ppMutexId;
    if (pthread_mutex_destroy(mtx))
        return 1;

    Test_PalFree(mtx);
    return 0;
}

/******************************************************************************/
uint8_t Test_PalMutexLock(Test_PalMutex *ppMutexId, uint32_t timeout)
{
    pthread_mutex_t *mtx = *ppMutexId;
    (void)timeout;
    return pthread_mutex_lock(mtx) ? 1 : 0 ;
}

/******************************************************************************/
uint8_t Test_PalMutexUnlock(Test_PalMutex *ppMutexId)
{
    pthread_mutex_t *mtx = *ppMutexId;
    return pthread_mutex_unlock(mtx) ? 1 : 0 ;
}

/******************************************************************************/
uint8_t Test_PalBinarySemaphoreCreate(Test_PalBinarySemaphore *ppBinSemphrId)
{
    pBinary_semaphore pLocal = Test_PalMalloc(
                    sizeof(struct binary_semaphore));
    if (pLocal == NULL)
        return 1;

    if (pthread_mutex_init(&pLocal->mutex, NULL)) {
        Test_PalFree(pLocal);
        return 1;
    }

    pthread_cond_init(&pLocal->cvar, NULL);
    pLocal->val = 0;
    *ppBinSemphrId = pLocal;
    return 0;
}

/******************************************************************************/
uint8_t Test_PalBinarySemaphoreDestroy(Test_PalBinarySemaphore *ppBinSemphrId)
{
    pBinary_semaphore pLocal = *ppBinSemphrId;

    pthread_cond_destroy(&pLocal->cvar);

    if (pthread_mutex_destroy(&pLocal->mutex))
        return 1;

    Test_PalFree(pLocal);
    return 0;
}

/******************************************************************************/
uint8_t Test_PalBinarySemaphoreTake(Test_PalBinarySemaphore *ppBinSemphrId,
                            uint32_t timeout)
{
    pBinary_semaphore pLocal = *ppBinSemphrId;
    struct timeval tod;
    struct timespec ts;

    pthread_mutex_lock(&pLocal->mutex);

    while (!pLocal->val) {
        if (timeout == INFINITE) {
            pthread_cond_wait(&pLocal->cvar, &pLocal->mutex);
        } else {
            gettimeofday(&tod, NULL);
            ts.tv_sec = time(NULL) + timeout / 1000;
            ts.tv_nsec = tod.tv_usec * 1000 + 1000 * 1000 * (timeout % 1000);
            ts.tv_sec += ts.tv_nsec / (1000 * 1000 * 1000);
            ts.tv_nsec %= (1000 * 1000 * 1000);

            if (pthread_cond_timedwait(&pLocal->cvar,
            &pLocal->mutex, (const struct timespec *)&ts)) {
                pthread_mutex_unlock(&pLocal->mutex);
                return 1;
            }
        }
    }

    pLocal->val -= 1;

    pthread_mutex_unlock(&pLocal->mutex);
    return 0;
}

/******************************************************************************/
uint8_t Test_PalBinarySemaphoreGive(Test_PalBinarySemaphore *ppBinSemphrId)
{
    pBinary_semaphore pLocal = *ppBinSemphrId;

    pthread_mutex_lock(&pLocal->mutex);

    if (pLocal->val == 1) {
        pthread_mutex_unlock(&pLocal->mutex);
        return 1;
    }

    pLocal->val += 1;
    pthread_cond_signal(&pLocal->cvar);

    pthread_mutex_unlock(&pLocal->mutex);
    return 0;
}
