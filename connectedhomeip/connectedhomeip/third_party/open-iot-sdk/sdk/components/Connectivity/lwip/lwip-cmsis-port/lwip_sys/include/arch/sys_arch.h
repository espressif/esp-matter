/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ARCH_SYS_ARCH_H_
#define ARCH_SYS_ARCH_H_

#include "cmsis_os2.h"

typedef osThreadId_t sys_thread_t;
typedef osMutexId_t sys_mutex_t;
typedef osSemaphoreId_t sys_sem_t;
typedef osMessageQueueId_t sys_mbox_t;

#define sys_mutex_valid(mutex) (((mutex) != NULL) && (*(mutex) != NULL))
#define sys_mutex_set_invalid(mutex) \
    if (mutex) {                     \
        *(mutex) = NULL;             \
    }
#define sys_sem_valid(sem) (((sem) != NULL) && (*(sem) != NULL))
#define sys_sem_set_invalid(sem) \
    if (sem) {                   \
        *(sem) = NULL;           \
    }
#define sys_mbox_valid(mbox) (((mbox) != NULL) && (*(mbox) != NULL))
#define sys_mbox_set_invalid(mbox) \
    if (mbox) {                    \
        *(mbox) = NULL;            \
    }

#endif /* ARCH_SYS_ARCH_H_ */
