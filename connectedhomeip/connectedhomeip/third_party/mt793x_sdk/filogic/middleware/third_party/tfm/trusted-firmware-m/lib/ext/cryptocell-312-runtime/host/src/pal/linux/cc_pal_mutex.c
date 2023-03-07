/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */



/************* Include Files ****************/
#include <time.h>
#include "cc_pal_types.h"
#include "cc_pal_mutex.h"
#include "cc_pal_log.h"
#include <stdio.h>

/************************ Defines ******************************/

/************************ Enums ******************************/

/************************ Typedefs ******************************/

/************************ Global Data ******************************/

/************************ Private Functions ******************************/

/************************ Public Functions ******************************/

/**
 * @brief This function purpose is to create a mutex.
 *
 *
 * @param[out] pMutexId - Pointer to created mutex handle
 *
 * @return returns 0 on success, otherwise indicates failure
 */
CCError_t CC_PalMutexCreate(CC_PalMutex *pMutexId)
{
    int  rc = CC_SUCCESS;

    rc = pthread_mutex_init(pMutexId, NULL);
    if (rc != 0) {
        printf /* CC_PAL_LOG_ERR */("pthread_mutex_init failed 0x%x", rc);
        return CC_FAIL;
    }
    return CC_SUCCESS;
}


/**
 * @brief This function purpose is to destroy a mutex
 *
 *
 * @param[in] pMutexId - pointer to Mutex handle
 *
 * @return returns 0 on success, otherwise indicates failure
 */
CCError_t CC_PalMutexDestroy(CC_PalMutex *pMutexId)
{
    int  rc = CC_SUCCESS;

    rc = pthread_mutex_destroy(pMutexId);
    if (rc != 0) {
        printf /* CC_PAL_LOG_ERR */("pthread_mutex_destroy failed 0x%x", rc);
        return CC_FAIL;
    }
    return CC_SUCCESS;
}


/**
 * @brief This function purpose is to Wait for Mutex with aTimeOut. aTimeOut is
 *        specified in milliseconds. (CC_INFINITE is blocking)
 *
 *
 * @param[in] pMutexId - pointer to Mutex handle
 * @param[in] timeOut - timeout in mSec, or CC_INFINITE
 *
 * @return returns 0 on success, otherwise indicates failure
 */
CCError_t CC_PalMutexLock (CC_PalMutex *pMutexId, uint32_t timeOut)
{
    int  rc = CC_SUCCESS;

    timeOut = timeOut; // to remove compilation warnings

    rc = pthread_mutex_lock(pMutexId);
    if (rc != 0) {
        printf /* CC_PAL_LOG_ERR */("pthread_mutex_lock failed 0x%x", rc);
        return CC_FAIL;
    }
    return CC_SUCCESS;
}



/**
 * @brief This function purpose is to release the mutex.
 *
 *
 * @param[in] pMutexId - pointer to Mutex handle
 *
 * @return returns 0 on success, otherwise indicates failure
 */
CCError_t CC_PalMutexUnlock (CC_PalMutex *pMutexId)
{
    int  rc = CC_SUCCESS;

    rc = pthread_mutex_unlock(pMutexId);
    if (rc != 0) {
        printf /* CC_PAL_LOG_ERR */("pthread_mutex_unlock failed 0x%x", rc);
        return CC_FAIL;
    }
    return CC_SUCCESS;
}
