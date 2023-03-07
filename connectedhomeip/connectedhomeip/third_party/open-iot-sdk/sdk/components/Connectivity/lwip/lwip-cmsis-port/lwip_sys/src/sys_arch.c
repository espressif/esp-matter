/*
 * Copyright (c) 2021, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "arch/sys_arch.h"

#include "cmsis_os2.h"
#include "lwip/opt.h"
#include "lwip/sys.h"

#include <string.h>

static uint32_t tick_freq;

/* Time to kernel ticks */
static uint32_t ms2tick(uint32_t ms)
{
    uint32_t tick = ms;

    if (ms == 0U) {
        tick = osWaitForever;
    } else if (tick_freq != 0U) {
        tick = (ms * tick_freq) / 1000U;
        tick += ((ms * tick_freq) % 1000U) ? 1 : 0;
    }

    return (tick);
}

/* Kernel ticks to time */
static uint32_t tick2ms(uint32_t tick)
{
    if (tick_freq != 0U) {
        tick = (tick * 1000U) / tick_freq;
    }
    return (tick);
}

/* Mutexes */
err_t sys_mutex_new(sys_mutex_t *mutex)
{
    static const osMutexAttr_t attr = {NULL, osMutexPrioInherit, NULL, 0};
    osMutexId_t id;

    id = osMutexNew(&attr);
    if (id == NULL) {
        return ERR_MEM;
    }
    *mutex = id;
    return ERR_OK;
}

void sys_mutex_free(sys_mutex_t *mutex)
{
    osMutexDelete(*mutex);
}

void sys_mutex_lock(sys_mutex_t *mutex)
{
    osMutexAcquire(*mutex, osWaitForever);
}

void sys_mutex_unlock(sys_mutex_t *mutex)
{
    osMutexRelease(*mutex);
}

/* Semaphores */
err_t sys_sem_new(sys_sem_t *sem, u8_t count)
{
    osSemaphoreId_t id;

    id = osSemaphoreNew(1U, count, NULL);
    if (id == NULL) {
        return ERR_MEM;
    }
    *sem = id;
    return ERR_OK;
}

void sys_sem_free(sys_sem_t *sem)
{
    osSemaphoreDelete(*sem);
}

u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{
    osStatus_t status;

    status = osSemaphoreAcquire(*sem, ms2tick(timeout));
    if (status != osOK) {
        return SYS_ARCH_TIMEOUT;
    }
    return 0U;
}

void sys_sem_signal(sys_sem_t *sem)
{
    osSemaphoreRelease(*sem);
}

/* Mailboxes */
err_t sys_mbox_new(sys_mbox_t *mbox, int size)
{
    osMessageQueueId_t id;

    LWIP_ASSERT("Mbox size must be non-negative", (size >= 0));

    id = osMessageQueueNew((uint32_t)size, sizeof(void *), NULL);
    if (id == NULL) {
        return ERR_MEM;
    }
    *mbox = id;
    return ERR_OK;
}

void sys_mbox_free(sys_mbox_t *mbox)
{
    osMessageQueueDelete(*mbox);
}

void sys_mbox_post(sys_mbox_t *mbox, void *msg)
{
    osMessageQueuePut(*mbox, &msg, 0U, osWaitForever);
}

err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{
    osStatus_t status;
    status = osMessageQueuePut(*mbox, &msg, 0U, 0U);
    if (status != osOK) {
        return ERR_MEM;
    }
    return ERR_OK;
}

err_t sys_mbox_trypost_fromisr(sys_mbox_t *q, void *msg)
{
    return sys_mbox_trypost(q, msg);
}

u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout)
{
    osStatus_t status;

    status = osMessageQueueGet(*mbox, msg, NULL, ms2tick(timeout));
    if (status != osOK) {
        return SYS_ARCH_TIMEOUT;
    }
    return 0U;
}

u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)
{
    osStatus_t status;

    status = osMessageQueueGet(*mbox, msg, NULL, 0U);
    if (status != osOK) {
        return SYS_MBOX_EMPTY;
    }
    return 0U;
}

/* Threads */
sys_thread_t sys_thread_new(const char *name, lwip_thread_fn function, void *arg, int stacksize, int prio)
{
    osThreadAttr_t attr;
    osThreadId_t id;

    LWIP_ASSERT("Stack size must be non-negative", (stacksize >= 0));

    memset(&attr, 0, sizeof(attr));
    attr.name = name;
    attr.stack_size = (uint32_t)stacksize;
    attr.priority = (osPriority_t)prio;

    id = osThreadNew(function, arg, &attr);
    return id;
}

/* Time */
void sys_msleep(u32_t ms)
{
    osDelay(ms2tick(ms));
}

u32_t sys_now()
{
    return (tick2ms(osKernelGetTickCount()));
}

u32_t sys_jiffies()
{
    return (osKernelGetTickCount());
}

/* Critical sections */
#if SYS_LIGHTWEIGHT_PROT

/* This function returns a value that is used by sys_arch_unprotect.
 * This allows the system to lock recursively. */
sys_prot_t sys_arch_protect(void)
{
    sys_prot_t pval;

    /* for our OS port locking the scheduler is enough to protect against concurent access */
    pval = osKernelLock();
    return pval;
}

void sys_arch_unprotect(sys_prot_t pval)
{
    /* that passed in value allows us to support nested calls to this function,
     * the value is returned by the paired osKernelLock, which knows if the lock
     * was already present when called */
    osKernelRestoreLock(pval);
}

#endif /* SYS_LIGHTWEIGHT_PROT */

void sys_init(void)
{
    tick_freq = osKernelGetTickFreq();
    if (tick_freq == 1000U) {
        // No scaling for 1ms ticks
        tick_freq = 0U;
    }
}
