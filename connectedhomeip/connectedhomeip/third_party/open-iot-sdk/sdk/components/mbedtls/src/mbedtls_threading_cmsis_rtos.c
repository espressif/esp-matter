/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbedtls_threading_cmsis_rtos.h"

#include "mbedtls/threading.h"

static const osMutexAttr_t mutex_attr = {
    "mbedtls",                             // human readable mutex name
    osMutexRecursive | osMutexPrioInherit, // attr_bits
    NULL,                                  // memory for control block
    0U                                     // size for control block
};

static void threading_mutex_init_cmsis_rtos(mbedtls_threading_mutex_t *mutex)
{
    if (mutex == NULL || mutex->mutex != NULL) {
        return;
    }

    mutex->mutex = osMutexNew(&mutex_attr);
}

static void threading_mutex_free_cmsis_rtos(mbedtls_threading_mutex_t *mutex)
{
    if (mutex == NULL || mutex->mutex == NULL) {
        return;
    }

    osStatus_t status = osMutexDelete(mutex->mutex);
    if (status == osOK) {
        mutex->mutex = NULL;
    }
}

static int threading_mutex_lock_cmsis_rtos(mbedtls_threading_mutex_t *mutex)
{
    if (mutex == NULL || mutex->mutex == NULL) {
        return MBEDTLS_ERR_THREADING_BAD_INPUT_DATA;
    }

    osStatus_t status = osMutexAcquire(mutex->mutex, 0U);
    if (status != osOK) {
        return MBEDTLS_ERR_THREADING_MUTEX_ERROR;
    }

    return 0;
}

static int threading_mutex_unlock_cmsis_rtos(mbedtls_threading_mutex_t *mutex)
{
    if (mutex == NULL || mutex->mutex == NULL) {
        return MBEDTLS_ERR_THREADING_BAD_INPUT_DATA;
    }

    osStatus_t status = osMutexRelease(mutex->mutex);
    if (status != osOK) {
        return MBEDTLS_ERR_THREADING_MUTEX_ERROR;
    }

    return 0;
}

void mbedtls_threading_set_cmsis_rtos()
{
    mbedtls_threading_set_alt(threading_mutex_init_cmsis_rtos,
                              threading_mutex_free_cmsis_rtos,
                              threading_mutex_lock_cmsis_rtos,
                              threading_mutex_unlock_cmsis_rtos);
}
